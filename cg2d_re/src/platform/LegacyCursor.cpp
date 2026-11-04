#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/platform/LegacyCursor.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"

#include <Windows.h>

#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::platform {

static inline constexpr std::int32_t kSystemCursorImage = 0x000055F0;
static inline constexpr std::uint8_t kCursorLayer = 0xE7;

struct CursorState0Record {
    std::uint32_t imageBits;
    std::uint32_t packedOffsets;
};
static_assert(sizeof(CursorState0Record) == 8);

// 原 [0x55CE80,0x55CE88)：状态 0 的唯一可写记录。
static CursorState0Record gCursorState0 {0x0003BE98u, 0};


static std::int32_t ReadCursorImage(const std::uint32_t state)
{
    if (state == 0) {
        return std::bit_cast<std::int32_t>(gCursorState0.imageBits);
    }
    return std::bit_cast<std::int32_t>(
        resource::gCursorPathStorage.cursor[state - 1].value04);
}

static std::int16_t ReadCursorOffsetX(const std::uint32_t state)
{
    const std::uint32_t packed = state == 0
        ? gCursorState0.packedOffsets
        : resource::gCursorPathStorage.cursor[state - 1].value08;
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(packed));
}

static std::int16_t ReadCursorOffsetY(const std::uint32_t state)
{
    const std::uint32_t packed = state == 0
        ? gCursorState0.packedOffsets
        : resource::gCursorPathStorage.cursor[state - 1].value08;
    return std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(packed >> 16));
}

static void UpdatePulse(
    session::ClientSession& session,
    const std::uint64_t elapsed,
    const std::uint32_t bit,
    std::uint64_t& anchor)
{
    if (elapsed <= 500) {
        const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
        anchor = 0;
        session.UiFrame().mouse.state->pulseButtons = pulseButtons & ~bit;
        return;
    }

    const std::uint64_t currentAnchor = anchor;
    if (currentAnchor == 0) {
        const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
        anchor = elapsed;
        session.UiFrame().mouse.state->pulseButtons = pulseButtons | bit;
        return;
    }

    const std::uint64_t repeatElapsed = elapsed - currentAnchor;
    if (bit == LegacyMouseButton_Right) {
        // 原右侧 506EE5：先保存 pulse，再判断重复间隔并发布锚点。
        const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
        if (repeatElapsed >= 100) {
            anchor = elapsed;
            session.UiFrame().mouse.state->pulseButtons = pulseButtons | bit;
        } else {
            session.UiFrame().mouse.state->pulseButtons = pulseButtons & ~bit;
        }
        return;
    }

    if (repeatElapsed >= 100) {
        const std::uint32_t pulseButtons = session.UiFrame().mouse.state->pulseButtons;
        anchor = elapsed;
        session.UiFrame().mouse.state->pulseButtons = pulseButtons | bit;
        return;
    }

    session.UiFrame().mouse.state->pulseButtons &= ~bit;
}

static cg::render::GraphicDescriptor16 MakeCursorDescriptor()
{
    cg::render::GraphicDescriptor16 descriptor {};
    descriptor.color = 0xFFFFFFFFu;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    return descriptor;
}

static void DrawTableCursor(session::ClientSession& session)
{
    const cg::render::GraphicDescriptor16 descriptor =
        MakeCursorDescriptor();

    const std::uint32_t imageState = session.UiFrame().mouse.requestedCursor;
    const std::int32_t image = ReadCursorImage(imageState);
    const std::int32_t mouseY = (session.UiFrame().mouse.state->mouseY + 16);
    const std::uint32_t yState = session.UiFrame().mouse.requestedCursor;
    const std::int32_t y = mouseY + ReadCursorOffsetY(yState);
    const std::int32_t mouseX = (session.UiFrame().mouse.state->mouseX + 16);
    const std::uint32_t xState = session.UiFrame().mouse.requestedCursor;
    const std::int32_t x = mouseX + ReadCursorOffsetX(xState);

    cg::render::DrawGraphic(session,
        x, y, kCursorLayer, image, 0, &descriptor);
}

