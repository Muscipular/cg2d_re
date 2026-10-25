#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowCrystalExchangeShopElementId : ElementId {
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

// 原函数：0x004C39A0。
static void WindowCrystalExchangeShop_RefreshRemaining(); // 窗口70五条剩余量文本。

// 原函数：0x004C3A50。
static void WindowCrystalExchangeShop_RefreshRows(); // 名称两行和价格，独立分页/显示缓冲。

// 原函数：0x004C3D40。
static void WindowCrystalExchangeShop_RefreshQuantities(); // 五条已选数量。

// 原函数：0x004C3DA0。
static void WindowCrystalExchangeShop_RefreshAmount(); // 总额与指定背包物品数量比较。

// 原函数：0x004C3E20。
static void WindowCrystalExchangeShop_CountSlots(); // 二十条选中商品占槽。

// 原函数：0x004C3E70。
static void WindowCrystalExchangeShop_Refresh(); // 首次/分页/数量脏标刷新。

// 原函数：0x004C4270。
static void WindowCrystalExchangeShop_Submit(std::int32_t value); // 低U8为0才按实时商品组包，命令1。

// 原函数：0x004C43C0。
static std::int32_t WindowCrystalExchangeShop_ButtonOk(ElementId elementId, std::int32_t state); // 交换物数量/背包门禁及确认预览。

// 原函数：0x004C50C0。
static std::int32_t WindowCrystalExchangeShop_HandleConfirmation(ElementId elementId, std::int32_t unusedState); // 窗口70确认覆盖层。

// 原函数：0x004C4CB0。
static std::int32_t WindowCrystalExchangeShop_HandleInventory(ElementId elementId, std::int32_t state); // 仅首格命中，仍绘完整20格。

// 原函数：0x004C4B70。
static std::int32_t WindowCrystalExchangeShop_HandleItemIcon(ElementId elementId, std::int32_t state); // 窗口70商品图标及四页提示。

// 原函数：0x004C46C0。
static std::int32_t WindowCrystalExchangeShop_ButtonPreviousRow(ElementId elementId, std::int32_t state); // 窗口70上一行。

// 原函数：0x004C4740。
static std::int32_t WindowCrystalExchangeShop_ButtonNextRow(ElementId elementId, std::int32_t state); // 下一行。

// 原函数：0x004C47C0。
static std::int32_t WindowCrystalExchangeShop_ButtonPreviousPage(ElementId elementId, std::int32_t state); // 上一页。

// 原函数：0x004C4840。
static std::int32_t WindowCrystalExchangeShop_ButtonNextPage(ElementId elementId, std::int32_t state); // 下一页。

// 原函数：0x004C48F0。
static std::int32_t WindowCrystalExchangeShop_Scroll(ElementId elementId, std::int32_t state); // 滚轮，仅回写分页。

// 原函数：0x004C4940。
static std::int32_t WindowCrystalExchangeShop_ButtonDecrease(ElementId elementId, std::int32_t state); // 减数量，窗口70独立脏标。

// 原函数：0x004C4A50。
static std::int32_t WindowCrystalExchangeShop_ButtonIncrease(ElementId elementId, std::int32_t state); // 加数量，零上限采用交换文案。

// 原函数：0x004C54D0。
static std::int32_t WindowCrystalExchangeShop_ShowTooltip(
    std::int32_t index, std::int32_t page, std::int32_t unused); // 共用D303FC提示键。

// 原函数：0x004C3530。
static void WindowCrystalExchangeShop_Release();

// 原函数：0x004C3F00。
static std::int32_t WindowCrystalExchangeShop_Update(std::int32_t value);

// 原函数：0x004C4180。
static std::int32_t WindowCrystalExchangeShop_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::array<WindowCrystalItem, 20> gWindow70ItemsD30428 {}; // 原全局：0x00D30428。

static std::int32_t gWindow70ValueD67BA4 = 0; // 原全局：0x00D67BA4。

static std::uint8_t gWindow70FlagD673D8 = 0; // 原全局：0x00D673D8。

static std::int32_t gWindow70ImageD2E43C = 0; // 原全局：0x00D2E43C。

// 原全局：0x00D6791C。 第十协议字段，仍S32。
static std::int32_t gWindow70ValueD6791C = 0; // 原全局：0x00D6791C。

static std::int32_t gWindow70DialogTypeD45D6C = 0; // 原全局：0x00D45D6C。

static std::int32_t gWindow70ScrollCC4B68 = 0; // 原全局：0x00CC4B68。

static std::int32_t gWindow70DirtyD67BA8 = 0; // 原全局：0x00D67BA8。

