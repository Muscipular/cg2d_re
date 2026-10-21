#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowGroupSelectElementId : ElementId {
    ButtonClose = 1,
    HandleScrollButtons = 2,
    HandleScrollButtons_3 = 3,
    Element4 = 4,
    HandleVerticalScroll = 5,
    Scroll = 6,
    HandlePageButtons = 7,
    HandlePageButtons_8 = 8,
    TabCategory = 9,
    TabCategory_10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    ButtonSelectAll = 20,
    SelectRow = 21,
    SelectRow_22 = 22,
    SelectRow_23 = 23,
    SelectRow_24 = 24,
    SelectRow_25 = 25,
    SelectRow_26 = 26,
    SelectRow_27 = 27,
    Element28 = 28,
    Element29 = 29,
    Element30 = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    Element36 = 36,
    Element37 = 37,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004B4B00。
static std::int32_t WindowGroupSelect_Update(std::int32_t value); // 群组选人初始化/恢复与父窗门禁。

// 原函数：0x004B4B70。
static void WindowGroupSelect_Release(); // 释放时仅关闭仍处于群发方式的22。

// 原函数：0x004B4B90。
static std::int32_t WindowGroupSelect_Draw(std::int32_t value); // 滚动反求、七行刷新、公共绘制。

// 原函数：0x004B4BB0。
static std::int32_t WindowGroupSelect_HandlePageButtons(ElementId elementId, std::int32_t state); // 前后翻七行，仅hover消费。

// 原函数：0x004B4CA0。
static std::int32_t WindowGroupSelect_HandleScrollButtons(ElementId elementId, std::int32_t state); // bit400单行滚动，发声后修正位置。

// 原函数：0x004B4DB0。
static std::int32_t WindowGroupSelect_ClampScroll(); // 顺序修正上下界，保留count<7时负上界。

// 原函数：0x004B4DF0。
static std::int32_t WindowGroupSelect_DragScroll(std::int32_t unused); // 非负最大值反求并发布滚动位置。

// 原函数：0x004B4E20。
static void WindowGroupSelect_Refresh(); // 三种标题和七行显隐，不改选择标志。

// 原函数：0x004B5210。
static std::int32_t WindowGroupSelect_TabCategory(ElementId elementId, std::int32_t state); // 切换三模式后清滚动位置。

// 原函数：0x004B53C0。
static std::int32_t WindowGroupSelect_ButtonSelectAll(ElementId elementId, std::int32_t state); // 全组选择，普通联系人借原排序索引。

// 原函数：0x004B5550。
static std::int32_t WindowGroupSelect_SelectRow(ElementId elementId, std::int32_t state); // 七行选择，点击时重读滚动位置。

// 原函数：0x004B5A40。
static std::int32_t WindowGroupSelect_Scroll(ElementId elementId, std::int32_t state); // 零wheel仍调用4743B0。

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
    {WindowElementType_Graphic, 239, 9, 11, 11, 1, {.graphic = &gWindowGraphic572E38}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowGroupSelectElementId::ButtonClose)},
    {WindowElementType_Graphic, 239, 88, 11, 11, 1, {.graphic = &gWindowGraphic572E50}, &WindowGroupSelect_HandleScrollButtons, -1, static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons)},
    {WindowElementType_Graphic, 239, 275, 11, 11, 1, {.graphic = &gWindowGraphic572E80}, &WindowGroupSelect_HandleScrollButtons, -1, static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons_3)},
    {WindowElementType_Graphic, 239, 99, 10, 14, 1, {.graphic = &gWindowGraphic572E68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element4)},
    {WindowElementType_Graphic, 239, 99, 10, 177, 1, {.graphic = &gWindowGraphicD678E0}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowGroupSelectElementId::HandleVerticalScroll)},
    {WindowElementType_EventArea, 0, 0, 280, 317, 1, {}, &WindowGroupSelect_Scroll, -1, static_cast<ElementId>(WindowGroupSelectElementId::Scroll)},
    {WindowElementType_Graphic, 20, 288, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_HandlePageButtons, -1, static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons)},
    {WindowElementType_Graphic, 60, 288, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_HandlePageButtons, -1, static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons_8)},
    {WindowElementType_Graphic, 111, 30, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_TabCategory, -1, static_cast<ElementId>(WindowGroupSelectElementId::TabCategory)},
    {WindowElementType_Graphic, 218, 30, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_TabCategory, -1, static_cast<ElementId>(WindowGroupSelectElementId::TabCategory_10)},
    {WindowElementType_Text, 131, 33, 0, 0, 1, {.text = &gWindowText572F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element11)},
    {WindowElementType_Text, 23, 57, 0, 0, 1, {.text = &gWindowText572F58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element12)},
    {WindowElementType_Text, 23, 93, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element13)},
    {WindowElementType_Text, 23, 122, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element14)},
    {WindowElementType_Text, 23, 151, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element15)},
    {WindowElementType_Text, 23, 180, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element16)},
    {WindowElementType_Text, 23, 209, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element17)},
    {WindowElementType_Text, 23, 238, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element18)},
    {WindowElementType_Text, 23, 267, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element19)},
    {WindowElementType_Graphic, 214, 55, 19, 19, 1, {.graphic = &gWindowGraphic572F28}, &WindowGroupSelect_ButtonSelectAll, -1, static_cast<ElementId>(WindowGroupSelectElementId::ButtonSelectAll)},
    {WindowElementType_Graphic, 214, 91, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow)},
    {WindowElementType_Graphic, 214, 120, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_22)},
    {WindowElementType_Graphic, 214, 149, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_23)},
    {WindowElementType_Graphic, 214, 178, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_24)},
    {WindowElementType_Graphic, 214, 207, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_25)},
    {WindowElementType_Graphic, 214, 236, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_26)},
    {WindowElementType_Graphic, 214, 265, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow, -1, static_cast<ElementId>(WindowGroupSelectElementId::SelectRow_27)},
    {WindowElementType_Graphic, 16, 50, 0, 0, 1, {.graphic = &gWindowGraphic572F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element28)},
    {WindowElementType_Graphic, 16, 86, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element29)},
    {WindowElementType_Graphic, 16, 115, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element30)},
    {WindowElementType_Graphic, 16, 144, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element31)},
    {WindowElementType_Graphic, 16, 173, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element32)},
    {WindowElementType_Graphic, 16, 202, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element33)},
    {WindowElementType_Graphic, 16, 231, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element34)},
    {WindowElementType_Graphic, 16, 260, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element35)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572E20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element36)},
    {WindowElementType_Graphic, 10, 25, 0, 0, 1, {.graphic = &gWindowGraphic572EF8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGroupSelectElementId::Element37)},
};

