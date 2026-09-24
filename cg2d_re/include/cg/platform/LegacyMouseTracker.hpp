#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原 x86 鼠标按下追踪节点。next/prev 是原 W32 链接槽，不承载 x64 指针。
struct LegacyMouseTrackerNode32 {
    std::int32_t x;
    std::int32_t y;
    std::int32_t tag;
    std::int32_t count;
    std::uint32_t next;
    std::uint32_t prev;
};

static_assert(std::is_standard_layout_v<LegacyMouseTrackerNode32>);
static_assert(std::is_trivially_copyable_v<LegacyMouseTrackerNode32>);
static_assert(sizeof(LegacyMouseTrackerNode32) == 0x18);
static_assert(offsetof(LegacyMouseTrackerNode32, x) == 0x00);
static_assert(offsetof(LegacyMouseTrackerNode32, y) == 0x04);
static_assert(offsetof(LegacyMouseTrackerNode32, tag) == 0x08);
static_assert(offsetof(LegacyMouseTrackerNode32, count) == 0x0C);
static_assert(offsetof(LegacyMouseTrackerNode32, next) == 0x10);
static_assert(offsetof(LegacyMouseTrackerNode32, prev) == 0x14);

// 原 0x58ACBC、0x58ACC0 的唯一 signed32 配置存储。修改 capacity 后须调用重置。
extern std::int32_t gLegacyMouseTrackerThreshold;
extern std::int32_t gLegacyMouseTrackerCapacity;

// 原 sub_506580@[0x506580,0x5066C1)：重建节点池和空闲/活动链。
std::int32_t ResetLegacyMouseTracker_506580();

// 原 sub_5066D0@[0x5066D0,0x5066E9)：恢复默认配置后重建追踪器。
std::int32_t ResetLegacyMouseTrackerDefaults_5066D0();

// 原 sub_5067F0@[0x5067F0,0x50686C)：按 (x,y,tag) 追踪按下次数。
std::int32_t TrackLegacyMouseDown_5067F0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t tag);

} // namespace cg::platform
