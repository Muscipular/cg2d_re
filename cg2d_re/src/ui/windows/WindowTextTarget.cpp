#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

// 窗口45借用调用者的两张表；待安装回调与当前回调是两个独立原槽。
const char* const* gTextChoiceNamesD30130 = nullptr; // 借用原GBK名单，窗口正文在49F0A0转为UTF-16。 // 原全局：0x00D30130。

const std::uint8_t* gTextChoiceColorsCB1E18 = nullptr; // 原全局：0x00CB1E18。

std::int32_t gTextChoiceCountCC4CE4 = 0; // 原全局：0x00CC4CE4。

WindowTextChoiceCallback gTextChoicePendingC9A974 = nullptr; // 原全局：0x00C9A974。

WindowTextChoiceCallback gTextChoiceCallbackCA0AC0 = nullptr; // 原全局：0x00CA0AC0。

std::int32_t gTextChoiceOffsetD2D9C0 = 0; // 原全局：0x00D2D9C0。

std::uint8_t gTextChoiceEnabledCA0008 = 0; // 原全局：0x00CA0008。
} // namespace


// 原函数：0x0049F000。
login::YobiAction* WindowTextTarget_Open(
    const char* const* const names, const std::uint8_t* const colors,
    const std::int32_t count, const WindowTextChoiceCallback callback)
{
    gTextChoiceNamesD30130 = names;
    gTextChoiceColorsCB1E18 = colors;
    gTextChoiceCountCC4CE4 = count;
    gTextChoicePendingC9A974 = callback;
    return WindowRuntime_CreateAnimation(WindowId_TextTarget, 1, 0, nullptr);
}

// 原函数：0x0049F040。
void WindowTextTarget_Release()
{
    const WindowTextChoiceCallback callback = gTextChoiceCallbackCA0AC0;
    if (callback != nullptr) {
        static_cast<void>(callback(-1, nullptr));
    }
}

// 原函数：0x0049F060。
std::int32_t WindowTextTarget_Update(const std::int32_t value)
{
    if (value == -1) {
        gTextChoiceOffsetD2D9C0 = 0;
        WindowTextTarget_Refresh(0);
        const WindowTextChoiceCallback callback = gTextChoicePendingC9A974;
        gTextChoiceEnabledCA0008 = 1;
        gTextChoiceCallbackCA0AC0 = callback;
    }
    return 1;
}

// 原函数：0x0049F0A0。
void WindowTextTarget_Refresh(const std::int32_t offset)
{
    const char* const* names = gTextChoiceNamesD30130;
    std::int32_t row = 0;
    for (; row < 5 && offset + row < gTextChoiceCountCC4CE4; ++row) {
        if (names != nullptr) {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data);
            text->text = cg::text::FromGbkWide(names[offset + row]);
            const std::uint8_t color = gTextChoiceColorsCB1E18[offset + row];
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data)->bytes00[0] =
                static_cast<std::byte>(color);
            names = gTextChoiceNamesD30130;
        }
    }
    for (; row < 5; ++row) {
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    }
}

// 原函数：0x0049F1A0。
std::int32_t WindowTextTarget_Draw(const std::int32_t)
{
    const std::int32_t visible = gTextChoiceCountCC4CE4 - 5 > 0 ? 1 : 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        gCurrentWindow->elements[index].visible = visible;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    if ((static_cast<const WindowMouseData*>(elements[1].data)->flags & 1u) != 0) {
        gTextChoiceOffsetD2D9C0 = WindowScroll_ReadPosition(elements + 1, gTextChoiceCountCC4CE4 - 5);
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
    }
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(
        245300, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049F260。
std::int32_t WindowTextTarget_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gTextChoiceOffsetD2D9C0 > 0) {
            --gTextChoiceOffsetD2D9C0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t offset = gTextChoiceOffsetD2D9C0;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gTextChoiceCountCC4CE4 - 5;
        static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, offset));
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
        consumed = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x0049F310。
std::int32_t WindowTextTarget_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gTextChoiceOffsetD2D9C0 < gTextChoiceCountCC4CE4 - 5) {
            ++gTextChoiceOffsetD2D9C0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t offset = gTextChoiceOffsetD2D9C0;
        const std::int32_t maximum = gTextChoiceCountCC4CE4 - 5;
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 2, maximum, offset));
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
        consumed = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x0049F3D0。
