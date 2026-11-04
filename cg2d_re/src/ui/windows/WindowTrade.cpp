#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PetTechniqueRecord.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowTradeElementId : ElementId {
    HandleAmountInput = 1,
    ButtonClose = 2,
    ButtonPublish = 3,
    ButtonConfirm = 4,
    ButtonWithdraw = 5,
    Element6 = 6,
    Element7 = 7,
    HandleVerticalScroll = 8,
    HandleSelectedPanel = 9,
    Element10 = 10,
    HandleVerticalScroll_11 = 11,
    HandleOwnPanel = 12,
    Element13 = 13,
    HandleVerticalScroll_14 = 14,
    HandleRemoteSelection = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。




// 原4B1260的14参槽位回调另携所属会话；借用数值与描述符，返回值原端弃用。
using WindowTradePanelCallback = void (*)(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 回调与内部函数前置声明。

// 原函数：0x0048B450。
static std::int32_t WindowItem_DrawPetTooltip(session::ClientSession& session, std::int32_t side, std::int32_t index, std::int32_t page);

// 原函数：0x004E5710。
static std::int32_t WindowTrade_ResetRemoteItems(session::ClientSession& session, std::int32_t side); // 先清侧状态，再清物品/宠物整组。

// 原函数：0x004E52B0。
static void WindowTrade_CopyOwnPet(session::ClientSession& session, std::int32_t side); // 同源玩家宠物属性、20项技能及6个装备名称复制。

// 原函数：0x004E5600。
static void WindowTrade_SyncOwnItems(session::ClientSession& session, std::int32_t side); // 顺序同步20条交易物品。

// 原函数：0x004B0110。
static cg::yobi::YobiAction* WindowTrade_Open(session::ClientSession& session); // 清交易U8，创建窗口43动画并发布借用T*。

// 窗口43三元素组：低S16位置、仅隐藏时联动、只写图形颜色。
// 原函数：0x004B0130。
static void WindowTrade_MovePanel(
    WindowInstance* window, std::int32_t index, std::int32_t x, std::int32_t y);

// 原函数：0x004B0180。
static void WindowTrade_SetPanelVisible(WindowInstance* window, std::int32_t index, std::int32_t visible);

// 原函数：0x004B01B0。
static void WindowTrade_SetPanelColor(WindowInstance* window, std::int32_t index, std::uint32_t color);

// 原函数：0x004AFF70。
static std::int32_t WindowTrade_RemoveItem(session::ClientSession& session, std::int32_t index); // 释放物品选择，再减同源悬停字节。

// 原函数：0x004B0090。
static std::int32_t WindowTrade_RemovePet(session::ClientSession& session, std::int32_t index); // 只释放宠物选择及标记。

// 原函数：0x004B0200。
static void WindowTrade_Release(WindowInstance&); // 窗口43释放回调，原返回值弃用。

// 原函数：0x004B0290。
static std::int32_t WindowTrade_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口43初始化、关闭检查与四阶段更新。

// 原函数：0x004B08D0。
static std::int32_t WindowTrade_Draw(WindowInstance& callbackWindow, std::int32_t unused); // 四类提示后绘制完整当前窗口。

// 原函数：0x004AFF30。
static std::int32_t WindowTrade_AddItem(session::ClientSession& session, std::int32_t index, std::int32_t slot); // 空槽领取并标记物品。

// 原函数：0x004AFFB0。
static std::int32_t WindowTrade_SwapSelectedItems(session::ClientSession& session, std::int32_t first, std::int32_t second); // 仅交换选择槽字节。

// 原函数：0x004B0060。
static std::int32_t WindowTrade_AddPet(session::ClientSession& session, std::int32_t index, std::int32_t slot); // 空槽领取并标记宠物。

// 原函数：0x004B00C0。
static std::int32_t WindowTrade_SwapSelectedPets(session::ClientSession& session, std::int32_t first, std::int32_t second);

// 原函数：0x004B1940。
static void WindowTrade_HandleOwnInventory(
    session::ClientSession& session,
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 自有物品交易页，20格玩家背包。

// 原函数：0x004B2480。
static std::int32_t WindowTrade_HandleOwnPanel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x004B2500。
static void WindowTrade_RefreshOwnPanel(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* unusedAmount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 原函数：0x004B2060。
static void WindowTrade_HandleOwnPets(
    session::ClientSession& session,
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 自有宠物交易页，按索引对取原宠物。

// 原函数：0x004B25D0。
static void WindowTrade_HandleOwnItemDrag(session::ClientSession& session, std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);

// 原函数：0x004B2E00。
static void WindowTrade_HandleOwnPetDrag(session::ClientSession& session, std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);

// 原函数：0x004B2710。
static std::int32_t WindowTrade_StartOwnItemDrag(session::ClientSession& session, std::int32_t index); // kind6，绑定真实物品拖动回调。

// 原函数：0x004B2F50。
static std::int32_t WindowTrade_StartOwnPetDrag(session::ClientSession& session, std::int32_t index); // kind7，绑定真实宠物拖动回调。

// 原函数：0x004B2730。
static void WindowTrade_HandleSelectedItem(
    session::ClientSession& session,
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 己方已选物品完整交互/绘制。

// 原函数：0x004B2F70。
static void WindowTrade_HandleSelectedPet(
    session::ClientSession& session,
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 己方已选宠物完整交互/绘制。

// 原函数：0x004B3430。
static std::int32_t WindowTrade_HandleSelectedPanel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x004B34C0。
static void WindowTrade_RefreshSelectedPanel(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 原函数：0x004B0F20。
static std::int32_t WindowTrade_DrawPetRow(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int8_t side, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 空宠物槽仍画背景。

// 原函数：0x004B1260。
static std::int32_t WindowTrade_HandlePanel(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    const std::int32_t* backgroundImages, std::uint8_t* tab,
    std::int32_t scrollElement, std::int32_t* scrollValue,
    std::int32_t numericElement, std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* descriptor, WindowTradePanelCallback callback); // TQN01：低W16坐标/scroll BYTE及声效后fresh滚动目标，callback与真实S32 0/2返回保。

// 原函数：0x004B3580。
static std::int32_t WindowTrade_HandleRemoteSelection(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 对方交易面板入口。

// 原函数：0x004B3610。
static void WindowTrade_HandleRemoteItem(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 原函数：0x004B1900。
static bool WindowTrade_HitTestOwnPetRow(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height); // X未命中则不读Y。

// 原函数：0x004AFFE0。
static std::int32_t WindowTrade_ReadSelectedItem(session::ClientSession& session, std::int32_t index); // S8物品槽，负值归-1。

// 原函数：0x004B00F0。
static std::int32_t WindowTrade_ReadSelectedPet(session::ClientSession& session, std::int32_t index); // S8宠物槽，负值归-1。

// 原函数：0x004B0A50。
static std::int32_t WindowTrade_ButtonPublish(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 公开两组交易槽及数值。

// 原函数：0x004B0CF0。
static std::int32_t WindowTrade_ButtonWithdraw(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 撤回公开，无计时门禁。

// 原函数：0x004B0DB0。
static std::int32_t WindowTrade_ButtonConfirm(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 确认/撤销确认，共享500ms门槛。

// 原函数：0x004B09E0。
static std::int32_t WindowTrade_HandleAmountInput(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 交易输入激活/聊天恢复。

// 原函数：0x004B4070。
static std::int32_t WindowTrade_DrawRemotePetSkillRow(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t skill); // 外部宠物技能名称行，空行仍绘背景。

// 原函数：0x004B45A0。
static std::int32_t WindowTrade_DrawSelectedPetSkillRow(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t side,
    std::int32_t index, std::int32_t skill); // 交易宠物技能行，空文本仍绘制行图。

// 原函数：0x004B4680。
static std::int32_t WindowTrade_DrawSelectedPetTooltip(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t index); // 完整交易宠物提示。

// 原函数：0x004B3B00。
static std::int32_t WindowTrade_DrawOwnPetSkillRow(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t petIndex,
    std::int32_t skillPosition); // 己方技能按S16索引对取名称。

// 原函数：0x004E4E60。
static void WindowTrade_RequestTargetList(session::ClientSession& session, std::int32_t index, const wchar_t* name); // 先发送选择请求，失败仍继续本地打开。

// 原函数：0x004E4E80。
static cg::yobi::YobiAction* WindowTrade_OpenTargetList(session::ClientSession& session, const char* text); // 选择表绑定真实交易回调后打开44。

// 原函数：0x004E5770。
static void WindowTrade_BuildRemoteItemDescription(session::ClientSession& session, std::int32_t index, std::int32_t side, const char* text); // 32行及页数。

// 原函数：0x004E5830。
static std::int32_t WindowTrade_ReceiveRemoteItems(session::ClientSession& session, const char* text); // 每条14字段，返回终止字段状态1。

// 原函数：0x004E5A40。
static std::int32_t WindowTrade_ReceiveRemotePet(session::ClientSession& session, std::int32_t index, const char* text); // 单宠物属性与双名称。

// 原函数：0x004E5CD0。
static std::int32_t WindowTrade_ReceiveRemotePetSkills(session::ClientSession& session, std::int32_t index, const char* text); // 清空整表后接收20个技能名及6个装备名。

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x005805FC。 原字节提示的借用指针表。
static const std::array<const wchar_t*, 6> gTradeNumberHints5805FC {{ // 原全局：0x005805FC。
    L"指定金额。",
    L"指定提领金额。",
    L"指定金额数量。",
    L"减少一个位数。",
    L"指定全部金额。",
    L"把指定金额归零。",
}};

static WindowGraphicData gWindowGraphic572940 {0x3BA88, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572940。

static WindowGraphicData gWindowGraphic572958 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572958。

static WindowGraphicData gWindowGraphic572970 {0x3BA89, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572970。

static WindowGraphicData gWindowGraphic572988 {0x3BA8C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572988。

static WindowGraphicData gWindowGraphic5729A0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005729A0。

static WindowGraphicData gWindowGraphic572AD8 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572AD8。

static WindowTextPrototype gWindowText572AF0 { // 原全局：0x00572AF0。
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, L"",
};

static std::uint32_t gWindowMouseFlagsD678B8 = 0; // 原全局：0x00D678B8。

static std::uint32_t gWindowMouseFlagsD678C4 = 0; // 原全局：0x00D678C4。

static std::uint32_t gWindowMouseFlagsD678D0 = 0; // 原全局：0x00D678D0。

static WindowElementPrototype gWindowElements572AF8[20] { // 原全局：0x00572AF8。
    {WindowElementType_Input, 0, 0, 567, 339, 1, {.unusedValue = 0}, &WindowTrade_HandleAmountInput, -1, static_cast<ElementId>(WindowTradeElementId::HandleAmountInput)},
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic572958}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowTradeElementId::ButtonClose)},
    {WindowElementType_Graphic, 476, 309, 49, 17, 1, {.graphic = &gWindowGraphic572970}, &WindowTrade_ButtonPublish, -1, static_cast<ElementId>(WindowTradeElementId::ButtonPublish)},
    {WindowElementType_Graphic, 446, 309, 49, 17, 1, {.graphic = &gWindowGraphic572988}, &WindowTrade_ButtonConfirm, -1, static_cast<ElementId>(WindowTradeElementId::ButtonConfirm)},
    {WindowElementType_Graphic, 502, 309, 49, 17, 1, {.graphic = &gWindowGraphic5729A0}, &WindowTrade_ButtonWithdraw, -1, static_cast<ElementId>(WindowTradeElementId::ButtonWithdraw)},
    {WindowElementType_EventArea, 376, 136, 106, 85, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element6)},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element7)},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678C4}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowTradeElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleSelectedPanel, -1, static_cast<ElementId>(WindowTradeElementId::HandleSelectedPanel)},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element10)},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678B8}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowTradeElementId::HandleVerticalScroll_11)},
    {WindowElementType_Graphic, 297, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleOwnPanel, -1, static_cast<ElementId>(WindowTradeElementId::HandleOwnPanel)},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element13)},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678D0}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowTradeElementId::HandleVerticalScroll_14)},
    {WindowElementType_Graphic, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleRemoteSelection, -1, static_cast<ElementId>(WindowTradeElementId::HandleRemoteSelection)},
    {WindowElementType_Text, 133, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element16)},
    {WindowElementType_Text, 281, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element17)},
    {WindowElementType_Text, 317, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element18)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572940}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element19)},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowTradeElementId::Element20)},
};

