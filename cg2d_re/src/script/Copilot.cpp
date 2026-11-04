#include "LuaUI.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "../world/WorldMapState.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <format>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

namespace cg::script {
namespace copilot_detail {

struct Point {
    int x;
    int y;
};

// 八方向邻居按原顺序遍历；五条寻路路径共享固定偏移。
constexpr int kNeighborOffsetsX[] = {0, 1, 1, 1, 0, -1, -1, -1};
constexpr int kNeighborOffsetsY[] = {-1, -1, 0, 1, 1, 1, 0, -1};

constexpr int kMapBufferLimit = 2601;
constexpr int kMaxOptimisticDistance = 160;
constexpr int kSearchPadding = 24;
constexpr int kKnownWalkable = 1;
constexpr int kKnownBlocked = 2;
constexpr int kMaxCoordinateDistance = 10000;
constexpr int kMaxOptimisticNodes = 262144;
constexpr uint64_t kDiskMapRefreshMs = 5000;
constexpr uint64_t kDiskMapRetryMs = 3000;
constexpr uint64_t kFullSearchTimeLimitMs = 100;
constexpr int kTimedSearchCheckInterval = 512;
constexpr uint64_t kCopilotTickMs = 80;
constexpr uint64_t kDynamicBlockMs = 180000;
constexpr uint64_t kSceneSkipLogMs = 1000;
constexpr uint64_t kCellLogMs = 1000;
constexpr uint64_t kNoPathDetailLogMs = 1000;
constexpr int kObjFootprintScan = 32;
constexpr int kRecentAvoidCount = 7;
constexpr int kNearRetargetMaxSteps = 12;
constexpr int kMoveSegmentSteps = 2;
constexpr int kReplanMoveDistance = 5;

static auto& gAutoTargetX = world::gMoveTargetX9A7F88;
static auto& gAutoTargetY = world::gMoveTargetY9A7F98;


struct DiskMapCache {
    int mapId = -1;
    int floor = -1;
    int group = -1;
    int width = 0;
    int height = 0;
    bool loaded = false;
    uint64_t lastAttemptMs = 0;
    uint64_t lastLoadedMs = 0;
    std::vector<unsigned char> cells;
    std::vector<unsigned short> tileData;
    std::vector<unsigned short> objData;
    std::vector<unsigned short> hitData;
};

// 会话独占的完整规划现场，原初值和缓存布局保持。
struct CopilotState {
    std::unordered_map<uint64_t, unsigned char> knownMap;
    int lastMapId = -1;
    int lastMapFloor = -1;
    int lastMapGroup = -1;
    DiskMapCache diskMap;
    bool copilotEnabled = false;
    Point copilotTarget{};
    int copilotMapId = -1;
    int copilotMapFloor = -1;
    uint64_t lastCopilotTickMs = 0;
    uint64_t lastBusyLogMs = 0;
    uint64_t lastSceneSkipLogMs = 0;
    uint64_t lastCellLogKey = 0;
    uint64_t lastCellLogMs = 0;
    uint64_t lastNoPathDetailLogMs = 0;
    Point lastObservedPlayer{};
    bool hasLastObservedPlayer = false;
    std::array<Point, kRecentAvoidCount> recentPlayers{};
    int recentPlayerCount = 0;
    std::unordered_map<uint64_t, uint64_t> dynamicBlocked;
    Point lastRequestedWaypoint{};
    bool hasLastRequestedWaypoint = false;
    Point expectedAutoTarget{};
    bool hasExpectedAutoTarget = false;
    Point lastRequestPlayer{};
    int lastRequestDistance = 0;
    bool hasPendingMoveCheck = false;
    bool moveCheckSawBusy = false;
    Point lastPlanPlayer{};
    Point cachedWaypoint{};
    Point cachedTarget{};
    int cachedMapId = -1;
    int cachedMapFloor = -1;
    int cachedMapGroup = -1;
    bool hasCachedWaypoint = false;
    std::vector<Point> autoCopilotRoute;
};


static uint64_t nowMs()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

static void clearRecentPlayers(session::ClientSession& session);
static void resetPlanCache(session::ClientSession& session);
static void resetMoveProgress(session::ClientSession& session);
static void clearCopilotRoute(session::ClientSession& session);

static bool isCopilotSceneReady(session::ClientSession& session)
{
    return session.WorldScene().sceneType == SCENE_NORMAL && session.WorldScene().sceneState == 3;
}

static bool skipSceneNotReady(session::ClientSession& session, [[maybe_unused]] const char* where)
{
    if (isCopilotSceneReady(session)) {
        return false;
    }

    uint64_t now = nowMs();
    if (session.Lua().copilot.data->lastSceneSkipLogMs == 0 || now - session.Lua().copilot.data->lastSceneSkipLogMs >= kSceneSkipLogMs) {
        session.Lua().copilot.data->lastSceneSkipLogMs = now;
        // m_log_iw(L"[copilot] skip scene where=%hs scene=%d state=%d needScene=%d needState=3\n", where, session.WorldScene().sceneType, session.WorldScene().sceneState, SCENE_NORMAL);
    }
    session.Lua().copilot.data->hasLastObservedPlayer = false;
    clearRecentPlayers(session);
    resetPlanCache(session);
    resetMoveProgress(session);
    clearCopilotRoute(session);
    session.Lua().copilot.data->hasLastRequestedWaypoint = false;
    session.Lua().copilot.data->lastCopilotTickMs = 0;
    return true;
}

static size_t diskIndex(int width, int x, int y)
{
    return static_cast<size_t>(x) + static_cast<size_t>(y) * width;
}

static void setDiskState(std::vector<unsigned char>& state, int width, int height, int x, int y, unsigned char value)
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    unsigned char& cell = state[diskIndex(width, x, y)];
    if (value == 2) {
        cell = 2;
    } else if (cell != 2) {
        cell = 1;
    }
}

static void applySingleCellRule(session::ClientSession& session, std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(session, raw);
    if (imageId > 999 || (imageId >= 60 && imageId <= 79)) {
        std::int32_t seqNo = 0;
        unsigned short flag = 0;
        resource::GetGraphicMapNoIndex(imageId, &seqNo);
        resource::GetGraphicFlag1(seqNo, &flag);
        if (flag == 0) {
            setDiskState(state, width, height, x, y, 1);
        } else if (flag == 2) {
            setDiskState(state, width, height, x, y, 2);
        }
        return;
    }

    switch (imageId) {
    case 0:
        if ((raw & 0x4000) != 0) {
            setDiskState(state, width, height, x, y, 1);
        }
        break;
    case 2:
    case 5:
    case 6:
    case 9:
    case 10:
        setDiskState(state, width, height, x, y, 1);
        break;
    case 4:
        setDiskState(state, width, height, x, y, 2);
        break;
    default:
        break;
    }
}

static void applyFootprintRule(std::vector<unsigned char>& state, int width, int height, int x, int y, unsigned short flag, std::int32_t seqNo)
{
    unsigned short sxRaw = 0;
    unsigned short syRaw = 0;
    resource::GetGraphicSxSy(seqNo, &sxRaw, &syRaw);

    short sx = static_cast<short>(sxRaw);
    short sy = static_cast<short>(syRaw);
    if (sx <= 0 || sy <= 0) {
        return;
    }

    unsigned char value = flag == 2 ? 2 : 1;
    for (int yy = 0; yy < sy; ++yy) {
        for (int xx = 0; xx < sx; ++xx) {
            setDiskState(state, width, height, x + xx, y - yy, value);
        }
    }
}

static void applyObjectRule(session::ClientSession& session, std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(session, raw);
    if (imageId > 999) {
        std::int32_t seqNo = 0;
        unsigned short flag = 0;
        resource::GetGraphicMapNoIndex(imageId, &seqNo);
        resource::GetGraphicFlag1(seqNo, &flag);
        if (flag == 0 || flag == 2) {
            applyFootprintRule(state, width, height, x, y, flag, seqNo);
        }
        return;
    }

    applySingleCellRule(session, state, width, height, raw, x, y);
}

static std::vector<unsigned char> synthesizeDiskCells(session::ClientSession& session,
    int width, int height, const std::vector<unsigned short>& tileData, const std::vector<unsigned short>& objData, const std::vector<unsigned short>& hitData)
{
    size_t cellCount = tileData.size();
    std::vector<unsigned char> state(cellCount, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            applySingleCellRule(session, state, width, height, tileData[diskIndex(width, x, y)], x, y);
        }
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            applyObjectRule(session, state, width, height, objData[diskIndex(width, x, y)], x, y);
        }
    }

    std::vector<unsigned char> cells(cellCount, kKnownWalkable);
    for (size_t i = 0; i < cellCount; ++i) {
        if ((hitData[i] & 0x00FF) == 1 || state[i] == 1) {
            cells[i] = kKnownBlocked;
        }
    }
    return cells;
}

static bool isBusy(session::ClientSession& session)
{
    return session.WorldScene().movement.pathCount > 0 || session.WorldScene().movement.pendingDirectionCount > 0 || session.WorldScene().movement.inputBlocked != 0 || session.WorldScene().movement.transitionRequestPending != 0 || session.WorldScene().movement.directionalTransitionPending != 0;
}

