#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"


namespace cg::ui {
namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextCB0028 {}; // 原全局：0x00CB0028。

input::InputContext gInputContextCC2378 {}; // 原全局：0x00CC2378。

input::InputContext gInputContextCF4E60 {}; // 原全局：0x00CF4E60。


namespace {

// 原D046D8连续浏览区；仅复用值类型布局，不与玩家或交易区共享存储。
struct StallBrowseData {
    std::array<char, 17> sellerName00; // 原成员偏移：+0x00。
    std::array<char, 17> stallName11; // 原成员偏移：+0x11。
    std::array<char, 57> description22; // 原成员偏移：+0x22。
    std::array<std::byte, 1> bytes5B; // 原成员偏移：+0x5B。
    std::int32_t target5C; // 原成员偏移：+0x5C。 4CED60发布的浏览对象编号，不是地址。
    std::int32_t imageNo60; // 原成员偏移：+0x60。
    std::int32_t action64; // 原成员偏移：+0x64。
    std::int32_t direction68; // 原成员偏移：+0x68。
    std::array<world::detail::PlayerItemRecord, 20> items;
    std::array<std::int32_t, 20> itemValuesD13E74; // 原全局：0x00D13E74。
    std::array<std::int32_t, 20> itemValuesD13EC4; // 原全局：0x00D13EC4。
    std::array<world::detail::PetRuntimeRecord, 5> pets;
    std::array<std::int32_t, 5> petValuesD2D464; // 原全局：0x00D2D464。
    std::array<std::int32_t, 5> petValuesD2D478; // 原全局：0x00D2D478。
};

// 指向原启动入口创建的唯一私有对象；不持有或复制宠物数据。
StallBrowseData* gStallBrowseData = nullptr;

// 自身摊位上架项只引用玩家槽号；不同于浏览摊位的两张并行数值表。
struct StallOffer {
    std::int32_t slot;
    std::int32_t value;
};

} // namespace

// 回调与内部函数前置声明。

// 原函数：0x004CE700。
static void WindowStall_ReceiveOpenResult(std::int32_t result); // 开摊结果：玩家标志、名字及窗口67首元素。

// 原函数：0x004CE8B0。
static void WindowStall_ReceiveCloseResult(std::int32_t result); // 收摊结果：两路均先清玩家标志/等待/门禁。

// 原函数：0x004CE930。
static void WindowStall_ReceivePurchaseResult(std::int32_t result); // 购买结果：浏览记录和两并行表原序搬移，或显示原提示。

// 原函数：0x004CF8A0。
static void WindowStall_BuildItemDescription(world::detail::PlayerItemRecord* item, const char* text); // 固定32行描述及前一行回扫。

// 原函数：0x004CECB0。
static std::int32_t WindowStall_RequestPurchase(); // 选择索引直接查询后发购买请求，返回宽提示长度（含NUL）或原负值。

// 原函数：0x004CD420。
static std::int32_t WindowStall_ButtonClose(std::int32_t index, std::int32_t state); // 两等待门禁、关闭标记和原图形快照。

// 原函数：0x004CDE00。
static void WindowStall_Release(); // 67/68共用释放，条件发送后无条件SE57。

// 原函数：0x004CDB10。
static std::int32_t WindowStall_Draw(std::int32_t unusedValue); // 两类摊位预览、失效上架剔除与提示绘制。

// 原函数：0x004D27C0。
static std::int32_t WindowStall_AddListing(std::int32_t value); // 当前选择追加到自身上架表，先清选择最后发布数量。

// 原函数：0x004CD4F0。
static std::int32_t WindowStall_Update(std::int32_t value); // 共用初始化、双滚动/输入同步、文本绘制及金额尾。

// 原函数：0x004D2080。
static std::int32_t WindowStall_EditDescription(std::int32_t unusedIndex, std::int32_t state); // 固定元素8说明编辑，hover不消费。

// 原函数：0x004D2170。
static std::int32_t WindowStall_EditName(std::int32_t unusedIndex, std::int32_t state); // 固定元素6名称编辑，同源Input对象。

// 原函数：0x004CDE40。
static std::int32_t WindowStall_ButtonPreviousListing(std::int32_t index, std::int32_t state); // 上架列表向前，bit400动作。

// 原函数：0x004CDFA0。
static std::int32_t WindowStall_ButtonNextListing(std::int32_t index, std::int32_t state); // 上架列表向后，音效后重读滚动参数。

// 原函数：0x004CE0F0。
static std::int32_t WindowStall_ScrollListings(std::int32_t unusedIndex, std::int32_t state); // 上架列表滚轮，始终返回0。

// 原函数：0x004CE1F0。
static std::int32_t WindowStall_ButtonPreviousPet(std::int32_t index, std::int32_t state); // 宠物候选向前，非宠物模式仍可消费bit400。

// 原函数：0x004CE2B0。
static std::int32_t WindowStall_ButtonNextPet(std::int32_t index, std::int32_t state); // 宠物候选向后，最大页1。

// 原函数：0x004CE370。
static std::int32_t WindowStall_ScrollPetCandidates(std::int32_t unusedIndex, std::int32_t state); // 宠物候选滚轮，始终返回0。

// 原函数：0x004CE3F0。
static std::int32_t WindowStall_TabCategory(std::int32_t index, std::int32_t state); // 类别页签及七项候选控件原序显隐。

// 原函数：0x004CFE70。
static std::int32_t WindowStall_SubmitListings(); // 槽序上架组包、转义发送及同源保存记录回写，返回0。

// 原函数：0x004D0230。
static std::int32_t WindowStall_DrawTooltipOverlay(std::int32_t index, std::int32_t state); // 共用提示覆盖层，只返回hover位。

// 原函数：0x004CF940。
static std::int32_t WindowStall_HandleButtons(std::int32_t index, std::int32_t state); // 自身/浏览主要按钮，禁用与普通收尾分离。

// 原函数：0x004CE8A0。
static std::int32_t WindowStall_RequestClose(); // 收摊请求，透传原发送返回。

// 原函数：0x004CED50。
static WindowInstance* WindowStall_ShowPurchaseConfirmation(); // 显示购买确认，返回入口窗口。

// 原函数：0x004CED60。
static void WindowStall_OpenBrowse(
    std::int32_t status, std::int32_t target, const char* stallName,
    const char* description, const char* itemPayload, const char* petPayload); // 浏览区完整生产后打开68。

// 原函数：0x004CFC40。
static WindowElement* WindowStall_EditItemPrice(); // 物品定价输入初始化，返回末次取得的元素表。

// 原函数：0x004CFCA0。
static void WindowStall_EditPetPrice(std::int32_t petIndex); // 固定六装备槽检查后进入宠物定价。

// 原函数：0x004CFD70。
static std::int32_t WindowStall_RemoveListing(); // 删除上架项，保持标记/数量/搬移/选择与页回退顺序。

// 原函数：0x004D2260。
static std::int32_t WindowStall_HandlePriceInput(std::int32_t unusedIndex, std::int32_t state); // 完整定价键盘和鼠标交互，返回hover位。

// 原函数：0x004D2840。
static std::int32_t WindowStall_HandlePurchaseConfirmation(std::int32_t unusedIndex, std::int32_t state); // 购买确认及物品/宠物详情，二次读模式。

// 原函数：0x004D1D50。
static std::int32_t WindowStall_HandleInventory(std::int32_t index, std::int32_t state); // 玩家背包20格，首命中交互但完整绘制。

// 原函数：0x004D04A0。
static std::int32_t WindowStall_HandleItemRow(std::int32_t index, std::int32_t state); // 上架/浏览物品行，固定记录与价格重读分离。

// 原函数：0x004D16F0。
static std::int32_t WindowStall_HandlePetCandidate(std::int32_t index, std::int32_t state); // 宠物候选，逐项重读候选页并保持CryptInt顺序。

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
static std::int32_t WindowStall_HandlePetRow(std::int32_t index, std::int32_t state); // 上架/浏览宠物行，末尾金额两次读取及详情分派。

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00C8A978。 摆摊结果提示，4D0230消费同一文本。
static std::wstring gStallMessageC8A978 {}; // 原全局：0x00C8A978。

// 原全局：0x00D67BE4。 原strncpy16及D67BF4终止字节。
static std::array<char, 17> gStallNameD67BE4 {}; // 原全局：0x00D67BE4。

// 原全局：0x00D67C31。 请求与回复共用等待标记。
static std::uint8_t gStallPendingD67C31 = 0; // 原全局：0x00D67C31。

static std::int32_t gStallMessageModeD67C34 = 0; // 原全局：0x00D67C34。

static std::int32_t gStallMessageValueD67C38 = 0; // 原全局：0x00D67C38。

// 原全局：0x0057B440。 原可变金额串，止于下一数字原型57B450。
static std::wstring gStallMoneyText57B440 {L"01234567"}; // 原全局：0x0057B440。

static_assert(!std::is_trivially_destructible_v<StallBrowseData>);

static_assert(offsetof(StallBrowseData, sellerName00) == 0);

static_assert(offsetof(StallBrowseData, stallName11) == 0x11);

static_assert(offsetof(StallBrowseData, description22) == 0x22);

static_assert(offsetof(StallBrowseData, bytes5B) == 0x5B);

static_assert(offsetof(StallBrowseData, target5C) == 0x5C);

static_assert(offsetof(StallBrowseData, imageNo60) == 0x60);

static_assert(offsetof(StallBrowseData, action64) == 0x64);

static_assert(offsetof(StallBrowseData, direction68) == 0x68);

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

static std::array<char, 57> gStallDescriptionD67BF8 {}; // 原全局：0x00D67BF8。

static WindowInputPrototype gStallNameInput581130 { // 原全局：0x00581130。
    {{&cg::ui::detail::gInputContextCB0028}, WindowInputReferenceKind::Context},
    230, 11, 3, 4, 0, {}, 1, 16, 0, 0,
};

static WindowInputPrototype gStallDescriptionInput581360 { // 原全局：0x00581360。
    {{&cg::ui::detail::gInputContextCC2378}, WindowInputReferenceKind::Context},
    299, 51, 3, 12, 0, {}, 2, 28, 14, 0,
};

static WindowInputPrototype gStallPriceInput581590 { // 原全局：0x00581590。
    {{&cg::ui::detail::gInputContextCF4E60}, WindowInputReferenceKind::Context},
    0, -100, 3, 12, 127, {}, 1, 0, 0, 0,
};

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
    L"請輸入店名八個字",
};

