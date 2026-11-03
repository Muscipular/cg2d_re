#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowConfirmationElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    ButtonOk = 12,
    ButtonCancel = 13,
    ButtonActionTwo = 14,
    ButtonActionThree = 15,
    ButtonActionFour = 16,
    ButtonActionFive = 17,
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
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口47初始化：显隐、文本、初始化回调。

// 原函数：0x004AE620。
static std::int32_t WindowConfirmation_ButtonOk(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // CTRL确认，动作槽0。

// 原函数：0x004AE690。
static std::int32_t WindowConfirmation_ButtonCancel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动作槽1，取消音效55。

// 原函数：0x004AE700。
static std::int32_t WindowConfirmation_ButtonActionTwo(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动作槽2，音效51。

// 原函数：0x004AE770。
static std::int32_t WindowConfirmation_ButtonActionThree(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动作槽3，音效55。

// 原函数：0x004AE7E0。
static std::int32_t WindowConfirmation_ButtonActionFour(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动作槽4，音效51。

// 原函数：0x004AE850。
static std::int32_t WindowConfirmation_ButtonActionFive(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动作槽5，音效51。

// 原函数：0x004AE8C0。
static WindowInstance* WindowConfirmation_RequestClose(); // 固定窗口47标关闭，返回同一实例。

// 原函数：0x004AE8D0。
static wchar_t* WindowConfirmation_RefreshText(); // 十一文本清理与宽字符LF分行刷新。

// 原函数：0x004AE9C0。
static std::int32_t WindowConfirmation_RefreshButtons(); // 六按钮逐项显隐，原最终偏移216。

// 原函数：0x004AEA10。
static std::int32_t WindowConfirmation_PadText(std::wstring& destination, std::wstring_view text); // 按原GBK列宽单位补空格，正文保持UTF-16。

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic570560 {0x3BE34, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570560。

static WindowGraphicData gWindowGraphic570578 {0x3BE35, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570578。

static WindowGraphicData gWindowGraphic570590 {0x3BE36, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570590。

static WindowGraphicData gWindowGraphic5705A8 {0x3BE37, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705A8。

static WindowGraphicData gWindowGraphic5705C0 {0x3BE38, 0, 0, 74, 54, 0x80FFFFFFu}; // 原全局：0x005705C0。

static WindowGraphicData gWindowGraphic5705D8 {0x3BE39, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705D8。

static WindowGraphicData gWindowGraphic5705F0 {0x3BE3A, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705F0。

static WindowGraphicData gWindowGraphic570608 {0x3BE3B, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570608。

static WindowGraphicData gWindowGraphic570620 {0x3BE3C, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570620。

static WindowGraphicData gWindowGraphic570638 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570638。

static WindowGraphicData gWindowGraphic570650 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570650。

static WindowGraphicData gWindowGraphic570668 {0x3BE4B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570668。

static WindowGraphicData gWindowGraphic570680 {0x3BE4E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570680。

static WindowGraphicData gWindowGraphic570698 {0x3BE51, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570698。

static WindowGraphicData gWindowGraphic5706B0 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005706B0。

static WindowTextPrototype gWindowText5706C8 {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"content"}; // 原全局：0x005706C8。 // 原常量参考：0x00585E9C。

static WindowElementPrototype gWindowElements5706D0[34] { // 原全局：0x005706D0。
    {WindowElementType_Text, 30, 10, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element1)},
    {WindowElementType_Text, 30, 23, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element2)},
    {WindowElementType_Text, 30, 36, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element3)},
    {WindowElementType_Text, 30, 49, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element4)},
    {WindowElementType_Text, 30, 62, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element5)},
    {WindowElementType_Text, 30, 75, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element6)},
    {WindowElementType_Text, 30, 88, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element7)},
    {WindowElementType_Text, 30, 101, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element8)},
    {WindowElementType_Text, 30, 114, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element9)},
    {WindowElementType_Text, 30, 127, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element10)},
    {WindowElementType_Text, 30, 140, 0, 0, 0, {.text = &gWindowText5706C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element11)},
    {WindowElementType_Graphic, 45, 130, 66, 17, 1, {.graphic = &gWindowGraphic570638}, &WindowConfirmation_ButtonOk, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonOk)},
    {WindowElementType_Graphic, 111, 130, 66, 17, 1, {.graphic = &gWindowGraphic570650}, &WindowConfirmation_ButtonCancel, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonCancel)},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570668}, &WindowConfirmation_ButtonActionTwo, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonActionTwo)},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic570680}, &WindowConfirmation_ButtonActionThree, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonActionThree)},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570698}, &WindowConfirmation_ButtonActionFour, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonActionFour)},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic5706B0}, &WindowConfirmation_ButtonActionFive, -1, static_cast<ElementId>(WindowConfirmationElementId::ButtonActionFive)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570560}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element18)},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic570578}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element19)},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic570590}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element20)},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element21)},
    {WindowElementType_Graphic, 215, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element22)},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element23)},
    {WindowElementType_Graphic, 74, 156, 0, 0, 1, {.graphic = &gWindowGraphic570608}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element24)},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic570620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element25)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element26)},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element27)},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element28)},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element29)},
    {WindowElementType_Graphic, 74, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element30)},
    {WindowElementType_Graphic, 148, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element31)},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element32)},
    {WindowElementType_Graphic, 74, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element33)},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowConfirmationElementId::Element34)},
};

