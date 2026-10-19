#include "LuaUI.hpp"
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
static auto& gMoveRequest = world::gPlayerPathRebuildPending_C821FC;
static auto& gPathCount = world::gMovePathCount_C82200;
static auto& gMoveQueueCount = world::gPendingDirectionCount_C82202;
static auto& gMoveLock = world::gMapMovementInputBlocked_C82220;
static auto& gTileEventLock = world::gMapTransitionRequestPending_9A86D8;
static auto& gTargetEventLock = world::gDirectionalMapTransitionPending_9FB92E;
static auto& gMapWidth = world::gMapSpanX;
static auto& gMapHeight = world::gMapSpanY;
static auto& gMoveBlockMap = world::gMapWalkabilityArray;
static auto& gMapId = world::gCurrentMapId_9A86DC;
static auto& gMapGroup = world::gMapCachePathKey_9E0434;
static auto& gMapFloor = world::gMapFloor;

static std::unordered_map<uint64_t, unsigned char> gKnownMap;
static int gLastMapId = -1;
static int gLastMapFloor = -1;
static int gLastMapGroup = -1;

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

static DiskMapCache gDiskMap;
static bool gCopilotEnabled = false;
static Point gCopilotTarget{};
static int gCopilotMapId = -1;
static int gCopilotMapFloor = -1;
static uint64_t gLastCopilotTickMs = 0;
static uint64_t gLastBusyLogMs = 0;
static uint64_t gLastSceneSkipLogMs = 0;
static uint64_t gLastCellLogKey = 0;
static uint64_t gLastCellLogMs = 0;
static uint64_t gLastNoPathDetailLogMs = 0;
static Point gLastObservedPlayer{};
static bool gHasLastObservedPlayer = false;
static std::array<Point, kRecentAvoidCount> gRecentPlayers{};
static int gRecentPlayerCount = 0;
static std::unordered_map<uint64_t, uint64_t> gDynamicBlocked;
static Point gLastRequestedWaypoint{};
static bool gHasLastRequestedWaypoint = false;
static Point gExpectedAutoTarget{};
static bool gHasExpectedAutoTarget = false;
static Point gLastRequestPlayer{};
static int gLastRequestDistance = 0;
static bool gHasPendingMoveCheck = false;
static bool gMoveCheckSawBusy = false;
static Point gLastPlanPlayer{};
static Point gCachedWaypoint{};
static Point gCachedTarget{};
static int gCachedMapId = -1;
static int gCachedMapFloor = -1;
static int gCachedMapGroup = -1;
static bool gHasCachedWaypoint = false;
static std::vector<Point> gAutoCopilotRoute;

static uint64_t nowMs()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

static void clearRecentPlayers();
static void resetPlanCache();
static void resetMoveProgress();
static void clearCopilotRoute();

static bool isCopilotSceneReady()
{
    return world::gCurrentSceneType == SCENE_NORMAL && world::gCurrentSceneState == 3;
}

