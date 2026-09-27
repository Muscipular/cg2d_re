#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::resource::animation {

// 原 gAnimate 的 0x24 字节记录；当前只解释二分查询读取的首 W32。
struct AnimateRecord {
    std::uint32_t animeNo;
    std::array<std::uint8_t, 0x20> remaining;
};

static_assert(std::is_standard_layout_v<AnimateRecord>);
static_assert(std::is_trivially_copyable_v<AnimateRecord>);
static_assert(sizeof(AnimateRecord) == 0x24);
static_assert(offsetof(AnimateRecord, animeNo) == 0);

// 原 9A4A58/9A4A60/9A4A4C；由待落位的 GraphicAlloc/AnimeInit 发布。
// 新端 backing 需保留 count 索引：原 getIndex 将元素数作为包含上界。
extern AnimateRecord* gAnimateRecords9A4A58;
extern std::int32_t gAnimateCount9A4A60;
extern std::int32_t gMaxAnimateCount9A4A4C;

// 原 45B420：qsort 的无符号首 W32 升序比较。
std::int32_t __cdecl Compare_45B420(const void* left, const void* right);

// 原 45B390/45B400：包含上界，跨度不足 10 时顺扫。
std::int32_t __cdecl BinarySearch_45B390(
    std::uint32_t animeNo, std::int32_t min, std::int32_t max);
std::int32_t __cdecl GetIndex_45B400(std::uint32_t animeNo);

} // namespace cg::resource::animation
