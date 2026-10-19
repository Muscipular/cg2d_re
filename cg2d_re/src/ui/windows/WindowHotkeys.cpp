#include "../../scene/login/LoginSavedState.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContexts5F9758[24] {}; // 原全局：0x005F9758。


// 回调与内部函数前置声明。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(std::int32_t value); // 窗口25初始化，按分类绑定十行回调。

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(std::int32_t value); // 窗口25说明/预设文本绘制及编辑焦点收尾。

// 原函数：0x004953B0。
static std::int32_t WindowHotkeys_ButtonClose(std::int32_t index, std::int32_t state); // 关闭并写回预设文本，不直接持久化。

// 原函数：0x00495450。
static std::int32_t WindowHotkeys_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行，bit400。

// 原函数：0x00495500。
static std::int32_t WindowHotkeys_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行，音效后重新取计数。

// 原函数：0x004955C0。
static std::int32_t WindowHotkeys_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上十行，音效先于位置修改。

// 原函数：0x00495670。
static std::int32_t WindowHotkeys_ButtonNextPage(std::int32_t index, std::int32_t state); // 下十行，保留三阶段计数读取。

// 原函数：0x00495750。
static std::int32_t WindowHotkeys_Scroll(std::int32_t index, std::int32_t state); // 窗口25滚轮，恒返0。

// 原函数：0x004957B0。
static std::int32_t WindowHotkeys_TabFirstCategory(std::int32_t index, std::int32_t state); // 分类0与十行复位。

// 原函数：0x004958D0。
static std::int32_t WindowHotkeys_TabSecondCategory(std::int32_t index, std::int32_t state); // 分类1与十行复位。

// 原函数：0x004959F0。
static std::int32_t WindowHotkeys_TabThirdCategory(std::int32_t index, std::int32_t state); // 分类2与十行复位。

// 原函数：0x00495B10。
static std::int32_t WindowHotkeys_TabPresetText(std::int32_t index, std::int32_t state); // 分类3只换回调，不复位行图号。

// 原函数：0x00495C20。
static std::int32_t WindowHotkeys_EditPresetText(std::int32_t index, std::int32_t state); // 预设文本备份、初始化及同源焦点。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(std::int32_t value);

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00CC4B20。 S8选中记录，495130初始化为-1。
static std::int8_t gHotkeySelectedCC4B20 = 0; // 原全局：0x00CC4B20。

// 原全局：0x0057DF48。 借24项InputContext中的当前编辑项。
static WindowInputPrototype gHotkeyInputPrototype57DF48 { // 原全局：0x0057DF48。
    {{nullptr}, WindowInputReferenceKind::Context},
    0, 0, 3, 8, 0, {}, 1, 24, 0, 0,
};

static constexpr std::array<std::int8_t, 8> kHotkeyCounts56AEC4 {14, 27, 16, 24, 14, 27, 16, 24}; // 原常量参考：0x0056AEC4。

static constexpr const wchar_t* kHotkeyText56933C[14] { // 原常量参考：0x0056933C。
    L"开启/关闭人物状态视窗　　　　Ctrl + W",
    L"开启/关闭人物技能视窗　　　　Ctrl + E",
    L"开启/关闭物品视窗　　　　　　Ctrl + R",
    L"开启/关闭宠物状态视窗　　　　Ctrl + A",
    L"开启/关闭名片视窗　　　　　　Ctrl + S",
    L"开启/关闭宠物图鉴视窗　　　　Ctrl + D",
    L"开启/关闭系统功能视窗　　　　ESC",
    L"开启/关闭情绪动作视窗　　　　Ctrl + F",
    L"开启/关闭小地图视窗　　　　　Ctrl + Q",
    L"开启/关闭热键一览视窗　　　　Ctrl + Y",
    L"开启/关闭黑名单列表视窗　　　End",
    L"开启/关闭冒险者日志视窗　　　F9",
    L"把移至画面范围外的视窗移回来 Ctrl + F12",
    L"关闭所有开启中的视窗　　　 　SHIFT + F12",
};

