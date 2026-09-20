#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::input {

inline constexpr std::size_t InputTextCapacity = 0x120;

// 原 InputContext，固定承接 x86 的 0x15C 字节值布局。
struct InputContext {
    char text[InputTextCapacity];
    std::int32_t x;
    std::int32_t y;
    std::int32_t caretByteOffset;
    std::int32_t totalByteLength;
    std::int32_t field130;
    std::int32_t field134;
    std::uint8_t field138[4];
    std::int32_t field13C;
    std::int32_t field140;
    std::int32_t maxLine;
    std::int32_t maxCharPerLine;
    std::int32_t field14C;
    std::int32_t encrypt;
    std::int32_t field154;
    std::int32_t field158;
};

static_assert(offsetof(InputContext, text) == 0x000);
static_assert(offsetof(InputContext, x) == 0x120);
static_assert(offsetof(InputContext, y) == 0x124);
static_assert(offsetof(InputContext, caretByteOffset) == 0x128);
static_assert(offsetof(InputContext, totalByteLength) == 0x12C);
static_assert(offsetof(InputContext, maxLine) == 0x144);
static_assert(offsetof(InputContext, maxCharPerLine) == 0x148);
static_assert(offsetof(InputContext, encrypt) == 0x150);
static_assert(sizeof(InputContext) == 0x15C);

// 原 0x626C04：当前输入上下文；允许由 SetContext 写为 nullptr。
extern InputContext* gCurrentInputData;

// 下列状态只保留本函数已取证的读写语义，不扩展业务命名。
extern std::int32_t gValue626C0C; // 接受路径最终写 20。
extern std::int32_t gValue626C20; // 超过 maxLine 时选择截断或回滚分支。
extern std::int32_t gValue626C28; // 特定回滚路径写 1。

// 原 Input::SetContext@[0x44B330,0x44B33A)。
void SetContext_44B330(InputContext* context);

// 原 sub_449CF0@[0x449CF0,0x449D92)。
void InitializeContext_449CF0(
    InputContext* context,
    std::int32_t field130,
    std::int32_t field134,
    std::int32_t renderMode,
    std::int32_t fontType,
    std::int32_t color,
    const char* initialText,
    std::int32_t maxLine,
    std::int32_t maxCharPerLine,
    std::int32_t field14C,
    std::int32_t encrypt);

// 原 Input::AppendChar@[0x44ADB0,0x44B044)。
void AppendChar_44ADB0(char character);

} // namespace cg::input
