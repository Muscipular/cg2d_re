#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/core/m_log.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldObjectState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowDialogElementId : ElementId {
    HandleButtons = 1,
    HandleButtons_2 = 2,
    HandleButtons_3 = 3,
    HandleButtons_4 = 4,
    HandleButtons_5 = 5,
    HandleButtons_6 = 6,
    HandleTextSelection = 7,
    Element8 = 8,
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
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049CE70。
static std::int32_t WindowDialog_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六按钮与原位图号恢复。

// 原函数：0x0049CF70。
static std::int32_t WindowDialog_HandleTextSelection(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 输入行切换/文本行选择。

// 原函数：0x0049DA30。
static std::int32_t WindowDialog_Reply(
    session::ClientSession& session, std::int32_t button, std::int32_t row,
    const std::wstring* texts, std::uint8_t inputCount);

// 原函数：0x0049D2E0。
static void WindowDialog_Route(
    session::ClientSession& session,
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // SRD01：完整同类续接/重入新建及原五参数分派，原位text与副作用保持，非全机器返回等价。

// 原函数：0x0050E130。
static void WindowDialog_RouteReply(
    cg::network::CgConnection& networkConnection, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text); // 50E110门禁后原共享尾入口。

// 原函数：0x0049DDB0。
static void WindowDialog_ParseGraphicTag(session::ClientSession& session, const char* text, std::int16_t x, std::int16_t y); // 内嵌g,标记原字节解析。

// 原函数：0x0049DF30。
static cg::yobi::YobiAction* WindowDialog_OpenText(
    session::ClientSession& session, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, std::int32_t request,
    std::int32_t target, char* text); // 原位文本处理，借用共享行表创建常规对话。

// 原函数：0x0049E1D0。
static std::int32_t WindowDialog_DrawHealing(
    session::ClientSession& session, WindowInstance* window,
    std::int32_t button, std::int32_t row, std::int32_t unusedEvents);

// 原函数：0x0049E400。
static cg::yobi::YobiAction* WindowDialog_OpenHealing(
    session::ClientSession& session, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t unusedButtons, std::int32_t request,
    std::int32_t target, const char* text); // 六治疗记录及第七合计项，共借CB1398行表。

// 原函数：0x0049C710。
static std::int32_t WindowDialog_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049CAD0。
static std::int32_t WindowDialog_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049CDF0。
static void WindowDialog_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

static constexpr std::array<std::int32_t, 5> gHealthImages57CBA4 {22520, 22521, 22522, 22523, 22524}; // 原全局：0x0057CBA4。

static_assert(std::is_same_v<decltype(&WindowDialog_Reply), WindowDialogReplyCallback>);

static_assert(std::is_same_v<decltype(&WindowDialog_DrawHealing), WindowDialogFrameCallback>);

static WindowGraphicData gWindowGraphic56C9F8 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C9F8。

static WindowGraphicData gWindowGraphic56CA10 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA10。

static WindowGraphicData gWindowGraphic56CA28 {0x3BE4B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA28。

static WindowGraphicData gWindowGraphic56CA40 {0x3BE4E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA40。

static WindowGraphicData gWindowGraphic56CA58 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA58。

static WindowGraphicData gWindowGraphic56CA70 {0x3BE51, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA70。

static std::int32_t gWindowAnimationImageD6780C = 0; // 原全局：0x00D6780C。

static WindowElementPrototype gWindowElements56CA88[19] { // 原全局：0x0056CA88。
    {WindowElementType_Graphic, 0, 0, 66, 17, 1, {.graphic = &gWindowGraphic56C9F8}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons)},
    {WindowElementType_Graphic, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA10}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons_2)},
    {WindowElementType_Graphic, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA28}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons_3)},
    {WindowElementType_Graphic, 60, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA40}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons_4)},
    {WindowElementType_Graphic, 60, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA58}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons_5)},
    {WindowElementType_Graphic, 120, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA70}, &WindowDialog_HandleButtons, -1, static_cast<ElementId>(WindowDialogElementId::HandleButtons_6)},
    {WindowElementType_EventArea, 120, 0, 28, 8, 1, {.unusedValue = 0}, &WindowDialog_HandleTextSelection, -1, static_cast<ElementId>(WindowDialogElementId::HandleTextSelection)},
    {WindowElementType_Input, 80, 42, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element8)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element9)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element10)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element11)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element12)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element13)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element14)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element15)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element16)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element17)},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowDialogElementId::Element18)},
    // 原运行第19项与下一文本/元素源字节重叠；值1不是函数，visible0阻止正常调用。
    {WindowElementType_EventArea, 0x593850, 3, 0, 44, 0, {.unusedValue = 0},
        reinterpret_cast<WindowElementCallback>(std::uintptr_t{1}), -1, static_cast<ElementId>(WindowDialogElementId::Element19)},
};

// 原型：窗口36；原全局：0x0054E870。
WindowPrototype gWindowPrototypeDialog {2, 3, 0, 0, 0, 30, 0x80010101u, 19, gWindowElements56CA88, &WindowDialog_Update, &WindowDialog_Draw, &WindowDialog_Release}; // 36: 54E870

// 窗口函数实现。

// 原函数：0x0049C4D0。
void WindowDialog_Reset(session::ClientSession& session)
{
    auto& dialog = session.UiFrame().native.dialog;
    for (auto& image : dialog.images) {
        image.imageNo = 0;
    }
    dialog.imageCount = 0;
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_Dialog].window;
    if (window != nullptr) {
        for (std::int32_t index = 8; index < 18; ++index) {
            if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        }
    }
}