std::int32_t WindowTextTarget_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gTextChoiceOffsetD2D9C0 > 0 ? 51 : 55, 320));
        gTextChoiceOffsetD2D9C0 -= 5;
        if (gTextChoiceOffsetD2D9C0 < 0) {
            gTextChoiceOffsetD2D9C0 = 0;
        }
        const std::int32_t offset = gTextChoiceOffsetD2D9C0;
        const std::int32_t maximum = gTextChoiceCountCC4CE4 - 5;
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 3, maximum, offset));
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
        consumed = 1;
    }
    graphic->imageNo = 243030;
    if ((state & 1) != 0) {
        graphic->imageNo = 243032;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243031;
    }
    return consumed;
}

// 原函数：0x0049F490。
std::int32_t WindowTextTarget_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t oldMaximum = gTextChoiceCountCC4CE4 - 5;
        static_cast<void>(audio::PlaySe_52C450(
            oldMaximum > 0 && gTextChoiceOffsetD2D9C0 < oldMaximum ? 51 : 55, 320));
        std::int32_t offset = gTextChoiceOffsetD2D9C0 + 5;
        const std::int32_t maximum = gTextChoiceCountCC4CE4 - 5;
        gTextChoiceOffsetD2D9C0 = offset;
        if (offset > maximum) {
            offset = maximum;
            gTextChoiceOffsetD2D9C0 = offset;
        }
        if (offset < 0) {
            offset = 0;
            gTextChoiceOffsetD2D9C0 = offset;
        }
        static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 4, maximum, offset));
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
        consumed = 1;
    }
    graphic->imageNo = 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243034;
    }
    return consumed;
}

// 原函数：0x0049F570。
std::int32_t WindowTextTarget_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gTextChoiceCountCC4CE4;
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t offset = gTextChoiceOffsetD2D9C0;
        gTextChoiceOffsetD2D9C0 =
            WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, offset, count - 5, wheel);
    }
    return 0;
}

// 原函数：0x0049F5C0。
std::int32_t WindowTextTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"關閉此視窗";
    }
    if ((state & 2) != 0) {
        if (gTextChoiceEnabledCA0008 != 0) {
            const WindowTextChoiceCallback callback = gTextChoiceCallbackCA0AC0;
            if (callback != nullptr) {
                if (callback(-2, nullptr) != 0) {
                    gCurrentWindow->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                gTextChoiceEnabledCA0008 = 0;
            }
        }
        consumed = 1;
    }
    graphic->imageNo = 243042;
    if ((state & 1) != 0) {
        graphic->imageNo = 243044;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243043;
    }
    return consumed;
}

// 原函数：0x0049F680。
std::int32_t WindowTextTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t selected = gTextChoiceOffsetD2D9C0 + index - 14;
    // 原门槛只比较行号，不将滚动偏移并入检查。
    if (index - 14 >= gTextChoiceCountCC4CE4) {
        return 0;
    }
    if ((state & 1) != 0) {
        WindowElement_DrawHoverFrame(gCurrentWindow->elements + index, state, login::detail::gValueD69768);
        gHoverTextD68AE8 = L"選擇此人物。";
    }
    if ((state & 2) != 0) {
        if (gTextChoiceEnabledCA0008 != 0) {
            const WindowTextChoiceCallback callback = gTextChoiceCallbackCA0AC0;
            if (callback != nullptr) {
                if (callback(selected, gTextChoiceNamesD30130[selected]) != 0) {
                    gCurrentWindow->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                gTextChoiceEnabledCA0008 = 0;
            }
        }
        return 1;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic56D010 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D010。

WindowGraphicData gWindowGraphic56D028 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D028。

WindowGraphicData gWindowGraphic56D040 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D040。

WindowGraphicData gWindowGraphic56D058 {243042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D058。

WindowGraphicData gWindowGraphic56D070 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D070。

WindowGraphicData gWindowGraphic56D088 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D088。

WindowTextPrototype gWindowText56D0A0 { // 原全局：0x0056D0A0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};

WindowTextPrototype gWindowText56D0A8 { // 原全局：0x0056D0A8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"請選擇對象。", // 原常量参考：0x00585F54。
};

} // namespace

// 原型：窗口45；原全局：0x0054E900。
WindowPrototype gWindowPrototypeTextTarget {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56D0B0, &WindowTextTarget_Update, &WindowTextTarget_Draw, &WindowTextTarget_Release}; // 45: 54E900

WindowElementPrototype gWindowElements56D0B0[19] { // 原全局：0x0056D0B0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56D010}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56D010.imageNo)}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56D028}, &WindowTextTarget_ButtonPreviousRow},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56D040}, &WindowTextTarget_ButtonNextRow},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56D070}, &WindowTextTarget_ButtonPreviousPage},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56D088}, &WindowTextTarget_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTextTarget_Scroll},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56D058}, &WindowTextTarget_ButtonCancel},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56D0A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
};

} // namespace cg::ui::detail
