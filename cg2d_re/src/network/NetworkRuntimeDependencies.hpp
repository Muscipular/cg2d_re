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

// 原 [0x50CF60,0x50CF6A)：忽略网络首参，写共享点数并返回完整值。
std::int32_t Sub_50CF60(std::uintptr_t socket, std::int32_t value);

// 原 [0x50D700,0x50D73A)：场景门禁后写阶段与两项 base62 回复值。
std::int32_t Sub_50D700(std::uintptr_t socket, const char* payload);

// 原 [0x50E830,0x50E844)：共享 W32 非零直接返回，零时清并回写公会邮件。
std::int32_t Sub_50E830(std::uintptr_t unusedSocket);

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

namespace cg::world::detail {

// 原 0xD2F788/0xFF0C10/0xFF0D04/0xFF0D08：复用世界状态的唯一 S32/W16 存储。
extern std::int32_t gValueD2F788;
extern std::uint16_t gValueFF0C10;
extern std::int32_t gValueFF0D04;
extern std::int32_t gValueFF0D08;

} // namespace cg::world::detail
