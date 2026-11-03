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
enum class WindowShopElementId : ElementId {
    HandleConfirmation = 1,
    ButtonClose = 2,
    ButtonOk = 3,
    ButtonCancel = 4,
    ButtonAddAll = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    HandleVerticalScroll = 12,
    ButtonPreviousRow = 13,
    ButtonNextRow = 14,
    ButtonPreviousPage = 15,
    ButtonNextPage = 16,
    Scroll = 17,
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
    ButtonDecrease = 43,
    ButtonDecrease_44 = 44,
    ButtonDecrease_45 = 45,
    ButtonDecrease_46 = 46,
    ButtonDecrease_47 = 47,
    ButtonIncrease = 48,
    ButtonIncrease_49 = 49,
    ButtonIncrease_50 = 50,
    ButtonIncrease_51 = 51,
    ButtonIncrease_52 = 52,
    HandleItemIcon = 53,
    HandleItemIcon_54 = 54,
    HandleItemIcon_55 = 55,
    HandleItemIcon_56 = 56,
    HandleItemIcon_57 = 57,
    Element58 = 58,
    Element59 = 59,
    Element60 = 60,
    Element61 = 61,
    Element62 = 62,
    HandleInventory = 63,
    HandleSaleDrop = 64,
    Element65 = 65,
    HandleSideArea = 66,
};

namespace detail {

// G08：仅参考实际安装的mode 0增减路径使用，其他数量窗口保留原上限。
static constexpr std::int32_t kShopPurchaseQuantityLimit = 9999;

static std::int32_t CalculateRepeatedShopQuantity(
    session::ClientSession& session, const std::int32_t candidate, const std::int32_t state, const bool increase)
{
    auto& shop = session.UiFrame().native.shop;
    auto& pressedTick = shop.pressedTicks[increase ? 1 : 0];
    const std::uint64_t now = platform::ReadTickCount64();
    if ((state & MouseFlag_LDownEvent) != 0) {
        pressedTick = now;
    }
    return now > pressedTick + 1000u
        ? std::min<std::int32_t>(candidate * (increase ? 120 : 80) / 100, kShopPurchaseQuantityLimit)
        : candidate;
}

// IWT01：商品文本为内部对象，不再依赖原C58H字节布局。

// 回调与内部函数前置声明。

// 原函数：0x0048B560。
static std::int32_t WindowItem_DrawShopTooltip(
    session::ClientSession& session, std::int32_t index, std::int32_t page, std::int32_t showUnavailable); // 同源商品及可选不可装备提示。

// 原函数：0x004B8310。
static void WindowShop_RefreshBuyRows(WindowInstance& callbackWindow); // 买入五行名称/价格/不可装备提示，保留原分行终止偏移。

// 原函数：0x004B8670。
static void WindowShop_RefreshBuyQuantities(WindowInstance& callbackWindow); // 买入五行所选数量文本。

// 原函数：0x004B86C0。
static void WindowShop_RefreshBuyAmount(WindowInstance& callbackWindow); // 全20商品金额合计及元素7颜色。

// 原函数：0x004B8730。
static void WindowShop_CountBuySlots(WindowInstance& callbackWindow); // 全20商品按上限分组计算占用数量。

// 原函数：0x004B8780。
static void WindowShop_RefreshSellRows(WindowInstance& callbackWindow); // 出售五行按S8选择槽取商品，不另检查use。

// 原函数：0x004B8B10。
static void WindowShop_RefreshSellQuantities(WindowInstance& callbackWindow); // 出售数量及上限>1时的加减元素可见性。

// 原函数：0x004B8BE0。
static void WindowShop_RefreshSellAmount(WindowInstance& callbackWindow); // 全20选择槽的金额合计，元素7颜色固定0。

// 原函数：0x004B8C50。
static void WindowShop_CountSellItems(WindowInstance& callbackWindow); // 全20选择槽直接累计所选数，不做买入分组。

// 原函数：0x004B8C90。
static void WindowShop_RefreshExchangeRows(WindowInstance& callbackWindow); // 模式2五行，按指定物品数量与价格加合计比较。

// 原函数：0x004B8FF0。
static void WindowShop_RefreshExchangeQuantities(WindowInstance& callbackWindow); // 模式2数量，使用项直接重显加减元素。

// 原函数：0x004B9090。
static void WindowShop_RefreshExchangeAmount(WindowInstance& callbackWindow); // 模式2合计写CF4A28，不写金额颜色。

// 原函数：0x004B90E0。
static void WindowShop_CountExchangeItems(WindowInstance& callbackWindow); // 模式2全部使用商品累计所选数。

// 原函数：0x004B9110。
static void WindowShop_RefreshDetailName(WindowInstance& callbackWindow); // 模式3首行详情名与数值，短名称的次行为空文本。

// 原函数：0x004B9270。
static void WindowShop_RefreshDetailAmount(WindowInstance& callbackWindow); // 模式3三次读取D046C4，先两个显示值后乘详情单价。

// 原函数：0x004B92C0。
static void WindowShop_CountDetailItems(WindowInstance& callbackWindow); // 模式3详情数值或全部28玩家物品的匹配数量。

// 原函数：0x004BBED0。
static std::int32_t WindowShop_HandleItemIcon(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 商品图标，仅hover内bit4翻页。

// 原函数：0x004BC060。
static std::int32_t WindowShop_HandleDetailIcon(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 独立详情图标，bit4无hover门禁。

// 原函数：0x004BC230。
static std::int32_t WindowShop_HandleSaleSelection(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 出售选择行，接kind9交换/取消/拖动。

// 原函数：0x004BC660。
static std::int32_t WindowShop_HandleInventory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 背包20格命中、加入/拖动/翻页及两组绘制。

// 原函数：0x004B9B10。
static void WindowShop_Submit(WindowInstance& callbackWindow, std::int32_t selection); // 确认选择低U8为0才按模式发送，返回值不消费。

// 原函数：0x004B9F40。
static std::int32_t WindowShop_ButtonOk(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六模式确认生产，借用同源覆盖层。

// 原函数：0x004BD100。
static std::int32_t WindowShop_HandleConfirmation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 确认覆盖层，事件直接读共享鼠标。

// 原函数：0x004BA950。
static std::int32_t WindowShop_ButtonCancel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 取消发送command2，发送后关闭。

// 原函数：0x004BAA50。
static std::int32_t WindowShop_ButtonAddAll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 批量加入出售选择，每轮固定重置/音效尾。

// 原函数：0x004BCEE0。
static std::int32_t WindowShop_HandleSaleDrop(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // kind2拖放加入，hover模式门禁独立。

// 原函数：0x004BAD10。
static std::int32_t WindowShop_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // bit400向前一行，触边仍刷新。

// 原函数：0x004BAE10。
static std::int32_t WindowShop_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // bit400向后一行，触边仍刷新。

// 原函数：0x004BAF10。
static std::int32_t WindowShop_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // bit2向前五行，音效后重读位置。

// 原函数：0x004BB010。
static std::int32_t WindowShop_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // bit2向后五行，先上限后负值修正。

// 原函数：0x004BB130。
static std::int32_t WindowShop_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 共享滚轮更新并刷新，恒返回0。

// 原函数：0x004BB1D0。
static std::int32_t WindowShop_ButtonDecrease(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 数量减少，兑换模式两轮背包占槽计算。

// 原函数：0x004BB6E0。
static std::int32_t WindowShop_ButtonIncrease(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 数量增加，保留行价格与选择槽索引差别。

// 原函数：0x004B6C50。
static void WindowShop_HandleSaleDrag(
    session::ClientSession& session,
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind9出售拖动生命周期。

// 原函数：0x004B6DA0。
static std::int32_t WindowShop_StartSaleDrag(session::ClientSession& session, std::int32_t sourceIndex); // 透传kind9注册返回。

// 原函数：0x004BC150。
static void WindowShop_RemoveSaleSelection(session::ClientSession& session, WindowInstance* currentWindow); // 三次读取来源，撤销选择并刷新窗口53。

// 原函数：0x004B6DC0。
static void WindowShop_Release(WindowInstance&);

// 原函数：0x004B9310。
static std::int32_t WindowShop_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004B99D0。
static std::int32_t WindowShop_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr wchar_t kShopUnavailableHint586BF4[] = L"＊现在无法装备"; // 原常量参考：0x00586BF4。

static std::int32_t gQuantityLimit581C94 = 999; // 原全局：0x00581C94。 原装载值999，商店及其他数量窗口同源上限。

// 4B9F40六模式原字节文案，保留格式与定宽空格。
static constexpr std::array<const wchar_t*, 6> kShopConfirmHoverHints {
    L"买进指定的物品。",
    L"卖掉指定的物品。",
    L"交换指定的物品。",
    L"交换指定的物品。",
    L"鉴定指定的物品。",
    L"修理指定的物品。",
};

static constexpr std::array<const wchar_t*, 6> kShopConfirmHeadings {
    L"持有金额           金币    购买金额           金币",
    L"持有金额           金币    卖出金额           金币",
    L"{:<24}    个  交换数      个",
    L"手持数          个    给予个数           个",
    L"持有金额           金币    鉴定金额           金币",
    L"持有金额           金币    修理金额           金币",
};

static constexpr std::array<const wchar_t*, 6> kShopConfirmQuestions {
    L"确认要购买这些物品？",
    L"卖掉这些物品。可以吗？",
    L"交换这些物品。可以吗？",
    L"给予这些物品。可以吗？",
    L"鉴定这些物品。可以吗？",
    L"修理这些物品。可以吗？",
};

static constexpr std::array<const wchar_t*, 6> kShopConfirmAcceptHints {
    L"买进物品。",
    L"卖掉。",
    L"进行交换。",
    L"进行交换。",
    L"进行鉴定。",
    L"进行修理。",
};

static WindowGraphicData gWindowGraphic573AF0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573AF0。

static WindowGraphicData gWindowGraphic573B08 {0x3BA9C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B08。

static WindowGraphicData gWindowGraphic573B20 {0x3BA9D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B20。

static WindowGraphicData gWindowGraphic573B38 {0x3BA9E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B38。

static WindowGraphicData gWindowGraphic573B50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B50。

static WindowGraphicData gWindowGraphic573B68 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B68。

static WindowGraphicData gWindowGraphic573B80 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B80。

static WindowGraphicData gWindowGraphic573B98 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B98。

static WindowGraphicData gWindowGraphic573BB0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BB0。

static WindowGraphicData gWindowGraphic573BC8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BC8。

static WindowGraphicData gWindowGraphic573BE0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BE0。

static WindowGraphicData gWindowGraphic573BF8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BF8。

static WindowGraphicData gWindowGraphic573C10 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C10。

static WindowGraphicData gWindowGraphic573C28 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C28。

static WindowGraphicData gWindowGraphic573C40 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C40。

static WindowGraphicData gWindowGraphic573C58 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C58。

static WindowGraphicData gWindowGraphic573C70 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C70。

static WindowGraphicData gWindowGraphic573C88 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C88。

static WindowGraphicData gWindowGraphic573CA0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CA0。

static WindowGraphicData gWindowGraphic573CB8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CB8。

static WindowGraphicData gWindowGraphic573CD0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CD0。

static WindowTextPrototype gWindowText573CF8 { // 原全局：0x00573CF8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText573D00 { // 原全局：0x00573D00。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};

static WindowTextPrototype gWindowText573D08 { // 原全局：0x00573D08。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前无法装备", // 原常量参考：0x00585DD8。
};

static std::int32_t gWindowAnimationImage573D10 = 100000; // 原全局：0x00573D10。

static WindowNumberData gWindowNumber573D18 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D18。

static WindowNumberData gWindowNumber573D20 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D20。

static WindowNumberData gWindowNumber573D28 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D28。

static WindowNumberData gWindowNumber573D30 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D30。

static WindowNumberData gWindowNumber573D38 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D38。

static WindowNumberData gWindowNumber573D40 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D40。

static WindowNumberData gWindowNumber573D48 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D48。

static WindowNumberData gWindowNumber573D50 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D50。

static WindowNumberData gWindowNumber573D58 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D58。

static WindowNumberData gWindowNumber573D60 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D60。

static WindowNumberData gWindowNumber573D68 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D68。

static WindowNumberData gWindowNumber573D70 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D70。

static WindowNumberData gWindowNumber573D78 {nullptr, 0, 3, 1, 0}; // 原全局：0x00573D78。

// 51的flags源与49图形源+10H重叠，实例仍分别按type4/type1创建。
static std::uint32_t& gWindowMouseFlagsD678F0 = // 原全局：0x00D678F0。
    reinterpret_cast<std::uint32_t&>(gWindowGraphicD678E0.height);

static WindowElementPrototype gWindowElements573D80[66] { // 原全局：0x00573D80。
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowShop_HandleConfirmation, -1, static_cast<ElementId>(WindowShopElementId::HandleConfirmation)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic573AF0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowShopElementId::ButtonClose)},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic573C88}, &WindowShop_ButtonOk, -1, static_cast<ElementId>(WindowShopElementId::ButtonOk)},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic573CA0}, &WindowShop_ButtonCancel, -1, static_cast<ElementId>(WindowShopElementId::ButtonCancel)},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic573CA0}, &WindowShop_ButtonAddAll, -1, static_cast<ElementId>(WindowShopElementId::ButtonAddAll)},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText573CF8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element6)},
    {WindowElementType_Number, 124, 37, 0, 0, 1, {.number = &gWindowNumber573D20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element7)},
    {WindowElementType_Number, 255, 37, 0, 0, 1, {.number = &gWindowNumber573D28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element8)},
    {WindowElementType_Number, 389, 30, 0, 0, 1, {.number = &gWindowNumber573D18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element9)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage573D10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element10)},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic573B50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element11)},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD678F0}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowShopElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic573B68}, &WindowShop_ButtonPreviousRow, -1, static_cast<ElementId>(WindowShopElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic573B80}, &WindowShop_ButtonNextRow, -1, static_cast<ElementId>(WindowShopElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CB8}, &WindowShop_ButtonPreviousPage, -1, static_cast<ElementId>(WindowShopElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CD0}, &WindowShop_ButtonNextPage, -1, static_cast<ElementId>(WindowShopElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowShop_Scroll, -1, static_cast<ElementId>(WindowShopElementId::Scroll)},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element18)},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element19)},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element20)},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element21)},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element22)},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element23)},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element24)},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element25)},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element26)},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element27)},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element28)},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element29)},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element30)},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element31)},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element32)},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber573D30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element33)},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber573D38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element34)},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber573D40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element35)},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber573D48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element36)},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber573D50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element37)},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber573D58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element38)},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber573D60}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element39)},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber573D68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element40)},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber573D70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element41)},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber573D78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element42)},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic573B98}, &WindowShop_ButtonDecrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonDecrease)},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic573BB0}, &WindowShop_ButtonDecrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonDecrease_44)},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic573BC8}, &WindowShop_ButtonDecrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonDecrease_45)},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic573BE0}, &WindowShop_ButtonDecrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonDecrease_46)},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic573BF8}, &WindowShop_ButtonDecrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonDecrease_47)},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic573C10}, &WindowShop_ButtonIncrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonIncrease)},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic573C28}, &WindowShop_ButtonIncrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonIncrease_49)},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic573C40}, &WindowShop_ButtonIncrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonIncrease_50)},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic573C58}, &WindowShop_ButtonIncrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonIncrease_51)},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic573C70}, &WindowShop_ButtonIncrease, -1, static_cast<ElementId>(WindowShopElementId::ButtonIncrease_52)},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon, -1, static_cast<ElementId>(WindowShopElementId::HandleItemIcon)},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon, -1, static_cast<ElementId>(WindowShopElementId::HandleItemIcon_54)},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon, -1, static_cast<ElementId>(WindowShopElementId::HandleItemIcon_55)},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon, -1, static_cast<ElementId>(WindowShopElementId::HandleItemIcon_56)},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon, -1, static_cast<ElementId>(WindowShopElementId::HandleItemIcon_57)},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element58)},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element59)},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element60)},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element61)},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element62)},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowShop_HandleInventory, -1, static_cast<ElementId>(WindowShopElementId::HandleInventory)},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic573B20}, &WindowShop_HandleSaleDrop, -1, static_cast<ElementId>(WindowShopElementId::HandleSaleDrop)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic573B08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopElementId::Element65)},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowShopElementId::HandleSideArea)},
};