// 原型：窗口43；原全局：0x0054EBD0。
WindowPrototype gWindowPrototypeTrade {0, 4, 20, 25, 567, 339, 0x80010101u, 20, gWindowElements572AF8, &WindowTrade_Update, &WindowTrade_Draw, &WindowTrade_Release}; // 43: 54EBD0

// 窗口函数实现。

// 原函数：0x0048B450。
static std::int32_t WindowItem_DrawPetTooltip(
    session::ClientSession& session,
    const std::int32_t side, const std::int32_t index, const std::int32_t page)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& trade = session.UiFrame().native.trade;
    const std::int32_t key = index + 200;
    if (interaction.tooltipSource != key) {
        interaction.tooltipX = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        interaction.tooltipY = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        interaction.tooltipX = static_cast<std::uint16_t>(interaction.tooltipX + (left ? 60 : -257));
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        interaction.tooltipY = static_cast<std::uint16_t>(interaction.tooltipY + (top ? 60 : -174));
    }
    const std::int32_t sideOffset =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side)) - 10;
    interaction.tooltipSource = key;
    const auto* const record = &trade.items[key + sideOffset * 20];
    std::uint32_t flags;
    std::memcpy(&flags, &record->flags, sizeof(flags));
    const std::wstring title = (flags & 0x100u) != 0 ? record->memo : record->name;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags >> 9) & 1u);
    const auto* const lines = record->description.data() + page * 8;
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    return WindowItem_DrawTooltip(session, x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
}

// 原函数：0x004E5DA0。
void WindowTrade_SwapRemoteItems(
    session::ClientSession& session,
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    auto& trade = session.UiFrame().native.trade;
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    auto* const left = &trade.items[side * 20 + first - 8];
    auto* const right = &trade.items[side * 20 + second - 8];
    std::swap(*left, *right);
}

// 原函数：0x004E1840。
void WindowTrade_SwapDisplayItems(
    session::ClientSession& session,
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    auto& storage = session.UiFrame().native.storage;
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    auto* const left = &storage.groups[side].items[first - 8];
    auto* const right = &storage.groups[side].items[second - 8];
    std::swap(*left, *right);
}

// 原函数：0x004B0000。
void WindowTrade_ResetSelection(session::ClientSession& session, const std::int32_t first, const std::int32_t second)
{
    auto& trade = session.UiFrame().native.trade;
    if (WindowRuntime_States(session)[WindowId_Trade].window == nullptr) {
        return;
    }
    for (std::uint8_t& slot : trade.itemSlots) {
        const std::int32_t value = std::bit_cast<std::int8_t>(slot);
        if (value == first) {
            slot = static_cast<std::uint8_t>(second);
        } else if (value == second) {
            slot = static_cast<std::uint8_t>(first);
        }
    }
    const std::uint8_t firstMark = trade.itemMarks[first];
    const std::uint8_t secondMark = trade.itemMarks[second];
    trade.itemMarks[first] = secondMark;
    trade.itemMarks[second] = firstMark;
}

// 原函数：0x004AD060。
void WindowTrade_RemapTextListSelection(session::ClientSession& session, const std::int32_t first, const std::int32_t second)
{
    auto& textList = session.UiFrame().native.textList;
    if (WindowRuntime_States(session)[WindowId_TextList].window != nullptr && textList.selectedItem == first) {
        textList.selectedItem = second;
    }
}

// 原函数：0x004E54E0。
void WindowTrade_CopyOwnItem(session::ClientSession& session, const std::int32_t side, const std::int32_t index)
{
    auto& trade = session.UiFrame().native.trade;
    if (index < 0 || index >= 20) {
        return;
    }
    auto* const record = &trade.items[index + side * 20];
    *record = {};
    const world::PlayerItemRecord& source = session.WorldScene().playerItems[index + 8];
    if (source.used == 0) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    record->used = std::uint16_t{1};
    const std::uint16_t flags = source.flags;
    record->imageNo = imageNo;
    const std::int32_t itemId = source.itemId;
    record->count = count;
    record->itemId = itemId;
    record->flags = static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags));
    if (!source.memo.empty()) {
        record->memo = source.memo;
    }
    record->name = source.name;
    if ((flags & 1u) == 0) {
        record->selection = std::bit_cast<std::int16_t>(std::uint16_t{0xFFFF});
    }
}

// 原函数：0x004E5600。
static void WindowTrade_SyncOwnItems(session::ClientSession& session, const std::int32_t side)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        WindowTrade_CopyOwnItem(session, side, index);
    }
}

// 原函数：0x004B0110。
static cg::yobi::YobiAction* WindowTrade_Open(session::ClientSession& session)
{
    auto& trade = session.UiFrame().native.trade;
    trade.valueD67B80 = 0;
    cg::yobi::YobiAction* const action = WindowRuntime_CreateAnimation(session, WindowId_Trade, 1, 0, nullptr);
    trade.windowAction = action;
    return action;
}

// 原函数：0x004B0130。
static void WindowTrade_MovePanel(
    WindowInstance* const window, const std::int32_t index,
    const std::int32_t x, const std::int32_t y)
{
    const std::int32_t shortX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const std::int32_t shortY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->x = shortX;
    if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->y = shortY;
    if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(index - 1)) : nullptr; indexedElement3) indexedElement3->x = shortX + 252;
    if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(index - 1)) : nullptr; indexedElement4) indexedElement4->y = shortY + 37;
    if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(index - 2)) : nullptr; indexedElement5) indexedElement5->x = shortX + 252;
}

// 原函数：0x004B0180。
static void WindowTrade_SetPanelVisible(
    WindowInstance* const window, const std::int32_t index, const std::int32_t visible)
{
    if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = visible;
    if (visible == 0) {
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index - 1)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(index - 2)) : nullptr; indexedElement3) indexedElement3->visible = 0;
    }
}

// 原函数：0x004B01B0。
static void WindowTrade_SetPanelColor(
    WindowInstance* const window, const std::int32_t index, const std::uint32_t color)
{
    auto* const graphicElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->color = color;
    }
    return;
}

// 原函数：0x004AFF30。
static std::int32_t WindowTrade_AddItem(session::ClientSession& session, const std::int32_t index, const std::int32_t slot)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& trade = session.UiFrame().native.trade;
    if (std::bit_cast<std::int8_t>(trade.itemSlots[index]) >= 0) {
        return 0;
    }
    trade.itemSlots[index] = static_cast<std::uint8_t>(slot);
    auto* const hover = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    const std::uint8_t previous = hover[slot];
    trade.itemMarks[slot] = 1;
    hover[slot] = static_cast<std::uint8_t>(previous + 1);
    return 1;
}

// 原函数：0x004AFFB0。
static std::int32_t WindowTrade_SwapSelectedItems(session::ClientSession& session, const std::int32_t first, const std::int32_t second)
{
    auto& trade = session.UiFrame().native.trade;
    const std::uint8_t previous = trade.itemSlots[first];
    trade.itemSlots[first] = trade.itemSlots[second];
    trade.itemSlots[second] = previous;
    return first;
}

// 原函数：0x004B0060。
static std::int32_t WindowTrade_AddPet(session::ClientSession& session, const std::int32_t index, const std::int32_t slot)
{
    auto& trade = session.UiFrame().native.trade;
    if (std::bit_cast<std::int8_t>(trade.petSlots[index]) >= 0) {
        return 0;
    }
    trade.petSlots[index] = static_cast<std::uint8_t>(slot);
    trade.petMarks[slot] = 1;
    return 1;
}

// 原函数：0x004B00C0。
static std::int32_t WindowTrade_SwapSelectedPets(session::ClientSession& session, const std::int32_t first, const std::int32_t second)
{
    auto& trade = session.UiFrame().native.trade;
    const std::uint8_t previous = trade.petSlots[first];
    trade.petSlots[first] = trade.petSlots[second];
    trade.petSlots[second] = previous;
    return first;
}

// 原函数：0x004AFF70。
static std::int32_t WindowTrade_RemoveItem(session::ClientSession& session, const std::int32_t index)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& trade = session.UiFrame().native.trade;
    const std::int32_t slot = std::bit_cast<std::int8_t>(trade.itemSlots[index]);
    if (slot < 0) {
        return -1;
    }
    trade.itemSlots[index] = 0xFF;
    auto* const hover = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    const std::uint8_t previous = hover[slot];
    trade.itemMarks[slot] = 0;
    hover[slot] = static_cast<std::uint8_t>(previous - 1);
    return slot;
}

// 原函数：0x004B0090。
static std::int32_t WindowTrade_RemovePet(session::ClientSession& session, const std::int32_t index)
{
    auto& trade = session.UiFrame().native.trade;
    const std::int32_t slot = std::bit_cast<std::int8_t>(trade.petSlots[index]);
    if (slot < 0) {
        return -1;
    }
    trade.petSlots[index] = 0xFF;
    trade.petMarks[slot] = 0;
    return slot;
}

// 原函数：0x004B0200。
static void WindowTrade_Release(WindowInstance& releasedWindow)
{
    auto& trade = releasedWindow.ownerSession->UiFrame().native.trade;
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_Quantity].window;
    if (window != nullptr && WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_Quantity].value20 == 43) {
        window->state |= 0x8000u;
    }
    if (trade.valueD67B80 == 0) {
        static_cast<void>(releasedWindow.ownerSession->Connection().SendTradeDecision(0));
        trade.valueD67CBC = 1;
    }
    for (std::int32_t index = 0; index < 20; ++index) {
        static_cast<void>(WindowTrade_RemoveItem(*releasedWindow.ownerSession, index));
    }
    for (std::int32_t index = 0; index < 5; ++index) {
        static_cast<void>(WindowTrade_RemovePet(*releasedWindow.ownerSession, index));
    }
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    trade.valueD67B80 = 0;
}

