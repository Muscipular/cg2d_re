#include "cg/session/ClientSession.hpp"
#include "StartupNative.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "cg/text/Encoding.hpp"

#include "imeImpl.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/session/ClientHost.hpp"
#include "WindowProcedureState.hpp"
#include "SessionWindow.hpp"

#include "cg/input/Input.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <string>

namespace cg::platform {

std::uint32_t gCurrentClientWidth = 0; // 原全局：0x009A6F2C。
std::uint32_t gCurrentClientHeight = 0; // 原全局：0x009A6F30。
std::uint32_t gWindowResizePhase = 0; // 原全局：0x009A7F68。

// 唯一物理输入路由；稳定ID不拥有会话，关闭后的抬起不能写入新当前项。
static std::array<std::uint64_t, LegacyKeyStateCount> gKeyPressOwners {};
static std::array<std::uint64_t, 2> gMousePressOwners {}; // 左、右真实按下归属。

bool RouteSessionKeyDown(const std::uint32_t key, const std::uint32_t keyMessageBits) noexcept
{
    const auto currentId = session::GetClientHost().CurrentId();
    auto& owner = gKeyPressOwners[key];
    if ((keyMessageBits & 0x40000000u) == 0 || owner == 0) {
        owner = currentId;
    }
    return currentId != 0 && owner == currentId;
}

static session::ClientSession* ConsumePhysicalPressOwner(
    std::uint64_t& owner, const bool useCurrentWhenUnbound) noexcept
{
    const std::uint64_t id = owner;
    owner = 0;
    auto& host = session::GetClientHost();
    return id != 0 ? host.FindSession(id)
        : useCurrentWhenUnbound ? host.CurrentSession() : nullptr;
}

// 原函数：0x00459760。
int ClearKeyFlags_459760(session::ClientSession& session) noexcept
{
    for (std::size_t index = 0; index < LegacyKeyStateCount; ++index) {
        session.UiFrame().keyStates[index] &= 0xEAu;
    }
    return static_cast<int>(LegacyKeyStateCount);
}

// 原函数：0x00459780。
int ResetKeyState_459780(session::ClientSession& session) noexcept
{
    for (std::size_t index = 0; index < LegacyKeyStateCount; ++index) {
        session.UiFrame().keyStates[index] &= 0xE8u;
    }
    return static_cast<int>(LegacyKeyStateCount);
}

bool IsSessionInputTarget(const session::ClientSession& session) noexcept
{
    return session::GetClientHost().CurrentSession() == &session && IsSessionWindowInputAvailable();
}

std::uint8_t ReadSessionKeyState(session::ClientSession& session, const std::int32_t key) noexcept
{
    if (!IsSessionInputTarget(session)) return 0;
    return session.UiFrame().keyStates[key];
}

// 原函数：0x004597B0。
int ReadKeyPressFlag_4597B0(session::ClientSession& session, const std::int32_t key) noexcept
{
    return static_cast<int>(ReadSessionKeyState(session, key) & 0x10u);
}

// 原函数：0x004597A0。
int ReadKeyFlag4_4597A0(session::ClientSession& session, const std::int32_t key) noexcept
{
    return static_cast<int>(ReadSessionKeyState(session, key) & 4u);
}

// CTM01/A07：复制后释放剪贴板，再交给既有输入上下文按完整Unicode字符插入。
static void PasteClipboard(session::ClientSession& session, HWND window)
{
    if (::OpenClipboard(window) == FALSE) {
        return;
    }
    std::wstring contents;
    {
        struct ClipboardScope {
            HANDLE data = nullptr;
            bool locked = false;
            ~ClipboardScope()
            {
                if (locked) {
                    static_cast<void>(::GlobalUnlock(data));
                }
                static_cast<void>(::CloseClipboard());
            }
        } clipboard;
        const UINT format = ::IsClipboardFormatAvailable(CF_UNICODETEXT)
            ? CF_UNICODETEXT : CF_TEXT;
        clipboard.data = ::GetClipboardData(format);
        if (clipboard.data == nullptr) {
            return;
        }
        const void* const data = ::GlobalLock(clipboard.data);
        if (data == nullptr) {
            return;
        }
        clipboard.locked = true;
        if (format == CF_UNICODETEXT) {
            contents = static_cast<const wchar_t*>(data);
        } else {
            contents = text::FromLegacyDisplayWide(static_cast<const char*>(data));
        }
    }
    input::AppendText(session, contents);
}

static inline constexpr UINT kResizeWindowFlags = 0x26u;
struct MouseCoordinates {
    std::int32_t x;
    std::int32_t y;
};

static void RebuildRendererIfGraphicDirty()
{
    if (render::gGraphicDirty == 1) {
        auto* const session = session::GetClientHost().CurrentSession();
        if (session == nullptr) return;
        render::gGraphicDirty = 0;
        render::RebuildRenderer_466B80(*session);
    }
}

static void UpdateKeyDownState(session::ClientSession& session, const std::uint32_t key)
{
    const std::uint8_t pressed = static_cast<std::uint8_t>(
        session.UiFrame().keyStates[key] | 2u);
    session.UiFrame().keyStates[key] = pressed;
    if ((pressed & 8u) == 0) {
        session.UiFrame().keyStates[key] = static_cast<std::uint8_t>(pressed | 0x0Cu);
    }
    session.UiFrame().keyStates[key] = static_cast<std::uint8_t>(
        session.UiFrame().keyStates[key] | 0x10u);
}

static void UpdateKeyUpState(session::ClientSession& session, const std::uint32_t key)
{
    session.UiFrame().keyStates[key] = static_cast<std::uint8_t>(
        (session.UiFrame().keyStates[key] & 0xF5u) | 1u);
}

void RouteSessionKeyRelease(const std::uint32_t key, const bool useCurrentWhenUnbound) noexcept
{
    if (auto* const target = ConsumePhysicalPressOwner(gKeyPressOwners[key], useCurrentWhenUnbound)) {
        UpdateKeyUpState(*target, key);
    }
}

static MouseCoordinates MapMouseCoordinates(const std::uint32_t packedCoordinates)
{
    const std::int32_t x = static_cast<std::uint16_t>(packedCoordinates);
    const std::int32_t y = static_cast<std::uint16_t>(packedCoordinates >> 16);
    if (gCurrentClientWidth == static_cast<std::uint32_t>(gWindowWidth)
        && gCurrentClientHeight == static_cast<std::uint32_t>(gWindowHeight)) {
        return MouseCoordinates {x, y};
    }

    // R02：尺寸不同时先换算 Y、后换算 X，分别读取当前客户区尺寸。
    const std::int32_t mappedY = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(y) * static_cast<std::uint32_t>(gWindowHeight)) / gCurrentClientHeight);
    const std::int32_t mappedX = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(x) * static_cast<std::uint32_t>(gWindowWidth)) / gCurrentClientWidth);
    return MouseCoordinates {mappedX, mappedY};
}