// 原型：窗口53；原全局：0x0054EC60。
WindowPrototype gWindowPrototypeShop {0, 4, 10, 30, 549, 366, 0x80010101u, 66, gWindowElements573D80, &WindowShop_Update, &WindowShop_Draw, &WindowShop_Release}; // 53: 54EC60

// 窗口函数实现。

// 原函数：0x0048B560。
static std::int32_t WindowItem_DrawShopTooltip(
    session::ClientSession& session, const std::int32_t index, const std::int32_t page, const std::int32_t showUnavailable)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& shop = session.UiFrame().native.shop;
    std::int32_t key = index + 300;
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
    const auto* record = &shop.items[index];
    std::uint32_t flags;
    std::memcpy(&flags, &record->flags, sizeof(flags));
    std::wstring title = record->memo;
    std::int8_t titleColor;
    if ((flags & 0x100u) != 0) {
        titleColor = 5;
    } else {
        std::int32_t count;
        std::memcpy(&count, &record->count, sizeof(count));
        if (count <= 1) {
            title = record->name;
        } else {
            title = std::format(L"{}({})", record->name, count);
            key = interaction.tooltipSource;
        }
        record = &shop.items[key - 300];
        std::memcpy(&flags, &record->flags, sizeof(flags));
        titleColor = static_cast<std::int8_t>((flags >> 9) & 1u);
    }
    const wchar_t* const footer = record->allowed == 0 && showUnavailable != 0
        ? kShopUnavailableHint586BF4 : nullptr;
    const auto* const lines = record->description.data() + page * 8;
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 8, footer, 4);
}

// 原函数：0x004B6C50。
static void WindowShop_HandleSaleDrag(
    session::ClientSession& session,
    const std::int32_t message, const std::int32_t unused, const std::int32_t sourceIndex)
{
    auto& shop = session.UiFrame().native.shop;
    static_cast<void>(unused);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 || message == 4) {
            WindowInstance* const target = WindowRuntime_States(session)[WindowId_Shop].window;
            WindowShop_RemoveSaleSelection(session, target);
        }
        return;
    }
    std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[sourceIndex]);
    if (shop.items[item].used == 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
        static_cast<void>(WindowDrag_StartAtPosition(session, 9, sourceIndex, x, y, &WindowShop_HandleSaleDrag));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        WindowDrag_Cancel(session);
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    item = std::bit_cast<std::int8_t>(shop.selectedSlots[sourceIndex]);
    std::int32_t imageNo;
    std::memcpy(&imageNo, &shop.items[item].imageNo, sizeof(imageNo));
    const std::int32_t y =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

// 原函数：0x004B6DA0。
static std::int32_t WindowShop_StartSaleDrag(session::ClientSession& session, const std::int32_t sourceIndex)
{
    return WindowDrag_Start(session, 9, sourceIndex, &WindowShop_HandleSaleDrag);
}

// 原函数：0x004BC150。
static void WindowShop_RemoveSaleSelection(session::ClientSession& session, WindowInstance* const currentWindow)
{
    auto& shop = session.UiFrame().native.shop;
    if (!currentWindow) return;
    if (shop.dragFlag != 0) {
        shop.dragFlag = 0;
        return;
    }
    std::int32_t source = WindowDrag_ReadSourceIndex(session);
    std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[source]);
    shop.items[item].selected = std::int32_t{0};
    source = WindowDrag_ReadSourceIndex(session);
    item = std::bit_cast<std::int8_t>(shop.selectedSlots[source]);
    const auto selectedMask = static_cast<std::uint32_t>(shop.selectionMask);
    std::int32_t inventoryIndex;
    std::memcpy(&inventoryIndex, &shop.items[item].inventoryIndex, sizeof(inventoryIndex));
    shop.selectionMask = std::bit_cast<std::int32_t>(selectedMask & ~(1u << (inventoryIndex - 8)));
    source = WindowDrag_ReadSourceIndex(session);
    std::int32_t position = shop.scroll;
    shop.selectedSlots[source] = static_cast<char>(-1);
    const std::int32_t maximum = shop.itemCount - 5;
    if (position > maximum) {
        position = maximum;
        shop.scroll = position;
    }
    if (position < 0) {
        position = 0;
        shop.scroll = 0;
    }
    auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
    }
    WindowShop_RefreshSellRows(*currentWindow);
    WindowShop_RefreshSellQuantities(*currentWindow);
    WindowShop_RefreshSellAmount(*currentWindow);
    WindowShop_CountSellItems(*currentWindow);
    shop.message = -1;
}

// 原函数：0x004B6DC0。
static void WindowShop_Release(WindowInstance& releasedWindow)
{
    auto& inventory = releasedWindow.ownerSession->UiFrame().native.inventory;
    auto& npcDialog = releasedWindow.ownerSession->WorldScene().npcDialog;
    auto& shop = releasedWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = releasedWindow.ownerSession->UiFrame().native.material;
    if (shop.dialogType == npcDialog.type) {
        npcDialog.type = -1;
    }
    if (std::bit_cast<std::int8_t>(materialState.shopMode) > 0) {
        auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
        for (std::int32_t slot = 0; slot < 28; ++slot) {
            --occupancy[slot];
        }
    }
}

// 原函数：0x004B6E00。
cg::yobi::YobiAction* WindowShop_OpenBuy(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& shop = session.UiFrame().native.shop;
    auto& materialState = session.UiFrame().native.material;
    std::array<char, 2304> buffer;
    materialState.shopMode = 0;
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            std::wstring& line = npcLines[1 + group * 4 + row].text;
            static_cast<void>(input::ExtractGbkTextLine(line, buffer.data(), row, 20));
        }
    }
    shop.itemCount = 0;
    shop.items.fill({});
    std::int32_t field = 6;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        auto* const record = &shop.items[index];
        record->used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        record->name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char line[3072]{};
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
            record->description[row] = cg::text::FromGbkWide(line);
        }
        record->allowed = static_cast<char>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        record->maximum = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        ++shop.itemCount;
    }
    const std::int32_t dialogType = npcDialog.type;
    shop.currencyItemId = -1;
    shop.dialogType = dialogType;
    return WindowRuntime_CreateAnimation(session, WindowId_Shop, 1, 0, nullptr);
}

// 原函数：0x004B70D0。
cg::yobi::YobiAction* WindowShop_OpenSell(session::ClientSession& session, const char* const text)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& shop = session.UiFrame().native.shop;
    auto& materialState = session.UiFrame().native.material;
    std::array<char, 2304> buffer;
    materialState.shopMode = 1;
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 767, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 4; ++row) {
        std::wstring& line = npcLines[row + 1].text;
        static_cast<void>(input::ExtractGbkTextLine(line, buffer.data(), row, 20));
    }
    npcLines[5].text = L"　无法再持有";
    npcLines[6].text = L"　更高的金额了";
    npcLines[7].text = L"";
    shop.replyItemCount = 0;
    npcLines[8].text = L"";
    shop.items.fill({});
    std::int32_t field = 4;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        auto* const record = &shop.items[index];
        record->used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        record->name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        record->count = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->inventoryIndex = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->flags = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) != 0) {
            record->memo = cg::text::FromGbkWide(buffer.data());
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 20; ++row) {
            char line[3072]{};
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
            record->description[row] = cg::text::FromGbkWide(line);
        }
        const auto value = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        record->allowed = std::bit_cast<char>(value);
        record->maximum = static_cast<std::int32_t>(std::bit_cast<std::int8_t>(value));
        record->valueC38 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        ++shop.replyItemCount;
    }
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::int32_t index = 0;
        for (; index < 20; ++index) {
            const auto* const record = &shop.items[index];
            if (record->used != 0) {
                std::int32_t inventoryIndex;
                std::memcpy(&inventoryIndex, &record->inventoryIndex, sizeof(inventoryIndex));
                if (inventoryIndex == slot + 8) {
                    break;
                }
            }
        }
        shop.inventoryMap[slot] = -1;
        if (index < 20) {
            shop.inventoryMap[slot] = static_cast<std::int8_t>(index);
        }
    }
    shop.selectionMask = 0;
    shop.selectedSlots.fill(static_cast<char>(-1));
    shop.itemCount = 20;
    shop.currencyItemId = -1;
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    shop.dialogType = npcDialog.type;
    return WindowRuntime_CreateAnimation(session, WindowId_Shop, 1, 0, nullptr);
}

// 原函数：0x004B74E0。
cg::yobi::YobiAction* WindowShop_OpenExchange(session::ClientSession& session, const char* const text)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& shop = session.UiFrame().native.shop;
    auto& materialState = session.UiFrame().native.material;
    std::array<char, 2304> buffer;
    materialState.shopMode = 2;
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            std::wstring& line = npcLines[1 + group * 4 + row].text;
            static_cast<void>(input::ExtractGbkTextLine(line, buffer.data(), row, 20));
        }
    }
    shop.currencyItemId = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 7, 28, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    shop.exchangeText = cg::text::FromGbkWide(buffer.data());
    shop.items.fill({});
    shop.itemCount = 0;
    std::int32_t field = 8;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        auto* const record = &shop.items[index];
        record->used = 1;
        record->allowed = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        record->name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t maximum = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->maximum = maximum;
        if (maximum <= 0) {
            record->maximum = std::int32_t{1};
        }
        record->count = std::int32_t{0};
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char line[3072]{};
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
            record->description[row] = cg::text::FromGbkWide(line);
        }
        ++shop.itemCount;
    }
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    shop.dialogType = npcDialog.type;
    return WindowRuntime_CreateAnimation(session, WindowId_Shop, 1, 0, nullptr);
}

