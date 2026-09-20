#pragma once

#include <cstdint>

namespace cg::platform {

// 原 0x9A6F2C/0x9A6F30：最近一次 WM_SIZE 的客户区尺寸。
extern std::uint32_t gCurrentClientWidth;
extern std::uint32_t gCurrentClientHeight;

// 原 0x9A7F68：窗口化尺寸修正的 0/1/2 阶段。
extern std::uint32_t gWindowResizePhase;

// 原 IME 平台状态。原生句柄使用指针宽承载，不向业务接口泄漏 Win32 类型。
extern std::uintptr_t gImeContext;
extern std::uintptr_t gPreviousImeContext; // 原 0x73EDDC；本项不接入生产逻辑。
extern char gImeComposition[256];
extern std::uint8_t gImeAttributes[256];
extern std::int32_t gImeCompositionLength;
extern std::uint32_t gImeConversionMode;
extern std::uint32_t gImeSentenceMode;
extern void* gImeCandidateList;

} // namespace cg::platform
