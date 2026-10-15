#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

// 控件载荷、窗口状态、组件表与原型。

InputContext gInputContextCC49C0 {}; // 原全局：0x00CC49C0。

} // namespace cg::input

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004AD090。
static void WindowTextList_ReleaseLines(); // 释放窗口51的行文本及原生指针表。

// 原函数：0x004AD0F0。
static std::uint32_t WindowTextList_RefreshLines(WindowInstance* window); // 九行刷新，返回末行含NUL长度。

// 原函数：0x004AD780。
static void WindowTextList_Release(); // 51释放回调：行表、物品标记、条件关闭请求。

// 原函数：0x004AD800。
static std::int32_t WindowTextList_Update(std::int32_t value); // 51初始化、移动关闭及权限可见性。

// 原函数：0x004AD9F0。
static std::int32_t WindowTextList_Draw(std::int32_t unused); // 51滚动消费、标题输入及完整绘制。

// 原函数：0x004ADAA0。
static std::int32_t WindowTextList_DrawConfirmation(std::int32_t index, std::int32_t unused); // 51模态提示及固定确认按钮。

// 原函数：0x004ADBD0。
static std::int32_t WindowTextList_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行，bit400。

// 原函数：0x004ADC80。
static std::int32_t WindowTextList_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行，bit400。

// 原函数：0x004ADD50。
static std::int32_t WindowTextList_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上九行，bit2。

// 原函数：0x004ADE10。
static std::int32_t WindowTextList_ButtonNextPage(std::int32_t index, std::int32_t state); // 下九行，bit2。

// 原函数：0x004ADEF0。
static std::int32_t WindowTextList_Scroll(std::int32_t index, std::int32_t state); // 51滚轮，draw阶段再刷新文本。

// 原函数：0x004ADF40。
static std::int32_t WindowTextList_SubmitTitle(std::int32_t index, std::int32_t state); // 标题双次转义与N请求。

// 原函数：0x004AE170。
static std::int32_t WindowTextList_SubmitSelection(std::int32_t index, std::int32_t state); // 当前选择I请求，不预拒绝负值。

// 原函数：0x004AE260。
static std::int32_t WindowTextList_EditTitle(std::int32_t index, std::int32_t state); // 标题编辑焦点与InputContext借用。

// 原函数：0x004AE350。
static std::int32_t WindowTextList_HandleItemSelection(std::int32_t index, std::int32_t state); // 51物品选择落点/拖出与失效清理。

// 原函数：0x004AE5B0。
static std::int32_t WindowTextList_DrawBackgroundTooltip(std::int32_t unused, std::int32_t state); // 仅在提示为空时填背景提示。

// 原函数：0x004AD780。
static void WindowTextList_Release();

// 原函数：0x004AD800。
static std::int32_t WindowTextList_Update(std::int32_t value);

// 原函数：0x004AD9F0。
static std::int32_t WindowTextList_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gTitleInputPrototype57EE10 { // 原全局：0x0057EE10。
    {{&input::gInputContextCC49C0}, WindowInputReferenceKind::Context},
    0, 0, 3, 12, 0, {}, 1, 16, 0, 0,
};

// 窗口51拥有行表/行文本；其他载荷仅复制或借用，不另建文本owner。
static std::vector<std::wstring> gWindowLinesD67B70; // 原全局：0x00D67B70。

static std::int32_t gWindowLineCountD67B74 = 0; // 原全局：0x00D67B74。

static std::int32_t gWindowLineScrollCF46B8 = 0; // 原全局：0x00CF46B8。

static std::wstring gWindowTitleCC4118 {}; // 原全局：0x00CC4118。

static std::int32_t gWindowRequestD672B0 = 0; // 原全局：0x00D672B0。

static std::int32_t gWindowRequestCF46AC = 0; // 原全局：0x00CF46AC。

static std::int32_t gWindowCloseRequestCC4B28 = 0; // 原全局：0x00CC4B28。