// 原函数：0x004B0290。
static std::int32_t WindowTrade_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowInstance* const initialWindow = currentWindow;
        trade.windowAction = nullptr;
        trade.valueCC4224 = 0;
        trade.valueD2E2B0 = 0;
        trade.phase = 0;
        WindowTrade_SetPanelVisible(initialWindow, 11, 1);
        WindowTrade_MovePanel(currentWindow, 11, 287, 28);
        WindowTrade_SetPanelColor(currentWindow, 11, 0xFFFFFFFFu);
        WindowTrade_SetPanelVisible(currentWindow, 8, 1);
        WindowTrade_MovePanel(currentWindow, 8, 15, 28);
        WindowTrade_SetPanelColor(currentWindow, 8, 0xFFFFFFFFu);
        WindowTrade_SetPanelVisible(currentWindow, 14, 0);
        WindowTrade_MovePanel(currentWindow, 14, 15, 28);
        WindowTrade_SetPanelColor(currentWindow, 14, 0xFFFFFFFFu);
        if (auto* const buttonPublishElement = currentWindow->FindElementById(WindowTradeElementId::ButtonPublish); buttonPublishElement) buttonPublishElement->visible = 1;
        if (auto* const buttonConfirmElement = currentWindow->FindElementById(WindowTradeElementId::ButtonConfirm); buttonConfirmElement) buttonConfirmElement->visible = 0;
        if (auto* const buttonWithdrawElement = currentWindow->FindElementById(WindowTradeElementId::ButtonWithdraw); buttonWithdrawElement) buttonWithdrawElement->visible = 0;
        WindowInstance* const scrollWindow = currentWindow;
        trade.valueD2EA20 = 0xFF;
        trade.valueD6742C = 0;
        trade.valueC8AF81 = 0;
        trade.scrollCF3B0C = 0;
        trade.scrollCC4C90 = 0;
        trade.scrollD67430 = 0;
        auto* const handleVerticalScroll_11Element = scrollWindow->FindElementById(WindowTradeElementId::HandleVerticalScroll_11);
        if (handleVerticalScroll_11Element) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScroll_11Element, 2, 0));
        }
        const std::int32_t firstScroll = trade.scrollCC4C90;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowTradeElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, 2, firstScroll));
        }
        WindowInstance* const lastScrollWindow = currentWindow;
        const std::int32_t lastScroll = trade.scrollD67430;
        auto* const handleVerticalScroll_14Element = lastScrollWindow->FindElementById(WindowTradeElementId::HandleVerticalScroll_14);
        if (handleVerticalScroll_14Element) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScroll_14Element, 2, lastScroll));
        }
        trade.itemMarks.fill(0);
        // 原W32/U8交错写入同一数组，保留4B0424的窗口快照时点。
        std::memset(trade.petMarks.data(), 0, 4);
        std::memset(trade.itemSlots.data(), 0xFF, 4);
        std::memset(trade.itemSlots.data() + 4, 0xFF, 4);
        trade.petMarks[4] = 0;
        WindowInstance* const textWindow = currentWindow;
        std::memset(trade.itemSlots.data() + 8, 0xFF, 4);
        std::memset(trade.itemSlots.data() + 12, 0xFF, 4);
        std::memset(trade.petSlots.data(), 0xFF, 4);
        std::memset(trade.itemSlots.data() + 16, 0xFF, 4);
        trade.petSlots[4] = 0xFF;
        auto* const element16 = textWindow->FindElementById(WindowTradeElementId::Element16);
        if (element16) {
            auto* const otherName = element16->data.text;
            otherName->text = std::format(L"{:>16}", trade.name);
        }
        auto* elementsWindow = currentWindow;
        const wchar_t* const playerName = world::GetPlayerDisplayText(*callbackWindow.ownerSession, 1);
        auto* const element17 = elementsWindow->FindElementById(WindowTradeElementId::Element17);
        if (element17) {
            auto* const separator = element17->data.text;
            separator->text = L"<=>";
        }
        auto* const element18 = currentWindow->FindElementById(WindowTradeElementId::Element18);
        if (element18) {
            auto* const ownName = element18->data.text;
            ownName->text = std::format(L"{:<16}", playerName);
        }
        const std::int32_t y =
            static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerY);
        const std::int32_t x =
            static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerX);
        static_cast<void>(WindowProfile_SaveOrigin(*callbackWindow.ownerSession, x, y));
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 1, 1) != 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (trade.valueD67B80 != 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    switch (std::bit_cast<std::int8_t>(trade.phase)) {
    case 0:
        if (trade.valueD2E2B0 > 0) {
            WindowInstance* const window = currentWindow;
            trade.phase = 1;
            WindowTrade_SetPanelVisible(window, 14, 1);
            WindowTrade_MovePanel(currentWindow, 14, 15, 28);
            WindowTrade_SetPanelColor(currentWindow, 14, 0x00FFFFFFu);
            trade.step = 23;
        }
        break;
    case 1: {
        WindowInstance* const window = currentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(trade.step);
        auto* const handleSelectedPanelElement = window->FindElementById(WindowTradeElementId::HandleSelectedPanel);
        if (handleSelectedPanelElement) {
            if (handleSelectedPanelElement->x + step < 287) {
                WindowTrade_SetPanelVisible(window, 8, 1);
                WindowInstance* const moveWindow = currentWindow;
                const std::int32_t moveStep = std::bit_cast<std::int8_t>(trade.step);
                auto* const handleSelectedPanelElement2 = moveWindow->FindElementById(WindowTradeElementId::HandleSelectedPanel);
                if (handleSelectedPanelElement2) {
                    const std::int32_t x = std::bit_cast<std::int16_t>(
                        static_cast<std::uint16_t>(handleSelectedPanelElement2->x));
                    WindowTrade_MovePanel(moveWindow, 8, x + moveStep, 28);
                }
                const std::uint8_t oldStep = trade.step;
                WindowInstance* const firstWindow = currentWindow;
                trade.step = static_cast<std::uint8_t>(oldStep - 1);
                auto* const handleOwnPanelElement = firstWindow->FindElementById(WindowTradeElementId::HandleOwnPanel);
                if (handleOwnPanelElement) {
                    const auto* first = handleOwnPanelElement->data.graphic;
                    const auto alpha = std::max(0, static_cast<std::int32_t>(first->color >> 24) - 10);
                    WindowTrade_SetPanelColor(firstWindow, 11,
                        (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
                }
                WindowInstance* const secondWindow = currentWindow;
                auto* const handleRemoteSelectionElement = secondWindow->FindElementById(WindowTradeElementId::HandleRemoteSelection);
                if (handleRemoteSelectionElement) {
                    const auto* second = handleRemoteSelectionElement->data.graphic;
                    const auto alpha = std::min(255, static_cast<std::int32_t>(second->color >> 24) + 10);
                    WindowTrade_SetPanelColor(secondWindow, 14,
                        (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
                }
            } else {
                WindowTrade_SetPanelColor(window, 14, 0xFFFFFFFFu);
                WindowTrade_SetPanelVisible(currentWindow, 8, 1);
                WindowTrade_MovePanel(currentWindow, 8, 287, 28);
                WindowTrade_SetPanelVisible(currentWindow, 11, 0);
                if (auto* const buttonPublishElement = currentWindow->FindElementById(WindowTradeElementId::ButtonPublish); buttonPublishElement) buttonPublishElement->visible = 0;
                if (auto* const buttonConfirmElement = currentWindow->FindElementById(WindowTradeElementId::ButtonConfirm); buttonConfirmElement) buttonConfirmElement->visible = 1;
                if (auto* const buttonWithdrawElement = currentWindow->FindElementById(WindowTradeElementId::ButtonWithdraw); buttonWithdrawElement) buttonWithdrawElement->visible = 1;
                trade.phase = 3;
            }
        }
        break;
    }
    case 2: {
        WindowInstance* const window = currentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(trade.step);
        auto* const handleSelectedPanelElement = window->FindElementById(WindowTradeElementId::HandleSelectedPanel);
        if (handleSelectedPanelElement) {
            if (handleSelectedPanelElement->x - step > 15) {
                WindowTrade_SetPanelVisible(window, 8, 1);
                WindowInstance* const moveWindow = currentWindow;
                const std::int32_t moveStep = std::bit_cast<std::int8_t>(trade.step);
                auto* const handleSelectedPanelElement2 = moveWindow->FindElementById(WindowTradeElementId::HandleSelectedPanel);
                if (handleSelectedPanelElement2) {
                    const std::int32_t x = std::bit_cast<std::int16_t>(
                        static_cast<std::uint16_t>(handleSelectedPanelElement2->x));
                    WindowTrade_MovePanel(moveWindow, 8, x - moveStep, 28);
                }
                const std::uint8_t oldStep = trade.step;
                WindowInstance* const firstWindow = currentWindow;
                trade.step = static_cast<std::uint8_t>(oldStep - 1);
                auto* const handleOwnPanelElement = firstWindow->FindElementById(WindowTradeElementId::HandleOwnPanel);
                if (handleOwnPanelElement) {
                    const auto* first = handleOwnPanelElement->data.graphic;
                    const auto alpha = std::min(255, static_cast<std::int32_t>(first->color >> 24) + 10);
                    WindowTrade_SetPanelColor(firstWindow, 11,
                        (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
                }
                WindowInstance* const secondWindow = currentWindow;
                auto* const handleRemoteSelectionElement = secondWindow->FindElementById(WindowTradeElementId::HandleRemoteSelection);
                if (handleRemoteSelectionElement) {
                    const auto* second = handleRemoteSelectionElement->data.graphic;
                    const auto alpha = std::max(0, static_cast<std::int32_t>(second->color >> 24) - 10);
                    WindowTrade_SetPanelColor(secondWindow, 14,
                        (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
                }
            } else {
                WindowTrade_SetPanelColor(window, 11, 0xFFFFFFFFu);
                WindowTrade_SetPanelVisible(currentWindow, 8, 1);
                WindowTrade_MovePanel(currentWindow, 8, 15, 28);
                WindowTrade_SetPanelVisible(currentWindow, 14, 0);
                if (auto* const buttonPublishElement = currentWindow->FindElementById(WindowTradeElementId::ButtonPublish); buttonPublishElement) buttonPublishElement->visible = 1;
                if (auto* const buttonConfirmElement = currentWindow->FindElementById(WindowTradeElementId::ButtonConfirm); buttonConfirmElement) buttonConfirmElement->visible = 0;
                if (auto* const buttonWithdrawElement = currentWindow->FindElementById(WindowTradeElementId::ButtonWithdraw); buttonWithdrawElement) buttonWithdrawElement->visible = 0;
                trade.phase = 0;
            }
        }
        break;
    }
    case 3:
        if (trade.valueD2E2B0 <= 0) {
            WindowInstance* const window = currentWindow;
            trade.phase = 2;
            WindowTrade_SetPanelVisible(window, 11, 1);
            WindowTrade_MovePanel(currentWindow, 11, 287, 28);
            WindowTrade_SetPanelColor(currentWindow, 11, 0x00FFFFFFu);
            trade.step = 23;
        }
        break;
    default:
        break;
    }
    trade.preview = -1;
    return 1;
}

// 原函数：0x004B08D0。
static std::int32_t WindowTrade_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    std::int32_t preview = trade.preview;
    if (preview >= 0) {
        if (preview < 300) {
            const bool ownItem = preview < 100;
            if (!ownItem) {
                preview -= 200;
                trade.preview = preview;
            }
            if (preview >= 0) {
                const std::int32_t page = std::bit_cast<std::int8_t>(trade.tooltipPage);
                if (ownItem) {
                    static_cast<void>(WindowItem_DrawEquipmentTooltip(*callbackWindow.ownerSession, preview, page));
                } else {
                    static_cast<void>(WindowItem_DrawPetTooltip(*callbackWindow.ownerSession, 0, preview, page));
                }
            }
        } else {
            preview -= 300;
            trade.preview = preview;
            // 原4B0970在异常分类>=600时读未初始化字节；不补零或指定分支。
            std::uint8_t ownPet;
            if (preview < 100) {
                ownPet = 1;
            } else if (preview < 300) {
                ownPet = 0;
                preview -= 200;
                trade.preview = preview;
            }
            if (preview >= 0) {
                const bool left = (platform::gLegacyMouseState.mouseX < 320);
                const std::int32_t x = left ? 413 : 0;
                const std::uint8_t index = static_cast<std::uint8_t>(trade.preview);
                if (ownPet != 0) {
                    static_cast<void>(WindowTrade_DrawOwnPetTooltip(*callbackWindow.ownerSession, x, 0, index, true));
                } else {
                    static_cast<void>(WindowTrade_DrawSelectedPetTooltip(*callbackWindow.ownerSession, x, 0, 0, index));
                }
            }
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004B0F20。
static std::int32_t WindowTrade_DrawPetRow(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y, const std::int8_t side,
    const std::int32_t index, const std::uint8_t textLayer, const std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    const auto* const record = &trade.pets[index + side * 5];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used != 0) {
        constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 8, color, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
            textLayer, 8, color, record->name, 0, 0));
        std::wstring buffer;
        const auto drawNumber = [&](const std::int32_t value, const std::wstring_view format,
                                    const std::int32_t dx, const std::int32_t dy) {
            cg::text::FormatTo(buffer, format, value);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
                textLayer, 8, color, buffer, 0, 0));
        };
        drawNumber(record->level, L"{:3}", 35, 22);
        std::int16_t tribe;
        std::memcpy(&tribe, &record->tribe, sizeof(tribe));
        if (tribe >= 0 && tribe < 20) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
                textLayer, 8, color, gTribeNames57CB54[tribe], 0, 0));
        }
        drawNumber(record->maxHp, L"{:4}", 210, 5);
        drawNumber(record->maxFp, L"{:4}", 210, 22);
        drawNumber(record->combatStats[0], L"{:3}", 95, 40);
        drawNumber(record->combatStats[1], L"{:3}", 155, 40);
        drawNumber(record->combatStats[2], L"{:3}", 215, 40);
        drawNumber(record->combatStats[3], L"{:3}", 95, 55);
        drawNumber(record->combatStats[4], L"{:3}", 155, 55);
        drawNumber(record->combatStats[5], L"{:3}", 215, 55);
        drawNumber(record->value5C, L"{:2}", 45, 55);
    }
    return render::DrawGraphic3_520970(
        x, y, graphicLayer, 243141, 0, 1, 0, descriptor);
}

// TQN01：原4B1260完整524条；低W16坐标/scroll BYTE、滚动值先元素及前一滚动框坐标原序，原callback/格式历史保。
// 原函数：0x004B1260。
static std::int32_t WindowTrade_HandlePanel(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t* const backgroundImages, std::uint8_t* const tab,
    const std::int32_t scrollElement, std::int32_t* const scrollValue,
    const std::int32_t numericElement, const std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* const descriptor, const WindowTradePanelCallback callback)
{
    auto& inputs = window->ownerSession->UiFrame().native.inputs;
    if (!window) return 0;
    auto* elementsWindow = window;
    auto* const panelElement = window->FindElementById(NativeElementKey(index));
    if (!panelElement) return 0; // 当前面板没有命中区域，只结束该元素回调。
    std::uint16_t elementX;
    std::uint16_t elementY;
    std::uint16_t windowX;
    std::uint16_t windowY;
    std::memcpy(&elementX, &panelElement->x, sizeof(elementX));
    std::memcpy(&elementY, &panelElement->y, sizeof(elementY));
    std::memcpy(&windowX, &window->x, sizeof(windowX));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(elementX + windowX));
    std::memcpy(&windowY, &window->y, sizeof(windowY));
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(elementY + windowY));
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t scrollPosition = *scrollValue;
        auto* const wheelElementElement = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(scrollElement)) : nullptr;
        if (wheelElementElement) {
            WindowElement* const wheelElement = wheelElementElement;
            *scrollValue = WindowScroll_ApplyWheel(wheelElement, scrollPosition, 2, wheel);
        }
    }
    auto* const scrollElement2 = window ? window->FindElementById(NativeElementKey(scrollElement)) : nullptr;
    if (scrollElement2) {
        WindowElement* const scroll = scrollElement2;
        const auto scrollFlags = reinterpret_cast<const unsigned char*>(
            &scroll->data.mouse->flags)[0];
        if ((scrollFlags & 1u) != 0) {
            *scrollValue = WindowScroll_ReadPosition(scroll, 2);
        }
    }
    static constexpr std::array<std::array<std::int16_t, 4>, 5> rectangles54F508 {{
        {12, 240, 80, 15}, {97, 240, 80, 15}, {182, 240, 80, 15},
        {252, 28, 11, 11}, {252, 220, 11, 11},
    }};
    std::int32_t selected = -1;
    if ((state & MouseFlag_Hover) != 0) {
        for (std::int32_t part = 0; part < 5; ++part) {
            const auto& rectangle = rectangles54F508[part];
            if (WindowTrade_HitTestOwnPetRow(x + rectangle[0], y + rectangle[1], rectangle[2], rectangle[3])) {
                selected = part;
            }
        }
    }
    const std::int32_t result = state & MouseFlag_LDownEvent;
    if (result != 0 && selected >= 0 && selected <= 2) {
        static_cast<void>(audio::PlaySe_52C450(
            std::bit_cast<std::int8_t>(*tab) == selected ? 55 : 51, 320));
        *tab = static_cast<std::uint8_t>(selected);
        static_cast<void>(WindowTrade_HandleAmountInput(*window, NativeElementKey(0), 2));
    }
    if ((state & MouseFlag_LRepeatDown) != 0 && *tab == 1) {
        if (selected == 3) {
            if (*scrollValue > 0) {
                --*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t scrollPosition = *scrollValue;
            auto* const scrollTargetElement = window ? window->FindElementById(NativeElementKey(scrollElement)) : nullptr;
            if (scrollTargetElement) {
                WindowElement* const scrollTarget = scrollTargetElement;
                static_cast<void>(WindowScroll_SetPosition(scrollTarget, 2, scrollPosition));
            }
        } else if (selected == 4) {
            if (*scrollValue < 2) {
                ++*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t scrollPosition = *scrollValue;
            auto* const scrollTargetElement = window ? window->FindElementById(NativeElementKey(scrollElement)) : nullptr;
            if (scrollTargetElement) {
                WindowElement* const scrollTarget = scrollTargetElement;
                static_cast<void>(WindowScroll_SetPosition(scrollTarget, 2, scrollPosition));
            }
        }
    }
    std::uint8_t numericResult = 0;
    if (numericElement >= 0 && *tab == 2) {
        std::uint32_t color;
        color = descriptor->color;
        const std::uint8_t flags =
            window->ownerSession->UiFrame().inputState.currentContext == &inputs.tradeAmount ? 3 : 1;
        numericResult = WindowInput_HandleNumber(
            window, numericElement, state, maximum, 223, gTradeNumberHints5805FC.data(),
            flags, color);
    }
    std::int32_t displayAmount; // 原回调负责写入，不为无回调的异常路径补零。
    if (callback != nullptr) {
        // 仅适配已证不消费该参数的-1哨兵；正索引仍借用同一W32，见窗口SPEC。
        auto* const valueSlotElement = window ? window->FindElementById(NativeElementKey(numericElement)) : nullptr;
        if (valueSlotElement) {
            std::uint32_t* const valueSlot =
                numericElement == -1 ? nullptr : &valueSlotElement->state;
            const std::int32_t scrollSnapshot = *scrollValue;
            const std::int32_t mouseY =
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
            const std::int32_t mouseX =
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
            callback(*window->ownerSession, x, y, mouseX, mouseY, state, tab, scrollSnapshot, numericResult, valueSlot,
                     &itemCount, &petCount, &amount, &displayAmount, descriptor);
        }
    }
    std::int32_t imageNo = *tab == 0 ? 243028 : (selected == 0 ? 243029 : 243027);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 12, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 1 ? 243022 : (selected == 1 ? 243023 : 243021);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 97, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 2 ? 243025 : (selected == 2 ? 243026 : 243024);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 182, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(scrollElement)) : nullptr; indexedElement3) indexedElement3->visible = 0;
    if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(scrollElement - 1)) : nullptr; indexedElement4) indexedElement4->visible = 0;
    if (*tab == 1) {
        auto* const previousElement = window ? window->FindElementById(NativeElementKey(scrollElement - 1)) : nullptr;
        if (previousElement) {
            WindowElement* const previous = previousElement;
            const std::int32_t currentWindowY = window->y;
            const std::int32_t previousY = previous->y;
            const std::int32_t drawY = currentWindowY + previousY;
            const std::int32_t previousX = previous->x;
            const std::int32_t currentWindowX = window->x;
            const std::int32_t drawX = currentWindowX + previousX;
            static_cast<void>(render::DrawGraphic3_520970(
                drawX, drawY, 223, 243096, 0, 1, 0, descriptor));
        }
        imageNo = selected == 3 ? ((state & MouseFlag_LHoldPhysical) != 0 ? 243091 : 243092) : 243090;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 28, 223, imageNo, 0, 1, 0, descriptor));
        imageNo = selected == 4 ? ((state & MouseFlag_LHoldPhysical) != 0 ? 243094 : 243095) : 243093;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 220, 223, imageNo, 0, 1, 0, descriptor));
        if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(scrollElement)) : nullptr; indexedElement5) indexedElement5->visible = 1;
        if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(scrollElement - 1)) : nullptr; indexedElement6) indexedElement6->visible = 1;
    }
    std::wstring buffer;
    if (*tab == 2) {
        cg::text::FormatTo(buffer, L"{:9}", displayAmount);
        WindowNumber_Draw(x + 179, y + 88, buffer, 3, 0, 1, 223);
    }
    cg::text::FormatTo(buffer, L"{:3}", itemCount);
    WindowNumber_Draw(x + 71, y + 258, buffer, 3, 0, 1, 223);
    cg::text::FormatTo(buffer, L"{:3}", petCount);
    WindowNumber_Draw(x + 156, y + 258, buffer, 3, 0, 1, 223);
    cg::text::FormatTo(buffer, L"{:10}", amount);
    WindowNumber_Draw(x + 241, y + 258, buffer, 3, 0, 1, 223);
    imageNo = backgroundImages[std::bit_cast<std::int8_t>(*tab)];
    static_cast<void>(render::DrawGraphic3_520970(
        x, y, 223, imageNo, 0, 1, 0, descriptor));
    return result;
}

