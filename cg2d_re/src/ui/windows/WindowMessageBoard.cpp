#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

#include <cstring>
#include <ctime>

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowMessageBoardElementId : ElementId {
    HandleButtons = 1,
    HandleInput = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    HandleButtons_6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    HandleButtons_13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    HandleButtons_20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    HandleButtons_27 = 27,
    Element28 = 28,
    Element29 = 29,
    Element30 = 30,
    HandleButtons_31 = 31,
    HandleButtons_32 = 32,
    HandleButtons_33 = 33,
    HandleButtons_34 = 34,
    Element35 = 35,
    Element36 = 36,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。


// 回调与内部函数前置声明。

// 原函数：0x004B5B70。
static std::int32_t WindowMessageBoard_HandleInput(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 输入焦点及非空文本提示。

// 原函数：0x004B5C30。
static std::int32_t WindowMessageBoard_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 删除/写入/取消/翻页完整交互。

// 原函数：0x004B62A0。
static void WindowMessageBoard_RefreshRecords(WindowInstance* callbackWindow); // 四条记录刷新元素及输入位置；唯一caller弃用原地址低字节返回。

// 原函数：0x004B6C20。
static std::int32_t WindowMessageBoard_Close(WindowInstance* callbackWindow); // 清对话类型、关闭入口时实例并透传SE57结果。

// 原函数：0x004B5AD0。
static std::int32_t WindowMessageBoard_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004B5B50。
static void WindowMessageBoard_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00580618。 借Input唯一CC2080，供更新与编辑交互。

// 原D30138..D303F8；4B6970整区清零，4DFD38从+9起消费，其他字段尚未冻结。
static std::array<std::byte, 0x2C0> gWindow50AlternateRecordsD30138 {}; // 原全局：0x00D30138。

static WindowGraphicData gWindowGraphic5734D8 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734D8。

static WindowGraphicData gWindowGraphic5734F0 {0x3BB3C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734F0。

static WindowGraphicData gWindowGraphic573508 {0x3BB3D, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00573508。

static WindowGraphicData gWindowGraphic573520 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573520。

static WindowGraphicData gWindowGraphic573538 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573538。

static WindowGraphicData gWindowGraphic573550 {0x3BB3E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573550。

static WindowGraphicData gWindowGraphic573568 {0x3BB41, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573568。

static WindowGraphicData gWindowGraphic573580 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573580。

static WindowGraphicData gWindowGraphic573598 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573598。

static WindowTextPrototype gWindowText5735B0 { // 原全局：0x005735B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"留言板文字列",
};

static WindowTextPrototype gWindowText5735B8 { // 原全局：0x005735B8。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"名称",
};

static WindowTextPrototype gWindowText5735C0 { // 原全局：0x005735C0。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"留言时间",
};

static WindowElementPrototype gWindowElements5735C8[36] { // 原全局：0x005735C8。
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic573520}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons)},
    {WindowElementType_Input, 27, 362, 503, 42, 1, {.unusedValue = 0}, &WindowMessageBoard_HandleInput, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleInput)},
    {WindowElementType_Graphic, 22, 38, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element3)},
    {WindowElementType_Text, 135, 46, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element4)},
    {WindowElementType_Text, 330, 46, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element5)},
    {WindowElementType_Graphic, 530, 46, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_6)},
    {WindowElementType_Text, 100, 73, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element7)},
    {WindowElementType_Text, 100, 89, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element8)},
    {WindowElementType_Text, 100, 105, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element9)},
    {WindowElementType_Graphic, 22, 114, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element10)},
    {WindowElementType_Text, 135, 122, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element11)},
    {WindowElementType_Text, 330, 122, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element12)},
    {WindowElementType_Graphic, 530, 122, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_13)},
    {WindowElementType_Text, 100, 150, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element14)},
    {WindowElementType_Text, 100, 166, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element15)},
    {WindowElementType_Text, 100, 182, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element16)},
    {WindowElementType_Graphic, 22, 190, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element17)},
    {WindowElementType_Text, 135, 198, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element18)},
    {WindowElementType_Text, 330, 198, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element19)},
    {WindowElementType_Graphic, 530, 198, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_20)},
    {WindowElementType_Text, 100, 226, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element21)},
    {WindowElementType_Text, 100, 242, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element22)},
    {WindowElementType_Text, 100, 258, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element23)},
    {WindowElementType_Graphic, 22, 266, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element24)},
    {WindowElementType_Text, 135, 274, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element25)},
    {WindowElementType_Text, 330, 274, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element26)},
    {WindowElementType_Graphic, 530, 274, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_27)},
    {WindowElementType_Text, 100, 302, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element28)},
    {WindowElementType_Text, 100, 318, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element29)},
    {WindowElementType_Text, 100, 334, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element30)},
    {WindowElementType_Graphic, 538, 364, 49, 17, 1, {.graphic = &gWindowGraphic573550}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_31)},
    {WindowElementType_Graphic, 538, 386, 49, 17, 1, {.graphic = &gWindowGraphic573568}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_32)},
    {WindowElementType_Graphic, 576, 342, 17, 17, 1, {.graphic = &gWindowGraphic573580}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_33)},
    {WindowElementType_Graphic, 536, 342, 17, 17, 1, {.graphic = &gWindowGraphic573598}, &WindowMessageBoard_HandleButtons, -1, static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_34)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5734F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element35)},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic573508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMessageBoardElementId::Element36)},
};

