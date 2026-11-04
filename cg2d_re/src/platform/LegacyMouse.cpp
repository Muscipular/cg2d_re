#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"

#include <cstring>

namespace cg::platform {

std::uint32_t ReadSessionMouseHeldButtons(session::ClientSession& session) noexcept
{
    return IsSessionInputTarget(session) ? session.UiFrame().mouse.state->heldButtons : 0;
}

std::uint32_t ReadSessionMouseEdgeButtons(session::ClientSession& session) noexcept
{
    return IsSessionInputTarget(session) ? session.UiFrame().mouse.state->edgeButtons : 0;
}

std::uint32_t ReadSessionMousePulseButtons(session::ClientSession& session) noexcept
{
    return IsSessionInputTarget(session) ? session.UiFrame().mouse.state->pulseButtons : 0;
}

std::uint32_t ReadSessionMouseWheelDelta(session::ClientSession& session) noexcept
{
    return IsSessionInputTarget(session) ? session.UiFrame().mouse.state->tailReset : 0;
}

void InitializeLegacyMouseStorage(session::ClientSession& session)
{
    auto& state = session.UiFrame().mouse.state;
    if (!state) {
        state = std::make_unique<LegacyMouseState>();
    }
}

// 原函数：0x00506870。
std::int32_t ResetLegacyMouseState_506870(session::ClientSession& session)
{
    auto& state = *session.UiFrame().mouse.state;
    const LegacyCryptInt::RawPayload zero{};
    state.mouseX.RestoreRaw(zero);
    state.mouseY.RestoreRaw(zero);
    for (LegacyCryptInt& value : state.buttonCoordinates) {
        value.RestoreRaw(zero);
    }
    // 原整块零字节：普通字段直接清零，不重构十成员或消费RNG。
    constexpr std::size_t tail = offsetof(LegacyMouseState, heldButtons);
    std::memset(reinterpret_cast<std::byte*>(&state) + tail, 0, sizeof(state) - tail);
    session.UiFrame().mouse.requestedCursor = 0;
    state.valueD68DB8 = -1;
    return 0;
}

// 原函数：0x00506890。
void ResetLegacyMousePosition_506890(session::ClientSession& session, const std::int32_t x, const std::int32_t y)
{
    session.UiFrame().mouse.state->mouseX = x;

    session.UiFrame().mouse.state->mouseY = y;
}

// 原函数：0x00506900。
void UpdateLegacyMouseLeftDown_506900(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | LegacyMouseEdge_LDown;
    session.UiFrame().mouse.state->heldButtons = heldButtons | LegacyMouseButton_Left;
    session.UiFrame().mouse.state->pulseButtons = pulseButtons | LegacyMouseButton_Left;

    session.UiFrame().mouse.state->buttonCoordinates[0] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    session.UiFrame().mouse.state->leftDownTick = tick;
    session.UiFrame().mouse.state->leftElapsed = 0;
}

// 原函数：0x005069E0。
std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    session.UiFrame().mouse.state->heldButtons = heldButtons & ~LegacyMouseButton_Left;
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | LegacyMouseEdge_LUp;
    session.UiFrame().mouse.state->pulseButtons = pulseButtons & ~LegacyMouseButton_Left;

    session.UiFrame().mouse.state->buttonCoordinates[2] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[3] = static_cast<std::int32_t>(y);

    session.UiFrame().mouse.state->leftDownTick = 0;
    session.UiFrame().mouse.state->leftElapsed = 0;
    return 0;
}

// 原函数：0x00506A90。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    session.UiFrame().mouse.state->heldButtons = heldButtons | LegacyMouseButton_Left;
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | (LegacyMouseEdge_LDown | LegacyMouseEdge_LDblClick);
    session.UiFrame().mouse.state->pulseButtons = pulseButtons | LegacyMouseButton_Left;

    session.UiFrame().mouse.state->buttonCoordinates[0] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    session.UiFrame().mouse.state->leftDownTick = tick;
    session.UiFrame().mouse.state->leftElapsed = 0;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00506B50。
void UpdateLegacyMouseRightDown_506B50(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    session.UiFrame().mouse.state->heldButtons = heldButtons | LegacyMouseButton_Right;
    session.UiFrame().mouse.state->pulseButtons = pulseButtons | LegacyMouseButton_Right;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | LegacyMouseEdge_RDown;

    session.UiFrame().mouse.state->buttonCoordinates[4] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    session.UiFrame().mouse.state->rightDownTick = tick;
    session.UiFrame().mouse.state->rightElapsed = 0;
}

// 原函数：0x00506C40。
std::int32_t UpdateLegacyMouseRightUp_506C40(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    session.UiFrame().mouse.state->heldButtons = heldButtons & ~LegacyMouseButton_Right;
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | LegacyMouseEdge_RUp;
    session.UiFrame().mouse.state->pulseButtons = pulseButtons & ~LegacyMouseButton_Right;

    session.UiFrame().mouse.state->buttonCoordinates[6] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[7] = static_cast<std::int32_t>(y);

    session.UiFrame().mouse.state->rightDownTick = 0;
    session.UiFrame().mouse.state->rightElapsed = 0;
    return 0;
}

// 原函数：0x00506CF0。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    session::ClientSession& session,
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = session.UiFrame().mouse.state->heldButtons;
    const std::uint32_t edgeButtons = session.UiFrame().mouse.state->edgeButtons;
    session.UiFrame().mouse.state->heldButtons = heldButtons | LegacyMouseButton_Right;
    const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
    session.UiFrame().mouse.state->edgeButtons = edgeButtons | (LegacyMouseEdge_RDown | LegacyMouseEdge_RDblClick);
    session.UiFrame().mouse.state->pulseButtons = pulseButtons | LegacyMouseButton_Right;

    session.UiFrame().mouse.state->buttonCoordinates[4] = static_cast<std::int32_t>(x);

    session.UiFrame().mouse.state->buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint64_t tick = ReadTickCount64();
    session.UiFrame().mouse.state->rightDownTick = tick;
    session.UiFrame().mouse.state->rightElapsed = 0;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00506DB0。
std::int32_t AddLegacyMouseWheelDelta_506DB0(session::ClientSession& session, const std::int32_t delta) noexcept
{
    session.UiFrame().mouse.state->tailReset += static_cast<std::uint32_t>(delta);
    return delta;
}

} // namespace cg::platform