// 原函数：0x004B1940。
static void WindowTrade_HandleOwnInventory(
    session::ClientSession& session,
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& trade = session.UiFrame().native.trade;
    auto& uiFrame = session.UiFrame();
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0x80FFFFFFu;
    dimmed.color = color;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(session) != 2) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (trade.phase != 0) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX(session);
        const std::int32_t dropY = WindowDrag_ReadY(session);
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 8) {
                WindowItem_RequestTransfer(session, 43, 11, source, position + 8);
            }
            break;
        }
        WindowDrag_Reset(session);
    }
    if (*tab != 0) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0;
    if (hover) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position + 8;
                break;
            }
        }
        if (hovered >= 0 && trade.phase == 0
            && session.WorldScene().playerItems[hovered].used != 0) {
            if ((session.WorldScene().playerItems[hovered].flags & 1u) == 0) {
                uiFrame.hoverText = L"此物品无法交易。";
            } else if (trade.itemMarks[hovered] == 0) {
                uiFrame.hoverText = L"用滑鼠拖放方式，登录此笔物品交易。";
            }
        }
    }
    if (trade.valueD2EA20 == 0) {
        if (trade.hoverSlotCB0A4C != hovered) {
            trade.tooltipPage = 0;
        }
        trade.hoverSlotCB0A4C = hovered;
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0
        && (WindowDrag_ReadFocusWindowId(session) == 43 || WindowDrag_ReadFocusWindowId(session) == 0)) {
        if (session.WorldScene().playerItems[hovered].used != 0
            && (session.WorldScene().playerItems[hovered].flags & 1u) != 0
            && trade.itemMarks[hovered] == 0) {
            std::int32_t target = 0;
            while (target < 20 && WindowTrade_ReadSelectedItem(session, target) >= 0) {
                ++target;
            }
            if (target < 20) {
                static_cast<void>(WindowTrade_RemoveItem(session, target));
                static_cast<void>(WindowTrade_AddItem(session, target, hovered));
                *tab = 0;
            }
            if (trade.valueC8AF81 != 0) {
                trade.valueC8AF81 = 0;
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if ((events & MouseFlag_Hover) != 0 && hovered >= 0) {
        if (trade.phase == 0 && (events & MouseFlag_LDownEvent) != 0
            && session.WorldScene().playerItems[hovered].used != 0
            && trade.itemMarks[hovered] == 0) {
            static_cast<void>(WindowItemDrag_Start(session, hovered, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & MouseFlag_RDownEvent) != 0 && session.WorldScene().playerItems[hovered].used != 0) {
            const std::int32_t pages = session.WorldScene().playerItems[hovered].pages;
            trade.tooltipPage = static_cast<std::uint8_t>(trade.tooltipPage + 1);
            if (std::bit_cast<std::int8_t>(trade.tooltipPage) >= pages) {
                trade.tooltipPage = 0;
            }
        }
    }
    if (WindowDrag_ReadKind(session) == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        if (WindowDrag_ReadFocusWindowId(session) == 43 && WindowDrag_ReadFocusElement(session) == static_cast<ElementId>(WindowTradeElementId::HandleOwnPanel)) {
            const std::int32_t left = x + 50 * ((source - 8) % 5) + 13;
            const std::int32_t top = y + 52 * ((source - 8) / 5) + 26;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, 254, 0);
        }
    }
    std::wstring buffer;
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * ((hovered - 8) % 5) + 13;
        const std::int32_t top = y + 52 * ((hovered - 8) / 5) + 26;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        const world::PlayerItemRecord& item = session.WorldScene().playerItems[hovered];
        if (item.used != 0) {
            const std::size_t memoLength = item.memo.size();
            trade.preview = hovered;
            if (memoLength != 0 && (item.flags & 0x100u) != 0) {
                uiFrame.hoverText = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                uiFrame.hoverText = item.memo + L"[han]";
            }
        }
        trade.valueD2EA20 = 0;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    dimmed = *descriptor;
    reinterpret_cast<unsigned char*>(&dimmed.color)[3] >>= 1;
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = position + 8;
        const world::PlayerItemRecord& item = session.WorldScene().playerItems[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if (std::bit_cast<std::int8_t>(trade.itemMarks[slot]) > 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 21), static_cast<std::uint16_t>(centerY - 21),
                static_cast<std::uint16_t>(centerX + 21), static_cast<std::uint16_t>(centerY + 21),
                223, 251, 0);
        } else if (occupancy[slot] != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 24), static_cast<std::uint16_t>(centerY - 24),
                static_cast<std::uint16_t>(centerX + 24), static_cast<std::uint16_t>(centerY + 24),
                223, 249, 0);
        }
        if ((item.flags & 1u) == 0) {
            static_cast<void>(cg::render::DrawGraphic(centerX, centerY, 223, 23040, 0, nullptr));
        }
        const render::GraphicDescriptor16* const mode =
            std::bit_cast<std::int8_t>(trade.itemMarks[slot]) > 0 || occupancy[slot] != 0
            ? &dimmed : descriptor;
        static_cast<void>(cg::render::DrawGraphic(centerX, centerY, 223, item.imageNo, 0, mode));
        const std::int32_t count = item.count;
        if (count > 0) {
            cg::text::FormatTo(buffer, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

// 原函数：0x004B2060。
static void WindowTrade_HandleOwnPets(
    session::ClientSession& session,
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    auto& uiFrame = session.UiFrame();
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(session) != 3) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (trade.phase != 0) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX(session);
        const std::int32_t dropY = WindowDrag_ReadY(session);
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        // 原三坐标却扫描五次，沿既定异常边界，不补坐标或模拟栈残值。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t position = scroll + row;
            if (position >= 0) {
                const std::int32_t target =
                    std::bit_cast<std::int16_t>(session.WorldScene().petPairs[position].index);
                if (trade.petMarks[target] == 0) {
                    WindowItem_RequestPetTransfer(session, 43, 11, source, target);
                }
            }
            break;
        }
        WindowDrag_Reset(session);
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (cg::ui::HitTestMouseRect_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    std::int32_t petIndex = -1;
    if (hovered >= 0) {
        petIndex = std::bit_cast<std::int16_t>(session.WorldScene().petPairs[hovered].index);
        if (petIndex >= 0) {
            uiFrame.hoverText = L"用滑鼠拖放方式，登录此笔宠物交易。";
            if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0
                && (WindowDrag_ReadFocusWindowId(session) == 43 || WindowDrag_ReadFocusWindowId(session) == 0)) {
                if ((*session.WorldScene().petRecords)[petIndex].used != 0
                    && trade.petMarks[petIndex] == 0) {
                    std::int32_t target = 0;
                    while (target < 5 && WindowTrade_ReadSelectedPet(session, target) >= 0) {
                        ++target;
                    }
                    if (target < 5) {
                        static_cast<void>(WindowTrade_RemovePet(session, target));
                        static_cast<void>(WindowTrade_AddPet(session, target, petIndex));
                        *tab = 1;
                    }
                    if (trade.valueC8AF81 != 1) {
                        trade.valueC8AF81 = 1;
                    }
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
        }
    }
    if ((events & MouseFlag_Hover) != 0 && petIndex >= 0 && trade.phase == 0
        && (events & MouseFlag_LDownEvent) != 0 && (*session.WorldScene().petRecords)[petIndex].used != 0
        && trade.petMarks[petIndex] == 0) {
        static_cast<void>(WindowPetDrag_Start(session, petIndex, 1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (WindowDrag_ReadKind(session) == 3) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        if (WindowDrag_ReadFocusWindowId(session) == 43 && WindowDrag_ReadFocusElement(session) == static_cast<ElementId>(WindowTradeElementId::HandleOwnPanel)) {
            const std::int32_t row = WindowItem_FindSortedPetIndex(session, source) - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            if ((*session.WorldScene().petRecords)[petIndex].used != 0) {
                trade.preview = petIndex + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t slot =
            std::bit_cast<std::int16_t>(session.WorldScene().petPairs[scroll + row].index);
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        if (std::bit_cast<std::int8_t>(trade.petMarks[slot]) > 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 238), static_cast<std::uint16_t>(top + 67),
                223, 251, 0);
        }
        const std::int32_t current =
            std::bit_cast<std::int16_t>(session.WorldScene().petPairs[scroll + row].index);
        WindowBank_DrawOwnPetTooltip(session,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), current, 3);
        static_cast<void>(cg::render::DrawGraphic(
            left + 120, top + 35, 223, 243141, 0, descriptor));
    }
}

// 原函数：0x004B2480。
static std::int32_t WindowTrade_HandleOwnPanel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        const auto* const graphic = graphicElement->data.graphic;
        const std::uint32_t color = graphic->color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        descriptor.color = color;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F530 {243143, 243145, 243144};
    const std::int32_t amount = callbackWindow.ownerSession->WorldScene().player->valueFC;
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F530.data(), &trade.valueD6742C,
        10, &trade.scrollCF3B0C, 5, amount, 0, 0, amount, &descriptor, WindowTrade_RefreshOwnPanel);
}

// 原函数：0x004B2500。
static void WindowTrade_RefreshOwnPanel(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t* const valueSlot,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t*,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    const bool confirmed = (numericResult & 2u) != 0;
    if (confirmed && trade.valueC8AF81 != 2) {
        trade.valueC8AF81 = 2;
    }
    WindowTrade_HandleOwnInventory(session, tab, x, y, mouseX, mouseY, state, descriptor);
    WindowTrade_HandleOwnPets(session, tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    if (confirmed) {
        trade.valueD046D0 = std::bit_cast<std::int32_t>(*valueSlot);
        *valueSlot = 0;
    }
    *displayAmount = std::bit_cast<std::int32_t>(*valueSlot);
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        std::uint16_t used;
        std::memcpy(&used, &trade.items[position + 20], sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        std::uint16_t used;
        std::memcpy(&used, &trade.pets[position + 5], sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
}

// 原函数：0x004B25D0。
static void WindowTrade_HandleOwnItemDrag(
    session::ClientSession& session,
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(WindowTrade_RemoveItem(session, sourceIndex));
        }
        return;
    }
    if (WindowTrade_ReadSelectedItem(session, sourceIndex) < 0
        || session.WorldScene().playerItems[WindowTrade_ReadSelectedItem(session, sourceIndex)].used == 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
        static_cast<void>(WindowDrag_StartAtPosition(session, 6, sourceIndex, x, y, WindowTrade_HandleOwnItemDrag));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        WindowDrag_Cancel(session);
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t imageNo =
        session.WorldScene().playerItems[WindowTrade_ReadSelectedItem(session, sourceIndex)].imageNo;
    const std::int32_t y =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

// 原函数：0x004B2E00。
static void WindowTrade_HandleOwnPetDrag(
    session::ClientSession& session,
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(WindowTrade_RemovePet(session, sourceIndex));
        }
        return;
    }
    if (WindowTrade_ReadSelectedPet(session, sourceIndex) < 0
        || (*session.WorldScene().petRecords)[WindowTrade_ReadSelectedPet(session, sourceIndex)].used == 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
        static_cast<void>(WindowDrag_StartAtPosition(session, 7, sourceIndex, x, y, WindowTrade_HandleOwnPetDrag));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        WindowDrag_Cancel(session);
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t slot = WindowTrade_ReadSelectedPet(session, sourceIndex);
    const auto petY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        (platform::gLegacyMouseState.mouseY - 35)));
    const auto petX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        (platform::gLegacyMouseState.mouseX - 120)));
    WindowBank_DrawOwnPetTooltip(session, petX, petY, slot, 5);
    const std::int32_t y =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
}

