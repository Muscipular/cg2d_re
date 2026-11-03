#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowProfileSearchElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    Element3 = 3,
    HandleSearchButtons = 4,
    HandleSearchButtons_5 = 5,
    HandleSearchButtons_6 = 6,
    Element7 = 7,
    EditKeyword = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    Element27 = 27,
    Element28 = 28,
    Element29 = 29,
    Element30 = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    Element36 = 36,
    Element37 = 37,
    Element38 = 38,
    Element39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
    Element44 = 44,
    Element45 = 45,
    Element46 = 46,
    Element47 = 47,
    Element48 = 48,
    SelectResult = 49,
    SelectResult_50 = 50,
    SelectResult_51 = 51,
    SelectResult_52 = 52,
    SelectResult_53 = 53,
    SelectResult_54 = 54,
    SelectResult_55 = 55,
    SelectResult_56 = 56,
    SelectResult_57 = 57,
    SelectResult_58 = 58,
    SelectResult_59 = 59,
    SelectResult_60 = 60,
    SelectResult_61 = 61,
    SelectResult_62 = 62,
    SelectResult_63 = 63,
    SelectResult_64 = 64,
    SelectResult_65 = 65,
    SelectResult_66 = 66,
    SelectResult_67 = 67,
    SelectResult_68 = 68,
    Element69 = 69,
    Element70 = 70,
    HandleSideArea = 71,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextCB0610 {}; // 原全局：0x00CB0610。

// 回调与内部函数前置声明。

// 原函数：0x004CA410。
// 关联原入口：0x004CA438（共享尾已并入本函数正文）。
static std::int32_t WindowProfileSearch_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口61首次刷结果，距离关闭后仍同步关键词。

// 原函数：0x004CA450。
static std::uint32_t WindowProfileSearch_DrawKeyword(); // 活动输入与关键词显示互斥，返回含NUL的UTF-16码元数。

// 原函数：0x004CA500。
static void WindowProfileSearch_RefreshResults(); // 固定20条结果、三组标题刷新；不重新显示已隐藏的非空行。

// 原函数：0x004CA6F0。
static std::int32_t WindowProfileSearch_HandleSearchButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 搜索/上页/下页双层转义请求。

