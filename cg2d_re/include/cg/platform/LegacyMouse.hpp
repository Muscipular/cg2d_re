#pragma once

#include "cg/platform/LegacyCryptInt.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原 [0xD68CF8,0xD68DC4) 的旧鼠标状态。末尾 0xD68DC4 由 Startup.hpp
// 中的既有唯一存储承接，不属于本结构。
struct LegacyMouseState {
    LegacyCryptInt mouseX;
    LegacyCryptInt mouseY;
    LegacyCryptInt buttonCoordinates[8];
    std::uint32_t heldButtons;
    std::uint32_t edgeButtons;
    std::uint32_t pulseButtons;
    std::uint32_t leftDownTick;
    std::uint32_t leftElapsed;
    std::uint32_t rightDownTick;
    std::uint32_t rightElapsed;
    std::uint32_t valueD68DB4;
    std::int32_t valueD68DB8;
    std::uint32_t drawingSuppression;
    std::uint32_t tailReset;
};

static_assert(std::is_standard_layout_v<LegacyMouseState>);
static_assert(std::is_trivially_copyable_v<LegacyMouseState>);
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

extern LegacyMouseState gLegacyMouseState;

// 原CRT 5530F8：同一鼠标十值构造后真实登记整体退出，完整S32传出。
std::int32_t InitializeLegacyMouseRuntime_506340();

// 原右锚 0xD68BF0 与左锚 0xD68DC8；不并入中间连续状态。
extern std::uint32_t gValueD68BF0;
extern std::uint32_t gValueD68DC8;

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
