#include "LuaUI.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include <algorithm>
#include <array>
#include <chrono>
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

static auto& gPlayerX = world::detail::gPlayerX;
static auto& gPlayerY = world::detail::gPlayerY;
static auto& gAutoTargetX = world::detail::gMoveTargetX9A7F88;
static auto& gAutoTargetY = world::detail::gMoveTargetY9A7F98;
static auto& gMoveRequest = world::detail::gValueC821FC;
static auto& gPathCount = world::detail::gValueC82200;
static auto& gMoveQueueCount = world::detail::gValueC82202;
static auto& gMoveLock = world::detail::gValueC82220;
static auto& gTileEventLock = world::detail::gValue9A86D8;
static auto& gTargetEventLock = world::detail::gValue9FB92E;
static auto& gMapXStart = world::detail::gMapXStart;
static auto& gMapYStart = world::detail::gMapYStart;
static auto& gMapWidth = world::detail::gMapSpanX;
static auto& gMapHeight = world::detail::gMapSpanY;
static auto& gMoveBlockMap = world::detail::gMapWalkabilityArray;
static auto& gMapId = world::detail::gValue9A86DC;
static auto& gMapGroup = world::detail::gValue9E0434;
static auto& gMapFloor = world::gMapFloor;

std::unordered_map<uint64_t, unsigned char> gKnownMap;
int gLastMapId = -1;
int gLastMapFloor = -1;
int gLastMapGroup = -1;

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

DiskMapCache gDiskMap;
bool gCopilotEnabled = false;
Point gCopilotTarget{};
int gCopilotMapId = -1;
int gCopilotMapFloor = -1;
uint64_t gLastCopilotTickMs = 0;
uint64_t gLastBusyLogMs = 0;
uint64_t gLastSceneSkipLogMs = 0;
uint64_t gLastCellLogKey = 0;
uint64_t gLastCellLogMs = 0;
uint64_t gLastNoPathDetailLogMs = 0;
Point gLastObservedPlayer{};
bool gHasLastObservedPlayer = false;
std::array<Point, kRecentAvoidCount> gRecentPlayers{};
int gRecentPlayerCount = 0;
std::unordered_map<uint64_t, uint64_t> gDynamicBlocked;
Point gLastRequestedWaypoint{};
bool gHasLastRequestedWaypoint = false;
Point gExpectedAutoTarget{};
bool gHasExpectedAutoTarget = false;
Point gLastRequestPlayer{};
int gLastRequestDistance = 0;
bool gHasPendingMoveCheck = false;
bool gMoveCheckSawBusy = false;
Point gLastPlanPlayer{};
Point gCachedWaypoint{};
Point gCachedTarget{};
int gCachedMapId = -1;
int gCachedMapFloor = -1;
int gCachedMapGroup = -1;
bool gHasCachedWaypoint = false;
std::vector<Point> gAutoCopilotRoute;

uint64_t nowMs()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

void clearRecentPlayers();
void resetPlanCache();
void resetMoveProgress();
void clearCopilotRoute();

bool isCopilotSceneReady()
{
    return world::gCurrentSceneType == SCENE_NORMAL && world::gCurrentSceneState == 3;
}

bool skipSceneNotReady(const char* where)
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

size_t diskIndex(int width, int x, int y)
{
    return static_cast<size_t>(x) + static_cast<size_t>(y) * width;
}

void setDiskState(std::vector<unsigned char>& state, int width, int height, int x, int y, unsigned char value)
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

