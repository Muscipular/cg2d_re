#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowCrystalShopElementId : ElementId {
    HandleConfirmation = 1,
    ButtonClose = 2,
    ButtonOk = 3,
    ButtonCancel = 4,
    DrawButton = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    HandleVerticalScroll = 11,
    ButtonPreviousRow = 12,
    ButtonNextRow = 13,
    ButtonPreviousPage = 14,
    ButtonNextPage = 15,
    Scroll = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    Element27 = 27,
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
    Element38 = 38,
    Element39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
    Element44 = 44,
    Element45 = 45,
    Element46 = 46,
    ButtonDecrease = 47,
    ButtonDecrease_48 = 48,
    ButtonDecrease_49 = 49,
    ButtonDecrease_50 = 50,
    ButtonDecrease_51 = 51,
    ButtonIncrease = 52,
    ButtonIncrease_53 = 53,
    ButtonIncrease_54 = 54,
    ButtonIncrease_55 = 55,
    ButtonIncrease_56 = 56,
    HandleItemIcon = 57,
    HandleItemIcon_58 = 58,
    HandleItemIcon_59 = 59,
    HandleItemIcon_60 = 60,
    HandleItemIcon_61 = 61,
    Element62 = 62,
    Element63 = 63,
    Element64 = 64,
    Element65 = 65,
    Element66 = 66,
    HandleInventory = 67,
    Element68 = 68,
    Element69 = 69,
    HandleSideArea = 70,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C1700。
static void WindowCrystalShop_RefreshRemaining(WindowInstance& callbackWindow); // 窗口69五条剩余量文本。

// 原函数：0x004C17B0。
static void WindowCrystalShop_RefreshRows(WindowInstance& callbackWindow); // 名称两行和价格，保留原分行空隙字节。

// 原函数：0x004C1AA0。
static void WindowCrystalShop_RefreshQuantities(WindowInstance& callbackWindow); // 五条已选数量。

// 原函数：0x004C1B00。
static void WindowCrystalShop_RefreshAmount(WindowInstance& callbackWindow); // 总额与玩家晶石比较，写type7 color。

// 原函数：0x004C1B70。
static void WindowCrystalShop_CountSlots(WindowInstance& callbackWindow); // 二十条选中商品占槽。

// 原函数：0x004C1BC0。
static void WindowCrystalShop_Refresh(WindowInstance& callbackWindow); // 首次/分页/数量脏标刷新。

// 原函数：0x004C2060。
static void WindowCrystalShop_Submit(WindowInstance& callbackWindow, std::int32_t value); // 低U8为0才按实时商品组包，命令0。

