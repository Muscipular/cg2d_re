#pragma once

#include "render/Texture.hpp"
#include "resource/ResourceTypes.hpp"

#include <cstddef>
#include <cstdint>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>

namespace cg::render {

class TextureCache;

} // namespace cg::render

namespace cg::world {

// IMapObjectImageProvider 提供地图 obj 图档解码边界。
class IMapObjectImageProvider {
public:
    virtual ~IMapObjectImageProvider() = default;

    IMapObjectImageProvider(const IMapObjectImageProvider&) = delete;
    IMapObjectImageProvider& operator=(const IMapObjectImageProvider&) = delete;

    IMapObjectImageProvider(IMapObjectImageProvider&&) = delete;
    IMapObjectImageProvider& operator=(IMapObjectImageProvider&&) = delete;

    [[nodiscard]] virtual std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) = 0;

protected:
    IMapObjectImageProvider() = default;
};

struct MapObjectTexture {
    std::uint32_t imageNo = 0;
    cg::render::TextureHandle texture;
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
};

// MapObjectTextureCache 独立缓存地图 obj 纹理，并按 LRU/idle 规则清退。
class MapObjectTextureCache final {
public:
    MapObjectTextureCache(
        cg::render::TextureCache& textureCache,
        IMapObjectImageProvider& imageProvider,
        std::size_t capacity = 100,
        std::uint64_t idleTimeoutMs = 30000) noexcept;

    [[nodiscard]] const MapObjectTexture* GetOrLoad(std::uint32_t imageNo, std::uint64_t nowMs);
    void PruneIdle(std::uint64_t nowMs);
    void Clear();

    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] static std::string MakeTextureKey(std::uint32_t imageNo);

private:
    using LruList = std::list<std::uint32_t>;

    struct CachedTexture {
        MapObjectTexture objectTexture;
        std::string textureKey;
        std::uint64_t lastUsedMs = 0;
        LruList::iterator lruIterator;
    };

    using TextureMap = std::unordered_map<std::uint32_t, CachedTexture>;

    void Touch(CachedTexture& cached, std::uint64_t nowMs) noexcept;
    void EvictLru();
    void RemoveCached(TextureMap::iterator iter);

    cg::render::TextureCache& textureCache_;
    IMapObjectImageProvider& imageProvider_;
    std::size_t capacity_ = 100;
    std::uint64_t idleTimeoutMs_ = 30000;
    LruList lru_;
    TextureMap cache_;
};

} // namespace cg::world
