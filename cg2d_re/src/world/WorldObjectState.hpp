#pragma once

#include "cg/platform/LegacyCryptInt.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::world::detail {

// 原0x13C记录的增量映射；未消费区不计最终指针/字段映射，见World对象SPEC。
struct WorldObjectRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t category; // 原成员偏移：+0x02。
    std::int32_t objectIndex; // 原成员偏移：+0x04。
    std::uint32_t imageNo; // 原成员偏移：+0x08。
    platform::LegacyCryptInt coordinate0C; // 原成员偏移：+0x0C。
    platform::LegacyCryptInt coordinate1C; // 原成员偏移：+0x1C。
    std::int32_t level2C; // 原成员偏移：+0x2C。
    std::int32_t direction; // 原成员偏移：+0x30。
    std::int32_t value34; // 原成员偏移：+0x34。
    std::int32_t value38; // 原+38；value34待转方向对应的目标X。
    std::int32_t value3C; // 原+3C；同一待转方向对应的目标Y。
    std::uint16_t value40; // 原成员偏移：+0x40。
    std::uint16_t flags42; // 原成员偏移：+0x42。
    std::uint8_t marker44; // 原成员偏移：+0x44。 442BD0置1，449CC0读取；与flags42分属不同槽。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::int32_t value4C; // 原成员偏移：+0x4C。
    std::int32_t value50; // 原成员偏移：+0x50。
    std::array<char, 17> name54; // 原成员偏移：+0x54。 446470只清首字节；生产入口按原字节长度门槛复制。
    std::array<char, 17> title65; // 原成员偏移：+0x65。
    std::array<char, 33> text76; // 原成员偏移：+0x76。
    std::array<char, 33> text97; // 原成员偏移：+0x97。
    std::array<char, 17> textB8; // 原成员偏移：+0xB8。
    std::array<char, 61> nameC9; // 原成员偏移：+0xC9。 445D20/445F40仅在长度<=60时复制含NUL原字节。
    std::array<std::byte, 6> bytes106; // 原成员偏移：+0x106。
    std::int32_t value10C; // 原成员偏移：+0x10C。 442CA0/442D00/442D60的共同数值槽。
    std::array<std::byte, 4> bytes110; // 原成员偏移：+0x110。
    std::uint32_t flags114; // 原成员偏移：+0x114。
    std::int32_t value118; // 原成员偏移：+0x118。
    login::YobiAction* action; // 原+11C，Yobi链拥有，本表只借用。
    std::uint16_t flags120; // 原成员偏移：+0x120。
    std::array<std::byte, 2> bytes122; // 原成员偏移：+0x122。
    std::int32_t value124; // 原+124，442F70状态图号表下标。
    std::array<char, 17> text128; // 原+128；445350的第4段称号。
    std::array<std::byte, 3> bytes139; // 原成员偏移：+0x139。
};

inline constexpr std::size_t kWorldObjectCapacity = 1000;
extern std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects; // 原全局：0x005AC4C0。
extern std::int32_t gWorldObjectFindCursor; // 原5AC498。
extern std::int32_t gWorldObjectFreeCursor; // 原5AC49C。
extern std::int32_t gWorldObjectUsedCount; // 原5AC4B4。
extern std::int32_t gWorldObjectUpperBound; // 原5F9738；不是占用数。

static_assert(offsetof(WorldObjectRecord, coordinate0C) == 0x0C);
static_assert(offsetof(WorldObjectRecord, coordinate1C) == 0x1C);
static_assert(offsetof(WorldObjectRecord, level2C) == 0x2C);
static_assert(offsetof(WorldObjectRecord, direction) == 0x30);
static_assert(offsetof(WorldObjectRecord, value34) == 0x34);
static_assert(offsetof(WorldObjectRecord, value38) == 0x38);
static_assert(offsetof(WorldObjectRecord, value3C) == 0x3C);
static_assert(offsetof(WorldObjectRecord, flags42) == 0x42);
static_assert(offsetof(WorldObjectRecord, marker44) == 0x44);
static_assert(offsetof(WorldObjectRecord, value48) == 0x48);
static_assert(offsetof(WorldObjectRecord, value4C) == 0x4C);
static_assert(offsetof(WorldObjectRecord, value50) == 0x50);
static_assert(offsetof(WorldObjectRecord, name54) == 0x54);
static_assert(offsetof(WorldObjectRecord, title65) == 0x65);
static_assert(offsetof(WorldObjectRecord, text76) == 0x76);
static_assert(offsetof(WorldObjectRecord, text97) == 0x97);
static_assert(offsetof(WorldObjectRecord, textB8) == 0xB8);
static_assert(offsetof(WorldObjectRecord, nameC9) == 0xC9);
static_assert(offsetof(WorldObjectRecord, bytes106) == 0x106);
static_assert(offsetof(WorldObjectRecord, value10C) == 0x10C);
static_assert(offsetof(WorldObjectRecord, bytes110) == 0x110);
static_assert(offsetof(WorldObjectRecord, flags114) == 0x114);
static_assert(offsetof(WorldObjectRecord, value118) == 0x118);
static_assert(offsetof(WorldObjectRecord, action) == 0x120);
static_assert(offsetof(WorldObjectRecord, flags120) == 0x128);
static_assert(offsetof(WorldObjectRecord, bytes122) == 0x12A);
static_assert(offsetof(WorldObjectRecord, value124) == 0x12C);
static_assert(offsetof(WorldObjectRecord, text128) == 0x130);
static_assert(offsetof(WorldObjectRecord, bytes139) == 0x141);
static_assert(sizeof(WorldObjectRecord) == 0x148);

} // namespace cg::world::detail
