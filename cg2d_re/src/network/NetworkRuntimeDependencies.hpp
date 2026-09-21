#pragma once

#include <cstdint>

namespace cg::network::detail {

// sub_403E50@0x403E50 尚未实现；只保留当前 IDA 的真实回调契约。
std::int32_t Sub_403E50(
    std::uint32_t value,
    const void* data,
    std::uint32_t size);

} // namespace cg::network::detail