// 原函数：0x0049C510。
void WindowDialog_AddImage(
    session::ClientSession& session, const std::uint8_t used, const std::int32_t imageNo, const std::uint8_t direction,
    const std::int16_t x, const std::int16_t y)
{
    auto& dialog = session.UiFrame().native.dialog;
    const std::int32_t count = std::bit_cast<std::int8_t>(dialog.imageCount);
    if (count >= 10) {
        return;
    }
    auto& image = dialog.images[count];
    image.imageNo = imageNo;
    image.direction = direction;
    image.x = x;
    image.y = y;
    image.used = used;
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_Dialog].window;
    if (window != nullptr) {
        auto* const element = window->FindElementById(NativeElementKey(count + 8));
        if (element) {
            cg::yobi::SetYobiActionField2A8(
                element->data.animation->action, imageNo);
            cg::yobi::SetYobiActionField2B8(
                element->data.animation->action,
                std::bit_cast<std::int8_t>(direction));
            element->visible = 0;
            element->x = x;
            element->y = y;
            if (used != 0) {
                element->visible = 1;
            }
        }
    }
    ++dialog.imageCount;
}

// 原函数：0x0049C600。
cg::yobi::YobiAction* WindowDialog_Open(
    session::ClientSession& session, std::int32_t x, std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t buttons, WindowDialogTextLine* const lines,
    const std::int32_t lineCount, const std::int32_t lineHeight,
    const std::int32_t firstSelectableLine, const std::uint8_t inputCount,
    std::int16_t* const inputY, const std::int32_t inputCapacity,
    const std::uint8_t distance, const std::uint8_t options,
    const WindowDialogReplyCallback reply, const WindowDialogFrameCallback frame,
    const std::uint32_t flags, const std::uint32_t mode)
{
    auto& dialog = session.UiFrame().native.dialog;
    m_log_iw(L"%d %d %d %d => ", x, y, width, height);
    AdaptWindowPosition(x, y, width, height);
    x = static_cast<std::int16_t>(x);
    y = static_cast<std::int16_t>(y);
    // 36／37共享动态原型；恢复位置应使用本次请求的适配结果，不再回到占位的0,0。
    auto& prototype = WindowRuntime_DialogPrototype(session);
    prototype.defaultX = x;
    prototype.defaultY = y;
    m_log_iw(L"%d %d %d %d\n", x, y, width, height);
    dialog.message.width = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(width));
    dialog.message.lines = lines;
    dialog.message.firstSelectableLine = firstSelectableLine;
    dialog.message.inputY = inputY;
    dialog.message.inputCount = inputCount;
    WindowRuntime_States(session)[WindowId_DialogCreate].y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    dialog.message.inputCapacity = inputCapacity;
    WindowRuntime_States(session)[WindowId_DialogCreate].x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    dialog.message.imageNo = imageNo;
    dialog.message.distance = distance;
    dialog.message.lineCount = lineCount;
    dialog.message.options = options;
    const auto height16 = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(height));
    dialog.message.height = height16;
    dialog.message.reply = reply;
    dialog.message.buttons = buttons;
    dialog.message.lineHeight = lineHeight;
    dialog.message.frame = frame;
    const std::int32_t maximumLines = cg::yobi::CalculateDialogVisibleLines_49C5E0(height16, buttons, lineHeight);
    if (dialog.message.lineCount > maximumLines) {
        dialog.message.lineCount = maximumLines;
    }
    WindowInstance* const first = WindowRuntime_States(session)[WindowId_Dialog].window;
    if (first != nullptr) {
        first->state |= 0x8000u;
    }
    WindowInstance* const second = WindowRuntime_States(session)[WindowId_DialogCreate].window;
    if (second != nullptr) {
        second->state |= 0x8000u;
    }
    return cg::yobi::CreateWindowOpeningAnimation_475950(session, x, y, width, height, 37, flags, mode, nullptr);
}