static bool skipSceneNotReady([[maybe_unused]] const char* where)
{
    if (isCopilotSceneReady()) {
        return false;
    }

    uint64_t now = nowMs();
    if (gLastSceneSkipLogMs == 0 || now - gLastSceneSkipLogMs >= kSceneSkipLogMs) {
        gLastSceneSkipLogMs = now;
        // m_log_i("[copilot] skip scene where=%s scene=%d state=%d needScene=%d needState=3\n", where, world::gCurrentSceneType, world::gCurrentSceneState, SCENE_NORMAL);
    }
    gHasLastObservedPlayer = false;
    clearRecentPlayers();
    resetPlanCache();
    resetMoveProgress();
    clearCopilotRoute();
    gHasLastRequestedWaypoint = false;
    gLastCopilotTickMs = 0;
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

static void applySingleCellRule(std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(raw);
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

static void applyObjectRule(std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(raw);
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

    applySingleCellRule(state, width, height, raw, x, y);
}

static std::vector<unsigned char> synthesizeDiskCells(
    int width, int height, const std::vector<unsigned short>& tileData, const std::vector<unsigned short>& objData, const std::vector<unsigned short>& hitData)
{
    size_t cellCount = tileData.size();
    std::vector<unsigned char> state(cellCount, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            applySingleCellRule(state, width, height, tileData[diskIndex(width, x, y)], x, y);
        }
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            applyObjectRule(state, width, height, objData[diskIndex(width, x, y)], x, y);
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

static bool isBusy()
{
    return gPathCount > 0 || gMoveQueueCount > 0 || gMoveLock != 0 || gTileEventLock != 0 || gTargetEventLock != 0;
}

static bool isValidMap()
{
    return gMapWidth > 0 && gMapHeight > 0 && gMapWidth * gMapHeight <= kMapBufferLimit;
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

static void resetPlanCache()
{
    gHasCachedWaypoint = false;
}

static void resetMoveProgress()
{
    gHasPendingMoveCheck = false;
    gMoveCheckSawBusy = false;
    gHasExpectedAutoTarget = false;
}

static void clearCopilotRoute()
{
    gAutoCopilotRoute.clear();
}

static void cacheCopilotRoute(const std::vector<Point>& path)
{
    gAutoCopilotRoute = path;
}

static void clearRecentPlayers()
{
    gRecentPlayerCount = 0;
}

static void rememberRecentPlayer(Point p)
{
    if (gRecentPlayerCount > 0 && samePoint(gRecentPlayers[0], p)) {
        return;
    }
    int moveCount = std::min(gRecentPlayerCount, kRecentAvoidCount - 1);
    for (int i = moveCount; i > 0; --i) {
        gRecentPlayers[i] = gRecentPlayers[i - 1];
    }
    gRecentPlayers[0] = p;
    if (gRecentPlayerCount < kRecentAvoidCount) {
        ++gRecentPlayerCount;
    }
}

static bool isRecentPlayer(Point p)
{
    for (int i = 0; i < gRecentPlayerCount; ++i) {
        if (samePoint(gRecentPlayers[i], p)) {
            return true;
        }
    }
    return false;
}

static bool isDynamicBlocked(Point p)
{
    uint64_t key = makeKey(p.x, p.y);
    auto it = gDynamicBlocked.find(key);
    if (it == gDynamicBlocked.end()) {
        return false;
    }

    uint64_t now = nowMs();
    if (it->second <= now) {
        gDynamicBlocked.erase(it);
        return false;
    }
    return true;
}
static bool putDynamicBlocked(Point p)
{
    bool existed = isDynamicBlocked(p);
    gDynamicBlocked[makeKey(p.x, p.y)] = nowMs() + kDynamicBlockMs;
    return !existed;
}

static void markDynamicBlocked(Point p, [[maybe_unused]] const char* reason)
{
    bool added = putDynamicBlocked(p);
    if (added) {
        // m_log_i("[copilot] dynamic block %d,%d reason=%s ttl=%u\n", p.x, p.y, reason, static_cast<unsigned int>(kDynamicBlockMs));
    }
}


static void syncVisibleCache()
{
    if (!isValidMap()) {
        return;
    }
    if (static_cast<std::uint32_t>(gLastMapId) != gMapId || gLastMapFloor != gMapFloor || gLastMapGroup != gMapGroup) {
        // m_log_i("[copilot] map changed map=%d group=%d floor=%d visible=%dx%d start=%d,%d\n", gMapId, gMapGroup, gMapFloor, gMapWidth, gMapHeight, gMapXStart, gMapYStart);
        gKnownMap.clear();
        gDiskMap = DiskMapCache{};
        gDynamicBlocked.clear();
        gHasLastRequestedWaypoint = false;
        resetPlanCache();
        resetMoveProgress();
        gLastMapId = gMapId;
        gLastMapFloor = gMapFloor;
        gLastMapGroup = gMapGroup;
    }

    for (int y = 0; y < gMapHeight; ++y) {
        for (int x = 0; x < gMapWidth; ++x) {
            int globalX = cg::world::gMapXStart + x;
            int globalY = cg::world::gMapYStart + y;
            gKnownMap[makeKey(globalX, globalY)] = gMoveBlockMap[x + y * gMapWidth] == 1 ? kKnownBlocked : kKnownWalkable;
        }
    }
}

static std::wstring makeMapFileName()
{
    return gMapId ? std::format(L"map\\{}\\{}\\{}.dat", gMapId, gMapGroup, gMapFloor)
        : std::format(L"map\\0\\{}.dat", gMapFloor);
}

static bool loadDiskMap()
{
    uint64_t now = nowMs();
    bool sameMap = static_cast<std::uint32_t>(gDiskMap.mapId) == gMapId && gDiskMap.floor == gMapFloor && gDiskMap.group == gMapGroup;
    if (sameMap && gDiskMap.loaded) {
        if (now - gDiskMap.lastLoadedMs < kDiskMapRefreshMs || now - gDiskMap.lastAttemptMs < kDiskMapRetryMs) {
            return true;
        }
    }
    if (sameMap && !gDiskMap.loaded && now - gDiskMap.lastAttemptMs < kDiskMapRetryMs) {
        return false;
    }

    DiskMapCache next;
    next.mapId = gMapId;
    next.floor = gMapFloor;
    next.group = gMapGroup;
    next.lastAttemptMs = now;

    auto fail = [&]([[maybe_unused]] const char* reason) {
        // m_log_i("[copilot] loadDiskMap failed reason=%s map=%d group=%d floor=%d keepOld=%d\n", reason, gMapId, gMapGroup, gMapFloor, sameMap && gDiskMap.loaded ? 1 : 0);
        if (sameMap && gDiskMap.loaded) {
            gDiskMap.lastAttemptMs = now;
            return true;
        }
        gDiskMap = next;
        return false;
    };

    FILE* file = _wfopen(makeMapFileName().c_str(), L"rb");
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
    next.cells = synthesizeDiskCells(next.width, next.height, tileData, objData, hitData);
    next.tileData = std::move(tileData);
    next.objData = std::move(objData);
    next.hitData = std::move(hitData);
    next.loaded = true;
    next.lastLoadedMs = now;
    [[maybe_unused]] size_t blockedCount = std::count(next.cells.begin(), next.cells.end(), static_cast<unsigned char>(kKnownBlocked));
    // m_log_i("[copilot] loadDiskMap ok map=%d group=%d floor=%d size=%dx%d blocked=%u\n", gMapId, gMapGroup, gMapFloor, next.width, next.height, static_cast<unsigned int>(blockedCount));
    gDiskMap = next;
    return true;
}

static unsigned char diskCell(Point p)
{
    if (!loadDiskMap()) {
        return kKnownWalkable;
    }
    if (p.x < 0 || p.y < 0 || p.x >= gDiskMap.width || p.y >= gDiskMap.height) {
        return kKnownWalkable;
    }
    return gDiskMap.cells[p.x + p.y * gDiskMap.width];
}

static bool inDiskBounds(Point p)
{
    return gDiskMap.loaded && p.x >= 0 && p.y >= 0 && p.x < gDiskMap.width && p.y < gDiskMap.height;
}

static unsigned char cachedKnownOrDiskCell(Point p)
{
    if (isDynamicBlocked(p)) {
        return kKnownBlocked;
    }
    auto it = gKnownMap.find(makeKey(p.x, p.y));
    if (it != gKnownMap.end()) {
        return it->second;
    }
    if (!inDiskBounds(p)) {
        return kKnownWalkable;
    }
    return gDiskMap.cells[static_cast<size_t>(p.x) + static_cast<size_t>(p.y) * gDiskMap.width];
}

struct CellGraphicInfo {
    int imageId = 0;
    std::int32_t seqNo = 0;
    unsigned short flag = 0xFFFF;
    unsigned short sx = 0;
    unsigned short sy = 0;
};

static CellGraphicInfo readCellGraphicInfo(unsigned short raw)
{
    CellGraphicInfo info;
    info.imageId = world::ResolveMapGraphicNumber_46CF00(raw);
    if (info.imageId > 999 || (info.imageId >= 60 && info.imageId <= 79)) {
        resource::GetGraphicMapNoIndex(info.imageId, &info.seqNo);
        resource::GetGraphicFlag1(info.seqNo, &info.flag);
        if (info.imageId > 999) {
            resource::GetGraphicSxSy(info.seqNo, &info.sx, &info.sy);
        }
    }
    return info;
}

static unsigned char rawBlockState(unsigned short raw)
{
    int imageId = world::ResolveMapGraphicNumber_46CF00(raw);
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

[[maybe_unused]] static void logFootprintSources(Point p, [[maybe_unused]] const char* label)
{
    if (!gDiskMap.loaded || !inDiskBounds(p)) {
        return;
    }

    int logged = 0;
    int minX = std::max(0, p.x - kObjFootprintScan);
    int maxX = std::min(gDiskMap.width - 1, p.x);
    int minY = p.y;
    int maxY = std::min(gDiskMap.height - 1, p.y + kObjFootprintScan);
    for (int y = minY; y <= maxY && logged < 8; ++y) {
        for (int x = minX; x <= maxX && logged < 8; ++x) {
            size_t index = diskIndex(gDiskMap.width, x, y);
            if (index >= gDiskMap.objData.size()) {
                continue;
            }
            unsigned short objRaw = gDiskMap.objData[index];
            if (objRaw == 0) {
                continue;
            }
            CellGraphicInfo objInfo = readCellGraphicInfo(objRaw);
            if (objInfo.imageId <= 999 || !footprintCovers({x, y}, objInfo, p)) {
                continue;
            }
            // m_log_i("[copilot] cell %s cover p=%d,%d byObj=%d,%d raw=%04X img=%d flag=%u sx=%u sy=%u blockValue=%u\n", label, p.x, p.y, x, y, objRaw, objInfo.imageId, objInfo.flag, objInfo.sx, objInfo.sy, objInfo.flag == 0 ? 1 : objInfo.flag == 2 ? 2 : 0);
            ++logged;
        }
    }
}

[[maybe_unused]] static void logDiskCellDetail(Point p, [[maybe_unused]] const char* label);
static int markObjectFootprintBlocked(Point origin, [[maybe_unused]] unsigned short raw, CellGraphicInfo info, [[maybe_unused]] Point cover, [[maybe_unused]] const char* reason)
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
            if (!inDiskBounds(cell)) {
                continue;
            }
            ++marked;
            if (putDynamicBlocked(cell)) {
                ++added;
            }
        }
    }

    if (marked > 0) {
        // m_log_i("[copilot] dynamic obj block origin=%d,%d raw=%04X img=%d flag=%u sx=%u sy=%u cover=%d,%d cells=%d new=%d reason=%s ttl=%u\n", origin.x, origin.y, raw, info.imageId, info.flag, info.sx, info.sy, cover.x, cover.y, marked, added, reason, static_cast<unsigned int>(kDynamicBlockMs));
    }
    return marked;
}

static int markCoveringObjectBlocks(Point p, [[maybe_unused]] const char* reason)
{
    if (!gDiskMap.loaded || !inDiskBounds(p)) {
        return 0;
    }

    int marked = 0;
    int minX = std::max(0, p.x - kObjFootprintScan);
    int maxX = std::min(gDiskMap.width - 1, p.x);
    int minY = p.y;
    int maxY = std::min(gDiskMap.height - 1, p.y + kObjFootprintScan);
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            size_t index = diskIndex(gDiskMap.width, x, y);
            if (index >= gDiskMap.objData.size()) {
                continue;
            }
            unsigned short objRaw = gDiskMap.objData[index];
            if (objRaw == 0) {
                continue;
            }
            CellGraphicInfo objInfo = readCellGraphicInfo(objRaw);
            if (objInfo.imageId <= 999 || objInfo.flag != 0 || !footprintCovers({x, y}, objInfo, p)) {
                continue;
            }
            marked += markObjectFootprintBlocked({x, y}, objRaw, objInfo, p, reason);
        }
    }
    return marked;
}

