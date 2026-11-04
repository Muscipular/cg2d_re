#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowAppearanceElementId : ElementId {
    ButtonOk = 1,
    Element2 = 2,
    Element3 = 3,
    CycleClass = 4,
    Element5 = 5,
    CycleClass_6 = 6,
    CycleFifthDigit = 7,
    Element8 = 8,
    CycleFifthDigit_9 = 9,
    CycleOnesDigit = 10,
    Element11 = 11,
    CycleOnesDigit_12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C5C30。
static std::int32_t WindowAppearance_ButtonOk(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 图号原文本直接发送，不转义。

// 原函数：0x004C5D00。
static std::int32_t WindowAppearance_CycleClass(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // class按版本上下界循环。

// 原函数：0x004C5DE0。
static std::int32_t WindowAppearance_CycleFifthDigit(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // fifth分量0..4循环。

// 原函数：0x004C5EB0。
static std::int32_t WindowAppearance_CycleOnesDigit(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // ones分量0..4循环。

// 原函数：0x004C5F80。
static std::int32_t WindowAppearance_SendDialogReply(
    session::ClientSession& session, std::int32_t button, std::int32_t row, const std::wstring* unusedTexts,
    std::uint8_t unusedInputCount); // 类型37回复，图号转义后发完整button。

// 原函数：0x004C5850。
static std::int32_t WindowAppearance_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004C5B70。
static std::int32_t WindowAppearance_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004C5C00。
static void WindowAppearance_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic576EF0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576EF0。

static WindowGraphicData gWindowGraphic576F08 {30000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F08。

static WindowGraphicData gWindowGraphic576F20 {245042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F20。

static WindowGraphicData gWindowGraphic576F38 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F38。

static WindowGraphicData gWindowGraphic576F50 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F50。

static WindowTextPrototype gWindowText576F68 {{}, L"请选择脸部、眼睛及嘴巴。"}; // 原全局：0x00576F68。 // 原常量参考：0x00585D98。

static WindowTextPrototype gWindowText576F70 {{}, L"脸部"}; // 原全局：0x00576F70。 // 原常量参考：0x00585D90。

static WindowTextPrototype gWindowText576F78 {{}, L"眼睛"}; // 原全局：0x00576F78。 // 原常量参考：0x00585D88。

static WindowTextPrototype gWindowText576F80 {{}, L"嘴巴"}; // 原全局：0x00576F80。 // 原常量参考：0x00585D80。

static std::int32_t gWindowAnimationImage576F88 = 100000; // 原全局：0x00576F88。

// 原全局：0x00576F90。
// 原576F90的十五项，三个分量按钮复用源图形但实例各自分配。
static WindowElementPrototype gWindowElements576F90[15] {
    {WindowElementType_Graphic, 197, 206, 66, 17, 1, {.graphic = &gWindowGraphic576EF0}, &WindowAppearance_ButtonOk, -1, static_cast<ElementId>(WindowAppearanceElementId::ButtonOk)},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element2)},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element3)},
    {WindowElementType_Graphic, 144, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleClass, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleClass)},
    {WindowElementType_Text, 194, 28, 0, 0, 1, {.text = &gWindowText576F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element5)},
    {WindowElementType_Graphic, 260, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleClass, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleClass_6)},
    {WindowElementType_Graphic, 144, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleFifthDigit, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleFifthDigit)},
    {WindowElementType_Text, 194, 49, 0, 0, 1, {.text = &gWindowText576F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element8)},
    {WindowElementType_Graphic, 260, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleFifthDigit, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleFifthDigit_9)},
    {WindowElementType_Graphic, 144, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleOnesDigit, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleOnesDigit)},
    {WindowElementType_Text, 194, 70, 0, 0, 1, {.text = &gWindowText576F80}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element11)},
    {WindowElementType_Graphic, 260, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleOnesDigit, -1, static_cast<ElementId>(WindowAppearanceElementId::CycleOnesDigit_12)},
    {WindowElementType_Animation, 390, 120, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576F88}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element13)},
    {WindowElementType_Text, 20, 147, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element14)},
    {WindowElementType_Text, 20, 163, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAppearanceElementId::Element15)},
};