// 原函数：0x004B77E0。
cg::yobi::YobiAction* WindowShop_OpenItemDetails(session::ClientSession& session, const char* const text)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& shop = session.UiFrame().native.shop;
    auto& materialState = session.UiFrame().native.material;
    std::array<char, 2304> buffer;
    materialState.shopMode = 3;
    shop.detail = {};
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    shop.detailMode = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    for (std::int32_t group = 0; group < 4; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 4, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            std::wstring& line = npcLines[1 + group * 4 + row].text;
            static_cast<void>(input::ExtractGbkTextLine(line, buffer.data(), row, 20));
        }
    }
    auto* const detail = &shop.detail;
    detail->price = network::ParseDelimitedInteger_52CFA0(text, '|', 8);
    detail->itemId = network::ParseDelimitedInteger_52CFA0(text, '|', 9);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 10, 768, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    detail->name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 11, 767, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 32; ++row) {
        char line[3072]{};
        if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
            line[0] = '\0';
        }
        detail->description[row] = cg::text::FromGbkWide(line);
    }
    detail->quantity = network::ParseDelimitedInteger_52CFA0(text, '|', 12);
    detail->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 13);
    detail->capacity = network::ParseDelimitedInteger_52CFA0(text, '|', 14);
    detail->valueC34 = network::ParseDelimitedInteger_52CFA0(text, '|', 15);
    std::int32_t value;
    std::memcpy(&value, &detail->itemId, sizeof(value));
    shop.detailSelected = 0;
    shop.currencyItemId = value;
    shop.valueD672D4 = 0;
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    shop.dialogType = npcDialog.type;
    return WindowRuntime_CreateAnimation(session, WindowId_Shop, 1, 0, nullptr);
}

// 4B7AB0/4B7EE0同一七字段流程；只区分模式字节和第二行原字节提示。
static cg::yobi::YobiAction* WindowShop_CreateService(
    session::ClientSession& session,
    const char* const text, const char mode, const std::wstring_view prompt)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    auto& shop = session.UiFrame().native.shop;
    auto& materialState = session.UiFrame().native.material;
    std::array<char, 2304> buffer;
    materialState.shopMode = mode;
    shop.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    npcLines[1].text = L"欢迎光临。";
    npcLines[2].text = prompt;
    npcLines[3].text = L"";
    npcLines[4].text = L"";
    npcLines[5].text = L"你身上的钱不够！";
    npcLines[6].text = L"";
    npcLines[7].text = L"";
    shop.replyItemCount = 0;
    npcLines[8].text = L"";
    shop.items.fill({});
    std::int32_t field = 3;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        auto* const record = &shop.items[index];
        record->used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        record->name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        record->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->inventoryIndex = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->flags = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record->maximum = std::int32_t{1};
        record->allowed = 1;
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) != 0) {
            record->memo = cg::text::FromGbkWide(buffer.data());
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char line[3072]{};
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
            record->description[row] = cg::text::FromGbkWide(line);
        }
        ++shop.replyItemCount;
    }
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::int32_t index = 0;
        for (; index < 20; ++index) {
            const auto* const record = &shop.items[index];
            if (record->used != 0) {
                std::int32_t inventoryIndex;
                std::memcpy(&inventoryIndex, &record->inventoryIndex, sizeof(inventoryIndex));
                if (inventoryIndex == slot + 8) {
                    break;
                }
            }
        }
        const std::uint16_t used = world::gPlayerRuntimeSlots.items[slot + 8].used;
        shop.inventoryMap[slot] = -1;
        if (used != 0) {
            shop.inventoryMap[slot] = 20;
        }
        if (index < 20) {
            shop.inventoryMap[slot] = static_cast<std::int8_t>(index);
        }
    }
    std::int32_t value;
    std::memcpy(&value, &shop.detail.itemId, sizeof(value));
    shop.selectedSlots.fill(static_cast<char>(-1));
    shop.selectionMask = 0;
    shop.itemCount = 20;
    shop.currencyItemId = value;
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    shop.dialogType = npcDialog.type;
    return WindowRuntime_CreateAnimation(session, WindowId_Shop, 1, 0, nullptr);
}

// 原函数：0x004B7AB0。
cg::yobi::YobiAction* WindowShop_OpenIdentify(session::ClientSession& session, const char* const text)
{
    return WindowShop_CreateService(
        session,
        text, 4, L"你想鉴定什么物品呢？");
}

// 原函数：0x004B7EE0。
cg::yobi::YobiAction* WindowShop_OpenRepair(session::ClientSession& session, const char* const text)
{
    return WindowShop_CreateService(
        session,
        text, 5, L"你想修理什么物品呢？");
}

// 原函数：0x004B8310。
static void WindowShop_RefreshBuyRows(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& priceText = shop.priceTexts[row];
        priceText.clear();
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 17));
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
        auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement2) {
            indexedElement2->data.text->text.clear();
        }
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement3) indexedElement3->visible = 0;
        const auto* record = &shop.items[shop.scroll + row];
        if (record->used == 0) {
            continue;
        }
        std::int32_t price;
        std::memcpy(&price, &record->price, sizeof(price));
        cg::text::FormatTo(priceText, L"{:10}", price);
        if (row == 4) {
            shop.priceTail = static_cast<std::uint16_t>(priceText[8])
                | static_cast<std::uint16_t>(priceText[9] << 8);
        }
        record = &shop.items[shop.scroll + row];
        std::uint32_t flags;
        std::memcpy(&flags, &record->flags, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            buffer = record->memo;
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
            if (indexedElement4) {
                indexedElement4->data.text->style.color =
                    std::byte{5};
            }
            auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
            if (indexedElement5) {
                indexedElement5->data.text->style.color =
                    std::byte{5};
            }
        } else {
            std::int32_t count;
            std::memcpy(&count, &record->count, sizeof(count));
            if (count <= 1) {
                buffer = record->name;
            } else {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({})", record->name, count));
            }
            record = &shop.items[shop.scroll + row];
            std::memcpy(&flags, &record->flags, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17)); indexedElement4) indexedElement4->data.text->style.color = color;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22)); indexedElement5) indexedElement5->data.text->style.color = color;
        }
        auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
        auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement4 && indexedElement5) {
            WindowRuntime_FillTwoTextLines(indexedElement4->data.text, indexedElement5->data.text, buffer);
        }
        record = &shop.items[shop.scroll + row];
        if (record->allowed != 0) {
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32)); indexedElement6) indexedElement6->data.number->color = 0;
        } else {
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32)); indexedElement6) indexedElement6->data.number->color = 4;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement7) indexedElement7->visible = 1;
            if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement8) indexedElement8->data.text->text = kShopUnavailableHint586BF4;
            auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(row + 27));
            if (indexedElement9) {
                indexedElement9->data.text->style.color =
                    std::byte{4};
            }
        }
    }
}

// 原函数：0x004B8670。
static void WindowShop_RefreshBuyQuantities(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t scroll = shop.scroll;
        std::wstring& text = shop.countTexts[row];
        text.clear();
        const auto* const record = &shop.items[scroll + row];
        if (record->used != 0) {
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            cg::text::FormatTo(text, L"{:2}", selected);
        }
    }
}

// 原函数：0x004B86C0。
static void WindowShop_RefreshBuyAmount(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::int32_t total = 0;
    shop.total = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto* const record = &shop.items[index];
        if (record->used != 0) {
            std::int32_t selected;
            std::int32_t price;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::memcpy(&price, &record->price, sizeof(price));
            total += price * selected;
        }
    }
    shop.total = total;
    cg::text::FormatTo(shop.totalText, L"{:10}", total);
    const std::int32_t currentTotal = shop.total;
    const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
    auto* const element8 = currentWindow->FindElementById(WindowShopElementId::Element8);
    if (element8) {
        element8->data.number->color =
            currentTotal <= money ? 0 : 6;
    }
    return;
}

// 原函数：0x004B8730。
static void WindowShop_CountBuySlots(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    std::int32_t total = 0;
    shop.quantity = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto* const record = &shop.items[index];
        if (record->used != 0) {
            std::int32_t divisor;
            std::int32_t selected;
            std::memcpy(&divisor, &record->maximum, sizeof(divisor));
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (divisor <= 0) {
                divisor = 2;
            }
            total += (divisor + selected - 1) / divisor;
        }
    }
    shop.quantity = total;
}

// 原函数：0x004B8780。
static void WindowShop_RefreshSellRows(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& priceText = shop.priceTexts[row];
        priceText.clear();
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 17));
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
        auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement2) {
            indexedElement2->data.text->text.clear();
        }
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement3) indexedElement3->visible = 0;
        std::int32_t index = std::bit_cast<std::int8_t>(
            shop.selectedSlots[shop.scroll + row]);
        if (index < 0) {
            continue;
        }
        const auto* record = &shop.items[index];
        std::int32_t price;
        std::memcpy(&price, &record->price, sizeof(price));
        cg::text::FormatTo(priceText, L"{:10}", price);
        if (row == 4) {
            shop.priceTail = static_cast<std::uint16_t>(priceText[8])
                | static_cast<std::uint16_t>(priceText[9] << 8);
        }
        index = std::bit_cast<std::int8_t>(shop.selectedSlots[shop.scroll + row]);
        record = &shop.items[index];
        std::uint32_t flags;
        std::memcpy(&flags, &record->flags, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            buffer = record->memo;
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
            if (indexedElement4) {
                indexedElement4->data.text->style.color =
                    std::byte{5};
            }
            auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
            if (indexedElement5) {
                indexedElement5->data.text->style.color =
                    std::byte{5};
            }
        } else {
            std::int32_t count;
            std::memcpy(&count, &record->count, sizeof(count));
            if (count <= 1) {
                buffer = record->name;
            } else {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({})", record->name, count));
            }
            index = std::bit_cast<std::int8_t>(shop.selectedSlots[shop.scroll + row]);
            record = &shop.items[index];
            std::memcpy(&flags, &record->flags, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17)); indexedElement4) indexedElement4->data.text->style.color = color;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22)); indexedElement5) indexedElement5->data.text->style.color = color;
        }
        auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
        auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement4 && indexedElement5) {
            WindowRuntime_FillTwoTextLines(indexedElement4->data.text, indexedElement5->data.text, buffer);
        }
        index = std::bit_cast<std::int8_t>(shop.selectedSlots[shop.scroll + row]);
        record = &shop.items[index];
        if (record->allowed != 0) {
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32));
            if (indexedElement6) {
                indexedElement6->data.number->color =
                    selected != 0 ? 0 : 7;
            }
        } else {
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32)); indexedElement6) indexedElement6->data.number->color = 6;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement7) indexedElement7->visible = 1;
            if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement8) indexedElement8->data.text->text = L"＊无法贩卖";
            auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(row + 27));
            if (indexedElement9) {
                indexedElement9->data.text->style.color =
                    std::byte{6};
            }
        }
    }
}

// 原函数：0x004B8B10。
static void WindowShop_RefreshSellQuantities(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& text = shop.countTexts[row];
        text.clear();
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 42)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 47)); indexedElement2) indexedElement2->visible = 0;
        std::int32_t index = std::bit_cast<std::int8_t>(
            shop.selectedSlots[shop.scroll + row]);
        if (index >= 0) {
            const auto* record = &shop.items[index];
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            cg::text::FormatTo(text, L"{:2}", selected);
            index = std::bit_cast<std::int8_t>(shop.selectedSlots[shop.scroll + row]);
            record = &shop.items[index];
            std::int32_t maximum;
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            if (maximum > 1) {
                if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 42)); indexedElement3) indexedElement3->visible = 1;
                if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 47)); indexedElement4) indexedElement4->visible = 1;
            }
        }
    }
}

// 原函数：0x004B8BE0。
static void WindowShop_RefreshSellAmount(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::int32_t total = 0;
    shop.total = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const std::int32_t index = std::bit_cast<std::int8_t>(shop.selectedSlots[slot]);
        if (index >= 0) {
            const auto* const record = &shop.items[index];
            std::int32_t price;
            std::int32_t selected;
            std::memcpy(&price, &record->price, sizeof(price));
            std::memcpy(&selected, &record->selected, sizeof(selected));
            total += price * selected;
        }
    }
    shop.total = total;
    cg::text::FormatTo(shop.totalText, L"{:10}", total);
    if (auto* const element8 = currentWindow->FindElementById(WindowShopElementId::Element8); element8) element8->data.number->color = 0;
}

// 原函数：0x004B8C50。
static void WindowShop_CountSellItems(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    std::int32_t total = 0;
    shop.quantity = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const std::int32_t index = std::bit_cast<std::int8_t>(shop.selectedSlots[slot]);
        if (index >= 0) {
            const auto* const record = &shop.items[index];
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            total += selected;
        }
    }
    shop.quantity = total;
}