static MouseCoordinates MapNonClientMouseCoordinates(HWND window, const std::uint32_t packedCoordinates)
{
    POINT point {
        static_cast<std::int16_t>(static_cast<std::uint16_t>(packedCoordinates)),
        static_cast<std::int16_t>(static_cast<std::uint16_t>(packedCoordinates >> 16))
    };
    static_cast<void>(ScreenToClient(window, &point));
    return MapMouseCoordinates(static_cast<std::uint16_t>(point.x)
        | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(point.y)) << 16));
}

void RouteSessionMouseRelease(const bool leftButton, const LPARAM screenCoordinates)
{
    auto* const target = ConsumePhysicalPressOwner(gMousePressOwners[leftButton ? 0 : 1], false);
    if (target == nullptr) return;
    const MouseCoordinates coordinates = MapNonClientMouseCoordinates(
        reinterpret_cast<HWND>(gLegacyWindow), static_cast<std::uint32_t>(screenCoordinates));
    if (leftButton) {
        static_cast<void>(UpdateLegacyMouseLeftUp_5069E0(*target, coordinates.x, coordinates.y));
    } else {
        static_cast<void>(UpdateLegacyMouseRightUp_506C40(*target, coordinates.x, coordinates.y));
    }
}

// 原 WIN_PROC@[0x466DC0,0x467904)。消息载荷先截取原 x86 低 32 位，
// 默认分支仍把本机原生参数原样交还 DefWindowProcW。
LRESULT CALLBACK WindowProcedure(
    HWND window,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    LRESULT frameResult = 0;
    if (HandleSessionWindowMessage(window, message, wParam, lParam, frameResult)) {
        return frameResult;
    }
    const std::uint32_t wParam32 = static_cast<std::uint32_t>(wParam);
    const std::uint32_t lParam32 = static_cast<std::uint32_t>(lParam);
    auto* const inputSession = session::GetClientHost().CurrentSession();

    switch (message) {
    case WM_ACTIVATE: {
        if (inputSession != nullptr) {
            static_cast<void>(ResetKeyState_459780(*inputSession));
        }
        const bool activating = wParam32 == WA_ACTIVE || wParam32 == WA_CLICKACTIVE;
        if (activating && render::gRenderer != nullptr) {
            if (render::IsLost_4536D0() == 1) {
                gRendererLost = 1;
            }
            render::PaletteBinding* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr) {
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
        const std::uint64_t tick = ReadTickCount64();
        gLastRenderDt = static_cast<double>(tick);
        return 0;
    }

    case WM_SIZE: {
        gWindowActiveState = 2;
        if (wParam32 == SIZE_MINIMIZED) {
            gWindowActiveState = 0;
        }
        const std::uint64_t tick = ReadTickCount64();
        gLastRenderDt = static_cast<double>(tick);

        gCurrentClientWidth = static_cast<std::uint16_t>(lParam32);
        gCurrentClientHeight = static_cast<std::uint16_t>(lParam32 >> 16);
        if (wParam32 == SIZE_MINIMIZED) {
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
        RECT rect {};
        SetRect(&rect, 0, 0, gWindowWidth, gWindowHeight);
        AdjustSessionWindowRect(rect, GetDpiForWindow(window));
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
        if (inputSession != nullptr && static_cast<std::uint16_t>(lParam32) == HTCLIENT) {
            SetCursor(reinterpret_cast<HCURSOR>(gCursorHandles[inputSession->UiFrame().mouse.consumedCursor]));
            return 1;
        }
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_NCMOUSEMOVE: {
        if (inputSession == nullptr) return 0;
        UpdateSystemCursorLocation(false);
        inputSession->UiFrame().mouse.state->drawingSuppression = 1;
        return 0;
    }

    case WM_KEYDOWN:
        if (inputSession == nullptr) return 0;
        if (!RouteSessionKeyDown(wParam32, lParam32)) return 0;
        if ((wParam32 == 'V' && (::GetKeyState(VK_CONTROL) & 0x8000) != 0)
            || (wParam32 == VK_INSERT && (::GetKeyState(VK_SHIFT) & 0x8000) != 0)) {
            if (auto* const currentSession = session::GetClientHost().CurrentSession()) {
                PasteClipboard(*currentSession, window);
            }
            return 0;
        }
        RebuildRendererIfGraphicDirty();
        if (gWindowActiveState == 2) {
            UpdateKeyDownState(*inputSession, wParam32);
        }
        return 0;

    case WM_KEYUP: {
        auto* const target = ConsumePhysicalPressOwner(gKeyPressOwners[wParam32], true);
        if (target == nullptr) return 0;
        RebuildRendererIfGraphicDirty();
        UpdateKeyUpState(*target, wParam32);
        return 0;
    }

    case WM_PASTE:
        if (auto* const currentSession = session::GetClientHost().CurrentSession()) {
            PasteClipboard(*currentSession, window);
        }
        return 0;

    case WM_SYSKEYDOWN:
        if (wParam32 == VK_RETURN) {
            // WO01：取消Alt+Enter切换，不重建窗口或呈现器。
            return 0;
        }
        RebuildRendererIfGraphicDirty();
        if (wParam32 == VK_F10) {
            if (inputSession == nullptr) return 0;
            if (!RouteSessionKeyDown(wParam32, lParam32)) return 0;
            // 原 4671C3..4671D4 仅在按位更新完成后写一次键状态。
            std::uint8_t pressed = static_cast<std::uint8_t>(inputSession->UiFrame().keyStates[VK_F10] | 2u);
            if ((pressed & 8u) == 0) {
                pressed = static_cast<std::uint8_t>(pressed | 0x0Cu);
            }
            inputSession->UiFrame().keyStates[VK_F10] = static_cast<std::uint8_t>(pressed | 0x10u);
            return 0;
        }
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_SYSKEYUP:
        if (wParam32 == VK_F10) {
            if (auto* const target = ConsumePhysicalPressOwner(gKeyPressOwners[wParam32], true)) {
                RebuildRendererIfGraphicDirty();
                UpdateKeyUpState(*target, wParam32);
            }
            return 0;
        }
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_CHAR: {
        auto* const currentSession = session::GetClientHost().CurrentSession();
        if (currentSession == nullptr) return 0;
        const wchar_t unit = static_cast<wchar_t>(wParam);
        if (unit == L'\r' || GetAsyncKeyState(VK_MENU) != 0) {
            return 0;
        }
        // W窗口的WM_CHAR按UTF-16码元发送；跨消息组合代理对，不能截成W8。
        auto& highSurrogate = currentSession->UiFrame().ime.highSurrogate;
        if (unit >= 0xD800 && unit <= 0xDBFF) {
            highSurrogate = unit;
            return 0;
        }
        const wchar_t units[2] {highSurrogate, unit};
        if (highSurrogate != 0 && unit >= 0xDC00 && unit <= 0xDFFF) {
            input::AppendText(*currentSession, std::wstring_view(units, 2));
        } else {
            input::AppendText(*currentSession, std::wstring_view(&unit, 1));
        }
        highSurrogate = 0;
        return 0;
    }

    case WM_IME_STARTCOMPOSITION:
        return 0;

    case WM_IME_ENDCOMPOSITION:
        if (auto* const imeSession = FindWindowImeSession()) {
            imeSession->UiFrame().ime.composition.clear();
        }
        return 0;

    case WM_IME_COMPOSITION:
        if (auto* const imeSession = FindWindowImeSession()) {
            static_cast<void>(HandleImeCompositionFlags_459580(*imeSession, std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(lParam32))));
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam32 & 0xFFF0u) == SC_CLOSE
            && MessageBoxW(
                window,
                L"可以结束魔力宝贝吗？",
                L"确认",
                MB_OKCANCEL) == IDCANCEL) {
            const std::uint64_t tick = ReadTickCount64();
            gLastRenderDt = static_cast<double>(tick);
            return 0;
        }
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_MOUSEMOVE: {
        if (inputSession == nullptr) return 0;
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        ResetLegacyMousePosition_506890(*inputSession, coordinates.x, coordinates.y);
        UpdateSystemCursorLocation(true);
        inputSession->UiFrame().mouse.state->drawingSuppression = 0;
        RebuildRendererIfGraphicDirty();
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (inputSession == nullptr) return 0;
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        gMousePressOwners[0] = session::GetClientHost().CurrentId();
        UpdateLegacyMouseLeftDown_506900(*inputSession, coordinates.x, coordinates.y);
        return 0;
    }

    case WM_LBUTTONUP: {
        auto* const target = ConsumePhysicalPressOwner(gMousePressOwners[0], true);
        if (target == nullptr) return 0;
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseLeftUp_5069E0(*target, coordinates.x, coordinates.y));
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        if (inputSession == nullptr) return 0;
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        gMousePressOwners[0] = session::GetClientHost().CurrentId();
        static_cast<void>(
            UpdateLegacyMouseLeftDoubleClick_506A90(*inputSession, coordinates.x, coordinates.y));
        return 0;
    }

    case WM_RBUTTONDOWN: {
        if (inputSession == nullptr) return 0;
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        gMousePressOwners[1] = session::GetClientHost().CurrentId();
        UpdateLegacyMouseRightDown_506B50(*inputSession, coordinates.x, coordinates.y);
        return 0;
    }

    case WM_RBUTTONUP: {
        auto* const target = ConsumePhysicalPressOwner(gMousePressOwners[1], true);
        if (target == nullptr) return 0;
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        static_cast<void>(
            UpdateLegacyMouseRightUp_506C40(*target, coordinates.x, coordinates.y));
        return 0;
    }

    case WM_RBUTTONDBLCLK: {
        if (inputSession == nullptr) return 0;
        if (gWindowActiveState != 2) {
            return 0;
        }
        const MouseCoordinates coordinates = MapMouseCoordinates(lParam32);
        gMousePressOwners[1] = session::GetClientHost().CurrentId();
        static_cast<void>(
            UpdateLegacyMouseRightDoubleClick_506CF0(*inputSession, coordinates.x, coordinates.y));
        return 0;
    }

    case WM_NCLBUTTONUP:
        RouteSessionMouseRelease(true, lParam);
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_NCRBUTTONUP:
        RouteSessionMouseRelease(false, lParam);
        return DefWindowProcW(window, message, wParam, lParam);

    case WM_MBUTTONDOWN:
        return 0;

    case WM_MOUSEWHEEL:
        if (inputSession != nullptr && gWindowActiveState == 2) {
            const std::int16_t wheel = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(wParam32 >> 16));
            script::AddLuaMouseWheel(*inputSession, wheel);
            static_cast<void>(AddLegacyMouseWheelDelta_506DB0(*inputSession,
                -static_cast<std::int32_t>(wheel)));
        }
        return 0;

    case WM_IME_NOTIFY: {
        auto* const imeSession = FindWindowImeSession();
        if (imeSession == nullptr) return 0;
        HIMC context = reinterpret_cast<HIMC>(imeSession->UiFrame().ime.context);
        if (context != nullptr) {
            static_assert(sizeof(DWORD) == sizeof(std::uint32_t));
            ImmGetConversionStatus(
                context,
                reinterpret_cast<DWORD*>(&imeSession->UiFrame().ime.conversionMode),
                reinterpret_cast<DWORD*>(&imeSession->UiFrame().ime.sentenceMode));
            // 原467809：外部调用后重取共享上下文，不沿用调用前快照。
            context = reinterpret_cast<HIMC>(imeSession->UiFrame().ime.context);
        }

        if (wParam32 == IMN_OPENCANDIDATE
            || wParam32 == IMN_CHANGECANDIDATE) {
            const DWORD requiredSize =
                ImmGetCandidateListW(context, 0, nullptr, 0);
            if (imeSession->UiFrame().ime.candidateList != nullptr) {
                std::free(imeSession->UiFrame().ime.candidateList);
            }
            if (requiredSize != 0) {
                void* const candidates = std::calloc(1, requiredSize);
                if (wParam32 == IMN_CHANGECANDIDATE) {
                    // 原4678B5/4678BE：变化通知先重取上下文，再发布候选表。
                    context = reinterpret_cast<HIMC>(imeSession->UiFrame().ime.context);
                    imeSession->UiFrame().ime.candidateList = candidates;
                } else {
                    // 原467849/467850：打开通知先发布候选表，再重取上下文。
                    imeSession->UiFrame().ime.candidateList = candidates;
                    context = reinterpret_cast<HIMC>(imeSession->UiFrame().ime.context);
                }
                ImmGetCandidateListW(
                    context,
                    0,
                    reinterpret_cast<LPCANDIDATELIST>(candidates),
                    requiredSize);
                return 0;
            }
            imeSession->UiFrame().ime.candidateList = nullptr;
            return 0;
        }
        if (wParam32 == IMN_CLOSECANDIDATE) {
            if (imeSession->UiFrame().ime.candidateList != nullptr) {
                std::free(imeSession->UiFrame().ime.candidateList);
            }
            imeSession->UiFrame().ime.candidateList = nullptr;
        }
        return 0;
    }

    case WM_IME_SETCONTEXT: {
        const std::uint32_t filtered = lParam32 & 0x7FFFFFF0u;
        return DefWindowProcW(
            window,
            message,
            wParam,
            static_cast<LPARAM>(filtered));
    }

    default:
        return DefWindowProcW(window, message, wParam, lParam);
    }
}

} // namespace cg::platform
