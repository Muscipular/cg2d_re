#pragma once

#include <cstdint>
#include <string_view>

namespace cg::platform {

// 原 hWnd@0x9A6F34 的原生窗口引用；由窗口初始化链绑定，不截断为 32 位。
extern std::uintptr_t gLegacyWindow;

// 消息正文与标题统一UTF-16；内部拥有NUL结尾副本后交MessageBoxW，保原窗口／标志／返回。
int ShowWideMessage(std::wstring_view text, std::wstring_view caption, std::uint32_t flags);

// 原SetWindowTextA的标题边界：UTF-8转UTF-16后调用SetWindowTextW，保原生宽度窗口引用。
std::int32_t SetLegacyWindowText(std::uintptr_t window, std::wstring_view text);

// 原 PostMessageA 边界：保留 HWND/WPARAM/LPARAM 的原生宽度与 BOOL 返回值。
std::int32_t PostLegacyWindowMessage(
    std::uintptr_t window,
    std::uint32_t message,
    std::uintptr_t wParam,
    std::intptr_t lParam);

// 原 ShellExecuteA 边界：保留 HWND/HINSTANCE 的原生宽度，不解释返回的错误码。
std::intptr_t ExecuteLegacyShell(
    std::uintptr_t window,
    const char* operation,
    const char* file,
    const char* parameters,
    const char* directory,
    std::int32_t showCommand);

} // namespace cg::platform
