#include "world/SceneGroundAtlasCache.hpp"

#include "render/TextureCache.hpp"

#include <algorithm>
#include <cstring>
#include <limits>
#include <string_view>
#include <utility>

namespace cg::world {
namespace {

constexpr int AtlasDefaultRowWidth = 2048;
constexpr int Rgba8BytesPerPixel = 4;

struct DecodedTile {
    std::uint32_t imageNo = 0;
    cg::resource::DecodedImage image;
    cg::render::RectF source;
};

struct PendingEntry {
    std::uint16_t mapX = 0;
    std::uint16_t mapY = 0;
    std::uint32_t imageNo = 0;
    TileRenderLayer layer = TileRenderLayer::Normal;
    std::size_t tileIndex = 0;
};

struct AtlasBuffer {
    int width = 0;
    int height = 0;
    int pitch = 0;
    std::vector<std::uint8_t> pixels;
};

[[nodiscard]] std::uint32_t LayerMask(TileRenderLayer layer) noexcept
{
    return 1U << static_cast<std::uint32_t>(layer);
}

[[nodiscard]] std::string MakeRegionKey(std::uint32_t imageNo)
{
    return std::to_string(imageNo);
}

[[nodiscard]] bool TryGetRgba8ByteCount(int width, int height, std::size_t& byteCount) noexcept
{
    if (width <= 0 || height <= 0) {
        return false;
    }

    const auto pixelCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    if (pixelCount > std::numeric_limits<std::size_t>::max() / Rgba8BytesPerPixel) {
        return false;
    }

    byteCount = pixelCount * Rgba8BytesPerPixel;
    return true;
}

[[nodiscard]] bool IsValidDecodedTileImage(const cg::resource::DecodedImage& image) noexcept
{
    std::size_t expectedBytes = 0;
    return image.format == cg::render::PixelFormat::Rgba8
        && TryGetRgba8ByteCount(image.width, image.height, expectedBytes)
        && image.pixels.size() == expectedBytes;
}

[[nodiscard]] bool BuildPackedAtlasBuffer(std::vector<DecodedTile>& tiles, AtlasBuffer& buffer)
{
    if (tiles.empty()) {
        return true;
    }

    int atlasWidth = AtlasDefaultRowWidth;
    for (const DecodedTile& tile : tiles) {
        atlasWidth = std::max(atlasWidth, tile.image.width);
    }

    int rowX = 0;
    int rowY = 0;
    int rowHeight = 0;
    int rowLimit = AtlasDefaultRowWidth;

    for (DecodedTile& tile : tiles) {
        const int tileRowLimit = std::max(AtlasDefaultRowWidth, tile.image.width);
        if (rowHeight == 0) {
            rowLimit = tileRowLimit;
        } else if (rowX + tile.image.width > rowLimit) {
            rowY += rowHeight;
            rowX = 0;
            rowHeight = 0;
            rowLimit = tileRowLimit;
        }

        tile.source = cg::render::RectF{
            static_cast<float>(rowX),
            static_cast<float>(rowY),
            static_cast<float>(tile.image.width),
            static_cast<float>(tile.image.height),
        };

        rowX += tile.image.width;
        rowHeight = std::max(rowHeight, tile.image.height);
    }

    const int atlasHeight = rowY + rowHeight;
    if (atlasHeight <= 0 || atlasWidth > std::numeric_limits<int>::max() / Rgba8BytesPerPixel) {
        return false;
    }

    const int pitch = atlasWidth * Rgba8BytesPerPixel;
    if (static_cast<std::size_t>(atlasHeight) > std::numeric_limits<std::size_t>::max() / static_cast<std::size_t>(pitch)) {
        return false;
    }

    buffer.width = atlasWidth;
    buffer.height = atlasHeight;
    buffer.pitch = pitch;
    buffer.pixels.assign(static_cast<std::size_t>(pitch) * static_cast<std::size_t>(atlasHeight), 0U);

    for (const DecodedTile& tile : tiles) {
        const auto dstX = static_cast<int>(tile.source.x);
        const auto dstY = static_cast<int>(tile.source.y);
        const int srcPitch = tile.image.width * Rgba8BytesPerPixel;
        for (int y = 0; y < tile.image.height; ++y) {
            const std::size_t srcOffset = static_cast<std::size_t>(y) * static_cast<std::size_t>(srcPitch);
            const std::size_t dstOffset =
                (static_cast<std::size_t>(dstY + y) * static_cast<std::size_t>(buffer.width)
                    + static_cast<std::size_t>(dstX))
                * Rgba8BytesPerPixel;
            std::memcpy(buffer.pixels.data() + dstOffset, tile.image.pixels.data() + srcOffset, static_cast<std::size_t>(srcPitch));
        }
    }

    return true;
}

} // namespace

SceneGroundAtlasCache::SceneGroundAtlasCache(
    cg::render::TextureCache& textureCache,
    IGroundTileImageProvider& imageProvider) noexcept
    : textureCache_(textureCache)
    , imageProvider_(imageProvider)
{
}

const SceneGroundAtlas* SceneGroundAtlasCache::GetOrBuild(
    const MapTileAtlasCacheKey& key,
    const cg::resource::MapData& map)
{
    PruneForKey(key);

    if (const auto iter = cache_.find(key); iter != cache_.end()) {
        return &iter->second.atlas;
    }

    const std::string textureKey = MakeTextureKey(key);
    std::optional<SceneGroundAtlas> atlas = BuildAtlas(key, map, textureKey);
    if (!atlas.has_value()) {
        return nullptr;
    }

    try {
        const auto [iter, inserted] = cache_.emplace(key, CachedAtlas{std::move(*atlas), textureKey});
        return inserted ? &iter->second.atlas : &iter->second.atlas;
    } catch (...) {
        static_cast<void>(textureCache_.Remove(textureKey));
        throw;
    }
}

std::string SceneGroundAtlasCache::MakeTextureKey(const MapTileAtlasCacheKey& key)
{
    return "scene-ground-atlas:"
        "mapId="
        + std::to_string(key.mapId)
        + ";serverNo=" + std::to_string(key.serverNo)
        + ";floor=" + std::to_string(key.floor)
        + ";tileLayerMask=" + std::to_string(key.tileLayerMask)
        + ";mapGeneration=" + std::to_string(key.mapGeneration)
        + ";resourceGeneration=" + std::to_string(key.resourceGeneration);
}

std::optional<SceneGroundAtlas> SceneGroundAtlasCache::BuildAtlas(
    const MapTileAtlasCacheKey& key,
    const cg::resource::MapData& map,
    std::string_view textureKey)
{
    const std::size_t cellCount = map.CellCount();
    if (map.tileData.size() < cellCount) {
        return std::nullopt;
    }

    std::vector<DecodedTile> tiles;
    std::vector<PendingEntry> pendingEntries;
    std::unordered_map<std::uint32_t, std::size_t> imageToTileIndex;

    for (std::size_t index = 0; index < cellCount; ++index) {
        const std::uint32_t imageNo = DecodeMapCellForWorld(map.tileData[index]);
        const std::optional<TileRenderLayer> layer = ClassifyWorldTileLayer(imageNo);
        if (!layer.has_value() || (key.tileLayerMask & LayerMask(*layer)) == 0U) {
            continue;
        }

        std::size_t tileIndex = 0;
        if (const auto iter = imageToTileIndex.find(imageNo); iter != imageToTileIndex.end()) {
            tileIndex = iter->second;
        } else {
            std::optional<cg::resource::DecodedImage> image = imageProvider_.DecodeTile(imageNo);
            if (!image.has_value() || !IsValidDecodedTileImage(*image)) {
                return std::nullopt;
            }

            tileIndex = tiles.size();
            imageToTileIndex.emplace(imageNo, tileIndex);
            tiles.push_back(DecodedTile{imageNo, std::move(*image), {}});
        }

        pendingEntries.push_back(PendingEntry{
            static_cast<std::uint16_t>(index % map.width),
            static_cast<std::uint16_t>(index / map.width),
            imageNo,
            *layer,
            tileIndex,
        });
    }

    AtlasBuffer buffer;
    if (!BuildPackedAtlasBuffer(tiles, buffer)) {
        return std::nullopt;
    }

    SceneGroundAtlas result;
    result.key = key;

    if (!tiles.empty()) {
        const cg::render::TextureHandle texture =
            textureCache_.LoadRgba8(textureKey, buffer.width, buffer.height, buffer.pixels.data(), buffer.pitch);
        if (!texture.IsValid()) {
            return std::nullopt;
        }
        result.atlas.SetTexture(texture);

        for (const DecodedTile& tile : tiles) {
            static_cast<void>(result.atlas.AddRegion(MakeRegionKey(tile.imageNo), tile.source));
        }
    }

    result.entries.reserve(pendingEntries.size());
    for (const PendingEntry& pending : pendingEntries) {
        const DecodedTile& tile = tiles[pending.tileIndex];
        result.entries.push_back(SceneGroundAtlasEntry{
            pending.mapX,
            pending.mapY,
            pending.imageNo,
            pending.layer,
            tile.source,
            tile.image.offsetX,
            tile.image.offsetY,
            tile.image.width,
            tile.image.height,
        });
    }

    return result;
}

void SceneGroundAtlasCache::PruneForKey(const MapTileAtlasCacheKey& key)
{
    const bool resourceChanged = std::any_of(
        cache_.begin(),
        cache_.end(),
        [&key](const auto& entry) {
            return entry.first.resourceGeneration != key.resourceGeneration;
        });
    if (resourceChanged) {
        for (const auto& entry : cache_) {
            RemoveCachedTexture(entry.second);
        }
        cache_.clear();
        return;
    }

    for (auto iter = cache_.begin(); iter != cache_.end();) {
        const MapTileAtlasCacheKey& cachedKey = iter->first;
        if (cachedKey.mapId == key.mapId
            && cachedKey.serverNo == key.serverNo
            && cachedKey.floor == key.floor
            && cachedKey.mapGeneration != key.mapGeneration) {
            RemoveCachedTexture(iter->second);
            iter = cache_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void SceneGroundAtlasCache::RemoveCachedTexture(const CachedAtlas& cached)
{
    static_cast<void>(textureCache_.Remove(cached.textureKey));
}

} // namespace cg::world
