#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "../../scene/login/LoginSavedState.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldObjectState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowStallElementId : ElementId {
    DrawTooltipOverlay = 1,
    HandlePriceInput = 2,
    ButtonClose = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    EditName = 7,
    Element8 = 8,
    EditDescription = 9,
    Element10 = 10,
    ScrollListings = 11,
    ScrollPetCandidates = 12,
    HandleItemRow = 13,
    HandleItemRow_14 = 14,
    HandleItemRow_15 = 15,
    HandleItemRow_16 = 16,
    HandleItemRow_17 = 17,
    HandleItemRow_18 = 18,
    HandleItemRow_19 = 19,
    HandleItemRow_20 = 20,
    HandleItemRow_21 = 21,
    HandleItemRow_22 = 22,
    HandleInventory = 23,
    HandlePetRow = 24,
    HandlePetRow_25 = 25,
    HandlePetRow_26 = 26,
    HandlePetCandidate = 27,
    HandlePetCandidate_28 = 28,
    HandlePetCandidate_29 = 29,
    HandlePetCandidate_30 = 30,
    HandleButtons = 31,
    HandleButtons_32 = 32,
    HandleButtons_33 = 33,
    HandleButtons_34 = 34,
    HandleButtons_35 = 35,
    TabCategory = 36,
    TabCategory_37 = 37,
    Element38 = 38,
    HandleVerticalScroll = 39,
    ButtonPreviousListing = 40,
    ButtonNextListing = 41,
    Element42 = 42,
    HandleVerticalScroll_43 = 43,
    ButtonPreviousPet = 44,
    ButtonNextPet = 45,
    Element46 = 46,
    Element47 = 47,
    Element48 = 48,
};

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowStallElementId_gWindowElements57BB08 : ElementId {
    DrawTooltipOverlay = 1,
    HandlePurchaseConfirmation = 2,
    ButtonClose = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    ScrollListings = 9,
    ScrollPetCandidates = 10,
    HandleItemRow = 11,
    HandleItemRow_12 = 12,
    HandleItemRow_13 = 13,
    HandleItemRow_14 = 14,
    HandleItemRow_15 = 15,
    HandleItemRow_16 = 16,
    HandleItemRow_17 = 17,
    HandleItemRow_18 = 18,
    HandleItemRow_19 = 19,
    HandleItemRow_20 = 20,
    HandleInventory = 21,
    HandlePetRow = 22,
    HandlePetRow_23 = 23,
    HandlePetRow_24 = 24,
    HandlePetCandidate = 25,
    HandlePetCandidate_26 = 26,
    HandlePetCandidate_27 = 27,
    HandlePetCandidate_28 = 28,
    HandleButtons = 29,
    HandleButtons_30 = 30,
    TabCategory = 31,
    TabCategory_32 = 32,
    Element33 = 33,
    HandleVerticalScroll = 34,
    ButtonPreviousListing = 35,
    ButtonNextListing = 36,
    Element37 = 37,
    HandleVerticalScroll_38 = 38,
    ButtonPreviousPet = 39,
    ButtonNextPet = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。




// 原D046D8连续浏览区；仅复用值类型布局，不与玩家或交易区共享存储。
struct StallBrowseData {
    std::wstring sellerName00; // 原成员偏移：+0x00。
    std::wstring stallName11; // 原成员偏移：+0x11。
    std::wstring description22; // 原成员偏移：+0x22。
    std::array<std::byte, 1> bytes5B; // 原成员偏移：+0x5B。
    std::int32_t target5C; // 原成员偏移：+0x5C。 4CED60发布的浏览对象编号，不是地址。
    std::int32_t imageNo60; // 原成员偏移：+0x60。
    std::int32_t action64; // 原成员偏移：+0x64。
    std::int32_t direction68; // 原成员偏移：+0x68。
    std::array<world::PlayerItemRecord, 20> items;
    std::array<std::int32_t, 20> itemValuesD13E74; // 原全局：0x00D13E74。
    std::array<std::int32_t, 20> itemValuesD13EC4; // 原全局：0x00D13EC4。
    std::array<world::PetRuntimeRecord, 5> pets;
    std::array<std::int32_t, 5> petValuesD2D464; // 原全局：0x00D2D464。
    std::array<std::int32_t, 5> petValuesD2D478; // 原全局：0x00D2D478。
};

// 指向原启动入口创建的唯一私有对象；不持有或复制宠物数据。
static StallBrowseData* gStallBrowseData = nullptr;

// 自身摊位上架项只引用玩家槽号；不同于浏览摊位的两张并行数值表。
struct StallOffer {
    std::int32_t slot;
    std::int32_t value;
};

// 回调与内部函数前置声明。

// 原函数：0x004CE700。
static void WindowStall_ReceiveOpenResult(session::ClientSession& session, std::int32_t result); // 开摊结果：玩家标志、名字及窗口67首元素。

// 原函数：0x004CE8B0。
static void WindowStall_ReceiveCloseResult(session::ClientSession& session, std::int32_t result); // 收摊结果：两路均先清玩家标志/等待/门禁。

// 原函数：0x004CE930。
static void WindowStall_ReceivePurchaseResult(session::ClientSession& session, std::int32_t result); // 购买结果：浏览记录和两并行表原序搬移，或显示原提示。

// 原函数：0x004CF8A0。
static void WindowStall_BuildItemDescription(world::PlayerItemRecord* item, const char* text); // 固定32行描述及前一行回扫。

// 原函数：0x004CECB0。
static std::int32_t WindowStall_RequestPurchase(WindowInstance* callbackWindow); // 选择索引直接查询后发购买请求，返回宽提示长度（含NUL）或原负值。

// 原函数：0x004CD420。
static std::int32_t WindowStall_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 两等待门禁、关闭标记和原图形快照。

// 原函数：0x004CDE00。
static void WindowStall_Release(WindowInstance&); // 67/68共用释放，条件发送后无条件SE57。

// 原函数：0x004CDB10。
static std::int32_t WindowStall_Draw(WindowInstance& callbackWindow, std::int32_t unusedValue); // 两类摊位预览、失效上架剔除与提示绘制。

// 原函数：0x004D27C0。
static std::int32_t WindowStall_AddListing(std::int32_t value); // 当前选择追加到自身上架表，先清选择最后发布数量。

// 原函数：0x004CD4F0。
static std::int32_t WindowStall_Update(WindowInstance& callbackWindow, std::int32_t value); // 共用初始化、双滚动/输入同步、文本绘制及金额尾。

// 原函数：0x004D2080。
static std::int32_t WindowStall_EditDescription(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 固定元素8说明编辑，hover不消费。

// 原函数：0x004D2170。
static std::int32_t WindowStall_EditName(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 固定元素6名称编辑，同源Input对象。

// 原函数：0x004CDE40。
static std::int32_t WindowStall_ButtonPreviousListing(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上架列表向前，bit400动作。

// 原函数：0x004CDFA0。
static std::int32_t WindowStall_ButtonNextListing(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上架列表向后，音效后重读滚动参数。

// 原函数：0x004CE0F0。
static std::int32_t WindowStall_ScrollListings(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上架列表滚轮，始终返回0。

// 原函数：0x004CE1F0。
static std::int32_t WindowStall_ButtonPreviousPet(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物候选向前，非宠物模式仍可消费bit400。

// 原函数：0x004CE2B0。
static std::int32_t WindowStall_ButtonNextPet(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物候选向后，最大页1。

// 原函数：0x004CE370。
static std::int32_t WindowStall_ScrollPetCandidates(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物候选滚轮，始终返回0。

// 原函数：0x004CE3F0。
static std::int32_t WindowStall_TabCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 类别页签及七项候选控件原序显隐。

// 原函数：0x004CFE70。
static std::int32_t WindowStall_SubmitListings(WindowInstance* callbackWindow); // 槽序上架组包、转义发送及同源保存记录回写，返回0。

// 原函数：0x004D0230。
static std::int32_t WindowStall_DrawTooltipOverlay(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 共用提示覆盖层，只返回hover位。

// 原函数：0x004CF940。
static std::int32_t WindowStall_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 自身/浏览主要按钮，禁用与普通收尾分离。

// 原函数：0x004CE8A0。
static network::SendResult WindowStall_RequestClose(); // 收摊请求，透传发送结果。

// 原函数：0x004CED50。
static WindowInstance* WindowStall_ShowPurchaseConfirmation(WindowInstance* callbackWindow); // 显示购买确认，返回入口窗口。

// 原函数：0x004CED60。
static void WindowStall_OpenBrowse(
    session::ClientSession& session,
    std::int32_t status, std::int32_t target, const char* stallName,
    const char* description, const char* itemPayload, const char* petPayload); // 浏览区完整生产后打开68。

// 原函数：0x004CFC40。
static WindowElement* WindowStall_EditItemPrice(WindowInstance* callbackWindow); // 物品定价输入初始化，返回末次取得的元素表。

// 原函数：0x004CFCA0。
static void WindowStall_EditPetPrice(WindowInstance* callbackWindow, std::int32_t petIndex); // 固定六装备槽检查后进入宠物定价。

// 原函数：0x004CFD70。
static std::int32_t WindowStall_RemoveListing(); // 删除上架项，保持标记/数量/搬移/选择与页回退顺序。

// 原函数：0x004D2260。
static std::int32_t WindowStall_HandlePriceInput(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 完整定价键盘和鼠标交互，返回hover位。

// 原函数：0x004D2840。
static std::int32_t WindowStall_HandlePurchaseConfirmation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 购买确认及物品/宠物详情，二次读模式。

// 原函数：0x004D1D50。
static std::int32_t WindowStall_HandleInventory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 玩家背包20格，首命中交互但完整绘制。

// 原函数：0x004D04A0。
static std::int32_t WindowStall_HandleItemRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上架/浏览物品行，固定记录与价格重读分离。

// 原函数：0x004D16F0。
static std::int32_t WindowStall_HandlePetCandidate(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物候选，逐项重读候选页并保持CryptInt顺序。

// 原函数：0x004D1C00。
static std::int32_t WindowStall_DrawCandidateAttributes(
    std::int32_t petIndex, std::int32_t attribute, std::int32_t x, std::int32_t y); // 候选属性条，透传末绘制返回。

// 原函数：0x004D0990。
static std::int32_t WindowStall_DrawPetSkills(
    std::int32_t x, std::int32_t y, std::int32_t petIndex, std::int32_t skillPosition); // 浏览宠物技能，使用独立浏览记录。

// 原函数：0x004D0A80。
static std::int32_t WindowStall_DrawPetDetails(
    std::int32_t x, std::int32_t y, std::int32_t index); // 完整浏览宠物详情，复用D67B84预览。

// 原函数：0x004D0EB0。
static std::int32_t WindowStall_HandlePetRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上架/浏览宠物行，末尾金额两次读取及详情分派。

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00C8A978。 摆摊结果提示，4D0230消费同一文本。
static std::wstring gStallMessageC8A978 {}; // 原全局：0x00C8A978。

// 原全局：0x00D67BE4。 原strncpy16及D67BF4终止字节。
static std::wstring gStallNameD67BE4 {}; // 原全局：0x00D67BE4。

// 原全局：0x00D67C31。 请求与回复共用等待标记。
static std::uint8_t gStallPendingD67C31 = 0; // 原全局：0x00D67C31。

static std::int32_t gStallMessageModeD67C34 = 0; // 原全局：0x00D67C34。

static std::int32_t gStallMessageValueD67C38 = 0; // 原全局：0x00D67C38。

// 原全局：0x0057B440。 原可变金额串，止于下一数字原型57B450。
static std::wstring gStallMoneyText57B440 {L"01234567"}; // 原全局：0x0057B440。

static_assert(!std::is_trivially_destructible_v<StallBrowseData>);

static std::int32_t gStallItemPageD67BC8 = 0; // 原全局：0x00D67BC8。

static std::int32_t gStallItemCountD67BCC = 0; // 原全局：0x00D67BCC。

static std::int32_t gStallPetPageD67BD0 = 0; // 原全局：0x00D67BD0。

static std::int32_t gStallPetCountD67BD4 = 0; // 原全局：0x00D67BD4。

static std::int32_t gStallModeD67BDC = 0; // 原全局：0x00D67BDC。

static std::int32_t gStallItemSelection581124 = -1; // 原全局：0x00581124。

static std::int32_t gStallPetSelection581128 = -1; // 原全局：0x00581128。

static std::uint8_t gStallOwn58111C = 1; // 原全局：0x0058111C。

static std::int32_t gStallHover581120 = -1; // 原全局：0x00581120。

static std::int32_t gStallDescriptionPageD67C3C = 0; // 原全局：0x00D67C3C。

static std::int32_t gStallPetCandidatePageD67BD8 = 0; // 原全局：0x00D67BD8。

static std::wstring gStallDescriptionD67BF8 {}; // 原全局：0x00D67BF8。




static std::int32_t gStallPriceD67C40 = 0; // 原全局：0x00D67C40。

static std::int32_t gStallButtonStateD67C44 = 0; // 原全局：0x00D67C44。

static std::int32_t gStallInventoryHover5817FC = -1; // 原全局：0x005817FC。

static constexpr std::array<std::array<std::int32_t, 3>, 6> kStallButtonImages54EEE0 {{ // 原常量参考：0x0054EEE0。
    {98891, 98886, 98890},
    {98888, 98889, 98887},
    {98885, 98883, 98884},
    {98870, 98868, 98869},
    {98882, 98880, 98881},
    {98873, 98871, 98872},
}};

static constexpr std::array<std::array<std::int32_t, 3>, 2> kStallTabImages54EF40 {{ // 原常量参考：0x0054EF40。
    {98864, 98862, 98863},
    {98867, 98865, 98866},
}};

static constexpr std::array<std::array<std::int32_t, 3>, 2> kStallPurchaseButtonImages54EF28 {{ // 原常量参考：0x0054EF28。
    {98876, 98874, 98875},
    {98879, 98877, 98878},
}};

static_assert(sizeof(StallOffer) == 8);

static_assert(offsetof(StallOffer, value) == 4);

static std::array<StallOffer, 20> gStallItemOffersD671D8 {}; // 原全局：0x00D671D8。

static std::array<StallOffer, 5> gStallPetOffersCC3E98 {}; // 原全局：0x00CC3E98。

static std::array<std::uint8_t, 20> gStallItemUsedCC31AC {}; // 原全局：0x00CC31AC。

static std::array<std::uint8_t, 5> gStallPetUsedCF400C {}; // 原全局：0x00CF400C。

static WindowGraphicData gWindowGraphic57B288 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B288。

static WindowGraphicData gWindowGraphic57B2A0 {98891, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2A0。

static WindowGraphicData gWindowGraphic57B2B8 {98888, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2B8。

static WindowGraphicData gWindowGraphic57B2D0 {98885, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2D0。

static WindowGraphicData gWindowGraphic57B2E8 {98870, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2E8。

static WindowGraphicData gWindowGraphic57B300 {98882, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B300。

static WindowGraphicData gWindowGraphic57B318 {98873, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B318。

static WindowGraphicData gWindowGraphic57B360 {98859, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B360。

static WindowGraphicData gWindowGraphic57B378 {98861, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B378。

static WindowGraphicData gWindowGraphic57B390 {98854, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B390。

static WindowGraphicData gWindowGraphic57B3A8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3A8。

static WindowGraphicData gWindowGraphic57B3C0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3C0。

static WindowGraphicData gWindowGraphic57B3D8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3D8。

static WindowGraphicData gWindowGraphic57B3F0 {98864, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3F0。

static WindowGraphicData gWindowGraphic57B408 {98867, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B408。

static WindowTextPrototype gWindowText57B420 { // 原全局：0x0057B420。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}},
    L"ˋˋˋˋ",
};

static WindowTextPrototype gWindowText57B428 { // 原全局：0x0057B428。
    {std::byte{0x8C}, std::byte{4}, std::byte{0}, std::byte{0}},
    L"请输入店名八个字",
};

static WindowTextPrototype gWindowText57B430 { // 原全局：0x0057B430。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}},
    L"请输入店铺描述",
};

static WindowNumberData gWindowNumber57B450 {&gStallMoneyText57B440, 0, 3, 1, 0}; // 原全局：0x0057B450。

static std::int32_t gWindowAnimationImage57B458 = 100000; // 原全局：0x0057B458。

static std::uint32_t gWindowMouseFlagsD67A10 = 0; // 原全局：0x00D67A10。

static WindowElementPrototype gWindowElements57B508[48] { // 原全局：0x0057B508。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay, -1, static_cast<ElementId>(WindowStallElementId::DrawTooltipOverlay)},
    {WindowElementType_Input, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePriceInput, -1, static_cast<ElementId>(WindowStallElementId::HandlePriceInput)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose, -1, static_cast<ElementId>(WindowStallElementId::ButtonClose)},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element4)},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element5)},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element6)},
    {WindowElementType_Input, 230, 11, 120, 16, 1, {.unusedValue = 0}, &WindowStall_EditName, -1, static_cast<ElementId>(WindowStallElementId::EditName)},
    {WindowElementType_Text, 298, 51, 0, 0, 1, {.text = &gWindowText57B430}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element8)},
    {WindowElementType_Input, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowStall_EditDescription, -1, static_cast<ElementId>(WindowStallElementId::EditDescription)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element10)},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings, -1, static_cast<ElementId>(WindowStallElementId::ScrollListings)},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates, -1, static_cast<ElementId>(WindowStallElementId::ScrollPetCandidates)},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow)},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_14)},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_15)},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_16)},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_17)},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_18)},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_19)},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_20)},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_21)},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId::HandleItemRow_22)},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory, -1, static_cast<ElementId>(WindowStallElementId::HandleInventory)},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId::HandlePetRow)},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId::HandlePetRow_25)},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId::HandlePetRow_26)},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId::HandlePetCandidate)},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId::HandlePetCandidate_28)},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId::HandlePetCandidate_29)},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId::HandlePetCandidate_30)},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2A0}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId::HandleButtons)},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2B8}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId::HandleButtons_32)},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2E8}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId::HandleButtons_33)},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId::HandleButtons_34)},
    {WindowElementType_Graphic, 322, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B318}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId::HandleButtons_35)},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory, -1, static_cast<ElementId>(WindowStallElementId::TabCategory)},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory, -1, static_cast<ElementId>(WindowStallElementId::TabCategory_37)},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element38)},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowStallElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing, -1, static_cast<ElementId>(WindowStallElementId::ButtonPreviousListing)},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing, -1, static_cast<ElementId>(WindowStallElementId::ButtonNextListing)},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element42)},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowStallElementId::HandleVerticalScroll_43)},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet, -1, static_cast<ElementId>(WindowStallElementId::ButtonPreviousPet)},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet, -1, static_cast<ElementId>(WindowStallElementId::ButtonNextPet)},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element46)},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element47)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId::Element48)},
};

