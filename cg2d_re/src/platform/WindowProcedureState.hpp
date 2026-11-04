#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cg::platform {

// 原 0x9A6F2C/0x9A6F30：最近一次 WM_SIZE 的客户区尺寸。
extern std::uint32_t gCurrentClientWidth; // 原全局：0x009A6F2C。
extern std::uint32_t gCurrentClientHeight; // 原全局：0x009A6F30。

// 原 0x9A7F68：窗口化尺寸修正的 0/1/2 阶段。
extern std::uint32_t gWindowResizePhase;

// 原窗口IME关联句柄；文本/候选归会话，上下文关联链尚待随宿主拆分。
extern std::uintptr_t gImeContext; // 原全局：0x0073EDD8。
extern std::uintptr_t gPreviousImeContext; // 原全局：0x0073EDDC。

} // namespace cg::platform