static bool isValidMap(session::ClientSession& session)
{
    return session.WorldScene().map.spanX > 0 && session.WorldScene().map.spanY > 0 && session.WorldScene().map.spanX * session.WorldScene().map.spanY <= kMapBufferLimit;
}

static bool isSaneTarget(Point start, Point target)
{
    return std::llabs(static_cast<long long>(target.x) - start.x) <= kMaxCoordinateDistance
        && std::llabs(static_cast<long long>(target.y) - start.y) <= kMaxCoordinateDistance;
}

static uint64_t makeKey(int x, int y)
{
    return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(y);
}

static Point keyToPoint(uint64_t key)
{
    return {
        static_cast<int>(static_cast<int32_t>(static_cast<uint32_t>(key >> 32))),
        static_cast<int>(static_cast<int32_t>(static_cast<uint32_t>(key))),
    };
}

static bool samePoint(Point a, Point b)
{
    return a.x == b.x && a.y == b.y;
}

static int pointDistance(Point a, Point b)
{
    return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

static void resetPlanCache(session::ClientSession& session)
{
    session.Lua().copilot.data->hasCachedWaypoint = false;
}

static void resetMoveProgress(session::ClientSession& session)
{
    session.Lua().copilot.data->hasPendingMoveCheck = false;
    session.Lua().copilot.data->moveCheckSawBusy = false;
    session.Lua().copilot.data->hasExpectedAutoTarget = false;
}

static void clearCopilotRoute(session::ClientSession& session)
{
    session.Lua().copilot.data->autoCopilotRoute.clear();
}

static void cacheCopilotRoute(session::ClientSession& session, const std::vector<Point>& path)
{
    session.Lua().copilot.data->autoCopilotRoute = path;
}

static void clearRecentPlayers(session::ClientSession& session)
{
    session.Lua().copilot.data->recentPlayerCount = 0;
}

static void rememberRecentPlayer(session::ClientSession& session, Point p)
{
    if (session.Lua().copilot.data->recentPlayerCount > 0 && samePoint(session.Lua().copilot.data->recentPlayers[0], p)) {
        return;
    }
    int moveCount = std::min(session.Lua().copilot.data->recentPlayerCount, kRecentAvoidCount - 1);
    for (int i = moveCount; i > 0; --i) {
        session.Lua().copilot.data->recentPlayers[i] = session.Lua().copilot.data->recentPlayers[i - 1];
    }
    session.Lua().copilot.data->recentPlayers[0] = p;
    if (session.Lua().copilot.data->recentPlayerCount < kRecentAvoidCount) {
        ++session.Lua().copilot.data->recentPlayerCount;
    }
}

static bool isRecentPlayer(session::ClientSession& session, Point p)
{
    for (int i = 0; i < session.Lua().copilot.data->recentPlayerCount; ++i) {
        if (samePoint(session.Lua().copilot.data->recentPlayers[i], p)) {
            return true;
        }
    }
    return false;
}

static bool isDynamicBlocked(session::ClientSession& session, Point p)
{
    uint64_t key = makeKey(p.x, p.y);
    auto it = session.Lua().copilot.data->dynamicBlocked.find(key);
    if (it == session.Lua().copilot.data->dynamicBlocked.end()) {
        return false;
    }

    uint64_t now = nowMs();
    if (it->second <= now) {
        session.Lua().copilot.data->dynamicBlocked.erase(it);
        return false;
    }
    return true;
}
static bool putDynamicBlocked(session::ClientSession& session, Point p)
{
    bool existed = isDynamicBlocked(session, p);
    session.Lua().copilot.data->dynamicBlocked[makeKey(p.x, p.y)] = nowMs() + kDynamicBlockMs;
    return !existed;
}

static void markDynamicBlocked(session::ClientSession& session, Point p, [[maybe_unused]] const char* reason)
{
    bool added = putDynamicBlocked(session, p);
    if (added) {
        // m_log_iw(L"[copilot] dynamic block %d,%d reason=%hs ttl=%u\n", p.x, p.y, reason, static_cast<unsigned int>(kDynamicBlockMs));
    }
}


static void syncVisibleCache(session::ClientSession& session)
{
    if (!isValidMap(session)) {
        return;
    }
    if (static_cast<std::uint32_t>(session.Lua().copilot.data->lastMapId) != session.WorldScene().map.mapId || session.Lua().copilot.data->lastMapFloor != session.WorldScene().map.floor || session.Lua().copilot.data->lastMapGroup != session.WorldScene().map.cachePathKey) {
        // m_log_iw(L"[copilot] map changed map=%d group=%d floor=%d visible=%dx%d start=%d,%d\n", session.WorldScene().map.mapId, session.WorldScene().map.cachePathKey, session.WorldScene().map.floor, session.WorldScene().map.spanX, session.WorldScene().map.spanY, session.WorldScene().map.xStart, session.WorldScene().map.yStart);
        session.Lua().copilot.data->knownMap.clear();
        session.Lua().copilot.data->diskMap = DiskMapCache{};
        session.Lua().copilot.data->dynamicBlocked.clear();
        session.Lua().copilot.data->hasLastRequestedWaypoint = false;
        resetPlanCache(session);
        resetMoveProgress(session);
        session.Lua().copilot.data->lastMapId = session.WorldScene().map.mapId;
        session.Lua().copilot.data->lastMapFloor = session.WorldScene().map.floor;
        session.Lua().copilot.data->lastMapGroup = session.WorldScene().map.cachePathKey;
    }

    for (int y = 0; y < session.WorldScene().map.spanY; ++y) {
        for (int x = 0; x < session.WorldScene().map.spanX; ++x) {
            int globalX = session.WorldScene().map.xStart + x;
            int globalY = session.WorldScene().map.yStart + y;
            session.Lua().copilot.data->knownMap[makeKey(globalX, globalY)] = session.WorldScene().map.data.walkability[x + y * session.WorldScene().map.spanX] == 1 ? kKnownBlocked : kKnownWalkable;
        }
    }
}

static std::wstring makeMapFileName(session::ClientSession& session)
{
    return session.WorldScene().map.mapId ? std::format(L"map\\{}\\{}\\{}.dat", session.WorldScene().map.mapId, session.WorldScene().map.cachePathKey, static_cast<std::int32_t>(session.WorldScene().map.floor))
        : std::format(L"map\\0\\{}.dat", static_cast<std::int32_t>(session.WorldScene().map.floor));
}

static bool loadDiskMap(session::ClientSession& session)
{
    uint64_t now = nowMs();
    bool sameMap = static_cast<std::uint32_t>(session.Lua().copilot.data->diskMap.mapId) == session.WorldScene().map.mapId && session.Lua().copilot.data->diskMap.floor == session.WorldScene().map.floor && session.Lua().copilot.data->diskMap.group == session.WorldScene().map.cachePathKey;
    if (sameMap && session.Lua().copilot.data->diskMap.loaded) {
        if (now - session.Lua().copilot.data->diskMap.lastLoadedMs < kDiskMapRefreshMs || now - session.Lua().copilot.data->diskMap.lastAttemptMs < kDiskMapRetryMs) {
            return true;
        }
    }
    if (sameMap && !session.Lua().copilot.data->diskMap.loaded && now - session.Lua().copilot.data->diskMap.lastAttemptMs < kDiskMapRetryMs) {
        return false;
    }

    DiskMapCache next;
    next.mapId = session.WorldScene().map.mapId;
    next.floor = session.WorldScene().map.floor;
    next.group = session.WorldScene().map.cachePathKey;
    next.lastAttemptMs = now;

    auto fail = [&]([[maybe_unused]] const char* reason) {
        // m_log_iw(L"[copilot] loadDiskMap failed reason=%hs map=%d group=%d floor=%d keepOld=%d\n", reason, session.WorldScene().map.mapId, session.WorldScene().map.cachePathKey, session.WorldScene().map.floor, sameMap && session.Lua().copilot.data->diskMap.loaded ? 1 : 0);
        if (sameMap && session.Lua().copilot.data->diskMap.loaded) {
            session.Lua().copilot.data->diskMap.lastAttemptMs = now;
            return true;
        }
        session.Lua().copilot.data->diskMap = next;
        return false;
    };

    FILE* file = _wfopen(makeMapFileName(session).c_str(), L"rb");
    if (!file) {
        return fail("open");
    }

    unsigned char magic[4] = {};
    if (std::fread(magic, 1, sizeof(magic), file) != sizeof(magic) || magic[0] != 'M' || magic[1] != 'A' || magic[2] != 'P' || magic[3] != 1) {
        std::fclose(file);
        return fail("magic");
    }

    int header[4] = {};
    if (std::fread(header, sizeof(int), 4, file) != 4 || header[2] <= 0 || header[3] <= 0) {
        std::fclose(file);
        return fail("header");
    }

    long long cellCount64 = static_cast<long long>(header[2]) * header[3];
    if (cellCount64 <= 0 || cellCount64 > 10000000) {
        std::fclose(file);
        return fail("cellCount");
    }

    size_t cellCount = static_cast<size_t>(cellCount64);
    std::vector<unsigned short> tileData(cellCount);
    std::vector<unsigned short> objData(cellCount);
    std::vector<unsigned short> hitData(cellCount);
    bool readOk = std::fread(tileData.data(), sizeof(unsigned short), tileData.size(), file) == tileData.size()
        && std::fread(objData.data(), sizeof(unsigned short), objData.size(), file) == objData.size()
        && std::fread(hitData.data(), sizeof(unsigned short), hitData.size(), file) == hitData.size();
    std::fclose(file);
    if (!readOk) {
        return fail("layers");
    }

    next.width = header[2];
    next.height = header[3];
    next.cells = synthesizeDiskCells(session, next.width, next.height, tileData, objData, hitData);
    next.tileData = std::move(tileData);
    next.objData = std::move(objData);
    next.hitData = std::move(hitData);
    next.loaded = true;
    next.lastLoadedMs = now;
    [[maybe_unused]] size_t blockedCount = std::count(next.cells.begin(), next.cells.end(), static_cast<unsigned char>(kKnownBlocked));
    // m_log_iw(L"[copilot] loadDiskMap ok map=%d group=%d floor=%d size=%dx%d blocked=%u\n", session.WorldScene().map.mapId, session.WorldScene().map.cachePathKey, session.WorldScene().map.floor, next.width, next.height, static_cast<unsigned int>(blockedCount));
    session.Lua().copilot.data->diskMap = next;
    return true;
}

static unsigned char diskCell(session::ClientSession& session, Point p)
{
    if (!loadDiskMap(session)) {
        return kKnownWalkable;
    }
    if (p.x < 0 || p.y < 0 || p.x >= session.Lua().copilot.data->diskMap.width || p.y >= session.Lua().copilot.data->diskMap.height) {
        return kKnownWalkable;
    }
    return session.Lua().copilot.data->diskMap.cells[p.x + p.y * session.Lua().copilot.data->diskMap.width];
}

static bool inDiskBounds(session::ClientSession& session, Point p)
{
    return session.Lua().copilot.data->diskMap.loaded && p.x >= 0 && p.y >= 0 && p.x < session.Lua().copilot.data->diskMap.width && p.y < session.Lua().copilot.data->diskMap.height;
}

static unsigned char cachedKnownOrDiskCell(session::ClientSession& session, Point p)
{
    if (isDynamicBlocked(session, p)) {
        return kKnownBlocked;
    }
    auto it = session.Lua().copilot.data->knownMap.find(makeKey(p.x, p.y));
    if (it != session.Lua().copilot.data->knownMap.end()) {
        return it->second;
    }
    if (!inDiskBounds(session, p)) {
        return kKnownWalkable;
    }
    return session.Lua().copilot.data->diskMap.cells[static_cast<size_t>(p.x) + static_cast<size_t>(p.y) * session.Lua().copilot.data->diskMap.width];
}

struct CellGraphicInfo {
    int imageId = 0;
    std::int32_t seqNo = 0;
    unsigned short flag = 0xFFFF;
    unsigned short sx = 0;
    unsigned short sy = 0;
};

static CellGraphicInfo readCellGraphicInfo(session::ClientSession& session, unsigned short raw)
{
    CellGraphicInfo info;
    info.imageId = world::ResolveMapGraphicNumber_46CF00(session, raw);
    if (info.imageId > 999 || (info.imageId >= 60 && info.imageId <= 79)) {
        resource::GetGraphicMapNoIndex(info.imageId, &info.seqNo);
        resource::GetGraphicFlag1(info.seqNo, &info.flag);
        if (info.imageId > 999) {
            resource::GetGraphicSxSy(info.seqNo, &info.sx, &info.sy);
        }
    }
    return info;
}

static unsigned char rawBlockState(session::ClientSession& session, unsigned short raw)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(session, raw);
    if (imageId > 999 || (imageId >= 60 && imageId <= 79)) {
        std::int32_t seqNo = 0;
        unsigned short flag = 0;
        resource::GetGraphicMapNoIndex(imageId, &seqNo);
        resource::GetGraphicFlag1(seqNo, &flag);
        if (flag == 0) {
            return 1;
        }
        if (flag == 2) {
            return 2;
        }
        return 0;
    }

    switch (imageId) {
    case 0:
        return (raw & 0x4000) != 0 ? 1 : 0;
    case 2:
    case 5:
    case 6:
    case 9:
    case 10:
        return 1;
    case 4:
        return 2;
    default:
        return 0;
    }
}