static std::int32_t gWindow70CachedScroll580868 = -1; // 原全局：0x00580868。

static std::int32_t gWindow70TotalCB0F74 = 0; // 原全局：0x00CB0F74。

static std::int32_t gWindow70SlotsCF4E44 = 0; // 原全局：0x00CF4E44。

static std::array<std::wstring, 5> gWindow70PriceTextsCF49C4 {}; // 原全局：0x00CF49C4。

static std::array<std::wstring, 5> gWindow70CountTextsCC4848 {}; // 原全局：0x00CC4848。

static std::wstring gWindow70TotalTextD2D804 {}; // 原全局：0x00D2D804。

static std::int32_t gWindow70Tooltip580860 = -1; // 原全局：0x00580860。

static std::int32_t gWindow70Message580864 = -1; // 原全局：0x00580864。

static std::int32_t gWindow70TooltipPageD67BA0 = 0; // 原全局：0x00D67BA0。

static std::int32_t gWindow70InventoryHover58086C = -2; // 原全局：0x0058086C。

static std::wstring gWindow70BalanceTextCF3F14 {}; // 原全局：0x00CF3F14。

static WindowValueCallback gWindow70ConfirmCallbackCF4AA4 = nullptr; // 原全局：0x00CF4AA4。

static std::array<WindowConfirmationItem, 20> gWindow70ConfirmItemsCB0AE8 {}; // 原全局：0x00CB0AE8。

static std::array<WindowConfirmationAmount, 2> gWindow70ConfirmAmountsD2E290 {}; // 原全局：0x00D2E290。

static std::wstring gWindow70ConfirmHeadingCF4A40 {}; // 原全局：0x00CF4A40。

static std::wstring gWindow70ConfirmQuestionCF4ABC {}; // 原全局：0x00CF4ABC。

static std::wstring gWindow70ConfirmAcceptD2DD6C {}; // 原全局：0x00D2DD6C。

static std::wstring gWindow70ConfirmCancelCC48BC {}; // 原全局：0x00CC48BC。

// IWT01：晶石商品为含宽文本的内部记录。

static WindowGraphicData gWindowGraphic5762F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005762F0。

