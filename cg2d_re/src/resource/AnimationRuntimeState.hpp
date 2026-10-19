#pragma once

#include "cg/resource/ResourcePaths.hpp"


#include <cstdio>
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::resource::animation::detail {

struct FrameEntry;

// 原 action 子项步长 0x1C；+0x18 的可释放指针在 x64 拓宽。
struct ActionEntry {
    std::int16_t direction;         // 原 +00，磁盘头低 W16。
    std::int16_t action;            // 原 +02。
    std::uint32_t duration;         // 原 +04，非零帧数时除以 16*frameCount，最小 1。
    std::uint32_t durationLowWord;  // 原 +08，仅保存磁盘 duration 的低 W16 零扩展值。
    std::uint32_t frameCount;       // 原 +0C。
    std::int16_t paletteIndex;      // 原 +10，扩展头 W16。
    std::uint16_t revert;           // 原 +12，扩展头 W16 位型。
    std::uint32_t value14;          // 原 +14，扩展头 W32；短头时为 FFFFFFFF。
    FrameEntry* frames;             // 原 +18，45B760生产的12B帧表。
};
static_assert(std::is_trivially_copyable_v<ActionEntry>);
static_assert(sizeof(ActionEntry) == 0x20);
static_assert(offsetof(ActionEntry, duration) == 0x04);
static_assert(offsetof(ActionEntry, durationLowWord) == 0x08);
static_assert(offsetof(ActionEntry, frameCount) == 0x0C);
static_assert(offsetof(ActionEntry, paletteIndex) == 0x10);
static_assert(offsetof(ActionEntry, revert) == 0x12);
static_assert(offsetof(ActionEntry, value14) == 0x14);
static_assert(offsetof(ActionEntry, frames) == 0x18);

// 原45B650只读取子项+18帧引用；P4前24B与帧载荷未证，不借用主表动作字段。
struct CacheActionEntry {
    std::array<std::uint8_t, 0x18> prefix; // 原成员偏移：+0x00。
    void* frames;                  // 原+18只free/清空，未知载荷保留类型擦除。
};
static_assert(std::is_standard_layout_v<CacheActionEntry>);
static_assert(std::is_trivially_copyable_v<CacheActionEntry>);
static_assert(sizeof(CacheActionEntry) == 0x20);
static_assert(offsetof(CacheActionEntry, frames) == 0x18);

// 原 45B760 生产的 12B 帧；磁盘帧只有前 10B，末两字节不由装载写入。
struct FrameEntry {
    std::uint32_t graphicIndex; // 原成员偏移：+0x00。
    std::int16_t x; // 原成员偏移：+0x04。
    std::int16_t y; // 原成员偏移：+0x06。
    std::uint16_t flag; // 原成员偏移：+0x08。
    std::array<std::uint8_t, 2> unwrittenTail; // 原成员偏移：+0x0A。
};
static_assert(sizeof(FrameEntry) == 0x0C);
static_assert(offsetof(FrameEntry, flag) == 0x08);
static_assert(offsetof(FrameEntry, unwrittenTail) == 0x0A);

// 原 CoordInit 的无指针 8B 记录；只给 graphicIndex 加当前来源文件的基址。
struct CoordinateEntry {
    std::uint32_t graphicIndex; // 原成员偏移：+0x00。
    std::int16_t x; // 原成员偏移：+0x04。
    std::int16_t y; // 原成员偏移：+0x06。
};
static_assert(sizeof(CoordinateEntry) == 8);
static_assert(offsetof(CoordinateEntry, x) == 4);
static_assert(offsetof(CoordinateEntry, y) == 6);

// 原 9A29F8 的 0x10 字节槽，+8/+C 为双向链引用。
struct MainSlot {
    std::int32_t index; // 原成员偏移：+0x00。
    std::uint32_t accountedBytes_04; // 原+04：按原端结构大小累计的记账字节，非x64实际分配大小。
    MainSlot* next; // 原成员偏移：+0x08。
    MainSlot* previous; // 原成员偏移：+0x0C。
};
static_assert(sizeof(MainSlot) == 0x18);
static_assert(offsetof(MainSlot, next) == 0x08);
static_assert(offsetof(MainSlot, previous) == 0x10);

// 原 9A3A48 的独立 0x10 字节槽；当前仅使用首 W32。
struct CacheSlot {
    std::int32_t index; // 原成员偏移：+0x00。
    std::array<std::uint8_t, 0x0C> remaining; // 原成员偏移：+0x04。
};
static_assert(sizeof(CacheSlot) == 0x10);

extern std::array<std::uint32_t, BinLoaderMaxGroups> gFileCloseFlags9A29D0; // 原全局：0x009A29D0。
extern std::array<MainSlot, 256> gAnimationSlots_9A29F8; // 原全局：0x009A29F8。
extern MainSlot gAnimationSlotListTail_9A3A00; // 原全局：0x009A3A00。
extern MainSlot gAnimationSlotListHead_9A3A10; // 原全局：0x009A3A10。
extern std::array<std::FILE*, BinLoaderMaxGroups> gAnimateFiles9A3A20; // 原全局：0x009A3A20。
extern std::array<std::FILE*, BinLoaderMaxGroups> gCoordinateFiles9A2988; // 原全局：0x009A2988。
extern std::array<CacheSlot, 256> gCacheSlots9A3A48; // 原全局：0x009A3A48。
extern std::uint32_t gLoadedAnimationCount_9A4A68; // 原全局：0x009A4A68。
extern std::uint32_t gLoadedAnimationAccountedBytes_9A4A6C; // 原全局：0x009A4A6C。
// 原 9A4A54：AnimeInit 错误路径交给 free 的独立槽；载荷类型未证，释放后不清。
extern void* gCleanupAllocation9A4A54;

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

// 原函数：0x0045B440。
std::int32_t ReleaseAnimationSlotResources_45B440(std::int32_t* slotIndex); // ACL01：真实slotIndex/0或1，held table按原条件重取。
// 原函数：0x0045B590。
std::int32_t AppendAnimationSlotToTail_45B590(detail::MainSlot* slot); // ACL01：持尾前驱按原序挂链，返回零。
// 原函数：0x0045B5B0。
std::int32_t MoveAnimationSlotToTail_45B5B0(detail::MainSlot* slot); // ACL01：前后指针先取齐，再摘链/挂尾。
// 原函数：0x0045B5D0。
std::int32_t InitializeAnimationSlotList_45B5D0(); // ACL01：原256主槽及两哨兵初始化，正文保持。
// 原函数：0x0045B530。
detail::MainSlot* AcquireOldestAnimationSlot_45B530(); // ACL01：清理非零后取齐数量/字节，返回真实oldest或空。

// 原 45C150：仅在有文件/action、无已加载坐标且 count 非零时加载；保留计数门禁。
std::int32_t CoordInit_45C150(std::int32_t index);

// 原45BF60；BLM01读取版本／完整Info后交付FILE，失败不留下临时文件。
// CoordLoadInit取得成功FILE后替换所有组的共享来源，路径顺序为数据、Info。
std::int32_t CoordLoad_45BF60(
    std::FILE** outputFile, const wchar_t* dataPath, const wchar_t* infoPath);

} // namespace cg::resource::animation