// 原函数：0x004CA9C0。
static std::int32_t WindowProfileSearch_SelectResult(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 按行发查看请求，随后重读窗口62高亮。

// 原函数：0x004CAB60。
static void WindowProfileSearch_Release(WindowInstance&); // 先清对话类型，再标记62/63。

// 原函数：0x004CAB90。
static std::int32_t WindowProfileSearch_EditKeyword(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 固定元素7关键词编辑入口。

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gProfileSearchInputPrototype580A80 { // 原全局：0x00580A80。
    {{&cg::ui::detail::gInputContextCB0610}, WindowInputReferenceKind::Context},
    20, 370, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowGraphicData gWindowGraphic578BE8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578BE8。

static WindowGraphicData gWindowGraphic578C00 {0x3C11B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C00。

static WindowGraphicData gWindowGraphic578C18 {0x3C11C, 0, 0, 0, 0, 0x8FFFFFFFu}; // 原全局：0x00578C18。

static WindowGraphicData gWindowGraphic578C30 {0x3C12A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C30。

static WindowGraphicData gWindowGraphic578C48 {0x3C11D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C48。

static WindowGraphicData gWindowGraphic578C60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C60。

static WindowGraphicData gWindowGraphic578C78 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C78。

static WindowGraphicData gWindowGraphic578C90 {0x3C120, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C90。

static WindowTextPrototype gWindowText578CA8 { // 原全局：0x00578CA8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"类别", // 原常量参考：0x00585D70。
};

static WindowTextPrototype gWindowText578CB0 { // 原全局：0x00578CB0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"讯息", // 原常量参考：0x00585D64。
};

static WindowTextPrototype gWindowText578CB8 { // 原全局：0x00578CB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00585D5C。
};

static WindowTextPrototype gWindowText578CC0 { // 原全局：0x00578CC0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"介绍内容", // 原常量参考：0x00585D54。
};

static WindowElementPrototype gWindowElements578CC8[71] { // 原全局：0x00578CC8。
    {WindowElementType_Graphic, 284, 9, 10, 10, 1, {.graphic = &gWindowGraphic578BE8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowProfileSearchElementId::ButtonClose)},
    {WindowElementType_Text, 120, 30, 0, 0, 1, {.text = &gWindowText578CA8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element2)},
    {WindowElementType_Graphic, 140, 8, 0, 0, 1, {.graphic = &gWindowGraphic578C48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element3)},
    {WindowElementType_Graphic, 190, 367, 55, 20, 1, {.graphic = &gWindowGraphic578C90}, &WindowProfileSearch_HandleSearchButtons, -1, static_cast<ElementId>(WindowProfileSearchElementId::HandleSearchButtons)},
    {WindowElementType_Graphic, 250, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C60}, &WindowProfileSearch_HandleSearchButtons, -1, static_cast<ElementId>(WindowProfileSearchElementId::HandleSearchButtons_5)},
    {WindowElementType_Graphic, 270, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C78}, &WindowProfileSearch_HandleSearchButtons, -1, static_cast<ElementId>(WindowProfileSearchElementId::HandleSearchButtons_6)},
    {WindowElementType_Text, 20, 370, 0, 0, 1, {.text = &gWindowText578CC0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element7)},
    {WindowElementType_Input, 20, 370, 168, 14, 1, {.unusedValue = 0}, &WindowProfileSearch_EditKeyword, -1, static_cast<ElementId>(WindowProfileSearchElementId::EditKeyword)},
    {WindowElementType_Text, 20, 47, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element9)},
    {WindowElementType_Text, 20, 63, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element10)},
    {WindowElementType_Text, 20, 79, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element11)},
    {WindowElementType_Text, 20, 95, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element12)},
    {WindowElementType_Text, 20, 111, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element13)},
    {WindowElementType_Text, 20, 127, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element14)},
    {WindowElementType_Text, 20, 143, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element15)},
    {WindowElementType_Text, 20, 159, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element16)},
    {WindowElementType_Text, 20, 175, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element17)},
    {WindowElementType_Text, 20, 191, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element18)},
    {WindowElementType_Text, 20, 207, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element19)},
    {WindowElementType_Text, 20, 223, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element20)},
    {WindowElementType_Text, 20, 239, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element21)},
    {WindowElementType_Text, 20, 255, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element22)},
    {WindowElementType_Text, 20, 271, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element23)},
    {WindowElementType_Text, 20, 287, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element24)},
    {WindowElementType_Text, 20, 303, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element25)},
    {WindowElementType_Text, 20, 319, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element26)},
    {WindowElementType_Text, 20, 335, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element27)},
    {WindowElementType_Text, 20, 351, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element28)},
    {WindowElementType_Text, 195, 47, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element29)},
    {WindowElementType_Text, 195, 63, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element30)},
    {WindowElementType_Text, 195, 79, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element31)},
    {WindowElementType_Text, 195, 95, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element32)},
    {WindowElementType_Text, 195, 111, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element33)},
    {WindowElementType_Text, 195, 127, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element34)},
    {WindowElementType_Text, 195, 143, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element35)},
    {WindowElementType_Text, 195, 159, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element36)},
    {WindowElementType_Text, 195, 175, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element37)},
    {WindowElementType_Text, 195, 191, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element38)},
    {WindowElementType_Text, 195, 207, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element39)},
    {WindowElementType_Text, 195, 223, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element40)},
    {WindowElementType_Text, 195, 239, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element41)},
    {WindowElementType_Text, 195, 255, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element42)},
    {WindowElementType_Text, 195, 271, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element43)},
    {WindowElementType_Text, 195, 287, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element44)},
    {WindowElementType_Text, 195, 303, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element45)},
    {WindowElementType_Text, 195, 319, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element46)},
    {WindowElementType_Text, 195, 335, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element47)},
    {WindowElementType_Text, 195, 351, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element48)},
    {WindowElementType_Graphic, 15, 45, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult)},
    {WindowElementType_Graphic, 15, 61, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_50)},
    {WindowElementType_Graphic, 15, 77, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_51)},
    {WindowElementType_Graphic, 15, 93, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_52)},
    {WindowElementType_Graphic, 15, 109, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_53)},
    {WindowElementType_Graphic, 15, 125, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_54)},
    {WindowElementType_Graphic, 15, 141, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_55)},
    {WindowElementType_Graphic, 15, 157, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_56)},
    {WindowElementType_Graphic, 15, 173, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_57)},
    {WindowElementType_Graphic, 15, 189, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_58)},
    {WindowElementType_Graphic, 15, 205, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_59)},
    {WindowElementType_Graphic, 15, 221, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_60)},
    {WindowElementType_Graphic, 15, 237, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_61)},
    {WindowElementType_Graphic, 15, 253, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_62)},
    {WindowElementType_Graphic, 15, 269, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_63)},
    {WindowElementType_Graphic, 15, 285, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_64)},
    {WindowElementType_Graphic, 15, 301, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_65)},
    {WindowElementType_Graphic, 15, 317, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_66)},
    {WindowElementType_Graphic, 15, 333, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_67)},
    {WindowElementType_Graphic, 15, 349, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult, -1, static_cast<ElementId>(WindowProfileSearchElementId::SelectResult_68)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic578C00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element69)},
    {WindowElementType_Graphic, 14, 27, 0, 0, 1, {.graphic = &gWindowGraphic578C18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileSearchElementId::Element70)},
    {WindowElementType_EventArea, 300, 0, 30, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowProfileSearchElementId::HandleSideArea)},
};

