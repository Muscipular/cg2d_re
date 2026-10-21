#include "../WindowRuntimeInternal.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowOwnProfileElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    EditField = 6,
    EditField_7 = 7,
    EditField_8 = 8,
    EditField_9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    ButtonClose = 17,
    HandleButtons = 18,
    Element19 = 19,
    HandleButtons_20 = 20,
    HandleButtons_21 = 21,
    HandleButtons_22 = 22,
    Element23 = 23,
    HandleButtons_24 = 24,
    HandleButtons_25 = 25,
    HandleButtons_26 = 26,
    HandleButtons_27 = 27,
    HandleButtons_28 = 28,
    HandleButtons_29 = 29,
    HandleButtons_30 = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    HandleSideArea = 34,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextD2D828 {}; // 原全局：0x00D2D828。

input::InputContext gInputContextD64CB0 {}; // 原全局：0x00D64CB0。

input::InputContext gInputContextD2D670 {}; // 原全局：0x00D2D670。

input::InputContext gInputContextCAFEC8 {}; // 原全局：0x00CAFEC8。


// 回调与内部函数前置声明。

// 原函数：0x004792D0。
static std::int32_t WindowOwnProfile_EditField(ElementId elementId, std::int32_t state); // 窗口58元素5..8编辑焦点。

// 原函数：0x004744F0。
static void WindowOwnProfile_Release();

// 原函数：0x00478320。
static std::int32_t WindowOwnProfile_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x0057CEE8。 窗口58三项/正文的可写输入原型。
static WindowInputPrototype gOwnProfileInputPrototype57CEE8 { // 原全局：0x0057CEE8。
    {{&cg::ui::detail::gInputContextD64CB0}, WindowInputReferenceKind::Context},
    97, 108, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D118 { // 原全局：0x0057D118。
    {{&cg::ui::detail::gInputContextD2D670}, WindowInputReferenceKind::Context},
    97, 130, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D348 { // 原全局：0x0057D348。
    {{&cg::ui::detail::gInputContextCAFEC8}, WindowInputReferenceKind::Context},
    97, 152, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D578 { // 原全局：0x0057D578。
    {{&cg::ui::detail::gInputContextD2D828}, WindowInputReferenceKind::Context},
    97, 170, 3, 12, 6, {}, 4, 28, 14, 0,
};

static WindowGraphicData gWindowGraphic5613F0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005613F0。

static WindowGraphicData gWindowGraphic561408 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561408。

static WindowGraphicData gWindowGraphic561420 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561420。

static WindowGraphicData gWindowGraphic561438 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561438。

static WindowGraphicData gWindowGraphic561450 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561450。

static WindowGraphicData gWindowGraphic561468 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561468。

static WindowGraphicData gWindowGraphic561480 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561480。

static WindowGraphicData gWindowGraphic561498 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00561498。

static WindowGraphicData gWindowGraphic5614B0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614B0。

static WindowGraphicData gWindowGraphic5614C8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614C8。

static WindowGraphicData gWindowGraphic5614E0 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614E0。

static WindowGraphicData gWindowGraphic5614F8 {0x3C113, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614F8。

static WindowTextPrototype gWindowText561510 { // 原全局：0x00561510。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText561518 { // 原全局：0x00561518。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText561520 { // 原全局：0x00561520。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"称号", // 原常量参考：0x005860D0。
};

static WindowTextPrototype gWindowText561528 { // 原全局：0x00561528。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"职业", // 原常量参考：0x0058621C。
};

static WindowTextPrototype gWindowText561530 { // 原全局：0x00561530。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"公会", // 原常量参考：0x005860C8。
};

static WindowTextPrototype gWindowText561540 { // 原全局：0x00561540。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"SELL", // 原常量参考：0x005860B8。
};

static WindowTextPrototype gWindowText561548 { // 原全局：0x00561548。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"BUY", // 原常量参考：0x005860B4。
};

static WindowTextPrototype gWindowText561550 { // 原全局：0x00561550。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"ABOUT", // 原常量参考：0x005860AC。
};

static WindowElementPrototype gWindowElements561558[34] { // 原全局：0x00561558。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText561510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element1)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText561518}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element2)},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText561520}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element3)},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText561528}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element4)},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText561530}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element5)},
    {WindowElementType_Input, 90, 105, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField, -1, static_cast<ElementId>(WindowOwnProfileElementId::EditField)},
    {WindowElementType_Input, 90, 129, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField, -1, static_cast<ElementId>(WindowOwnProfileElementId::EditField_7)},
    {WindowElementType_Input, 90, 153, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField, -1, static_cast<ElementId>(WindowOwnProfileElementId::EditField_8)},
    {WindowElementType_Input, 90, 177, 168, 54, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField, -1, static_cast<ElementId>(WindowOwnProfileElementId::EditField_9)},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText561540}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element10)},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText561548}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element11)},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element12)},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element13)},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element14)},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element15)},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element16)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5613F0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowOwnProfileElementId::ButtonClose)},
    {WindowElementType_Graphic, 15, 29, 64, 72, 1, {.graphic = &gWindowGraphic5614F8}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons)},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5613F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element19)},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic561408}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_20)},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic561420}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_21)},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic561438}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_22)},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic561450}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element23)},
    {WindowElementType_Graphic, 266, 105, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_24)},
    {WindowElementType_Graphic, 266, 128, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_25)},
    {WindowElementType_Graphic, 266, 151, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_26)},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_27)},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_28)},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_29)},
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5614C8}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleButtons_30)},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic561468}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element31)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561480}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element32)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561498}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowOwnProfileElementId::Element33)},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowOwnProfileElementId::HandleSideArea)},
};