// 原函数：0x004C21B0。
static std::int32_t WindowCrystalShop_ButtonOk(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 余额/背包门禁及确认预览生产。

// 原函数：0x004C2FA0。
static std::int32_t WindowCrystalShop_HandleConfirmation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 窗口69确认覆盖层。

// 原函数：0x004C2B20。
static std::int32_t WindowCrystalShop_HandleInventory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 背包20格命中和绘制。

// 原函数：0x004C29E0。
static std::int32_t WindowCrystalShop_HandleItemIcon(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口69商品图标及四页提示。

// 原函数：0x004C2530。
static std::int32_t WindowCrystalShop_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口69上一行。

// 原函数：0x004C25B0。
static std::int32_t WindowCrystalShop_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一行。

// 原函数：0x004C2630。
static std::int32_t WindowCrystalShop_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上一页，真实入口为4C2630。

// 原函数：0x004C26B0。
static std::int32_t WindowCrystalShop_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一页。

// 原函数：0x004C2760。
static std::int32_t WindowCrystalShop_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 滚轮，仅回写分页。

// 原函数：0x004C27B0。
static std::int32_t WindowCrystalShop_ButtonDecrease(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 减数量，无used门禁，事件尾清消息。

// 原函数：0x004C28C0。
static std::int32_t WindowCrystalShop_ButtonIncrease(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 加数量，used/limit门禁，不清消息。

// 原函数：0x004C33B0。
static std::int32_t WindowCrystalShop_ShowTooltip(
    session::ClientSession& session, std::int32_t index, std::int32_t page, std::int32_t unused); // 保留caller第三实参。

// 原函数：0x004C1390。
static void WindowCrystalShop_Release(WindowInstance&);

// 原函数：0x004C1C50。
static std::int32_t WindowCrystalShop_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004C1EC0。
static std::int32_t WindowCrystalShop_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// IWT01：晶石商品为含宽文本的内部记录。

static WindowGraphicData gWindowGraphic5756F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005756F0。

static WindowGraphicData gWindowGraphic575708 {0x17F32, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575708。

static WindowGraphicData gWindowGraphic575720 {0x17F33, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575720。

static WindowGraphicData gWindowGraphic575738 {0x17F34, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575738。

static WindowGraphicData gWindowGraphic575750 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575750。

static WindowGraphicData gWindowGraphic575768 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575768。

static WindowGraphicData gWindowGraphic575780 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575780。

static WindowGraphicData gWindowGraphic575798 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575798。

static WindowGraphicData gWindowGraphic5757B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757B0。

static WindowGraphicData gWindowGraphic5757C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757C8。

static WindowGraphicData gWindowGraphic5757E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757E0。

static WindowGraphicData gWindowGraphic5757F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757F8。

static WindowGraphicData gWindowGraphic575810 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575810。

static WindowGraphicData gWindowGraphic575828 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575828。

static WindowGraphicData gWindowGraphic575840 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575840。

static WindowGraphicData gWindowGraphic575858 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575858。

static WindowGraphicData gWindowGraphic575870 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575870。

static WindowGraphicData gWindowGraphic575888 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575888。

static WindowGraphicData gWindowGraphic5758A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758A0。

static WindowGraphicData gWindowGraphic5758B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758B8。

static WindowGraphicData gWindowGraphic5758D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758D0。

static WindowTextPrototype gWindowText5758F8 { // 原全局：0x005758F8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText575900 { // 原全局：0x00575900。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText575908 { // 原全局：0x00575908。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前无法装备", // 原常量参考：0x00585DD8。
};

static WindowTextPrototype gWindowText575910 { // 原全局：0x00575910。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, L"剩余 %2d 个", // 原常量参考：0x00585DCC。
};

static std::int32_t gWindowAnimationImage575918 = 100000; // 原全局：0x00575918。

static WindowNumberData gWindowNumber575920 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575920。

static WindowNumberData gWindowNumber575930 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575930。

static WindowNumberData gWindowNumber575938 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575938。

static WindowNumberData gWindowNumber575940 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575940。

static WindowNumberData gWindowNumber575948 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575948。

static WindowNumberData gWindowNumber575950 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575950。

static WindowNumberData gWindowNumber575958 {nullptr, 10, 3, 1, 0}; // 原全局：0x00575958。

static WindowNumberData gWindowNumber575960 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575960。

static WindowNumberData gWindowNumber575968 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575968。

static WindowNumberData gWindowNumber575970 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575970。

static WindowNumberData gWindowNumber575978 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575978。

static WindowNumberData gWindowNumber575980 {nullptr, 0, 3, 1, 0}; // 原全局：0x00575980。

static std::uint32_t gWindowMouseFlagsD67910 = 0; // 原全局：0x00D67910。

// 原全局：0x00575988。
// 原575988的完整70项，显示缓冲与69更新/绘制保持同源。
static WindowElementPrototype gWindowElements575988[70] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleConfirmation, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleConfirmation)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5756F0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonClose)},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic575888}, &WindowCrystalShop_ButtonOk, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonOk)},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5758A0}, &WindowCrystalTrade_ButtonCancel, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonCancel)},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5758A0}, &WindowCrystalTrade_DrawButton, -1, static_cast<ElementId>(WindowCrystalShopElementId::DrawButton)},
    {WindowElementType_Text, 425, 44, 0, 14, 1, {.text = &gWindowText5758F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element6)},
    {WindowElementType_Number, 260, 37, 0, 0, 1, {.number = &gWindowNumber575930}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element7)},
    {WindowElementType_Number, 400, 32, 0, 0, 1, {.number = &gWindowNumber575920}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element8)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage575918}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element9)},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic575750}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element10)},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67910}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic575768}, &WindowCrystalShop_ButtonPreviousRow, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic575780}, &WindowCrystalShop_ButtonNextRow, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758B8}, &WindowCrystalShop_ButtonPreviousPage, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758D0}, &WindowCrystalShop_ButtonNextPage, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowCrystalShop_Scroll, -1, static_cast<ElementId>(WindowCrystalShopElementId::Scroll)},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element17)},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element18)},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element19)},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element20)},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element21)},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element22)},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element23)},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element24)},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element25)},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element26)},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element27)},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element28)},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element29)},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element30)},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element31)},
    {WindowElementType_Text, 83, 87, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element32)},
    {WindowElementType_Text, 83, 142, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element33)},
    {WindowElementType_Text, 83, 197, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element34)},
    {WindowElementType_Text, 83, 252, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element35)},
    {WindowElementType_Text, 83, 307, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element36)},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber575938}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element37)},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber575940}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element38)},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber575948}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element39)},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber575950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element40)},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber575958}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element41)},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber575960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element42)},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber575968}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element43)},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber575970}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element44)},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber575978}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element45)},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber575980}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element46)},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic575798}, &WindowCrystalShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonDecrease)},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5757B0}, &WindowCrystalShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonDecrease_48)},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5757C8}, &WindowCrystalShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonDecrease_49)},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5757E0}, &WindowCrystalShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonDecrease_50)},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5757F8}, &WindowCrystalShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonDecrease_51)},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic575810}, &WindowCrystalShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonIncrease)},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic575828}, &WindowCrystalShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonIncrease_53)},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic575840}, &WindowCrystalShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonIncrease_54)},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic575858}, &WindowCrystalShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonIncrease_55)},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic575870}, &WindowCrystalShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalShopElementId::ButtonIncrease_56)},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleItemIcon)},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleItemIcon_58)},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleItemIcon_59)},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleItemIcon_60)},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleItemIcon_61)},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element62)},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element63)},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element64)},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element65)},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element66)},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleInventory, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleInventory)},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic575720}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element68)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic575708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalShopElementId::Element69)},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowCrystalShopElementId::HandleSideArea)},
};