// 原型：窗口56；原全局：0x0054ED20。
WindowPrototype gWindowPrototypeAppearance {0, 4, 96, 108, 448, 240, 0x80010101u, 15, gWindowElements576F90, &WindowAppearance_Update, &WindowAppearance_Draw, &WindowAppearance_Release}; // 56: 54ED20

// 窗口函数实现。

// 原函数：0x004C5790。
cg::yobi::YobiAction* WindowAppearance_Open(
    session::ClientSession& session, const std::int32_t request, const std::int32_t target, const char* const text)
{
    auto& appearance = session.UiFrame().native.appearance;
    auto& shop = session.UiFrame().native.shop;
    appearance.dialogType = 36;
    appearance.request = request;
    appearance.target = target;
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char name[52];
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 2, 16, name));
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    appearance.texts.name = cg::text::FromGbkWide(name);
    std::array<char, 3072> buffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 1023, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 2; ++row) {
        char message[46];
        static_cast<void>(input::ExtractLegacyLine_503AB0(
            message, 46, buffer.data(), row, 45));
        appearance.texts.messages[row] = cg::text::FromGbkWide(message);
    }
    return WindowRuntime_CreateAnimation(session, WindowId_Appearance, 1, 0, nullptr);
}

// 原函数：0x004C5850。
static std::int32_t WindowAppearance_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value != -1) {
        return 1;
    }
    std::int32_t version = callbackWindow.ownerSession->Connection().GraphicVersionOverride();
    if (version == 0xFF) {
        version = callbackWindow.ownerSession->Connection().PukVersion();
    }
    const std::int32_t converted = character_select::detail::ConvertEarlierCharacterGraphicToModern_51BFF0(
        character_select::detail::ConvertClassicCharacterGraphicToCompact_51BB80(callbackWindow.ownerSession->WorldScene().player->value17C));
    appearance.minimum = 0;
    if (converted >= 241400) {
        appearance.minimum = 1;
    }
    appearance.maximum = 0;
    if (version >= 5) {
        appearance.maximum = 1;
    }
    if (version >= 7) {
        appearance.maximum = 2;
    }
    if (converted >= 205000 && converted <= 205600) {
        appearance.minimum = 0;
        appearance.maximum = 0;
    }
    if (converted >= 270000 && converted <= 270200) {
        appearance.minimum = 0;
        appearance.maximum = 0;
    }
    if (auto* const element14 = currentWindow->FindElementById(WindowAppearanceElementId::Element14); element14) element14->data.text->text = appearance.texts.messages[0];
    if (auto* const element15 = currentWindow->FindElementById(WindowAppearanceElementId::Element15); element15) element15->data.text->text = appearance.texts.messages[1];
    for (std::int32_t index = 3; index <= 5; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
    }
    if (appearance.maximum - appearance.minimum > 0) {
        for (std::int32_t index = 3; index <= 5; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        }
    }
    for (std::int32_t index = 6; index <= 11; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
    }
    // 原门禁读取上次分类；本次玩家图号分解在可见性处理之后。
    const std::int32_t previousBase = appearance.base;
    if (previousBase == 33100 || previousBase == 33101) {
        for (std::int32_t index = 3; index <= 11; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
    }
    static_cast<void>(character_select::detail::DecomposeCharacterAppearanceGraphic_4C56E0(
        callbackWindow.ownerSession->WorldScene().player->value17C, &appearance.base, &appearance.imageClass,
        &appearance.quarter, &appearance.fifth, &appearance.ones));
    const std::int32_t ones = appearance.ones;
    const std::int32_t fifth = appearance.fifth;
    const std::int32_t quarter = appearance.quarter;
    const std::int32_t imageClass = appearance.imageClass;
    const std::int32_t base = appearance.base;
    const std::int32_t image = character_select::detail::ComposeCharacterAppearanceGraphic_4C5650(
        base, imageClass, quarter, fifth, ones);
    if (auto* const element3 = currentWindow->FindElementById(WindowAppearanceElementId::Element3); element3) element3->data.graphic->imageNo = image;
    if (auto* const element13 = currentWindow->FindElementById(WindowAppearanceElementId::Element13)) {
        auto* const animation = element13->data.animation;
        cg::yobi::YobiAction* const action = animation->action;
        cg::yobi::SetYobiActionField2A8(action, callbackWindow.ownerSession->WorldScene().player->imageNo);
        cg::yobi::SetYobiActionField2B0(animation->action, 0);
        cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
    }
    return 1;
}