static constexpr const wchar_t* kHotkeyText569608[27] { // 原常量参考：0x00569608。
    L"允许/拒绝对战　　　　　　　　Ctrl + P",
    L"允许/拒绝组队交谈　　　　　　Ctrl + ;",
    L"允许/拒绝组队　　　　　　　　Ctrl + .",
    L"允许/拒绝交换名片　　　　　　Ctrl + L",
    L"允许/拒绝交易　　　　　　　　Ctrl + ,",
    L"允许/拒绝被邀请入家族　　　　Ctrl + N",
    L"提出对战要求　　　　　　　　 Ctrl + K",
    L"观战　　　　　　　　　　　　 Ctrl + V",
    L"加入冒险队伍　　　　　　　　 Ctrl + /",
    L"提出交换名片的要求　　　　　 Ctrl + [",
    L"提出交易的要求　　　　　　　 Ctrl + ]",
    L"邀请对方加入家族　　　　　　 Ctrl + M",
    L"更改文字大小　　　　　　　　 Ctrl + O",
    L"交谈视窗显示行数增加↑　　　 Ctrl + ↑",
    L"交谈视窗显示行数减少↓　　　 Ctrl + ↓",
    L"交谈视窗显示字数增加←　　　 Ctrl + ←",
    L"交谈视窗显示字数减少→　　　 Ctrl + →",
    L"扩大交谈范围　　　　　　　　 SHIFT + →",
    L"缩小交谈范围　　　　　　　　 SHIFT + ←",
    L"消除交谈显示文字　　　　　　 HOME",
    L"开启/关闭交谈视窗　　　　　　SHIFT + HOME",
    L"切换聊天显示模式　　　 　　　Ctrl + HOME",
    L"将滑鼠圈选的人加入黑名单中　 Ctrl+Insert",
    L"将滑鼠圈选的人从黑名单中移除 Ctrl+Delete",
    L"游戏画面撷取　　　　　　　　 F12",
    L"切换人物头上显示名称　　　　 F11",
    L"开启/关闭人物状态显示　　　　Ctrl + F11",
};

static constexpr const wchar_t* kHotkeyText569B6C[16] { // 原常量参考：0x00569B6C。
    L"坐下　　　　　　　　　　　　 Ctrl + 1",
    L"挥手 　　　　　　　　　　　　Ctrl + 2",
    L"点头 　　　　　　　　　　　　Ctrl + 3",
    L"高兴 　　　　　　　　　　　　Ctrl + 4",
    L"愤怒 　　　　　　　　　　　　Ctrl + 5",
    L"悲伤 　　　　　　　　　　　　Ctrl + 6",
    L"投掷 　　　　　　　　　　　　Ctrl + 7",
    L"站立 　　　　　　　　　　　　Ctrl + 8",
    L"走路 　　　　　　　　　　　　Ctrl + 9",
    L"倒下 　　　　　　　　　　　　Ctrl + 0",
    L"跑步 　　　　　　　　　　　　Ctrl + T",
    L"攻击 　　　　　　　　　　　　Ctrl + -",
    L"防御 　　　　　　　　　　　　Ctrl + H",
    L"受伤 　　　　　　　　　　　　Ctrl + G",
    L"魔法 　　　　　　　　　　　　Ctrl + B",
    LR"(猜拳 　　　　　　　　　　　　Ctrl + \)",
};

