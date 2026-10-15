#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
static std::int32_t WindowProfileCategory_Update(std::int32_t value); // 窗口60仅初始化刷新，随后检查距离。

// 原函数：0x004C9F20。
static void WindowProfileCategory_Refresh(); // 固定32/32/15项分类的显隐、文本和选择颜色。

// 原函数：0x004CA1A0。
static std::int32_t WindowProfileCategory_SelectCategory(std::int32_t index, std::int32_t state); // 分类点击发送后复制共享默认关键词。

// 原函数：0x004CA300。
static void WindowProfileCategory_Release(); // 先清对话类型，再标记61/62/63。

// 原函数：0x004CB4A0。
static std::int32_t WindowProfileCategory_ResolveCategory(std::int32_t index, std::int32_t* group, std::int32_t* value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic577740 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577740。

static WindowGraphicData gWindowGraphic577758 {0x3C119, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577758。

static WindowGraphicData gWindowGraphic577770 {0x3BB3D, 0, 0, 0, 0, 0x8FFFFFFFu}; // 原全局：0x00577770。

static WindowGraphicData gWindowGraphic577788 {0x3C11A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577788。

static WindowTextPrototype gWindowText5777A0 { // 原全局：0x005777A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"類別",
};

static WindowElementPrototype gWindowElements5777A8[162] { // 原全局：0x005777A8。
    {WindowElementType_Text, 50, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic577740}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic577770}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 600, 0, 34, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口60；原全局：0x0054ED90。
WindowPrototype gWindowPrototypeProfileCategory {0, 4, 5, 20, 634, 424, 0x80000080u, 162, gWindowElements5777A8, &WindowProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowProfileCategory_Release}; // 60: 54ED90

// 窗口函数实现。

// 原函数：0x004C9F20。
static void WindowProfileCategory_Refresh()
{
    constexpr std::array<std::int32_t, 3> counts {32, 32, 15};
    constexpr std::array<std::int32_t, 3> textBases {0, 32, 64};
    for (std::int32_t group = 0; group < 3; ++group) {
        for (std::int32_t index = 1; index <= counts[group]; ++index) {
            const auto& record = world::detail::ReadCategoryRecord105FA24(group, index);
            const std::int32_t textIndex = textBases[group] + index - 1;
            if (record.used != 0) {
                gCurrentWindow->elements[textIndex].visible = 1;
                gCurrentWindow->elements[textIndex + 79].visible = 1;
                static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data)->text = cg::text::FromGbkWide(record.text0C.data());
                static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data)->style.color =
                    record.selected10C != 0 ? std::byte{0} : std::byte{7};
            } else {
                gCurrentWindow->elements[textIndex].visible = 0;
                gCurrentWindow->elements[textIndex + 79].visible = 0;
            }
        }
    }
}

// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
static std::int32_t WindowProfileCategory_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowProfileCategory_Refresh();
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004CA300。
static void WindowProfileCategory_Release()
{
    world::detail::gValue581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(2));
    static_cast<void>(WindowProfileView_RequestCategoryClose(3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
}

// 原函数：0x004CB4A0。
static std::int32_t WindowProfileCategory_ResolveCategory(
    const std::int32_t index, std::int32_t* const group, std::int32_t* const value)
{
    if (index - 79 < 32) {
        *group = 0;
        *value = world::detail::ReadCategoryRecord105FA24(0, index - 78).value04;
    } else if (index - 79 < 64) {
        *group = 1;
        *value = world::detail::ReadCategoryRecord105FA24(1, index - 110).value04;
    } else {
        *group = 2;
        *value = world::detail::ReadCategoryRecord105FA24(2, index - 142).value04;
    }
    return 1;
}

// 原函数：0x004CA1A0。
static std::int32_t WindowProfileCategory_SelectCategory(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"在此目錄中搜尋";
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 246042;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0;
    }
    if ((state & 2) == 0 || WindowProfileView_ReadSelection(index) == 0) {
        return consumed;
    }
    std::int32_t group;
    std::int32_t value;
    if (WindowProfileCategory_ResolveCategory(index, &group, &value) != 0) {
        std::array<std::wstring, 2> buffers;
        cg::text::FormatTo(buffers[0], L"{}|{}|", group, value);
        buffers[1] = input::EscapeTextAndNotify(buffers[0], 255);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, buffers[1]));
        gProfileSearchKeyword1075988.clear();
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