// 原函数：0x004C5B70。
static std::int32_t WindowAppearance_Draw(WindowInstance& callbackWindow, const std::int32_t unused)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unused);
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 2, 2) != 0 || npcDialog.type != appearance.dialogType) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    WindowInstance* const window = currentWindow;
    cg::yobi::DrawNineGrid_49BE00(
        245300, static_cast<std::int16_t>(window->x), static_cast<std::int16_t>(window->y),
        static_cast<std::int16_t>(window->width), static_cast<std::int16_t>(window->height),
        223, -1, std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004C5C00。
static void WindowAppearance_Release(WindowInstance& releasedWindow)
{
    auto& npcDialog = releasedWindow.ownerSession->WorldScene().npcDialog;
    auto& appearance = releasedWindow.ownerSession->UiFrame().native.appearance;
    if (npcDialog.type == appearance.dialogType) {
        npcDialog.type = -1;
        appearance.dialogType = -1;
    }
}

// 原函数：0x004C5C30。
static std::int32_t WindowAppearance_ButtonOk(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t ones = appearance.ones;
            const std::int32_t fifth = appearance.fifth;
            const std::int32_t quarter = appearance.quarter;
            const std::int32_t imageClass = appearance.imageClass;
            const std::int32_t base = appearance.base;
            const std::int32_t image = character_select::detail::ComposeCharacterAppearanceGraphic_4C5650(
                base, imageClass, quarter, fifth, ones);
            const auto buffer = std::to_wstring(image);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerY);
            const std::int32_t x =
                static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 1, buffer));
            currentWindow->state |= 0x8000u;
            consumed = 1;
        }
        graphic->imageNo = 243048;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243050;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243049;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C5D00。
static std::int32_t WindowAppearance_CycleClass(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        std::int32_t imageClass = appearance.imageClass + (elementId < static_cast<ElementId>(WindowAppearanceElementId::Element5) ? -1 : 1);
        const std::int32_t minimum = appearance.minimum;
        appearance.imageClass = imageClass;
        if (imageClass < minimum) {
            imageClass = appearance.maximum;
            appearance.imageClass = imageClass;
        } else if (imageClass > appearance.maximum) {
            imageClass = minimum;
            appearance.imageClass = imageClass;
        }
        const std::int32_t ones = appearance.ones;
        const std::int32_t fifth = appearance.fifth;
        const std::int32_t quarter = appearance.quarter;
        const std::int32_t base = appearance.base;
        const std::int32_t image = character_select::detail::ComposeCharacterAppearanceGraphic_4C5650(
            base, imageClass, quarter, fifth, ones);
        if (auto* const element3 = currentWindow->FindElementById(WindowAppearanceElementId::Element3); element3) element3->data.graphic->imageNo = image;
        consumed = 1;
    }
    if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element5) ? 243030 : 243033;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element5) ? 243032 : 243035;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element5) ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5DE0。
static std::int32_t WindowAppearance_CycleFifthDigit(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        std::int32_t fifth = appearance.fifth + (elementId < static_cast<ElementId>(WindowAppearanceElementId::Element8) ? -1 : 1);
        appearance.fifth = fifth;
        if (fifth < 0) {
            fifth = 4;
            appearance.fifth = fifth;
        } else if (fifth >= 5) {
            fifth = 0;
            appearance.fifth = fifth;
        }
        const std::int32_t ones = appearance.ones;
        const std::int32_t quarter = appearance.quarter;
        const std::int32_t base = appearance.base;
        const std::int32_t imageClass = appearance.imageClass;
        const std::int32_t image = character_select::detail::ComposeCharacterAppearanceGraphic_4C5650(
            base, imageClass, quarter, fifth, ones);
        if (auto* const element3 = currentWindow->FindElementById(WindowAppearanceElementId::Element3); element3) element3->data.graphic->imageNo = image;
        consumed = 1;
    }
    if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element8) ? 243030 : 243033;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element8) ? 243032 : 243035;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element8) ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5EB0。
