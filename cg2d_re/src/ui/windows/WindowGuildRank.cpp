#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowGuildRankElementId : ElementId {
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
    ButtonClose = 12,
    ButtonOk = 13,
    Element14 = 14,
    HandleVerticalScroll = 15,
    HandleScrollButtons = 16,
    HandleScrollButtons_17 = 17,
    Scroll = 18,
    Element19 = 19,
    SelectRow = 20,
    SelectRow_21 = 21,
    SelectRow_22 = 22,
    SelectRow_23 = 23,
    SelectRow_24 = 24,
    SelectRow_25 = 25,
    SelectRow_26 = 26,
    SelectRow_27 = 27,
    SelectRow_28 = 28,
    SelectRow_29 = 29,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口55只初始化滚动位置。

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(WindowInstance& callbackWindow, std::int32_t unused); // 刷新/公共绘制后反求滚动，再画九宫格。

// 原函数：0x004A4220。
static std::int32_t WindowGuildRank_SelectRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 职级行选择及当前选择图号。

// 原函数：0x004A42C0。
static std::int32_t WindowGuildRank_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 固定入口窗口关闭、声音后的图形更新。

// 原函数：0x004A4370。
static std::int32_t WindowGuildRank_ButtonOk(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // uHb设置后重取窗口关闭，返回仅由hover决定。

// 原函数：0x004A4420。
static std::int32_t WindowGuildRank_HandleScrollButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上下滚动先发声，只修各自一侧边界。

// 原函数：0x004A4540。
static std::uint32_t WindowGuildRank_Refresh(WindowInstance* callbackWindow); // 十行职级/成员姓名刷新，返回含NUL姓名长度。

// 原函数：0x004A45E0。
static std::int32_t WindowGuildRank_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 非零滚轮推进，恒返回0。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56F7A8 {0x3B59C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7A8。

static WindowGraphicData gWindowGraphic56F7C0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7C0。

static WindowGraphicData gWindowGraphic56F7D8 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7D8。

static WindowGraphicData gWindowGraphic56F7F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7F0。

static WindowGraphicData gWindowGraphic56F808 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F808。

static WindowGraphicData gWindowGraphic56F820 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F820。

static WindowGraphicData gWindowGraphic56F838 {0x3BAA3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F838。

static WindowTextPrototype gWindowText56F850 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族称号"}; // 原全局：0x0056F850。 // 原常量参考：0x00585ED8。

static WindowTextPrototype gWindowText56F858 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"成员名称"}; // 原全局：0x0056F858。 // 原常量参考：0x00585ECC。

static std::uint32_t gWindowMouseFlagsD67860 = 0; // 原全局：0x00D67860。

static WindowElementPrototype gWindowElements56F860[29] { // 原全局：0x0056F860。
    {WindowElementType_Text, 7, 21, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element1)},
    {WindowElementType_Text, 7, 37, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element2)},
    {WindowElementType_Text, 7, 53, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element3)},
    {WindowElementType_Text, 7, 69, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element4)},
    {WindowElementType_Text, 7, 85, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element5)},
    {WindowElementType_Text, 7, 101, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element6)},
    {WindowElementType_Text, 7, 117, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element7)},
    {WindowElementType_Text, 7, 133, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element8)},
    {WindowElementType_Text, 7, 149, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element9)},
    {WindowElementType_Text, 7, 165, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element10)},
    {WindowElementType_Text, 8, 181, 0, 0, 1, {.text = &gWindowText56F858}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element11)},
    {WindowElementType_Graphic, 150, 181, 49, 17, 1, {.graphic = &gWindowGraphic56F7C0}, &WindowGuildRank_ButtonClose, -1, static_cast<ElementId>(WindowGuildRankElementId::ButtonClose)},
    {WindowElementType_Graphic, 206, 181, 32, 17, 1, {.graphic = &gWindowGraphic56F7D8}, &WindowGuildRank_ButtonOk, -1, static_cast<ElementId>(WindowGuildRankElementId::ButtonOk)},
    {WindowElementType_Graphic, 224, 0, 0, 14, 1, {.graphic = &gWindowGraphic56F7F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element14)},
    {WindowElementType_MouseArea, 225, 33, 11, 135, 1, {.mouseFlags = &gWindowMouseFlagsD67860}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowGuildRankElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 224, 22, 11, 11, 1, {.graphic = &gWindowGraphic56F808}, &WindowGuildRank_HandleScrollButtons, -1, static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons)},
    {WindowElementType_Graphic, 224, 168, 11, 11, 1, {.graphic = &gWindowGraphic56F820}, &WindowGuildRank_HandleScrollButtons, -1, static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons_17)},
    {WindowElementType_EventArea, 0, 0, 243, 206, 1, {.unusedValue = 0}, &WindowGuildRank_Scroll, -1, static_cast<ElementId>(WindowGuildRankElementId::Scroll)},
    {WindowElementType_Graphic, 221, 18, 0, 0, 1, {.graphic = &gWindowGraphic56F838}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildRankElementId::Element19)},
    {WindowElementType_Graphic, 5, 19, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow)},
    {WindowElementType_Graphic, 5, 35, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_21)},
    {WindowElementType_Graphic, 5, 51, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_22)},
    {WindowElementType_Graphic, 5, 67, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_23)},
    {WindowElementType_Graphic, 5, 83, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_24)},
    {WindowElementType_Graphic, 5, 99, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_25)},
    {WindowElementType_Graphic, 5, 115, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_26)},
    {WindowElementType_Graphic, 5, 131, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_27)},
    {WindowElementType_Graphic, 5, 147, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_28)},
    {WindowElementType_Graphic, 5, 163, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow, -1, static_cast<ElementId>(WindowGuildRankElementId::SelectRow_29)},
};

