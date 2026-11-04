#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/session/ClientSession.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowTextTargetElementId : ElementId {
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

// 原函数：0x0049F040。
static void WindowTextTarget_Release(WindowInstance&);

// 原函数：0x0049F060。
static std::int32_t WindowTextTarget_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049F0A0。
static void WindowTextTarget_Refresh(WindowInstance& callbackWindow, std::int32_t offset); // 刷新五行，未用行清空正文但保留颜色。

// 原函数：0x0049F1A0。
static std::int32_t WindowTextTarget_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049F260。
static std::int32_t WindowTextTarget_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F310。
static std::int32_t WindowTextTarget_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F3D0。
static std::int32_t WindowTextTarget_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F490。
static std::int32_t WindowTextTarget_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F570。
static std::int32_t WindowTextTarget_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F5C0。
static std::int32_t WindowTextTarget_ButtonCancel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x0049F680。
static std::int32_t WindowTextTarget_SelectRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56D010 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D010。

static WindowGraphicData gWindowGraphic56D028 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D028。

static WindowGraphicData gWindowGraphic56D040 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D040。

static WindowGraphicData gWindowGraphic56D058 {243042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D058。

static WindowGraphicData gWindowGraphic56D070 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D070。

static WindowGraphicData gWindowGraphic56D088 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D088。

static WindowTextPrototype gWindowText56D0A0 { // 原全局：0x0056D0A0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};

static WindowTextPrototype gWindowText56D0A8 { // 原全局：0x0056D0A8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"请选择对象。", // 原常量参考：0x00585F54。
};

static WindowElementPrototype gWindowElements56D0B0[19] { // 原全局：0x0056D0B0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56D010}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element1)},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56D010.imageNo)}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowTextTargetElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56D028}, &WindowTextTarget_ButtonPreviousRow, -1, static_cast<ElementId>(WindowTextTargetElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56D040}, &WindowTextTarget_ButtonNextRow, -1, static_cast<ElementId>(WindowTextTargetElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56D070}, &WindowTextTarget_ButtonPreviousPage, -1, static_cast<ElementId>(WindowTextTargetElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56D088}, &WindowTextTarget_ButtonNextPage, -1, static_cast<ElementId>(WindowTextTargetElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTextTarget_Scroll, -1, static_cast<ElementId>(WindowTextTargetElementId::Scroll)},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56D058}, &WindowTextTarget_ButtonCancel, -1, static_cast<ElementId>(WindowTextTargetElementId::ButtonCancel)},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56D0A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element9)},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element10)},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element11)},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element12)},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element13)},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTextTargetElementId::Element14)},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow, -1, static_cast<ElementId>(WindowTextTargetElementId::SelectRow)},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow, -1, static_cast<ElementId>(WindowTextTargetElementId::SelectRow_16)},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow, -1, static_cast<ElementId>(WindowTextTargetElementId::SelectRow_17)},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow, -1, static_cast<ElementId>(WindowTextTargetElementId::SelectRow_18)},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow, -1, static_cast<ElementId>(WindowTextTargetElementId::SelectRow_19)},
};

// 原型：窗口45；原全局：0x0054E900。
WindowPrototype gWindowPrototypeTextTarget {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56D0B0, &WindowTextTarget_Update, &WindowTextTarget_Draw, &WindowTextTarget_Release}; // 45: 54E900

// 窗口函数实现。

// 原函数：0x0049F000。
cg::yobi::YobiAction* WindowTextTarget_Open(
    session::ClientSession& session, const wchar_t* const* const names, const std::uint8_t* const colors,
    const std::int32_t count, const WindowTextChoiceCallback callback)
{
    auto& choice = session.UiFrame().native.textTarget;
    choice.names = names;
    choice.colors = colors;
    choice.count = count;
    choice.pending = callback;
    return WindowRuntime_CreateAnimation(session, WindowId_TextTarget, 1, 0, nullptr);
}

// 原函数：0x0049F040。
static void WindowTextTarget_Release(WindowInstance& releasedWindow)
{
    auto& session = *releasedWindow.ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    const WindowTextChoiceCallback callback = choice.callback;
    if (callback != nullptr) {
        static_cast<void>(callback(session, -1, nullptr));
    }
}

// 原函数：0x0049F060。
static std::int32_t WindowTextTarget_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    if (value == -1) {
        auto& session = *callbackWindow.ownerSession;
        auto& choice = session.UiFrame().native.textTarget;
        choice.offset = 0;
        WindowTextTarget_Refresh(callbackWindow, 0);
        const WindowTextChoiceCallback callback = choice.pending;
        choice.enabled = 1;
        choice.callback = callback;
    }
    return 1;
}

// 原函数：0x0049F0A0。
static void WindowTextTarget_Refresh(WindowInstance& callbackWindow, const std::int32_t offset)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    const wchar_t* const* names = choice.names;
    std::int32_t row = 0;
    for (; row < 5 && offset + row < choice.count; ++row) {
        if (names != nullptr) {
            auto* const textElement = currentWindow->FindElementById(NativeElementKey(row + 9));
            if (textElement) {
                auto* const text = textElement->data.text;
                text->text = names[offset + row];
            }
            const std::uint8_t color = choice.colors[offset + row];
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 9));
            if (indexedElement) {
                indexedElement->data.text->style.color =
                    static_cast<std::byte>(color);
            }
            names = choice.names;
        }
    }
    for (; row < 5; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 9)); indexedElement) indexedElement->data.text->text = L"";
    }
}