// 原函数：0x004B2710。
static std::int32_t WindowTrade_StartOwnItemDrag(session::ClientSession& session, const std::int32_t index)
{
    return WindowDrag_Start(session, 6, index, WindowTrade_HandleOwnItemDrag);
}

// 原函数：0x004B2F50。
static std::int32_t WindowTrade_StartOwnPetDrag(session::ClientSession& session, const std::int32_t index)
{
    return WindowDrag_Start(session, 7, index, WindowTrade_HandleOwnPetDrag);
}

// 原函数：0x004B2730。
static void WindowTrade_HandleSelectedItem(
    session::ClientSession& session,
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    auto& uiFrame = session.UiFrame();
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(session) != 2 && WindowDrag_ReadKind(session) != 6) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (trade.phase != 0) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (*tab != 0) {
        if ((events & MouseFlag_DragFinish) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex(session);
            if (WindowItem_ClassifyItemSlot(source) == 2) {
                std::int32_t target = 0;
                while (target < 20 && WindowTrade_ReadSelectedItem(session, target) >= 0) {
                    ++target;
                }
                if (target < 20 && (session.WorldScene().playerItems[source].flags & 1u) != 0) {
                    static_cast<void>(WindowTrade_RemoveItem(session, target));
                    static_cast<void>(WindowTrade_AddItem(session, target, source));
                    *tab = 0;
                }
            }
            WindowDrag_Reset(session);
        }
        if ((events & MouseFlag_Dragging) != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
        return;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX(session);
        const std::int32_t dropY = WindowDrag_ReadY(session);
        const std::int32_t kind = WindowDrag_ReadKind(session);
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        for (std::int32_t target = 0; target < 20; ++target) {
            const std::int32_t left = x + 50 * (target % 5) + 13;
            const std::int32_t top = y + 52 * (target / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (kind == 2) {
                if (WindowItem_ClassifyItemSlot(source) == 2
                    && (session.WorldScene().playerItems[source].flags & 1u) != 0) {
                    static_cast<void>(WindowTrade_RemoveItem(session, target));
                    static_cast<void>(WindowTrade_AddItem(session, target, source));
                }
            } else if (kind == 6) {
                static_cast<void>(WindowTrade_SwapSelectedItems(session, source, target));
            }
            WindowDrag_Reset(session);
            break;
        }
    }
    std::int32_t hovered = -1;
    const bool hover = (events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0;
    if (hover) {
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const std::int32_t left = x + 50 * (slot % 5) + 13;
            const std::int32_t top = y + 52 * (slot / 5) + 26;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = slot;
                break;
            }
        }
    }
    const std::uint8_t phase = trade.phase;
    if (phase == 0 && hovered >= 0) {
        uiFrame.hoverText = L"用滑鼠拖放方式，取消此笔物品交易。";
    }
    if (trade.valueD2EA20 == 1) {
        if (trade.hoverSlotC8AA78 != hovered) {
            trade.tooltipPage = 0;
        }
        trade.hoverSlotC8AA78 = hovered;
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0
        && (phase != 0 || ((WindowDrag_ReadFocusWindowId(session) == 43 || WindowDrag_ReadFocusWindowId(session) == 0)
                           && WindowTrade_RemoveItem(session, hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & MouseFlag_Hover) != 0 && hovered >= 0 && WindowTrade_ReadSelectedItem(session, hovered) >= 0) {
        if (trade.phase == 0 && (events & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(WindowTrade_StartOwnItemDrag(session, hovered));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & MouseFlag_RDownEvent) != 0) {
            trade.tooltipPage = static_cast<std::uint8_t>(trade.tooltipPage + 1);
            const std::int32_t slot = WindowTrade_ReadSelectedItem(session, hovered);
            if (std::bit_cast<std::int8_t>(trade.tooltipPage)
                >= session.WorldScene().playerItems[slot].pages) {
                trade.tooltipPage = 0;
            }
        }
    }
    if (WindowDrag_ReadKind(session) == 6 && WindowDrag_ReadFocusWindowId(session) == 43 && WindowDrag_ReadFocusElement(session) == static_cast<ElementId>(WindowTradeElementId::HandleSelectedPanel)) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        if (WindowTrade_ReadSelectedItem(session, source) >= 0
            && session.WorldScene().playerItems[WindowTrade_ReadSelectedItem(session, source)].used != 0) {
            const std::int32_t left = x + 50 * (source % 5) + 13;
            const std::int32_t top = y + 52 * (source / 5) + 26;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    std::wstring buffer;
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * (hovered % 5) + 13;
        const std::int32_t top = y + 52 * (hovered / 5) + 26;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        const std::int32_t slot = WindowTrade_ReadSelectedItem(session, hovered);
        if (slot >= 0) {
            const world::PlayerItemRecord& item = session.WorldScene().playerItems[slot];
            if (item.used != 0) {
                trade.preview = slot;
                if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                    uiFrame.hoverText = item.name + L"[kok]";
                } else if ((item.flags & 0x200u) != 0) {
                    uiFrame.hoverText = item.memo + L"[han]";
                }
            }
        }
        trade.valueD2EA20 = 1;
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = WindowTrade_ReadSelectedItem(session, position);
        if (slot >= 0 && session.WorldScene().playerItems[slot].used != 0
            && (session.WorldScene().playerItems[slot].flags & 1u) == 0) {
            static_cast<void>(WindowTrade_RemoveItem(session, position));
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = WindowTrade_ReadSelectedItem(session, position);
        if (slot < 0) {
            continue;
        }
        const world::PlayerItemRecord& item = session.WorldScene().playerItems[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if ((item.flags & 1u) == 0) {
            static_cast<void>(cg::render::DrawGraphic(
                centerX, centerY, 223, 23040, 0, nullptr));
        }
        static_cast<void>(cg::render::DrawGraphic(
            centerX, centerY, 223, item.imageNo, 0, descriptor));
        const std::int32_t count = item.count;
        if (count > 0) {
            cg::text::FormatTo(buffer, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

// 原函数：0x004B2F70。
static void WindowTrade_HandleSelectedPet(
    session::ClientSession& session,
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    auto& uiFrame = session.UiFrame();
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(session) != 3 && WindowDrag_ReadKind(session) != 7) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (trade.phase != 0) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if (*tab != 1) {
        if ((events & MouseFlag_DragFinish) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex(session);
            if (WindowItem_ClassifyPetSlot(source) == 1) {
                std::int32_t target = 0;
                while (target < 5 && WindowTrade_ReadSelectedPet(session, target) >= 0) {
                    ++target;
                }
                if (target < 5) {
                    static_cast<void>(WindowTrade_RemovePet(session, target));
                    static_cast<void>(WindowTrade_AddPet(session, target, source));
                    *tab = 1;
                }
            }
            WindowDrag_Reset(session);
        }
        if ((events & MouseFlag_Dragging) != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
        return;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX(session);
        const std::int32_t dropY = WindowDrag_ReadY(session);
        const std::int32_t kind = WindowDrag_ReadKind(session);
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        // 原仅初始化三项却扫描五次；不补坐标或模拟x86栈残值，异常边界见SPEC。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t target = scroll + row;
            if (target >= 0) {
                if (kind == 3) {
                    if (WindowItem_ClassifyPetSlot(source) == 1) {
                        static_cast<void>(WindowTrade_RemovePet(session, target));
                        static_cast<void>(WindowTrade_AddPet(session, target, source));
                    }
                } else if (kind == 7) {
                    static_cast<void>(WindowTrade_SwapSelectedPets(session, source, target));
                }
                WindowDrag_Reset(session);
            }
            break;
        }
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (cg::ui::HitTestMouseRect_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    const std::uint8_t phase = trade.phase;
    if (phase == 0 && hovered >= 0) {
        uiFrame.hoverText = L"用滑鼠拖放方式，取消此笔宠物交易。";
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0
        && (phase != 0 || ((WindowDrag_ReadFocusWindowId(session) == 43 || WindowDrag_ReadFocusWindowId(session) == 0)
                           && WindowTrade_RemovePet(session, hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & MouseFlag_Hover) != 0 && hovered >= 0 && WindowTrade_ReadSelectedPet(session, hovered) >= 0
        && trade.phase == 0 && (events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(WindowTrade_StartOwnPetDrag(session, hovered));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (WindowDrag_ReadKind(session) == 7 && WindowDrag_ReadFocusWindowId(session) == 43 && WindowDrag_ReadFocusElement(session) == static_cast<ElementId>(WindowTradeElementId::HandleSelectedPanel)) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(session);
        if (WindowTrade_ReadSelectedPet(session, source) >= 0
            && (*session.WorldScene().petRecords)[WindowTrade_ReadSelectedPet(session, source)].used != 0) {
            const std::int32_t row = source - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            const std::int32_t slot = WindowTrade_ReadSelectedPet(session, hovered);
            if (slot >= 0 && (*session.WorldScene().petRecords)[slot].used != 0) {
                trade.preview = slot + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        const std::int32_t slot = WindowTrade_ReadSelectedPet(session, scroll + row);
        if (slot >= 0 && (*session.WorldScene().petRecords)[slot].used != 0) {
            WindowBank_DrawOwnPetTooltip(session,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), slot, 3);
            static_cast<void>(cg::render::DrawGraphic(
                left + 120, top + 35, 223, 243141, 0, descriptor));
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                left, top, 223, 243141, 0, 1, 0, descriptor));
        }
    }
}

// 原函数：0x004B3430。
static std::int32_t WindowTrade_HandleSelectedPanel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        const auto* const graphic = graphicElement->data.graphic;
        const std::uint32_t color = graphic->color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        descriptor.color = color;
    }
    if (trade.valueD2E2B0 == 2) {
        reinterpret_cast<unsigned char*>(&descriptor.color)[3] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F53C {244367, 244371, 244369};
    const std::int32_t amount = callbackWindow.ownerSession->WorldScene().player->valueFC;
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F53C.data(), &trade.valueC8AF81,
        7, &trade.scrollCC4C90, -1, 0, 0, 0, amount, &descriptor, WindowTrade_RefreshSelectedPanel);
}

// 原函数：0x004B34C0。
static void WindowTrade_RefreshSelectedPanel(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY, const std::int32_t state,
    std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    if ((numericResult & 2u) != 0 && trade.valueD6742C != 2) {
        trade.valueD6742C = 2;
    }
    WindowTrade_HandleSelectedItem(session, tab, x, y, mouseX, mouseY, state, descriptor);
    WindowTrade_HandleSelectedPet(session, tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    *displayAmount = trade.valueD046D0;
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        if (WindowTrade_ReadSelectedItem(session, position) >= 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        if (WindowTrade_ReadSelectedPet(session, position) >= 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = trade.valueD046D0;
}

// 原函数：0x004B3580。
static std::int32_t WindowTrade_HandleRemoteSelection(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        const auto* const graphic = graphicElement->data.graphic;
        const std::uint32_t color = graphic->color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        descriptor.color = color;
    }
    if (trade.valueCC4224 == 2) {
        reinterpret_cast<unsigned char*>(&descriptor.color)[3] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F548 {244368, 244372, 244370};
    const std::int32_t amount = callbackWindow.ownerSession->WorldScene().player->valueFC;
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F548.data(), &trade.tabCC4D1A,
        13, &trade.scrollD67430, -1, 0, 0, 0, amount, &descriptor, WindowTrade_HandleRemoteItem);
}

// 原函数：0x004B3610。
static void WindowTrade_HandleRemoteItem(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y, const std::int32_t, const std::int32_t,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    auto& trade = session.UiFrame().native.trade;
    auto& uiFrame = session.UiFrame();
    const std::uint8_t page = *tab;
    std::wstring buffer;
    if (page == 0) {
        std::int32_t hovered = -1;
        if ((state & MouseFlag_Hover) != 0) {
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const std::int32_t left = x + 50 * (slot % 5) + 13;
                const std::int32_t top = y + 52 * (slot / 5) + 26;
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = slot;
                    std::uint16_t used;
                    std::memcpy(&used, &trade.items[slot], sizeof(used));
                    if (used != 0) {
                        uiFrame.hoverText = L"以此物品向对方提出交易。";
                    }
                    break;
                }
            }
        }
        if (trade.valueD2EA20 == 2) {
            if (trade.hoverSlotC8B314 != hovered) {
                trade.tooltipPage = 0;
            }
            trade.hoverSlotC8B314 = hovered;
        }
        if (hovered >= 0 && (state & MouseFlag_RDownEvent) != 0) {
            const auto* const record = &trade.items[hovered];
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t pages;
                std::memcpy(&pages, &record->pages, sizeof(pages));
                trade.tooltipPage = static_cast<std::uint8_t>(trade.tooltipPage + 1);
                if (std::bit_cast<std::int8_t>(trade.tooltipPage) >= pages) {
                    trade.tooltipPage = 0;
                }
            }
        }
        if ((state & MouseFlag_Hover) != 0 && hovered >= 0) {
            const std::int32_t left = x + 50 * (hovered % 5) + 13;
            const std::int32_t top = y + 52 * (hovered / 5) + 25;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            const auto* const record = &trade.items[hovered];
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                const auto& memo = record->memo;
                const std::size_t memoLength = memo.size();
                trade.preview = hovered + 200;
                bool showName = false;
                if (memoLength != 0) {
                    std::uint32_t flags;
                    std::memcpy(&flags, &record->flags, sizeof(flags));
                    showName = (flags & 0x100u) != 0;
                }
                if (showName) {
                    cg::text::FormatTo(buffer, L"{}[kok]", record->name);
                    uiFrame.hoverText = buffer;
                } else {
                    std::uint32_t flags;
                    std::memcpy(&flags, &record->flags, sizeof(flags));
                    if ((flags & 0x200u) != 0) {
                        cg::text::FormatTo(buffer, L"{}[han]", memo);
                        uiFrame.hoverText = buffer;
                    }
                }
            }
            trade.valueD2EA20 = 2;
        }
        static_cast<void>(render::ClearTextQueueTail_456D50(3));
        static_cast<void>(render::DrawText_456780(
            0, 0, 3, 1, 0, L"", 0, 0));
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const auto* const record = &trade.items[slot];
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used == 0) {
                continue;
            }
            const std::int32_t centerX = x + 50 * (slot % 5) + 37;
            const std::int32_t centerY = y + 52 * (slot / 5) + 51;
            std::int16_t value02;
            std::memcpy(&value02, &record->selection, sizeof(value02));
            if (value02 < 0) {
                static_cast<void>(cg::render::DrawGraphic(
                    centerX, centerY, 223, 23040, 0, nullptr));
            }
            std::int32_t imageNo;
            std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
            static_cast<void>(cg::render::DrawGraphic(
                centerX, centerY, 223, imageNo, 0, descriptor));
            std::int32_t count;
            std::memcpy(&count, &record->count, sizeof(count));
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    } else if (page == 1) {
        constexpr std::array<std::int16_t, 3> rowY {26, 95, 164};
        if ((state & MouseFlag_Hover) != 0) {
            for (std::int32_t row = 0; row < 3; ++row) {
                const std::int32_t top = y + rowY[row];
                if (cg::ui::HitTestMouseRect_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                    const std::int32_t slot = scroll + row;
                    if (slot >= 0) {
                        std::uint16_t used;
                        std::memcpy(&used, &trade.pets[slot], sizeof(used));
                        if (used != 0) {
                            uiFrame.hoverText = L"以此宠物向对方提出交易。";
                            trade.preview = slot + 500;
                        }
                    }
                    break;
                }
            }
        }
        for (std::int32_t row = 0; row < 3; ++row) {
            static_cast<void>(WindowTrade_DrawPetRow(session,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 11)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + rowY[row])),
                0, scroll + row, 3, 223, descriptor));
        }
    }
    *displayAmount = trade.valueCF4688;
    std::int32_t items = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, &trade.items[slot], sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, &trade.pets[slot], sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = trade.valueCF4688;
}

// 原函数：0x004B1900。
static bool WindowTrade_HitTestOwnPetRow(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t relativeX =
        (platform::gLegacyMouseState.mouseX - static_cast<std::int32_t>(x));
    if (relativeX < 0 || relativeX >= width) {
        return false;
    }
    const std::int32_t relativeY =
        (platform::gLegacyMouseState.mouseY - static_cast<std::int32_t>(y));
    return relativeY >= 0 && relativeY < height;
}

// 原函数：0x004B09E0。
static std::int32_t WindowTrade_HandleAmountInput(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    if (trade.valueD6742C == 2) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowInstance* const window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Trade].window;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.tradeAmountPrototype, 0, -100, 0));
            input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.tradeAmount);
            auto* const handleAmountInputElement = window ? window->FindElementById(WindowTradeElementId::HandleAmountInput) : nullptr;
            if (handleAmountInputElement) {
                callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handleAmountInputElement->data.input;
            }
            auto* const handleAmountInputElement2 = window ? window->FindElementById(WindowTradeElementId::HandleAmountInput) : nullptr;
            if (handleAmountInputElement2) {
                auto* const data = handleAmountInputElement2->data.input;
                data->context = &inputs.tradeAmount;
            }
        }
    } else if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.tradeAmount) {
        static_cast<void>(WindowChat_RestoreInput(*callbackWindow.ownerSession));
    }
    return 0;
}

