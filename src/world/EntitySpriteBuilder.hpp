#pragma once

#include "render/RenderTypes.hpp"
#include "render/Texture.hpp"
#include "world/AnimationState.hpp"
#include "world/EntityId.hpp"
#include "world/Movement.hpp"
#include "world/Transform.hpp"
#include "world/WorldRenderer.hpp"

#include <cstdint>
#include <optional>

namespace cg::resource {

struct AnimationClip;

} // namespace cg::resource

namespace cg::world {

struct EntitySpriteGraphic {
    cg::render::TextureHandle texture{};
    cg::render::RectF source{};
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
};

class IEntitySpriteGraphicResolver {
public:
    virtual ~IEntitySpriteGraphicResolver() = default;

    IEntitySpriteGraphicResolver(const IEntitySpriteGraphicResolver&) = delete;
    IEntitySpriteGraphicResolver& operator=(const IEntitySpriteGraphicResolver&) = delete;

    IEntitySpriteGraphicResolver(IEntitySpriteGraphicResolver&&) = delete;
    IEntitySpriteGraphicResolver& operator=(IEntitySpriteGraphicResolver&&) = delete;

    [[nodiscard]] virtual std::optional<EntitySpriteGraphic> ResolveEntityGraphic(
        std::int32_t graphicSeqNo,
        std::int16_t paletteIndex) = 0;

protected:
    IEntitySpriteGraphicResolver() = default;
};

struct EntitySpriteBuildInput {
    EntityId id{};
    const Movement* movement = nullptr;
    AnimationState* animation = nullptr;
    const cg::resource::AnimationClip* clip = nullptr;
    IEntitySpriteGraphicResolver* graphicResolver = nullptr;
    std::int16_t movingAction = -1;
    TileCoord originTile{};
    ScreenPixelCoord originScreen{};
    std::uint64_t nowTimeMs = 0;
    std::uint64_t deltaMs = 0;
    std::uint32_t layer = 0;
    std::int32_t sortY = 0;
};

[[nodiscard]] std::optional<WorldEntitySprite> BuildEntitySprite(const EntitySpriteBuildInput& input);

} // namespace cg::world
