#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049F040。
static void WindowTextTarget_Release();

// 原函数：0x0049F060。
static std::int32_t WindowTextTarget_Update(std::int32_t value);

// 原函数：0x0049F0A0。
static void WindowTextTarget_Refresh(std::int32_t offset); // 刷新五行，未用行清空正文但保留颜色。

// 原函数：0x0049F1A0。
static std::int32_t WindowTextTarget_Draw(std::int32_t value);

// 原函数：0x0049F260。
static std::int32_t WindowTextTarget_ButtonPreviousRow(std::int32_t index, std::int32_t state);

// 原函数：0x0049F310。
static std::int32_t WindowTextTarget_ButtonNextRow(std::int32_t index, std::int32_t state);

// 原函数：0x0049F3D0。
static std::int32_t WindowTextTarget_ButtonPreviousPage(std::int32_t index, std::int32_t state);

// 原函数：0x0049F490。
static std::int32_t WindowTextTarget_ButtonNextPage(std::int32_t index, std::int32_t state);

// 原函数：0x0049F570。
static std::int32_t WindowTextTarget_Scroll(std::int32_t index, std::int32_t state);

// 原函数：0x0049F5C0。
static std::int32_t WindowTextTarget_ButtonCancel(std::int32_t index, std::int32_t state);

// 原函数：0x0049F680。
static std::int32_t WindowTextTarget_SelectRow(std::int32_t index, std::int32_t state);

// 控件载荷、窗口状态、组件表与原型。

// 窗口45借用调用者的两张表；待安装回调与当前回调是两个独立原槽。
static const wchar_t* const* gTextChoiceNamesD30130 = nullptr; // 借用宽文本名单，窗口正文直接读取。 // 原全局：0x00D30130。

static const std::uint8_t* gTextChoiceColorsCB1E18 = nullptr; // 原全局：0x00CB1E18。

static std::int32_t gTextChoiceCountCC4CE4 = 0; // 原全局：0x00CC4CE4。

static WindowTextChoiceCallback gTextChoicePendingC9A974 = nullptr; // 原全局：0x00C9A974。

static WindowTextChoiceCallback gTextChoiceCallbackCA0AC0 = nullptr; // 原全局：0x00CA0AC0。

static std::int32_t gTextChoiceOffsetD2D9C0 = 0; // 原全局：0x00D2D9C0。

static std::uint8_t gTextChoiceEnabledCA0008 = 0; // 原全局：0x00CA0008。

static WindowGraphicData gWindowGraphic56D010 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D010。

static WindowGraphicData gWindowGraphic56D028 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D028。

static WindowGraphicData gWindowGraphic56D040 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D040。

static WindowGraphicData gWindowGraphic56D058 {243042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D058。

static WindowGraphicData gWindowGraphic56D070 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D070。

static WindowGraphicData gWindowGraphic56D088 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D088。

static WindowTextPrototype gWindowText56D0A0 { // 原全局：0x0056D0A0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};

static WindowTextPrototype gWindowText56D0A8 { // 原全局：0x0056D0A8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"请选择对象。", // 原常量参考：0x00585F54。
};

static WindowElementPrototype gWindowElements56D0B0[19] { // 原全局：0x0056D0B0。
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

// 原型：窗口45；原全局：0x0054E900。
WindowPrototype gWindowPrototypeTextTarget {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56D0B0, &WindowTextTarget_Update, &WindowTextTarget_Draw, &WindowTextTarget_Release}; // 45: 54E900

// 窗口函数实现。

// 原函数：0x0049F000。
cg::yobi::YobiAction* WindowTextTarget_Open(
    const wchar_t* const* const names, const std::uint8_t* const colors,
    const std::int32_t count, const WindowTextChoiceCallback callback)
{
    gTextChoiceNamesD30130 = names;
    gTextChoiceColorsCB1E18 = colors;
    gTextChoiceCountCC4CE4 = count;
    gTextChoicePendingC9A974 = callback;
    return WindowRuntime_CreateAnimation(WindowId_TextTarget, 1, 0, nullptr);
}

// 原函数：0x0049F040。
static void WindowTextTarget_Release()
{
    const WindowTextChoiceCallback callback = gTextChoiceCallbackCA0AC0;
    if (callback != nullptr) {
        static_cast<void>(callback(-1, nullptr));
    }
}

// 原函数：0x0049F060。
static std::int32_t WindowTextTarget_Update(const std::int32_t value)
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
static void WindowTextTarget_Refresh(const std::int32_t offset)
{
    const wchar_t* const* names = gTextChoiceNamesD30130;
    std::int32_t row = 0;
    for (; row < 5 && offset + row < gTextChoiceCountCC4CE4; ++row) {
        if (names != nullptr) {
            auto* const text = gCurrentWindow->elements[row + 9].data.text;
            text->text = names[offset + row];
            const std::uint8_t color = gTextChoiceColorsCB1E18[offset + row];
            gCurrentWindow->elements[row + 9].data.text->style.color =
                static_cast<std::byte>(color);
            names = gTextChoiceNamesD30130;
        }
    }
    for (; row < 5; ++row) {
        gCurrentWindow->elements[row + 9].data.text->text = L"";
    }
}

// 原函数：0x0049F1A0。
static std::int32_t WindowTextTarget_Draw(const std::int32_t)
{
    const std::int32_t visible = gTextChoiceCountCC4CE4 - 5 > 0 ? 1 : 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        gCurrentWindow->elements[index].visible = visible;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    if ((elements[1].data.mouse->flags & 1u) != 0) {
        gTextChoiceOffsetD2D9C0 = WindowScroll_ReadPosition(elements + 1, gTextChoiceCountCC4CE4 - 5);
        WindowTextTarget_Refresh(gTextChoiceOffsetD2D9C0);
    }
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(
        245300, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049F260。
static std::int32_t WindowTextTarget_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = gCurrentWindow->elements[index].data.graphic;
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
static std::int32_t WindowTextTarget_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = gCurrentWindow->elements[index].data.graphic;
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
static std::int32_t WindowTextTarget_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = gCurrentWindow->elements[index].data.graphic;
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
static std::int32_t WindowTextTarget_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = gCurrentWindow->elements[index].data.graphic;
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
static std::int32_t WindowTextTarget_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gTextChoiceCountCC4CE4;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t offset = gTextChoiceOffsetD2D9C0;
        gTextChoiceOffsetD2D9C0 =
            WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, offset, count - 5, wheel);
    }
    return 0;
}

// 原函数：0x0049F5C0。
static std::int32_t WindowTextTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = gCurrentWindow->elements[index].data.graphic;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"关闭此视窗";
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
static std::int32_t WindowTextTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t selected = gTextChoiceOffsetD2D9C0 + index - 14;
    // 原门槛只比较行号，不将滚动偏移并入检查。
    if (index - 14 >= gTextChoiceCountCC4CE4) {
        return 0;
    }
    if ((state & 1) != 0) {
        WindowElement_DrawHoverFrame(gCurrentWindow->elements + index, state, cg::render::gHoverFramePaletteIndex_D69768);
        gHoverTextD68AE8 = L"选择此人物。";
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
