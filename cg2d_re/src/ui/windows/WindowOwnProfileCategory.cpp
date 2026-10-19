#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C94F0。
static std::int32_t WindowOwnProfileCategory_SelectCategory(std::int32_t index, std::int32_t state); // 十行分类选择，写当前资料。

// 原函数：0x004C96F0。
static std::int32_t WindowOwnProfileCategory_HandleScrollButtons(std::int32_t index, std::int32_t state); // 窗口59四滚动/翻页按钮。

// 原函数：0x004C9B80。
static std::int32_t WindowOwnProfileCategory_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原分类count-10拖动换算；旧X代码1992。

// 原函数：0x004C9BE0。
static std::int32_t WindowOwnProfileCategory_Scroll(std::int32_t unusedIndex, std::int32_t state); // 三模式滚轮，恒返0。

// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
static std::int32_t WindowOwnProfileCategory_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00CA0444。 窗口59六入口共用的S32页码。
static std::int32_t gProfileScrollCA0444 = 0; // 原全局：0x00CA0444。

static WindowGraphicData gWindowGraphic577170 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577170。

static WindowGraphicData gWindowGraphic577188 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577188。

static WindowGraphicData gWindowGraphic5771A0 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005771A0。

static WindowGraphicData gWindowGraphic5771B8 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771B8。

static WindowGraphicData gWindowGraphic5771E8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771E8。

static WindowGraphicData gWindowGraphic577200 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577200。

static WindowGraphicData gWindowGraphic577218 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577218。

static WindowGraphicData gWindowGraphic577248 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577248。

static WindowGraphicData gWindowGraphic577260 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577260。

static WindowGraphicData gWindowGraphic577278 {0x3C386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577278。

static WindowTextPrototype gWindowText577290 { // 原全局：0x00577290。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"物品", // 原常量参考：0x00585D78。
};

static std::uint32_t gWindowMouseFlagsD67930 = 0; // 原全局：0x00D67930。

static WindowElementPrototype gWindowElements577298[32] { // 原全局：0x00577298。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic577170}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic5771E8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD67930}, &WindowOwnProfileCategory_DragScroll},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic577200}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic577218}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 89, 7, 0, 0, 1, {.graphic = &gWindowGraphic577278}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic577248}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic577260}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowOwnProfileCategory_Scroll},
    {WindowElementType_Text, 20, 30, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 46, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 62, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 78, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 94, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 110, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 126, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 142, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 158, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 174, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577188}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5771A0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口59；原全局：0x0054ED60。
WindowPrototype gWindowPrototypeOwnProfileCategory {0, 4, 38, 95, 225, 220, 0x80010101u, 32, gWindowElements577298, &WindowOwnProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 59: 54ED60

// 窗口函数实现。

// 原函数：0x004C9C70。
static void WindowOwnProfileCategory_Refresh()
{
    const std::int32_t mode = gProfileCategory1085DA0;
    std::int32_t image;
    switch (mode) {
    case 0: image = 0x3C386; break;
    case 1: image = 0x3C387; break;
    case 2: image = 0x3C385; break;
    default: return;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[5].data)->imageNo = image;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(mode, page + row);
        const wchar_t* const source =
            record.used != 0 ? record.text0C.data() : L"";
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data);
        data->text = source;
    }
}

// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
static std::int32_t WindowOwnProfileCategory_Update(const std::int32_t value)
{
    if (value == -1) {
        gProfileScrollCA0444 = 0;
    }
    if (gWindowStates[WindowId_OwnProfile].window == nullptr) {
        gCurrentWindow->state |= 0x8000u;
    }
    WindowOwnProfileCategory_Refresh();
    return 1;
}

// 原函数：0x004C94F0。
static std::int32_t WindowOwnProfileCategory_SelectCategory(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        switch (gProfileCategory1085DA0) {
        case 0: gHoverTextD68AE8 = L"设定贩卖的目录" /* 原常量参考：0x005884F4。 */; break;
        case 1: gHoverTextD68AE8 = L"设定购买的目录" /* 原常量参考：0x00588504。 */; break;
        case 2: gHoverTextD68AE8 = L"设定描述的目录" /* 原常量参考：0x00588514。 */; break;
        default: break;
        }
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB04;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB02;
    }

    const std::int32_t selectedMode = gProfileCategory1085DA0;
    if (selectedMode >= 0 && selectedMode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const std::int32_t selected =
            world::detail::gCurrentProfile10738E8.entries410[selectedMode].value00;
        if (selected == page + index - 19) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB03;
        }
    }
    if ((state & 2) == 0) {
        return consumed;
    }

    const std::int32_t mode = gProfileCategory1085DA0;
    if (mode >= 0 && mode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(mode, page + index - 19);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t value = record.value04;
        const std::int32_t image = record.value08;
        auto& entry = world::detail::gCurrentProfile10738E8.entries410[mode];
        entry.value00 = value;
        entry.value04 = image;
    }
    static_cast<void>(WindowChat_RestoreInput());
    return 1;
}