// 原函数：0x004B8C90。
static void WindowShop_RefreshExchangeRows(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& priceText = shop.priceTexts[row];
        priceText.clear();
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 17));
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
        auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement2) {
            indexedElement2->data.text->text.clear();
        }
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement3) indexedElement3->visible = 0;
        const auto* record = &shop.items[shop.scroll + row];
        if (record->used == 0) {
            continue;
        }
        std::int32_t price;
        std::memcpy(&price, &record->price, sizeof(price));
        cg::text::FormatTo(priceText, L"{:10}", price);
        if (row == 4) {
            shop.priceTail = static_cast<std::uint16_t>(priceText[8])
                | static_cast<std::uint16_t>(priceText[9] << 8);
        }
        record = &shop.items[shop.scroll + row];
        std::uint32_t flags;
        std::memcpy(&flags, &record->flags, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            buffer = record->memo;
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
            if (indexedElement4) {
                indexedElement4->data.text->style.color =
                    std::byte{5};
            }
            auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
            if (indexedElement5) {
                indexedElement5->data.text->style.color =
                    std::byte{5};
            }
        } else {
            std::int32_t count;
            std::memcpy(&count, &record->count, sizeof(count));
            if (count <= 1) {
                buffer = record->name;
            } else {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({})", record->name, count));
            }
            record = &shop.items[shop.scroll + row];
            std::memcpy(&flags, &record->flags, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17)); indexedElement4) indexedElement4->data.text->style.color = color;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22)); indexedElement5) indexedElement5->data.text->style.color = color;
        }
        auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 17));
        auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement4 && indexedElement5) {
            WindowRuntime_FillTwoTextLines(indexedElement4->data.text, indexedElement5->data.text, buffer);
        }
        record = &shop.items[shop.scroll + row];
        const std::int32_t available = shop.available;
        std::memcpy(&price, &record->price, sizeof(price));
        const std::int32_t total = shop.total;
        if (available >= price + total) {
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32)); indexedElement6) indexedElement6->data.number->color = 0;
        } else {
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 32)); indexedElement6) indexedElement6->data.number->color = 7;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement7) indexedElement7->visible = 1;
            if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(row + 27)); indexedElement8) indexedElement8->data.text->text = L"＊你的数量不够加工";
            auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(row + 27));
            if (indexedElement9) {
                indexedElement9->data.text->style.color =
                    std::byte{4};
            }
        }
    }
}

// 原函数：0x004B8FF0。
static void WindowShop_RefreshExchangeQuantities(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& text = shop.countTexts[row];
        text.clear();
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 42)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 47)); indexedElement2) indexedElement2->visible = 0;
        const auto* const record = &shop.items[shop.scroll + row];
        if (record->used != 0) {
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            cg::text::FormatTo(text, L"{:2}", selected);
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 42)); indexedElement3) indexedElement3->visible = 1;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 47)); indexedElement4) indexedElement4->visible = 1;
        }
    }
}

// 原函数：0x004B9090。
static void WindowShop_RefreshExchangeAmount(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    std::int32_t total = 0;
    shop.total = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto* const record = &shop.items[index];
        if (record->used != 0) {
            std::int32_t selected;
            std::int32_t price;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::memcpy(&price, &record->price, sizeof(price));
            total += price * selected;
        }
    }
    shop.total = total;
    cg::text::FormatTo(shop.valueText, L"{:10}", total);
}

// 原函数：0x004B90E0。
static void WindowShop_CountExchangeItems(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    std::int32_t total = 0;
    shop.quantity = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const auto* const record = &shop.items[index];
        if (record->used != 0) {
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            total += selected;
        }
    }
    shop.quantity = total;
}

// 原函数：0x004B9110。
static void WindowShop_RefreshDetailName(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    std::int32_t price;
    std::memcpy(&price, &shop.detail.valueC34, sizeof(price));
    cg::text::FormatTo(shop.priceTexts[0], L"{:10}", price);
    auto* const element18 = currentWindow->FindElementById(WindowShopElementId::Element18);
    auto* const element23 = currentWindow->FindElementById(WindowShopElementId::Element23);
    if (element18 && element23) {
        WindowRuntime_FillTwoTextLines(element18->data.text,
            element23->data.text, shop.detail.name);
    }
    return;
}

// 原函数：0x004B9270。
static void WindowShop_RefreshDetailAmount(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    cg::text::FormatTo(shop.countTexts[0], L"{:2}", shop.detailSelected);
    cg::text::FormatTo(shop.valueText, L"{:10}", shop.detailSelected);
    std::int32_t price;
    std::memcpy(&price, &shop.detail.price, sizeof(price));
    shop.total = price * shop.detailSelected;
}

// 原函数：0x004B92C0。
static void WindowShop_CountDetailItems(WindowInstance& callbackWindow)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    const std::int32_t mode = shop.detailMode;
    shop.quantity = 0;
    if (mode == 0) {
        std::memcpy(&shop.quantity, &shop.detail.quantity, sizeof(shop.quantity));
        return;
    }
    std::int32_t itemId;
    std::memcpy(&itemId, &shop.detail.itemId, sizeof(itemId));
    std::int32_t total = 0;
    for (const world::PlayerItemRecord& record : world::gPlayerRuntimeSlots.items) {
        if (record.used != 0 && record.itemId == itemId) {
            total += record.count;
        }
    }
    shop.quantity = total;
}

// 原函数：0x004B9310。
static std::int32_t WindowShop_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        // 原型只保留数字样式；实例在首次消费前借所属会话的稳定文本。
        if (auto* const element = currentWindow->FindElementById(WindowShopElementId::Element7); element && element->data.number) {
            element->data.number->text = &shop.valueText;
        }
        if (auto* const element = currentWindow->FindElementById(WindowShopElementId::Element8); element && element->data.number) {
            element->data.number->text = &shop.totalText;
        }
        if (auto* const element = currentWindow->FindElementById(WindowShopElementId::Element9); element && element->data.number) {
            element->data.number->text = &shop.moneyText;
        }
        for (std::int32_t row = 0; row < 5; ++row) {
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(32 + row)); element && element->data.number) {
                element->data.number->text = &shop.priceTexts[row];
            }
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(37 + row)); element && element->data.number) {
                element->data.number->text = &shop.countTexts[row];
            }
        }
        shop.scroll = 0;
        shop.available = -1;
        if (auto* const handleConfirmationElement = currentWindow->FindElementById(WindowShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 0;
        for (std::int32_t row = 0; row < 5; ++row) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(17 + row)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(22 + row)); indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(27 + row)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(32 + row)); indexedElement4) indexedElement4->visible = 1;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(37 + row)); indexedElement5) indexedElement5->visible = 1;
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(42 + row)); indexedElement6) indexedElement6->visible = 1;
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(47 + row)); indexedElement7) indexedElement7->visible = 1;
            if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement8) indexedElement8->visible = 1;
            if (auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(57 + row)); indexedElement9) indexedElement9->visible = 1;
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            WindowShop_RefreshBuyAmount(callbackWindow);
            WindowShop_CountBuySlots(callbackWindow);
            if (auto* const handleSaleDropElement = currentWindow->FindElementById(WindowShopElementId::HandleSaleDrop); handleSaleDropElement) handleSaleDropElement->data.graphic->imageNo = 244381;
            for (std::int32_t row = 0; row < 5; ++row) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement) indexedElement->visible = 1;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement2) indexedElement2->mouseCallback = &WindowShop_HandleItemIcon;
                auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(57 + row));
                if (indexedElement3) {
                    indexedElement3->data.graphic->imageNo =
                        244382;
                }
            }
            if (auto* const element7 = currentWindow->FindElementById(WindowShopElementId::Element7); element7) element7->visible = 0;
            if (auto* const buttonAddAllElement = currentWindow->FindElementById(WindowShopElementId::ButtonAddAll); buttonAddAllElement) buttonAddAllElement->visible = 0;
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            WindowShop_RefreshSellAmount(callbackWindow);
            WindowShop_CountSellItems(callbackWindow);
            if (auto* const handleSaleDropElement2 = currentWindow->FindElementById(WindowShopElementId::HandleSaleDrop); handleSaleDropElement2) handleSaleDropElement2->data.graphic->imageNo = 244381;
            for (std::int32_t row = 0; row < 5; ++row) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement) indexedElement->visible = 1;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement2) indexedElement2->mouseCallback = &WindowShop_HandleSaleSelection;
                auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(57 + row));
                if (indexedElement3) {
                    indexedElement3->data.graphic->imageNo =
                        244382;
                }
            }
            if (auto* const element72 = currentWindow->FindElementById(WindowShopElementId::Element7); element72) element72->visible = 0;
            if (auto* const buttonAddAllElement2 = currentWindow->FindElementById(WindowShopElementId::ButtonAddAll); buttonAddAllElement2) buttonAddAllElement2->visible = 1;
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            WindowShop_RefreshExchangeAmount(callbackWindow);
            WindowShop_CountExchangeItems(callbackWindow);
            if (auto* const handleSaleDropElement3 = currentWindow->FindElementById(WindowShopElementId::HandleSaleDrop); handleSaleDropElement3) handleSaleDropElement3->data.graphic->imageNo = 244388;
            for (std::int32_t row = 0; row < 5; ++row) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement) indexedElement->visible = 1;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement2) indexedElement2->mouseCallback = &WindowShop_HandleItemIcon;
                auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(57 + row));
                if (indexedElement3) {
                    indexedElement3->data.graphic->imageNo =
                        244389;
                }
            }
            if (auto* const element73 = currentWindow->FindElementById(WindowShopElementId::Element7); element73) element73->visible = 1;
            if (auto* const buttonAddAllElement3 = currentWindow->FindElementById(WindowShopElementId::ButtonAddAll); buttonAddAllElement3) buttonAddAllElement3->visible = 0;
            break;
        case 3:
            WindowShop_RefreshDetailName(callbackWindow);
            WindowShop_RefreshDetailAmount(callbackWindow);
            for (std::int32_t row = 1; row < 5; ++row) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(17 + row)); indexedElement) indexedElement->visible = 0;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(22 + row)); indexedElement2) indexedElement2->visible = 0;
                if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(27 + row)); indexedElement3) indexedElement3->visible = 0;
                if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(32 + row)); indexedElement4) indexedElement4->visible = 0;
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(37 + row)); indexedElement5) indexedElement5->visible = 0;
                if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(42 + row)); indexedElement6) indexedElement6->visible = 0;
                if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(47 + row)); indexedElement7) indexedElement7->visible = 0;
                if (auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(52 + row)); indexedElement8) indexedElement8->visible = 0;
                if (auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(57 + row)); indexedElement9) indexedElement9->visible = 0;
            }
            if (auto* const handleSaleDropElement4 = currentWindow->FindElementById(WindowShopElementId::HandleSaleDrop); handleSaleDropElement4) handleSaleDropElement4->data.graphic->imageNo = 244388;
            if (auto* const element18 = currentWindow->FindElementById(WindowShopElementId::Element18); element18) element18->visible = 1;
            if (auto* const element23 = currentWindow->FindElementById(WindowShopElementId::Element23); element23) element23->visible = 1;
            if (auto* const element28 = currentWindow->FindElementById(WindowShopElementId::Element28); element28) element28->visible = 0;
            if (auto* const element33 = currentWindow->FindElementById(WindowShopElementId::Element33); element33) element33->visible = 1;
            if (auto* const element38 = currentWindow->FindElementById(WindowShopElementId::Element38); element38) element38->visible = 1;
            if (auto* const buttonDecreaseElement = currentWindow->FindElementById(WindowShopElementId::ButtonDecrease); buttonDecreaseElement) buttonDecreaseElement->visible = 1;
            if (auto* const buttonIncreaseElement = currentWindow->FindElementById(WindowShopElementId::ButtonIncrease); buttonIncreaseElement) buttonIncreaseElement->visible = 1;
            if (auto* const handleItemIconElement = currentWindow->FindElementById(WindowShopElementId::HandleItemIcon); handleItemIconElement) handleItemIconElement->visible = 1;
            if (auto* const element58 = currentWindow->FindElementById(WindowShopElementId::Element58); element58) element58->visible = 1;
            if (auto* const handleItemIconElement2 = currentWindow->FindElementById(WindowShopElementId::HandleItemIcon); handleItemIconElement2) handleItemIconElement2->mouseCallback = &WindowShop_HandleDetailIcon;
            if (auto* const element582 = currentWindow->FindElementById(WindowShopElementId::Element58); element582) element582->data.graphic->imageNo = 244389;
            if (auto* const element74 = currentWindow->FindElementById(WindowShopElementId::Element7); element74) element74->visible = 1;
            if (auto* const buttonAddAllElement4 = currentWindow->FindElementById(WindowShopElementId::ButtonAddAll); buttonAddAllElement4) buttonAddAllElement4->visible = 0;
            break;
        default:
            break;
        }
        shop.rowKeys[0] = -2;
        shop.tooltipIndex = -1;
        shop.rowKeys[1] = -2;
        shop.message = -1;
        shop.rowKeys[2] = -2;
        shop.tooltipPage = 0;
        shop.rowKeys[3] = -2;
        shop.rowKeys[4] = -2;
        auto* const element6 = currentWindow->FindElementById(WindowShopElementId::Element6);
        if (element6) {
            auto* const title = element6->data.text;
            title->text = npcLines[0].text;
        }
        auto* const element10 = currentWindow->FindElementById(WindowShopElementId::Element10);
        if (element10) {
            auto* animation = element10->data.animation;
            cg::yobi::SetYobiActionField2A8(animation->action, shop.imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    if (shop.dialogType != npcDialog.type) {
        currentWindow->state |= 0x8000u;
    }
    if (auto* const element11 = currentWindow->FindElementById(WindowShopElementId::Element11); element11) element11->visible = 1;
    if (shop.itemCount - 5 <= 0) {
        if (auto* const element112 = currentWindow->FindElementById(WindowShopElementId::Element11); element112) element112->visible = 0;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowShopElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement && (handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
        auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowShopElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement2) {
            shop.scroll = WindowScroll_ReadPosition(handleVerticalScrollElement2, shop.itemCount - 5);
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
    }
    cg::text::FormatTo(shop.moneyText, L"{:10}", world::gPlayerRuntimeSlots.valueFC);
    if (materialState.shopMode == 0) {
        const std::int32_t total = shop.total;
        const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
        auto* const element8 = currentWindow->FindElementById(WindowShopElementId::Element8);
        if (element8) {
            element8->data.number->color =
                total <= money ? 0 : 6;
        }
    }
    if (materialState.shopMode == 2) {
        const std::int32_t previous = shop.available;
        const std::int32_t itemId = shop.currencyItemId;
        shop.available = 0;
        std::int32_t total = 0;
        for (const world::PlayerItemRecord& record : world::gPlayerRuntimeSlots.items) {
            if (record.used != 0 && record.itemId == itemId) {
                total += record.count;
            }
        }
        shop.available = total;
        if (total != previous) {
            WindowShop_RefreshExchangeRows(callbackWindow);
            cg::text::FormatTo(shop.totalText, L"{:10}", shop.available);
        }
    }
    if (shop.message >= 0) {
        shop.message = -1;
    }
    shop.tooltipIndex = -1;
    if (materialState.shopMode == 3) {
        WindowShop_CountDetailItems(callbackWindow);
        std::int32_t amount;
        if (shop.detailMode != 0) {
            amount = shop.quantity;
        } else {
            std::memcpy(&amount, &shop.detail.quantity, sizeof(amount));
        }
        cg::text::FormatTo(shop.totalText, L"{:10}", amount);
    }
    return 1;
}

// 原函数：0x004B9B10。
static void WindowShop_Submit(WindowInstance& callbackWindow, const std::int32_t selection)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return;
    if (static_cast<std::uint8_t>(selection) != 0) {
        return;
    }
    const auto mode = std::bit_cast<std::int8_t>(materialState.shopMode);
    if (mode < 0 || mode > 5) {
        return;
    }
    std::wstring fragment;
    std::wstring text;
    std::wstring escaped;
    if (mode == 3) {
        cg::text::FormatTo(text, L"1|{}", shop.detailSelected);
    } else {
        text.clear();
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const auto* const record = &shop.items[index];
            if (record->used == 0) {
                continue;
            }
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (selected == 0) {
                continue;
            }
            if (count > 0) {
                text += L"|";
            }
            if (mode == 4 || mode == 5) {
                std::int32_t inventoryIndex;
                std::memcpy(&inventoryIndex, &record->inventoryIndex, sizeof(inventoryIndex));
                cg::text::FormatTo(fragment, L"{}", inventoryIndex);
            } else {
                std::int32_t itemIndex = index;
                if (mode == 1) {
                    std::memcpy(&itemIndex, &record->inventoryIndex, sizeof(itemIndex));
                }
                cg::text::FormatTo(fragment, L"{}|{}", itemIndex, selected);
            }
            text += fragment;
            ++count;
        }
    }
    escaped = input::EscapeTextAndNotify(text, mode == 3 ? 256 : 255);
    const std::int32_t target = npcDialog.target;
    const std::int32_t request = npcDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
    currentWindow->state |= 0x8000u;
}

