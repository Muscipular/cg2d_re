#include "../WindowComponents.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/render/Palette.hpp"
#include "../../world/WorldMapState.hpp"
#include <cmath>
#include <limits>
#include <cstring>

namespace cg::ui {
using namespace component_detail;
static constexpr int MAP_EX_TILE_W = 128;
static constexpr int MAP_EX_TILE_H = 64;
static constexpr int MAP_EX_VISIBLE_RADIUS = 25;
static constexpr int MAP_EX_OBJECT_EXTENT = 255;
static constexpr int MAP_EX_AUT_DATA_COUNT = 159201;
static constexpr int MAP_EX_NEW_MAP_NO_MIN = 220000;
static constexpr int MAP_EX_NEW_MAP_NO_MAX = 265535;
static constexpr size_t MAP_EX_TILE_CACHE_MAX = 160;
static constexpr int MAP_EX_TILE_BUILD_INTERVAL = 3;
static constexpr int MAP_EX_MAX_MAP_CELLS = 10000000;
static constexpr size_t MAP_EX_MAX_CANVAS_POINTS = 1024;




static uint64_t MakeMapExTileKey(int tileX, int tileY) {
    return (static_cast<uint64_t>(static_cast<uint32_t>(tileX)) << 32) | static_cast<uint32_t>(tileY);
}

static int EffectiveMapExValue(int value, int fallback) {
    return value >= 0 ? value : fallback;
}

static size_t MapExIndex(int width, int x, int y) {
    return static_cast<size_t>(x) + static_cast<size_t>(y) * width;
}

static uint32_t MapExRgbToArgb(uint32_t rgb) {
    return 0xff000000 | (rgb & 0x00ffffff);
}

static uint32_t MapExPaletteIndexToRgb(uint8_t colorIndex) { return render::GetAutoMapPaletteRgb(colorIndex); }

static int MapExDivRoundNearest(int value, int divisor) {
    if (value >= 0) {
        return (value + divisor / 2) / divisor;
    }
    return -((-value + divisor / 2) / divisor);
}

static bool MapExPlotPixel(std::vector<uint32_t>& pixels, int width, int height, int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    pixels[static_cast<size_t>(y) * width + x] = color;
    return true;
}

static void MapExPlotDiamond(std::vector<uint32_t>& pixels, int width, int height, int x, int y, uint32_t color) {
    MapExPlotPixel(pixels, width, height, x, y, color);
    MapExPlotPixel(pixels, width, height, x - 1, y, color);
    MapExPlotPixel(pixels, width, height, x + 1, y, color);
    MapExPlotPixel(pixels, width, height, x, y - 1, color);
}

static int MapExDecodeMapNo(unsigned short raw) {
    return world::ResolveMapGraphicNumber_46CF00(raw);
}

static uint8_t MapExColorIndexFromMapNo(int mapNo) {
    if (mapNo >= MAP_EX_NEW_MAP_NO_MIN && mapNo <= MAP_EX_NEW_MAP_NO_MAX) {
        return resource::gAutoMapColors927F48[mapNo - 200000];
    }
    if (mapNo <= 0 || mapNo >= static_cast<int>(sizeof(resource::gAutoMapColors927F48))) {
        return 0;
    }
    return resource::gAutoMapColors927F48[mapNo];
}

static uint8_t MapExColorIndexFromRaw(unsigned short raw) {
    return MapExColorIndexFromMapNo(MapExDecodeMapNo(raw));
}

static uint8_t MapExNewMapColorIndexFromRaw(unsigned short raw) {
    int mapNo = MapExDecodeMapNo(raw);
    if (mapNo >= MAP_EX_NEW_MAP_NO_MIN && mapNo <= MAP_EX_NEW_MAP_NO_MAX) {
        return resource::gAutoMapColors927F48[mapNo - 200000];
    }
    return 0;
}

static void MapExScreenFromMap(int mapWidth, int gridX, int gridY, int& screenX, int& screenY) {
    screenX = 1 + 2 * (gridX + gridY);
    screenY = mapWidth + 1 + gridY - gridX;
}

static void MapExIncludeScreenPoint(int mapWidth, int screenX, int screenY, int& minMapX, int& maxMapX, int& minMapY, int& maxMapY) {
    double halfX = (static_cast<double>(screenX) - 1.0) / 2.0;
    double deltaY = static_cast<double>(screenY - mapWidth - 1);
    double mapXd = (halfX - deltaY) / 2.0;
    double mapYd = (halfX + deltaY) / 2.0;
    minMapX = std::min(minMapX, static_cast<int>(std::floor(mapXd)) - 2);
    maxMapX = std::max(maxMapX, static_cast<int>(std::ceil(mapXd)) + 2);
    minMapY = std::min(minMapY, static_cast<int>(std::floor(mapYd)) - 2);
    maxMapY = std::max(maxMapY, static_cast<int>(std::ceil(mapYd)) + 2);
}

void UIMapExControl::MapExTile::ReleaseTexture() {
    if (texture) {
        RendererReleaseSurfaceTexture(texture);
        texture = nullptr;
    }
}

UIMapExControl::MapExTile::~MapExTile() {
    ReleaseTexture();
}

UIMapExControl::MapExTile::MapExTile(MapExTile&& other) noexcept {
    *this = std::move(other);
}

UIMapExControl::MapExTile& UIMapExControl::MapExTile::operator=(MapExTile&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    ReleaseTexture();
    mapId = other.mapId;
    group = other.group;
    floor = other.floor;
    mode = other.mode;
    tileX = other.tileX;
    tileY = other.tileY;
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
    lastAccess = other.lastAccess;
    dirty = other.dirty;
    texture = other.texture;
    pixels = std::move(other.pixels);
    other.texture = nullptr;
    other.dirty = true;
    return *this;
}

UIMapExControl::UIMapExControl(int winId, uint32_t generation, uint32_t id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, int mapX, int mapY, int mapId, int floor, WindowEventBinding callbacks)
    : UIControl(winId, generation, id, TYPE_MAP_EX, name, x, y, w, h, visible, callbacks), mapX(mapX), mapY(mapY), mapId(mapId), floor(floor) {
}

int UIMapExControl::AddPoint(MapExPoint point) {
    point.type = std::clamp(point.type, 0, 3);
    point.size = std::clamp(point.size, 1, 16);
    if (point.colors.empty()) {
        point.colors.push_back(0xffffffff);
    }
    points.push_back(std::move(point));
    return static_cast<int>(points.size());
}

int UIMapExControl::RemovePoints(int pointX, int pointY) {
    size_t oldSize = points.size();
    points.erase(std::remove_if(points.begin(), points.end(), [pointX, pointY](const MapExPoint& point) {
        return point.mapX == pointX && point.mapY == pointY;
    }), points.end());
    return static_cast<int>(oldSize - points.size());
}

const std::vector<UIMapExControl::MapExPoint>& UIMapExControl::GetPoints() const {
    return points;
}

void UIMapExControl::ResetMapCache(int effectiveMapId, int effectiveGroup, int effectiveFloor) {
    diskMap = DiskMapCache{};
    visibleMap = VisibleMapCache{};
    mapTiles.clear();
    mapWidth = 0;
    mapHeight = 0;
    mapBufferW = 0;
    mapBufferH = 0;
    mapBufferMode = -1;
    tileBuildFrameCounter = 0;
    loadedMapId = effectiveMapId;
    loadedMapGroup = effectiveGroup;
    loadedFloor = effectiveFloor;
}

bool UIMapExControl::LoadDiskMapIfNeeded(int effectiveMapId, int effectiveGroup, int effectiveFloor) {
    if (diskMap.loaded && diskMap.mapId == effectiveMapId && diskMap.group == effectiveGroup && diskMap.floor == effectiveFloor) {
        mapWidth = diskMap.width;
        mapHeight = diskMap.height;
        return true;
    }

    DiskMapCache next;
    next.mapId = effectiveMapId;
    next.group = effectiveGroup;
    next.floor = effectiveFloor;

    next.fileName = world::FormatMapDataPath_468520(effectiveMapId, effectiveGroup, effectiveFloor);

    FILE* file = _wfopen(next.fileName.c_str(), L"rb");
    if (!file) {
        diskMap = next;
        return false;
    }

    unsigned char magic[4] = {};
    int header[4] = {};
    bool headerOk = fread(magic, 1, sizeof(magic), file) == sizeof(magic)
        && magic[0] == 'M'
        && magic[1] == 'A'
        && magic[2] == 'P'
        && magic[3] == 1
        && fread(header, sizeof(int), 4, file) == 4
        && header[2] > 0
        && header[3] > 0;
    if (!headerOk) {
        fclose(file);
        diskMap = next;
        return false;
    }

    long long cellCount64 = static_cast<long long>(header[2]) * header[3];
    if (cellCount64 <= 0 || cellCount64 > MAP_EX_MAX_MAP_CELLS) {
        fclose(file);
        diskMap = next;
        return false;
    }

    size_t cellCount = static_cast<size_t>(cellCount64);
    next.width = header[2];
    next.height = header[3];
    next.tileData.resize(cellCount);
    next.objData.resize(cellCount);
    next.hitData.resize(cellCount);

    bool readOk = fread(next.tileData.data(), sizeof(unsigned short), next.tileData.size(), file) == next.tileData.size()
        && fread(next.objData.data(), sizeof(unsigned short), next.objData.size(), file) == next.objData.size()
        && fread(next.hitData.data(), sizeof(unsigned short), next.hitData.size(), file) == next.hitData.size();
    fclose(file);
    if (!readOk) {
        diskMap = DiskMapCache{};
        diskMap.mapId = effectiveMapId;
        diskMap.group = effectiveGroup;
        diskMap.floor = effectiveFloor;
        diskMap.fileName = next.fileName;
        return false;
    }

    next.loaded = true;
    diskMap = std::move(next);
    mapWidth = diskMap.width;
    mapHeight = diskMap.height;
    return true;
}

void UIMapExControl::SyncVisibleMapIfNeeded(int effectiveMapId, int effectiveGroup, int effectiveFloor) {
    short visibleW = world::gMapSpanX;
    short visibleH = world::gMapSpanY;
    if (visibleW <= 0 || visibleH <= 0) {
        visibleMap = VisibleMapCache{};
        return;
    }

    long long cellCount64 = static_cast<long long>(visibleW) * visibleH;
    if (cellCount64 <= 0 || cellCount64 > static_cast<long long>(world::gMapTileArray.size())) {
        visibleMap = VisibleMapCache{};
        return;
    }

    int playerX = static_cast<std::int32_t>(world::gPlayerX);
    int playerY = static_cast<std::int32_t>(world::gPlayerY);
    int startX = world::gMapXStart;
    int startY = world::gMapYStart;
    bool same = visibleMap.loaded
        && visibleMap.mapId == effectiveMapId
        && visibleMap.group == effectiveGroup
        && visibleMap.floor == effectiveFloor
        && visibleMap.startX == startX
        && visibleMap.startY == startY
        && visibleMap.width == visibleW
        && visibleMap.height == visibleH
        && visibleMap.playerX == playerX
        && visibleMap.playerY == playerY;
    if (same) {
        return;
    }

    int oldLeft = visibleMap.playerX - MAP_EX_VISIBLE_RADIUS;
    int oldTop = visibleMap.playerY - MAP_EX_VISIBLE_RADIUS;
    int oldRight = visibleMap.playerX + MAP_EX_VISIBLE_RADIUS;
    int oldBottom = visibleMap.playerY + MAP_EX_VISIBLE_RADIUS;
    bool hadOld = visibleMap.loaded;

    VisibleMapCache next;
    next.mapId = effectiveMapId;
    next.group = effectiveGroup;
    next.floor = effectiveFloor;
    next.startX = startX;
    next.startY = startY;
    next.width = visibleW;
    next.height = visibleH;
    next.playerX = playerX;
    next.playerY = playerY;
    next.loaded = true;

    size_t cellCount = static_cast<size_t>(cellCount64);
    next.tileData.assign(world::gMapTileArray.data(), world::gMapTileArray.data() + cellCount);
    next.objData.assign(world::gMapObjArray.data(), world::gMapObjArray.data() + cellCount);
    next.hitData.assign(world::gMapHitArray.data(), world::gMapHitArray.data() + cellCount);
    visibleMap = std::move(next);

    if (hadOld) {
        MarkTilesDirtyForMapRect(oldLeft, oldTop, oldRight, oldBottom);
    }
    MarkTilesDirtyForMapRect(playerX - MAP_EX_VISIBLE_RADIUS, playerY - MAP_EX_VISIBLE_RADIUS, playerX + MAP_EX_VISIBLE_RADIUS, playerY + MAP_EX_VISIBLE_RADIUS);
}

bool UIMapExControl::PrepareMap() {
    int effectiveMapId = EffectiveMapExValue(mapId, world::gCurrentMapId_9A86DC);
    int effectiveGroup = world::gMapCachePathKey_9E0434;
    int effectiveFloor = EffectiveMapExValue(floor, world::gMapFloor);
    if (effectiveMapId != loadedMapId || effectiveGroup != loadedMapGroup || effectiveFloor != loadedFloor) {
        ResetMapCache(effectiveMapId, effectiveGroup, effectiveFloor);
    }

    if (element().width <= 0 || element().height <= 0 || !LoadDiskMapIfNeeded(effectiveMapId, effectiveGroup, effectiveFloor)) {
        mapBufferW = 0;
        mapBufferH = 0;
        mapBufferMode = -1;
        return false;
    }

    int mode = world::gMapResourceMode_9DAD68 == 1 ? 1 : 0;
    int nextBufferW = 2 * (mapWidth + mapHeight) + 2;
    int nextBufferH = mapWidth + mapHeight + 2;
    if (mapBufferW != nextBufferW || mapBufferH != nextBufferH || mapBufferMode != mode) {
        mapTiles.clear();
        mapBufferW = nextBufferW;
        mapBufferH = nextBufferH;
        mapBufferMode = mode;
    }

    if (static_cast<std::uint32_t>(effectiveMapId) == world::gCurrentMapId_9A86DC && effectiveGroup == world::gMapCachePathKey_9E0434 && effectiveFloor == world::gMapFloor) {
        SyncVisibleMapIfNeeded(effectiveMapId, effectiveGroup, effectiveFloor);
    } else {
        visibleMap = VisibleMapCache{};
    }
    return mapWidth > 0 && mapHeight > 0 && mapBufferW > 0 && mapBufferH > 0;
}

void UIMapExControl::MarkTilesDirtyForMapRect(int left, int top, int right, int bottom) {
    if (mapWidth <= 0 || mapHeight <= 0 || mapBufferW <= 0 || mapBufferH <= 0 || mapTiles.empty()) {
        return;
    }

    left = std::clamp(left, 0, mapWidth - 1);
    right = std::clamp(right, 0, mapWidth - 1);
    top = std::clamp(top, 0, mapHeight - 1);
    bottom = std::clamp(bottom, 0, mapHeight - 1);
    if (left > right || top > bottom) {
        return;
    }

    int sx1 = 0;
    int sy1 = 0;
    int sx2 = 0;
    int sy2 = 0;
    int sx3 = 0;
    int sy3 = 0;
    int sx4 = 0;
    int sy4 = 0;
    MapExScreenFromMap(mapWidth, left, top, sx1, sy1);
    MapExScreenFromMap(mapWidth, right, top, sx2, sy2);
    MapExScreenFromMap(mapWidth, left, bottom, sx3, sy3);
    MapExScreenFromMap(mapWidth, right, bottom, sx4, sy4);

    int minX = std::min(std::min(sx1, sx2), std::min(sx3, sx4)) - 4;
    int maxX = std::max(std::max(sx1, sx2), std::max(sx3, sx4)) + 4;
    int minY = std::min(std::min(sy1, sy2), std::min(sy3, sy4)) - 4;
    int maxY = std::max(std::max(sy1, sy2), std::max(sy3, sy4)) + 4;

    int startTileX = std::max(0, minX / MAP_EX_TILE_W);
    int endTileX = std::min((mapBufferW - 1) / MAP_EX_TILE_W, std::max(0, maxX) / MAP_EX_TILE_W);
    int startTileY = std::max(0, minY / MAP_EX_TILE_H);
    int endTileY = std::min((mapBufferH - 1) / MAP_EX_TILE_H, std::max(0, maxY) / MAP_EX_TILE_H);
    for (int tileY = startTileY; tileY <= endTileY; ++tileY) {
        for (int tileX = startTileX; tileX <= endTileX; ++tileX) {
            MapExTile* tile = FindTile(tileX, tileY);
            if (tile) {
                tile->dirty = true;
            }
        }
    }
}

UIMapExControl::MapExTile& UIMapExControl::GetOrCreateTile(int tileX, int tileY) {
    auto [it, inserted] = mapTiles.try_emplace(MakeMapExTileKey(tileX, tileY));
    auto& tile = it->second;
    if (inserted) {
        tile.tileX = tileX;
        tile.tileY = tileY;
        tile.x = tileX * MAP_EX_TILE_W;
        tile.y = tileY * MAP_EX_TILE_H;
        tile.w = MAP_EX_TILE_W;
        tile.h = MAP_EX_TILE_H;
        tile.dirty = true;
    }
    tile.lastAccess = platform::ReadTickCount64();
    return tile;
}

UIMapExControl::MapExTile* UIMapExControl::FindTile(int tileX, int tileY) {
    auto it = mapTiles.find(MakeMapExTileKey(tileX, tileY));
    if (it == mapTiles.end()) {
        return nullptr;
    }
    it->second.lastAccess = platform::ReadTickCount64();
    return &it->second;
}

void UIMapExControl::BuildOneVisibleTile(int startTileX, int endTileX, int startTileY, int endTileY) {
    int centerScreenX = 0;
    int centerScreenY = 0;
    MapExScreenFromMap(mapWidth, mapX, mapY, centerScreenX, centerScreenY);

    bool found = false;
    int bestTileX = 0;
    int bestTileY = 0;
    int bestPriority = 0;
    long long bestDistance = std::numeric_limits<long long>::max();
    for (int tileY = startTileY; tileY <= endTileY; ++tileY) {
        for (int tileX = startTileX; tileX <= endTileX; ++tileX) {
            MapExTile* tile = FindTile(tileX, tileY);
            bool missingTile = !tile || tile->pixels.empty() || !tile->texture || tile->mapId != loadedMapId || tile->group != loadedMapGroup || tile->floor != loadedFloor || tile->mode != mapBufferMode;
            bool needsBuild = missingTile || tile->dirty;
            if (!needsBuild) {
                continue;
            }

            int priority = missingTile ? 0 : 1;
            int tileCenterX = tileX * MAP_EX_TILE_W + MAP_EX_TILE_W / 2;
            int tileCenterY = tileY * MAP_EX_TILE_H + MAP_EX_TILE_H / 2;
            long long dx = tileCenterX - centerScreenX;
            long long dy = tileCenterY - centerScreenY;
            long long distance = dx * dx + dy * dy;
            if (!found || priority < bestPriority || (priority == bestPriority && distance < bestDistance)) {
                found = true;
                bestTileX = tileX;
                bestTileY = tileY;
                bestPriority = priority;
                bestDistance = distance;
            }
        }
    }

    if (!found) {
        return;
    }

    auto& tile = GetOrCreateTile(bestTileX, bestTileY);
    BuildMapTile(tile, bestTileX, bestTileY, mapBufferMode);
}

void UIMapExControl::BuildMapTile(MapExTile& tile, int tileX, int tileY, int mode) {
    int tileLeft = tileX * MAP_EX_TILE_W;
    int tileTop = tileY * MAP_EX_TILE_H;

    tile.mapId = loadedMapId;
    tile.group = loadedMapGroup;
    tile.floor = loadedFloor;
    tile.mode = mode;
    tile.tileX = tileX;
    tile.tileY = tileY;
    tile.x = tileLeft;
    tile.y = tileTop;
    tile.w = MAP_EX_TILE_W;
    tile.h = MAP_EX_TILE_H;
    tile.dirty = false;
    tile.lastAccess = platform::ReadTickCount64();
    tile.pixels.assign(static_cast<size_t>(tile.w) * tile.h, 0xff000000);

    if (!diskMap.loaded || mapWidth <= 0 || mapHeight <= 0) {
        tile.dirty = !UpdateTileTexture(tile);
        return;
    }

    int minMapX = mapWidth;
    int maxMapX = -1;
    int minMapY = mapHeight;
    int maxMapY = -1;
    int left = tileLeft - 2;
    int top = tileTop - 2;
    int right = tileLeft + tile.w + 2;
    int bottom = tileTop + tile.h + 2;
    MapExIncludeScreenPoint(mapWidth, left, top, minMapX, maxMapX, minMapY, maxMapY);
    MapExIncludeScreenPoint(mapWidth, right, top, minMapX, maxMapX, minMapY, maxMapY);
    MapExIncludeScreenPoint(mapWidth, left, bottom, minMapX, maxMapX, minMapY, maxMapY);
    MapExIncludeScreenPoint(mapWidth, right, bottom, minMapX, maxMapX, minMapY, maxMapY);

    minMapX = std::clamp(minMapX, 0, mapWidth - 1);
    maxMapX = std::clamp(maxMapX, 0, mapWidth - 1);
    minMapY = std::clamp(minMapY, 0, mapHeight - 1);
    maxMapY = std::clamp(maxMapY, 0, mapHeight - 1);
    if (minMapX > maxMapX || minMapY > maxMapY) {
        tile.dirty = !UpdateTileTexture(tile);
        return;
    }

    auto visibleCell = [&](int gridX, int gridY, unsigned short& tileRaw, unsigned short& objRaw, unsigned short& hitRaw) {
        if (!visibleMap.loaded) {
            return false;
        }
        if (gridX < visibleMap.playerX - MAP_EX_VISIBLE_RADIUS || gridX > visibleMap.playerX + MAP_EX_VISIBLE_RADIUS || gridY < visibleMap.playerY - MAP_EX_VISIBLE_RADIUS || gridY > visibleMap.playerY + MAP_EX_VISIBLE_RADIUS) {
            return false;
        }
        int localX = gridX - visibleMap.startX;
        int localY = gridY - visibleMap.startY;
        if (localX < 0 || localY < 0 || localX >= visibleMap.width || localY >= visibleMap.height) {
            return false;
        }
        size_t index = MapExIndex(visibleMap.width, localX, localY);
        if (index >= visibleMap.tileData.size() || index >= visibleMap.objData.size() || index >= visibleMap.hitData.size()) {
            return false;
        }
        tileRaw = visibleMap.tileData[index];
        objRaw = visibleMap.objData[index];
        hitRaw = visibleMap.hitData[index];
        return true;
    };

    auto diskCell = [&](int gridX, int gridY, unsigned short& tileRaw, unsigned short& objRaw, unsigned short& hitRaw) {
        if (gridX < 0 || gridY < 0 || gridX >= diskMap.width || gridY >= diskMap.height) {
            return false;
        }
        size_t index = MapExIndex(diskMap.width, gridX, gridY);
        if (index >= diskMap.tileData.size() || index >= diskMap.objData.size() || index >= diskMap.hitData.size()) {
            return false;
        }
        tileRaw = diskMap.tileData[index];
        objRaw = diskMap.objData[index];
        hitRaw = diskMap.hitData[index];
        return true;
    };

    auto readCell = [&](int gridX, int gridY, unsigned short& tileRaw, unsigned short& objRaw, unsigned short& hitRaw) {
        if (visibleCell(gridX, gridY, tileRaw, objRaw, hitRaw)) {
            return true;
        }
        return diskCell(gridX, gridY, tileRaw, objRaw, hitRaw);
    };

    int regionW = maxMapX - minMapX + 1;
    int regionH = maxMapY - minMapY + 1;
    std::vector<uint8_t> colorIndexes(static_cast<size_t>(regionW) * regionH, 0);
    std::vector<uint8_t> newMapColorIndexes(static_cast<size_t>(regionW) * regionH, 0);
    for (int gridY = minMapY; gridY <= maxMapY; ++gridY) {
        for (int gridX = minMapX; gridX <= maxMapX; ++gridX) {
            unsigned short tileRaw = 0;
            unsigned short objRaw = 0;
            unsigned short hitRaw = 0;
            if (!readCell(gridX, gridY, tileRaw, objRaw, hitRaw)) {
                continue;
            }
            size_t regionIndex = MapExIndex(regionW, gridX - minMapX, gridY - minMapY);
            colorIndexes[regionIndex] = MapExColorIndexFromRaw(tileRaw);
            newMapColorIndexes[regionIndex] = MapExNewMapColorIndexFromRaw(tileRaw);
        }
    }

    int objectMinX = std::max(0, minMapX - MAP_EX_OBJECT_EXTENT + 1);
    int objectMaxX = maxMapX;
    int objectMinY = minMapY;
    int objectMaxY = std::min(mapHeight - 1, maxMapY + MAP_EX_OBJECT_EXTENT - 1);
    for (int objectY = objectMinY; objectY <= objectMaxY; ++objectY) {
        for (int objectX = objectMinX; objectX <= objectMaxX; ++objectX) {
            unsigned short tileRaw = 0;
            unsigned short objRaw = 0;
            unsigned short hitRaw = 0;
            if (!readCell(objectX, objectY, tileRaw, objRaw, hitRaw)) {
                continue;
            }

            int mapNo = MapExDecodeMapNo(objRaw);
            if (mapNo <= 0 || mapNo >= static_cast<int>(sizeof(resource::gAutoMapColors927F48))) {
                continue;
            }
            uint8_t color = MapExColorIndexFromMapNo(mapNo);
            if (!color) {
                continue;
            }

            int seqNo = 0;
            unsigned short flag = 0;
            resource::GetGraphicMapNoIndex(objRaw, &seqNo);
            resource::GetGraphicFlag1(seqNo, &flag);
            if (flag) {
                if (objectX >= minMapX && objectX <= maxMapX && objectY >= minMapY && objectY <= maxMapY) {
                    colorIndexes[MapExIndex(regionW, objectX - minMapX, objectY - minMapY)] = color;
                }
                continue;
            }

            unsigned short sx = 0;
            unsigned short sy = 0;
            resource::GetGraphicSxSy(seqNo, &sx, &sy);
            int coverLeft = std::max(minMapX, objectX);
            int coverRight = std::min(maxMapX, objectX + static_cast<int>(sx) - 1);
            int coverTop = std::max(minMapY, objectY - static_cast<int>(sy) + 1);
            int coverBottom = std::min(maxMapY, objectY);
            for (int coverY = coverTop; coverY <= coverBottom; ++coverY) {
                for (int coverX = coverLeft; coverX <= coverRight; ++coverX) {
                    colorIndexes[MapExIndex(regionW, coverX - minMapX, coverY - minMapY)] = color;
                }
            }
        }
    }

    bool argbMode = mode == 1;
    for (int gridY = minMapY; gridY <= maxMapY; ++gridY) {
        for (int gridX = minMapX; gridX <= maxMapX; ++gridX) {
            int mapIndex = gridY * mapWidth + gridX;
            size_t regionIndex = MapExIndex(regionW, gridX - minMapX, gridY - minMapY);
            uint32_t argb = 0;
            if (argbMode) {
                uint32_t rgb = MapExPaletteIndexToRgb(newMapColorIndexes[regionIndex]);
                if (rgb) {
                    argb = MapExRgbToArgb(rgb);
                }
            }
            if (argbMode && mapIndex >= 0 && mapIndex < MAP_EX_AUT_DATA_COUNT) {
                if (!argb) {
                    const world::AutRecordAF10D8& unit = world::gAutDataAF10D8[mapIndex];
                    uint32_t rgb = (static_cast<unsigned char>(unit.value00) << 16) | (static_cast<unsigned char>(unit.value01) << 8) | static_cast<unsigned char>(unit.value02);
                    if (rgb) {
                        argb = MapExRgbToArgb(rgb);
                    }
                }
            }
            if (!argb) {
                uint8_t colorIndex = colorIndexes[regionIndex];
                uint32_t rgb = MapExPaletteIndexToRgb(colorIndex);
                if (rgb) {
                    argb = MapExRgbToArgb(rgb);
                }
            }
            if (argb) {
                int screenX = 0;
                int screenY = 0;
                MapExScreenFromMap(mapWidth, gridX, gridY, screenX, screenY);
                MapExPlotDiamond(tile.pixels, tile.w, tile.h, screenX - tile.x, screenY - tile.y, argb);
            }
        }
    }
    tile.dirty = !UpdateTileTexture(tile);
}

bool UIMapExControl::UpdateTileTexture(MapExTile& tile) {
    if (tile.pixels.empty() || tile.w <= 0 || tile.h <= 0) {
        tile.ReleaseTexture();
        return false;
    }
    if (!tile.texture) {
        tile.texture = RendererCreateSurfaceTexture(tile.w, tile.h);
    }
    if (!tile.texture) {
        return false;
    }
    if (RendererUpdateSurfaceTexture(tile.texture, tile.pixels.data(), tile.w, tile.h)) {
        return true;
    }

    tile.ReleaseTexture();
    tile.texture = RendererCreateSurfaceTexture(tile.w, tile.h);
    if (!tile.texture) {
        return false;
    }
    if (!RendererUpdateSurfaceTexture(tile.texture, tile.pixels.data(), tile.w, tile.h)) {
        tile.ReleaseTexture();
        return false;
    }
    return true;
}

static void MapExAppendPointShape(std::vector<CanvasShape>& shapes, int type, float x, float y, int size, COLORREF color) {
    CanvasShape shape{};
    shape.color = color;
    switch (type) {
        case 0:
            shape.type = CanvasShapeType::Circle;
            shape.x = x;
            shape.y = y;
            shape.radius = static_cast<float>(size);
            shape.segments = std::clamp(size * 6, 12, 32);
            break;
        case 1:
            shape.type = CanvasShapeType::Rect;
            shape.x = x - static_cast<float>(size);
            shape.y = y - static_cast<float>(size);
            shape.w = static_cast<float>(size * 2 + 1);
            shape.h = static_cast<float>(size * 2 + 1);
            break;
        case 2:
            shape.type = CanvasShapeType::Path;
            shape.triangles.push_back({x, y - static_cast<float>(size)});
            shape.triangles.push_back({x + static_cast<float>(size), y + static_cast<float>(size)});
            shape.triangles.push_back({x - static_cast<float>(size), y + static_cast<float>(size)});
            break;
        case 3:
        default:
            shape.type = CanvasShapeType::Path;
            shape.triangles.push_back({x, y + static_cast<float>(size)});
            shape.triangles.push_back({x - static_cast<float>(size), y - static_cast<float>(size)});
            shape.triangles.push_back({x + static_cast<float>(size), y - static_cast<float>(size)});
            break;
    }
    shapes.push_back(std::move(shape));
}

void UIMapExControl::DrawPoints(int absX, int absY, int drawX, int drawY, const UIClipRect& controlClip) {
    if (points.empty() || mapWidth <= 0 || element().width <= 0 || element().height <= 0) {
        return;
    }

    uint32_t colorFrame = pointRenderFrame++;
    std::vector<CanvasShape> shapes;
    shapes.reserve(std::min(points.size(), MAP_EX_MAX_CANVAS_POINTS));
    for (const auto& point: points) {
        if (point.colors.empty()) {
            continue;
        }

        int screenX = 0;
        int screenY = 0;
        MapExScreenFromMap(mapWidth, point.mapX, point.mapY, screenX, screenY);
        int size = std::clamp(point.size, 1, 16);
        float pointSize = static_cast<float>(size);
        float localX = static_cast<float>(drawX + screenX - absX);
        float localY = static_cast<float>(drawY + screenY - absY);
        if (localX + pointSize < 0.0f || localY + pointSize < 0.0f || localX - pointSize >= static_cast<float>(element().width) || localY - pointSize >= static_cast<float>(element().height)) {
            continue;
        }

        COLORREF color = point.colors[colorFrame % point.colors.size()];
        MapExAppendPointShape(shapes, std::clamp(point.type, 0, 3), localX, localY, size, color);
        if (shapes.size() >= MAP_EX_MAX_CANVAS_POINTS) {
            break;
        }
    }

    if (!shapes.empty()) {
        RendererDrawCanvasClipped(absX, absY, element().width, element().height, LAYER_UI, shapes, &controlClip);
    }
}

void UIMapExControl::TrimMapTileCache(int startTileX, int endTileX, int startTileY, int endTileY) {
    while (mapTiles.size() > MAP_EX_TILE_CACHE_MAX) {
        auto oldest = mapTiles.end();
        for (auto it = mapTiles.begin(); it != mapTiles.end(); ++it) {
            const auto& tile = it->second;
            bool visibleTile = tile.tileX >= startTileX && tile.tileX <= endTileX && tile.tileY >= startTileY && tile.tileY <= endTileY;
            if (visibleTile) {
                continue;
            }
            if (oldest == mapTiles.end() || tile.lastAccess < oldest->second.lastAccess) {
                oldest = it;
            }
        }
        if (oldest == mapTiles.end()) {
            break;
        }
        mapTiles.erase(oldest);
    }
}

bool UIMapExControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UIMapExControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1) {
        return true;
    }

    const int absX = ox + element().x;
    const int absY = oy + element().y;
    UIClipRect controlRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + element().width),
        static_cast<short>(absY + element().height)
    };
    UIClipRect controlClip{};
    if (!UIIntersectClipRect(controlClip, clipRect, controlRect)) {
        return true;
    }

    if (!PrepareMap()) {
        RendererDrawRectClipped(static_cast<int16_t>(absX), static_cast<int16_t>(absY), element().width, element().height, LAYER_UI, 0xff000000, &controlClip);
        return true;
    }

    int centerScreenX = 0;
    int centerScreenY = 0;
    MapExScreenFromMap(mapWidth, mapX, mapY, centerScreenX, centerScreenY);
    int drawX = absX + element().width / 2 - centerScreenX;
    int drawY = absY + element().height / 2 - centerScreenY;
    int srcLeft = std::max(0, static_cast<int>(controlClip.left) - drawX);
    int srcTop = std::max(0, static_cast<int>(controlClip.top) - drawY);
    int srcRight = std::min(mapBufferW, static_cast<int>(controlClip.right) - drawX);
    int srcBottom = std::min(mapBufferH, static_cast<int>(controlClip.bottom) - drawY);
    if (srcRight <= srcLeft || srcBottom <= srcTop) {
        RendererDrawRectClipped(static_cast<int16_t>(absX), static_cast<int16_t>(absY), element().width, element().height, LAYER_UI, 0xff000000, &controlClip);
        return true;
    }

    int startTileX = srcLeft / MAP_EX_TILE_W;
    int endTileX = (srcRight - 1) / MAP_EX_TILE_W;
    int startTileY = srcTop / MAP_EX_TILE_H;
    int endTileY = (srcBottom - 1) / MAP_EX_TILE_H;

    DrawPoints(absX, absY, drawX, drawY, controlClip);
    if (tileBuildFrameCounter == 0) {
        BuildOneVisibleTile(startTileX, endTileX, startTileY, endTileY);
    }
    tileBuildFrameCounter = (tileBuildFrameCounter + 1) % MAP_EX_TILE_BUILD_INTERVAL;
    for (int tileY = startTileY; tileY <= endTileY; ++tileY) {
        for (int tileX = startTileX; tileX <= endTileX; ++tileX) {
            MapExTile* tile = FindTile(tileX, tileY);
            if (!tile || !tile->texture || tile->w <= 0 || tile->h <= 0) {
                continue;
            }
            RendererDrawSurfaceTextureClipped(tile->texture, drawX + tile->x, drawY + tile->y, tile->w, tile->h, 0xffffffff, LAYER_UI, &controlClip);
        }
    }
    TrimMapTileCache(startTileX, endTileX, startTileY, endTileY);
    RendererDrawRectClipped(static_cast<int16_t>(absX), static_cast<int16_t>(absY), element().width, element().height, LAYER_UI, 0xff000000, &controlClip);
    return true;
}