static constexpr const wchar_t* kHotkeyText569E9C[14] { // 原常量参考：0x00569E9C。
    L"开启/关闭人物状态视窗　　　　Ctrl + Q",
    L"开启/关闭人物技能视窗　　　　Ctrl + W",
    L"开启/关闭物品视窗　　　　　　Ctrl + E",
    L"开启/关闭宠物状态视窗　　　　Ctrl + R",
    L"开启/关闭名片视窗　　　　　　Ctrl + T",
    L"开启/关闭宠物图鉴视窗　　　　Ctrl + Y",
    L"开启/关闭系统功能视窗　　　　ESC",
    L"开启/关闭情绪动作视窗　　　　Ctrl + A",
    L"开启/关闭小地图视窗　　　　　Ctrl + S",
    L"开启/关闭热键一览视窗　　　　Ctrl + D",
    L"开启/关闭黑名单列表视窗　　　End",
    L"开启/关闭冒险者日志视窗　　　F9",
    L"把移至画面范围外的视窗移回来 Ctrl + F12",
    L"关闭所有开启中的视窗　　　　 SHIFT + F12",
};

static constexpr const wchar_t* kHotkeyText56A168[27] { // 原常量参考：0x0056A168。
    L"允许/拒绝对战　　　　　　　　Ctrl + U",
    L"允许/拒绝组队交谈　　　　　　Ctrl + I",
    L"允许/拒绝组队　　　　　　　　Ctrl + O",
    L"允许/拒绝交换名片　　　　　　Ctrl + P",
    L"允许/拒绝交易　　　　　　　　Ctrl + F",
    L"允许/拒绝被邀请入家族　　　　Ctrl + [",
    L"提出对战要求　　　　　　　　 Ctrl + J",
    L"观战　　　　　　　　　　　　 Ctrl + K",
    L"加入冒险队伍　　　　　　　　 Ctrl + L",
    L"提出交换名片的要求　　　　　 Ctrl + ;",
    LR"(提出交易的要求　　　　　　　 Ctrl + ")",
    L"邀请对方加入家族　　　　　　 Ctrl + ]",
    L"更改文字大小　　　　　　　　 Ctrl + Z",
    L"交谈视窗显示行数增加↑　　　 Ctrl + ↑",
    L"交谈视窗显示行数减少↓　　　 Ctrl + ↓",
    L"交谈视窗显示字数增加←　　　 Ctrl + ←",
    L"交谈视窗显示字数减少→　　　 Ctrl + →",
    L"扩大交谈范围　　　　　　　　 SHIFT + →",
    L"缩小交谈范围　　　　　　　　 SHIFT + ←",
    L"消除交谈显示文字　　　　　　 HOME",
    L"开启/关闭交谈视窗　　　　　　SHIFT + HOME",
    L"切换聊天显示模式　　　 　　　Ctrl + HOME",
    L"将滑鼠圈选的人加入黑名单中　 Ctrl+Insert",
    L"将滑鼠圈选的人从黑名单中移除 Ctrl+Delete",
    L"游戏画面撷取　　　　　　　　 F12",
    L"切换人物头上显示名称　　　　 F11",
    L"开启/关闭人物状态显示　　　　Ctrl + F11",
};

static constexpr const wchar_t* kHotkeyText56A6CC[16] { // 原常量参考：0x0056A6CC。
    L"坐下　　　　　　　　　　　　 Ctrl + 1",
    L"挥手　　　　　　　　　　　　 Ctrl + 2",
    L"点头　　　　　　　　　　　　 Ctrl + 3",
    L"高兴　　　　　　　　　　　　 Ctrl + 4",
    L"愤怒　　　　　　　　　　　　 Ctrl + 5",
    L"悲伤　　　　　　　　　　　　 Ctrl + 6",
    L"投掷　　　　　　　　　　　　 Ctrl + 7",
    L"站立　　　　　　　　　　　　 Ctrl + 8",
    L"走路　　　　　　　　　　　　 Ctrl + 9",
    L"倒下　　　　　　　　　　　　 Ctrl + B",
    L"跑步　　　　　　　　　　　　 Ctrl + N",
    L"攻击　　　　　　　　　　　　 Ctrl + M",
    L"防御　　　　　　　　　　　　 Ctrl + ,",
    L"受伤　　　　　　　　　　　　 Ctrl + .",
    L"魔法　　　　　　　　　　　　 Ctrl + /",
    LR"(猜拳　　　　　　　　　　　　 Ctrl + \)",
};

