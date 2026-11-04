#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cg::platform {

// 会话自己的IME文本与候选内存；原生窗口上下文关联另由平台生命周期接线。
struct ImeRuntimeState {
    ImeRuntimeState() = default;
    ~ImeRuntimeState();
    ImeRuntimeState(const ImeRuntimeState&) = delete;
    ImeRuntimeState& operator=(const ImeRuntimeState&) = delete;
    ImeRuntimeState(ImeRuntimeState&&) = delete;
    ImeRuntimeState& operator=(ImeRuntimeState&&) = delete;
    std::wstring description; // 原73ED70，借用到本会话后续更新。
    std::wstring composition; // 原73EDE0，完整UTF-16组合正文。
    std::vector<std::uint8_t> attributes; // 原73EEE0，按UTF-16码元位置对应。
    std::int32_t cursorPosition = 0;
    std::uint32_t conversionMode = 0; // 原73EFE4。
    std::uint32_t sentenceMode = 0; // 原73EFE8。
    void* candidateList = nullptr; // 原calloc候选表，由本会话释放。
    std::uint8_t state73EFF0 = 0;
    wchar_t highSurrogate = 0; // WM_CHAR跨消息待组合的高代理码元。
};

} // namespace cg::platform
