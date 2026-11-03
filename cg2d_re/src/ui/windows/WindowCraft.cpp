#include "cg/network/CgConnection.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowCraftElementId : ElementId {
    DrawInvalidEngravingNotice = 1,
    DrawEmptyEngravingNotice = 2,
    ButtonCloseIcon = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    HandleEngravingInput = 7,
    Element8 = 8,
    ButtonExecute = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    HandleMaterialSlot = 16,
    HandleMaterialSlot_17 = 17,
    HandleMaterialSlot_18 = 18,
    HandleMaterialSlot_19 = 19,
    HandleMaterialSlot_20 = 20,
    HandleMaterialSlot_21 = 21,
    HandleSingleMaterial = 22,
    DrawResultImage = 23,
    DrawResultImage_24 = 24,
    Element25 = 25,
    ButtonExecute_26 = 26,
    ButtonContinue = 27,
    ButtonClose = 28,
    HandleInventory = 29,
    Element30 = 30,
    HandleSideArea = 31,
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextD2DE70 {}; // 原全局：0x00D2DE70。

// 回调与内部函数前置声明。

// 原函数：0x00484D60。
static std::int32_t WindowCraft_HandleMaterialTooltip(ElementId elementId, std::int32_t state); // 六材料槽悬停、翻页与图形绘制。

// 原函数：0x0047D4B0。
static std::int32_t WindowCraft_HasRecipeCrystalLevel(); // 晶石等级读取后比较当前配方等级。

// 原函数：0x0047D4F0。
static std::int32_t WindowCraft_HasSelectedCrystalLevel(); // 单选物品不加8，晶石读取后重新取得选择索引。

// 原函数：0x0047D530。
static std::int32_t WindowCraft_HasMaterialCrystalLevel(); // 六材料首个匹配类别，晶石读取后重取槽号再比较等级。

// 原函数：0x0047D5B0。
static std::int32_t WindowCraft_HasFirstMaterialCrystalLevel(); // 首材料slot+8等级在晶石读取前快照。

// 原函数：0x0047E1B0。
static WindowAnimationData* WindowCraft_WaitCraft(WindowInstance* window); // 制造等待模式1，按配方等级生成同源截止。

// 原函数：0x0047EF00。
static WindowAnimationData* WindowCraft_WaitItemWork(WindowInstance* window); // 单选等待模式4，刻印排除宠物辅助。

// 原函数：0x004801B0。
static WindowAnimationData* WindowCraft_WaitPacking(WindowInstance* window); // 打包等待模式13，仍读取绝对单选槽等级。

// 原函数：0x00480600。
static void WindowCraft_WaitDecoration(WindowInstance* window); // 装饰等待固定8000ms，原被AL覆盖的返回值由caller弃用。

// 原函数：0x00480C20。
static WindowAnimationData* WindowCraft_WaitRefining(WindowInstance* window); // 精炼等待模式10，一次RAND且不读物品等级。

// 原函数：0x00483EB0。
static std::int32_t WindowCraft_HandleMaterialSlot(ElementId elementId, std::int32_t state); // 六材料完整投放/换位/逐帧校验与绘制。

// 原函数：0x0047DF50。
static WindowAnimationData* WindowCraft_InitializeRecipe(WindowInstance* window); // 制造模式0完整初始化，逆序发布五材料需求。

// 原函数：0x0047FAE0。
static WindowAnimationData* WindowCraft_InitializeDecoration(WindowInstance* window); // 装饰模式6，固定魔力100。

// 原函数：0x0047FD00。
static WindowAnimationData* WindowCraft_InitializePetDecoration(WindowInstance* window); // 宠物装备装饰模式15，独立标题。

// 原函数：0x0047FF20。
static WindowAnimationData* WindowCraft_InitializePacking(WindowInstance* window); // 模式12材料类别文本与完整六材料交互。

// 原函数：0x00480A00。
static WindowAnimationData* WindowCraft_InitializeRefining(WindowInstance* window); // 精炼模式9，采样同源计时并按发送模式配置动作。

// 原函数：0x00483E30。
static void WindowCraft_ReleaseMaterialSlot(
    std::int32_t destination, std::int32_t source, std::int32_t material); // 先释放再发布材料槽/占用。

// 原函数：0x004880D0。
static std::int32_t WindowCraft_ReadRequiredSkillLevel(std::int32_t skillId, std::int32_t extraLevel); // 完整快照后查技能等级，附加值只读低U8。

// 原函数：0x00488160。
static std::int32_t WindowCraft_ReadMaxMaterialLevel(); // 前三材料占用等级与0的最大值。

// 原函数：0x00488180。
static std::int32_t WindowCraft_ReadFirstMaterialId(); // 六槽中首个材料类别0/1/2对应的itemId。

// 原函数：0x004881D0。
static void WindowCraft_ValidatePackingMaterials(std::int32_t destination, std::int32_t source); // 模式12完整材料校验、清理与发布。

// 原函数：0x0047DE00。
static void WindowCraft_HandleMaterialDrag(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // 材料/单选拖动与释放。

// 原函数：0x0047DF30。
static std::int32_t WindowCraft_StartMaterialDrag(std::int32_t sourceIndex); // 安装类型10的47DE00拖动回调。

// 原函数：0x00482270。
static std::int32_t WindowCraft_ButtonExecute(ElementId elementId, std::int32_t state); // 执行资格、五等待及刻印直接发送。

// 原函数：0x00482FA0。
static std::int32_t WindowCraft_ButtonContinue(ElementId elementId, std::int32_t state); // 续作，初始化后独立重读模式5。

// 原函数：0x004830B0。
static std::int32_t WindowCraft_ButtonClose(ElementId elementId, std::int32_t state); // 底部关闭，点击仍返回0。

// 原函数：0x00483150。
static std::int32_t WindowCraft_HandleInventory(ElementId elementId, std::int32_t state); // 20格背包投放/快速选择/拖动及绘制。

// 原函数：0x00484FB0。
static std::int32_t WindowCraft_DrawResultImage(ElementId elementId, std::int32_t unusedState); // 结果图，负图号不绘制。

// 原函数：0x00485030。
static std::int32_t WindowCraft_HandleEngravingInput(ElementId elementId, std::int32_t state); // 刻印输入焦点，借用唯一D2DE70。

// 原函数：0x00485160。
static std::int32_t WindowCraft_ButtonCloseIcon(ElementId elementId, std::int32_t state); // 右上关闭，点击返回1。

// 原函数：0x00485200。
static std::int32_t WindowCraft_DrawInvalidEngravingNotice(ElementId elementId, std::int32_t unusedState); // 非法刻印字符覆盖提示。

// 原函数：0x00485350。
static std::int32_t WindowCraft_DrawEmptyEngravingNotice(ElementId elementId, std::int32_t unusedState); // 空白刻印覆盖提示。

// 原函数：0x004847C0。
static std::int32_t WindowCraft_HandleSingleMaterial(ElementId elementId, std::int32_t state); // 单选投放/校验/拖动与绘制。

// 原函数：0x0047EC00。
static WindowAnimationData* WindowCraft_InitializeItemWork(WindowInstance* window); // 单选模式3完整初始化，返回借用载荷。

// 原函数：0x0047E5E0。
static WindowAnimationData* WindowCraft_ShowRecipeResult(WindowInstance* window); // 制造结果完整窗口11展示。

// 原函数：0x0047EBE0。
static WindowAnimationData* WindowCraft_RefreshRecipeResult(); // 仅已有窗口11才更新，不自动创建。

// 原函数：0x00484E90。
static std::int32_t WindowCraft_HandleItemTooltip(ElementId elementId, std::int32_t state); // 单物品悬停/翻页，物品编号不加8。

// 原函数：0x0047F3D0。
static WindowAnimationData* WindowCraft_ShowItemWorkResult(WindowInstance* window); // 物品加工结果展示，保留入口选择快照及交叉门禁。

// 原函数：0x0047FAC0。
static WindowAnimationData* WindowCraft_RefreshItemWorkResult(); // 仅已有窗口11才展示物品结果。

// 原函数：0x0047DC20。
static std::int32_t WindowCraft_ReleaseMaterials(std::int32_t index); // 六槽/单选两路释放，单选尾接聊天恢复。

// 原函数：0x00481010。
static void WindowCraft_Release();

// 原函数：0x004810A0。
static std::int32_t WindowCraft_Update(std::int32_t value);

// 原函数：0x00482000。
static std::int32_t WindowCraft_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::uint8_t gSkillPairIndexD67AD8 = 0; // 原全局：0x00D67AD8。 配方查询按S8解释。

static std::int32_t gRecipeIndex5846BC = -1; // 原全局：0x005846BC。 原装载初值-1。

// 原全局：0x00D67B00。 原数值状态，47D490清零；非地址。
static std::int32_t gCraftPetAssistActive_D67B00 = 0; // 原全局：0x00D67B00。

static std::array<std::int32_t, 6> gSelectedSlotsCA0A90 {}; // 原全局：0x00CA0A90。

static std::array<std::int32_t, 6> gMaterialIndicesCA0458 {}; // 原全局：0x00CA0458。

static std::array<std::int32_t, 6> gSelectionCountsD66E0C {}; // 原全局：0x00D66E0C。

static std::array<std::int32_t, 6> gSelectionImagesCF3FF4 {}; // 原全局：0x00CF3FF4。

static std::int32_t gSelectedItemD460C0 = 0; // 原全局：0x00D460C0。

static std::int32_t gSelectedImageCC3ED8 = 0; // 原全局：0x00CC3ED8。

static std::int32_t gSelectedCountCC483C = 0; // 原全局：0x00CC483C。

static std::uint8_t gSelectionModeD2D650 = 0; // 原全局：0x00D2D650。

static std::array<std::uint8_t, 28> gItemMarksD67290 {}; // 原全局：0x00D67290。

static std::array<std::uint8_t, 20> gItemMarksD68A90 {}; // 原全局：0x00D68A90。 不为原越界访问扩展为28B。

static std::int32_t gCraftBackpackHover57DA24 = -2; // 原全局：0x0057DA24。 483150唯一读写，区别于材料/单选悬停槽。

static std::int32_t gCraftRecipeIdD2E440 = 0; // 原全局：0x00D2E440。 主更新重定位配方使用的原id快照。

static std::uint64_t gCraftDeadlineCF3B04 = 0; // 原全局：0x00CF3B04。 CTM01制作等待阶段的同源64位截止值。

static std::int32_t gCraftResultCC46D8 = 0; // 原全局：0x00CC46D8。

static std::int32_t gCraftExperienceCC3204 = 0; // 原全局：0x00CC3204。

static std::int32_t gCraftValueCC4840 = 0; // 原全局：0x00CC4840。

static std::int32_t gCraftLevelUpCF4974 = 0; // 原全局：0x00CF4974。

static std::int32_t gCraftEnduranceD2F328 = 0; // 原全局：0x00D2F328。

static std::int32_t gCraftDexterityCA0448 = 0; // 原全局：0x00CA0448。

static std::int32_t gCraftIntelligenceCB0F84 = 0; // 原全局：0x00CB0F84。

static std::int32_t gCraftInjuryD2EA78 = 0; // 原全局：0x00D2EA78。

static std::int32_t gCraftResultImageCF4B60 = 0; // 原全局：0x00CF4B60。

static std::int32_t gCraftDisplayImageCB2204 = 0; // 原全局：0x00CB2204。

static std::wstring gCraftNameCF46D4 {}; // 原全局：0x00CF46D4。

static std::wstring gCraftResultSuffixD672B4; // 原全局：0x00D672B4。

// 只承接已证NUL文本合同；原固定容量未证，不猜数组上界或保留邻区。
static std::wstring gCraftResultDetailsD2E92C; // 原全局：0x00D2E92C。

static std::array<std::int32_t, 6> gMaterialHoverSlotsCB0A8C {}; // 原全局：0x00CB0A8C。

static std::int32_t gMaterialHintPageCF4FD8 = 0; // 原全局：0x00CF4FD8。

static std::int32_t gMaterialHoveredItemD67424 = 0; // 原全局：0x00D67424。

// 原57D7F8：单选加工输入原型，只借用Input拥有的D2DE70。
static WindowInputPrototype gCraftInputPrototype57D7F8 { // 原全局：0x0057D7F8。
    {{&cg::ui::detail::gInputContextD2DE70}, WindowInputReferenceKind::Context},
    0, 0, 3, 12, 127, {}, 1, 16, 0, 0,
};

static std::int32_t gItemWorkResultCB05FC = 0; // 原全局：0x00CB05FC。

static std::int32_t gItemWorkExperienceC8A950 = 0; // 原全局：0x00C8A950。

static std::int32_t gItemWorkValueD2E2AC = 0; // 原全局：0x00D2E2AC。

static std::int32_t gItemWorkLevelUpCB03D8 = 0; // 原全局：0x00CB03D8。

static std::int32_t gItemWorkEnduranceCF46B4 = 0; // 原全局：0x00CF46B4。

static std::int32_t gItemWorkDexterityCB0A14 = 0; // 原全局：0x00CB0A14。

static std::int32_t gItemWorkIntelligenceD30418 = 0; // 原全局：0x00D30418。

static std::int32_t gItemWorkInjuryD2E2A8 = 0; // 原全局：0x00D2E2A8。

static std::int32_t gItemWorkSkillIdD67CF0 = 0; // 原全局：0x00D67CF0。

static std::wstring gItemWorkSuffixD2DADC; // 原全局：0x00D2DADC。

static std::wstring gItemWorkDetailsD2DB00; // 原全局：0x00D2DB00。 独立NUL文本合同，与制造详情不共用。

static std::int32_t gMaterialSingleHoverCB0AA4 = 0; // 原全局：0x00CB0AA4。

static constexpr std::array<std::array<std::int16_t, 3>, 6> kSkillPreviewPoints57D7D0 {{ // 原常量参考：0x0057D7D0。
    {30, -45, 0}, {-30, -45, 7}, {-50, -20, 6},
    {-35, 22, 4}, {35, 22, 3}, {50, -20, 2},
}};

static WindowGraphicData gWindowGraphic561FE0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561FE0。

static WindowGraphicData gWindowGraphic561FF8 {0x3BA38, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561FF8。

static WindowGraphicData gWindowGraphic562010 {0x3BA3A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562010。

static WindowGraphicData gWindowGraphic562028 {0x3BA3B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562028。

static WindowGraphicData gWindowGraphic562040 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562040。

static WindowGraphicData gWindowGraphic562058 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562058。

static WindowGraphicData gWindowGraphic562070 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562070。

static WindowGraphicData gWindowGraphic562088 {0x3BA3C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562088。

static WindowGraphicData gWindowGraphic5620A0 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005620A0。

static WindowGraphicData gWindowGraphic5620B8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005620B8。

static WindowTextPrototype gWindowText5620D0 { // 原全局：0x005620D0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？　　000", // 原常量参考：0x00586078。
};

static std::int32_t gWindowAnimationImage5620D8 = 0x1AF16; // 原全局：0x005620D8。

static std::int32_t gWindowAnimationImage5620DC = 0x1AF16; // 原全局：0x005620DC。

static WindowElementPrototype gWindowElements5620E0[31] { // 原全局：0x005620E0。
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowCraft_DrawInvalidEngravingNotice, -1, static_cast<ElementId>(WindowCraftElementId::DrawInvalidEngravingNotice)},
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowCraft_DrawEmptyEngravingNotice, -1, static_cast<ElementId>(WindowCraftElementId::DrawEmptyEngravingNotice)},
    {WindowElementType_Graphic, 543, 8, 11, 11, 1, {.graphic = &gWindowGraphic561FE0}, &WindowCraft_ButtonCloseIcon, -1, static_cast<ElementId>(WindowCraftElementId::ButtonCloseIcon)},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element4)},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element5)},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element6)},
    {WindowElementType_EventArea, 20, 57, 150, 12, 1, {.unusedValue = 0}, &WindowCraft_HandleEngravingInput, -1, static_cast<ElementId>(WindowCraftElementId::HandleEngravingInput)},
    {WindowElementType_Input, 20, 53, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element8)},
    {WindowElementType_Graphic, 190, 54, 49, 17, 1, {.graphic = &gWindowGraphic5620B8}, &WindowCraft_ButtonExecute, -1, static_cast<ElementId>(WindowCraftElementId::ButtonExecute)},
    {WindowElementType_Text, 20, 29, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element10)},
    {WindowElementType_Text, 20, 43, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element11)},
    {WindowElementType_Text, 20, 57, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element12)},
    {WindowElementType_Text, 20, 71, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element13)},
    {WindowElementType_Text, 20, 85, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element14)},
    {WindowElementType_Graphic, 20, 29, 0, 0, 1, {.graphic = &gWindowGraphic562010}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element15)},
    {WindowElementType_EventArea, 190, 75, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot)},
    {WindowElementType_EventArea, 190, 125, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot_17)},
    {WindowElementType_EventArea, 190, 175, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot_18)},
    {WindowElementType_EventArea, 243, 75, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot_19)},
    {WindowElementType_EventArea, 243, 125, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot_20)},
    {WindowElementType_EventArea, 243, 175, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot, -1, static_cast<ElementId>(WindowCraftElementId::HandleMaterialSlot_21)},
    {WindowElementType_Graphic, 231, 175, 48, 48, 1, {.graphic = &gWindowGraphic5620A0}, &WindowCraft_HandleSingleMaterial, -1, static_cast<ElementId>(WindowCraftElementId::HandleSingleMaterial)},
    {WindowElementType_Graphic, 190, 27, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &WindowCraft_DrawResultImage, -1, static_cast<ElementId>(WindowCraftElementId::DrawResultImage)},
    {WindowElementType_Graphic, 231, 45, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &WindowCraft_DrawResultImage, -1, static_cast<ElementId>(WindowCraftElementId::DrawResultImage_24)},
    {WindowElementType_Graphic, 188, 73, 0, 0, 1, {.graphic = &gWindowGraphic562028}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element25)},
    {WindowElementType_Graphic, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562040}, &WindowCraft_ButtonExecute, -1, static_cast<ElementId>(WindowCraftElementId::ButtonExecute_26)},
    {WindowElementType_Graphic, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562058}, &WindowCraft_ButtonContinue, -1, static_cast<ElementId>(WindowCraftElementId::ButtonContinue)},
    {WindowElementType_Graphic, 126, 209, 49, 17, 1, {.graphic = &gWindowGraphic562070}, &WindowCraft_ButtonClose, -1, static_cast<ElementId>(WindowCraftElementId::ButtonClose)},
    {WindowElementType_EventArea, 307, 28, 248, 204, 1, {.unusedValue = 0}, &WindowCraft_HandleInventory, -1, static_cast<ElementId>(WindowCraftElementId::HandleInventory)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561FF8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowCraftElementId::Element30)},
    {WindowElementType_EventArea, 566, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowCraftElementId::HandleSideArea)},
};

