#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原 x86 节点 +0x10/+0x14 是非拥有地址；x64 只拓宽链接，前四个 S32 保持。
struct LegacyMouseTrackerNode {
    std::int32_t x;
    std::int32_t y;
    std::int32_t tag;
    std::int32_t count;
    LegacyMouseTrackerNode* next;
    LegacyMouseTrackerNode* prev;
};

static_assert(std::is_standard_layout_v<LegacyMouseTrackerNode>);
static_assert(std::is_trivially_copyable_v<LegacyMouseTrackerNode>);
static_assert(sizeof(LegacyMouseTrackerNode)
    == 0x10 + 2 * sizeof(LegacyMouseTrackerNode*));
static_assert(offsetof(LegacyMouseTrackerNode, x) == 0x00);
static_assert(offsetof(LegacyMouseTrackerNode, y) == 0x04);
static_assert(offsetof(LegacyMouseTrackerNode, tag) == 0x08);
static_assert(offsetof(LegacyMouseTrackerNode, count) == 0x0C);
static_assert(offsetof(LegacyMouseTrackerNode, next) == 0x10);
static_assert(offsetof(LegacyMouseTrackerNode, prev)
    == 0x10 + sizeof(LegacyMouseTrackerNode*));

// 原 0x58ACBC、0x58ACC0 的唯一 signed32 配置存储。修改 capacity 后须调用重置。
extern std::int32_t gLegacyMouseTrackerThreshold;
extern std::int32_t gLegacyMouseTrackerCapacity;

// BRU01：只读原固定池映像偏移[0,2400)；链接BYTE按原节点/哨兵身份投影。
// 不跟随动态nativePool、不消费RNG或返回可解引用地址；正常调用点见登录负分类SPEC。
std::uint8_t ReadLegacyMouseTrackerFixedByte(std::size_t byteOffset);

// 原 sub_506580@[0x506580,0x5066C1)：重建节点池和空闲/活动链。
std::int32_t ResetLegacyMouseTracker_506580();

// 原 sub_5066D0@[0x5066D0,0x5066E9)：恢复默认配置后重建追踪器。
std::int32_t ResetLegacyMouseTrackerDefaults_5066D0();

// 原 [0x50E7B0,0x50E7C8)：忽略网络首参，先写容量、再写阈值并重建追踪器。
std::int32_t ConfigureLegacyMouseTracker_50E7B0(
    std::uintptr_t unusedSocket,
    std::int32_t capacity,
    std::int32_t threshold);

// 原 sub_5067F0@[0x5067F0,0x50686C)：按 (x,y,tag) 追踪按下次数。
std::int32_t TrackLegacyMouseDown_5067F0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t tag);

} // namespace cg::platform