// 原型：窗口69；原全局：0x0054ECC0。
WindowPrototype gWindowPrototypeCrystalShop {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements575988, &WindowCrystalShop_Update, &WindowCrystalShop_Draw, &WindowCrystalShop_Release}; // 69: 54ECC0

// 窗口函数实现。

// 原函数：0x004C1390。
static void WindowCrystalShop_Release(WindowInstance& releasedWindow)
{
    auto& npcDialog = releasedWindow.ownerSession->WorldScene().npcDialog;
    auto& crystal = releasedWindow.ownerSession->UiFrame().native.crystalShop;
    const std::int32_t type = crystal.dialogType;
    if (type == npcDialog.type) {
        npcDialog.type = -1;
    }
}

// 原函数：0x004C13B0。
cg::yobi::YobiAction* WindowCrystalShop_Open(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& crystal = session.UiFrame().native.crystalShop;
    auto& shop = session.UiFrame().native.shop;
    crystal.initialized = 0;
    crystal.flag = 0;
    crystal.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    std::array<char, 2304> buffer;
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(npcLines[group * 4 + row + 1].text, buffer.data(), row, 20));
        }
    }
    shop.itemCount = 0;
    crystal.items.fill({});
    std::int32_t field = 8;
    for (WindowCrystalItem* item = crystal.items.data();
         item != crystal.items.data() + crystal.items.size(); ++item, field += 7) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field - 2, 767, buffer.data()) == 1) {
            break;
        }
        crystal.items[shop.itemCount].used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) > 28) {
            item->name = L"???";
        } else {
            crystal.items[shop.itemCount].name = cg::text::FromGbkWide(buffer.data());
        }
        item->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field - 1);
        item->valueC24 = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        item->limitC4C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        item->valueC2C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        const std::int32_t remaining = network::ParseDelimitedInteger_52CFA0(text, '|', field + 3);
        item->remainingC50 = remaining;
        if (remaining != -1) {
            WindowCrystalItem& current = crystal.items[shop.itemCount];
            if (remaining < current.limitC4C) {
                current.limitC4C = current.remainingC50;
            }
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field + 4, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::int32_t currentCount = shop.itemCount;
        for (std::int32_t row = 0; row < 32; ++row) {
            auto& destination = crystal.items[currentCount].description[row];
            char line[3072]{};
            const std::int32_t copied = input::ExtractLegacyLine_503AB0(
                line, 3072, buffer.data(), row, 32);
            destination = cg::text::FromGbkWide(line);
            currentCount = shop.itemCount;
            if (copied == 0) {
                crystal.items[currentCount].description[row].clear();
            }
        }
        shop.itemCount = currentCount + 1;
    }
    const std::int32_t type = npcDialog.type;
    shop.currencyItemId = -1;
    crystal.dialogType = type;
    return WindowRuntime_CreateAnimation(session, WindowId_CrystalShop, 1, 0, nullptr);
}

// 原函数：0x004C1700。
static void WindowCrystalShop_RefreshRemaining(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 31)); indexedElement) indexedElement->visible = 0;
        const WindowCrystalItem& item = crystal.items[crystal.scroll + row];
        if (item.used != 0 && item.remainingC50 >= 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 31)); indexedElement2) indexedElement2->visible = 1;
            const std::int32_t index = crystal.scroll + row;
            WindowInstance* const window = currentWindow;
            const std::int32_t remaining = crystal.items[index].remainingC50;
            auto* const indexedElement3 = window->FindElementById(NativeElementKey(row + 31));
            if (indexedElement3) {
                cg::text::FormatTo(indexedElement3->data.text->text, L"剩余 {:2} 个", remaining);
            }
        }
    }
}