// 原函数：0x004B9F40。
static std::int32_t WindowShop_ButtonOk(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        const auto mode = std::bit_cast<std::int8_t>(materialState.shopMode);
        if (mode >= 0 && mode <= 5) {
            uiFrame.hoverText = kShopConfirmHoverHints[mode];
        }
    }
    const std::int32_t available = shop.quantity;
    if (available <= 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((state & MouseFlag_LDownEvent) != 0) {
        const auto mode = std::bit_cast<std::int8_t>(materialState.shopMode);
        bool overlayReady = false;
        std::int32_t textMode = mode;
        switch (mode) {
        case 0:
        case 2: {
            if (mode == 0) {
                const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
                if (money < shop.total) {
                    shop.message = -2;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                    break;
                }
                const std::int32_t freeSlots = world::CountFreeInventorySlots_518C60();
                if (shop.quantity > freeSlots) {
                    shop.message = -3;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                    break;
                }
            }
            shop.confirmationCallback = &WindowShop_Submit;
            std::int32_t count = 0;
            for (std::int32_t item = 0; item < 20; ++item) {
                const auto* const record = &shop.items[item];
                if (record->used == 0) {
                    continue;
                }
                std::int32_t quantity;
                std::memcpy(&quantity, &record->selected, sizeof(quantity));
                if (quantity == 0) {
                    continue;
                }
                WindowConfirmationItem& preview = shop.confirmationItems[count];
                preview.visible = 1;
                preview.name = record->name;
                std::int32_t imageNo;
                std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
                preview.color = 0;
                preview.imageNo = imageNo;
                preview.quantity = quantity;
                preview.factor = 0;
                ++count;
            }
            for (; count < 20; ++count) {
                shop.confirmationItems[count].visible = 0;
            }
            WindowConfirmationAmount& left = shop.confirmationAmounts[0];
            WindowConfirmationAmount& right = shop.confirmationAmounts[1];
            if (mode == 0) {
                const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
                left.y = 343;
                right.y = 343;
                const std::int32_t total = shop.total;
                left.visible = 1;
                left.x = 293;
                left.value = money;
                right.visible = 1;
                right.x = 457;
                right.value = total;
            } else {
                const std::int32_t total = shop.total;
                const std::int32_t materialCount = shop.available;
                left.visible = 1;
                left.x = 354;
                left.y = 343;
                left.value = materialCount;
                right.visible = 1;
                right.x = 450;
                if (total < 1000) {
                    right.x = 438;
                }
                right.y = 343;
                right.value = total;
            }
            overlayReady = true;
            break;
        }
        case 1: {
            shop.confirmationCallback = &WindowShop_Submit;
            std::int32_t count = 0;
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const auto item = static_cast<std::int8_t>(shop.selectedSlots[slot]);
                if (item < 0) {
                    continue;
                }
                WindowConfirmationItem& preview = shop.confirmationItems[count];
                preview.visible = 1;
                const auto* const record = &shop.items[item];
                std::uint32_t flags;
                std::memcpy(&flags, &record->flags, sizeof(flags));
                if ((flags & 0x100u) != 0) {
                    preview.name = record->memo;
                    preview.color = 5;
                } else {
                    preview.name = record->name;
                    preview.color = (flags & 0x200u) != 0 ? 1 : 0;
                }
                std::int32_t imageNo;
                std::int32_t quantity;
                std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
                std::memcpy(&quantity, &record->selected, sizeof(quantity));
                preview.imageNo = imageNo;
                preview.quantity = quantity;
                std::int32_t itemCount;
                std::memcpy(&itemCount, &record->count, sizeof(itemCount));
                preview.factor = 0;
                if (itemCount > 1) {
                    std::memcpy(&preview.factor, &record->valueC38, sizeof(preview.factor));
                }
                ++count;
            }
            for (; count < 20; ++count) {
                shop.confirmationItems[count].visible = 0;
            }
            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
            WindowConfirmationAmount& left = shop.confirmationAmounts[0];
            WindowConfirmationAmount& right = shop.confirmationAmounts[1];
            left.y = 343;
            right.y = 343;
            const std::int32_t total = shop.total;
            left.visible = 1;
            left.x = 293;
            left.value = money;
            right.visible = 1;
            right.x = 457;
            right.value = total;
            overlayReady = true;
            break;
        }
        case 3: {
            WindowConfirmationItem& preview = shop.confirmationItems[0];
            preview.visible = 1;
            shop.confirmationCallback = &WindowShop_Submit;
            preview.name = shop.detail.name;
            std::int32_t imageNo;
            std::memcpy(&imageNo, &shop.detail.imageNo, sizeof(imageNo));
            preview.color = 0;
            preview.imageNo = imageNo;
            const std::int32_t quantity = shop.detailSelected;
            preview.factor = 0;
            preview.quantity = quantity;
            for (std::int32_t item = 1; item < 20; ++item) {
                shop.confirmationItems[item].visible = 0;
            }
            WindowConfirmationAmount& left = shop.confirmationAmounts[0];
            WindowConfirmationAmount& right = shop.confirmationAmounts[1];
            left.visible = 1;
            left.x = 293;
            left.y = 343;
            left.value = available;
            right.visible = 1;
            right.x = 457;
            right.y = 343;
            right.value = quantity;
            overlayReady = true;
            break;
        }
        case 4:
        case 5: {
            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
            if (money < shop.total) {
                shop.message = -2;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                break;
            }
            shop.confirmationCallback = &WindowShop_Submit;
            std::int32_t count = 0;
            const auto* flagsRecord = &shop.items[0];
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const auto item = static_cast<std::int8_t>(shop.selectedSlots[slot]);
                if (item < 0) {
                    continue;
                }
                WindowConfirmationItem& preview = shop.confirmationItems[count];
                preview.visible = 1;
                const auto* const record = &shop.items[item];
                preview.name = record->name;
                preview.color = 0;
                // 原flags来源按紧凑显示项推进，不跟随选择槽映射。
                std::uint32_t flags;
                std::memcpy(&flags, &flagsRecord->flags, sizeof(flags));
                if ((flags & 0x200u) != 0) {
                    preview.color = 1;
                }
                std::int32_t imageNo;
                std::int32_t quantity;
                std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
                std::memcpy(&quantity, &record->selected, sizeof(quantity));
                preview.imageNo = imageNo;
                preview.quantity = quantity;
                preview.factor = 0;
                ++flagsRecord;
                ++count;
            }
            for (; count < 20; ++count) {
                shop.confirmationItems[count].visible = 0;
            }
            const std::int32_t currentMoney = world::gPlayerRuntimeSlots.valueFC;
            WindowConfirmationAmount& left = shop.confirmationAmounts[0];
            WindowConfirmationAmount& right = shop.confirmationAmounts[1];
            left.y = 343;
            right.y = 343;
            const std::int32_t total = shop.total;
            left.visible = 1;
            right.value = total;
            textMode = materialState.shopMode == 4 ? 4 : 5;
            left.x = 293;
            left.value = currentMoney;
            right.visible = 1;
            right.x = 457;
            overlayReady = true;
            break;
        }
        default:
            break;
        }
        if (overlayReady) {
            if (mode == 2) {
                cg::text::FormatTo(shop.confirmationHeading, kShopConfirmHeadings[textMode], shop.exchangeText);
            } else {
                cg::text::FormatTo(shop.confirmationHeading, kShopConfirmHeadings[textMode]);
            }
            cg::text::FormatTo(shop.confirmationQuestion, kShopConfirmQuestions[textMode]);
            shop.confirmationAcceptHint = kShopConfirmAcceptHints[textMode];
            const wchar_t* const cancelHint = kShopHints57E6C4[2];
            if (mode == 2) {
                WindowInstance* const window = currentWindow;
                shop.confirmationCancelHint = cancelHint;
                if (auto* const handleConfirmationElement = window->FindElementById(WindowShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 1;
            } else {
                shop.confirmationCancelHint = cancelHint;
                if (auto* const handleConfirmationElement = currentWindow->FindElementById(WindowShopElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 1;
            }
            currentWindow->state |= 2u;
            static_cast<void>(audio::PlaySe_52C450(78, 320));
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
    if (shop.quantity <= 0) {
        if (graphic) graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004BA950。
static std::int32_t WindowShop_ButtonCancel(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hover) {
            const auto mode = std::bit_cast<std::int8_t>(materialState.shopMode);
            if (mode >= 0 && mode <= 5) {
                uiFrame.hoverText = kShopHints57E6C4[2];
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 2, L""));
            currentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            consumed = 1;
        }
        graphic->imageNo = 245320;
        if (hover) {
            graphic->imageNo = 245322;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 245321;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004BAA50。
static std::int32_t WindowShop_ButtonAddAll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    bool available = false;
    for (std::int32_t inventorySlot = 0; inventorySlot < 20; ++inventorySlot) {
        if ((static_cast<std::uint32_t>(shop.selectionMask) & (1u << inventorySlot)) == 0) {
            // 原可用性预扫没有实际加入时的映射范围门禁。
            const std::int32_t item = shop.inventoryMap[inventorySlot];
            std::uint8_t allowed;
            if (item == -1) {
                // 原4BAA95读取CF4FC4，与表情选择首字节同源。
                allowed = callbackWindow.ownerSession->UiFrame().native.emoteSelection[0];
            } else if (item == 20) {
                // 原D052FC对应description[30][72]；旧行最多32B，此零初值尾字节没有写入者。
                allowed = 0;
            } else {
                allowed = static_cast<std::uint8_t>(shop.items[item].allowed);
            }
            if (allowed != 0) {
                available = true;
                break;
            }
        }
    }
    const bool hover = (state & MouseFlag_Hover) != 0;
    if (hover) {
        if (!available) {
            uiFrame.hoverText = L"没有可登录的物品";
        } else {
            switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
            case 0:
            case 2:
            case 3:
                uiFrame.hoverText = L"";
                break;
            case 1:
                uiFrame.hoverText = L"登录所有可卖的物品";
                break;
            case 4:
                uiFrame.hoverText = L"登录所有可鉴定的物品";
                break;
            case 5:
                uiFrame.hoverText = L"登录所有可修理的物品";
                break;
            default:
                break;
            }
        }
    }
    if (!available) {
        if (graphic) graphic->imageNo = 243204;
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            return 1;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        for (std::int32_t inventorySlot = 0; inventorySlot < 20; ++inventorySlot) {
            const std::int32_t item = shop.inventoryMap[inventorySlot];
            if (item >= 0 && item < 20
                && (static_cast<std::uint32_t>(shop.selectionMask) & (1u << inventorySlot)) == 0) {
                auto* const record = &shop.items[item];
                if (record->allowed != 0) {
                    std::int32_t slot = 0;
                    while (slot < 20 && std::bit_cast<std::int8_t>(shop.selectedSlots[slot]) >= 0) {
                        ++slot;
                    }
                    if (slot < 20) {
                        bool exceedsLimit = false;
                        if (materialState.shopMode == 1) {
                            std::int32_t amount;
                            std::memcpy(&amount, &record->price, sizeof(amount));
                            amount += shop.total;
                            amount += world::gPlayerRuntimeSlots.valueFC;
                            exceedsLimit = amount > world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
                        }
                        if (exceedsLimit) {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                            shop.message = -2;
                        } else {
                            shop.selectedSlots[slot] = static_cast<char>(item);
                            std::int32_t maximumCount;
                            std::memcpy(&maximumCount, &record->maximum, sizeof(maximumCount));
                            record->selected = maximumCount;
                            if (maximumCount <= 0) {
                                record->selected = std::int32_t{1};
                            }
                            const auto mask = static_cast<std::uint32_t>(shop.selectionMask);
                            WindowInstance* const window = currentWindow;
                            const std::int32_t maximum = shop.itemCount - 5;
                            shop.selectionMask = std::bit_cast<std::int32_t>(mask | (1u << inventorySlot));
                            const std::int32_t position = shop.scroll;
                            auto* const handleVerticalScrollElement = window->FindElementById(WindowShopElementId::HandleVerticalScroll);
                            if (handleVerticalScrollElement) {
                                static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
                            }
                            WindowShop_RefreshSellRows(callbackWindow);
                            WindowShop_RefreshSellQuantities(callbackWindow);
                            WindowShop_RefreshSellAmount(callbackWindow);
                            WindowShop_CountSellItems(callbackWindow);
                        }
                    }
                }
            }
            static_cast<void>(WindowDrag_ResetMouse(*callbackWindow.ownerSession));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243203;
    if (hover) {
        if (graphic) graphic->imageNo = 243205;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243204;
    }
    return consumed;
}

// 原函数：0x004BCEE0。
static std::int32_t WindowShop_HandleSaleDrop(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto events = static_cast<std::uint8_t>(state);
    if ((events & MouseFlag_Hover) != 0) {
        const wchar_t* hint;
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 1:
            hint = L"把要卖的物品拖放到这里。";
            break;
        case 4:
            hint = L"把要鉴定的物品拖放到这里。";
            break;
        case 5:
            hint = L"把要修理的物品拖放到这里。";
            break;
        default:
            return 0;
        }
        uiFrame.hoverText = hint;
    }
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 2) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t inventorySlot = WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession) - 8;
        if (inventorySlot >= 0) {
            const std::int32_t item = shop.inventoryMap[inventorySlot];
            if (item >= 0 && item < 20
                && (static_cast<std::uint32_t>(shop.selectionMask) & (1u << inventorySlot)) == 0) {
                auto* const record = &shop.items[item];
                if (record->allowed != 0) {
                    std::int32_t slot = 0;
                    while (slot < 20 && std::bit_cast<std::int8_t>(shop.selectedSlots[slot]) >= 0) {
                        ++slot;
                    }
                    if (slot < 20) {
                        bool exceedsLimit = false;
                        if (materialState.shopMode == 1) {
                            std::int32_t amount;
                            std::memcpy(&amount, &record->price, sizeof(amount));
                            amount += shop.total;
                            amount += world::gPlayerRuntimeSlots.valueFC;
                            exceedsLimit = amount > world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
                        }
                        if (exceedsLimit) {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                            shop.message = -2;
                        } else {
                            shop.selectedSlots[slot] = static_cast<char>(item);
                            std::int32_t maximumCount;
                            std::memcpy(&maximumCount, &record->maximum, sizeof(maximumCount));
                            record->selected = maximumCount;
                            if (maximumCount <= 0) {
                                record->selected = std::int32_t{1};
                            }
                            const auto mask = static_cast<std::uint32_t>(shop.selectionMask);
                            const std::int32_t position = shop.scroll;
                            const std::int32_t maximum = shop.itemCount - 5;
                            shop.selectionMask = std::bit_cast<std::int32_t>(mask | (1u << inventorySlot));
                            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowShopElementId::HandleVerticalScroll);
                            if (handleVerticalScrollElement) {
                                static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
                            }
                            WindowShop_RefreshSellRows(callbackWindow);
                            WindowShop_RefreshSellQuantities(callbackWindow);
                            WindowShop_RefreshSellAmount(callbackWindow);
                            WindowShop_CountSellItems(callbackWindow);
                        }
                    }
                }
            }
        }
        WindowDrag_Reset(*callbackWindow.ownerSession);
    }
    if (WindowDrag_ReadKind(*callbackWindow.ownerSession) != 0 && (events & MouseFlag_Dragging) != 0) {
        const WindowInstance* const window = currentWindow;
        const std::int32_t frame = cg::render::gHoverFramePaletteIndex_D69768;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 3), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + element.width), static_cast<std::uint16_t>(y + element.height),
                223, frame, 0);
        }
    }
    return 0;
}