static WindowElementPrototype gWindowElements57BB08[43] { // 原全局：0x0057BB08。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::DrawTooltipOverlay)},
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePurchaseConfirmation, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePurchaseConfirmation)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ButtonClose)},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element4)},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element5)},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element6)},
    {WindowElementType_EventArea, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element7)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element8)},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ScrollListings)},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ScrollPetCandidates)},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow)},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_12)},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_13)},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_14)},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_15)},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_16)},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_17)},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_18)},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_19)},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleItemRow_20)},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleInventory)},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetRow)},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetRow_23)},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetRow_24)},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetCandidate)},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetCandidate_26)},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetCandidate_27)},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandlePetCandidate_28)},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2D0}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleButtons)},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleButtons_30)},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::TabCategory)},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::TabCategory_32)},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element33)},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleVerticalScroll)},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ButtonPreviousListing)},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ButtonNextListing)},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element37)},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::HandleVerticalScroll_38)},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ButtonPreviousPet)},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::ButtonNextPet)},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element41)},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element42)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStallElementId_gWindowElements57BB08::Element43)},
};

// 原型：窗口67；原全局：0x0054EF58。
WindowPrototype gWindowPrototypeOwnStall {0, 4, 10, 30, 549, 366, 0x80000080u, 48, gWindowElements57B508, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}; // 67: 54EF58

// 原型：窗口68；原全局：0x0054EF88。
WindowPrototype gWindowPrototypeStallBrowse {0, 4, 10, 30, 549, 366, 0x80000080u, 43, gWindowElements57BB08, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}; // 68: 54EF88

// 窗口函数实现。

 // 原全局：0x00D046D8。

// 原函数：0x004CDE40。
static std::int32_t WindowStall_ButtonPreviousListing(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetPageD67BD0 > 0) {
                --gStallPetPageD67BD0;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t page = gStallPetPageD67BD0;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = gStallPetCountD67BD4 - 3;
            auto* const indexedElement = window->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, page));
            }
        } else {
            if (gStallItemPageD67BC8 > 0) {
                --gStallItemPageD67BC8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t page = gStallItemPageD67BC8;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = gStallItemCountD67BCC - 10;
            auto* const indexedElement = window->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, page));
            }
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

// 原函数：0x004CDFA0。
static std::int32_t WindowStall_ButtonNextListing(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (gStallModeD67BDC == 1) {
            const std::int32_t maximum = gStallPetCountD67BD4 - 3;
            const std::int32_t page = gStallPetPageD67BD0;
            if (page < maximum) {
                gStallPetPageD67BD0 = page + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t current = gStallPetPageD67BD0;
            const std::int32_t limit = gStallPetCountD67BD4 - 3;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(
                    indexedElement, limit, current));
            }
        } else {
            const std::int32_t maximum = gStallItemCountD67BCC - 10;
            const std::int32_t page = gStallItemPageD67BC8;
            if (page < maximum) {
                gStallItemPageD67BC8 = page + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t current = gStallItemPageD67BC8;
            const std::int32_t limit = gStallItemCountD67BCC - 10;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(
                    indexedElement, limit, current));
            }
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

// 原函数：0x004CE0F0。
static std::int32_t WindowStall_ScrollListings(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t mode = gStallModeD67BDC;
        if (mode == 1) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t maximum = gStallPetCountD67BD4 - 3;
            const std::int32_t page = gStallPetPageD67BD0;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                gStallPetPageD67BD0 = WindowScroll_ApplyWheel(
                    indexedElement, page, maximum, wheel);
            }
        } else {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t maximum = gStallItemCountD67BCC - 10;
            const std::int32_t page = gStallItemPageD67BC8;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey((own ? 38 : 33)));
            if (indexedElement) {
                gStallItemPageD67BC8 = WindowScroll_ApplyWheel(
                    indexedElement, page, maximum, wheel);
            }
        }
    }
    return 0;
}

// 原函数：0x004CE1F0。
static std::int32_t WindowStall_ButtonPreviousPet(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetCandidatePageD67BD8 > 0) {
                --gStallPetCandidatePageD67BD8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            auto* elementsWindow = currentWindow;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey((own ? 42 : 37)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, 1, page));
            }
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

// 原函数：0x004CE2B0。
static std::int32_t WindowStall_ButtonNextPet(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetCandidatePageD67BD8 < 1) {
                ++gStallPetCandidatePageD67BD8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            auto* elementsWindow = currentWindow;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey((own ? 42 : 37)));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, 1, page));
            }
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

// 原函数：0x004CE370。
static std::int32_t WindowStall_ScrollPetCandidates(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t mode = gStallModeD67BDC;
        if (mode == 1) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            WindowInstance* const window = currentWindow;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            auto* const indexedElement = window->FindElementById(NativeElementKey((own ? 42 : 37)));
            if (indexedElement) {
                gStallPetCandidatePageD67BD8 =
                    WindowScroll_ApplyWheel(indexedElement, page, 1, wheel);
            }
        }
    }
    return 0;
}

// 原函数：0x004CE3F0。
static std::int32_t WindowStall_TabCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    const bool ownAtEntry = gStallOwn58111C != 0;
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t category = index;
    if (ownAtEntry) {
        if (elementId == static_cast<ElementId>(WindowStallElementId::TabCategory)) {
            category = 0;
        } else if (elementId == static_cast<ElementId>(WindowStallElementId::TabCategory_37)) {
            category = 1;
        }
    } else if (elementId == static_cast<ElementId>(WindowStallElementId::HandleButtons)) {
        category = 0;
    } else if (elementId == static_cast<ElementId>(WindowStallElementId::HandleButtons_32)) {
        category = 1;
    }
    std::int32_t consumed = 0;
    if (category == gStallModeD67BDC) {
        if (graphic) graphic->imageNo = kStallTabImages54EF40[category][0];
    } else {
        if ((state & MouseFlag_Hover) != 0) {
            if (graphic) graphic->imageNo = kStallTabImages54EF40[category][2];
            consumed = 1;
        } else {
            if (graphic) graphic->imageNo = kStallTabImages54EF40[category][1];
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const bool own = gStallOwn58111C != 0;
            gStallModeD67BDC = category;
            gStallItemSelection581124 = -1;
            if (category == 1) {
                const std::int32_t page = gStallPetPageD67BD0;
                WindowInstance* const window = currentWindow;
                const std::int32_t maximum = gStallPetCountD67BD4 - 3;
                auto* const indexedElement = window->FindElementById(NativeElementKey((own ? 38 : 33)));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, page));
                }
            } else {
                const std::int32_t page = gStallItemPageD67BC8;
                WindowInstance* const window = currentWindow;
                const std::int32_t maximum = gStallItemCountD67BCC - 10;
                auto* const indexedElement = window->FindElementById(NativeElementKey((own ? 38 : 33)));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, page));
                }
            }
            consumed = 1;
        }
    }
    const bool own = gStallOwn58111C != 0;
    const std::int32_t visible = gStallModeD67BDC == 1 ? 1 : 0;
    if (own) {
        constexpr std::array<std::int32_t, 7> slots {41, 42, 43, 44, 11, 45, 46};
        for (const std::int32_t slot : slots) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(slot)); indexedElement) indexedElement->visible = visible;
        }
    } else {
        constexpr std::array<std::int32_t, 7> slots {36, 37, 38, 39, 9, 40, 41};
        for (const std::int32_t slot : slots) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(slot)); indexedElement) indexedElement->visible = visible;
        }
    }
    return consumed;
}

// 原函数：0x004D27C0。
static std::int32_t WindowStall_AddListing(const std::int32_t value)
{
    const std::int32_t mode = gStallModeD67BDC;
    if (mode == 0) {
        const std::int32_t count = gStallItemCountD67BCC;
        const std::int32_t slot = gStallItemSelection581124;
        gStallItemSelection581124 = -1;
        gStallItemOffersD671D8[count].slot = slot;
        gStallItemOffersD671D8[count].value = value;
        gStallItemUsedCC31AC[slot] = 1;
        gStallItemCountD67BCC = count + 1;
        return count + 1;
    }
    if (mode == 1) {
        const std::int32_t count = gStallPetCountD67BD4;
        const std::int32_t slot = gStallPetSelection581128;
        gStallPetSelection581128 = -1;
        gStallPetOffersCC3E98[count].slot = slot;
        gStallPetOffersCC3E98[count].value = value;
        gStallPetUsedCF400C[slot] = 1;
        gStallPetCountD67BD4 = count + 1;
        return count + 1;
    }
    return mode - 1;
}

// 原函数：0x004CE8A0。
static network::SendResult WindowStall_RequestClose()
{
    return cg::network::GetGameConnection().CloseStallRequest();
}