// 原型：窗口11；原全局：0x0054E450。
WindowPrototype gWindowPrototypeCraft {0, 4, 37, 102, 563, 239, 0x80010101u, 31, gWindowElements5620E0, &WindowCraft_Update, &WindowCraft_Draw, &WindowCraft_Release}; // 11: 54E450

// 窗口函数实现。

// 原函数：0x004F4630。
std::int32_t WindowCraft_CountRecipeMaterial(const std::int32_t itemId)
{
    const std::int8_t pairIndex = std::bit_cast<std::int8_t>(gSkillPairIndexD67AD8);
    if (pairIndex < 0 || pairIndex >= world::kSkillSlotCount) return 0;
    const std::int16_t skillIndex = std::bit_cast<std::int16_t>(world::gSkillPairsF63370[pairIndex].index);
    if (skillIndex < 0 || skillIndex >= world::kSkillSlotCount) return 0;
    const auto& recipes = world::gSkillsF1BAFC[skillIndex].recipes8E8;
    const std::int32_t recipeIndex = gRecipeIndex5846BC;
    // IWT01：未选择时不再将技术区解释为配方；有效索引直接按材料结构读取。
    if (recipeIndex < 0 || recipeIndex >= static_cast<std::int32_t>(recipes.size())) return 0;
    for (const auto& item : recipes[recipeIndex].items6C) {
        if (item.itemId == itemId) return item.count;
    }
    return 0;
}

// 原函数：0x0047DC20。
static std::int32_t WindowCraft_ReleaseMaterials(const std::int32_t index)
{
    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    if (index < 6) {
        const std::int32_t slot = gSelectedSlotsCA0A90[index];
        if (slot < 0) {
            return slot;
        }
        const std::uint8_t mode = gSelectionModeD2D650;
        if (mode == 6 || mode == 15) {
            gItemMarksD68A90[slot] = 0;
        }
        const std::int32_t material = gMaterialIndicesCA0458[index];
        if (material >= 0) {
            gMaterialOccupancyCC4894.materialValues[material] = 0;
        }
        hover[slot + 8] = static_cast<unsigned char>(hover[slot + 8] - 1);
        gItemMarksD67290[slot + 8] = 0;
        gSelectionCountsD66E0C[index] = 0;
        gSelectedSlotsCA0A90[index] = -1;
        gSelectionImagesCF3FF4[index] = -1;
        gMaterialIndicesCA0458[index] = -1;
        return -1;
    }
    const std::int32_t slot = gSelectedItemD460C0;
    if (slot < 0) {
        return slot;
    }
    const unsigned char previous = hover[slot];
    gItemMarksD67290[slot] = 0;
    hover[slot] = static_cast<unsigned char>(previous - 1);
    gSelectedItemD460C0 = -1;
    gSelectedImageCC3ED8 = -1;
    gSelectedCountCC483C = -1;
    return WindowChat_RestoreInput();
}

// 原函数：0x0047DCD0。
void WindowCraft_ResetSelection(const std::int32_t first, const std::int32_t second)
{
    if (WindowRuntime_States()[WindowId_Craft].window == nullptr) {
        return;
    }
    const std::int32_t firstSlot = first - 8;
    const std::int32_t secondSlot = second - 8;
    for (std::int32_t index = 0; index < 6; ++index) {
        const std::int32_t slot = gSelectedSlotsCA0A90[index];
        if (slot < 0) {
            continue;
        }
        if (slot == firstSlot) {
            if (secondSlot < 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(index));
            } else {
                gSelectedSlotsCA0A90[index] = secondSlot;
            }
        } else if (slot == secondSlot) {
            if (firstSlot < 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(index));
            } else {
                gSelectedSlotsCA0A90[index] = firstSlot;
            }
        }
    }
    const std::int32_t selected = gSelectedItemD460C0;
    if (selected == first) {
        gSelectedItemD460C0 = second;
        const std::int32_t imageNo = world::gPlayerRuntimeSlots.items[second].imageNo;
        // 原47DD63使用Items.itemId+62E0，确与另一分支的基址相差八条。
        const std::int32_t itemId = world::gPlayerRuntimeSlots.items[second + 8].itemId;
        gSelectedImageCC3ED8 = imageNo;
        gSelectedCountCC483C = WindowCraft_CountRecipeMaterial(itemId);
    } else if (selected == second) {
        gSelectedItemD460C0 = first;
        const std::int32_t imageNo = world::gPlayerRuntimeSlots.items[first].imageNo;
        const std::int32_t itemId = world::gPlayerRuntimeSlots.items[first].itemId;
        gSelectedImageCC3ED8 = imageNo;
        gSelectedCountCC483C = WindowCraft_CountRecipeMaterial(itemId);
    }
    if (first < 0 || second < 0) {
        return;
    }
    const std::uint8_t firstMark = gItemMarksD67290[first];
    const std::uint8_t secondMark = gItemMarksD67290[second];
    gItemMarksD67290[first] = secondMark;
    gItemMarksD67290[second] = firstMark;
    const std::uint8_t mode = gSelectionModeD2D650;
    if (mode == 6 || mode == 15) {
        const std::uint8_t firstValue = gItemMarksD68A90[first];
        const std::uint8_t secondValue = gItemMarksD68A90[second];
        gItemMarksD68A90[first] = secondValue;
        gItemMarksD68A90[second] = firstValue;
    }
}

// 原函数：0x0047D490。
void WindowCraft_ResetItemSelection()
{
    gCraftPetAssistActive_D67B00 = 0;
}

// 原函数：0x0047D4A0。
std::int32_t WindowCraft_ReadItemSelection()
{
    return gCraftPetAssistActive_D67B00;
}

// 装饰/精炼无18A26回退，也不在末区间命中后重读玩家图号。
static void WindowCraft_SetWorkingAction(WindowInstance* const window, const bool specialImageFallback = true)
{
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 0;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, 6);
    }
    std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
    for (std::size_t index = 0; index < kCraftAlternateActionImages.size(); ++index) {
        const auto& range = kCraftAlternateActionImages[index];
        if (imageNo >= range[0] && imageNo <= range[1]) {
            auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
            if (element53) {
                cg::yobi::SetYobiActionField2B0(
                    element53->data.animation->action, 16);
            }
            if (specialImageFallback || index + 1 < kCraftAlternateActionImages.size()) {
                imageNo = world::gPlayerRuntimeSlots.imageNo;
            }
        }
    }
    if (specialImageFallback && imageNo == 0x18A26) {
        auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
        if (element53) {
            cg::yobi::SetYobiActionField2B0(
                element53->data.animation->action, 6);
        }
    }
}

// 五等待入口共有的倍率/通知/双tick尾；调用者在本函数返回后才发布模式。
static WindowAnimationData* WindowCraft_FinishWait(WindowInstance* const window, std::uint32_t duration)
{
    const std::uint8_t rebirthMode = gSkillRebirthModeEnabled_5F9754;
    gCraftDeadlineCF3B04 = duration;
    if (rebirthMode != 0) {
        const std::int32_t rebirth = world::gPlayerRuntimeSlots.value08;
        duration = duration * kSkillRebirthRates57D7A4[rebirth] / 100u;
        gCraftDeadlineCF3B04 = duration;
    }
    if (gSkillSendModeC8A970 != 0) {
        const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
        duration = static_cast<std::uint32_t>(gCraftDeadlineCF3B04) * kSkillCrystalRates57D7B4[crystalLevel] / 100u;
        gCraftDeadlineCF3B04 = duration;
    }
    static_cast<void>(cg::network::GetGameConnection().SendCraftDuration(static_cast<std::int32_t>(duration / 1000u)));
    const std::uint64_t tick = platform::ReadTickCount64();
    gCraftDeadlineCF3B04 = gCraftDeadlineCF3B04 + tick;
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    gCraftDisplayImageCB2204 = -1;
    gSkillPreviewTickCF46C8 = platform::ReadTickCount64();
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    if (gSkillSendModeC8A970 != 0) {
        return WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    }
    return WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 0, 10, 5);
}

// 仅新补的制造/单选/装饰创建入口共用；材料模式12另有无条件状态写。
static void WindowCraft_SelectState(const std::int32_t skillId)
{
    std::int32_t state = 9;
    switch (skillId) {
    case 215: state = 10; break;
    case 219: case 2033: state = 11; break;
    case 217: case 218: case 2031: case 2032: state = 12; break;
    default: break;
    }
    if (world::GetPlayerStatusIcon_51BEC0() != state) {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(state));
    }
}

// 五创建入口的原关闭顺序；只标关闭，不同步释放或撤回已递增计数。
static cg::yobi::YobiAction* WindowCraft_Create()
{
    for (const std::int32_t id : {11, 12, 45, 46, 13}) {
        WindowInstance* const window = WindowRuntime_States()[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowRuntime_CreateAnimation(WindowId_Craft, 1, 0, nullptr);
}

// 仅模式6/15/12/9共有的原释放/清表序列，进入时仍使用旧模式释放。
static void WindowCraft_ResetMaterialMode()
{
    for (std::int32_t index = 0; index < 6; ++index) {
        static_cast<void>(WindowCraft_ReleaseMaterials(index));
    }
    static_cast<void>(WindowCraft_ReleaseMaterials(6));
    gItemMarksD67290.fill(0);
    gItemMarksD68A90.fill(0);
    gMaterialHoverSlotsCB0A8C.fill(-2);
    gMaterialSingleHoverCB0AA4 = -2;
    gMaterialOccupancyCC4894 = {};
    gMaterialIndicesCA0458.fill(-1);
    gSelectedSlotsCA0A90.fill(-1);
    gSelectionCountsD66E0C.fill(0);
    gSelectionImagesCF3FF4.fill(-1);
}

// 四新增模式共用的元素写序；计时、动作创建与模式发布仍留在各原入口。
static void WindowCraft_PrepareMaterialElements(WindowInstance* const window)
{
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialSlot;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 0;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, 0);
    }
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    gCraftDisplayImageCB2204 = -1;
}

// 原函数：0x0047E1B0。
static WindowAnimationData* WindowCraft_WaitCraft(WindowInstance* const window)
{
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowCraft_HasRecipeCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        gSkillSendModeC8A970 = 1;
    }
    gCraftPetAssistActive_D67B00 = gSkillSendModeC8A970;
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"　　　...　作成中　..." /* 原常量参考：0x005865C4。 */;
    for (std::int32_t index = 10; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    WindowCraft_SetWorkingAction(window);
    const std::int32_t random = platform::Rand16() % 6;
    const std::int32_t skill = gCraftSkillIndexCC2368;
    const std::int32_t recipe = gCraftRecipeIndexD2D638;
    const std::int32_t level = world::gSkillsF1BAFC[skill].recipes8E8[recipe].level;
    const auto duration = static_cast<std::uint32_t>(random + level * 3 + 5) * 1000u;
    WindowAnimationData* const result = WindowCraft_FinishWait(window, duration);
    gSelectionModeD2D650 = 1;
    return result;
}

// 原函数：0x0047EF00。
static WindowAnimationData* WindowCraft_WaitItemWork(WindowInstance* const window)
{
    gSkillSendModeC8A970 = 0;
    std::int32_t skill;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowCraft_HasSelectedCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        skill = gCraftSkillIndexCC2368;
        if (world::gSkillsF1BAFC[skill].skillId != 220) {
            gSkillSendModeC8A970 = 1;
        }
    } else {
        skill = gCraftSkillIndexCC2368;
    }
    const std::int32_t active = gSkillSendModeC8A970;
    const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
    gCraftPetAssistActive_D67B00 = active;
    const wchar_t* hint = nullptr;
    switch (skillId) {
    case 220: hint = L"　　　…　刻印中　…" /* 原常量参考：0x005866DC。 */; break;
    case 2025: hint = L"　　　…　精炼中　…" /* 原常量参考：0x005866C4。 */; break;
    case 219: case 2033: hint = L"　　　…　鉴定中　…" /* 原常量参考：0x005866AC。 */; break;
    case 217: case 218: case 2031: case 2032: hint = L"　　　…　修理中　…" /* 原常量参考：0x00586694。 */; break;
    default: break;
    }
    if (hint != nullptr) {
        if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = hint;
    }
    for (std::int32_t index = 10; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA39;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 0;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 1;
    if (auto* const handleSingleMaterialElement2 = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement2) handleSingleMaterialElement2->mouseCallback = WindowCraft_HandleItemTooltip;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 1;
    if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
        if (auto* const drawResultImage_24Element2 = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element2) drawResultImage_24Element2->visible = 0;
    }
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    WindowCraft_SetWorkingAction(window);
    const std::int32_t random = platform::Rand16() % 6;
    const std::int32_t selected = gSelectedItemD460C0;
    const std::int32_t level = world::gPlayerRuntimeSlots.items[selected].level;
    const auto duration = static_cast<std::uint32_t>(random + level * 3 + 5) * 1000u;
    WindowAnimationData* const result = WindowCraft_FinishWait(window, duration);
    gSelectionModeD2D650 = 4;
    return result;
}

// 原函数：0x004801B0。
static WindowAnimationData* WindowCraft_WaitPacking(WindowInstance* const window)
{
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowCraft_HasSelectedCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0
        && world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId != 2026) {
        gSkillSendModeC8A970 = 1;
    }
    gCraftPetAssistActive_D67B00 = gSkillSendModeC8A970;
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"　　　…　打包中　…" /* 原常量参考：0x00586830。 */;
    for (std::int32_t index = 10; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA39;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 0;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 1;
    if (auto* const handleSingleMaterialElement2 = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement2) handleSingleMaterialElement2->mouseCallback = WindowCraft_HandleItemTooltip;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 1;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    WindowCraft_SetWorkingAction(window);
    const std::int32_t random = platform::Rand16() % 6;
    const std::int32_t selected = gSelectedItemD460C0;
    const std::int32_t level = world::gPlayerRuntimeSlots.items[selected].level;
    const auto duration = static_cast<std::uint32_t>(random + level * 3 + 5) * 1000u;
    WindowAnimationData* const result = WindowCraft_FinishWait(window, duration);
    gSelectionModeD2D650 = 13;
    return result;
}

// 原函数：0x00480600。
static void WindowCraft_WaitDecoration(WindowInstance* const window)
{
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowCraft_HasMaterialCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        gSkillSendModeC8A970 = 1;
    }
    gCraftPetAssistActive_D67B00 = gSkillSendModeC8A970;
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"　　　…　装饰中　…" /* 原常量参考：0x00586848。 */;
    for (std::int32_t index = 10; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    WindowCraft_SetWorkingAction(window, false);
    static_cast<void>(WindowCraft_FinishWait(window, 8000));
    const std::uint8_t mode = gSelectionModeD2D650;
    if (mode == 6) {
        gSelectionModeD2D650 = 7;
    } else if (mode == 15) {
        gSelectionModeD2D650 = 16;
    }
}

// 原函数：0x00480C20。
static WindowAnimationData* WindowCraft_WaitRefining(WindowInstance* const window)
{
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowCraft_HasFirstMaterialCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        gSkillSendModeC8A970 = 1;
    }
    gCraftPetAssistActive_D67B00 = gSkillSendModeC8A970;
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"　　　…　精炼中　…" /* 原常量参考：0x005866C4。 */;
    for (std::int32_t index = 10; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3E;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    WindowCraft_SetWorkingAction(window, false);
    const auto duration = static_cast<std::uint32_t>(platform::Rand16() % 6 + 5) * 1000u;
    WindowAnimationData* const result = WindowCraft_FinishWait(window, duration);
    gSelectionModeD2D650 = 10;
    return result;
}

// 原函数：0x0047D5F0。
cg::yobi::YobiAction* WindowCraft_OpenRecipe(
    const std::int32_t skill, const std::int32_t recipe, const std::int32_t request)
{
    gCraftSkillIndexCC2368 = skill;
    gCraftRequestValueD30400 = request;
    gCraftRecipeIndexD2D638 = recipe;
    gSelectionModeD2D650 = 0;
    gCraftRecipeIdD2E440 = world::gSkillsF1BAFC[skill].recipes8E8[recipe].itemId;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const std::int32_t currentSkill = gCraftSkillIndexCC2368;
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillOperationCount_D67AFD = references;
    const std::int32_t skillId = world::gSkillsF1BAFC[currentSkill].skillId;
    gSkillStatusIconReferences_D67AFE = skillReferences;
    WindowCraft_SelectState(skillId);
    return WindowCraft_Create();
}

// 原函数：0x0047D780。
cg::yobi::YobiAction* WindowCraft_OpenItemWork(const std::int32_t skill, const std::int32_t request)
{
    gCraftRequestValueD30400 = request;
    gCraftSkillIndexCC2368 = skill;
    gSelectionModeD2D650 = 3;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillStatusIconReferences_D67AFE = skillReferences;
    const std::int32_t currentSkill = gCraftSkillIndexCC2368;
    gSkillOperationCount_D67AFD = references;
    WindowCraft_SelectState(world::gSkillsF1BAFC[currentSkill].skillId);
    return WindowCraft_Create();
}

