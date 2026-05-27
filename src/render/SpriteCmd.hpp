#pragma once

#include <cstdint>

#include "Color.hpp"
#include "RenderTypes.hpp"
#include "Texture.hpp"

namespace cg::render {

// SpriteCmd 表示一次 sprite 绘制命令，可用于地图、角色与 UI sprite。
struct SpriteCmd {
    TextureHandle texture = {};
    RectF source = {};
    RectF destination = {};
    ColorF color = {1.0F, 1.0F, 1.0F, 1.0F};
    std::uint32_t layer = 0;
    std::int32_t sortY = 0;
    std::uint32_t order = 0;
};

} // namespace cg::render
