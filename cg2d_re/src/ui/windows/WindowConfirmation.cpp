#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(std::int32_t value); // 窗口47初始化：显隐、文本、初始化回调。

// 原函数：0x004AE620。
static std::int32_t WindowConfirmation_ButtonOk(std::int32_t index, std::int32_t state); // CTRL确认，动作槽0。

// 原函数：0x004AE690。
static std::int32_t WindowConfirmation_ButtonCancel(std::int32_t index, std::int32_t state); // 动作槽1，取消音效55。

// 原函数：0x004AE700。
static std::int32_t WindowConfirmation_ButtonActionTwo(std::int32_t index, std::int32_t state); // 动作槽2，音效51。

// 原函数：0x004AE770。
static std::int32_t WindowConfirmation_ButtonActionThree(std::int32_t index, std::int32_t state); // 动作槽3，音效55。

// 原函数：0x004AE7E0。
static std::int32_t WindowConfirmation_ButtonActionFour(std::int32_t index, std::int32_t state); // 动作槽4，音效51。

// 原函数：0x004AE850。
static std::int32_t WindowConfirmation_ButtonActionFive(std::int32_t index, std::int32_t state); // 动作槽5，音效51。

// 原函数：0x004AE8C0。
static WindowInstance* WindowConfirmation_RequestClose(); // 固定窗口47标关闭，返回同一实例。

// 原函数：0x004AE8D0。
static wchar_t* WindowConfirmation_RefreshText(); // 十一文本清理与宽字符LF分行刷新。

// 原函数：0x004AE9C0。
static std::int32_t WindowConfirmation_RefreshButtons(); // 六按钮逐项显隐，原最终偏移216。

// 原函数：0x004AEA10。
static std::int32_t WindowConfirmation_PadText(std::wstring& destination, std::wstring_view text); // 按原GBK列宽单位补空格，正文保持UTF-16。

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(std::int32_t value);

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
    {WindowElementType_Text, 30, 10, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 23, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 36, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 49, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 62, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 75, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 88, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 101, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 114, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 127, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 140, 0, 0, 0, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 45, 130, 66, 17, 1, {.graphic = &gWindowGraphic570638}, &WindowConfirmation_ButtonOk},
    {WindowElementType_Graphic, 111, 130, 66, 17, 1, {.graphic = &gWindowGraphic570650}, &WindowConfirmation_ButtonCancel},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570668}, &WindowConfirmation_ButtonActionTwo},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic570680}, &WindowConfirmation_ButtonActionThree},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570698}, &WindowConfirmation_ButtonActionFour},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic5706B0}, &WindowConfirmation_ButtonActionFive},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570560}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic570578}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic570590}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 215, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 156, 0, 0, 1, {.graphic = &gWindowGraphic570608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic570620}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
};

// 原型：窗口47；原全局：0x0054EA80。
WindowPrototype gWindowPrototypeConfirmation {0, 4, 209, 159, 222, 162, 0x80010101u, 34, gWindowElements5706D0, &WindowConfirmation_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 47: 54EA80

// 窗口函数实现。

static std::int32_t WindowConfirmation_HandleButton(
    const std::int32_t index, const std::int32_t state, const std::int32_t idleImage,
    WindowElementCallback* const callbackSlot, const std::int32_t sound, const bool requiresControl)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = idleImage;
    if ((state & 1) != 0) {
        graphic->imageNo = idleImage + 2;
    }
    if ((state & 2) == 0 || (requiresControl && (platform::gKeyStates[17] & 2) == 0)) {
        return 0;
    }
    static_cast<void>(WindowConfirmation_RequestClose());
    const WindowElementCallback callback = *callbackSlot;
    if (callback != nullptr) {
        static_cast<void>(callback(index, state));
    }
    static_cast<void>(audio::PlaySe_52C450(sound, 320));
    return 1;
}

// 原函数：0x004AE5F0。
static std::int32_t WindowConfirmation_Update(const std::int32_t value)
{
    if (value == -1) {
        static_cast<void>(WindowConfirmation_RefreshButtons());
        static_cast<void>(WindowConfirmation_RefreshText());
        const WindowCallback callback = gWindow47Runtime.initializeCC4250;
        if (callback != nullptr) {
            static_cast<void>(callback(-1));
        }
    }
    return 1;
}

// 原函数：0x004AE620。
static std::int32_t WindowConfirmation_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE4B, &gWindow47Runtime.actionsCC4254[0], 51, true);
}

// 原函数：0x004AE690。
static std::int32_t WindowConfirmation_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE4E, &gWindow47Runtime.actionsCC4254[1], 55, false);
}

// 原函数：0x004AE700。
static std::int32_t WindowConfirmation_ButtonActionTwo(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3B568, &gWindow47Runtime.actionsCC4254[2], 51, false);
}

// 原函数：0x004AE770。
static std::int32_t WindowConfirmation_ButtonActionThree(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE48, &gWindow47Runtime.actionsCC4254[3], 55, false);
}

// 原函数：0x004AE7E0。
static std::int32_t WindowConfirmation_ButtonActionFour(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE54, &gWindow47Runtime.actionsCC4254[4], 51, false);
}

// 原函数：0x004AE850。
static std::int32_t WindowConfirmation_ButtonActionFive(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE51, &gWindow47Runtime.actionsCC4254[5], 51, false);
}

// 原函数：0x004AE8C0。
static WindowInstance* WindowConfirmation_RequestClose()
{
    WindowInstance* const window = gWindowStates[WindowId_Confirmation].window;
    window->state |= 0x8000u;
    return window;
}

// 原函数：0x004AE8D0。
static wchar_t* WindowConfirmation_RefreshText()
{
    for (std::int32_t index = 0; index < 11; ++index) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
        data->text = L"";
    }
    std::wstring text = gWindow47Runtime.textCC426C;
    std::wstring line;
    wchar_t* context = nullptr;
    wchar_t* token = std::wcstok(text.data(), L"\n" /* 原常量参考：0x005531C4。 */, &context);
    std::int32_t row = 0;
    while (token != nullptr) {
        static_cast<void>(WindowConfirmation_PadText(line, token));
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[row].data);
        data->text = line;
        token = std::wcstok(nullptr, L"\n" /* 原常量参考：0x005531C4。 */, &context);
        ++row;
    }
    return token;
}

// 原函数：0x004AE9C0。
static std::int32_t WindowConfirmation_RefreshButtons()
{
    std::int32_t mask = gWindow47Runtime.visibleMaskCC4240;
    for (std::int32_t index = 11; index < 17; ++index) {
        gCurrentWindow->elements[index].visible = 0;
        if ((mask & 1) != 0) {
            gCurrentWindow->elements[index].visible = 1;
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
