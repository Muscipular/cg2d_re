#pragma once

#include <cstdint>
#include <string_view>

namespace cg::platform {

// 原 hWnd@0x9A6F34 的原生窗口引用；由窗口初始化链绑定，不截断为 32 位。
extern std::uintptr_t gLegacyWindow;

// 当前客户端是否为前台窗口；用于终止失焦后的UI鼠标捕获，不向业务暴露HWND。
bool IsClientWindowForeground() noexcept;

// 消息正文与标题统一UTF-16；内部拥有NUL结尾副本后交MessageBoxW，保原窗口／标志／返回。
int ShowWideMessage(std::wstring_view text, std::wstring_view caption, std::uint32_t flags);

} // namespace cg::platform
