#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextCB0610 {}; // 原全局：0x00CB0610。
} // namespace cg::input

namespace cg::ui {
namespace detail {
namespace {

WindowInputPrototype gProfileSearchInputPrototype580A80 { // 原全局：0x00580A80。
    {{&input::gInputContextCB0610}, WindowInputReferenceKind::Context},
    20, 370, 3, 12, 6, {}, 1, 26, 0, 0,
};
} // namespace


// 原函数：0x004CA500。
void WindowProfileSearch_RefreshResults()
{
    const auto& search = world::detail::ReadProfileSearch105C398();
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto& record = search.records[index];
        if (record.used != 0) {
            static_cast<WindowTextData*>(gCurrentWindow->elements[index + 8].data)->text = cg::text::FromGbkWide(record.text10C.data());
            static_cast<WindowTextData*>(gCurrentWindow->elements[index + 28].data)->text = cg::text::FromGbkWide(record.text04.data());
        } else {
            gCurrentWindow->elements[index + 8].visible = 0;
            gCurrentWindow->elements[index + 28].visible = 0;
            gCurrentWindow->elements[index + 48].visible = 0;
        }
    }
    const std::int32_t group = search.value00;
    switch (group) {
    case 0:
    case 1:
    case 2: {
        const char* const title = world::detail::ReadCategoryText105FA24(group, search.value04);
        static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = cg::text::FromGbkWide(title);
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo =
            group == 0 ? 246045 : (group == 1 ? 0 : 246046);
        break;
    }
    default:
        break;
    }
}

// 原函数：0x004CA450。
std::uint32_t WindowProfileSearch_DrawKeyword()
{
    if (input::gCurrentInputData == &input::gInputContextCB0610) {
        gCurrentWindow->elements[6].visible = 0;
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(WindowInput_ActivateElement(
            &gProfileSearchInputPrototype580A80, window->x, window->y, 1));
        const std::wstring& text = input::ContextText(*gProfileSearchInputPrototype580A80.input.context);
        const auto length = static_cast<std::uint32_t>(text.size() + 1);
        gProfileSearchKeyword1075988 = text;
        return length;
    }
    gCurrentWindow->elements[6].visible = 1;
    std::wstring& text = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data)->text;
    const auto length = static_cast<std::uint32_t>(
        gProfileSearchKeyword1075988.size() + 1);
    text = gProfileSearchKeyword1075988;
    return length;
}

// 原函数：0x004CA410。
// 关联原入口：0x004CA438（共享尾已并入本函数正文）。
std::int32_t WindowProfileSearch_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowProfileSearch_RefreshResults();
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowProfileSearch_DrawKeyword());
    return 1;
}

// 原函数：0x004CAB60。
void WindowProfileSearch_Release()
{
    world::detail::gValue581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
}

// 原函数：0x004CAB90。
std::int32_t WindowProfileSearch_EditKeyword(const std::int32_t, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"請輸入關鍵字";
        consumed = 1;
    }
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (gFocusedElementDataD45F94 != window->elements[7].data) {
            gProfileSearchInputPrototype580A80.text = gProfileSearchKeyword1075988;
            static_cast<void>(WindowInput_ActivateElement(
                &gProfileSearchInputPrototype580A80, window->x, window->y, 0));
            input::SetContext_44B330(&input::gInputContextCB0610);
            gFocusedElementDataD45F94 = gCurrentWindow->elements[7].data;
            static_cast<WindowInputData*>(gCurrentWindow->elements[7].data)->context =
                &input::gInputContextCB0610;
        }
        return 1;
    }
    return consumed;
}

// 原函数：0x004CA6F0。
std::int32_t WindowProfileSearch_HandleSearchButtons(const std::int32_t index, const std::int32_t state)
{
    if (index < 3 || index > 5) {
        return 0;
    }
    constexpr std::array<std::int32_t, 3> normalImages {246048, 243030, 243033};
    constexpr std::array<std::int32_t, 3> hoverImages {246047, 243032, 243035};
    constexpr std::array<std::int32_t, 3> commands {1, 16, 32};
    constexpr const wchar_t* hints[] {
        L"使用關鍵字檢索",
        L"上一頁",
        L"下一頁",
    };
    const std::int32_t position = index - 3;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = hoverImages[position];
        gHoverTextD68AE8 = hints[position];
        consumed = 1;
    } else {
        graphic->imageNo = normalImages[position];
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    const std::wstring keyword = input::EscapeText(gProfileSearchKeyword1075988, 256);
    const auto& search = world::detail::ReadProfileSearch105C398();
    const std::int32_t entry = search.value04;
    const std::int32_t group = search.value00;
    const std::wstring combined = std::to_wstring(group) + L"|" + std::to_wstring(entry) + L"|" + keyword;
    const std::wstring packet = input::EscapeTextAndNotify(combined, 255);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, commands[position], packet));
    return 1;
}

// 原函数：0x004CA9C0。
std::int32_t WindowProfileSearch_SelectResult(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t position = index - 48;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"瀏覽此名玩家的資料表";
        consumed = 1;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 246059;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 246060;
    }
    if ((state & 2) != 0) {
        const auto& record = world::detail::ReadProfileSearch105C398().records[position];
        const std::int32_t key = record.value104;
        const std::wstring combined = std::to_wstring(key) + L"|" + cg::text::FromGbkWide(record.text04.data());
        const std::wstring packet = input::EscapeTextAndNotify(combined, 255);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 4, packet));
        consumed = 1;
    }
    if (gWindowStates[WindowId_ProfileView].window != nullptr) {
        const std::int32_t key = world::detail::ReadViewedProfile105ECE0().value00;
        if (key == world::detail::ReadProfileSearch105C398().records[position].value104) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 246058;
        }
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