// 原型：窗口61；原全局：0x0054EDC0。
WindowPrototype gWindowPrototypeProfileSearch {0, 4, 5, 20, 305, 398, 0x80000080u, 71, gWindowElements578CC8, &WindowProfileSearch_Update, &WindowRuntime_DrawDefault, &WindowProfileSearch_Release}; // 61: 54EDC0

// 窗口函数实现。

// 原函数：0x004CA500。
static void WindowProfileSearch_RefreshResults()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const auto& search = world::detail::gProfileSearch105C398;
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto& record = search.records[index];
        if (record.used != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 8)); indexedElement) indexedElement->data.text->text = record.text10C;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index + 28)); indexedElement2) indexedElement2->data.text->text = record.text04;
        } else {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 8)); indexedElement) indexedElement->visible = 0;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index + 28)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(index + 48)); indexedElement3) indexedElement3->visible = 0;
        }
    }
    const std::int32_t group = search.value00;
    switch (group) {
    case 0:
    case 1:
    case 2: {
        const wchar_t* const title = world::detail::ReadCategoryText105FA24(group, search.value04);
        if (auto* const element2 = currentWindow->FindElementById(WindowProfileSearchElementId::Element2); element2) element2->data.text->text = title;
        auto* const element3 = currentWindow->FindElementById(WindowProfileSearchElementId::Element3);
        if (element3) {
            element3->data.graphic->imageNo =
                group == 0 ? 246045 : (group == 1 ? 0 : 246046);
        }
        break;
    }
    default:
        break;
    }
}

// 原函数：0x004CA450。
static std::uint32_t WindowProfileSearch_DrawKeyword()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (input::gCurrentInputData == &cg::ui::detail::gInputContextCB0610) {
        if (auto* const element7 = currentWindow->FindElementById(WindowProfileSearchElementId::Element7); element7) element7->visible = 0;
        WindowInstance* const window = currentWindow;
        static_cast<void>(WindowInput_ActivateElement(
            &gProfileSearchInputPrototype580A80, window->x, window->y, 1));
        const std::wstring& text = input::ContextText(*gProfileSearchInputPrototype580A80.input.context);
        const auto length = static_cast<std::uint32_t>(text.size() + 1);
        gProfileSearchKeyword1075988 = text;
        return length;
    }
    if (auto* const element7 = currentWindow->FindElementById(WindowProfileSearchElementId::Element7); element7) element7->visible = 1;
    auto* const element72 = currentWindow->FindElementById(WindowProfileSearchElementId::Element7);
    if (element72) {
        std::wstring& text = element72->data.text->text;
        const auto length = static_cast<std::uint32_t>(
            gProfileSearchKeyword1075988.size() + 1);
        text = gProfileSearchKeyword1075988;
        return length;
    }
    return 0;
}

