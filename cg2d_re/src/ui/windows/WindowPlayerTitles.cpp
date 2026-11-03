#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPlayerTitlesElementId : ElementId {
    EditDraft = 1,
    Element2 = 2,
    DragScroll = 3,
    HandleButtons = 4,
    HandleButtons_5 = 5,
    Scroll = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    ButtonClose = 17,
    HandleButtons_18 = 18,
    HandleButtons_19 = 19,
    Element20 = 20,
    HandleButtons_21 = 21,
    HandleButtons_22 = 22,
    HandleButtons_23 = 23,
    HandleButtons_24 = 24,
    HandleButtons_25 = 25,
    HandleButtons_26 = 26,
    SelectTitle = 27,
    SelectTitle_28 = 28,
    SelectTitle_29 = 29,
    SelectTitle_30 = 30,
    SelectTitle_31 = 31,
    SelectTitle_32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    HandleButtons_36 = 36,
    HandleSideArea = 37,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。


// 回调与内部函数前置声明。

// 原函数：0x00477540。
static std::int32_t WindowPlayerTitles_EditDraft(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 草稿编辑焦点。

// 原函数：0x00477600。
static std::int32_t WindowPlayerTitles_SelectTitle(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六行称号选择/删除。

// 原函数：0x00477710。
static std::int32_t WindowPlayerTitles_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 完整按钮分派与背景清模式。

// 原函数：0x004782A0。
static std::int32_t WindowPlayerTitles_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 称号滚轮，恒返0。

// 原函数：0x004782E0。
static std::int32_t WindowPlayerTitles_DragScroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WSP01：原称号拖动页码；旧X代码1959。

// 原函数：0x004744A0。
static void WindowPlayerTitles_Release(WindowInstance&);

// 原函数：0x004774C0。
// 关联原入口：0x0047752C（共享尾已并入本函数正文）。
static std::int32_t WindowPlayerTitles_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D45FC0。 窗口9草稿，区别于玩家ownTitle和原型内嵌文本。
static std::wstring gTitleDraftD45FC0 {}; // 原全局：0x00D45FC0。

// 原全局：0x0057CCB8。 原型当前context为编辑正文来源。

static WindowGraphicData gWindowGraphic560DA0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DA0。

static WindowGraphicData gWindowGraphic560DB8 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DB8。

static WindowGraphicData gWindowGraphic560DD0 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DD0。

static WindowGraphicData gWindowGraphic560DE8 {0x3BAB9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DE8。

static WindowGraphicData gWindowGraphic560E00 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E00。

static WindowGraphicData gWindowGraphic560E18 {0x3B574, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E18。

static WindowGraphicData gWindowGraphic560E30 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E30。

static WindowGraphicData gWindowGraphic560E48 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E48。

static WindowGraphicData gWindowGraphic560E60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E60。

static WindowGraphicData gWindowGraphic560E78 {0x3B59D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E78。

static WindowGraphicData gWindowGraphic560E90 {0x3BAB3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E90。

static WindowGraphicData gWindowGraphic560EA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EA8。

static WindowGraphicData gWindowGraphic560EC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00560EC0。

static WindowGraphicData gWindowGraphic560ED8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560ED8。

static WindowGraphicData gWindowGraphic560EF0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EF0。

static WindowGraphicData gWindowGraphic560F08 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F08。

static WindowGraphicData gWindowGraphic560F20 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F20。

static WindowTextPrototype gWindowText560F38 { // 原全局：0x00560F38。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText560F40 { // 原全局：0x00560F40。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText560F48 { // 原全局：0x00560F48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"称号", // 原常量参考：0x005860D0。
};

static std::uint32_t gWindowMouseFlagsD676F0 = 0; // 原全局：0x00D676F0。

static std::uint32_t gWindowMouseFlagsD6772C = 0; // 原全局：0x00D6772C。

static WindowElementPrototype gWindowElements560F50[37] { // 原全局：0x00560F50。
    {WindowElementType_Input, 105, 31, 144, 12, 1, {.unusedValue = 0}, &WindowPlayerTitles_EditDraft, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::EditDraft)},
    {WindowElementType_Graphic, 239, 104, 0, 14, 1, {.graphic = &gWindowGraphic560ED8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element2)},
    {WindowElementType_MouseArea, 239, 104, 11, 72, 1, {.mouseFlags = &gWindowMouseFlagsD676F0}, &WindowPlayerTitles_DragScroll, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::DragScroll)},
    {WindowElementType_Graphic, 239, 93, 11, 11, 1, {.graphic = &gWindowGraphic560EF0}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons)},
    {WindowElementType_Graphic, 239, 175, 11, 11, 1, {.graphic = &gWindowGraphic560F08}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_5)},
    {WindowElementType_EventArea, 0, 0, 315, 261, 1, {.unusedValue = 0}, &WindowPlayerTitles_Scroll, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Scroll)},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560F38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element7)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560F40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element8)},
    {WindowElementType_Text, 105, 31, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element9)},
    {WindowElementType_Text, 105, 64, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element10)},
    {WindowElementType_Text, 25, 94, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element11)},
    {WindowElementType_Text, 25, 110, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element12)},
    {WindowElementType_Text, 25, 126, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element13)},
    {WindowElementType_Text, 25, 142, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element14)},
    {WindowElementType_Text, 25, 158, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element15)},
    {WindowElementType_Text, 25, 174, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element16)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560DA0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::ButtonClose)},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DB8}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_18)},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic560DD0}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_19)},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DE8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element20)},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560F20}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_21)},
    {WindowElementType_Graphic, 260, 27, 49, 17, 1, {.graphic = &gWindowGraphic560E00}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_22)},
    {WindowElementType_Graphic, 260, 61, 49, 17, 1, {.graphic = &gWindowGraphic560E18}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_23)},
    {WindowElementType_Graphic, 260, 175, 49, 17, 1, {.graphic = &gWindowGraphic560E30}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_24)},
    {WindowElementType_Graphic, 220, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E48}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_25)},
    {WindowElementType_Graphic, 199, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E60}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_26)},
    {WindowElementType_Graphic, 23, 92, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle)},
    {WindowElementType_Graphic, 23, 108, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle_28)},
    {WindowElementType_Graphic, 23, 124, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle_29)},
    {WindowElementType_Graphic, 23, 140, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle_30)},
    {WindowElementType_Graphic, 23, 156, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle_31)},
    {WindowElementType_Graphic, 23, 172, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::SelectTitle_32)},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic560E90}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element33)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560EA8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element34)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560EC0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::Element35)},
    {WindowElementType_MouseArea, 0, 0, 374, 281, 1, {.mouseFlags = &gWindowMouseFlagsD6772C}, &WindowPlayerTitles_HandleButtons, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_36)},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPlayerTitlesElementId::HandleSideArea)},
};