static constexpr const wchar_t* kHotkeyText56A9FC[24] { // 原常量参考：0x0056A9FC。
    L"F1　　　:",
    L"F2　　　:",
    L"F3　　　:",
    L"F4　　　:",
    L"F5　　　:",
    L"F6　　　:",
    L"F7　　　:",
    L"F8　　　:",
    L"Ctrl+F1 :",
    L"Ctrl+F2 :",
    L"Ctrl+F3 :",
    L"Ctrl+F4 :",
    L"Ctrl+F5 :",
    L"Ctrl+F6 :",
    L"Ctrl+F7 :",
    L"Ctrl+F8 :",
    L"Shift+F1:",
    L"Shift+F2:",
    L"Shift+F3:",
    L"Shift+F4:",
    L"Shift+F5:",
    L"Shift+F6:",
    L"Shift+F7:",
    L"Shift+F8:",
};

static constexpr std::array<const wchar_t* const*, 8> kHotkeyTextTables56AECC {
    kHotkeyText56933C, kHotkeyText569608, kHotkeyText569B6C, kHotkeyText56A9FC,
    kHotkeyText569E9C, kHotkeyText56A168, kHotkeyText56A6CC, kHotkeyText56A9FC,
};

static WindowGraphicData gWindowGraphic568E70 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568E70。

