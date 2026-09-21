#pragma once

#include <cstdint>

namespace cg::network::detail {

// sub_403E50@0x403E50 尚未实现；只保留当前 IDA 的真实回调契约。
std::int32_t Sub_403E50(
    std::uint32_t value,
    const void* data,
    std::uint32_t size);

// 原 sub_5176B0@0x5176B0 尚未实现；只保留真实 W32 返回合同，最终链接仍未闭合。
std::int32_t Sub_5176B0(const char* text);

} // namespace cg::network::detail
