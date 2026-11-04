#include "cg/session/ClientSession.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowOwnProfileCategoryElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    DragScroll = 3,
    HandleScrollButtons = 4,
    HandleScrollButtons_5 = 5,
    Element6 = 6,
    HandleScrollButtons_7 = 7,
    HandleScrollButtons_8 = 8,
    Scroll = 9,
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
    SelectCategory = 20,
    SelectCategory_21 = 21,
    SelectCategory_22 = 22,
    SelectCategory_23 = 23,
    SelectCategory_24 = 24,
    SelectCategory_25 = 25,
    SelectCategory_26 = 26,
    SelectCategory_27 = 27,
    SelectCategory_28 = 28,
    SelectCategory_29 = 29,
    Element30 = 30,
    Element31 = 31,
    HandleSideArea = 32,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C94F0。
static std::int32_t WindowOwnProfileCategory_SelectCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 十行分类选择，写当前资料。

// 原函数：0x004C96F0。
static std::int32_t WindowOwnProfileCategory_HandleScrollButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口59四滚动/翻页按钮。

// 原函数：0x004C9B80。
static std::int32_t WindowOwnProfileCategory_DragScroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WSP01：原分类count-10拖动换算；旧X代码1992。

// 原函数：0x004C9BE0。
static std::int32_t WindowOwnProfileCategory_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 三模式滚轮，恒返0。

// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
static std::int32_t WindowOwnProfileCategory_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00CA0444。 窗口59六入口共用的S32页码。
static std::int32_t gProfileScrollCA0444 = 0; // 原全局：0x00CA0444。

static WindowGraphicData gWindowGraphic577170 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577170。

static WindowGraphicData gWindowGraphic577188 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577188。

static WindowGraphicData gWindowGraphic5771A0 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005771A0。

static WindowGraphicData gWindowGraphic5771B8 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771B8。

static WindowGraphicData gWindowGraphic5771E8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771E8。

static WindowGraphicData gWindowGraphic577200 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577200。

static WindowGraphicData gWindowGraphic577218 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577218。

static WindowGraphicData gWindowGraphic577248 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577248。

static WindowGraphicData gWindowGraphic577260 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577260。

static WindowGraphicData gWindowGraphic577278 {0x3C386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577278。

static WindowTextPrototype gWindowText577290 { // 原全局：0x00577290。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"物品", // 原常量参考：0x00585D78。
};

static std::uint32_t gWindowMouseFlagsD67930 = 0; // 原全局：0x00D67930。

static WindowElementPrototype gWindowElements577298[32] { // 原全局：0x00577298。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic577170}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::ButtonClose)},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic5771E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element2)},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD67930}, &WindowOwnProfileCategory_DragScroll, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::DragScroll)},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic577200}, &WindowOwnProfileCategory_HandleScrollButtons, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::HandleScrollButtons)},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic577218}, &WindowOwnProfileCategory_HandleScrollButtons, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::HandleScrollButtons_5)},
    {WindowElementType_Graphic, 89, 7, 0, 0, 1, {.graphic = &gWindowGraphic577278}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element6)},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic577248}, &WindowOwnProfileCategory_HandleScrollButtons, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::HandleScrollButtons_7)},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic577260}, &WindowOwnProfileCategory_HandleScrollButtons, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::HandleScrollButtons_8)},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowOwnProfileCategory_Scroll, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Scroll)},
    {WindowElementType_Text, 20, 30, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element10)},
    {WindowElementType_Text, 20, 46, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element11)},
    {WindowElementType_Text, 20, 62, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element12)},
    {WindowElementType_Text, 20, 78, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element13)},
    {WindowElementType_Text, 20, 94, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element14)},
    {WindowElementType_Text, 20, 110, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element15)},
    {WindowElementType_Text, 20, 126, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element16)},
    {WindowElementType_Text, 20, 142, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element17)},
    {WindowElementType_Text, 20, 158, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element18)},
    {WindowElementType_Text, 20, 174, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element19)},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory)},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_21)},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_22)},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_23)},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_24)},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_25)},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_26)},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_27)},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_28)},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::SelectCategory_29)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577188}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element30)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5771A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::Element31)},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowOwnProfileCategoryElementId::HandleSideArea)},
};

// 原型：窗口59；原全局：0x0054ED60。
WindowPrototype gWindowPrototypeOwnProfileCategory {0, 4, 38, 95, 225, 220, 0x80010101u, 32, gWindowElements577298, &WindowOwnProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 59: 54ED60

// 窗口函数实现。

// 原函数：0x004C9C70。
static void WindowOwnProfileCategory_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    const std::int32_t mode = gProfileCategory1085DA0;
    std::int32_t image;
    switch (mode) {
    case 0: image = 0x3C386; break;
    case 1: image = 0x3C387; break;
    case 2: image = 0x3C385; break;
    default: return;
    }
    if (auto* const element6 = currentWindow->FindElementById(WindowOwnProfileCategoryElementId::Element6); element6) element6->data.graphic->imageNo = image;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(*callbackWindow->ownerSession, mode, page + row);
        const wchar_t* const source =
            record.used != 0 ? record.text0C.data() : L"";
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(row + 9));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = source;
        }
    }
}

// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
static std::int32_t WindowOwnProfileCategory_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        gProfileScrollCA0444 = 0;
    }
    if (WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_OwnProfile].window == nullptr) {
        currentWindow->state |= 0x8000u;
    }
    WindowOwnProfileCategory_Refresh(&callbackWindow);
    return 1;
}