// 原函数：0x004CED50。
static WindowInstance* WindowStall_ShowPurchaseConfirmation(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return nullptr;
    WindowInstance* const window = currentWindow;
    if (auto* const handlePriceInputElement = window->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 1;
    return window;
}

// 原函数：0x004CFC40。
static WindowElement* WindowStall_EditItemPrice(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return nullptr;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    gStallPriceD67C40 = 0;
    static_cast<void>(WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.stallPricePrototype, 0, -100, 0));
    input::SetContext_44B330(*currentWindow->ownerSession, &inputs.stallPrice);
    WindowInstance* const window = currentWindow;
    auto* const handlePriceInputElement = window->FindElementById(WindowStallElementId::HandlePriceInput);
    if (handlePriceInputElement) {
        currentWindow->ownerSession->UiFrame().native.pointer.focusedElementData = handlePriceInputElement->data.input;
    }
    if (auto* const handlePriceInputElement2 = window->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement2) handlePriceInputElement2->data.input->context = &inputs.stallPrice;
    auto* elementsWindow = currentWindow;
    if (auto* const handlePriceInputElement3 = elementsWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement3) handlePriceInputElement3->visible = 1;
    return elementsWindow->FindElementById(WindowStallElementId::DrawTooltipOverlay);
}

// 原函数：0x004CFCA0。
static void WindowStall_EditPetPrice(WindowInstance* callbackWindow, const std::int32_t petIndex)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    const std::int32_t itemSlots = pet.values6D8[0];
    for (std::int32_t slot = 0; slot < 6; ++slot) {
        if (slot < itemSlots && pet.items6E4[slot].used != 0) {
            world::AppendChatMessage_44B050(*currentWindow->ownerSession, L"请卸下宠物身上的装备后再操作。", 4, 3);
            return;
        }
    }
    gStallPriceD67C40 = 0;
    static_cast<void>(WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.stallPricePrototype, 0, -100, 0));
    input::SetContext_44B330(*currentWindow->ownerSession, &inputs.stallPrice);
    WindowInstance* const window = currentWindow;
    auto* const handlePriceInputElement = window->FindElementById(WindowStallElementId::HandlePriceInput);
    if (handlePriceInputElement) {
        currentWindow->ownerSession->UiFrame().native.pointer.focusedElementData = handlePriceInputElement->data.input;
    }
    if (auto* const handlePriceInputElement2 = window->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement2) handlePriceInputElement2->data.input->context = &inputs.stallPrice;
    if (auto* const handlePriceInputElement3 = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement3) handlePriceInputElement3->visible = 1;
}

// 原函数：0x004CFD70。
static std::int32_t WindowStall_RemoveListing()
{
    const std::int32_t mode = gStallModeD67BDC;
    if (mode == 1) {
        const std::int32_t index = gStallPetSelection581128 - 5;
        const std::int32_t count = gStallPetCountD67BD4 - 1;
        StallOffer* const offer = gStallPetOffersCC3E98.data() + index;
        gStallPetUsedCF400C[offer->slot] = 0;
        gStallPetCountD67BD4 = count;
        if (count > 0 && index < count) {
            std::memmove(offer, offer + 1,
                static_cast<std::size_t>(count - index) * sizeof(StallOffer));
        }
        const std::int32_t page = gStallPetPageD67BD0;
        gStallPetSelection581128 = -1;
        const std::int32_t remaining = count - page;
        if (remaining < 3 && page > 0) {
            gStallPetPageD67BD0 = page - 1;
        }
        return remaining;
    }
    if (mode == 0) {
        const std::int32_t index = gStallItemSelection581124 - 20;
        const std::int32_t count = gStallItemCountD67BCC - 1;
        StallOffer* const offer = gStallItemOffersD671D8.data() + index;
        gStallItemUsedCC31AC[offer->slot] = 0;
        gStallItemCountD67BCC = count;
        if (count > 0 && index < count) {
            std::memmove(offer, offer + 1,
                static_cast<std::size_t>(count - index) * sizeof(StallOffer));
        }
        const std::int32_t page = gStallItemPageD67BC8;
        gStallItemSelection581124 = -1;
        const std::int32_t remaining = count - page;
        if (remaining < 10 && page > 0) {
            gStallItemPageD67BC8 = page - 1;
        }
        return remaining;
    }
    return mode - 1;
}

// 原函数：0x004CF940。
static std::int32_t WindowStall_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    bool disabled = false;
    std::int32_t imageGroup = state;
    if (gStallOwn58111C != 0) {
        switch (static_cast<WindowStallElementId>(elementId)) {
        case WindowStallElementId::HandleButtons:
            imageGroup = 0;
            if ((gStallItemCountD67BCC <= 0 && gStallPetCountD67BD4 <= 0)
                || gStallPendingD67C31 != 0) {
                disabled = true;
            } else if ((state & MouseFlag_LDownEvent) != 0) {
                const std::uint8_t gate = gOwnStallOpenForBusiness_D67BE0;
                gStallPendingD67C31 = 1;
                gOwnStallOpenForBusiness_D67BE0 = gate == 0 ? 1 : 0;
                gStallItemSelection581124 = -1;
                gStallPetSelection581128 = -1;
                static_cast<void>(WindowStall_SubmitListings(&callbackWindow));
            }
            break;
        case WindowStallElementId::HandleButtons_32:
            imageGroup = 1;
            if (gStallPendingD67C31 != 0) {
                disabled = true;
            } else if ((state & MouseFlag_LDownEvent) != 0) {
                gStallPendingD67C31 = 1;
                gStallItemSelection581124 = -1;
                gStallPetSelection581128 = -1;
                static_cast<void>(WindowStall_RequestClose());
            }
            break;
        case WindowStallElementId::HandleButtons_33: {
            imageGroup = 3;
            const std::int32_t mode = gStallModeD67BDC;
            if (mode == 0) {
                const std::int32_t selected = gStallItemSelection581124;
                if (selected < 20 || selected >= 40) {
                    disabled = true;
                } else if ((state & MouseFlag_LDownEvent) != 0) {
                    static_cast<void>(WindowStall_RemoveListing());
                }
            } else if (mode == 1) {
                const std::int32_t selected = gStallPetSelection581128;
                if (selected < 5 || selected >= 10) {
                    disabled = true;
                } else if ((state & MouseFlag_LDownEvent) != 0) {
                    static_cast<void>(WindowStall_RemoveListing());
                }
            }
            break;
        }
        case WindowStallElementId::HandleButtons_34:
            imageGroup = 4;
            if (gOwnStallOpenForBusiness_D67BE0 != 0) {
                disabled = true;
            } else if ((state & MouseFlag_LDownEvent) != 0) {
                window->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
            break;
        case WindowStallElementId::HandleButtons_35: {
            imageGroup = 5;
            const std::int32_t mode = gStallModeD67BDC;
            if (mode == 0) {
                const std::int32_t selected = gStallItemSelection581124;
                if (selected < 0 || selected >= 20) {
                    disabled = true;
                } else if (gStallItemUsedCC31AC[selected] == 0 && (state & MouseFlag_LDownEvent) != 0) {
                    static_cast<void>(WindowStall_EditItemPrice(&callbackWindow));
                }
            } else if (mode == 1) {
                const std::int32_t selected = gStallPetSelection581128;
                if (selected < 0 || selected >= 5) {
                    disabled = true;
                } else if (gStallPetUsedCF400C[selected] == 0 && (state & MouseFlag_LDownEvent) != 0) {
                    WindowStall_EditPetPrice(&callbackWindow, selected);
                }
            }
            break;
        }
        default:
            break;
        }
    } else if (elementId == static_cast<ElementId>(WindowStallElementId::HandlePetCandidate_29)) {
        imageGroup = 2;
        if (gStallPendingD67C31 != 0) {
            disabled = true;
        } else {
            const std::int32_t itemCount = gStallItemCountD67BCC;
            const std::int32_t mode = gStallModeD67BDC;
            if (itemCount <= 0 && mode == 0) {
                disabled = true;
            } else {
                const std::int32_t petCount = gStallPetCountD67BD4;
                if (petCount <= 0 && mode == 1) {
                    disabled = true;
                } else if ((state & MouseFlag_LDownEvent) != 0) {
                    static_cast<void>(WindowStall_ShowPurchaseConfirmation(&callbackWindow));
                }
            }
        }
    } else if (elementId == static_cast<ElementId>(WindowStallElementId::HandlePetCandidate_30)) {
        imageGroup = 4;
        if (gStallPendingD67C31 != 0) {
            disabled = true;
        } else if ((state & MouseFlag_LDownEvent) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    if (disabled) {
        if (graphic) graphic->imageNo = kStallButtonImages54EEE0[imageGroup][1];
        gStallButtonStateD67C44 = state;
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = kStallButtonImages54EEE0[imageGroup][2];
        consumed = 1;
    } else {
        if (graphic) graphic->imageNo = kStallButtonImages54EEE0[imageGroup][0];
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = kStallButtonImages54EEE0[imageGroup][1];
        consumed = 1;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        input::InputContext* const context = callbackWindow.ownerSession->UiFrame().inputState.currentContext;
        if (context == &inputs.stallName || context == &inputs.stallDescription) {
            WindowChat_RestoreInput(*callbackWindow.ownerSession);
        }
        gStallButtonStateD67C44 = state;
        return 1;
    }
    gStallButtonStateD67C44 = state;
    return consumed;
}

// 原函数：0x004CFE70。
static std::int32_t WindowStall_SubmitListings(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    std::wstring buffer;
    std::wstring name = L"无名店铺";
    std::wstring description = L"老板没什么可介绍的";
    std::wstring offers;
    std::int32_t count = gStallItemCountD67BCC;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::int32_t index = 0;
        if (count > 0) {
            const StallOffer* offer = gStallItemOffersD671D8.data();
            while (offer->slot != slot) {
                ++index;
                ++offer;
                if (index >= count) {
                    break;
                }
            }
            if (index < count) {
                cg::text::FormatTo(buffer, L"{}|", gStallItemOffersD671D8[index].value);
                offers += buffer;
                count = gStallItemCountD67BCC;
            }
        }
        if (index == count) {
            offers += L"0|";
        }
    }
    count = gStallPetCountD67BD4;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        std::int32_t index = 0;
        if (count > 0) {
            const StallOffer* offer = gStallPetOffersCC3E98.data();
            while (offer->slot != slot) {
                ++index;
                ++offer;
                if (index >= count) {
                    break;
                }
            }
            if (index < count) {
                cg::text::FormatTo(buffer, L"{}|", gStallPetOffersCC3E98[index].value);
                offers += buffer;
                count = gStallPetCountD67BD4;
            }
        }
        if (index == count) {
            offers += L"0|";
        }
    }
    offers = input::EscapeTextAndNotify(offers, 275);
    if (!gStallNameD67BE4.empty()) { name = gStallNameD67BE4; }
    name = input::EscapeTextAndNotify(name, 275);
    // 原发布字段最多32/112字节；截断仅作用于此次协议字段。
    const auto nameBytes = cg::text::ToGbk(name);
    name = cg::text::FromGbkWide(nameBytes.substr(0, 32));
    if (!gStallDescriptionD67BF8.empty()) { description = gStallDescriptionD67BF8; }
    description = input::EscapeTextAndNotify(description, 275);
    const auto descriptionBytes = cg::text::ToGbk(description);
    description = cg::text::FromGbkWide(descriptionBytes.substr(0, 112));
    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
    static_cast<void>(cg::network::GetGameConnection().PublishStallRequest(name, description, offers));
    if (!inputs.stallNamePrototype.input.context->empty()) {
        login::detail::GetCurrentSavedStallNameFF4D18() = name;
    }
    if (!inputs.stallDescriptionPrototype.input.context->empty()) {
        login::detail::GetCurrentSavedStallDescriptionFF4D28() = description;
    }
    gStallMessageC8A978 = L"摆摊请求中";
    WindowInstance* const window = currentWindow;
    gStallMessageModeD67C34 = 0;
    gStallMessageValueD67C38 = 0;
    if (auto* const drawTooltipOverlayElement = window->FindElementById(WindowStallElementId::DrawTooltipOverlay); drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 1;
    return 0;
}

// 原函数：0x004D0230。
static std::int32_t WindowStall_DrawTooltipOverlay(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t mode = gStallMessageModeD67C34;
    if (mode != 0 && mode != 1) {
        return state & MouseFlag_Hover;
    }
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const std::int32_t width = cg::render::TextMeasure(gStallMessageC8A978, 0);
    WindowInstance* window = currentWindow;
    const auto textY = static_cast<std::int16_t>(window->y + (window->height - 16) / 2);
    const auto textX = static_cast<std::int16_t>(window->x + (window->width - width) / 2);
    static_cast<void>(render::DrawText_456780(
        textX, textY, 4, 0, 0, gStallMessageC8A978, 0, 0));
    if (mode == 1) {
        window = currentWindow;
        const std::int32_t centerY = window->height / 2 + window->y;
        const std::int32_t centerX = window->x + window->width / 2;
        const bool hit = cg::ui::HitTestMouseRect_5049A0(
            centerX - 33, centerY + 18, centerX + 33, centerY + 35, -1) != 0;
        window = currentWindow;
        const std::int32_t buttonY = window->height / 2 + window->y + 27;
        const std::int32_t buttonX = window->x + window->width / 2;
        static_cast<void>(cg::render::DrawGraphic(
            buttonX, buttonY, 227, hit ? 243050 : 243048, 0, &descriptor));
        if (hit && (state & MouseFlag_LDownEvent) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
        }
    }
    window = currentWindow;
    const auto backgroundY = static_cast<std::int16_t>(window->y + (window->height - 92) / 2);
    const auto backgroundX = static_cast<std::int16_t>(window->x + (window->width - 220) / 2);
    cg::yobi::DrawNineGrid_49BE00(
        245300, backgroundX, backgroundY, 220, 92, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    return state & MouseFlag_Hover;
}

// 原函数：0x004D0990。
static std::int32_t WindowStall_DrawPetSkills(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t petIndex, const std::int32_t skillPosition)
{
    render::GraphicDescriptor16 descriptor {};
    const auto& pet = gStallBrowseData->pets[petIndex];
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
            static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY = static_cast<std::int16_t>(y);
    const std::int32_t shortX = static_cast<std::int16_t>(x);
    return cg::render::DrawGraphic(shortX + 105, shortY + 8, 230, 243100, 0, &descriptor);
}

// 原函数：0x004D0A80。
static std::int32_t WindowStall_DrawPetDetails(
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const auto byteIndex = static_cast<std::uint8_t>(index);
    const std::int32_t shortIndex = std::bit_cast<std::int8_t>(byteIndex);
    if (shortIndex < 0 || shortIndex > 5) {
        return byteIndex;
    }
    const auto& pet = gStallBrowseData->pets[shortIndex];
    if (pet.used == 0) {
        return shortIndex;
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
            static_cast<std::int16_t>(x + 60), static_cast<std::int16_t>(y + 11),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY = static_cast<std::int16_t>(y);
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        cg::text::FormatTo(buffer, L"{:10}", pet.values70[5 + row]);
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 144), static_cast<std::int16_t>(shortY + 23 + row * 15),
            5, 12, 0, buffer, 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = cg::yobi::NewYobiAction_404AE0(2, 0);
    }
    const std::int32_t imageNo = pet.imageNo6A0;
    cg::yobi::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(gPreviewActionD67B84, 100, 0, 0));
    cg::yobi::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX = static_cast<std::int16_t>(x);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
    const render::GraphicDescriptor16* const mode = cg::yobi::GetYobiActionDrawMode(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 230, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t segments = pet.values70[21 + row] / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 230, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 230, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 230, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 230, images[3], 0, 1, 0, &descriptor));
        }
    }
    const std::int32_t skillPage = WindowPetSkills_GetPreviewPage(&pet);
    for (std::int32_t row = 0; row < 10; ++row) {
        static_cast<void>(WindowStall_DrawPetSkills(shortX + 10, shortY + 141 + row * 16, shortIndex, skillPage * 10 + row));
    }
    static_cast<void>(render::DrawText_456780(static_cast<std::int16_t>(shortX + 10), static_cast<std::int16_t>(shortY + 303), 5, 12, 0,
        skillPage == 0 ? L"第1/2页 按PageDown键看下一页" : L"第2/2页 按PageUp键看上一页", 0, 0));
    return render::DrawGraphic3_520970(shortX, shortY, 230, 244373, 0, 1, 0, &descriptor);
}