static WindowGraphicData gWindowGraphic576308 {0x18661, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576308。

static WindowGraphicData gWindowGraphic576320 {0x18662, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576320。

static WindowGraphicData gWindowGraphic576338 {0x18663, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576338。

static WindowGraphicData gWindowGraphic576350 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576350。

static WindowGraphicData gWindowGraphic576368 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576368。

static WindowGraphicData gWindowGraphic576380 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576380。

static WindowGraphicData gWindowGraphic576398 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576398。

static WindowGraphicData gWindowGraphic5763B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763B0。

static WindowGraphicData gWindowGraphic5763C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763C8。

static WindowGraphicData gWindowGraphic5763E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763E0。

static WindowGraphicData gWindowGraphic5763F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763F8。

static WindowGraphicData gWindowGraphic576410 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576410。

static WindowGraphicData gWindowGraphic576428 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576428。

static WindowGraphicData gWindowGraphic576440 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576440。

static WindowGraphicData gWindowGraphic576458 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576458。

static WindowGraphicData gWindowGraphic576470 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576470。

static WindowGraphicData gWindowGraphic576488 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576488。

static WindowGraphicData gWindowGraphic5764A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764A0。

static WindowGraphicData gWindowGraphic5764B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764B8。

static WindowGraphicData gWindowGraphic5764D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764D0。

static WindowTextPrototype gWindowText5764F8 { // 原全局：0x005764F8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText576500 { // 原全局：0x00576500。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText576508 { // 原全局：0x00576508。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前无法装备", // 原常量参考：0x00585DD8。
};

static WindowTextPrototype gWindowText576510 { // 原全局：0x00576510。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, L"剩余 %2d 个", // 原常量参考：0x00585DCC。
};

static std::int32_t gWindowAnimationImage576518 = 100000; // 原全局：0x00576518。

static WindowNumberData gWindowNumber576520 {&gWindow70BalanceTextCF3F14, 10, 3, 1, 0}; // 原全局：0x00576520。

static WindowNumberData gWindowNumber576530 {&gWindow70TotalTextD2D804, 0, 3, 1, 0}; // 原全局：0x00576530。

static WindowNumberData gWindowNumber576538 {&gWindow70PriceTextsCF49C4[0], 10, 3, 1, 0}; // 原全局：0x00576538。

static WindowNumberData gWindowNumber576540 {&gWindow70PriceTextsCF49C4[1], 10, 3, 1, 0}; // 原全局：0x00576540。

static WindowNumberData gWindowNumber576548 {&gWindow70PriceTextsCF49C4[2], 10, 3, 1, 0}; // 原全局：0x00576548。

static WindowNumberData gWindowNumber576550 {&gWindow70PriceTextsCF49C4[3], 10, 3, 1, 0}; // 原全局：0x00576550。

static WindowNumberData gWindowNumber576558 {&gWindow70PriceTextsCF49C4[4], 10, 3, 1, 0}; // 原全局：0x00576558。

static WindowNumberData gWindowNumber576560 {&gWindow70CountTextsCC4848[0], 0, 3, 1, 0}; // 原全局：0x00576560。

static WindowNumberData gWindowNumber576568 {&gWindow70CountTextsCC4848[1], 0, 3, 1, 0}; // 原全局：0x00576568。

static WindowNumberData gWindowNumber576570 {&gWindow70CountTextsCC4848[2], 0, 3, 1, 0}; // 原全局：0x00576570。

static WindowNumberData gWindowNumber576578 {&gWindow70CountTextsCC4848[3], 0, 3, 1, 0}; // 原全局：0x00576578。

static WindowNumberData gWindowNumber576580 {&gWindow70CountTextsCC4848[4], 0, 3, 1, 0}; // 原全局：0x00576580。

static std::uint32_t gWindowMouseFlagsD67920 = 0; // 原全局：0x00D67920。

// 原全局：0x00576588。
// 原576588的完整70项，保留70背包回调的单格命中语义。
static WindowElementPrototype gWindowElements576588[70] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleConfirmation, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleConfirmation)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5762F0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonClose)},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic576488}, &WindowCrystalExchangeShop_ButtonOk, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonOk)},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5764A0}, &WindowCrystalTrade_ButtonCancel, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonCancel)},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5764A0}, &WindowCrystalTrade_DrawButton, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::DrawButton)},
    {WindowElementType_Text, 425, 44, 0, 14, 1, {.text = &gWindowText5764F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element6)},
    {WindowElementType_Number, 260, 37, 0, 0, 1, {.number = &gWindowNumber576530}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element7)},
    {WindowElementType_Number, 400, 32, 0, 0, 1, {.number = &gWindowNumber576520}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element8)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576518}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element9)},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic576350}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element10)},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67920}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic576368}, &WindowCrystalExchangeShop_ButtonPreviousRow, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic576380}, &WindowCrystalExchangeShop_ButtonNextRow, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764B8}, &WindowCrystalExchangeShop_ButtonPreviousPage, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764D0}, &WindowCrystalExchangeShop_ButtonNextPage, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_Scroll, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Scroll)},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element17)},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element18)},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element19)},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element20)},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element21)},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element22)},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element23)},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element24)},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element25)},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element26)},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element27)},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element28)},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element29)},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element30)},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element31)},
    {WindowElementType_Text, 83, 87, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element32)},
    {WindowElementType_Text, 83, 142, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element33)},
    {WindowElementType_Text, 83, 197, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element34)},
    {WindowElementType_Text, 83, 252, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element35)},
    {WindowElementType_Text, 83, 307, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element36)},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber576538}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element37)},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber576540}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element38)},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber576548}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element39)},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber576550}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element40)},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber576558}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element41)},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber576560}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element42)},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber576568}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element43)},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber576570}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element44)},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber576578}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element45)},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber576580}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element46)},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic576398}, &WindowCrystalExchangeShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonDecrease)},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5763B0}, &WindowCrystalExchangeShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonDecrease_48)},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5763C8}, &WindowCrystalExchangeShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonDecrease_49)},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5763E0}, &WindowCrystalExchangeShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonDecrease_50)},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5763F8}, &WindowCrystalExchangeShop_ButtonDecrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonDecrease_51)},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic576410}, &WindowCrystalExchangeShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonIncrease)},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic576428}, &WindowCrystalExchangeShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonIncrease_53)},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic576440}, &WindowCrystalExchangeShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonIncrease_54)},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic576458}, &WindowCrystalExchangeShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonIncrease_55)},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic576470}, &WindowCrystalExchangeShop_ButtonIncrease, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::ButtonIncrease_56)},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleItemIcon)},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleItemIcon_58)},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleItemIcon_59)},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleItemIcon_60)},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleItemIcon_61)},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element62)},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element63)},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element64)},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element65)},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element66)},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleInventory, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleInventory)},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic576320}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element68)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic576308}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::Element69)},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowCrystalExchangeShopElementId::HandleSideArea)},
};