// 原型：窗口49；原全局：0x0054EC00。
WindowPrototype gWindowPrototypeGroupSelect {0, 4, 68, 84, 280, 317, 0x80010101u, 37, gWindowElements572F90, &WindowGroupSelect_Update, &WindowGroupSelect_Draw, &WindowGroupSelect_Release}; // 49: 54EC00

// 窗口函数实现。

// 原函数：0x004B4B00。
static std::int32_t WindowGroupSelect_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        if (gLoginWindowRuntime.mailGroupCC1A30.initialize == 0) {
            gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
            WindowGroupSelect_ResetRecipients();
            gLoginWindowRuntime.mailGroupCC1A30.mode = 0;
            WindowGroupSelect_BuildRecipients(0);
            if (auto* const buttonSelectAllElement = currentWindow->FindElementById(WindowGroupSelectElementId::ButtonSelectAll); buttonSelectAllElement) buttonSelectAllElement->visible = 1;
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
static std::int32_t WindowGroupSelect_HandlePageButtons(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (elementId != static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons) && elementId != static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons_8)) {
        return 0;
    }
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t idle = elementId == static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons) ? 0x3B556 : 0x3B559;
        graphic->imageNo = idle;
        std::int32_t result = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = idle + 2;
            result = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            graphic->imageNo = idle + 1;
            const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
            gLoginWindowRuntime.mailGroupScrollD67B88 = position + (elementId == static_cast<ElementId>(WindowGroupSelectElementId::HandlePageButtons) ? -7 : 7);
            static_cast<void>(WindowGroupSelect_ClampScroll());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = idle + 1;
        }
        return result;
    }
    return 0;
}

// 原函数：0x004B4CA0。
static std::int32_t WindowGroupSelect_HandleScrollButtons(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (elementId != static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons) && elementId != static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons_3)) {
        return 0;
    }
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t idle = elementId == static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons) ? 0x3B592 : 0x3B595;
        graphic->imageNo = idle;
        std::int32_t result = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = idle + 2;
            result = 1;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            graphic->imageNo = idle + 1;
            std::int32_t sound = 51;
            if (elementId == static_cast<ElementId>(WindowGroupSelectElementId::HandleScrollButtons)) {
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
    return 0;
}

// 原函数：0x004B4DB0。
static std::int32_t WindowGroupSelect_ClampScroll()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
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
    auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowGroupSelectElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        return WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position);
    }
    return 0;
}

// 原函数：0x004B4DF0。
static std::int32_t WindowGroupSelect_DragScroll(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
    WindowInstance* const window = currentWindow;
    const std::int32_t maximum = count - 7;
    auto* const handleVerticalScrollElement = window->FindElementById(WindowGroupSelectElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        const std::int32_t position = WindowScroll_ReadPosition(handleVerticalScrollElement, maximum < 0 ? 0 : maximum);
        gLoginWindowRuntime.mailGroupScrollD67B88 = position;
        return position;
    }
    return 0;
}