// 原函数：0x004B4070。
static std::int32_t WindowTrade_DrawRemotePetSkillRow(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t skill)
{
    auto& storage = session.UiFrame().native.storage;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const wchar_t* const text = storage.pets[index].techniques[skill].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return cg::render::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 银行／交易第三页复用技能行底图；名称已转换为UTF-16，不显示物品属性。
static void WindowTrade_DrawPetEquipmentRow(
    const std::int32_t x, const std::int32_t y, const std::wstring_view name)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = 0xFFFFFFFFu;
    descriptor.mixFlags = render::GraphicMixFlag_OriginalPalette;
    if (!name.empty()) {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(0, 0, 5, 1, 0, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
            5, 12, 0, name, 0, 0));
    }
    static_cast<void>(render::DrawGraphic(x + 105, y + 8, 0xE6, 0x3B59C, 0, &descriptor));
}

// 银行／交易两页技能及一页装备的页脚，绘制在十行列表下方。
static void WindowTrade_DrawPetPageHint(
    const std::int32_t x, const std::int32_t y, const std::int32_t page)
{
    constexpr std::array<std::wstring_view, 3> hints {
        L"技能1/3页 按PageDown键看下一页",
        L"2/3页 PageUp上页 PageDown装备",
        L"装备3/3页 按PageUp键看上一页",
    };
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 10), static_cast<std::int16_t>(y + 303),
        5, 12, 0, hints[page], 0, 0));
}