static std::wstring gWindowModalTextC8AF88 {}; // 原全局：0x00C8AF88。

static WindowGraphicData gWindowGraphic5718A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718A8。

static WindowGraphicData gWindowGraphic5718C0 {0x3BA6A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718C0。

static WindowGraphicData gWindowGraphic5718D8 {0x3BA6B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005718D8。

static WindowGraphicData gWindowGraphic5718F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718F0。

static WindowGraphicData gWindowGraphic571908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571908。

static WindowGraphicData gWindowGraphic571920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571920。

static WindowGraphicData gWindowGraphic571938 {0x3B9F7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571938。

static WindowGraphicData gWindowGraphic571950 {0x3BA6C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571950。

static WindowGraphicData gWindowGraphic571968 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571968。

static WindowGraphicData gWindowGraphic571980 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571980。

static WindowTextPrototype gWindowText571998 { // 原全局：0x00571998。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族獸", // 原常量参考：0x00585E88。
};

static WindowTextPrototype gWindowTexts5719A0[9] { // 原全局：0x005719A0。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
};

static std::uint32_t gWindowMouseFlagsD67898 = 0; // 原全局：0x00D67898。

static WindowElementPrototype gWindowElements5719E8[26] { // 原全局：0x005719E8。
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowTextList_DrawConfirmation},
    {WindowElementType_Graphic, 211, 8, 11, 11, 1, {.graphic = &gWindowGraphic5718A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_EventArea, 57, 30, 150, 12, 1, {.unusedValue = 0}, &WindowTextList_EditTitle},
    {WindowElementType_Text, 57, 30, 0, 0, 1, {.text = &gWindowText571998}, &WindowElement_NoOp},
    {WindowElementType_Input, 57, 30, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 64, 0, 0, 1, {.text = &gWindowTexts5719A0[0]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 81, 0, 0, 1, {.text = &gWindowTexts5719A0[1]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 98, 0, 0, 1, {.text = &gWindowTexts5719A0[2]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 115, 0, 0, 1, {.text = &gWindowTexts5719A0[3]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 132, 0, 0, 1, {.text = &gWindowTexts5719A0[4]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 149, 0, 0, 1, {.text = &gWindowTexts5719A0[5]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 166, 0, 0, 1, {.text = &gWindowTexts5719A0[6]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 183, 0, 0, 1, {.text = &gWindowTexts5719A0[7]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 200, 0, 0, 1, {.text = &gWindowTexts5719A0[8]}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 212, 75, 0, 14, 1, {.graphic = &gWindowGraphic5718F0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 212, 75, 11, 136, 1, {.mouseFlags = &gWindowMouseFlagsD67898}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 212, 65, 11, 11, 1, {.graphic = &gWindowGraphic571908}, &WindowTextList_ButtonPreviousRow},
    {WindowElementType_Graphic, 212, 210, 11, 11, 1, {.graphic = &gWindowGraphic571920}, &WindowTextList_ButtonNextRow},
    {WindowElementType_Graphic, 169, 225, 18, 18, 1, {.graphic = &gWindowGraphic571968}, &WindowTextList_ButtonPreviousPage},
    {WindowElementType_Graphic, 189, 225, 18, 18, 1, {.graphic = &gWindowGraphic571980}, &WindowTextList_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 231, 305, 1, {.unusedValue = 0}, &WindowTextList_Scroll},
    {WindowElementType_Graphic, 165, 43, 57, 17, 1, {.graphic = &gWindowGraphic571938}, &WindowTextList_SubmitTitle},
    {WindowElementType_Graphic, 74, 237, 57, 17, 1, {.graphic = &gWindowGraphic571950}, &WindowTextList_SubmitSelection},
    {WindowElementType_EventArea, 15, 225, 48, 48, 1, {.unusedValue = 0}, &WindowTextList_HandleItemSelection},
    {WindowElementType_Graphic, 0, 0, 267, 305, 1, {.graphic = &gWindowGraphic5718C0}, &WindowTextList_DrawBackgroundTooltip},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5718D8}, &WindowElement_NoOp},
};

// 原型：窗口51；原全局：0x0054EB70。
WindowPrototype gWindowPrototypeTextList {0, 4, 100, 100, 231, 305, 0x80010101u, 26, gWindowElements5719E8, &WindowTextList_Update, &WindowTextList_Draw, &WindowTextList_Release}; // 51: 54EB70

// 窗口函数实现。

// 原函数：0x004AD090。
static void WindowTextList_ReleaseLines()
{
    gWindowLinesD67B70.clear();
    gWindowLineCountD67B74 = 0;
}

// 原函数：0x004AD0F0。
static std::uint32_t WindowTextList_RefreshLines(WindowInstance* const window)
{
    std::uint32_t result = 0;
    for (std::int32_t line = 0; line < 9; ++line) {
        const std::int32_t index = gWindowLineScrollCF46B8 + line;
        const std::int32_t count = gWindowLineCountD67B74;
        auto* const data = static_cast<WindowTextData*>(window->elements[line + 5].data);
        const std::wstring_view source =
            index < count ? std::wstring_view(gWindowLinesD67B70[index]) : std::wstring_view{};
        result = static_cast<std::uint32_t>(source.size() + 1);
        data->text = source;
    }
    return result;
}

// 两个生产入口共享原双遍算法；预计数的<=和实际末段推进30不能合并。
static void WindowTextList_RebuildLines(char* const text)
{
    const std::wstring contents = cg::text::FromGbkWide(text);
    gWindowLinesD67B70 = render::Font::split(contents, 0, 30 * render::gFontMetrics[0].sWidth);
    gWindowLineCountD67B74 = static_cast<std::int32_t>(gWindowLinesD67B70.size());
}

// 原函数：0x004AD180。
login::YobiAction* WindowTextList_Open(
    const std::int32_t request, const std::int32_t target, const char* const text)
{
    gWindowRequestD672B0 = request;
    gWindowRequestCF46AC = target;
    std::array<char, 3072> buffer;
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer.data()) != 0) {
        return nullptr;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    gWindowTitleCC4118 = cg::text::FromGbkWide(buffer.data());
    WindowTextList_ReleaseLines();
    network::ExtractDelimitedField_52CED0(text, '|', 2, 1023, buffer.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    WindowTextList_RebuildLines(buffer.data());
    return WindowRuntime_CreateAnimation(WindowId_TextList, 1, 0, nullptr);
}

// 原函数：0x004AD470。
void WindowTextList_ContinueDialog(const char* const text)
{
    std::array<char, 3072> buffer;
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer.data()) != 0) {
        return;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    WindowInstance* const window = gWindowStates[WindowId_TextList].window;
    gWindowTitleCC4118 = cg::text::FromGbkWide(buffer.data());
    auto* const title = static_cast<WindowTextData*>(window->elements[3].data);
    title->text = gWindowTitleCC4118;
    WindowTextList_ReleaseLines();
    network::ExtractDelimitedField_52CED0(text, '|', 2, 1023, buffer.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    WindowTextList_RebuildLines(buffer.data());
    WindowInstance* const updatedWindow = gWindowStates[WindowId_TextList].window;
    if (updatedWindow != nullptr) {
        static_cast<void>(WindowTextList_RefreshLines(updatedWindow));
    }
}

// 原函数：0x004AD780。
static void WindowTextList_Release()
{
    WindowTextList_ReleaseLines();
    if (gSelectedItem57EE0C >= 0) {
        auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
        const unsigned char value = hover[gSelectedItem57EE0C];
        if (std::bit_cast<std::int8_t>(value) >= 0) {
            hover[gSelectedItem57EE0C] = static_cast<unsigned char>(value - 1);
        }
        gSelectedItem57EE0C = -1;
    }
    if (gWindowCloseRequestCC4B28 != 0) {
        const std::int32_t target = gWindowRequestCF46AC;
        const std::int32_t request = gWindowRequestD672B0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, L"C"));
    }
    world::detail::gValue581800 = -1;
}

// 原函数：0x004AD800。
static std::int32_t WindowTextList_Update(const std::int32_t value)
{
    if (value == -1) {
        gSelectedItem57EE0C = -1;
        gWindowLineScrollCF46B8 = 0;
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
        auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        title->text = gWindowTitleCC4118;
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        gCurrentWindow->elements[0].visible = 0;
        gWindowCloseRequestCC4B28 = 1;
    }
    bool close = true;
    if (world::detail::gValue581800 == 32) {
        close = WindowProfile_CheckDistance(1, 1) != 0;
    } else {
        gWindowCloseRequestCC4B28 = 0;
    }
    if (close) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (input::gCurrentInputData != &input::gInputContextCC49C0) {
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
    }
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0) {
        gCurrentWindow->elements[22].visible = 0;
        if (gSelectedItem57EE0C >= 0) {
            gCurrentWindow->elements[22].visible = 1;
        }
    }
    gCurrentWindow->elements[2].visible = 0;
    gCurrentWindow->elements[21].visible = 0;
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 0x80u) != 0) {
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[21].visible = 1;
    }
    gCurrentWindow->elements[22].visible = 0;
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0) {
        if (gSelectedItem57EE0C < 0) {
            return 1;
        }
        gCurrentWindow->elements[22].visible = 1;
    }
    if (gSelectedItem57EE0C >= 0
        && world::detail::BorrowPlayerItemsFDA8A4()[gSelectedItem57EE0C].itemType == 53) {
        gCurrentWindow->elements[22].visible = 1;
    }
    return 1;
}

// 原函数：0x004AD9F0。
static std::int32_t WindowTextList_Draw(const std::int32_t)
{
    gCurrentWindow->elements[14].visible = 1;
    if (gWindowLineCountD67B74 - 9 <= 0) {
        gCurrentWindow->elements[14].visible = 0;
    }
    WindowInstance* window = gCurrentWindow;
    auto* const mouse = static_cast<WindowMouseData*>(window->elements[15].data);
    if ((static_cast<std::uint8_t>(mouse->flags) & 1u) != 0) {
        gWindowLineScrollCF46B8 = WindowScroll_ReadPosition(
            &window->elements[15], gWindowLineCountD67B74 - 9);
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        window = gCurrentWindow;
    }
    const std::int32_t y = window->y + window->elements[4].y;
    const std::int32_t x = window->x + window->elements[4].x;
    static_cast<void>(WindowInput_ActivateElement(&gTitleInputPrototype57EE10, x, y, 2));
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004ADAA0。
static std::int32_t WindowTextList_DrawConfirmation(const std::int32_t index, const std::int32_t)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t imageNo = 243048;
    if (login::detail::Sub_5049A0(287, 218, 353, 235, -1) != 0) {
        imageNo = 243050;
        if ((static_cast<std::uint8_t>(platform::BorrowLegacyMouseStateStorage()->edgeButtons) & 1u) != 0) {
            gCurrentWindow->elements[index].visible = 0;
            gCurrentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            imageNo = 243049;
        }
    }
    static_cast<void>(login::detail::DrawGraphic(320, 227, 226, imageNo, 0, &descriptor));
    static_cast<void>(render::DrawText_456780(185, 185, 4, 0, 0, gWindowModalTextC8AF88, 0, static_cast<std::uint8_t>(login::detail::gValueD69768)));
    login::detail::DrawNineGrid_49BE00(
        245300, 155, 155, 330, 110, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004ADBD0。
static std::int32_t WindowTextList_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowLineScrollCF46B8 > 0) {
            --gWindowLineScrollCF46B8;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[index - 1], maximum, position));
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return result;
}

// 原函数：0x004ADC80。
static std::int32_t WindowTextList_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t last = gWindowLineCountD67B74 - 9;
        if (gWindowLineScrollCF46B8 < (last < 0 ? 0 : last)) {
            ++gWindowLineScrollCF46B8;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[index - 2], maximum, position));
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return result;
}

// 原函数：0x004ADD50。
static std::int32_t WindowTextList_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gWindowLineScrollCF46B8 > 0 ? 51 : 55, 320));
        gWindowLineScrollCF46B8 -= 9;
        if (gWindowLineScrollCF46B8 < 0) {
            gWindowLineScrollCF46B8 = 0;
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[index - 3], maximum, position));
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243030;
    if ((state & 1) != 0) {
        graphic->imageNo = 243032;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243031;
    }
    return result;
}