static WindowTextPrototype gWindowText57B430 { // 原全局：0x0057B430。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}},
    L"請輸入店舖描述",
};

static WindowNumberData gWindowNumber57B450 {&gStallMoneyText57B440, 0, 3, 1, 0}; // 原全局：0x0057B450。

static std::int32_t gWindowAnimationImage57B458 = 100000; // 原全局：0x0057B458。

static std::uint32_t gWindowMouseFlagsD67A10 = 0; // 原全局：0x00D67A10。

static WindowElementPrototype gWindowElements57B508[48] { // 原全局：0x0057B508。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay},
    {WindowElementType_Input, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePriceInput},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp},
    {WindowElementType_Input, 230, 11, 120, 16, 1, {.unusedValue = 0}, &WindowStall_EditName},
    {WindowElementType_Text, 298, 51, 0, 0, 1, {.text = &gWindowText57B430}, &WindowElement_NoOp},
    {WindowElementType_Input, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowStall_EditDescription},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2A0}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2B8}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2E8}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 322, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B318}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp},
};

static WindowElementPrototype gWindowElements57BB08[43] { // 原全局：0x0057BB08。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay},
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePurchaseConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2D0}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp},
};

// 原型：窗口67；原全局：0x0054EF58。
WindowPrototype gWindowPrototypeOwnStall {0, 4, 10, 30, 549, 366, 0x80000080u, 48, gWindowElements57B508, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}; // 67: 54EF58

// 原型：窗口68；原全局：0x0054EF88。
WindowPrototype gWindowPrototypeStallBrowse {0, 4, 10, 30, 549, 366, 0x80000080u, 43, gWindowElements57BB08, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}; // 68: 54EF88

// 窗口函数实现。

 // 原全局：0x00D046D8。

// 原函数：0x004CDE40。
static std::int32_t WindowStall_ButtonPreviousListing(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetPageD67BD0 > 0) {
                --gStallPetPageD67BD0;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t page = gStallPetPageD67BD0;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t maximum = gStallPetCountD67BD4 - 3;
            static_cast<void>(WindowScroll_SetPosition(window->elements + (own ? 38 : 33), maximum, page));
        } else {
            if (gStallItemPageD67BC8 > 0) {
                --gStallItemPageD67BC8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            const std::int32_t page = gStallItemPageD67BC8;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t maximum = gStallItemCountD67BCC - 10;
            static_cast<void>(WindowScroll_SetPosition(window->elements + (own ? 38 : 33), maximum, page));
        }
        consumed = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x004CDFA0。
static std::int32_t WindowStall_ButtonNextListing(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
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
            static_cast<void>(WindowScroll_SetPosition(
                gCurrentWindow->elements + (own ? 38 : 33), limit, current));
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
            static_cast<void>(WindowScroll_SetPosition(
                gCurrentWindow->elements + (own ? 38 : 33), limit, current));
        }
        consumed = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x004CE0F0。
static std::int32_t WindowStall_ScrollListings(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t mode = gStallModeD67BDC;
        if (mode == 1) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t maximum = gStallPetCountD67BD4 - 3;
            const std::int32_t page = gStallPetPageD67BD0;
            gStallPetPageD67BD0 = WindowScroll_ApplyWheel(
                gCurrentWindow->elements + (own ? 38 : 33), page, maximum, wheel);
        } else {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t maximum = gStallItemCountD67BCC - 10;
            const std::int32_t page = gStallItemPageD67BC8;
            gStallItemPageD67BC8 = WindowScroll_ApplyWheel(
                gCurrentWindow->elements + (own ? 38 : 33), page, maximum, wheel);
        }
    }
    return 0;
}

// 原函数：0x004CE1F0。
static std::int32_t WindowStall_ButtonPreviousPet(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetCandidatePageD67BD8 > 0) {
                --gStallPetCandidatePageD67BD8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            WindowElement* const elements = gCurrentWindow->elements;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            static_cast<void>(WindowScroll_SetPosition(elements + (own ? 42 : 37), 1, page));
        }
        consumed = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x004CE2B0。
static std::int32_t WindowStall_ButtonNextPet(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gStallModeD67BDC == 1) {
            if (gStallPetCandidatePageD67BD8 < 1) {
                ++gStallPetCandidatePageD67BD8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const bool own = gStallOwn58111C != 0;
            WindowElement* const elements = gCurrentWindow->elements;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            static_cast<void>(WindowScroll_SetPosition(elements + (own ? 42 : 37), 1, page));
        }
        consumed = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x004CE370。
static std::int32_t WindowStall_ScrollPetCandidates(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t mode = gStallModeD67BDC;
        if (mode == 1) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t page = gStallPetCandidatePageD67BD8;
            gStallPetCandidatePageD67BD8 =
                WindowScroll_ApplyWheel(window->elements + (own ? 42 : 37), page, 1, wheel);
        }
    }
    return 0;
}

