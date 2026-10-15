#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

constexpr wchar_t kGuildRankSelectHint5876F4[] = L"選擇此稱號。"; // 原常量参考：0x005876F4。

constexpr wchar_t kGuildRankApplyHint587704[] = L"設定已選擇的稱號。"; // 原常量参考：0x00587704。
} // namespace


// 原函数：0x004A4180。
std::int32_t WindowGuildRank_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.mailRankScrollCA0474 = 0;
    }
    return 1;
}

// 原函数：0x004A41A0。
std::int32_t WindowGuildRank_Draw(const std::int32_t)
{
    static_cast<void>(WindowGuildRank_Refresh());
    WindowRuntime_DrawCurrent();
    gLoginWindowRuntime.mailRankScrollCA0474 = WindowScroll_ReadPosition(&gCurrentWindow->elements[14], 21);
    WindowInstance* const window = gCurrentWindow;
    const std::int16_t height = static_cast<std::int16_t>(window->height);
    const std::int16_t width = static_cast<std::int16_t>(window->width);
    const std::int16_t y = static_cast<std::int16_t>(window->y);
    const std::int16_t x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(0x3BE3E, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004A4220。
std::int32_t WindowGuildRank_SelectRow(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kGuildRankSelectHint5876F4;
        consumed = 1;
    }
    const std::int32_t scroll = gLoginWindowRuntime.mailRankScrollCA0474;
    if ((state & 2) != 0) {
        gLoginWindowRuntime.mailRankCF4A14 = scroll + index - 19;
        consumed = 1;
    }
    const std::int32_t selected = gLoginWindowRuntime.mailRankCF4A14;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = selected == scroll + index - 19 ? 0x3B59E : 0x3B59C;
    return consumed;
}

// 原函数：0x004A42C0。
std::int32_t WindowGuildRank_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsClose55331C;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B562;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B564;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B563;
    }
    return consumed;
}

// 原函数：0x004A4370。
std::int32_t WindowGuildRank_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B570;
        gHoverTextD68AE8 = kGuildRankApplyHint587704;
        consumed = 1;
    } else {
        graphic->imageNo = 0x3B56E;
    }
    if ((state & 2) != 0) {
        const std::int32_t rank = gLoginWindowRuntime.mailRankCF4A14;
        const std::int32_t key = gLoginWindowRuntime.mailMemberKeyD45F88;
        const std::uintptr_t socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_511350(socket, key, rank + 1));
        WindowInstance* const window = gCurrentWindow;
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return consumed;
}

// 原函数：0x004A4420。
std::int32_t WindowGuildRank_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    if (index != 15 && index != 16) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    const std::int32_t idle = index == 15 ? 0x3B592 : 0x3B595;
    graphic->imageNo = hover ? idle + 2 : idle;
    if ((state & 0x400) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        std::int32_t position;
        if (index == 15) {
            const std::int32_t previous = gLoginWindowRuntime.mailRankScrollCA0474;
            gLoginWindowRuntime.mailRankScrollCA0474 = previous - 1;
            if (previous - 1 < 0) {
                gLoginWindowRuntime.mailRankScrollCA0474 = 0;
            }
            position = gLoginWindowRuntime.mailRankScrollCA0474;
        } else {
            position = gLoginWindowRuntime.mailRankScrollCA0474 + 1;
            gLoginWindowRuntime.mailRankScrollCA0474 = position;
            if (position > 21) {
                position = 21;
                gLoginWindowRuntime.mailRankScrollCA0474 = position;
            }
        }
        WindowScroll_SetPosition(&gCurrentWindow->elements[14], 21, position);
    }
    return hover ? 1 : 0;
}

// 原函数：0x004A4540。
std::uint32_t WindowGuildRank_Refresh()
{
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int32_t scroll = gLoginWindowRuntime.mailRankScrollCA0474;
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[row].data);
        const char* const rank = login::detail::BorrowGuildRankText1051518(scroll + row + 1);
        data->text = cg::text::FromGbkWide(rank);
    }
    const std::int32_t member = gLoginWindowRuntime.mailMemberIndexCB1D18;
    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
    const char* const name = login::detail::gOrderedRecords1051B14[member].text18;
    const std::uint32_t length = static_cast<std::uint32_t>(std::strlen(name) + 1);
    data->text = cg::text::FromGbkWide(name);
    return length;
}

// 原函数：0x004A45E0。
std::int32_t WindowGuildRank_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        if (wheel != 0) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t position = gLoginWindowRuntime.mailRankScrollCA0474;
            gLoginWindowRuntime.mailRankScrollCA0474 = WindowScroll_ApplyWheel(&window->elements[14], position, 21, wheel);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
