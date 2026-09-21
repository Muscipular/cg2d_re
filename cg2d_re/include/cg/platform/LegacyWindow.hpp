#pragma once

#include <cstdint>

namespace cg::platform {

// 原 hWnd@0x9A6F34 的原生窗口引用；由窗口初始化链绑定，不截断为 32 位。
extern std::uintptr_t gLegacyWindow;

// 原 MessageBoxA 边界：仅传递原字节串，不在这里猜测或变更旧客户端编码。
int ShowLegacyMessage(const char* text, const char* caption, std::uint32_t flags);

// 原 ShowCursor 边界：保留调用方传入的完整 W32，不归一化为 bool。
std::int32_t ShowLegacyCursor(std::int32_t show);

// 原 SetWindowTextA 边界：原样传递原生宽度窗口引用与窄字节串。
std::int32_t SetLegacyWindowText(std::uintptr_t window, const char* text);

// 原 PostMessageA 边界：保留 HWND/WPARAM/LPARAM 的原生宽度与 BOOL 返回值。
std::int32_t PostLegacyWindowMessage(
    std::uintptr_t window,
    std::uint32_t message,
    std::uintptr_t wParam,
    std::intptr_t lParam);

} // namespace cg::platform
