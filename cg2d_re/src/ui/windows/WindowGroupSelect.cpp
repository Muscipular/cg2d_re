#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004B4B00。
static std::int32_t WindowGroupSelect_Update(std::int32_t value); // 群组选人初始化/恢复与父窗门禁。

// 原函数：0x004B4B70。
static void WindowGroupSelect_Release(); // 释放时仅关闭仍处于群发方式的22。

// 原函数：0x004B4B90。
static std::int32_t WindowGroupSelect_Draw(std::int32_t value); // 滚动反求、七行刷新、公共绘制。

// 原函数：0x004B4BB0。
static std::int32_t WindowGroupSelect_HandlePageButtons(std::int32_t index, std::int32_t state); // 前后翻七行，仅hover消费。

// 原函数：0x004B4CA0。
static std::int32_t WindowGroupSelect_HandleScrollButtons(std::int32_t index, std::int32_t state); // bit400单行滚动，发声后修正位置。

// 原函数：0x004B4DB0。
static std::int32_t WindowGroupSelect_ClampScroll(); // 顺序修正上下界，保留count<7时负上界。

// 原函数：0x004B4DF0。
static std::int32_t WindowGroupSelect_DragScroll(std::int32_t unused); // 非负最大值反求并发布滚动位置。

// 原函数：0x004B4E20。
static void WindowGroupSelect_Refresh(); // 三种标题和七行显隐，不改选择标志。

// 原函数：0x004B5210。
static std::int32_t WindowGroupSelect_TabCategory(std::int32_t index, std::int32_t state); // 切换三模式后清滚动位置。

// 原函数：0x004B53C0。
static std::int32_t WindowGroupSelect_ButtonSelectAll(std::int32_t index, std::int32_t state); // 全组选择，普通联系人借原排序索引。

// 原函数：0x004B5550。
static std::int32_t WindowGroupSelect_SelectRow(std::int32_t index, std::int32_t state); // 七行选择，点击时重读滚动位置。

// 原函数：0x004B5A40。
static std::int32_t WindowGroupSelect_Scroll(std::int32_t unused, std::int32_t state); // 零wheel仍调用4743B0。

// 原函数：0x004B5630。
static void WindowGroupSelect_ResetRecipients(); // 清计数/模式及两200项数组，不清初始化标志。

// 原函数：0x004B5660。
static void WindowGroupSelect_BuildRecipients(std::int32_t mode); // 稀疏名单生产，不按count压紧槽位。

// 原函数：0x004B5760。
static void WindowGroupSelect_SortRecipients(std::int32_t category); // 原CRT次序排序60或200个S32索引。

// 原函数：0x004B57B0。
static std::int32_t WindowGroupSelect_CompareRecipients(const std::int32_t* left, const std::int32_t* right);

// 原函数：0x004B4B00。
static std::int32_t WindowGroupSelect_Update(std::int32_t value);

// 原函数：0x004B4B70。
static void WindowGroupSelect_Release();