// 原型：窗口70；原全局：0x0054ECF0。
WindowPrototype gWindowPrototypeCrystalExchangeShop {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements576588, &WindowCrystalExchangeShop_Update, &WindowCrystalExchangeShop_Draw, &WindowCrystalExchangeShop_Release}; // 70: 54ECF0

// 窗口函数实现。

// 原函数：0x004C3530。
static void WindowCrystalExchangeShop_Release()
{
    const std::int32_t type = gWindow70DialogTypeD45D6C;
    if (type == world::gCurrentDialogType_581800) {
        world::gCurrentDialogType_581800 = -1;
    }
}

// 原函数：0x004C3550。
cg::yobi::YobiAction* WindowCrystalExchangeShop_Open(const char* const text)
{
    gWindow70ValueD67BA4 = 0;
    gWindow70FlagD673D8 = 0;
    gWindow70ImageD2E43C = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    std::array<char, 2304> buffer;
    for (std::int32_t group = 0; group < 7; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[group * 4 + row + 1].text, buffer.data(), row, 20));
        }
    }
    gWindow70ValueD6791C = network::ParseDelimitedInteger_52CFA0(text, '|', 10);
    gShopItemCountCB1E10 = 0;
    gWindow70ItemsD30428.fill({});
    std::int32_t field = 13;
    for (WindowCrystalItem* item = gWindow70ItemsD30428.data();
         item != gWindow70ItemsD30428.data() + gWindow70ItemsD30428.size(); ++item, field += 7) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field - 2, 767, buffer.data()) == 1) {
            break;
        }
        gWindow70ItemsD30428[gShopItemCountCB1E10].used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) > 28) {
            item->name = L"???";
        } else {
            gWindow70ItemsD30428[gShopItemCountCB1E10].name = cg::text::FromGbkWide(buffer.data());
        }
        item->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field - 1);
        item->valueC24 = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        item->limitC4C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        item->valueC2C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        item->remainingC50 = network::ParseDelimitedInteger_52CFA0(text, '|', field + 3);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field + 4, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::int32_t currentCount = gShopItemCountCB1E10;
        for (std::int32_t row = 0; row < 32; ++row) {
            auto& destination = gWindow70ItemsD30428[currentCount].description[row];
            char line[3072]{};
            const std::int32_t copied = input::ExtractLegacyLine_503AB0(
                line, 3072, buffer.data(), row, 32);
            destination = cg::text::FromGbkWide(line);
            currentCount = gShopItemCountCB1E10;
            if (copied == 0) {
                gWindow70ItemsD30428[currentCount].description[row].clear();
            }
        }
        gShopItemCountCB1E10 = currentCount + 1;
    }
    const std::int32_t type = world::gCurrentDialogType_581800;
    gShopExchangeCurrencyItemId_D2D608 = -1;
    gWindow70DialogTypeD45D6C = type;
    return WindowRuntime_CreateAnimation(WindowId_CrystalExchangeShop, 1, 0, nullptr);
}

// 原函数：0x004C39A0。
static void WindowCrystalExchangeShop_RefreshRemaining()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 31)); indexedElement) indexedElement->visible = 0;
        const WindowCrystalItem& item = gWindow70ItemsD30428[gWindow70ScrollCC4B68 + row];
        if (item.used != 0 && item.remainingC50 >= 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 31)); indexedElement2) indexedElement2->visible = 1;
            const std::int32_t index = gWindow70ScrollCC4B68 + row;
            WindowInstance* const window = currentWindow;
            const std::int32_t remaining = gWindow70ItemsD30428[index].remainingC50;
            auto* const indexedElement3 = window->FindElementById(NativeElementKey(row + 31));
            if (indexedElement3) {
                cg::text::FormatTo(indexedElement3->data.text->text, L"剩余 {:2} 个", remaining);
            }
        }
    }
}

// 原函数：0x004C3A50。
static void WindowCrystalExchangeShop_RefreshRows()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        WindowInstance* const initial = currentWindow;
        std::wstring& price = gWindow70PriceTextsCF49C4[row];
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
        const WindowCrystalItem* item = &gWindow70ItemsD30428[gWindow70ScrollCC4B68 + row];
        if (item->used == 0) {
            continue;
        }
        cg::text::FormatTo(price, L"{:10}", item->valueC24);
        item = &gWindow70ItemsD30428[gWindow70ScrollCC4B68 + row];
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
                gWindow70ItemsD30428[gWindow70ScrollCC4B68 + row].flags;
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

