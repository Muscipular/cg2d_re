#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"

#include <cstring>

namespace cg::platform {

// 原函数：0x00506870。
std::int32_t ResetLegacyMouseState_506870()
{
    auto& state = gLegacyMouseState;
    const LegacyCryptInt::RawPayload zero{};
    state.mouseX.RestoreRaw(zero);
    state.mouseY.RestoreRaw(zero);
    for (LegacyCryptInt& value : state.buttonCoordinates) {
        value.RestoreRaw(zero);
    }
    // 原整块零字节：普通字段直接清零，不重构十成员或消费RNG。
    constexpr std::size_t tail = offsetof(LegacyMouseState, heldButtons);
    std::memset(reinterpret_cast<std::byte*>(&state) + tail, 0, sizeof(state) - tail);
    gRequestedCursorState_D68DC4 = 0;
    state.valueD68DB8 = -1;
    return 0;
}

// 原函数：0x00506890。
void ResetLegacyMousePosition_506890(const std::int32_t x, const std::int32_t y)
{
    gLegacyMouseState.mouseX = x;

    gLegacyMouseState.mouseY = y;
}

// 原函数：0x00506900。
void UpdateLegacyMouseLeftDown_506900(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | LegacyMouseEdge_LDown;
    gLegacyMouseState.heldButtons = heldButtons | LegacyMouseButton_Left;
    gLegacyMouseState.pulseButtons = pulseButtons | LegacyMouseButton_Left;

    gLegacyMouseState.buttonCoordinates[0] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    gLegacyMouseState.leftDownTick = tick;
    gLegacyMouseState.leftElapsed = 0;
}

// 原函数：0x005069E0。
std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons & ~LegacyMouseButton_Left;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | LegacyMouseEdge_LUp;
    gLegacyMouseState.pulseButtons = pulseButtons & ~LegacyMouseButton_Left;

    gLegacyMouseState.buttonCoordinates[2] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[3] = static_cast<std::int32_t>(y);

    gLegacyMouseState.leftDownTick = 0;
    gLegacyMouseState.leftElapsed = 0;
    return 0;
}

// 原函数：0x00506A90。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons | LegacyMouseButton_Left;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | (LegacyMouseEdge_LDown | LegacyMouseEdge_LDblClick);
    gLegacyMouseState.pulseButtons = pulseButtons | LegacyMouseButton_Left;

    gLegacyMouseState.buttonCoordinates[0] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    gLegacyMouseState.leftDownTick = tick;
    gLegacyMouseState.leftElapsed = 0;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00506B50。
void UpdateLegacyMouseRightDown_506B50(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.heldButtons = heldButtons | LegacyMouseButton_Right;
    gLegacyMouseState.pulseButtons = pulseButtons | LegacyMouseButton_Right;
    gLegacyMouseState.edgeButtons = edgeButtons | LegacyMouseEdge_RDown;

    gLegacyMouseState.buttonCoordinates[4] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    gLegacyMouseState.rightDownTick = tick;
    gLegacyMouseState.rightElapsed = 0;
}

// 原函数：0x00506C40。
std::int32_t UpdateLegacyMouseRightUp_506C40(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons & ~LegacyMouseButton_Right;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | LegacyMouseEdge_RUp;
    gLegacyMouseState.pulseButtons = pulseButtons & ~LegacyMouseButton_Right;

    gLegacyMouseState.buttonCoordinates[6] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[7] = static_cast<std::int32_t>(y);

    gLegacyMouseState.rightDownTick = 0;
    gLegacyMouseState.rightElapsed = 0;
    return 0;
}

// 原函数：0x00506CF0。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons | LegacyMouseButton_Right;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | (LegacyMouseEdge_RDown | LegacyMouseEdge_RDblClick);
    gLegacyMouseState.pulseButtons = pulseButtons | LegacyMouseButton_Right;

    gLegacyMouseState.buttonCoordinates[4] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    gLegacyMouseState.rightDownTick = tick;
    gLegacyMouseState.rightElapsed = 0;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00506DB0。
std::int32_t AddLegacyMouseWheelDelta_506DB0(const std::int32_t delta) noexcept
{
    gLegacyMouseState.tailReset += static_cast<std::uint32_t>(delta);
    return delta;
}

} // namespace cg::platform