static void markDynamicBlockedByMapSource(Point p, [[maybe_unused]] const char* reason)
{
    // logDiskCellDetail(p, reason);
    if (!loadDiskMap() || !inDiskBounds(p)) {
        markDynamicBlocked(p, reason);
        return;
    }

    size_t index = diskIndex(gDiskMap.width, p.x, p.y);
    if (index >= gDiskMap.tileData.size() || index >= gDiskMap.objData.size() || index >= gDiskMap.hitData.size()) {
        markDynamicBlocked(p, reason);
        return;
    }

    unsigned short tileRaw = gDiskMap.tileData[index];
    unsigned short objRaw = gDiskMap.objData[index];
    unsigned short hitRaw = gDiskMap.hitData[index];
    bool hitWalkable = (hitRaw & 0x00FF) != 1;
    bool tileWalkable = rawBlockState(tileRaw) != 1;
    if (hitWalkable && tileWalkable) {
        int objMarked = markCoveringObjectBlocks(p, "stuck-obj");
        if (objMarked > 0) {
            return;
        }
        if (objRaw != 0) {
            CellGraphicInfo objInfo = readCellGraphicInfo(objRaw);
            if (objInfo.imageId > 999) {
                if (markObjectFootprintBlocked(p, objRaw, objInfo, p, "stuck-obj-self") > 0) {
                    return;
                }
            }
            markDynamicBlocked(p, "stuck-obj-cell");
            return;
        }
    }
    if (hitWalkable && objRaw == 0) {
        markDynamicBlocked(p, "stuck-tile");
        return;
    }

    markDynamicBlocked(p, reason);
}