// 原函数：0x0047D8F0。
cg::yobi::YobiAction* WindowCraft_OpenDecoration(
    const std::int32_t skill, const std::int32_t request, const std::int32_t kind)
{
    gCraftSkillIndexCC2368 = skill;
    gCraftRequestValueD30400 = request;
    gSelectionModeD2D650 = 6;
    if (kind == 1) {
        gSelectionModeD2D650 = 15;
    }
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillStatusIconReferences_D67AFE = skillReferences;
    const std::int32_t currentSkill = gCraftSkillIndexCC2368;
    gSkillOperationCount_D67AFD = references;
    WindowCraft_SelectState(world::gSkillsF1BAFC[currentSkill].skillId);
    return WindowCraft_Create();
}

// 原函数：0x0047DA70。
cg::yobi::YobiAction* WindowCraft_OpenRefining(const std::int32_t skill, const std::int32_t request)
{
    gCraftRequestValueD30400 = request;
    gCraftSkillIndexCC2368 = skill;
    gSelectionModeD2D650 = 9;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillOperationCount_D67AFD = references;
    gSkillStatusIconReferences_D67AFE = skillReferences;
    if (world::GetPlayerStatusIcon_51BEC0() != 9) {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(9));
    }
    return WindowCraft_Create();
}

// 原函数：0x0047DB50。
cg::yobi::YobiAction* WindowCraft_OpenMaterials(const std::int32_t skill, const std::int32_t request)
{
    gCraftRequestValueD30400 = request;
    gCraftSkillIndexCC2368 = skill;
    gSelectionModeD2D650 = 12;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillOperationCount_D67AFD = references;
    gSkillStatusIconReferences_D67AFE = skillReferences;
    static_cast<void>(world::RequestPlayerStatusIcon_51BE50(9));
    return WindowCraft_Create();
}

// 原函数：0x0047FAE0。
static WindowAnimationData* WindowCraft_InitializeDecoration(WindowInstance* const window)
{
    WindowCraft_ResetMaterialMode();
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"装饰" /* 原常量参考：0x00586784。 */;
    auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr;
    if (element11) {
        cg::text::FormatTo(element11->data.text->text, L"                     魔力{:3}" /* 原常量参考：0x0058678C。 */, 100);
    }
    for (std::int32_t index = 11; index <= 13; ++index) {
        auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
    }
    WindowCraft_PrepareMaterialElements(window);
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    gSelectionModeD2D650 = 6;
    return result;
}

// 原函数：0x0047FD00。
static WindowAnimationData* WindowCraft_InitializePetDecoration(WindowInstance* const window)
{
    WindowCraft_ResetMaterialMode();
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"装饰宠物装备" /* 原常量参考：0x005867AC。 */;
    auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr;
    if (element11) {
        cg::text::FormatTo(element11->data.text->text, L"                     魔力{:3}" /* 原常量参考：0x0058678C。 */, 100);
    }
    for (std::int32_t index = 11; index <= 13; ++index) {
        auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
    }
    WindowCraft_PrepareMaterialElements(window);
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    gSelectionModeD2D650 = 15;
    return result;
}

// 原函数：0x0047FF20。
static WindowAnimationData* WindowCraft_InitializePacking(WindowInstance* const window)
{
    WindowCraft_ResetMaterialMode();
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"请选择制作材料。" /* 原常量参考：0x0058681C。 */;
    if (auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr; element11) element11->data.text->text = L"                   料理/药水" /* 原常量参考：0x005867FC。 */;
    if (auto* const element12 = window ? window->FindElementById(WindowCraftElementId::Element12) : nullptr; element12) element12->data.text->text = L"                        木材" /* 原常量参考：0x005867DC。 */;
    if (auto* const element13 = window ? window->FindElementById(WindowCraftElementId::Element13) : nullptr; element13) element13->data.text->text = L"                        布料" /* 原常量参考：0x005867BC。 */;
    if (auto* const element14 = window ? window->FindElementById(WindowCraftElementId::Element14) : nullptr; element14) element14->data.text->text = L"";
    WindowCraft_PrepareMaterialElements(window);
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    gSelectionModeD2D650 = 12;
    return result;
}

// 原函数：0x00480A00。
static WindowAnimationData* WindowCraft_InitializeRefining(WindowInstance* const window)
{
    WindowCraft_ResetMaterialMode();
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = L"精炼" /* 原常量参考：0x00586860。 */;
    for (std::int32_t index = 10; index <= 13; ++index) {
        auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
    }
    WindowCraft_PrepareMaterialElements(window);
    gSkillPreviewTickCF46C8 = platform::ReadTickCount64();
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = gSkillSendModeC8A970 != 0
        ? WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5)
        : WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 0, 10, 5);
    gSelectionModeD2D650 = 9;
    return result;
}

// 原函数：0x00483EB0。
static std::int32_t WindowCraft_HandleMaterialSlot(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const std::int32_t destination = index - 15;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 10 && WindowDrag_ReadKind() != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        if (WindowDrag_ReadKind() == 2) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            // 各模式早退汇合到同一次InvDragReset，保留释放与占用写入的不同顺序。
            const auto receiveInventory = [&] {
                const std::uint8_t mode = gSelectionModeD2D650;
                if (mode == 0) {
                    if (source < 8 || source >= 100) {
                        return;
                    }
                    const auto* const item = &world::gPlayerRuntimeSlots.items[source];
                    if (item->hasDescription == 0) {
                        return;
                    }
                    std::int32_t count = item->count;
                    if (count == 0) {
                        count = 1;
                    }
                    std::int32_t skill = gCraftSkillIndexCC2368;
                    const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
                    if (skillId != 215 && skillId != 216) {
                        if (WindowCraft_ClassifySpecialMaterial(item->itemType, skillId) != 0) {
                            for (std::int32_t position = 0; position < 6; ++position) {
                                const std::int32_t selected = gSelectedSlotsCA0A90[position];
                                if (selected >= 0) {
                                    const std::int32_t currentId =
                                        world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                                    const std::int32_t type =
                                        world::gPlayerRuntimeSlots.items[selected + 8].itemType;
                                    if (WindowCraft_ClassifySpecialMaterial(type, currentId) != 0) {
                                        return;
                                    }
                                }
                            }
                            if (gSelectedSlotsCA0A90[destination] >= 0) {
                                static_cast<void>(WindowCraft_ReleaseMaterials(destination));
                            }
                            const std::int32_t itemId = item->itemId;
                            const std::int32_t imageNo = item->imageNo;
                            gSelectedSlotsCA0A90[destination] = source - 8;
                            gSelectionImagesCF3FF4[destination] = imageNo;
                            gSelectionCountsD66E0C[destination] = WindowCraft_CountRecipeMaterial(itemId);
                            gMaterialIndicesCA0458[destination] = -1;
                            auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
                            const unsigned char previous = hover[source];
                            gItemMarksD67290[source] = 1;
                            hover[source] = static_cast<unsigned char>(previous + 1);
                            return;
                        }
                        skill = gCraftSkillIndexCC2368;
                    }
                    const std::int32_t recipeIndex = gCraftRecipeIndexD2D638;
                    const auto* const materials =
                        world::gSkillsF1BAFC[skill].recipes8E8[recipeIndex].items6C.data();
                    std::int32_t material = 0;
                    for (; material < 5; ++material) {
                        const std::int32_t itemId = materials[material].itemId;
                        if (itemId < 0) {
                            continue;
                        }
                        const std::int32_t occupied = gMaterialOccupancyCC4894.materialValues[material];
                        if (occupied == 0 && gItemMarksD67290[source] == 0
                            && item->itemId == itemId && count >= materials[material].count) {
                            break;
                        }
                    }
                    if (material >= 5) {
                        return;
                    }
                    if (gSelectedSlotsCA0A90[destination] >= 0) {
                        static_cast<void>(WindowCraft_ReleaseMaterials(destination));
                    }
                    const std::int32_t itemId = item->itemId;
                    const std::int32_t imageNo = item->imageNo;
                    gMaterialOccupancyCC4894.materialValues[material] = 1;
                    gSelectedSlotsCA0A90[destination] = source - 8;
                    gSelectionImagesCF3FF4[destination] = imageNo;
                    gSelectionCountsD66E0C[destination] = WindowCraft_CountRecipeMaterial(itemId);
                    gMaterialIndicesCA0458[destination] = material;
                    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
                    const unsigned char previous = hover[source];
                    gItemMarksD67290[source] = 1;
                    hover[source] = static_cast<unsigned char>(previous + 1);
                    return;
                }
                if (mode == 6 || mode == 15) {
                    const std::int32_t accepted = mode == 6
                        ? world::IsItemEligibleForDecoration_505040(source) : world::IsItemEligibleForPetDecoration_5050B0(source);
                    if (accepted == 0) {
                        return;
                    }
                    if (gSelectedSlotsCA0A90[destination] >= 0) {
                        static_cast<void>(WindowCraft_ReleaseMaterials(destination));
                    }
                    const std::int32_t type = world::gPlayerRuntimeSlots.items[source].itemType;
                    const std::int32_t material = mode == 6
                        ? (type == 38 || type == 25 || type == 39 ? 1 : 0)
                        : (type == 80 ? 1 : 0);
                    const std::int32_t occupied = gMaterialOccupancyCC4894.materialValues[material];
                    if (occupied != 0) {
                        return;
                    }
                    gMaterialOccupancyCC4894.materialValues[material] = 1;
                    WindowCraft_ReleaseMaterialSlot(destination, source, material);
                    gItemMarksD68A90[source - 8] = 1;
                    return;
                }
                if (mode == 9) {
                    if (world::IsItemEligibleForRefining_505110(source) == 0) {
                        return;
                    }
                    if (gSelectedSlotsCA0A90[destination] >= 0) {
                        static_cast<void>(WindowCraft_ReleaseMaterials(destination));
                    }
                    const std::int32_t occupied = gMaterialOccupancyCC4894.materialValues[0];
                    if (occupied != 0) {
                        return;
                    }
                    gMaterialOccupancyCC4894.materialValues[0] = 1;
                    gSelectedSlotsCA0A90[destination] = source - 8;
                    const auto* const item = &world::gPlayerRuntimeSlots.items[source];
                    const std::int32_t itemId = item->itemId;
                    gSelectionImagesCF3FF4[destination] = item->imageNo;
                    gSelectionCountsD66E0C[destination] = WindowCraft_CountRecipeMaterial(itemId);
                    gMaterialIndicesCA0458[destination] = 0;
                    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
                    const unsigned char previous = hover[source];
                    gItemMarksD67290[source] = 1;
                    hover[source] = static_cast<unsigned char>(previous + 1);
                    return;
                }
                if (mode == 12) {
                    WindowCraft_ValidatePackingMaterials(destination, source);
                }
            };
            receiveInventory();
            WindowDrag_Reset();
        } else if (WindowDrag_ReadKind() == 10) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            if (source != destination) {
                const std::int32_t sourceSlot = gSelectedSlotsCA0A90[source];
                const std::int32_t destinationSlot = gSelectedSlotsCA0A90[destination];
                gSelectedSlotsCA0A90[destination] = sourceSlot;
                const std::int32_t sourceImage = gSelectionImagesCF3FF4[source];
                gSelectedSlotsCA0A90[source] = destinationSlot;
                const std::int32_t destinationImage = gSelectionImagesCF3FF4[destination];
                gSelectionImagesCF3FF4[destination] = sourceImage;
                const std::int32_t sourceCount = gSelectionCountsD66E0C[source];
                gSelectionImagesCF3FF4[source] = destinationImage;
                const std::int32_t destinationCount = gSelectionCountsD66E0C[destination];
                gSelectionCountsD66E0C[destination] = sourceCount;
                const std::int32_t sourceMaterial = gMaterialIndicesCA0458[source];
                gSelectionCountsD66E0C[source] = destinationCount;
                const std::int32_t destinationMaterial = gMaterialIndicesCA0458[destination];
                gMaterialIndicesCA0458[destination] = sourceMaterial;
                gMaterialIndicesCA0458[source] = destinationMaterial;
            }
            WindowDrag_Reset();
        }
    }
    const std::uint8_t mode = gSelectionModeD2D650;
    if (mode == 0) {
        for (std::int32_t position = 0; position < 6; ++position) {
            const std::int32_t slot = gSelectedSlotsCA0A90[position];
            if (slot < 0) {
                continue;
            }
            const auto* const item = &world::gPlayerRuntimeSlots.items[slot + 8];
            std::int32_t count = item->count;
            if (count == 0) {
                count = 1;
            }
            const std::int32_t material = gMaterialIndicesCA0458[position];
            bool accepted;
            if (material >= 0) {
                const std::int32_t skill = gCraftSkillIndexCC2368;
                const std::int32_t recipe = gCraftRecipeIndexD2D638;
                const std::int32_t itemId = item->itemId;
                const auto* const requirement =
                    &world::gSkillsF1BAFC[skill].recipes8E8[recipe].items6C[material];
                accepted = itemId == requirement->itemId && count >= requirement->count;
            } else {
                const std::int32_t skillId =
                    world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                accepted = skillId != 215 && skillId != 216
                    && WindowCraft_ClassifySpecialMaterial(item->itemType, skillId) != 0;
            }
            if (!accepted) {
                static_cast<void>(WindowCraft_ReleaseMaterials(position));
            }
        }
    } else if (mode == 6 || mode == 15) {
        bool foundSpecial = false;
        bool foundOther = false;
        for (std::int32_t position = 0; position < 6; ++position) {
            const std::int32_t slot = gSelectedSlotsCA0A90[position];
            if (slot < 0) {
                continue;
            }
            const std::int32_t accepted = mode == 6
                ? world::IsItemEligibleForDecoration_505040(slot + 8) : world::IsItemEligibleForPetDecoration_5050B0(slot + 8);
            if (accepted == 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(position));
                continue;
            }
            const std::int32_t selected = gSelectedSlotsCA0A90[position];
            const std::int32_t type = world::gPlayerRuntimeSlots.items[selected + 8].itemType;
            const bool special = mode == 6
                ? type == 38 || type == 25 || type == 39 : type == 80;
            if (special) {
                if (foundSpecial) {
                    static_cast<void>(WindowCraft_ReleaseMaterials(position));
                }
                foundSpecial = true;
            } else {
                if (foundOther) {
                    static_cast<void>(WindowCraft_ReleaseMaterials(position));
                }
                foundOther = true;
            }
        }
    } else if (mode == 9) {
        for (std::int32_t position = 0; position < 6; ++position) {
            const std::int32_t slot = gSelectedSlotsCA0A90[position];
            if (slot >= 0 && world::IsItemEligibleForRefining_505110(slot + 8) == 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(position));
            }
        }
    }
    const bool hovered = (events & MouseFlag_Hover) != 0;
    if (hovered && gSelectedSlotsCA0A90[destination] >= 0 && (events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(WindowCraft_StartMaterialDrag(destination));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 10 && WindowDrag_ReadSourceIndex() == destination) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t y = window->y + element->y;
            const std::int32_t x = window->x + element->x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46), 223, 254, 0);
        }
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = window->x + element->x;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            const std::int32_t y = window->y + element->y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48), 223, imageNo, 0);
        }
    }
    if ((events & MouseFlag_Dragging) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = window->x + element->x;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            const std::int32_t y = window->y + element->y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48), 223, imageNo, 0);
        }
    }
    if (hovered) {
        gHoverTextD68AE8 = L"以滑鼠拖放方式，将素材置放于此处。";
    }
    const std::int32_t selected = gSelectedSlotsCA0A90[destination];
    if (selected < 0) {
        return 0;
    }
    if (hovered) {
        if (gMaterialHoverSlotsCB0A8C[destination] != destination) {
            gMaterialHintPageCF4FD8 = 0;
        }
        gMaterialHoverSlotsCB0A8C[destination] = destination;
        if ((events & MouseFlag_RDownEvent) != 0) {
            ++gMaterialHintPageCF4FD8;
            if (gMaterialHintPageCF4FD8 >= 4) {
                gMaterialHintPageCF4FD8 = 0;
            }
            consumed = 1;
        }
        gMaterialHoveredItemD67424 = selected + 8;
    }
    WindowInstance* const window = currentWindow;
    const std::int32_t imageNo = gSelectionImagesCF3FF4[destination];
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t y = element->y + window->y + 24;
        const std::int32_t x = element->x + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x0047DF50。
static WindowAnimationData* WindowCraft_InitializeRecipe(WindowInstance* const window)
{
    for (std::int32_t index = 0; index < 6; ++index) {
        static_cast<void>(WindowCraft_ReleaseMaterials(index));
    }
    static_cast<void>(WindowCraft_ReleaseMaterials(6));
    gItemMarksD67290.fill(0);
    gMaterialHoverSlotsCB0A8C.fill(-2);
    gMaterialSingleHoverCB0AA4 = -2;
    gMaterialOccupancyCC4894 = {};
    gMaterialIndicesCA0458.fill(-1);
    gSelectedSlotsCA0A90.fill(-1);
    gSelectionCountsD66E0C.fill(0);
    gSelectionImagesCF3FF4.fill(-1);
    std::int32_t recipe = gCraftRecipeIndexD2D638;
    std::int32_t row = 0;
    for (std::int32_t material = 4; material >= 0; --material) {
        const std::int32_t skill = gCraftSkillIndexCC2368;
        gMaterialOccupancyCC4894.materialValues[material] = 1;
        const auto* const requirement =
            &world::gSkillsF1BAFC[skill].recipes8E8[recipe].items6C[material];
        if (requirement->itemId < 0) {
            continue;
        }
        const std::int32_t count = requirement->count;
        gMaterialOccupancyCC4894.materialValues[material] = 0;
        if (count <= 0) {
            row = 1;
        }
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(9 + row)) : nullptr; indexedElement) indexedElement->data.text->text = std::format(L"{:<24}{:3}", requirement->name, count);
        recipe = gCraftRecipeIndexD2D638;
        ++row;
    }
    for (; row < 5; ++row) {
        auto* const indexedElement = window ? window->FindElementById(NativeElementKey(9 + row)) : nullptr;
        if (indexedElement) {
            indexedElement->data.text->text.clear();
        }
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3A;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialSlot;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 0;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, 0);
    }
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    gCraftDisplayImageCB2204 = -1;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    gSelectionModeD2D650 = 0;
    return result;
}