// 原函数：0x004C96F0。
static std::int32_t WindowOwnProfileCategory_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };
    switch (index) {
    case 3: {
        if ((state & 1) != 0) {
            setImage(0x3B594);
            consumed = 1;
        } else {
            setImage(0x3B592);
        }
        if ((state & 8) != 0) {
            setImage(0x3B593);
            consumed = 1;
        }
        if ((state & 0x400) == 0) {
            return consumed;
        }
        const std::int32_t page = gProfileScrollCA0444 - 1;
        gProfileScrollCA0444 = page;
        if (page < 0) {
            gProfileScrollCA0444 = 0;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t position = gProfileScrollCA0444;
            WindowInstance* window;
            std::int32_t count;
            if (mode == 2) {
                window = gCurrentWindow;
                count = world::detail::ReadCategoryCount105FA1C();
            } else {
                count = world::detail::ReadCategoryCount105FA18();
                window = gCurrentWindow;
            }
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, count - 10, position));
        }
        return 1;
    }
    case 4: {
        if ((state & 1) != 0) {
            setImage(0x3B597);
            consumed = 1;
        } else {
            setImage(0x3B595);
        }
        if ((state & 8) != 0) {
            setImage(0x3B596);
            consumed = 1;
        }
        if ((state & 0x400) == 0) {
            return consumed;
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            const std::int32_t page = gProfileScrollCA0444 + 1;
            gProfileScrollCA0444 = page;
            if (page <= count - 11) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gProfileScrollCA0444;
            const std::int32_t currentCount = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, currentCount - 10, position));
        }
        return 1;
    }
    case 6:
        setImage((state & 1) != 0 ? 0x3B558 : 0x3B556);
        if ((state & 2) != 0) {
            std::int32_t page = gProfileScrollCA0444 - 10;
            gProfileScrollCA0444 = page;
            if (page < 0) {
                page = 0;
                gProfileScrollCA0444 = page;
            }
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, count - 10, page));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3B557);
        }
        return consumed;
    case 7:
        setImage((state & 1) != 0 ? 0x3B55B : 0x3B559);
        if ((state & 2) != 0) {
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                const std::int32_t page = gProfileScrollCA0444 + 10;
                gProfileScrollCA0444 = page;
                if (page > count - 11) {
                    gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                const std::int32_t position = gProfileScrollCA0444;
                const std::int32_t currentCount = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, currentCount - 10, position));
            }
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3B55A);
        }
        return consumed;
    default:
        return 0;
    }
}

// WSP01：旧X片段1992；恰1且mode0..2才按原分类count-10更新。
// 原函数：0x004C9B80。
static std::int32_t WindowOwnProfileCategory_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result == 1) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            WindowInstance* const window = gCurrentWindow;
            gProfileScrollCA0444 = WindowScroll_ReadPosition(window->elements + 2, count - 10);
        }
    }
    return result;
}

// 原函数：0x004C9BE0。
static std::int32_t WindowOwnProfileCategory_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            const std::int32_t page = gProfileScrollCA0444;
            WindowInstance* const window = gCurrentWindow;
            gProfileScrollCA0444 = WindowScroll_ApplyWheel(window->elements + 2, page, count - 10, wheel);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
