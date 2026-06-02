#pragma once

#include "render/RenderTypes.hpp"
#include "world/Transform.hpp"

namespace cg::world {

struct MapObject;
struct MapObjectTexture;
struct SceneGroundAtlasEntry;

[[nodiscard]] bool IsSpriteVisible(
    const cg::render::RectF& cameraRect,
    const cg::render::RectF& spriteRect) noexcept;

[[nodiscard]] cg::render::RectF MakeTileSpriteRect(
    const SceneGroundAtlasEntry& entry,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept;

[[nodiscard]] bool IsTileSpriteVisible(
    const cg::render::RectF& cameraRect,
    const SceneGroundAtlasEntry& entry,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept;

[[nodiscard]] cg::render::RectF MakeMapObjectSpriteRect(
    const MapObject& object,
    const MapObjectTexture& texture,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept;

[[nodiscard]] bool IsMapObjectSpriteVisible(
    const cg::render::RectF& cameraRect,
    const MapObject& object,
    const MapObjectTexture& texture,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept;

[[nodiscard]] cg::render::RectF MakeEntitySpriteRect(
    float anchorScreenX,
    float anchorScreenY,
    float drawOffsetX,
    float drawOffsetY,
    float width,
    float height) noexcept;

[[nodiscard]] bool IsEntitySpriteVisible(
    const cg::render::RectF& cameraRect,
    float anchorScreenX,
    float anchorScreenY,
    float drawOffsetX,
    float drawOffsetY,
    float width,
    float height) noexcept;

} // namespace cg::world