void applySingleCellRule(std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::detail::Sub_46CF00(raw);
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

void applyFootprintRule(std::vector<unsigned char>& state, int width, int height, int x, int y, unsigned short flag, std::int32_t seqNo)
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

void applyObjectRule(std::vector<unsigned char>& state, int width, int height, unsigned short raw, int x, int y)
{
    int imageId = world::detail::Sub_46CF00(raw);
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

std::vector<unsigned char> synthesizeDiskCells(
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

bool isBusy()
{
    return gPathCount > 0 || gMoveQueueCount > 0 || gMoveLock != 0 || gTileEventLock != 0 || gTargetEventLock != 0;
}

bool isValidMap()
{
    return gMapWidth > 0 && gMapHeight > 0 && gMapWidth * gMapHeight <= kMapBufferLimit;
}

bool isSaneTarget(Point start, Point target)
{
    return std::llabs(static_cast<long long>(target.x) - start.x) <= kMaxCoordinateDistance
        && std::llabs(static_cast<long long>(target.y) - start.y) <= kMaxCoordinateDistance;
}

uint64_t makeKey(int x, int y)
{
    return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(y);
}

Point keyToPoint(uint64_t key)
{
    return {
        static_cast<int>(static_cast<int32_t>(static_cast<uint32_t>(key >> 32))),
        static_cast<int>(static_cast<int32_t>(static_cast<uint32_t>(key))),
    };
}

bool samePoint(Point a, Point b)
{
    return a.x == b.x && a.y == b.y;
}

int pointDistance(Point a, Point b)
{
    return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

void resetPlanCache()
{
    gHasCachedWaypoint = false;
}

void resetMoveProgress()
{
    gHasPendingMoveCheck = false;
    gMoveCheckSawBusy = false;
    gHasExpectedAutoTarget = false;
}

void clearCopilotRoute()
{
    gAutoCopilotRoute.clear();
}

void cacheCopilotRoute(const std::vector<Point>& path)
{
    gAutoCopilotRoute = path;
}

void clearRecentPlayers()
{
    gRecentPlayerCount = 0;
}

void rememberRecentPlayer(Point p)
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

bool isRecentPlayer(Point p)
{
    for (int i = 0; i < gRecentPlayerCount; ++i) {
        if (samePoint(gRecentPlayers[i], p)) {
            return true;
        }
    }
    return false;
}

bool isDynamicBlocked(Point p)
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
bool putDynamicBlocked(Point p)
{
    bool existed = isDynamicBlocked(p);
    gDynamicBlocked[makeKey(p.x, p.y)] = nowMs() + kDynamicBlockMs;
    return !existed;
}

void markDynamicBlocked(Point p, const char* reason)
{
    bool added = putDynamicBlocked(p);
    if (added) {
        // m_log_i("[copilot] dynamic block %d,%d reason=%s ttl=%u\n", p.x, p.y, reason, static_cast<unsigned int>(kDynamicBlockMs));
    }
}


void syncVisibleCache()
{
    if (!isValidMap()) {
        return;
    }
    if (gLastMapId != gMapId || gLastMapFloor != gMapFloor || gLastMapGroup != gMapGroup) {
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
            int globalX = gMapXStart + x;
            int globalY = gMapYStart + y;
            gKnownMap[makeKey(globalX, globalY)] = gMoveBlockMap[x + y * gMapWidth] == 1 ? kKnownBlocked : kKnownWalkable;
        }
    }
}

std::string makeMapFileName()
{
    char fileName[260] = {};
    if (gMapId) {
        std::snprintf(fileName, sizeof(fileName), "map\\%d\\%d\\%d.dat", gMapId, gMapGroup, gMapFloor);
    } else {
        std::snprintf(fileName, sizeof(fileName), "map\\0\\%d.dat", gMapFloor);
    }
    return fileName;
}

bool loadDiskMap()
{
    uint64_t now = nowMs();
    bool sameMap = gDiskMap.mapId == gMapId && gDiskMap.floor == gMapFloor && gDiskMap.group == gMapGroup;
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

    auto fail = [&](const char* reason) {
        // m_log_i("[copilot] loadDiskMap failed reason=%s map=%d group=%d floor=%d keepOld=%d\n", reason, gMapId, gMapGroup, gMapFloor, sameMap && gDiskMap.loaded ? 1 : 0);
        if (sameMap && gDiskMap.loaded) {
            gDiskMap.lastAttemptMs = now;
            return true;
        }
        gDiskMap = next;
        return false;
    };

    FILE* file = std::fopen(makeMapFileName().c_str(), "rb");
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
    size_t blockedCount = std::count(next.cells.begin(), next.cells.end(), static_cast<unsigned char>(kKnownBlocked));
    // m_log_i("[copilot] loadDiskMap ok map=%d group=%d floor=%d size=%dx%d blocked=%u\n", gMapId, gMapGroup, gMapFloor, next.width, next.height, static_cast<unsigned int>(blockedCount));
    gDiskMap = next;
    return true;
}

unsigned char diskCell(Point p)
{
    if (!loadDiskMap()) {
        return kKnownWalkable;
    }
    if (p.x < 0 || p.y < 0 || p.x >= gDiskMap.width || p.y >= gDiskMap.height) {
        return kKnownWalkable;
    }
    return gDiskMap.cells[p.x + p.y * gDiskMap.width];
}

bool inDiskBounds(Point p)
{
    return gDiskMap.loaded && p.x >= 0 && p.y >= 0 && p.x < gDiskMap.width && p.y < gDiskMap.height;
}

unsigned char cachedKnownOrDiskCell(Point p)
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

CellGraphicInfo readCellGraphicInfo(unsigned short raw)
{
    CellGraphicInfo info;
    info.imageId = world::detail::Sub_46CF00(raw);
    if (info.imageId > 999 || (info.imageId >= 60 && info.imageId <= 79)) {
        resource::GetGraphicMapNoIndex(info.imageId, &info.seqNo);
        resource::GetGraphicFlag1(info.seqNo, &info.flag);
        if (info.imageId > 999) {
            resource::GetGraphicSxSy(info.seqNo, &info.sx, &info.sy);
        }
    }
    return info;
}

unsigned char rawBlockState(unsigned short raw)
{
    int imageId = world::detail::Sub_46CF00(raw);
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

bool footprintCovers(Point origin, CellGraphicInfo info, Point p)
{
    short sx = static_cast<short>(info.sx);
    short sy = static_cast<short>(info.sy);
    return sx > 0 && sy > 0 && p.x >= origin.x && p.x < origin.x + sx && p.y <= origin.y && p.y > origin.y - sy;
}

void logFootprintSources(Point p, const char* label)
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

void logDiskCellDetail(Point p, const char* label);
int markObjectFootprintBlocked(Point origin, unsigned short raw, CellGraphicInfo info, Point cover, const char* reason)
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

int markCoveringObjectBlocks(Point p, const char* reason)
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

void markDynamicBlockedByMapSource(Point p, const char* reason)
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

void logDiskCellDetail(Point p, const char* label)
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

bool toLocal(Point p, int& lx, int& ly)
{
    lx = p.x - gMapXStart;
    ly = p.y - gMapYStart;
    return lx >= 0 && ly >= 0 && lx < gMapWidth && ly < gMapHeight;
}

bool canWalkLocal(int lx, int ly)
{
    return isValidMap() && lx >= 0 && ly >= 0 && lx < gMapWidth && ly < gMapHeight
        && gMoveBlockMap[lx + ly * gMapWidth] != 1;
}

bool canWalkLocalPoint(int lx, int ly)
{
    return canWalkLocal(lx, ly) && !isDynamicBlocked({gMapXStart + lx, gMapYStart + ly});
}

bool canWalkGlobal(Point p)
{
    if (!isValidMap()) {
        return false;
    }
    int lx = 0;
    int ly = 0;
    return !isDynamicBlocked(p) && toLocal(p, lx, ly) && canWalkLocal(lx, ly);
}

bool canReachLocal(Point start, Point target)
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

    static constexpr int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static constexpr int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    size_t head = 0;
    while (head < queue.size()) {
        int index = queue[head++];
        if (index == targetIndex) {
            return true;
        }
        int cx = index % gMapWidth;
        int cy = index / gMapWidth;
        for (int i = 0; i < 8; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (dx[i] != 0 && dy[i] != 0 && (!canWalkLocalPoint(cx + dx[i], cy) || !canWalkLocalPoint(cx, cy + dy[i]))) {
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

bool isForwardWaypoint(Point player, Point waypoint)
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

bool isBidirectionalWaypoint(Point player, Point waypoint)
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

void logNoPathDetail(Point player, Point target)
{
    uint64_t now = nowMs();
    if (gLastNoPathDetailLogMs != 0 && now - gLastNoPathDetailLogMs < kNoPathDetailLogMs) {
        return;
    }
    gLastNoPathDetailLogMs = now;
    loadDiskMap();

    int px = player.x - gMapXStart;
    int py = player.y - gMapYStart;
    int tx = target.x - gMapXStart;
    int ty = target.y - gMapYStart;
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
unsigned char knownCell(Point p)
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

bool optimisticWalkable(Point p)
{
    return knownCell(p) != kKnownBlocked;
}

int optimisticMoveCost(Point p, bool diagonal)
{
    int cost = diagonal ? 14 : 10;
    return knownCell(p) == kKnownWalkable ? cost : cost + 30;
}

long long distanceScore(int lx, int ly, Point target)
{
    long long x = static_cast<long long>(gMapXStart) + lx;
    long long y = static_cast<long long>(gMapYStart) + ly;
    long long dx = x - target.x;
    long long dy = y - target.y;
    return dx * dx + dy * dy;
}

bool isBetterFallback(int index, int bestIndex, int step, int bestStep, Point target)
{
    int x = index % gMapWidth;
    int y = index / gMapWidth;
    int bestX = bestIndex % gMapWidth;
    int bestY = bestIndex / gMapWidth;
    long long score = distanceScore(x, y, target);
    long long bestScore = distanceScore(bestX, bestY, target);
    return score < bestScore || (score == bestScore && step < bestStep);
}

Point clampTarget(Point start, Point target)
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

int heuristic(Point a, Point b)
{
    long long dx = std::llabs(static_cast<long long>(a.x) - b.x);
    long long dy = std::llabs(static_cast<long long>(a.y) - b.y);
    long long diagonal = std::min(dx, dy);
    long long straight = std::max(dx, dy) - diagonal;
    long long value = diagonal * 14 + straight * 10;
    return value > std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : static_cast<int>(value);
}

bool buildTimedFullPath(Point start, Point target, uint64_t timeLimitMs, std::vector<Point>& path)
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
    static constexpr int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static constexpr int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

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
            Point next{current.x + dx[i], current.y + dy[i]};
            if (!walkable(next)) {
                continue;
            }
            bool diagonal = dx[i] != 0 && dy[i] != 0;
            if (diagonal && (!walkable({current.x + dx[i], current.y}) || !walkable({current.x, current.y + dy[i]}))) {
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

bool buildOptimisticPath(Point start, Point target, std::vector<Point>& path)
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
    int height = static_cast<int>(height64);
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
    static constexpr int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static constexpr int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

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
            Point next{current.x + dx[i], current.y + dy[i]};
            if (!inSearch(next) || !optimisticWalkable(next)) {
                continue;
            }
            bool diagonal = dx[i] != 0 && dy[i] != 0;
            if (diagonal) {
                Point sideX{current.x + dx[i], current.y};
                Point sideY{current.x, current.y + dy[i]};
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

bool buildPath(Point start, Point target, std::vector<Point>& path)
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

    static constexpr int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static constexpr int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

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
            bool currentIsRecent = isRecentPlayer({gMapXStart + cx, gMapYStart + cy});
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
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (dx[i] != 0 && dy[i] != 0 && (!canWalkLocalPoint(cx + dx[i], cy) || !canWalkLocalPoint(cx, cy + dy[i]))) {
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

    Point bestPoint{gMapXStart + bestIndex % gMapWidth, gMapYStart + bestIndex / gMapWidth};
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
        reversePath.push_back({gMapXStart + index % gMapWidth, gMapYStart + index / gMapWidth});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

bool buildNearRetargetPath(Point start, Point target, std::vector<Point>& path)
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

    static constexpr int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static constexpr int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    while (head < queue.size()) {
        int index = queue[head++];
        int cx = index % gMapWidth;
        int cy = index / gMapWidth;
        Point current{gMapXStart + cx, gMapYStart + cy};

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
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (!canWalkLocalPoint(nx, ny)) {
                continue;
            }
            if (dx[i] != 0 && dy[i] != 0 && (!canWalkLocalPoint(cx + dx[i], cy) || !canWalkLocalPoint(cx, cy + dy[i]))) {
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
        reversePath.push_back({gMapXStart + index % gMapWidth, gMapYStart + index / gMapWidth});
    }

    path.reserve(reversePath.size() - 1);
    for (auto it = reversePath.rbegin() + 1; it != reversePath.rend(); ++it) {
        path.push_back(*it);
    }
    return !path.empty();
}

void requestMove(Point target)
{
    Point player{gPlayerX.value(), gPlayerY.value()};
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

void cacheWaypoint(Point player, Point target, Point waypoint)
{
    gLastPlanPlayer = player;
    gCachedTarget = target;
    gCachedWaypoint = waypoint;
    gCachedMapId = gMapId;
    gCachedMapFloor = gMapFloor;
    gCachedMapGroup = gMapGroup;
    gHasCachedWaypoint = true;
}

bool tryUseCachedWaypoint(Point player, Point target)
{
    if (!gHasCachedWaypoint || !samePoint(target, gCachedTarget) || gCachedMapId != gMapId || gCachedMapFloor != gMapFloor
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

bool pickVisibleWaypoint(const std::vector<Point>& path, Point player, Point& waypoint, bool allowRecentFallback = false)
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

using namespace copilot_detail;

void autoCopilot(int x, int y)
{
    if (skipSceneNotReady("autoCopilot")) {
        return;
    }
    syncVisibleCache();

    Point target{x, y};
    Point player{gPlayerX.value(), gPlayerY.value()};
    // m_log_i("[copilot] plan player=%d,%d target=%d,%d map=%d group=%d floor=%d scene=%d state=%d\n", player.x, player.y, target.x, target.y, gMapId, gMapGroup, gMapFloor, world::gCurrentSceneType, world::gCurrentSceneState);
    if (player.x == target.x && player.y == target.y) {
        // m_log_i("[copilot] already arrived target=%d,%d\n", target.x, target.y);
        clearCopilotRoute();
        return;
    }
    if (!isValidMap()) {
        // m_log_i("[copilot] skip invalid visible map width=%d height=%d cells=%d limit=%d\n", gMapWidth, gMapHeight, gMapWidth * gMapHeight, kMapBufferLimit);
        clearCopilotRoute();
        return;
    }
    if (!isSaneTarget(player, target)) {
        // m_log_i("[copilot] skip insane target player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
        clearCopilotRoute();
        return;
    }
    if (isBusy()) {
        // m_log_i("[copilot] skip busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", gPathCount, gMoveQueueCount, gMoveLock, gTileEventLock, gTargetEventLock);
        return;
    }
    if (loadDiskMap() && (!inDiskBounds(player) || !inDiskBounds(target))) {
        clearCopilotRoute();
        return;
    }
    if (tryUseCachedWaypoint(player, target)) {
        return;
    }

    std::vector<Point> path;
    Point waypoint{};

    bool fullPath = buildTimedFullPath(player, target, kFullSearchTimeLimitMs, path);
    // m_log_i("[copilot] fullPath ok=%d steps=%u\n", fullPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (fullPath && pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use fullPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        cacheCopilotRoute(path);
        cacheWaypoint(player, target, waypoint);
        requestMove(waypoint);
        return;
    }

    bool optimisticPath = buildOptimisticPath(player, target, path);
    // m_log_i("[copilot] optimisticPath ok=%d steps=%u\n", optimisticPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (optimisticPath && pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use optimisticPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        cacheCopilotRoute(path);
        cacheWaypoint(player, target, waypoint);
        requestMove(waypoint);
        return;
    }

    bool localPath = buildPath(player, target, path);
    // m_log_i("[copilot] localPath ok=%d steps=%u\n", localPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (localPath && pickVisibleWaypoint(path, player, waypoint)) {
        // m_log_i("[copilot] use localPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        cacheCopilotRoute(path);
        cacheWaypoint(player, target, waypoint);
        requestMove(waypoint);
        return;
    }

    bool nearPath = buildNearRetargetPath(player, target, path);
    // m_log_i("[copilot] nearPath ok=%d steps=%u\n", nearPath ? 1 : 0, static_cast<unsigned int>(path.size()));
    if (nearPath && pickVisibleWaypoint(path, player, waypoint, true)) {
        // m_log_i("[copilot] use nearPath waypoint=%d,%d\n", waypoint.x, waypoint.y);
        cacheCopilotRoute(path);
        cacheWaypoint(player, target, waypoint);
        requestMove(waypoint);
        return;
    }

    // m_log_i("[copilot] no path player=%d,%d target=%d,%d\n", player.x, player.y, target.x, target.y);
    // logNoPathDetail(player, target);
    clearCopilotRoute();
}

void startCopilot(int x, int y)
{
    gCopilotTarget = {x, y};
    gCopilotMapId = gMapId;
    gCopilotMapFloor = gMapFloor;
    gCopilotEnabled = true;
    gLastCopilotTickMs = 0;
    gLastBusyLogMs = 0;
    gHasLastObservedPlayer = false;
    clearRecentPlayers();
    resetPlanCache();
    resetMoveProgress();
    gDynamicBlocked.clear();
    gLastNoPathDetailLogMs = 0;
    gLastSceneSkipLogMs = 0;
    gLastCellLogKey = 0;
    gLastCellLogMs = 0;
    gHasLastRequestedWaypoint = false;
    clearCopilotRoute();
    m_log_i("[copilot] start target=%d,%d player=%d,%d scene=%d state=%d\n", x, y, gPlayerX.value(), gPlayerY.value(), world::gCurrentSceneType, world::gCurrentSceneState);
}

void stopCopilot()
{
    if (gCopilotEnabled) {
        m_log_i("[copilot] stop target=%d,%d player=%d,%d\n", gCopilotTarget.x, gCopilotTarget.y, gPlayerX.value(), gPlayerY.value());
    }
    gCopilotEnabled = false;
    resetPlanCache();
    resetMoveProgress();
    clearCopilotRoute();
}

int getAutoCopliotState()
{
    return gCopilotEnabled ? 1 : 0;
}

std::vector<std::pair<int, int> > getAutoCopliotRoute()
{
    std::vector<std::pair<int, int> > route;
    if (!gCopilotEnabled || !isCopilotSceneReady() || gMapId != gCopilotMapId || gMapFloor != gCopilotMapFloor) {
        return route;
    }

    route.reserve(gAutoCopilotRoute.size());
    for (const auto& point: gAutoCopilotRoute) {
        route.emplace_back(point.x, point.y);
    }
    return route;
}

void CopilotProc()
{
    if (!gCopilotEnabled) {
        return;
    }
    if (gMapId != gCopilotMapId || gMapFloor != gCopilotMapFloor) {
        m_log_i("[copilot] stop map changed expected=%d,%d actual=%d,%d target=%d,%d player=%d,%d\n", gCopilotMapId,
            gCopilotMapFloor, gMapId, gMapFloor, gCopilotTarget.x, gCopilotTarget.y, gPlayerX.value(), gPlayerY.value());
        stopCopilot();
        return;
    }
    if (skipSceneNotReady("proc")) {
        return;
    }

    Point player{gPlayerX.value(), gPlayerY.value()};
    if (player.x == gCopilotTarget.x && player.y == gCopilotTarget.y) {
        m_log_i("[copilot] arrived target=%d,%d\n", gCopilotTarget.x, gCopilotTarget.y);
        stopCopilot();
        return;
    }
    if (gHasExpectedAutoTarget && samePoint(player, gExpectedAutoTarget)) {
        resetMoveProgress();
    }
    if (gHasExpectedAutoTarget) {
        Point clientTarget{gAutoTargetX.value(), gAutoTargetY.value()};
        if (!samePoint(clientTarget, gExpectedAutoTarget)) {
            m_log_i("[copilot] stop external target expected=%d,%d actual=%d,%d player=%d,%d\n", gExpectedAutoTarget.x,
                gExpectedAutoTarget.y, clientTarget.x, clientTarget.y, player.x, player.y);
            stopCopilot();
            return;
        }
    }
    uint64_t now = nowMs();
    if (isBusy()) {
        if (gHasPendingMoveCheck) {
            gMoveCheckSawBusy = true;
        }
        if (gLastBusyLogMs == 0 || now - gLastBusyLogMs >= 1000) {
            gLastBusyLogMs = now;
            m_log_i("[copilot] wait busy path=%d queue=%d moveLock=%d tileEvent=%d targetEvent=%d\n", gPathCount, gMoveQueueCount, gMoveLock, gTileEventLock, gTargetEventLock);
        }
        return;
    }


    if (gLastCopilotTickMs != 0 && now - gLastCopilotTickMs < kCopilotTickMs) {
        return;
    }

    if (!gHasLastObservedPlayer) {
        gLastObservedPlayer = player;
        gHasLastObservedPlayer = true;
    } else if (player.x != gLastObservedPlayer.x || player.y != gLastObservedPlayer.y) {
        rememberRecentPlayer(gLastObservedPlayer);
        gLastObservedPlayer = player;
    }

    int requestDist = gHasPendingMoveCheck ? pointDistance(player, gLastRequestedWaypoint) : 0;
    int requestProgress = gHasPendingMoveCheck ? gLastRequestDistance - requestDist : 0;
    int movedFromRequest = gHasPendingMoveCheck ? pointDistance(player, gLastRequestPlayer) : 0;
    if (gHasPendingMoveCheck) {
        if (requestDist == 0) {
            resetMoveProgress();
        } else if (gMoveCheckSawBusy || movedFromRequest > 0) {
            if (requestProgress <= 0 && movedFromRequest == 0) {
                markDynamicBlockedByMapSource(gLastRequestedWaypoint, "stuck");
                gHasLastRequestedWaypoint = false;
                resetPlanCache();
                resetMoveProgress();
            } else if (movedFromRequest > 0 || requestProgress > 0) {
                gLastRequestPlayer = player;
                gLastRequestDistance = requestDist;
                gMoveCheckSawBusy = false;
            }
        }
    }

    gLastCopilotTickMs = now;
    // m_log_i("[copilot] proc player=%d,%d target=%d,%d reqDist=%d reqProgress=%d scene=%d state=%d\n", player.x, player.y,
    //     gCopilotTarget.x, gCopilotTarget.y, requestDist, requestProgress, world::gCurrentSceneType, world::gCurrentSceneState);
    autoCopilot(gCopilotTarget.x, gCopilotTarget.y);
}

} // namespace cg::script
