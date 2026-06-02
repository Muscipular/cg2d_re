#pragma once

#include "render/RenderTypes.hpp"
#include "render/SpriteCmd.hpp"
#include "render/Texture.hpp"
#include "world/EntityId.hpp"
#include "world/MapObject.hpp"
#include "world/MapTileAtlasCacheKey.hpp"
#include "world/Transform.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace cg::resource {

struct MapData;

} // namespace cg::resource

namespace cg::world {

class MapObjectTextureCache;
class SceneGroundAtlasCache;

// WorldRenderCamera 表达当前地图渲染视口和 tile 投影原点。
struct WorldRenderCamera {
    TileCoord originTile{};
    ScreenPixelCoord originScreen{};
    cg::render::RectF cameraRect{};
};

// WorldMapObjectSprite 是 WorldRenderer 接收的轻量地图 obj 输入。
struct WorldMapObjectSprite {
    MapObject object{};
    std::int32_t mapNo = 0;
    std::int32_t flag2 = 0;
};

// WorldEntitySprite 表达已解析到可画帧的 world entity sprite。
struct WorldEntitySprite {
    EntityId id{};
    cg::render::TextureHandle texture{};
    cg::render::RectF source{};
    float anchorScreenX = 0.0F;
    float anchorScreenY = 0.0F;
    float drawOffsetX = 0.0F;
    float drawOffsetY = 0.0F;
    float width = 0.0F;
    float height = 0.0F;
    std::uint32_t layer = 0;
    std::int32_t sortY = 0;
};

// WorldRenderLayerMap 显式声明地图 obj 分组到 SpriteCmd layer 的映射。
struct WorldRenderLayerMap {
    std::uint32_t mapNoOne = 0;
    std::uint32_t highMapNoFlag2Three = 0;
    std::uint32_t highMapNoOtherFlag2 = 0;
    std::uint32_t unknown = 0;
};

// WorldRenderOrderBase 为不同类别保留独立原始入队索引空间。
struct WorldRenderOrderBase {
    std::uint32_t ground = 0;
    std::uint32_t mapObject = 0;
    std::uint32_t entity = 0;
};

struct WorldRenderInput {
    const cg::resource::MapData* map = nullptr;
    MapTileAtlasCacheKey groundKey{};
    std::span<const WorldMapObjectSprite> mapObjects{};
    std::span<const WorldEntitySprite> entities{};
    WorldRenderCamera camera{};
    std::uint64_t nowMs = 0;
    WorldRenderLayerMap layers{};
    WorldRenderOrderBase orderBase{};
};

struct WorldRenderFrame {
    std::vector<cg::render::SpriteCmd> sprites;

    void Clear() noexcept;
};

class WorldRenderer final {
public:
    WorldRenderer(SceneGroundAtlasCache& groundAtlasCache, MapObjectTextureCache& mapObjectTextureCache) noexcept;

    void BuildFrame(const WorldRenderInput& input, WorldRenderFrame& out);

private:
    SceneGroundAtlasCache& groundAtlasCache_;
    MapObjectTextureCache& mapObjectTextureCache_;
};

} // namespace cg::world
