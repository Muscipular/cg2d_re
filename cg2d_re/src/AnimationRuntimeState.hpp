#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace cg::resource::animation::detail {

// 原 action 子项步长 0x1C；+0x18 的可释放指针在 x64 拓宽。
struct ActionEntry {
    std::array<std::uint8_t, 0x18> values;
    void* frames;
};
static_assert(std::is_trivially_copyable_v<ActionEntry>);
static_assert(sizeof(ActionEntry) == 0x20);
static_assert(offsetof(ActionEntry, frames) == 0x18);

// 原 9A29F8 的 0x10 字节槽，+8/+C 为双向链引用。
struct MainSlot {
    std::int32_t index;
    std::uint32_t value04;
    MainSlot* next;
    MainSlot* previous;
};
static_assert(sizeof(MainSlot) == 0x18);
static_assert(offsetof(MainSlot, next) == 0x08);
static_assert(offsetof(MainSlot, previous) == 0x10);

// 原 9A3A48 的独立 0x10 字节槽；当前仅使用首 W32。
struct CacheSlot {
    std::int32_t index;
    std::array<std::uint8_t, 0x0C> remaining;
};
static_assert(sizeof(CacheSlot) == 0x10);

extern std::array<std::uint32_t, 9> gFileCloseFlags9A29D0;
extern std::array<MainSlot, 256> gMainSlots9A29F8;
extern MainSlot gLeftSentinel9A3A00;
extern MainSlot gRightSentinel9A3A10;
extern std::array<std::FILE*, 9> gAnimateFiles9A3A20;
extern std::array<CacheSlot, 256> gCacheSlots9A3A48;
extern std::uint32_t gActiveCount9A4A68;
extern std::uint32_t gActiveBytes9A4A6C;

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

std::int32_t Sub_45B440(std::int32_t* slotIndex);
std::int32_t Sub_45B590(detail::MainSlot* slot);
std::int32_t Sub_45B5B0(detail::MainSlot* slot);
std::int32_t Sub_45B5D0();
detail::MainSlot* Sub_45B530();

} // namespace cg::resource::animation