// 原函数：0x004B4140。
std::int32_t WindowTrade_DrawRemotePetTooltip(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& storage = session.UiFrame().native.storage;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const auto* const record = &storage.pets[shortIndex];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, L"", 0, 0));
    const wchar_t* const title = record->speciesName.c_str();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, &record->baseStats[row], sizeof(value));
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    std::int32_t maximumWeight;
    std::memcpy(&maximumWeight, &record->maxWeight, sizeof(maximumWeight));
    if (maximumWeight > 0) {
        std::int32_t weight;
        std::memcpy(&weight, &record->weight, sizeof(weight));
        cg::text::FormatTo(buffer, L"{:3} / {:3}", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            31, 5, 12, 0, buffer, 0, 0));
    }
    if (interaction.previewAction == nullptr) {
        interaction.previewAction = cg::yobi::NewYobiAction_404AE0(session, 2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
    cg::yobi::ConfigureWindowPetPreview(interaction.previewAction, imageNo);
    static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(interaction.previewAction, 100, 0, 0));
    cg::yobi::YobiAction* const action = interaction.previewAction;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        cg::yobi::GetYobiActionDrawMode(action);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, &record->elements[row], sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    const std::int32_t page = WindowPetSkills_GetPreviewPage(session, record, 3);
    for (std::int32_t row = 0; row < 10; ++row) {
        if (page == 2) {
            const std::wstring name = row < static_cast<std::int32_t>(record->equipmentNames.size())
                ? record->equipmentNames[row] : std::wstring{};
            WindowTrade_DrawPetEquipmentRow(shortX + 10, shortY + 141 + row * 16, name);
        } else {
            static_cast<void>(WindowTrade_DrawRemotePetSkillRow(
                session,
                shortX + 10, shortY + 141 + row * 16, shortIndex, page * 10 + row));
        }
    }
    WindowTrade_DrawPetPageHint(shortX, shortY, page);
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004B45A0。
static std::int32_t WindowTrade_DrawSelectedPetSkillRow(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y, const std::int32_t side,
    const std::int32_t index, const std::int32_t skill)
{
    auto& trade = session.UiFrame().native.trade;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const wchar_t* const text =
        trade.pets[index + shortSide * 5].techniques[skill].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return cg::render::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 原函数：0x004B4680。
static std::int32_t WindowTrade_DrawSelectedPetTooltip(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t index)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& trade = session.UiFrame().native.trade;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::int32_t slot = shortIndex + shortSide * 5;
    const auto* const record = &trade.pets[slot];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return slot;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, L"", 0, 0));
    const wchar_t* const title = record->speciesName.c_str();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, &record->baseStats[row], sizeof(value));
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    if (interaction.previewAction == nullptr) {
        interaction.previewAction = cg::yobi::NewYobiAction_404AE0(session, 2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
    cg::yobi::ConfigureWindowPetPreview(interaction.previewAction, imageNo);
    static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(interaction.previewAction, 100, 0, 0));
    cg::yobi::YobiAction* const action = interaction.previewAction;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        cg::yobi::GetYobiActionDrawMode(action);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, &record->elements[row], sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    const std::int32_t page = WindowPetSkills_GetPreviewPage(session, record, 3);
    for (std::int32_t row = 0; row < 10; ++row) {
        if (page == 2) {
            const std::wstring name = row < static_cast<std::int32_t>(record->equipmentNames.size())
                ? record->equipmentNames[row] : std::wstring{};
            WindowTrade_DrawPetEquipmentRow(shortX + 10, shortY + 141 + row * 16, name);
        } else {
            static_cast<void>(WindowTrade_DrawSelectedPetSkillRow(session,
                shortX + 10, shortY + 141 + row * 16, side, shortIndex, page * 10 + row));
        }
    }
    WindowTrade_DrawPetPageHint(shortX, shortY, page);
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004B3B00。
static std::int32_t WindowTrade_DrawOwnPetSkillRow(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t petIndex, const std::int32_t skillPosition)
{
    render::GraphicDescriptor16 descriptor {};
    const auto& pet = (*session.WorldScene().petRecords)[petIndex];
    const std::int16_t skillIndex =
        std::bit_cast<std::int16_t>(pet.techniquePairs650[skillPosition].index);
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const wchar_t* const text = pet.techniquesD8[skillIndex].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return cg::render::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 原函数：0x004B3BF0。
std::int32_t WindowTrade_DrawOwnPetTooltip(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y, const std::int32_t index,
    const bool showEquipmentPage)
{
    auto& interaction = session.UiFrame().native.interaction;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const auto& pet = (*session.WorldScene().petRecords)[shortIndex];
    if (pet.used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, L"", 0, 0));
    const wchar_t* const title = pet.name6AC.data();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t value = pet.values70[5 + row];
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    const std::int32_t maximumWeight = pet.values6D8[2];
    if (maximumWeight > 0) {
        const std::int32_t weight = pet.values6D8[1];
        cg::text::FormatTo(buffer, L"{:3} / {:3}", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 31)),
            5, 12, 0, buffer, 0, 0));
    }
    if (interaction.previewAction == nullptr) {
        interaction.previewAction = cg::yobi::NewYobiAction_404AE0(session, 2, 0);
    }
    const std::int32_t imageNo = pet.imageNo6A0;
    cg::yobi::ConfigureWindowPetPreview(interaction.previewAction, imageNo);
    static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(interaction.previewAction, 100, 0, 0));
    cg::yobi::YobiAction* const action = interaction.previewAction;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        cg::yobi::GetYobiActionDrawMode(action);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t segments = pet.values70[21 + row] / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    const std::int32_t page = WindowPetSkills_GetPreviewPage(session, &pet, showEquipmentPage ? 3 : 2);
    for (std::int32_t row = 0; row < 10; ++row) {
        if (page == 2) {
            const std::wstring_view name = row < static_cast<std::int32_t>(pet.items6E4.size())
                && pet.items6E4[row].used != 0 ? std::wstring_view{pet.items6E4[row].name} : std::wstring_view{};
            WindowTrade_DrawPetEquipmentRow(shortX + 10, shortY + 141 + row * 16, name);
        } else {
            static_cast<void>(WindowTrade_DrawOwnPetSkillRow(session,
                shortX + 10, shortY + 141 + row * 16, shortIndex, page * 10 + row));
        }
    }
    if (showEquipmentPage) {
        WindowTrade_DrawPetPageHint(shortX, shortY, page);
    } else {
        static_cast<void>(render::DrawText_456780(static_cast<std::int16_t>(shortX + 10), static_cast<std::int16_t>(shortY + 303), 5, 12, 0,
            page == 0 ? L"第1/2页 按PageDown键看下一页" : L"第2/2页 按PageUp键看上一页", 0, 0));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004AFFE0。
static std::int32_t WindowTrade_ReadSelectedItem(session::ClientSession& session, const std::int32_t index)
{
    auto& trade = session.UiFrame().native.trade;
    const std::int32_t slot = std::bit_cast<std::int8_t>(trade.itemSlots[index]);
    return slot < 0 ? -1 : slot;
}

// 原函数：0x004B00F0。
static std::int32_t WindowTrade_ReadSelectedPet(session::ClientSession& session, const std::int32_t index)
{
    auto& trade = session.UiFrame().native.trade;
    const std::int32_t slot = std::bit_cast<std::int8_t>(trade.petSlots[index]);
    return slot < 0 ? -1 : slot;
}

// 原函数：0x004B0A50。
static std::int32_t WindowTrade_ButtonPublish(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    if (hover) {
        uiFrame.hoverText = L"向对方公开交易内容。";
    }
    std::int32_t result = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        std::wstring petSource;
        std::wstring itemSource;
        std::int32_t count = 0;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t slot = WindowTrade_ReadSelectedItem(*callbackWindow.ownerSession, position);
            if (slot >= 0) {
                if (count != 0) {
                    itemSource += L"|";
                }
                itemSource += std::format(L"{}|{}", slot, position);
                ++count;
            }
        }
        const auto itemCopy = itemSource;
        count = 0;
        for (std::int32_t position = 0; position < 5; ++position) {
            const std::int32_t slot = WindowTrade_ReadSelectedPet(*callbackWindow.ownerSession, position);
            if (slot >= 0) {
                if (count != 0) {
                    petSource += L"|";
                }
                petSource += std::format(L"{}|{}", slot, position);
                ++count;
            }
        }
        const auto petCopy = petSource;
        const std::int32_t amount = trade.valueD046D0;
        static_cast<void>(callbackWindow.ownerSession->Connection().SendTradeOffer(itemCopy, petCopy, amount));
        WindowInstance* const window = currentWindow;
        trade.valueD2E2B0 = 1;
        trade.scrollD67430 = 0;
        auto* const handleVerticalScroll_14Element = window->FindElementById(WindowTradeElementId::HandleVerticalScroll_14);
        if (handleVerticalScroll_14Element) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScroll_14Element, 2, 0));
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        result = 1;
    }
    if (graphic) graphic->imageNo = 0x3BA89;
    if (hover) {
        if (graphic) graphic->imageNo = 0x3BA8B;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3BA8A;
    }
    return result;
}

// 原函数：0x004B0CF0。
static std::int32_t WindowTrade_ButtonWithdraw(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = L"还是算了。";
        }
        std::int32_t result = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(callbackWindow.ownerSession->Connection().WithdrawTradeConfirmation());
            const std::int32_t previous = trade.valueCC4224;
            trade.valueD2E2B0 = 0;
            if (previous == 2) {
                trade.valueCC4224 = 1;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            trade.step = 23;
            result = 1;
        }
        graphic->imageNo = 0x3B562;
        if (hover) {
            graphic->imageNo = 0x3B564;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B563;
        }
        return result;
    }
    return 0;
}

// 原函数：0x004B0DB0。
static std::int32_t WindowTrade_ButtonConfirm(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& trade = callbackWindow.ownerSession->UiFrame().native.trade;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& interaction = callbackWindow.ownerSession->UiFrame().native.interaction;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = L"确定交易。";
        }
        std::int32_t result = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t phase = trade.valueD2E2B0;
            if ((phase == 1 && trade.valueCC4224 >= 1) || phase == 2) {
                const std::uint64_t tick = platform::ReadTickCount64();
                if (interaction.actionTick + 500u < tick) {
                    if (phase == 1) {
                        static_cast<void>(callbackWindow.ownerSession->Connection().SendTradeDecision(1));
                        trade.valueD2E2B0 = 2;
                        trade.valueD67CB8 = 1;
                        trade.valueD67CBC = 1;
                    } else {
                        static_cast<void>(callbackWindow.ownerSession->Connection().WithdrawTradeConfirmation());
                        const std::int32_t previous = trade.valueCC4224;
                        trade.valueD2E2B0 = 0;
                        if (previous == 2) {
                            trade.valueCC4224 = 1;
                        }
                    }
                    interaction.actionTick = platform::ReadTickCount64();
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            result = 1;
        }
        graphic->imageNo = 0x3BA8C;
        if (hover) {
            graphic->imageNo = 0x3BA8E;
        }
        if (trade.valueD2E2B0 == 2) {
            graphic->imageNo = 0x3BA8D;
        }
        return result;
    }
    return 0;
}

// 原函数：0x004E5620。
cg::yobi::YobiAction* WindowTrade_ReceiveStart(session::ClientSession& session, const std::wstring_view name)
{
    auto& trade = session.UiFrame().native.trade;
    trade.items.fill({});
    trade.pets.fill({});
    trade.valueCF4A1C = 0;
    trade.valueD673E0 = 0;
    trade.valueCC4C94 = 0;
    trade.valueC8AF68 = 0;
    trade.valueCF4688 = 0;
    trade.valueCB03CC = 0;
    trade.valueCF4AB8 = 0;
    trade.valueD046D0 = 0;
    trade.valueCC4224 = 0;
    trade.valueD2E2B0 = 0;
    WindowTrade_SyncOwnItems(session, 1);
    WindowTrade_CopyOwnPet(session, 1);
    trade.name = cg::text::ToGbk(name).size() <= 16 ? name : L"???";
    trade.valueD67CB8 = 0;
    trade.valueD67CBC = 0;
    trade.valueCC3EC4 = -1;
    trade.valueD66E08 = 0;
    trade.valueCC2344 = 0;
    return WindowTrade_Open(session);
}

// 原函数：0x004E4E60。
static void WindowTrade_RequestTargetList(session::ClientSession& session, const std::int32_t index, const wchar_t* const name)
{
    static_cast<void>(session.Connection().SelectTradePartnerRequest(index));
    static_cast<void>(WindowTrade_ReceiveStart(session, name));
}

// 原函数：0x004E4E80。
static cg::yobi::YobiAction* WindowTrade_OpenTargetList(session::ClientSession& session, const char* const text)
{
    return WindowTradeTarget_ReceiveTargets(session, text, &WindowTrade_RequestTargetList);
}