// 原函数：0x004F4430。
std::int32_t WindowCraft_ClassifySpecialMaterial(const std::int32_t itemType, const std::int32_t skillId)
{
    if (skillId >= 601 && skillId <= 605) {
        return itemType == 80 ? 1 : 0;
    }
    return itemType == 38 || itemType == 25 || itemType == 39 ? 1 : 0;
}

// 原函数：0x00483E30。
static void WindowCraft_ReleaseMaterialSlot(
    const std::int32_t destination, const std::int32_t source, const std::int32_t material)
{
    static_cast<void>(WindowCraft_ReleaseMaterials(destination));
    gSelectedSlotsCA0A90[destination] = source - 8;
    const auto* const item = &world::gPlayerRuntimeSlots.items[source];
    const std::int32_t itemId = item->itemId;
    gSelectionImagesCF3FF4[destination] = item->imageNo;
    gSelectionCountsD66E0C[destination] = WindowCraft_CountRecipeMaterial(itemId);
    gMaterialIndicesCA0458[destination] = material;
    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    hover[source] = static_cast<unsigned char>(hover[source] + 1);
    gItemMarksD67290[source] = 1;
}

// 原函数：0x004880D0。
static std::int32_t WindowCraft_ReadRequiredSkillLevel(const std::int32_t skillId, const std::int32_t extraLevel)
{
    for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
        const world::SkillPairRecord pair = world::gSkillPairsF63370[index];
        const std::int16_t skillIndex = std::bit_cast<std::int16_t>(pair.index);
        if (skillIndex < 0 || skillIndex >= world::kSkillSlotCount) continue;
        const auto& skill = world::gSkillsF1BAFC[skillIndex];
        if (skill.skillId != skillId || pair.used == 0) {
            continue;
        }
        std::int32_t level = skill.level;
        if (static_cast<std::uint8_t>(extraLevel) == 1) {
            ++level;
        }
        return level < skill.maxLevel ? level : skill.maxLevel;
    }
    return 0;
}

// 原函数：0x00488160。
static std::int32_t WindowCraft_ReadMaxMaterialLevel()
{
    std::int32_t maximum = 0;
    for (std::int32_t index = 0; index < 3; ++index) {
        const std::int32_t value = gMaterialOccupancyCC4894.materialValues[index];
        if (value > maximum) {
            maximum = value;
        }
    }
    return maximum;
}

// 原函数：0x00488180。
static std::int32_t WindowCraft_ReadFirstMaterialId()
{
    for (std::int32_t index = 0; index < 6; ++index) {
        const std::int32_t material = gMaterialIndicesCA0458[index];
        if (material == 0 || material == 1 || material == 2) {
            const std::int32_t slot = gSelectedSlotsCA0A90[index];
            return world::gPlayerRuntimeSlots.items[slot + 8].itemId;
        }
    }
    return -1;
}

// 原函数：0x004881D0。
static void WindowCraft_ValidatePackingMaterials(const std::int32_t destination, const std::int32_t source)
{
    std::int32_t flags = 0;
    std::int32_t material = 5;
    const std::int32_t maximum = WindowCraft_ReadMaxMaterialLevel();
    const auto* const item = &world::gPlayerRuntimeSlots.items[source];
    if (item->hasDescription == 0) {
        return;
    }
    std::int32_t requiredCount = 0;
    switch (item->itemType) {
    case 23:
    case 43:
        material = 0;
        requiredCount = 3;
        break;
    case 30:
        material = 3;
        requiredCount = 10;
        break;
    case 31:
        material = 4;
        requiredCount = 10;
        break;
    default:
        break;
    }
    if (item->count < requiredCount) {
        return;
    }
    const std::uint8_t crystal = gSkillRebirthModeEnabled_5F9754;
    const std::int32_t level = item->level;
    if (level > WindowCraft_ReadRequiredSkillLevel(2026, crystal != 0 ? 1 : 0)) {
        return;
    }
    if (material >= 0 && material <= 2) {
        flags = 5;
        if (maximum > 0) {
            const std::int32_t itemId = WindowCraft_ReadFirstMaterialId();
            if (item->itemId == itemId) {
                for (material = 0; material < 3; ++material) {
                    const std::int32_t occupied = gMaterialOccupancyCC4894.materialValues[material];
                    if (occupied <= 0) {
                        break;
                    }
                }
                if (material >= 3) {
                    return;
                }
            } else {
                flags = 13;
            }
        }
    } else if (material == 3 || material == 4) {
        if (maximum <= 0 || level == maximum) {
            flags = 3;
        }
    }
    const bool accepted = (flags & 1) != 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        if (!accepted) {
            break;
        }
        if (index == destination) {
            continue;
        }
        if ((flags & 2) != 0 && gMaterialIndicesCA0458[index] == material) {
            static_cast<void>(WindowCraft_ReleaseMaterials(index));
        }
        if ((flags & 4) != 0) {
            const std::int32_t slot = gSelectedSlotsCA0A90[index];
            const std::int32_t sourceLevel = item->level;
            if (world::gPlayerRuntimeSlots.items[slot + 8].level != sourceLevel) {
                static_cast<void>(WindowCraft_ReleaseMaterials(index));
            }
        }
        if ((flags & 8) != 0) {
            const std::int32_t otherMaterial = gMaterialIndicesCA0458[index];
            if (otherMaterial == 0 || otherMaterial == 1 || otherMaterial == 2) {
                static_cast<void>(WindowCraft_ReleaseMaterials(index));
            }
        }
    }
    static_cast<void>(WindowCraft_ReleaseMaterials(6));
    if (accepted) {
        WindowCraft_ReleaseMaterialSlot(destination, source, material);
        gMaterialOccupancyCC4894.materialValues[material] = item->level;
    }
}