// 原函数：0x004D0EB0。
static std::int32_t WindowStall_HandlePetRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gStallModeD67BDC != 1) {
        return 0;
    }
    const bool own = gStallOwn58111C != 0;
    const std::int32_t row = index - (own ? 23 : 21);
    const std::int32_t position = gStallPetPageD67BD0 + row;
    if (position >= gStallPetCountD67BD4) {
        return 0;
    }
    world::PetRuntimeRecord* const pet = own
        ? &world::gPetRuntimeRecords[gStallPetOffersCC3E98[position].slot]
        : &gStallBrowseData->pets[position];
    if (pet->used == 0) {
        return 0;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", pet->name6AC.data());
    {
        WindowInstance* const window = currentWindow;
        const auto x = static_cast<std::int16_t>(window->x + 77);
        const auto y = static_cast<std::int16_t>(window->y + 91 * row + 58);
        static_cast<void>(render::DrawText_456780(
            x, y, 3, 13, std::bit_cast<std::int8_t>(std::uint8_t{138}), buffer, 0, 0));
    }
    const auto drawText = [&](const std::int32_t dx, const std::int32_t dy) {
        WindowInstance* const window = currentWindow;
        const auto y = static_cast<std::int16_t>(window->y + 91 * row + dy);
        const auto x = static_cast<std::int16_t>(window->x + dx);
        static_cast<void>(render::DrawText_456780(
            x, y, 3, 13, std::bit_cast<std::int8_t>(std::uint8_t{138}), buffer, 0, 0));
    };
    cg::text::FormatTo(buffer, L"{:4}", static_cast<std::int32_t>(pet->values0C[1]));
    drawText(227, 58);
    cg::text::FormatTo(buffer, L"{:3}", pet->value08);
    drawText(53, 75);
    const std::int16_t tribe = std::bit_cast<std::int16_t>(pet->value04);
    if (tribe >= 0 && tribe < 20) {
        cg::text::FormatTo(buffer, L"{}", gTribeNames57CB54[tribe]);
    } else {
        buffer.clear();
    }
    drawText(120, 75);
    cg::text::FormatTo(buffer, L"{:4}", static_cast<std::int32_t>(pet->values0C[3]));
    drawText(227, 75);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[0]);
    drawText(117, 93);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[1]);
    drawText(176, 93);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[2]);
    drawText(234, 93);
    cg::text::FormatTo(buffer, L"{:3}", pet->value6A4);
    drawText(53, 111);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[3]);
    drawText(117, 111);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[4]);
    drawText(176, 111);
    cg::text::FormatTo(buffer, L"{:3}", pet->values70[10]);
    drawText(234, 111);
    cg::text::FormatTo(buffer, L"{:3}", pet->lvFlag6C);
    drawText(91, 129);
    std::int32_t consumed = 0;
    if (gOwnStallOpenForBusiness_D67BE0 != 0 && gStallOwn58111C != 0) {
        render::GraphicDescriptor16 descriptor {};
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        const std::uint32_t color = 0x80808080u;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            static_cast<void>(render::DrawGraphic3_520970(
                x, y, 223, mode <= 1 ? 98849 : 98850, 0, 1, 0, &descriptor));
        }
    } else {
        if ((state & MouseFlag_Hover) != 0) {
            WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                    static_cast<std::uint16_t>(x + 240), static_cast<std::uint16_t>(y + 90),
                    223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            gStallPetSelection581128 = gStallPetPageD67BD0 + row + 5;
        }
        const bool currentOwn = gStallOwn58111C != 0;
        if ((state & MouseFlag_LDblClickEvent) != 0) {
            if (currentOwn) {
                static_cast<void>(WindowStall_RemoveListing());
            } else {
                static_cast<void>(WindowStall_ShowPurchaseConfirmation(&callbackWindow));
            }
        }
        const std::int32_t current = gStallPetPageD67BD0 + row;
        if (current == gStallPetSelection581128 - 5) {
            WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                    static_cast<std::uint16_t>(x + 238), static_cast<std::uint16_t>(y + 88),
                    223, 254, 0);
            }
        }
    }
    const bool priceOwn = gStallOwn58111C != 0;
    std::int32_t price;
    if (priceOwn) {
        cg::text::FormatTo(buffer, L"{}", gStallPetOffersCC3E98[gStallPetPageD67BD0 + row].value);
        price = gStallPetOffersCC3E98[gStallPetPageD67BD0 + row].value;
    } else {
        cg::text::FormatTo(buffer, L"{}", gStallBrowseData->petValuesD2D464[gStallPetPageD67BD0 + row]);
        price = gStallBrowseData->petValuesD2D464[gStallPetPageD67BD0 + row];
    }
    std::int8_t priceColor = 0;
    if (price >= 10000) {
        if (price < 100000) {
            priceColor = 5;
        } else if (price < 1000000) {
            priceColor = 1;
        } else if (price < 10000000) {
            priceColor = 4;
        }
    }
    {
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y + 91 * row + 128;
        const std::int32_t x = window->x + 243;
        WindowNumber_Draw(x, y, buffer, 0, priceColor, 1, 223);
    }
    if ((state & MouseFlag_Hover) != 0) {
        WindowInstance* const window = currentWindow;
        const std::int32_t windowX = window->x;
        const std::int32_t y = window->y + 20;
        const std::int32_t x = windowX + 510 > 640 ? windowX - 230 : windowX + 280;
        if (priceOwn) {
            const auto petIndex = static_cast<std::uint8_t>(
                gStallPetOffersCC3E98[gStallPetPageD67BD0 + row].slot);
            static_cast<void>(WindowTrade_DrawOwnPetTooltip(x, y, petIndex));
        } else {
            const auto petIndex = static_cast<std::uint8_t>(gStallPetPageD67BD0 + row);
            static_cast<void>(WindowStall_DrawPetDetails(x, y, petIndex));
        }
    }
    return consumed;
}

// 原函数：0x004D1C00。
static std::int32_t WindowStall_DrawCandidateAttributes(
    const std::int32_t petIndex, const std::int32_t attribute,
    const std::int32_t x, const std::int32_t y)
{
    const std::int32_t segments = world::gPetRuntimeRecords[petIndex].values70[21 + attribute] / 10;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    // 原5817BC与57E434静态图号同值，不合并原数据地址身份。
    if (segments == 1) {
        return render::DrawGraphic3_520970(
            x, y, 223, gPetBarImages57E434[attribute][3], 0, 1, 0, &descriptor);
    }
    if (segments > 1) {
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 223, gPetBarImages57E434[attribute][0], 0, 1, 0, &descriptor));
        std::int32_t middleX = x + 6;
        for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
            static_cast<void>(render::DrawGraphic3_520970(
                middleX, y, 223, gPetBarImages57E434[attribute][1], 0, 1, 0, &descriptor));
            middleX += 6;
        }
        return render::DrawGraphic3_520970(
            x + 6 * segments - 6, y, 223, gPetBarImages57E434[attribute][2], 0, 1, 0, &descriptor);
    }
    return 0;
}

// 原函数：0x004D16F0。
static std::int32_t WindowStall_HandlePetCandidate(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gStallModeD67BDC != 1) {
        return 0;
    }
    const std::int32_t row = index - (gStallOwn58111C != 0 ? 26 : 24);
    const auto* const initialPet = &world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row];
    if (initialPet->used == 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            gStallPetSelection581128 = -1;
        }
        return 0;
    }
    std::wstring buffer;
    const auto drawText = [&](const std::int32_t dx, const std::int32_t dy) {
        WindowInstance* const window = currentWindow;
        const auto y = static_cast<std::int16_t>(window->y + row * 50 + dy);
        const auto x = static_cast<std::int16_t>(window->x + dx);
        static_cast<void>(render::DrawText_456780(x, y, 3, 12, 0, buffer, 0, 0));
    };
    cg::text::FormatTo(buffer, L"{}", initialPet->name6AC.data());
    drawText(298, 132);
    cg::text::FormatTo(buffer, L"{:3}", world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].value08);
    drawText(425, 132);
    const std::int32_t maxHp = static_cast<std::int32_t>(world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[1]);
    const std::int32_t hp = static_cast<std::int32_t>(world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[0]);
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawText(333, 146);
    const std::int32_t maxFp = static_cast<std::int32_t>(world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[3]);
    const std::int32_t fp = static_cast<std::int32_t>(world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[2]);
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawText(333, 160);
    const std::int16_t tribe = std::bit_cast<std::int16_t>(
        world::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].value04);
    if (tribe >= 0 && tribe < 20) {
        cg::text::FormatTo(buffer, L"{}", gTribeNames57CB54[tribe]);
    } else {
        buffer.clear();
    }
    drawText(396, 146);
    constexpr std::array<std::array<std::int32_t, 2>, 4> barPositions {{
        {163, 5}, {163, 15}, {163, 25}, {163, 35},
    }};
    for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y + barPositions[attribute][1] + 50 * row + 127;
        const std::int32_t x = window->x + barPositions[attribute][0] + 292;
        const std::int32_t petIndex = gStallPetCandidatePageD67BD8 + row;
        static_cast<void>(WindowStall_DrawCandidateAttributes(petIndex, attribute, x, y));
    }
    std::int32_t consumed = 0;
    if (gOwnStallOpenForBusiness_D67BE0 == 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t position = gStallPetCandidatePageD67BD8 + row;
        bool offered = gStallPetUsedCF400C[position] != 0;
        if (own && (state & MouseFlag_LDownEvent) != 0 && offered) {
            gStallPetSelection581128 = -1;
            offered = gStallPetUsedCF400C[position] != 0;
        }
        if (!offered && own) {
            if ((state & MouseFlag_Hover) != 0) {
                WindowInstance* const window = currentWindow;
                auto* const elementPtr = window->FindElementById(elementId);
                if (elementPtr) {
                    const WindowElement& element = (*elementPtr);
                    const std::int32_t y = window->y + element.y;
                    const std::int32_t x = window->x + element.x;
                    cg::render::DrawGraphicHover(
                        static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                        static_cast<std::uint16_t>(x + 231), static_cast<std::uint16_t>(y + 48),
                        223, cg::render::gHoverFramePaletteIndex_D69768, 0);
                }
                consumed = 1;
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                gStallPetSelection581128 = gStallPetCandidatePageD67BD8 + row;
            }
            if ((state & MouseFlag_LDblClickEvent) != 0) {
                WindowStall_EditPetPrice(&callbackWindow, gStallPetSelection581128);
            }
        }
        if ((offered || own) && gStallOwn58111C != 0
            && gStallPetCandidatePageD67BD8 + row == gStallPetSelection581128) {
            WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                    static_cast<std::uint16_t>(x + 229), static_cast<std::uint16_t>(y + 46),
                    223, 254, 0);
            }
        }
    }
    if ((state & MouseFlag_Hover) != 0) {
        WindowInstance* const window = currentWindow;
        std::int32_t x = window->x + 20;
        const std::int32_t y = window->y + 20;
        if (x < 0) {
            x += window->width;
        }
        const auto petIndex = static_cast<std::uint8_t>(gStallPetCandidatePageD67BD8 + row);
        static_cast<void>(WindowTrade_DrawOwnPetTooltip(x, y, petIndex));
    }
    if (gStallOwn58111C != 0 && gStallPetUsedCF400C[gStallPetCandidatePageD67BD8 + row] != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 229), static_cast<std::uint16_t>(y + 46),
                223, 256, 0);
        }
    }
    return consumed;
}