static bool footprintCovers(Point origin, CellGraphicInfo info, Point p)
{
    short sx = static_cast<short>(info.sx);
    short sy = static_cast<short>(info.sy);
    return sx > 0 && sy > 0 && p.x >= origin.x && p.x < origin.x + sx && p.y <= origin.y && p.y > origin.y - sy;
}

[[maybe_unused]] static void logFootprintSources(session::ClientSession& session, Point p, [[maybe_unused]] const char* label)
{
    if (!session.Lua().copilot.data->diskMap.loaded || !inDiskBounds(session, p)) {
        return;
    }

    int logged = 0;
    int minX = std::max(0, p.x - kObjFootprintScan);
    int maxX = std::min(session.Lua().copilot.data->diskMap.width - 1, p.x);
    int minY = p.y;
    int maxY = std::min(session.Lua().copilot.data->diskMap.height - 1, p.y + kObjFootprintScan);
    for (int y = minY; y <= maxY && logged < 8; ++y) {
        for (int x = minX; x <= maxX && logged < 8; ++x) {
            size_t index = diskIndex(session.Lua().copilot.data->diskMap.width, x, y);
            if (index >= session.Lua().copilot.data->diskMap.objData.size()) {
                continue;
            }
            unsigned short objRaw = session.Lua().copilot.data->diskMap.objData[index];
            if (objRaw == 0) {
                continue;
            }
            CellGraphicInfo objInfo = readCellGraphicInfo(session, objRaw);
            if (objInfo.imageId <= 999 || !footprintCovers({x, y}, objInfo, p)) {
                continue;
            }
            // m_log_iw(L"[copilot] cell %hs cover p=%d,%d byObj=%d,%d raw=%04X img=%d flag=%u sx=%u sy=%u blockValue=%u\n", label, p.x, p.y, x, y, objRaw, objInfo.imageId, objInfo.flag, objInfo.sx, objInfo.sy, objInfo.flag == 0 ? 1 : objInfo.flag == 2 ? 2 : 0);
            ++logged;
        }
    }
}

[[maybe_unused]] static void logDiskCellDetail(session::ClientSession& session, Point p, [[maybe_unused]] const char* label);
static int markObjectFootprintBlocked(session::ClientSession& session, Point origin, [[maybe_unused]] unsigned short raw, CellGraphicInfo info, [[maybe_unused]] Point cover, [[maybe_unused]] const char* reason)
{
    short sx = static_cast<short>(info.sx);
    short sy = static_cast<short>(info.sy);
    if (sx <= 0 || sy <= 0) {
        return 0;
    }

    int marked = 0;
    int added = 0;
    for (int yy = 0; yy < sy; ++yy) {
        for (int xx = 0; xx < sx; ++xx) {
            Point cell{origin.x + xx, origin.y - yy};
            if (!inDiskBounds(session, cell)) {
                continue;
            }
            ++marked;
            if (putDynamicBlocked(session, cell)) {
                ++added;
            }
        }
    }

    if (marked > 0) {
        // m_log_iw(L"[copilot] dynamic obj block origin=%d,%d raw=%04X img=%d flag=%u sx=%u sy=%u cover=%d,%d cells=%d new=%d reason=%hs ttl=%u\n", origin.x, origin.y, raw, info.imageId, info.flag, info.sx, info.sy, cover.x, cover.y, marked, added, reason, static_cast<unsigned int>(kDynamicBlockMs));
    }
    return marked;
}

static int markCoveringObjectBlocks(session::ClientSession& session, Point p, [[maybe_unused]] const char* reason)
{
    if (!session.Lua().copilot.data->diskMap.loaded || !inDiskBounds(session, p)) {
        return 0;
    }

    int marked = 0;
    int minX = std::max(0, p.x - kObjFootprintScan);
    int maxX = std::min(session.Lua().copilot.data->diskMap.width - 1, p.x);
    int minY = p.y;
    int maxY = std::min(session.Lua().copilot.data->diskMap.height - 1, p.y + kObjFootprintScan);
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            size_t index = diskIndex(session.Lua().copilot.data->diskMap.width, x, y);
            if (index >= session.Lua().copilot.data->diskMap.objData.size()) {
                continue;
            }
            unsigned short objRaw = session.Lua().copilot.data->diskMap.objData[index];
            if (objRaw == 0) {
                continue;
            }
            CellGraphicInfo objInfo = readCellGraphicInfo(session, objRaw);
            if (objInfo.imageId <= 999 || objInfo.flag != 0 || !footprintCovers({x, y}, objInfo, p)) {
                continue;
            }
            marked += markObjectFootprintBlocked(session, {x, y}, objRaw, objInfo, p, reason);
        }
    }
    return marked;
}

