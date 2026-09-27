#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::resource::animation {

// 原 gAnimate 记录为 0x24 字节；三个原 W32 指针在 x64 承载为原生指针。
struct AnimateRecord {
    std::uint32_t animeNo;                   // 原 +00，查询与排序键。
    std::uint16_t count;                     // 原 +04，action 数量。
    std::uint16_t coordinateCount;           // 原 +06。
    std::int16_t fileIndex;                  // 原 +08。
    std::uint16_t value0A;                   // 原 +0A，意义未证。
    std::uint32_t offset;                    // 原 +0C。
    void* listNode;                          // 原 +10，装载时写入链节点。
    std::uint32_t coordinateOffset;          // 原 +14。
    void* actionList;                        // 原 +18，可释放子表。
    void* coordinates;                       // 原 +1C，可释放坐标。
    std::uint16_t flag;                      // 原 +20。
    std::array<std::uint8_t, 2> value22;    // 原 +22/+23，意义未证。
};

// 原 9A4A5C 独立的 0x24 字节缓存记录；+18/+1C 是可释放引用。
struct AnimeCacheRecord {
    std::array<std::uint32_t, 6> values;  // 原 +00..+17，字段暂不拆分。
    void* actionList;                     // 原 +18，可释放子表。
    void* buffer;                         // 原 +1C，可释放缓冲。
    std::array<std::uint8_t, 4> tail;     // 原 +20..+23，意义未证。
};

static_assert(std::is_standard_layout_v<AnimateRecord>);
static_assert(std::is_trivially_copyable_v<AnimateRecord>);
static_assert(sizeof(AnimateRecord) == 0x38);
static_assert(offsetof(AnimateRecord, animeNo) == 0);
static_assert(offsetof(AnimateRecord, listNode) == 0x10);
static_assert(offsetof(AnimateRecord, coordinateOffset) == 0x18);
static_assert(offsetof(AnimateRecord, actionList) == 0x20);
static_assert(offsetof(AnimateRecord, coordinates) == 0x28);
static_assert(offsetof(AnimateRecord, flag) == 0x30);
static_assert(std::is_standard_layout_v<AnimeCacheRecord>);
static_assert(std::is_trivially_copyable_v<AnimeCacheRecord>);
static_assert(sizeof(AnimeCacheRecord) == 0x30);
static_assert(offsetof(AnimeCacheRecord, actionList) == 0x18);
static_assert(offsetof(AnimeCacheRecord, buffer) == 0x20);

// 原 9A4A58/9A4A60/9A4A4C；GraphicAlloc 分配主表，AnimeInit 发布元素数。
// 新端 backing 需保留 count 索引：原 getIndex 将元素数作为包含上界。
extern AnimateRecord* gAnimateRecords9A4A58;
extern AnimeCacheRecord* gAnimateCache9A4A5C;
extern std::int32_t gAnimateCount9A4A60;
extern std::int32_t gMaxAnimateCount9A4A4C;

// 原 45B420：qsort 的无符号首 W32 升序比较。
std::int32_t __cdecl Compare_45B420(const void* left, const void* right);

// 原 45B390/45B400：包含上界，跨度不足 10 时顺扫。
std::int32_t __cdecl BinarySearch_45B390(
    std::uint32_t animeNo, std::int32_t min, std::int32_t max);
std::int32_t __cdecl GetIndex_45B400(std::uint32_t animeNo);

} // namespace cg::resource::animation