// 原型：窗口47；原全局：0x0054EA80。
WindowPrototype gWindowPrototypeConfirmation {0, 4, 209, 159, 222, 162, 0x80010101u, 34, gWindowElements5706D0, &WindowConfirmation_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 47: 54EA80

// 窗口函数实现。

static std::int32_t WindowConfirmation_HandleButton(
    WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state, const std::int32_t idleImage,
    WindowElementCallback* const callbackSlot, const std::int32_t sound, const bool requiresControl)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = idleImage;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = idleImage + 2;
        }
    }
    if ((state & MouseFlag_LDownEvent) == 0 || (requiresControl && (platform::gKeyStates[17] & 2) == 0)) {
        return 0;
    }
    static_cast<void>(WindowConfirmation_RequestClose());
    const WindowElementCallback callback = *callbackSlot;
    if (callback != nullptr) {
        static_cast<void>(callback(callbackWindow, elementId, state));
    }
    static_cast<void>(audio::PlaySe_52C450(sound, 320));
    return 1;
}

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    if (value == -1) {
        static_cast<void>(WindowConfirmation_RefreshButtons());
        static_cast<void>(WindowConfirmation_RefreshText());
        const WindowCallback callback = gWindow47Runtime.initializeCC4250;
        if (callback != nullptr) {
            static_cast<void>(callback(callbackWindow, -1));
        }
    }
    return 1;
}

// 原函数：0x004AE620。
static std::int32_t WindowConfirmation_ButtonOk(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3BE4B, &gWindow47Runtime.actionsCC4254[0], 51, true);
}

// 原函数：0x004AE690。
static std::int32_t WindowConfirmation_ButtonCancel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3BE4E, &gWindow47Runtime.actionsCC4254[1], 55, false);
}

// 原函数：0x004AE700。
static std::int32_t WindowConfirmation_ButtonActionTwo(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3B568, &gWindow47Runtime.actionsCC4254[2], 51, false);
}

// 原函数：0x004AE770。
static std::int32_t WindowConfirmation_ButtonActionThree(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3BE48, &gWindow47Runtime.actionsCC4254[3], 55, false);
}

// 原函数：0x004AE7E0。
static std::int32_t WindowConfirmation_ButtonActionFour(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3BE54, &gWindow47Runtime.actionsCC4254[4], 51, false);
}

// 原函数：0x004AE850。
static std::int32_t WindowConfirmation_ButtonActionFive(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(callbackWindow, elementId, state, 0x3BE51, &gWindow47Runtime.actionsCC4254[5], 51, false);
}

// 原函数：0x004AE8C0。
static WindowInstance* WindowConfirmation_RequestClose()
{
    WindowInstance* const window = WindowRuntime_States()[WindowId_Confirmation].window;
    window->state |= 0x8000u;
    return window;
}

// 原函数：0x004AE8D0。
static wchar_t* WindowConfirmation_RefreshText()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return nullptr;
    for (std::int32_t index = 0; index < 11; ++index) {
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(index));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = L"";
        }
    }
    std::wstring text = gWindow47Runtime.textCC426C;
    std::wstring line;
    wchar_t* context = nullptr;
    wchar_t* token = std::wcstok(text.data(), L"\n" /* 原常量参考：0x005531C4。 */, &context);
    std::int32_t row = 0;
    while (token != nullptr) {
        static_cast<void>(WindowConfirmation_PadText(line, token));
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(row));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = line;
        }
        token = std::wcstok(nullptr, L"\n" /* 原常量参考：0x005531C4。 */, &context);
        ++row;
    }
    return token;
}

// 原函数：0x004AE9C0。
static std::int32_t WindowConfirmation_RefreshButtons()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t mask = gWindow47Runtime.visibleMaskCC4240;
    for (std::int32_t index = 11; index < 17; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        if ((mask & 1) != 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->visible = 1;
        }
        mask >>= 1;
    }
    return 216;
}

// 原函数：0x004AEA10。
static std::int32_t WindowConfirmation_PadText(std::wstring& destination, const std::wstring_view text)
{
    std::int32_t length = 0; // 原GBK排版单位：ASCII一列，其余完整字符两列。
    for (std::size_t offset = 0; offset < text.size(); offset = cg::text::Next(text, offset)) {
        length += text[offset] < 0x80 ? 1 : 2;
    }
    const std::int32_t widthUnits = gWindow47Runtime.widthUnitsCC4244;
    std::int32_t spaces = ((widthUnits * 74 - length * 12 - 20) / 2) / 12;
    destination = L"";
    while (spaces > 0) {
        destination += L" " /* 原常量参考：0x0055C9BC。 */;
        --spaces;
    }
    destination += text;
    return 0;
}

} // namespace detail

} // namespace cg::ui
