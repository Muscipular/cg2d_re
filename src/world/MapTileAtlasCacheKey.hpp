#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::world {

inline constexpr std::uint32_t MapTileAtlasSpecialLayerMask = 1U << 1U;
inline constexpr std::uint32_t MapTileAtlasNormalLayerMask = 1U << 5U;
inline constexpr std::uint32_t MapTileAtlasAllLayerMask = MapTileAtlasSpecialLayerMask | MapTileAtlasNormalLayerMask;

// MapTileAtlasCacheKey 标识地图 tile atlas 的可复用缓存粒度。
struct MapTileAtlasCacheKey {
    int mapId{};
    int serverNo{};
    int floor{};
    std::uint32_t tileLayerMask{};
    std::uint64_t mapGeneration{};
    std::uint64_t resourceGeneration{};

    bool operator==(const MapTileAtlasCacheKey&) const = default;
};

struct MapTileAtlasCacheKeyHash {
    [[nodiscard]] std::size_t operator()(const MapTileAtlasCacheKey& key) const noexcept
    {
        std::size_t hash = static_cast<std::size_t>(0xcbf29ce484222325ULL);
        Combine(hash, static_cast<std::uint32_t>(key.mapId));
        Combine(hash, static_cast<std::uint32_t>(key.serverNo));
        Combine(hash, static_cast<std::uint32_t>(key.floor));
        Combine(hash, key.tileLayerMask);
        Combine(hash, key.mapGeneration);
        Combine(hash, key.resourceGeneration);
        return hash;
    }

private:
    static void Combine(std::size_t& hash, std::uint64_t value) noexcept
    {
        hash ^= static_cast<std::size_t>(value + 0x9e3779b97f4a7c15ULL + (hash << 6U) + (hash >> 2U));
    }
};

} // namespace cg::world
