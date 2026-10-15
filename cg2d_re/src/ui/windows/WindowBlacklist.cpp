#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

// 控件载荷、窗口状态、组件表与原型。

InputContext gInputContextC8B1B8 {}; // 原全局：0x00C8B1B8。

} // namespace cg::input

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004CCED0。
static void WindowBlacklist_Refresh(); // 窗口65十行序号/名称与活动输入刷新。

// 原函数：0x004CC6E0。
static std::int32_t WindowBlacklist_EditRow(std::int32_t index, std::int32_t state); // 删除后重读行，再决定提示或编辑。

// 原函数：0x004CC9B0。
static std::int32_t WindowBlacklist_HandleButtons(std::int32_t index, std::int32_t state); // 六个黑名单按钮及原消费差异。

// 原函数：0x004CCE90。
static std::int32_t WindowBlacklist_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原90参数拖动换算；旧X代码1999。

// 原函数：0x004CD150。
static login::YobiAction* WindowBlacklist_ConfirmClear(); // 生产窗口47的清空确认及两个真实回调。

// 原函数：0x004CD1E0。
static std::int32_t WindowBlacklist_Clear(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x004CD220。
static std::int32_t WindowBlacklist_CancelClear(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x004CC6C0。
static std::int32_t WindowBlacklist_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gBlacklistInputPrototype580EF0 { // 原全局：0x00580EF0。
    {{&input::gInputContextC8B1B8}, WindowInputReferenceKind::Context},
    75, 30, 3, 9, 0, {}, 1, 20, 0, 0,
};

static WindowGraphicData gWindowGraphic57A310 {243001, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A310。

static WindowGraphicData gWindowGraphic57A328 {36813, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A328。

static WindowGraphicData gWindowGraphic57A340 {244481, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0057A340。

static WindowGraphicData gWindowGraphic57A358 {244483, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A358。

static WindowGraphicData gWindowGraphic57A370 {243380, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A370。

static WindowGraphicData gWindowGraphic57A388 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A388。

static WindowGraphicData gWindowGraphic57A3A0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3A0。

static WindowGraphicData gWindowGraphic57A3B8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3B8。

static WindowGraphicData gWindowGraphic57A3D0 {244485, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3D0。

static WindowGraphicData gWindowGraphic57A3E8 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3E8。

static WindowGraphicData gWindowGraphic57A400 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A400。

static WindowGraphicData gWindowGraphic57A418 {36806, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A418。

static WindowGraphicData gWindowGraphic57A430 {36809, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A430。

static WindowTextPrototype gWindowText57A448 { // 原全局：0x0057A448。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"???",
};

static std::uint32_t gWindowMouseFlagsD679B8 = 0; // 原全局：0x00D679B8。

static WindowElementPrototype gWindowElements57A450[71] { // 原全局：0x0057A450。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic57A310}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic57A388}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD679B8}, &WindowBlacklist_DragScroll},
    {WindowElementType_Graphic, 206, 31, 11, 11, 1, {.graphic = &gWindowGraphic57A3A0}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic57A3B8}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A3E8}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 60, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A400}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 100, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A418}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 155, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A430}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic57A328}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic57A340}, &WindowElement_NoOp},
};

// 原型：窗口65；原全局：0x0054EE80。
WindowPrototype gWindowPrototypeBlacklist {0, 4, 38, 95, 251, 210, 0x80010101u, 71, gWindowElements57A450, &WindowBlacklist_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 65: 54EE80

// 窗口函数实现。

// 原函数：0x004CC6A0。
std::int32_t WindowBlacklist_ResetState()
{
    gLoginWindowRuntime.valueCC31A4 = 5;
    gLoginWindowRuntime.valueD2E444 = 0;
    gLoginWindowRuntime.valueCB0604 = 0;
    return 0;
}

// 原函数：0x004CCED0。
static void WindowBlacklist_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    for (std::int32_t row = 0; row < 10; ++row) {
        gCurrentWindow->elements[9 + row * 3].visible = 0;
        gCurrentWindow->elements[10 + row * 3].visible = 0;
        gCurrentWindow->elements[11 + row * 3].visible = 0;
        const char* const recordAddress =
            reinterpret_cast<const char*>(blacklist::gRecords[row].data());
        const char* text;
        WindowTextData* destination;
        // 原门禁比较静态记录地址，不是记录首字节；空记录也绘行号。
        if (recordAddress != nullptr) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444;
            const std::uint32_t color = 0xFFFFFFFFu;
            std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
            descriptor.bytes[13] = 4;
            const std::int32_t number = page + row + 1;
            for (std::int32_t column = 0; column < 3; ++column) {
                WindowInstance* const window = gCurrentWindow;
                WindowElement* const elements = window->elements;
                const std::int32_t digit = column == 0 ? (number % 1000) / 100
                    : column == 1 ? (number % 100) / 10 : number % 10;
                const WindowElement& element = elements[9 + row * 3 + column];
                const std::int32_t y = element.y + window->y + 4;
                const std::int32_t x = element.x + window->x + 1;
                static_cast<void>(login::detail::DrawGraphic(
                    x, y, 223, digit + 0x3B6B4, 0, &descriptor));
            }
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            text = reinterpret_cast<const char*>(blacklist::gRecords[currentPage + row].data());
            destination = static_cast<WindowTextData*>(gCurrentWindow->elements[49 + row].data);
        } else {
            WindowInstance* const window = gCurrentWindow;
            text = input::LegacyText593850();
            destination = static_cast<WindowTextData*>(window->elements[49 + row].data);
        }
        destination->text = cg::text::FromGbkWide(text);
        gCurrentWindow->elements[39 + row].visible = 0;
    }
    if (input::gCurrentInputData == &input::gInputContextC8B1B8) {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, x, y, 1));
        render::DrawInputText_4568A0(&input::gInputContextC8B1B8);
    }
}

