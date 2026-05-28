#include "AutoMapRules.hpp"

namespace cg::resource {
namespace {

constexpr std::uint8_t AutoMapPaletteBegin = 16;
constexpr std::uint8_t AutoMapPaletteEnd = 239;

[[nodiscard]] std::uint64_t SquaredDistance(const Rgba8& color, std::uint64_t r, std::uint64_t g, std::uint64_t b) noexcept
{
    const std::int64_t dr = static_cast<std::int64_t>(r) - color.r;
    const std::int64_t dg = static_cast<std::int64_t>(g) - color.g;
    const std::int64_t db = static_cast<std::int64_t>(b) - color.b;
    return static_cast<std::uint64_t>(dr * dr + dg * dg + db * db);
}

} // namespace

AutoMapScreenPoint ProjectMapCellToAutoMap(std::int32_t mapWidth, std::int32_t x, std::int32_t y) noexcept
{
    // 来源：MapExScreenFromMap / DrawAutoMapSurface；screenY 以 mapWidth 为基准。
    return AutoMapScreenPoint{
        static_cast<std::int32_t>(1 + 2 * (x + y)),
        static_cast<std::int32_t>(mapWidth + 1 + y - x),
    };
}

std::uint32_t DecodeMapCell(std::uint32_t raw) noexcept
{
    // 来源：IDA 0x0046CF20 初始化 dword_C35010，0x0046CF00 查询。
    // 仅导出运行期可校对规则，不导出未初始化 IDB 字节。
    if (raw >= 0x10000U) {
        return raw;
    }

    return raw < 20000U ? raw : raw + 200000U;
}

std::uint8_t ComputeAutoMapColorIndex(std::span<const std::uint8_t> indexedPixels, const Palette& palette) noexcept
{
    std::uint64_t sumR = 0;
    std::uint64_t sumG = 0;
    std::uint64_t sumB = 0;
    std::uint64_t count = 0;

    for (const std::uint8_t index : indexedPixels) {
        if (index == 0) {
            continue;
        }

        const Rgba8& color = palette[index];
        sumR += color.r;
        sumG += color.g;
        sumB += color.b;
        ++count;
    }

    if (count == 0) {
        return 0;
    }

    const std::uint64_t averageR = sumR / count;
    const std::uint64_t averageG = sumG / count;
    const std::uint64_t averageB = sumB / count;

    std::uint8_t nearestIndex = AutoMapPaletteBegin;
    std::uint64_t nearestDistance = SquaredDistance(palette[nearestIndex], averageR, averageG, averageB);

    for (std::uint16_t index = AutoMapPaletteBegin + 1; index <= AutoMapPaletteEnd; ++index) {
        const std::uint8_t paletteIndex = static_cast<std::uint8_t>(index);
        const std::uint64_t distance = SquaredDistance(palette[paletteIndex], averageR, averageG, averageB);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestIndex = paletteIndex;
        }
    }

    return nearestIndex;
}

} // namespace cg::resource
