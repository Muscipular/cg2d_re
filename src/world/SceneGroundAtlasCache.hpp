#pragma once

#include "render/TextureAtlas.hpp"
#include "resource/MapData.hpp"
#include "resource/ResourceTypes.hpp"
#include "world/IdaRenderRules.hpp"
#include "world/MapTileAtlasCacheKey.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cg::render {

class TextureCache;

} // namespace cg::render

namespace cg::world {

// IGroundTileImageProvider 提供普通地图地板图档解码边界。
class IGroundTileImageProvider {
public:
    virtual ~IGroundTileImageProvider() = default;

    IGroundTileImageProvider(const IGroundTileImageProvider&) = delete;
    IGroundTileImageProvider& operator=(const IGroundTileImageProvider&) = delete;

    IGroundTileImageProvider(IGroundTileImageProvider&&) = delete;
    IGroundTileImageProvider& operator=(IGroundTileImageProvider&&) = delete;

    [[nodiscard]] virtual std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) = 0;

protected:
    IGroundTileImageProvider() = default;
};

struct SceneGroundAtlasEntry {
    std::uint16_t mapX = 0;
    std::uint16_t mapY = 0;
    std::uint32_t imageNo = 0;
    TileRenderLayer layer = TileRenderLayer::Normal;
    cg::render::RectF source;
    int offsetX = 0;
    int offsetY = 0;
    int width = 0;
    int height = 0;
};

struct SceneGroundAtlas {
    MapTileAtlasCacheKey key;
    cg::render::TextureAtlas atlas;
    std::vector<SceneGroundAtlasEntry> entries;
};

// SceneGroundAtlasCache 以地图 key 缓存已上传的地板 atlas。
class SceneGroundAtlasCache final {
public:
    SceneGroundAtlasCache(cg::render::TextureCache& textureCache, IGroundTileImageProvider& imageProvider) noexcept;

    [[nodiscard]] const SceneGroundAtlas* GetOrBuild(
        const MapTileAtlasCacheKey& key,
        const cg::resource::MapData& map);

    [[nodiscard]] static std::string MakeTextureKey(const MapTileAtlasCacheKey& key);

private:
    struct CachedAtlas {
        SceneGroundAtlas atlas;
        std::string textureKey;
    };

    [[nodiscard]] std::optional<SceneGroundAtlas> BuildAtlas(
        const MapTileAtlasCacheKey& key,
        const cg::resource::MapData& map,
        std::string_view textureKey);

    void PruneForKey(const MapTileAtlasCacheKey& key);
    void RemoveCachedTexture(const CachedAtlas& cached);

    cg::render::TextureCache& textureCache_;
    IGroundTileImageProvider& imageProvider_;
    std::unordered_map<MapTileAtlasCacheKey, CachedAtlas, MapTileAtlasCacheKeyHash> cache_;
};

} // namespace cg::world