// 原函数：0x004C3D40。
static void WindowCrystalExchangeShop_RefreshQuantities()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t scroll = gWindow70ScrollCC4B68;
        std::wstring& text = gWindow70CountTextsCC4848[row];
        text.clear();
        const WindowCrystalItem& item = gWindow70ItemsD30428[scroll + row];
        if (item.used != 0) {
            cg::text::FormatTo(text, L"{:2}", item.selectedCount);
        }
    }
}

// 原函数：0x004C3DA0。
static void WindowCrystalExchangeShop_RefreshAmount()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    std::int32_t total = 0;
    gWindow70TotalCB0F74 = 0;
    for (const WindowCrystalItem& item : gWindow70ItemsD30428) {
        if (item.used != 0) {
            const std::int32_t quantity = item.selectedCount;
            total += quantity * item.valueC24;
        }
    }
    gWindow70TotalCB0F74 = total;
    cg::text::FormatTo(gWindow70TotalTextD2D804, L"{:10}", total);
    const std::int32_t owned = world::CountInventoryItemQuantity_51B5F0(gWindow70ValueD6791C);
    const std::int32_t currentTotal = gWindow70TotalCB0F74;
    auto* const element7 = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::Element7);
    if (element7) {
        auto* const number = element7->data.number;
        number->color = currentTotal <= owned ? 10 : 6;
    }
    return;
}

// 原函数：0x004C3E20。
static void WindowCrystalExchangeShop_CountSlots()
{
    std::int32_t slots = 0;
    gWindow70SlotsCF4E44 = 0;
    for (const WindowCrystalItem& item : gWindow70ItemsD30428) {
        if (item.used != 0) {
            std::int32_t stack = item.valueC2C;
            const std::int32_t quantity = item.selectedCount;
            if (stack <= 0) {
                stack = 2;
            }
            slots += (stack + quantity - 1) / stack;
        }
    }
    gWindow70SlotsCF4E44 = slots;
}

// 原函数：0x004C3E70。
static void WindowCrystalExchangeShop_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const std::int32_t initialized = gWindow70ValueD67BA4;
    const std::int32_t scroll = gWindow70ScrollCC4B68;
    if (initialized != 0 && gWindow70CachedScroll580868 == scroll) {
        if (gWindow70DirtyD67BA8 != 0) {
            gWindow70DirtyD67BA8 = 0;
            WindowCrystalExchangeShop_RefreshAmount();
            WindowCrystalExchangeShop_RefreshQuantities();
            WindowCrystalExchangeShop_CountSlots();
        }
        return;
    }
    gWindow70ValueD67BA4 = 1;
    gWindow70CachedScroll580868 = scroll;
    WindowCrystalExchangeShop_RefreshRows();
    WindowCrystalExchangeShop_RefreshAmount();
    WindowCrystalExchangeShop_RefreshQuantities();
    WindowCrystalExchangeShop_CountSlots();
    WindowCrystalExchangeShop_RefreshRemaining();
    const std::int32_t position = gWindow70ScrollCC4B68;
    WindowInstance* const window = currentWindow;
    const std::int32_t maximum = gShopItemCountCB1E10 - 5;
    auto* const handleVerticalScrollElement = window->FindElementById(WindowCrystalExchangeShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
    }
    return;
}