// 原函数：0x004D04A0。
static std::int32_t WindowStall_HandleItemRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gOwnStallOpenForBusiness_D67BE0 != 0) {
        const std::int32_t petCount = gStallPetCountD67BD4;
        const std::int32_t itemCount = gStallItemCountD67BCC;
        if (petCount + itemCount <= 0) {
            static_cast<void>(WindowStallBrowse_RequestClose(*callbackWindow.ownerSession));
            static_cast<void>(WindowOwnStall_RequestClose(*callbackWindow.ownerSession));
        }
    }
    if (gStallModeD67BDC != 0) {
        return 0;
    }
    const bool own = gStallOwn58111C != 0;
    const std::int32_t row = index - (own ? 12 : 10);
    const std::int32_t position = gStallItemPageD67BC8 + row;
    if (position >= gStallItemCountD67BCC) {
        return 0;
    }
    world::PlayerItemRecord* item;
    if (own) {
        const std::int32_t slot = gStallItemOffersD671D8[position].slot;
        item = &world::gPlayerRuntimeSlots.items[slot + 8];
        if (item->used == 0) {
            return 0;
        }
        if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
            gStallHover581120 = slot + 8;
        }
    } else {
        item = &gStallBrowseData->items[position];
        if (item->used == 0) {
            return 0;
        }
        if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
            gStallHover581120 = position + 200;
        }
    }
    if ((state & MouseFlag_RDownEvent) != 0) {
        ++gStallDescriptionPageD67C3C;
        if (gStallDescriptionPageD67C3C >= item->pages) {
            gStallDescriptionPageD67C3C = 0;
        }
    }
    std::wstring buffer;
    for (std::int32_t line = 0; line < 2; ++line) {
        std::wstring nameLine;
        static_cast<void>(input::ExtractTextLine(nameLine, item->name, line, 10));
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const auto y = static_cast<std::int16_t>(window->y + element.y + 5 + line * 14);
            const auto x = static_cast<std::int16_t>(window->x + element.x + 52);
            static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, nameLine, 0, 0));
        }
    }
    std::int32_t consumed = 0;
    if (gOwnStallOpenForBusiness_D67BE0 != 0 && gStallOwn58111C != 0) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        WindowInstance* const window = currentWindow;
        render::GraphicDescriptor16 descriptor {};
        const std::uint32_t color = 0x80808080u;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            static_cast<void>(render::DrawGraphic3_520970(
                x, y, 223, mode <= 1 ? 98847 : 98848, 0, 1, 0, &descriptor));
        }
    } else {
        if ((state & MouseFlag_Hover) != 0) {
            WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                    static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                    223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            gStallItemSelection581124 = gStallItemPageD67BC8 + row + 20;
        }
        const bool currentOwn = gStallOwn58111C != 0;
        if ((state & MouseFlag_LDblClickEvent) != 0) {
            if (currentOwn) {
                static_cast<void>(WindowStall_RemoveListing());
            } else {
                static_cast<void>(WindowStall_ShowPurchaseConfirmation(&callbackWindow));
            }
        }
        const std::int32_t current = gStallItemPageD67BC8 + row;
        if (current == gStallItemSelection581124 - 20) {
            WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                    static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
                    223, 254, 0);
            }
        }
    }
    std::int32_t price;
    if (gStallOwn58111C != 0) {
        price = gStallItemOffersD671D8[gStallItemPageD67BC8 + row].value;
    } else {
        price = gStallBrowseData->itemValuesD13E74[gStallItemPageD67BC8 + row];
    }
    cg::text::FormatTo(buffer, L"{}", price);
    std::int8_t priceColor = 0;
    if (price >= 10000) {
        if (price < 100000) {
            priceColor = 5;
        } else if (price < 1000000) {
            priceColor = 1;
        } else if (price < 10000000) {
            priceColor = 4;
        }
    }
    {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t y = element.y + window->y + 35;
            const std::int32_t x = element.x + window->x + 105;
            WindowNumber_Draw(x, y, buffer, 0, priceColor, 1, 223);
        }
    }
    const std::int32_t count = item->count;
    if (count > 0) {
        std::wstring countBuffer;
        cg::text::FormatTo(countBuffer, L"{:3}", count);
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const auto y = static_cast<std::int16_t>(window->y + element.y + 32);
            const auto x = static_cast<std::int16_t>(window->x + element.x + 21);
            static_cast<void>(render::DrawText_456780(x, y, 3, 1, 1, countBuffer, 0, 0));
        }
    }
    WindowInstance* const window = currentWindow;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement& element = (*elementPtr);
        const std::int32_t imageNo = item->imageNo;
        const std::int32_t y = element.y + window->y + 24;
        const std::int32_t x = element.x + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x004D1D50。
static std::int32_t WindowStall_HandleInventory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gStallModeD67BDC != 0) {
        return 0;
    }
    std::int32_t hovered = -1;
    std::wstring buffer;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        auto* const item = &world::gPlayerRuntimeSlots.items[slot + 8];
        if (item->used == 0) {
            continue;
        }
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x + 50 * (slot % 5);
            const std::int32_t y = window->y + element.y + 50 * (slot / 5);
            render::GraphicDescriptor16 descriptor {};
            const std::uint32_t color = 0xFFFFFFFFu;
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            if (gStallOwn58111C != 0 && gStallItemUsedCC31AC[slot] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                    static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
                    223, 256, 0);
            }
            if (hovered < 0 && (state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0
                && cg::ui::HitTestMouseRect_5049A0(x, y, x + 48, y + 48, -1) != 0) {
                hovered = slot;
                if (gStallInventoryHover5817FC != slot) {
                    gStallDescriptionPageD67C3C = 0;
                }
                const std::uint8_t gate = gOwnStallOpenForBusiness_D67BE0;
                gStallInventoryHover5817FC = slot;
                if (gate == 0) {
                    if (gStallOwn58111C != 0 && (state & MouseFlag_LDownEvent) != 0
                        && (gStallItemUsedCC31AC[slot] != 0 || (item->flags & 0x60u) != 0)) {
                        gStallItemSelection581124 = -1;
                    }
                    if (gStallItemUsedCC31AC[slot] == 0 && (item->flags & 0x60u) == 0) {
                        if ((state & MouseFlag_Hover) != 0) {
                            cg::render::DrawGraphicHover(
                                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
                        }
                        if (gStallOwn58111C != 0) {
                            if ((state & MouseFlag_LDownEvent) != 0) {
                                gStallItemSelection581124 = slot;
                            }
                            if ((state & MouseFlag_LDblClickEvent) != 0) {
                                static_cast<void>(WindowStall_EditItemPrice(&callbackWindow));
                            }
                        }
                    }
                }
                if ((state & MouseFlag_RDownEvent) != 0) {
                    const std::int32_t page = gStallDescriptionPageD67C3C + 1;
                    const std::int32_t pages = item->pages;
                    gStallDescriptionPageD67C3C = page;
                    if (page >= pages) {
                        gStallDescriptionPageD67C3C = 0;
                    }
                }
                gStallHover581120 = slot + 8;
            }
            if (gStallOwn58111C != 0 && gOwnStallOpenForBusiness_D67BE0 == 0 && slot == gStallItemSelection581124) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                    static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
                    223, 254, 0);
            }
            const std::int32_t count = item->count;
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x + 21), static_cast<std::int16_t>(y + 32),
                    3, 1, 1, buffer, 0, 0));
            }
            if (gStallOwn58111C != 0 && (item->flags & 0x60u) != 0) {
                static_cast<void>(cg::render::DrawGraphic(x + 24, y + 24, 223, 23040, 0, &descriptor));
            }
            static_cast<void>(cg::render::DrawGraphic(x + 24, y + 24, 223, item->imageNo, 0, &descriptor));
        }
    }
    return (state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0 ? 1 : 0;
}

// 原函数：0x004D2260。
static std::int32_t WindowStall_HandlePriceInput(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const bool clicked = (state & MouseFlag_LDownEvent) != 0;
    if (clicked) {
        static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.stallPricePrototype, 0, -100, 0));
        input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.stallPrice);
        WindowInstance* const window = currentWindow;
        auto* const handlePriceInputElement = window->FindElementById(WindowStallElementId::HandlePriceInput);
        if (handlePriceInputElement) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handlePriceInputElement->data.input;
        }
        if (auto* const handlePriceInputElement2 = window->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement2) handlePriceInputElement2->data.input->context = &inputs.stallPrice;
    }
    if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.stallPrice) {
        for (std::int32_t digit = 0; digit < 10; ++digit) {
            if (platform::ReadKeyFlag4_4597A0(digit + 48) != 0
                || platform::ReadKeyFlag4_4597A0(digit + 96) != 0) {
                gStallPriceD67C40 = digit + 10 * gStallPriceD67C40;
            }
            if (gStallPriceD67C40 > 9999999) {
                gStallPriceD67C40 = 9999999;
            }
        }
        if (platform::ReadKeyFlag4_4597A0(8) != 0) {
            gStallPriceD67C40 /= 10;
        }
    }
    constexpr std::array<std::array<std::int32_t, 2>, 10> positions {{
        {18, 104}, {18, 84}, {42, 84}, {65, 84}, {18, 64},
        {42, 64}, {65, 64}, {18, 44}, {42, 44}, {65, 44},
    }};
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y + 115;
    const std::int32_t x = window->x + 206;
    if (cg::ui::HitTestMouseRect_5049A0(x + 103, y + 6, x + 118, y + 19, -1) != 0) {
        if (clicked) {
            if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.stallPrice) {
                WindowChat_RestoreInput(*callbackWindow.ownerSession);
            }
            if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                x + 106, y + 9, 227, 243002, 0, 1, 0, &descriptor));
        }
    } else {
        static_cast<void>(render::DrawGraphic3_520970(
            x + 106, y + 9, 227, 243000, 0, 1, 0, &descriptor));
    }
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        const std::int32_t top = y + positions[digit][1];
        const std::int32_t left = x + positions[digit][0];
        std::int32_t imageState = 0;
        if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 21, top + 19, -1) != 0) {
            if (clicked) {
                gStallPriceD67C40 = digit + 10 * gStallPriceD67C40;
            }
            if (gStallPriceD67C40 > 9999999) {
                gStallPriceD67C40 = 9999999;
            }
            imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
        }
        if (imageState >= 0) {
            static_cast<void>(cg::render::DrawGraphic(
                left + 10, top + 10, 227, gNumericKeyImages54F030[digit][imageState], 0, &descriptor));
        }
    }
    std::int32_t imageState = 0;
    if (cg::ui::HitTestMouseRect_5049A0(x + 93, y + 64, x + 118, y + 83, -1) != 0) {
        if (clicked) {
            gStallPriceD67C40 /= 10;
        }
        imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
    }
    if (imageState >= 0) {
        static_cast<void>(cg::render::DrawGraphic(
            x + 103, y + 74, 227, gNumericKeyImages54F030[10][imageState], 0, &descriptor));
    }
    imageState = 0;
    if (cg::ui::HitTestMouseRect_5049A0(x + 93, y + 104, x + 118, y + 123, -1) != 0) {
        if (clicked) {
            gStallPriceD67C40 = 0;
        }
        imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
    }
    if (imageState >= 0) {
        static_cast<void>(cg::render::DrawGraphic(
            x + 103, y + 114, 227, gNumericKeyImages54F030[12][imageState], 0, &descriptor));
    }
    imageState = 1;
    if (gStallPriceD67C40 != 0) {
        if (cg::ui::HitTestMouseRect_5049A0(x + 65, y + 104, x + 87, y + 123, -1) != 0) {
            if (clicked) {
                static_cast<void>(WindowStall_AddListing(gStallPriceD67C40));
                if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.stallPrice) {
                    WindowChat_RestoreInput(*callbackWindow.ownerSession);
                }
                if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
            }
            imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
        } else {
            imageState = 0;
        }
    }
    if (imageState >= 0) {
        static_cast<void>(cg::render::DrawGraphic(
            x + 75, y + 114, 227, gNumericKeyImages54F030[13][imageState], 0, &descriptor));
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", gStallPriceD67C40);
    WindowNumber_Draw(x + 105, y + 29, buffer, 3, 0, 1, 227);
    static_cast<void>(render::DrawGraphic3_520970(x, y, 226, 98855, 0, 1, 0, &descriptor));
    return state & MouseFlag_Hover;
}