// 原型：窗口58；原全局：0x0054E3F0。
WindowPrototype gWindowPrototypeOwnProfile {0, 4, 15, 97, 315, 261, 0x80000080u, 34, gWindowElements561558, &WindowOwnProfile_Update, &WindowRuntime_DrawDefault, &WindowOwnProfile_Release}; // 58: 54E3F0

// 窗口函数实现。

// 原478A10三项正文共享分支；第一项在求长度后才取data，另两项在此前取。
static void WindowOwnProfile_RefreshEntry(
    const std::int32_t index, input::InputContext* const context,
    WindowInputPrototype* const prototype)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    auto& current = world::detail::gCurrentProfile10738E8.entries410[index];
    const std::int32_t element = index + 9;
    const bool editing = input::gCurrentInputData == context;
    auto* elementsWindow = currentWindow;
    if (editing) {
        if (auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(prototype, x, y, 1));
        const auto& text = input::ContextText(*prototype->input.context);
        char fileBytes[256]{};
        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), text);
        current.text08 = text;
        return;
    }

    WindowTextData* data;
    std::size_t length;
    if (index == 0) {
        length = cg::text::ToGbk(current.text08).size() + 1;
        auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(element));
        if (!indexedElement) { return; }
        data = indexedElement->data.text;
    } else {
        auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(element));
        if (!indexedElement) { return; }
        data = indexedElement->data.text;
        length = cg::text::ToGbk(current.text08).size() + 1;
    }
    data->text = current.text08;
    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 1;
    const auto& saved = world::detail::ReadSelectedProfile1074938();
    std::byte color {6};
    if (saved.entries410[index].text08 == current.text08) {
        const std::int32_t currentValue = current.value00;
        const std::int32_t savedValue = saved.entries410[index].value00;
        if (savedValue == currentValue) {
            color = saved.value04 != 0 && currentValue != 0 ? std::byte{0} : std::byte{7};
        }
    }
    if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(element)); indexedElement2) indexedElement2->data.text->style.color = color;
}

// 原478A10完整刷新：保持每个阶段的角色采样和窗口重取，不缓存跨输入调用的data。
// 原函数：0x00478A10。
static void WindowOwnProfile_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    auto& current = world::detail::gCurrentProfile10738E8;
    auto* portraitElementsWindow = currentWindow;
    const std::int32_t portrait = world::gPlayerRuntimeSlots.value17C;
    if (auto* const element19 = portraitElementsWindow->FindElementById(WindowOwnProfileElementId::Element19); element19) element19->data.graphic->imageNo = portrait;
    const std::int32_t publicFlag = world::detail::SelectedProfileValue04();
    auto* const handleButtonsElement = currentWindow->FindElementById(WindowOwnProfileElementId::HandleButtons);
    if (handleButtonsElement) {
        handleButtonsElement->data.graphic->imageNo =
            publicFlag != 0 ? 0 : 0x3C113;
    }

    auto* const element1 = currentWindow->FindElementById(WindowOwnProfileElementId::Element1);
    if (element1) {
        auto* const nameData = element1->data.text;
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        nameData->text = name;
    }
    std::wstring number;
    cg::text::FormatTo(number, L"{:3}", world::gPlayerRuntimeSlots.level);
    auto* const element2 = currentWindow->FindElementById(WindowOwnProfileElementId::Element2);
    if (element2) {
        auto* const levelData = element2->data.text;
        levelData->text = number;
    }

    auto* titleElementsWindow = currentWindow;
    const wchar_t* const title = world::GetPlayerDisplayText(2);
    auto* const element3 = titleElementsWindow->FindElementById(WindowOwnProfileElementId::Element3);
    if (element3) {
        auto* const titleData = element3->data.text;
        titleData->text = title;
    }
    auto* jobElementsWindow = currentWindow;
    const auto& job = world::GetPlayerJobName();
    auto* const element4 = jobElementsWindow->FindElementById(WindowOwnProfileElementId::Element4);
    if (element4) {
        auto* const jobData = element4->data.text;
        jobData->text = job;
    }

    WindowInstance* const guildWindow = currentWindow;
    const std::int32_t guildIndex = cg::world::ReadGuildTextIndex10514B0();
    auto* const element5 = guildWindow->FindElementById(WindowOwnProfileElementId::Element5);
    if (element5) {
        auto* const guildData = element5->data.text;
        const wchar_t* const guild = guildIndex == -1
            ? L"" : cg::world::GetGuildName();
        guildData->text = guild;
    }

    for (std::int32_t index = 0; index < 3; ++index) {
        auto* elementsWindow = currentWindow;
        const std::int32_t image = current.entries410[index].value04;
        if (auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(index + 26)); indexedElement) indexedElement->data.graphic->imageNo = image;
    }
    WindowOwnProfile_RefreshEntry(0, &cg::ui::detail::gInputContextD64CB0, &gOwnProfileInputPrototype57CEE8);
    WindowOwnProfile_RefreshEntry(1, &cg::ui::detail::gInputContextD2D670, &gOwnProfileInputPrototype57D118);
    WindowOwnProfile_RefreshEntry(2, &cg::ui::detail::gInputContextCAFEC8, &gOwnProfileInputPrototype57D348);

    if (input::gCurrentInputData == &cg::ui::detail::gInputContextD2D828) {
        for (std::int32_t index = 12; index < 16; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gOwnProfileInputPrototype57D578, x, y, 1));
        const auto& text = input::ContextText(*gOwnProfileInputPrototype57D578.input.context);
        char fileBytes[256]{};
        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), text);
        current.text728 = text;
    } else {
        std::wstring line;
        for (std::int32_t index = 0; index < 4; ++index) {
            static_cast<void>(input::ExtractTextLine(
                line, current.text728.data(), index, 28));
            auto* elementsWindow = currentWindow;
            auto* const dataElement = elementsWindow->FindElementById(NativeElementKey(index + 12));
            if (dataElement) {
                auto* const data = dataElement->data.text;
                data->text = line;
            }
        }
        for (std::int32_t index = 12; index < 16; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        }
        const auto& saved = world::detail::ReadSelectedProfile1074938();
        std::byte color {6};
        if (saved.text728 == current.text728) {
            color = saved.value04 != 0 ? std::byte{0} : std::byte{7};
        }
        for (std::int32_t index = 12; index < 16; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->data.text->style.color = color;
        }
    }
    const std::int32_t changed = world::detail::HasProfileDraftChanges_533B00();
    if (auto* const handleButtons_30Element = currentWindow->FindElementById(WindowOwnProfileElementId::HandleButtons_30); handleButtons_30Element) handleButtons_30Element->visible = changed != 0 ? 1 : 0;
}

