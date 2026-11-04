#include "cg/session/ClientSession.hpp"
#include "../../scene/login/LoginSavedState.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowHotkeysElementId : ElementId {
    Element1 = 1,
    ButtonClose = 2,
    Element3 = 3,
    HandleVerticalScroll = 4,
    ButtonPreviousRow = 5,
    ButtonNextRow = 6,
    ButtonPreviousPage = 7,
    ButtonNextPage = 8,
    Scroll = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    TabFirstCategory = 20,
    TabSecondCategory = 21,
    TabThirdCategory = 22,
    TabPresetText = 23,
    Element24 = 24,
    HandleSideArea = 25,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口25初始化，按分类绑定十行回调。

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(WindowInstance& callbackWindow, std::int32_t value); // 窗口25说明/预设文本绘制及编辑焦点收尾。

// 原函数：0x004953B0。
static std::int32_t WindowHotkeys_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 关闭并写回预设文本，不直接持久化。

// 原函数：0x00495450。
static std::int32_t WindowHotkeys_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上一行，bit400。

// 原函数：0x00495500。
static std::int32_t WindowHotkeys_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一行，音效后重新取计数。

// 原函数：0x004955C0。
static std::int32_t WindowHotkeys_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上十行，音效先于位置修改。

// 原函数：0x00495670。
static std::int32_t WindowHotkeys_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下十行，保留三阶段计数读取。

// 原函数：0x00495750。
static std::int32_t WindowHotkeys_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口25滚轮，恒返0。

// 原函数：0x004957B0。
static std::int32_t WindowHotkeys_TabFirstCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 分类0与十行复位。

// 原函数：0x004958D0。
static std::int32_t WindowHotkeys_TabSecondCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 分类1与十行复位。

// 原函数：0x004959F0。
static std::int32_t WindowHotkeys_TabThirdCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 分类2与十行复位。

// 原函数：0x00495B10。
static std::int32_t WindowHotkeys_TabPresetText(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 分类3只换回调，不复位行图号。

// 原函数：0x00495C20。
static std::int32_t WindowHotkeys_EditPresetText(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 预设文本备份、初始化及同源焦点。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr std::array<std::int8_t, 8> kHotkeyCounts56AEC4 {14, 27, 16, 24, 14, 27, 16, 24}; // 原常量参考：0x0056AEC4。

static constexpr const wchar_t* kHotkeyText56933C[14] { // 原常量参考：0x0056933C。
    L"开启/关闭人物状态视窗　　　　Ctrl + W",
    L"开启/关闭人物技能视窗　　　　Ctrl + E",
    L"开启/关闭物品视窗　　　　　　Ctrl + R",
    L"开启/关闭宠物状态视窗　　　　Ctrl + A",
    L"开启/关闭名片视窗　　　　　　Ctrl + S",
    L"开启/关闭宠物图鉴视窗　　　　Ctrl + D",
    L"关闭最上层视窗／开启系统菜单　ESC",
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
    L"关闭最上层视窗／开启系统菜单　ESC",
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
    {WindowElementType_Input, 80, 41, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element1)},
    {WindowElementType_Graphic, 265, 9, 11, 11, 1, {.graphic = &gWindowGraphic568E70}, &WindowHotkeys_ButtonClose, -1, static_cast<ElementId>(WindowHotkeysElementId::ButtonClose)},
    {WindowElementType_Graphic, 253, 68, 0, 14, 1, {.graphic = &gWindowGraphic568EA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element3)},
    {WindowElementType_MouseArea, 254, 68, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD677F0}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowHotkeysElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 253, 58, 11, 11, 1, {.graphic = &gWindowGraphic568EB8}, &WindowHotkeys_ButtonPreviousRow, -1, static_cast<ElementId>(WindowHotkeysElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 253, 204, 11, 11, 1, {.graphic = &gWindowGraphic568ED0}, &WindowHotkeys_ButtonNextRow, -1, static_cast<ElementId>(WindowHotkeysElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 227, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F48}, &WindowHotkeys_ButtonPreviousPage, -1, static_cast<ElementId>(WindowHotkeysElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 245, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F60}, &WindowHotkeys_ButtonNextPage, -1, static_cast<ElementId>(WindowHotkeysElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 0, 0, 286, 233, 1, {.unusedValue = 0}, &WindowHotkeys_Scroll, -1, static_cast<ElementId>(WindowHotkeysElementId::Scroll)},
    {WindowElementType_Graphic, 25, 56, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element10)},
    {WindowElementType_Graphic, 25, 72, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element11)},
    {WindowElementType_Graphic, 25, 88, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element12)},
    {WindowElementType_Graphic, 25, 104, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element13)},
    {WindowElementType_Graphic, 25, 120, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element14)},
    {WindowElementType_Graphic, 25, 136, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element15)},
    {WindowElementType_Graphic, 25, 152, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element16)},
    {WindowElementType_Graphic, 25, 168, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element17)},
    {WindowElementType_Graphic, 25, 184, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element18)},
    {WindowElementType_Graphic, 25, 200, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element19)},
    {WindowElementType_Graphic, 25, 36, 49, 17, 1, {.graphic = &gWindowGraphic568EE8}, &WindowHotkeys_TabFirstCategory, -1, static_cast<ElementId>(WindowHotkeysElementId::TabFirstCategory)},
    {WindowElementType_Graphic, 75, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F00}, &WindowHotkeys_TabSecondCategory, -1, static_cast<ElementId>(WindowHotkeysElementId::TabSecondCategory)},
    {WindowElementType_Graphic, 125, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabThirdCategory, -1, static_cast<ElementId>(WindowHotkeysElementId::TabThirdCategory)},
    {WindowElementType_Graphic, 175, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabPresetText, -1, static_cast<ElementId>(WindowHotkeysElementId::TabPresetText)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic568E88}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowHotkeysElementId::Element24)},
    {WindowElementType_EventArea, 288, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowHotkeysElementId::HandleSideArea)},
};