// 原函数：0x004CE3F0。
static std::int32_t WindowStall_TabCategory(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    const bool ownAtEntry = gStallOwn58111C != 0;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t category = index;
    if (ownAtEntry) {
        if (index == 35) {
            category = 0;
        } else if (index == 36) {
            category = 1;
        }
    } else if (index == 30) {
        category = 0;
    } else if (index == 31) {
        category = 1;
    }
    std::int32_t consumed = 0;
    if (category == gStallModeD67BDC) {
        graphic->imageNo = kStallTabImages54EF40[category][0];
    } else {
        if ((state & 1) != 0) {
            graphic->imageNo = kStallTabImages54EF40[category][2];
            consumed = 1;
        } else {
            graphic->imageNo = kStallTabImages54EF40[category][1];
        }
        if ((state & 2) != 0) {
            const bool own = gStallOwn58111C != 0;
            gStallModeD67BDC = category;
            gStallItemSelection581124 = -1;
            if (category == 1) {
                const std::int32_t page = gStallPetPageD67BD0;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t maximum = gStallPetCountD67BD4 - 3;
                static_cast<void>(WindowScroll_SetPosition(window->elements + (own ? 38 : 33), maximum, page));
            } else {
                const std::int32_t page = gStallItemPageD67BC8;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t maximum = gStallItemCountD67BCC - 10;
                static_cast<void>(WindowScroll_SetPosition(window->elements + (own ? 38 : 33), maximum, page));
            }
            consumed = 1;
        }
    }
    const bool own = gStallOwn58111C != 0;
    const std::int32_t visible = gStallModeD67BDC == 1 ? 1 : 0;
    if (own) {
        constexpr std::array<std::int32_t, 7> slots {41, 42, 43, 44, 11, 45, 46};
        for (const std::int32_t slot : slots) {
            gCurrentWindow->elements[slot].visible = visible;
        }
    } else {
        constexpr std::array<std::int32_t, 7> slots {36, 37, 38, 39, 9, 40, 41};
        for (const std::int32_t slot : slots) {
            gCurrentWindow->elements[slot].visible = visible;
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
static std::int32_t WindowStall_RequestClose()
{
    return network::Sub_512160(platform::gConnectionSocket);
}

// 原函数：0x004CED50。
static WindowInstance* WindowStall_ShowPurchaseConfirmation()
{
    WindowInstance* const window = gCurrentWindow;
    window->elements[1].visible = 1;
    return window;
}

// 原函数：0x004CFC40。
static WindowElement* WindowStall_EditItemPrice()
{
    gStallPriceD67C40 = 0;
    static_cast<void>(WindowInput_ActivateElement(&gStallPriceInput581590, 0, -100, 0));
    input::SetContext_44B330(&cg::ui::detail::gInputContextCF4E60);
    WindowInstance* const window = gCurrentWindow;
    gFocusedElementDataD45F94 = window->elements[1].data;
    static_cast<WindowInputData*>(window->elements[1].data)->context = &cg::ui::detail::gInputContextCF4E60;
    WindowElement* const elements = gCurrentWindow->elements;
    elements[1].visible = 1;
    return elements;
}

// 原函数：0x004CFCA0。
static void WindowStall_EditPetPrice(const std::int32_t petIndex)
{
    const auto& pet = world::detail::gPetRuntimeRecords[petIndex];
    const std::int32_t itemSlots = pet.values6D8[0];
    for (std::int32_t slot = 0; slot < 6; ++slot) {
        if (slot < itemSlots && pet.items6E4[slot].used != 0) {
            world::detail::Sub_44B050(L"請卸下寵物身上的裝備後再操作。", 4, 3);
            return;
        }
    }
    gStallPriceD67C40 = 0;
    static_cast<void>(WindowInput_ActivateElement(&gStallPriceInput581590, 0, -100, 0));
    input::SetContext_44B330(&cg::ui::detail::gInputContextCF4E60);
    WindowInstance* const window = gCurrentWindow;
    gFocusedElementDataD45F94 = window->elements[1].data;
    static_cast<WindowInputData*>(window->elements[1].data)->context = &cg::ui::detail::gInputContextCF4E60;
    gCurrentWindow->elements[1].visible = 1;
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
static std::int32_t WindowStall_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    bool disabled = false;
    std::int32_t imageGroup = state;
    if (gStallOwn58111C != 0) {
        switch (index) {
        case 30:
            imageGroup = 0;
            if ((gStallItemCountD67BCC <= 0 && gStallPetCountD67BD4 <= 0)
                || gStallPendingD67C31 != 0) {
                disabled = true;
            } else if ((state & 2) != 0) {
                const std::uint8_t gate = gValueD67BE0;
                gStallPendingD67C31 = 1;
                gValueD67BE0 = gate == 0 ? 1 : 0;
                gStallItemSelection581124 = -1;
                gStallPetSelection581128 = -1;
                static_cast<void>(WindowStall_SubmitListings());
            }
            break;
        case 31:
            imageGroup = 1;
            if (gStallPendingD67C31 != 0) {
                disabled = true;
            } else if ((state & 2) != 0) {
                gStallPendingD67C31 = 1;
                gStallItemSelection581124 = -1;
                gStallPetSelection581128 = -1;
                static_cast<void>(WindowStall_RequestClose());
            }
            break;
        case 32: {
            imageGroup = 3;
            const std::int32_t mode = gStallModeD67BDC;
            if (mode == 0) {
                const std::int32_t selected = gStallItemSelection581124;
                if (selected < 20 || selected >= 40) {
                    disabled = true;
                } else if ((state & 2) != 0) {
                    static_cast<void>(WindowStall_RemoveListing());
                }
            } else if (mode == 1) {
                const std::int32_t selected = gStallPetSelection581128;
                if (selected < 5 || selected >= 10) {
                    disabled = true;
                } else if ((state & 2) != 0) {
                    static_cast<void>(WindowStall_RemoveListing());
                }
            }
            break;
        }
        case 33:
            imageGroup = 4;
            if (gValueD67BE0 != 0) {
                disabled = true;
            } else if ((state & 2) != 0) {
                window->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
            break;
        case 34: {
            imageGroup = 5;
            const std::int32_t mode = gStallModeD67BDC;
            if (mode == 0) {
                const std::int32_t selected = gStallItemSelection581124;
                if (selected < 0 || selected >= 20) {
                    disabled = true;
                } else if (gStallItemUsedCC31AC[selected] == 0 && (state & 2) != 0) {
                    static_cast<void>(WindowStall_EditItemPrice());
                }
            } else if (mode == 1) {
                const std::int32_t selected = gStallPetSelection581128;
                if (selected < 0 || selected >= 5) {
                    disabled = true;
                } else if (gStallPetUsedCF400C[selected] == 0 && (state & 2) != 0) {
                    WindowStall_EditPetPrice(selected);
                }
            }
            break;
        }
        default:
            break;
        }
    } else if (index == 28) {
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
                } else if ((state & 2) != 0) {
                    static_cast<void>(WindowStall_ShowPurchaseConfirmation());
                }
            }
        }
    } else if (index == 29) {
        imageGroup = 4;
        if (gStallPendingD67C31 != 0) {
            disabled = true;
        } else if ((state & 2) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    if (disabled) {
        graphic->imageNo = kStallButtonImages54EEE0[imageGroup][1];
        gStallButtonStateD67C44 = state;
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = kStallButtonImages54EEE0[imageGroup][2];
        consumed = 1;
    } else {
        graphic->imageNo = kStallButtonImages54EEE0[imageGroup][0];
    }
    if ((state & 8) != 0) {
        graphic->imageNo = kStallButtonImages54EEE0[imageGroup][1];
        consumed = 1;
    }
    if ((state & 2) != 0) {
        input::InputContext* const context = input::gCurrentInputData;
        if (context == &cg::ui::detail::gInputContextCB0028 || context == &cg::ui::detail::gInputContextCC2378) {
            WindowChat_RestoreInput();
        }
        gStallButtonStateD67C44 = state;
        return 1;
    }
    gStallButtonStateD67C44 = state;
    return consumed;
}

// 原函数：0x004CFE70。
static std::int32_t WindowStall_SubmitListings()
{
    std::wstring buffer;
    char name[33] {};
    cg::text::StoreGbk(name, sizeof(name), L"無名店舖");
    char description[113] {};
    cg::text::StoreGbk(description, sizeof(description), L"老闆沒什麼可介紹的");
    std::wstring offers;
    char escaped[276];
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
    const std::size_t nameLength = std::strlen(gStallNameD67BE4.data());
    if (nameLength != 0) {
        std::sprintf(name, "%s", gStallNameD67BE4.data());
    }
    cg::NullSub_509080();
    input::EscapeGbkText(name, escaped, 275);
    std::strncpy(name, escaped, static_cast<std::uint32_t>(32));
    const std::size_t descriptionLength = std::strlen(gStallDescriptionD67BF8.data());
    name[32] = '\0';
    if (descriptionLength != 0) {
        std::sprintf(description, "%s", gStallDescriptionD67BF8.data());
    }
    cg::NullSub_509080();
    input::EscapeGbkText(description, escaped, 275);
    std::strncpy(description, escaped, static_cast<std::uint32_t>(112));
    const std::uintptr_t socket = platform::gConnectionSocket;
    description[112] = '\0';
    static_cast<void>(network::Sub_5120D0(socket, cg::text::FromGbkWide(name), cg::text::FromGbkWide(description), offers));
    if (!gStallNameInput581130.input.context->empty()) {
        std::strcpy(login::detail::GetCurrentSavedStallNameFF4D18(), name);
    }
    if (!gStallDescriptionInput581360.input.context->empty()) {
        std::strcpy(login::detail::GetCurrentSavedStallDescriptionFF4D28(), description);
    }
    gStallMessageC8A978 = L"擺攤請求中";
    WindowInstance* const window = gCurrentWindow;
    gStallMessageModeD67C34 = 0;
    gStallMessageValueD67C38 = 0;
    window->elements[0].visible = 1;
    return 0;
}

// 原函数：0x004D0230。
static std::int32_t WindowStall_DrawTooltipOverlay(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t mode = gStallMessageModeD67C34;
    if (mode != 0 && mode != 1) {
        return state & 1;
    }
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const std::int32_t width = cg::render::TextMeasure(gStallMessageC8A978, 0);
    WindowInstance* window = gCurrentWindow;
    const auto textY = static_cast<std::int16_t>(window->y + (window->height - 16) / 2);
    const auto textX = static_cast<std::int16_t>(window->x + (window->width - width) / 2);
    static_cast<void>(render::DrawText_456780(
        textX, textY, 4, 0, 0, gStallMessageC8A978, 0, 0));
    if (mode == 1) {
        window = gCurrentWindow;
        const std::int32_t centerY = window->height / 2 + window->y;
        const std::int32_t centerX = window->x + window->width / 2;
        const bool hit = cg::ui::HitTestMouseRect_5049A0(
            centerX - 33, centerY + 18, centerX + 33, centerY + 35, -1) != 0;
        window = gCurrentWindow;
        const std::int32_t buttonY = window->height / 2 + window->y + 27;
        const std::int32_t buttonX = window->x + window->width / 2;
        static_cast<void>(cg::render::DrawGraphic(
            buttonX, buttonY, 227, hit ? 243050 : 243048, 0, &descriptor));
        if (hit && (state & 2) != 0) {
            gCurrentWindow->elements[index].visible = 0;
        }
    }
    window = gCurrentWindow;
    const auto backgroundY = static_cast<std::int16_t>(window->y + (window->height - 92) / 2);
    const auto backgroundX = static_cast<std::int16_t>(window->x + (window->width - 220) / 2);
    cg::yobi::detail::DrawNineGrid_49BE00(
        245300, backgroundX, backgroundY, 220, 92, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    return state & 1;
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
    const char* const text = pet.techniquesD8[skillIndex].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, L"", 0, 0));
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
            5, 12, 0, cg::text::FromGbkWide(text).c_str(), 0, 0));
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
    const char* const title = pet.name6AC.data();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 60), static_cast<std::int16_t>(y + 11),
            5, 12, 0, cg::text::FromGbkWide(title).c_str(), 0, 0));
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
        gPreviewActionD67B84 = cg::yobi::detail::NewYobiAction_404AE0(2, 0);
    }
    const std::int32_t imageNo = pet.imageNo6A0;
    cg::yobi::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(cg::yobi::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    cg::yobi::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX = static_cast<std::int16_t>(x);
    const std::int32_t graphicIndex = cg::yobi::detail::GetYobiActionField20(action);
    const render::GraphicDescriptor16* const mode = cg::yobi::detail::GetYobiActionDrawMode(action);
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
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(WindowStall_DrawPetSkills(shortX + 10, shortY + 141 + skill * 16, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(shortX, shortY, 230, 244373, 0, 1, 0, &descriptor);
}

// 原函数：0x004D0EB0。
static std::int32_t WindowStall_HandlePetRow(const std::int32_t index, const std::int32_t state)
{
    if (gStallModeD67BDC != 1) {
        return 0;
    }
    const bool own = gStallOwn58111C != 0;
    const std::int32_t row = index - (own ? 23 : 21);
    const std::int32_t position = gStallPetPageD67BD0 + row;
    if (position >= gStallPetCountD67BD4) {
        return 0;
    }
    world::detail::PetRuntimeRecord* const pet = own
        ? &world::detail::gPetRuntimeRecords[gStallPetOffersCC3E98[position].slot]
        : &gStallBrowseData->pets[position];
    if (pet->used == 0) {
        return 0;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", cg::text::FromGbkWide(pet->name6AC.data()).c_str());
    {
        WindowInstance* const window = gCurrentWindow;
        const auto x = static_cast<std::int16_t>(window->x + 77);
        const auto y = static_cast<std::int16_t>(window->y + 91 * row + 58);
        static_cast<void>(render::DrawText_456780(
            x, y, 3, 13, std::bit_cast<std::int8_t>(std::uint8_t{138}), buffer, 0, 0));
    }
    const auto drawText = [&](const std::int32_t dx, const std::int32_t dy) {
        WindowInstance* const window = gCurrentWindow;
        const auto y = static_cast<std::int16_t>(window->y + 91 * row + dy);
        const auto x = static_cast<std::int16_t>(window->x + dx);
        static_cast<void>(render::DrawText_456780(
            x, y, 3, 13, std::bit_cast<std::int8_t>(std::uint8_t{138}), buffer, 0, 0));
    };
    cg::text::FormatTo(buffer, L"{:4}", pet->values0C[1].value());
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
    cg::text::FormatTo(buffer, L"{:4}", pet->values0C[3].value());
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
    if (gValueD67BE0 != 0 && gStallOwn58111C != 0) {
        render::GraphicDescriptor16 descriptor {};
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        const std::uint32_t color = 0x80808080u;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t y = window->y + element.y;
        const std::int32_t x = window->x + element.x;
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 223, mode <= 1 ? 98849 : 98850, 0, 1, 0, &descriptor));
    } else {
        if ((state & 1) != 0) {
            WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 240), static_cast<std::uint16_t>(y + 90),
                223, cg::render::gValueD69768, 0);
            consumed = 1;
        }
        if ((state & 2) != 0) {
            gStallPetSelection581128 = gStallPetPageD67BD0 + row + 5;
        }
        const bool currentOwn = gStallOwn58111C != 0;
        if ((state & 0x100) != 0) {
            if (currentOwn) {
                static_cast<void>(WindowStall_RemoveListing());
            } else {
                static_cast<void>(WindowStall_ShowPurchaseConfirmation());
            }
        }
        const std::int32_t current = gStallPetPageD67BD0 + row;
        if (current == gStallPetSelection581128 - 5) {
            WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 238), static_cast<std::uint16_t>(y + 88),
                223, 254, 0);
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
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y + 91 * row + 128;
        const std::int32_t x = window->x + 243;
        WindowNumber_Draw(x, y, buffer, 0, priceColor, 1, 223);
    }
    if ((state & 1) != 0) {
        WindowInstance* const window = gCurrentWindow;
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
    const std::int32_t segments = world::detail::gPetRuntimeRecords[petIndex].values70[21 + attribute] / 10;
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
static std::int32_t WindowStall_HandlePetCandidate(const std::int32_t index, const std::int32_t state)
{
    if (gStallModeD67BDC != 1) {
        return 0;
    }
    const std::int32_t row = index - (gStallOwn58111C != 0 ? 26 : 24);
    const auto* const initialPet = &world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row];
    if (initialPet->used == 0) {
        if ((state & 2) != 0) {
            gStallPetSelection581128 = -1;
        }
        return 0;
    }
    std::wstring buffer;
    const auto drawText = [&](const std::int32_t dx, const std::int32_t dy) {
        WindowInstance* const window = gCurrentWindow;
        const auto y = static_cast<std::int16_t>(window->y + row * 50 + dy);
        const auto x = static_cast<std::int16_t>(window->x + dx);
        static_cast<void>(render::DrawText_456780(x, y, 3, 12, 0, buffer, 0, 0));
    };
    cg::text::FormatTo(buffer, L"{}", cg::text::FromGbkWide(initialPet->name6AC.data()).c_str());
    drawText(298, 132);
    cg::text::FormatTo(buffer, L"{:3}", world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].value08);
    drawText(425, 132);
    const std::int32_t maxHp = world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[1].value();
    const std::int32_t hp = world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[0].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawText(333, 146);
    const std::int32_t maxFp = world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[3].value();
    const std::int32_t fp = world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].values0C[2].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawText(333, 160);
    const std::int16_t tribe = std::bit_cast<std::int16_t>(
        world::detail::gPetRuntimeRecords[gStallPetCandidatePageD67BD8 + row].value04);
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
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y + barPositions[attribute][1] + 50 * row + 127;
        const std::int32_t x = window->x + barPositions[attribute][0] + 292;
        const std::int32_t petIndex = gStallPetCandidatePageD67BD8 + row;
        static_cast<void>(WindowStall_DrawCandidateAttributes(petIndex, attribute, x, y));
    }
    std::int32_t consumed = 0;
    if (gValueD67BE0 == 0) {
        const bool own = gStallOwn58111C != 0;
        const std::int32_t position = gStallPetCandidatePageD67BD8 + row;
        bool offered = gStallPetUsedCF400C[position] != 0;
        if (own && (state & 2) != 0 && offered) {
            gStallPetSelection581128 = -1;
            offered = gStallPetUsedCF400C[position] != 0;
        }
        if (!offered && own) {
            if ((state & 1) != 0) {
                WindowInstance* const window = gCurrentWindow;
                const WindowElement& element = window->elements[index];
                const std::int32_t y = window->y + element.y;
                const std::int32_t x = window->x + element.x;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                    static_cast<std::uint16_t>(x + 231), static_cast<std::uint16_t>(y + 48),
                    223, cg::render::gValueD69768, 0);
                consumed = 1;
            }
            if ((state & 2) != 0) {
                gStallPetSelection581128 = gStallPetCandidatePageD67BD8 + row;
            }
            if ((state & 0x100) != 0) {
                WindowStall_EditPetPrice(gStallPetSelection581128);
            }
        }
        if ((offered || own) && gStallOwn58111C != 0
            && gStallPetCandidatePageD67BD8 + row == gStallPetSelection581128) {
            WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 229), static_cast<std::uint16_t>(y + 46),
                223, 254, 0);
        }
    }
    if ((state & 1) != 0) {
        WindowInstance* const window = gCurrentWindow;
        std::int32_t x = window->x + 20;
        const std::int32_t y = window->y + 20;
        if (x < 0) {
            x += window->width;
        }
        const auto petIndex = static_cast<std::uint8_t>(gStallPetCandidatePageD67BD8 + row);
        static_cast<void>(WindowTrade_DrawOwnPetTooltip(x, y, petIndex));
    }
    if (gStallOwn58111C != 0 && gStallPetUsedCF400C[gStallPetCandidatePageD67BD8 + row] != 0) {
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t y = window->y + element.y;
        const std::int32_t x = window->x + element.x;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
            static_cast<std::uint16_t>(x + 229), static_cast<std::uint16_t>(y + 46),
            223, 256, 0);
    }
    return consumed;
}

