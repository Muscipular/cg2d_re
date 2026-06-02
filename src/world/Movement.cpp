#include "world/Movement.hpp"

namespace cg::world {

namespace {

[[nodiscard]] WorldPixelCoordF ToWorldPixelCoordF(WorldPixelCoord coord) noexcept
{
    return WorldPixelCoordF{
        static_cast<float>(coord.x),
        static_cast<float>(coord.y),
    };
}

[[nodiscard]] WorldPixelCoordF Lerp(WorldPixelCoord start, WorldPixelCoord target, float progress) noexcept
{
    return WorldPixelCoordF{
        static_cast<float>(start.x) + ((static_cast<float>(target.x) - static_cast<float>(start.x)) * progress),
        static_cast<float>(start.y) + ((static_cast<float>(target.y) - static_cast<float>(start.y)) * progress),
    };
}

} // namespace

Movement MakePixelMovement(
    WorldPixelCoord start,
    WorldPixelCoord target,
    std::uint64_t startTimeMs,
    std::uint64_t durationMs,
    Direction direction) noexcept
{
    return Movement{
        start,
        target,
        startTimeMs,
        durationMs,
        direction,
    };
}

Movement MakeTileMovement(
    TileCoord start,
    TileCoord target,
    std::uint64_t startTimeMs,
    std::uint64_t durationMs,
    Direction direction) noexcept
{
    return MakePixelMovement(
        TileToWorldPixel(start),
        TileToWorldPixel(target),
        startTimeMs,
        durationMs,
        direction);
}

MovementSample SampleMovement(const Movement& movement, std::uint64_t nowTimeMs) noexcept
{
    if (nowTimeMs < movement.startTimeMs) {
        return MovementSample{
            ToWorldPixelCoordF(movement.start),
            0.0F,
            movement.direction,
            false,
        };
    }

    if (movement.durationMs == 0U) {
        return MovementSample{
            ToWorldPixelCoordF(movement.target),
            1.0F,
            movement.direction,
            true,
        };
    }

    const std::uint64_t elapsedMs = nowTimeMs - movement.startTimeMs;
    if (elapsedMs >= movement.durationMs) {
        return MovementSample{
            ToWorldPixelCoordF(movement.target),
            1.0F,
            movement.direction,
            true,
        };
    }

    const float progress = static_cast<float>(elapsedMs) / static_cast<float>(movement.durationMs);
    return MovementSample{
        Lerp(movement.start, movement.target, progress),
        progress,
        movement.direction,
        false,
    };
}

} // namespace cg::world
