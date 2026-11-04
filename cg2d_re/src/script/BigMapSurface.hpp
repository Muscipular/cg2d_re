#pragma once

#include "cg/render/PngImage.hpp"
#include <memory>
#include <string>
#include <utility>

namespace cg::session { class ClientSession; }

namespace cg::script {

// BMUI02：大地图专用真实资源缩略底图；拥有地图快照、分块与持久化缓存，不依赖MapEx。
class BigMapSurface final {
public:
    BigMapSurface();
    ~BigMapSurface();
    BigMapSurface(const BigMapSurface&) = delete;
    BigMapSurface& operator=(const BigMapSurface&) = delete;

    int x = 0, y = 0, w = 1, h = 1;
    int mapWidth = 0, mapHeight = 0;
    double mapX = 0, mapY = 0; // 视口中心保留亚格坐标，拖动不量化到整格。

    bool PrepareMap(session::ClientSession& session);
    void Render(int originX, int originY, std::uint8_t layer, const render::PngRectangle& clip);
    std::pair<int, int> MapToScreen(int gridX, int gridY) const;
    std::pair<int, int> ScreenToMap(int screenX, int screenY) const;
    void PanFrom(double startX, double startY, int deltaX, int deltaY);
    bool IsExplored(int gridX, int gridY) const;
    std::wstring Status() const;

private:
    struct State;
    std::unique_ptr<State> state_;
};

} // namespace cg::script