// 原型：窗口55；原全局：0x0054E9C0。
WindowPrototype gWindowPrototypeGuildRank {0, 4, 100, 100, 243, 206, 0x80000080u, 29, gWindowElements56F860, &WindowGuildRank_Update, &WindowGuildRank_Draw, &WindowRuntime_ReleaseDefault}; // 55: 54E9C0

// 窗口函数实现。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    if (value == -1) {
        business.mailRankScrollCA0474 = 0;
    }
    return 1;
}

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(WindowGuildRank_Refresh(&callbackWindow));
    WindowRuntime_DrawCurrent(callbackWindow);
    auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowGuildRankElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        business.mailRankScrollCA0474 = WindowScroll_ReadPosition(handleVerticalScrollElement, 21);
    }
    WindowInstance* const window = currentWindow;
    const std::int16_t height = static_cast<std::int16_t>(window->height);
    const std::int16_t width = static_cast<std::int16_t>(window->width);
    const std::int16_t y = static_cast<std::int16_t>(window->y);
    const std::int16_t x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(0x3BE3E, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004A4220。
static std::int32_t WindowGuildRank_SelectRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"选择此称号。" /* 原常量参考：0x005876F4。 */;
        consumed = 1;
    }
    const std::int32_t scroll = business.mailRankScrollCA0474;
    if ((state & MouseFlag_LDownEvent) != 0) {
        business.mailRankCF4A14 = scroll + index - 19;
        consumed = 1;
    }
    const std::int32_t selected = business.mailRankCF4A14;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = selected == scroll + index - 19 ? 0x3B59E : 0x3B59C;
    }
    return consumed;
}

// 原函数：0x004A42C0。
static std::int32_t WindowGuildRank_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = kSettingsClose55331C;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B562;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B564;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B563;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004A4370。
static std::int32_t WindowGuildRank_ButtonOk(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B570;
            uiFrame.hoverText = L"设定已选择的称号。" /* 原常量参考：0x00587704。 */;
            consumed = 1;
        } else {
            graphic->imageNo = 0x3B56E;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t rank = business.mailRankCF4A14;
            const std::int32_t key = business.mailMemberKeyD45F88;
            const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
            static_cast<void>(callbackWindow.ownerSession->Connection().SendGuildMemberRankChange(key, rank + 1));
            WindowInstance* const window = currentWindow;
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004A4420。
static std::int32_t WindowGuildRank_HandleScrollButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (elementId != static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons) && elementId != static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons_17)) {
        return 0;
    }
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    const std::int32_t idle = elementId == static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons) ? 0x3B592 : 0x3B595;
    if (graphic) graphic->imageNo = hover ? idle + 2 : idle;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        std::int32_t position;
        if (elementId == static_cast<ElementId>(WindowGuildRankElementId::HandleScrollButtons)) {
            const std::int32_t previous = business.mailRankScrollCA0474;
            business.mailRankScrollCA0474 = previous - 1;
            if (previous - 1 < 0) {
                business.mailRankScrollCA0474 = 0;
            }
            position = business.mailRankScrollCA0474;
        } else {
            position = business.mailRankScrollCA0474 + 1;
            business.mailRankScrollCA0474 = position;
            if (position > 21) {
                position = 21;
                business.mailRankScrollCA0474 = position;
            }
        }
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowGuildRankElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            WindowScroll_SetPosition(handleVerticalScrollElement, 21, position);
        }
    }
    return hover ? 1 : 0;
}

// 原函数：0x004A4540。
static std::uint32_t WindowGuildRank_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int32_t scroll = business.mailRankScrollCA0474;
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(row));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            const wchar_t* const rank = cg::world::GetGuildRankText(*callbackWindow->ownerSession, scroll + row + 1);
            data->text = rank;
        }
    }
    const std::int32_t member = business.mailMemberIndexCB1D18;
    auto* const element11 = currentWindow->FindElementById(WindowGuildRankElementId::Element11);
    if (element11) {
        auto* const data = element11->data.text;
        const wchar_t* const name = callbackWindow->ownerSession->WorldScene().guild.orderedRecords[member].name18.c_str();
        const std::uint32_t length = static_cast<std::uint32_t>(cg::text::ToGbk(name).size() + 1);
        data->text = name;
        return length;
    }
    return 0;
}

// 原函数：0x004A45E0。
static std::int32_t WindowGuildRank_Scroll(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        if (wheel != 0) {
            WindowInstance* const window = currentWindow;
            const std::int32_t position = business.mailRankScrollCA0474;
            auto* const handleVerticalScrollElement = window->FindElementById(WindowGuildRankElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement) {
                business.mailRankScrollCA0474 = WindowScroll_ApplyWheel(handleVerticalScrollElement, position, 21, wheel);
            }
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