static void UpdateNativeCursor(session::ClientSession& session)
{
    // 原全局：0x00D68BE8；本函数保存上一次光标状态。
    static std::uint32_t previousCursorState = 0;
    const std::uint32_t currentState = session.UiFrame().mouse.requestedCursor;
    const bool acceptsInput = IsSessionInputTarget(session);
    if (acceptsInput && currentState != previousCursorState) {
        // 原端未检查 API 结果且会继续读取未初始化局部；新端只将局部清零以避免 UB。
        POINT point {};
        RECT clientRect {};
        GetCursorPos(&point);
        ScreenToClient(reinterpret_cast<HWND>(gLegacyWindow), &point);
        GetClientRect(reinterpret_cast<HWND>(gLegacyWindow), &clientRect);

        if (point.x >= 0 && point.x < clientRect.right
            && point.y >= 0 && point.y < clientRect.bottom) {
            const std::uint32_t handleState = session.UiFrame().mouse.requestedCursor;
            SetCursor(reinterpret_cast<HCURSOR>(gCursorHandles[handleState]));
        }
    }

    if (acceptsInput) previousCursorState = session.UiFrame().mouse.requestedCursor;

    if (session.UiFrame().mouse.state->drawingSuppression == 0) {
        const std::int32_t y = (session.UiFrame().mouse.state->mouseY + 16);
        const std::int32_t x = (session.UiFrame().mouse.state->mouseX + 16);
        cg::render::DrawGraphic(session,
            x, y, kCursorLayer, kSystemCursorImage, 0, nullptr);
    }
}

// 原函数：0x00506DE0。
std::int32_t RequestCursorState_506DE0(
    session::ClientSession& session,
    const std::int32_t requestedState,
    const std::int32_t force) noexcept
{
    if (force != 0) {
        session.UiFrame().mouse.requestedCursor = std::bit_cast<std::uint32_t>(requestedState);
        return requestedState;
    }

    const std::uint32_t currentState = session.UiFrame().mouse.requestedCursor;
    if (currentState != 0) {
        return std::bit_cast<std::int32_t>(currentState);
    }

    session.UiFrame().mouse.requestedCursor = std::bit_cast<std::uint32_t>(requestedState);
    return requestedState;
}

// 原函数：0x00506DD0。
void ResetLegacyMouseTail_506DD0(session::ClientSession& session)
{
    session.UiFrame().mouse.state->tailReset = 0;
}

// 原函数：0x00506E00。
void UpdateCursor_506E00(session::ClientSession& session)
{
    const std::uint64_t leftDownTick = session.UiFrame().mouse.state->leftDownTick;
    session.UiFrame().mouse.state->edgeButtons = LegacyMouseEdge_None;

    std::uint64_t leftElapsed;
    if (leftDownTick != 0) {
        const std::uint64_t now = ReadTickCount64();
        leftElapsed = now - session.UiFrame().mouse.state->leftDownTick;
        session.UiFrame().mouse.state->leftElapsed = leftElapsed;
    } else {
        leftElapsed = session.UiFrame().mouse.state->leftElapsed;
    }

    std::uint64_t rightElapsed;
    if (session.UiFrame().mouse.state->rightDownTick != 0) {
        const std::uint64_t now = ReadTickCount64();
        rightElapsed = now - session.UiFrame().mouse.state->rightDownTick;
        // 原 506E43 在发布 rightElapsed 前重新保存左侧样本。
        leftElapsed = session.UiFrame().mouse.state->leftElapsed;
        session.UiFrame().mouse.state->rightElapsed = rightElapsed;
    } else {
        rightElapsed = session.UiFrame().mouse.state->rightElapsed;
    }

    UpdatePulse(session, leftElapsed, LegacyMouseButton_Left, session.UiFrame().mouse.leftRepeatAnchorMs);
    UpdatePulse(session, rightElapsed, LegacyMouseButton_Right, session.UiFrame().mouse.rightRepeatAnchorMs);

    if (gNativeCursorEnabled_D67AE4 == 0) {
        if (session.UiFrame().mouse.state->drawingSuppression == 0) {
            DrawTableCursor(session);
        }
    } else {
        UpdateNativeCursor(session);
    }

    const std::uint32_t consumedState = session.UiFrame().mouse.requestedCursor;
    session.UiFrame().mouse.requestedCursor = 0;
    session.UiFrame().mouse.consumedCursor = consumedState;
    ResetLegacyMouseTail_506DD0(session);
}

} // namespace cg::platform