// 原函数：0x004C94F0。
static std::int32_t WindowOwnProfileCategory_SelectCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        switch (gProfileCategory1085DA0) {
        case 0: uiFrame.hoverText = L"设定贩卖的目录" /* 原常量参考：0x005884F4。 */; break;
        case 1: uiFrame.hoverText = L"设定购买的目录" /* 原常量参考：0x00588504。 */; break;
        case 2: uiFrame.hoverText = L"设定描述的目录" /* 原常量参考：0x00588514。 */; break;
        default: break;
        }
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB04;
        consumed = 1;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB02;
    }

    const std::int32_t selectedMode = gProfileCategory1085DA0;
    if (selectedMode >= 0 && selectedMode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const std::int32_t selected =
            callbackWindow.ownerSession->WorldScene().mail.data->currentProfile.entries410[selectedMode].value00;
        if (selected == page + index - 19) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB03;
        }
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return consumed;
    }

    const std::int32_t mode = gProfileCategory1085DA0;
    if (mode >= 0 && mode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(*callbackWindow.ownerSession, mode, page + index - 19);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t value = record.value04;
        const std::int32_t image = record.value08;
        auto& entry = callbackWindow.ownerSession->WorldScene().mail.data->currentProfile.entries410[mode];
        entry.value00 = value;
        entry.value04 = image;
    }
    static_cast<void>(WindowChat_RestoreInput(*callbackWindow.ownerSession));
    return 1;
}

// 原函数：0x004C96F0。
static std::int32_t WindowOwnProfileCategory_HandleScrollButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    switch (static_cast<WindowOwnProfileCategoryElementId>(elementId)) {
    case WindowOwnProfileCategoryElementId::HandleScrollButtons: {
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B594);
            consumed = 1;
        } else {
            setImage(0x3B592);
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B593);
            consumed = 1;
        }
        if ((state & MouseFlag_LRepeatDown) == 0) {
            return consumed;
        }
        const std::int32_t page = gProfileScrollCA0444 - 1;
        gProfileScrollCA0444 = page;
        if (page < 0) {
            gProfileScrollCA0444 = 0;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t position = gProfileScrollCA0444;
            WindowInstance* window;
            std::int32_t count;
            if (mode == 2) {
                window = currentWindow;
                count = world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession);
            } else {
                count = world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
                window = currentWindow;
            }
            auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, count - 10, position));
            }
        }
        return 1;
    }
    case WindowOwnProfileCategoryElementId::HandleScrollButtons_5: {
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B597);
            consumed = 1;
        } else {
            setImage(0x3B595);
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B596);
            consumed = 1;
        }
        if ((state & MouseFlag_LRepeatDown) == 0) {
            return consumed;
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
            const std::int32_t page = gProfileScrollCA0444 + 1;
            gProfileScrollCA0444 = page;
            if (page <= count - 11) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gProfileScrollCA0444;
            const std::int32_t currentCount = mode == 2
                ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
            WindowInstance* const window = currentWindow;
            auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, currentCount - 10, position));
            }
        }
        return 1;
    }
    case WindowOwnProfileCategoryElementId::HandleScrollButtons_7:
        setImage((state & MouseFlag_Hover) != 0 ? 0x3B558 : 0x3B556);
        if ((state & MouseFlag_LDownEvent) != 0) {
            std::int32_t page = gProfileScrollCA0444 - 10;
            gProfileScrollCA0444 = page;
            if (page < 0) {
                page = 0;
                gProfileScrollCA0444 = page;
            }
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                    : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
                WindowInstance* const window = currentWindow;
                auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, count - 10, page));
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B557);
        }
        return consumed;
    case WindowOwnProfileCategoryElementId::HandleScrollButtons_8:
        setImage((state & MouseFlag_Hover) != 0 ? 0x3B55B : 0x3B559);
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                    : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
                const std::int32_t page = gProfileScrollCA0444 + 10;
                gProfileScrollCA0444 = page;
                if (page > count - 11) {
                    gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                const std::int32_t position = gProfileScrollCA0444;
                const std::int32_t currentCount = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                    : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
                WindowInstance* const window = currentWindow;
                auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, currentCount - 10, position));
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B55A);
        }
        return consumed;
    default:
        return 0;
    }
}

// WSP01：旧X片段1992；恰1且mode0..2才按原分类count-10更新。
// 原函数：0x004C9B80。
static std::int32_t WindowOwnProfileCategory_DragScroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t result = WindowElement_HandleVerticalScroll(callbackWindow, elementId, state);
    if (result == 1) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
            WindowInstance* const window = currentWindow;
            auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
            if (dragScrollElement) {
                gProfileScrollCA0444 = WindowScroll_ReadPosition(dragScrollElement, count - 10);
            }
        }
    }
    return result;
}

// 原函数：0x004C9BE0。
static std::int32_t WindowOwnProfileCategory_Scroll(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C(*callbackWindow.ownerSession)
                : world::detail::ReadCategoryCount105FA18(*callbackWindow.ownerSession);
            const std::int32_t page = gProfileScrollCA0444;
            WindowInstance* const window = currentWindow;
            auto* const dragScrollElement = window->FindElementById(WindowOwnProfileCategoryElementId::DragScroll);
            if (dragScrollElement) {
                gProfileScrollCA0444 = WindowScroll_ApplyWheel(dragScrollElement, page, count - 10, wheel);
            }
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
