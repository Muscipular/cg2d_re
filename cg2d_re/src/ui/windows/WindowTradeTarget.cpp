#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowTradeTargetElementId : ElementId {
    Element1 = 1,
    HandleVerticalScroll = 2,
    ButtonPreviousRow = 3,
    ButtonNextRow = 4,
    ButtonPreviousPage = 5,
    ButtonNextPage = 6,
    Scroll = 7,
    ButtonCancel = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    SelectRow = 15,
    SelectRow_16 = 16,
    SelectRow_17 = 17,
    SelectRow_18 = 18,
    SelectRow_19 = 19,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049E990。
static void WindowTradeTarget_Refresh(WindowInstance* callbackWindow, std::int32_t offset); // 逐项复制五行选择文本，重读当前窗口载荷。

// 原函数：0x0049E8F0。
static std::int32_t WindowTradeTarget_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口44初始化与移动关闭。

// 原函数：0x0049EAA0。
static std::int32_t WindowTradeTarget_Draw(WindowInstance& callbackWindow, std::int32_t unused); // 窗口44滚动消费、完整绘制和九宫格。

// 原函数：0x0049EB50。
static std::int32_t WindowTradeTarget_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上一行。

// 原函数：0x0049EC00。
static std::int32_t WindowTradeTarget_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一行。

// 原函数：0x0049ECC0。
static std::int32_t WindowTradeTarget_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上五行。

// 原函数：0x0049ED80。
static std::int32_t WindowTradeTarget_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下五行。

// 原函数：0x0049EE60。
static std::int32_t WindowTradeTarget_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 滚轮像素余量与偏移。

// 原函数：0x0049EEB0。
static std::int32_t WindowTradeTarget_ButtonCancel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 取消提示/关闭及按钮三态。

// 原函数：0x0049EF60。
static std::int32_t WindowTradeTarget_SelectRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 实际选择回调，随后重读窗口关闭。

// 控件载荷、窗口状态、组件表与原型。


static WindowGraphicData gWindowGraphic56CCF0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CCF0。

static WindowGraphicData gWindowGraphic56CD08 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD08。

static WindowGraphicData gWindowGraphic56CD20 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD20。

static WindowGraphicData gWindowGraphic56CD38 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD38。

static WindowGraphicData gWindowGraphic56CD50 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD50。

static WindowGraphicData gWindowGraphic56CD68 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD68。

static WindowTextPrototype gWindowTexts56CD80[5] { // 原全局：0x0056CD80。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
};

static WindowTextPrototype gWindowText56CDA8 { // 原全局：0x0056CDA8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"请选择对象。", // 原常量参考：0x00585F54。
};

static WindowElementPrototype gWindowElements56CDB0[19] { // 原全局：0x0056CDB0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56CCF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element1)},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56CCF0.imageNo)}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowTradeTargetElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56CD08}, &WindowTradeTarget_ButtonPreviousRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56CD20}, &WindowTradeTarget_ButtonNextRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56CD50}, &WindowTradeTarget_ButtonPreviousPage, -1, static_cast<ElementId>(WindowTradeTargetElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56CD68}, &WindowTradeTarget_ButtonNextPage, -1, static_cast<ElementId>(WindowTradeTargetElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTradeTarget_Scroll, -1, static_cast<ElementId>(WindowTradeTargetElementId::Scroll)},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56CD38}, &WindowTradeTarget_ButtonCancel, -1, static_cast<ElementId>(WindowTradeTargetElementId::ButtonCancel)},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56CDA8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element9)},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowTexts56CD80[0]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element10)},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowTexts56CD80[1]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element11)},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowTexts56CD80[2]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element12)},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowTexts56CD80[3]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element13)},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowTexts56CD80[4]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeTargetElementId::Element14)},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::SelectRow)},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::SelectRow_16)},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::SelectRow_17)},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::SelectRow_18)},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow, -1, static_cast<ElementId>(WindowTradeTargetElementId::SelectRow_19)},
};

// 原型：窗口44；原全局：0x0054E8D0。
WindowPrototype gWindowPrototypeTradeTarget {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56CDB0, &WindowTradeTarget_Update, &WindowTradeTarget_Draw, &WindowRuntime_ReleaseDefault}; // 44: 54E8D0

// 窗口函数实现。

// 原函数：0x0049E800。
cg::yobi::YobiAction* WindowTradeTarget_ReceiveTargets(
    session::ClientSession& session,
    const char* const text, const WindowListSelectionCallback callback)
{
    auto& targets = session.UiFrame().native.tradeTarget;
    targets.count = 0;
    for (auto& name : targets.names) {
        name.clear();
    }
    std::array<char, 768> buffer;
    std::int32_t count = 0;
    for (std::int32_t processed = 0; processed < 10 && count < 10; ++processed) {
        if (network::ExtractDelimitedField_52CED0(
                text, '|', processed + 1, 255, buffer.data()) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        count = targets.count;
        targets.names[count] = cg::text::FromGbkWide(buffer.data());
        targets.count = ++count;
    }
    targets.callback = callback;
    const std::int32_t y =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    static_cast<void>(WindowProfile_SaveOrigin(session, x, y));
    return WindowRuntime_CreateAnimation(session, WindowId_TradeTarget, 1, 0, nullptr);
}

// 原函数：0x0049E990。
static void WindowTradeTarget_Refresh(WindowInstance* callbackWindow, const std::int32_t offset)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& targets = callbackWindow->ownerSession->UiFrame().native.tradeTarget;
    for (std::int32_t line = 0; line < 5; ++line) {
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(9 + line));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = targets.names[offset + line];
        }
    }
}

