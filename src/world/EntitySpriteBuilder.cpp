#include "world/EntitySpriteBuilder.hpp"

#include "world/MovementAnimation.hpp"

namespace cg::world {
namespace {

struct ScreenAnchor {
    float x = 0.0F;
    float y = 0.0F;
};

[[nodiscard]] bool IsValidGraphic(const EntitySpriteGraphic& graphic) noexcept
{
    return graphic.texture.IsValid() && graphic.width > 0 && graphic.height > 0;
}

[[nodiscard]] ScreenAnchor ProjectWorldPixelToScreen(
    WorldPixelCoordF world,
    TileCoord originTile,
    ScreenPixelCoord originScreen) noexcept
{
    const WorldPixelCoord originWorld = TileToWorldPixel(originTile);
    const float deltaX = world.x - static_cast<float>(originWorld.x);
    const float deltaY = world.y - static_cast<float>(originWorld.y);

    return ScreenAnchor{
        static_cast<float>(originScreen.x) + ((deltaX + deltaY) * 0.5F),
        static_cast<float>(originScreen.y) + ((deltaY - deltaX) * 0.375F),
    };
}

} // namespace

std::optional<WorldEntitySprite> BuildEntitySprite(const EntitySpriteBuildInput& input)
{
    if (!input.id.IsValid()
        || input.movement == nullptr
        || input.animation == nullptr
        || input.clip == nullptr
        || input.graphicResolver == nullptr) {
        return std::nullopt;
    }

    const MovementAnimationSample sample = AdvanceMovementAnimation(
        *input.animation,
        *input.movement,
        *input.clip,
        input.movingAction,
        input.nowTimeMs,
        input.deltaMs);
    if (!sample.frame.has_value() || sample.frame->frame == nullptr) {
        return std::nullopt;
    }

    std::optional<EntitySpriteGraphic> graphic =
        input.graphicResolver->ResolveEntityGraphic(sample.frame->graphicSeqNo);
    if (!graphic.has_value() || !IsValidGraphic(*graphic)) {
        return std::nullopt;
    }

    const ScreenAnchor anchor = ProjectWorldPixelToScreen(
        sample.movement.position,
        input.originTile,
        input.originScreen);
    const AnimationFrameOffset drawOffset =
        ComputeAnimationDrawOffset(*sample.frame->frame, graphic->offsetX, graphic->offsetY);

    return WorldEntitySprite{
        input.id,
        graphic->texture,
        graphic->source,
        anchor.x,
        anchor.y,
        static_cast<float>(drawOffset.x),
        static_cast<float>(drawOffset.y),
        static_cast<float>(graphic->width),
        static_cast<float>(graphic->height),
        input.layer,
        input.sortY,
    };
}

} // namespace cg::world