static std::int32_t WindowAppearance_CycleOnesDigit(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& appearance = callbackWindow.ownerSession->UiFrame().native.appearance;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        std::int32_t ones = appearance.ones + (elementId < static_cast<ElementId>(WindowAppearanceElementId::Element11) ? -1 : 1);
        appearance.ones = ones;
        if (ones < 0) {
            ones = 4;
            appearance.ones = ones;
        } else if (ones >= 5) {
            ones = 0;
            appearance.ones = ones;
        }
        const std::int32_t quarter = appearance.quarter;
        const std::int32_t imageClass = appearance.imageClass;
        const std::int32_t fifth = appearance.fifth;
        const std::int32_t base = appearance.base;
        const std::int32_t image = character_select::detail::ComposeCharacterAppearanceGraphic_4C5650(
            base, imageClass, quarter, fifth, ones);
        if (auto* const element3 = currentWindow->FindElementById(WindowAppearanceElementId::Element3); element3) element3->data.graphic->imageNo = image;
        consumed = 1;
    }
    if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element11) ? 243030 : 243033;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element11) ? 243032 : 243035;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = elementId < static_cast<ElementId>(WindowAppearanceElementId::Element11) ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5F80。
static std::int32_t WindowAppearance_SendDialogReply(
    session::ClientSession& session, const std::int32_t button, const std::int32_t row, const std::wstring* const unusedTexts,
    const std::uint8_t unusedInputCount)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& imageDialog = session.UiFrame().native.imageDialog;
    static_cast<void>(unusedTexts);
    static_cast<void>(unusedInputCount);
    if (button == 0) {
        if (row < 0) {
            npcDialog.type = -1;
        }
        return 1;
    }
    std::wstring text;
    std::wstring escaped;
    cg::text::FormatTo(text, L"{}", imageDialog.imageNo);
    escaped = input::EscapeTextAndNotify(text, 255);
    const std::int32_t target = imageDialog.target;
    const std::int32_t request = imageDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    static_cast<void>(session.Connection().SendDialogResponse(x, y, request, target, button, escaped));
    npcDialog.type = -1;
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// 原函数：0x004C6060。
cg::yobi::YobiAction* WindowAppearance_OpenDialogPreview(
    session::ClientSession& session, const std::int32_t buttons, const std::int32_t request, const std::int32_t target,
    const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& imageDialog = session.UiFrame().native.imageDialog;
    WindowDialog_Reset(session);
    imageDialog.type = npcDialog.type;
    imageDialog.request = request;
    imageDialog.target = target;
    imageDialog.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    std::array<char, 3000> buffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 1000, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    const std::int32_t columns = cg::yobi::CalculateDialogTextColumns_49C5C0(448);
    const std::int32_t count = cg::yobi::CalculateDialogVisibleLines_49C5E0(240, buttons, 16);
    imageDialog.lines[0].text.clear();
    imageDialog.lines[1].text.clear();
    imageDialog.lines[2].text.clear();
    for (std::int32_t row = 3; row < count; ++row) {
        std::wstring& destination = imageDialog.lines[row].text;
        std::wstring suffix;
        static_cast<void>(input::ExtractGbkTextLine(suffix, buffer.data(), row - 3, columns - 14));
        destination.assign(14, L' ');
        destination.append(suffix);
    }
    WindowDialog_AddImage(session, 1, imageDialog.imageNo, 0, 86, 98);
    return WindowDialog_Open(
        session, 96, 108, 448, 240, 245300, buttons, imageDialog.lines.data(),
        count, 16, 0, 0, nullptr, columns, 2, 0, &WindowAppearance_SendDialogReply, nullptr, 1, 0);
}

} // namespace detail

} // namespace cg::ui
