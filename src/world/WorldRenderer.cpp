#include "world/WorldRenderer.hpp"

#include "world/IdaRenderRules.hpp"
#include "world/MapObjectSpriteBuilder.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundAtlasCache.hpp"
#include "world/SceneGroundSpriteBuilder.hpp"
#include "world/VisibilityCulling.hpp"

#include <cstddef>
#include <optional>

namespace cg::world {
namespace {

[[nodiscard]] std::uint32_t MapObjectLayerForGroup(
    const WorldRenderLayerMap& layers,
    MapObjectRenderGroup group) noexcept
{
    switch (group) {
    case MapObjectRenderGroup::MapNoOne:
        return layers.mapNoOne;
    case MapObjectRenderGroup::HighMapNoFlag2Three:
        return layers.highMapNoFlag2Three;
    case MapObjectRenderGroup::HighMapNoOtherFlag2:
        return layers.highMapNoOtherFlag2;
    }

    return layers.unknown;
}

} // namespace

void WorldRenderFrame::Clear() noexcept
{
    sprites.clear();
}

WorldRenderer::WorldRenderer(
    SceneGroundAtlasCache& groundAtlasCache,
    MapObjectTextureCache& mapObjectTextureCache) noexcept
    : groundAtlasCache_(groundAtlasCache)
    , mapObjectTextureCache_(mapObjectTextureCache)
{
}

void WorldRenderer::BuildFrame(const WorldRenderInput& input, WorldRenderFrame& out)
{
    out.Clear();

    const WorldRenderCamera& camera = input.camera;
    if (input.map != nullptr) {
        const SceneGroundAtlas* ground = groundAtlasCache_.GetOrBuild(input.groundKey, *input.map);
        if (ground != nullptr) {
            static_cast<void>(AppendSceneGroundSpriteCmds(
                *ground,
                camera.originTile,
                camera.originScreen,
                input.orderBase.ground,
                camera.cameraRect,
                out.sprites));
        }
    }

    for (std::size_t index = 0; index < input.mapObjects.size(); ++index) {
        const WorldMapObjectSprite& mapObject = input.mapObjects[index];
        const MapObjectTexture* texture = mapObjectTextureCache_.GetOrLoad(mapObject.object.imageNo, input.nowMs);
        const std::optional<GroupedMapObjectSpriteCmd> built = BuildMapObjectSpriteCmd(
            MapObjectRenderInput{&mapObject.object, mapObject.mapNo, mapObject.flag2, texture},
            camera.originTile,
            camera.originScreen,
            input.orderBase.mapObject + static_cast<std::uint32_t>(index),
            camera.cameraRect);
        if (!built.has_value()) {
            continue;
        }

        cg::render::SpriteCmd sprite = built->sprite;
        sprite.layer = MapObjectLayerForGroup(input.layers, built->group);
        out.sprites.push_back(sprite);
    }

    for (std::size_t index = 0; index < input.entities.size(); ++index) {
        const WorldEntitySprite& entity = input.entities[index];
        if (!entity.texture.IsValid() || entity.width <= 0.0F || entity.height <= 0.0F) {
            continue;
        }

        const cg::render::RectF destination = MakeEntitySpriteRect(
            entity.anchorScreenX,
            entity.anchorScreenY,
            entity.drawOffsetX,
            entity.drawOffsetY,
            entity.width,
            entity.height);
        if (!IsSpriteVisible(camera.cameraRect, destination)) {
            continue;
        }

        cg::render::SpriteCmd sprite{};
        sprite.texture = entity.texture;
        sprite.source = entity.source;
        sprite.destination = destination;
        sprite.layer = entity.layer;
        sprite.sortY = entity.sortY;
        sprite.order = MakeIdaCompatibleOrder(input.orderBase.entity + static_cast<std::uint32_t>(index));
        out.sprites.push_back(sprite);
    }
}

} // namespace cg::world
