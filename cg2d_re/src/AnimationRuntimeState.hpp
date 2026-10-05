#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {
struct LegacyFile;
} // namespace cg::platform

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
    std::array<std::uint8_t, 0x18> prefix;
    void* frames;                  // 原+18只free/清空，未知载荷保留类型擦除。
};
static_assert(std::is_standard_layout_v<CacheActionEntry>);
static_assert(std::is_trivially_copyable_v<CacheActionEntry>);
static_assert(sizeof(CacheActionEntry) == 0x20);
static_assert(offsetof(CacheActionEntry, frames) == 0x18);

// 原 45B760 生产的 12B 帧；磁盘帧只有前 10B，末两字节不由装载写入。
struct FrameEntry {
    std::uint32_t graphicIndex;
    std::int16_t x;
    std::int16_t y;
    std::uint16_t flag;
    std::array<std::uint8_t, 2> unwrittenTail;
};
static_assert(sizeof(FrameEntry) == 0x0C);
static_assert(offsetof(FrameEntry, flag) == 0x08);
static_assert(offsetof(FrameEntry, unwrittenTail) == 0x0A);

// 原 CoordInit 的无指针 8B 记录；只给 graphicIndex 加当前来源文件的基址。
struct CoordinateEntry {
    std::uint32_t graphicIndex;
    std::int16_t x;
    std::int16_t y;
};
static_assert(sizeof(CoordinateEntry) == 8);
static_assert(offsetof(CoordinateEntry, x) == 4);
static_assert(offsetof(CoordinateEntry, y) == 6);

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
extern std::array<platform::LegacyFile*, 9> gAnimateFiles9A3A20;
extern std::array<platform::LegacyFile*, 9> gCoordinateFiles9A2988;
extern std::array<CacheSlot, 256> gCacheSlots9A3A48;
extern std::uint32_t gActiveCount9A4A68;
extern std::uint32_t gActiveBytes9A4A6C;
// 原 9A4A54：AnimeInit 错误路径交给 free 的独立槽；载荷类型未证，释放后不清。
extern void* gCleanupAllocation9A4A54;

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

std::int32_t Sub_45B440(std::int32_t* slotIndex);
std::int32_t Sub_45B590(detail::MainSlot* slot);
std::int32_t Sub_45B5B0(detail::MainSlot* slot);
std::int32_t Sub_45B5D0();
detail::MainSlot* Sub_45B530();

// 原 45C150：仅在有文件/action、无已加载坐标且 count 非零时加载；保留计数门禁。
std::int32_t CoordInit_45C150(std::int32_t index);

// 原 45BF60：先发布数据文件，再读 Info 版本/索引；失败不回收已打开文件。
// 由 CoordLoadInit 向同一 gCoordinateFiles9A2988 槽传址，路径顺序为数据、Info。
std::int32_t CoordLoad_45BF60(
    platform::LegacyFile** outputFile, const char* dataPath, const char* infoPath);

} // namespace cg::resource::animation