// 原型：窗口9；原全局：0x0054E3C0。
WindowPrototype gWindowPrototypePlayerTitles {0, 4, 15, 97, 315, 261, 0x80000080u, 37, gWindowElements560F50, &WindowPlayerTitles_Update, &WindowRuntime_DrawDefault, &WindowPlayerTitles_Release}; // 9: 54E3C0

// 窗口函数实现。

// 原函数：0x00477540。
static std::int32_t WindowPlayerTitles_EditDraft(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"设定自己的称号。" /* 原常量参考：0x005863C4。 */;
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    WindowInstance* const window = currentWindow;
    auto* const editDraftElement = window->FindElementById(WindowPlayerTitlesElementId::EditDraft);
    if (editDraftElement) {
        if (callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData != editDraftElement->data.input) {
            inputs.playerTitlePrototype.text = gTitleDraftD45FC0;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.playerTitlePrototype, x, y, 0));
            input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.playerTitle);
            WindowInstance* const focused = currentWindow;
            auto* const editDraftElement2 = focused->FindElementById(WindowPlayerTitlesElementId::EditDraft);
            if (editDraftElement2) {
                callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = editDraftElement2->data.input;
            }
            auto* const editDraftElement3 = focused->FindElementById(WindowPlayerTitlesElementId::EditDraft);
            if (editDraftElement3) {
                editDraftElement3->data.input->context =
                    &inputs.playerTitle;
            }
        }
    }
    return 1;
}