// 原函数：0x004C17B0。
static void WindowCrystalShop_RefreshRows(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        WindowInstance* const initial = currentWindow;
        std::wstring& price = crystal.priceTexts[row];
        price.clear();
        auto* const indexedElement = initial->FindElementById(NativeElementKey(row + 16));
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
        auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 21));
        if (indexedElement2) {
            indexedElement2->data.text->text.clear();
        }
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 26)); indexedElement3) indexedElement3->visible = 0;
        const WindowCrystalItem* item = &crystal.items[crystal.scroll + row];
        if (item->used == 0) {
            continue;
        }
        cg::text::FormatTo(price, L"{:10}", item->valueC24);
        item = &crystal.items[crystal.scroll + row];
        if ((item->flags & 0x100u) != 0) {
            buffer = item->tips;
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 16));
            if (indexedElement4) {
                indexedElement4->data.text->style.color =
                    std::byte{5};
            }
            auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 21));
            if (indexedElement5) {
                indexedElement5->data.text->style.color =
                    std::byte{5};
            }
        } else {
            const std::int32_t stack = item->valueC2C;
            if (stack > 1) {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({}{})", item->name, L"堆叠数", stack));
            } else {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({})", item->name, L"不可堆叠"));
            }
            const std::uint32_t flags =
                crystal.items[crystal.scroll + row].flags;
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 16)); indexedElement4) indexedElement4->data.text->style.color = color;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 21)); indexedElement5) indexedElement5->data.text->style.color = color;
        }
        auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 16));
        auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 21));
        if (indexedElement4 && indexedElement5) {
            WindowRuntime_FillTwoTextLines(indexedElement4->data.text, indexedElement5->data.text, buffer);
        }
    }
}

// 原函数：0x004C1AA0。
static void WindowCrystalShop_RefreshQuantities(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t scroll = crystal.scroll;
        std::wstring& text = crystal.countTexts[row];
        text.clear();
        const WindowCrystalItem& item = crystal.items[scroll + row];
        if (item.used != 0) {
            cg::text::FormatTo(text, L"{:2}", item.selectedCount);
        }
    }
}

// 原函数：0x004C1B00。
static void WindowCrystalShop_RefreshAmount(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::int32_t total = 0;
    crystal.total = 0;
    for (const WindowCrystalItem& item : crystal.items) {
        if (item.used != 0) {
            const std::int32_t quantity = item.selectedCount;
            total += quantity * item.valueC24;
        }
    }
    crystal.total = total;
    cg::text::FormatTo(crystal.totalText, L"{:10}", total);
    const std::int32_t currentTotal = crystal.total;
    const std::int32_t owned = callbackWindow.ownerSession->WorldScene().player->value15BCC;
    auto* const element7 = currentWindow->FindElementById(WindowCrystalShopElementId::Element7);
    if (element7) {
        auto* const number = element7->data.number;
        number->color = currentTotal <= owned ? 10 : 6;
    }
    return;
}

// 原函数：0x004C1B70。
static void WindowCrystalShop_CountSlots(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    std::int32_t slots = 0;
    crystal.slots = 0;
    for (const WindowCrystalItem& item : crystal.items) {
        if (item.used != 0) {
            std::int32_t stack = item.valueC2C;
            const std::int32_t quantity = item.selectedCount;
            if (stack <= 0) {
                stack = 2;
            }
            slots += (stack + quantity - 1) / stack;
        }
    }
    crystal.slots = slots;
}

// 原函数：0x004C1BC0。
static void WindowCrystalShop_Refresh(WindowInstance& callbackWindow)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    const std::int32_t initialized = crystal.initialized;
    const std::int32_t scroll = crystal.scroll;
    if (initialized != 0 && crystal.cachedScroll == scroll) {
        if (crystal.dirty != 0) {
            crystal.dirty = 0;
            WindowCrystalShop_RefreshAmount(callbackWindow);
            WindowCrystalShop_RefreshQuantities(callbackWindow);
            WindowCrystalShop_CountSlots(callbackWindow);
        }
        return;
    }
    crystal.initialized = 1;
    crystal.cachedScroll = scroll;
    WindowCrystalShop_RefreshRows(callbackWindow);
    WindowCrystalShop_RefreshAmount(callbackWindow);
    WindowCrystalShop_RefreshQuantities(callbackWindow);
    WindowCrystalShop_CountSlots(callbackWindow);
    WindowCrystalShop_RefreshRemaining(callbackWindow);
    const std::int32_t position = crystal.scroll;
    WindowInstance* const window = currentWindow;
    const std::int32_t maximum = shop.itemCount - 5;
    auto* const handleVerticalScrollElement = window->FindElementById(WindowCrystalShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
    }
    return;
}

