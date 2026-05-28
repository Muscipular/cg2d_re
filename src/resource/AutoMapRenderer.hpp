#pragma once

#include "MapData.hpp"
#include "Palette.hpp"
#include "ResourceManager.hpp"

#include <cstdint>
#include <vector>

namespace cg::resource {

// AutoMapImage 保存自动地图 RGBA8 输出，不包含 SDL 或渲染后端资源。
struct AutoMapImage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels;
};

// AutoMapRenderOptions 控制自动地图三层显示开关。
struct AutoMapRenderOptions {
    bool drawTiles = true;
    bool drawObjects = true;
    bool drawCollision = false;
    Rgba8 collisionColor {255, 0, 0, 160};
};

// RenderAutoMapRgba8 输出纯 RGBA8 buffer；缺少颜色或 AUT_DATA fallback 时保持透明。
[[nodiscard]] AutoMapImage RenderAutoMapRgba8(
    const MapData& map,
    const ResourceManager& resourceManager,
    const Palette& palette,
    const AutoMapRenderOptions& options);

} // namespace cg::resource