// 原函数：0x004BAD10。
static std::int32_t WindowShop_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (shop.scroll > 0) {
            --shop.scroll;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = shop.scroll;
        WindowInstance* const window = currentWindow;
        const std::int32_t maximum = shop.itemCount - 5;
        auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
        if (indexedElement) {
            static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243090;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 243092;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x004BAE10。
static std::int32_t WindowShop_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        const std::int32_t oldMaximum = shop.itemCount - 5;
        if (shop.scroll < oldMaximum) {
            ++shop.scroll;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = shop.scroll;
        const std::int32_t maximum = shop.itemCount - 5;
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 2));
        if (indexedElement) {
            static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243093;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 243095;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x004BAF10。
static std::int32_t WindowShop_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(shop.scroll > 0 ? 51 : 55, 320));
        shop.scroll -= 5;
        if (shop.scroll < 0) {
            shop.scroll = 0;
        }
        const std::int32_t position = shop.scroll;
        const std::int32_t maximum = shop.itemCount - 5;
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
        if (indexedElement) {
            static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243030;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 243032;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243031;
    }
    return consumed;
}

// 原函数：0x004BB010。
static std::int32_t WindowShop_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t oldMaximum = shop.itemCount - 5;
        const bool canMove = oldMaximum > 0 && shop.scroll < oldMaximum;
        static_cast<void>(audio::PlaySe_52C450(canMove ? 51 : 55, 320));
        std::int32_t position = shop.scroll + 5;
        const std::int32_t maximum = shop.itemCount - 5;
        shop.scroll = position;
        if (position > maximum) {
            position = maximum;
            shop.scroll = position;
        }
        if (position < 0) {
            position = 0;
            shop.scroll = position;
        }
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
        if (indexedElement) {
            static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243033;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 243035;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243034;
    }
    return consumed;
}

// 原函数：0x004BB130。
static std::int32_t WindowShop_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const std::int32_t count = shop.itemCount;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = shop.scroll;
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 5));
        if (indexedElement) {
            shop.scroll =
                WindowScroll_ApplyWheel(indexedElement, position, count - 5, wheel);
        }
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
            WindowShop_RefreshBuyRows(callbackWindow);
            WindowShop_RefreshBuyQuantities(callbackWindow);
            break;
        case 1:
        case 4:
        case 5:
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
            break;
        case 2:
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        default:
            break;
        }
    }
    return 0;
}