// 原函数：0x004ADE10。
static std::int32_t WindowTextList_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t last = gWindowLineCountD67B74 - 9;
        static_cast<void>(audio::PlaySe_52C450(
            last > 0 && gWindowLineScrollCF46B8 < last ? 51 : 55, 320));
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        gWindowLineScrollCF46B8 += 9;
        const std::int32_t limit = maximum < 0 ? 0 : maximum;
        if (gWindowLineScrollCF46B8 > limit) {
            gWindowLineScrollCF46B8 = limit;
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[index - 4], maximum, position));
        static_cast<void>(WindowTextList_RefreshLines(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243034;
    }
    return result;
}

// 原函数：0x004ADEF0。
static std::int32_t WindowTextList_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gWindowLineCountD67B74;
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t position = gWindowLineScrollCF46B8;
        gWindowLineScrollCF46B8 =
            WindowScroll_ApplyWheel(&gCurrentWindow->elements[index - 5], position, count - 9, wheel);
    }
    return 0;
}

// 原函数：0x004ADF40。
static std::int32_t WindowTextList_SubmitTitle(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if (elements[4].visible != 0) {
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"變更家族獸名稱。";
        }
        if ((state & 2) != 0 || platform::ReadKeyFlag4_4597A0(13) != 0) {
            const std::wstring proposedTitle = input::ContextText(input::gInputContextCC49C0);
            const std::wstring name = input::EscapeTextAndNotify(proposedTitle, 255);
            const std::wstring packet = input::EscapeText(L"N|" + name, 255);
            gWindowTitleCC4118 = proposedTitle;
            static_cast<void>(WindowChat_RestoreInput());
            auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
            title->text = gWindowTitleCC4118;
            gCurrentWindow->elements[4].visible = 0;
            gCurrentWindow->elements[2].visible = 1;
            gCurrentWindow->elements[3].visible = 1;
            const std::int32_t target = gWindowRequestCF46AC;
            const std::int32_t request = gWindowRequestD672B0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, packet));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    graphic->imageNo = 244215;
    if ((state & 1) != 0) {
        graphic->imageNo = 244217;
    }
    if (input::gCurrentInputData != &input::gInputContextCC49C0) {
        graphic->imageNo = 244216;
    }
    return consumed;
}

