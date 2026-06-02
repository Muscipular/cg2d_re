#pragma once

#include "render/SpriteCmd.hpp"
#include "world/SceneGroundAtlasCache.hpp"
#include "world/Transform.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cg::world {

[[nodiscard]] std::size_t AppendSceneGroundSpriteCmds(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    std::vector<cg::render::SpriteCmd>& out);

[[nodiscard]] std::size_t AppendSceneGroundSpriteCmds(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    const cg::render::RectF& cameraRect,
    std::vector<cg::render::SpriteCmd>& out);

} // namespace cg::world
