#pragma once

#include <Windows.h>

namespace cg::platform {

// NSM01：只扩展非客户区，游戏客户区仍保持原尺寸和原点。
void AdjustSessionWindowRect(RECT& rectangle, UINT dpi) noexcept;
bool HandleSessionWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);

// 窗口回调只排队；主循环消息泵后、遍历会话前处理，最后一项关闭返回false。
bool ProcessSessionWindowCommands();
void RefreshSessionWindow();
void ShutdownSessionWindow() noexcept;

} // namespace cg::platform
