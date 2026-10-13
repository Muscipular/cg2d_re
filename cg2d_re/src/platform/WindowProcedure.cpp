#include "StartupNative.hpp"

#include "ImeComposition.hpp"
#include "MainLoopDependencies.hpp"
#include "WindowProcedureDependencies.hpp"
#include "WindowProcedureState.hpp"

#include "cg/Input.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <imm.h>

#include <bit>
#include <cstdlib>

namespace cg::platform {

std::uint8_t gKeyStates[LegacyKeyStateCount] {}; // 原全局：0x0073EFF8。

std::uint32_t gCurrentClientWidth = 0; // 原全局：0x009A6F2C。
std::uint32_t gCurrentClientHeight = 0; // 原全局：0x009A6F30。
std::uint32_t gWindowResizePhase = 0; // 原全局：0x009A7F68。

char gImeDescription[100] {}; // 原全局：0x0073ED70。
std::uintptr_t gImeContext = 0; // 原全局：0x0073EDD8。
std::uintptr_t gPreviousImeContext = 0; // 原全局：0x0073EDDC。
char gImeComposition[256] {}; // 原全局：0x0073EDE0。
std::uint8_t gImeAttributes[256] {}; // 原全局：0x0073EEE0。
std::int32_t gImeCompositionCursorPosition = 0;
std::uint32_t gImeConversionMode = 0; // 原全局：0x0073EFE4。
std::uint32_t gImeSentenceMode = 0; // 原全局：0x0073EFE8。
void* gImeCandidateList = nullptr;
std::uint8_t gImeState73EFF0 = 0; // 原全局：0x0073EFF0。

// 原函数：0x00459760。
int ClearKeyFlags_459760() noexcept
{
    for (std::size_t index = 0; index < LegacyKeyStateCount; ++index) {
        gKeyStates[index] &= 0xEAu;
    }
    return static_cast<int>(LegacyKeyStateCount);
}

// 原函数：0x00459780。
int ResetKeyState_459780() noexcept
{
    for (std::size_t index = 0; index < LegacyKeyStateCount; ++index) {
        gKeyStates[index] &= 0xE8u;
    }
    return static_cast<int>(LegacyKeyStateCount);
}

// 原函数：0x004597B0。
int ReadKeyPressFlag_4597B0(const std::int32_t key) noexcept
{
    return static_cast<int>(gKeyStates[key] & 0x10u);
}

// 原函数：0x004597A0。
int ReadKeyFlag4_4597A0(const std::int32_t key) noexcept
{
    return static_cast<int>(gKeyStates[key] & 4u);
}

// 原函数：0x004597C0。
int ReadControlKeyPressFlag_4597C0(const std::int32_t key) noexcept
{
    if ((gKeyStates[17] & 2u) == 0) {
        return 0;
    }
    return (gKeyStates[key] & 0x10u) != 0 ? 1 : 0;
}

namespace {

inline constexpr UINT kResizeWindowFlags = 0x26u;
inline constexpr char kCloseQuestion[] =
    "\xBF\xC9\xD2\xD4\xBD\x59\xCA\xF8\xC4\xA7"
    "\xC1\xA6\x8C\x9A\xD8\x90\x86\xE1\xA3\xBF";
inline constexpr char kConfirmCaption[] = "\xB4\x5F\xD5\x4A";

struct MouseCoordinates {
    std::int32_t x;
    std::int32_t y;
};

void RebuildRendererIfGraphicDirty()
{
    if (render::gGraphicDirty == 1) {
        render::gGraphicDirty = 0;
        render::RebuildRenderer_466B80();
    }
}

void UpdateKeyDownState(const std::uint32_t key)
{
    const std::uint8_t pressed = static_cast<std::uint8_t>(
        gKeyStates[key] | 2u);
    gKeyStates[key] = pressed;
    if ((pressed & 8u) == 0) {
        gKeyStates[key] = static_cast<std::uint8_t>(pressed | 0x0Cu);
    }
    gKeyStates[key] = static_cast<std::uint8_t>(
        gKeyStates[key] | 0x10u);
}

void UpdateKeyUpState(const std::uint32_t key)
{
    gKeyStates[key] = static_cast<std::uint8_t>(
        (gKeyStates[key] & 0xF5u) | 1u);
}

MouseCoordinates MapMouseCoordinates(const std::uint32_t packedCoordinates)
{
    const std::int32_t x = static_cast<std::uint16_t>(packedCoordinates);
    const std::int32_t y = static_cast<std::uint16_t>(packedCoordinates >> 16);
    if (gCurrentClientWidth == 640u && gCurrentClientHeight == 480u) {
        return MouseCoordinates {x, y};
    }

    // 原非 640×480 路径先换算 Y、后换算 X，分别读取当前客户区尺寸。
    const std::int32_t mappedY = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(y) * 480u) / gCurrentClientHeight);
    const std::int32_t mappedX = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(x) * 640u) / gCurrentClientWidth);
    return MouseCoordinates {mappedX, mappedY};
}

} // namespace

