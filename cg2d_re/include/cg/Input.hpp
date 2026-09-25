#pragma once

#include <cstddef>
#include <cstdint>

namespace cg {

// 原 nullsub_1@[0x509080,0x509081)：唯一指令为 retn，不读取参数且不定义返回值。
void NullSub_509080() noexcept;

} // namespace cg

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

// 原 sub_404060@[0x404060,0x40411E)，返回 true 表示拒绝输入文本。
[[nodiscard]] bool IsRejectedInputText_404060(const char* legacyText);

// 原 sub_44B190@[0x44B190,0x44B1C5)，判断零结尾旧 ANSI 串的末单元。
[[nodiscard]] std::int32_t ClassifyLastAnsiUnit_44B190(const char* text);

// 原 sub_44B440@[0x44B440,0x44B49D)，按低 S16 偏移判断旧双字节边界，返回 AL=0/1/2。
[[nodiscard]] std::uint8_t ClassifyLegacyBoundary_44B440(
    const char* text, std::int16_t byteOffset);

// 原 sub_44B4A0@[0x44B4A0,0x44B561)：跳过旧双字节尾字节，返回可写的首个匹配位置。
[[nodiscard]] char* FindLegacyByte_44B4A0(char* text, const char* target);

// 原 0x626C04：当前输入上下文；允许由 SetContext 写为 nullptr。
extern InputContext* gCurrentInputData;

// 原 stru_626AA0@0x626AA0：本项只保留地址身份与映像零初始化。
extern InputContext gInputContext626AA0;
// 原连续区间 [0x5F9758,0x5FB7F8)：24 项 InputContext 的唯一 owner。
extern InputContext gInputContexts5F9758[24];
// 原回车分派按地址比较的七个独立 InputContext；各自保留原地址身份。
extern InputContext gInputContextD66CA8;
extern InputContext gInputContextCB03F0;
extern InputContext gInputContextCC46E0;
extern InputContext gInputContextCC2080;
extern InputContext gInputContextD2D828;
extern InputContext gInputContextCA0180;
extern InputContext gInputContextC8B1B8;
// 原 dword_C9A96C@0xC9A96C：该特殊上下文的外部控制值，映像初值为零。
extern std::int32_t gValueC9A96C;

// 原 [593850,5939B4) 的前缀文本；紧邻独立 InputContext@593858。
const char* LegacyText593850() noexcept;

// 下列状态只保留当前已取证的读写语义，不扩展业务命名。
extern std::int32_t gValue626C0C; // 输入相位计数；接受字符后写 20。
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

// 原 sub_52D1B0：旧文本字节转义，始终返回目标缓冲指针。
char* EscapeLegacyText_52D1B0(
    const char* source, char* destination, std::int32_t capacity);

// 原 sub_52D2D0：先调用空叶，再转换输入上下文的文本。
char* EscapeInputText_52D2D0(
    const InputContext* context, char* destination, std::int32_t capacity);

// 原 52D150/52D2F0：原位还原旧字节转义；包装随后调用空叶并透传原指针。
char* UnescapeLegacyText_52D150(char* text);
char* UnescapeLegacyTextAndNotify_52D2F0(char* text);

// 原 sub_52D570：逐字节从原缓冲区删除指定集合，尾部写 NUL，返回低 W8 零。
char RemoveLegacyBytes_52D570(char* text, const char* excluded);

// 原 sub_4596D0：按唯一旧键状态表分派输入编辑及尾部音频动作。
void ProcessKeyInput_4596D0();

} // namespace cg::input