// 原函数：0x004C1C50。
static std::int32_t WindowCrystalShop_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const entryWindow = currentWindow;
    if (value == -1) {
        // 数字原型只留样式，实例借所属会话的稳定文本。
        if (auto* const element = currentWindow->FindElementById(WindowCrystalShopElementId::Element7); element && element->data.number) {
            element->data.number->text = &crystal.totalText;
        }
        if (auto* const element = currentWindow->FindElementById(WindowCrystalShopElementId::Element8); element && element->data.number) {
            element->data.number->text = &crystal.balanceText;
        }
        for (std::int32_t row = 0; row < 5; ++row) {
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(36 + row)); element && element->data.number) {
                element->data.number->text = &crystal.priceTexts[row];
            }
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(41 + row)); element && element->data.number) {
                element->data.number->text = &crystal.countTexts[row];
            }
        }
        crystal.scroll = 0;
        shop.available = -1;
        if (auto* const handleConfirmationElement = entryWindow->FindElementById(WindowCrystalShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 0;
        for (std::int32_t row = 0; row < 5; ++row) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(16 + row)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(21 + row)); indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(26 + row)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(36 + row)); indexedElement4) indexedElement4->visible = 1;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(41 + row)); indexedElement5) indexedElement5->visible = 1;
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(46 + row)); indexedElement6) indexedElement6->visible = 1;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(51 + row)); indexedElement7) indexedElement7->visible = 1;
            if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(56 + row)); indexedElement8) indexedElement8->visible = 1;
            if (auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(61 + row)); indexedElement9) indexedElement9->visible = 1;
            if (auto* const indexedElement10 = currentWindow->FindElementById(NativeElementKey(56 + row)); indexedElement10) indexedElement10->mouseCallback = &WindowCrystalShop_HandleItemIcon;
            auto* const indexedElement11 = currentWindow->FindElementById(NativeElementKey(61 + row));
            if (indexedElement11) {
                indexedElement11->data.graphic->imageNo =
                    98100;
            }
        }
        if (auto* const drawButtonElement = currentWindow->FindElementById(WindowCrystalShopElementId::DrawButton); drawButtonElement) drawButtonElement->visible = 0;
        WindowInstance* const window = currentWindow;
        shop.rowKeys[0] = -2;
        crystal.tooltip = -1;
        shop.rowKeys[1] = -2;
        crystal.message = -1;
        shop.rowKeys[2] = -2;
        crystal.tooltipPage = 0;
        shop.rowKeys[3] = -2;
        shop.rowKeys[4] = -2;
        auto* const element6 = window->FindElementById(WindowCrystalShopElementId::Element6);
        if (element6) {
            auto* const title = element6->data.text;
            title->text = npcLines[0].text;
        }
        auto* const element9 = currentWindow->FindElementById(WindowCrystalShopElementId::Element9);
        if (element9) {
            auto* animation = element9->data.animation;
            const std::int32_t imageNo = crystal.imageNo;
            cg::yobi::SetYobiActionField2A8(animation->action, imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    const std::int32_t type = crystal.dialogType;
    if (type != npcDialog.type) {
        currentWindow->state |= 0x8000u;
    }
    if (auto* const element10 = currentWindow->FindElementById(WindowCrystalShopElementId::Element10); element10) element10->visible = 1;
    if (shop.itemCount - 5 <= 0) {
        if (auto* const element102 = currentWindow->FindElementById(WindowCrystalShopElementId::Element10); element102) element102->visible = 0;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowCrystalShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
            auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowCrystalShopElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement2) {
                crystal.scroll = WindowScroll_ReadPosition(handleVerticalScrollElement2, shop.itemCount - 5);
            }
        }
    }
    cg::text::FormatTo(crystal.balanceText, L"{:10}", callbackWindow.ownerSession->WorldScene().player->value15BCC);
    if (crystal.message >= 0) {
        crystal.message = -1;
    }
    crystal.tooltip = -1;
    return 1;
}

// 原函数：0x004C1EC0。
static std::int32_t WindowCrystalShop_Draw(WindowInstance& callbackWindow, const std::int32_t unused)
{
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& interaction = callbackWindow.ownerSession->UiFrame().native.interaction;
    static_cast<void>(unused);
    const WindowInstance* const window = currentWindow;
    const std::int32_t x = window->x + 298;
    std::int32_t y = window->y + 50;
    const std::int32_t tooltip = crystal.tooltip;
    if (tooltip >= 0) {
        if (tooltip < 100) {
            static_cast<void>(WindowItem_DrawEquipmentTooltip(*callbackWindow.ownerSession, tooltip, crystal.tooltipPage));
        } else if (tooltip < 200) {
            const std::uint8_t flag = crystal.flag;
            const std::int32_t page = crystal.tooltipPage;
            static_cast<void>(WindowCrystalShop_ShowTooltip(*callbackWindow.ownerSession, tooltip - 100, page, flag == 0));
        } else {
            static_cast<void>(WindowItem_DrawWorkTooltip(*callbackWindow.ownerSession, crystal.tooltipPage));
        }
    }
    const std::int32_t message = crystal.message;
    interaction.tooltipColor = 0;
    const WindowDialogTextLine* line = npcLines.data() + 1 + 4 * (-1 - message);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (!line->text.empty()) {
            static_cast<void>(WindowItem_SplitTooltipLines(*callbackWindow.ownerSession, x, y, 3, 8, 0, line->text.data(), 0, 0));
        }
        ++line;
        y += 10;
    }
    WindowCrystalShop_Refresh(callbackWindow);
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004C2060。
static void WindowCrystalShop_Submit(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    if (static_cast<std::uint8_t>(value) != 0) {
        return;
    }
    std::wstring fragment;
    std::wstring text;
    std::wstring escaped;
    text.clear();
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const WindowCrystalItem& item = crystal.items[index];
        if (item.used != 0) {
            const std::int32_t quantity = item.selectedCount;
            if (quantity != 0) {
                if (count > 0) {
                    text += L"|";
                }
                cg::text::FormatTo(fragment, L"{}|{}", index, quantity);
                text += fragment;
                ++count;
            }
        }
    }
    escaped = input::EscapeTextAndNotify(text, 255);
    const std::int32_t target = npcDialog.target;
    const std::int32_t request = npcDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
    currentWindow->state |= 0x8000u;
}

