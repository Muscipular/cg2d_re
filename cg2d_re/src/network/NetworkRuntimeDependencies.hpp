#pragma once

#include <cstdint>

namespace cg::network::detail {

// sub_403E50@0x403E50 尚未实现；只保留当前 IDA 的真实回调契约。
std::int32_t Sub_403E50(
    std::uint32_t value,
    const void* data,
    std::uint32_t size);

// 原 sub_5176B0@0x5176B0 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::int32_t Sub_5176B0(const char* text);

// 原 sub_517730@0x517730 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::uint8_t* Sub_517730(
    std::int32_t value,
    std::uint8_t* destination,
    std::int32_t capacity);

} // namespace cg::network::detail