// 原函数：0x004CA410。
// 关联原入口：0x004CA438（共享尾已并入本函数正文）。
static std::int32_t WindowProfileSearch_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowProfileSearch_RefreshResults();
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowProfileSearch_DrawKeyword());
    return 1;
}

// 原函数：0x004CAB60。
static void WindowProfileSearch_Release(WindowInstance& releasedWindow)
{
    world::gCurrentDialogType_581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(*releasedWindow.ownerSession, 3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(*releasedWindow.ownerSession, 4));
}

// 原函数：0x004CAB90。
static std::int32_t WindowProfileSearch_EditKeyword(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"请输入关键字";
        consumed = 1;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const editKeywordElement = window->FindElementById(WindowProfileSearchElementId::EditKeyword);
        if (editKeywordElement) {
            if (gFocusedElementDataD45F94 != editKeywordElement->data.input) {
                gProfileSearchInputPrototype580A80.text = gProfileSearchKeyword1075988;
                static_cast<void>(WindowInput_ActivateElement(
                    &gProfileSearchInputPrototype580A80, window->x, window->y, 0));
                input::SetContext_44B330(&cg::ui::detail::gInputContextCB0610);
                auto* const editKeywordElement2 = currentWindow->FindElementById(WindowProfileSearchElementId::EditKeyword);
                if (editKeywordElement2) {
                    gFocusedElementDataD45F94 = editKeywordElement2->data.input;
                }
                auto* const editKeywordElement3 = currentWindow->FindElementById(WindowProfileSearchElementId::EditKeyword);
                if (editKeywordElement3) {
                    editKeywordElement3->data.input->context =
                        &cg::ui::detail::gInputContextCB0610;
                }
            }
        }
        return 1;
    }
    return consumed;
}

// 原函数：0x004CA6F0。
static std::int32_t WindowProfileSearch_HandleSearchButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (elementId < static_cast<ElementId>(WindowProfileSearchElementId::HandleSearchButtons) || elementId > static_cast<ElementId>(WindowProfileSearchElementId::HandleSearchButtons_6)) {
        return 0;
    }
    constexpr std::array<std::int32_t, 3> normalImages {246048, 243030, 243033};
    constexpr std::array<std::int32_t, 3> hoverImages {246047, 243032, 243035};
    constexpr std::array<std::int32_t, 3> commands {1, 16, 32};
    constexpr const wchar_t* hints[] {
        L"使用关键字检索",
        L"上一页",
        L"下一页",
    };
    const std::int32_t position = index - 3;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = hoverImages[position];
            gHoverTextD68AE8 = hints[position];
            consumed = 1;
        } else {
            graphic->imageNo = normalImages[position];
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
    }
    const std::wstring keyword = input::EscapeText(gProfileSearchKeyword1075988, 256);
    const auto& search = world::detail::gProfileSearch105C398;
    const std::int32_t entry = search.value04;
    const std::int32_t group = search.value00;
    const std::wstring combined = std::to_wstring(group) + L"|" + std::to_wstring(entry) + L"|" + keyword;
    const std::wstring packet = input::EscapeTextAndNotify(combined, 255);
    const std::int32_t target = world::gDialogTargetD2DE6C;
    const std::int32_t request = world::gDialogRequestCC21E0;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, commands[position], packet));
    return 1;
}

// 原函数：0x004CA9C0。
static std::int32_t WindowProfileSearch_SelectResult(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t position = index - 48;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"浏览此名玩家的资料表";
        consumed = 1;
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 246059;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 246060;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        const auto& record = world::detail::gProfileSearch105C398.records[position];
        const std::int32_t key = record.value104;
        const std::wstring combined = std::to_wstring(key) + L"|" + record.text04;
        const std::wstring packet = input::EscapeTextAndNotify(combined, 255);
        const std::int32_t target = world::gDialogTargetD2DE6C;
        const std::int32_t request = world::gDialogRequestCC21E0;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 4, packet));
        consumed = 1;
    }
    if (WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_ProfileView].window != nullptr) {
        const std::int32_t key = world::detail::gViewedProfile105ECE0.value00;
        if (key == world::detail::gProfileSearch105C398.records[position].value104) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 246058;
        }
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
