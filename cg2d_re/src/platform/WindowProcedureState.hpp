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

// 原 IME 平台状态。原生句柄使用指针宽承载，不向业务接口泄漏 Win32 类型。
extern std::wstring gImeDescription; // 原0x0073ED70；UTL02动态UTF-16描述。
extern std::uintptr_t gImeContext; // 原全局：0x0073EDD8。
extern std::uintptr_t gPreviousImeContext; // 原全局：0x0073EDDC。
extern std::wstring gImeComposition; // 原0x0073EDE0；UTL02动态UTF-16组合正文。
extern std::vector<std::uint8_t> gImeAttributes; // 原0x0073EEE0；与UTF-16码元位置对应。
extern std::int32_t gImeCompositionCursorPosition; // UTL02：UTF-16码元偏移。
extern std::uint32_t gImeConversionMode; // 原全局：0x0073EFE4。
extern std::uint32_t gImeSentenceMode; // 原全局：0x0073EFE8。
extern void* gImeCandidateList;
// 原 0x73EFF0：当前证据不足以确定语义，仅保留地址关联。
extern std::uint8_t gImeState73EFF0;

} // namespace cg::platform