// 原 WIN_PROC@[0x466DC0,0x467904)。消息载荷先截取原 x86 低 32 位，
// 默认分支仍把本机原生参数原样交还 DefWindowProcA。
LRESULT CALLBACK WindowProcedure(
    HWND window,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    const std::uint32_t wParam32 = static_cast<std::uint32_t>(wParam);
    const std::uint32_t lParam32 = static_cast<std::uint32_t>(lParam);

    switch (message) {
    case WM_ACTIVATE: {
        static_cast<void>(ResetKeyState_459780());
        const bool activating = wParam32 == WA_ACTIVE || wParam32 == WA_CLICKACTIVE;
        if (activating && render::gRenderer != nullptr) {
            if (render::IsLost_4536D0() == 1) {
                gRendererLost = 1;
            }
            render::PaletteBinding* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr && gWindowMode != 0) {
                static_cast<void>(
                    render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette));
            }
        }
        if ((gRendererFlags & 0x10u) != 0u) {
            gWindowActiveState = 0;
            if (activating) {
                gWindowActiveState = 1;
            }
        }
        const std::uint32_t tick = ReadTickCount32();
        gLastRenderDt = static_cast<double>(tick);
        return 0;
    }

    case WM_SIZE: {
        gWindowActiveState = 2;
        if (wParam32 == SIZE_MINIMIZED) {
            gWindowActiveState = 0;
        }
        const std::uint32_t tick = ReadTickCount32();
        gLastRenderDt = static_cast<double>(tick);

        gCurrentClientWidth = static_cast<std::uint16_t>(lParam32);
        gCurrentClientHeight = static_cast<std::uint16_t>(lParam32 >> 16);
        if (gWindowMode == 0) {
            return 0;
        }
        if (gCurrentClientWidth == static_cast<std::uint32_t>(gWindowWidth)
            && gCurrentClientHeight == static_cast<std::uint32_t>(gWindowHeight)) {
            gWindowResizePhase = 0;
            return 0;
        }

        if (gWindowResizePhase == 0u) {
            gWindowResizePhase = 1;
            RECT rect {};
            GetWindowRect(window, &rect);
            const std::int32_t outerWidth =
                rect.right + gWindowWidth
                - static_cast<std::int32_t>(gCurrentClientWidth) - rect.left;
            const std::int32_t outerHeight =
                rect.bottom + gWindowHeight
                - static_cast<std::int32_t>(gCurrentClientHeight) - rect.top;
            SetWindowPos(
                window,
                HWND_NOTOPMOST,
                0,
                0,
                outerWidth,
                outerHeight,
                kResizeWindowFlags);
            return 0;
        }
        if (gWindowResizePhase != 1u) {
            return 0;
        }

        gWindowResizePhase = 2;
        const LONG style = GetWindowLongA(window, GWL_STYLE);
        RECT rect {};
        if (gWindowMode != 0) {
            SetRect(&rect, 0, 0, gWindowWidth, gWindowHeight);
        } else {
            SetRect(&rect, 0, 0, 640, 480);
        }
        AdjustWindowRectEx(&rect, static_cast<DWORD>(style), FALSE, 0);
        SetWindowPos(
            window,
            HWND_NOTOPMOST,
            0,
            0,
            rect.right - rect.left,
            rect.bottom - rect.top,
            kResizeWindowFlags);
        return 0;
    }

    case WM_CLOSE:
        DestroyWindow(window);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SETCURSOR:
        if (static_cast<std::uint16_t>(lParam32) == HTCLIENT) {
            SetCursor(reinterpret_cast<HCURSOR>(gCursorHandles[gValue9A6F44]));
            return 1;
        }
        return DefWindowProcA(window, message, wParam, lParam);

    case WM_NCMOUSEMOVE: {
        const std::uint32_t oldSuppression = gLegacyMouseState.drawingSuppression;
        gLegacyMouseState.heldButtons = 0;
        gLegacyMouseState.edgeButtons = 0;
        if (oldSuppression == 0u) {
            ShowCursor(TRUE);
            gLegacyMouseState.drawingSuppression = 1;
        }
        return 0;
    }

    case WM_KEYDOWN:
        RebuildRendererIfGraphicDirty();
        if (gWindowActiveState == 2) {
            UpdateKeyDownState(wParam32);
        }
        return 0;

    case WM_KEYUP:
        RebuildRendererIfGraphicDirty();
        UpdateKeyUpState(wParam32);
        return 0;

    case WM_SYSKEYDOWN:
        RebuildRendererIfGraphicDirty();
        if (wParam32 == VK_RETURN) {
            if (gValue73ECC0 == 2 || render::gRenderer == nullptr) {
                return 0;
            }
            gWindowMode = gWindowMode != 1;
            render::RebuildRenderer_466B80();
            static_cast<void>(InitializeWindow());
            const std::uint32_t tick = ReadTickCount32();
            gWindowResizePhase = 0;
            gLastRenderDt = static_cast<double>(tick);
            return 0;
        }
        if (wParam32 == VK_F10) {
            // 原 4671C3..4671D4 仅在按位更新完成后写一次键状态。
            std::uint8_t pressed = static_cast<std::uint8_t>(gKeyStates[VK_F10] | 2u);
            if ((pressed & 8u) == 0) {
                pressed = static_cast<std::uint8_t>(pressed | 0x0Cu);
            }
            gKeyStates[VK_F10] = static_cast<std::uint8_t>(pressed | 0x10u);
            return 0;
        }
        return DefWindowProcA(window, message, wParam, lParam);

    case WM_CHAR:
        if (static_cast<std::uint8_t>(wParam32) == '\r') {
            return 0;
        }
        if (GetAsyncKeyState(VK_MENU) != 0) {
            return 0;
        }
        input::AppendChar_44ADB0(
            static_cast<char>(static_cast<std::uint8_t>(wParam32)));
        return 0;

    case WM_IME_STARTCOMPOSITION:
        return 0;

    case WM_IME_ENDCOMPOSITION:
        gImeComposition[0] = '\0';
        return 0;

    case WM_IME_COMPOSITION:
        static_cast<void>(Sub_459580(std::bit_cast<std::int16_t>(
            static_cast<std::uint16_t>(lParam32))));
        return 0;

    case WM_SYSCOMMAND:
        if (wParam32 == SC_CLOSE && gWindowMode == 1
            && MessageBoxA(
                window,
                kCloseQuestion,
                kConfirmCaption,
                MB_OKCANCEL) == IDCANCEL) {
            const std::uint32_t tick = ReadTickCount32();
            gLastRenderDt = static_cast<double>(tick);
            return 0;
        }
        return DefWindowProcA(window, message, wParam, lParam);

    case WM_MOUSEMOVE: {
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        ResetLegacyMousePosition_506890(coordinates.x, coordinates.y);
        if (gLegacyMouseState.drawingSuppression == 1u) {
            ShowCursor(FALSE);
            gLegacyMouseState.drawingSuppression = 0;
        }
        RebuildRendererIfGraphicDirty();
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseLeftDown_506900(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_LBUTTONUP: {
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseLeftUp_5069E0(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseLeftDoubleClick_506A90(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_RBUTTONDOWN: {
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseRightDown_506B50(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_RBUTTONUP: {
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseRightUp_506C40(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_RBUTTONDBLCLK: {
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseRightDoubleClick_506CF0(coordinates.x, coordinates.y));
        return 0;
    }

    case WM_MBUTTONDOWN:
        return 0;

    case WM_MOUSEWHEEL:
        if (gWindowActiveState == 2) {
            const std::int16_t wheel = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(wParam32 >> 16));
            static_cast<void>(AddLegacyMouseWheelDelta_506DB0(
                -static_cast<std::int32_t>(wheel)));
        }
        return 0;

    case WM_IME_NOTIFY: {
        HIMC context = reinterpret_cast<HIMC>(gImeContext);
        if (context != nullptr) {
            static_assert(sizeof(DWORD) == sizeof(std::uint32_t));
            ImmGetConversionStatus(
                context,
                reinterpret_cast<DWORD*>(&gImeConversionMode),
                reinterpret_cast<DWORD*>(&gImeSentenceMode));
            // 原467809：外部调用后重取共享上下文，不沿用调用前快照。
            context = reinterpret_cast<HIMC>(gImeContext);
        }

        if (wParam32 == IMN_OPENCANDIDATE
            || wParam32 == IMN_CHANGECANDIDATE) {
            const DWORD requiredSize =
                ImmGetCandidateListA(context, 0, nullptr, 0);
            if (gImeCandidateList != nullptr) {
                std::free(gImeCandidateList);
            }
            if (requiredSize != 0) {
                void* const candidates = std::calloc(1, requiredSize);
                if (wParam32 == IMN_CHANGECANDIDATE) {
                    // 原4678B5/4678BE：变化通知先重取上下文，再发布候选表。
                    context = reinterpret_cast<HIMC>(gImeContext);
                    gImeCandidateList = candidates;
                } else {
                    // 原467849/467850：打开通知先发布候选表，再重取上下文。
                    gImeCandidateList = candidates;
                    context = reinterpret_cast<HIMC>(gImeContext);
                }
                ImmGetCandidateListA(
                    context,
                    0,
                    reinterpret_cast<LPCANDIDATELIST>(candidates),
                    requiredSize);
                return 0;
            }
            gImeCandidateList = nullptr;
            return 0;
        }
        if (wParam32 == IMN_CLOSECANDIDATE) {
            if (gImeCandidateList != nullptr) {
                std::free(gImeCandidateList);
            }
            gImeCandidateList = nullptr;
        }
        return 0;
    }

    case WM_IME_SETCONTEXT: {
        const std::uint32_t filtered = lParam32 & 0x7FFFFFF0u;
        return DefWindowProcA(
            window,
            message,
            wParam,
            static_cast<LPARAM>(filtered));
    }

    default:
        return DefWindowProcA(window, message, wParam, lParam);
    }
}

} // namespace cg::platform