[[maybe_unused]] static void logDiskCellDetail(Point p, [[maybe_unused]] const char* label)
{
    uint64_t now = nowMs();
    uint64_t key = makeKey(p.x, p.y);
    if (gLastCellLogKey == key && now - gLastCellLogMs < kCellLogMs) {
        return;
    }
    gLastCellLogKey = key;
    gLastCellLogMs = now;

    if (!loadDiskMap() || !inDiskBounds(p)) {
        // m_log_i("[copilot] cell %s p=%d,%d disk=unavailable loaded=%d size=%dx%d\n", label, p.x, p.y, gDiskMap.loaded ? 1 : 0, gDiskMap.width, gDiskMap.height);
        return;
    }

    size_t index = diskIndex(gDiskMap.width, p.x, p.y);
    if (index >= gDiskMap.cells.size() || index >= gDiskMap.tileData.size() || index >= gDiskMap.objData.size() || index >= gDiskMap.hitData.size()) {
        // m_log_i("[copilot] cell %s p=%d,%d disk=bad-cache index=%u cells=%u tile=%u obj=%u hit=%u\n", label, p.x, p.y, static_cast<unsigned int>(index), static_cast<unsigned int>(gDiskMap.cells.size()), static_cast<unsigned int>(gDiskMap.tileData.size()), static_cast<unsigned int>(gDiskMap.objData.size()), static_cast<unsigned int>(gDiskMap.hitData.size()));
        return;
    }
    // unsigned short tileRaw = gDiskMap.tileData[index];
    // unsigned short objRaw = gDiskMap.objData[index];
    // unsigned short hitRaw = gDiskMap.hitData[index];
    // CellGraphicInfo tileInfo = readCellGraphicInfo(tileRaw);
    // CellGraphicInfo objInfo = readCellGraphicInfo(objRaw);
    // int lx = p.x - gMapXStart;
    // int ly = p.y - gMapYStart;
    // int visible = isValidMap() && lx >= 0 && ly >= 0 && lx < gMapWidth && ly < gMapHeight ? gMoveBlockMap[lx + ly * gMapWidth] : -1;
    // unsigned char disk = gDiskMap.cells[index];
    // unsigned char known = cachedKnownOrDiskCell(p);
    // m_log_i(
    //     "[copilot] cell %s p=%d,%d tile=%04X img=%d flag=%u sx=%u sy=%u obj=%04X img=%d flag=%u sx=%u sy=%u hit=%04X low=%u disk=%u known=%u visible=%d dyn=%d\n",
    //     label, p.x, p.y, tileRaw, tileInfo.imageId, tileInfo.flag, tileInfo.sx, tileInfo.sy, objRaw, objInfo.imageId, objInfo.flag, objInfo.sx,
    //     objInfo.sy, hitRaw, hitRaw & 0x00FF, disk, known, visible, isDynamicBlocked(p) ? 1 : 0);
    // logFootprintSources(p, label);
}

static bool toLocal(Point p, int& lx, int& ly)
{
    lx = p.x - cg::world::gMapXStart;
    ly = p.y - cg::world::gMapYStart;
    return lx >= 0 && ly >= 0 && lx < gMapWidth && ly < gMapHeight;
}

static bool canWalkLocal(int lx, int ly)
{
    return isValidMap() && lx >= 0 && ly >= 0 && lx < gMapWidth && ly < gMapHeight
        && gMoveBlockMap[lx + ly * gMapWidth] != 1;
}

static bool canWalkLocalPoint(int lx, int ly)
{
    return canWalkLocal(lx, ly) && !isDynamicBlocked({cg::world::gMapXStart + lx, cg::world::gMapYStart + ly});
}

static bool canWalkGlobal(Point p)
{
    if (!isValidMap()) {
        return false;
    }
    int lx = 0;
    int ly = 0;
    return !isDynamicBlocked(p) && toLocal(p, lx, ly) && canWalkLocal(lx, ly);
}

static bool canReachLocal(Point start, Point target)
{
    if (samePoint(start, target)) {
        return true;
    }

    int startX = 0;
    int startY = 0;
    int targetX = 0;
    int targetY = 0;
    if (!toLocal(start, startX, startY) || !toLocal(target, targetX, targetY) || !canWalkGlobal(target)) {
        return false;
    }

    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(gMapWidth * gMapHeight);

    int startIndex = startX + startY * gMapWidth;
    int targetIndex = targetX + targetY * gMapWidth;
    visited[startIndex] = 1;
    queue.push_back(startIndex);

    size_t head = 0;
    while (head < queue.size()) {
        int index = queue[head++];
        if (index == targetIndex) {
            return true;
        }
        int cx = index % gMapWidth;
        int cy = index / gMapWidth;
        for (int i = 0; i < 8; ++i) {
            int nx = cx + kNeighborOffsetsX[i];
            int ny = cy + kNeighborOffsetsY[i];
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * gMapWidth;
            if (visited[nextIndex]) {
                continue;
            }
            visited[nextIndex] = 1;
            queue.push_back(nextIndex);
        }
    }
    return false;
}

static bool isForwardWaypoint(Point player, Point waypoint)
{
    if (!canWalkGlobal(waypoint)) {
        return false;
    }
    bool forward = canReachLocal(player, waypoint);
    if (!forward) {
        // m_log_i("[copilot] reject waypoint=%d,%d forward=0 backward=0\n", waypoint.x, waypoint.y);
        return false;
    }
    return true;
}

static bool isBidirectionalWaypoint(Point player, Point waypoint)
{
    if (!isForwardWaypoint(player, waypoint)) {
        return false;
    }
    bool backward = canReachLocal(waypoint, player);
    if (!backward) {
        // m_log_i("[copilot] defer waypoint=%d,%d forward=1 backward=0\n", waypoint.x, waypoint.y);
        return false;
    }
    return true;
}