// 原函数：0x004BB1D0。
static std::int32_t WindowShop_ButtonDecrease(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const std::int32_t row = shop.scroll + index - 42;
    const bool hover = (state & MouseFlag_Hover) != 0;
    auto permitted = static_cast<std::uint8_t>(index);
    std::int32_t decrement; // 原模式2仅在hover许可路径赋值，不补栈残留默认值。
    if (hover) {
        permitted = 0;
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0: {
            std::int32_t selected;
            std::memcpy(&selected, &shop.items[row].selected, sizeof(selected));
            permitted = selected > 0;
            break;
        }
        case 1:
        case 4:
        case 5: {
            const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
            std::int32_t selected;
            std::memcpy(&selected, &shop.items[item].selected, sizeof(selected));
            permitted = selected > 1;
            break;
        }
        case 2: {
            const auto* const record = &shop.items[row];
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (selected <= 0) {
                break;
            }
            std::int32_t remaining = shop.total;
            std::int32_t price;
            std::memcpy(&price, &record->price, sizeof(price));
            std::int32_t itemId = shop.currencyItemId;
            remaining -= price;
            std::int32_t released = 0;
            for (std::int32_t slot = 8; slot < 28; ++slot) {
                const world::PlayerItemRecord& item = world::gPlayerRuntimeSlots.items[slot];
                if (item.used != 0 && item.itemId == itemId) {
                    remaining -= item.count;
                    if (remaining < 0) {
                        break;
                    }
                    ++released;
                }
            }
            std::int32_t available = released + world::CountFreeInventorySlots_518C60();
            for (std::int32_t item = 0; item < 20; ++item) {
                std::int32_t count;
                std::memcpy(&count, &shop.items[item].count, sizeof(count));
                available -= count;
            }
            if (available >= 0) {
                decrement = 1;
                permitted = 1;
                break;
            }
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::int32_t maximum;
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            std::int32_t candidate = selected % maximum;
            if (candidate == 0) {
                candidate = maximum;
            }
            if (selected < candidate) {
                break;
            }
            std::memcpy(&price, &record->price, sizeof(price));
            remaining = shop.total;
            const std::int32_t cost = price * candidate;
            itemId = shop.currencyItemId;
            remaining -= cost;
            released = 0;
            for (std::int32_t slot = 8; slot < 28; ++slot) {
                const world::PlayerItemRecord& item = world::gPlayerRuntimeSlots.items[slot];
                if (item.used != 0 && item.itemId == itemId) {
                    remaining -= item.count;
                    if (remaining < 0) {
                        break;
                    }
                    ++released;
                }
            }
            available = released + world::CountFreeInventorySlots_518C60();
            std::int32_t occupied = 0;
            for (std::int32_t item = 0; item < 20; ++item) {
                std::int32_t count;
                std::memcpy(&count, &shop.items[item].count, sizeof(count));
                occupied += count;
            }
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            const std::int32_t adjusted = (selected - candidate + maximum - 1) / maximum;
            std::int32_t count;
            std::memcpy(&count, &record->count, sizeof(count));
            if (available >= adjusted - count + occupied) {
                decrement = candidate;
                permitted = 1;
            }
            break;
        }
        case 3:
            permitted = shop.detailSelected > 0;
            break;
        default:
            break;
        }
        const wchar_t* hint = nullptr;
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
            hint = permitted != 0
                ? L"减少物品的数量。"
                : L"无法再减少物品的数量了。";
            break;
        case 3:
            hint = permitted != 0
                ? L"减少给予这些物品的数量。"
                : L"无法再减少了。";
            break;
        default:
            break;
        }
        if (hint != nullptr) {
            uiFrame.hoverText = hint;
        }
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0: {
            auto* const record = &shop.items[row];
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (selected > 0) {
                record->selected = CalculateRepeatedShopQuantity(*callbackWindow.ownerSession, selected - 1, state, false);
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            shop.message = -1;
            WindowShop_RefreshBuyQuantities(callbackWindow);
            WindowShop_RefreshBuyAmount(callbackWindow);
            WindowShop_CountBuySlots(callbackWindow);
            break;
        }
        case 1:
        case 4:
        case 5: {
            const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
            auto* const record = &shop.items[item];
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (selected > 1) {
                record->selected = selected - 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            shop.message = -1;
            WindowShop_RefreshSellQuantities(callbackWindow);
            WindowShop_RefreshSellAmount(callbackWindow);
            WindowShop_CountSellItems(callbackWindow);
            break;
        }
        case 2:
            if (permitted != 0) {
                auto* const record = &shop.items[row];
                std::int32_t selected;
                std::memcpy(&selected, &record->selected, sizeof(selected));
                selected -= decrement;
                record->selected = selected;
                std::int32_t maximum;
                std::memcpy(&maximum, &record->maximum, sizeof(maximum));
                record->count = (selected + maximum - 1) / maximum;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            shop.message = -1;
            WindowShop_RefreshExchangeAmount(callbackWindow);
            WindowShop_CountExchangeItems(callbackWindow);
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            break;
        case 3: {
            const std::int32_t selected = shop.detailSelected;
            if (selected > 0) {
                shop.valueD672D4 = 0;
                shop.detailSelected = selected - 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                std::int32_t capacity;
                std::int32_t current;
                std::memcpy(&capacity, &shop.detail.capacity, sizeof(capacity));
                std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                const std::int32_t available = shop.quantity;
                std::int32_t amount = capacity - current;
                if (amount >= available) {
                    amount = available;
                }
                const std::int32_t limit = gQuantityLimit581C94;
                shop.detailSelected = amount;
                if (amount >= limit) {
                    shop.detailSelected = limit;
                }
            }
            WindowShop_RefreshDetailName(callbackWindow);
            WindowShop_RefreshDetailAmount(callbackWindow);
            break;
        }
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243006;
    if (hover) {
        if (graphic) graphic->imageNo = 243008;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243007;
    }
    return consumed;
}

// 原函数：0x004BB6E0。
static std::int32_t WindowShop_ButtonIncrease(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const std::int32_t row = shop.scroll + index - 47;
    const bool hover = (state & MouseFlag_Hover) != 0;
    auto permitted = static_cast<std::uint8_t>(index);
    // 原模式2只在对应hover路径赋值；不补异常事件组合的栈残留默认值。
    std::int32_t message;
    std::int32_t increment;
    std::int32_t detailSelected = 0; // detailSelectedRead及其互补分支在使用前必定覆盖。
    bool detailSelectedRead = false;
    if (hover) {
        permitted = 0;
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0: {
            const auto* const record = &shop.items[row];
            if (record->used != 0) {
                std::int32_t selected;
                std::memcpy(&selected, &record->selected, sizeof(selected));
                permitted = selected < kShopPurchaseQuantityLimit;
            }
            break;
        }
        case 1:
        case 4:
        case 5: {
            const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
            if (item < 0) {
                break;
            }
            const auto* const record = &shop.items[item];
            std::int32_t selected;
            std::int32_t maximum;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            permitted = selected < maximum;
            const std::int32_t total = shop.total;
            const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
            std::int32_t price;
            std::memcpy(&price, &shop.items[row].price, sizeof(price));
            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
            if (price + total + money > limit) {
                permitted = 0;
            }
            break;
        }
        case 2: {
            const auto* const record = &shop.items[row];
            if (record->used == 0) {
                break;
            }
            std::int32_t price;
            std::memcpy(&price, &record->price, sizeof(price));
            const std::int32_t total = shop.total;
            message = -1;
            const std::int32_t cost = total + price;
            if (shop.available < cost) {
                message = -2;
                break;
            }
            const std::int32_t itemId = shop.currencyItemId;
            std::int32_t remaining = cost;
            increment = remaining;
            std::int32_t released = 0;
            std::int32_t slot = 8;
            for (; slot < 28; ++slot) {
                const world::PlayerItemRecord& item = world::gPlayerRuntimeSlots.items[slot];
                if (item.used != 0 && item.itemId == itemId) {
                    remaining -= item.count;
                    if (remaining < 0) {
                        break;
                    }
                    increment = remaining;
                    ++released;
                }
            }
            std::int32_t available = released + world::CountFreeInventorySlots_518C60();
            for (std::int32_t item = 0; item < 20; ++item) {
                std::int32_t count;
                std::memcpy(&count, &shop.items[item].count, sizeof(count));
                available -= count;
            }
            std::int32_t maximum;
            std::int32_t selected;
            std::int32_t occupied;
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            std::memcpy(&selected, &record->selected, sizeof(selected));
            const std::int32_t numerator = selected + maximum;
            std::memcpy(&occupied, &record->count, sizeof(occupied));
            if (numerator / maximum > occupied) {
                --available;
            }
            if (available >= 0) {
                increment = 1;
                permitted = 1;
                break;
            }
            std::int32_t accumulated = 0;
            std::int32_t deficit = -available;
            for (; slot < 28; ++slot) {
                const world::PlayerItemRecord& item = world::gPlayerRuntimeSlots.items[slot];
                if (item.used != 0 && item.itemId == shop.currencyItemId) {
                    const std::int32_t count = item.count;
                    std::memcpy(&price, &record->price, sizeof(price));
                    std::int32_t candidate = (count - increment + price - 1) / price;
                    const std::int32_t currentTotal = shop.total;
                    ++candidate;
                    if (shop.available < price * candidate + currentTotal) {
                        message = -3;
                        break;
                    }
                    accumulated += candidate;
                    if ((numerator + accumulated - 1) / maximum - occupied <= deficit) {
                        increment = accumulated;
                        permitted = 1;
                        break;
                    }
                    ++deficit;
                }
            }
            if (permitted == 0) {
                message = -3;
            }
            break;
        }
        case 3:
            if (row == 0) {
                if (shop.detailMode == 0) {
                    const std::int32_t available = shop.quantity;
                    detailSelected = shop.detailSelected;
                    detailSelectedRead = true;
                    if (available - detailSelected > 0) {
                        std::int32_t current;
                        std::int32_t capacity;
                        std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                        std::memcpy(&capacity, &shop.detail.capacity, sizeof(capacity));
                        if (current + detailSelected < capacity && detailSelected < gQuantityLimit581C94) {
                            permitted = 1;
                        }
                    }
                } else {
                    detailSelected = shop.detailSelected;
                    detailSelectedRead = true;
                    std::int32_t quantity;
                    std::memcpy(&quantity, &shop.detail.quantity, sizeof(quantity));
                    if (detailSelected < quantity) {
                        std::int32_t current;
                        std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                        if (current - detailSelected > 0) {
                            const std::int32_t total = shop.total;
                            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
                            if (total < money && detailSelected < gQuantityLimit581C94) {
                                permitted = 1;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    if (!detailSelectedRead) {
        detailSelected = shop.detailSelected;
    }
    if (hover) {
        const wchar_t* hint = nullptr;
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
            hint = permitted != 0
                ? L"增加物品的数量。"
                : L"无法再增加物品的数量了。";
            break;
        case 3:
            hint = permitted != 0
                ? L"增加给予这些物品的数量。"
                : L"无法再增加了。";
            break;
        default:
            break;
        }
        if (hint != nullptr) {
            uiFrame.hoverText = hint;
        }
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
        case 0: {
            auto* const record = &shop.items[row];
            if (record->used == 0) {
                break;
            }
            std::int32_t selected;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            if (selected < kShopPurchaseQuantityLimit) {
                record->selected = CalculateRepeatedShopQuantity(*callbackWindow.ownerSession, selected + 1, state, true);
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            WindowShop_RefreshBuyQuantities(callbackWindow);
            WindowShop_RefreshBuyAmount(callbackWindow);
            WindowShop_CountBuySlots(callbackWindow);
            break;
        }
        case 1:
        case 4:
        case 5: {
            const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
            if (item < 0) {
                break;
            }
            auto* const record = &shop.items[item];
            std::int32_t selected;
            std::int32_t maximum;
            std::memcpy(&selected, &record->selected, sizeof(selected));
            std::memcpy(&maximum, &record->maximum, sizeof(maximum));
            bool canIncrease = false;
            if (selected < maximum) {
                const std::int32_t total = shop.total;
                const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
                const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
                std::int32_t price;
                std::memcpy(&price, &shop.items[row].price, sizeof(price));
                canIncrease = price + total + money <= limit;
            }
            if (canIncrease) {
                record->selected = selected + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            WindowShop_RefreshSellQuantities(callbackWindow);
            WindowShop_RefreshSellAmount(callbackWindow);
            WindowShop_CountSellItems(callbackWindow);
            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
            const std::int32_t total = shop.total;
            const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
            if (total + money > limit) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                shop.message = -2;
            }
            break;
        }
        case 2: {
            auto* const record = &shop.items[row];
            if (record->used == 0) {
                break;
            }
            shop.message = message;
            if (permitted != 0) {
                std::int32_t selected;
                std::memcpy(&selected, &record->selected, sizeof(selected));
                selected += increment;
                record->selected = selected;
                std::int32_t maximum;
                std::memcpy(&maximum, &record->maximum, sizeof(maximum));
                record->count = (selected + maximum - 1) / maximum;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            WindowShop_RefreshExchangeRows(callbackWindow);
            WindowShop_RefreshExchangeQuantities(callbackWindow);
            WindowShop_RefreshExchangeAmount(callbackWindow);
            WindowShop_CountExchangeItems(callbackWindow);
            break;
        }
        case 3: {
            if (row != 0) {
                break;
            }
            bool canIncrease = false;
            if (shop.detailMode == 0) {
                const std::int32_t available = shop.quantity - detailSelected;
                if (available <= 0) {
                    shop.message = -2;
                } else {
                    std::int32_t current;
                    std::int32_t capacity;
                    std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                    std::memcpy(&capacity, &shop.detail.capacity, sizeof(capacity));
                    if (current + detailSelected < capacity && detailSelected < gQuantityLimit581C94) {
                        canIncrease = true;
                    } else {
                        std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                        std::memcpy(&capacity, &shop.detail.capacity, sizeof(capacity));
                        if (current + detailSelected >= capacity) {
                            shop.message = -3;
                        }
                    }
                }
            } else {
                std::int32_t quantity;
                std::memcpy(&quantity, &shop.detail.quantity, sizeof(quantity));
                if (detailSelected >= quantity) {
                    shop.message = -2;
                } else {
                    std::int32_t current;
                    std::memcpy(&current, &shop.detail.valueC34, sizeof(current));
                    if (current - detailSelected <= 0) {
                        shop.message = -3;
                    } else {
                        const std::int32_t total = shop.total;
                        const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
                        if (total < money && detailSelected < gQuantityLimit581C94) {
                            canIncrease = true;
                        } else {
                            const std::int32_t updatedMoney = world::gPlayerRuntimeSlots.valueFC;
                            const std::int32_t updatedTotal = shop.total;
                            if (updatedTotal >= updatedMoney) {
                                shop.message = -4;
                            }
                        }
                    }
                }
            }
            if (canIncrease) {
                shop.message = -1;
                shop.detailSelected = detailSelected + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                shop.detailSelected = 0;
            }
            WindowShop_RefreshDetailName(callbackWindow);
            WindowShop_RefreshDetailAmount(callbackWindow);
            break;
        }
        default:
            break;
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243003;
    if (hover) {
        if (graphic) graphic->imageNo = 243005;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243004;
    }
    return consumed;
}

// 原函数：0x004BBED0。
static std::int32_t WindowShop_HandleItemIcon(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t item = shop.scroll + index - 52;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
    }
    const auto* const record = &shop.items[item];
    if (record->used == 0) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        if (shop.rowKeys[index - 52] != item) {
            shop.tooltipPage = 0;
        }
        shop.rowKeys[index - 52] = item;
        if ((state & MouseFlag_RDownEvent) != 0) {
            ++shop.tooltipPage;
            if (shop.tooltipPage >= 4) {
                shop.tooltipPage = 0;
            }
            consumed = 1;
        }
        shop.tooltipIndex = item + 100;
    }
    if (record->allowed == 0) {
        static_cast<void>(render::ClearTextQueueTail_456D50(3));
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x + 1;
            const std::int32_t y = window->y + element.y + 31;
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)), 3, 0, 6, L"X", 0, 0));
        }
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement& element = (*elementPtr);
        std::int32_t imageNo;
        std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
        static_cast<void>(cg::render::DrawGraphic(
            element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x004BC060。
static std::int32_t WindowShop_HandleDetailIcon(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_RDownEvent) != 0) {
        ++shop.tooltipPage;
        if (shop.tooltipPage >= 4) {
            shop.tooltipPage = 0;
        }
        consumed = 1;
    }
    if ((state & MouseFlag_Hover) != 0) {
        shop.tooltipIndex = 200;
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement& element = (*elementPtr);
        std::int32_t imageNo;
        std::memcpy(&imageNo, &shop.detail.imageNo, sizeof(imageNo));
        static_cast<void>(cg::render::DrawGraphic(
            element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x004BC230。
static std::int32_t WindowShop_HandleSaleSelection(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t row = shop.scroll + index - 52;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    if (shop.rowKeys[index - 52] != row) {
        shop.tooltipPage = 0;
    }
    shop.rowKeys[index - 52] = row;
    auto events = static_cast<std::uint8_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 9 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 2) {
        consumed = 1;
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
    }
    if ((events & MouseFlag_DragFinish) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) == 9) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession);
        if (source != row) {
            const char sourceItem = shop.selectedSlots[source];
            const char targetItem = shop.selectedSlots[row];
            shop.selectedSlots[row] = sourceItem;
            shop.selectedSlots[source] = targetItem;
            WindowShop_RefreshSellRows(callbackWindow);
            WindowShop_RefreshSellQuantities(callbackWindow);
        }
        WindowDrag_Reset(*callbackWindow.ownerSession);
    }
    const bool hover = (events & MouseFlag_Hover) != 0;
    if (hover && std::bit_cast<std::int8_t>(shop.selectedSlots[row]) >= 0) {
        const char mode = materialState.shopMode;
        if (mode == 1 || mode == 4 || mode == 5) {
            uiFrame.hoverText = kShopHints57E6C4[2];
        }
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0) {
        static_cast<void>(WindowDrag_ResetMouse(*callbackWindow.ownerSession));
        const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
        shop.selectedSlots[row] = static_cast<char>(-1);
        auto* const record = &shop.items[item];
        record->selected = std::int32_t{0};
        std::int32_t inventoryIndex;
        std::memcpy(&inventoryIndex, &record->inventoryIndex, sizeof(inventoryIndex));
        const auto selectedMask = static_cast<std::uint32_t>(shop.selectionMask);
        shop.selectionMask = std::bit_cast<std::int32_t>(selectedMask & ~(1u << (inventoryIndex - 8)));
        WindowShop_RefreshSellRows(callbackWindow);
        WindowShop_RefreshSellQuantities(callbackWindow);
        WindowShop_RefreshSellAmount(callbackWindow);
        WindowShop_CountSellItems(callbackWindow);
        shop.message = -1;
    }
    if (hover && std::bit_cast<std::int8_t>(shop.selectedSlots[row]) >= 0
        && (events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(WindowShop_StartSaleDrag(*callbackWindow.ownerSession, row));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (WindowDrag_ReadKind(*callbackWindow.ownerSession) == 9 && WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession) == row) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
                223, 254, 0);
        }
    }
    if (hover) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
    }
    if ((events & MouseFlag_Dragging) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) == 9) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
    }
    const std::int32_t item = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
    if (item < 0) {
        return 0;
    }
    if (hover) {
        if (shop.rowKeys[index - 52] != row) {
            shop.tooltipPage = 0;
        }
        shop.rowKeys[index - 52] = row;
        if ((events & MouseFlag_RDownEvent) != 0) {
            ++shop.tooltipPage;
            if (shop.tooltipPage >= 4) {
                shop.tooltipPage = 0;
            }
            consumed = 1;
        }
        shop.tooltipIndex = item + 100;
        const auto* const record = &shop.items[item];
        std::wstring buffer;
        std::uint32_t flags = 0;
        if (!record->memo.empty()) {
            std::memcpy(&flags, &record->flags, sizeof(flags));
        }
        if ((flags & 0x100u) != 0) {
            cg::text::FormatTo(buffer, L"{}[kok]", record->name);
            uiFrame.hoverText = buffer;
        } else {
            std::memcpy(&flags, &record->flags, sizeof(flags));
            if ((flags & 0x200u) != 0) {
                cg::text::FormatTo(buffer, L"{}[han]", record->memo);
                uiFrame.hoverText = buffer;
            }
        }
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement& element = (*elementPtr);
        const std::int32_t drawItem = std::bit_cast<std::int8_t>(shop.selectedSlots[row]);
        std::int32_t imageNo;
        std::memcpy(&imageNo, &shop.items[drawItem].imageNo, sizeof(imageNo));
        static_cast<void>(cg::render::DrawGraphic(
            element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x004BC660。
static std::int32_t WindowShop_HandleInventory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto events = static_cast<std::uint8_t>(state);
    std::int32_t consumed = 0;
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 2) {
        events &= static_cast<std::uint8_t>(~(MouseFlag_Dragging | MouseFlag_DragFinish));
        consumed = 1;
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        for (std::int32_t hitSlot = 0; hitSlot < 20; ++hitSlot) {
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * (hitSlot % 5) + window->x + 2;
                const std::int32_t top = element.y + 50 * (hitSlot / 5) + window->y + 2;
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = hitSlot;
                    break;
                }
            }
        }
    }
    // 命中下标承接原临时槽；右键空槽扫描仍覆写，后续提示不能改读hovered。
    std::int32_t scanSlot = hovered;
    const bool hover = (events & MouseFlag_Hover) != 0;
    auto mode = std::bit_cast<std::int8_t>(materialState.shopMode);
    if (hover && hovered >= 0) {
        const wchar_t* hint = nullptr;
        switch (mode) {
        case 1:
            hint = L"以滑鼠拖放方式选择要卖的物品。";
            break;
        case 4:
            hint = L"以滑鼠拖放方式选择要鉴定的物品。";
            break;
        case 5:
            hint = L"以滑鼠拖放方式选择要修理的物品。";
            break;
        default:
            break;
        }
        if (hint != nullptr) {
            uiFrame.hoverText = hint;
            mode = std::bit_cast<std::int8_t>(materialState.shopMode);
        }
    }
    if (shop.inventoryHover != hovered) {
        shop.tooltipPage = 0;
    }
    shop.inventoryHover = hovered;
    const auto& items = world::gPlayerRuntimeSlots.items;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0) {
        if (mode == 1 || mode == 4 || mode == 5) {
            if (WindowDrag_ReadFocusWindowId(*callbackWindow.ownerSession) == 53 || WindowDrag_ReadFocusWindowId(*callbackWindow.ownerSession) == 0) {
                const std::int32_t item = shop.inventoryMap[hovered];
                if (item < 0 || item >= 20
                    || (static_cast<std::uint32_t>(shop.selectionMask) & (1u << hovered)) != 0
                    || shop.items[item].allowed == 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    scanSlot = 0;
                    while (scanSlot < 20
                        && std::bit_cast<std::int8_t>(shop.selectedSlots[scanSlot]) >= 0) {
                        ++scanSlot;
                    }
                    if (scanSlot >= 20) {
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else {
                        auto* const record = &shop.items[item];
                        bool exceedsLimit = false;
                        if (materialState.shopMode == 1) {
                            std::int32_t amount;
                            std::memcpy(&amount, &record->price, sizeof(amount));
                            amount += shop.total;
                            amount += world::gPlayerRuntimeSlots.valueFC;
                            exceedsLimit = amount > world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
                        }
                        if (exceedsLimit) {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                            shop.message = -2;
                        } else {
                            std::int32_t maximumCount;
                            std::memcpy(&maximumCount, &record->maximum, sizeof(maximumCount));
                            shop.selectedSlots[scanSlot] = static_cast<char>(item);
                            record->selected = maximumCount;
                            if (maximumCount <= 0) {
                                record->selected = std::int32_t{1};
                            }
                            const auto mask = static_cast<std::uint32_t>(shop.selectionMask);
                            const std::int32_t position = shop.scroll;
                            const std::int32_t maximum = shop.itemCount - 5;
                            shop.selectionMask = std::bit_cast<std::int32_t>(mask | (1u << hovered));
                            auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowShopElementId::HandleVerticalScroll);
                            if (handleVerticalScrollElement) {
                                static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
                            }
                            WindowShop_RefreshSellRows(callbackWindow);
                            WindowShop_RefreshSellQuantities(callbackWindow);
                            WindowShop_RefreshSellAmount(callbackWindow);
                            WindowShop_CountSellItems(callbackWindow);
                        }
                    }
                }
                static_cast<void>(WindowDrag_ResetMouse(*callbackWindow.ownerSession));
            }
            consumed = 1;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if (hover && hovered >= 0) {
        if ((mode == 1 || mode == 4 || mode == 5) && (events & MouseFlag_LDownEvent) != 0) {
            const std::int32_t item = shop.inventoryMap[hovered];
            if (item >= 0 && item < 20
                && (static_cast<std::uint32_t>(shop.selectionMask) & (1u << hovered)) == 0) {
                const std::int32_t mappedItem = shop.inventoryMap[scanSlot];
                if (shop.items[mappedItem].allowed != 0) {
                    static_cast<void>(WindowItemDrag_Start(*callbackWindow.ownerSession, hovered + 8, 0));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
            consumed = 1;
        }
        if ((events & MouseFlag_RDownEvent) != 0) {
            if (items[hovered + 8].used != 0) {
                std::int32_t page = shop.tooltipPage;
                const std::int32_t pages = items[hovered + 8].pages;
                ++page;
                shop.tooltipPage = page;
                if (page >= pages) {
                    shop.tooltipPage = 0;
                }
            }
            consumed = 1;
        }
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
            mode = std::bit_cast<std::int8_t>(materialState.shopMode);
            if ((mode != 1 && mode != 4 && mode != 5) || shop.inventoryMap[scanSlot] >= 0) {
                shop.tooltipIndex = hovered + 8;
            }
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
    switch (std::bit_cast<std::int8_t>(materialState.shopMode)) {
    case 0:
    case 2:
    case 3:
        for (std::int32_t position = 0; position < 20; ++position) {
            const world::PlayerItemRecord& item = items[position + 8];
            if (item.used == 0) {
                continue;
            }
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t x = element.x + 50 * (position % 5) + window->x + 26;
                const std::int32_t y = element.y + 50 * (position / 5) + window->y + 26;
                if (item.itemId == shop.currencyItemId) {
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
        break;
    case 1:
    case 4:
    case 5:
        for (std::int32_t position = 0; position < 20; ++position) {
            const world::PlayerItemRecord& item = items[position + 8];
            if (shop.inventoryMap[position] < 0 || item.used == 0) {
                continue;
            }
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t x = element.x + 50 * (position % 5) + window->x + 26;
                const std::int32_t y = element.y + 50 * (position / 5) + window->y + 26;
                if ((static_cast<std::uint32_t>(shop.selectionMask) & (1u << position)) != 0) {
                    cg::render::DrawGraphicHover(
                        static_cast<std::uint16_t>(x - 22), static_cast<std::uint16_t>(y - 22),
                        static_cast<std::uint16_t>(x + 22), static_cast<std::uint16_t>(y + 22),
                        223, 256, 0);
                }
                const std::int32_t mappedItem = shop.inventoryMap[position];
                // 原4BCE19读取D052FC；该原行尾字节始终为零，宽字符串不承接固定区旁读。
                const char allowed = mappedItem == 20
                    ? 0
                    : shop.items[mappedItem].allowed;
                if (allowed == 0) {
                    static_cast<void>(cg::render::DrawGraphic(x, y, 223, 23040, 0, nullptr));
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
        break;
    default:
        break;
    }
    return consumed;
}

// 原函数：0x004BD100。
static std::int32_t WindowShop_HandleConfirmation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t unusedState)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unusedState);
    // 原覆盖层矩形(0,35,640,383)接近原画布中心；独立适配，不使用商店窗口位置。
    std::int32_t panelX = 0;
    std::int32_t panelY = 35;
    AdaptWindowPosition(panelX, panelY, 640, 383);
    const std::int32_t offsetY = panelY - 35;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const bool acceptHover = cg::ui::HitTestMouseRect_5049A0(panelX + 100, offsetY + 381, panelX + 166, offsetY + 398, -1) != 0;
    const bool cancelHover = cg::ui::HitTestMouseRect_5049A0(panelX + 510, offsetY + 381, panelX + 576, offsetY + 398, -1) != 0;
    std::int32_t acceptImage = 245323;
    if (acceptHover) {
        acceptImage = 245325;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        uiFrame.hoverText = shop.confirmationAcceptHint;
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = shop.confirmationCallback;
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
        uiFrame.hoverText = shop.confirmationCancelHint;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        static_cast<void>(0);
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            const WindowValueCallback callback = shop.confirmationCallback;
            if (callback != nullptr) {
                callback(callbackWindow, 1);
            }
            cancelImage = 245327;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(panelX + 100, offsetY + 381, 226, acceptImage, 0, 1, 0, &descriptor));
    static_cast<void>(render::DrawGraphic3_520970(panelX + 510, offsetY + 381, 226, cancelImage, 0, 1, 0, &descriptor));
    std::wstring buffer;
    for (std::int32_t column = 0; column < 4; ++column) {
        const std::int32_t x = panelX + 18 + column * 152;
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t y = offsetY + 47 + row * 65;
            const WindowConfirmationItem& item = shop.confirmationItems[column * 5 + row];
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
    static_cast<void>(render::DrawText_456780(static_cast<std::int16_t>(panelX + 190), static_cast<std::int16_t>(offsetY + 377), 4, 12, 0, shop.confirmationHeading, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    static_cast<void>(render::DrawText_456780(static_cast<std::int16_t>(panelX + 190), static_cast<std::int16_t>(offsetY + 393), 4, 12, 0, shop.confirmationQuestion, 0, static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    for (const WindowConfirmationAmount& amount : shop.confirmationAmounts) {
        if (amount.visible != 0) {
            cg::text::FormatTo(buffer, L"{}", amount.value);
            const std::int32_t y = offsetY + amount.y;
            const std::int32_t x = panelX + amount.x;
            WindowNumber_Draw(x, y + 35, buffer, 0, 4, 1, 226);
        }
    }
    cg::yobi::DrawNineGrid_49BE00(
        245300, panelX, panelY, 640, 383, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004B99D0。
static std::int32_t WindowShop_Draw(WindowInstance& callbackWindow, const std::int32_t unused)
{
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    auto& materialState = callbackWindow.ownerSession->UiFrame().native.material;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unused);
    const WindowInstance* const window = currentWindow;
    const std::int32_t x = window->x + 298;
    std::int32_t y = window->y + 50;
    char mode = materialState.shopMode;
    if (mode == 1) {
        const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
        const std::int32_t total = money + shop.total;
        const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
        if (total > limit) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            mode = materialState.shopMode;
            shop.message = -2;
        }
    }
    if (mode == 3 && shop.detailMode == 0 && shop.quantity <= 0) {
        shop.message = -4;
    }
    const std::int32_t tooltip = shop.tooltipIndex;
    if (tooltip >= 0) {
        if (tooltip < 100) {
            static_cast<void>(WindowItem_DrawEquipmentTooltip(*callbackWindow.ownerSession, tooltip, shop.tooltipPage));
        } else if (tooltip < 200) {
            static_cast<void>(WindowItem_DrawShopTooltip(*callbackWindow.ownerSession, tooltip - 100, shop.tooltipPage, mode == 0));
        } else {
            static_cast<void>(WindowItem_DrawWorkTooltip(*callbackWindow.ownerSession, shop.tooltipPage));
        }
    }
    const std::int32_t message = shop.message;
    gTooltipColorD67D30 = 0;
    const WindowDialogTextLine* line = npcLines.data() + 1 + 4 * (-1 - message);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (!line->text.empty()) {
            static_cast<void>(WindowItem_SplitTooltipLines(x, y, 3, 8, 0, line->text.data(), 0, 0));
        }
        ++line;
        y += 10;
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

} // namespace detail

} // namespace cg::ui
