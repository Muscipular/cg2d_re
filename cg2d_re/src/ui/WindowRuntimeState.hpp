#pragma once

#include "cg/ui/WindowRuntime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::input {
struct InputContext;
}

namespace cg::ui {

using WindowCallback = std::int32_t (*)(std::int32_t value);
using WindowReleaseCallback = void (*)();
using WindowElementCallback = std::int32_t (*)(std::int32_t index, std::int32_t state);
using WindowValueCallback = void (*)(std::int32_t value);
using WindowDragCallback = void (*)(
    std::int32_t message, std::int32_t flags, std::int32_t sourceIndex);

// 原type1的24B图形载荷，六个数值字段，不含地址。
struct WindowGraphicData {
    std::int32_t imageNo;
    std::int32_t offsetX;
    std::int32_t offsetY;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t color;
};

// 原type5首槽是真实输入上下文引用；外层载荷拥有、上下文不拥有。
struct WindowInputData {
    input::InputContext* context;
};

// 原type7的文本T*与四个字节；字符串借用，不随外层free。
struct WindowNumberData {
    char* text;
    std::uint8_t color;
    std::int8_t style;
    std::uint8_t flags;
    std::uint8_t byte07;
};

// 原第2类元素的8B载荷；471622写真实Yobi，471334只给它标死。
struct WindowAnimationData {
    login::YobiAction* action;
    std::uint32_t value04;
};

// 原第3类元素的8B载荷；文本由47167F分配，47134F释放。
struct WindowTextData {
    std::array<std::byte, 4> bytes00;
    char* text;
};

// 原type4的12B鼠标载荷；滚动回调与元素命中共用，不含地址。
struct WindowMouseData {
    std::uint32_t flags;
    std::int32_t deltaX;
    std::int32_t deltaY;
};

// 仅按已证字段增量映射；未消费数据不计最终布局冻结，见UI窗口SPEC。
struct WindowElement {
    std::int32_t type;
    std::uint32_t state;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t visible;
    void* data; // 原+1C，七类数据共用；后续按type解释，不是地址数值。
    WindowElementCallback mouseCallback; // 原+20。
};

// 原实例+40的16B可拖动区域，472D20逐项命中。
struct WindowDragRect {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

struct WindowInstance {
    std::uint32_t state; // 原+00；关闭请求置bit15。
    std::int32_t id;
    std::int32_t layer;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t dragMouseX1C;
    std::int32_t dragMouseY20;
    std::uint32_t flags;
    std::int32_t elementCount;
    WindowElement* elements; // 原+2C。
    WindowCallback update; // 原+30。
    WindowCallback draw; // 原+34。
    WindowReleaseCallback release; // 原+38。
    std::int32_t secondaryCount; // 原+3C。
    std::array<WindowDragRect, 10> dragRects40; // 原[+40,+E0)。
};

namespace detail {

// 原输入原型首W32的两种解释；不对原生T*进行原x86高字节测试。
enum class WindowInputReferenceKind : std::uint8_t {
    Context,
    RejectedValue,
};

struct WindowInputReference {
    union {
        input::InputContext* context;
        std::uint32_t rejectedValue; // 原高字节非零的拒绝值，低24位也保留。
    };
    WindowInputReferenceKind kind;
};

// 原57E970的0x22C输入原型，不同于窗口type1的图形原型。
struct WindowInputPrototype {
    WindowInputReference input;
    std::int32_t x;
    std::int32_t y;
    std::int32_t renderMode;
    std::int32_t fontType;
    std::int32_t color;
    std::array<char, 0x204> text; // 原+18文本起点至+21C前的字节区，未消费尾部不冻结语义。
    std::int32_t maxLine;
    std::int32_t maxCharPerLine;
    std::int32_t value224;
    std::int32_t encrypt;
};

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowValueCallback callback; // 原窗口42的+18，禁止截成W32。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x;
    std::int32_t y;
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10;
    std::uint32_t value14;
    WindowUserState userState; // 原+18；后续生产者按实际用途激活成员。
    std::array<std::byte, 4> bytes1C;
    std::uint32_t value20;
    std::uint32_t value24;
    std::uint32_t value28;
    std::array<std::byte, 36> bytes2C; // 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50;
};

inline constexpr std::size_t kWindowCount = 71;
extern std::array<WindowInstance*, kWindowCount> gWindows; // 原C8B094。
extern std::array<WindowState, kWindowCount> gWindowStates; // 原C9A9B0。
extern WindowInstance* gCurrentWindow; // 原CF4694。
extern WindowState* gCurrentWindowState; // 原CF4684。
extern std::uint8_t gValueD67BE0; // 原多个UI生产/消费入口共用，不并入窗口记录。
extern std::int32_t gDragItemIndex584DA4; // 4FAEF0/4FB8F2的真实写入链待接。
extern void* gFocusedElementDataD45F94; // 原异型元素载荷地址，仅借用，不是InputContext。
extern std::int32_t gMouseValue57C110;
extern std::int32_t gDragSourceIndex57C114;
extern WindowDragCallback gDragCallback57C118;
extern std::int16_t gDragX57C11C;
extern std::int16_t gDragY57C11E;
extern std::int32_t gFocusWindowId57C120;
extern std::int32_t gFocusElementIndex57C124;
extern std::int32_t gMousePhase57C128;
extern std::int32_t gCurrentElementIndex57C12C; // 原映像初值-1，不等同焦点元素。
extern std::int32_t gCurrentWindowIdD67A1C;
extern std::uint32_t gWindowHasFocusCA0AB4; // 元素回调成功后占用，独立于焦点窗口id。
extern WindowInputPrototype gChatInputPrototype57E970;
extern std::int32_t gChatWindowModeD66E04; // 原独立S32，窗口35/57选择与设置文件共用。
extern std::uint8_t gValue5F9754; // 原多窗口/物品UI共享字节，观察进入可能清零。
extern std::int32_t gChatWidthUnitsC9A930; // 原聊天宽度单位，输入滚动/聊天绘制共用。
extern std::int32_t gChatHeightUnitsC9A938; // 原聊天可见高度的字体9行高倍数。
extern std::int32_t gChatScrollOffsetC9A940; // 原相对共享写指针的回看条数。

// 原输入原型四模式更新及聊天恢复，复用既有InputContext owner。
std::uint8_t Sub_476570(
    WindowInputPrototype* prototype, std::int32_t x, std::int32_t y,
    std::uint32_t mode);
std::int32_t Sub_4A7B00();
void Sub_4740F0(char* destination); // 前10项技能索引的S16槽数，格式化后复制原字节。
void Sub_4A7590(input::InputContext* context); // 按聊天宽度滚动目标输入并绘制。
void Sub_4A7640(input::InputContext* context); // 按固定1024像素滚动目标输入并绘制。
void Sub_4A7750(std::int32_t x, std::int32_t y); // 原字节分行、底部逆序绘制及前项字体回写。
std::int32_t Sub_46FBC0(std::int32_t unused); // 原默认draw回调，完整绘制后返回1。
std::int32_t Sub_49E750(std::int32_t value); // 窗口39文本居中/到时关闭请求。
std::int32_t Sub_49BDB0(std::int32_t unused); // 完整元素绘制后叠加九宫格背景。
std::int32_t Sub_472F00(
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
void Sub_473560(std::int32_t index); // 固定元素派发鼠标事件，回调后重读共享阶段。
std::int32_t Sub_4741A0(std::int32_t index, std::int32_t state); // 前一元素的垂直滚动。
std::int32_t Sub_4763E0(); // 通用鼠标重置，不清来源索引/坐标。
std::int32_t Sub_472D20(); // 窗口拖动/焦点命中，返回两位处理掩码。
std::int32_t Sub_4730A0(); // 原表压紧、关闭释放及按2000标记前插排序。
void Sub_473E40(); // 每轮窗口处理前的鼠标阶段推进。
void Sub_4771B0(
    std::int32_t x, std::int32_t y, const char* text,
    std::int8_t style, std::int8_t color, std::uint8_t flags, std::uint8_t layer);
std::int32_t Sub_477460(const char* text, std::int8_t style); // 原字节长度乘运行步进减扣宽。

static_assert(sizeof(WindowInputReference) == 16);
static_assert(offsetof(WindowGraphicData, offsetX) == 4);
static_assert(offsetof(WindowGraphicData, offsetY) == 8);
static_assert(offsetof(WindowGraphicData, width) == 0x0C);
static_assert(offsetof(WindowGraphicData, height) == 0x10);
static_assert(offsetof(WindowGraphicData, color) == 0x14);
static_assert(sizeof(WindowGraphicData) == 0x18);
static_assert(sizeof(WindowInputData) == 8);
static_assert(offsetof(WindowNumberData, color) == 8);
static_assert(offsetof(WindowNumberData, style) == 9);
static_assert(offsetof(WindowNumberData, flags) == 0x0A);
static_assert(offsetof(WindowNumberData, byte07) == 0x0B);
static_assert(sizeof(WindowNumberData) == 16);
static_assert(offsetof(WindowInputPrototype, text) == 0x24);
static_assert(offsetof(WindowInputPrototype, maxLine) == 0x228);
static_assert(sizeof(WindowInputPrototype) == 0x238);
static_assert(offsetof(WindowAnimationData, action) == 0);
static_assert(offsetof(WindowAnimationData, value04) == 8);
static_assert(sizeof(WindowAnimationData) == 16);
static_assert(offsetof(WindowTextData, text) == 8);
static_assert(sizeof(WindowTextData) == 16);
static_assert(offsetof(WindowMouseData, deltaX) == 4);
static_assert(offsetof(WindowMouseData, deltaY) == 8);
static_assert(sizeof(WindowMouseData) == 12);
static_assert(offsetof(WindowElement, data) == 0x20);
static_assert(offsetof(WindowElement, mouseCallback) == 0x28);
static_assert(sizeof(WindowElement) == 0x30);
static_assert(offsetof(WindowInstance, state) == 0);
static_assert(offsetof(WindowInstance, elements) == 0x30);
static_assert(offsetof(WindowInstance, update) == 0x38);
static_assert(offsetof(WindowInstance, draw) == 0x40);
static_assert(offsetof(WindowInstance, release) == 0x48);
static_assert(offsetof(WindowInstance, secondaryCount) == 0x50);
static_assert(offsetof(WindowDragRect, y) == 4);
static_assert(offsetof(WindowDragRect, width) == 8);
static_assert(offsetof(WindowDragRect, height) == 12);
static_assert(sizeof(WindowDragRect) == 16);
static_assert(offsetof(WindowInstance, dragMouseX1C) == 0x1C);
static_assert(offsetof(WindowInstance, dragMouseY20) == 0x20);
static_assert(offsetof(WindowInstance, dragRects40) == 0x54);
static_assert(sizeof(WindowInstance) == 0xF8);
static_assert(offsetof(WindowState, window) == 0x10);
static_assert(offsetof(WindowState, userState) == 0x20);
static_assert(offsetof(WindowState, marker50) == 0x5C);
static_assert(sizeof(WindowState) == 0x60);

} // namespace detail
} // namespace cg::ui