// 原函数：0x004C3F00。
static std::int32_t WindowCrystalExchangeShop_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const entryWindow = currentWindow;
    if (value == -1) {
        gWindow70ScrollCC4B68 = 0;
        gShopAvailableCF4A24 = -1;
        if (auto* const handleConfirmationElement = entryWindow->FindElementById(WindowCrystalExchangeShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 0;
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
            if (auto* const indexedElement10 = currentWindow->FindElementById(NativeElementKey(56 + row)); indexedElement10) indexedElement10->mouseCallback = &WindowCrystalExchangeShop_HandleItemIcon;
            auto* const indexedElement11 = currentWindow->FindElementById(NativeElementKey(61 + row));
            if (indexedElement11) {
                indexedElement11->data.graphic->imageNo =
                    99939;
            }
        }
        if (auto* const drawButtonElement = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::DrawButton); drawButtonElement) drawButtonElement->visible = 0;
        WindowInstance* const window = currentWindow;
        gShopRowKeysC9A954[0] = -2;
        gWindow70Tooltip580860 = -1;
        gShopRowKeysC9A954[1] = -2;
        gWindow70Message580864 = -1;
        gShopRowKeysC9A954[2] = -2;
        gWindow70TooltipPageD67BA0 = 0;
        gShopRowKeysC9A954[3] = -2;
        gShopRowKeysC9A954[4] = -2;
        auto* const element6 = window->FindElementById(WindowCrystalExchangeShopElementId::Element6);
        if (element6) {
            auto* const title = element6->data.text;
            title->text = gNpcDialogLinesCC24D4[0].text;
        }
        auto* const element9 = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::Element9);
        if (element9) {
            auto* animation = element9->data.animation;
            const std::int32_t imageNo = gWindow70ImageD2E43C;
            cg::yobi::SetYobiActionField2A8(animation->action, imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    const std::int32_t type = gWindow70DialogTypeD45D6C;
    if (type != world::gCurrentDialogType_581800) {
        currentWindow->state |= 0x8000u;
    }
    if (auto* const element10 = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::Element10); element10) element10->visible = 1;
    if (gShopItemCountCB1E10 - 5 <= 0) {
        if (auto* const element102 = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::Element10); element102) element102->visible = 0;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowCrystalExchangeShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
            auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowCrystalExchangeShopElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement2) {
                gWindow70ScrollCC4B68 = WindowScroll_ReadPosition(handleVerticalScrollElement2, gShopItemCountCB1E10 - 5);
            }
        }
    }
    const std::int32_t owned = world::CountInventoryItemQuantity_51B5F0(gWindow70ValueD6791C);
    cg::text::FormatTo(gWindow70BalanceTextCF3F14, L"{:10}", owned);
    if (gWindow70Message580864 >= 0) {
        gWindow70Message580864 = -1;
    }
    gWindow70Tooltip580860 = -1;
    return 1;
}

// 原函数：0x004C4180。
static std::int32_t WindowCrystalExchangeShop_Draw(const std::int32_t unused)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unused);
    const WindowInstance* const window = currentWindow;
    const std::int32_t x = window->x + 298;
    std::int32_t y = window->y + 50;
    const std::int32_t tooltip = gWindow70Tooltip580860;
    if (tooltip >= 0) {
        if (tooltip < 100) {
            static_cast<void>(WindowItem_DrawEquipmentTooltip(tooltip, gWindow70TooltipPageD67BA0));
        } else if (tooltip < 200) {
            const std::uint8_t flag = gWindow70FlagD673D8;
            const std::int32_t page = gWindow70TooltipPageD67BA0;
            static_cast<void>(WindowCrystalExchangeShop_ShowTooltip(tooltip - 100, page, flag == 0));
        } else {
            static_cast<void>(WindowItem_DrawWorkTooltip(gWindow70TooltipPageD67BA0));
        }
    }
    const std::int32_t message = gWindow70Message580864;
    gTooltipColorD67D30 = 0;
    const WindowDialogTextLine* line = gNpcDialogLinesCC24D4.data() + 1 + 4 * (-1 - message);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (!line->text.empty()) {
            static_cast<void>(WindowItem_SplitTooltipLines(x, y, 3, 8, 0, line->text.data(), 0, 0));
        }
        ++line;
        y += 10;
    }
    WindowCrystalExchangeShop_Refresh();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004C4270。
static void WindowCrystalExchangeShop_Submit(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
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
        const WindowCrystalItem& item = gWindow70ItemsD30428[index];
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
    const std::int32_t target = world::gDialogTargetD2DE6C;
    const std::int32_t request = world::gDialogRequestCC21E0;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(cg::network::GetGameConnection().SendDialogResponse(x, y, request, target, 1, escaped));
    currentWindow->state |= 0x8000u;
}

