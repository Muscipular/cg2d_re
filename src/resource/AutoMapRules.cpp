#include "AutoMapRules.hpp"

namespace cg::resource {

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

} // namespace cg::resource
