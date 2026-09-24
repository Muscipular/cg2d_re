#pragma once

#include <cstdint>

namespace cg::network::detail {

// 原 [0x5082D0,0x508310) / [0x508310,0x508350)：NetworkLoop 错误日志叶。
std::int32_t Sub_5082D0();
std::int32_t Sub_508310();

// 原 [0x50E6C0,0x50E6D8)：忽略首参数，转发后三项到空间音频生产者。
std::int32_t __cdecl Sub_50E6C0(
    std::int32_t unused, std::int32_t index,
    std::int32_t sourceX, std::int32_t sourceY);

// 原 sub_4038A0@0x4038A0：就地变换 text；key3/key5 参与逐轮计算，
// 第二、四参数未读取；返回末轮方向，零轮返回 0。
char Sub_4038A0(
    char* text,
    std::int32_t,
    const char* key3,
    std::int32_t,
    const char* key5);

// sub_403E50@0x403E50 尚未实现；只保留当前 IDA 的真实回调契约。
std::int32_t Sub_403E50(
    std::uintptr_t socket,
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

namespace cg::character_select::detail {

extern char gText9A4EAC[128];

} // namespace cg::character_select::detail

namespace cg::platform {

void ParseProtectionTables_404600(const char* input);

} // namespace cg::platform