static WindowGraphicData gWindowGraphic568E88 {0x3B902, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568E88。

static WindowGraphicData gWindowGraphic568EA0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EA0。

static WindowGraphicData gWindowGraphic568EB8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EB8。

static WindowGraphicData gWindowGraphic568ED0 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568ED0。

static WindowGraphicData gWindowGraphic568EE8 {0x3B903, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EE8。

static WindowGraphicData gWindowGraphic568F00 {0x3B906, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F00。

static WindowGraphicData gWindowGraphic568F18 {0x3B909, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F18。

static WindowGraphicData gWindowGraphic568F30 {0x3B59C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F30。

static WindowGraphicData gWindowGraphic568F48 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F48。

static WindowGraphicData gWindowGraphic568F60 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F60。

static std::uint32_t gWindowMouseFlagsD677F0 = 0; // 原全局：0x00D677F0。

static WindowElementPrototype gWindowElements568F78[25] { // 原全局：0x00568F78。
    {WindowElementType_Input, 80, 41, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 265, 9, 11, 11, 1, {.graphic = &gWindowGraphic568E70}, &WindowHotkeys_ButtonClose},
    {WindowElementType_Graphic, 253, 68, 0, 14, 1, {.graphic = &gWindowGraphic568EA0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 254, 68, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD677F0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 253, 58, 11, 11, 1, {.graphic = &gWindowGraphic568EB8}, &WindowHotkeys_ButtonPreviousRow},
    {WindowElementType_Graphic, 253, 204, 11, 11, 1, {.graphic = &gWindowGraphic568ED0}, &WindowHotkeys_ButtonNextRow},
    {WindowElementType_Graphic, 227, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F48}, &WindowHotkeys_ButtonPreviousPage},
    {WindowElementType_Graphic, 245, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F60}, &WindowHotkeys_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 286, 233, 1, {.unusedValue = 0}, &WindowHotkeys_Scroll},
    {WindowElementType_Graphic, 25, 56, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 72, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 88, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 104, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 120, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 136, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 152, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 168, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 184, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 200, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 36, 49, 17, 1, {.graphic = &gWindowGraphic568EE8}, &WindowHotkeys_TabFirstCategory},
    {WindowElementType_Graphic, 75, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F00}, &WindowHotkeys_TabSecondCategory},
    {WindowElementType_Graphic, 125, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabThirdCategory},
    {WindowElementType_Graphic, 175, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabPresetText},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic568E88}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 288, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口25；原全局：0x0054E690。
WindowPrototype gWindowPrototypeHotkeys {0, 4, 15, 117, 286, 233, 0x80010101u, 25, gWindowElements568F78, &WindowHotkeys_Update, &WindowHotkeys_Draw, &WindowRuntime_ReleaseDefault}; // 25: 54E690

// 窗口函数实现。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const window = gCurrentWindow;
        gWindowStates[WindowId_Hotkeys].value14 = 0;
        WindowElement* const elements = window->elements;
        const std::int32_t height = elements[2].height;
        const std::int32_t y = elements[3].y;
        elements[2].y = y - (height >> 1);
        const std::uint32_t category = gWindowStates[WindowId_Hotkeys].value10;
        gHotkeySelectedCC4B20 = -1;
        const WindowElementCallback callback = category == 3 ? &WindowHotkeys_EditPresetText : &WindowElement_NoOp;
        for (std::int32_t index = 9; index <= 18; ++index) {
            gCurrentWindow->elements[index].mouseCallback = callback;
        }
    }
    return 1;
}

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(const std::int32_t)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const mouse = static_cast<WindowMouseData*>(elements[3].data);
    elements[2].visible = 1;
    const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
    const std::uint32_t category = gWindowStates[WindowId_Hotkeys].value10;
    const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
    if (maximum <= 0) {
        gCurrentWindow->elements[2].visible = 0;
    }
    if ((mouse->flags & 1) != 0) {
        const std::uint32_t currentCategory = gWindowStates[WindowId_Hotkeys].value10;
        const std::int32_t currentMode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t currentMaximum =
            kHotkeyCounts56AEC4[currentCategory + currentMode * 4] - 10;
        gWindowStates[WindowId_Hotkeys].value14 = static_cast<std::uint32_t>(
            WindowScroll_ReadPosition(&gCurrentWindow->elements[3], currentMaximum));
    }

    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int8_t marker = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(render::gHoverFramePaletteIndex_D69768));
        const std::uint32_t currentCategory = gWindowStates[WindowId_Hotkeys].value10;
        const std::int32_t currentMode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Hotkeys].value14);
        const wchar_t* const text = kHotkeyTextTables56AECC[currentCategory + currentMode * 4][scroll + row];
        WindowInstance* const window = gCurrentWindow;
        const std::int16_t y = static_cast<std::int16_t>(window->y);
        const std::int16_t x = static_cast<std::int16_t>(window->x);
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 31),
            static_cast<std::int16_t>(y + row * 16 + 58), 3, 8, 0, text, 0, marker));
    }

    if (gWindowStates[WindowId_Hotkeys].value10 == 3) {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Hotkeys].value14);
            const std::int8_t selected = gHotkeySelectedCC4B20;
            const std::int32_t recordIndex = scroll + row;
            if (recordIndex == selected) {
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(
                    &gHotkeyInputPrototype57DF48, x + 96, y + row * 16 + 58, 2));
            } else {
                const std::int8_t marker = std::bit_cast<std::int8_t>(
                    static_cast<std::uint8_t>(render::gHoverFramePaletteIndex_D69768));
                const std::wstring text = input::ContextText(cg::ui::detail::gInputContexts5F9758[recordIndex]);
                WindowInstance* const window = gCurrentWindow;
                const std::int16_t y = static_cast<std::int16_t>(window->y);
                const std::int16_t x = static_cast<std::int16_t>(window->x);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x + 96),
                    static_cast<std::int16_t>(y + row * 16 + 58), 3, 8, 0, text, 0, marker));
            }
        }
        const std::int8_t selected = gHotkeySelectedCC4B20;
        if (selected >= 0) {
            const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Hotkeys].value14);
            if (scroll > selected || selected >= scroll + 10) {
                static_cast<void>(WindowInput_ActivateElement(&gHotkeyInputPrototype57DF48, 0, -100, 2));
            }
        }
    } else {
        // 原比较上界是最后一项起点，不是整个数组末端。
        const auto currentInput = reinterpret_cast<std::uintptr_t>(input::gCurrentInputData);
        const auto firstInput = reinterpret_cast<std::uintptr_t>(&cg::ui::detail::gInputContexts5F9758[0]);
        const auto lastInput = reinterpret_cast<std::uintptr_t>(&cg::ui::detail::gInputContexts5F9758[23]);
        if (currentInput >= firstInput && currentInput <= lastInput) {
            WindowChat_RestoreInput();
            gHotkeySelectedCC4B20 = -1;
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004953B0。
static std::int32_t WindowHotkeys_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B538;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B53A;
        gHoverTextD68AE8 = kSettingsClose55331C;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    graphic->imageNo = 0x3B539;
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(login::detail::CapturePresetChatTexts_520640());
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// 原函数：0x00495450。
static std::int32_t WindowHotkeys_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        if (position > 0) {
            windowState->value14 = static_cast<std::uint32_t>(position - 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[3], maximum, current));
        consumed = 1;
    }
    graphic->imageNo = 0x3B592;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593;
    }
    return consumed;
}