// 原函数：0x00478320。
static std::int32_t WindowOwnProfile_Update(const std::int32_t)
{
    WindowOwnProfile_Refresh();
    return 1;
}

// 原函数：0x004744F0。
static void WindowOwnProfile_Release()
{
    if (gWindowStates[WindowId_PlayerTitles].window != nullptr) {
        WindowInstance* const window = gWindowStates[WindowId_OwnProfile].window;
        gWindowStates[WindowId_PlayerProfile].x = window->x;
        gWindowStates[WindowId_PlayerProfile].y = window->y;
        gWindowStates[WindowId_PlayerAttributes].x = window->x;
        gWindowStates[WindowId_PlayerAttributes].y = window->y;
        gWindowStates[WindowId_PlayerTitles].x = window->x;
        gWindowStates[WindowId_PlayerTitles].y = window->y;
    }
}

// 原函数：0x004792D0。
static std::int32_t WindowOwnProfile_EditField(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInputPrototype* prototype;
    input::InputContext* context;
    switch (static_cast<WindowOwnProfileElementId>(elementId)) {
    case WindowOwnProfileElementId::EditField:
        prototype = &gOwnProfileInputPrototype57CEE8;
        context = &cg::ui::detail::gInputContextD64CB0;
        break;
    case WindowOwnProfileElementId::EditField_7:
        prototype = &gOwnProfileInputPrototype57D118;
        context = &cg::ui::detail::gInputContextD2D670;
        break;
    case WindowOwnProfileElementId::EditField_8:
        prototype = &gOwnProfileInputPrototype57D348;
        context = &cg::ui::detail::gInputContextCAFEC8;
        break;
    case WindowOwnProfileElementId::EditField_9:
        prototype = &gOwnProfileInputPrototype57D578;
        context = &cg::ui::detail::gInputContextD2D828;
        break;
    default:
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"请输入描述";
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    WindowInstance* const window = currentWindow;
    auto* const indexedElement = window->FindElementById(elementId);
    if (indexedElement) {
        if (gFocusedElementDataD45F94 != indexedElement->data.input) {
            const auto& current = world::detail::gCurrentProfile10738E8;
            const wchar_t* const source =
                elementId == static_cast<ElementId>(WindowOwnProfileElementId::EditField_9) ? current.text728.data() : current.entries410[index - 5].text08.data();
            prototype->text = source;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(prototype, x, y, 0));
            input::SetContext_44B330(context);
            WindowInstance* const focused = currentWindow;
            auto* const indexedElement2 = focused->FindElementById(elementId);
            if (indexedElement2) {
                gFocusedElementDataD45F94 = indexedElement2->data.input;
            }
            if (auto* const indexedElement3 = focused->FindElementById(elementId); indexedElement3) indexedElement3->data.input->context = context;
        }
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