// 原型：窗口50；原全局：0x0054EC30。
WindowPrototype gWindowPrototypeMessageBoard {0, 4, 5, 20, 634, 424, 0x80010101u, 36, gWindowElements5735C8, &WindowMessageBoard_Update, &WindowRuntime_DrawDefault, &WindowMessageBoard_Release}; // 50: 54EC30

// 窗口函数实现。

// 原函数：0x004B5A90。
void WindowMessageBoard_ResetRecords(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    business.valueD3FAD0 = 0;
    for (WindowListRecordD3FAD4& record : business.recordsD3FAD4) {
        record.value00 = 0;
        record.value408 = 0;
        record.texts08[1].clear();
        record.texts08[2].clear();
        record.texts08[3].clear();
        record.texts08[0].clear();
        record.value04 = 0;
    }
}

// 原函数：0x004B5AD0。
static std::int32_t WindowMessageBoard_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        inputs.messageBoardPrototype.input.context->clearText();
        WindowInstance* const window = currentWindow;
        static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.messageBoardPrototype, window->x, window->y, 0));
        input::SetContext_44B330(*callbackWindow.ownerSession, inputs.messageBoardPrototype.input.context);
        auto* const handleInputElement = currentWindow->FindElementById(WindowMessageBoardElementId::HandleInput);
        if (handleInputElement) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handleInputElement->data.input;
        }
        auto* const handleInputElement2 = currentWindow->FindElementById(WindowMessageBoardElementId::HandleInput);
        if (handleInputElement2) {
            handleInputElement2->data.input->context =
                inputs.messageBoardPrototype.input.context;
        }
    }
    WindowMessageBoard_RefreshRecords(&callbackWindow);
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        static_cast<void>(WindowMessageBoard_Close(&callbackWindow));
    }
    return 1;
}

// 原函数：0x004B5B50。
static void WindowMessageBoard_Release(WindowInstance&)
{
    if (world::gCurrentDialogType_581800 == 21) {
        world::gCurrentDialogType_581800 = -1;
    }
}

// 原函数：0x004B5B70。
static std::int32_t WindowMessageBoard_HandleInput(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_Hover) != 0 && !inputs.messageBoardPrototype.input.context->empty()) {
        gHoverTextD68AE8 = L"写下讯息。" /* 原常量参考：0x00587C84。 */;
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    WindowInstance* const window = currentWindow;
    auto* const indexedElement = window->FindElementById(elementId);
    if (indexedElement) {
        if (callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData != indexedElement->data.input) {
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.messageBoardPrototype, window->x, window->y, 0));
            input::SetContext_44B330(*callbackWindow.ownerSession, inputs.messageBoardPrototype.input.context);
            auto* const indexedElement2 = currentWindow->FindElementById(elementId);
            if (indexedElement2) {
                callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = indexedElement2->data.input;
            }
            auto* const indexedElement3 = currentWindow->FindElementById(elementId);
            if (indexedElement3) {
                indexedElement3->data.input->context =
                    inputs.messageBoardPrototype.input.context;
            }
        }
    }
    return 1;
}