// 原函数：0x004C21B0。
static std::int32_t WindowCrystalShop_ButtonOk(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        uiFrame.hoverText = L"买进指定的物品。" /* 原常量参考：0x00587FA8。 */;
    }
    if (crystal.slots <= 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t balance = callbackWindow.ownerSession->WorldScene().player->value15BCC;
        if (balance < crystal.total) {
            crystal.message = -2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            const std::int32_t freeSlots = world::CountFreeInventorySlots_518C60(*callbackWindow.ownerSession);
            if (crystal.slots > freeSlots) {
                crystal.message = -3;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                crystal.confirmCallback = &WindowCrystalShop_Submit;
                std::int32_t count = 0;
                for (const WindowCrystalItem& item : crystal.items) {
                    if (item.used == 0) {
                        continue;
                    }
                    const std::int32_t quantity = item.selectedCount;
                    if (quantity == 0) {
                        continue;
                    }
                    WindowConfirmationItem& preview = crystal.confirmItems[count];
                    preview.visible = 1;
                    preview.name = item.name;
                    const std::int32_t imageNo = item.imageNo;
                    preview.color = 0;
                    preview.imageNo = imageNo;
                    preview.quantity = quantity;
                    preview.factor = 0;
                    ++count;
                }
                for (; count < 20; ++count) {
                    crystal.confirmItems[count].visible = 0;
                }
                WindowConfirmationAmount& left = crystal.confirmAmounts[0];
                WindowConfirmationAmount& right = crystal.confirmAmounts[1];
                const std::int32_t currentBalance = callbackWindow.ownerSession->WorldScene().player->value15BCC;
                left.y = 343;
                right.y = 343;
                const std::int32_t total = crystal.total;
                left.visible = 1;
                left.x = 296;
                left.value = currentBalance;
                right.visible = 1;
                right.x = 454;
                right.value = total;
                (crystal.confirmHeading = L"持有水晶            颗     花费水晶            颗" /* 原常量参考：0x00588378。 */);
                (crystal.confirmQuestion = L"确认要购买这些物品？" /* 原常量参考：0x00587F0C。 */);
                crystal.confirmAccept = L"买进物品。" /* 原常量参考：0x00587F00。 */;
                crystal.confirmCancel = L"还是算了。" /* 原常量参考：0x00585AD0。 */;
                if (auto* const handleConfirmationElement = currentWindow->FindElementById(WindowCrystalShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 1;
                currentWindow->state |= 2u;
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243048;
    if (hover) {
        if (graphic) graphic->imageNo = 243050;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243049;
    }
    if (crystal.slots <= 0) {
        if (graphic) graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004C2530。
static std::int32_t WindowCrystalShop_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t position = crystal.scroll;
            if (position > 0) {
                crystal.scroll = position - 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
        graphic->imageNo = 243090;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243092;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243091;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C25B0。
static std::int32_t WindowCrystalShop_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t count = shop.itemCount;
            const std::int32_t position = crystal.scroll;
            if (position < count - 5) {
                crystal.scroll = position + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
        graphic->imageNo = 243093;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243095;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243094;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C2630。
static std::int32_t WindowCrystalShop_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(crystal.scroll > 0 ? 51 : 55, 320));
            const std::int32_t position = crystal.scroll - 5;
            crystal.scroll = position;
            if (position < 0) {
                crystal.scroll = 0;
            }
            consumed = 1;
        }
        graphic->imageNo = 243030;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243032;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243031;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C26B0。
static std::int32_t WindowCrystalShop_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t oldMaximum = shop.itemCount - 5;
            const bool canAdvance = oldMaximum > 0 && crystal.scroll < oldMaximum;
            static_cast<void>(audio::PlaySe_52C450(canAdvance ? 51 : 55, 320));
            std::int32_t position = crystal.scroll + 5;
            const std::int32_t maximum = shop.itemCount - 5;
            crystal.scroll = position;
            if (position > maximum) {
                position = maximum;
                crystal.scroll = position;
            }
            if (position < 0) {
                crystal.scroll = 0;
            }
            consumed = 1;
        }
        graphic->imageNo = 243033;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 243035;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243034;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C2760。
static std::int32_t WindowCrystalShop_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = shop.itemCount;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = crystal.scroll;
        WindowInstance* const window = currentWindow;
        auto* const indexedElement = window->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            crystal.scroll =
                WindowScroll_ApplyWheel(indexedElement, position, count - 5, wheel);
        }
    }
    return 0;
}

// 原函数：0x004C27B0。
static std::int32_t WindowCrystalShop_ButtonDecrease(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        WindowCrystalItem* const item = &crystal.items[crystal.scroll + index - 46];
        std::int32_t consumed = 0;
        if (hover) {
            const wchar_t* const hint = item->selectedCount > 0
                ? L"减少物品的数量。"
                : L"无法再减少物品的数量了。";
            uiFrame.hoverText = hint;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t count = item->selectedCount;
            if (count > 0) {
                item->selectedCount = count - 1;
                crystal.dirty = 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            crystal.message = -1;
            consumed = 1;
        }
        graphic->imageNo = 243006;
        if (hover) {
            graphic->imageNo = 243008;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243007;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C28C0。
static std::int32_t WindowCrystalShop_ButtonIncrease(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        WindowCrystalItem* const item = &crystal.items[crystal.scroll + index - 51];
        std::int32_t consumed = 0;
        if (hover) {
            const std::int32_t limit = item->limitC4C;
            const std::int32_t count = item->selectedCount;
            const wchar_t* hint;
            if (count < limit) {
                hint = L"增加物品的数量。";
            } else if (limit == 0) {
                hint = L"已经卖完了！";
            } else {
                hint = L"无法再增加物品的数量了。";
            }
            uiFrame.hoverText = hint;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (item->used != 0) {
                const std::int32_t count = item->selectedCount;
                const std::int32_t limit = item->limitC4C;
                if (count < limit) {
                    item->selectedCount = count + 1;
                    crystal.dirty = 1;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
            consumed = 1;
        }
        graphic->imageNo = 243003;
        if (hover) {
            graphic->imageNo = 243005;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243004;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C29E0。
static std::int32_t WindowCrystalShop_HandleItemIcon(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t selected = crystal.scroll + index - 56;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    const WindowCrystalItem* const item = &crystal.items[selected];
    if (item->used == 0) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        if (shop.rowKeys[index - 56] != selected) {
            crystal.tooltipPage = 0;
        }
        shop.rowKeys[index - 56] = selected;
        if ((state & MouseFlag_RDownEvent) != 0) {
            ++crystal.tooltipPage;
            if (crystal.tooltipPage >= 4) {
                crystal.tooltipPage = 0;
            }
            consumed = 1;
        }
        crystal.tooltip = selected + 100;
    }
    if (item->remainingC50 == 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t windowY = window->y;
            const std::int32_t y = element.y + windowY + 24;
            const std::int32_t elementX = element.x;
            const std::int32_t x = elementX + window->x + 24;
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, 98102, 0, &descriptor));
        }
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement& element = (*elementPtr);
        const std::int32_t windowY = window->y;
        const std::int32_t imageNo = item->imageNo;
        const std::int32_t y = element.y + windowY + 24;
        const std::int32_t elementX = element.x;
        const std::int32_t x = elementX + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x004C2B20。
static std::int32_t WindowCrystalShop_HandleInventory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * (slot % 5) + window->x + 2;
                const std::int32_t top = element.y + 50 * (slot / 5) + window->y + 2;
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = slot;
                    break;
                }
            }
        }
    }
    if (crystal.inventoryHover != hovered) {
        crystal.tooltipPage = 0;
        crystal.inventoryHover = hovered;
    }
    const auto& items = callbackWindow.ownerSession->WorldScene().playerItems;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    } else if ((events & MouseFlag_Hover) != 0 && hovered >= 0 && (events & MouseFlag_RDownEvent) != 0) {
        if (items[hovered + 8].used != 0) {
            std::int32_t page = crystal.tooltipPage;
            const std::int32_t pages = items[hovered + 8].pages;
            ++page;
            crystal.tooltipPage = page;
            if (page >= pages) {
                crystal.tooltipPage = 0;
            }
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind(*callbackWindow.ownerSession) == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession);
        if (source >= 8) {
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 2;
                const std::int32_t top = element.y + 50 * ((source - 8) / 5) + window->y + 2;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                    static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                    223, 254, 0);
            }
        }
    }
    std::wstring buffer;
    if (hovered >= 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 2;
            const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 2;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
        const world::PlayerItemRecord& item = items[hovered + 8];
        if (item.used != 0) {
            crystal.tooltip = hovered + 8;
            if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                uiFrame.hoverText = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                uiFrame.hoverText = item.memo + L"[han]";
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const world::PlayerItemRecord& item = items[slot + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = element.x + 50 * (slot % 5) + window->x + 26;
            const std::int32_t y = element.y + 50 * (slot / 5) + window->y + 26;
            const std::int32_t itemId = item.itemId;
            if (itemId == shop.currencyItemId) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 22), static_cast<std::uint16_t>(y - 22),
                    static_cast<std::uint16_t>(x + 22), static_cast<std::uint16_t>(y + 22),
                    223, 254, 0);
            }
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
            const std::int32_t count = item.count;
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    }
    return consumed;
}

