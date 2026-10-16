#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049E990。
static void WindowTradeTarget_Refresh(std::int32_t offset); // 逐项复制五行选择文本，重读当前窗口载荷。

// 原函数：0x0049E8F0。
static std::int32_t WindowTradeTarget_Update(std::int32_t value); // 窗口44初始化与移动关闭。

// 原函数：0x0049EAA0。
static std::int32_t WindowTradeTarget_Draw(std::int32_t unused); // 窗口44滚动消费、完整绘制和九宫格。

// 原函数：0x0049EB50。
static std::int32_t WindowTradeTarget_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行。

// 原函数：0x0049EC00。
static std::int32_t WindowTradeTarget_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行。

// 原函数：0x0049ECC0。
static std::int32_t WindowTradeTarget_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上五行。

// 原函数：0x0049ED80。
static std::int32_t WindowTradeTarget_ButtonNextPage(std::int32_t index, std::int32_t state); // 下五行。

// 原函数：0x0049EE60。
static std::int32_t WindowTradeTarget_Scroll(std::int32_t index, std::int32_t state); // 滚轮像素余量与偏移。

// 原函数：0x0049EEB0。
static std::int32_t WindowTradeTarget_ButtonCancel(std::int32_t index, std::int32_t state); // 取消提示/关闭及按钮三态。

// 原函数：0x0049EF60。
static std::int32_t WindowTradeTarget_SelectRow(std::int32_t index, std::int32_t state); // 实际选择回调，随后重读窗口关闭。

// 控件载荷、窗口状态、组件表与原型。

static std::array<std::array<char, 17>, 10> gWindowListNamesCF3F20 {}; // 原全局：0x00CF3F20。

static std::int32_t gWindowListCountCF46A8 = 0; // 原全局：0x00CF46A8。

// 原全局：0x00CB1E14。 49EF60借用名单文本并弃返回。
static WindowListSelectionCallback gWindowListCallbackCB1E14 = nullptr; // 原全局：0x00CB1E14。

static std::int32_t gWindowListOffsetCC3EF4 = 0; // 原全局：0x00CC3EF4。

static_assert(sizeof(gWindowListNamesCF3F20) == 170);

static WindowGraphicData gWindowGraphic56CCF0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CCF0。

static WindowGraphicData gWindowGraphic56CD08 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD08。

static WindowGraphicData gWindowGraphic56CD20 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD20。

static WindowGraphicData gWindowGraphic56CD38 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD38。

static WindowGraphicData gWindowGraphic56CD50 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD50。

static WindowGraphicData gWindowGraphic56CD68 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD68。

static WindowTextPrototype gWindowTexts56CD80[5] { // 原全局：0x0056CD80。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
};

static WindowTextPrototype gWindowText56CDA8 { // 原全局：0x0056CDA8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"请选择对象。", // 原常量参考：0x00585F54。
};

static WindowElementPrototype gWindowElements56CDB0[19] { // 原全局：0x0056CDB0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56CCF0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56CCF0.imageNo)}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56CD08}, &WindowTradeTarget_ButtonPreviousRow},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56CD20}, &WindowTradeTarget_ButtonNextRow},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56CD50}, &WindowTradeTarget_ButtonPreviousPage},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56CD68}, &WindowTradeTarget_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTradeTarget_Scroll},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56CD38}, &WindowTradeTarget_ButtonCancel},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56CDA8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowTexts56CD80[0]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowTexts56CD80[1]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowTexts56CD80[2]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowTexts56CD80[3]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowTexts56CD80[4]}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
};

// 原型：窗口44；原全局：0x0054E8D0。
WindowPrototype gWindowPrototypeTradeTarget {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56CDB0, &WindowTradeTarget_Update, &WindowTradeTarget_Draw, &WindowRuntime_ReleaseDefault}; // 44: 54E8D0

// 窗口函数实现。

// 原函数：0x0049E800。
cg::yobi::YobiAction* WindowTradeTarget_ReceiveTargets(
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
        world::detail::gPlayerY.value();
    const std::int32_t x =
        world::detail::gPlayerX.value();
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    return WindowRuntime_CreateAnimation(WindowId_TradeTarget, 1, 0, nullptr);
}

// 原函数：0x0049E990。
static void WindowTradeTarget_Refresh(const std::int32_t offset)
{
    for (std::int32_t line = 0; line < 5; ++line) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[9 + line].data);
        data->text = cg::text::FromGbkWide(gWindowListNamesCF3F20[offset + line].data());
    }
}

// 原函数：0x0049E8F0。
static std::int32_t WindowTradeTarget_Update(const std::int32_t value)
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
static std::int32_t WindowTradeTarget_Draw(const std::int32_t)
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
    cg::yobi::detail::DrawNineGrid_49BE00(
        0x3BE34, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049EB50。
static std::int32_t WindowTradeTarget_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowTradeTarget_ButtonNextRow(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowTradeTarget_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowTradeTarget_ButtonNextPage(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowTradeTarget_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gWindowListCountCF46A8;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = gWindowListOffsetCC3EF4;
        gWindowListOffsetCC3EF4 =
            WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, position, count - 5, wheel);
    }
    return 0;
}

// 原函数：0x0049EEB0。
static std::int32_t WindowTradeTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowTradeTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    if (index - 14 >= gWindowListCountCF46A8) {
        return 0;
    }
    if ((state & 1) != 0) {
        const std::uint32_t imageNo = cg::render::gValueD69768;
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