// 原函数：0x004AE170。
static std::int32_t WindowTextList_SubmitSelection(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"用滑鼠拖放物品至此視窗做為飼料。";
    }
    if ((state & 2) != 0) {
        std::array<char, 156> buffer;
        std::sprintf(buffer.data(), "I|%d", gSelectedItem57EE0C);
        const std::int32_t target = gWindowRequestCF46AC;
        const std::int32_t request = gWindowRequestD672B0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, cg::text::FromGbkWide(buffer.data())));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 244332;
    if ((state & 1) != 0) {
        graphic->imageNo = 244334;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 244333;
    }
    if (gSelectedItem57EE0C < 0) {
        graphic->imageNo = 244333;
    }
    return consumed;
}

// 原函数：0x004AE260。
static std::int32_t WindowTextList_EditTitle(const std::int32_t index, const std::int32_t state)
{
    const std::uint32_t imageNo = login::detail::gValueD69768;
    WindowElement_DrawHoverFrame(&gCurrentWindow->elements[index], state, imageNo);
    if ((state & 2) == 0) {
        return 0;
    }
    gCurrentWindow->elements[2].visible = 0;
    gCurrentWindow->elements[3].visible = 0;
    gCurrentWindow->elements[4].visible = 1;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y + window->elements[4].y;
    const std::int32_t x = window->x + window->elements[4].x;
    static_cast<void>(WindowInput_ActivateElement(&gTitleInputPrototype57EE10, x, y, 0));
    input::SetContext_44B330(&input::gInputContextCC49C0);
    input::LoadInitialText_44A160(gWindowTitleCC4118);
    WindowInstance* const focusedWindow = gCurrentWindow;
    gFocusedElementDataD45F94 = focusedWindow->elements[4].data;
    static_cast<WindowInputData*>(focusedWindow->elements[4].data)->context = &input::gInputContextCC49C0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    return 1;
}

