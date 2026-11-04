#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cg::platform {

// 会话独占IME上下文、文本与候选内存；窗口默认上下文只由平台借用。
struct ImeRuntimeState {
    ImeRuntimeState() = default;
    ~ImeRuntimeState();
    ImeRuntimeState(const ImeRuntimeState&) = delete;
    ImeRuntimeState& operator=(const ImeRuntimeState&) = delete;
    ImeRuntimeState(ImeRuntimeState&&) = delete;
    ImeRuntimeState& operator=(ImeRuntimeState&&) = delete;
    std::uintptr_t context = 0; // 原73EDD8；本会话创建的实际原生上下文。
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