// 原函数：0x004D04A0。
static std::int32_t WindowStall_HandleItemRow(const std::int32_t index, const std::int32_t state)
{
    if (gValueD67BE0 != 0) {
        const std::int32_t petCount = gStallPetCountD67BD4;
        const std::int32_t itemCount = gStallItemCountD67BCC;
        if (petCount + itemCount <= 0) {
            static_cast<void>(WindowStallBrowse_RequestClose());
            static_cast<void>(WindowOwnStall_RequestClose());
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
    world::detail::PlayerItemRecord* item;
    if (own) {
        const std::int32_t slot = gStallItemOffersD671D8[position].slot;
        item = &world::detail::gPlayerRuntimeSlots.items[slot + 8];
        if (item->used == 0) {
            return 0;
        }
        if ((state & 0x21) != 0) {
            gStallHover581120 = slot + 8;
        }
    } else {
        item = &gStallBrowseData->items[position];
        if (item->used == 0) {
            return 0;
        }
        if ((state & 0x21) != 0) {
            gStallHover581120 = position + 200;
        }
    }
    if ((state & 4) != 0) {
        ++gStallDescriptionPageD67C3C;
        if (gStallDescriptionPageD67C3C >= item->pages) {
            gStallDescriptionPageD67C3C = 0;
        }
    }
    std::wstring buffer;
    for (std::int32_t line = 0; line < 2; ++line) {
        std::wstring nameLine;
        static_cast<void>(input::ExtractTextLine(nameLine, item->name, line, 10));
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const auto y = static_cast<std::int16_t>(window->y + element.y + 5 + line * 14);
        const auto x = static_cast<std::int16_t>(window->x + element.x + 52);
        static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, nameLine, 0, 0));
    }
    std::int32_t consumed = 0;
    if (gValueD67BE0 != 0 && gStallOwn58111C != 0) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        WindowInstance* const window = gCurrentWindow;
        render::GraphicDescriptor16 descriptor {};
        const std::uint32_t color = 0x80808080u;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        const WindowElement& element = window->elements[index];
        const std::int32_t y = window->y + element.y;
        const std::int32_t x = window->x + element.x;
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 223, mode <= 1 ? 98847 : 98848, 0, 1, 0, &descriptor));
    } else {
        if ((state & 1) != 0) {
            WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                223, cg::render::gValueD69768, 0);
            consumed = 1;
        }
        if ((state & 2) != 0) {
            gStallItemSelection581124 = gStallItemPageD67BC8 + row + 20;
        }
        const bool currentOwn = gStallOwn58111C != 0;
        if ((state & 0x100) != 0) {
            if (currentOwn) {
                static_cast<void>(WindowStall_RemoveListing());
            } else {
                static_cast<void>(WindowStall_ShowPurchaseConfirmation());
            }
        }
        const std::int32_t current = gStallItemPageD67BC8 + row;
        if (current == gStallItemSelection581124 - 20) {
            WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t y = window->y + element.y;
            const std::int32_t x = window->x + element.x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
                223, 254, 0);
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
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t y = element.y + window->y + 35;
        const std::int32_t x = element.x + window->x + 105;
        WindowNumber_Draw(x, y, buffer, 0, priceColor, 1, 223);
    }
    const std::int32_t count = item->count;
    if (count > 0) {
        std::wstring countBuffer;
        cg::text::FormatTo(countBuffer, L"{:3}", count);
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const auto y = static_cast<std::int16_t>(window->y + element.y + 32);
        const auto x = static_cast<std::int16_t>(window->x + element.x + 21);
        static_cast<void>(render::DrawText_456780(x, y, 3, 1, 1, countBuffer, 0, 0));
    }
    WindowInstance* const window = gCurrentWindow;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const WindowElement& element = window->elements[index];
    const std::int32_t imageNo = item->imageNo;
    const std::int32_t y = element.y + window->y + 24;
    const std::int32_t x = element.x + window->x + 24;
    static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    return consumed;
}

