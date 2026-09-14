#include "cg/platform/Startup.hpp"
#include "cg/platform/LegacyWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {

// 原 0x465A30；会改变系统桌面颜色，恢复依赖尚未完成，见 doc/cg2d_window_lifecycle.md。
int SetWindowedDesktopColor()
{
    const INT element = COLOR_BACKGROUND;
    const COLORREF color = 0x00A4A0A0;
    return SetSysColors(1, &element, &color);
}

// 原 WndInit@0x4669F0；保留创建、已有窗口及窗口模式的各分支。
int InitializeWindow()
{
    DWORD style;
    RECT rect;
    if (gWindowMode) {
        style = 0x00CA0000;
        SetWindowedDesktopColor();
        // 原 0x466A08 在 helper 返回后重读模式；变为零时仍保留窗口化 style。
        if (gWindowMode) {
            SetRect(&rect, 0, 0, gWindowWidth, gWindowHeight);
        } else {
            SetRect(&rect, 0, 0, 640, 480);
        }
    } else {
        style = 0x90000000;
        SetRect(&rect, 0, 0, 640, 480);
    }
    AdjustWindowRectEx(&rect, style, FALSE, 0);

    if (gLegacyWindow) {
        // GWL_STYLE 的值仍是 32 位 LONG，不是需要 Ptr 宽化的窗口指针字段。
        SetWindowLongA(reinterpret_cast<HWND>(gLegacyWindow), GWL_STYLE, static_cast<LONG>(style));
        ShowWindow(reinterpret_cast<HWND>(gLegacyWindow), gShowCommand);
        if (gWindowMode) {
            SetWindowPos(
                reinterpret_cast<HWND>(gLegacyWindow), HWND_NOTOPMOST,
                gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
                SWP_FRAMECHANGED);
        } else {
            SetWindowPos(
                reinterpret_cast<HWND>(gLegacyWindow), HWND_NOTOPMOST,
                0, 0, rect.right - rect.left, rect.bottom - rect.top,
                SWP_FRAMECHANGED);
        }
    } else {
        HWND window;
        if (gWindowMode) {
            window = CreateWindowExA(
                0, gWindowClassName, gWindowClassName, style,
                gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
                nullptr, nullptr, reinterpret_cast<HINSTANCE>(gInstance), nullptr);
        } else {
            window = CreateWindowExA(
                0, gWindowClassName, gWindowClassName, style,
                0, 0, rect.right - rect.left, rect.bottom - rect.top,
                nullptr, nullptr, reinterpret_cast<HINSTANCE>(gInstance), nullptr);
        }
        gLegacyWindow = reinterpret_cast<std::uintptr_t>(window);
    }

    // 原 0x466B56 的公共尾部；已有窗口也必须再次 ShowWindow。
    ShowWindow(reinterpret_cast<HWND>(gLegacyWindow), gShowCommand);
    return UpdateWindow(reinterpret_cast<HWND>(gLegacyWindow));
}

// 原 0x466D50；移除并派发整个队列，遇到 WM_QUIT 立即返回零。
int PumpWindowMessages()
{
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            return 0;
        }
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return 1;
}

} // namespace cg::platform
