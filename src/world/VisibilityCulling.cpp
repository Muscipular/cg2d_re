#include "world/VisibilityCulling.hpp"

#include "world/MapObject.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundAtlasCache.hpp"

#include <cstdint>

namespace cg::world {

bool IsSpriteVisible(const cg::render::RectF& cameraRect, const cg::render::RectF& spriteRect) noexcept
{
    if (cameraRect.width <= 0.0F
        || cameraRect.height <= 0.0F
        || spriteRect.width <= 0.0F
        || spriteRect.height <= 0.0F) {
        return false;
    }

    const float cameraRight = cameraRect.x + cameraRect.width;
    const float cameraBottom = cameraRect.y + cameraRect.height;
    const float spriteRight = spriteRect.x + spriteRect.width;
    const float spriteBottom = spriteRect.y + spriteRect.height;

    return cameraRect.x < spriteRight
        && spriteRect.x < cameraRight
        && cameraRect.y < spriteBottom
        && spriteRect.y < cameraBottom;
}

cg::render::RectF MakeTileSpriteRect(
    const SceneGroundAtlasEntry& entry,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept
{
    const ScreenPixelCoord screen = ProjectTileDeltaToScreen(
        TileCoord{
            static_cast<std::int32_t>(entry.mapX) - originTile.x,
            static_cast<std::int32_t>(entry.mapY) - originTile.y,
        },
        originScreen);

    return cg::render::RectF{
        static_cast<float>(screen.x + entry.offsetX),
        static_cast<float>(screen.y + entry.offsetY),
        static_cast<float>(entry.width),
        static_cast<float>(entry.height),
    };
}

bool IsTileSpriteVisible(
    const cg::render::RectF& cameraRect,
    const SceneGroundAtlasEntry& entry,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept
{
    return IsSpriteVisible(cameraRect, MakeTileSpriteRect(entry, originTile, originScreen));
}

cg::render::RectF MakeMapObjectSpriteRect(
    const MapObject& object,
    const MapObjectTexture& texture,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept
{
    const ScreenPixelCoord screen = ProjectTileDeltaToScreen(
        TileCoord{
            object.tile.x - originTile.x,
            object.tile.y - originTile.y,
        },
        originScreen);

    return cg::render::RectF{
        static_cast<float>(screen.x + texture.offsetX),
        static_cast<float>(screen.y + texture.offsetY),
        static_cast<float>(texture.width),
        static_cast<float>(texture.height),
    };
}

bool IsMapObjectSpriteVisible(
    const cg::render::RectF& cameraRect,
    const MapObject& object,
    const MapObjectTexture& texture,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept
{
    return IsSpriteVisible(cameraRect, MakeMapObjectSpriteRect(object, texture, originTile, originScreen));
}

cg::render::RectF MakeEntitySpriteRect(
    float anchorScreenX,
    float anchorScreenY,
    float drawOffsetX,
    float drawOffsetY,
    float width,
    float height) noexcept
{
    return cg::render::RectF{
        anchorScreenX + drawOffsetX,
        anchorScreenY + drawOffsetY,
        width,
        height,
    };
}

bool IsEntitySpriteVisible(
    const cg::render::RectF& cameraRect,
    float anchorScreenX,
    float anchorScreenY,
    float drawOffsetX,
    float drawOffsetY,
    float width,
    float height) noexcept
{
    return IsSpriteVisible(
        cameraRect,
        MakeEntitySpriteRect(anchorScreenX, anchorScreenY, drawOffsetX, drawOffsetY, width, height));
}

} // namespace cg::world
