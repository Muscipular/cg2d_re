#pragma once

#include "world/Transform.hpp"

#include <cstdint>

namespace cg::world {

// WorldPixelCoordF 表示采样后的世界像素浮点坐标。
struct WorldPixelCoordF {
    float x{};
    float y{};

    [[nodiscard]] constexpr bool operator==(const WorldPixelCoordF&) const noexcept = default;
};

// Movement 表示一次世界像素移动，坐标存储保持整数。
struct Movement {
    WorldPixelCoord start{};
    WorldPixelCoord target{};
    std::uint64_t startTimeMs{};
    std::uint64_t durationMs{};
    Direction direction{Direction::None};
};

// MovementSample 表示按时间采样后的移动状态。
struct MovementSample {
    WorldPixelCoordF position{};
    float progress{};
    Direction direction{Direction::None};
    bool finished{};
};

[[nodiscard]] Movement MakePixelMovement(
    WorldPixelCoord start,
    WorldPixelCoord target,
    std::uint64_t startTimeMs,
    std::uint64_t durationMs,
    Direction direction) noexcept;

[[nodiscard]] Movement MakeTileMovement(
    TileCoord start,
    TileCoord target,
    std::uint64_t startTimeMs,
    std::uint64_t durationMs,
    Direction direction) noexcept;

[[nodiscard]] MovementSample SampleMovement(const Movement& movement, std::uint64_t nowTimeMs) noexcept;

} // namespace cg::world