// 原型：窗口25；原全局：0x0054E690。
WindowPrototype gWindowPrototypeHotkeys {0, 4, 15, 117, 286, 233, 0x80010101u, 25, gWindowElements568F78, &WindowHotkeys_Update, &WindowHotkeys_Draw, &WindowRuntime_ReleaseDefault}; // 25: 54E690

// 窗口函数实现。

// 原函数：0x00495130。
static std::int32_t WindowHotkeys_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowInstance* const window = currentWindow;
        WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value14 = 0;
        auto* elementsWindow = window;
        auto* const element3 = elementsWindow->FindElementById(WindowHotkeysElementId::Element3);
        auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (element3 && handleVerticalScrollElement) {
            element3->y = handleVerticalScrollElement->y - (element3->height >> 1);
        }
        const std::uint32_t category = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value10;
        inputs.hotkeySelected = -1;
        const WindowElementCallback callback = category == 3 ? &WindowHotkeys_EditPresetText : &WindowElement_NoOp;
        for (std::int32_t index = 9; index <= 18; ++index) {
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index));
            if (indexedElement) {
                indexedElement->mouseCallback = callback;
            }
        }
    }
    return 1;
}

// 原函数：0x004951C0。
static std::int32_t WindowHotkeys_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
    auto* const element3 = elementsWindow->FindElementById(WindowHotkeysElementId::Element3);
    if (element3) {
        element3->visible = 1;
    }
    const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
    const std::uint32_t category = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value10;
    const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
    if (maximum <= 0) {
        auto* const element32 = currentWindow->FindElementById(WindowHotkeysElementId::Element3);
        if (element32) {
            element32->visible = 0;
        }
    }
    if (handleVerticalScrollElement && (handleVerticalScrollElement->data.mouse->flags & 1) != 0) {
        const std::uint32_t currentCategory = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value10;
        const std::int32_t currentMode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t currentMaximum =
            kHotkeyCounts56AEC4[currentCategory + currentMode * 4] - 10;
        auto* const handleVerticalScrollElement2 = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement2) {
            WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value14 = static_cast<std::uint32_t>(
                WindowScroll_ReadPosition(handleVerticalScrollElement2, currentMaximum));
        }
    }

    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int8_t marker = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(render::gHoverFramePaletteIndex_D69768));
        const std::uint32_t currentCategory = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value10;
        const std::int32_t currentMode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t scroll = std::bit_cast<std::int32_t>(WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value14);
        const wchar_t* const text = kHotkeyTextTables56AECC[currentCategory + currentMode * 4][scroll + row];
        WindowInstance* const window = currentWindow;
        const std::int16_t y = static_cast<std::int16_t>(window->y);
        const std::int16_t x = static_cast<std::int16_t>(window->x);
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 31),
            static_cast<std::int16_t>(y + row * 16 + 58), 3, 8, 0, text, 0, marker));
    }

    if (WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value10 == 3) {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t scroll = std::bit_cast<std::int32_t>(WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value14);
            const std::int8_t selected = inputs.hotkeySelected;
            const std::int32_t recordIndex = scroll + row;
            if (recordIndex == selected) {
                WindowInstance* const window = currentWindow;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(
                    *callbackWindow.ownerSession,
                    &inputs.hotkeyPrototype, x + 96, y + row * 16 + 58, 2));
            } else {
                const std::int8_t marker = std::bit_cast<std::int8_t>(
                    static_cast<std::uint8_t>(render::gHoverFramePaletteIndex_D69768));
                const std::wstring text = input::ContextText(inputs.hotkeys[recordIndex]);
                WindowInstance* const window = currentWindow;
                const std::int16_t y = static_cast<std::int16_t>(window->y);
                const std::int16_t x = static_cast<std::int16_t>(window->x);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x + 96),
                    static_cast<std::int16_t>(y + row * 16 + 58), 3, 8, 0, text, 0, marker));
            }
        }
        const std::int8_t selected = inputs.hotkeySelected;
        if (selected >= 0) {
            const std::int32_t scroll = std::bit_cast<std::int32_t>(WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Hotkeys].value14);
            if (scroll > selected || selected >= scroll + 10) {
                static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.hotkeyPrototype, 0, -100, 2));
            }
        }
    } else {
        // 原比较上界是最后一项起点，不是整个数组末端。
        const auto currentInput = reinterpret_cast<std::uintptr_t>(callbackWindow.ownerSession->UiFrame().inputState.currentContext);
        const auto firstInput = reinterpret_cast<std::uintptr_t>(&inputs.hotkeys[0]);
        const auto lastInput = reinterpret_cast<std::uintptr_t>(&inputs.hotkeys[23]);
        if (currentInput >= firstInput && currentInput <= lastInput) {
            WindowChat_RestoreInput(*callbackWindow.ownerSession);
            inputs.hotkeySelected = -1;
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004953B0。
static std::int32_t WindowHotkeys_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = 0x3B538;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B53A;
            uiFrame.hoverText = kSettingsClose55331C;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return 0;
        }
        graphic->imageNo = 0x3B539;
    }
    currentWindow->state |= 0x8000u;
    static_cast<void>(login::detail::CapturePresetChatTexts_520640(*callbackWindow.ownerSession));
    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
    return 1;
}

