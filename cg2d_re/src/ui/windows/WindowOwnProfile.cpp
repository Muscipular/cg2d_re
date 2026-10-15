#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

// 控件载荷、窗口状态、组件表与原型。

InputContext gInputContextD2D828 {}; // 原全局：0x00D2D828。

InputContext gInputContextD64CB0 {}; // 原全局：0x00D64CB0。

InputContext gInputContextD2D670 {}; // 原全局：0x00D2D670。

InputContext gInputContextCAFEC8 {}; // 原全局：0x00CAFEC8。

} // namespace cg::input

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004792D0。
static std::int32_t WindowOwnProfile_EditField(std::int32_t index, std::int32_t state); // 窗口58元素5..8编辑焦点。

// 原函数：0x004744F0。
static void WindowOwnProfile_Release();

// 原函数：0x00478320。
static std::int32_t WindowOwnProfile_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x0057CEE8。 窗口58三项/正文的可写输入原型。
static WindowInputPrototype gOwnProfileInputPrototype57CEE8 { // 原全局：0x0057CEE8。
    {{&input::gInputContextD64CB0}, WindowInputReferenceKind::Context},
    97, 108, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D118 { // 原全局：0x0057D118。
    {{&input::gInputContextD2D670}, WindowInputReferenceKind::Context},
    97, 130, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D348 { // 原全局：0x0057D348。
    {{&input::gInputContextCAFEC8}, WindowInputReferenceKind::Context},
    97, 152, 3, 12, 6, {}, 1, 26, 0, 0,
};

static WindowInputPrototype gOwnProfileInputPrototype57D578 { // 原全局：0x0057D578。
    {{&input::gInputContextD2D828}, WindowInputReferenceKind::Context},
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
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText561518 { // 原全局：0x00561518。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText561520 { // 原全局：0x00561520。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x005860D0。
};

static WindowTextPrototype gWindowText561528 { // 原全局：0x00561528。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"職業", // 原常量参考：0x0058621C。
};

static WindowTextPrototype gWindowText561530 { // 原全局：0x00561530。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"公會", // 原常量参考：0x005860C8。
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
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText561510}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText561518}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText561520}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText561528}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText561530}, &WindowElement_NoOp},
    {WindowElementType_Input, 90, 105, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 129, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 153, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 177, 168, 54, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText561540}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText561548}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5613F0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 64, 72, 1, {.graphic = &gWindowGraphic5614F8}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5613F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic561408}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic561420}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic561438}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic561450}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 266, 105, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 266, 128, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 266, 151, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5614C8}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic561468}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561480}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561498}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口58；原全局：0x0054E3F0。
WindowPrototype gWindowPrototypeOwnProfile {0, 4, 15, 97, 315, 261, 0x80000080u, 34, gWindowElements561558, &WindowOwnProfile_Update, &WindowRuntime_DrawDefault, &WindowOwnProfile_Release}; // 58: 54E3F0

// 窗口函数实现。

// 原478A10三项正文共享分支；第一项在求长度后才取data，另两项在此前取。
static void WindowOwnProfile_RefreshEntry(
    const std::int32_t index, input::InputContext* const context,
    WindowInputPrototype* const prototype)
{
    auto& current = world::detail::BorrowCurrentProfile10738E8().entries410[index];
    const std::int32_t element = index + 9;
    const bool editing = input::gCurrentInputData == context;
    WindowElement* const elements = gCurrentWindow->elements;
    if (editing) {
        elements[element].visible = 0;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(prototype, x, y, 1));
        cg::text::StoreGbk(current.text08.data(), current.text08.size(), input::ContextText(*prototype->input.context));
        return;
    }

    WindowTextData* data;
    std::size_t length;
    if (index == 0) {
        length = std::strlen(current.text08.data()) + 1;
        data = static_cast<WindowTextData*>(elements[element].data);
    } else {
        data = static_cast<WindowTextData*>(elements[element].data);
        length = std::strlen(current.text08.data()) + 1;
    }
    data->text = cg::text::FromGbkWide(current.text08.data());
    gCurrentWindow->elements[element].visible = 1;
    const auto& saved = world::detail::ReadSelectedProfile1074938();
    std::byte color {6};
    if (std::strcmp(saved.entries410[index].text08.data(), current.text08.data()) == 0) {
        const std::int32_t currentValue = current.value00;
        const std::int32_t savedValue = saved.entries410[index].value00;
        if (savedValue == currentValue) {
            color = saved.value04 != 0 && currentValue != 0 ? std::byte{0} : std::byte{7};
        }
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[element].data)->bytes00[0] = color;
}