// 原函数：0x004B5C30。
static std::int32_t WindowMessageBoard_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::wstring escaped;
    std::wstring text;
    std::int32_t consumed = 0;
    switch (static_cast<WindowMessageBoardElementId>(elementId)) {
    case WindowMessageBoardElementId::HandleButtons:
    case WindowMessageBoardElementId::HandleButtons_32: {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons) ? 243002 : 244547;
                const wchar_t* const hint = elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons)
                    ? L"关闭这个视窗。"
                    : kShopHints57E6C4[2];
                gHoverTextD68AE8 = hint;
                consumed = 1;
            } else {
                graphic->imageNo = elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons) ? 243001 : 244545;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(WindowMessageBoard_Close(&callbackWindow));
        }
        return consumed;
    }
    case WindowMessageBoardElementId::HandleButtons_6:
    case WindowMessageBoardElementId::HandleButtons_13:
    case WindowMessageBoardElementId::HandleButtons_20:
    case WindowMessageBoardElementId::HandleButtons_27: {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 36818;
                gHoverTextD68AE8 = L"删除此讯息。";
                consumed = 1;
            } else {
                graphic->imageNo = 36816;
            }
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        cg::text::FormatTo(text, L"{}", (index - 5) / 7);
        escaped = input::EscapeTextAndNotify(text, 255);
        const std::int32_t target = world::gDialogTargetD2DE6C;
        const std::int32_t request = world::gDialogRequestCC21E0;
        const std::int32_t y =
            static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x =
            static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 64, escaped));
        static_cast<void>(WindowMessageBoard_Close(&callbackWindow));
        return 1;
    }
    case WindowMessageBoardElementId::HandleButtons_31: {
        auto* const handleButtons_31Element = currentWindow->FindElementById(WindowMessageBoardElementId::HandleButtons_31);
        if (handleButtons_31Element) {
            auto* const graphic = handleButtons_31Element->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 244544;
                gHoverTextD68AE8 = L"写下讯息。" /* 原常量参考：0x00587C84。 */;
                consumed = 1;
            } else {
                graphic->imageNo = 244542;
            }
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        input::InputContext* const context = inputs.messageBoardPrototype.input.context;
        if (!context->empty()) {
            const std::wstring escapedText = input::EscapeTextAndNotify(input::ContextText(*context), 256);
            const std::int32_t target = world::gDialogTargetD2DE6C;
            const std::int32_t request = world::gDialogRequestCC21E0;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 1, escapedText));
            inputs.messageBoardPrototype.input.context->clearText();
            static_cast<void>(WindowMessageBoard_Close(&callbackWindow));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 1;
    }
    case WindowMessageBoardElementId::HandleButtons_33:
    case WindowMessageBoardElementId::HandleButtons_34: {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            const std::int32_t baseImage = elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_33) ? 243033 : 243030;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = baseImage + 2;
                const wchar_t* const hint = elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_33)
                    ? L"切换至下一页视窗。"
                    : L"回到上一页视窗。";
                gHoverTextD68AE8 = hint;
                consumed = 1;
            } else {
                graphic->imageNo = baseImage;
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                const std::int32_t target = world::gDialogTargetD2DE6C;
                const std::int32_t request = world::gDialogRequestCC21E0;
                escaped.clear();
                const std::int32_t y =
                    static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x =
                    static_cast<std::int32_t>(world::gPlayerX);
                static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, elementId == static_cast<ElementId>(WindowMessageBoardElementId::HandleButtons_33) ? 32 : 16, escaped));
                static_cast<void>(WindowMessageBoard_Close(&callbackWindow));
                consumed = 1;
            }
            if ((state & MouseFlag_LHoldPhysical) != 0) {
                if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = baseImage + 1;
            }
        }
        return consumed;
    }
    default:
        return 0;
    }
}