// 原函数：0x004B4E20。
static void WindowGroupSelect_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
    if (mode != 0 && mode != 1 && mode != 2) {
        return;
    }
    WindowInstance* const titleWindow = currentWindow;
    const std::wstring& title = mode == 0 ? gWindowText572F78.text
        : mode == 1 ? gWindowText572F80.text : gWindowText572F88.text;
    auto* const element11 = titleWindow->FindElementById(WindowGroupSelectElementId::Element11);
    if (element11) {
        auto* const titleData = element11->data.text;
        titleData->text = title;
    }
    WindowInstance* const subtitleWindow = currentWindow;
    const std::wstring& subtitle = mode == 0 ? gWindowText572F58.text
        : mode == 1 ? gWindowText572F60.text : gWindowText572F68.text;
    auto* const element12 = subtitleWindow->FindElementById(WindowGroupSelectElementId::Element12);
    if (element12) {
        auto* const subtitleData = element12->data.text;
        subtitleData->text = subtitle;
    }
    for (std::int32_t row = 0; row < 7; ++row) {
        const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
        const std::int32_t position = scroll + row;
        const std::int32_t source = gLoginWindowRuntime.mailGroupCC1A30.indices[position];
        const std::int32_t visible = source != -1 ? 1 : 0;
        if (visible != 0) {
            WindowInstance* const window = currentWindow;
            const std::wstring name = mode == 0 ? world::detail::gContactsF19FB0[source].name10
                : mode == 1 ? cg::world::gOrderedRecords1051B14[source].name18
                    : cg::world::GetGuildRankText(position);
            auto* const textElement = window->FindElementById(NativeElementKey(12 + row));
            if (textElement) {
                auto* const text = textElement->data.text;
                text->text = name;
            }
        }
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(12 + row)); indexedElement) indexedElement->visible = visible;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(20 + row)); indexedElement2) indexedElement2->visible = visible;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(28 + row)); indexedElement3) indexedElement3->visible = visible;
    }
}

// 原函数：0x004B5210。
static std::int32_t WindowGroupSelect_TabCategory(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (elementId != static_cast<ElementId>(WindowGroupSelectElementId::TabCategory) && elementId != static_cast<ElementId>(WindowGroupSelectElementId::TabCategory_10)) {
        return 0;
    }
    const std::int32_t idle = elementId == static_cast<ElementId>(WindowGroupSelectElementId::TabCategory) ? 0x3B556 : 0x3B559;
    std::int32_t result = 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = idle + 2;
            gHoverTextD68AE8 = L"切换集团邮件。";
            result = 1;
        } else {
            graphic->imageNo = idle;
        }
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (elementId == static_cast<ElementId>(WindowGroupSelectElementId::TabCategory)) {
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
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        auto* const currentElement = currentWindow->FindElementById(elementId);
        if (currentElement) {
            auto* const current = currentElement->data.graphic;
            current->imageNo = idle + 1;
        }
    }
    return result;
}

// 原函数：0x004B53C0。
static std::int32_t WindowGroupSelect_ButtonSelectAll(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t result = 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3BA20;
            const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
            if (mode == 0 || mode == 1 || mode == 2) {
                gHoverTextD68AE8 = L"准备寄送邮件给此集团。";
            }
            result = 1;
        } else {
            graphic->imageNo = 0x3BA1E;
        }
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
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
        auto* const currentElement = currentWindow->FindElementById(elementId);
        if (currentElement) {
            auto* const current = currentElement->data.graphic;
            current->imageNo = 0x3BA1F;
        }
    }
    return result;
}

// 原函数：0x004B5550。
static std::int32_t WindowGroupSelect_SelectRow(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
    const std::int32_t selected = gLoginWindowRuntime.mailGroupCC1A30.selected[scroll + index - 20];
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = selected == 1 ? 0x3BA21 : 0x3BA22;
    }
    std::int32_t result = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = selected == 1
            ? L"对方的邮件寄过来了。"
            : L"准备寄送邮件给对方。";
        result = 1;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
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
static std::int32_t WindowGroupSelect_Scroll(const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t maximum = count - 7;
        WindowInstance* const window = currentWindow;
        const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
        auto* const handleVerticalScrollElement = window->FindElementById(WindowGroupSelectElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            gLoginWindowRuntime.mailGroupScrollD67B88 =
                WindowScroll_ApplyWheel(handleVerticalScrollElement, position, maximum < 0 ? 0 : maximum, wheel);
        }
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
