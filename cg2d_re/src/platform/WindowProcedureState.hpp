#pragma once

#include <Windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace cg::platform {

// 原 0x9A6F2C/0x9A6F30：最近一次 WM_SIZE 的客户区尺寸。
extern std::uint32_t gCurrentClientWidth; // 原全局：0x009A6F2C。
extern std::uint32_t gCurrentClientHeight; // 原全局：0x009A6F30。

// 原 0x9A7F68：窗口化尺寸修正的 0/1/2 阶段。
extern std::uint32_t gWindowResizePhase;

// NSM01：同一次按住的重复消息不得输入切入会话；消息泵在TranslateMessage前调用。
bool RouteSessionKeyDown(std::uint32_t key, std::uint32_t keyMessageBits) noexcept;

// 仅转送实际收到的非游戏层鼠标抬起，不为未记录的按下制造游戏事件。
void RouteSessionMouseRelease(bool leftButton, LPARAM screenCoordinates);
void RouteSessionKeyRelease(std::uint32_t key, bool useCurrentWhenUnbound) noexcept;

} // namespace cg::platform
