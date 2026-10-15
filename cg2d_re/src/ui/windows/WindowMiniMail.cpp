#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextCA0180 {}; // 原全局：0x00CA0180。
} // namespace cg::input

namespace cg::ui {
namespace detail {
namespace {

WindowInputPrototype gMiniMailInputPrototype580CB0 { // 原全局：0x00580CB0。
    {{&input::gInputContextCA0180}, WindowInputReferenceKind::Context},
    22, 70, 3, 12, 0, {}, 10, 28, 12, 0,
};
} // namespace


// 原函数：0x004CB5D0。
void WindowMiniMail_Refresh()
{
    static_cast<WindowTextData*>(gCurrentWindow->elements[12].data)->text = cg::text::FromGbkWide(gMiniMailCompose105F508.name.data());
    WindowInstance* const window = gCurrentWindow;
    static_cast<void>(WindowInput_ActivateElement(&gMiniMailInputPrototype580CB0, window->x, window->y, 2));
    std::int32_t divisor = 1;
    for (std::int32_t index = 0; index < 8; ++index) {
        const std::int32_t cost = world::detail::ReadCategoryValue105FA20();
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index + 3].data)->imageNo =
            cost < divisor ? 0 : cost / divisor % 10 + 245520;
        divisor *= 10;
    }
    if (world::detail::ReadCategoryValue105FA20() == 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[3].data)->imageNo = 245520;
    }
}

// 原函数：0x004CB550。
std::int32_t WindowMiniMail_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t mode =
            gMiniMailCompose105F508.target == world::detail::gMiniMailSelectionD673E4 ? 1 : 0;
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(WindowInput_ActivateElement(&gMiniMailInputPrototype580CB0, window->x, window->y, mode));
        input::SetContext_44B330(&input::gInputContextCA0180);
        gFocusedElementDataD45F94 = gCurrentWindow->elements[1].data;
        static_cast<WindowInputData*>(gCurrentWindow->elements[1].data)->context =
            &input::gInputContextCA0180;
        const std::int32_t target = gMiniMailCompose105F508.target;
        gMiniMailInputPrototype580CB0.text.clear();
        world::detail::gMiniMailSelectionD673E4 = target;
    }
    WindowMiniMail_Refresh();
    return 1;
}

// 原函数：0x004CB920。
std::int32_t WindowMiniMail_HandleInput(const std::int32_t, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (gFocusedElementDataD45F94 != window->elements[1].data) {
            static_cast<void>(WindowInput_ActivateElement(
                &gMiniMailInputPrototype580CB0, window->x, window->y, 1));
            input::SetContext_44B330(&input::gInputContextCA0180);
            gFocusedElementDataD45F94 = gCurrentWindow->elements[1].data;
            static_cast<WindowInputData*>(gCurrentWindow->elements[1].data)->context =
                &input::gInputContextCA0180;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (input::gCurrentInputData != &input::gInputContextCA0180) {
        render::DrawInputText_4568A0(&input::gInputContextCA0180);
    }
    return consumed;
}

// 原函数：0x004CB6A0。
std::int32_t WindowMiniMail_Send(const std::int32_t index, const std::int32_t state)
{
    if (index != 2) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 244253;
        std::wstring hint;
        cg::text::FormatTo(hint, kMiniMailCostHint588588, world::detail::ReadCategoryValue105FA20());
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = 244251;
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    if (input::gInputContextCA0180.empty()) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return consumed;
    }
    const std::wstring text = input::ContextText(input::gInputContextCA0180);
    std::array<char, 300> storedText; // 原邮件0x140字节记录的+0x14正文，含NUL。
    cg::text::StoreGbk(storedText.data(), storedText.size(), text);
    const std::wstring escapedName = input::EscapeText(cg::text::FromGbkWide(gMiniMailCompose105F508.name.data()), 256);
    const std::wstring escapedText = input::EscapeText(text, 1024);
    const std::wstring combined = std::to_wstring(gMiniMailCompose105F508.target) + L"|" + escapedName + L"|" + escapedText + L"|0";
    const std::wstring packet = input::EscapeTextAndNotify(combined, 1023);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, 8, packet));
    const std::int32_t contact = world::detail::Sub_534370(gMiniMailCompose105F508.target);
    if (contact != -1) {
        std::int32_t time;
        platform::ReadLegacyTime32_53BA26(&time);
        const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&time);
        const auto* const bytes = reinterpret_cast<const std::byte*>(calendar);
        std::int32_t minute;
        std::memcpy(&minute, bytes + 4, sizeof(minute));
        std::int32_t hour;
        std::memcpy(&hour, bytes + 8, sizeof(hour));
        std::int32_t day;
        std::memcpy(&day, bytes + 12, sizeof(day));
        std::int32_t month;
        std::memcpy(&month, bytes + 16, sizeof(month));
        std::int32_t year;
        std::memcpy(&year, bytes + 20, sizeof(year));
        char timestamp[64];
        std::sprintf(timestamp, "%4d/%2d/%2d %02d:%02d",
            year + 1900, month + 1, day, hour, minute);
        world::detail::Sub_5343C0(contact, 1, timestamp, storedText.data());
    }
    static_cast<void>(WindowProfileView_RequestCategoryClose(3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    input::gInputContextCA0180.clearText();
    world::detail::gMiniMailSelectionD673E4 = -1;
    return 1;
}

} // namespace detail

} // namespace cg::ui