[[maybe_unused]] static void logNoPathDetail(Point player, Point target)
{
    uint64_t now = nowMs();
    if (gLastNoPathDetailLogMs != 0 && now - gLastNoPathDetailLogMs < kNoPathDetailLogMs) {
        return;
    }
    gLastNoPathDetailLogMs = now;
    loadDiskMap();

    int px = player.x - cg::world::gMapXStart;
    int py = player.y - cg::world::gMapYStart;
    int tx = target.x - cg::world::gMapXStart;
    int ty = target.y - cg::world::gMapYStart;
    int playerVisible = isValidMap() && px >= 0 && py >= 0 && px < gMapWidth && py < gMapHeight ? gMoveBlockMap[px + py * gMapWidth] : -1;
    int targetVisible = isValidMap() && tx >= 0 && ty >= 0 && tx < gMapWidth && ty < gMapHeight ? gMoveBlockMap[tx + ty * gMapWidth] : -1;
    unsigned char playerKnown = cachedKnownOrDiskCell(player);
    unsigned char targetKnown = cachedKnownOrDiskCell(target);
    m_log_i("[copilot] no path detail playerWalkable=%d playerKnown=%u playerVisible=%d playerDyn=%d targetKnown=%u targetVisible=%d targetDyn=%d diskLoaded=%d diskSize=%dx%d dyn=%u known=%u\n",
        canWalkGlobal(player) ? 1 : 0, playerKnown, playerVisible, isDynamicBlocked(player) ? 1 : 0, targetKnown, targetVisible,
        isDynamicBlocked(target) ? 1 : 0, gDiskMap.loaded ? 1 : 0, gDiskMap.width, gDiskMap.height,
        static_cast<unsigned int>(gDynamicBlocked.size()), static_cast<unsigned int>(gKnownMap.size()));
    // logDiskCellDetail(player, "no-path-player");
    // logDiskCellDetail(target, "no-path-target");
}
static unsigned char knownCell(Point p)
{
    if (isDynamicBlocked(p)) {
        return kKnownBlocked;
    }
    auto it = gKnownMap.find(makeKey(p.x, p.y));
    if (it == gKnownMap.end()) {
        return diskCell(p);
    }
    return it->second;
}

static bool optimisticWalkable(Point p)
{
    return knownCell(p) != kKnownBlocked;
}

static int optimisticMoveCost(Point p, bool diagonal)
{
    int cost = diagonal ? 14 : 10;
    return knownCell(p) == kKnownWalkable ? cost : cost + 30;
}

static long long distanceScore(int lx, int ly, Point target)
{
    long long x = static_cast<long long>(cg::world::gMapXStart) + lx;
    long long y = static_cast<long long>(cg::world::gMapYStart) + ly;
    long long dx = x - target.x;
    long long dy = y - target.y;
    return dx * dx + dy * dy;
}