// 原函数：0x0049E8F0。
static std::int32_t WindowTradeTarget_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        const std::int32_t visible = targets.count > 5 ? 1 : 0;
        targets.offset = 0;
        for (std::int32_t index = 0; index < 6; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = visible;
        }
        WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 1, 1) != 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
    }
    return 1;
}

// 原函数：0x0049EAA0。
static std::int32_t WindowTradeTarget_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (auto* const element1 = currentWindow->FindElementById(WindowTradeTargetElementId::Element1); element1) element1->visible = 1;
    if (targets.count - 5 <= 0) {
        if (auto* const element12 = currentWindow->FindElementById(WindowTradeTargetElementId::Element1); element12) element12->visible = 0;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowTradeTargetElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        const auto* const mouse = handleVerticalScrollElement->data.mouse;
        if ((mouse->flags & 1u) != 0) {
            auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowTradeTargetElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement2) {
                targets.offset = WindowScroll_ReadPosition(handleVerticalScrollElement2, targets.count - 5);
            }
            WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    WindowInstance* const window = currentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(
        0x3BE34, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(*callbackWindow.ownerSession, 3));
    return 1;
}

// 原函数：0x0049EB50。
static std::int32_t WindowTradeTarget_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t result = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (targets.offset > 0) {
                --targets.offset;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
            }
            const std::int32_t position = targets.offset;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = targets.count - 5;
            auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
            result = 1;
        }
        graphic->imageNo = 0x3B592;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B594;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B593;
        }
        return result;
    }
    return 0;
}

// 原函数：0x0049EC00。
static std::int32_t WindowTradeTarget_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t result = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (targets.offset < targets.count - 5) {
                ++targets.offset;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
            }
            const std::int32_t position = targets.offset;
            const std::int32_t maximum = targets.count - 5;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 2));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
            result = 1;
        }
        graphic->imageNo = 0x3B595;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B597;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B596;
        }
        return result;
    }
    return 0;
}

// 原函数：0x0049ECC0。
static std::int32_t WindowTradeTarget_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t result = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, targets.offset > 0 ? 51 : 55, 320));
            targets.offset -= 5;
            if (targets.offset < 0) {
                targets.offset = 0;
            }
            const std::int32_t position = targets.offset;
            const std::int32_t maximum = targets.count - 5;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
            result = 1;
        }
        graphic->imageNo = 0x3B556;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B558;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B557;
        }
        return result;
    }
    return 0;
}

// 原函数：0x0049ED80。
static std::int32_t WindowTradeTarget_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t result = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t oldMaximum = targets.count - 5;
            const bool canMove = oldMaximum > 0 && targets.offset < oldMaximum;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, canMove ? 51 : 55, 320));
            std::int32_t position = targets.offset + 5;
            const std::int32_t maximum = targets.count - 5;
            targets.offset = position;
            if (position > maximum) {
                position = maximum;
                targets.offset = position;
            }
            if (position < 0) {
                position = 0;
                targets.offset = position;
            }
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            WindowTradeTarget_Refresh(&callbackWindow, targets.offset);
            result = 1;
        }
        graphic->imageNo = 0x3B559;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B55B;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B55A;
        }
        return result;
    }
    return 0;
}

// 原函数：0x0049EE60。
static std::int32_t WindowTradeTarget_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = targets.count;
        const auto wheel = std::bit_cast<std::int32_t>(platform::ReadSessionMouseWheelDelta(*callbackWindow.ownerSession));
        const std::int32_t position = targets.offset;
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            targets.offset =
                WindowScroll_ApplyWheel(indexedElement, position, count - 5, wheel);
        }
    }
    return 0;
}

// 原函数：0x0049EEB0。
static std::int32_t WindowTradeTarget_ButtonCancel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t result = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"中止。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            result = 1;
        }
        graphic->imageNo = 0x3B562;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B564;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B563;
        }
        return result;
    }
    return 0;
}

// 原函数：0x0049EF60。
static std::int32_t WindowTradeTarget_SelectRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& targets = callbackWindow.ownerSession->UiFrame().native.tradeTarget;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (index - 14 >= targets.count) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        const std::uint32_t imageNo = callbackWindow.ownerSession->UiFrame().hoverFramePaletteIndex;
        auto* const indexedElement = currentWindow->FindElementById(elementId);
        if (indexedElement) {
            WindowElement_DrawHoverFrame(indexedElement, state, imageNo);
        }
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    const WindowListSelectionCallback callback = targets.callback;
    if (callback != nullptr) {
        const std::int32_t selected = targets.offset + index - 14;
        callback(*callbackWindow.ownerSession, selected, targets.names[selected].data());
    }
    currentWindow->state |= 0x8000u;
    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
    return 1;
}

} // namespace detail

} // namespace cg::ui