// 原函数：0x004D2840。
static std::int32_t WindowStall_HandlePurchaseConfirmation(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    std::int32_t y = window->y + 183;
    std::int32_t x = window->x + 274;
    const bool petLayout = gStallModeD67BDC != 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t buttonY;
    std::int32_t acceptX;
    std::int32_t cancelX;
    if (petLayout) {
        x -= 144;
        y -= 99;
        acceptX = 143;
        buttonY = 162;
        cancelX = 205;
    } else {
        x -= 83;
        y -= 79;
        acceptX = 26;
        buttonY = 120;
        cancelX = 88;
    }
    const std::int32_t acceptTop = y + buttonY;
    const std::int32_t acceptLeft = x + acceptX;
    std::int32_t imageState = 0;
    if (cg::ui::HitTestMouseRect_5049A0(
        acceptLeft, acceptTop, acceptLeft + 49, acceptTop + 17, -1) != 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(WindowStall_RequestPurchase(&callbackWindow));
            if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
        }
        imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
    }
    if (imageState >= 0) {
        static_cast<void>(render::DrawGraphic3_520970(
            acceptLeft, acceptTop, 227, kStallPurchaseButtonImages54EF28[0][imageState],
            0, 1, 0, &descriptor));
    }
    const std::int32_t cancelTop = y + buttonY;
    const std::int32_t cancelLeft = x + cancelX;
    imageState = 0;
    if (cg::ui::HitTestMouseRect_5049A0(
        cancelLeft, cancelTop, cancelLeft + 49, cancelTop + 17, -1) != 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
        }
        imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : ((state & MouseFlag_Hover) != 0 ? 2 : -1);
    }
    if (imageState >= 0) {
        static_cast<void>(render::DrawGraphic3_520970(
            cancelLeft, cancelTop, 227, kStallPurchaseButtonImages54EF28[1][imageState],
            0, 1, 0, &descriptor));
    }
    std::wstring buffer;
    std::wstring details;
    if (gStallModeD67BDC == 0) {
        auto* const item = gStallBrowseData->items.data() + (gStallItemSelection581124 - 20);
        if (cg::ui::HitTestMouseRect_5049A0(x + 22, y + 58, x + 70, y + 106, -1) != 0) {
            if ((state & MouseFlag_Hover) != 0) {
                gStallHover581120 = gStallItemSelection581124 + 180;
            }
            if ((state & MouseFlag_RDownEvent) != 0) {
                ++gStallDescriptionPageD67C3C;
                if (gStallDescriptionPageD67C3C >= item->pages) {
                    gStallDescriptionPageD67C3C = 0;
                }
            }
        }
        if (cg::ui::HitTestMouseRect_5049A0(x + 133, y + 6, x + 148, y + 19, -1) != 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
            } else {
                static_cast<void>(render::DrawGraphic3_520970(
                    x + 136, y + 9, 227, 243002, 0, 1, 0, &descriptor));
            }
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                x + 136, y + 9, 227, 243000, 0, 1, 0, &descriptor));
        }
        std::wstring nameLine;
        static_cast<void>(input::ExtractTextLine(nameLine, item->name, 0, 10));
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 74), static_cast<std::int16_t>(y + 58),
            4, 8, 0, nameLine, 0, 0));
        static_cast<void>(input::ExtractTextLine(nameLine, item->name, 1, 10));
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 74), static_cast<std::int16_t>(y + 72),
            4, 8, 0, nameLine, 0, 0));
        static_cast<void>(cg::render::DrawGraphic(x + 46, y + 82, 227, item->imageNo, 0, &descriptor));
        cg::text::FormatTo(buffer, L"{}", gStallBrowseData->itemValuesD13E74[gStallItemSelection581124 - 20]);
        WindowNumber_Draw(x + 127, y + 93, buffer, 3, 0, 1, 227);
        const std::int32_t count = item->count;
        if (count > 0) {
            cg::text::FormatTo(details, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(x + 43), static_cast<std::int16_t>(y + 90),
                4, 1, 1, details, 0, 0));
        }
        static_cast<void>(render::DrawGraphic3_520970(x, y, 226, 98856, 0, 1, 0, &descriptor));
    } else {
        if (cg::ui::HitTestMouseRect_5049A0(x + 254, y + 6, x + 269, y + 19, -1) != 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                if (auto* const handlePriceInputElement = currentWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
            } else {
                static_cast<void>(render::DrawGraphic3_520970(
                    x + 257, y + 9, 227, 243002, 0, 1, 0, &descriptor));
            }
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                x + 257, y + 9, 227, 243000, 0, 1, 0, &descriptor));
        }
        auto* const pet = gStallBrowseData->pets.data() + (gStallPetSelection581128 - 5);
        const auto drawPetText = [&](const std::int32_t dx, const std::int32_t dy) {
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(x + dx), static_cast<std::int16_t>(y + dy),
                4, 13, std::bit_cast<std::int8_t>(std::uint8_t{138}), details, 0, 0));
        };
        cg::text::FormatTo(details, L"{}", pet->name6AC.data());
        drawPetText(75, 61);
        cg::text::FormatTo(details, L"{:4}", static_cast<std::int32_t>(pet->values0C[1]));
        drawPetText(225, 61);
        cg::text::FormatTo(details, L"{:3}", pet->value08);
        drawPetText(51, 78);
        const std::int16_t tribe = std::bit_cast<std::int16_t>(pet->value04);
        if (tribe >= 0 && tribe < 20) {
            cg::text::FormatTo(details, L"{}", gTribeNames57CB54[tribe]);
        } else {
            details.clear();
        }
        drawPetText(118, 78);
        cg::text::FormatTo(details, L"{:4}", static_cast<std::int32_t>(pet->values0C[3]));
        drawPetText(225, 78);
        cg::text::FormatTo(details, L"{:3}", pet->values70[0]);
        drawPetText(115, 96);
        cg::text::FormatTo(details, L"{:3}", pet->values70[1]);
        drawPetText(174, 96);
        cg::text::FormatTo(details, L"{:3}", pet->values70[2]);
        drawPetText(232, 96);
        cg::text::FormatTo(details, L"{:3}", pet->value6A4);
        drawPetText(51, 114);
        cg::text::FormatTo(details, L"{:3}", pet->values70[3]);
        drawPetText(115, 114);
        cg::text::FormatTo(details, L"{:3}", pet->values70[4]);
        drawPetText(174, 114);
        cg::text::FormatTo(details, L"{:3}", pet->values70[10]);
        drawPetText(232, 114);
        cg::text::FormatTo(details, L"{:3}", pet->lvFlag6C);
        drawPetText(89, 132);
        cg::text::FormatTo(details, L"{}", gStallBrowseData->petValuesD2D464[gStallPetSelection581128 - 5]);
        WindowNumber_Draw(x + 241, y + 131, details, 3, 0, 1, 227);
        static_cast<void>(render::DrawGraphic3_520970(x, y, 226, 98860, 0, 1, 0, &descriptor));
    }
    return state & MouseFlag_Hover;
}

// 原函数：0x004CD4F0。
static std::int32_t WindowStall_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    if (value == -1) {
        WindowInstance* const initial = activeWindow;
        const std::int32_t id = initial->id;
        gStallItemPageD67BC8 = 0;
        if (id == 67) {
            gStallItemCountD67BCC = 0;
            gStallPetPageD67BD0 = 0;
            gStallPetCountD67BD4 = 0;
            gStallPetCandidatePageD67BD8 = 0;
            gStallModeD67BDC = 0;
            gStallOwn58111C = 1;
        } else {
            gStallPetPageD67BD0 = 0;
            gStallPetCandidatePageD67BD8 = 0;
            gStallModeD67BDC = 0;
            gStallOwn58111C = 0;
        }
        gStallHover581120 = -1;
        gStallItemSelection581124 = -1;
        gStallPetSelection581128 = -1;
        std::memset(gStallItemUsedCC31AC.data(), 0, 4);
        std::memset(gStallItemUsedCC31AC.data() + 4, 0, 4);
        std::memset(gStallPetUsedCF400C.data(), 0, 4);
        std::memset(gStallItemUsedCC31AC.data() + 8, 0, 4);
        gOwnStallOpenForBusiness_D67BE0 = 0;
        std::memset(gStallItemUsedCC31AC.data() + 12, 0, 4);
        gStallDescriptionPageD67C3C = 0;
        gStallNameD67BE4.clear();
        gStallDescriptionD67BF8.clear();
        std::memset(gStallItemUsedCC31AC.data() + 16, 0, 4);
        gStallPetUsedCF400C[4] = 0;
        if (auto* const drawTooltipOverlayElement = initial->FindElementById(WindowStallElementId::DrawTooltipOverlay); drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 0;
        if (auto* const handlePriceInputElement = activeWindow->FindElementById(WindowStallElementId::HandlePriceInput); handlePriceInputElement) handlePriceInputElement->visible = 0;
    }
    std::wstring lineText;
    if (gStallOwn58111C != 0) {
        const bool showScroll = gStallModeD67BDC == 1
            ? gStallPetCountD67BD4 > 3 : gStallItemCountD67BCC > 10;
        if (auto* const element38 = activeWindow->FindElementById(WindowStallElementId::Element38); element38) element38->visible = showScroll ? 1 : 0;
        auto* elementsWindow = activeWindow;
        auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowStallElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
                if (gStallModeD67BDC == 1) {
                    auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowStallElementId::HandleVerticalScroll);
                    if (handleVerticalScrollElement2) {
                        gStallPetPageD67BD0 = WindowScroll_ReadPosition(handleVerticalScrollElement2, gStallPetCountD67BD4 - 3);
                    }
                } else {
                    auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowStallElementId::HandleVerticalScroll);
                    if (handleVerticalScrollElement2) {
                        gStallItemPageD67BC8 = WindowScroll_ReadPosition(handleVerticalScrollElement2, gStallItemCountD67BCC - 10);
                    }
                }
            }
        }
        elementsWindow = activeWindow;
        auto* const handleVerticalScroll_43Element = elementsWindow->FindElementById(WindowStallElementId::HandleVerticalScroll_43);
        if (handleVerticalScroll_43Element) {
            if ((handleVerticalScroll_43Element->data.mouse->flags & 1u) != 0
                && gStallModeD67BDC == 1) {
                auto* const handleVerticalScroll_43Element2 = elementsWindow->FindElementById(WindowStallElementId::HandleVerticalScroll_43);
                if (handleVerticalScroll_43Element2) {
                    gStallPetCandidatePageD67BD8 = WindowScroll_ReadPosition(handleVerticalScroll_43Element2, 1);
                }
            }
        }
        if (auto* const element5 = activeWindow->FindElementById(WindowStallElementId::Element5); element5) element5->data.text->text = world::GetPlayerDisplayText(1);
        if (auto* const element8 = activeWindow->FindElementById(WindowStallElementId::Element8); element8) element8->visible = 0;
        if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.stallDescription
            && gOwnStallOpenForBusiness_D67BE0 == 0 && gStallOwn58111C != 0) {
            WindowInstance* const window = activeWindow;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.stallDescriptionPrototype, window->x, window->y, 1));
            char fieldBytes[57]{};
            cg::text::StoreGbk(fieldBytes, sizeof(fieldBytes), input::ContextText(*inputs.stallDescriptionPrototype.input.context));
            gStallDescriptionD67BF8 = input::ContextText(*inputs.stallDescriptionPrototype.input.context);
        } else if (!gStallDescriptionD67BF8.empty()) {
            for (std::int32_t line = 0; line < 3; ++line) {
                static_cast<void>(input::ExtractTextLine(lineText, gStallDescriptionD67BF8, line, 28));
                WindowInstance* const window = activeWindow;
                auto* currentWindow = window;
                auto* const editDescriptionElement = currentWindow->FindElementById(WindowStallElementId::EditDescription);
                if (editDescriptionElement) {
                    const std::int16_t y = std::bit_cast<std::int16_t>(
                        static_cast<std::uint16_t>(editDescriptionElement->y + window->y + line * 14));
                    auto* const editDescriptionElement2 = currentWindow->FindElementById(WindowStallElementId::EditDescription);
                    if (editDescriptionElement2) {
                        const std::int16_t x = std::bit_cast<std::int16_t>(
                            static_cast<std::uint16_t>(editDescriptionElement2->x + window->x));
                        static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, lineText, 0, 0));
                    }
                }
            }
        } else {
            const auto& source = login::detail::GetCurrentSavedStallDescriptionFF4D28();
            WindowInstance* const window = activeWindow;
            gStallDescriptionD67BF8 = source;
            if (auto* const element82 = window->FindElementById(WindowStallElementId::Element8); element82) element82->visible = 1;
        }
        if (auto* const element6 = activeWindow->FindElementById(WindowStallElementId::Element6); element6) element6->visible = 0;
        if (callbackWindow.ownerSession->UiFrame().inputState.currentContext == &inputs.stallName
            && gOwnStallOpenForBusiness_D67BE0 == 0 && gStallOwn58111C != 0) {
            WindowInstance* const window = activeWindow;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.stallNamePrototype, window->x, window->y, 1));
            char fieldBytes[17]{};
            cg::text::StoreGbk(fieldBytes, sizeof(fieldBytes), input::ContextText(*inputs.stallNamePrototype.input.context));
            gStallNameD67BE4 = input::ContextText(*inputs.stallNamePrototype.input.context);
        } else if (!gStallNameD67BE4.empty()) {
            WindowInstance* const window = activeWindow;
            auto* currentWindow = window;
            auto* const editNameElement = currentWindow->FindElementById(WindowStallElementId::EditName);
            if (editNameElement) {
                const std::int16_t y = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(editNameElement->y + window->y));
                auto* const editNameElement2 = currentWindow->FindElementById(WindowStallElementId::EditName);
                if (editNameElement2) {
                    const std::int16_t x = std::bit_cast<std::int16_t>(
                        static_cast<std::uint16_t>(editNameElement2->x + window->x));
                    static_cast<void>(render::DrawText_456780(x, y, 3, 4,
                        std::bit_cast<std::int8_t>(std::uint8_t{0x8C}), gStallNameD67BE4, 0, 0));
                }
            }
        } else {
            const auto& source = login::detail::GetCurrentSavedStallNameFF4D18();
            WindowInstance* const window = activeWindow;
            gStallNameD67BE4 = source;
            if (auto* const element62 = window->FindElementById(WindowStallElementId::Element6); element62) element62->visible = 1;
        }
        const std::uint8_t gate = gOwnStallOpenForBusiness_D67BE0;
        gStallHover581120 = -1;
        if (gate != 0) {
            if (auto* const handleButtonsElement = activeWindow->FindElementById(WindowStallElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = 0;
            if (auto* const handleButtons_32Element = activeWindow->FindElementById(WindowStallElementId::HandleButtons_32); handleButtons_32Element) handleButtons_32Element->visible = 1;
        } else {
            if (auto* const handleButtonsElement = activeWindow->FindElementById(WindowStallElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = 1;
            if (auto* const handleButtons_32Element = activeWindow->FindElementById(WindowStallElementId::HandleButtons_32); handleButtons_32Element) handleButtons_32Element->visible = 0;
        }
    } else {
        const bool showScroll = gStallModeD67BDC == 1
            ? gStallPetCountD67BD4 > 3 : gStallItemCountD67BCC > 10;
        if (auto* const handleButtons_33Element = activeWindow->FindElementById(WindowStallElementId::HandleButtons_33); handleButtons_33Element) handleButtons_33Element->visible = showScroll ? 1 : 0;
        auto* elementsWindow = activeWindow;
        auto* const handleButtons_34Element = elementsWindow->FindElementById(WindowStallElementId::HandleButtons_34);
        if (handleButtons_34Element) {
            if ((handleButtons_34Element->data.mouse->flags & 1u) != 0) {
                if (gStallModeD67BDC == 1) {
                    auto* const handleButtons_34Element2 = elementsWindow->FindElementById(WindowStallElementId::HandleButtons_34);
                    if (handleButtons_34Element2) {
                        gStallPetPageD67BD0 = WindowScroll_ReadPosition(handleButtons_34Element2, gStallPetCountD67BD4 - 3);
                    }
                } else {
                    auto* const handleButtons_34Element2 = elementsWindow->FindElementById(WindowStallElementId::HandleButtons_34);
                    if (handleButtons_34Element2) {
                        gStallItemPageD67BC8 = WindowScroll_ReadPosition(handleButtons_34Element2, gStallItemCountD67BCC - 10);
                    }
                }
            }
        }
        elementsWindow = activeWindow;
        auto* const element38 = elementsWindow->FindElementById(WindowStallElementId::Element38);
        if (element38) {
            if ((element38->data.mouse->flags & 1u) != 0
                && gStallModeD67BDC == 1) {
                auto* const element382 = elementsWindow->FindElementById(WindowStallElementId::Element38);
                if (element382) {
                    gStallPetCandidatePageD67BD8 = WindowScroll_ReadPosition(element382, 1);
                }
            }
        }
        if (auto* const element5 = activeWindow->FindElementById(WindowStallElementId::Element5); element5) element5->data.text->text = gStallBrowseData->sellerName00;
        if (auto* const element6 = activeWindow->FindElementById(WindowStallElementId::Element6); element6) element6->data.text->text = gStallBrowseData->stallName11;
        for (std::int32_t line = 0; line < 3; ++line) {
            static_cast<void>(input::ExtractTextLine(lineText, gStallBrowseData->description22, line, 28));
            WindowInstance* const window = activeWindow;
            auto* currentWindow = window;
            auto* const editNameElement = currentWindow->FindElementById(WindowStallElementId::EditName);
            if (editNameElement) {
                const std::int16_t y = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(editNameElement->y + window->y + line * 14));
                auto* const editNameElement2 = currentWindow->FindElementById(WindowStallElementId::EditName);
                if (editNameElement2) {
                    const std::int16_t x = std::bit_cast<std::int16_t>(
                        static_cast<std::uint16_t>(editNameElement2->x + window->x));
                    static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, lineText, 0, 0));
                }
            }
        }
        gStallHover581120 = -1;
    }
    cg::text::FormatTo(gStallMoneyText57B440, L"{}", world::gPlayerRuntimeSlots.valueFC);
    return 1;
}