// 原函数：0x004D1D50。
static std::int32_t WindowStall_HandleInventory(const std::int32_t index, const std::int32_t state)
{
    if (gStallModeD67BDC != 0) {
        return 0;
    }
    std::int32_t hovered = -1;
    std::wstring buffer;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        auto* const item = &world::detail::gPlayerRuntimeSlots.items[slot + 8];
        if (item->used == 0) {
            continue;
        }
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
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
        if (hovered < 0 && (state & 0x21) != 0
            && cg::ui::HitTestMouseRect_5049A0(x, y, x + 48, y + 48, -1) != 0) {
            hovered = slot;
            if (gStallInventoryHover5817FC != slot) {
                gStallDescriptionPageD67C3C = 0;
            }
            const std::uint8_t gate = gValueD67BE0;
            gStallInventoryHover5817FC = slot;
            if (gate == 0) {
                if (gStallOwn58111C != 0 && (state & 2) != 0
                    && (gStallItemUsedCC31AC[slot] != 0 || (item->flags & 0x60u) != 0)) {
                    gStallItemSelection581124 = -1;
                }
                if (gStallItemUsedCC31AC[slot] == 0 && (item->flags & 0x60u) == 0) {
                    if ((state & 1) != 0) {
                        cg::render::DrawGraphicHover(
                            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                            223, cg::render::gValueD69768, 0);
                    }
                    if (gStallOwn58111C != 0) {
                        if ((state & 2) != 0) {
                            gStallItemSelection581124 = slot;
                        }
                        if ((state & 0x100) != 0) {
                            static_cast<void>(WindowStall_EditItemPrice());
                        }
                    }
                }
            }
            if ((state & 4) != 0) {
                const std::int32_t page = gStallDescriptionPageD67C3C + 1;
                const std::int32_t pages = item->pages;
                gStallDescriptionPageD67C3C = page;
                if (page >= pages) {
                    gStallDescriptionPageD67C3C = 0;
                }
            }
            gStallHover581120 = slot + 8;
        }
        if (gStallOwn58111C != 0 && gValueD67BE0 == 0 && slot == gStallItemSelection581124) {
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
    return (state & 0x21) != 0 ? 1 : 0;
}

// 原函数：0x004D2260。
static std::int32_t WindowStall_HandlePriceInput(const std::int32_t, const std::int32_t state)
{
    const bool clicked = (state & 2) != 0;
    if (clicked) {
        static_cast<void>(WindowInput_ActivateElement(&gStallPriceInput581590, 0, -100, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextCF4E60);
        WindowInstance* const window = gCurrentWindow;
        gFocusedElementDataD45F94 = window->elements[1].data;
        static_cast<WindowInputData*>(window->elements[1].data)->context = &cg::ui::detail::gInputContextCF4E60;
    }
    if (input::gCurrentInputData == &cg::ui::detail::gInputContextCF4E60) {
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
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y + 115;
    const std::int32_t x = window->x + 206;
    if (cg::ui::HitTestMouseRect_5049A0(x + 103, y + 6, x + 118, y + 19, -1) != 0) {
        if (clicked) {
            if (input::gCurrentInputData == &cg::ui::detail::gInputContextCF4E60) {
                WindowChat_RestoreInput();
            }
            gCurrentWindow->elements[1].visible = 0;
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
            imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
        imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
        imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
                if (input::gCurrentInputData == &cg::ui::detail::gInputContextCF4E60) {
                    WindowChat_RestoreInput();
                }
                gCurrentWindow->elements[1].visible = 0;
            }
            imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
    return state & 1;
}

// 原函数：0x004D2840。
static std::int32_t WindowStall_HandlePurchaseConfirmation(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
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
        if ((state & 2) != 0) {
            static_cast<void>(WindowStall_RequestPurchase());
            gCurrentWindow->elements[1].visible = 0;
        }
        imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
        if ((state & 2) != 0) {
            gCurrentWindow->elements[1].visible = 0;
        }
        imageState = (state & 8) != 0 ? 1 : ((state & 1) != 0 ? 2 : -1);
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
            if ((state & 1) != 0) {
                gStallHover581120 = gStallItemSelection581124 + 180;
            }
            if ((state & 4) != 0) {
                ++gStallDescriptionPageD67C3C;
                if (gStallDescriptionPageD67C3C >= item->pages) {
                    gStallDescriptionPageD67C3C = 0;
                }
            }
        }
        if (cg::ui::HitTestMouseRect_5049A0(x + 133, y + 6, x + 148, y + 19, -1) != 0) {
            if ((state & 2) != 0) {
                gCurrentWindow->elements[1].visible = 0;
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
            if ((state & 2) != 0) {
                gCurrentWindow->elements[1].visible = 0;
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
        cg::text::FormatTo(details, L"{}", cg::text::FromGbkWide(pet->name6AC.data()).c_str());
        drawPetText(75, 61);
        cg::text::FormatTo(details, L"{:4}", pet->values0C[1].value());
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
        cg::text::FormatTo(details, L"{:4}", pet->values0C[3].value());
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
    return state & 1;
}

// 原函数：0x004CD4F0。
static std::int32_t WindowStall_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const initial = gCurrentWindow;
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
        gValueD67BE0 = 0;
        std::memset(gStallItemUsedCC31AC.data() + 12, 0, 4);
        gStallDescriptionPageD67C3C = 0;
        gStallNameD67BE4[0] = '\0';
        gStallDescriptionD67BF8[0] = '\0';
        std::memset(gStallItemUsedCC31AC.data() + 16, 0, 4);
        gStallPetUsedCF400C[4] = 0;
        initial->elements[0].visible = 0;
        gCurrentWindow->elements[1].visible = 0;
    }
    std::wstring lineText;
    if (gStallOwn58111C != 0) {
        const bool showScroll = gStallModeD67BDC == 1
            ? gStallPetCountD67BD4 > 3 : gStallItemCountD67BCC > 10;
        gCurrentWindow->elements[37].visible = showScroll ? 1 : 0;
        WindowElement* elements = gCurrentWindow->elements;
        if ((static_cast<WindowMouseData*>(elements[38].data)->flags & 1u) != 0) {
            if (gStallModeD67BDC == 1) {
                gStallPetPageD67BD0 = WindowScroll_ReadPosition(elements + 38, gStallPetCountD67BD4 - 3);
            } else {
                gStallItemPageD67BC8 = WindowScroll_ReadPosition(elements + 38, gStallItemCountD67BCC - 10);
            }
        }
        elements = gCurrentWindow->elements;
        if ((static_cast<WindowMouseData*>(elements[42].data)->flags & 1u) != 0
            && gStallModeD67BDC == 1) {
            gStallPetCandidatePageD67BD8 = WindowScroll_ReadPosition(elements + 42, 1);
        }
        static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = cg::text::FromGbkWide(world::detail::GetPlayerDisplayText(1));
        gCurrentWindow->elements[7].visible = 0;
        if (input::gCurrentInputData == &cg::ui::detail::gInputContextCC2378
            && gValueD67BE0 == 0 && gStallOwn58111C != 0) {
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(WindowInput_ActivateElement(&gStallDescriptionInput581360, window->x, window->y, 1));
            cg::text::StoreGbk(gStallDescriptionD67BF8.data(), 57, input::ContextText(*gStallDescriptionInput581360.input.context));
            gStallDescriptionD67BF8[56] = '\0';
        } else if (std::strlen(gStallDescriptionD67BF8.data()) != 0) {
            for (std::int32_t line = 0; line < 3; ++line) {
                static_cast<void>(input::ExtractGbkTextLine(lineText, gStallDescriptionD67BF8.data(), line, 28));
                WindowInstance* const window = gCurrentWindow;
                WindowElement* const current = window->elements;
                const std::int16_t y = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(current[8].y + window->y + line * 14));
                const std::int16_t x = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(current[8].x + window->x));
                static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, lineText, 0, 0));
            }
        } else {
            const char* const source = login::detail::GetCurrentSavedStallDescriptionFF4D28();
            const std::size_t bytes = std::strlen(source) + 1;
            WindowInstance* const window = gCurrentWindow;
            std::memcpy(gStallDescriptionD67BF8.data(), source, bytes);
            window->elements[7].visible = 1;
        }
        gCurrentWindow->elements[5].visible = 0;
        if (input::gCurrentInputData == &cg::ui::detail::gInputContextCB0028
            && gValueD67BE0 == 0 && gStallOwn58111C != 0) {
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(WindowInput_ActivateElement(&gStallNameInput581130, window->x, window->y, 1));
            cg::text::StoreGbk(gStallNameD67BE4.data(), gStallNameD67BE4.size(), input::ContextText(*gStallNameInput581130.input.context));
            gStallNameD67BE4[16] = '\0';
        } else if (std::strlen(gStallNameD67BE4.data()) != 0) {
            WindowInstance* const window = gCurrentWindow;
            WindowElement* const current = window->elements;
            const std::int16_t y = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(current[6].y + window->y));
            const std::int16_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(current[6].x + window->x));
            static_cast<void>(render::DrawText_456780(x, y, 3, 4,
                std::bit_cast<std::int8_t>(std::uint8_t{0x8C}), cg::text::FromGbkWide(gStallNameD67BE4.data()).c_str(), 0, 0));
        } else {
            const char* const source = login::detail::GetCurrentSavedStallNameFF4D18();
            const std::size_t bytes = std::strlen(source) + 1;
            WindowInstance* const window = gCurrentWindow;
            std::memcpy(gStallNameD67BE4.data(), source, bytes);
            window->elements[5].visible = 1;
        }
        const std::uint8_t gate = gValueD67BE0;
        gStallHover581120 = -1;
        if (gate != 0) {
            gCurrentWindow->elements[30].visible = 0;
            gCurrentWindow->elements[31].visible = 1;
        } else {
            gCurrentWindow->elements[30].visible = 1;
            gCurrentWindow->elements[31].visible = 0;
        }
    } else {
        const bool showScroll = gStallModeD67BDC == 1
            ? gStallPetCountD67BD4 > 3 : gStallItemCountD67BCC > 10;
        gCurrentWindow->elements[32].visible = showScroll ? 1 : 0;
        WindowElement* elements = gCurrentWindow->elements;
        if ((static_cast<WindowMouseData*>(elements[33].data)->flags & 1u) != 0) {
            if (gStallModeD67BDC == 1) {
                gStallPetPageD67BD0 = WindowScroll_ReadPosition(elements + 33, gStallPetCountD67BD4 - 3);
            } else {
                gStallItemPageD67BC8 = WindowScroll_ReadPosition(elements + 33, gStallItemCountD67BCC - 10);
            }
        }
        elements = gCurrentWindow->elements;
        if ((static_cast<WindowMouseData*>(elements[37].data)->flags & 1u) != 0
            && gStallModeD67BDC == 1) {
            gStallPetCandidatePageD67BD8 = WindowScroll_ReadPosition(elements + 37, 1);
        }
        static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = cg::text::FromGbkWide(gStallBrowseData->sellerName00.data());
        static_cast<WindowTextData*>(gCurrentWindow->elements[5].data)->text = cg::text::FromGbkWide(gStallBrowseData->stallName11.data());
        for (std::int32_t line = 0; line < 3; ++line) {
            static_cast<void>(input::ExtractGbkTextLine(lineText, gStallBrowseData->description22.data(), line, 28));
            WindowInstance* const window = gCurrentWindow;
            WindowElement* const current = window->elements;
            const std::int16_t y = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(current[6].y + window->y + line * 14));
            const std::int16_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(current[6].x + window->x));
            static_cast<void>(render::DrawText_456780(x, y, 3, 8, 0, lineText, 0, 0));
        }
        gStallHover581120 = -1;
    }
    cg::text::FormatTo(gStallMoneyText57B440, L"{}", world::detail::gPlayerRuntimeSlots.valueFC);
    return 1;
}

