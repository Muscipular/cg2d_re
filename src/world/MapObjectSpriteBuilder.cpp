#include "world/MapObjectSpriteBuilder.hpp"

#include "world/MapObjectTextureCache.hpp"
#include "world/VisibilityCulling.hpp"

namespace cg::world {

namespace {

std::optional<GroupedMapObjectSpriteCmd> BuildMapObjectSpriteCmdImpl(
    const MapObjectRenderInput& input,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t enqueueIndex,
    const cg::render::RectF* cameraRect)
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

    const cg::render::RectF destination = MakeMapObjectSpriteRect(*object, *texture, originTile, originScreen);
    if (cameraRect != nullptr && !IsSpriteVisible(*cameraRect, destination)) {
        return std::nullopt;
    }

    cg::render::SpriteCmd sprite{};
    sprite.texture = texture->texture;
    sprite.source = cg::render::RectF{
        0.0F,
        0.0F,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height),
    };
    sprite.destination = destination;
    sprite.sortY = 0;
    sprite.order = MakeIdaCompatibleOrder(enqueueIndex);

    return GroupedMapObjectSpriteCmd{
        *group,
        sprite,
    };
}

} // namespace

std::optional<GroupedMapObjectSpriteCmd> BuildMapObjectSpriteCmd(
    const MapObjectRenderInput& input,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t enqueueIndex)
{
    return BuildMapObjectSpriteCmdImpl(input, originTile, originScreen, enqueueIndex, nullptr);
}

std::optional<GroupedMapObjectSpriteCmd> BuildMapObjectSpriteCmd(
    const MapObjectRenderInput& input,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t enqueueIndex,
    const cg::render::RectF& cameraRect)
{
    return BuildMapObjectSpriteCmdImpl(input, originTile, originScreen, enqueueIndex, &cameraRect);
}

} // namespace cg::world