// 原函数：0x0049C710。
static std::int32_t WindowDialog_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& dialog = callbackWindow.ownerSession->UiFrame().native.dialog;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const initialWindow = currentWindow;
    if (dialog.message.closeCallbackPending != 0 && (initialWindow->state & 0x8000u) != 0) {
        for (std::int32_t index = 0; index < 19; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        return 1;
    }
    if (value == -1) {
        const std::int32_t width = dialog.message.width;
        const std::int32_t buttons = dialog.message.buttons;
        initialWindow->width = width;
        const std::int32_t margin = buttons != 0 ? 24 : 0;
        currentWindow->height = dialog.message.height;
        // WindowDialog_Open已按实际请求矩形定位，此处只发布尺寸，避免再次适配。
        const std::uint32_t mask = static_cast<std::uint32_t>(dialog.message.buttons);
        std::int32_t buttonCount = 0;
        for (std::uint32_t bit = 1; bit < 64; bit <<= 1) {
            if ((mask & bit) != 0) {
                ++buttonCount;
            }
        }
        const std::int32_t spacing = (currentWindow->width - 48) / (buttonCount + 1);
        std::int32_t x = spacing - 9;
        for (std::int32_t index = 0, bit = 1; index < 6; ++index, bit <<= 1) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
            if ((dialog.message.buttons & bit) != 0) {
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->visible = 1;
                if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement3) indexedElement3->width = 66;
                if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement4) indexedElement4->height = 17;
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement5) indexedElement5->x = x;
                x += spacing;
                WindowInstance* const window = currentWindow;
                if (auto* const indexedElement6 = window->FindElementById(NativeElementKey(index)); indexedElement6) indexedElement6->y = window->height + margin / 2 - margin - 25;
            }
        }
        if (auto* const handleTextSelectionElement = currentWindow->FindElementById(WindowDialogElementId::HandleTextSelection); handleTextSelectionElement) handleTextSelectionElement->x = 0;
        if (auto* const handleTextSelectionElement2 = currentWindow->FindElementById(WindowDialogElementId::HandleTextSelection); handleTextSelectionElement2) handleTextSelectionElement2->y = 0;
        WindowInstance* window = currentWindow;
        if (auto* const handleTextSelectionElement3 = window->FindElementById(WindowDialogElementId::HandleTextSelection); handleTextSelectionElement3) handleTextSelectionElement3->width = window->width;
        window = currentWindow;
        if (auto* const handleTextSelectionElement4 = window->FindElementById(WindowDialogElementId::HandleTextSelection); handleTextSelectionElement4) handleTextSelectionElement4->height = window->height;
        const std::int32_t count = std::bit_cast<std::int8_t>(dialog.message.inputCount);
        dialog.message.selectedInput = -1;
        if (count > 0) {
            dialog.message.inputTexts.clear();
            try { dialog.message.inputTexts.resize(static_cast<std::size_t>(count)); }
            catch (const std::bad_alloc&) { dialog.message.inputTexts.clear(); }
            dialog.inputPrototype.maxCharPerLine = dialog.message.inputCapacity;
            if (auto* const element8 = currentWindow->FindElementById(WindowDialogElementId::Element8); element8) element8->visible = 1;
            const std::int16_t inputY = dialog.message.inputY[0];
            window = currentWindow;
            const std::int32_t y = window->y + inputY;
            const std::int32_t inputX = window->x + 24;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &dialog.inputPrototype, inputX, y, 0));
            input::SetContext_44B330(*callbackWindow.ownerSession, &dialog.input);
            window = currentWindow;
            auto* const element82 = window->FindElementById(WindowDialogElementId::Element8);
            if (element82) {
                callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = element82->data.input;
            }
            if (auto* const element83 = window->FindElementById(WindowDialogElementId::Element8); element83) element83->data.input->context = &dialog.input;
            dialog.message.selectedInput = 0;
        }
        dialog.message.closeCallbackPending = 1;
        for (std::int32_t index = 8; index < 18; ++index) {
            const auto& image = dialog.images[index - 8];
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index));
            if (indexedElement) {
                cg::yobi::SetYobiActionField2A8(
                    indexedElement->data.animation->action,
                    image.imageNo);
            }
            auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index));
            if (indexedElement2) {
                cg::yobi::SetYobiActionField2B8(
                    indexedElement2->data.animation->action,
                    std::bit_cast<std::int8_t>(image.direction));
            }
            auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(index));
            if (indexedElement3) {
                cg::yobi::SetYobiActionMixFlagsOverrideValue(
                    indexedElement3->data.animation->action, render::GraphicMixFlag_OriginalPalette);
            }
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(index));
            if (indexedElement4) {
                cg::yobi::SetYobiActionMixFlagsOverrideMask(
                    indexedElement4->data.animation->action, render::GraphicMixFlag_OriginalPalette);
            }
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement5) indexedElement5->x = image.x;
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement6) indexedElement6->y = image.y;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement7) indexedElement7->visible = 0;
            if (image.used != 0) {
                if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement8) indexedElement8->visible = 1;
            }
        }
        return 1;
    }
    if (std::bit_cast<std::int8_t>(dialog.message.inputCount) > 0
        && dialog.message.inputTexts.empty()) {
        initialWindow->state |= 0x8000u;
        dialog.message.closeCallbackPending = 0;
    }
    dialog.message.hoveredButton = 0;
    dialog.message.hoveredRow = -1;
    dialog.message.events = MouseFlag_None;
    return 1;
}