static bool isBetterFallback(int index, int bestIndex, int step, int bestStep, Point target)
{
    int x = index % gMapWidth;
    int y = index / gMapWidth;
    int bestX = bestIndex % gMapWidth;
    int bestY = bestIndex / gMapWidth;
    long long score = distanceScore(x, y, target);
    long long bestScore = distanceScore(bestX, bestY, target);
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

static bool buildTimedFullPath(Point start, Point target, uint64_t timeLimitMs, std::vector<Point>& path)
{
    path.clear();

    uint64_t deadline = nowMs() + timeLimitMs;
    if (!loadDiskMap() || nowMs() >= deadline || !inDiskBounds(start) || !inDiskBounds(target)) {
        return false;
    }

    auto walkable = [](Point p) {
        return inDiskBounds(p) && cachedKnownOrDiskCell(p) != kKnownBlocked;
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

static bool buildOptimisticPath(Point start, Point target, std::vector<Point>& path)
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
        bool currentIsRecent = isRecentPlayer(current);
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
            if (!inSearch(next) || !optimisticWalkable(next)) {
                continue;
            }
            bool diagonal = kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0;
            if (diagonal) {
                Point sideX{current.x + kNeighborOffsetsX[i], current.y};
                Point sideY{current.x, current.y + kNeighborOffsetsY[i]};
                if (!optimisticWalkable(sideX) || !optimisticWalkable(sideY)) {
                    continue;
                }
            }

            int nextIndex = indexOf(next);
            int nextCost = cost[index] + optimisticMoveCost(next, diagonal);
            if (nextCost >= cost[nextIndex]) {
                continue;
            }
            cost[nextIndex] = nextCost;
            parent[nextIndex] = index;
            open.push({nextIndex, nextCost + heuristic(next, planTarget)});
        }
    }

    Point bestPoint{minX + bestIndex % width, minY + bestIndex / width};
    if (isRecentPlayer(bestPoint) && !samePoint(bestPoint, target) && fallbackIndex != -1 && !fallbackIsRecent) {
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

static bool buildPath(Point start, Point target, std::vector<Point>& path)
{
    path.clear();
    if (!isValidMap()) {
        return false;
    }

    int startX = 0;
    int startY = 0;
    if (!toLocal(start, startX, startY)) {
        return false;
    }

    int targetX = 0;
    int targetY = 0;
    bool exactTarget = canWalkGlobal(target) && toLocal(target, targetX, targetY);
    int targetIndex = exactTarget ? targetX + targetY * gMapWidth : -1;
    int startIndex = startX + startY * gMapWidth;

    std::array<int, kMapBufferLimit> parent{};
    std::array<int, kMapBufferLimit> steps{};
    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(gMapWidth * gMapHeight);

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
        int cx = index % gMapWidth;
        int cy = index / gMapWidth;

        if (index == targetIndex) {
            bestIndex = index;
            break;
        }

        if (index != startIndex) {
            long long fallbackCandidateScore = distanceScore(cx, cy, target);
            bool currentIsRecent = isRecentPlayer({cg::world::gMapXStart + cx, cg::world::gMapYStart + cy});
            if (fallbackIndex == -1 || (fallbackIsRecent && !currentIsRecent)
                || (fallbackIsRecent == currentIsRecent
                    && (fallbackCandidateScore < fallbackScore || (fallbackCandidateScore == fallbackScore && steps[index] < fallbackStep)))) {
                fallbackIndex = index;
                fallbackScore = fallbackCandidateScore;
                fallbackStep = steps[index];
                fallbackIsRecent = currentIsRecent;
            }
        }

        if (isBetterFallback(index, bestIndex, steps[index], bestStep, target)) {
            bestIndex = index;
            bestStep = steps[index];
        }

        for (int i = 0; i < 8; ++i) {
            int nx = cx + kNeighborOffsetsX[i];
            int ny = cy + kNeighborOffsetsY[i];
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * gMapWidth;
            if (visited[nextIndex]) {
                continue;
            }
            visited[nextIndex] = 1;
            parent[nextIndex] = index;
            steps[nextIndex] = steps[index] + 1;
            queue.push_back(nextIndex);
        }
    }

    Point bestPoint{cg::world::gMapXStart + bestIndex % gMapWidth, cg::world::gMapYStart + bestIndex / gMapWidth};
    if (isRecentPlayer(bestPoint) && !samePoint(bestPoint, target) && fallbackIndex != -1 && !fallbackIsRecent) {
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
        reversePath.push_back({cg::world::gMapXStart + index % gMapWidth, cg::world::gMapYStart + index / gMapWidth});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

static bool buildNearRetargetPath(Point start, Point target, std::vector<Point>& path)
{
    path.clear();
    if (!isValidMap()) {
        return false;
    }

    int startX = 0;
    int startY = 0;
    if (!toLocal(start, startX, startY)) {
        return false;
    }

    int startIndex = startX + startY * gMapWidth;
    std::array<int, kMapBufferLimit> parent{};
    std::array<int, kMapBufferLimit> steps{};
    std::array<unsigned char, kMapBufferLimit> visited{};
    std::vector<int> queue;
    queue.reserve(gMapWidth * gMapHeight);

    parent[startIndex] = -1;
    visited[startIndex] = 1;
    queue.push_back(startIndex);

    int bestIndex = -1;
    long long bestScore = std::numeric_limits<long long>::max();
    int bestStep = 0;
    size_t head = 0;


    while (head < queue.size()) {
        int index = queue[head++];
        int cx = index % gMapWidth;
        int cy = index / gMapWidth;
        Point current{cg::world::gMapXStart + cx, cg::world::gMapYStart + cy};

        if (index != startIndex && (!isRecentPlayer(current) || samePoint(current, target))) {
            long long score = distanceScore(cx, cy, target);
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
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (kNeighborOffsetsX[i] != 0 && kNeighborOffsetsY[i] != 0 && (!canWalkLocalPoint(cx + kNeighborOffsetsX[i], cy) || !canWalkLocalPoint(cx, cy + kNeighborOffsetsY[i]))) {
                continue;
            }

            int nextIndex = nx + ny * gMapWidth;
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
        reversePath.push_back({cg::world::gMapXStart + index % gMapWidth, cg::world::gMapYStart + index / gMapWidth});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

static void requestMove(Point target)
{
    Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    gAutoTargetX = target.x;
    gAutoTargetY = target.y;
    gExpectedAutoTarget = target;
    gHasExpectedAutoTarget = true;
    // m_log_i("[copilot] requestMove waypoint=%d,%d player=%d,%d\n", target.x, target.y, player.x, player.y);
    // logDiskCellDetail(target, "request");
    gLastRequestedWaypoint = target;
    gHasLastRequestedWaypoint = true;
    gLastRequestPlayer = player;
    gLastRequestDistance = pointDistance(player, target);
    gHasPendingMoveCheck = true;
    gMoveCheckSawBusy = false;
    gMoveRequest = 1;
}

static void cacheWaypoint(Point player, Point target, Point waypoint)
{
    gLastPlanPlayer = player;
    gCachedTarget = target;
    gCachedWaypoint = waypoint;
    gCachedMapId = gMapId;
    gCachedMapFloor = gMapFloor;
    gCachedMapGroup = gMapGroup;
    gHasCachedWaypoint = true;
}

static bool tryUseCachedWaypoint(Point player, Point target)
{
    if (!gHasCachedWaypoint || !samePoint(target, gCachedTarget) || static_cast<std::uint32_t>(gCachedMapId) != gMapId || gCachedMapFloor != gMapFloor
        || gCachedMapGroup != gMapGroup) {
        return false;
    }
    if (samePoint(player, gCachedWaypoint) || pointDistance(player, gCachedWaypoint) > kMoveSegmentSteps
        || pointDistance(player, gLastPlanPlayer) >= kReplanMoveDistance) {
        resetPlanCache();
        return false;
    }
    if (!isForwardWaypoint(player, gCachedWaypoint)) {
        // m_log_i("[copilot] drop cached waypoint=%d,%d player=%d,%d\n", gCachedWaypoint.x, gCachedWaypoint.y, player.x, player.y);
        resetPlanCache();
        return false;
    }

    // m_log_i("[copilot] reuse waypoint=%d,%d player=%d,%d moved=%d\n", gCachedWaypoint.x, gCachedWaypoint.y, player.x, player.y,
    //     pointDistance(player, gLastPlanPlayer));
    requestMove(gCachedWaypoint);
    return true;
}

static bool pickVisibleWaypoint(const std::vector<Point>& path, Point player, Point& waypoint, bool allowRecentFallback = false)
{
    if (path.empty()) {
        return false;
    }
    auto avoidRecent = [](Point p) {
        return isRecentPlayer(p) && !samePoint(p, gCopilotTarget);
    };
    int lastIndex = std::min<int>(kMoveSegmentSteps - 1, static_cast<int>(path.size()) - 1);
    int recentIndex = -1;
    int oneWayIndex = -1;
    int oneWayRecentIndex = -1;
    for (int i = lastIndex; i >= 0; --i) {
        Point next = path[i];
        if (pointDistance(player, next) > kMoveSegmentSteps) {
            // m_log_i("[copilot] reject waypoint=%d,%d player=%d,%d reason=too-far\n", next.x, next.y, player.x, player.y);
            continue;
        }
        bool recent = avoidRecent(next);
        if (isBidirectionalWaypoint(player, next)) {
            if (recent) {
                if (recentIndex == -1) {
                    recentIndex = i;
                }
                continue;
            }
            waypoint = next;
            return true;
        }
        if (!isForwardWaypoint(player, next)) {
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
        // m_log_i("[copilot] use one-way waypoint=%d,%d index=%d\n", waypoint.x, waypoint.y, oneWayIndex);
        return true;
    }
    if (recentIndex != -1) {
        if (!allowRecentFallback) {
            // m_log_i("[copilot] skip recent waypoint=%d,%d recent=%d\n", path[recentIndex].x, path[recentIndex].y, gRecentPlayerCount);
            return false;
        }
        waypoint = path[recentIndex];
        // m_log_i("[copilot] use recent waypoint=%d,%d index=%d recent=%d\n", waypoint.x, waypoint.y, recentIndex, gRecentPlayerCount);
        return true;
    }
    if (oneWayRecentIndex != -1) {
        if (!allowRecentFallback) {
            // m_log_i("[copilot] skip recent waypoint=%d,%d recent=%d\n", path[oneWayRecentIndex].x, path[oneWayRecentIndex].y, gRecentPlayerCount);
            return false;
        }
        waypoint = path[oneWayRecentIndex];
        // m_log_i("[copilot] use one-way recent waypoint=%d,%d index=%d recent=%d\n", waypoint.x, waypoint.y, oneWayRecentIndex,
            // gRecentPlayerCount);
        return true;
    }
    return false;
}

} // namespace copilot_detail


// 目标改变时可只刷新显示路径；已有移动段仍沿原忙碌门禁完成，不提前发出下一段。
static void PlanCopilot(int x, int y, bool routeOnly)
{
    if (copilot_detail::skipSceneNotReady("autoCopilot")) {
        return;
    }
    copilot_detail::syncVisibleCache();

    copilot_detail::Point target{x, y};
    copilot_detail::Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    // m_log_i("[copilot] plan player=%d,%d target=%d,%d map=%d group=%d floor=%d scene=%d state=%d\n", player.x, player.y, target.x, target.y, gMapId, gMapGroup, gMapFloor, world::gCurrentSceneType, world::gCurrentSceneState);
    if (player.x == target.x && player.y == target.y) {
        // m_log_i("[copilot] already arrived target=%d,%d\n", target.x, target.y);
        copilot_detail::clearCopilotRoute();
        return;
    }
    if (!copilot_detail::isValidMap()) {
        // m_log_i("[copilot] skip invalid visible map width=%d height=%d cells=%d limit=%d\n", gMapWidth, gMapHeight, gMapWidth * gMapHeight, kMapBufferLimit);
        copilot_detail::clearCopilotRoute();
        return;
    }
    if (!copilot_detail::isSaneTarget(player, target)) {
        // m_log_i("[copilot] skip insane target player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
        copilot_detail::clearCopilotRoute();
        return;
    }
    if (!routeOnly && copilot_detail::isBusy()) {
        // m_log_i("[copilot] skip busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", gPathCount, gMoveQueueCount, gMoveLock, gTileEventLock, gTargetEventLock);
        return;
    }
    if (copilot_detail::loadDiskMap() && (!copilot_detail::inDiskBounds(player) || !copilot_detail::inDiskBounds(target))) {
        copilot_detail::clearCopilotRoute();
        return;
    }
    if (!routeOnly && copilot_detail::tryUseCachedWaypoint(player, target)) {
        return;
    }

    std::vector<copilot_detail::Point> path;
    copilot_detail::Point waypoint{};

    bool fullPath = copilot_detail::buildTimedFullPath(player, target, copilot_detail::kFullSearchTimeLimitMs, path);
    // m_log_i("[copilot] fullPath ok=%d steps=%u\n", fullPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (fullPath && copilot_detail::pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use fullPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(player, target, waypoint);
        copilot_detail::requestMove(waypoint);
        return;
    }

    bool optimisticPath = copilot_detail::buildOptimisticPath(player, target, path);
    // m_log_i("[copilot] optimisticPath ok=%d steps=%u\n", optimisticPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (optimisticPath && copilot_detail::pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use optimisticPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(player, target, waypoint);
        copilot_detail::requestMove(waypoint);
        return;
    }

    bool localPath = copilot_detail::buildPath(player, target, path);
    // m_log_i("[copilot] localPath ok=%d steps=%u\n", localPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (localPath && copilot_detail::pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use localPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(player, target, waypoint);
        copilot_detail::requestMove(waypoint);
        return;
    }

    bool nearPath = copilot_detail::buildNearRetargetPath(player, target, path);
    // m_log_i("[copilot] nearPath ok=%d steps=%u\n", nearPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (nearPath && copilot_detail::pickVisibleWaypoint(path, player, waypoint, true)) {
        // m_log_i("[copilot] use nearPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        copilot_detail::cacheCopilotRoute(path);
        if (routeOnly) return;
        copilot_detail::cacheWaypoint(player, target, waypoint);
        copilot_detail::requestMove(waypoint);
        return;
    }

    // m_log_i("[copilot] no path player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
    // logNoPathDetail(player, target);
    copilot_detail::clearCopilotRoute();
}

void autoCopilot(int x, int y)
{
    PlanCopilot(x, y, false);
}

void startCopilot(int x, int y)
{
    copilot_detail::gCopilotTarget = {x, y};
    copilot_detail::gCopilotMapId = copilot_detail::gMapId;
    copilot_detail::gCopilotMapFloor = copilot_detail::gMapFloor;
    copilot_detail::gCopilotEnabled = true;
    copilot_detail::gLastCopilotTickMs = 0;
    copilot_detail::gLastBusyLogMs = 0;
    copilot_detail::gHasLastObservedPlayer = false;
    copilot_detail::clearRecentPlayers();
    copilot_detail::resetPlanCache();
    copilot_detail::resetMoveProgress();
    copilot_detail::gDynamicBlocked.clear();
    copilot_detail::gLastNoPathDetailLogMs = 0;
    copilot_detail::gLastSceneSkipLogMs = 0;
    copilot_detail::gLastCellLogKey = 0;
    copilot_detail::gLastCellLogMs = 0;
    copilot_detail::gHasLastRequestedWaypoint = false;
    copilot_detail::clearCopilotRoute();
    m_log_i("[copilot] start target=%d,%d player=%d,%d scene=%d state=%d\n", x, y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY), world::gCurrentSceneType, world::gCurrentSceneState);
    PlanCopilot(x, y, true);
}

void stopCopilot()
{
    if (copilot_detail::gCopilotEnabled) {
        m_log_i("[copilot] stop target=%d,%d player=%d,%d\n", copilot_detail::gCopilotTarget.x, copilot_detail::gCopilotTarget.y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY));
    }
    copilot_detail::gCopilotEnabled = false;
    copilot_detail::resetPlanCache();
    copilot_detail::resetMoveProgress();
    copilot_detail::clearCopilotRoute();
}

