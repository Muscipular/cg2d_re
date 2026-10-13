#pragma once

#include "cg/platform/LegacyCryptInt.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

namespace cg::platform {

// 原 [0xD68CF8,0xD68DC4) 的旧鼠标状态。末尾 0xD68DC4 由 Startup.hpp
// 中的既有唯一存储承接，不属于本结构。
struct LegacyMouseState {
    // 原0x00506460、0x00506530：成员正序构造及失败前缀清理由C++承接。
    LegacyMouseState() = default;
    // 原0x00506380：成员自动逆序析构。
    ~LegacyMouseState() = default;
    LegacyCryptInt mouseX; // 原成员偏移：+0x00。
    LegacyCryptInt mouseY; // 原成员偏移：+0x10。
    LegacyCryptInt buttonCoordinates[8]; // 原成员偏移：+0x20。
    std::uint32_t heldButtons; // 原成员偏移：+0xA0。
    std::uint32_t edgeButtons; // 原成员偏移：+0xA4。
    std::uint32_t pulseButtons; // 原成员偏移：+0xA8。
    std::uint32_t leftDownTick; // 原成员偏移：+0xAC。
    std::uint32_t leftElapsed; // 原成员偏移：+0xB0。
    std::uint32_t rightDownTick; // 原成员偏移：+0xB4。
    std::uint32_t rightElapsed; // 原成员偏移：+0xB8。
    std::uint32_t valueD68DB4; // 原全局：0x00D68DB4。
    std::int32_t valueD68DB8; // 原全局：0x00D68DB8。
    std::uint32_t drawingSuppression; // 原成员偏移：+0xC4。
    std::uint32_t tailReset; // 原成员偏移：+0xC8。
};

static_assert(std::is_standard_layout_v<LegacyMouseState>);
static_assert(!std::is_trivially_destructible_v<LegacyMouseState>);
static_assert(sizeof(LegacyMouseState) == 0xCC);
static_assert(offsetof(LegacyMouseState, mouseX) == 0x00);
static_assert(offsetof(LegacyMouseState, mouseY) == 0x10);
static_assert(offsetof(LegacyMouseState, buttonCoordinates) == 0x20);
static_assert(sizeof(LegacyMouseState::buttonCoordinates) == 0x80);
static_assert(offsetof(LegacyMouseState, heldButtons) == 0xA0);
static_assert(offsetof(LegacyMouseState, edgeButtons) == 0xA4);
static_assert(offsetof(LegacyMouseState, pulseButtons) == 0xA8);
static_assert(offsetof(LegacyMouseState, leftDownTick) == 0xAC);
static_assert(offsetof(LegacyMouseState, leftElapsed) == 0xB0);
static_assert(offsetof(LegacyMouseState, rightDownTick) == 0xB4);
static_assert(offsetof(LegacyMouseState, rightElapsed) == 0xB8);
static_assert(offsetof(LegacyMouseState, valueD68DB4) == 0xBC);
static_assert(offsetof(LegacyMouseState, valueD68DB8) == 0xC0);
static_assert(offsetof(LegacyMouseState, drawingSuppression) == 0xC4);
static_assert(offsetof(LegacyMouseState, tailReset) == 0xC8);

std::optional<LegacyMouseState>& BorrowLegacyMouseStateStorage(); // 原全局：0x00D68CF8。

// 对应原CRT 5530F8；鼠标拥有者接管同一十值构析，原构造前缀清理保持。
// 原函数：0x00506340。
class MouseRuntimeStartup final {
public:
    // 原函数：0x00506340；只接管同组原始载荷的启动寿命。
    MouseRuntimeStartup();
    ~MouseRuntimeStartup();
    MouseRuntimeStartup(const MouseRuntimeStartup&) = delete;
    MouseRuntimeStartup& operator=(const MouseRuntimeStartup&) = delete;
};

// 原右锚 0xD68BF0 与左锚 0xD68DC8；不并入中间连续状态。
extern std::uint32_t gValueD68BF0; // 原全局：0x00D68BF0。
extern std::uint32_t gValueD68DC8; // 原全局：0x00D68DC8。

// 原 sub_506870@[0x506870,0x50688B)。返回值保持为 0。
std::int32_t ResetLegacyMouseState_506870();

// 原 MouseEvent::Reset_0@[0x506890,0x5068FF)，严格先赋 X、后赋 Y。
void ResetLegacyMousePosition_506890(std::int32_t x, std::int32_t y);

// 原 MouseEvent::UpdateLDown@[0x506900,0x5069DD)。
std::int32_t UpdateLegacyMouseLeftDown_506900(
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateLUp@[0x5069E0,0x506A8B)。
std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateLDbClick@[0x506A90,0x506B44)。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateRDown@[0x506B50,0x506C32)。
std::int32_t UpdateLegacyMouseRightDown_506B50(
    std::int32_t x,
    std::int32_t y);

// 原 sub_506C40@[0x506C40,0x506CEB)，对应右键抬起。
std::int32_t UpdateLegacyMouseRightUp_506C40(
    std::int32_t x,
    std::int32_t y);

// 原 sub_506CF0@[0x506CF0,0x506DA4)，对应右键双击。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    std::int32_t x,
    std::int32_t y);

// 原 sub_506DB0@[0x506DB0,0x506DC3)；按 32 位回绕累加滚轮增量并返回原增量。
std::int32_t AddLegacyMouseWheelDelta_506DB0(std::int32_t delta) noexcept;

} // namespace cg::platform
