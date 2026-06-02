#include "world/MapObjectTextureCache.hpp"

#include "render/TextureCache.hpp"

#include <iterator>
#include <limits>
#include <string>

namespace cg::world {
namespace {

constexpr int Rgba8BytesPerPixel = 4;

[[nodiscard]] bool TryGetRgba8ByteCount(int width, int height, std::size_t& byteCount) noexcept
{
    if (width <= 0 || height <= 0) {
        return false;
    }

    const auto safeWidth = static_cast<std::size_t>(width);
    const auto safeHeight = static_cast<std::size_t>(height);
    if (safeWidth > std::numeric_limits<std::size_t>::max() / safeHeight) {
        return false;
    }

    const std::size_t pixelCount = safeWidth * safeHeight;
    if (pixelCount > std::numeric_limits<std::size_t>::max() / Rgba8BytesPerPixel) {
        return false;
    }

    byteCount = pixelCount * Rgba8BytesPerPixel;
    return true;
}

[[nodiscard]] bool IsValidDecodedObjectImage(const cg::resource::DecodedImage& image) noexcept
{
    std::size_t expectedBytes = 0;
    return image.format == cg::render::PixelFormat::Rgba8
        && TryGetRgba8ByteCount(image.width, image.height, expectedBytes)
        && image.pixels.size() == expectedBytes
        && image.width <= std::numeric_limits<int>::max() / Rgba8BytesPerPixel;
}

[[nodiscard]] bool IsIdle(std::uint64_t lastUsedMs, std::uint64_t nowMs, std::uint64_t idleTimeoutMs) noexcept
{
    if (nowMs < lastUsedMs) {
        return false;
    }

    return nowMs - lastUsedMs >= idleTimeoutMs;
}

} // namespace

MapObjectTextureCache::MapObjectTextureCache(
    cg::render::TextureCache& textureCache,
    IMapObjectImageProvider& imageProvider,
    std::size_t capacity,
    std::uint64_t idleTimeoutMs) noexcept
    : textureCache_(textureCache)
    , imageProvider_(imageProvider)
    , capacity_(capacity)
    , idleTimeoutMs_(idleTimeoutMs)
{
}

const MapObjectTexture* MapObjectTextureCache::GetOrLoad(std::uint32_t imageNo, std::uint64_t nowMs)
{
    if (capacity_ == 0U) {
        return nullptr;
    }

    if (const auto iter = cache_.find(imageNo); iter != cache_.end()) {
        Touch(iter->second, nowMs);
        return &iter->second.objectTexture;
    }

    std::optional<cg::resource::DecodedImage> image = imageProvider_.DecodeObject(imageNo);
    if (!image.has_value() || !IsValidDecodedObjectImage(*image)) {
        return nullptr;
    }

    const std::string textureKey = MakeTextureKey(imageNo);
    const int pitch = image->width * Rgba8BytesPerPixel;
    const cg::render::TextureHandle texture =
        textureCache_.LoadRgba8(textureKey, image->width, image->height, image->pixels.data(), pitch);
    if (!texture.IsValid()) {
        return nullptr;
    }

    bool lruInserted = false;
    LruList::iterator lruIterator;
    try {
        lru_.push_back(imageNo);
        lruIterator = std::prev(lru_.end());
        lruInserted = true;

        const MapObjectTexture objectTexture{
            imageNo,
            texture,
            image->width,
            image->height,
            image->offsetX,
            image->offsetY,
        };
        const auto [iter, inserted] = cache_.emplace(
            imageNo,
            CachedTexture{
                objectTexture,
                textureKey,
                nowMs,
                lruIterator,
            });
        if (!inserted) {
            lru_.erase(lruIterator);
            static_cast<void>(textureCache_.Remove(textureKey));
            Touch(iter->second, nowMs);
        }
        while (cache_.size() > capacity_ && !lru_.empty()) {
            EvictLru();
        }
        return &iter->second.objectTexture;
    } catch (...) {
        if (lruInserted) {
            lru_.erase(lruIterator);
        }
        static_cast<void>(textureCache_.Remove(textureKey));
        throw;
    }
}

void MapObjectTextureCache::PruneIdle(std::uint64_t nowMs)
{
    for (auto iter = cache_.begin(); iter != cache_.end();) {
        if (!IsIdle(iter->second.lastUsedMs, nowMs, idleTimeoutMs_)) {
            ++iter;
            continue;
        }

        auto eraseIter = iter;
        ++iter;
        RemoveCached(eraseIter);
    }
}

void MapObjectTextureCache::Clear()
{
    for (const auto& entry : cache_) {
        static_cast<void>(textureCache_.Remove(entry.second.textureKey));
    }

    cache_.clear();
    lru_.clear();
}

std::size_t MapObjectTextureCache::Size() const noexcept
{
    return cache_.size();
}

std::string MapObjectTextureCache::MakeTextureKey(std::uint32_t imageNo)
{
    return "map-object-texture:imageNo=" + std::to_string(imageNo);
}

void MapObjectTextureCache::Touch(CachedTexture& cached, std::uint64_t nowMs) noexcept
{
    cached.lastUsedMs = nowMs;
    lru_.splice(lru_.end(), lru_, cached.lruIterator);
}

void MapObjectTextureCache::EvictLru()
{
    if (lru_.empty()) {
        return;
    }

    const std::uint32_t imageNo = lru_.front();
    if (const auto iter = cache_.find(imageNo); iter != cache_.end()) {
        RemoveCached(iter);
        return;
    }

    lru_.pop_front();
}

void MapObjectTextureCache::RemoveCached(TextureMap::iterator iter)
{
    static_cast<void>(textureCache_.Remove(iter->second.textureKey));
    lru_.erase(iter->second.lruIterator);
    cache_.erase(iter);
}

} // namespace cg::world