// 原函数：0x00495500。
static std::int32_t WindowHotkeys_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        if (position < maximum) {
            windowState->value14 = static_cast<std::uint32_t>(position + 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t currentCategory = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t currentMode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t currentMaximum = kHotkeyCounts56AEC4[currentCategory + currentMode * 4] - 10;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[3], currentMaximum, current));
        consumed = 1;
    }
    graphic->imageNo = 0x3B595;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B597;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B596;
    }
    return consumed;
}

// 原函数：0x004955C0。
static std::int32_t WindowHotkeys_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        if (std::bit_cast<std::int32_t>(windowState->value14) > 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14) - 10;
        windowState->value14 = static_cast<std::uint32_t>(position);
        if (position < 0) {
            windowState->value14 = 0;
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[3], maximum, current));
        consumed = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00495670。
static std::int32_t WindowHotkeys_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        if (maximum > 0 && std::bit_cast<std::int32_t>(windowState->value14) < maximum) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14) + 10;
        windowState->value14 = static_cast<std::uint32_t>(position);
        const std::int32_t limitMode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t limitCategory = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t limit = kHotkeyCounts56AEC4[limitCategory + limitMode * 4] - 10;
        if (position > limit) {
            windowState->value14 = static_cast<std::uint32_t>(limit);
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t currentMode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t currentMaximum = kHotkeyCounts56AEC4[limitCategory + currentMode * 4] - 10;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[3], currentMaximum, current));
        consumed = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00495750。
static std::int32_t WindowHotkeys_Scroll(const std::int32_t index, const std::int32_t state)
{
    WindowState* const windowState = gCurrentWindowState;
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        windowState->value14 = static_cast<std::uint32_t>(
            WindowScroll_ApplyWheel(&gCurrentWindow->elements[index - 5], position, maximum, wheel));
    }
    return 0;
}

// 原函数：0x004957B0。
static std::int32_t WindowHotkeys_TabFirstCategory(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"显示游戏功能视窗相关热键。" /* 原常量参考：0x00586FA4。 */;
    }
    if ((state & 2) != 0) {
        if (windowState->value10 != 0) {
            windowState->value14 = 0;
            windowState->value10 = 0;
            WindowElement* const current = gCurrentWindow->elements;
            const std::int32_t height = current[2].height;
            const std::int32_t y = current[3].y;
            current[2].y = y - (height >> 1);
            for (std::int32_t row = 9; row <= 18; ++row) {
                gCurrentWindow->elements[row].mouseCallback = &WindowElement_NoOp;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[row].data)->imageNo = 0x3B59C;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B903;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B905;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B904;
    }
    if (windowState->value10 == 0) {
        graphic->imageNo = 0x3B904;
    }
    return consumed;
}