// 原函数：0x004E0100。
void WindowTrade_CopyDisplayItems(session::ClientSession& session, const std::int32_t side, const std::int32_t index)
{
    auto& storage = session.UiFrame().native.storage;
    if (index < 0 || index >= 20) {
        return;
    }
    const world::PlayerItemRecord& source = session.WorldScene().playerItems[index + 8];
    const bool used = source.used != 0;
    auto* const record = &storage.groups[side].items[index];
    *record = {};
    if (!used) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    record->used = std::uint16_t{1};
    record->imageNo = imageNo;
    record->count = count;
    const std::uint16_t flags = source.flags;
    record->flags = static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags));
    if (!source.memo.empty()) {
        record->memo = source.memo;
    }
    record->name = source.name;
    for (std::int32_t line = 0; line < 32; ++line) {
        record->description[line] = source.description[line];
    }
    record->pages = source.pages;
    if ((flags & 0x60u) != 0) {
        record->selection = std::bit_cast<std::int16_t>(std::uint16_t{0xFFFF});
    }
}

// 原函数：0x004E52B0。
static void WindowTrade_CopyOwnPet(session::ClientSession& session, const std::int32_t side)
{
    auto& trade = session.UiFrame().native.trade;
    for (std::int32_t index = 0; index < 5; ++index) {
        world::PetRuntimeRecord& source = (*session.WorldScene().petRecords)[index];
        if (source.used == 0) {
            continue;
        }
        auto* const record = &trade.pets[index + side * 5];
        const bool hasUserName = source.name6BD[0] != '\0';
        record->used = std::uint16_t{1};
        record->name = (hasUserName ? source.name6BD : source.name6AC);
        const std::int32_t imageNo = source.imageNo6A0;
        const std::int32_t value08 = source.value08;
        record->speciesName = source.name6AC.data();
        record->tribe = source.value04;
        record->imageNo = imageNo;
        record->level = value08;
        const std::array<std::int32_t*, 3> initialVitals {&record->hp, &record->maxHp, &record->fp};
        for (std::int32_t value = 0; value < 3; ++value) {
            *initialVitals[value] = static_cast<std::int32_t>(source.values0C[value]);
        }
        const std::int32_t fourth =
            static_cast<std::int32_t>(source.values0C[3]);
        std::int32_t first = source.values70[5];
        std::int32_t second = source.values70[6];
        record->maxFp = fourth;
        std::int32_t third = source.values70[7];
        record->baseStats[0] = first;
        first = source.values70[8];
        record->baseStats[1] = second;
        second = source.values70[9];
        record->baseStats[2] = third;
        third = source.values70[0];
        record->baseStats[3] = first;
        first = source.values70[1];
        record->baseStats[4] = second;
        second = source.values70[2];
        record->combatStats[0] = third;
        third = source.values70[3];
        record->combatStats[1] = first;
        first = source.values70[4];
        record->combatStats[2] = second;
        second = source.values70[10];
        record->combatStats[3] = third;
        third = source.value6A4;
        record->combatStats[4] = first;
        record->combatStats[5] = second;
        record->value5C = third;
        for (std::int32_t value = 0; value < 4; ++value) {
            (*&record->elements[value]) = source.values70[21 + value];
        }
        for (std::int32_t skill = 0; skill < world::kPetTechniqueSlotCount; ++skill) {
            const std::int16_t sourceIndex =
                std::bit_cast<std::int16_t>(source.techniquePairs650[skill].index);
            record->techniques[skill] = source.techniquesD8[sourceIndex];
        }
        for (std::size_t slot = 0; slot < record->equipmentNames.size(); ++slot) {
            const auto& item = source.items6E4[slot];
            record->equipmentNames[slot] = item.used != 0 ? item.name : std::wstring{};
        }
    }
}

// 原函数：0x004E5710。
static std::int32_t WindowTrade_ResetRemoteItems(session::ClientSession& session, const std::int32_t side)
{
    auto& trade = session.UiFrame().native.trade;
    if (side == 0) {
        trade.valueCF4688 = 0;
    } else {
        trade.valueD046D0 = 0;
    }
    std::fill_n(trade.items.data() + side * 20, 20, WindowTradeItem{});
    std::fill_n(trade.pets.data() + side * 5, 5, WindowTradePet{});
    return 0;
}

// 原函数：0x004E5770。
static void WindowTrade_BuildRemoteItemDescription(
    session::ClientSession& session,
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    auto& trade = session.UiFrame().native.trade;
    auto* const record = &trade.items[index + side * 20];
    auto& rows = record->description;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char row[3072]{};
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            rows[line] = cg::text::FromGbkWide(row);
            last = line;
        } else {
            rows[line].clear();
        }
    }
    // 原回扫从最后成功行的前一行开始，不检查最后成功行本身。
    while (last > 0 && rows[last - 1].empty()) {
        --last;
    }
    record->pages = last < 0 ? std::int32_t{0} : last / 8 + 1;
}

// 原函数：0x004E5830。
static std::int32_t WindowTrade_ReceiveRemoteItems(session::ClientSession& session, const char* const text)
{
    auto& trade = session.UiFrame().native.trade;
    char name[256];
    char memo[256];
    char description[2304];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    while (status != 1) {
        auto* const record = &trade.items[index];
        *record = {};
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        record->used = std::uint16_t{1};
        if (std::strlen(name) <= 28) {
            record->name = cg::text::FromGbkWide(name);
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 3, 767, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        WindowTrade_BuildRemoteItemDescription(session, index, 0, description);
        network::ExtractDelimitedField_52CED0(text, '|', field + 13, 255, memo);
        input::UnescapeLegacyTextAndNotify_52D2F0(memo);
        if (std::strlen(memo) <= 16) {
            record->memo = cg::text::FromGbkWide(memo);
        }
        record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field + 4);
        record->itemId = network::ParseDelimitedInteger_52CFA0(text, '|', field + 10);
        record->count = network::ParseDelimitedInteger_52CFA0(text, '|', field + 12);
        record->flags = network::ParseDelimitedInteger_52CFA0(text, '|', field + 9);
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        status = network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    }
    return status;
}

// 原函数：0x004E5A40。
static std::int32_t WindowTrade_ReceiveRemotePet(session::ClientSession& session, const std::int32_t index, const char* const text)
{
    auto& trade = session.UiFrame().native.trade;
    auto* const record = &trade.pets[index];
    *record = {};
    char name[256];
    std::int32_t result =
        network::ExtractDelimitedField_52CED0(text, '|', 1, 255, name);
    if (result != 0) {
        return result;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    record->name = cg::text::FromGbkWide(std::strlen(name) <= 16 ? name : "???");
    result = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    record->level = result;
    if (result < 0) {
        return result;
    }
    record->used = std::uint16_t{1};
    record->tribe = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(text, '|', 3));
    record->maxHp = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    const std::array<std::int32_t*, 7> replyCombat {
        &record->maxFp, &record->combatStats[0], &record->combatStats[1],
        &record->combatStats[2], &record->combatStats[3], &record->combatStats[4], &record->combatStats[5],
    };
    for (std::int32_t field = 5; field <= 11; ++field) {
        *replyCombat[field - 5] = network::ParseDelimitedInteger_52CFA0(text, '|', field);
    }
    record->value5C = network::ParseDelimitedInteger_52CFA0(text, '|', 12);
    network::ExtractDelimitedField_52CED0(text, '|', 13, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    record->speciesName = cg::text::FromGbkWide(std::strlen(name) <= 16 ? name : "???");
    record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 14);
    const std::array<std::int32_t*, 9> replyStats {
        &record->baseStats[0], &record->baseStats[1], &record->baseStats[2], &record->baseStats[3], &record->baseStats[4],
        &record->elements[0], &record->elements[1], &record->elements[2], &record->elements[3],
    };
    for (std::int32_t field = 15; field <= 23; ++field) {
        result = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        *replyStats[field - 15] = result;
    }
    return result;
}

// 原函数：0x004E5CD0。
static std::int32_t WindowTrade_ReceiveRemotePetSkills(session::ClientSession& session, const std::int32_t index, const char* const text)
{
    auto& trade = session.UiFrame().native.trade;
    if (index < 0 || index >= 5) {
        return index;
    }
    auto* const record = &trade.pets[index];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return index;
    }
    auto* skill = record->techniques.data();
    record->techniques.fill({});
    record->equipmentNames.fill({});
    char name[256];
    for (std::int32_t field = 1; field <= world::kPetTechniqueSlotCount; ++field) {
        const std::int32_t status =
            network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        if (status == 1) {
            return status;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::size_t length = std::strlen(name);
        skill->used = std::uint16_t{1};
        if (length <= 24) {
            skill->name = cg::text::FromGbkWide(name);
        }
        ++skill;
    }
    // 已确认Yq扩展：固定20个技能名之后是六个装备名，空槽仍占一个字段。
    for (std::size_t slot = 0; slot < record->equipmentNames.size(); ++slot) {
        const std::int32_t status = network::ExtractDelimitedField_52CED0(text, '|',
            world::kPetTechniqueSlotCount + 1 + static_cast<std::int32_t>(slot), 255, name);
        if (status == 1) {
            return status;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        record->equipmentNames[slot] = cg::text::FromGbkWide(std::strlen(name) <= 28 ? name : "???");
    }
    return world::kPetTechniqueSlotCount;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E3C0。
void WindowTrade_ReceiveTargetReply(cg::network::CgConnection& connection, const char* const text)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    if (world::ReadRippleGateOnce(connection.Owner()) != 0) {
        return;
    }
    static_cast<void>(detail::WindowTrade_OpenTargetList(connection.Owner(), text));
}

// 原函数：0x0050E3F0。
void WindowTrade_ReceiveStartReply(cg::network::CgConnection& networkConnection, char* const text, const std::int32_t)
{
    if (networkConnection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    if (networkConnection.Owner().WorldScene().logoutToTownPending != 0) {
        static_cast<void>(networkConnection.SendTradeDecision(0));
        return;
    }
    if (world::ReadRippleGateOnce(networkConnection.Owner()) != 0) {
        return;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    static_cast<void>(detail::WindowTrade_ReceiveStart(networkConnection.Owner(), cg::text::FromGbkWide(text)));
}

// 原函数：0x0050E440。
std::int32_t WindowTrade_ReceiveCancelReply(cg::network::CgConnection& connection)
{
    auto& trade = connection.Owner().UiFrame().native.trade;
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    trade.valueCC4224 = 1;
    return gate;
}

// 原函数：0x0050E470。
std::int32_t WindowTrade_ReceiveStateReply(cg::network::CgConnection& connection, const std::int32_t value)
{
    auto& trade = connection.Owner().UiFrame().native.trade;
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    trade.valueCF4688 = value;
    return value;
}

// 原函数：0x0050E4A0。
std::int32_t WindowTrade_ReceiveItemReply(cg::network::CgConnection& connection, const char* const text)
{
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemoteItems(connection.Owner(), text);
}

// 原函数：0x0050E4D0。
std::int32_t WindowTrade_ReceivePetReply(cg::network::CgConnection& connection, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemotePet(connection.Owner(), index, text);
}

// 原函数：0x0050E500。
std::int32_t WindowTrade_ReceivePetSkillReply(cg::network::CgConnection& connection, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemotePetSkills(connection.Owner(), index, text);
}

// 原函数：0x0050E530。
std::int32_t WindowTrade_ReceiveRemoteResetReply(cg::network::CgConnection& connection)
{
    auto& trade = connection.Owner().UiFrame().native.trade;
    const std::int32_t scene = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce(connection.Owner());
    if (gate != 0) {
        return gate;
    }
    trade.valueCC4224 = 0;
    static_cast<void>(detail::WindowTrade_ResetRemoteItems(connection.Owner(), 0));
    const std::int32_t previous = trade.valueD2E2B0;
    if (previous == 2) {
        trade.valueD2E2B0 = 1;
    }
    return previous;
}

// 原函数：0x0050E570。
void WindowTrade_ReceiveCloseReply(cg::network::CgConnection& connection, const std::int32_t value)
{
    auto& trade = connection.Owner().UiFrame().native.trade;
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    if (world::ReadRippleGateOnce(connection.Owner()) != 0) {
        return;
    }
    if (value == 1) {
        trade.valueCC4224 = 2;
    } else if (value == 0 || value == 2) {
        WindowInstance* const window = detail::WindowRuntime_States(connection.Owner())[WindowId_Trade].window;
        trade.valueD67B80 = 1;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
}

} // namespace cg::ui
