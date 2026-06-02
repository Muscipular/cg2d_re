#pragma once

#include <cstdint>

namespace cg::world {

// TileCoord 表示地图 tile 坐标。
struct TileCoord {
    std::int32_t x{};
    std::int32_t y{};

    [[nodiscard]] constexpr bool operator==(const TileCoord&) const noexcept = default;
};

// WorldPixelCoord 表示世界像素坐标。
struct WorldPixelCoord {
    std::int32_t x{};
    std::int32_t y{};

    [[nodiscard]] constexpr bool operator==(const WorldPixelCoord&) const noexcept = default;
};

// ScreenPixelCoord 表示屏幕像素坐标。
struct ScreenPixelCoord {
    std::int32_t x{};
    std::int32_t y{};

    [[nodiscard]] constexpr bool operator==(const ScreenPixelCoord&) const noexcept = default;
};

// Direction 对应八方向值，不复用资源层方向类型。
enum class Direction : std::int8_t {
    None = -1,
    North = 0,
    Northeast = 1,
    East = 2,
    Southeast = 3,
    South = 4,
    Southwest = 5,
    West = 6,
    Northwest = 7,
};

// TileToWorldPixel 使用当前已确认的 tile 到世界像素规则。
[[nodiscard]] WorldPixelCoord TileToWorldPixel(TileCoord tile) noexcept;

// ProjectTileDeltaToScreen 按相对 tile delta 投影到屏幕坐标。
[[nodiscard]] ScreenPixelCoord ProjectTileDeltaToScreen(TileCoord delta, ScreenPixelCoord origin) noexcept;

} // namespace cg::world
