#pragma once

#include <cstdint>
#include <span>

#include "Palette.hpp"

namespace cg::resource {

struct AutoMapScreenPoint {
    std::int32_t x{};
    std::int32_t y{};
};

// DecodeMapCell 校对自 IDA 0x0046CF20 初始化表与 0x0046CF00 查询逻辑。
[[nodiscard]] std::uint32_t DecodeMapCell(std::uint32_t raw) noexcept;

// 自动地图颜色索引来源：AutoMapGetColorIndex(0x00451D80)。
// 只处理 indexed 像素 + 默认 Palette 到 colorIndex 的纯计算。
[[nodiscard]] std::uint8_t ComputeAutoMapColorIndex(
    std::span<const std::uint8_t> indexedPixels,
    const Palette& palette) noexcept;

// 自动地图投影公式来源：MapExScreenFromMap / DrawAutoMapSurface。
// screenY 以 mapWidth 为基准，不使用 mapHeight。
[[nodiscard]] AutoMapScreenPoint ProjectMapCellToAutoMap(
    std::int32_t mapWidth,
    std::int32_t x,
    std::int32_t y) noexcept;

} // namespace cg::resource