// 原函数：0x004B4B90。
static std::int32_t WindowGroupSelect_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic572E20 {0x3BA15, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E20。

static WindowGraphicData gWindowGraphic572E38 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E38。

static WindowGraphicData gWindowGraphic572E50 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E50。

static WindowGraphicData gWindowGraphic572E68 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E68。

static WindowGraphicData gWindowGraphic572E80 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E80。

static WindowGraphicData gWindowGraphic572E98 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E98。

static WindowGraphicData gWindowGraphic572EB0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572EB0。

static WindowGraphicData gWindowGraphic572EC8 {0x3BA17, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572EC8。

static WindowGraphicData gWindowGraphic572EF8 {0x3BA16, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00572EF8。

static WindowGraphicData gWindowGraphic572F10 {0x3BA18, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F10。

static WindowGraphicData gWindowGraphic572F28 {0x3BA1E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F28。

static WindowGraphicData gWindowGraphic572F40 {0x3BA21, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F40。

static WindowTextPrototype gWindowText572F70 { // 原全局：0x00572F70。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"人员",
};

// 原全局：0x00572F58。 49三类副标题，元素原型与刷新同源。
static WindowTextPrototype gWindowText572F58 { // 原全局：0x00572F58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"持有名片＆上线的人员",
};

static WindowTextPrototype gWindowText572F60 { // 原全局：0x00572F60。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"同家族内上线的人员",
};

static WindowTextPrototype gWindowText572F68 { // 原全局：0x00572F68。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"同家族全部的人员",
};

// 原全局：0x00572F78。 49三类标题。
static WindowTextPrototype gWindowText572F78 { // 原全局：0x00572F78。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"交换过名片的人",
};

static WindowTextPrototype gWindowText572F80 { // 原全局：0x00572F80。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"家族成员",
};

static WindowTextPrototype gWindowText572F88 { // 原全局：0x00572F88。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"家族称号",
};

static WindowElementPrototype gWindowElements572F90[37] { // 原全局：0x00572F90。
    {WindowElementType_Graphic, 239, 9, 11, 11, 1, {.graphic = &gWindowGraphic572E38}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 239, 88, 11, 11, 1, {.graphic = &gWindowGraphic572E50}, &WindowGroupSelect_HandleScrollButtons},
    {WindowElementType_Graphic, 239, 275, 11, 11, 1, {.graphic = &gWindowGraphic572E80}, &WindowGroupSelect_HandleScrollButtons},
    {WindowElementType_Graphic, 239, 99, 10, 14, 1, {.graphic = &gWindowGraphic572E68}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 99, 10, 177, 1, {.graphic = &gWindowGraphicD678E0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_EventArea, 0, 0, 280, 317, 1, {}, &WindowGroupSelect_Scroll},
    {WindowElementType_Graphic, 20, 288, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_HandlePageButtons},
    {WindowElementType_Graphic, 60, 288, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_HandlePageButtons},
    {WindowElementType_Graphic, 111, 30, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_TabCategory},
    {WindowElementType_Graphic, 218, 30, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_TabCategory},
    {WindowElementType_Text, 131, 33, 0, 0, 1, {.text = &gWindowText572F78}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 57, 0, 0, 1, {.text = &gWindowText572F58}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 93, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 122, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 151, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 180, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 209, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 238, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 267, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 55, 19, 19, 1, {.graphic = &gWindowGraphic572F28}, &WindowGroupSelect_ButtonSelectAll},
    {WindowElementType_Graphic, 214, 91, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 120, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 149, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 178, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 207, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 236, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 265, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 16, 50, 0, 0, 1, {.graphic = &gWindowGraphic572F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 86, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 115, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 144, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 173, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 202, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 231, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 260, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572E20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 10, 25, 0, 0, 1, {.graphic = &gWindowGraphic572EF8}, &WindowElement_NoOp},
};

// 原型：窗口49；原全局：0x0054EC00。
WindowPrototype gWindowPrototypeGroupSelect {0, 4, 68, 84, 280, 317, 0x80010101u, 37, gWindowElements572F90, &WindowGroupSelect_Update, &WindowGroupSelect_Draw, &WindowGroupSelect_Release}; // 49: 54EC00

// 窗口函数实现。

// 原函数：0x004B4B00。
static std::int32_t WindowGroupSelect_Update(const std::int32_t value)
{
    if (value == -1) {
        if (gLoginWindowRuntime.mailGroupCC1A30.initialize == 0) {
            gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
            WindowGroupSelect_ResetRecipients();
            gLoginWindowRuntime.mailGroupCC1A30.mode = 0;
            WindowGroupSelect_BuildRecipients(0);
            gCurrentWindow->elements[19].visible = 1;
        }
        gLoginWindowRuntime.mailGroupCC1A30.initialize = 0;
        return 1;
    }
    if (gWindowStates[WindowId_MailCompose].window == nullptr) {
        gWindowStates[WindowId_GroupSelect].window->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004B4B70。
static void WindowGroupSelect_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_MailCompose].window;
    if (window != nullptr && gLoginWindowRuntime.prefixD2F0B8.value10 == 1) {
        window->state |= 0x8000u;
    }
}

// 原函数：0x004B4B90。
static std::int32_t WindowGroupSelect_Draw(const std::int32_t value)
{
    static_cast<void>(WindowGroupSelect_DragScroll(value));
    WindowGroupSelect_Refresh();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004B4BB0。
static std::int32_t WindowGroupSelect_HandlePageButtons(const std::int32_t index, const std::int32_t state)
{
    if (index != 6 && index != 7) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t idle = index == 6 ? 0x3B556 : 0x3B559;
    graphic->imageNo = idle;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        result = 1;
    }
    if ((state & 2) != 0) {
        graphic->imageNo = idle + 1;
        const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position + (index == 6 ? -7 : 7);
        static_cast<void>(WindowGroupSelect_ClampScroll());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if ((state & 8) != 0) {
        graphic->imageNo = idle + 1;
    }
    return result;
}

// 原函数：0x004B4CA0。
static std::int32_t WindowGroupSelect_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    if (index != 1 && index != 2) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t idle = index == 1 ? 0x3B592 : 0x3B595;
    graphic->imageNo = idle;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        result = 1;
    }
    if ((state & 0x400) != 0) {
        graphic->imageNo = idle + 1;
        std::int32_t sound = 51;
        if (index == 1) {
            const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88 - 1;
            gLoginWindowRuntime.mailGroupScrollD67B88 = position;
            if (position < 0) {
                gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
                sound = 55;
            }
        } else {
            const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
            const std::int32_t maximum = gLoginWindowRuntime.mailGroupCC1A30.count - 7;
            gLoginWindowRuntime.mailGroupScrollD67B88 = position + 1;
            if (position + 1 > maximum) {
                gLoginWindowRuntime.mailGroupScrollD67B88 = maximum;
                sound = 55;
            }
        }
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
        static_cast<void>(WindowGroupSelect_ClampScroll());
        result = 1;
    }
    return result;
}

// 原函数：0x004B4DB0。
static std::int32_t WindowGroupSelect_ClampScroll()
{
    std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
    if (position < 0) {
        position = 0;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    }
    const std::int32_t maximum = gLoginWindowRuntime.mailGroupCC1A30.count - 7;
    if (position > maximum) {
        position = maximum;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    }
    return WindowScroll_SetPosition(&gCurrentWindow->elements[4], maximum, position);
}

// 原函数：0x004B4DF0。
static std::int32_t WindowGroupSelect_DragScroll(const std::int32_t)
{
    const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t maximum = count - 7;
    const std::int32_t position = WindowScroll_ReadPosition(&window->elements[4], maximum < 0 ? 0 : maximum);
    gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    return position;
}

// 原函数：0x004B4E20。
static void WindowGroupSelect_Refresh()
{
    const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
    if (mode != 0 && mode != 1 && mode != 2) {
        return;
    }
    WindowInstance* const titleWindow = gCurrentWindow;
    const std::wstring& title = mode == 0 ? gWindowText572F78.text
        : mode == 1 ? gWindowText572F80.text : gWindowText572F88.text;
    auto* const titleData = static_cast<WindowTextData*>(titleWindow->elements[10].data);
    titleData->text = title;
    WindowInstance* const subtitleWindow = gCurrentWindow;
    const std::wstring& subtitle = mode == 0 ? gWindowText572F58.text
        : mode == 1 ? gWindowText572F60.text : gWindowText572F68.text;
    auto* const subtitleData = static_cast<WindowTextData*>(subtitleWindow->elements[11].data);
    subtitleData->text = subtitle;
    for (std::int32_t row = 0; row < 7; ++row) {
        const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
        const std::int32_t position = scroll + row;
        const std::int32_t source = gLoginWindowRuntime.mailGroupCC1A30.indices[position];
        const std::int32_t visible = source != -1 ? 1 : 0;
        if (visible != 0) {
            WindowInstance* const window = gCurrentWindow;
            const std::wstring name = mode == 0 ? world::detail::gContactsF19FB0[source].name10
                : cg::text::FromGbkWide(mode == 1 ? cg::world::gOrderedRecords1051B14[source].text18
                    : cg::world::GetGuildRankText(position));
            auto* const text = static_cast<WindowTextData*>(window->elements[12 + row].data);
            text->text = name;
        }
        gCurrentWindow->elements[12 + row].visible = visible;
        gCurrentWindow->elements[20 + row].visible = visible;
        gCurrentWindow->elements[28 + row].visible = visible;
    }
}

// 原函数：0x004B5210。
static std::int32_t WindowGroupSelect_TabCategory(const std::int32_t index, const std::int32_t state)
{
    if (index != 8 && index != 9) {
        return 0;
    }
    const std::int32_t idle = index == 8 ? 0x3B556 : 0x3B559;
    std::int32_t result = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        gHoverTextD68AE8 = L"切换集团邮件。";
        result = 1;
    } else {
        graphic->imageNo = idle;
    }
    if ((state & 2) != 0) {
        if (index == 8) {
            const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode - 1;
            gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            if (mode < 0) {
                gLoginWindowRuntime.mailGroupCC1A30.mode = 2;
            }
            WindowGroupSelect_BuildRecipients(gLoginWindowRuntime.mailGroupCC1A30.mode);
        } else {
            std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode + 1;
            gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            if (mode > 2) {
                mode = 0;
                gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            }
            WindowGroupSelect_BuildRecipients(mode);
        }
        gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        result = 1;
    }
    if ((state & 8) != 0) {
        auto* const current = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        current->imageNo = idle + 1;
    }
    return result;
}

// 原函数：0x004B53C0。
static std::int32_t WindowGroupSelect_ButtonSelectAll(const std::int32_t index, const std::int32_t state)
{
    std::int32_t result = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BA20;
        const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (mode == 0 || mode == 1 || mode == 2) {
            gHoverTextD68AE8 = L"准备寄送邮件给此集团。";
        }
        result = 1;
    } else {
        graphic->imageNo = 0x3BA1E;
    }
    if ((state & 2) != 0) {
        const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (mode == 0 || mode == 1 || mode == 2) {
            std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
            for (std::int32_t row = 0; row < count; ++row) {
                const std::int32_t source = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                if (source != -1) {
                    if (mode == 2) {
                        gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                    } else if (mode == 1) {
                        if (cg::world::gOrderedRecords1051B14[source].value0A != 0) {
                            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                        }
                    } else {
                        // 原普通分支查CC2E6C排序表，不用CC1A38名单值取联系人。
                        const std::int32_t ordered = gLoginWindowRuntime.mailGroupOrderCC2E6C[row];
                        if (world::detail::gContactsF19FB0[ordered].value02 != 0) {
                            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                        }
                    }
                }
                count = gLoginWindowRuntime.mailGroupCC1A30.count;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        auto* const current = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        current->imageNo = 0x3BA1F;
    }
    return result;
}

// 原函数：0x004B5550。
static std::int32_t WindowGroupSelect_SelectRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
    const std::int32_t selected = gLoginWindowRuntime.mailGroupCC1A30.selected[scroll + index - 20];
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = selected == 1 ? 0x3BA21 : 0x3BA22;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = selected == 1
            ? L"对方的邮件寄过来了。"
            : L"准备寄送邮件给对方。";
        result = 1;
    }
    if ((state & 2) != 0) {
        const std::int32_t currentScroll = gLoginWindowRuntime.mailGroupScrollD67B88;
        const std::int32_t row = currentScroll + index - 20;
        const std::int32_t current = gLoginWindowRuntime.mailGroupCC1A30.selected[row];
        if (current != 0) {
            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 0;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return result;
}

// 原函数：0x004B5A40。
static std::int32_t WindowGroupSelect_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t maximum = count - 7;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
        gLoginWindowRuntime.mailGroupScrollD67B88 =
            WindowScroll_ApplyWheel(&window->elements[4], position, maximum < 0 ? 0 : maximum, wheel);
    }
    return 0;
}

// 原函数：0x004B5630。
static void WindowGroupSelect_ResetRecipients()
{
    gLoginWindowRuntime.mailGroupCC1A30.count = 0;
    gLoginWindowRuntime.mailGroupCC1A30.mode = 0;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupCC1A30.indices[index] = 0;
        gLoginWindowRuntime.mailGroupCC1A30.selected[index] = 0;
    }
}

// 原函数：0x004B5660。
static void WindowGroupSelect_BuildRecipients(const std::int32_t mode)
{
    gLoginWindowRuntime.mailGroupCC1A30.count = 0;
    gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupCC1A30.indices[index] = -1;
        gLoginWindowRuntime.mailGroupCC1A30.selected[index] = 0;
    }
    switch (mode) {
    case 0:
        WindowGroupSelect_SortRecipients(0);
        for (std::int32_t index = 0; index < 60; ++index) {
            const std::int32_t source = gLoginWindowRuntime.mailGroupOrderCC2E6C[index];
            if (world::detail::gContactsF19FB0[source].used == 1) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = source;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    case 1:
        WindowGroupSelect_SortRecipients(1);
        for (std::int32_t index = 0; index < 200; ++index) {
            const std::int32_t source = gLoginWindowRuntime.mailGroupOrderCC2E6C[index];
            if (cg::world::gOrderedRecords1051B14[source].value08 == 1) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = source;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    case 2:
        for (std::int32_t index = 0; index < 32; ++index) {
            if (cg::world::ReadGuildRankFlags1051514(index) != 0) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = index;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    }
}

// 原函数：0x004B5760。
static void WindowGroupSelect_SortRecipients(const std::int32_t category)
{
    gLoginWindowRuntime.mailGroupSortCategoryCB0600 = category;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupOrderCC2E6C[index] = index;
    }
    WindowMail_SortIndices(gLoginWindowRuntime.mailGroupOrderCC2E6C.data(),
        category == 0 ? 60u : 200u, WindowGroupSelect_CompareRecipients);
}

// 原函数：0x004B57B0。
static std::int32_t WindowGroupSelect_CompareRecipients(const std::int32_t* const left, const std::int32_t* const right)
{
    const std::int32_t category = gLoginWindowRuntime.mailGroupSortCategoryCB0600;
    const std::int32_t rightIndex = *right;
    const std::int32_t leftIndex = *left;
    const bool leftUsed = category == 0
        ? world::detail::gContactsF19FB0[leftIndex].used != 0
        : cg::world::gOrderedRecords1051B14[leftIndex].value08 != 0;
    const bool rightUsed = category == 0
        ? world::detail::gContactsF19FB0[rightIndex].used != 0
        : cg::world::gOrderedRecords1051B14[rightIndex].value08 != 0;
    if (leftUsed != rightUsed) {
        return leftUsed ? -1 : 1;
    }
    if (!leftUsed) {
        return 0;
    }
    const std::uint32_t mode = platform::gContactSortMode_D67CD0;
    if (mode > 4) {
        return 0;
    }
    if (category == 0) {
        const auto* const other = &world::detail::gContactsF19FB0[rightIndex];
        const auto* const current = &world::detail::gContactsF19FB0[leftIndex];
        switch (mode) {
        case 0: return WindowContacts_CompareContactName(current, other);
        case 1: return WindowContacts_CompareContactTitle(current, other);
        case 2: return WindowContacts_CompareContactValue(current, other);
        case 3: return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4: return WindowContacts_CompareContactNumber(current, other);
        }
    } else {
        const auto* const other = &cg::world::gOrderedRecords1051B14[rightIndex];
        const auto* const current = &cg::world::gOrderedRecords1051B14[leftIndex];
        switch (mode) {
        case 0: return WindowContacts_CompareGuildName(current, other);
        case 1: return WindowContacts_CompareGuildValue(current, other);
        case 2: return WindowContacts_CompareGuildStatus(current, other);
        case 3: return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4: return WindowContacts_CompareGuildNumber(current, other);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
