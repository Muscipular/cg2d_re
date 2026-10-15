#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::array<std::array<char, 17>, 10> gWindowListNamesCF3F20 {}; // 原全局：0x00CF3F20。

std::int32_t gWindowListCountCF46A8 = 0; // 原全局：0x00CF46A8。

WindowListSelectionCallback gWindowListCallbackCB1E14 = nullptr; // 原全局：0x00CB1E14。

std::int32_t gWindowListOffsetCC3EF4 = 0; // 原全局：0x00CC3EF4。

static_assert(sizeof(gWindowListNamesCF3F20) == 170);

// 原函数：0x0049E800。
login::YobiAction* WindowTradeTarget_ReceiveTargets(
    const char* const text, const WindowListSelectionCallback callback)
{
    gWindowListCountCF46A8 = 0;
    for (auto& name : gWindowListNamesCF3F20) {
        name[0] = '\0';
    }
    std::array<char, 768> buffer;
    std::int32_t count = 0;
    for (std::int32_t processed = 0; processed < 10 && count < 10; ++processed) {
        if (network::ExtractDelimitedField_52CED0(
                text, '|', processed + 1, 255, buffer.data()) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        count = gWindowListCountCF46A8;
        char* const destination = gWindowListNamesCF3F20[count].data();
        const std::size_t length = std::strlen(buffer.data()) + 1;
        const std::size_t prefix = length & ~std::size_t{3};
        std::memcpy(destination, buffer.data(), prefix);
        gWindowListCountCF46A8 = ++count;
        std::memcpy(destination + prefix, buffer.data() + prefix, length - prefix);
    }
    gWindowListCallbackCB1E14 = callback;
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    return WindowRuntime_CreateAnimation(WindowId_TradeTarget, 1, 0, nullptr);
}

// 原函数：0x0049E990。
void WindowTradeTarget_Refresh(const std::int32_t offset)
{
    for (std::int32_t line = 0; line < 5; ++line) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[9 + line].data);
        data->text = cg::text::FromGbkWide(gWindowListNamesCF3F20[offset + line].data());
    }
}

// 原函数：0x0049E8F0。
std::int32_t WindowTradeTarget_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t visible = gWindowListCountCF46A8 > 5 ? 1 : 0;
        gWindowListOffsetCC3EF4 = 0;
        for (std::int32_t index = 0; index < 6; ++index) {
            gCurrentWindow->elements[index].visible = visible;
        }
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
    }
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x0049EAA0。
std::int32_t WindowTradeTarget_Draw(const std::int32_t)
{
    gCurrentWindow->elements[0].visible = 1;
    if (gWindowListCountCF46A8 - 5 <= 0) {
        gCurrentWindow->elements[0].visible = 0;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    const auto* const mouse = static_cast<const WindowMouseData*>(elements[1].data);
    if ((mouse->flags & 1u) != 0) {
        gWindowListOffsetCC3EF4 = WindowScroll_ReadPosition(elements + 1, gWindowListCountCF46A8 - 5);
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
    }
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(
        0x3BE34, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049EB50。
std::int32_t WindowTradeTarget_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowListOffsetCC3EF4 > 0) {
            --gWindowListOffsetCC3EF4;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, position));
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B592;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593;
    }
    return result;
}

// 原函数：0x0049EC00。
std::int32_t WindowTradeTarget_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowListOffsetCC3EF4 < gWindowListCountCF46A8 - 5) {
            ++gWindowListOffsetCC3EF4;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 2, maximum, position));
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B595;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B597;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B596;
    }
    return result;
}

// 原函数：0x0049ECC0。
std::int32_t WindowTradeTarget_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gWindowListOffsetCC3EF4 > 0 ? 51 : 55, 320));
        gWindowListOffsetCC3EF4 -= 5;
        if (gWindowListOffsetCC3EF4 < 0) {
            gWindowListOffsetCC3EF4 = 0;
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 3, maximum, position));
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return result;
}

// 原函数：0x0049ED80。
std::int32_t WindowTradeTarget_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t oldMaximum = gWindowListCountCF46A8 - 5;
        const bool canMove = oldMaximum > 0 && gWindowListOffsetCC3EF4 < oldMaximum;
        static_cast<void>(audio::PlaySe_52C450(canMove ? 51 : 55, 320));
        std::int32_t position = gWindowListOffsetCC3EF4 + 5;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        gWindowListOffsetCC3EF4 = position;
        if (position > maximum) {
            position = maximum;
            gWindowListOffsetCC3EF4 = position;
        }
        if (position < 0) {
            position = 0;
            gWindowListOffsetCC3EF4 = position;
        }
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 4, maximum, position));
        WindowTradeTarget_Refresh(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return result;
}

// 原函数：0x0049EE60。
std::int32_t WindowTradeTarget_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gWindowListCountCF46A8;
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t position = gWindowListOffsetCC3EF4;
        gWindowListOffsetCC3EF4 =
            WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, position, count - 5, wheel);
    }
    return 0;
}

// 原函数：0x0049EEB0。
std::int32_t WindowTradeTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"中止。";
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        result = 1;
    }
    graphic->imageNo = 0x3B562;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B564;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B563;
    }
    return result;
}

// 原函数：0x0049EF60。
std::int32_t WindowTradeTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    if (index - 14 >= gWindowListCountCF46A8) {
        return 0;
    }
    if ((state & 1) != 0) {
        const std::uint32_t imageNo = login::detail::gValueD69768;
        WindowElement_DrawHoverFrame(gCurrentWindow->elements + index, state, imageNo);
    }
    if ((state & 2) == 0) {
        return 0;
    }
    const WindowListSelectionCallback callback = gWindowListCallbackCB1E14;
    if (callback != nullptr) {
        const std::int32_t selected = gWindowListOffsetCC3EF4 + index - 14;
        callback(selected, gWindowListNamesCF3F20[selected].data());
    }
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

} // namespace detail

} // namespace cg::ui
