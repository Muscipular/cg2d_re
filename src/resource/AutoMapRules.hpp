#pragma once

#include <cstdint>
#include <optional>
#include <span>

#include "Palette.hpp"
#include "ResourceTypes.hpp"

namespace cg::resource {

struct AutoMapScreenPoint {
    std::int32_t x{};
    std::int32_t y{};
};

// AutoMapCellRect 表示自动地图格子覆盖范围，端点均包含。
struct AutoMapCellRect {
    std::int32_t minX{};
    std::int32_t minY{};
    std::int32_t maxX{};
    std::int32_t maxY{};
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

// 自动地图 collision overlay 显示标志来源：automap.cpp / MapExControl。
// 仅表示阻挡层显示开关，不是完整寻路/碰撞规则。
[[nodiscard]] bool IsAutoMapCollisionVisible(std::uint16_t hitRaw) noexcept;

// 自动地图 object 覆盖规则使用 Graphic::getFlag1，即 GraphicInfo::flag % 100。
// colorIndex 为 0 表示不绘制。
[[nodiscard]] std::optional<AutoMapCellRect> ComputeObjectCoverRect(
    const GraphicInfo& info,
    std::int32_t objectX,
    std::int32_t objectY,
    std::uint8_t colorIndex) noexcept;

} // namespace cg::resource
