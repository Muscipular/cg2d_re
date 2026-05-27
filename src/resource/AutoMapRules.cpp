#include "AutoMapRules.hpp"

namespace cg::resource {

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