// 原函数：0x004B62A0。
static void WindowMessageBoard_RefreshRecords(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    std::wstring buffer;
    for (std::int32_t index = 0; index < 4; ++index) {
        const auto& record = business.recordsD3FAD4[index];
        const std::int32_t first = 2 + index * 7;
        auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(first));
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            graphic->imageNo = record.value00;
        }
        auto* const nameElement = currentWindow->FindElementById(NativeElementKey(first + 1));
        if (nameElement) {
            auto* const name = nameElement->data.text;
            name->text = record.texts08[0];
        }
        __time64_t time = static_cast<std::uint32_t>(record.value04);
        if (time > 0) {
            const std::tm* const calendar = currentWindow->ownerSession->LocalTime(&time);
            const std::int32_t minute = calendar->tm_min;
            const std::int32_t hour = calendar->tm_hour;
            const std::int32_t day = calendar->tm_mday;
            const std::int32_t month = calendar->tm_mon;
            const std::int32_t year = calendar->tm_year;
            cg::text::FormatTo(buffer, L"{:4}/{:02}/{:02}/{:02}:{:02}", year + 1900, month + 1, day, hour, minute);
            auto* const dateElement = currentWindow->FindElementById(NativeElementKey(first + 2));
            if (dateElement) {
                auto* const date = dateElement->data.text;
                date->text = buffer;
            }
        } else {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(first + 2)); indexedElement) indexedElement->visible = 0;
        }
        for (std::int32_t line = 1; line < 4; ++line) {
            auto* const textElement = currentWindow->FindElementById(NativeElementKey(first + 3 + line));
            if (textElement) {
                auto* const text = textElement->data.text;
                text->text = record.texts08[line];
            }
        }
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(first + 3)); indexedElement) indexedElement->visible = record.value408 != 0 ? 1 : 0;
    }
    if (auto* const handleButtons_33Element = currentWindow->FindElementById(WindowMessageBoardElementId::HandleButtons_33); handleButtons_33Element) handleButtons_33Element->visible = (business.valueD3FAD0 & 0x20u) != 0 ? 1 : 0;
    if (auto* const handleButtons_34Element = currentWindow->FindElementById(WindowMessageBoardElementId::HandleButtons_34); handleButtons_34Element) handleButtons_34Element->visible = (business.valueD3FAD0 & 0x10u) != 0 ? 1 : 0;
    WindowInstance* const window = currentWindow;
    auto* const handleInputElement = window->FindElementById(WindowMessageBoardElementId::HandleInput);
    if (handleInputElement) {
        if (currentWindow->ownerSession->UiFrame().native.pointer.focusedElementData == handleInputElement->data.input) {
            static_cast<void>(WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.messageBoardPrototype, window->x, window->y, 2));
        }
    }
    return;
}

// 原函数：0x004B6C20。
static std::int32_t WindowMessageBoard_Close(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    world::gCurrentDialogType_581800 = -1;
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
}

// 原函数：0x004B6970。
std::int32_t WindowMessageBoard_ReceiveRecords(session::ClientSession& session, const std::int32_t flags, char* const text)
{
    auto& business = session.UiFrame().native.business;
    business.valueD3FAD0 = static_cast<std::uint32_t>(flags);
    input::UnescapeLegacyText_52D150(text);
    gWindow50AlternateRecordsD30138.fill(std::byte{0});
    char titleGbk[17]{};
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 1, 16, titleGbk));
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(titleGbk);
    std::array<char, 3072> buffer;
    char* const fieldText = buffer.data();
    char* const copiedText = buffer.data() + 768;
    std::int32_t field = 2;
    std::int32_t offset = 0;
    do {
        auto& record = business.recordsD3FAD4[offset / 0x40C];
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value00 = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(imageNo);
        record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value408 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        record.texts08[0] = cg::text::FromGbkWide(std::strlen(fieldText) > 16 ? "???" : fieldText);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        std::strcpy(copiedText, fieldText);
        const auto length = static_cast<std::int32_t>(std::strlen(fieldText));
        if (std::strlen(fieldText) > 140) {
            record.texts08[1].clear();
            record.texts08[2].clear();
            record.texts08[3].clear();
        } else {
            std::int32_t position = 0;
            for (std::int32_t line = 1; line < 4; ++line) {
                char destination[256]{};
                std::int32_t used = 0;
                for (;;) {
                    const char value = copiedText[position];
                    if (value == '\r') {
                        ++position;
                        break;
                    }
                    ++position;
                    destination[used] = value;
                    if (position > length) {
                        break;
                    }
                    if (++used >= 70) {
                        break;
                    }
                }
                destination[used] = '\0';
                if (input::ClassifyLastAnsiUnit_44B190(destination) == 3) {
                    --position;
                    destination[used - 1] = '\0';
                }
                record.texts08[line] = cg::text::FromGbkWide(destination);
            }
        }
        offset += 0x40C;
    } while (offset < 0x1030);
    return offset;
}

} // namespace detail

} // namespace cg::ui