// 原函数：0x004CC6C0。
static std::int32_t WindowBlacklist_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueD2E444 = 0;
    }
    WindowBlacklist_Refresh();
    return 1;
}

// 原函数：0x004CC6E0。
static std::int32_t WindowBlacklist_EditRow(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 0
            ? L"刪除此筆黑名單。" /* 原常量参考：0x00588654。 */ : L"拒絕接收此人的發話。" /* 原常量参考：0x00588668。 */;
        gHoverTextD68AE8 = hint;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FCC;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB02;
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
        const std::int32_t recordIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
        const char* const record = reinterpret_cast<const char*>(blacklist::gRecords[recordIndex].data());
        if (std::strcmp(record, input::LegacyText593850()) != 0) {
            std::wstring message;
            char name[17] {};
            std::strcpy(name, record);
            if (blacklist::ContainsBlacklistRecord_4013D0(cg::text::FromGbkWide(name)) != 0) {
                const wchar_t* const format = blacklist::RemoveBlacklistRecord_4012F0(cg::text::FromGbkWide(name)) != 0
                    ? L"已從黑名單列表中移除{}" /* 原常量参考：0x0058863C。 */ : L"無法將{}從黑名單列表中移除" /* 原常量参考：0x00588620。 */;
                cg::text::FormatTo(message, format, cg::text::FromGbkWide(name).c_str());
            } else {
                cg::text::FormatTo(message, L"{}不在黑名單列表中" /* 原常量参考：0x0058860C。 */, cg::text::FromGbkWide(name).c_str());
            }
            world::detail::Sub_44B050(message, 4, 3);
        }
    }
    const std::int32_t currentIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
    const char* const currentRecord = reinterpret_cast<const char*>(blacklist::gRecords[currentIndex].data());
    if (std::strlen(currentRecord) != 0) {
        input::gInputContextC8B1B8.reset();
        input::SetContext_44B330(&input::gInputContext626AA0);
        world::detail::Sub_44B050(L"此欄位已有名單" /* 原常量参考：0x005885FC。 */, 4, 3);
        return 1;
    }
    WindowInstance* const window = gCurrentWindow;
    void* const focused = gFocusedElementDataD45F94;
    WindowElement* const elements = window->elements;
    void* const target = elements[0].data;
    const std::int32_t y = elements[index].y;
    const bool changeFocus = focused != target;
    gBlacklistInputPrototype580EF0.y = y + 1;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, windowX, windowY, 0));
    input::SetContext_44B330(&input::gInputContextC8B1B8);
    if (changeFocus) {
        gFocusedElementDataD45F94 = gCurrentWindow->elements[0].data;
    }
    return 1;
}