// 原函数：0x0049CAD0。
static std::int32_t WindowDialog_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& dialog = callbackWindow.ownerSession->UiFrame().native.dialog;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = currentWindow;
    if (dialog.message.closeCallbackPending != 0 && (window->state & 0x8000u) != 0) {
        return 1;
    }
    render::GraphicDescriptor16 descriptor {};
    const std::int16_t offsetX = -150;
    const std::int16_t offsetY = -8;
    const std::int16_t height = 16;
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.sourceX = offsetX;
    descriptor.sourceY = offsetY;
    descriptor.sourceHeight = height;
    descriptor.color = color;
    descriptor.clipMode = 1;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if ((dialog.message.options & 4u) == 0) {
        const std::int32_t distance = std::bit_cast<std::int8_t>(dialog.message.distance);
        const std::int32_t close = WindowProfile_CheckDistance(distance, distance);
        window = currentWindow;
        if (close != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            window = currentWindow;
        }
    }
    const WindowDialogFrameCallback frame = dialog.message.frame;
    if (frame != nullptr) {
        const std::int32_t button = dialog.message.hoveredButton;
        if (button != 0) {
            dialog.message.events = MouseFlag_None;
        }
        const std::int32_t row = dialog.message.hoveredRow;
        if (row > 0) {
            dialog.message.events = MouseFlag_None;
        }
        const std::int32_t events = dialog.message.events;
        const std::int32_t close = frame(*callbackWindow.ownerSession, window, button, row, events);
        window = currentWindow;
        if (close != 0) {
            window->state |= 0x8000u;
            window = currentWindow;
            dialog.message.closeCallbackPending = 0;
        }
    }
    for (std::int32_t row = 0; row < dialog.message.lineCount; ++row) {
        const std::wstring& text = dialog.message.lines[row].text;
        if (!text.empty()) {
            static_cast<void>(render::DrawColoredText(
                static_cast<std::int16_t>(window->x + 24),
                static_cast<std::int16_t>(window->y + row * dialog.message.lineHeight + 20),
                3, 0, 0, text, 0, static_cast<std::int8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
            window = currentWindow;
        }
    }
    std::int32_t inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
    if (inputCount > 0) {
        const std::int32_t inputX = window->x + 24;
        std::int32_t selected = dialog.message.selectedInput;
        if (selected >= 0 && callbackWindow.ownerSession->UiFrame().inputState.currentContext != &dialog.input) {
            dialog.message.inputTexts[selected] = input::ContextText(dialog.input);
            selected = -1;
            window = currentWindow;
            inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
            dialog.message.selectedInput = -1;
        }
        std::int32_t row = 0;
        while (row < inputCount) {
            if (row == selected) {
                render::DrawInputText_4568A0(*callbackWindow.ownerSession, callbackWindow.ownerSession->UiFrame().inputState.currentContext);
            } else {
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(inputX),
                    static_cast<std::int16_t>(window->y + dialog.message.inputY[row]),
                    3, 0, 0, dialog.message.inputTexts[row], 0,
                    static_cast<std::int8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
            }
            inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
            ++row;
            window = currentWindow;
            if (row < inputCount) {
                selected = dialog.message.selectedInput;
            }
        }
        if ((dialog.message.options & 2u) != 0) {
            const std::int32_t right = window->width - 24;
            std::int32_t left = 24;
            while (left < right) {
                auto width = static_cast<std::int16_t>(right - left);
                if (width > 300) {
                    width = 300;
                }
                descriptor.sourceWidth = width;
                std::int32_t input = 0;
                if (inputCount > 0) {
                    do {
                        const std::int32_t y = window->y + dialog.message.inputY[input];
                        const std::int32_t x = window->x + left;
                        static_cast<void>(render::DrawGraphic3_520970(
                            x, y, 223, 243225, 0, 1, 0, &descriptor));
                        inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
                        ++input;
                        if (input < inputCount) {
                            window = currentWindow;
                        }
                    } while (input < inputCount);
                    width = descriptor.sourceWidth;
                    window = currentWindow;
                }
                left += width;
            }
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    window = currentWindow;
    cg::yobi::DrawNineGrid_49BE00(
        dialog.message.imageNo, static_cast<std::int16_t>(window->x),
        static_cast<std::int16_t>(window->y), static_cast<std::int16_t>(window->width),
        static_cast<std::int16_t>(window->height), 223, -1,
        std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049CDF0。
static void WindowDialog_Release(WindowInstance& releasedWindow)
{
    auto& dialog = releasedWindow.ownerSession->UiFrame().native.dialog;
    if (dialog.message.closeCallbackPending != 0) {
        const WindowDialogReplyCallback reply = dialog.message.reply;
        if (reply != nullptr) {
            static_cast<void>(reply(*releasedWindow.ownerSession, 0, -1, nullptr, 0));
        }
    }
    std::int32_t count = std::bit_cast<std::int8_t>(dialog.message.inputCount);
    if (count <= 0) {
        return;
    }
    dialog.message.inputTexts.clear(); // 原0x0049CD70：正文由宽字符串结束寿命。
}

// 原函数：0x0049CE70。
static std::int32_t WindowDialog_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& dialog = callbackWindow.ownerSession->UiFrame().native.dialog;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static constexpr std::int32_t images[6][3] {
        {243048, 243049, 243050},
        {245320, 245321, 245322},
        {245323, 245324, 245325},
        {245326, 245327, 245328},
        {245332, 245333, 245334},
        {245329, 245330, 245331},
    };
    auto* const dataElement = currentWindow->FindElementById(elementId);
    if (dataElement) {
        auto* const data = dataElement->data.graphic;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hovered) {
            dialog.message.hoveredButton =
                std::bit_cast<std::int32_t>(1u << (static_cast<std::uint32_t>(index) & 31u));
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (dialog.message.reply != nullptr) {
                const std::int32_t selected = dialog.message.selectedInput;
                if (selected >= 0) {
                    dialog.message.inputTexts[selected] = input::ContextText(dialog.input);
                }
                const std::uint8_t count = dialog.message.inputCount;
                const std::wstring* const texts = dialog.message.inputTexts.data();
                const auto button =
                    std::bit_cast<std::int32_t>(1u << (static_cast<std::uint32_t>(index) & 31u));
                if (dialog.message.reply(*callbackWindow.ownerSession, button, -1, texts, count) != 0) {
                    currentWindow->state |= 0x8000u;
                    dialog.message.closeCallbackPending = 0;
                }
            }
            consumed = 1;
        }
        data->imageNo = images[index][0];
        if (hovered) {
            data->imageNo = images[index][2];
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            data->imageNo = images[index][1];
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049CF70。
static std::int32_t WindowDialog_HandleTextSelection(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& dialog = callbackWindow.ownerSession->UiFrame().native.dialog;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
    dialog.message.events = state;
    WindowInstance* window = currentWindow;
    const auto inputX = static_cast<std::int16_t>(window->x + 24);
    const auto inputWidth = static_cast<std::int16_t>(window->width - 48);
    std::int32_t consumed = 0;
    std::int32_t selected = 0;
    if (inputCount > 0) {
        do {
            const std::int32_t top = dialog.message.inputY[selected] + window->y;
            if (cg::ui::HitTestMouseRect_5049A0(inputX, top, inputX + inputWidth, top + 16, -1) != 0) {
                break;
            }
            inputCount = std::bit_cast<std::int8_t>(dialog.message.inputCount);
            ++selected;
            if (selected < inputCount) {
                window = currentWindow;
            }
        } while (selected < inputCount);
        window = currentWindow;
    }
    if (selected < std::bit_cast<std::int8_t>(dialog.message.inputCount)) {
        if (selected != dialog.message.selectedInput) {
            const std::int32_t top = dialog.message.inputY[selected] + window->y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(inputX), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(inputX + inputWidth), static_cast<std::uint16_t>(top + 16),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            if ((state & MouseFlag_LDownEvent) != 0) {
                const std::int32_t previous = dialog.message.selectedInput;
                if (previous >= 0) {
                    dialog.message.inputTexts[previous] = input::ContextText(dialog.input);
                }
                if (auto* const element8 = currentWindow->FindElementById(WindowDialogElementId::Element8); element8) element8->visible = 1;
                const std::int16_t inputY = dialog.message.inputY[selected];
                const std::int32_t y = inputY + currentWindow->y;
                static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &dialog.inputPrototype, inputX, y, 0));
                input::SetContext_44B330(*callbackWindow.ownerSession, &dialog.input);
                window = currentWindow;
                auto* const element82 = window->FindElementById(WindowDialogElementId::Element8);
                if (element82) {
                    callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = element82->data.input;
                }
                if (auto* const element83 = window->FindElementById(WindowDialogElementId::Element8); element83) element83->data.input->context = &dialog.input;
                const std::wstring* const texts = dialog.message.inputTexts.data();
                dialog.message.selectedInput = selected;
                input::LoadInitialText_44A160(*callbackWindow.ownerSession, texts[selected]);
                consumed = 1;
            }
        }
    } else if (dialog.message.firstSelectableLine > 0) {
        const auto left = static_cast<std::int16_t>(window->x + 22);
        const auto top = static_cast<std::int16_t>(window->y + 18);
        const auto width = static_cast<std::int16_t>(window->width - 48);
        std::int32_t lineCount = dialog.message.lineCount;
        std::int32_t offset = 0;
        std::int32_t row = 0;
        while (row < lineCount) {
            if (cg::ui::HitTestMouseRect_5049A0(left, top + offset, left + width, top + offset + 20, -1) != 0) {
                lineCount = dialog.message.lineCount;
                if (row < lineCount) {
                    dialog.message.hoveredRow = row;
                }
                break;
            }
            const std::int32_t lineHeight = dialog.message.lineHeight;
            lineCount = dialog.message.lineCount;
            offset += lineHeight;
            ++row;
        }
        const std::int32_t lineHeight = dialog.message.lineHeight;
        row = dialog.message.firstSelectableLine;
        offset = row * lineHeight;
        if (row < lineCount) {
            bool found = false;
            do {
                if (cg::ui::HitTestMouseRect_5049A0(
                        left, top + offset, left + width, top + offset + 20, -1) != 0) {
                    found = true;
                    break;
                }
                const std::int32_t currentLineHeight = dialog.message.lineHeight;
                lineCount = dialog.message.lineCount;
                offset += currentLineHeight;
                ++row;
            } while (row < lineCount);
            if (found && row < dialog.message.lineCount
                && !dialog.message.lines[row].text.empty() && row >= 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top + offset),
                    static_cast<std::uint16_t>(left + width), static_cast<std::uint16_t>(top + offset + 20),
                    223, cg::render::gHoverFramePaletteIndex_D69768, 0);
                if ((state & MouseFlag_LDownEvent) != 0) {
                    const std::int32_t previous = dialog.message.selectedInput;
                    if (previous >= 0) {
                        dialog.message.inputTexts[previous] = input::ContextText(dialog.input);
                    }
                    const WindowDialogReplyCallback reply = dialog.message.reply;
                    if (reply != nullptr) {
                        const std::uint8_t count = dialog.message.inputCount;
                        const std::int32_t first = dialog.message.firstSelectableLine;
                        const std::wstring* const texts = dialog.message.inputTexts.data();
                        if (reply(*callbackWindow.ownerSession, 0, row - first, texts, count) != 0) {
                            currentWindow->state |= 0x8000u;
                            dialog.message.closeCallbackPending = 0;
                        }
                    }
                    consumed = 1;
                }
            }
        }
    }
    return (dialog.message.options & 1u) != 0 ? 1 : consumed;
}

// 原函数：0x0049DA30。
static std::int32_t WindowDialog_Reply(
    session::ClientSession& session, const std::int32_t button, const std::int32_t row,
    const std::wstring* const texts, const std::uint8_t inputCount)
{
    auto& dialog = session.UiFrame().native.dialog;
    std::wstring reply;
    if (button == 0) {
        if (row < 0) { world::gCurrentDialogType_581800 = -1; }
    } else {
        if (dialog.reply.type == 2) {
            cg::core::NullSub_509080();
            reply = input::EscapeText(std::to_wstring(-dialog.reply.firstLine));
        } else if (dialog.reply.type == 35) {
            if ((button & 1) != 0) {
                if (texts[0].empty() && texts[1].empty()) { return 0; }
                if (input::IsValidDelimitedFieldText_476740(texts[0]) == 0) { return 0; }
                if (input::IsValidDelimitedFieldText_476740(texts[1]) == 0) { return 0; }
                if (input::ContainsPipeOrBackslash_45E770(texts[0]) == 1) { return 0; }
                if (input::ContainsPipeOrBackslash_45E770(texts[1]) == 1) { return 0; }
            }
            cg::core::NullSub_509080();
            const std::wstring first = input::EscapeText(texts[0]);
            cg::core::NullSub_509080();
            const std::wstring second = input::EscapeText(texts[1]);
            reply = input::EscapeText(first + L"|" + second);
        } else {
            cg::core::NullSub_509080();
            reply = input::EscapeText(inputCount == 1 ? std::wstring_view(texts[0]) : std::wstring_view{});
        }
        // 原255B协议预算保留；动态编辑正文不在此静默截断。
        if (cg::text::ToGbk(reply).size() >= 255) {
            throw cg::text::GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "回复超过GBK协议字段容量");
        }
        const std::int32_t target = dialog.reply.target;
        const std::int32_t request = dialog.reply.request;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        if (session.Connection().SendDialogResponse(x, y, request, target, button,
                reply) != network::SendResult::Queued) { return 0; }
        world::gCurrentDialogType_581800 = -1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (row >= 0) {
        cg::core::NullSub_509080();
        reply = input::EscapeText(std::to_wstring(row + 1));
        const std::int32_t target = dialog.reply.target;
        const std::int32_t request = dialog.reply.request;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        if (session.Connection().SendDialogResponse(x, y, request, target, 0,
                reply) != network::SendResult::Queued) { return 0; }
        world::gCurrentDialogType_581800 = -1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    return 1;
}

// 原函数：0x0049DDB0。
static void WindowDialog_ParseGraphicTag(session::ClientSession& session, const char* const text, std::int16_t x, std::int16_t y)
{
    if (text[0] != 'g' || text[1] != ',') {
        return;
    }
    // 原32B区跨字段复用；空格只跳过写入，不压缩索引或初始化残留。
    std::array<char, 32> number;
    const char* cursor = text + 2;
    std::int32_t index = 0;
    while (cursor[index] != '\0' && cursor[index] != ',' && cursor[index] != '(') {
        if (cursor[index] != ' ') {
            number[index] = cursor[index];
        }
        ++index;
    }
    const char delimiter = cursor[index];
    number[index] = '\0';
    cursor += index + 1;
    const std::int32_t imageNo = std::atoi(number.data());
    std::uint8_t direction = 0;
    if (delimiter == ',') {
        ++cursor;
        index = 0;
        while (cursor[index] != '\0' && cursor[index] != ',' && cursor[index] != '(') {
            if (cursor[index] != ' ') {
                number[index] = cursor[index];
            }
            ++index;
        }
        number[index] = '\0';
        cursor += index + 1;
        direction = static_cast<std::uint8_t>(std::atoi(number.data()));
    }
    const char* const coordinates = const_cast<char*>(std::strchr(cursor, '('));
    if (coordinates != nullptr) {
        cursor = coordinates + 1;
        if (const_cast<char*>(std::strchr(cursor, ')')) != nullptr) {
            index = 0;
            while (cursor[index] != '\0' && cursor[index] != ',') {
                if (cursor[index] != ' ') {
                    number[index] = cursor[index];
                }
                ++index;
            }
            number[index] = '\0';
            cursor += index + 1;
            x = static_cast<std::int16_t>(std::atoi(number.data()));
            index = 0;
            while (cursor[index] != '\0' && cursor[index] != ',') {
                if (cursor[index] != ' ') {
                    number[index] = cursor[index];
                }
                ++index;
            }
            number[index] = '\0';
            y = static_cast<std::int16_t>(std::atoi(number.data()));
        }
    }
    WindowDialog_AddImage(session, 1, imageNo, direction, x, y);
}

// SRD01：原49D2E0完整515条；三阶段分派/原六处存放解析合并条件、fresh窗口与Y-X RNG/状态发布保持。
// 原函数：0x0049D2E0。
static void WindowDialog_Route(
    session::ClientSession& session,
    const std::int32_t type, const std::int32_t value, const std::int32_t request,
    const std::int32_t target, char* const text)
{
    std::int32_t currentType = world::gCurrentDialogType_581800;
    cg::yobi::YobiAction* created = nullptr;
    if (currentType >= 0) {
        switch (request) {
        case 326:
        case 327:
        case 369:
        case 388:
        case 389:
        case 390:
            if (type != 32) {
                WindowRuntime_CloseByCategory(session, 2);
                WindowRuntime_CloseByCategory(session, 3);
                currentType = -1;
                world::gCurrentDialogType_581800 = -1;
            }
            break;
        default:
            break;
        }
    }
    bool enterNewType = false;
    if (type == currentType) {
        switch (currentType) {
        case 21: {
            world::gCurrentDialogType_581800 = type;
            gDialogValueCF4E5C = value;
            world::gDialogRequestCC21E0 = request;
            world::gDialogTargetD2DE6C = target;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(WindowProfile_SaveOrigin(x, y));
            static_cast<void>(WindowMessageBoard_ReceiveRecords(value, text));
            static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_MessageBoard, 1, 0, nullptr));
            break;
        }
        case 23:
        case 33:
            if (WindowBank_ReceiveStorageData(text) != 0) {
                if (WindowRuntime_States(session)[WindowId_Bank].window != nullptr) {
                    break;
                }
                created = cg::yobi::CreateBankWindowOpeningAnimation_4ABEF0(type == 33 ? 1 : 0, request, target, 1, 0);
                if (created != nullptr) {
                    break;
                }
            }
            if (WindowRuntime_States(session)[WindowId_Bank].window == nullptr) {
                enterNewType = true;
            }
            break;
        case 34:
            if (WindowBank_ReceiveStorageData(text) != 0) {
                if (WindowRuntime_States(session)[WindowId_ItemStorage].window != nullptr) {
                    break;
                }
                created = cg::yobi::CreateItemStorageWindowOpeningAnimation_4ACE80(0, request, target, 0, 1, 0);
                if (created != nullptr) {
                    break;
                }
            }
            if (WindowRuntime_States(session)[WindowId_ItemStorage].window == nullptr) {
                enterNewType = true;
            }
            break;
        case 32:
            if (WindowRuntime_States(session)[WindowId_TextList].window == nullptr) {
                enterNewType = true;
            } else {
                WindowTextList_ContinueDialog(text);
            }
            break;
        default:
            break;
        }
    }
    if (!enterNewType && world::gCurrentDialogType_581800 >= 0) {
        return;
    }
    world::gCurrentDialogType_581800 = type;
    gDialogValueCF4E5C = value;
    world::gDialogRequestCC21E0 = request;
    world::gDialogTargetD2DE6C = target;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    switch (type) {
    case 0:
    case 1:
    case 2:
    case 35:
        static_cast<void>(WindowDialog_OpenText(session, 86, 98, 468, 260, 245300, value, request, target, text));
        return;
    case 10:
    case 11:
        static_cast<void>(WindowDialog_OpenText(session, 32, 12, 576, 432, 245300, value, request, target, text));
        return;
    case 5:
        static_cast<void>(WindowShopChoice_OpenBuySell(text));
        return;
    case 6:
        static_cast<void>(WindowShop_OpenBuy(session, text));
        return;
    case 7:
        static_cast<void>(WindowShop_OpenSell(session, text));
        return;
    case 16:
        static_cast<void>(WindowShopChoice_OpenSkillChoice(text));
        return;
    case 17:
        static_cast<void>(WindowPetSkillChoice_OpenLearn(text));
        return;
    case 18:
        static_cast<void>(WindowPetSkillChoice_OpenForget(text));
        return;
    case 19:
        static_cast<void>(WindowDialog_OpenHealing(session, 96, 108, 448, 240, 245300, value, request, target, text));
        return;
    case 20:
        static_cast<void>(WindowShop_OpenIdentify(session, text));
        return;
    case 21:
        static_cast<void>(WindowMessageBoard_ReceiveRecords(value, text));
        static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_MessageBoard, 1, 0, nullptr));
        return;
    case 22:
        static_cast<void>(WindowShop_OpenRepair(session, text));
        return;
    case 23:
    case 33:
        if (WindowBank_ReceiveStorageData(text) != 0 && WindowRuntime_States(session)[WindowId_Bank].window == nullptr) {
            created = cg::yobi::CreateBankWindowOpeningAnimation_4ABEF0(type == 33 ? 1 : 0, request, target, 1, 0);
        }
        if (created == nullptr && WindowRuntime_States(session)[WindowId_Bank].window == nullptr) {
            world::gCurrentDialogType_581800 = -1;
        }
        return;
    case 24:
        static_cast<void>(WindowPetSkillChoice_OpenPetSkills(text));
        return;
    case 25:
        WindowPetSkillChoice_ReceivePetCandidates(text);
        return;
    case 26:
    case 44:
        WindowPetSkillChoice_ReceiveSelectedPetSkills(text);
        return;
    case 27:
        static_cast<void>(WindowShopChoice_OpenExchangeChoice(text));
        return;
    case 28:
        static_cast<void>(WindowShop_OpenExchange(session, text));
        return;
    case 29:
        static_cast<void>(WindowInventory_OpenSpecialSelection(session, text));
        return;
    case 30:
        static_cast<void>(WindowShopChoice_OpenGiveReceiveChoice(text));
        return;
    case 31:
        static_cast<void>(WindowShop_OpenItemDetails(session, text));
        return;
    case 32:
        static_cast<void>(WindowTextList_Open(request, target, text));
        return;
    case 34:
        if (WindowBank_ReceiveStorageData(text) != 0 && WindowRuntime_States(session)[WindowId_ItemStorage].window == nullptr) {
            created = cg::yobi::CreateItemStorageWindowOpeningAnimation_4ACE80(0, request, target, 0, 1, 0);
        }
        if (created == nullptr && WindowRuntime_States(session)[WindowId_ItemStorage].window == nullptr) {
            world::gCurrentDialogType_581800 = -1;
        }
        return;
    case 36:
        static_cast<void>(WindowAppearance_Open(session, request, target, text));
        return;
    case 37:
        static_cast<void>(WindowAppearance_OpenDialogPreview(session, value, request, target, text));
        return;
    case 42:
        static_cast<void>(WindowPetSkillChoice_OpenPetSkillsByTribe(text));
        return;
    case 43:
        WindowPetSkillChoice_ReceiveTribeCandidates(text);
        return;
    case 45:
        static_cast<void>(WindowShopChoice_OpenFirstCommandList(text));
        return;
    case 46:
        static_cast<void>(WindowCrystalShop_Open(session, text));
        return;
    case 47:
        static_cast<void>(WindowShopChoice_OpenSecondCommandList(text));
        return;
    case 48:
        static_cast<void>(WindowCrystalExchangeShop_Open(session, text));
        return;
    default:
        return;
    }
}