// 原函数：0x004C43C0。
static std::int32_t WindowCrystalExchangeShop_ButtonOk(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        gHoverTextD68AE8 = L"买进指定的物品。" /* 原常量参考：0x00587FA8。 */;
    }
    if (gWindow70SlotsCF4E44 <= 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t balance = world::CountInventoryItemQuantity_51B5F0(gWindow70ValueD6791C);
        if (balance < gWindow70TotalCB0F74) {
            gWindow70Message580864 = -2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            const std::int32_t freeSlots = world::CountFreeInventorySlots_518C60();
            if (gWindow70SlotsCF4E44 > freeSlots) {
                gWindow70Message580864 = -3;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                gWindow70ConfirmCallbackCF4AA4 = &WindowCrystalExchangeShop_Submit;
                std::int32_t count = 0;
                for (const WindowCrystalItem& item : gWindow70ItemsD30428) {
                    if (item.used == 0) {
                        continue;
                    }
                    const std::int32_t quantity = item.selectedCount;
                    if (quantity == 0) {
                        continue;
                    }
                    WindowConfirmationItem& preview = gWindow70ConfirmItemsCB0AE8[count];
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
                    gWindow70ConfirmItemsCB0AE8[count].visible = 0;
                }
                WindowConfirmationAmount& left = gWindow70ConfirmAmountsD2E290[0];
                WindowConfirmationAmount& right = gWindow70ConfirmAmountsD2E290[1];
                const std::int32_t itemId = gWindow70ValueD6791C;
                left.visible = 1;
                left.x = 296;
                left.y = 343;
                left.value = world::CountInventoryItemQuantity_51B5F0(itemId);
                const std::int32_t total = gWindow70TotalCB0F74;
                right.visible = 1;
                right.x = 454;
                right.y = 343;
                right.value = total;
                (gWindow70ConfirmHeadingCF4A40 = L"持有交易品               花费交易品            " /* 原常量参考：0x005883D4。 */);
                (gWindow70ConfirmQuestionCF4ABC = L"确认要交换这些物品？" /* 原常量参考：0x005883BC。 */);
                gWindow70ConfirmAcceptD2DD6C = L"买进物品。" /* 原常量参考：0x00587F00。 */;
                gWindow70ConfirmCancelCC48BC = L"还是算了。" /* 原常量参考：0x00585AD0。 */;
                if (auto* const handleConfirmationElement = currentWindow->FindElementById(WindowCrystalExchangeShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 1;
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
    if (gWindow70SlotsCF4E44 <= 0) {
        if (graphic) graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004C46C0。
static std::int32_t WindowCrystalExchangeShop_ButtonPreviousRow(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t position = gWindow70ScrollCC4B68;
            if (position > 0) {
                gWindow70ScrollCC4B68 = position - 1;
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

// 原函数：0x004C4740。
static std::int32_t WindowCrystalExchangeShop_ButtonNextRow(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t count = gShopItemCountCB1E10;
            const std::int32_t position = gWindow70ScrollCC4B68;
            if (position < count - 5) {
                gWindow70ScrollCC4B68 = position + 1;
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

// 原函数：0x004C47C0。
static std::int32_t WindowCrystalExchangeShop_ButtonPreviousPage(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(gWindow70ScrollCC4B68 > 0 ? 51 : 55, 320));
            const std::int32_t position = gWindow70ScrollCC4B68 - 5;
            gWindow70ScrollCC4B68 = position;
            if (position < 0) {
                gWindow70ScrollCC4B68 = 0;
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

// 原函数：0x004C4840。
static std::int32_t WindowCrystalExchangeShop_ButtonNextPage(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t oldMaximum = gShopItemCountCB1E10 - 5;
            const bool canAdvance = oldMaximum > 0 && gWindow70ScrollCC4B68 < oldMaximum;
            static_cast<void>(audio::PlaySe_52C450(canAdvance ? 51 : 55, 320));
            std::int32_t position = gWindow70ScrollCC4B68 + 5;
            const std::int32_t maximum = gShopItemCountCB1E10 - 5;
            gWindow70ScrollCC4B68 = position;
            if (position > maximum) {
                position = maximum;
                gWindow70ScrollCC4B68 = position;
            }
            if (position < 0) {
                gWindow70ScrollCC4B68 = 0;
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

// 原函数：0x004C48F0。
static std::int32_t WindowCrystalExchangeShop_Scroll(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = gShopItemCountCB1E10;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = gWindow70ScrollCC4B68;
        WindowInstance* const window = currentWindow;
        auto* const indexedElement = window->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            gWindow70ScrollCC4B68 =
                WindowScroll_ApplyWheel(indexedElement, position, count - 5, wheel);
        }
    }
    return 0;
}

// 原函数：0x004C4940。
static std::int32_t WindowCrystalExchangeShop_ButtonDecrease(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        WindowCrystalItem* const item = &gWindow70ItemsD30428[gWindow70ScrollCC4B68 + index - 46];
        std::int32_t consumed = 0;
        if (hover) {
            const wchar_t* const hint = item->selectedCount > 0
                ? L"减少物品的数量。"
                : L"无法再减少物品的数量了。";
            gHoverTextD68AE8 = hint;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t count = item->selectedCount;
            if (count > 0) {
                item->selectedCount = count - 1;
                gWindow70DirtyD67BA8 = 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            gWindow70Message580864 = -1;
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

// 原函数：0x004C4A50。
static std::int32_t WindowCrystalExchangeShop_ButtonIncrease(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        WindowCrystalItem* const item = &gWindow70ItemsD30428[gWindow70ScrollCC4B68 + index - 51];
        std::int32_t consumed = 0;
        if (hover) {
            const std::int32_t limit = item->limitC4C;
            const std::int32_t count = item->selectedCount;
            const wchar_t* hint;
            if (count < limit) {
                hint = L"增加物品的数量。";
            } else if (limit == 0) {
                hint = L"已无商品交换!";
            } else {
                hint = L"无法再增加物品的数量了。";
            }
            gHoverTextD68AE8 = hint;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            if (item->used != 0) {
                const std::int32_t count = item->selectedCount;
                const std::int32_t limit = item->limitC4C;
                if (count < limit) {
                    item->selectedCount = count + 1;
                    gWindow70DirtyD67BA8 = 1;
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

// 原函数：0x004C4B70。
static std::int32_t WindowCrystalExchangeShop_HandleItemIcon(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t selected = gWindow70ScrollCC4B68 + index - 56;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    const WindowCrystalItem* const item = &gWindow70ItemsD30428[selected];
    if (item->used == 0) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        if (gShopRowKeysC9A954[index - 56] != selected) {
            gWindow70TooltipPageD67BA0 = 0;
        }
        gShopRowKeysC9A954[index - 56] = selected;
        if ((state & MouseFlag_RDownEvent) != 0) {
            ++gWindow70TooltipPageD67BA0;
            if (gWindow70TooltipPageD67BA0 >= 4) {
                gWindow70TooltipPageD67BA0 = 0;
            }
            consumed = 1;
        }
        gWindow70Tooltip580860 = selected + 100;
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

// 原函数：0x004C4CB0。
static std::int32_t WindowCrystalExchangeShop_HandleInventory(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const WindowInstance* const window = currentWindow;
        auto* elementsWindow = window;
        const std::int32_t windowX = window->x;
        const std::int32_t windowY = window->y;
        auto* const elementPtr = elementsWindow->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t left = element.x + windowX + 2;
            const std::int32_t top = element.y + windowY + 2;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = 0;
            }
        }
    }
    if (gWindow70InventoryHover58086C != hovered) {
        gWindow70TooltipPageD67BA0 = 0;
        gWindow70InventoryHover58086C = hovered;
    }
    const auto& items = world::gPlayerRuntimeSlots.items;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    } else if ((events & MouseFlag_Hover) != 0 && hovered >= 0 && (events & MouseFlag_RDownEvent) != 0) {
        if (items[hovered + 8].used != 0) {
            std::int32_t page = gWindow70TooltipPageD67BA0;
            const std::int32_t pages = items[hovered + 8].pages;
            ++page;
            gWindow70TooltipPageD67BA0 = page;
            if (page >= pages) {
                gWindow70TooltipPageD67BA0 = 0;
            }
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
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
            gWindow70Tooltip580860 = hovered + 8;
            if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                gHoverTextD68AE8 = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                gHoverTextD68AE8 = item.memo + L"[han]";
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
            if (itemId == gShopExchangeCurrencyItemId_D2D608) {
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

// 原函数：0x004C50C0。
static std::int32_t WindowCrystalExchangeShop_HandleConfirmation(const ElementId elementId, const std::int32_t unusedState)
{
    WindowInstance* const currentWindow = gCurrentWindow;
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
        gHoverTextD68AE8 = gWindow70ConfirmAcceptD2DD6C;
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = gWindow70ConfirmCallbackCF4AA4;
            if (callback != nullptr) {
                callback(0);
            }
            acceptImage = 245324;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    std::int32_t cancelImage = 245326;
    if (cancelHover) {
        cancelImage = 245328;
        gHoverTextD68AE8 = gWindow70ConfirmCancelCC48BC;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        static_cast<void>(0);
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = gWindow70ConfirmCallbackCF4AA4;
            if (callback != nullptr) {
                callback(1);
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
            const WindowConfirmationItem& item = gWindow70ConfirmItemsCB0AE8[column * 5 + row];
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
    static_cast<void>(render::DrawText_456780(190, 377, 4, 12, 0, gWindow70ConfirmHeadingCF4A40, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    static_cast<void>(render::DrawText_456780(190, 393, 4, 12, 0, gWindow70ConfirmQuestionCF4ABC, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    for (const WindowConfirmationAmount& amount : gWindow70ConfirmAmountsD2E290) {
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

// 原函数：0x004C54D0。
static std::int32_t WindowCrystalExchangeShop_ShowTooltip(
    const std::int32_t index, const std::int32_t page, const std::int32_t unused)
{
    static_cast<void>(unused);
    const std::int32_t key = index + 300;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    gTooltipSourceD303FC = key;
    const WindowCrystalItem* const item = &gWindow70ItemsD30428[index];
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
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 8, nullptr, 4);
}

} // namespace detail

} // namespace cg::ui