// 原函数：0x004AE350。
static std::int32_t WindowTextList_HandleItemSelection(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    const auto x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(window->elements[index].x + window->x));
    const auto y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(window->elements[index].y + window->y));
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2 && WindowDrag_ReadKind() != 8) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0
                || world::detail::BorrowPlayerItemsFDA8A4()[source].itemType == 53) {
                const unsigned char previous = occupancy[gSelectedItem57EE0C];
                if (std::bit_cast<std::int8_t>(previous) >= 0) {
                    occupancy[gSelectedItem57EE0C] = static_cast<unsigned char>(previous - 1);
                }
                const unsigned char next = occupancy[source];
                gSelectedItem57EE0C = source;
                occupancy[source] = static_cast<unsigned char>(next + 1);
            } else {
                WindowInstance* const modalWindow = gCurrentWindow;
                gWindowModalTextC8AF88 = L"你沒有餵食寵物的權限。";
                modalWindow->elements[0].visible = 1;
                gCurrentWindow->state |= 2u;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            WindowDrag_Reset();
        }
    }
    std::int32_t selected;
    if ((events & 1u) != 0 && (events & 2u) != 0) {
        selected = gSelectedItem57EE0C;
        if (selected >= 0) {
            static_cast<void>(WindowItemDrag_Start(selected, 1));
            const unsigned char previous = occupancy[gSelectedItem57EE0C];
            if (std::bit_cast<std::int8_t>(previous) >= 0) {
                occupancy[gSelectedItem57EE0C] = static_cast<unsigned char>(previous - 1);
            }
            gSelectedItem57EE0C = -1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            selected = gSelectedItem57EE0C;
        }
        consumed = 1;
    } else {
        selected = gSelectedItem57EE0C;
    }
    if ((events & 0x21u) != 0) {
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
        selected = gSelectedItem57EE0C;
    }
    if (selected >= 0) {
        const auto& item = world::detail::BorrowPlayerItemsFDA8A4()[selected];
        if (item.used != 0) {
            static_cast<void>(login::detail::DrawGraphic(
                x + 24, y + 24, 223, item.imageNo, 0, &descriptor));
            return consumed;
        }
        const unsigned char previous = occupancy[selected];
        if (std::bit_cast<std::int8_t>(previous) >= 0) {
            occupancy[selected] = static_cast<unsigned char>(previous - 1);
        }
        gSelectedItem57EE0C = -1;
    }
    return consumed;
}

// 原函数：0x004AE5B0。
static std::int32_t WindowTextList_DrawBackgroundTooltip(const std::int32_t, const std::int32_t state)
{
    if ((state & 1) != 0 && gHoverTextD68AE8.empty()) {
        gHoverTextD68AE8 = L"從物品視窗以滑鼠拖放方式選擇要餵的飼料。";
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