// 原函数：0x00477600。
static std::int32_t WindowPlayerTitles_SelectTitle(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B59E;
            gHoverTextD68AE8 = L"设定此称号。" /* 原常量参考：0x005863D8。 */;
            consumed = 1;
        } else {
            graphic->imageNo = 0x3B59D;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
    }
    const std::int32_t mode = business.valuesCF4E50[0];
    if (mode == 0 || mode == 1) {
        const std::int32_t id = world::gSortedTitlesDE03F8[business.valuesCF4E50[1] + index - 26].id;
        // 删除模式直接查原表，只有普通选择先拒绝负id。
        if (mode == 1 || id >= 0) {
            const std::int32_t originalIndex = world::FindTitleIndexById(id);
            if (originalIndex >= 0) {
                if (mode == 1) {
                    static_cast<void>(callbackWindow.ownerSession->Connection().DeletePlayerTitleRequest(originalIndex));
                } else {
                    static_cast<void>(callbackWindow.ownerSession->Connection().SelectPlayerTitleRequest(originalIndex));
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    }
    return 1;
}

// 原函数：0x00477710。
static std::int32_t WindowPlayerTitles_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    switch (static_cast<WindowPlayerTitlesElementId>(elementId)) {
    case WindowPlayerTitlesElementId::HandleButtons_18:
    case WindowPlayerTitlesElementId::HandleButtons_19:
    case WindowPlayerTitlesElementId::HandleButtons_21: {
        if (elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_21) && callbackWindow.ownerSession->Connection().PukVersion() < 8u) {
            setImage(0x3C111);
            return 0;
        }
        const std::int32_t target = elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_18) ? 7 : elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_19) ? 8 : 58;
        const std::int32_t idle = elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_18) ? 0x3BAB5 : elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_19) ? 0x3B583 : 0x3C111;
        const std::int32_t hover = elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_18) ? 0x3BAB7 : elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_19) ? 0x3B585 : 0x3C112;
        if ((state & MouseFlag_Hover) != 0) {
            setImage(hover);
            gHoverTextD68AE8 = elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_18) ? L"切换成玩家状态视窗。" /* 原常量参考：0x005858D8。 */
                            : elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_19) ? L"切换成玩家明细视窗。" /* 原常量参考：0x005858C0。 */ : L"切换成玩家简介视窗" /* 原常量参考：0x0058637C。 */;
            consumed = 1;
        } else {
            setImage(idle);
        }
        if ((state & MouseFlag_LDownEvent) != 0 && WindowRuntime_States(*callbackWindow.ownerSession)[target].window == nullptr) {
            currentWindow->state |= 0x8000u;
            static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, target));
            WindowInstance* const xSource = currentWindow;
            WindowInstance* const xTarget = WindowRuntime_States(*callbackWindow.ownerSession)[target].window;
            xTarget->x = xSource->x;
            WindowInstance* const ySource = currentWindow;
            WindowInstance* const yTarget = WindowRuntime_States(*callbackWindow.ownerSession)[target].window;
            yTarget->y = ySource->y;
            static_cast<void>(audio::PlaySe_52C450(56, 320));
            WindowRuntime_QueuePagePresentation(*currentWindow, *yTarget);
            consumed = 1;
        }
        return consumed;
    }
    case WindowPlayerTitlesElementId::HandleButtons_22: {
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3BAC4);
            gHoverTextD68AE8 = L"设定此称号。" /* 原常量参考：0x005863D8。 */;
            consumed = 1;
        } else {
            setImage(0x3BAC2);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::wstring escapedText = input::EscapeTextAndNotify(gTitleDraftD45FC0, 128);
            static_cast<void>(callbackWindow.ownerSession->Connection().SetCustomPlayerTitleRequest(escapedText));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(WindowChat_RestoreInput(*callbackWindow.ownerSession));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3BAC3);
        }
        return consumed;
    }
    case WindowPlayerTitlesElementId::HandleButtons_23:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B576);
            gHoverTextD68AE8 = L"取消所选择的称号。" /* 原常量参考：0x005863FC。 */;
            consumed = 1;
        } else {
            setImage(0x3B574);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(callbackWindow.ownerSession->Connection().SelectPlayerTitleRequest(-1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B575);
        }
        return consumed;
    case WindowPlayerTitlesElementId::HandleButtons_24: {
        if (business.valuesCF4E50[0] == 1) {
            setImage(0x3B578);
            gHoverTextD68AE8 = L"删除所选择的称号。" /* 原常量参考：0x005863E8。 */;
        }
        const std::int32_t mode = business.valuesCF4E50[0];
        if ((state & MouseFlag_Hover) != 0) {
            if (mode == 0) {
                setImage(0x3B579);
            }
            consumed = 1;
        } else if (mode == 0) {
            setImage(0x3B577);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            business.valuesCF4E50[0] = business.valuesCF4E50[0] != 1 ? 1 : 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        return consumed;
    }
    case WindowPlayerTitlesElementId::HandleButtons:
    case WindowPlayerTitlesElementId::HandleButtons_5:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons) ? 0x3B594 : 0x3B597);
            consumed = 1;
        } else {
            setImage(elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons) ? 0x3B592 : 0x3B595);
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t page = business.valuesCF4E50[1];
            if (elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons) && page > 0) {
                business.valuesCF4E50[1] = page - 1;
                WindowInstance* const window = currentWindow;
                auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 58, page - 1));
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else if (elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons_5) && page < 58) {
                WindowInstance* const window = currentWindow;
                business.valuesCF4E50[1] = page + 1;
                auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 58, page + 1));
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPlayerTitlesElementId::HandleButtons) ? 0x3B593 : 0x3B596);
            consumed = 1;
        }
        return consumed;
    case WindowPlayerTitlesElementId::HandleButtons_25:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B55B);
            consumed = 1;
        } else {
            setImage(0x3B559);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            std::int32_t page = business.valuesCF4E50[1] + 6;
            business.valuesCF4E50[1] = page;
            if (page > 58) {
                page = 58;
                business.valuesCF4E50[1] = page;
            }
            WindowInstance* const window = currentWindow;
            auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 58, page));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B55A);
        }
        return consumed;
    case WindowPlayerTitlesElementId::HandleButtons_26:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B558);
            consumed = 1;
        } else {
            setImage(0x3B556);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t previousPage = business.valuesCF4E50[1] - 6;
            business.valuesCF4E50[1] = previousPage;
            if (previousPage < 0) {
                business.valuesCF4E50[1] = 0;
            }
            WindowInstance* const window = currentWindow;
            const std::int32_t page = business.valuesCF4E50[1];
            auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 58, page));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B557);
        }
        return consumed;
    case WindowPlayerTitlesElementId::HandleButtons_36:
        if ((state & MouseFlag_LDownEvent) != 0) {
            business.valuesCF4E50[0] = 0;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

// 原函数：0x004782A0。
static std::int32_t WindowPlayerTitles_Scroll(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        WindowInstance* const window = currentWindow;
        const std::int32_t page = business.valuesCF4E50[1];
        auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
        if (dragScrollElement) {
            business.valuesCF4E50[1] = WindowScroll_ApplyWheel(dragScrollElement, page, 58, wheel);
        }
    }
    return 0;
}

