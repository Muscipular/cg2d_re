#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "UIControl.hpp"

namespace cg::ui {
struct UIMapExControl : UIControl {
    struct MapExPoint {
        int mapX = 0;
        int mapY = 0;
        int type = 0;
        int size = 2;
        std::vector<COLORREF> colors;
    };

    int mapX = 0;
    int mapY = 0;
    int mapId = -1;
    int floor = -1;
    int mapWidth = 0;
    int mapHeight = 0;
    int lastMouseX = -1;
    int lastMouseY = -1;
    int lastClickX = -1;
    int lastClickY = -1;

    UIMapExControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, int mapX, int mapY, int mapId, int floor, WindowEventBinding callbacks);

    int AddPoint(MapExPoint point);
    int RemovePoints(int mapX, int mapY);
    const std::vector<MapExPoint>& GetPoints() const;

    // BMUI01：同步现有地图缓存；坐标相对于控件左上角，不含窗口偏移。
    bool PrepareMap();
    std::pair<int, int> MapToScreen(int gridX, int gridY) const;
    std::pair<int, int> ScreenToMap(int screenX, int screenY) const;
    bool IsExplored(int gridX, int gridY) const;

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    bool process(int index, int hasFocused, int captured) override;
    bool process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) override;

private:
    struct MapExTile {
        MapExTile() = default;
        ~MapExTile();
        MapExTile(const MapExTile&) = delete;
        MapExTile& operator=(const MapExTile&) = delete;
        MapExTile(MapExTile&& other) noexcept;
        MapExTile& operator=(MapExTile&& other) noexcept;

        int mapId = -1;
        int group = -1;
        int floor = -1;
        int mode = -1;
        int tileX = -1;
        int tileY = -1;
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        uint64_t lastAccess = 0;
        bool dirty = true;
        PngTexture* texture = nullptr;
        std::vector<uint32_t> pixels;

        void ReleaseTexture();
    };

    struct DiskMapCache {
        int mapId = -1;
        int group = -1;
        int floor = -1;
        int width = 0;
        int height = 0;
        bool loaded = false;
        std::wstring fileName;
        std::vector<unsigned short> tileData;
        std::vector<unsigned short> objData;
        std::vector<unsigned short> hitData;
    };

    struct VisibleMapCache {
        int mapId = -1;
        int group = -1;
        int floor = -1;
        int startX = 0;
        int startY = 0;
        int width = 0;
        int height = 0;
        int playerX = 0;
        int playerY = 0;
        bool loaded = false;
        std::vector<unsigned short> tileData;
        std::vector<unsigned short> objData;
        std::vector<unsigned short> hitData;
    };

    DiskMapCache diskMap;
    VisibleMapCache visibleMap;
    std::vector<MapExPoint> points;
    uint32_t pointRenderFrame = 0;
    std::map<uint64_t, MapExTile> mapTiles;
    int mapBufferW = 0;
    int mapBufferH = 0;
    int mapBufferMode = -1;
    int loadedMapId = -1;
    int loadedMapGroup = -1;
    int loadedFloor = -1;
    int tileBuildFrameCounter = 0;

    bool LoadDiskMapIfNeeded(int effectiveMapId, int effectiveGroup, int effectiveFloor);
    void SyncVisibleMapIfNeeded(int effectiveMapId, int effectiveGroup, int effectiveFloor);
    void ResetMapCache(int effectiveMapId, int effectiveGroup, int effectiveFloor);
    void MarkTilesDirtyForMapRect(int left, int top, int right, int bottom);
    void BuildOneVisibleTile(int startTileX, int endTileX, int startTileY, int endTileY);
    void BuildMapTile(MapExTile& tile, int tileX, int tileY, int mode);
    bool UpdateTileTexture(MapExTile& tile);
    void DrawPoints(int absX, int absY, int drawX, int drawY, const UIClipRect& controlClip);
    MapExTile& GetOrCreateTile(int tileX, int tileY);
    MapExTile* FindTile(int tileX, int tileY);
    void TrimMapTileCache(int startTileX, int endTileX, int startTileY, int endTileY);
    void UpdateMouseMapPos(short ox, short oy, const UIClipRect* clipRect);
};
} // namespace cg::ui