// 原函数：0x0049F1A0。
static std::int32_t WindowTextTarget_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    const std::int32_t visible = choice.count - 5 > 0 ? 1 : 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = visible;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowTextTargetElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
            auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowTextTargetElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement2) {
                choice.offset = WindowScroll_ReadPosition(handleVerticalScrollElement2, choice.count - 5);
            }
            WindowTextTarget_Refresh(callbackWindow, choice.offset);
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    WindowInstance* const window = currentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(*callbackWindow.ownerSession,
        245300, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(*callbackWindow.ownerSession, 3));
    return 1;
}

// 原函数：0x0049F260。
static std::int32_t WindowTextTarget_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (choice.offset > 0) {
                --choice.offset;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
            }
            const std::int32_t offset = choice.offset;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = choice.count - 5;
            auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, offset));
            }
            WindowTextTarget_Refresh(callbackWindow, choice.offset);
            consumed = 1;
        }
        graphic->imageNo = 243090;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243092;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243091;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049F310。
static std::int32_t WindowTextTarget_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (choice.offset < choice.count - 5) {
                ++choice.offset;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
            }
            const std::int32_t offset = choice.offset;
            const std::int32_t maximum = choice.count - 5;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 2));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, offset));
            }
            WindowTextTarget_Refresh(callbackWindow, choice.offset);
            consumed = 1;
        }
        graphic->imageNo = 243093;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243095;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243094;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049F3D0。
static std::int32_t WindowTextTarget_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, choice.offset > 0 ? 51 : 55, 320));
            choice.offset -= 5;
            if (choice.offset < 0) {
                choice.offset = 0;
            }
            const std::int32_t offset = choice.offset;
            const std::int32_t maximum = choice.count - 5;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, offset));
            }
            WindowTextTarget_Refresh(callbackWindow, choice.offset);
            consumed = 1;
        }
        graphic->imageNo = 243030;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243032;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243031;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049F490。
static std::int32_t WindowTextTarget_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t oldMaximum = choice.count - 5;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession,
                oldMaximum > 0 && choice.offset < oldMaximum ? 51 : 55, 320));
            std::int32_t offset = choice.offset + 5;
            const std::int32_t maximum = choice.count - 5;
            choice.offset = offset;
            if (offset > maximum) {
                offset = maximum;
                choice.offset = offset;
            }
            if (offset < 0) {
                offset = 0;
                choice.offset = offset;
            }
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, offset));
            }
            WindowTextTarget_Refresh(callbackWindow, choice.offset);
            consumed = 1;
        }
        graphic->imageNo = 243033;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243035;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243034;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049F570。
static std::int32_t WindowTextTarget_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = choice.count;
        const auto wheel = std::bit_cast<std::int32_t>(platform::ReadSessionMouseWheelDelta(*callbackWindow.ownerSession));
        const std::int32_t offset = choice.offset;
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            choice.offset =
                WindowScroll_ApplyWheel(indexedElement, offset, count - 5, wheel);
        }
    }
    return 0;
}

// 原函数：0x0049F5C0。
static std::int32_t WindowTextTarget_ButtonCancel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"关闭此视窗";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (choice.enabled != 0) {
                const WindowTextChoiceCallback callback = choice.callback;
                if (callback != nullptr) {
                    if (callback(session, -2, nullptr) != 0) {
                        currentWindow->state |= 0x8000u;
                        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                    }
                    choice.enabled = 0;
                }
            }
            consumed = 1;
        }
        graphic->imageNo = 243042;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243044;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243043;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049F680。
static std::int32_t WindowTextTarget_SelectRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& choice = session.UiFrame().native.textTarget;
    const std::int32_t selected = choice.offset + index - 14;
    // 原门槛只比较行号，不将滚动偏移并入检查。
    if (index - 14 >= choice.count) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        auto* const indexedElement = currentWindow->FindElementById(elementId);
        if (indexedElement) {
            WindowElement_DrawHoverFrame(indexedElement, state, callbackWindow.ownerSession->UiFrame().hoverFramePaletteIndex);
        }
        uiFrame.hoverText = L"选择此人物。";
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (choice.enabled != 0) {
            const WindowTextChoiceCallback callback = choice.callback;
            if (callback != nullptr) {
                if (callback(session, selected, choice.names[selected]) != 0) {
                    currentWindow->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                }
                choice.enabled = 0;
            }
        }
        return 1;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