// WSP01：旧X片段1959；滚动非零才反求称号页码，原型/代码身份沿CAC08。
// 原函数：0x004782E0。
static std::int32_t WindowPlayerTitles_DragScroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t result = WindowElement_HandleVerticalScroll(callbackWindow, elementId, state);
    if (result != 0) {
        WindowInstance* const window = currentWindow;
        auto* const dragScrollElement = window->FindElementById(WindowPlayerTitlesElementId::DragScroll);
        if (dragScrollElement) {
            business.valuesCF4E50[1] = WindowScroll_ReadPosition(dragScrollElement, 58);
        }
    }
    return result;
}

// 原478010：草稿比较分别控制颜色与确认显隐，六行固定表起点、逐行重取窗口。
// 原函数：0x00478010。
static void WindowPlayerTitles_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    auto* const element7 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element7);
    if (element7) {
        auto* const nameData = element7->data.text;
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        nameData->text = name;
    }

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::gPlayerRuntimeSlots.level);
    auto* const element8 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element8);
    if (element8) {
        auto* const levelData = element8->data.text;
        levelData->text = buffer;
    }

    const std::int32_t titleIndex = world::gPlayerRuntimeSlots.titleIndex100;
    auto* const element10 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element10);
    if (element10) {
        auto* const titleData = element10->data.text;
        const wchar_t* const title = world::GetTitleText(titleIndex);
        titleData->text = title;
    }

    if (currentWindow->ownerSession->UiFrame().inputState.currentContext != &inputs.playerTitle) {
        if (auto* const element9 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element9); element9) element9->visible = 1;
        auto* const element92 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element9);
        if (element92) {
            auto* const draftData = element92->data.text;
            draftData->text = gTitleDraftD45FC0;
        }
        const bool unchanged = gTitleDraftD45FC0.compare(world::GetPlayerDisplayText(2)) == 0;
        auto* const element93 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element9);
        if (element93) {
            auto* const colorData = element93->data.text;
            colorData->style.color = unchanged ? std::byte{0} : std::byte{6};
        }
    } else {
        gTitleDraftD45FC0 = input::ContextText(*inputs.playerTitlePrototype.input.context);
        if (auto* const element9 = currentWindow->FindElementById(WindowPlayerTitlesElementId::Element9); element9) element9->visible = 0;
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.playerTitlePrototype, x, y, 1));
    }

    const bool unchanged = gTitleDraftD45FC0.compare(world::GetPlayerDisplayText(2)) == 0;
    if (auto* const handleButtons_22Element = currentWindow->FindElementById(WindowPlayerTitlesElementId::HandleButtons_22); handleButtons_22Element) handleButtons_22Element->visible = unchanged ? 0 : 1;

    const world::TitleRecord* record =
        world::gSortedTitlesDE03F8.data() + business.valuesCF4E50[1];
    for (std::int32_t index = 10; index < 16; ++index) {
        WindowInstance* const window = currentWindow;
        const wchar_t* const text = record->text.data();
        ++record;
        auto* const dataElement = window->FindElementById(NativeElementKey(index));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = text;
        }
    }
}

// 原4774C0及47752C公共尾，IDA分段不改变同一主更新的正常出口。
// 原函数：0x004774C0。
static std::int32_t WindowPlayerTitles_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        const wchar_t* const ownTitle = world::GetPlayerDisplayText(2);
        business.valuesCF4E50[1] = 0;
        gTitleDraftD45FC0 = ownTitle;

        auto* elementsWindow = currentWindow;
        const wchar_t* const displayTitle = world::GetPlayerDisplayText(2);
        auto* const element9 = elementsWindow->FindElementById(WindowPlayerTitlesElementId::Element9);
        if (element9) {
            auto* const data = element9->data.text;
            data->text = displayTitle;
        }
    }
    WindowPlayerTitles_Refresh(&callbackWindow);
    return 1;
}

// 原函数：0x004744A0。
static void WindowPlayerTitles_Release(WindowInstance& releasedWindow)
{
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerTitles].window;
    if (window != nullptr) {
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerProfile].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerProfile].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerAttributes].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerAttributes].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