// 原函数：0x004C2FA0。
static std::int32_t WindowCrystalShop_HandleConfirmation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t unusedState)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& crystal = callbackWindow.ownerSession->UiFrame().native.crystalShop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unusedState);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const bool acceptHover = cg::ui::HitTestMouseRect_5049A0(100, 381, 166, 398, -1) != 0;
    const bool cancelHover = cg::ui::HitTestMouseRect_5049A0(510, 381, 576, 398, -1) != 0;
    std::int32_t acceptImage = 245323;
    if (acceptHover) {
        acceptImage = 245325;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        uiFrame.hoverText = crystal.confirmAccept;
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = crystal.confirmCallback;
            if (callback != nullptr) {
                callback(callbackWindow, 0);
            }
            acceptImage = 245324;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    std::int32_t cancelImage = 245326;
    if (cancelHover) {
        cancelImage = 245328;
        uiFrame.hoverText = crystal.confirmCancel;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        static_cast<void>(0);
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = crystal.confirmCallback;
            if (callback != nullptr) {
                callback(callbackWindow, 1);
            }
            cancelImage = 245327;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(100, 381, 226, acceptImage, 0, 1, 0, &descriptor));
    static_cast<void>(render::DrawGraphic3_520970(510, 381, 226, cancelImage, 0, 1, 0, &descriptor));
    std::wstring buffer;
    for (std::int32_t column = 0; column < 4; ++column) {
        const std::int32_t x = 18 + column * 152;
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t y = 47 + row * 65;
            const WindowConfirmationItem& item = crystal.confirmItems[column * 5 + row];
            if (item.visible != 0) {
                static_cast<void>(cg::render::DrawGraphic(
                    x + 24, y + 24, 226, item.imageNo, 0, &descriptor));
                const std::int32_t factor = item.factor;
                if (factor != 0) {
                    cg::text::FormatTo(buffer, L"{:2} X {:2}", factor, item.quantity);
                } else {
                    cg::text::FormatTo(buffer, L"X {:2}", item.quantity);
                }
                const auto countFrame = static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x + 57), static_cast<std::int16_t>(y + 33),
                    4, 12, 0, buffer, 0, countFrame));
                const auto nameFrame = static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768);
                const auto nameColor = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(item.color));
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x), static_cast<std::int16_t>(y + 50),
                    4, 12, nameColor, item.name, 0, nameFrame));
            }
            static_cast<void>(cg::render::DrawGraphic(
                x + 24, y + 24, 226, 243139, 0, &descriptor));
        }
    }
    static_cast<void>(render::DrawText_456780(190, 377, 4, 12, 0, crystal.confirmHeading, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    static_cast<void>(render::DrawText_456780(190, 393, 4, 12, 0, crystal.confirmQuestion, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    for (const WindowConfirmationAmount& amount : crystal.confirmAmounts) {
        if (amount.visible != 0) {
            cg::text::FormatTo(buffer, L"{}", amount.value);
            const std::int32_t y = amount.y;
            const std::int32_t x = amount.x;
            WindowNumber_Draw(x, y + 35, buffer, 0, 4, 1, 226);
        }
    }
    cg::yobi::DrawNineGrid_49BE00(
        245300, 0, 35, 640, 383, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004C33B0。
static std::int32_t WindowCrystalShop_ShowTooltip(
    session::ClientSession& session, const std::int32_t index, const std::int32_t page, const std::int32_t unused)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& crystal = session.UiFrame().native.crystalShop;
    static_cast<void>(unused);
    const std::int32_t key = index + 300;
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
    interaction.tooltipSource = key;
    const WindowCrystalItem* const item = &crystal.items[index];
    const std::uint32_t flags = item->flags;
    std::wstring title = item->tips;
    std::int8_t titleColor;
    if ((flags & 0x100u) != 0) {
        titleColor = 5;
    } else {
        title = item->name;
        titleColor = static_cast<std::int8_t>((flags >> 9) & 1u);
    }
    const auto* const lines = item->description.data() + page * 8;
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    return WindowItem_DrawTooltip(session, x, y, title.c_str(), titleColor, lines, 8, nullptr, 4);
}

} // namespace detail

} // namespace cg::ui