// 原函数：0x0047DE00。
static void WindowCraft_HandleMaterialDrag(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 || message == 3 || message == 4) {
            static_cast<void>(WindowCraft_ReleaseMaterials(sourceIndex));
        }
        return;
    }
    if (sourceIndex >= 6 && gSelectedImageCC3ED8 < 0) {
        WindowDrag_Cancel();
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        const auto y = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        static_cast<void>(WindowDrag_StartAtPosition(10, sourceIndex, x, y, WindowCraft_HandleMaterialDrag));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t imageNo =
        sourceIndex < 6 ? gSelectionImagesCF3FF4[sourceIndex] : gSelectedImageCC3ED8;
    const std::int32_t y =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

// 原函数：0x0047DF30。
static std::int32_t WindowCraft_StartMaterialDrag(const std::int32_t sourceIndex)
{
    return WindowDrag_Start(10, sourceIndex, WindowCraft_HandleMaterialDrag);
}

// 原函数：0x00482F70。
std::int32_t WindowCraft_ClassifyWork(const std::int32_t itemType)
{
    if (itemType >= 7 && itemType <= 14) {
        return 2;
    }
    if ((itemType >= 0 && itemType <= 7) || (itemType >= 70 && itemType <= 79)) {
        return 1;
    }
    return 0;
}

// 原函数：0x00482270。
static std::int32_t WindowCraft_ButtonExecute(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const elementPtr = currentWindow->FindElementById(elementId);
    if (!elementPtr) { return 0; }
    WindowElement* const element = elementPtr;
    auto* const graphic = element->data.graphic;
    std::int32_t consumed = 0;
    bool enabled = false;
    const auto recipeCost = [] {
        const std::int32_t skill = gCraftSkillIndexCC2368;
        const std::int32_t recipe = gCraftRecipeIndexD2D638;
        const auto* const record = &world::gSkillsF1BAFC[skill];
        return record->recipes8E8[recipe].value04 * record->fpReduce / 100;
    };
    switch (std::bit_cast<std::int8_t>(gSelectionModeD2D650)) {
    case 0:
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            if (gMaterialOccupancyCC4894.materialValues[slot] == 0) {
                element->visible = 0;
                return 0;
            }
        }
        enabled = !(world::gPlayerRuntimeSlots.fp < recipeCost());
        break;
    case 3: {
        const std::int32_t selected = gSelectedItemD460C0;
        if (selected < 0) {
            element->visible = 0;
            if (auto* const element12 = currentWindow->FindElementById(WindowCraftElementId::Element12); element12) element12->data.text->text = L"";
            return 0;
        }
        if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
            enabled = !(world::gPlayerRuntimeSlots.fp < 10);
        } else {
            enabled = !(world::gPlayerRuntimeSlots.fp <
                world::gPlayerRuntimeSlots.items[selected].level * 10);
            const std::int32_t level = world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].level;
            auto* const element12 = currentWindow->FindElementById(WindowCraftElementId::Element12);
            if (element12) {
                cg::text::FormatTo(element12->data.text->text, L"                     魔力{:3}" /* 原常量参考：0x0058678C。 */, level * 10);
            }
        }
        const std::int32_t skill = gCraftSkillIndexCC2368;
        const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
        if (skillId == 220) {
            const auto* const item = &world::gPlayerRuntimeSlots.items[gSelectedItemD460C0];
            const std::uint16_t flags = item->flags;
            if (((flags & 0x200u) != 0 && (flags & 0x400u) == 0)
                || ((flags & 0x100u) != 0 && (flags & 0x400u) == 0)
                || item->hasDescription == 0 || item->count > 0 || item->itemType == 47) {
                enabled = false;
            }
        } else if (skillId == 219 || skillId == 2033) {
            const auto* const item = &world::gPlayerRuntimeSlots.items[gSelectedItemD460C0];
            if (item->hasDescription != 0) {
                enabled = false;
            } else {
                const std::int32_t level =
                    world::gSkillsF1BAFC[skill].level + (gSkillRebirthModeEnabled_5F9754 != 0 ? 1 : 0);
                if (item->level > level) {
                    enabled = false;
                }
            }
        } else if (skillId == 217 || skillId == 218 || skillId == 2031 || skillId == 2032) {
            const auto* const item = &world::gPlayerRuntimeSlots.items[gSelectedItemD460C0];
            if (item->hasDescription == 0) {
                enabled = false;
            } else if (WindowCraft_ClassifyWork(item->itemType) == 2) {
                const std::int32_t currentId =
                    world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                if (currentId == 217 || currentId == 2031) {
                    enabled = false;
                }
            } else if (WindowCraft_ClassifyWork(
                           world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].itemType) == 1) {
                const std::int32_t currentId =
                    world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                if (currentId == 218 || currentId == 2032) {
                    enabled = false;
                }
            } else if (WindowCraft_ClassifyWork(
                           world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].itemType) == 0) {
                enabled = false;
            } else {
                const std::int32_t currentSkill = gCraftSkillIndexCC2368;
                const std::uint8_t crystal = gSkillRebirthModeEnabled_5F9754;
                const std::int32_t level = world::gSkillsF1BAFC[currentSkill].level;
                const std::int32_t slot = gSelectedItemD460C0;
                if (world::gPlayerRuntimeSlots.items[slot].level > level + (crystal != 0 ? 1 : 0)) {
                    enabled = false;
                }
            }
        }
        break;
    }
    case 6:
    case 15:
        if (gMaterialOccupancyCC4894.materialValues[0] == 0 || gMaterialOccupancyCC4894.materialValues[1] == 0) {
            element->visible = 0;
            return 0;
        }
        enabled = !(world::gPlayerRuntimeSlots.fp < 100);
        break;
    case 9:
        if (gMaterialOccupancyCC4894.materialValues[0] == 0) {
            element->visible = 0;
            return 0;
        }
        enabled = true;
        break;
    case 12: {
        const std::int32_t fourth = gMaterialOccupancyCC4894.materialValues[4];
        const std::int32_t third = gMaterialOccupancyCC4894.materialValues[3];
        const std::int32_t second = gMaterialOccupancyCC4894.materialValues[2];
        const std::int32_t first = gMaterialOccupancyCC4894.materialValues[1];
        const std::int32_t zeroth = gMaterialOccupancyCC4894.materialValues[0];
        if ((fourth & third & second & first & zeroth) == 0) {
            element->visible = 0;
            return 0;
        }
        enabled = true;
        break;
    }
    default:
        break;
    }
    const bool hovered = (state & MouseFlag_Hover) != 0;
    if (hovered) {
        if (enabled) {
            gHoverTextD68AE8 = L"执行。" /* 原常量参考：0x005868D8。 */;
        }
        bool insufficient = false;
        switch (std::bit_cast<std::int8_t>(gSelectionModeD2D650)) {
        case 0:
            insufficient = (world::gPlayerRuntimeSlots.fp < recipeCost());
            break;
        case 3:
            if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
                insufficient = (world::gPlayerRuntimeSlots.fp < 10);
            } else {
                insufficient = (world::gPlayerRuntimeSlots.fp <
                world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].level * 10);
            }
            break;
        case 6:
        case 15:
            insufficient = (world::gPlayerRuntimeSlots.fp < 100);
            break;
        default:
            break;
        }
        if (insufficient) {
            gHoverTextD68AE8 = kWindow12NoFp5864F0;
        }
    }
    if (gSelectionModeD2D650 == 0 && (state & MouseFlag_LDownEvent) != 0) {
        if (enabled) {
            static_cast<void>(WindowCraft_WaitCraft(currentWindow));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if (gSelectionModeD2D650 == 3
        && ((state & MouseFlag_LDownEvent) != 0 || platform::ReadKeyFlag4_4597A0(13) != 0)) {
        if (!enabled) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId != 220) {
            static_cast<void>(WindowCraft_WaitItemWork(currentWindow));
            if (input::gCurrentInputData == &cg::ui::detail::gInputContextD2DE70) {
                static_cast<void>(WindowChat_RestoreInput());
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            std::wstring trimmed;
            std::wstring buffer;
            const auto trimInput = [&] {
                trimmed = input::ContextText(cg::ui::detail::gInputContextD2DE70);
                static_cast<void>(std::erase(trimmed, L' '));
                // 原删除集合字节仍按GBK解码，直接处理宽正文。
                for (const wchar_t character : cg::text::FromGbkWide("\x81\x40")) {
                    static_cast<void>(std::erase(trimmed, character));
                }
            };
            bool send = false;
            std::int32_t notice = -1;
            const auto flags = world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].flags;
            if ((flags & 0x400u) != 0) {
                if (input::ContainsPipeOrBackslash_45E770(input::ContextText(cg::ui::detail::gInputContextD2DE70)) == 1) {
                    notice = 0;
                } else {
                    trimInput();
                    if (trimmed.empty()
                        && cg::ui::detail::gInputContextD2DE70.length() != 0) {
                        notice = 1;
                    } else {
                        send = true;
                    }
                }
            } else if (!cg::ui::detail::gInputContextD2DE70.empty()) {
                trimInput();
                if (trimmed.empty()) {
                    notice = 1;
                } else if (input::ContainsPipeOrBackslash_45E770(input::ContextText(cg::ui::detail::gInputContextD2DE70)) == 1) {
                    notice = 0;
                } else {
                    send = true;
                }
            }
            if (notice >= 0) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(notice)); indexedElement) indexedElement->visible = 1;
                if (input::gCurrentInputData == &cg::ui::detail::gInputContextD2DE70) {
                    static_cast<void>(WindowChat_RestoreInput());
                }
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (send) {
                if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
                    buffer = input::ContextText(cg::ui::detail::gInputContextD2DE70);
                } else {
                    buffer.clear();
                }
                cg::core::NullSub_509080();
                std::int32_t offset = 0;
                if (gSkillRebirthModeEnabled_5F9754 != 0) {
                    offset = 100;
                } else if (gSkillSendModeC8A970 != 0) {
                    gSkillPetImageCB0A18 = WindowSkill_ReadBattlePetImage();
                    offset = 200;
                }
                const std::int32_t request = std::bit_cast<std::int8_t>(gSkillRequestByteD64E13);
                const std::int32_t selected = gSelectedItemD460C0;
                const std::int32_t skill = gCraftSkillIndexCC2368;
                static_cast<void>(cg::network::GetGameConnection().SendSkillUseRequest(skill + offset, request, selected, buffer));
                input::InputContext* const current = input::gCurrentInputData;
                gSelectionModeD2D650 = 5;
                if (current == &cg::ui::detail::gInputContextD2DE70) {
                    static_cast<void>(WindowChat_RestoreInput());
                }
                cg::ui::detail::gInputContextD2DE70.clearText();
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        consumed = 1;
    }
    const std::uint8_t mode = gSelectionModeD2D650;
    if ((mode == 6 || mode == 15) && (state & MouseFlag_LDownEvent) != 0) {
        if (enabled) {
            WindowCraft_WaitDecoration(currentWindow);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if (gSelectionModeD2D650 == 9 && (state & MouseFlag_LDownEvent) != 0) {
        if (enabled) {
            static_cast<void>(WindowCraft_WaitRefining(currentWindow));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if (gSelectionModeD2D650 == 12 && (state & MouseFlag_LDownEvent) != 0) {
        if (enabled) {
            static_cast<void>(WindowCraft_WaitPacking(currentWindow));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if (elementId == static_cast<ElementId>(WindowCraftElementId::ButtonExecute_26)) {
        graphic->imageNo = 0x8FD3;
        if (hovered) {
            graphic->imageNo = 0x8FD5;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0 || !enabled) {
            graphic->imageNo = 0x8FD4;
        }
        return consumed;
    }
    graphic->imageNo = 0x3BAC2;
    if (hovered) {
        graphic->imageNo = 0x3BAC4;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        graphic->imageNo = 0x3BAC3;
    }
    if (enabled) {
        if (gSelectionModeD2D650 == 3) {
            if (auto* const element10 = currentWindow->FindElementById(WindowCraftElementId::Element10); element10) element10->data.text->text = L"请输入名称。" /* 原常量参考：0x005868C8。 */;
            if (auto* const element11 = currentWindow->FindElementById(WindowCraftElementId::Element11); element11) element11->data.text->text = L"                     魔力 10" /* 原常量参考：0x005868A8。 */;
        }
        if (gSelectionModeD2D650 == 5) {
            if (auto* const element10 = currentWindow->FindElementById(WindowCraftElementId::Element10); element10) element10->data.text->text = L"";
            if (auto* const element11 = currentWindow->FindElementById(WindowCraftElementId::Element11); element11) element11->data.text->text = L"";
        }
    } else {
        if (auto* const element10 = currentWindow->FindElementById(WindowCraftElementId::Element10); element10) element10->data.text->text = L"目前无法对此物品" /* 原常量参考：0x00586894。 */;
        if (auto* const element11 = currentWindow->FindElementById(WindowCraftElementId::Element11); element11) element11->data.text->text = L"　进行刻印。" /* 原常量参考：0x00586884。 */;
        graphic->imageNo = 0x3BAC3;
    }
    return consumed;
}

// 原函数：0x00482FA0。
static std::int32_t WindowCraft_ButtonContinue(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hovered = (state & MouseFlag_Hover) != 0;
    if (hovered) {
        gHoverTextD68AE8 = L"继续执行。";
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (gSelectionModeD2D650 == 2) {
            const std::int32_t skillId =
                world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
            switch (skillId) {
            case 255:
                static_cast<void>(WindowCraft_InitializeDecoration(window));
                break;
            case 606:
                static_cast<void>(WindowCraft_InitializePetDecoration(window));
                break;
            case 2025:
                static_cast<void>(WindowCraft_InitializeRefining(window));
                break;
            case 2026:
                static_cast<void>(WindowCraft_InitializePacking(window));
                break;
            default:
                static_cast<void>(WindowCraft_InitializeRecipe(window));
                break;
            }
            window = currentWindow;
        }
        if (gSelectionModeD2D650 == 5) {
            static_cast<void>(WindowCraft_InitializeItemWork(window));
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (graphic) graphic->imageNo = 0x3BA31;
    if (hovered) {
        if (graphic) graphic->imageNo = 0x3BA33;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3BA32;
    }
    return 0;
}

// 原函数：0x004830B0。
static std::int32_t WindowCraft_ButtonClose(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        if (hovered) {
            gHoverTextD68AE8 = L"关闭此视窗";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowRuntime_States()[WindowId_Craft].window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        graphic->imageNo = 0x3B565;
        if (hovered) {
            graphic->imageNo = 0x3B567;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B566;
        }
    }
    return 0;
}

// 原函数：0x00483150。
static std::int32_t WindowCraft_HandleInventory(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto* const hoverMarks = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::uint32_t events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        const WindowInstance* const window = currentWindow;
        for (std::int32_t position = 0; position < 20; ++position) {
            if (hoverMarks[position + 8] != 0) {
                continue;
            }
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement* const element = elementPtr;
                const std::int32_t left = element->x + 50 * (position % 5) + window->x;
                const std::int32_t top = element->y + 50 * (position / 5) + window->y;
                if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                    continue;
                }
            }
            if (source != position + 8) {
                WindowItem_RequestTransfer(11, index, source, position + 8);
            }
            break;
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (hoverMarks[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement* const element = elementPtr;
                const std::int32_t left = element->x + 50 * (position % 5) + window->x;
                const std::int32_t top = element->y + 50 * (position / 5) + window->y;
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = position;
                    break;
                }
            }
        }
    }
    if ((events & MouseFlag_Hover) != 0 && hovered >= 0) {
        gHoverTextD68AE8 = L"以滑鼠拖放＆点选方式，来选择物品。";
    }
    if ((events & MouseFlag_Dragging) != 0 && WindowDrag_ReadSourceIndex() >= 100
        && (world::gPlayerRuntimeSlots.items[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if (gCraftBackpackHover57DA24 != hovered) {
        gMaterialHintPageCF4FD8 = 0;
    }
    gCraftBackpackHover57DA24 = hovered;
    if (hovered >= 0 && world::gPlayerRuntimeSlots.items[hovered + 8].used != 0
        && (platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDblClick) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 11 || WindowDrag_ReadFocusWindowId() == 0) {
            const std::uint8_t mode = gSelectionModeD2D650;
            if (mode == 0 || mode == 3 || mode == 6 || mode == 15 || mode == 9 || mode == 12) {
                static_cast<void>(WindowDrag_ResetMouse());
                const auto selectItem = [&] {
                    auto* const item = &world::gPlayerRuntimeSlots.items[hovered + 8];
                    const auto fail = [] {
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    };
                    const auto firstFreeSlot = [] {
                        std::int32_t slot = 0;
                        while (slot < 6 && gSelectedSlotsCA0A90[slot] >= 0) {
                            ++slot;
                        }
                        return slot;
                    };
                    // 此路径只填首空槽，没有483E30的预释放；标记仍在4F4630之后发布。
                    const auto publishMaterial = [&](const std::int32_t slot,
                                                     const std::int32_t material,
                                                     const bool extraMark) {
                        const std::int32_t itemId = item->itemId;
                        const std::int32_t imageNo = item->imageNo;
                        if (material >= 0) {
                            gMaterialOccupancyCC4894.materialValues[material] = 1;
                        }
                        gSelectedSlotsCA0A90[slot] = hovered;
                        gSelectionImagesCF3FF4[slot] = imageNo;
                        gSelectionCountsD66E0C[slot] = WindowCraft_CountRecipeMaterial(itemId);
                        gMaterialIndicesCA0458[slot] = material;
                        const unsigned char previous = hoverMarks[hovered + 8];
                        gItemMarksD67290[hovered + 8] = 1;
                        if (extraMark) {
                            gItemMarksD68A90[hovered] = 1;
                        }
                        hoverMarks[hovered + 8] = static_cast<unsigned char>(previous + 1);
                    };
                    switch (gSelectionModeD2D650) {
                    case 0: {
                        const std::int32_t slot = firstFreeSlot();
                        if (slot >= 6 || hovered + 8 < 8 || hovered + 8 >= 100
                            || item->hasDescription == 0) {
                            fail();
                            return;
                        }
                        std::int32_t count = item->count;
                        if (count == 0) {
                            count = 1;
                        }
                        std::int32_t skill = gCraftSkillIndexCC2368;
                        const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
                        if (skillId != 215 && skillId != 216) {
                            if (WindowCraft_ClassifySpecialMaterial(item->itemType, skillId) != 0) {
                                for (std::int32_t current = 0; current < 6; ++current) {
                                    const std::int32_t selected = gSelectedSlotsCA0A90[current];
                                    if (selected >= 0) {
                                        const std::int32_t currentId =
                                            world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                                        const std::int32_t type =
                                            world::gPlayerRuntimeSlots.items[selected + 8].itemType;
                                        if (WindowCraft_ClassifySpecialMaterial(type, currentId) != 0) {
                                            fail();
                                            return;
                                        }
                                    }
                                }
                                publishMaterial(slot, -1, false);
                                return;
                            }
                            skill = gCraftSkillIndexCC2368;
                        }
                        const std::int32_t recipeIndex = gCraftRecipeIndexD2D638;
                        const auto* const recipe = &world::gSkillsF1BAFC[skill].recipes8E8[recipeIndex];
                        for (std::int32_t material = 0; material < 5; ++material) {
                            const auto* const required = &recipe->items6C[material];
                            const std::int32_t itemId = required->itemId;
                            if (itemId >= 0 && gMaterialOccupancyCC4894.materialValues[material] == 0
                                && gItemMarksD67290[hovered + 8] == 0
                                && item->itemId == itemId && count >= required->count) {
                                publishMaterial(slot, material, false);
                                return;
                            }
                        }
                        fail();
                        return;
                    }
                    case 3: {
                        const std::int32_t selected = hovered + 8;
                        if (selected < 8 || selected >= 100) {
                            return;
                        }
                        if (gSelectedItemD460C0 >= 0) {
                            static_cast<void>(WindowCraft_ReleaseMaterials(6));
                        }
                        const std::int32_t skill = gCraftSkillIndexCC2368;
                        const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
                        if (skillId == 220) {
                            const std::uint16_t flags = item->flags;
                            if (((flags & 0x200u) != 0 && (flags & 0x400u) == 0)
                                || ((flags & 0x100u) != 0 && (flags & 0x400u) == 0)
                                || item->hasDescription == 0 || item->count > 0 || item->itemType == 47) {
                                fail();
                                return;
                            }
                        } else if (skillId == 219 || skillId == 2033) {
                            if (item->hasDescription != 0) {
                                world::AppendChatMessage_44B050(L"不需鉴定的物品", 4, 3);
                                fail();
                                return;
                            }
                            const std::uint8_t crystal = gSkillRebirthModeEnabled_5F9754;
                            const std::int32_t level = world::gSkillsF1BAFC[skill].level;
                            if (item->level > level + (crystal != 0 ? 1 : 0)) {
                                world::AppendChatMessage_44B050(L"不可鉴定的物品", 4, 3);
                                fail();
                                return;
                            }
                        } else if (skillId == 217 || skillId == 218 || skillId == 2031 || skillId == 2032) {
                            bool rejected = item->hasDescription == 0;
                            if (!rejected) {
                                if (WindowCraft_ClassifyWork(item->itemType) == 2) {
                                    const std::int32_t currentId =
                                        world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                                    rejected = currentId == 217 || currentId == 2031;
                                } else if (WindowCraft_ClassifyWork(item->itemType) == 1) {
                                    const std::int32_t currentId =
                                        world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                                    rejected = currentId == 218 || currentId == 2032;
                                } else if (WindowCraft_ClassifyWork(item->itemType) == 0) {
                                    rejected = true;
                                } else {
                                    const std::int32_t currentSkill = gCraftSkillIndexCC2368;
                                    const std::uint8_t crystal = gSkillRebirthModeEnabled_5F9754;
                                    const std::int32_t level = world::gSkillsF1BAFC[currentSkill].level;
                                    rejected = item->level > level + (crystal != 0 ? 1 : 0);
                                }
                            }
                            if (rejected) {
                                fail();
                                return;
                            }
                        }
                        const std::int32_t itemId = item->itemId;
                        const std::int32_t imageNo = item->imageNo;
                        gSelectedItemD460C0 = selected;
                        gSelectedImageCC3ED8 = imageNo;
                        gSelectedCountCC483C = WindowCraft_CountRecipeMaterial(itemId);
                        const std::int32_t currentSkill = gCraftSkillIndexCC2368;
                        const unsigned char previous = hoverMarks[hovered + 8];
                        const std::int32_t currentId = world::gSkillsF1BAFC[currentSkill].skillId;
                        hoverMarks[hovered + 8] = static_cast<unsigned char>(previous + 1);
                        gItemMarksD67290[hovered + 8] = 1;
                        if (currentId == 220) {
                            WindowInstance* const window = currentWindow;
                            auto* const element12 = window->FindElementById(WindowCraftElementId::Element12);
                            if (element12) {
                                const WindowElement* const inputElement = element12;
                                const std::int32_t y = inputElement->y + window->y;
                                const std::int32_t x = inputElement->x + window->x;
                                static_cast<void>(WindowInput_ActivateElement(&gCraftInputPrototype57D7F8, x, y, 0));
                            }
                        }
                        return;
                    }
                    case 6:
                    case 15: {
                        const std::uint8_t currentMode = gSelectionModeD2D650;
                        const std::int32_t allowed = currentMode == 6
                            ? world::IsItemEligibleForDecoration_505040(hovered + 8)
                            : world::IsItemEligibleForPetDecoration_5050B0(hovered + 8);
                        if (allowed == 0) {
                            return;
                        }
                        const std::int32_t slot = firstFreeSlot();
                        if (slot >= 6) {
                            fail();
                            return;
                        }
                        const std::int32_t type = item->itemType;
                        const bool special = currentMode == 6 ? type == 38 || type == 25 || type == 39
                                                              : type == 80;
                        const std::int32_t material = special ? 1 : 0;
                        if (gMaterialOccupancyCC4894.materialValues[material] == 0) {
                            publishMaterial(slot, material, true);
                        }
                        return;
                    }
                    case 9: {
                        if (world::IsItemEligibleForRefining_505110(hovered + 8) == 0) {
                            return;
                        }
                        const std::int32_t slot = firstFreeSlot();
                        if (slot >= 6) {
                            fail();
                            return;
                        }
                        if (gMaterialOccupancyCC4894.materialValues[0] == 0) {
                            publishMaterial(slot, 0, false);
                        }
                        return;
                    }
                    case 12: {
                        const std::int32_t slot = firstFreeSlot();
                        if (slot >= 6) {
                            fail();
                            return;
                        }
                        WindowCraft_ValidatePackingMaterials(slot, hovered + 8);
                        return;
                    }
                    default:
                        return;
                    }
                };
                selectItem();
            }
        }
        consumed = 1;
    } else if ((events & MouseFlag_Hover) != 0 && hovered >= 0) {
        if ((events & MouseFlag_LDownEvent) != 0) {
            if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
            if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
                const std::int32_t page = gMaterialHintPageCF4FD8 + 1;
                const std::int32_t pages = world::gPlayerRuntimeSlots.items[hovered + 8].pages;
                gMaterialHintPageCF4FD8 = page;
                if (page >= pages) {
                    gMaterialHintPageCF4FD8 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(elementId);
            if (elementPtr) {
                const WindowElement* const element = elementPtr;
                const std::int32_t left = element->x + 50 * ((source - 8) % 5) + window->x;
                const std::int32_t top = element->y + 50 * ((source - 8) / 5) + window->y;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left + 1), static_cast<std::uint16_t>(top + 2),
                    static_cast<std::uint16_t>(left + 47), static_cast<std::uint16_t>(top + 46), 223, 254, 0);
            }
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t left = element->x + 50 * (hovered % 5) + window->x;
            const std::int32_t top = element->y + 50 * (hovered / 5) + window->y;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 1), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 49), static_cast<std::uint16_t>(top + 48), 223, imageNo, 0);
        }
        if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
            gMaterialHoveredItemD67424 = hovered + 8;
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(0, 0, 3, 1, 0, L"", 0, 0));
    descriptor.color = color;
    std::wstring countText;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto* const item = &world::gPlayerRuntimeSlots.items[position + 8];
        if (item->used == 0) {
            continue;
        }
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = element->x + 50 * (position % 5) + window->x + 24;
            const std::int32_t y = element->y + 50 * (position / 5) + window->y + 24;
            if (hoverMarks[position + 8] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                    static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24), 223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item->imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item->imageNo, 0, &descriptor));
            }
            const std::int32_t count = item->count;
            if (count > 0) {
                cg::text::FormatTo(countText, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x - 3), static_cast<std::int16_t>(y + 7),
                    3, 1, 1, countText, 0, 0));
            }
        }
    }
    return consumed;
}

// 原函数：0x00484FB0。
static std::int32_t WindowCraft_DrawResultImage(const ElementId elementId, const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t imageNo = gCraftDisplayImageCB2204;
    if (imageNo < 0) {
        return 0;
    }
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t y = element->y + window->y + 24;
        const std::int32_t x = element->x + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    }
    return 0;
}

// 原函数：0x00485030。
static std::int32_t WindowCraft_HandleEngravingInput(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (gSelectedItemD460C0 < 0) {
        return 0;
    }
    auto* const indexedElement = currentWindow->FindElementById(elementId);
    if (indexedElement) {
        WindowElement_DrawHoverFrame(indexedElement, state, cg::render::gHoverFramePaletteIndex_D69768);
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    if (auto* const handleEngravingInputElement = currentWindow->FindElementById(WindowCraftElementId::HandleEngravingInput); handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = currentWindow->FindElementById(WindowCraftElementId::Element8); element8) element8->visible = 1;
    WindowInstance* const window = currentWindow;
    auto* const element12 = window->FindElementById(WindowCraftElementId::Element12);
    if (element12) {
        const std::int32_t y = element12->y + window->y;
        auto* const element122 = window->FindElementById(WindowCraftElementId::Element12);
        if (element122) {
            const std::int32_t x = element122->x + window->x;
            static_cast<void>(WindowInput_ActivateElement(&gCraftInputPrototype57D7F8, x, y, 1));
        }
    }
    input::SetContext_44B330(&cg::ui::detail::gInputContextD2DE70);
    WindowInstance* const current = currentWindow;
    auto* const element82 = current->FindElementById(WindowCraftElementId::Element8);
    if (element82) {
        gFocusedElementDataD45F94 = element82->data.input;
    }
    auto* const element83 = current->FindElementById(WindowCraftElementId::Element8);
    if (element83) {
        element83->data.input->context =
            &cg::ui::detail::gInputContextD2DE70;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    return 1;
}

// 原函数：0x00485160。
static std::int32_t WindowCraft_ButtonCloseIcon(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hovered) {
            gHoverTextD68AE8 = L"关闭这个视窗。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowRuntime_States()[WindowId_Craft].window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B538;
        if (hovered) {
            graphic->imageNo = 0x3B53A;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B539;
        }
        return consumed;
    }
    return 0;
}

static std::int32_t WindowCraft_DrawInputNotice(
    const std::int32_t index, const std::wstring_view first, const std::wstring_view second)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t imageNo = 0x3B568;
    if (cg::ui::HitTestMouseRect_5049A0(287, 268, 353, 285, -1) != 0) {
        imageNo = 0x3B56A;
        if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            imageNo = 0x3B569;
        }
    }
    static_cast<void>(cg::render::DrawGraphic(320, 277, 226, imageNo, 0, &descriptor));
    static_cast<void>(render::DrawText_456780(
        145, 185, 4, 0, 0, first, 0,
        static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    static_cast<void>(render::DrawText_456780(
        145, 205, 4, 0, 0, second, 0,
        static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768)));
    cg::yobi::DrawNineGrid_49BE00(
        0x3BE34, 115, 155, 410, 150, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x00485200。
static std::int32_t WindowCraft_DrawInvalidEngravingNotice(const ElementId elementId, const std::int32_t)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    return WindowCraft_DrawInputNotice(index,
        LR"(包含\ 或 | 的字元。)",
        LR"(包含\ 或 | 的字元不能用来刻印。)");
}

// 原函数：0x00485350。
static std::int32_t WindowCraft_DrawEmptyEngravingNotice(const ElementId elementId, const std::int32_t)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    return WindowCraft_DrawInputNotice(index,
        L"没有包含空白以外的字元",
        L"刻印名称不能包含空白。");
}

