#include "cg/platform/Startup.hpp"
#include "cg/core/m_log.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "../world/BattleState.hpp"

#include <Windows.h>
#include <cstring>

namespace cg::platform {

// 原465A30：改变系统桌面背景色；原颜色由启动链保存，ShutdownClient_457A40末尾恢复。
int SetWindowedDesktopColor()
{
    const INT element = COLOR_BACKGROUND;
    const COLORREF color = 0x00A4A0A0;
    return SetSysColors(1, &element, &color);
}

// 原 WndInit@0x4669F0；WO01固定窗口模式，保留创建及已有窗口的处理。
int InitializeWindow()
{
    // R02：首次 WndInit 前解析原进程参数；重建窗口复用已经发布的布局。
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        const char* const commandLine = GetCommandLineA();
        int width = 0;
        int height = 0;
        const char* option = std::strstr(commandLine, "/width:");
        if (option == nullptr) {
            option = std::strstr(commandLine, "/w:");
        }
        if (option != nullptr && std::sscanf(std::strchr(option, ':') + 1, "%d", &width) == 1) {
            gWindowWidth = width;
        }
        option = std::strstr(commandLine, "/height:");
        if (option == nullptr) {
            option = std::strstr(commandLine, "/h:");
        }
        if (option != nullptr && std::sscanf(std::strchr(option, ':') + 1, "%d", &height) == 1) {
            gWindowHeight = height;
        }
        ui::InitializeWindowLayout();
        m_log_iw(L"%d %d\n", width, height);
        layoutInitialized = true;
    }
    world::CenterBattleLayoutToWindow();
    const DWORD style = 0x00CA0000;
    RECT rect;
    SetWindowedDesktopColor();
    SetRect(&rect, 0, 0, gWindowWidth, gWindowHeight);
    AdjustWindowRectEx(&rect, style, FALSE, 0);

    if (gLegacyWindow) {
        // GWL_STYLE 的值仍是 32 位 LONG，不是需要 Ptr 宽化的窗口指针字段。
        SetWindowLongW(reinterpret_cast<HWND>(gLegacyWindow), GWL_STYLE, static_cast<LONG>(style));
        ShowWindow(reinterpret_cast<HWND>(gLegacyWindow), gShowCommand);
        SetWindowPos(
            reinterpret_cast<HWND>(gLegacyWindow), HWND_NOTOPMOST,
            gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED);
    } else {
        HWND window = CreateWindowExW(
            0, gWindowClassName, gWindowClassName, style,
            gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, reinterpret_cast<HINSTANCE>(gInstance), nullptr);
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
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            return 0;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 1;
}

} // namespace cg::platform