// 原函数：0x004958D0。
static std::int32_t WindowHotkeys_TabSecondCategory(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"显示画面左上角状态显示相关热键。" /* 原常量参考：0x00586FC0。 */;
    }
    if ((state & 2) != 0) {
        if (windowState->value10 != 1) {
            windowState->value14 = 0;
            windowState->value10 = 1;
            WindowElement* const current = gCurrentWindow->elements;
            const std::int32_t height = current[2].height;
            const std::int32_t y = current[3].y;
            current[2].y = y - (height >> 1);
            for (std::int32_t row = 9; row <= 18; ++row) {
                gCurrentWindow->elements[row].mouseCallback = &WindowElement_NoOp;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[row].data)->imageNo = 0x3B59C;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B906;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B908;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B907;
    }
    if (windowState->value10 == 1) {
        graphic->imageNo = 0x3B907;
    }
    return consumed;
}

// 原函数：0x004959F0。
static std::int32_t WindowHotkeys_TabThirdCategory(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"显示画面右上角情绪动作相关热键。" /* 原常量参考：0x00586FE4。 */;
    }
    if ((state & 2) != 0) {
        if (windowState->value10 != 2) {
            windowState->value14 = 0;
            windowState->value10 = 2;
            WindowElement* const current = gCurrentWindow->elements;
            const std::int32_t height = current[2].height;
            const std::int32_t y = current[3].y;
            current[2].y = y - (height >> 1);
            for (std::int32_t row = 9; row <= 18; ++row) {
                gCurrentWindow->elements[row].mouseCallback = &WindowElement_NoOp;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[row].data)->imageNo = 0x3B59C;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B909;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B90B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B90A;
    }
    if (windowState->value10 == 2) {
        graphic->imageNo = 0x3B90A;
    }
    return consumed;
}

// 原函数：0x00495B10。
static std::int32_t WindowHotkeys_TabPresetText(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    WindowState* const windowState = gCurrentWindowState;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"登录常用句。" /* 原常量参考：0x00587008。 */;
    }
    if ((state & 2) != 0) {
        if (windowState->value10 != 3) {
            windowState->value14 = 0;
            windowState->value10 = 3;
            WindowElement* const current = gCurrentWindow->elements;
            const std::int32_t height = current[2].height;
            const std::int32_t y = current[3].y;
            current[2].y = y - (height >> 1);
            for (std::int32_t row = 9; row <= 18; ++row) {
                gCurrentWindow->elements[row].mouseCallback = &WindowHotkeys_EditPresetText;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3BE60;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BE62;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BE61;
    }
    if (windowState->value10 == 3) {
        graphic->imageNo = 0x3BE61;
    }
    return consumed;
}

// 原函数：0x00495C20。
static std::int32_t WindowHotkeys_EditPresetText(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t row = std::bit_cast<std::int32_t>(gCurrentWindowState->value14) + index - 9;
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const auto selected = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(row));
        gHotkeySelectedCC4B20 = selected;
        input::InputContext* const context = &cg::ui::detail::gInputContexts5F9758[selected];
        gHotkeyInputPrototype57DF48.input = {{context}, WindowInputReferenceKind::Context};
        const std::wstring savedText = input::ContextText(*context);
        static_cast<void>(WindowInput_ActivateElement(&gHotkeyInputPrototype57DF48, 0, 0, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContexts5F9758[gHotkeySelectedCC4B20]);
        input::LoadInitialText_44A160(savedText);
        WindowInstance* const window = gCurrentWindow;
        gFocusedElementDataD45F94 = window->elements[0].data;
        input::InputContext* const selectedContext = &cg::ui::detail::gInputContexts5F9758[gHotkeySelectedCC4B20];
        static_cast<WindowInputData*>(window->elements[0].data)->context = selectedContext;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B59C;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B59E;
    }
    if (row == gHotkeySelectedCC4B20) {
        graphic->imageNo = 0x3B59D;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