// 原函数：0x004D2080。
static std::int32_t WindowStall_EditDescription(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gOwnStallOpenForBusiness_D67BE0 != 0 || gStallOwn58111C == 0) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"请输入描述" /* 原常量参考：0x00586450。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const editDescriptionElement = window->FindElementById(WindowStallElementId::EditDescription);
        if (editDescriptionElement) {
            if (callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData != editDescriptionElement->data.input) {
                inputs.stallDescriptionPrototype.text = gStallDescriptionD67BF8;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.stallDescriptionPrototype, x, y, 0));
                input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.stallDescription);
                WindowInstance* const current = currentWindow;
                auto* const editDescriptionElement2 = current->FindElementById(WindowStallElementId::EditDescription);
                if (editDescriptionElement2) {
                    callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = editDescriptionElement2->data.input;
                }
                auto* const editDescriptionElement3 = current->FindElementById(WindowStallElementId::EditDescription);
                if (editDescriptionElement3) {
                    editDescriptionElement3->data.input->context =
                        &inputs.stallDescription;
                }
            }
        }
        return 1;
    }
    return 0;
}

// 原函数：0x004D2170。
static std::int32_t WindowStall_EditName(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gOwnStallOpenForBusiness_D67BE0 != 0 || gStallOwn58111C == 0) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"请输入描述" /* 原常量参考：0x00586450。 */;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const editNameElement = window->FindElementById(WindowStallElementId::EditName);
        if (editNameElement) {
            if (callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData != editNameElement->data.input) {
                inputs.stallNamePrototype.text = gStallNameD67BE4;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.stallNamePrototype, x, y, 0));
                input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.stallName);
                WindowInstance* const current = currentWindow;
                auto* const editNameElement2 = current->FindElementById(WindowStallElementId::EditName);
                if (editNameElement2) {
                    callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = editNameElement2->data.input;
                }
                auto* const editNameElement3 = current->FindElementById(WindowStallElementId::EditName);
                if (editNameElement3) {
                    editNameElement3->data.input->context =
                        &inputs.stallName;
                }
            }
        }
        return 1;
    }
    return 0;
}

// 原函数：0x004CDB10。
static std::int32_t WindowStall_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (gStallOwn58111C != 0) {
        {
            auto* const element10 = currentWindow->FindElementById(WindowStallElementId::Element10);
            if (element10) {
                auto* const data = element10->data.animation;
                cg::yobi::YobiAction* const source = world::gPlayerRuntimeSlots.action;
                cg::yobi::YobiAction* const target = data->action;
                const std::int32_t value = cg::yobi::GetYobiActionField2A8(source);
                cg::yobi::SetYobiActionField2A8(target, value);
            }
        }
        {
            auto* const element10 = currentWindow->FindElementById(WindowStallElementId::Element10);
            if (element10) {
                auto* const data = element10->data.animation;
                cg::yobi::YobiAction* const source = world::gPlayerRuntimeSlots.action;
                cg::yobi::YobiAction* const target = data->action;
                const std::int32_t value = cg::yobi::GetYobiActionField2B0(source);
                cg::yobi::SetYobiActionField2B0(target, value);
            }
        }
        {
            auto* const element10 = currentWindow->FindElementById(WindowStallElementId::Element10);
            if (element10) {
                auto* const data = element10->data.animation;
                cg::yobi::YobiAction* const source = world::gPlayerRuntimeSlots.action;
                cg::yobi::YobiAction* const target = data->action;
                const std::int32_t value = cg::yobi::GetYobiActionField2B8(source);
                cg::yobi::SetYobiActionField2B8(target, value);
            }
        }
        if (gStallModeD67BDC != 1) {
            const std::int32_t hover = gStallHover581120;
            if (hover >= 0) {
                static_cast<void>(WindowItem_DrawEquipmentTooltip(hover, gStallDescriptionPageD67C3C));
            }
        }
        std::int32_t itemCount = gStallItemCountD67BCC;
        for (std::int32_t index = itemCount - 1; index >= 0; --index) {
            StallOffer* const offer = gStallItemOffersD671D8.data() + index;
            const std::int32_t slot = offer->slot;
            if (world::gPlayerRuntimeSlots.items[slot + 8].used == 0) {
                --itemCount;
                gStallItemUsedCC31AC[slot] = 0;
                gStallItemCountD67BCC = itemCount;
                if (itemCount > 0 && index < itemCount) {
                    std::memmove(offer, offer + 1,
                        static_cast<std::size_t>(itemCount - index) * sizeof(StallOffer));
                }
                const std::int32_t page = gStallItemPageD67BC8;
                gStallItemSelection581124 = -1;
                if (itemCount - page < 10 && page > 0) {
                    gStallItemPageD67BC8 = page - 1;
                }
            }
        }
        std::int32_t petCount = gStallPetCountD67BD4;
        for (std::int32_t index = petCount - 1; index >= 0; --index) {
            StallOffer* const offer = gStallPetOffersCC3E98.data() + index;
            const std::int32_t slot = offer->slot;
            if (world::gPetRuntimeRecords[slot].used == 0) {
                --petCount;
                gStallPetUsedCF400C[slot] = 0;
                gStallPetCountD67BD4 = petCount;
                if (petCount > 0 && index < petCount) {
                    std::memmove(offer, offer + 1,
                        static_cast<std::size_t>(petCount - index) * sizeof(StallOffer));
                }
                const std::int32_t page = gStallPetPageD67BD0;
                gStallPetSelection581128 = -1;
                if (petCount - page < 3 && page > 0) {
                    gStallPetPageD67BD0 = page - 1;
                }
            }
        }
    } else {
        {
            auto* const element8 = currentWindow->FindElementById(WindowStallElementId::Element8);
            if (element8) {
                auto* const data = element8->data.animation;
                const std::int32_t value = gStallBrowseData->imageNo60;
                cg::yobi::SetYobiActionField2A8(data->action, value);
            }
        }
        {
            auto* const element8 = currentWindow->FindElementById(WindowStallElementId::Element8);
            if (element8) {
                auto* const data = element8->data.animation;
                const std::int32_t value = gStallBrowseData->action64;
                cg::yobi::SetYobiActionField2B0(data->action, value);
            }
        }
        {
            auto* const element8 = currentWindow->FindElementById(WindowStallElementId::Element8);
            if (element8) {
                auto* const data = element8->data.animation;
                const std::int32_t value = gStallBrowseData->direction68;
                cg::yobi::SetYobiActionField2B8(data->action, value);
            }
        }
        if (gStallModeD67BDC != 1) {
            const std::int32_t hover = gStallHover581120;
            if (hover >= 0 && hover < 200) {
                static_cast<void>(WindowItem_DrawEquipmentTooltip(hover, gStallDescriptionPageD67C3C));
            } else if (hover >= 200) {
                const auto& item = gStallBrowseData->items[hover - 200];
                const std::uint16_t flags = item.flags;
                const std::wstring title = item.name;
                const std::int8_t color = (flags & 0x100u) != 0
                    ? 5 : static_cast<std::int8_t>((flags & 0x200u) >> 9);
                const std::int32_t page = gStallDescriptionPageD67C3C;
                const std::wstring* const lines = item.description.data() + page * 8;
                WindowInstance* const window = currentWindow;
                const std::int16_t y = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(window->y + 40));
                const std::int16_t x = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(window->x + 320));
                static_cast<void>(WindowItem_DrawTooltip(x, y, title.c_str(), color, lines, 8, nullptr, 0));
            }
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004CED60。
static void WindowStall_OpenBrowse(
    session::ClientSession& session,
    const std::int32_t status, const std::int32_t target,
    const char* const stallName, const char* const description,
    const char* const itemPayload, const char* const petPayload)
{
    gStallItemCountD67BCC = 0;
    gStallPetCountD67BD4 = 0;
    cg::yobi::YobiAction* const action = world::FindObjectYobi_4462B0(target);
    if (status != 0 || action == nullptr) {
        switch (status) {
        case 1:
            world::AppendChatMessage_44B050(session, L"没有可以浏览的摊位", 4, 0);
            break;
        case 2:
            world::AppendChatMessage_44B050(session, L"浏览的摊位已经收摊", 4, 0);
            break;
        case 3:
            world::AppendChatMessage_44B050(session, L"浏览的摊位正在忙，请稍后再试", 4, 0);
            break;
        }
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_StallBrowse].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        return;
    }

    auto& browse = *gStallBrowseData;
    browse.sellerName00.clear();
    browse.stallName11.clear();
    browse.description22.clear();
    browse.bytes5B.fill({});
    browse.target5C = 0;
    browse.imageNo60 = 0;
    browse.action64 = 0;
    browse.direction68 = 0;
    for (auto& item : browse.items) { item = {}; }
    browse.itemValuesD13E74.fill(0);
    browse.itemValuesD13EC4.fill(0);
    for (world::PetRuntimeRecord& pet : browse.pets) {
        pet.ClearRaw();
    }
    browse.petValuesD2D464.fill(0);
    browse.petValuesD2D478.fill(0);
    gStallBrowseData->sellerName00 = cg::text::FromGbkWide(
        cg::text::ToGbk(cg::yobi::GetYobiName(action)).substr(0, 16));
    gStallBrowseData->target5C = target;
    gStallBrowseData->imageNo60 = cg::yobi::GetYobiActionField2A8(action);
    gStallBrowseData->action64 = cg::yobi::GetYobiActionField2B0(action);
    gStallBrowseData->direction68 = cg::yobi::GetYobiActionField2B8(action);
    gStallBrowseData->stallName11 = cg::text::FromGbkWide(std::string_view(stallName, std::min(std::strlen(stallName), std::size_t{16})));
    gStallBrowseData->description22 = cg::text::FromGbkWide(std::string_view(description, std::min(std::strlen(description), std::size_t{56})));

    char itemName[256];
    char itemDescription[768];
    char itemMemo[256];
    const std::int32_t itemCount = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', 1);
    std::int32_t itemField = 4;
    for (std::int32_t index = 0; index < itemCount; ++index, itemField += 15) {
        auto& item = gStallBrowseData->items[gStallItemCountD67BCC];
        ++gStallItemCountD67BCC;
        item = {};
        gStallBrowseData->itemValuesD13E74[index] =
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField - 2);
        gStallBrowseData->itemValuesD13EC4[index] =
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField - 1);
        network::ExtractDelimitedField_52CED0(itemPayload, '|', itemField, 255, itemName);
        input::UnescapeLegacyTextAndNotify_52D2F0(itemName);
        item.used = 1;
        if (std::strlen(itemName) <= 28) {
            item.name = cg::text::FromGbkWide(itemName);
        }
        item.color = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 1);
        if (item.color < 0) {
            item.color = 0;
        }
        network::ExtractDelimitedField_52CED0(itemPayload, '|', itemField + 2, 767, itemDescription);
        input::UnescapeLegacyTextAndNotify_52D2F0(itemDescription);
        WindowStall_BuildItemDescription(&item, itemDescription);
        item.hasDescription = !item.description[0].empty() ? 1 : 0;
        item.imageNo = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 3);
        item.canUseInField = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 4));
        item.battleUseFlag = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 5));
        item.target = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 6));
        item.level = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 7);
        item.flags = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 8));
        item.itemId = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 9);
        item.itemType = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 10);
        item.count = network::ParseDelimitedInteger_52CFA0(itemPayload, '|', itemField + 11);
        network::ExtractDelimitedField_52CED0(itemPayload, '|', itemField + 12, 255, itemMemo);
        input::UnescapeLegacyTextAndNotify_52D2F0(itemMemo);
        if (std::strlen(itemMemo) <= 16) {
            item.memo = cg::text::FromGbkWide(itemMemo);
        }
    }

    char petName[256];
    char petSecondName[256];
    char techniqueName[256];
    char techniqueDescription[256];
    const std::int32_t petCount = network::ParseDelimitedInteger_52CFA0(petPayload, '|', 1);
    std::int32_t field = 2;
    for (std::int32_t index = 0; index < petCount; ++index) {
        auto& pet = gStallBrowseData->pets[gStallPetCountD67BD4];
        ++gStallPetCountD67BD4;
        gStallBrowseData->petValuesD2D464[index] =
            network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        gStallBrowseData->petValuesD2D478[index] =
            network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        ++field; // 原每条相对字段4不解析。
        pet.used = 1;
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.imageNo6A0 = character_select::ResolveCharacterImageForVersion(imageNo);
        pet.value04 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++));
        for (std::int32_t value = 0; value < 6; ++value) {
            const std::int32_t plain = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
            (pet.values0C[value] = static_cast<std::int32_t>(plain));
        }
        pet.value08 = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        for (std::int32_t value = 5; value <= 10; ++value) {
            pet.values70[value] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        }
        for (std::int32_t value = 21; value <= 24; ++value) {
            pet.values70[value] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        }
        pet.value6A4 = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.value02 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++));
        network::ExtractDelimitedField_52CED0(petPayload, '|', field++, 255, petName);
        input::UnescapeLegacyTextAndNotify_52D2F0(petName);
        if (std::strlen(petName) <= 16) {
            pet.name6AC = cg::text::FromGbkWide(petName);
        }
        network::ExtractDelimitedField_52CED0(petPayload, '|', field++, 255, petSecondName);
        input::UnescapeLegacyTextAndNotify_52D2F0(petSecondName);
        if (std::strlen(petSecondName) <= 16) {
            pet.name6BD = cg::text::FromGbkWide(petSecondName);
        }
        const auto flags = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++));
        if ((flags & 0x10u) != 0) {
            pet.follow6D4 = 1;
            pet.flags6AA = static_cast<std::uint16_t>(flags & 0xFFEFu);
        } else {
            pet.flags6AA = flags;
            pet.follow6D4 = 0;
        }
        pet.values70[25] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        ++field; // 原每条相对字段30不解析。
        for (std::int32_t value = 0; value < 5; ++value) {
            pet.values70[value] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        }
        for (std::int32_t value = 17; value <= 20; ++value) {
            pet.values70[value] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        }
        for (std::int32_t value = 11; value <= 16; ++value) {
            pet.values70[value] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        }
        pet.value6D0 = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.values6D8[1] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.values6D8[2] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.values6D8[0] = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.lvFlag6C = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        const std::int32_t techniqueCount = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
        pet.techniquesD8.fill({});
        for (std::int32_t technique = 0; technique < techniqueCount; ++technique) {
            techniqueDescription[0] = '\0';
            std::memset(techniqueDescription + 1, 0, sizeof(techniqueDescription) - 1);
            const std::int32_t slot = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
            network::ExtractDelimitedField_52CED0(petPayload, '|', field++, 255, techniqueName);
            input::UnescapeLegacyTextAndNotify_52D2F0(techniqueName);
            if (slot < 0 || slot >= world::kPetTechniqueSlotCount) {
                continue;
            }
            auto& record = pet.techniquesD8[slot];
            if (std::strlen(techniqueName) == 0) {
                record.name.clear();
                record.description.clear();
                record.fp = 0;
                record.used = 0;
            } else {
                record.used = 1;
                record.techniqueId = 0;
                record.name = cg::text::FromGbkWide(std::strlen(techniqueName) <= 24 ? techniqueName : "???");
                record.description = cg::text::FromGbkWide(std::strlen(techniqueDescription) <= 96 ? techniqueDescription : "???");
                record.fp = 0;
                record.field = 0;
                record.target = 0;
                record.sequence = 0;
            }
        }
        world::BuildPetTechniqueDisplayEntries_5185C0(pet.techniquesD8.data(), pet.techniquePairs650.data(), world::kPetTechniqueSlotCount);
    }
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_StallBrowse, 1, 0, nullptr));
}