// 原函数：0x0050E130。
static void WindowDialog_RouteReply(
    cg::network::CgConnection& connection, const std::int32_t type, const std::int32_t value,
    const std::int32_t request, const std::int32_t target, char* const text)
{
    if (value == -1) {
        if (type != world::gCurrentDialogType_581800) {
            return;
        }
        if (target != world::gDialogTargetD2DE6C) {
            return;
        }
        WindowRuntime_CloseByCategory(connection.Owner(), 3);
        world::gCurrentDialogType_581800 = -1;
        return;
    }
    WindowDialog_Route(connection.Owner(), type, value, request, target, text);
    cg::yobi::YobiAction* const action = world::FindObjectYobi_4462B0(target);
    world::FaceWorldActorTowardPlayer_43D860(action);
}

// 原函数：0x0049DF30。
static cg::yobi::YobiAction* WindowDialog_OpenText(
    session::ClientSession& session, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t buttons,
    const std::int32_t request, const std::int32_t target, char* text)
{
    auto& dialog = session.UiFrame().native.dialog;
    WindowDialog_Reset(session);
    dialog.reply.type = world::gCurrentDialogType_581800;
    dialog.reply.request = request;
    dialog.reply.target = target;
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    const std::int32_t buttonHeight = buttons != 0 ? 24 : 0;
    const std::int32_t type = dialog.reply.type;
    std::int32_t inputHeight = 0;
    if (type == 1 || type == 11) {
        inputHeight = 28;
        dialog.inputY[0] = static_cast<std::int16_t>(height - buttonHeight - 44);
    }
    if (world::gCurrentDialogType_581800 == 35) {
        const std::int32_t contentHeight = height - inputHeight - buttonHeight;
        dialog.inputY[0] = static_cast<std::int16_t>(contentHeight - 118);
        dialog.inputY[1] = static_cast<std::int16_t>(contentHeight - 67);
    }
    std::int32_t lineHeight = 16;
    dialog.reply.firstLine = 0;
    if (type == 2) {
        lineHeight = 20;
        dialog.reply.firstLine = network::ParseDelimitedInteger_52CFA0(text, '\n', 1);
        text = const_cast<char*>(std::strchr(text, '\n')) + 1;
    }
    const std::int32_t columns = cg::yobi::CalculateDialogTextColumns_49C5C0(static_cast<std::int16_t>(width));
    const std::int32_t lines = cg::yobi::CalculateDialogVisibleLines_49C5E0(
        static_cast<std::int16_t>(height - inputHeight), buttons, lineHeight);
    char* marker = input::FindLegacyByte_44B4A0(text, "@");
    while (marker != nullptr) {
        char* const content = marker + 1;
        if (content[0] == '@') {
            content[1] = static_cast<char>(0xFF);
        } else if (content[0] == 'g' && content[1] == ',') {
            char* const end = input::FindLegacyByte_44B4A0(content, "@");
            if (end == nullptr) {
                break;
            }
            *end = '\0';
            WindowDialog_ParseGraphicTag(session, content,
                static_cast<std::int16_t>(static_cast<std::int16_t>(width) / 2),
                static_cast<std::int16_t>(static_cast<std::int16_t>(height) / 2));
            std::memset(marker, 0xFF, static_cast<std::size_t>(end - marker + 1));
        }
        marker = input::FindLegacyByte_44B4A0(content, "@");
    }
    char* output = text;
    for (char* cursor = text; ; ++cursor) {
        if (static_cast<std::uint8_t>(*cursor) == 0xFFu) {
            continue;
        }
        *output = *cursor;
        if (*cursor == '\0') {
            break;
        }
        ++output;
    }
    for (std::int32_t row = 0; row < lines; ++row) {
        static_cast<void>(input::ExtractDialogTextLine(dialog.lines[row].text, text, row, columns));
    }
    if (world::gCurrentDialogType_581800 == 35) {
        return WindowDialog_Open(
            session, x, y, width, height, imageNo, buttons, dialog.lines.data(),
            lines, lineHeight, dialog.reply.firstLine, 2, dialog.inputY.data(),
            columns, 2, 2, &WindowDialog_Reply, nullptr, 1, 0);
    }
    return WindowDialog_Open(
        session, x, y, width, height, imageNo, buttons, dialog.lines.data(),
        lines, lineHeight, dialog.reply.firstLine, inputHeight != 0 ? 1 : 0,
        inputHeight != 0 ? dialog.inputY.data() : nullptr,
        columns, 2, 0, &WindowDialog_Reply, nullptr, 1, 0);
}