// 原函数：0x004D2080。
static std::int32_t WindowStall_EditDescription(const std::int32_t, const std::int32_t state)
{
    if (gValueD67BE0 != 0 || gStallOwn58111C == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"請輸入描述" /* 原常量参考：0x00586450。 */;
    }
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (gFocusedElementDataD45F94 != window->elements[8].data) {
            gStallDescriptionInput581360.text = cg::text::FromGbkWide(gStallDescriptionD67BF8.data());
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gStallDescriptionInput581360, x, y, 0));
            input::SetContext_44B330(&cg::ui::detail::gInputContextCC2378);
            WindowInstance* const current = gCurrentWindow;
            gFocusedElementDataD45F94 = current->elements[8].data;
            static_cast<WindowInputData*>(current->elements[8].data)->context =
                &cg::ui::detail::gInputContextCC2378;
        }
        return 1;
    }
    return 0;
}

// 原函数：0x004D2170。
static std::int32_t WindowStall_EditName(const std::int32_t, const std::int32_t state)
{
    if (gValueD67BE0 != 0 || gStallOwn58111C == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"請輸入描述" /* 原常量参考：0x00586450。 */;
    }
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (gFocusedElementDataD45F94 != window->elements[6].data) {
            gStallNameInput581130.text = cg::text::FromGbkWide(gStallNameD67BE4.data());
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gStallNameInput581130, x, y, 0));
            input::SetContext_44B330(&cg::ui::detail::gInputContextCB0028);
            WindowInstance* const current = gCurrentWindow;
            gFocusedElementDataD45F94 = current->elements[6].data;
            static_cast<WindowInputData*>(current->elements[6].data)->context =
                &cg::ui::detail::gInputContextCB0028;
        }
        return 1;
    }
    return 0;
}