static void markDynamicBlockedByMapSource(session::ClientSession& session, Point p, [[maybe_unused]] const char* reason)
{
    // logDiskCellDetail(session, p, reason);
    if (!loadDiskMap(session) || !inDiskBounds(session, p)) {
        markDynamicBlocked(session, p, reason);
        return;
    }

    size_t index = diskIndex(session.Lua().copilot.data->diskMap.width, p.x, p.y);
    if (index >= session.Lua().copilot.data->diskMap.tileData.size() || index >= session.Lua().copilot.data->diskMap.objData.size() || index >= session.Lua().copilot.data->diskMap.hitData.size()) {
        markDynamicBlocked(session, p, reason);
        return;
    }

    unsigned short tileRaw = session.Lua().copilot.data->diskMap.tileData[index];
    unsigned short objRaw = session.Lua().copilot.data->diskMap.objData[index];
    unsigned short hitRaw = session.Lua().copilot.data->diskMap.hitData[index];
    bool hitWalkable = (hitRaw & 0x00FF) != 1;
    bool tileWalkable = rawBlockState(session, tileRaw) != 1;
    if (hitWalkable && tileWalkable) {
        int objMarked = markCoveringObjectBlocks(session, p, "stuck-obj");
        if (objMarked > 0) {
            return;
        }
        if (objRaw != 0) {
            CellGraphicInfo objInfo = readCellGraphicInfo(session, objRaw);
            if (objInfo.imageId > 999) {
                if (markObjectFootprintBlocked(session, p, objRaw, objInfo, p, "stuck-obj-self") > 0) {
                    return;
                }
            }
            markDynamicBlocked(session, p, "stuck-obj-cell");
            return;
        }
    }
    if (hitWalkable && objRaw == 0) {
        markDynamicBlocked(session, p, "stuck-tile");
        return;
    }

    markDynamicBlocked(session, p, reason);
}

[[maybe_unused]] static void logDiskCellDetail(session::ClientSession& session, Point p, [[maybe_unused]] const char* label)
{
    uint64_t now = nowMs();
    uint64_t key = makeKey(p.x, p.y);
    if (session.Lua().copilot.data->lastCellLogKey == key && now - session.Lua().copilot.data->lastCellLogMs < kCellLogMs) {
        return;
    }
    session.Lua().copilot.data->lastCellLogKey = key;
    session.Lua().copilot.data->lastCellLogMs = now;

    if (!loadDiskMap(session) || !inDiskBounds(session, p)) {
        // m_log_iw(L"[copilot] cell %hs p=%d,%d disk=unavailable loaded=%d size=%dx%d\n", label, p.x, p.y, session.Lua().copilot.data->diskMap.loaded ? 1 : 0, session.Lua().copilot.data->diskMap.width, session.Lua().copilot.data->diskMap.height);
        return;
    }

    size_t index = diskIndex(session.Lua().copilot.data->diskMap.width, p.x, p.y);
    if (index >= session.Lua().copilot.data->diskMap.cells.size() || index >= session.Lua().copilot.data->diskMap.tileData.size() || index >= session.Lua().copilot.data->diskMap.objData.size() || index >= session.Lua().copilot.data->diskMap.hitData.size()) {
        // m_log_iw(L"[copilot] cell %hs p=%d,%d disk=bad-cache index=%u cells=%u tile=%u obj=%u hit=%u\n", label, p.x, p.y, static_cast<unsigned int>(index), static_cast<unsigned int>(session.Lua().copilot.data->diskMap.cells.size()), static_cast<unsigned int>(session.Lua().copilot.data->diskMap.tileData.size()), static_cast<unsigned int>(session.Lua().copilot.data->diskMap.objData.size()), static_cast<unsigned int>(session.Lua().copilot.data->diskMap.hitData.size()));
        return;
    }
    // unsigned short tileRaw = session.Lua().copilot.data->diskMap.tileData[index];
    // unsigned short objRaw = session.Lua().copilot.data->diskMap.objData[index];
    // unsigned short hitRaw = session.Lua().copilot.data->diskMap.hitData[index];
    // CellGraphicInfo tileInfo = readCellGraphicInfo(session, tileRaw);
    // CellGraphicInfo objInfo = readCellGraphicInfo(session, objRaw);
    // int lx = p.x - session.WorldScene().map.xStart;
    // int ly = p.y - session.WorldScene().map.yStart;
    // int visible = isValidMap(session) && lx >= 0 && ly >= 0 && lx < session.WorldScene().map.spanX && ly < session.WorldScene().map.spanY ? session.WorldScene().map.data.walkability[lx + ly * session.WorldScene().map.spanX] : -1;
    // unsigned char disk = session.Lua().copilot.data->diskMap.cells[index];
    // unsigned char known = cachedKnownOrDiskCell(session, p);
    // m_log_i(
    //     "[copilot] cell %s p=%d,%d tile=%04X img=%d flag=%u sx=%u sy=%u obj=%04X img=%d flag=%u sx=%u sy=%u hit=%04X low=%u disk=%u known=%u visible=%d dyn=%d\n",
    //     label, p.x, p.y, tileRaw, tileInfo.imageId, tileInfo.flag, tileInfo.sx, tileInfo.sy, objRaw, objInfo.imageId, objInfo.flag, objInfo.sx,
    //     objInfo.sy, hitRaw, hitRaw & 0x00FF, disk, known, visible, isDynamicBlocked(session, p) ? 1 : 0);
    // logFootprintSources(session, p, label);
}

static bool toLocal(session::ClientSession& session, Point p, int& lx, int& ly)
{
    lx = p.x - session.WorldScene().map.xStart;
    ly = p.y - session.WorldScene().map.yStart;
    return lx >= 0 && ly >= 0 && lx < session.WorldScene().map.spanX && ly < session.WorldScene().map.spanY;
}

static bool canWalkLocal(session::ClientSession& session, int lx, int ly)
{
    return isValidMap(session) && lx >= 0 && ly >= 0 && lx < session.WorldScene().map.spanX && ly < session.WorldScene().map.spanY
        && session.WorldScene().map.data.walkability[lx + ly * session.WorldScene().map.spanX] != 1;
}

static bool canWalkLocalPoint(session::ClientSession& session, int lx, int ly)
{
    return canWalkLocal(session, lx, ly) && !isDynamicBlocked(session, {session.WorldScene().map.xStart + lx, session.WorldScene().map.yStart + ly});
}

static bool canWalkGlobal(session::ClientSession& session, Point p)
{
    if (!isValidMap(session)) {
        return false;
    }
    int lx = 0;
    int ly = 0;
    return !isDynamicBlocked(session, p) && toLocal(session, p, lx, ly) && canWalkLocal(session, lx, ly);
}

