#pragma once

#include <Windows.h>

namespace cg::platform {

// 原 WndClass@0x9A6ED0；仅 OS 边界采用本机 x64 WNDCLASSW，不模拟原 x86 字节布局。
extern WNDCLASSW gWindowClass;

// 原 WIN_PROC@0x466DC0；必须实现完整原窗口过程后才能链接客户端。
LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

} // namespace cg::platform