std::pair<int, int> UIMapExControl::MapToScreen(int gridX, int gridY) const {
    return {element().width / 2 + 2 * (gridX - mapX + gridY - mapY),
        element().height / 2 + gridY - mapY - (gridX - mapX)};
}

std::pair<int, int> UIMapExControl::ScreenToMap(int screenX, int screenY) const {
    const int dx = screenX - element().width / 2, dy = screenY - element().height / 2;
    return {mapX + MapExDivRoundNearest(dx - 2 * dy, 4),
        mapY + MapExDivRoundNearest(dx + 2 * dy, 4)};
}

bool UIMapExControl::IsExplored(int gridX, int gridY) const {
    if (gridX < 0 || gridY < 0 || gridX >= mapWidth || gridY >= mapHeight) return false;
    const int localX = gridX - visibleMap.startX, localY = gridY - visibleMap.startY;
    if (visibleMap.loaded && std::abs(gridX - visibleMap.playerX) <= MAP_EX_VISIBLE_RADIUS
        && std::abs(gridY - visibleMap.playerY) <= MAP_EX_VISIBLE_RADIUS
        && localX >= 0 && localY >= 0 && localX < visibleMap.width && localY < visibleMap.height) {
        return (visibleMap.hitData[MapExIndex(visibleMap.width, localX, localY)] & 0x4000) != 0;
    }
    return diskMap.loaded && (diskMap.hitData[MapExIndex(diskMap.width, gridX, gridY)] & 0x4000) != 0;
}

void UIMapExControl::UpdateMouseMapPos(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1 || !hitable || element().width <= 0 || element().height <= 0) {
        lastMouseX = -1;
        lastMouseY = -1;
        return;
    }

    const int absX = ox + element().x;
    const int absY = oy + element().y;
    if (!MouseIsInRect(absX, absY, element().width, element().height) || !UIPointInClipRect(gMouseX, gMouseY, clipRect)) {
        lastMouseX = -1;
        lastMouseY = -1;
        return;
    }

    const auto position = ScreenToMap(gMouseX - absX, gMouseY - absY);
    lastMouseX = position.first;
    lastMouseY = position.second;
    if (gMouseClickState & 0x01) {
        lastClickX = lastMouseX;
        lastClickY = lastMouseY;
    }
}

bool UIMapExControl::process(int index, int hasFocused, int captured) {
    return process(index, hasFocused, captured, nullptr, static_cast<short>(gCurrentWindow->x), static_cast<short>(gCurrentWindow->y));
}

bool UIMapExControl::process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) {
    UpdateMouseMapPos(ox, oy, clipRect);
    return UIControl::process(index, hasFocused, captured, clipRect, ox, oy);
}
} // namespace cg::ui
