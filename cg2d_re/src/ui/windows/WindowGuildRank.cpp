#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(std::int32_t value); // 窗口55只初始化滚动位置。

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(std::int32_t unused); // 刷新/公共绘制后反求滚动，再画九宫格。

// 原函数：0x004A4220。
static std::int32_t WindowGuildRank_SelectRow(std::int32_t index, std::int32_t state); // 职级行选择及当前选择图号。

// 原函数：0x004A42C0。
static std::int32_t WindowGuildRank_ButtonClose(std::int32_t index, std::int32_t state); // 固定入口窗口关闭、声音后的图形更新。

// 原函数：0x004A4370。
static std::int32_t WindowGuildRank_ButtonOk(std::int32_t index, std::int32_t state); // uHb设置后重取窗口关闭，返回仅由hover决定。

// 原函数：0x004A4420。
static std::int32_t WindowGuildRank_HandleScrollButtons(std::int32_t index, std::int32_t state); // 上下滚动先发声，只修各自一侧边界。

// 原函数：0x004A4540。
static std::uint32_t WindowGuildRank_Refresh(); // 十行职级/成员姓名刷新，返回含NUL姓名长度。

// 原函数：0x004A45E0。
static std::int32_t WindowGuildRank_Scroll(std::int32_t unused, std::int32_t state); // 非零滚轮推进，恒返回0。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(std::int32_t value);

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56F7A8 {0x3B59C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7A8。

static WindowGraphicData gWindowGraphic56F7C0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7C0。

static WindowGraphicData gWindowGraphic56F7D8 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7D8。

static WindowGraphicData gWindowGraphic56F7F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7F0。

static WindowGraphicData gWindowGraphic56F808 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F808。

static WindowGraphicData gWindowGraphic56F820 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F820。

static WindowGraphicData gWindowGraphic56F838 {0x3BAA3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F838。

static WindowTextPrototype gWindowText56F850 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族稱號"}; // 原全局：0x0056F850。 // 原常量参考：0x00585ED8。

static WindowTextPrototype gWindowText56F858 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"成員名稱"}; // 原全局：0x0056F858。 // 原常量参考：0x00585ECC。

static std::uint32_t gWindowMouseFlagsD67860 = 0; // 原全局：0x00D67860。

static WindowElementPrototype gWindowElements56F860[29] { // 原全局：0x0056F860。
    {WindowElementType_Text, 7, 21, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 37, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 53, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 69, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 85, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 101, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 117, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 133, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 149, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 165, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 8, 181, 0, 0, 1, {.text = &gWindowText56F858}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 150, 181, 49, 17, 1, {.graphic = &gWindowGraphic56F7C0}, &WindowGuildRank_ButtonClose},
    {WindowElementType_Graphic, 206, 181, 32, 17, 1, {.graphic = &gWindowGraphic56F7D8}, &WindowGuildRank_ButtonOk},
    {WindowElementType_Graphic, 224, 0, 0, 14, 1, {.graphic = &gWindowGraphic56F7F0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 225, 33, 11, 135, 1, {.mouseFlags = &gWindowMouseFlagsD67860}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 224, 22, 11, 11, 1, {.graphic = &gWindowGraphic56F808}, &WindowGuildRank_HandleScrollButtons},
    {WindowElementType_Graphic, 224, 168, 11, 11, 1, {.graphic = &gWindowGraphic56F820}, &WindowGuildRank_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 243, 206, 1, {.unusedValue = 0}, &WindowGuildRank_Scroll},
    {WindowElementType_Graphic, 221, 18, 0, 0, 1, {.graphic = &gWindowGraphic56F838}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 5, 19, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 35, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 51, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 67, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 83, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 99, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 115, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 131, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 147, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 163, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
};

// 原型：窗口55；原全局：0x0054E9C0。
WindowPrototype gWindowPrototypeGuildRank {0, 4, 100, 100, 243, 206, 0x80000080u, 29, gWindowElements56F860, &WindowGuildRank_Update, &WindowGuildRank_Draw, &WindowRuntime_ReleaseDefault}; // 55: 54E9C0

// 窗口函数实现。

// 原函数：0x004A4180。
static std::int32_t WindowGuildRank_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.mailRankScrollCA0474 = 0;
    }
    return 1;
}

// 原函数：0x004A41A0。
static std::int32_t WindowGuildRank_Draw(const std::int32_t)
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
static std::int32_t WindowGuildRank_SelectRow(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"選擇此稱號。" /* 原常量参考：0x005876F4。 */;
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
static std::int32_t WindowGuildRank_ButtonClose(const std::int32_t index, const std::int32_t state)
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
static std::int32_t WindowGuildRank_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B570;
        gHoverTextD68AE8 = L"設定已選擇的稱號。" /* 原常量参考：0x00587704。 */;
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
static std::int32_t WindowGuildRank_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
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
static std::uint32_t WindowGuildRank_Refresh()
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
static std::int32_t WindowGuildRank_Scroll(const std::int32_t, const std::int32_t state)
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