// 原函数：0x0049E1D0。
static std::int32_t WindowDialog_DrawHealing(
    session::ClientSession& session, WindowInstance* const window, const std::int32_t button,
    std::int32_t row, const std::int32_t)
{
    auto& dialog = session.UiFrame().native.dialog;
    static_cast<void>(cg::yobi::CalculateDialogTextColumns_49C5C0(static_cast<std::int16_t>(window->width)));
    const std::int32_t visibleLines = cg::yobi::CalculateDialogVisibleLines_49C5E0(
        static_cast<std::int16_t>(window->height), 1, 16);
    if (button != 0) {
        gHoverTextD68AE8 = L"不治疗了。";
    }
    std::wstring buffer;
    for (std::int32_t index = 0; index < 7; ++index) {
        if (dialog.healing[index].level != 0) {
            cg::text::FormatTo(buffer, L"{:7} G", dialog.healing[index].cost);
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + 336),
                static_cast<std::int16_t>(window->y + index * 16 + 52),
                3, 0, 0, buffer, 0, static_cast<std::int8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
            if (index < 6) {
                const std::int32_t imageIndex = WindowItem_HealthImageLevel(dialog.healing[index].health);
                static_cast<void>(cg::render::DrawGraphic(
                    window->x + 68, window->y + index * 16 + 60,
                    223, gHealthImages57CBA4[imageIndex], 0, nullptr));
            }
        }
    }
    cg::text::FormatTo(buffer, L"               所持金 {:7} 金币", world::gPlayerRuntimeSlots.valueFC);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(window->x + 24),
        static_cast<std::int16_t>(window->y + 7 * 16 + 68),
        3, 0, 0, buffer, 0, static_cast<std::int8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    if (row >= visibleLines) {
        row = -1;
    }
    // row=-1时原前缀读取无论为何值都仍得到-1，只消去这次无结果读取。
    if (row != -1 && dialog.lines[row].text.empty()) {
        row = -1;
    }
    const std::int32_t first = dialog.reply.firstLine;
    if (row >= first && row <= 8) {
        const wchar_t* text;
        if (row == 8) {
            text = dialog.health != 0
                ? L"请求治疗所有受伤的人物。"
                : L"没有受伤的人物。";
        } else {
            text = dialog.healing[row - first].health != 0
                ? L"请求治疗此人物的伤势。"
                : L"此人物没有受伤。";
        }
        gHoverTextD68AE8 = text;
    }
    return 0;
}

