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
    std::uint16_t used;
    std::uint16_t category;
    std::int32_t objectIndex;
    std::uint32_t imageNo;
    platform::LegacyCryptInt coordinate0C;
    platform::LegacyCryptInt coordinate1C;
    std::array<std::byte, 8> bytes2C;
    std::int32_t value34;
    std::array<std::byte, 8> bytes38;
    std::uint16_t value40;
    std::uint16_t flags42;
    std::array<std::byte, 16> bytes44;
    std::array<std::byte, 0xC8> bytes54; // 本步仅446470清原+54首字节。
    login::YobiAction* action; // 原+11C，Yobi链拥有，本表只借用。
    std::uint16_t flags120;
    std::array<std::byte, 0x1A> bytes122;
};

inline constexpr std::size_t kWorldObjectCapacity = 1000;
extern std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects;
extern std::int32_t gWorldObjectFindCursor; // 原5AC498。
extern std::int32_t gWorldObjectFreeCursor; // 原5AC49C。
extern std::int32_t gWorldObjectUsedCount; // 原5AC4B4。
extern std::int32_t gWorldObjectUpperBound; // 原5F9738；不是占用数。

static_assert(offsetof(WorldObjectRecord, coordinate0C) == 0x0C);
static_assert(offsetof(WorldObjectRecord, coordinate1C) == 0x1C);
static_assert(offsetof(WorldObjectRecord, value34) == 0x34);
static_assert(offsetof(WorldObjectRecord, flags42) == 0x42);
static_assert(offsetof(WorldObjectRecord, bytes54) == 0x54);
static_assert(offsetof(WorldObjectRecord, action) == 0x120);
static_assert(offsetof(WorldObjectRecord, flags120) == 0x128);
static_assert(offsetof(WorldObjectRecord, bytes122) == 0x12A);
static_assert(sizeof(WorldObjectRecord) == 0x148);

} // namespace cg::world::detail