int getAutoCopliotState()
{
    return copilot_detail::gCopilotEnabled ? 1 : 0;
}

std::vector<std::pair<int, int> > getAutoCopliotRoute()
{
    std::vector<std::pair<int, int> > route;
    if (!copilot_detail::gCopilotEnabled || !copilot_detail::isCopilotSceneReady() || copilot_detail::gMapId != static_cast<std::uint32_t>(copilot_detail::gCopilotMapId) || copilot_detail::gMapFloor != copilot_detail::gCopilotMapFloor) {
        return route;
    }

    route.reserve(copilot_detail::gAutoCopilotRoute.size());
    for (const auto& point: copilot_detail::gAutoCopilotRoute) {
        route.emplace_back(point.x, point.y);
    }
    return route;
}

void CopilotProc()
{
    if (!copilot_detail::gCopilotEnabled) {
        return;
    }
    if (copilot_detail::gMapId != static_cast<std::uint32_t>(copilot_detail::gCopilotMapId) || copilot_detail::gMapFloor != copilot_detail::gCopilotMapFloor) {
        m_log_i("[copilot] stop map changed expected=%d,%d actual=%d,%d target=%d,%d player=%d,%d\n", copilot_detail::gCopilotMapId,
            copilot_detail::gCopilotMapFloor, copilot_detail::gMapId, copilot_detail::gMapFloor, copilot_detail::gCopilotTarget.x, copilot_detail::gCopilotTarget.y, static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY));
        stopCopilot();
        return;
    }
    if (copilot_detail::skipSceneNotReady("proc")) {
        return;
    }

    copilot_detail::Point player{static_cast<std::int32_t>(cg::world::gPlayerX), static_cast<std::int32_t>(cg::world::gPlayerY)};
    if (player.x == copilot_detail::gCopilotTarget.x && player.y == copilot_detail::gCopilotTarget.y) {
        m_log_i("[copilot] arrived target=%d,%d\n", copilot_detail::gCopilotTarget.x, copilot_detail::gCopilotTarget.y);
        stopCopilot();
        return;
    }
    if (copilot_detail::gHasExpectedAutoTarget && copilot_detail::samePoint(player, copilot_detail::gExpectedAutoTarget)) {
        copilot_detail::resetMoveProgress();
    }
    if (copilot_detail::gHasExpectedAutoTarget) {
        copilot_detail::Point clientTarget{static_cast<std::int32_t>(copilot_detail::gAutoTargetX), static_cast<std::int32_t>(copilot_detail::gAutoTargetY)};
        if (!copilot_detail::samePoint(clientTarget, copilot_detail::gExpectedAutoTarget)) {
            m_log_i("[copilot] stop external target expected=%d,%d actual=%d,%d player=%d,%d\n", copilot_detail::gExpectedAutoTarget.x,
                copilot_detail::gExpectedAutoTarget.y, clientTarget.x, clientTarget.y, player.x, player.y);
            stopCopilot();
            return;
        }
    }
    uint64_t now = copilot_detail::nowMs();
    if (copilot_detail::isBusy()) {
        if (copilot_detail::gHasPendingMoveCheck) {
            copilot_detail::gMoveCheckSawBusy = true;
        }
        if (copilot_detail::gLastBusyLogMs == 0 || now - copilot_detail::gLastBusyLogMs >= 1000) {
            copilot_detail::gLastBusyLogMs = now;
            m_log_i("[copilot] wait busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", copilot_detail::gPathCount, copilot_detail::gMoveQueueCount, copilot_detail::gMoveLock, copilot_detail::gTileEventLock, copilot_detail::gTargetEventLock);
        }
        return;
    }


    if (copilot_detail::gLastCopilotTickMs != 0 && now - copilot_detail::gLastCopilotTickMs < copilot_detail::kCopilotTickMs) {
        return;
    }

    if (!copilot_detail::gHasLastObservedPlayer) {
        copilot_detail::gLastObservedPlayer = player;
        copilot_detail::gHasLastObservedPlayer = true;
    } else if (player.x != copilot_detail::gLastObservedPlayer.x || player.y != copilot_detail::gLastObservedPlayer.y) {
        copilot_detail::rememberRecentPlayer(copilot_detail::gLastObservedPlayer);
        copilot_detail::gLastObservedPlayer = player;
    }

    int requestDist = copilot_detail::gHasPendingMoveCheck ? copilot_detail::pointDistance(player, copilot_detail::gLastRequestedWaypoint) : 0;
    int requestProgress = copilot_detail::gHasPendingMoveCheck ? copilot_detail::gLastRequestDistance - requestDist : 0;
    int movedFromRequest = copilot_detail::gHasPendingMoveCheck ? copilot_detail::pointDistance(player, copilot_detail::gLastRequestPlayer) : 0;
    if (copilot_detail::gHasPendingMoveCheck) {
        if (requestDist == 0) {
            copilot_detail::resetMoveProgress();
        } else if (copilot_detail::gMoveCheckSawBusy || movedFromRequest > 0) {
            if (requestProgress <= 0 && movedFromRequest == 0) {
                copilot_detail::markDynamicBlockedByMapSource(copilot_detail::gLastRequestedWaypoint, "stuck");
                copilot_detail::gHasLastRequestedWaypoint = false;
                copilot_detail::resetPlanCache();
                copilot_detail::resetMoveProgress();
            } else if (movedFromRequest > 0 || requestProgress > 0) {
                copilot_detail::gLastRequestPlayer = player;
                copilot_detail::gLastRequestDistance = requestDist;
                copilot_detail::gMoveCheckSawBusy = false;
            }
        }
    }

    copilot_detail::gLastCopilotTickMs = now;
    // m_log_i("[copilot] proc player=%d,%d target=%d,%d reqDist=%d reqProgress=%d scene=%d state=%d\n", player.x, player.y,
    //     gCopilotTarget.x, gCopilotTarget.y, requestDist, requestProgress, world::gCurrentSceneType, world::gCurrentSceneState);
    autoCopilot(copilot_detail::gCopilotTarget.x, copilot_detail::gCopilotTarget.y);
}

} // namespace cg::script