// 原函数：0x004847C0。
static std::int32_t WindowCraft_HandleSingleMaterial(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8 && source < 100) {
            if (gSelectedItemD460C0 >= 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(6));
            }
            gSelectedItemD460C0 = source;
            const auto* const item = &world::gPlayerRuntimeSlots.items[source];
            const std::int32_t itemId = item->itemId;
            gSelectedImageCC3ED8 = item->imageNo;
            gSelectedCountCC483C = WindowCraft_CountRecipeMaterial(itemId);
            const std::int32_t skill = gCraftSkillIndexCC2368;
            auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
            const unsigned char previous = hover[source];
            const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
            hover[source] = static_cast<unsigned char>(previous + 1);
            gItemMarksD67290[source] = 1;
            if (skillId == 220) {
                WindowInstance* const window = currentWindow;
                auto* const element12 = window->FindElementById(WindowCraftElementId::Element12);
                if (element12) {
                    const WindowElement* const element = element12;
                    const std::int32_t y = element->y + window->y;
                    const std::int32_t x = element->x + window->x;
                    static_cast<void>(WindowInput_ActivateElement(&gCraftInputPrototype57D7F8, x, y, 0));
                }
            }
        }
        WindowDrag_Reset();
    }
    const std::int32_t selected = gSelectedItemD460C0;
    if (selected >= 0) {
        const std::int32_t skill = gCraftSkillIndexCC2368;
        const std::int32_t skillId = world::gSkillsF1BAFC[skill].skillId;
        const auto* const item = &world::gPlayerRuntimeSlots.items[selected];
        if (skillId == 220) {
            const std::uint16_t flags = item->flags;
            if (((flags & 0x200u) != 0 && (flags & 0x400u) == 0)
                || ((flags & 0x100u) != 0 && (flags & 0x400u) == 0)
                || item->hasDescription == 0 || item->count > 0 || item->itemType == 47) {
                static_cast<void>(WindowCraft_ReleaseMaterials(6));
            }
        } else if (skillId == 219 || skillId == 2033) {
            if (item->hasDescription != 0) {
                static_cast<void>(WindowCraft_ReleaseMaterials(6));
                world::AppendChatMessage_44B050(L"不需鉴定的物品", 4, 3);
            } else {
                const std::int32_t level =
                    world::gSkillsF1BAFC[skill].level + (gSkillRebirthModeEnabled_5F9754 != 0 ? 1 : 0);
                if (item->level > level) {
                    static_cast<void>(WindowCraft_ReleaseMaterials(6));
                    world::AppendChatMessage_44B050(L"不可鉴定的物品", 4, 3);
                }
            }
        } else if (skillId == 217 || skillId == 218 || skillId == 2031 || skillId == 2032) {
            bool rejected = item->hasDescription == 0;
            if (!rejected) {
                // 原分类1/2分支直接结束校验，不落入末尾等级判断。
                if (WindowCraft_ClassifyWork(item->itemType) == 2) {
                    const std::int32_t currentId =
                        world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                    rejected = currentId == 217 || currentId == 2031;
                } else if (WindowCraft_ClassifyWork(
                               world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].itemType) == 1) {
                    const std::int32_t currentId =
                        world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
                    rejected = currentId == 218 || currentId == 2032;
                } else if (WindowCraft_ClassifyWork(
                               world::gPlayerRuntimeSlots.items[gSelectedItemD460C0].itemType) == 0) {
                    rejected = true;
                } else {
                    const std::int32_t currentSkill = gCraftSkillIndexCC2368;
                    const std::uint8_t crystal = gSkillRebirthModeEnabled_5F9754;
                    const std::int32_t skillLevel = world::gSkillsF1BAFC[currentSkill].level;
                    const std::int32_t slot = gSelectedItemD460C0;
                    rejected = world::gPlayerRuntimeSlots.items[slot].level
                        > skillLevel + (crystal != 0 ? 1 : 0);
                }
            }
            if (rejected) {
                static_cast<void>(WindowCraft_ReleaseMaterials(6));
            }
        }
    }
    const bool hovered = (events & MouseFlag_Hover) != 0;
    if (hovered && gSelectedItemD460C0 >= 0 && (events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(WindowCraft_StartMaterialDrag(6));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 10 && WindowDrag_ReadSourceIndex() == 6) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t y = window->y + element->y;
            const std::int32_t x = window->x + element->x;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46), 223, 254, 0);
        }
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = window->x + element->x;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            const std::int32_t y = window->y + element->y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48), 223, imageNo, 0);
        }
    }
    if ((events & MouseFlag_Dragging) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = window->x + element->x;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            const std::int32_t y = window->y + element->y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48), 223, imageNo, 0);
        }
    }
    if (hovered) {
        gHoverTextD68AE8 = L"以滑鼠拖放方式，将素材置放于此处。";
    }
    const std::int32_t currentSelected = gSelectedItemD460C0;
    if (currentSelected < 0) {
        return 0;
    }
    if (hovered) {
        if (gMaterialSingleHoverCB0AA4 != 6) {
            gMaterialHintPageCF4FD8 = 0;
        }
        gMaterialSingleHoverCB0AA4 = 6;
        if ((events & MouseFlag_RDownEvent) != 0) {
            ++gMaterialHintPageCF4FD8;
            if (gMaterialHintPageCF4FD8 >= 4) {
                gMaterialHintPageCF4FD8 = 0;
            }
            consumed = 1;
        }
        gMaterialHoveredItemD67424 = currentSelected;
    }
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t windowY = window->y;
        const std::int32_t imageNo = gSelectedImageCC3ED8;
        const std::int32_t y = element->y + windowY + 24;
        const std::int32_t x = element->x + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x0047EC00。
static WindowAnimationData* WindowCraft_InitializeItemWork(WindowInstance* const window)
{
    for (std::int32_t index = 0; index < 6; ++index) {
        static_cast<void>(WindowCraft_ReleaseMaterials(index));
    }
    static_cast<void>(WindowCraft_ReleaseMaterials(6));
    gItemMarksD67290.fill(0);
    gMaterialHoverSlotsCB0A8C.fill(-2);
    gMaterialSingleHoverCB0AA4 = -2;
    gMaterialOccupancyCC4894 = {};
    gMaterialIndicesCA0458.fill(-1);
    gSelectedSlotsCA0A90.fill(-1);
    gSelectionCountsD66E0C.fill(0);
    gSelectionImagesCF3FF4.fill(-1);
    const std::int32_t skillId = world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
    const wchar_t* hint = nullptr;
    switch (skillId) {
    case 220:
        hint = L"可刻印的物品";
        break;
    case 2025:
        hint = L"可精炼的物品";
        break;
    case 219:
    case 2033:
        hint = L"可鉴定的物品";
        break;
    case 217:
    case 218:
    case 2031:
    case 2032:
        hint = L"可修理的物品";
        break;
    default:
        break;
    }
    if (hint != nullptr) {
        if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = hint;
    }
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3A;
    if (auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr; element11) element11->data.text->text = L"　请选择。";
    for (std::int32_t index = 11; index <= 13; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA39;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 0;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 1;
    if (auto* const handleSingleMaterialElement2 = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement2) handleSingleMaterialElement2->mouseCallback = WindowCraft_HandleSingleMaterial;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 0;
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 0;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, 0);
    }
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    gCraftDisplayImageCB2204 = -1;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const result = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, 0, 5);
    gSelectionModeD2D650 = 3;
    return result;
}

// 原函数：0x0047D4B0。
static std::int32_t WindowCraft_HasRecipeCrystalLevel()
{
    const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
    const std::int32_t skill = gCraftSkillIndexCC2368;
    const std::int32_t recipe = gCraftRecipeIndexD2D638;
    return world::gSkillsF1BAFC[skill].recipes8E8[recipe].level == crystalLevel ? 1 : 0;
}

// 原函数：0x0047D4F0。
static std::int32_t WindowCraft_HasSelectedCrystalLevel()
{
    if (gSelectedItemD460C0 < 0) {
        return 0;
    }
    const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
    const std::int32_t slot = gSelectedItemD460C0;
    return world::gPlayerRuntimeSlots.items[slot].level == crystalLevel ? 1 : 0;
}

// 原函数：0x0047D530。
static std::int32_t WindowCraft_HasMaterialCrystalLevel()
{
    for (std::int32_t index = 0; index < 6; ++index) {
        const std::int32_t slot = gSelectedSlotsCA0A90[index];
        if (slot < 0) {
            continue;
        }
        const std::int32_t type = world::gPlayerRuntimeSlots.items[slot + 8].itemType;
        if (type == 38 || type == 25 || type == 39 || type == 80) {
            const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
            const std::int32_t selected = gSelectedSlotsCA0A90[index];
            return world::gPlayerRuntimeSlots.items[selected + 8].level == crystalLevel ? 1 : 0;
        }
    }
    return 0;
}

// 原函数：0x0047D5B0。
static std::int32_t WindowCraft_HasFirstMaterialCrystalLevel()
{
    const std::int32_t slot = gSelectedSlotsCA0A90[0];
    if (slot < 0) {
        return 0;
    }
    const std::int32_t level = world::gPlayerRuntimeSlots.items[slot + 8].level;
    const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
    return level == crystalLevel ? 1 : 0;
}

// 原函数：0x004810A0。
static std::int32_t WindowCraft_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const auto initializeMode = [currentWindow](const std::uint8_t mode) {
        switch (mode) {
        case 0: static_cast<void>(WindowCraft_InitializeRecipe(currentWindow)); break;
        case 3: static_cast<void>(WindowCraft_InitializeItemWork(currentWindow)); break;
        case 6: static_cast<void>(WindowCraft_InitializeDecoration(currentWindow)); break;
        case 15: static_cast<void>(WindowCraft_InitializePetDecoration(currentWindow)); break;
        case 9: static_cast<void>(WindowCraft_InitializeRefining(currentWindow)); break;
        case 12: static_cast<void>(WindowCraft_InitializePacking(currentWindow)); break;
        default: break;
        }
    };
    if (value == -1) {
        gItemMarksD67290.fill(0);
        gWindowActionCA0174 = nullptr;
        gMaterialHoverSlotsCB0A8C.fill(-2);
        gMaterialHoveredItemD67424 = -1;
        gMaterialIndicesCA0458.fill(-1);
        gSelectedSlotsCA0A90.fill(-1);
        gMaterialHintPageCF4FD8 = 0;
        gSelectionCountsD66E0C.fill(0);
        gSelectedItemD460C0 = -1;
        gSelectionImagesCF3FF4.fill(-1);
        const std::uint8_t mode = gSelectionModeD2D650;
        gSelectedImageCC3ED8 = -1;
        gSelectedCountCC483C = -1;
        initializeMode(mode);
        if (auto* const drawInvalidEngravingNoticeElement = currentWindow->FindElementById(WindowCraftElementId::DrawInvalidEngravingNotice); drawInvalidEngravingNoticeElement) drawInvalidEngravingNoticeElement->visible = 0;
        if (auto* const drawEmptyEngravingNoticeElement = currentWindow->FindElementById(WindowCraftElementId::DrawEmptyEngravingNotice); drawEmptyEngravingNoticeElement) drawEmptyEngravingNoticeElement->visible = 0;
        return 1;
    }
    if (std::bit_cast<std::int8_t>(gSelectionModeD2D650) <= 2) {
        const std::int32_t skill = gCraftSkillIndexCC2368;
        const std::int32_t recipe = gCraftRecipeIndexD2D638;
        const std::int32_t expected = gCraftRecipeIdD2E440;
        const auto* const record = &world::gSkillsF1BAFC[skill];
        if (expected != record->recipes8E8[recipe].itemId) {
            const std::int32_t skillId = record->skillId;
            if (skillId != 255 && skillId != 606 && skillId != 2026 && skillId != 2025) {
                for (std::int32_t index = 0;; ++index) {
                    const std::int32_t itemId = record->recipes8E8[index].itemId;
                    if (itemId < 0) {
                        currentWindow->state |= 0x8000u;
                        static_cast<void>(audio::PlaySe_52C450(57, 320));
                        break;
                    }
                    if (itemId == expected) {
                        gCraftRecipeIndexD2D638 = index;
                        break;
                    }
                }
            }
        }
    }
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        WindowRuntime_States()[WindowId_Craft].window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    bool matchingCrystal = false;
    bool petAvailable = false;
    if (WindowSkill_CanUseSpecialSkill() != 0) {
        petAvailable = true;
        const auto configureCrystal = [currentWindow] {
            if (WindowSkill_CheckPetFp() != 0) {
                static_cast<void>(WindowCraft_ConfigurePreviews_47D220(currentWindow, 3, 5, 1, 0, 5));
            } else {
                static_cast<void>(WindowCraft_ConfigurePreviews_47D220(currentWindow, 3, 5, 0, 10, 5));
            }
        };
        if (gSelectionModeD2D650 == 0 && WindowCraft_HasRecipeCrystalLevel() != 0) {
            matchingCrystal = true;
            configureCrystal();
        }
        bool engraving = false;
        if (gSelectionModeD2D650 == 3) {
            if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
                engraving = true;
            } else if (WindowCraft_HasSelectedCrystalLevel() != 0) {
                matchingCrystal = true;
                configureCrystal();
            }
        }
        if (!engraving) {
            const std::uint8_t mode = gSelectionModeD2D650;
            const bool matches = mode == 6 || mode == 15
                ? WindowCraft_HasMaterialCrystalLevel() != 0
                : mode == 9 ? WindowCraft_HasFirstMaterialCrystalLevel() != 0
                : mode == 12 && WindowCraft_HasMaterialCrystalLevel() != 0;
            if (matches) {
                matchingCrystal = true;
                static_cast<void>(WindowCraft_SetPreviewPositions(
                    currentWindow, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
                configureCrystal();
            }
        }
        switch (std::bit_cast<std::int8_t>(gSelectionModeD2D650)) {
        case 0: case 3: case 6: case 9: case 12: case 15:
            if (matchingCrystal) {
                const std::int32_t imageNo = WindowSkill_ReadBattlePetImage();
                static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 3, 5, imageNo));
                static_cast<void>(WindowCraft_SetPreviewPositions(
                    currentWindow, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
            } else {
                static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 3, 5, 0));
            }
            break;
        case 1: case 4: case 7: case 10: case 13: case 16:
            if (gSkillSendModeC8A970 != 0) {
                const std::int32_t imageNo = WindowSkill_ReadBattlePetImage();
                static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 3, 5, imageNo));
            } else {
                static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 3, 5, 0));
            }
            break;
        case 5:
            if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
                break;
            }
            [[fallthrough]];
        case 2:
            static_cast<void>(WindowCraft_SetPreviewImages(
                currentWindow, 3, 5, gSkillSendModeC8A970 != 0 ? gSkillPetImageCB0A18 : 0));
            break;
        default:
            break;
        }
    } else {
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 3, 5, 0));
    }
    if (gSkillSendModeC8A970 != 0) {
        const std::uint8_t mode = gSelectionModeD2D650;
        if (mode == 1 || mode == 4 || mode == 7 || mode == 16 || mode == 10 || mode == 13) {
            if (petAvailable && gCraftPetAssistActive_D67B00 != 0) {
                static_cast<void>(WindowCraft_AnimatePreviewPositions(currentWindow, 4, 3, 5, gCraftDeadlineCF3B04));
            } else {
                initializeMode(static_cast<std::uint8_t>(mode - 1));
            }
        }
    }
    std::wstring buffer;
    const std::wstring& requestText = buffer;
    std::wstring temporary;
    const auto expired = [] {
        const std::uint64_t tick = platform::ReadTickCount64();
        return gCraftDeadlineCF3B04 <= tick;
    };
    // 三种参数合同在读取共享状态时才选取，不能把请求值预先截为同一宽度。
    const auto sendRequest = [&](const bool recipeRequest, const bool singleTarget) {
        std::int32_t offset = 0;
        if (gSkillRebirthModeEnabled_5F9754 != 0) {
            offset = 100;
        } else if (gSkillSendModeC8A970 != 0) {
            gSkillPetImageCB0A18 = WindowSkill_ReadBattlePetImage();
            offset = 200;
        }
        std::int32_t skill;
        std::int32_t request;
        std::int32_t target = -1;
        if (recipeRequest) {
            skill = gCraftSkillIndexCC2368;
            request = gCraftRequestValueD30400;
        } else {
            request = std::bit_cast<std::int8_t>(gSkillRequestByteD64E13);
            if (singleTarget) {
                target = gSelectedItemD460C0;
            }
            skill = gCraftSkillIndexCC2368;
        }
        static_cast<void>(cg::network::GetGameConnection().SendSkillUseRequest(skill + offset, request, target, requestText));
    };
    bool hideControls = false;
    if (gSelectionModeD2D650 == 1 && expired()) {
        const std::int32_t skill = gCraftSkillIndexCC2368;
        const std::int32_t recipe = gCraftRecipeIndexD2D638;
        cg::text::FormatTo(buffer, L"{}", world::gSkillsF1BAFC[skill].recipes8E8[recipe].itemId);
        for (std::int32_t index = 0; index < 6; ++index) {
            const std::int32_t slot = gSelectedSlotsCA0A90[index];
            if (slot >= 0 && gMaterialIndicesCA0458[index] >= 0) {
                cg::text::FormatTo(temporary, L"|{}", slot + 8);
                buffer += temporary;
            }
        }
        for (std::int32_t index = 0; index < 6; ++index) {
            const std::int32_t slot = gSelectedSlotsCA0A90[index];
            if (slot >= 0 && gMaterialIndicesCA0458[index] < 0) {
                cg::text::FormatTo(temporary, L"|{}", slot + 8);
                buffer += temporary;
            }
        }
        cg::core::NullSub_509080();
        sendRequest(true, false);
        gSelectionModeD2D650 = 2;
        hideControls = true;
    } else if (gSelectionModeD2D650 == 4 && expired()) {
        if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
            buffer = input::ContextText(cg::ui::detail::gInputContextD2DE70);
        } else {
            buffer.clear();
        }
        cg::core::NullSub_509080();
        sendRequest(false, true);
        gSelectionModeD2D650 = 5;
        hideControls = true;
    } else {
        const std::uint8_t mode = gSelectionModeD2D650;
        if ((mode == 7 || mode == 16) && expired()) {
            // 原两局部未赋初值；正常分组由材料写入，不补默认值或模拟栈残留。
            std::array<std::int32_t, 2> groupedSlots;
            std::int32_t& ordinarySlot = groupedSlots[0];
            std::int32_t& specialSlot = groupedSlots[1];
            for (std::int32_t index = 0; index < 6; ++index) {
                const std::int32_t slot = gSelectedSlotsCA0A90[index];
                if (slot < 0) {
                    continue;
                }
                const std::int32_t type = world::gPlayerRuntimeSlots.items[slot + 8].itemType;
                if (type == 38 || type == 25 || type == 39 || type == 80) {
                    specialSlot = slot + 8;
                } else {
                    ordinarySlot = slot + 8;
                }
            }
            cg::text::FormatTo(buffer, L"{}|{}", ordinarySlot, specialSlot);
            cg::core::NullSub_509080();
            sendRequest(false, false);
            const std::wstring& name = world::gPlayerRuntimeSlots.items[ordinarySlot].name;
            const std::uint8_t currentMode = gSelectionModeD2D650;
            gCraftNameCF46D4 = name;
            if (currentMode == 7) {
                gSelectionModeD2D650 = 8;
                hideControls = true;
            } else if (currentMode == 16) {
                gSelectionModeD2D650 = 17;
                hideControls = true;
            }
        } else if (mode == 10 && expired()) {
            std::int32_t selected = -1;
            for (std::int32_t index = 0; index < 6; ++index) {
                const std::int32_t slot = gSelectedSlotsCA0A90[index];
                if (slot >= 0 && world::IsItemEligibleForRefining_505110(slot + 8) != 0) {
                    selected = gSelectedSlotsCA0A90[index] + 8;
                    break;
                }
            }
            cg::text::FormatTo(buffer, L"{}", selected);
            cg::core::NullSub_509080();
            sendRequest(false, false);
            gSelectionModeD2D650 = 11;
            gCraftNameCF46D4 = world::gPlayerRuntimeSlots.items[selected].name;
            hideControls = true;
        } else if (mode == 13 && expired()) {
            buffer.clear();
            temporary = buffer;
            for (std::int32_t index = 0; index < 6; ++index) {
                const std::int32_t slot = gSelectedSlotsCA0A90[index];
                if (slot >= 0 && gMaterialIndicesCA0458[index] >= 0) {
                    cg::text::FormatTo(temporary, L"{}|", slot + 8);
                    buffer += temporary;
                    if (gMaterialIndicesCA0458[index] == 0) {
                        const std::int32_t selected = gSelectedSlotsCA0A90[index];
                        gCraftNameCF46D4 = world::gPlayerRuntimeSlots.items[selected + 8].name;
                    }
                }
            }
            cg::core::NullSub_509080();
            sendRequest(false, false);
            gSelectionModeD2D650 = 14;
            hideControls = true;
        }
    }
    if (!hideControls) {
        const std::uint8_t mode = gSelectionModeD2D650;
        if (mode == 3) {
            if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
                if (auto* const element10 = currentWindow->FindElementById(WindowCraftElementId::Element10); element10) element10->data.text->text = L"刻印的物品";
                if (auto* const element11 = currentWindow->FindElementById(WindowCraftElementId::Element11); element11) element11->data.text->text = L"　请选择。";
                if (input::gCurrentInputData != &cg::ui::detail::gInputContextD2DE70) {
                    if (auto* const buttonExecuteElement = currentWindow->FindElementById(WindowCraftElementId::ButtonExecute); buttonExecuteElement) buttonExecuteElement->visible = 1;
                    if (auto* const handleEngravingInputElement = currentWindow->FindElementById(WindowCraftElementId::HandleEngravingInput); handleEngravingInputElement) handleEngravingInputElement->visible = 1;
                    if (auto* const element8 = currentWindow->FindElementById(WindowCraftElementId::Element8); element8) element8->visible = 0;
                }
            } else {
                if (auto* const buttonExecute_26Element = currentWindow->FindElementById(WindowCraftElementId::ButtonExecute_26); buttonExecute_26Element) buttonExecute_26Element->visible = 1;
            }
        } else if (mode == 0 || mode == 6 || mode == 15 || mode == 9 || mode == 12) {
            if (auto* const buttonExecute_26Element = currentWindow->FindElementById(WindowCraftElementId::ButtonExecute_26); buttonExecute_26Element) buttonExecute_26Element->visible = 1;
        } else {
            hideControls = true;
        }
    }
    if (hideControls) {
        if (auto* const buttonExecute_26Element = currentWindow->FindElementById(WindowCraftElementId::ButtonExecute_26); buttonExecute_26Element) buttonExecute_26Element->visible = 0;
        if (auto* const buttonExecuteElement = currentWindow->FindElementById(WindowCraftElementId::ButtonExecute); buttonExecuteElement) buttonExecuteElement->visible = 0;
    }
    const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(world::gPlayerRuntimeSlots.imageNo);
    const std::int32_t imageNo = WindowSkillResult_ReadPreviewImage(mapped, 0);
    auto* const element5 = currentWindow->FindElementById(WindowCraftElementId::Element5);
    if (element5) {
        cg::yobi::SetYobiActionField2A8(
            element5->data.animation->action, imageNo);
    }
    auto* const element52 = currentWindow->FindElementById(WindowCraftElementId::Element5);
    if (element52) {
        cg::yobi::SetYobiActionField2B8(
            element52->data.animation->action, cg::yobi::YobiDirection_Down);
    }
    if (input::gCurrentInputData == &cg::ui::detail::gInputContextD2DE70) {
        WindowInstance* const window = currentWindow;
        auto* const handleEngravingInputElement = window->FindElementById(WindowCraftElementId::HandleEngravingInput);
        if (handleEngravingInputElement) {
            const WindowElement* const element = handleEngravingInputElement;
            const std::int32_t y = element->y + window->y;
            const std::int32_t x = element->x + window->x;
            static_cast<void>(WindowInput_ActivateElement(&gCraftInputPrototype57D7F8, x, y, 2));
        }
    }
    gMaterialHoveredItemD67424 = -1;
    return 1;
}

