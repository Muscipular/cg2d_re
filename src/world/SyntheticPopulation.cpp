#include "world/SyntheticPopulation.hpp"

#include <cstdint>

namespace cg::world {
namespace {

class SyntheticRandom final {
public:
    explicit SyntheticRandom(std::uint32_t seed) noexcept
        : state_(seed)
    {
    }

    [[nodiscard]] std::uint32_t Next() noexcept
    {
        state_ = state_ * 1664525U + 1013904223U;
        return state_;
    }

private:
    std::uint32_t state_;
};

[[nodiscard]] bool IsTileRangeValid(TileCoord minTile, TileCoord maxTile) noexcept
{
    return minTile.x <= maxTile.x && minTile.y <= maxTile.y;
}

[[nodiscard]] bool IsScreenRangeValid(const cg::render::RectF& range) noexcept
{
    return range.width >= 0.0F && range.height >= 0.0F;
}

[[nodiscard]] bool IsLayerRangeValid(SyntheticPopulationLayerRange range) noexcept
{
    return range.min <= range.max;
}

[[nodiscard]] bool IsSortYRangeValid(SyntheticPopulationSortYRange range) noexcept
{
    return range.min <= range.max;
}

[[nodiscard]] bool HasRequiredResources(const SyntheticPopulationConfig& config) noexcept
{
    return config.count != 0U
        && !config.textures.empty()
        && !config.sources.empty()
        && config.width > 0.0F
        && config.height > 0.0F
        && IsTileRangeValid(config.minTile, config.maxTile)
        && IsLayerRangeValid(config.layerRange)
        && IsSortYRangeValid(config.sortYRange);
}

[[nodiscard]] bool IsConfigValid(const SyntheticPopulationConfig& config) noexcept
{
    if (!HasRequiredResources(config)) {
        return false;
    }

    switch (config.placement) {
    case SyntheticPopulationPlacement::Screen:
        return IsScreenRangeValid(config.screenRange);
    case SyntheticPopulationPlacement::Tile:
        return true;
    }

    return false;
}

[[nodiscard]] std::size_t NextIndex(SyntheticRandom& random, std::size_t size) noexcept
{
    return static_cast<std::size_t>(static_cast<std::uint64_t>(random.Next()) % size);
}

[[nodiscard]] std::uint32_t NextLayer(
    SyntheticRandom& random,
    SyntheticPopulationLayerRange range) noexcept
{
    const std::uint64_t minValue = static_cast<std::uint64_t>(range.min);
    const std::uint64_t maxValue = static_cast<std::uint64_t>(range.max);
    const std::uint64_t span = maxValue - minValue + 1ULL;
    const std::uint64_t offset = static_cast<std::uint64_t>(random.Next()) % span;
    return static_cast<std::uint32_t>(minValue + offset);
}

[[nodiscard]] std::int32_t NextSortY(
    SyntheticRandom& random,
    SyntheticPopulationSortYRange range) noexcept
{
    const std::int64_t minValue = static_cast<std::int64_t>(range.min);
    const std::int64_t maxValue = static_cast<std::int64_t>(range.max);
    const std::uint64_t span = static_cast<std::uint64_t>(maxValue - minValue) + 1ULL;
    const std::uint64_t offset = static_cast<std::uint64_t>(random.Next()) % span;
    const std::int64_t value = minValue + static_cast<std::int64_t>(offset);
    return static_cast<std::int32_t>(value);
}

[[nodiscard]] std::int32_t NextTileCoord(
    SyntheticRandom& random,
    std::int32_t minValue,
    std::int32_t maxValue) noexcept
{
    const std::int64_t minCoord = static_cast<std::int64_t>(minValue);
    const std::int64_t maxCoord = static_cast<std::int64_t>(maxValue);
    const std::uint64_t span = static_cast<std::uint64_t>(maxCoord - minCoord) + 1ULL;
    const std::uint64_t offset = static_cast<std::uint64_t>(random.Next()) % span;
    const std::int64_t value = minCoord + static_cast<std::int64_t>(offset);
    return static_cast<std::int32_t>(value);
}

[[nodiscard]] float NextScreenCoord(SyntheticRandom& random, float minValue, float range) noexcept
{
    if (range == 0.0F) {
        return minValue;
    }

    constexpr float UnitScale = 1.0F / 4294967295.0F;
    return minValue + static_cast<float>(random.Next()) * UnitScale * range;
}

void AssignScreenAnchor(SyntheticRandom& random, const SyntheticPopulationConfig& config, WorldEntitySprite& sprite)
{
    sprite.anchorScreenX = NextScreenCoord(random, config.screenRange.x, config.screenRange.width);
    sprite.anchorScreenY = NextScreenCoord(random, config.screenRange.y, config.screenRange.height);
}

void AssignTileAnchor(SyntheticRandom& random, const SyntheticPopulationConfig& config, WorldEntitySprite& sprite)
{
    constexpr std::int64_t TileScreenHalfWidth = 32;
    constexpr std::int64_t TileScreenHalfHeight = 24;

    const TileCoord tile{
        NextTileCoord(random, config.minTile.x, config.maxTile.x),
        NextTileCoord(random, config.minTile.y, config.maxTile.y),
    };
    const std::int64_t deltaX = static_cast<std::int64_t>(tile.x) - static_cast<std::int64_t>(config.originTile.x);
    const std::int64_t deltaY = static_cast<std::int64_t>(tile.y) - static_cast<std::int64_t>(config.originTile.y);
    const std::int64_t screenX = static_cast<std::int64_t>(config.originScreen.x) + (deltaX + deltaY) * TileScreenHalfWidth;
    const std::int64_t screenY = static_cast<std::int64_t>(config.originScreen.y) + (deltaY - deltaX) * TileScreenHalfHeight;
    sprite.anchorScreenX = static_cast<float>(screenX);
    sprite.anchorScreenY = static_cast<float>(screenY);
}

} // namespace

SyntheticPopulation GenerateSyntheticPopulation(const SyntheticPopulationConfig& config)
{
    SyntheticPopulation population;
    if (!IsConfigValid(config)) {
        return population;
    }

    population.store.Reserve(config.count);
    population.sprites.reserve(config.count);

    SyntheticRandom random{config.seed};
    for (std::size_t index = 0; index < config.count; ++index) {
        WorldEntitySprite sprite{};
        sprite.id = population.store.Create();
        sprite.texture = config.textures[NextIndex(random, config.textures.size())];
        sprite.source = config.sources[NextIndex(random, config.sources.size())];

        if (config.placement == SyntheticPopulationPlacement::Screen) {
            AssignScreenAnchor(random, config, sprite);
        } else {
            AssignTileAnchor(random, config, sprite);
        }

        sprite.width = config.width;
        sprite.height = config.height;
        sprite.layer = NextLayer(random, config.layerRange);
        sprite.sortY = NextSortY(random, config.sortYRange);
        population.sprites.push_back(sprite);
    }

    return population;
}

} // namespace cg::world