// 原函数：0x004CC9B0。
static std::int32_t WindowBlacklist_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    switch (index) {
    case 3:
    case 4: {
        const std::int32_t base = index == 3 ? 0x3B592 : 0x3B595;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            base + ((state & 1) != 0 ? 2 : 0);
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = base + 1;
            consumed = 1;
        }
        if ((state & 0x400) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (index == 3 ? -1 : 1);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = index == 3 ? page < 0 : page > 89;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = index == 3 ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            static_cast<void>(WindowScroll_SetPosition(&window->elements[2], 90, currentPage));
            consumed = 1;
        }
        break;
    }
    case 5:
    case 6: {
        const std::int32_t base = index == 5 ? 0x3B556 : 0x3B559;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            base + ((state & 1) != 0 ? 2 : 0);
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = index == 5 ? L"顯示前十筆的黑名單。" /* 原常量参考：0x005886CC。 */ : L"顯示後十筆的黑名單。" /* 原常量参考：0x005886B4。 */;
        }
        if ((state & 0x400) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (index == 5 ? -10 : 10);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = index == 5 ? page < 0 : page > 90;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = index == 5 ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            static_cast<void>(WindowScroll_SetPosition(&window->elements[2], 90, currentPage));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = base + 1;
        }
        break;
    }
    case 7:
        if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC7;
            gHoverTextD68AE8 = L"刪除所選擇的黑名單。" /* 原常量参考：0x0058869C。 */;
        }
        if ((state & 1) != 0) {
            if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC8;
            }
            gHoverTextD68AE8 = L"刪除所選擇的黑名單。" /* 原常量参考：0x0058869C。 */;
            consumed = 1;
        } else if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC6;
        }
        if ((state & 2) != 0) {
            gLoginWindowRuntime.blacklistDeleteModeD67BC4 =
                gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 1 ? 1 : 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    case 8:
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            (state & 1) != 0 ? 0x8FCB : 0x8FC9;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"可一次刪除所有的黑名單。" /* 原常量参考：0x00588680。 */;
            consumed = 1;
        }
        if ((state & 2) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FCA;
            static_cast<void>(WindowBlacklist_ConfirmClear());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    default:
        break;
    }
    return consumed;
}

// WSP01：旧X片段1999；恰1才按原90参数更新，保原X身份与返回。
// 原函数：0x004CCE90。
static std::int32_t WindowBlacklist_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t consumed = WindowElement_HandleVerticalScroll(index, state);
    if (consumed == 1) {
        gLoginWindowRuntime.valueD2E444 = WindowScroll_ReadPosition(&gCurrentWindow->elements[2], 90);
    }
    return consumed;
}

// 原函数：0x004CD150。
static login::YobiAction* WindowBlacklist_ConfirmClear()
{
    gWindow47Runtime.visibleMaskCC4240 = 3;
    gWindow47Runtime.widthUnitsCC4244 = 3;
    gWindow47Runtime.valueCC424C = 0;
    gWindow47Runtime.valueCC4248 = 0;
    gWindow47Runtime.initializeCC4250 = nullptr;
    gWindow47Runtime.actionsCC4254[5] = nullptr;
    gWindow47Runtime.actionsCC4254[4] = nullptr;
    gWindow47Runtime.actionsCC4254[3] = nullptr;
    gWindow47Runtime.actionsCC4254[2] = nullptr;
    gWindow47Runtime.actionsCC4254[0] = &WindowBlacklist_Clear;
    gWindow47Runtime.actionsCC4254[1] = &WindowBlacklist_CancelClear;
    gWindow47Runtime.textCC426C = L"" LR"(
)" L"  " LR"(
)" L"    確定刪除" LR"(
)" L"   所有的黑名單嗎？" LR"(
)" L" " LR"(
)" L" (確定刪除時，請按住" LR"(
)" L" CTRL鍵再選擇「是」)" /* 原常量参考：0x005886E4。 */;
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004CD1E0。
static std::int32_t WindowBlacklist_Clear(const std::int32_t, const std::int32_t)
{
    const wchar_t* const message = blacklist::ClearBlacklistRecords_401470() != 0
        ? L"已刪除所有的黑名單。" /* 原常量参考：0x00588754。 */ : L"黑名單並沒有內容可刪除。" /* 原常量参考：0x00588738。 */;
    world::detail::Sub_44B050(message, 4, 3);
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004CD220。
static std::int32_t WindowBlacklist_CancelClear(const std::int32_t, const std::int32_t)
{
    if (gWindowStates[WindowId_Blacklist].window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Blacklist, 1, 0, nullptr));
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