static bool canReachLocal(session::ClientSession& session, Point start, Point target)
{
    if (samePoint(start, target)) {
        return true;
    }

    int startX = 0;
    int startY = 0;
    int targetX = 0;
    int targetY = 0;
    if (!toLocal(session, start, startX, startY) || !toLocal(session, target, targetX, targetY) || !canWalkGlobal(session, target)) {
        return false;
    }

    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(session.WorldScene().map.spanX * session.WorldScene().map.spanY);

    int startIndex = startX + startY * session.WorldScene().map.spanX;
    int targetIndex = targetX + targetY * session.WorldScene().map.spanX;
    visited[startIndex] = 1;
    queue.push_back(startIndex);

    size_t head = 0;
    while (head < queue.size()) {
        int index = queue[head++];
        if (index == targetIndex) {
            return true;
        }
        int cx = index % session.WorldScene().map.spanX;
        int cy = index / session.WorldScene().map.spanX;
        for (int i = 0; i < 8; ++i) {
            int nx = cx + kNeighborOffsetsX[i];
            int ny = cy + kNeighborOffsetsY[i];
            if (!canWalkLocalPoint(session, nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(session, cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(session, cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * session.WorldScene().map.spanX;
            if (visited[nextIndex]) {
                continue;
            }
            visited[nextIndex] = 1;
            queue.push_back(nextIndex);
        }
    }
    return false;
}

static bool isForwardWaypoint(session::ClientSession& session, Point player, Point waypoint)
{
    if (!canWalkGlobal(session, waypoint)) {
        return false;
    }
    bool forward = canReachLocal(session, player, waypoint);
    if (!forward) {
        // m_log_iw(L"[copilot] reject waypoint=%d,%d forward=0 backward=0\n", waypoint.x, waypoint.y);
        return false;
    }
    return true;
}

static bool isBidirectionalWaypoint(session::ClientSession& session, Point player, Point waypoint)
{
    if (!isForwardWaypoint(session, player, waypoint)) {
        return false;
    }
    bool backward = canReachLocal(session, waypoint, player);
    if (!backward) {
        // m_log_iw(L"[copilot] defer waypoint=%d,%d forward=1 backward=0\n", waypoint.x, waypoint.y);
        return false;
    }
    return true;
}

[[maybe_unused]] static void logNoPathDetail(session::ClientSession& session, Point player, Point target)
{
    uint64_t now = nowMs();
    if (session.Lua().copilot.data->lastNoPathDetailLogMs != 0 && now - session.Lua().copilot.data->lastNoPathDetailLogMs < kNoPathDetailLogMs) {
        return;
    }
    session.Lua().copilot.data->lastNoPathDetailLogMs = now;
    loadDiskMap(session);

    int px = player.x - session.WorldScene().map.xStart;
    int py = player.y - session.WorldScene().map.yStart;
    int tx = target.x - session.WorldScene().map.xStart;
    int ty = target.y - session.WorldScene().map.yStart;
    int playerVisible = isValidMap(session) && px >= 0 && py >= 0 && px < session.WorldScene().map.spanX && py < session.WorldScene().map.spanY ? session.WorldScene().map.data.walkability[px + py * session.WorldScene().map.spanX] : -1;
    int targetVisible = isValidMap(session) && tx >= 0 && ty >= 0 && tx < session.WorldScene().map.spanX && ty < session.WorldScene().map.spanY ? session.WorldScene().map.data.walkability[tx + ty * session.WorldScene().map.spanX] : -1;
    unsigned char playerKnown = cachedKnownOrDiskCell(session, player);
    unsigned char targetKnown = cachedKnownOrDiskCell(session, target);
    m_log_iw(L"[copilot] no path detail playerWalkable=%d playerKnown=%u playerVisible=%d playerDyn=%d targetKnown=%u targetVisible=%d targetDyn=%d diskLoaded=%d diskSize=%dx%d dyn=%u known=%u\n",
        canWalkGlobal(session, player) ? 1 : 0, playerKnown, playerVisible, isDynamicBlocked(session, player) ? 1 : 0, targetKnown, targetVisible,
        isDynamicBlocked(session, target) ? 1 : 0, session.Lua().copilot.data->diskMap.loaded ? 1 : 0, session.Lua().copilot.data->diskMap.width, session.Lua().copilot.data->diskMap.height,
        static_cast<unsigned int>(session.Lua().copilot.data->dynamicBlocked.size()), static_cast<unsigned int>(session.Lua().copilot.data->knownMap.size()));
    // logDiskCellDetail(session, player, "no-path-player");
    // logDiskCellDetail(session, target, "no-path-target");
}
static unsigned char knownCell(session::ClientSession& session, Point p)
{
    if (isDynamicBlocked(session, p)) {
        return kKnownBlocked;
    }
    auto it = session.Lua().copilot.data->knownMap.find(makeKey(p.x, p.y));
    if (it == session.Lua().copilot.data->knownMap.end()) {
        return diskCell(session, p);
    }
    return it->second;
}

static bool optimisticWalkable(session::ClientSession& session, Point p)
{
    return knownCell(session, p) != kKnownBlocked;
}

static int optimisticMoveCost(session::ClientSession& session, Point p, bool diagonal)
{
    int cost = diagonal ? 14 : 10;
    return knownCell(session, p) == kKnownWalkable ? cost : cost + 30;
}

static long long distanceScore(session::ClientSession& session, int lx, int ly, Point target)
{
    long long x = static_cast<long long>(session.WorldScene().map.xStart) + lx;
    long long y = static_cast<long long>(session.WorldScene().map.yStart) + ly;
    long long dx = x - target.x;
    long long dy = y - target.y;
    return dx * dx + dy * dy;
}

static bool isBetterFallback(session::ClientSession& session, int index, int bestIndex, int step, int bestStep, Point target)
{
    int x = index % session.WorldScene().map.spanX;
    int y = index / session.WorldScene().map.spanX;
    int bestX = bestIndex % session.WorldScene().map.spanX;
    int bestY = bestIndex / session.WorldScene().map.spanX;
    long long score = distanceScore(session, x, y, target);
    long long bestScore = distanceScore(session, bestX, bestY, target);
    return score < bestScore || (score == bestScore && step < bestStep);
}

static Point clampTarget(Point start, Point target)
{
    long long dx = static_cast<long long>(target.x) - start.x;
    long long dy = static_cast<long long>(target.y) - start.y;
    long long distance = std::max(std::llabs(dx), std::llabs(dy));
    if (distance <= kMaxOptimisticDistance) {
        return target;
    }
    return {
        static_cast<int>(start.x + dx * kMaxOptimisticDistance / distance),
        static_cast<int>(start.y + dy * kMaxOptimisticDistance / distance),
    };
}

static int heuristic(Point a, Point b)
{
    long long dx = std::llabs(static_cast<long long>(a.x) - b.x);
    long long dy = std::llabs(static_cast<long long>(a.y) - b.y);
    long long diagonal = std::min(dx, dy);
    long long straight = std::max(dx, dy) - diagonal;
    long long value = diagonal * 14 + straight * 10;
    return value > std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : static_cast<int>(value);
}

static bool buildTimedFullPath(session::ClientSession& session, Point start, Point target, uint64_t timeLimitMs, std::vector<Point>& path)
{
    path.clear();

    uint64_t deadline = nowMs() + timeLimitMs;
    if (!loadDiskMap(session) || nowMs() >= deadline || !inDiskBounds(session, start) || !inDiskBounds(session, target)) {
        return false;
    }

    auto walkable = [&session](Point p) {
        return inDiskBounds(session, p) && cachedKnownOrDiskCell(session, p) != kKnownBlocked;
    };
    if (!walkable(target)) {
        return false;
    }

    struct SearchState {
        int cost = std::numeric_limits<int>::max();
        uint64_t parent = 0;
        unsigned char closed = 0;
    };
    struct Node {
        uint64_t key;
        int priority;
        bool operator<(const Node& other) const
        {
            return priority > other.priority;
        }
    };

    std::unordered_map<uint64_t, SearchState> states;
    states.reserve(8192);
    std::priority_queue<Node> open;

    uint64_t startKey = makeKey(start.x, start.y);
    uint64_t targetKey = makeKey(target.x, target.y);
    SearchState& startState = states[startKey];
    startState.cost = 0;
    startState.parent = startKey;
    open.push({startKey, heuristic(start, target)});

    int expanded = 0;

    while (!open.empty()) {
        if ((++expanded % kTimedSearchCheckInterval) == 0 && nowMs() >= deadline) {
            return false;
        }

        uint64_t currentKey = open.top().key;
        open.pop();
        auto currentIt = states.find(currentKey);
        if (currentIt == states.end() || currentIt->second.closed) {
            continue;
        }
        SearchState& currentState = currentIt->second;
        currentState.closed = 1;

        if (currentKey == targetKey) {
            std::vector<Point> reversePath;
            for (uint64_t key = targetKey;; key = states[key].parent) {
                reversePath.push_back(keyToPoint(key));
                if (key == startKey) {
                    break;
                }
            }
            path.reserve(reversePath.size() - 1);
            for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
                path.push_back(*it);
            }
            return !path.empty();
        }

        Point current = keyToPoint(currentKey);
        for (int i = 0; i < 8; ++i) {
            Point next{current.x + kNeighborOffsetsX[i], current.y + kNeighborOffsetsY[i]};
            if (!walkable(next)) {
                continue;
            }
            bool diagonal = kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0;
            if (diagonal && (!walkable({current.x + kNeighborOffsetsX[i], current.y}) || !walkable({current.x, current.y + kNeighborOffsetsY[i]}))) {
                continue;
            }

            int moveCost = diagonal ? 14 : 10;
            int nextCost = currentState.cost + moveCost;
            uint64_t nextKey = makeKey(next.x, next.y);
            SearchState& nextState = states[nextKey];
            if (nextCost >= nextState.cost) {
                continue;
            }

            nextState.cost = nextCost;
            nextState.parent = currentKey;
            int score = heuristic(next, target);
            int priority = nextCost > std::numeric_limits<int>::max() - score ? std::numeric_limits<int>::max() : nextCost + score;
            open.push({nextKey, priority});
        }
    }

    return false;
}

static bool buildOptimisticPath(session::ClientSession& session, Point start, Point target, std::vector<Point>& path)
{
    path.clear();

    Point planTarget = clampTarget(start, target);
    long long minX64 = static_cast<long long>(std::min(start.x, planTarget.x)) - kSearchPadding;
    long long maxX64 = static_cast<long long>(std::max(start.x, planTarget.x)) + kSearchPadding;
    long long minY64 = static_cast<long long>(std::min(start.y, planTarget.y)) - kSearchPadding;
    long long maxY64 = static_cast<long long>(std::max(start.y, planTarget.y)) + kSearchPadding;
    long long width64 = maxX64 - minX64 + 1;
    long long height64 = maxY64 - minY64 + 1;
    long long nodeCount64 = width64 * height64;
    if (width64 <= 0 || height64 <= 0 || nodeCount64 <= 0 || nodeCount64 > kMaxOptimisticNodes
        || minX64 < std::numeric_limits<int>::min() || maxX64 > std::numeric_limits<int>::max()
        || minY64 < std::numeric_limits<int>::min() || maxY64 > std::numeric_limits<int>::max()) {
        return false;
    }
    int minX = static_cast<int>(minX64);
    int maxX = static_cast<int>(maxX64);
    int minY = static_cast<int>(minY64);
    int maxY = static_cast<int>(maxY64);
    int width = static_cast<int>(width64);
    int nodeCount = static_cast<int>(nodeCount64);

    auto indexOf = [=](Point p) {
        return p.x - minX + (p.y - minY) * width;
    };
    auto inSearch = [=](Point p) {
        return p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
    };

    std::vector<int> parent(nodeCount, -1);
    std::vector<int> cost(nodeCount, std::numeric_limits<int>::max());
    std::vector<unsigned char> closed(nodeCount, 0);
    struct Node {
        int index;
        int priority;
        bool operator<(const Node& other) const
        {
            return priority > other.priority;
        }
    };
    std::priority_queue<Node> open;

    int startIndex = indexOf(start);
    int targetIndex = indexOf(planTarget);
    cost[startIndex] = 0;
    open.push({startIndex, heuristic(start, planTarget)});

    int bestIndex = startIndex;
    int bestScore = heuristic(start, target);
    int fallbackIndex = -1;
    int fallbackScore = std::numeric_limits<int>::max();
    bool fallbackIsRecent = false;

    while (!open.empty()) {
        int index = open.top().index;
        open.pop();
        if (closed[index]) {
            continue;
        }
        closed[index] = 1;

        Point current{minX + index % width, minY + index / width};
        int score = heuristic(current, target);
        bool currentIsRecent = isRecentPlayer(session, current);
        if (index != startIndex
            && (fallbackIndex == -1 || (fallbackIsRecent && !currentIsRecent)
                || (fallbackIsRecent == currentIsRecent
                    && (score < fallbackScore || (score == fallbackScore && cost[index] < cost[fallbackIndex]))))) {
            fallbackScore = score;
            fallbackIndex = index;
            fallbackIsRecent = currentIsRecent;
        }
        if (score < bestScore || (score == bestScore && cost[index] < cost[bestIndex])) {
            bestScore = score;
            bestIndex = index;
        }
        if (index == targetIndex) {
            bestIndex = index;
            break;
        }

        for (int i = 0; i < 8; ++i) {
            Point next{current.x + kNeighborOffsetsX[i], current.y + kNeighborOffsetsY[i]};
            if (!inSearch(next) || !optimisticWalkable(session, next)) {
                continue;
            }
            bool diagonal = kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0;
            if (diagonal) {
                Point sideX{current.x + kNeighborOffsetsX[i], current.y};
                Point sideY{current.x, current.y + kNeighborOffsetsY[i]};
                if (!optimisticWalkable(session, sideX) || !optimisticWalkable(session, sideY)) {
                    continue;
                }
            }

            int nextIndex = indexOf(next);
            int nextCost = cost[index] + optimisticMoveCost(session, next, diagonal);
            if (nextCost >= cost[nextIndex]) {
                continue;
            }
            cost[nextIndex] = nextCost;
            parent[nextIndex] = index;
            open.push({nextIndex, nextCost + heuristic(next, planTarget)});
        }
    }

    Point bestPoint{minX + bestIndex % width, minY + bestIndex / width};
    if (isRecentPlayer(session, bestPoint) && !samePoint(bestPoint, target) && fallbackIndex != -1 && !fallbackIsRecent) {
        bestIndex = fallbackIndex;
    }

    if (bestIndex == startIndex) {
        if (fallbackIndex == -1) {
            return false;
        }
        bestIndex = fallbackIndex;
    }

    std::vector<Point> reversePath;
    for (int index = bestIndex; index != -1; index = parent[index]) {
        reversePath.push_back({minX + index % width, minY + index / width});
    }
    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

static bool buildPath(session::ClientSession& session, Point start, Point target, std::vector<Point>& path)
{
    path.clear();
    if (!isValidMap(session)) {
        return false;
    }

    int startX = 0;
    int startY = 0;
    if (!toLocal(session, start, startX, startY)) {
        return false;
    }

    int targetX = 0;
    int targetY = 0;
    bool exactTarget = canWalkGlobal(session, target) && toLocal(session, target, targetX, targetY);
    int targetIndex = exactTarget ? targetX + targetY * session.WorldScene().map.spanX : -1;
    int startIndex = startX + startY * session.WorldScene().map.spanX;

    std::array<int, kMapBufferLimit> parent{};
    std::array<int, kMapBufferLimit> steps{};
    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(session.WorldScene().map.spanX * session.WorldScene().map.spanY);

    parent[startIndex] = -1;
    visited[startIndex] = 1;
    queue.push_back(startIndex);

    int bestIndex = startIndex;
    int bestStep = 0;
    int fallbackIndex = -1;
    long long fallbackScore = std::numeric_limits<long long>::max();
    int fallbackStep = 0;
    bool fallbackIsRecent = false;
    size_t head = 0;


    while (head < queue.size()) {
        int index = queue[head++];
        int cx = index % session.WorldScene().map.spanX;
        int cy = index / session.WorldScene().map.spanX;

        if (index == targetIndex) {
            bestIndex = index;
            break;
        }

        if (index != startIndex) {
            long long fallbackCandidateScore = distanceScore(session, cx, cy, target);
            bool currentIsRecent = isRecentPlayer(session, {session.WorldScene().map.xStart + cx, session.WorldScene().map.yStart + cy});
            if (fallbackIndex == -1 || (fallbackIsRecent && !currentIsRecent)
                || (fallbackIsRecent == currentIsRecent
                    && (fallbackCandidateScore < fallbackScore || (fallbackCandidateScore == fallbackScore && steps[index] < fallbackStep)))) {
                fallbackIndex = index;
                fallbackScore = fallbackCandidateScore;
                fallbackStep = steps[index];
                fallbackIsRecent = currentIsRecent;
            }
        }

        if (isBetterFallback(session, index, bestIndex, steps[index], bestStep, target)) {
            bestIndex = index;
            bestStep = steps[index];
        }

        for (int i = 0; i < 8; ++i) {
            int nx = cx + kNeighborOffsetsX[i];
            int ny = cy + kNeighborOffsetsY[i];
            if (!canWalkLocalPoint(session, nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(session, cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(session, cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * session.WorldScene().map.spanX;
            if (visited[nextIndex]) {
                continue;
            }
            visited[nextIndex] = 1;
            parent[nextIndex] = index;
            steps[nextIndex] = steps[index] + 1;
            queue.push_back(nextIndex);
        }
    }

    Point bestPoint{session.WorldScene().map.xStart + bestIndex % session.WorldScene().map.spanX, session.WorldScene().map.yStart + bestIndex / session.WorldScene().map.spanX};
    if (isRecentPlayer(session, bestPoint) && !samePoint(bestPoint, target) && fallbackIndex != -1 && !fallbackIsRecent) {
        bestIndex = fallbackIndex;
    }

    if (bestIndex == startIndex) {
        if (fallbackIndex == -1) {
            return false;
        }
        bestIndex = fallbackIndex;
    }

    std::vector<Point> reversePath;
    for (int index = bestIndex; index != -1; index = parent[index]) {
        reversePath.push_back({session.WorldScene().map.xStart + index % session.WorldScene().map.spanX, session.WorldScene().map.yStart + index / session.WorldScene().map.spanX});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

static bool buildNearRetargetPath(session::ClientSession& session, Point start, Point target, std::vector<Point>& path)
{
    path.clear();
    if (!isValidMap(session)) {
        return false;
    }

    int startX = 0;
    int startY = 0;
    if (!toLocal(session, start, startX, startY)) {
        return false;
    }

    int startIndex = startX + startY * session.WorldScene().map.spanX;
    std::array<int, kMapBufferLimit> parent{};
    std::array<int, kMapBufferLimit> steps{};
    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(session.WorldScene().map.spanX * session.WorldScene().map.spanY);

    parent[startIndex] = -1;
    visited[startIndex] = 1;
    queue.push_back(startIndex);

    int bestIndex = -1;
    long long bestScore = std::numeric_limits<long long>::max();
    int bestStep = 0;
    size_t head = 0;


    while (head < queue.size()) {
        int index = queue[head++];
        int cx = index % session.WorldScene().map.spanX;
        int cy = index / session.WorldScene().map.spanX;
        Point current{session.WorldScene().map.xStart + cx, session.WorldScene().map.yStart + cy};

        if (index != startIndex && (!isRecentPlayer(session, current) || samePoint(current, target))) {
            long long score = distanceScore(session, cx, cy, target);
            if (bestIndex == -1 || score < bestScore || (score == bestScore && steps[index] < bestStep)) {
                bestIndex = index;
                bestScore = score;
                bestStep = steps[index];
            }
        }
        if (steps[index] >= kNearRetargetMaxSteps) {
            continue;
        }

        for (int i = 0; i < 8; ++i) {
            int nx = cx + kNeighborOffsetsX[i];
            int ny = cy + kNeighborOffsetsY[i];
            if (!canWalkLocalPoint(session, nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(session, cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(session, cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * session.WorldScene().map.spanX;
            if (visited[nextIndex]) {
                continue;
            }
            visited[nextIndex] = 1;
            parent[nextIndex] = index;
            steps[nextIndex] = steps[index] + 1;
            queue.push_back(nextIndex);
        }
    }

    if (bestIndex == -1) {
        return false;
    }

    std::vector<Point> reversePath;
    for (int index = bestIndex; index != -1; index = parent[index]) {
        reversePath.push_back({session.WorldScene().map.xStart + index % session.WorldScene().map.spanX, session.WorldScene().map.yStart + index / session.WorldScene().map.spanX});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

static void requestMove(session::ClientSession& session, Point target)
{
    Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    gAutoTargetX = target.x;
    gAutoTargetY = target.y;
    session.Lua().copilot.data->expectedAutoTarget = target;
    session.Lua().copilot.data->hasExpectedAutoTarget = true;
    // m_log_iw(L"[copilot] requestMove waypoint=%d,%d player=%d,%d\n", target.x, target.y, player.x, player.y);
    // logDiskCellDetail(session, target, "request");
    session.Lua().copilot.data->lastRequestedWaypoint = target;
    session.Lua().copilot.data->hasLastRequestedWaypoint = true;
    session.Lua().copilot.data->lastRequestPlayer = player;
    session.Lua().copilot.data->lastRequestDistance = pointDistance(player, target);
    session.Lua().copilot.data->hasPendingMoveCheck = true;
    session.Lua().copilot.data->moveCheckSawBusy = false;
    session.WorldScene().movement.pathRebuildPending = 1;
}

static void cacheWaypoint(session::ClientSession& session, Point player, Point target, Point waypoint)
{
    session.Lua().copilot.data->lastPlanPlayer = player;
    session.Lua().copilot.data->cachedTarget = target;
    session.Lua().copilot.data->cachedWaypoint = waypoint;
    session.Lua().copilot.data->cachedMapId = session.WorldScene().map.mapId;
    session.Lua().copilot.data->cachedMapFloor = session.WorldScene().map.floor;
    session.Lua().copilot.data->cachedMapGroup = session.WorldScene().map.cachePathKey;
    session.Lua().copilot.data->hasCachedWaypoint = true;
}

static bool tryUseCachedWaypoint(session::ClientSession& session, Point player, Point target)
{
    if (!session.Lua().copilot.data->hasCachedWaypoint || !samePoint(target, session.Lua().copilot.data->cachedTarget) || static_cast<std::uint32_t>(session.Lua().copilot.data->cachedMapId) != session.WorldScene().map.mapId || session.Lua().copilot.data->cachedMapFloor != session.WorldScene().map.floor
        || session.Lua().copilot.data->cachedMapGroup != session.WorldScene().map.cachePathKey) {
        return false;
    }
    if (samePoint(player, session.Lua().copilot.data->cachedWaypoint) || pointDistance(player, session.Lua().copilot.data->cachedWaypoint) > kMoveSegmentSteps
        || pointDistance(player, session.Lua().copilot.data->lastPlanPlayer) >= kReplanMoveDistance) {
        resetPlanCache(session);
        return false;
    }
    if (!isForwardWaypoint(session, player, session.Lua().copilot.data->cachedWaypoint)) {
        // m_log_iw(L"[copilot] drop cached waypoint=%d,%d player=%d,%d\n", session.Lua().copilot.data->cachedWaypoint.x, session.Lua().copilot.data->cachedWaypoint.y, player.x, player.y);
        resetPlanCache(session);
        return false;
    }

    // m_log_iw(L"[copilot] reuse waypoint=%d,%d player=%d,%d moved=%d\n", session.Lua().copilot.data->cachedWaypoint.x, session.Lua().copilot.data->cachedWaypoint.y, player.x, player.y,
    //     pointDistance(player, session.Lua().copilot.data->lastPlanPlayer));
    requestMove(session, session.Lua().copilot.data->cachedWaypoint);
    return true;
}

static bool pickVisibleWaypoint(session::ClientSession& session, const std::vector<Point>& path, Point player, Point& waypoint, bool allowRecentFallback = false)
{
    if (path.empty()) {
        return false;
    }
    auto avoidRecent = [&session](Point p) {
        return isRecentPlayer(session, p) && !samePoint(p, session.Lua().copilot.data->copilotTarget);
    };
    int lastIndex = std::min<int>(kMoveSegmentSteps - 1, static_cast<int>(path.size()) - 1);
    int recentIndex = -1;
    int oneWayIndex = -1;
    int oneWayRecentIndex = -1;
    for (int i = lastIndex; i >= 0; --i) {
        Point next = path[i];
        if (pointDistance(player, next) > kMoveSegmentSteps) {
            // m_log_iw(L"[copilot] reject waypoint=%d,%d player=%d,%d reason=too-far\n", next.x, next.y, player.x, player.y);
            continue;
        }
        bool recent = avoidRecent(next);
        if (isBidirectionalWaypoint(session, player, next)) {
            if (recent) {
                if (recentIndex == -1) {
                    recentIndex = i;
                }
                continue;
            }
            waypoint = next;
            return true;
        }
        if (!isForwardWaypoint(session, player, next)) {
            continue;
        }
        if (recent) {
            if (oneWayRecentIndex == -1) {
                oneWayRecentIndex = i;
            }
            continue;
        }
        if (oneWayIndex == -1) {
            oneWayIndex = i;
        }
    }
    if (oneWayIndex != -1) {
        waypoint = path[oneWayIndex];
        // m_log_iw(L"[copilot] use one-way waypoint=%d,%d index=%d\n", waypoint.x, waypoint.y, oneWayIndex);
        return true;
    }
    if (recentIndex != -1) {
        if (!allowRecentFallback) {
            // m_log_iw(L"[copilot] skip recent waypoint=%d,%d recent=%d\n", path[recentIndex].x, path[recentIndex].y, session.Lua().copilot.data->recentPlayerCount);
            return false;
        }
        waypoint = path[recentIndex];
        // m_log_iw(L"[copilot] use recent waypoint=%d,%d index=%d recent=%d\n", waypoint.x, waypoint.y, recentIndex, session.Lua().copilot.data->recentPlayerCount);
        return true;
    }
    if (oneWayRecentIndex != -1) {
        if (!allowRecentFallback) {
            // m_log_iw(L"[copilot] skip recent waypoint=%d,%d recent=%d\n", path[oneWayRecentIndex].x, path[oneWayRecentIndex].y, session.Lua().copilot.data->recentPlayerCount);
            return false;
        }
        waypoint = path[oneWayRecentIndex];
        // m_log_iw(L"[copilot] use one-way recent waypoint=%d,%d index=%d recent=%d\n", waypoint.x, waypoint.y, oneWayRecentIndex,
            // session.Lua().copilot.data->recentPlayerCount);
        return true;
    }
    return false;
}

} // namespace copilot_detail

CopilotRuntimeState::CopilotRuntimeState()
    : data(std::make_unique<copilot_detail::CopilotState>())
{
}

CopilotRuntimeState::~CopilotRuntimeState() = default;


// 目标改变时可只刷新显示路径；已有移动段仍沿原忙碌门禁完成，不提前发出下一段。
static void PlanCopilot(session::ClientSession& session, int x, int y, bool routeOnly)
{
    if (copilot_detail::skipSceneNotReady(session, "autoCopilot")) {
        return;
    }
    copilot_detail::syncVisibleCache(session);

    copilot_detail::Point target{x, y};
    copilot_detail::Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    // m_log_iw(L"[copilot] plan player=%d,%d target=%d,%d map=%d group=%d floor=%d scene=%d state=%d\n", player.x, player.y, target.x, target.y, session.WorldScene().map.mapId, session.WorldScene().map.cachePathKey, session.WorldScene().map.floor, session.WorldScene().sceneType, session.WorldScene().sceneState);
    if (player.x == target.x && player.y == target.y) {
        // m_log_iw(L"[copilot] already arrived target=%d,%d\n", target.x, target.y);
        copilot_detail::clearCopilotRoute(session);
        return;
    }
    if (!copilot_detail::isValidMap(session)) {
        // m_log_iw(L"[copilot] skip invalid visible map width=%d height=%d cells=%d limit=%d\n", session.WorldScene().map.spanX, session.WorldScene().map.spanY, session.WorldScene().map.spanX * session.WorldScene().map.spanY, kMapBufferLimit);
        copilot_detail::clearCopilotRoute(session);
        return;
    }
    if (!copilot_detail::isSaneTarget(player, target)) {
        // m_log_iw(L"[copilot] skip insane target player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
        copilot_detail::clearCopilotRoute(session);
        return;
    }
    if (!routeOnly && copilot_detail::isBusy(session)) {
        // m_log_iw(L"[copilot] skip busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", session.WorldScene().movement.pathCount, session.WorldScene().movement.pendingDirectionCount, session.WorldScene().movement.inputBlocked, session.WorldScene().movement.transitionRequestPending, session.WorldScene().movement.directionalTransitionPending);
        return;
    }
    if (copilot_detail::loadDiskMap(session) && (!copilot_detail::inDiskBounds(session, player) || !copilot_detail::inDiskBounds(session, target))) {
        copilot_detail::clearCopilotRoute(session);
        return;
    }
    if (!routeOnly && copilot_detail::tryUseCachedWaypoint(session, player, target)) {
        return;
    }

    std::vector<copilot_detail::Point> path;
    copilot_detail::Point waypoint{};

    bool fullPath = copilot_detail::buildTimedFullPath(session, player, target, copilot_detail::kFullSearchTimeLimitMs, path);
    // m_log_iw(L"[copilot] fullPath ok=%d steps=%u\n", fullPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (fullPath && copilot_detail::pickVisibleWaypoint(session, path, player, waypoint)) {
        // m_log_iw(L"[copilot] use fullPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(session, path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(session, player, target, waypoint);
        copilot_detail::requestMove(session, waypoint);
        return;
    }

    bool optimisticPath = copilot_detail::buildOptimisticPath(session, player, target, path);
    // m_log_iw(L"[copilot] optimisticPath ok=%d steps=%u\n", optimisticPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (optimisticPath && copilot_detail::pickVisibleWaypoint(session, path, player, waypoint)) {
        // m_log_iw(L"[copilot] use optimisticPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(session, path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(session, player, target, waypoint);
        copilot_detail::requestMove(session, waypoint);
        return;
    }

    bool localPath = copilot_detail::buildPath(session, player, target, path);
    // m_log_iw(L"[copilot] localPath ok=%d steps=%u\n", localPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (localPath && copilot_detail::pickVisibleWaypoint(session, path, player, waypoint)) {
        // m_log_iw(L"[copilot] use localPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(session, path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(session, player, target, waypoint);
        copilot_detail::requestMove(session, waypoint);
        return;
    }

    bool nearPath = copilot_detail::buildNearRetargetPath(session, player, target, path);
    // m_log_iw(L"[copilot] nearPath ok=%d steps=%u\n", nearPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (nearPath && copilot_detail::pickVisibleWaypoint(session, path, player, waypoint, true)) {
        // m_log_iw(L"[copilot] use nearPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(session, path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(session, player, target, waypoint);
        copilot_detail::requestMove(session, waypoint);
        return;
    }

    // m_log_iw(L"[copilot] no path player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
    // logNoPathDetail(session, player, target);
    copilot_detail::clearCopilotRoute(session);
}

void autoCopilot(session::ClientSession& session, int x, int y)
{
    PlanCopilot(session, x, y, false);
}

void startCopilot(session::ClientSession& session, int x, int y)
{
    session.Lua().copilot.data->copilotTarget = {x, y};
    session.Lua().copilot.data->copilotMapId = session.WorldScene().map.mapId;
    session.Lua().copilot.data->copilotMapFloor = session.WorldScene().map.floor;
    session.Lua().copilot.data->copilotEnabled = true;
    session.Lua().copilot.data->lastCopilotTickMs = 0;
    session.Lua().copilot.data->lastBusyLogMs = 0;
    session.Lua().copilot.data->hasLastObservedPlayer = false;
    copilot_detail::clearRecentPlayers(session);
    copilot_detail::resetPlanCache(session);
    copilot_detail::resetMoveProgress(session);
    session.Lua().copilot.data->dynamicBlocked.clear();
    session.Lua().copilot.data->lastNoPathDetailLogMs = 0;
    session.Lua().copilot.data->lastSceneSkipLogMs = 0;
    session.Lua().copilot.data->lastCellLogKey = 0;
    session.Lua().copilot.data->lastCellLogMs = 0;
    session.Lua().copilot.data->hasLastRequestedWaypoint = false;
    copilot_detail::clearCopilotRoute(session);
    m_log_iw(L"[copilot] start target=%d,%d player=%d,%d scene=%d state=%d\n", x, y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY), session.WorldScene().sceneType, session.WorldScene().sceneState);
    PlanCopilot(session, x, y, true);
}

void stopCopilot(session::ClientSession& session)
{
    if (session.Lua().copilot.data->copilotEnabled) {
        m_log_iw(L"[copilot] stop target=%d,%d player=%d,%d\n", session.Lua().copilot.data->copilotTarget.x, session.Lua().copilot.data->copilotTarget.y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY));
    }
    session.Lua().copilot.data->copilotEnabled = false;
    copilot_detail::resetPlanCache(session);
    copilot_detail::resetMoveProgress(session);
    copilot_detail::clearCopilotRoute(session);
}

int getAutoCopliotState(session::ClientSession& session)
{
    return session.Lua().copilot.data->copilotEnabled ? 1 : 0;
}

std::vector<std::pair<int, int> > getAutoCopliotRoute(session::ClientSession& session)
{
    std::vector<std::pair<int, int> > route;
    if (!session.Lua().copilot.data->copilotEnabled || !copilot_detail::isCopilotSceneReady(session) || session.WorldScene().map.mapId != static_cast<std::uint32_t>(session.Lua().copilot.data->copilotMapId) || session.WorldScene().map.floor != session.Lua().copilot.data->copilotMapFloor) {
        return route;
    }

    route.reserve(session.Lua().copilot.data->autoCopilotRoute.size());
    for (const auto& point: session.Lua().copilot.data->autoCopilotRoute) {
        route.emplace_back(point.x, point.y);
    }
    return route;
}

void CopilotProc(session::ClientSession& session)
{
    if (!session.Lua().copilot.data->copilotEnabled) {
        return;
    }
    if (session.WorldScene().map.mapId != static_cast<std::uint32_t>(session.Lua().copilot.data->copilotMapId) || session.WorldScene().map.floor != session.Lua().copilot.data->copilotMapFloor) {
        m_log_iw(L"[copilot] stop map changed expected=%d,%d actual=%d,%d target=%d,%d player=%d,%d\n", session.Lua().copilot.data->copilotMapId,
            session.Lua().copilot.data->copilotMapFloor, session.WorldScene().map.mapId, session.WorldScene().map.floor, session.Lua().copilot.data->copilotTarget.x, session.Lua().copilot.data->copilotTarget.y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY));
        stopCopilot(session);
        return;
    }
    if (copilot_detail::skipSceneNotReady(session, "proc")) {
        return;
    }

    copilot_detail::Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    if (player.x == session.Lua().copilot.data->copilotTarget.x && player.y == session.Lua().copilot.data->copilotTarget.y) {
        m_log_iw(L"[copilot] arrived target=%d,%d\n", session.Lua().copilot.data->copilotTarget.x, session.Lua().copilot.data->copilotTarget.y);
        stopCopilot(session);
        return;
    }
    if (session.Lua().copilot.data->hasExpectedAutoTarget && copilot_detail::samePoint(player, session.Lua().copilot.data->expectedAutoTarget)) {
        copilot_detail::resetMoveProgress(session);
    }
    if (session.Lua().copilot.data->hasExpectedAutoTarget) {
        copilot_detail::Point clientTarget{static_cast<std::int32_t>(copilot_detail::gAutoTargetX), static_cast<std::int32_t>(copilot_detail::gAutoTargetY)};
        if (!copilot_detail::samePoint(clientTarget, session.Lua().copilot.data->expectedAutoTarget)) {
            m_log_iw(L"[copilot] stop external target expected=%d,%d actual=%d,%d player=%d,%d\n", session.Lua().copilot.data->expectedAutoTarget.x,
                session.Lua().copilot.data->expectedAutoTarget.y, clientTarget.x, clientTarget.y, player.x, player.y);
            stopCopilot(session);
            return;
        }
    }
    uint64_t now = copilot_detail::nowMs();
    if (copilot_detail::isBusy(session)) {
        if (session.Lua().copilot.data->hasPendingMoveCheck) {
            session.Lua().copilot.data->moveCheckSawBusy = true;
        }
        if (session.Lua().copilot.data->lastBusyLogMs == 0 || now - session.Lua().copilot.data->lastBusyLogMs >= 1000) {
            session.Lua().copilot.data->lastBusyLogMs = now;
            m_log_iw(L"[copilot] wait busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", session.WorldScene().movement.pathCount, session.WorldScene().movement.pendingDirectionCount, session.WorldScene().movement.inputBlocked, session.WorldScene().movement.transitionRequestPending, session.WorldScene().movement.directionalTransitionPending);
        }
        return;
    }


    if (session.Lua().copilot.data->lastCopilotTickMs != 0 && now - session.Lua().copilot.data->lastCopilotTickMs < copilot_detail::kCopilotTickMs) {
        return;
    }

    if (!session.Lua().copilot.data->hasLastObservedPlayer) {
        session.Lua().copilot.data->lastObservedPlayer = player;
        session.Lua().copilot.data->hasLastObservedPlayer = true;
    } else if (player.x != session.Lua().copilot.data->lastObservedPlayer.x || player.y != session.Lua().copilot.data->lastObservedPlayer.y) {
        copilot_detail::rememberRecentPlayer(session, session.Lua().copilot.data->lastObservedPlayer);
        session.Lua().copilot.data->lastObservedPlayer = player;
    }

    int requestDist = session.Lua().copilot.data->hasPendingMoveCheck ? copilot_detail::pointDistance(player, session.Lua().copilot.data->lastRequestedWaypoint) : 0;
    int requestProgress = session.Lua().copilot.data->hasPendingMoveCheck ? session.Lua().copilot.data->lastRequestDistance - requestDist : 0;
    int movedFromRequest = session.Lua().copilot.data->hasPendingMoveCheck ? copilot_detail::pointDistance(player, session.Lua().copilot.data->lastRequestPlayer) : 0;
    if (session.Lua().copilot.data->hasPendingMoveCheck) {
        if (requestDist == 0) {
            copilot_detail::resetMoveProgress(session);
        } else if (session.Lua().copilot.data->moveCheckSawBusy || movedFromRequest > 0) {
            if (requestProgress <= 0 && movedFromRequest == 0) {
                copilot_detail::markDynamicBlockedByMapSource(session, session.Lua().copilot.data->lastRequestedWaypoint, "stuck");
                session.Lua().copilot.data->hasLastRequestedWaypoint = false;
                copilot_detail::resetPlanCache(session);
                copilot_detail::resetMoveProgress(session);
            } else if (movedFromRequest > 0 || requestProgress > 0) {
                session.Lua().copilot.data->lastRequestPlayer = player;
                session.Lua().copilot.data->lastRequestDistance = requestDist;
                session.Lua().copilot.data->moveCheckSawBusy = false;
            }
        }
    }

    session.Lua().copilot.data->lastCopilotTickMs = now;
    // m_log_iw(L"[copilot] proc player=%d,%d target=%d,%d reqDist=%d reqProgress=%d scene=%d state=%d\n", player.x, player.y,
    //     session.Lua().copilot.data->copilotTarget.x, session.Lua().copilot.data->copilotTarget.y, requestDist, requestProgress, session.WorldScene().sceneType, session.WorldScene().sceneState);
    autoCopilot(session, session.Lua().copilot.data->copilotTarget.x, session.Lua().copilot.data->copilotTarget.y);
}

} // namespace cg::script