// 原函数：0x0049E400。
static cg::yobi::YobiAction* WindowDialog_OpenHealing(
    session::ClientSession& session, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t,
    const std::int32_t request, const std::int32_t target, const char* const text)
{
    auto& dialog = session.UiFrame().native.dialog;
    WindowDialog_Reset(session);
    dialog.reply.type = world::gCurrentDialogType_581800;
    dialog.reply.request = request;
    dialog.reply.target = target;
    for (auto& record : dialog.healing) {
        record.level = 0;
    }
    std::int32_t totalCost = 0;
    std::int32_t field = 1;
    dialog.health = 0;
    std::wstring buffer;
    char name[256];
    for (std::int32_t index = 0; index < 6; ++index) {
        auto& record = dialog.healing[index];
        record.level = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        if (record.level < 0) {
            record.level = 0;
            break;
        }
        record.health = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name));
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        record.name = cg::text::FromGbkWide(std::strlen(name) > 16 ? "???" : name);
        const std::int32_t cost = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t totalHealth = dialog.health;
        record.cost = cost;
        totalCost += cost;
        dialog.health = totalHealth + record.health;
    }
    dialog.healing[6].level = 1;
    dialog.healing[6].cost = totalCost;
    buffer = L"         请选择治疗对象";
    dialog.lines[0].text = buffer;
    buffer = L"   Health   LV        Name             Gold";
    dialog.lines[1].text = buffer;
    for (std::int32_t index = 0; index < 6; ++index) {
        const auto& record = dialog.healing[index];
        if (record.level != 0) {
            buffer = std::format(L"           {:3}  {:<16}", record.level, record.name);
            dialog.lines[index + 2].text = buffer;
        } else {
            dialog.lines[index + 2].text.clear();
        }
    }
    buffer = L"           完整治疗";
    dialog.lines[8].text = buffer;
    buffer = L""; // 原String1@D68A7C虚拟零空格式，不借用593850。
    dialog.lines[9].text = buffer;
    dialog.lines[10].text = L"";
    dialog.reply.firstLine = 2;
    const std::int32_t columns = cg::yobi::CalculateDialogTextColumns_49C5C0(static_cast<std::int16_t>(width));
    const std::int32_t first = dialog.reply.firstLine;
    return WindowDialog_Open(
        session, x, y, width, height, imageNo, 2, dialog.lines.data(), first + 9,
        16, first, 0, nullptr, columns, 2, 0, &WindowDialog_Reply, &WindowDialog_DrawHealing, 1, 0);
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E110。
void WindowDialog_ReceiveReply(cg::network::CgConnection& networkConnection, const std::int32_t type, const std::int32_t value,
    const std::int32_t request, const std::int32_t target, char* const text)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    if (world::gLogoutToTownPending_D67E78 != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    detail::WindowDialog_RouteReply(networkConnection, type, value, request, target, text);
}

} // namespace cg::ui