// 原478A10完整刷新：保持每个阶段的角色采样和窗口重取，不缓存跨输入调用的data。
// 原函数：0x00478A10。
static void WindowOwnProfile_Refresh()
{
    auto& current = world::detail::BorrowCurrentProfile10738E8();
    WindowElement* const portraitElements = gCurrentWindow->elements;
    const std::int32_t portrait = world::detail::ReadPlayerValue17C();
    static_cast<WindowGraphicData*>(portraitElements[18].data)->imageNo = portrait;
    const std::int32_t publicFlag = world::detail::SelectedProfileValue04();
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[17].data)->imageNo =
        publicFlag != 0 ? 0 : 0x3C113;

    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
    const char* const name = world::detail::BorrowPlayerDisplayText(1);
    nameData->text = cg::text::FromGbkWide(name);
    std::wstring number;
    cg::text::FormatTo(number, L"{:3}", world::detail::ReadPlayerLevel());
    auto* const levelData = static_cast<WindowTextData*>(gCurrentWindow->elements[1].data);
    levelData->text = number;

    WindowElement* const titleElements = gCurrentWindow->elements;
    const char* const title = world::detail::BorrowPlayerDisplayText(2);
    auto* const titleData = static_cast<WindowTextData*>(titleElements[2].data);
    titleData->text = cg::text::FromGbkWide(title);
    WindowElement* const jobElements = gCurrentWindow->elements;
    const char* const job = world::detail::BorrowPlayerJobName();
    auto* const jobData = static_cast<WindowTextData*>(jobElements[3].data);
    jobData->text = cg::text::FromGbkWide(job);

    WindowInstance* const guildWindow = gCurrentWindow;
    const std::int32_t guildIndex = login::detail::ReadGuildTextIndex10514B0();
    auto* const guildData = static_cast<WindowTextData*>(guildWindow->elements[4].data);
    const char* const guild = guildIndex == -1
        ? input::LegacyText593850() : login::detail::BorrowGuildName10514BC();
    guildData->text = cg::text::FromGbkWide(guild);

    for (std::int32_t index = 0; index < 3; ++index) {
        WindowElement* const elements = gCurrentWindow->elements;
        const std::int32_t image = current.entries410[index].value04;
        static_cast<WindowGraphicData*>(elements[index + 26].data)->imageNo = image;
    }
    WindowOwnProfile_RefreshEntry(0, &input::gInputContextD64CB0, &gOwnProfileInputPrototype57CEE8);
    WindowOwnProfile_RefreshEntry(1, &input::gInputContextD2D670, &gOwnProfileInputPrototype57D118);
    WindowOwnProfile_RefreshEntry(2, &input::gInputContextCAFEC8, &gOwnProfileInputPrototype57D348);

    if (input::gCurrentInputData == &input::gInputContextD2D828) {
        for (std::int32_t index = 12; index < 16; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gOwnProfileInputPrototype57D578, x, y, 1));
        cg::text::StoreGbk(current.text728.data(), current.text728.size(), input::ContextText(*gOwnProfileInputPrototype57D578.input.context));
    } else {
        std::wstring line;
        for (std::int32_t index = 0; index < 4; ++index) {
            static_cast<void>(input::ExtractGbkTextLine(
                line, current.text728.data(), index, 28));
            WindowElement* const elements = gCurrentWindow->elements;
            auto* const data = static_cast<WindowTextData*>(elements[index + 12].data);
            data->text = line;
        }
        for (std::int32_t index = 12; index < 16; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
        const auto& saved = world::detail::ReadSelectedProfile1074938();
        std::byte color {6};
        if (std::strcmp(saved.text728.data(), current.text728.data()) == 0) {
            color = saved.value04 != 0 ? std::byte{0} : std::byte{7};
        }
        for (std::int32_t index = 12; index < 16; ++index) {
            static_cast<WindowTextData*>(gCurrentWindow->elements[index].data)->bytes00[0] = color;
        }
    }
    const std::int32_t changed = world::detail::Sub_533B00();
    gCurrentWindow->elements[29].visible = changed != 0 ? 1 : 0;
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
static std::int32_t WindowOwnProfile_EditField(const std::int32_t index, const std::int32_t state)
{
    WindowInputPrototype* prototype;
    input::InputContext* context;
    switch (index) {
    case 5:
        prototype = &gOwnProfileInputPrototype57CEE8;
        context = &input::gInputContextD64CB0;
        break;
    case 6:
        prototype = &gOwnProfileInputPrototype57D118;
        context = &input::gInputContextD2D670;
        break;
    case 7:
        prototype = &gOwnProfileInputPrototype57D348;
        context = &input::gInputContextCAFEC8;
        break;
    case 8:
        prototype = &gOwnProfileInputPrototype57D578;
        context = &input::gInputContextD2D828;
        break;
    default:
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"請輸入描述";
    }
    if ((state & 2) == 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 != window->elements[index].data) {
        const auto& current = world::detail::BorrowCurrentProfile10738E8();
        const char* const source =
            index == 8 ? current.text728.data() : current.entries410[index - 5].text08.data();
        prototype->text = cg::text::FromGbkWide(source);
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(prototype, x, y, 0));
        input::SetContext_44B330(context);
        WindowInstance* const focused = gCurrentWindow;
        gFocusedElementDataD45F94 = focused->elements[index].data;
        static_cast<WindowInputData*>(focused->elements[index].data)->context = context;
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