// 原函数：0x004CDB10。
static std::int32_t WindowStall_Draw(const std::int32_t)
{
    if (gStallOwn58111C != 0) {
        {
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[9].data);
            cg::yobi::YobiAction* const source = world::detail::gPlayerRuntimeSlots.action;
            cg::yobi::YobiAction* const target = data->action;
            const std::int32_t value = cg::yobi::detail::GetYobiActionField2A8(source);
            cg::yobi::detail::SetYobiActionField2A8(target, value);
        }
        {
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[9].data);
            cg::yobi::YobiAction* const source = world::detail::gPlayerRuntimeSlots.action;
            cg::yobi::YobiAction* const target = data->action;
            const std::int32_t value = cg::yobi::detail::GetYobiActionField2B0(source);
            cg::yobi::detail::SetYobiActionField2B0(target, value);
        }
        {
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[9].data);
            cg::yobi::YobiAction* const source = world::detail::gPlayerRuntimeSlots.action;
            cg::yobi::YobiAction* const target = data->action;
            const std::int32_t value = cg::yobi::detail::GetYobiActionField2B8(source);
            cg::yobi::detail::SetYobiActionField2B8(target, value);
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
            if (world::detail::gPlayerRuntimeSlots.items[slot + 8].used == 0) {
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
            if (world::detail::gPetRuntimeRecords[slot].used == 0) {
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
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[7].data);
            const std::int32_t value = gStallBrowseData->imageNo60;
            cg::yobi::detail::SetYobiActionField2A8(data->action, value);
        }
        {
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[7].data);
            const std::int32_t value = gStallBrowseData->action64;
            cg::yobi::detail::SetYobiActionField2B0(data->action, value);
        }
        {
            auto* const data = static_cast<WindowAnimationData*>(gCurrentWindow->elements[7].data);
            const std::int32_t value = gStallBrowseData->direction68;
            cg::yobi::detail::SetYobiActionField2B8(data->action, value);
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
                WindowInstance* const window = gCurrentWindow;
                const std::int16_t y = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(window->y + 40));
                const std::int16_t x = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(window->x + 320));
                static_cast<void>(WindowItem_DrawTooltip(x, y, title.c_str(), color, lines, 8, nullptr, 0));
            }
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004CED60。
static void WindowStall_OpenBrowse(
    const std::int32_t status, const std::int32_t target,
    const char* const stallName, const char* const description,
    const char* const itemPayload, const char* const petPayload)
{
    gStallItemCountD67BCC = 0;
    gStallPetCountD67BD4 = 0;
    cg::yobi::YobiAction* const action = world::detail::FindObjectYobi_4462B0(target);
    if (status != 0 || action == nullptr) {
        switch (status) {
        case 1:
            world::detail::Sub_44B050(L"沒有可以瀏覽的攤位", 4, 0);
            break;
        case 2:
            world::detail::Sub_44B050(L"瀏覽的攤位已經收攤", 4, 0);
            break;
        case 3:
            world::detail::Sub_44B050(L"瀏覽的攤位正在忙，請稍後再試", 4, 0);
            break;
        }
        WindowInstance* const window = gWindowStates[WindowId_StallBrowse].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        return;
    }

    auto& browse = *gStallBrowseData;
    auto* const bytes = reinterpret_cast<std::byte*>(&browse);
    std::memset(bytes, 0, offsetof(StallBrowseData, items));
    for (auto& item : browse.items) { item = {}; }
    browse.itemValuesD13E74.fill(0);
    browse.itemValuesD13EC4.fill(0);
    for (world::detail::PetRuntimeRecord& pet : browse.pets) {
        pet.ClearRaw();
    }
    constexpr std::size_t tail = offsetof(StallBrowseData, petValuesD2D464);
    std::memset(bytes + tail, 0, sizeof(browse) - tail);
    std::strncpy(gStallBrowseData->sellerName00.data(), cg::yobi::detail::GetYobiName(action), static_cast<std::uint32_t>(16));
    gStallBrowseData->target5C = target;
    gStallBrowseData->imageNo60 = cg::yobi::detail::GetYobiActionField2A8(action);
    gStallBrowseData->action64 = cg::yobi::detail::GetYobiActionField2B0(action);
    gStallBrowseData->direction68 = cg::yobi::detail::GetYobiActionField2B8(action);
    std::strncpy(gStallBrowseData->stallName11.data(), stallName, static_cast<std::uint32_t>(16));
    std::strncpy(gStallBrowseData->description22.data(), description, static_cast<std::uint32_t>(56));

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
        pet.imageNo6A0 = character_select::detail::Sub_51B740(imageNo);
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
            std::strcpy(pet.name6AC.data(), petName);
        }
        network::ExtractDelimitedField_52CED0(petPayload, '|', field++, 255, petSecondName);
        input::UnescapeLegacyTextAndNotify_52D2F0(petSecondName);
        if (std::strlen(petSecondName) <= 16) {
            std::strcpy(pet.name6BD.data(), petSecondName);
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
        std::memset(pet.techniquesD8.data(), 0, sizeof(pet.techniquesD8));
        for (std::int32_t technique = 0; technique < techniqueCount; ++technique) {
            techniqueDescription[0] = '\0';
            std::memset(techniqueDescription + 1, 0, sizeof(techniqueDescription) - 1);
            const std::int32_t slot = network::ParseDelimitedInteger_52CFA0(petPayload, '|', field++);
            network::ExtractDelimitedField_52CED0(petPayload, '|', field++, 255, techniqueName);
            input::UnescapeLegacyTextAndNotify_52D2F0(techniqueName);
            auto& record = pet.techniquesD8[slot];
            if (std::strlen(techniqueName) == 0) {
                record.name[0] = '\0';
                record.description[0] = '\0';
                record.fp = 0;
                record.used = 0;
            } else {
                record.used = 1;
                record.techniqueId = 0;
                std::strcpy(record.name.data(), std::strlen(techniqueName) <= 24 ? techniqueName : "???");
                std::strcpy(record.description.data(),
                    std::strlen(techniqueDescription) <= 96 ? techniqueDescription : "???");
                record.fp = 0;
                record.field = 0;
                record.target = 0;
                record.sequence = 0;
            }
        }
        world::detail::Sub_5185C0(pet.techniquesD8.data(), pet.techniquePairs650.data(), 10);
    }
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_StallBrowse, 1, 0, nullptr));
}