// 原函数：0x004CF8A0。
static void WindowStall_BuildItemDescription(world::PlayerItemRecord* const item, const char* const text)
{
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char row[96]; // 原每行最多32B；解析后即转为运行时宽文本。
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            item->description[line] = cg::text::FromGbkWide(row);
            last = line;
        } else {
            item->description[line].clear();
        }
    }
    // 原回扫从最后成功行的前一行开始，不检查最后成功行本身。
    while (last > 0 && item->description[last - 1].empty()) {
        --last;
    }
    item->pages = last < 0 ? 0 : last / 8 + 1;
}

// 原函数：0x004CECB0。
static std::int32_t WindowStall_RequestPurchase(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t mode = gStallModeD67BDC;
    std::int32_t value;
    if (mode == 0) {
        value = gStallBrowseData->itemValuesD13E74[gStallItemSelection581124];
    } else if (mode == 1) {
        value = gStallBrowseData->petValuesD2D464[gStallPetSelection581128];
    } else {
        return mode - 1;
    }
    if (value < 0) {
        return value;
    }
    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
    gStallPendingD67C31 = 1;
    static_cast<void>(cg::network::GetGameConnection().PurchaseStallOffer(value));
    constexpr wchar_t message[] = L"购买请求中";
    const auto length = static_cast<std::int32_t>(std::size(message));
    gStallMessageModeD67C34 = 0;
    gStallMessageValueD67C38 = 0;
    gStallMessageC8A978 = message;
    if (auto* const drawTooltipOverlayElement = currentWindow->FindElementById(WindowStallElementId::DrawTooltipOverlay); drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 1;
    return length;
}

// 原函数：0x004CD420。
static std::int32_t WindowStall_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hovered) {
            uiFrame.hoverText = L"关闭这个视窗。";
        }
        if ((state & MouseFlag_LDownEvent) != 0 && gOwnStallOpenForBusiness_D67BE0 == 0 && gStallPendingD67C31 == 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            consumed = 1;
        }
        graphic->imageNo = 243000;
        if (hovered) {
            graphic->imageNo = 243002;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0 || gOwnStallOpenForBusiness_D67BE0 != 0 || gStallPendingD67C31 != 0) {
            graphic->imageNo = 243001;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004CDE00。
static void WindowStall_Release(WindowInstance& releasedWindow)
{
    if (gStallOwn58111C != 0 && gOwnStallOpenForBusiness_D67BE0 != 0) {
        static_cast<void>(releasedWindow.ownerSession->Connection().CloseStallRequest());
    }
    static_cast<void>(audio::PlaySe_52C450(57, 320));
}

// 原函数：0x004CE700。
static void WindowStall_ReceiveOpenResult(session::ClientSession& session, const std::int32_t result)
{
    if (result != 0) {
        const wchar_t* message;
        switch (result) {
        case 1:
            message = L"已经有人在此摆摊";
            break;
        case 3:
            message = L"两次摆摊时间间隔过短";
            break;
        case 4:
            message = L"这个子星最大摊位数已满";
            break;
        default:
            message = L"摆摊失败";
            break;
        }
        gStallMessageC8A978 = message;
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_OwnStall].window;
        world::ClearPlayerActionFlags(0x1000u);
        gStallMessageModeD67C34 = 1;
        gStallMessageValueD67C38 = 0;
        if (window != nullptr) {
            if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 1;
        }
        gOwnStallOpenForBusiness_D67BE0 = 0;
        gStallPendingD67C31 = 0;
        return;
    }
    std::wstring name = L"无名店铺";
    world::SetPlayerActionFlags(0x1000u);
    if (!gStallNameD67BE4.empty()) { name = gStallNameD67BE4; }
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_OwnStall].window;
    world::gPlayerRuntimeSlots.text15BB8 = name;
    if (window != nullptr) {
        if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 0;
    }
    gStallPendingD67C31 = 0;
}

// 原函数：0x004CE8B0。
static void WindowStall_ReceiveCloseResult(session::ClientSession& session, const std::int32_t result)
{
    if (result != 0) {
        gStallMessageC8A978 = L"收摊失败";
    }
    world::ClearPlayerActionFlags(0x1000u);
    gStallPendingD67C31 = 0;
    gOwnStallOpenForBusiness_D67BE0 = 0;
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_OwnStall].window;
    if (result != 0) {
        gStallMessageModeD67C34 = 1;
        gStallMessageValueD67C38 = 0;
        if (window != nullptr) {
            if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 1;
        }
    } else if (window != nullptr) {
        if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 0;
    }
}

// 原函数：0x004CE930。
static void WindowStall_ReceivePurchaseResult(session::ClientSession& session, const std::int32_t result)
{
    if (result == 0) {
        const std::int32_t mode = gStallModeD67BDC;
        if (mode == 1) {
            const std::int32_t index = gStallPetSelection581128 - 5;
            const std::int32_t count = gStallPetCountD67BD4 - 1;
            gStallPetCountD67BD4 = count;
            if (count > 0 && index < count) {
                const auto remaining = static_cast<std::size_t>(count - index);
                auto* const pets = gStallBrowseData->pets.data();
                std::move(pets + index + 1, pets + index + 1 + remaining, pets + index);
                std::memmove(gStallBrowseData->petValuesD2D464.data() + index,
                    gStallBrowseData->petValuesD2D464.data() + index + 1,
                    remaining * sizeof(std::int32_t));
                std::memmove(gStallBrowseData->petValuesD2D478.data() + index,
                    gStallBrowseData->petValuesD2D478.data() + index + 1,
                    remaining * sizeof(std::int32_t));
            }
            const std::int32_t page = gStallPetPageD67BD0;
            gStallPetSelection581128 = -1;
            if (count - page < 3 && page > 0) {
                gStallPetPageD67BD0 = page - 1;
            }
        } else if (mode == 0) {
            const std::int32_t count = gStallItemCountD67BCC - 1;
            const std::int32_t index = gStallItemSelection581124 - 20;
            gStallItemCountD67BCC = count;
            if (count > 0 && index < count) {
                const auto remaining = static_cast<std::size_t>(count - index);
                auto* const items = gStallBrowseData->items.data();
                for (std::size_t position = 0; position < remaining; ++position) {
                    items[index + position] = items[index + position + 1];
                }
                std::memmove(gStallBrowseData->itemValuesD13E74.data() + index,
                    gStallBrowseData->itemValuesD13E74.data() + index + 1,
                    remaining * sizeof(std::int32_t));
                std::memmove(gStallBrowseData->itemValuesD13EC4.data() + index,
                    gStallBrowseData->itemValuesD13EC4.data() + index + 1,
                    remaining * sizeof(std::int32_t));
            }
            const std::int32_t page = gStallItemPageD67BC8;
            gStallItemSelection581124 = -1;
            if (count - page < 10 && page > 0) {
                gStallItemPageD67BC8 = page - 1;
            }
        }
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_StallBrowse].window;
        if (window != nullptr) {
            if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 0;
        }
        gStallPendingD67C31 = 0;
        return;
    }
    constexpr const wchar_t* messages[] {
        L"没有找到老板",
        L"老板的摊位已经变更",
        L"您的物品空间已满",
        L"您的宠物空间已满",
        L"您的钱不够",
        L"要买的东西已经售出",
        L"老板的钱包已满",
        L"交易失败代码8",
        L"交易失败代码9",
        L"交易失败代码10",
        L"交易失败代码11",
        L"交易失败代码12",
    };
    const wchar_t* const message = result >= 1 && result <= 12
        ? messages[result - 1] : L"购买失败";
    gStallMessageC8A978 = message;
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_StallBrowse].window;
    gStallMessageModeD67C34 = 1;
    gStallMessageValueD67C38 = 0;
    if (window != nullptr) {
        if (auto* const drawTooltipOverlayElement = window ? window->FindElementById(WindowStallElementId::DrawTooltipOverlay) : nullptr; drawTooltipOverlayElement) drawTooltipOverlayElement->visible = 1;
    }
    gStallPendingD67C31 = 0;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E7D0。
void WindowStall_ReceiveOpenReply(cg::network::CgConnection& connection, const std::int32_t result)
{
    detail::WindowStall_ReceiveOpenResult(connection.Owner(), result);
}

// 原函数：0x0050E7E0。
void WindowStall_ReceiveCloseReply(cg::network::CgConnection& connection, const std::int32_t result)
{
    detail::WindowStall_ReceiveCloseResult(connection.Owner(), result);
}

// 原函数：0x0050E7F0。
void WindowStall_ReceiveBrowseReply(cg::network::CgConnection& connection, const std::int32_t status, const std::int32_t target,
    const char* const stallName, const char* const description,
    const char* const itemPayload, const char* const petPayload)
{
    detail::WindowStall_OpenBrowse(connection.Owner(), status, target, stallName, description, itemPayload, petPayload);
}

// 原函数：0x0050E820。
void WindowStall_ReceivePurchaseReply(cg::network::CgConnection& connection, const std::int32_t result)
{
    detail::WindowStall_ReceivePurchaseResult(connection.Owner(), result);
}

// 原函数：0x004CD240；LCI01启动拥有者替代CRT构造／退出登记。
StallBrowsePetValuesStartup::StallBrowsePetValuesStartup()
{
    WindowStall_ConstructBrowsePetValues();
}

void WindowStall_ConstructBrowsePetValues()
{
    // 原0x004CD360／0x004CD2D0：独立浏览区五项各六成员构造，退出自动逆序析构。
    static detail::StallBrowseData storage{};
    detail::gStallBrowseData = &storage;
}

// 原函数：0x004CD3E0。
WindowInstance* WindowStallBrowse_RequestClose(session::ClientSession& session)
{
    WindowInstance* const window = detail::WindowRuntime_States(session)[WindowId_StallBrowse].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

// 原函数：0x004CD400。
WindowInstance* WindowOwnStall_RequestClose(session::ClientSession& session)
{
    WindowInstance* const window = detail::WindowRuntime_States(session)[WindowId_OwnStall].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

} // namespace cg::ui
