#pragma once

#include "render/SpriteCmd.hpp"
#include "world/IdaRenderRules.hpp"
#include "world/MapObject.hpp"
#include "world/Transform.hpp"

#include <cstdint>
#include <optional>

namespace cg::world {

struct MapObjectTexture;

struct MapObjectRenderInput {
    const MapObject* object = nullptr;
    std::int32_t mapNo = 0;
    std::int32_t flag2 = 0;
    const MapObjectTexture* texture = nullptr;
};

struct GroupedMapObjectSpriteCmd {
    MapObjectRenderGroup group = MapObjectRenderGroup::MapNoOne;
    cg::render::SpriteCmd sprite{};
};

[[nodiscard]] std::optional<GroupedMapObjectSpriteCmd> BuildMapObjectSpriteCmd(
    const MapObjectRenderInput& input,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t enqueueIndex);

} // namespace cg::world