// 原函数：0x004CF8A0。
static void WindowStall_BuildItemDescription(world::detail::PlayerItemRecord* const item, const char* const text)
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
static std::int32_t WindowStall_RequestPurchase()
{
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
    const std::uintptr_t socket = platform::gConnectionSocket;
    gStallPendingD67C31 = 1;
    static_cast<void>(network::Sub_5121E0(socket, value));
    constexpr wchar_t message[] = L"購買請求中";
    const auto length = static_cast<std::int32_t>(std::size(message));
    gStallMessageModeD67C34 = 0;
    gStallMessageValueD67C38 = 0;
    gStallMessageC8A978 = message;
    gCurrentWindow->elements[0].visible = 1;
    return length;
}

// 原函数：0x004CD420。
static std::int32_t WindowStall_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    const bool hovered = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hovered) {
        gHoverTextD68AE8 = L"關閉這個視窗。";
    }
    if ((state & 2) != 0 && gValueD67BE0 == 0 && gStallPendingD67C31 == 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 243000;
    if (hovered) {
        graphic->imageNo = 243002;
    }
    if ((state & 8) != 0 || gValueD67BE0 != 0 || gStallPendingD67C31 != 0) {
        graphic->imageNo = 243001;
    }
    return consumed;
}

// 原函数：0x004CDE00。
static void WindowStall_Release()
{
    if (gStallOwn58111C != 0 && gValueD67BE0 != 0) {
        static_cast<void>(network::Sub_512160(platform::gConnectionSocket));
    }
    static_cast<void>(audio::PlaySe_52C450(57, 320));
}

// 原函数：0x004CE700。
static void WindowStall_ReceiveOpenResult(const std::int32_t result)
{
    if (result != 0) {
        const wchar_t* message;
        switch (result) {
        case 1:
            message = L"已經有人在此擺攤";
            break;
        case 3:
            message = L"兩次擺攤時間間隔過短";
            break;
        case 4:
            message = L"這個子星最大攤位數已滿";
            break;
        default:
            message = L"擺攤失敗";
            break;
        }
        gStallMessageC8A978 = message;
        WindowInstance* const window = gWindowStates[WindowId_OwnStall].window;
        world::detail::ClearPlayerActionFlags(0x1000u);
        gStallMessageModeD67C34 = 1;
        gStallMessageValueD67C38 = 0;
        if (window != nullptr) {
            window->elements[0].visible = 1;
        }
        gValueD67BE0 = 0;
        gStallPendingD67C31 = 0;
        return;
    }
    char name[33];
    cg::text::StoreGbk(name, sizeof(name), L"無名店舖");
    world::detail::SetPlayerActionFlags(0x1000u);
    std::memset(name + 9, 0, 24);
    if (std::strlen(gStallNameD67BE4.data()) != 0) {
        std::sprintf(name, "%s", gStallNameD67BE4.data());
    }
    WindowInstance* const window = gWindowStates[WindowId_OwnStall].window;
    std::strcpy(world::detail::gPlayerRuntimeSlots.text15BB8.data(), name);
    if (window != nullptr) {
        window->elements[0].visible = 0;
    }
    gStallPendingD67C31 = 0;
}

// 原函数：0x004CE8B0。
static void WindowStall_ReceiveCloseResult(const std::int32_t result)
{
    if (result != 0) {
        gStallMessageC8A978 = L"收攤失敗";
    }
    world::detail::ClearPlayerActionFlags(0x1000u);
    gStallPendingD67C31 = 0;
    gValueD67BE0 = 0;
    WindowInstance* const window = gWindowStates[WindowId_OwnStall].window;
    if (result != 0) {
        gStallMessageModeD67C34 = 1;
        gStallMessageValueD67C38 = 0;
        if (window != nullptr) {
            window->elements[0].visible = 1;
        }
    } else if (window != nullptr) {
        window->elements[0].visible = 0;
    }
}

// 原函数：0x004CE930。
static void WindowStall_ReceivePurchaseResult(const std::int32_t result)
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
        WindowInstance* const window = gWindowStates[WindowId_StallBrowse].window;
        if (window != nullptr) {
            window->elements[0].visible = 0;
        }
        gStallPendingD67C31 = 0;
        return;
    }
    constexpr const wchar_t* messages[] {
        L"沒有找到老闆",
        L"老闆的攤位已經變更",
        L"您的物品空間已滿",
        L"您的寵物空間已滿",
        L"您的錢不夠",
        L"要買的東西已經售出",
        L"老闆的錢包已滿",
        L"交易失敗代碼8",
        L"交易失敗代碼9",
        L"交易失敗代碼10",
        L"交易失敗代碼11",
        L"交易失敗代碼12",
    };
    const wchar_t* const message = result >= 1 && result <= 12
        ? messages[result - 1] : L"購買失敗";
    gStallMessageC8A978 = message;
    WindowInstance* const window = gWindowStates[WindowId_StallBrowse].window;
    gStallMessageModeD67C34 = 1;
    gStallMessageValueD67C38 = 0;
    if (window != nullptr) {
        window->elements[0].visible = 1;
    }
    gStallPendingD67C31 = 0;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E7D0。
void WindowStall_ReceiveOpenReply(const std::uintptr_t, const std::int32_t result)
{
    detail::WindowStall_ReceiveOpenResult(result);
}

// 原函数：0x0050E7E0。
void WindowStall_ReceiveCloseReply(const std::uintptr_t, const std::int32_t result)
{
    detail::WindowStall_ReceiveCloseResult(result);
}

// 原函数：0x0050E7F0。
void WindowStall_ReceiveBrowseReply(
    const std::uintptr_t, const std::int32_t status, const std::int32_t target,
    const char* const stallName, const char* const description,
    const char* const itemPayload, const char* const petPayload)
{
    detail::WindowStall_OpenBrowse(status, target, stallName, description, itemPayload, petPayload);
}

// 原函数：0x0050E820。
void WindowStall_ReceivePurchaseReply(const std::uintptr_t, const std::int32_t result)
{
    detail::WindowStall_ReceivePurchaseResult(result);
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
WindowInstance* WindowStallBrowse_RequestClose()
{
    WindowInstance* const window = detail::gWindowStates[WindowId_StallBrowse].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

// 原函数：0x004CD400。
WindowInstance* WindowOwnStall_RequestClose()
{
    WindowInstance* const window = detail::gWindowStates[WindowId_OwnStall].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

} // namespace cg::ui