// 原函数：0x00481010。
static void WindowCraft_Release()
{
    WindowInstance* const quantityWindow = WindowRuntime_States()[WindowId_Quantity].window;
    if (quantityWindow != nullptr && WindowRuntime_States()[WindowId_Quantity].value20 == 11) {
        quantityWindow->state |= 0x8000u;
    }
    gCraftPetAssistActive_D67B00 = 0;
    WindowRuntime_DecrementOperationCount();
    WindowRuntime_DecrementGatherOperationCount();
    --gSkillOperationCount_D67AFD;
    for (std::int32_t index = 0; index < 6; ++index) {
        static_cast<void>(WindowCraft_ReleaseMaterials(index));
    }
    static_cast<void>(WindowCraft_ReleaseMaterials(6));
    --gSkillStatusIconReferences_D67AFE;
    if (gSkillStatusIconReferences_D67AFE == 0) {
        const std::int32_t state = world::GetPlayerStatusIcon_51BEC0();
        if (state >= 9 && state <= 12) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(0));
        }
    }
}

// 原函数：0x00482000。
static std::int32_t WindowCraft_Draw(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const element5 = currentWindow->FindElementById(WindowCraftElementId::Element5);
    if (element5) {
        cg::yobi::YobiAction* const action =
            element5->data.animation->action;
        const std::uint8_t mode = gSelectionModeD2D650;
        const std::int32_t imageNo = cg::yobi::GetYobiActionField2A8(action);
        if (mode == 2 || mode == 5) {
            const std::int32_t actionId = cg::yobi::GetYobiActionField2B0(action);
            if (actionId == 18 || actionId == 5 || actionId == 15 || actionId == 10) {
                const bool succeeded = actionId == 18 || actionId == 5;
                const std::int32_t defaultAction = succeeded ? 5 : 10;
                cg::yobi::SetYobiActionField2B0(action, defaultAction);
                for (const auto& range : kCraftAlternateActionImages) {
                    if (imageNo >= range[0] && imageNo <= range[1]) {
                        cg::yobi::SetYobiActionField2B0(action, succeeded ? 18 : 15);
                    }
                }
                if (imageNo == 0x18A26) {
                    cg::yobi::SetYobiActionField2B0(action, defaultAction);
                }
            }
        }
        if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
            auto* const element52 = currentWindow->FindElementById(WindowCraftElementId::Element5);
            if (element52) {
                cg::yobi::SetYobiActionMixFlagsOverrideMask(
                    element52->data.animation->action, render::GraphicMixFlag_OriginalPalette);
            }
            auto* const element53 = currentWindow->FindElementById(WindowCraftElementId::Element5);
            if (element53) {
                cg::yobi::SetYobiActionMixFlagsOverrideValue(
                    element53->data.animation->action, render::GraphicMixFlag_OriginalPalette);
            }
        } else {
            auto* const element52 = currentWindow->FindElementById(WindowCraftElementId::Element5);
            if (element52) {
                cg::yobi::SetYobiActionMixFlagsOverrideMask(
                    element52->data.animation->action, render::GraphicMixFlag_None);
            }
        }
    }
    const std::int32_t hovered = gMaterialHoveredItemD67424;
    if (hovered >= 0) {
        static_cast<void>(WindowItem_DrawEquipmentTooltip(hovered, gMaterialHintPageCF4FD8));
    }
    if (gSelectedItemD460C0 >= 0) {
        WindowInstance* const window = currentWindow;
        auto* const element12 = window->FindElementById(WindowCraftElementId::Element12);
        if (element12) {
            const WindowElement* const element = element12;
            const auto y = static_cast<std::int16_t>(element->y + window->y);
            const auto x = static_cast<std::int16_t>(element->x + window->x);
            static_cast<void>(render::DrawText_456780(
                x, y, 3, 12, 127, cg::ui::detail::gInputContextD2DE70.text, 0, 0));
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0047D170。
std::int32_t WindowCraft_SetPreviewPositions(
    WindowInstance* const window, const std::int32_t anchor,
    const std::int32_t first, const std::int32_t second,
    const std::int32_t dx, const std::int32_t dy)
{
    if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(first)) : nullptr; indexedElement) indexedElement->visible = 0;
    auto* elementsWindow = window;
    if (auto* const indexedElement2 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(first)) : nullptr, * const indexedElement3 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(anchor)) : nullptr; indexedElement2 && indexedElement3) indexedElement2->x = indexedElement3->x + dx;
    elementsWindow = window;
    if (auto* const indexedElement4 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(first)) : nullptr, * const indexedElement5 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(anchor)) : nullptr; indexedElement4 && indexedElement5) indexedElement4->y = indexedElement5->y + dy;
    if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(second)) : nullptr; indexedElement6) indexedElement6->visible = 0;
    elementsWindow = window;
    if (auto* const indexedElement7 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(second)) : nullptr, * const indexedElement8 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(anchor)) : nullptr; indexedElement7 && indexedElement8) indexedElement7->x = indexedElement8->x + dx;
    elementsWindow = window;
    if (auto* const indexedElement9 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(second)) : nullptr, * const indexedElement10 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(anchor)) : nullptr; indexedElement9 && indexedElement10) indexedElement9->y = indexedElement10->y + dy;
    elementsWindow = window;
    auto* const dataElement = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(first)) : nullptr;
    if (!dataElement) { return 0; }
    auto* const data = dataElement->data.animation;
    if (cg::yobi::GetYobiActionField2A8(data->action) > 0) {
        if (auto* const indexedElement11 = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(dy > 0 ? first : second)) : nullptr; indexedElement11) indexedElement11->visible = 1;
    }
    return first * 36;
}

// 原函数：0x0047D220。
WindowAnimationData* WindowCraft_ConfigurePreviews_47D220(
    WindowInstance* const window, const std::int32_t first, const std::int32_t second,
    const std::int32_t flag, const std::int32_t actionId, const std::int32_t direction)
{
    if (!window) return nullptr;
    const std::uint32_t value = flag == 0 ? 1u : 0u;
    if (auto* const element = window->FindElementById(NativeElementKey(first))) {
        auto* const data = element->data.animation;
        data->value04 = value;
        cg::yobi::SetYobiActionField2B0(data->action, actionId);
        cg::yobi::SetYobiActionField2B8(data->action, direction);
    }
    if (auto* const element = window->FindElementById(NativeElementKey(second))) {
        auto* const data = element->data.animation;
        data->value04 = value;
        cg::yobi::SetYobiActionField2B0(data->action, actionId);
        cg::yobi::SetYobiActionField2B8(data->action, direction);
        return data;
    }
    return nullptr;
}

// 原函数：0x00484D60。
static std::int32_t WindowCraft_HandleMaterialTooltip(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto events = static_cast<std::uint8_t>(state);
    const std::int32_t slot = index - 15;
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t x = window->x + element->x;
            const std::int32_t y = window->y + element->y;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                0xDF, imageNo, 0);
        }
    }
    const std::int32_t selected = gSelectedSlotsCA0A90[slot];
    if (selected < 0) {
        return 0;
    }
    if ((events & MouseFlag_Hover) != 0) {
        if (gMaterialHoverSlotsCB0A8C[slot] != slot) {
            gMaterialHintPageCF4FD8 = 0;
        }
        gMaterialHoverSlotsCB0A8C[slot] = slot;
        if ((events & MouseFlag_RDownEvent) != 0) {
            ++gMaterialHintPageCF4FD8;
            if (gMaterialHintPageCF4FD8 >= 4) {
                gMaterialHintPageCF4FD8 = 0;
            }
            consumed = 1;
        }
        gMaterialHoveredItemD67424 = selected + 8;
    }
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t imageNo = gSelectionImagesCF3FF4[slot];
        const std::int32_t y = window->y + element->y + 24;
        const std::int32_t x = window->x + element->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x0047E5E0。
static WindowAnimationData* WindowCraft_ShowRecipeResult(WindowInstance* const window)
{
    gCraftPetAssistActive_D67B00 = 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        static_cast<void>(WindowCraft_ReleaseMaterials(index));
    }
    const std::int32_t skillIndex = gCraftSkillIndexCC2368;
    const std::int32_t skillId = world::gSkillsF1BAFC[skillIndex].skillId;
    const std::wstring name = skillId == 255 || skillId == 2026
        ? gCraftNameCF46D4
        : world::gSkillsF1BAFC[skillIndex].recipes8E8[gCraftRecipeIndexD2D638].name;
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = name;
    const std::int32_t result = gCraftResultCC46D8;
    auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr;
    if (element11) {
        auto* const resultText = element11->data.text;
        if (result != 0) {
            resultText->text = L"　制造成功了。" /* 原常量参考：0x00586638。 */;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        } else {
            resultText->text = L"　制造失败了。" /* 原常量参考：0x00586628。 */;
            static_cast<void>(audio::PlaySe_52C450(58, 320));
        }
    }
    if (gCraftLevelUpCF4974 != 0) {
        if (auto* const element12 = window ? window->FindElementById(WindowCraftElementId::Element12) : nullptr; element12) element12->data.text->text = kCraftLevelUp58661C;
    } else {
        auto* const element12 = window ? window->FindElementById(WindowCraftElementId::Element12) : nullptr;
        if (element12) {
            auto* const data = element12->data.text;
            cg::text::FormatTo(data->text, kCraftExperience58660C, gCraftExperienceCC3204);
        }
    }
    auto* const element13 = window ? window->FindElementById(WindowCraftElementId::Element13) : nullptr;
    if (element13) {
        std::wstring& increments = element13->data.text->text;
        increments.clear();
        if (gCraftEnduranceD2F328 != 0) {
            increments.append(std::format(kCraftEndurance586600, gCraftEnduranceD2F328));
        }
        if (gCraftDexterityCA0448 != 0) {
            increments.append(std::format(kCraftDexterity5865F4, gCraftDexterityCA0448));
        }
        if (gCraftIntelligenceCB0F84 != 0) {
            increments.append(std::format(kCraftIntelligence5865E8, gCraftIntelligenceCB0F84));
        }
    }
    if (auto* const element14 = window ? window->FindElementById(WindowCraftElementId::Element14) : nullptr; element14) element14->data.text->text = gCraftInjuryD2EA78 != 0 ? L"受伤了！" /* 原常量参考：0x005865DC。 */ : L"";
    const std::int32_t imageResult = gCraftResultCC46D8;
    gCraftDisplayImageCB2204 = -1;
    if (imageResult != 0) {
        gCraftDisplayImageCB2204 = gCraftResultImageCF4B60;
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA38;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3A;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 0;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 0;
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 1;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 1;
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 1;
    const bool succeeded = gCraftResultCC46D8 != 0;
    const std::int32_t defaultAction = succeeded ? 5 : 10;
    const std::int32_t alternateAction = succeeded ? 18 : 15;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, defaultAction);
    }
    std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
    for (const auto& range : kCraftAlternateActionImages) {
        if (imageNo >= range[0] && imageNo <= range[1]) {
            auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
            if (element53) {
                cg::yobi::SetYobiActionField2B0(
                    element53->data.animation->action, alternateAction);
            }
            imageNo = world::gPlayerRuntimeSlots.imageNo;
        }
    }
    if (imageNo == 0x18A26) {
        auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
        if (element53) {
            cg::yobi::SetYobiActionField2B0(
                element53->data.animation->action, defaultAction);
        }
    }
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* last;
    if (gSkillSendModeC8A970 != 0) {
        last = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, gCraftResultCC46D8 != 0 ? 6 : 8, 5);
    } else {
        last = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 0, 10, 5);
    }
    gSelectionModeD2D650 = 2;
    return last;
}