// 原函数：0x00495450。
static std::int32_t WindowHotkeys_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        if (position > 0) {
            windowState->value14 = static_cast<std::uint32_t>(position - 1);
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, current));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B592;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B594;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B593;
    }
    return consumed;
}

// 原函数：0x00495500。
static std::int32_t WindowHotkeys_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        if (position < maximum) {
            windowState->value14 = static_cast<std::uint32_t>(position + 1);
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t currentCategory = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t currentMode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t currentMaximum = kHotkeyCounts56AEC4[currentCategory + currentMode * 4] - 10;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, currentMaximum, current));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B595;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B597;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B596;
    }
    return consumed;
}

// 原函数：0x004955C0。
static std::int32_t WindowHotkeys_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (std::bit_cast<std::int32_t>(windowState->value14) > 0) {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14) - 10;
        windowState->value14 = static_cast<std::uint32_t>(position);
        if (position < 0) {
            windowState->value14 = 0;
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, current));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B556;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B558;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00495670。
static std::int32_t WindowHotkeys_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        if (maximum > 0 && std::bit_cast<std::int32_t>(windowState->value14) < maximum) {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14) + 10;
        windowState->value14 = static_cast<std::uint32_t>(position);
        const std::int32_t limitMode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t limitCategory = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t limit = kHotkeyCounts56AEC4[limitCategory + limitMode * 4] - 10;
        if (position > limit) {
            windowState->value14 = static_cast<std::uint32_t>(limit);
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(windowState->value14);
        const std::int32_t currentMode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t currentMaximum = kHotkeyCounts56AEC4[limitCategory + currentMode * 4] - 10;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, currentMaximum, current));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B559;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B55B;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00495750。
static std::int32_t WindowHotkeys_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::ReadSessionMouseWheelDelta(*callbackWindow.ownerSession));
        const std::int32_t category = std::bit_cast<std::int32_t>(windowState->value10);
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).hotkeyLayoutMode;
        const std::int32_t maximum = kHotkeyCounts56AEC4[category + mode * 4] - 10;
        const std::int32_t position = std::bit_cast<std::int32_t>(windowState->value14);
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            windowState->value14 = static_cast<std::uint32_t>(
                WindowScroll_ApplyWheel(indexedElement, position, maximum, wheel));
        }
    }
    return 0;
}

