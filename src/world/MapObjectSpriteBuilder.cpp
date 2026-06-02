#include "world/MapObjectSpriteBuilder.hpp"

#include "world/MapObjectTextureCache.hpp"

namespace cg::world {

std::optional<GroupedMapObjectSpriteCmd> BuildMapObjectSpriteCmd(
    const MapObjectRenderInput& input,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t enqueueIndex)
{
    const MapObject* object = input.object;
    const MapObjectTexture* texture = input.texture;
    if (object == nullptr
        || texture == nullptr
        || !texture->texture.IsValid()
        || texture->width <= 0
        || texture->height <= 0) {
        return std::nullopt;
    }

    const std::optional<MapObjectRenderGroup> group = ClassifyMapObjectRenderGroup(input.mapNo, input.flag2);
    if (!group.has_value()) {
        return std::nullopt;
    }

    const ScreenPixelCoord screen = ProjectTileDeltaToScreen(
        TileCoord{
            object->tile.x - originTile.x,
            object->tile.y - originTile.y,
        },
        originScreen);

    cg::render::SpriteCmd sprite{};
    sprite.texture = texture->texture;
    sprite.source = cg::render::RectF{
        0.0F,
        0.0F,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height),
    };
    sprite.destination = cg::render::RectF{
        static_cast<float>(screen.x + texture->offsetX),
        static_cast<float>(screen.y + texture->offsetY),
        static_cast<float>(texture->width),
        static_cast<float>(texture->height),
    };
    sprite.sortY = 0;
    sprite.order = MakeIdaCompatibleOrder(enqueueIndex);

    return GroupedMapObjectSpriteCmd{
        *group,
        sprite,
    };
}

} // namespace cg::world