// 原函数：0x0047EBE0。
static WindowAnimationData* WindowCraft_RefreshRecipeResult()
{
    WindowInstance* const window = WindowRuntime_States()[WindowId_Craft].window;
    return window != nullptr ? WindowCraft_ShowRecipeResult(window) : nullptr;
}

// 原函数：0x004F39F0。
WindowAnimationData* WindowCraft_ReceiveRecipeResult(const std::int32_t result, const char* const text)
{
    gCraftResultDetailsD2E92C.clear();
    gCraftResultCC46D8 = result;
    gCraftExperienceCC3204 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    gCraftValueCC4840 = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    gCraftLevelUpCF4974 = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gCraftEnduranceD2F328 = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    gCraftDexterityCA0448 = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    gCraftIntelligenceCB0F84 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gCraftInjuryD2EA78 = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    gCraftResultImageCF4B60 = network::ParseDelimitedInteger_52CFA0(text, '|', 8);
    if (result != 0) {
        gCraftResultSuffixD672B4 = L"的合成成功了。" /* 原常量参考：0x00589FB8。 */;
    } else {
        gCraftResultImageCF4B60 = 0;
        gCraftResultSuffixD672B4 = L"的合成失败了。" /* 原常量参考：0x00589FA8。 */;
    }
    char buffer[768];
    std::wstring lostItem;
    std::int32_t lines = 0;
    const auto appendValue = [&](const std::wstring_view format, const std::int32_t value) {
        // 四种数值行按原GBK最多22字节，始终低于28列换行门槛。
        gCraftResultDetailsD2E92C += std::vformat(format, std::make_wformat_args(value)) + L'\n';
        ++lines;
    };
    if (gCraftExperienceCC3204 > 0) {
        appendValue(kCraftExperience589820, gCraftExperienceCC3204);
    }
    if (gCraftLevelUpCF4974 != 0 && lines < 8) {
        gCraftResultDetailsD2E92C += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(64, 320));
    }
    if (gCraftEnduranceD2F328 != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, gCraftEnduranceD2F328);
    }
    if (gCraftDexterityCA0448 != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, gCraftDexterityCA0448);
    }
    if (gCraftIntelligenceCB0F84 != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, gCraftIntelligenceCB0F84);
    }
    for (std::int32_t index = 0; index < 6 && lines < 8; ++index) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', index + 9, 255, buffer));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        if (std::strlen(buffer) == 0) {
            break;
        }
        lostItem = std::format(L"{} 失去了！" /* 原常量参考：0x00589F9C。 */, cg::text::FromGbkWide(buffer));
        if (std::strlen(buffer) + 9 < 28) { // 原GBK行宽：名称字节数 + 空格1B +「失去了！」8B。
            lostItem += L'\n';
        }
        gCraftResultDetailsD2E92C += lostItem;
        ++lines;
    }
    if (gCraftInjuryD2EA78 != 0 && lines < 8) {
        gCraftResultDetailsD2E92C += LR"(受伤了！
)" /* 原常量参考：0x00589CD4。 */;
    }
    return WindowCraft_RefreshRecipeResult();
}

// 原函数：0x00484E90。
static std::int32_t WindowCraft_HandleItemTooltip(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto events = static_cast<std::uint8_t>(state);
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t y = window->y + element->y;
            const std::int32_t x = window->x + element->x;
            const std::uint32_t imageNo = cg::render::gHoverFramePaletteIndex_D69768;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
                0xDF, imageNo, 0);
        }
    }
    const std::int32_t selected = gSelectedItemD460C0;
    if (selected < 0) {
        return 0;
    }
    if ((events & MouseFlag_Hover) != 0) {
        if (gMaterialSingleHoverCB0AA4 != 6) {
            gMaterialHintPageCF4FD8 = 0;
        }
        gMaterialSingleHoverCB0AA4 = 6;
        if ((events & MouseFlag_RDownEvent) != 0) {
            ++gMaterialHintPageCF4FD8;
            if (gMaterialHintPageCF4FD8 >= 4) {
                gMaterialHintPageCF4FD8 = 0;
            }
            consumed = 1;
        }
        gMaterialHoveredItemD67424 = selected;
    }
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t imageNo = gSelectedImageCC3ED8;
        const std::int32_t y = window->y + element->y + 24;
        const std::int32_t x = window->x + element->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, imageNo, 0, &descriptor));
    }
    return consumed;
}

// 原函数：0x0047F3D0。
static WindowAnimationData* WindowCraft_ShowItemWorkResult(WindowInstance* const window)
{
    const std::int32_t entryResult = gItemWorkResultCB05FC;
    const std::int32_t selected = gSelectedItemD460C0;
    gCraftPetAssistActive_D67B00 = 0;
    if (entryResult != 0 && world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId != 220) {
        static_cast<void>(WindowCraft_ReleaseMaterials(6));
    }
    const auto* const item = &world::gPlayerRuntimeSlots.items[selected];
    if (auto* const element10 = window ? window->FindElementById(WindowCraftElementId::Element10) : nullptr; element10) element10->data.text->text = item->name;
    const std::int32_t skillId = world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId;
    const wchar_t* message = nullptr;
    switch (skillId) {
    case 217:
    case 218:
    case 2031:
    case 2032:
        message = gItemWorkResultCB05FC != 0 ? L"　的修理成功了！" /* 原常量参考：0x00586708。 */ : L"　的修理失败了。" /* 原常量参考：0x005866F4。 */;
        break;
    case 219:
    case 2033:
        message = gItemWorkResultCB05FC != 0 ? L"　的鉴定成功了！" /* 原常量参考：0x00586730。 */ : L"　的鉴定失败了。" /* 原常量参考：0x0058671C。 */;
        break;
    case 220:
        message = gItemWorkResultCB05FC != 0 ? L"　刻印上去了！" /* 原常量参考：0x00586774。 */ : L"　刻印失败了。" /* 原常量参考：0x00586764。 */;
        break;
    case 2025:
        message = gItemWorkResultCB05FC != 0 ? L"精炼成功了！" /* 原常量参考：0x00586754。 */ : L"精炼失败了！" /* 原常量参考：0x00586744。 */;
        break;
    }
    if (message != nullptr) {
        if (auto* const element11 = window ? window->FindElementById(WindowCraftElementId::Element11) : nullptr; element11) element11->data.text->text = message;
    }
    static_cast<void>(audio::PlaySe_52C450(gItemWorkResultCB05FC != 0 ? 57 : 58, 320));
    if (gItemWorkLevelUpCB03D8 != 0) {
        if (auto* const element12 = window ? window->FindElementById(WindowCraftElementId::Element12) : nullptr; element12) element12->data.text->text = kCraftLevelUp58661C;
    } else {
        auto* const element12 = window ? window->FindElementById(WindowCraftElementId::Element12) : nullptr;
        if (element12) {
            auto* const data = element12->data.text;
            cg::text::FormatTo(data->text, kCraftExperience58660C, gItemWorkExperienceC8A950);
        }
    }
    auto* const element13 = window ? window->FindElementById(WindowCraftElementId::Element13) : nullptr;
    if (element13) {
        std::wstring& increments = element13->data.text->text;
        increments.clear();
        // 原门禁取制造回复，显示值取本次物品回复；不能收敛成同一来源。
        if (gCraftEnduranceD2F328 != 0) {
            increments.append(std::format(kCraftEndurance586600, gItemWorkEnduranceCF46B4));
        }
        if (gCraftDexterityCA0448 != 0) {
            increments.append(std::format(kCraftDexterity5865F4, gItemWorkDexterityCB0A14));
        }
        if (gCraftIntelligenceCB0F84 != 0) {
            increments.append(std::format(kCraftIntelligence5865E8, gItemWorkIntelligenceD30418));
        }
    }
    if (auto* const element14 = window ? window->FindElementById(WindowCraftElementId::Element14) : nullptr; element14) element14->data.text->text = gItemWorkInjuryD2E2A8 != 0 ? L"受伤了！" /* 原常量参考：0x005865DC。 */ : L"";
    const std::int32_t imageResult = gItemWorkResultCB05FC;
    gCraftDisplayImageCB2204 = -1;
    if (imageResult != 0) {
        gCraftDisplayImageCB2204 = item->imageNo;
    }
    if (auto* const element30 = window ? window->FindElementById(WindowCraftElementId::Element30) : nullptr; element30) element30->data.graphic->imageNo = 0x3BA39;
    if (auto* const element15 = window ? window->FindElementById(WindowCraftElementId::Element15) : nullptr; element15) element15->data.graphic->imageNo = 0x3BA3A;
    for (std::int32_t index = 15; index <= 20; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement2) indexedElement2->mouseCallback = WindowCraft_HandleMaterialTooltip;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowCraftElementId::Element25) : nullptr; element25) element25->visible = 0;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowCraftElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const handleSingleMaterialElement = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement) handleSingleMaterialElement->visible = 1;
    if (auto* const handleSingleMaterialElement2 = window ? window->FindElementById(WindowCraftElementId::HandleSingleMaterial) : nullptr; handleSingleMaterialElement2) handleSingleMaterialElement2->mouseCallback = WindowCraft_HandleItemTooltip;
    if (auto* const drawResultImage_24Element = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element) drawResultImage_24Element->visible = 1;
    if (world::gSkillsF1BAFC[gCraftSkillIndexCC2368].skillId == 220) {
        if (auto* const drawResultImage_24Element2 = window ? window->FindElementById(WindowCraftElementId::DrawResultImage_24) : nullptr; drawResultImage_24Element2) drawResultImage_24Element2->visible = 0;
    }
    if (auto* const buttonExecute_26Element = window ? window->FindElementById(WindowCraftElementId::ButtonExecute_26) : nullptr; buttonExecute_26Element) buttonExecute_26Element->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowCraftElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 1;
    if (auto* const buttonCloseElement = window ? window->FindElementById(WindowCraftElementId::ButtonClose) : nullptr; buttonCloseElement) buttonCloseElement->visible = 1;
    if (auto* const element5 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr; element5) element5->data.animation->value04 = 1;
    const bool succeeded = gItemWorkResultCB05FC != 0;
    const std::int32_t defaultAction = succeeded ? 5 : 10;
    const std::int32_t alternateAction = succeeded ? 18 : 15;
    auto* const element52 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
    if (element52) {
        cg::yobi::SetYobiActionField2B0(
            element52->data.animation->action, defaultAction);
    }
    std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
    for (const auto& range : kCraftAlternateActionImages) {
        if (imageNo >= range[0] && imageNo <= range[1]) {
            auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
            if (element53) {
                cg::yobi::SetYobiActionField2B0(
                    element53->data.animation->action, alternateAction);
            }
            imageNo = world::gPlayerRuntimeSlots.imageNo;
        }
    }
    if (imageNo == 0x18A26) {
        auto* const element53 = window ? window->FindElementById(WindowCraftElementId::Element5) : nullptr;
        if (element53) {
            cg::yobi::SetYobiActionField2B0(
                element53->data.animation->action, defaultAction);
        }
    }
    if (auto* const handleEngravingInputElement = window ? window->FindElementById(WindowCraftElementId::HandleEngravingInput) : nullptr; handleEngravingInputElement) handleEngravingInputElement->visible = 0;
    if (auto* const element8 = window ? window->FindElementById(WindowCraftElementId::Element8) : nullptr; element8) element8->visible = 0;
    if (auto* const buttonExecuteElement = window ? window->FindElementById(WindowCraftElementId::ButtonExecute) : nullptr; buttonExecuteElement) buttonExecuteElement->visible = 0;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 4, 3, 5, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* last;
    if (gSkillSendModeC8A970 != 0) {
        last = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 1, gItemWorkResultCB05FC != 0 ? 6 : 8, 5);
    } else {
        last = WindowCraft_ConfigurePreviews_47D220(window, 3, 5, 0, 10, 5);
    }
    gSelectionModeD2D650 = 5;
    return last;
}

// 原函数：0x0047FAC0。
static WindowAnimationData* WindowCraft_RefreshItemWorkResult()
{
    WindowInstance* const window = WindowRuntime_States()[WindowId_Craft].window;
    return window != nullptr ? WindowCraft_ShowItemWorkResult(window) : nullptr;
}

// 原函数：0x004F1800。
WindowAnimationData* WindowCraft_ReceiveItemWorkResult(const std::int32_t result, const char* const text)
{
    gItemWorkDetailsD2DB00.clear();
    gItemWorkResultCB05FC = result;
    gItemWorkExperienceC8A950 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    gItemWorkValueD2E2AC = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    gItemWorkLevelUpCB03D8 = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gItemWorkEnduranceCF46B4 = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    gItemWorkDexterityCB0A14 = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    gItemWorkIntelligenceD30418 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gItemWorkInjuryD2E2A8 = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    const std::int32_t skillId = gItemWorkSkillIdD67CF0;
    const wchar_t* prefix;
    switch (skillId) {
    case 217:
    case 218:
    case 2031:
    case 2032: prefix = L"的修理" /* 原常量参考：0x00589CF8。 */; break;
    case 219:
    case 2033: prefix = L"的鉴定" /* 原常量参考：0x00589D00。 */; break;
    default: prefix = L"的刻印" /* 原常量参考：0x00589D08。 */; break;
    }
    gItemWorkSuffixD2DADC = prefix;
    gItemWorkSuffixD2DADC += result != 0 ? L"成功了。" /* 原常量参考：0x00589CEC。 */ : L"失败了。" /* 原常量参考：0x00589CE0。 */;
    std::int32_t lines = 0;
    const auto appendValue = [&](const std::wstring_view format, const std::int32_t value) {
        // 四种数值行按原GBK最多22字节，始终低于28列换行门槛。
        gItemWorkDetailsD2DB00 += std::vformat(format, std::make_wformat_args(value)) + L'\n';
        ++lines;
    };
    if (gItemWorkExperienceC8A950 > 0) {
        appendValue(kCraftExperience589820, gItemWorkExperienceC8A950);
    }
    if (gItemWorkLevelUpCB03D8 != 0 && lines < 8) {
        gItemWorkDetailsD2DB00 += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(64, 320));
    }
    if (gItemWorkEnduranceCF46B4 != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, gItemWorkEnduranceCF46B4);
    }
    if (gItemWorkDexterityCB0A14 != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, gItemWorkDexterityCB0A14);
    }
    if (gItemWorkIntelligenceD30418 != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, gItemWorkIntelligenceD30418);
    }
    if (gItemWorkInjuryD2E2A8 != 0 && lines < 8) {
        gItemWorkDetailsD2DB00 += LR"(受伤了！
)" /* 原常量参考：0x00589CD4。 */;
    }
    return WindowCraft_RefreshItemWorkResult();
}

// 原函数：0x0047D2E0。
WindowAnimationData* WindowCraft_AnimatePreviewPositions(
    WindowInstance*, const std::int32_t anchor,
    const std::int32_t first, const std::int32_t second, const std::uint64_t deadline)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return nullptr;
    const std::uint64_t tick = platform::ReadTickCount64();
    const std::uint64_t start = gSkillPreviewTickCF46C8;
    const std::int64_t interval = std::bit_cast<std::int64_t>(deadline - start) / 11;
    const std::int64_t phase = interval == 0 ? 0 : std::bit_cast<std::int64_t>(tick - start) / interval;
    if (static_cast<std::uint64_t>(phase) <= 10) {
        if ((phase & 1) == 0) {
            const std::int32_t index = static_cast<std::int32_t>(phase / 2);
            const std::int32_t next = index + 1 >= 6 ? 0 : index + 1;
            const std::int64_t elapsed = std::bit_cast<std::int64_t>(tick - gSkillPreviewTickCF46C8);
            const std::int64_t remainder = elapsed % interval;
            const auto& from = kSkillPreviewPoints57D7D0[index];
            const auto& to = kSkillPreviewPoints57D7D0[next];
            const std::int32_t y = static_cast<std::int32_t>(
                (to[1] - from[1]) * remainder / interval + from[1]);
            const std::int32_t x = static_cast<std::int32_t>(
                (to[0] - from[0]) * remainder / interval + from[0]);
            static_cast<void>(WindowCraft_SetPreviewPositions(currentWindow, anchor, first, second, x, y));
            return WindowCraft_ConfigurePreviews_47D220(currentWindow, first, second, 1, 1, to[2]);
        }
        std::int32_t index = static_cast<std::int32_t>(phase / 2 + 1);
        if (index > 7) {
            index = 0;
        }
        const auto& point = kSkillPreviewPoints57D7D0[index];
        static_cast<void>(WindowCraft_SetPreviewPositions(currentWindow, anchor, first, second, point[0], point[1]));
        return WindowCraft_ConfigurePreviews_47D220(currentWindow, first, second, 1, 0, 5);
    }
    static_cast<void>(WindowCraft_SetPreviewPositions(
        currentWindow, anchor, first, second, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    return WindowCraft_ConfigurePreviews_47D220(currentWindow, first, second, 1, 0, 5);
}

// 原函数：0x0047D280。
std::int32_t WindowCraft_SetPreviewImages(
    WindowInstance* const window, const std::int32_t first,
    const std::int32_t second, const std::int32_t imageNo)
{
    auto* const indexedElement = window ? window->FindElementById(NativeElementKey(first)) : nullptr;
    if (indexedElement) {
        cg::yobi::SetYobiActionField2A8(
            indexedElement->data.animation->action, imageNo);
    }
    auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(second)) : nullptr;
    if (indexedElement2) {
        cg::yobi::SetYobiActionField2A8(
            indexedElement2->data.animation->action, imageNo);
    }
    if (imageNo <= 0) {
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(first)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(second)) : nullptr; indexedElement4) indexedElement4->visible = 0;
    }
    return first * 36;
}

} // namespace detail

} // namespace cg::ui