// 原函数：0x004957B0。
static std::int32_t WindowHotkeys_TabFirstCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    auto* elementsWindow = activeWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"显示游戏功能视窗相关热键。" /* 原常量参考：0x00586FA4。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (windowState->value10 != 0) {
            windowState->value14 = 0;
            windowState->value10 = 0;
            auto* currentWindow = activeWindow;
            auto* const element3 = currentWindow->FindElementById(WindowHotkeysElementId::Element3);
            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
            if (element3 && handleVerticalScrollElement) {
                element3->y = handleVerticalScrollElement->y - (element3->height >> 1);
            }
            for (std::int32_t row = 9; row <= 18; ++row) {
                auto* const indexedElement = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement) {
                    indexedElement->mouseCallback = &WindowElement_NoOp;
                }
                auto* const indexedElement2 = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement2) {
                    indexedElement2->data.graphic->imageNo = 0x3B59C;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B903;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B905;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B904;
    }
    if (windowState->value10 == 0) {
        if (graphic) graphic->imageNo = 0x3B904;
    }
    return consumed;
}

// 原函数：0x004958D0。
static std::int32_t WindowHotkeys_TabSecondCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    auto* elementsWindow = activeWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"显示画面左上角状态显示相关热键。" /* 原常量参考：0x00586FC0。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (windowState->value10 != 1) {
            windowState->value14 = 0;
            windowState->value10 = 1;
            auto* currentWindow = activeWindow;
            auto* const element3 = currentWindow->FindElementById(WindowHotkeysElementId::Element3);
            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
            if (element3 && handleVerticalScrollElement) {
                element3->y = handleVerticalScrollElement->y - (element3->height >> 1);
            }
            for (std::int32_t row = 9; row <= 18; ++row) {
                auto* const indexedElement = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement) {
                    indexedElement->mouseCallback = &WindowElement_NoOp;
                }
                auto* const indexedElement2 = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement2) {
                    indexedElement2->data.graphic->imageNo = 0x3B59C;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B906;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B908;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B907;
    }
    if (windowState->value10 == 1) {
        if (graphic) graphic->imageNo = 0x3B907;
    }
    return consumed;
}

// 原函数：0x004959F0。
static std::int32_t WindowHotkeys_TabThirdCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    auto* elementsWindow = activeWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"显示画面右上角情绪动作相关热键。" /* 原常量参考：0x00586FE4。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (windowState->value10 != 2) {
            windowState->value14 = 0;
            windowState->value10 = 2;
            auto* currentWindow = activeWindow;
            auto* const element3 = currentWindow->FindElementById(WindowHotkeysElementId::Element3);
            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
            if (element3 && handleVerticalScrollElement) {
                element3->y = handleVerticalScrollElement->y - (element3->height >> 1);
            }
            for (std::int32_t row = 9; row <= 18; ++row) {
                auto* const indexedElement = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement) {
                    indexedElement->mouseCallback = &WindowElement_NoOp;
                }
                auto* const indexedElement2 = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement2) {
                    indexedElement2->data.graphic->imageNo = 0x3B59C;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B909;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B90B;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B90A;
    }
    if (windowState->value10 == 2) {
        if (graphic) graphic->imageNo = 0x3B90A;
    }
    return consumed;
}

// 原函数：0x00495B10。
static std::int32_t WindowHotkeys_TabPresetText(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    auto* elementsWindow = activeWindow;
    WindowState* const windowState = (&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id]);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"登录常用句。" /* 原常量参考：0x00587008。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (windowState->value10 != 3) {
            windowState->value14 = 0;
            windowState->value10 = 3;
            auto* currentWindow = activeWindow;
            auto* const element3 = currentWindow->FindElementById(WindowHotkeysElementId::Element3);
            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowHotkeysElementId::HandleVerticalScroll);
            if (element3 && handleVerticalScrollElement) {
                element3->y = handleVerticalScrollElement->y - (element3->height >> 1);
            }
            for (std::int32_t row = 9; row <= 18; ++row) {
                auto* const indexedElement = activeWindow->FindElementById(NativeElementKey(row));
                if (indexedElement) {
                    indexedElement->mouseCallback = &WindowHotkeys_EditPresetText;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3BE60;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3BE62;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3BE61;
    }
    if (windowState->value10 == 3) {
        if (graphic) graphic->imageNo = 0x3BE61;
    }
    return consumed;
}

// 原函数：0x00495C20。
static std::int32_t WindowHotkeys_EditPresetText(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const std::int32_t row = std::bit_cast<std::int32_t>((&WindowRuntime_States(*callbackWindow.ownerSession)[callbackWindow.id])->value14) + index - 9;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const auto selected = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(row));
        inputs.hotkeySelected = selected;
        input::InputContext* const context = &inputs.hotkeys[selected];
        inputs.hotkeyPrototype.input = {{context}, WindowInputReferenceKind::Context};
        const std::wstring savedText = input::ContextText(*context);
        static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.hotkeyPrototype, 0, 0, 0));
        input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.hotkeys[inputs.hotkeySelected]);
        input::LoadInitialText_44A160(*callbackWindow.ownerSession, savedText);
        WindowInstance* const window = currentWindow;
        auto* const element1 = window->FindElementById(WindowHotkeysElementId::Element1);
        if (element1) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = element1->data.input;
        }
        input::InputContext* const selectedContext = &inputs.hotkeys[inputs.hotkeySelected];
        auto* const element12 = window->FindElementById(WindowHotkeysElementId::Element1);
        if (element12) {
            element12->data.input->context = selectedContext;
        }
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B59C;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B59E;
    }
    if (row == inputs.hotkeySelected) {
        if (graphic) graphic->imageNo = 0x3B59D;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
