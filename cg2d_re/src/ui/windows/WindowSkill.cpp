#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowSkillElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    ButtonBack = 5,
    Element6 = 6,
    ButtonUse = 7,
    Element8 = 8,
    ButtonPetSkillBack = 9,
    Element10 = 10,
    Element11 = 11,
    HandleVerticalScroll = 12,
    ButtonPreviousRow = 13,
    ButtonNextRow = 14,
    ButtonPreviousPage = 15,
    ButtonNextPage = 16,
    Scroll = 17,
    HandleSkillRow = 18,
    HandleSkillRow_19 = 19,
    HandleSkillRow_20 = 20,
    HandleSkillRow_21 = 21,
    HandleSkillRow_22 = 22,
    HandleSkillRow_23 = 23,
    HandleSkillRow_24 = 24,
    HandleSkillRow_25 = 25,
    HandleSkillRow_26 = 26,
    HandleSkillRow_27 = 27,
    Element28 = 28,
    Element29 = 29,
    HandleSideArea = 30,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004EF710。
static std::int32_t WindowSkill_ClassifyAttributeSkill(std::int32_t skillId); // 221..224分类0..3，其余-1。

// 原函数：0x004F0200。
static void WindowSkill_ReceiveTextTargets(const char* text); // ysVc完整名单生产；唯一回复调用方弃用混合返回。

// 原函数：0x004871B0。
static cg::yobi::YobiAction* WindowSkill_OpenTextTargets(); // 技能目标名称/颜色借用及45创建，不回滚失败计数。

// 原函数：0x00487020。
static std::int32_t WindowSkill_SelectTextTarget(std::int32_t index, const wchar_t* text); // 请求属性/发送技能及释放通知。

// 原函数：0x00479B80。
static void WindowSkill_ShowSkillList(WindowInstance* window, std::int32_t skillIndex); // 技能列表定位，不以used过滤索引查找。

// 原函数：0x0047B6C0。
static void WindowSkill_DrawSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t enabled, std::int32_t textLayer, std::int32_t graphicLayer,
    std::uint32_t color); // 固定配对对应技能，需求槽数按S8。

// 原函数：0x0047B820。
static std::int32_t WindowSkill_HandleSkillRow(std::int32_t index, std::int32_t state); // 列表排序请求、打开技术与右键拖动。

// 原函数：0x00479700。
static void WindowSkill_HandleDrag(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind4拖动，仅message0推进。

// 原函数：0x004797A0。
static std::int32_t WindowSkill_StartDrag(std::int32_t sourceIndex); // 安装479700并透传返回。

// 原函数：0x00479C80。
static void WindowSkill_ShowTechniques(WindowInstance* window, std::int32_t pairIndex); // 切技术模式，负参数保留原技能。

// 原函数：0x00479DA0。
static void WindowSkill_ShowRecipes(WindowInstance* window, std::int32_t level); // 切配方，滚动先用旧上限。

// 原函数：0x00479EA0。
static void WindowSkill_ShowBattleSkills(WindowInstance* window, std::int32_t skillIndex); // 战斗列表及原过滤表尾字节计数门槛。

// 原函数：0x0047A000。
static void WindowSkill_ShowBattleTechniques(WindowInstance* window, std::int32_t index, std::uint8_t directSkill); // 战斗技术初始化。

// 原函数：0x0047C570。
static std::int32_t WindowSkill_HandleBattleSkillRow(std::int32_t index, std::int32_t state); // 战斗列表行，技能标志决定可用。

// 原函数：0x0047C700。
static std::int32_t WindowSkill_SelectBattleTechnique(std::int32_t index, std::int32_t state); // 战斗技术选择，独立FP读取与回调后重取窗口。

// 原函数：0x0047A1A0。
static void WindowSkill_ShowPetSkills(WindowInstance* window); // 宠物模式初始化，保留弃用结果的FP读取。

// 原函数：0x0047A320。
static void WindowSkill_ShowRidingSkills(WindowInstance* window); // 骑乘模式初始化，名称借自身战斗对象。

// 原函数：0x0047CB00。
static void WindowSkill_DrawPetSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 宠物行绘制，两次宠物查询。

// 原函数：0x0047CE70。
static void WindowSkill_DrawRidingSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 骑乘行绘制，固定技术记录。

// 原函数：0x0047CC80。
static std::int32_t WindowSkill_SelectPetSkill(std::int32_t index, std::int32_t state); // 宠物选择，不加滚动偏移。

// 原函数：0x0047CFE0。
static std::int32_t WindowSkill_SelectRidingSkill(std::int32_t index, std::int32_t state); // 骑乘选择，FP不足不补失败音效。

// 原函数：0x0047B110。
static std::int32_t WindowSkill_ButtonBack(std::int32_t index, std::int32_t state); // 固定入口窗口的返回按钮。

// 原函数：0x0047B210。
static std::int32_t WindowSkill_ButtonUse(std::int32_t index, std::int32_t state); // TUC01：先借元素再读U8动作标志；Y/X发送后更新同源占用。

// 原函数：0x0047C950。
static std::int32_t WindowSkill_ButtonPetSkillBack(std::int32_t index, std::int32_t state); // 无可用宠物技能时略过，通知-1。

// 原函数：0x0047B3D0。
static std::int32_t WindowSkill_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 前一行，bit400。

// 原函数：0x0047B470。
static std::int32_t WindowSkill_ButtonNextRow(std::int32_t index, std::int32_t state); // 后一行，bit400。

// 原函数：0x0047B520。
static std::int32_t WindowSkill_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 前十行，音效后重读页码。

// 原函数：0x0047B5D0。
static std::int32_t WindowSkill_ButtonNextPage(std::int32_t index, std::int32_t state); // 后十行，不额外钳制负上限。

// 原函数：0x0047B680。
static std::int32_t WindowSkill_Scroll(std::int32_t unusedIndex, std::int32_t state); // 同源滚轮，恒返0。

// 原函数：0x0047B9B0。
static void WindowSkill_HandleTechniqueRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t technique,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 技术行，>=100按战斗FP着色。

// 原函数：0x0047C240。
static void WindowSkill_HandleRecipeRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t recipe,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 配方行与独立图号表。

// 原函数：0x0047BC00。
static std::int32_t WindowSkill_SelectSkillType(std::int32_t index, std::int32_t state); // 五类技能选择与完整失败音效。

// 原函数：0x0047C3A0。
static std::int32_t WindowSkill_SelectRecipe(std::int32_t index, std::int32_t state); // 转生一致才处理配方，恒返0。

// 原函数：0x004EF750。
static cg::yobi::YobiAction* WindowSkill_ReceiveAttributeTargets(const char* text); // ND自身/远端属性生产，分类1跳过人物行。

// 原函数：0x004874B0。
static cg::yobi::YobiAction* WindowSkill_OpenAttributeTargets(); // 同源六属性视图与46创建，不回滚计数。

// 原函数：0x004872E0。
static std::int32_t WindowSkill_SelectAttributeTarget(std::int32_t index, const wchar_t* unusedText); // 按属性目标编号发送技能。

// 原函数：0x00479820。
static std::int32_t WindowSkill_FindSpecialSkill(); // 技能索引对中查1009，按S16读取槽号。

// 原函数：0x00479920。
static std::int32_t WindowSkill_ReadCrystalElement(); // 同一槽7的属性0..3；未用0，未知ID为-1。

// 原函数：0x00479A00。
static std::int32_t WindowSkill_FindPetTechnique(std::int32_t petIndex); // 宠物技术索引对中查40330..40332。

// 原函数：0x00479A70。
static std::int32_t WindowSkill_CheckPetLevel(std::int32_t petIndex, std::int32_t crystalLevel); // 宠物等级闭区间。

// 原函数：0x00486FE0。
static std::int32_t WindowSkill_MatchesCrystalLevel(); // 当前技能技术value90与重读晶石等级等值。

// 原函数：0x0047A620。
static std::int32_t WindowSkill_Update(std::int32_t value);

// 原函数：0x0047A7A0。
static std::int32_t WindowSkill_Draw(std::int32_t value);

// 原函数：0x0047B0E0。
static void WindowSkill_Release();

// 控件载荷、窗口状态、组件表与原型。

static std::int32_t gSkillTargetCountCA0004 = 0; // 原全局：0x00CA0004。

static std::int32_t gSkillTargetPageC8A974 = 0; // 原全局：0x00C8A974。

static std::int32_t gSkillTargetPageCountCA0178 = 0; // 原全局：0x00CA0178。

static std::int32_t gSkillSelectedTargetCC4B1C = 0; // 原全局：0x00CC4B1C。

static std::int32_t gSkillWindowModeD672AC = 0; // 原全局：0x00D672AC。

static std::int32_t gSkillWindowIndexD2EA60 = 0; // 原全局：0x00D2EA60。

static std::int32_t gSkillWindowScrollD2D648 = 0; // 原全局：0x00D2D648。

static std::int32_t gSkillWindowHoverD2F744 = 0; // 原全局：0x00D2F744。

static std::int32_t gSkillRecipeLevelC8B1B4 = 0; // 原全局：0x00C8B1B4。

static std::int32_t gSkillWindowMaximumCA000C = 0; // 原全局：0x00CA000C。

// 原前15项为配对行号、末字节D3FACB参与479FA9门槛；现扩为30行，仅统计有效槽。
static std::array<std::int8_t, world::kSkillSlotCount> gBattleSkillRowsD3FABC {}; // 原全局：0x00D3FABC。

static SkillWindowSelectionCallback gSkillSelectionCallbackCC4230 = nullptr; // 原全局：0x00CC4230。

static WindowValueCallback gPetSkillSelectionCallbackCC4D14 = nullptr; // 原全局：0x00CC4D14。

static std::array<std::wstring, 10> gSkillTargetNamesCB0550 {}; // 原全局：0x00CB0550。 独立宽文本名单，非LY表。

static std::array<std::int32_t, 10> gSkillTargetKindsD2F75C {}; // 原全局：0x00D2F75C。

static std::array<const wchar_t*, 10> gSkillTargetNameViewsD45F98 {}; // 原全局：0x00D45F98。

static std::array<std::uint8_t, 10> gSkillTargetColorsCB1DE4 {}; // 原全局：0x00CB1DE4。

static std::uint8_t gSkillTargetPendingC8AF5F = 0; // 原全局：0x00C8AF5F。

// ND六行属性使用各自原槽，不能与nFV值记录或ysVc名称表混用。
static std::int32_t gSkillStatusCountC8AA94 = 0; // 原全局：0x00C8AA94。

static std::array<std::wstring, 6> gSkillStatusNamesCC4670 {}; // 原全局：0x00CC4670。

static std::array<std::int32_t, 6> gSkillStatusLevelsCC31C0 {}; // 原全局：0x00CC31C0。

static std::array<std::int32_t, 6> gSkillStatusInjuriesD67600 {}; // 原全局：0x00D67600。

static std::array<std::int32_t, 6> gSkillStatusHpCC318C {}; // 原全局：0x00CC318C。

static std::array<std::int32_t, 6> gSkillStatusMaxHpD66C1C {}; // 原全局：0x00D66C1C。

static std::array<std::int32_t, 6> gSkillStatusFpCC2348 {}; // 原全局：0x00CC2348。

static std::array<std::int32_t, 6> gSkillStatusMaxFpD2EA80 {}; // 原全局：0x00D2EA80。

static std::array<std::int32_t, 6> gSkillStatusKindsCA0A78 {}; // 原全局：0x00CA0A78。

static std::array<std::int32_t, 6> gSkillStatusTargetsCF3FD0 {}; // 原全局：0x00CF3FD0。

static std::array<WindowStatusChoiceRecord, 6> gSkillStatusViewsCC4BE8 {}; // 原全局：0x00CC4BE8。

static constexpr std::array<std::array<std::int32_t, 2>, 8> kPetCrystalItemRanges {{
    {0x24B9, 0x24C2}, {0x24C3, 0x24CC}, {0x24CD, 0x24D6}, {0x24D7, 0x24E0},
    {0x24E1, 0x24E5}, {0x24E6, 0x24EA}, {0x24EB, 0x24EF}, {0x24F0, 0x24F4},
}};

// 原54F2C8起的15对等级闭区间，晶石等级1对应首项。
static constexpr std::array<std::array<std::int32_t, 2>, 15> kPetCrystalLevels54F2C8 {{
    {1, 15}, {9, 22}, {16, 29}, {23, 36}, {30, 43},
    {37, 50}, {44, 57}, {51, 64}, {58, 71}, {65, 78},
    {72, 85}, {79, 92}, {86, 99}, {93, 106}, {100, 1000},
}};

static constexpr std::array<std::int32_t, 15> kPetCrystalFp54F340 { // 原常量参考：0x0054F340。
    1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
};

static constexpr std::array<std::int32_t, 18> kSkillListImages54F37C { // 原常量参考：0x0054F37C。
    0x3B59F, 0x3B5A0, 0x3B5A1, 0x3B5A5, 0x3B5A6, 0x3B5A7,
    0x3B5A5, 0x3B5A6, 0x3B5A7, 0x3B5A8, 0x3B5A9, 0x3B5AA,
    0x3B5A2, 0x3B5A3, 0x3B5A4, 0x3B59F, 0x3B5A0, 0x3B5A1,
};

static constexpr std::array<std::int32_t, 3> kPetSkillImages54F454 {0x3B5B7, 0x3B5B8, 0x3B5B9}; // 原常量参考：0x0054F454。

static constexpr std::array<std::int32_t, 18> kSkillTechniqueImages54F3C4 { // 原常量参考：0x0054F3C4。
    0x3B5AB, 0x3B5AC, 0x3B5AD, 0x3B5B1, 0x3B5B2, 0x3B5B3,
    0x3B5B1, 0x3B5B2, 0x3B5B3, 0x3B5B4, 0x3B5B5, 0x3B5B6,
    0x3B5AE, 0x3B5AF, 0x3B5B0, 0x3B5AB, 0x3B5AC, 0x3B5AD,
};

static constexpr std::array<std::int32_t, 18> kSkillRecipeImages54F40C { // 原常量参考：0x0054F40C。
    0x3B5B7, 0x3B5B8, 0x3B5B9, 0x3B5BD, 0x3B5BE, 0x3B5BF,
    0x3B5BD, 0x3B5BE, 0x3B5BF, 0x3B5C0, 0x3B5C1, 0x3B5C2,
    0x3B5BA, 0x3B5BB, 0x3B5BC, 0x3B5B7, 0x3B5B8, 0x3B5B9,
};

static WindowGraphicData gWindowGraphic561A40 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A40。

static WindowGraphicData gWindowGraphic561A58 {0x3BA24, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A58。

static WindowGraphicData gWindowGraphic561A70 {0x3BA27, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00561A70。

static WindowGraphicData gWindowGraphic561A88 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A88。

static WindowGraphicData gWindowGraphic561AA0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AA0。

static WindowGraphicData gWindowGraphic561AB8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AB8。

static WindowGraphicData gWindowGraphic561AD0 {0x3B571, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AD0。

static WindowGraphicData gWindowGraphic561AE8 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AE8。

static WindowGraphicData gWindowGraphic561B00 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B00。

static WindowGraphicData gWindowGraphic561B18 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B18。

static WindowGraphicData gWindowGraphic561B30 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B30。

static WindowTextPrototype gWindowText561B48 { // 原全局：0x00561B48。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"无职", // 原常量参考：0x005860A4。
};

static WindowTextPrototype gWindowText561B50 { // 原全局：0x00561B50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"10/10", // 原常量参考：0x0058609C。
};

static WindowTextPrototype gWindowText561B58 { // 原全局：0x00561B58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"经验值", // 原常量参考：0x005861E0。
};

static WindowTextPrototype gWindowText561B60 { // 原全局：0x00561B60。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText561B68 { // 原全局：0x00561B68。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};

static WindowTextPrototype gWindowText561B70 { // 原全局：0x00561B70。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

static std::uint32_t gWindowMouseFlagsD67780 = 0; // 原全局：0x00D67780。

static WindowElementPrototype gWindowElements561B78[30] { // 原全局：0x00561B78。
    {WindowElementType_Graphic, 222, 9, 11, 11, 1, {.graphic = &gWindowGraphic561A40}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowSkillElementId::ButtonClose)},
    {WindowElementType_Text, 82, 12, 0, 0, 1, {.text = &gWindowText561B48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element2)},
    {WindowElementType_Text, 87, 12, 0, 0, 1, {.text = &gWindowText561B68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element3)},
    {WindowElementType_Text, 66, 32, 0, 0, 1, {.text = &gWindowText561B50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element4)},
    {WindowElementType_Graphic, 16, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AD0}, &WindowSkill_ButtonBack, -1, static_cast<ElementId>(WindowSkillElementId::ButtonBack)},
    {WindowElementType_Text, 103, 31, 0, 0, 1, {.text = &gWindowText561B58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element6)},
    {WindowElementType_Graphic, 184, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AE8}, &WindowSkill_ButtonUse, -1, static_cast<ElementId>(WindowSkillElementId::ButtonUse)},
    {WindowElementType_Text, 95, 31, 0, 0, 1, {.text = &gWindowText561B70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element8)},
    {WindowElementType_Graphic, 16, 28, 49, 17, 1, {.graphic = &gWindowGraphic561B00}, &WindowSkill_ButtonPetSkillBack, -1, static_cast<ElementId>(WindowSkillElementId::ButtonPetSkillBack)},
    {WindowElementType_Text, 190, 13, 0, 0, 1, {.text = &gWindowText561B60}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element10)},
    {WindowElementType_Graphic, 223, 57, 0, 14, 1, {.graphic = &gWindowGraphic561A88}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element11)},
    {WindowElementType_MouseArea, 223, 57, 11, 140, 1, {.mouseFlags = &gWindowMouseFlagsD67780}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowSkillElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 223, 47, 11, 11, 1, {.graphic = &gWindowGraphic561AA0}, &WindowSkill_ButtonPreviousRow, -1, static_cast<ElementId>(WindowSkillElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 223, 196, 11, 11, 1, {.graphic = &gWindowGraphic561AB8}, &WindowSkill_ButtonNextRow, -1, static_cast<ElementId>(WindowSkillElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 195, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B18}, &WindowSkill_ButtonPreviousPage, -1, static_cast<ElementId>(WindowSkillElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 215, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B30}, &WindowSkill_ButtonNextPage, -1, static_cast<ElementId>(WindowSkillElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 0, 0, 241, 258, 1, {.unusedValue = 0}, &WindowSkill_Scroll, -1, static_cast<ElementId>(WindowSkillElementId::Scroll)},
    {WindowElementType_EventArea, 14, 46, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow)},
    {WindowElementType_EventArea, 14, 62, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_19)},
    {WindowElementType_EventArea, 14, 78, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_20)},
    {WindowElementType_EventArea, 14, 94, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_21)},
    {WindowElementType_EventArea, 14, 110, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_22)},
    {WindowElementType_EventArea, 14, 126, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_23)},
    {WindowElementType_EventArea, 14, 142, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_24)},
    {WindowElementType_EventArea, 14, 158, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_25)},
    {WindowElementType_EventArea, 14, 174, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_26)},
    {WindowElementType_EventArea, 14, 190, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow, -1, static_cast<ElementId>(WindowSkillElementId::HandleSkillRow_27)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561A58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element28)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561A70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillElementId::Element29)},
    {WindowElementType_EventArea, 243, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSkillElementId::HandleSideArea)},
};

// 原型：窗口10；原全局：0x0054E420。
WindowPrototype gWindowPrototypeSkill {0, 4, 25, 122, 241, 258, 0x80010101u, 30, gWindowElements561B78, &WindowSkill_Update, &WindowSkill_Draw, &WindowSkill_Release}; // 10: 54E420

// 窗口函数实现。

// 原函数：0x004797C0。
void WindowSkill_AdjustSpecialHoverCount(const std::int32_t first, const std::int32_t second)
{
    if (gSkillRebirthModeEnabled_5F9754 == 0) {
        return;
    }
    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    if (first == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[second] = static_cast<unsigned char>(hover[second] - 1);
    }
    if (second == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[first] = static_cast<unsigned char>(hover[first] - 1);
    }
}

// 原函数：0x00479820。
static std::int32_t WindowSkill_FindSpecialSkill()
{
    for (const auto& pair : world::gSkillPairsF63370) {
        if (pair.used != 0) {
            const auto index = std::bit_cast<std::int16_t>(pair.index);
            if (world::gSkillsF1BAFC[index].skillId == 1009) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x00479860。
std::int32_t WindowSkill_ReadCrystalLevel()
{
    const auto& item = world::gPlayerRuntimeSlots.items[7];
    if (item.used == 0) {
        return 0;
    }
    const std::int32_t id = item.itemId;
    for (std::int32_t index = 0; index < 8; ++index) {
        const auto& range = kPetCrystalItemRanges[index];
        if (id >= range[0] && id <= range[1]) {
            return id - range[0] + (index < 4 ? 1 : 11);
        }
    }
    return 0;
}

// 原函数：0x00479920。
static std::int32_t WindowSkill_ReadCrystalElement()
{
    const auto& item = world::gPlayerRuntimeSlots.items[7];
    if (item.used == 0) {
        return 0;
    }
    const std::int32_t id = item.itemId;
    for (std::int32_t index = 0; index < 8; ++index) {
        const auto& range = kPetCrystalItemRanges[index];
        if (id >= range[0] && id <= range[1]) {
            return index % 4;
        }
    }
    return -1;
}

// 原函数：0x004799D0。
std::int32_t WindowSkill_ReadBattlePetImage()
{
    const std::int32_t index = world::FindBattlePetIndex_43C4B0();
    return index < 0 ? 0 : world::gPetRuntimeRecords[index].imageNo6A0;
}

// 原函数：0x00479A00。
static std::int32_t WindowSkill_FindPetTechnique(const std::int32_t petIndex)
{
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    const std::int32_t count = pet.value6A4;
    for (std::int32_t index = 0; index < count; ++index) {
        const auto slot = std::bit_cast<std::int16_t>(pet.techniquePairs650[index].index);
        const auto& technique = pet.techniquesD8[slot];
        if (technique.name[0] != '\0') {
            const std::int32_t id = technique.techniqueId;
            if (id >= 40330 && id <= 40332) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x00479A70。
static std::int32_t WindowSkill_CheckPetLevel(const std::int32_t petIndex, const std::int32_t crystalLevel)
{
    const std::int32_t level = world::gPetRuntimeRecords[petIndex].value08;
    const auto& range = kPetCrystalLevels54F2C8[crystalLevel - 1];
    if (level < range[0]) {
        return 0;
    }
    return level <= range[1] ? 1 : 0;
}

// 原函数：0x00479AB0。
std::int32_t WindowSkill_CheckPetFp()
{
    const std::int32_t index = world::FindBattlePetIndex_43C4B0();
    if (index < 0) {
        return 0;
    }
    const std::int32_t level = WindowSkill_ReadCrystalLevel();
    if (level < 1 || level > 15) {
        return 0;
    }
    const std::int32_t required = kPetCrystalFp54F340[level - 1];
    return (world::gPetRuntimeRecords[index].values0C[2] < static_cast<std::int32_t>(required)) ? 0 : 1;
}

// 原函数：0x00479B00。
std::int32_t WindowSkill_CanUseSpecialSkill()
{
    if (WindowSkill_FindSpecialSkill() == 0) {
        return 0;
    }
    const std::int32_t level = WindowSkill_ReadCrystalLevel();
    if (level < 1) {
        return 0;
    }
    const std::int32_t index = world::FindBattlePetIndex_43C4B0();
    if (index < 0 || WindowSkill_FindPetTechnique(index) == 0) {
        return 0;
    }
    const std::int32_t attribute = WindowSkill_ReadCrystalElement();
    if (world::gPetRuntimeRecords[index].values70[21 + attribute] < 50) {
        return 0;
    }
    return WindowSkill_CheckPetLevel(index, level) != 0 ? 1 : 0;
}

// 原函数：0x00486FE0。
static std::int32_t WindowSkill_MatchesCrystalLevel()
{
    const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
    const std::int32_t skill = gUsingSkillIndexD2F32C;
    const std::int32_t level = gUsingSkillLevelCA0AC4;
    return world::gSkillsF1BAFC[skill].techniques3C[level].value90 == crystalLevel ? 1 : 0;
}

// 原函数：0x004872E0。
static std::int32_t WindowSkill_SelectAttributeTarget(const std::int32_t index, const wchar_t*)
{
    if (index == -1) {
        WindowRuntime_DecrementOperationCount();
        const auto count = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD - 1);
        const auto skillCount = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE - 1);
        gSkillOperationCount_D67AFD = count;
        gSkillStatusIconReferences_D67AFE = skillCount;
        if (skillCount == 0 && world::GetPlayerStatusIcon_51BEC0() == 13) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(0));
        }
        return 1;
    }
    if (index == -3) {
        const std::int32_t skill = gUsingSkillIndexD2F32C;
        static_cast<void>(cg::network::GetGameConnection().RequestSkillTargetAttributes(skill));
        return 0;
    }
    if (index == -2) {
        return 1;
    }
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowSkill_MatchesCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        gSkillSendModeC8A970 = 1;
        gSkillPetImageCB0A18 = WindowSkill_ReadBattlePetImage();
    }
    if (gSkillTargetKindCF4A10 <= 1) {
        gSkillSendModeC8A970 = 0;
    }
    char extra[256];
    extra[0] = '\0';
    gSkillTargetNameD2F748 = gSkillStatusNamesCC4670[index];
    // 原509080只有ret；后续三分支均发送此空串，但不清局部其余字节。
    std::int32_t offset = 0;
    if (gSkillRebirthModeEnabled_5F9754 != 0) {
        offset = 100;
    } else if (gSkillSendModeC8A970 != 0) {
        offset = 200;
    }
    const std::int32_t target = gSkillStatusTargetsCF3FD0[index];
    const std::int32_t level = gUsingSkillLevelCA0AC4;
    const std::int32_t skill = gUsingSkillIndexD2F32C;
    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
    static_cast<void>(cg::network::GetGameConnection().SendSkillUseRequest(skill + offset, level, target, cg::text::FromGbkWide(extra)));
    return 0;
}

// 原函数：0x004874B0。
static cg::yobi::YobiAction* WindowSkill_OpenAttributeTargets()
{
    const std::int32_t count = gSkillStatusCountC8AA94;
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    gSkillSendModeC8A970 = 0;
    for (std::int32_t index = 0; index < count; ++index) {
        auto& view = gSkillStatusViewsCC4BE8[index];
        const std::int32_t level = gSkillStatusLevelsCC31C0[index];
        view.name = gSkillStatusNamesCC4670[index].data();
        view.level = level;
        view.hp = gSkillStatusHpCC318C[index];
        view.maxHp = gSkillStatusMaxHpD66C1C[index];
        view.fp = gSkillStatusFpCC2348[index];
        view.maxFp = gSkillStatusMaxFpD2EA80[index];
        view.injury = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gSkillStatusInjuriesD67600[index]));
        view.color = static_cast<std::uint8_t>(colors[gSkillStatusKindsCA0A78[index]]);
    }
    WindowRuntime_IncrementOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillOperationCount_D67AFD = references;
    gSkillStatusIconReferences_D67AFE = skillReferences;
    constexpr std::array<std::int32_t, 5> windowIds {11, 12, 45, 46, 13};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowAttributeTarget_Open(gSkillStatusViewsCC4BE8.data(), gSkillStatusCountC8AA94, WindowSkill_SelectAttributeTarget);
}

// 原函数：0x004EF750。
static cg::yobi::YobiAction* WindowSkill_ReceiveAttributeTargets(const char* const text)
{
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    const std::int32_t selected = gSkillSelectedTargetCC4B1C;
    const std::int32_t kind = gSkillTargetKindsD2F75C[selected];
    gSkillStatusCountC8AA94 = 0;
    if (kind == 0) {
        std::int32_t count = 0;
        if (gSkillTargetKindCF4A10 != 1) {
            const wchar_t* const name = world::GetPlayerDisplayText(1);
            gSkillStatusLevelsCC31C0[0] = world::gPlayerRuntimeSlots.level;
            gSkillStatusNamesCC4670[0] = name;
            gSkillStatusInjuriesD67600[0] = world::gPlayerRuntimeSlots.valueF4;
            const std::int32_t hp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.hp);
            gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = hp;
            const std::int32_t maxHp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxHp);
            gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = maxHp;
            const std::int32_t fp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.fp);
            const std::int32_t output = gSkillStatusCountC8AA94;
            gSkillStatusFpCC2348[output] = fp;
            const std::int32_t maxFp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxFp);
            count = gSkillStatusCountC8AA94 + 1;
            gSkillStatusMaxFpD2EA80[output] = maxFp;
            gSkillStatusKindsCA0A78[output] = 0;
            gSkillStatusTargetsCF3FD0[output] = 0;
            gSkillStatusCountC8AA94 = count;
        }
        std::int32_t target = 1;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            auto& pet = world::gPetRuntimeRecords[slot];
            if (pet.used == 0 || gLoginWindowRuntime.valuesCC1A18[slot] == 3) {
                continue;
            }
            const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            gSkillStatusNamesCC4670[count] = name;
            gSkillStatusInjuriesD67600[count] = pet.values70[25];
            gSkillStatusLevelsCC31C0[count] = pet.value08;
            const std::int32_t hp = static_cast<std::int32_t>(pet.values0C[0]);
            gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = hp;
            const std::int32_t maxHp = static_cast<std::int32_t>(pet.values0C[1]);
            gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = maxHp;
            const std::int32_t fp = static_cast<std::int32_t>(pet.values0C[2]);
            const std::int32_t output = gSkillStatusCountC8AA94;
            gSkillStatusFpCC2348[output] = fp;
            const std::int32_t maxFp = static_cast<std::int32_t>(pet.values0C[3]);
            count = gSkillStatusCountC8AA94 + 1;
            gSkillStatusMaxFpD2EA80[output] = maxFp;
            gSkillStatusTargetsCF3FD0[output] = target++;
            gSkillStatusKindsCA0A78[output] = 4;
            gSkillStatusCountC8AA94 = count;
        }
        return WindowSkill_OpenAttributeTargets();
    }

    char name[128];
    std::int32_t field = 1;
    std::int32_t target = 0;
    for (std::int32_t row = 0; row < 6; ++row) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 127, name) == 1) {
            break;
        }
        if (gSkillTargetKindCF4A10 == 1 && row == 0) {
            field += 6;
            ++target;
            continue;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        gSkillStatusNamesCC4670[gSkillStatusCountC8AA94] = cg::text::FromGbkWide(name);
        std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusLevelsCC31C0[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusInjuriesD67600[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t output = gSkillStatusCountC8AA94;
        gSkillStatusFpCC2348[output] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusMaxFpD2EA80[output] = value;
        switch (gSkillTargetKindsD2F75C[gSkillSelectedTargetCC4B1C]) {
        case 1: gSkillStatusKindsCA0A78[output] = row == 0 ? 1 : 5; break;
        case 2: gSkillStatusKindsCA0A78[output] = row == 0 ? 2 : 6; break;
        case 3: gSkillStatusKindsCA0A78[output] = 3; break;
        }
        const std::int32_t count = gSkillStatusCountC8AA94 + 1;
        gSkillStatusTargetsCF3FD0[output] = target++;
        gSkillStatusCountC8AA94 = count;
    }
    return WindowSkill_OpenAttributeTargets();
}

// 原函数：0x004EF710。
static std::int32_t WindowSkill_ClassifyAttributeSkill(const std::int32_t skillId)
{
    switch (skillId) {
    case 221: return 0;
    case 222: return 1;
    case 223: return 2;
    case 224: return 3;
    default: return -1;
    }
}

// 原函数：0x0047B6C0。
static void WindowSkill_DrawSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t enabled,
    const std::int32_t textLayer, const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (world::gSkillPairsF63370[pairIndex].used == 0) {
        return;
    }
    const auto skillIndex = std::bit_cast<std::int16_t>(world::gSkillPairsF63370[pairIndex].index);
    const auto* const skill = &world::gSkillsF1BAFC[skillIndex];
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, L"", 0, 0));
    const std::int8_t nameColor = enabled != 0 ? 0 : 7;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, skill->name, 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:2}", skill->level);
    WindowNumber_Draw(x + 155, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t requiredSlots = std::bit_cast<std::int8_t>(skill->requiredSlots19[0]);
    cg::text::FormatTo(buffer, L"{:2}", requiredSlots);
    WindowNumber_Draw(x + 193, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t kind = skill->canRebirth;
    const std::int32_t imageNo = kSkillListImages54F37C[kind * 3 + visualState];
    static_cast<void>(cg::render::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x00479700。
static void WindowSkill_HandleDrag(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    if (message != 0) {
        return;
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        const auto y = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        static_cast<void>(WindowDrag_StartAtPosition(4, sourceIndex, x, y, WindowSkill_HandleDrag));
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t y =
        (platform::gLegacyMouseState.mouseY - 8);
    const std::int32_t x =
        (platform::gLegacyMouseState.mouseX - 103);
    WindowSkill_DrawSkillRow(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y),
        2, sourceIndex, 1, 5, 230, 0x80FFFFFFu);
}

// 原函数：0x004797A0。
static std::int32_t WindowSkill_StartDrag(const std::int32_t sourceIndex)
{
    return WindowDrag_Start(4, sourceIndex, WindowSkill_HandleDrag);
}

// 原函数：0x0047B820。
static std::int32_t WindowSkill_HandleSkillRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t pairIndex = gSkillWindowScrollD2D648 + index - 17;
    if (world::gSkillPairsF63370[pairIndex].used == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 4) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source != pairIndex && world::gSkillReorderPending_DA3B20 == 0) {
            const std::int32_t targetSkill =
                std::bit_cast<std::int16_t>(world::gSkillPairsF63370[pairIndex].index);
            const std::int32_t sourceSkill =
                std::bit_cast<std::int16_t>(world::gSkillPairsF63370[source].index);
            static_cast<void>(cg::network::GetGameConnection().SwapPlayerSkillOrderRequest(sourceSkill, targetSkill));
            world::gSkillReorderPending_DA3B20 = 1;
        }
        WindowDrag_Reset();
    }
    const bool clicked = (events & MouseFlag_LDownEvent) != 0;
    if (clicked) {
        if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
            WindowSkill_ShowTechniques(currentWindow, pairIndex);
            WindowRuntime_IncrementOperationCount();
            static_cast<void>(WindowDrag_ResetMouse());
            consumed = 1;
        }
    } else if ((events & MouseFlag_Hover) != 0 && (events & MouseFlag_RDownEvent) != 0 && gSkillOperationCount_D67AFD == 0) {
        static_cast<void>(WindowSkill_StartDrag(pairIndex));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((events & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"打开这个技能的技能视窗。" /* 原常量参考：0x005864C8。 */;
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
    }
    if (clicked) {
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const auto y = static_cast<std::int16_t>(element->y + window->y);
        const auto x = static_cast<std::int16_t>(element->x + window->x);
        WindowSkill_DrawSkillRow(x, y, visualState, pairIndex, 1, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x00479B80。
static void WindowSkill_ShowSkillList(WindowInstance* const window, const std::int32_t skillIndex)
{
    std::int32_t row = 0;
    while (row < world::kSkillSlotCount
        && std::bit_cast<std::int16_t>(world::gSkillPairsF63370[row].index) != skillIndex) {
        ++row;
    }
    if (row >= world::kSkillSlotCount) {
        row = 0;
    }
    std::int32_t position = row - 9;
    gSkillWindowModeD672AC = 0;
    gSkillWindowScrollD2D648 = position;
    if (position < 0) {
        position = 0;
        gSkillWindowScrollD2D648 = position;
    }
    if (skillIndex < 0) {
        position = 0;
        gSkillWindowScrollD2D648 = position;
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    auto* const handleVerticalScrollElement = window ? window->FindElementById(WindowSkillElementId::HandleVerticalScroll) : nullptr;
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, oldMaximum, position));
    }
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element2 = window ? window->FindElementById(WindowSkillElementId::Element2) : nullptr; element2) element2->visible = 1;
    if (auto* const element4 = window ? window->FindElementById(WindowSkillElementId::Element4) : nullptr; element4) element4->visible = 1;
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
        if (world::gSkillPairsF63370[index].used != 0) {
            ++count;
        }
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA24;
    for (std::int32_t index = 17; index <= 26; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_HandleSkillRow;
    }
}

// 原函数：0x0047B9B0。
static void WindowSkill_HandleTechniqueRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    std::int32_t technique, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const bool battle = technique >= 100;
    if (battle) {
        technique -= 100;
    }
    const auto* record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return;
    }
    std::int8_t nameColor = 0;
    if (record->techniques3C[technique].value8C == 0) {
        nameColor = 7;
    } else if (battle) {
        const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
        const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
        cg::yobi::YobiAction* const action = world::gBattleEntries59E980[position];
        if (cg::yobi::IsYobiActionFpLessThan(action, cost)) {
            nameColor = 6;
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, L"", 0, 0));
    const auto& name =
        world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].name;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, name, 0, 0));
    std::wstring buffer;
    const std::int32_t level =
        world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].value90;
    cg::text::FormatTo(buffer, L"{:2}", level);
    WindowNumber_Draw(x + 155, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::int32_t kind = record->canRebirth;
    if (kind != 1 && kind != 2) {
        const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
        cg::text::FormatTo(buffer, L"{:3}", cost);
        WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    } else {
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 189), static_cast<std::int16_t>(y + 3),
            static_cast<std::uint8_t>(textLayer), 12, nameColor, kWindow12Unknown554590, 0, 0));
    }
    const std::int32_t currentKind = world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth;
    const std::int32_t imageNo = kSkillTechniqueImages54F3C4[currentKind * 3 + visualState];
    static_cast<void>(cg::render::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x0047C240。
static void WindowSkill_HandleRecipeRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t recipe, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8[recipe].itemId < 0) {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, L"", 0, 0));
    const auto* const row = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8[recipe];
    const std::int8_t nameColor = row->value0C != 0 ? 0 : 7;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, row->name, 0, 0));
    const auto* const record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::int32_t cost = record->recipes8E8[recipe].value04 * record->fpReduce / 100;
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", cost);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t kind = world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth;
    const std::int32_t imageNo = kSkillRecipeImages54F40C[kind * 3 + visualState];
    static_cast<void>(cg::render::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x00479C80。
static void WindowSkill_ShowTechniques(WindowInstance* const window, const std::int32_t pairIndex)
{
    gSkillWindowModeD672AC = 1;
    gSkillWindowScrollD2D648 = 0;
    if (pairIndex >= 0) {
        gSkillWindowIndexD2EA60 = std::bit_cast<std::int16_t>(
            world::gSkillPairsF63370[pairIndex].index);
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    auto* const handleVerticalScrollElement = window ? window->FindElementById(WindowSkillElementId::HandleVerticalScroll) : nullptr;
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, oldMaximum, 0));
    }
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element3) element3->visible = 1;
    if (auto* const buttonBackElement = window ? window->FindElementById(WindowSkillElementId::ButtonBack) : nullptr; buttonBackElement) buttonBackElement->visible = 1;
    if (auto* const element6 = window ? window->FindElementById(WindowSkillElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element10 = window ? window->FindElementById(WindowSkillElementId::Element10) : nullptr; element10) element10->visible = 1;
    if (auto* const buttonUseElement = window ? window->FindElementById(WindowSkillElementId::ButtonUse) : nullptr; buttonUseElement) buttonUseElement->visible = 0;
    const auto* const techniques =
        world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C.data();
    std::int32_t count = 0;
    while (count < world::kTechniqueSlotCount && techniques[count].name[0] != '\0') {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const buttonPreviousPageElement = window ? window->FindElementById(WindowSkillElementId::ButtonPreviousPage) : nullptr; buttonPreviousPageElement) buttonPreviousPageElement->visible = 0;
    if (auto* const buttonNextPageElement = window ? window->FindElementById(WindowSkillElementId::ButtonNextPage) : nullptr; buttonNextPageElement) buttonNextPageElement->visible = 0;
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA26;
    for (std::int32_t index = 17; index <= 26; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_SelectSkillType;
    }
}

// 原函数：0x0047B0E0。
static void WindowSkill_Release()
{
    const std::int32_t mode = gSkillWindowModeD672AC;
    if (mode == 1 || mode == 2) {
        WindowRuntime_DecrementOperationCount();
        if (gSkillWindowModeD672AC == 2) {
            WindowRuntime_DecrementGatherOperationCount();
            --gSkillOperationCount_D67AFD;
        }
    }
}

// 原函数：0x00479EA0。
static void WindowSkill_ShowBattleSkills(WindowInstance* const window, const std::int32_t skillIndex)
{
    std::int32_t row = 0;
    while (row < world::kSkillSlotCount
        && std::bit_cast<std::int16_t>(world::gSkillPairsF63370[row].index) != skillIndex) {
        ++row;
    }
    if (row >= world::kSkillSlotCount) {
        row = 0;
    }
    gSkillWindowModeD672AC = 3;
    gSkillWindowScrollD2D648 = row - 9;
    if (gSkillWindowScrollD2D648 < 0) {
        gSkillWindowScrollD2D648 = 0;
    }
    if (skillIndex < 0) {
        gSkillWindowScrollD2D648 = 0;
    }
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element2 = window ? window->FindElementById(WindowSkillElementId::Element2) : nullptr; element2) element2->visible = 1;
    if (auto* const element4 = window ? window->FindElementById(WindowSkillElementId::Element4) : nullptr; element4) element4->visible = 1;
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA24;
    std::int32_t end = 0;
    for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
        if (world::gSkillPairsF63370[index].used != 0) {
            const auto skill = std::bit_cast<std::int16_t>(world::gSkillPairsF63370[index].index);
            const std::int32_t kind = world::gSkillsF1BAFC[skill].canRebirth;
            if (kind == 0 || kind == 5) {
                gBattleSkillRowsD3FABC[end++] = static_cast<std::int8_t>(index);
            }
        }
    }
    while (end < world::kSkillSlotCount) {
        gBattleSkillRowsD3FABC[end++] = -1;
    }
    // 原479FA9／479FBE读取末尾及-1别名；按用户确认，仅统计有效槽，-1表示未选中。
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
        if (world::ReadSkillPairUsed(gBattleSkillRowsD3FABC[index]) != 0) {
            ++count;
        }
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 17; index <= 26; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_HandleBattleSkillRow;
    }
}

// 原函数：0x0047A000。
static void WindowSkill_ShowBattleTechniques(
    WindowInstance* const window, const std::int32_t index, const std::uint8_t directSkill)
{
    gSkillWindowModeD672AC = 4;
    gSkillWindowScrollD2D648 = 0;
    if (directSkill != 0) {
        gSkillWindowIndexD2EA60 = index;
    } else if (index >= 0) {
        gSkillWindowIndexD2EA60 = std::bit_cast<std::int16_t>(world::gSkillPairsF63370[index].index);
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    auto* const handleVerticalScrollElement = window ? window->FindElementById(WindowSkillElementId::HandleVerticalScroll) : nullptr;
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, oldMaximum, 0));
    }
    for (std::int32_t element = 1; element <= 16; ++element) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(element)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element3) element3->visible = 1;
    if (auto* const buttonBackElement = window ? window->FindElementById(WindowSkillElementId::ButtonBack) : nullptr; buttonBackElement) buttonBackElement->visible = 1;
    if (auto* const element6 = window ? window->FindElementById(WindowSkillElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element10 = window ? window->FindElementById(WindowSkillElementId::Element10) : nullptr; element10) element10->visible = 1;
    std::int32_t selectedSkill = gSkillWindowIndexD2EA60;
    if (world::gSkillsF1BAFC[selectedSkill].canRebirth != 0) {
        if (auto* const buttonUseElement = window ? window->FindElementById(WindowSkillElementId::ButtonUse) : nullptr; buttonUseElement) buttonUseElement->visible = 1;
        selectedSkill = gSkillWindowIndexD2EA60;
    }
    const auto* const techniques =
        world::gSkillsF1BAFC[selectedSkill].techniques3C.data();
    std::int32_t count = 0;
    while (count < world::kTechniqueSlotCount && techniques[count].name[0] != '\0') {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t element = 10; element <= 16; ++element) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(element)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const buttonPreviousPageElement = window ? window->FindElementById(WindowSkillElementId::ButtonPreviousPage) : nullptr; buttonPreviousPageElement) buttonPreviousPageElement->visible = 0;
    if (auto* const buttonNextPageElement = window ? window->FindElementById(WindowSkillElementId::ButtonNextPage) : nullptr; buttonNextPageElement) buttonNextPageElement->visible = 0;
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA26;
    for (std::int32_t element = 17; element <= 26; ++element) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(element)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_SelectBattleTechnique;
    }
}

// 原函数：0x0047C570。
static std::int32_t WindowSkill_HandleBattleSkillRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t pairIndex = gBattleSkillRowsD3FABC[gSkillWindowScrollD2D648 + index - 17];
    if (pairIndex < 0 || world::gSkillPairsF63370[pairIndex].used == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    const bool clicked = (state & MouseFlag_LDownEvent) != 0;
    if (clicked) {
        const std::int32_t skillIndex =
            std::bit_cast<std::int16_t>(world::gSkillPairsF63370[pairIndex].index);
        const std::uint32_t flags = world::gBattleSkillFlags5ABC04;
        const std::uint32_t mask = 1u << (static_cast<std::uint32_t>(skillIndex) & 31u);
        if ((flags & mask) != 0) {
            WindowSkill_ShowBattleTechniques(currentWindow, pairIndex, 0);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        } else {
            if (world::gSkillsF1BAFC[skillIndex].skillId == 72
                && world::CountFreeInventorySlots_518C60() == 0) {
                world::AppendChatMessage_44B050(L"由于持有物品满了，无法使用。" /* 原常量参考：0x00586548。 */, 4, 3);
            }
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    const std::uint32_t flags = world::gBattleSkillFlags5ABC04;
    if ((state & MouseFlag_Hover) != 0) {
        const auto skillIndex = *reinterpret_cast<const std::uint8_t*>(
            &world::gSkillPairsF63370[pairIndex].index);
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        if ((flags & (1u << (skillIndex & 31u))) != 0) {
            gHoverTextD68AE8 = L"打开这个技能的技能视窗。" /* 原常量参考：0x005864C8。 */;
        }
    }
    if (clicked) {
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const auto skillIndex = *reinterpret_cast<const std::uint8_t*>(
            &world::gSkillPairsF63370[pairIndex].index);
        const std::int32_t enabled = (flags & (1u << (skillIndex & 31u))) != 0 ? 1 : 0;
        // 原47C6DA..47C6E6依次读取元素Y/X、窗口Y/X低W16；共享坐标仍为W32。
        std::uint16_t elementY;
        std::memcpy(&elementY, &element->y, sizeof(elementY));
        std::uint16_t elementX;
        std::memcpy(&elementX, &element->x, sizeof(elementX));
        std::uint16_t windowY;
        std::memcpy(&windowY, &window->y, sizeof(windowY));
        const auto y = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementY) + std::bit_cast<std::int16_t>(windowY));
        std::uint16_t windowX;
        std::memcpy(&windowX, &window->x, sizeof(windowX));
        const auto x = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementX) + std::bit_cast<std::int16_t>(windowX));
        WindowSkill_DrawSkillRow(x, y, visualState, pairIndex, enabled, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x0047C700。
static std::int32_t WindowSkill_SelectBattleTechnique(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t technique = gSkillWindowScrollD2D648 + index - 17;
    const auto* record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return 0;
    }
    const std::int32_t techniqueCost = record->techniques3C[technique].value7C;
    const std::int32_t fpReduce = record->fpReduce;
    const std::int32_t cost = techniqueCost * fpReduce / 100;
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    bool allowed = record->techniques3C[technique].value8C != 0;
    if (allowed) {
        const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
        allowed = cg::yobi::IsYobiActionFpGreaterEqual(world::gBattleEntries59E980[position], cost);
    }
    const bool clicked = (state & MouseFlag_LDownEvent) != 0;
    if (clicked) {
        if (allowed) {
            const SkillWindowSelectionCallback callback = gSkillSelectionCallbackCC4230;
            if (callback != nullptr) {
                callback(gSkillWindowIndexD2EA60, technique);
            }
            currentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & MouseFlag_Hover) != 0) {
        record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        visualState = 2;
        gSkillWindowHoverD2F744 = technique;
        const wchar_t* hint = L"由于能力不足，无法使用此技能。" /* 原常量参考：0x00586588。 */;
        if (record->techniques3C[technique].value8C != 0) {
            const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
            hint = cg::yobi::IsYobiActionFpLessThan(world::gBattleEntries59E980[position], cost)
                ? L"由于魔力不足，无法使用此技能。" /* 原常量参考：0x00586568。 */ : L"使用此一等级的技能。" /* 原常量参考：0x00586518。 */;
        }
        gHoverTextD68AE8 = hint;
    }
    if (clicked && world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].value8C != 0) {
        const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
        if (cg::yobi::IsYobiActionFpGreaterEqual(world::gBattleEntries59E980[position], cost)) {
            visualState = 1;
        }
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        // 原47C91F..47C92B依次读取元素Y/X、窗口Y/X低W16；共享坐标仍为W32。
        std::uint16_t elementY;
        std::memcpy(&elementY, &element->y, sizeof(elementY));
        std::uint16_t elementX;
        std::memcpy(&elementX, &element->x, sizeof(elementX));
        std::uint16_t windowY;
        std::memcpy(&windowY, &window->y, sizeof(windowY));
        const auto y = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementY) + std::bit_cast<std::int16_t>(windowY));
        std::uint16_t windowX;
        std::memcpy(&windowX, &window->x, sizeof(windowX));
        const auto x = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementX) + std::bit_cast<std::int16_t>(windowX));
        WindowSkill_HandleTechniqueRow(x, y, visualState, technique + 100, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x0047A490。
cg::yobi::YobiAction* WindowSkill_Open()
{
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    const bool hasWindow = window != nullptr;
    gSkillSelectionCallbackCC4230 = nullptr;
    gSkillWindowModeD672AC = 0;
    if (hasWindow) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A4C0。
cg::yobi::YobiAction* WindowSkill_OpenBattleSkills(const SkillWindowSelectionCallback callback)
{
    gSkillWindowModeD672AC = 3;
    gSkillSelectionCallbackCC4230 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A500。
cg::yobi::YobiAction* WindowSkill_OpenBattleTechniques(
    const std::int32_t skillIndex, const SkillWindowSelectionCallback callback)
{
    gSkillSelectionCallbackCC4230 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    gSkillWindowModeD672AC = 4;
    gSkillWindowIndexD2EA60 = skillIndex;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047CB00。
static void WindowSkill_DrawPetSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    const std::int32_t petIndex = world::FindBattlePetIndex_43C4B0();
    const std::int32_t pairPetIndex = world::FindBattlePetIndex_43C4B0();
    const auto technique = std::bit_cast<std::int16_t>(
        world::gPetRuntimeRecords[pairPetIndex].techniquePairs650[pairIndex].index);
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto* const record = &world::gPetRuntimeRecords[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, L"", 0, 0));
    const std::int32_t position = world::GetBattlePetPosition_405B40(
        world::gBattleCurrentPosition5ABCA4);
    cg::yobi::YobiAction* const action = world::gBattleEntries59E980[position];
    const std::int32_t cost = record->fp;
    std::int8_t nameColor = cg::yobi::IsCostGreaterThanYobiActionFp(cost, action) ? 6 : 0;
    const std::uint32_t flags = world::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) == 0) {
        nameColor = 7;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, record->name, 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->fp);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    static_cast<void>(cg::render::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), kPetSkillImages54F454[visualState], 0, &descriptor));
}

// 原函数：0x0047CE70。
static void WindowSkill_DrawRidingSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    const auto petIndex = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
    render::GraphicDescriptor16 descriptor {};
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto technique = std::bit_cast<std::int16_t>(
        world::gPetRuntimeRecords[petIndex].techniquePairs650[pairIndex].index);
    descriptor.color = color;
    const auto* const record = &world::gPetRuntimeRecords[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, L"", 0, 0));
    const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
    const std::int32_t cost = record->fp;
    cg::yobi::YobiAction* const action = world::gBattleEntries59E980[position];
    std::int8_t nameColor = cg::yobi::IsCostGreaterThanYobiActionFp(cost, action) ? 6 : 0;
    const std::uint32_t flags = world::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) == 0) {
        nameColor = 7;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, record->name, 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->fp);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    static_cast<void>(cg::render::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), kPetSkillImages54F454[visualState], 0, &descriptor));
}

// 原函数：0x0047CC80。
static std::int32_t WindowSkill_SelectPetSkill(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t pairIndex = index - 17;
    std::int32_t pairPet = world::FindBattlePetIndex_43C4B0();
    auto technique = std::bit_cast<std::int16_t>(
        world::gPetRuntimeRecords[pairPet].techniquePairs650[pairIndex].index);
    const std::int32_t namePet = world::FindBattlePetIndex_43C4B0();
    if (world::gPetRuntimeRecords[namePet].techniquesD8[technique].name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    const std::uint32_t flags = world::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) != 0) {
        pairPet = world::FindBattlePetIndex_43C4B0();
        technique = std::bit_cast<std::int16_t>(
            world::gPetRuntimeRecords[pairPet].techniquePairs650[pairIndex].index);
        const std::int32_t costPet = world::FindBattlePetIndex_43C4B0();
        const std::int32_t playerPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t cost = world::gPetRuntimeRecords[costPet].techniquesD8[technique].fp;
        const std::int32_t position = world::GetBattlePetPosition_405B40(playerPosition);
        if (cg::yobi::IsYobiActionFpGreaterEqual(world::gBattleEntries59E980[position], cost)
            && (reinterpret_cast<const std::uint8_t*>(&state)[0] & MouseFlag_LDownEvent) != 0) {
            if (gPetSkillSelectionCallbackCC4D14 != nullptr) {
                const std::int32_t pet = world::FindBattlePetIndex_43C4B0();
                const auto selected = std::bit_cast<std::int16_t>(
                    world::gPetRuntimeRecords[pet].techniquePairs650[pairIndex].index);
                gPetSkillSelectionCallbackCC4D14(selected);
            }
            currentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((reinterpret_cast<const std::uint8_t*>(&state)[0] & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    const std::uint8_t visualFlags = reinterpret_cast<const std::uint8_t*>(&state)[0];
    if ((visualFlags & MouseFlag_Hover) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        gHoverTextD68AE8 = L"使用此一等级的技能。" /* 原常量参考：0x00586518。 */;
    }
    if ((visualFlags & MouseFlag_LDownEvent) != 0) {
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        // 原坐标按element Y/X、window Y/X取低W16，避免宽指针别名。
        std::uint16_t elementYBits;
        std::uint16_t elementXBits;
        std::uint16_t windowYBits;
        std::uint16_t windowXBits;
        std::memcpy(&elementYBits, &element->y, sizeof(elementYBits));
        std::memcpy(&elementXBits, &element->x, sizeof(elementXBits));
        std::memcpy(&windowYBits, &window->y, sizeof(windowYBits));
        std::memcpy(&windowXBits, &window->x, sizeof(windowXBits));
        const auto y = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementYBits) + std::bit_cast<std::int16_t>(windowYBits));
        const auto x = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementXBits) + std::bit_cast<std::int16_t>(windowXBits));
        WindowSkill_DrawPetSkillRow(x, y, visualState, pairIndex, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x0047CFE0。
static std::int32_t WindowSkill_SelectRidingSkill(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const auto petIndex = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
    const std::int32_t pairIndex = index - 17;
    const auto technique = std::bit_cast<std::int16_t>(
        world::gPetRuntimeRecords[petIndex].techniquePairs650[pairIndex].index);
    const auto* const record = &world::gPetRuntimeRecords[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    if ((world::gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) != 0) {
        const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t cost = record->fp;
        if (cg::yobi::IsYobiActionFpGreaterEqual(world::gBattleEntries59E980[position], cost)
            && (reinterpret_cast<const std::uint8_t*>(&state)[0] & MouseFlag_LDownEvent) != 0) {
            const WindowValueCallback callback = gPetSkillSelectionCallbackCC4D14;
            if (callback != nullptr) {
                const auto pet = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
                const auto selected = std::bit_cast<std::int16_t>(
                    world::gPetRuntimeRecords[pet].techniquePairs650[pairIndex].index);
                callback(selected);
            }
            currentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((reinterpret_cast<const std::uint8_t*>(&state)[0] & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    const std::uint8_t visualFlags = reinterpret_cast<const std::uint8_t*>(&state)[0];
    if ((visualFlags & MouseFlag_Hover) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        gHoverTextD68AE8 = L"使用此一等级的技能。" /* 原常量参考：0x00586518。 */;
    }
    if ((visualFlags & MouseFlag_LDownEvent) != 0) {
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        // 原坐标按element Y/X、window Y/X取低W16，避免宽指针别名。
        std::uint16_t elementYBits;
        std::uint16_t elementXBits;
        std::uint16_t windowYBits;
        std::uint16_t windowXBits;
        std::memcpy(&elementYBits, &element->y, sizeof(elementYBits));
        std::memcpy(&elementXBits, &element->x, sizeof(elementXBits));
        std::memcpy(&windowYBits, &window->y, sizeof(windowYBits));
        std::memcpy(&windowXBits, &window->x, sizeof(windowXBits));
        const auto y = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementYBits) + std::bit_cast<std::int16_t>(windowYBits));
        const auto x = static_cast<std::int16_t>(
            std::bit_cast<std::int16_t>(elementXBits) + std::bit_cast<std::int16_t>(windowXBits));
        WindowSkill_DrawRidingSkillRow(x, y, visualState, pairIndex, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x0047A1A0。
static void WindowSkill_ShowPetSkills(WindowInstance* const window)
{
    gSkillWindowModeD672AC = 5;
    gSkillWindowScrollD2D648 = 0;
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element3) element3->visible = 1;
    gSkillWindowMaximumCA000C = 0;
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    const std::int32_t petIndex = world::FindBattlePetIndex_43C4B0();
    if (petIndex < 0) {
        return;
    }
    const auto* const pet = &world::gPetRuntimeRecords[petIndex];
    const wchar_t* name = pet->name6BD.data();
    if (name[0] == '\0') {
        name = pet->name6AC.data();
    }
    if (auto* const element32 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element32) element32->data.text->text = name;
    std::int32_t index = 0;
    std::int32_t count = std::min(pet->value6A4, world::kPetTechniqueSlotCount);
    while (index < count) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[index].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t position = world::GetBattlePetPosition_405B40(
                world::gBattleCurrentPosition5ABCA4);
            cg::yobi::YobiAction* const action = world::gBattleEntries59E980[position];
            const std::int32_t cost = record->fp;
            static_cast<void>(cg::yobi::IsCostGreaterThanYobiActionFp(cost, action));
        }
        count = std::min(pet->value6A4, world::kPetTechniqueSlotCount);
        ++index;
    }
    if (auto* const buttonPetSkillBackElement = window ? window->FindElementById(WindowSkillElementId::ButtonPetSkillBack) : nullptr; buttonPetSkillBackElement) buttonPetSkillBackElement->visible = 1;
    gSkillWindowMaximumCA000C = std::max(0, std::min(pet->value6A4, world::kPetTechniqueSlotCount) - 10);
    if (auto* const element8 = window ? window->FindElementById(WindowSkillElementId::Element8) : nullptr; element8) element8->visible = 1;
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA28;
    for (std::int32_t elementIndex = 17; elementIndex <= 26; ++elementIndex) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(elementIndex)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_SelectPetSkill;
    }
}

// 原函数：0x0047A320。
static void WindowSkill_ShowRidingSkills(WindowInstance* const window)
{
    gSkillWindowModeD672AC = 6;
    gSkillWindowScrollD2D648 = 0;
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element3) element3->visible = 1;
    gSkillWindowMaximumCA000C = 0;
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    const auto petIndex = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
    if (petIndex < 0) {
        return;
    }
    const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
    auto* elementsWindow = window;
    cg::yobi::YobiAction* const displayAction = world::gBattleEntries59E980[position];
    auto* const element32 = elementsWindow ? elementsWindow->FindElementById(WindowSkillElementId::Element3) : nullptr;
    if (element32) {
        auto* const textData = element32->data.text;
        const wchar_t* const name = cg::yobi::GetYobiName(displayAction);
        textData->text = name;
    }
    const auto* const pet = &world::gPetRuntimeRecords[petIndex];
    std::int32_t index = 0;
    std::int32_t count = std::min(pet->value6A4, world::kPetTechniqueSlotCount);
    while (index < count) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[index].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
            cg::yobi::YobiAction* const action = world::gBattleEntries59E980[currentPosition];
            const std::int32_t cost = record->fp;
            static_cast<void>(cg::yobi::IsCostGreaterThanYobiActionFp(cost, action));
        }
        count = std::min(pet->value6A4, world::kPetTechniqueSlotCount);
        ++index;
    }
    if (auto* const buttonPetSkillBackElement = window ? window->FindElementById(WindowSkillElementId::ButtonPetSkillBack) : nullptr; buttonPetSkillBackElement) buttonPetSkillBackElement->visible = 1;
    gSkillWindowMaximumCA000C = std::max(0, std::min(pet->value6A4, world::kPetTechniqueSlotCount) - 10);
    if (auto* const element8 = window ? window->FindElementById(WindowSkillElementId::Element8) : nullptr; element8) element8->visible = 1;
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA28;
    for (std::int32_t elementIndex = 17; elementIndex <= 26; ++elementIndex) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(elementIndex)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_SelectRidingSkill;
    }
}

// 原函数：0x0047A540。
cg::yobi::YobiAction* WindowSkill_OpenPetSkills(const WindowValueCallback callback)
{
    gSkillWindowModeD672AC = 5;
    gPetSkillSelectionCallbackCC4D14 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A580。
cg::yobi::YobiAction* WindowSkill_OpenRidingSkills(const WindowValueCallback callback)
{
    gSkillWindowModeD672AC = 6;
    gPetSkillSelectionCallbackCC4D14 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047B110。
static std::int32_t WindowSkill_ButtonBack(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (!elementPtr) { return 0; }
    const WindowElement* const element = elementPtr;
    const bool hovered = (state & MouseFlag_Hover) != 0;
    auto* const graphic = element->data.graphic;
    std::int32_t consumed = 0;
    if (hovered) {
        const std::int32_t mode = gSkillWindowModeD672AC;
        if (mode == 1 || mode == 4) {
            gHoverTextD68AE8 = L"切换回技能一览视窗。" /* 原常量参考：0x00586488。 */;
        } else if (mode == 2) {
            gHoverTextD68AE8 = L"切换回技能等级视窗。" /* 原常量参考：0x00586470。 */;
        }
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & MouseFlag_LDownEvent) != 0) {
        switch (gSkillWindowModeD672AC) {
        case 1:
            WindowSkill_ShowSkillList(window, gSkillWindowIndexD2EA60);
            WindowRuntime_DecrementOperationCount();
            break;
        case 2:
            WindowSkill_ShowTechniques(window, -1);
            WindowRuntime_DecrementGatherOperationCount();
            --gSkillOperationCount_D67AFD;
            break;
        case 4:
            WindowSkill_ShowBattleSkills(window, gSkillWindowIndexD2EA60);
            break;
        default:
            break;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B571;
    if (hovered) {
        graphic->imageNo = 0x3B573;
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & MouseFlag_LHoldPhysical) != 0) {
        graphic->imageNo = 0x3B572;
    }
    return consumed;
}

// 原函数：0x0047A5C0。
std::int32_t WindowSkill_Back()
{
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window == nullptr) {
        return 0;
    }
    switch (gSkillWindowModeD672AC) {
    case 0:
    case 3:
    case 5:
    case 6:
        window->state |= 0x8000u;
        return 1;
    case 4:
        WindowSkill_ShowBattleSkills(window, gSkillWindowIndexD2EA60);
        return 0;
    default:
        return 0;
    }
}

// TUC01：初始元素→单字节动作标志→图形；持有 hover，Y/X发送后按原序发布转生/占用。
// 原函数：0x0047B210。
static std::int32_t WindowSkill_ButtonUse(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const initialWindow = currentWindow;
    auto* elementsWindow = initialWindow;
    const std::uint8_t actionFlags = world::ReadPlayerActionFlagsByte();
    auto* const graphicElement = elementsWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((actionFlags & 0x80u) != 0
            || world::IsObservationSceneTransitionActive_440B50() != 0
            || world::gPlayerRuntimeSlots.value08 == 0
            || world::gPlayerRuntimeSlots.timerRemaining0C <= 0
            || world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth == 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
            return 0;
        }
        const std::uint8_t rebirth = gSkillRebirthModeEnabled_5F9754;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            gHoverTextD68AE8 = rebirth != 0
                ? kSkillDisableRebirth5864B4 : kSkillEnableRebirth5864A0;
        }
        std::int32_t consumed = 0;
        if (gGatherOperationCount_D67AFC != 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        } else if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(cg::network::GetGameConnection().SendWorldActionRequest(x, y, rebirth != 0 ? 62 : 61));
            auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
            const std::uint8_t previous = occupancy[7];
            gSkillRebirthModeEnabled_5F9754 = rebirth != 0 ? 0 : 1;
            occupancy[7] = static_cast<std::uint8_t>(previous + (rebirth != 0 ? -1 : 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B57A;
        if (hover) {
            graphic->imageNo = 0x3B57C;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B57B;
        }
        if (gGatherOperationCount_D67AFC != 0) {
            graphic->imageNo = 0x3B57B;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0047C950。
static std::int32_t WindowSkill_ButtonPetSkillBack(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const std::int32_t mode = gSkillWindowModeD672AC;
    std::int32_t petIndex;
    std::int32_t position; // 原仅mode5/6写入；其他模式未初始化路径不模拟，见SPEC。
    if (mode == 5) {
        petIndex = world::FindBattlePetIndex_43C4B0();
        position = world::GetBattlePetPosition_405B40(world::gBattleCurrentPosition5ABCA4);
    } else if (mode == 6) {
        position = world::gBattleCurrentPosition5ABCA4;
        petIndex = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
    } else {
        petIndex = state;
    }
    const auto* const pet = &world::gPetRuntimeRecords[petIndex];
    std::int32_t count = 0;
    std::int32_t unavailable = 0;
    std::int32_t row = 0;
    std::int32_t capacity = pet->value6A4;
    while (row < capacity) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[row].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t cost = record->fp;
            cg::yobi::YobiAction* const action = world::gBattleEntries59E980[position];
            if (cg::yobi::IsCostGreaterThanYobiActionFp(cost, action)
                || (world::gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(row) & 31u))) == 0) {
                ++unavailable;
            }
            ++count;
        }
        capacity = pet->value6A4;
        ++row;
    }
    if (count > unavailable) {
        if (auto* const buttonPetSkillBackElement = currentWindow->FindElementById(WindowSkillElementId::ButtonPetSkillBack); buttonPetSkillBackElement) buttonPetSkillBackElement->visible = 0;
        return 0;
    }
    const bool hovered = (state & MouseFlag_Hover) != 0;
    if (hovered) {
        gHoverTextD68AE8 = L"略过此回合。" /* 原常量参考：0x005865A8。 */;
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const WindowValueCallback callback = gPetSkillSelectionCallbackCC4D14;
        if (callback != nullptr) {
            callback(-1);
        }
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3BE7E;
    if (hovered) {
        if (graphic) graphic->imageNo = 0x3BE80;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3BE7F;
    }
    return consumed;
}

// 原函数：0x0047A620。
static std::int32_t WindowSkill_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        const std::int32_t mode = gSkillWindowModeD672AC;
        gSkillWindowScrollD2D648 = 0;
        switch (mode) {
        case 0:
            WindowSkill_ShowSkillList(currentWindow, -1);
            break;
        case 3:
            WindowSkill_ShowBattleSkills(currentWindow, -1);
            break;
        case 4: {
            const std::int32_t skillIndex = gSkillWindowIndexD2EA60;
            WindowSkill_ShowBattleTechniques(currentWindow, skillIndex, 1);
            break;
        }
        case 5:
            WindowSkill_ShowPetSkills(currentWindow);
            break;
        case 6:
            WindowSkill_ShowRidingSkills(currentWindow);
            break;
        default:
            break;
        }
    }
    const std::int32_t mode = gSkillWindowModeD672AC;
    gSkillWindowHoverD2F744 = -1;
    if ((mode == 1 || mode == 2 || mode == 4)
        && world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].name[0] == '\0') {
        if (mode == 2) {
            WindowRuntime_DecrementGatherOperationCount();
            --gSkillOperationCount_D67AFD;
        }
        if (mode == 1 || mode == 2) {
            WindowRuntime_DecrementOperationCount();
        }
        WindowSkill_ShowSkillList(currentWindow, 0);
    }
    if (auto* const buttonUseElement = currentWindow->FindElementById(WindowSkillElementId::ButtonUse); buttonUseElement) buttonUseElement->visible = 0;
    if (gSkillWindowModeD672AC == 1) {
        if (auto* const buttonUseElement2 = currentWindow->FindElementById(WindowSkillElementId::ButtonUse); buttonUseElement2) buttonUseElement2->visible = 1;
    }
    if (auto* const buttonPetSkillBackElement = currentWindow->FindElementById(WindowSkillElementId::ButtonPetSkillBack); buttonPetSkillBackElement) buttonPetSkillBackElement->visible = 0;
    if (gSkillWindowModeD672AC == 5) {
        if (auto* const buttonPetSkillBackElement2 = currentWindow->FindElementById(WindowSkillElementId::ButtonPetSkillBack); buttonPetSkillBackElement2) buttonPetSkillBackElement2->visible = 1;
    }
    if (gSkillWindowModeD672AC == 6) {
        if (auto* const buttonPetSkillBackElement2 = currentWindow->FindElementById(WindowSkillElementId::ButtonPetSkillBack); buttonPetSkillBackElement2) buttonPetSkillBackElement2->visible = 1;
    }
    return 1;
}

// 原函数：0x0047A7A0。
static std::int32_t WindowSkill_Draw(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowSkillElementId::HandleVerticalScroll);
    if (auto* const element11 = elementsWindow->FindElementById(WindowSkillElementId::Element11); element11) element11->visible = 0;
    std::int32_t mode = gSkillWindowModeD672AC;
    if (mode >= 0 && mode <= 5 && gSkillWindowMaximumCA000C > 0) {
        if (auto* const element112 = currentWindow->FindElementById(WindowSkillElementId::Element11); element112) element112->visible = 1;
        mode = gSkillWindowModeD672AC;
    }
    std::int32_t position;
    if (handleVerticalScrollElement && (handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        position = WindowScroll_ReadPosition(handleVerticalScrollElement, maximum);
        mode = gSkillWindowModeD672AC;
        gSkillWindowScrollD2D648 = position;
    } else {
        position = gSkillWindowScrollD2D648;
    }
    std::wstring buffer;
    const auto drawDescription = [&](std::wstring_view text, const std::int32_t line, const std::int32_t columns) {
        if (input::ExtractTextLine(buffer, text, line, columns) != 0) {
            const WindowInstance* const window = currentWindow;
            const auto y = static_cast<std::int16_t>(window->y + 208 + 12 * line);
            const auto x = static_cast<std::int16_t>(window->x + 23);
            static_cast<void>(render::DrawText_456780(x, y, 3, 12, 0, buffer, 0, 0));
        }
    };
    switch (mode) {
    case 0:
    case 3: {
        if (mode == 0) {
            std::int32_t count = 0;
            for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
                if (world::gSkillPairsF63370[index].used != 0) {
                    ++count;
                }
            }
            gSkillWindowMaximumCA000C = count - 10;
            if (gSkillWindowMaximumCA000C < 0) {
                gSkillWindowMaximumCA000C = 0;
            }
            if (position > gSkillWindowMaximumCA000C) {
                gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
            }
        }
        if (auto* const element2 = currentWindow->FindElementById(WindowSkillElementId::Element2); element2) element2->data.text->text = world::GetPlayerJobName();
        std::int32_t usedSlots = 0;
        for (std::int32_t index = 0; index < world::kSkillSlotCount; ++index) {
            if (world::gSkillPairsF63370[index].used != 0) {
                const auto skill = std::bit_cast<std::int16_t>(world::gSkillPairsF63370[index].index);
                const auto* const record = &world::gSkillsF1BAFC[skill];
                if (mode == 3) {
                    const std::int32_t kind = record->canRebirth;
                    if (kind != 0 && kind != 5) {
                        continue;
                    }
                }
                usedSlots += std::bit_cast<std::int8_t>(record->requiredSlots19[0]);
            }
        }
        const std::int32_t totalSlots = world::gPlayerRuntimeSlots.value16C;
        cg::text::FormatTo(buffer, L"{:2}/{:2}", usedSlots, totalSlots);
        if (auto* const element4 = currentWindow->FindElementById(WindowSkillElementId::Element4); element4) element4->data.text->text = buffer;
        break;
    }
    case 1:
    case 4: {
        const auto* record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (mode == 1) {
            std::int32_t count = 0;
            while (count < world::kTechniqueSlotCount && record->techniques3C[count].name[0] != '\0') {
                ++count;
            }
            gSkillWindowMaximumCA000C = count - 10;
            if (gSkillWindowMaximumCA000C < 0) {
                gSkillWindowMaximumCA000C = 0;
            }
            if (position > gSkillWindowMaximumCA000C) {
                gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
            }
        }
        if (auto* const element3 = currentWindow->FindElementById(WindowSkillElementId::Element3); element3) element3->data.text->text = record->name;
        record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (record->level < record->maxLevel) {
            const std::int32_t next = record->nextExperience;
            const std::int32_t experience = record->experience;
            cg::text::FormatTo(buffer, L"{:4}/{:4}", experience, next);
        } else {
            cg::text::FormatTo(buffer, L"{:4}/ -- ", record->experience);
        }
        if (auto* const element6 = currentWindow->FindElementById(WindowSkillElementId::Element6); element6) element6->data.text->text = buffer;
        record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        const std::int32_t maximum = record->maxLevel;
        const std::int32_t level = record->level;
        cg::text::FormatTo(buffer, L"{:2}/{:2}", level, maximum);
        if (auto* const element10 = currentWindow->FindElementById(WindowSkillElementId::Element10); element10) element10->data.text->text = buffer;
        if (gSkillWindowHoverD2F744 >= 0) {
            for (std::int32_t line = 0; line < 4; ++line) {
                const std::int32_t hover = gSkillWindowHoverD2F744;
                const std::int32_t skill = gSkillWindowIndexD2EA60;
                drawDescription(world::gSkillsF1BAFC[skill].techniques3C[hover].description.data(), line, 26);
            }
        }
        break;
    }
    case 2: {
        const auto* const record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        std::int32_t count = 0;
        while (record->recipes8E8[count].itemId >= 0) {
            ++count;
        }
        gSkillWindowMaximumCA000C = count - 10;
        if (gSkillWindowMaximumCA000C < 0) {
            gSkillWindowMaximumCA000C = 0;
        }
        if (position > gSkillWindowMaximumCA000C) {
            gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
        }
        const std::int32_t levelIndex = gSkillRecipeLevelC8B1B4;
        if (auto* const element3 = currentWindow->FindElementById(WindowSkillElementId::Element3); element3) element3->data.text->text = record->techniques3C[levelIndex].name;
        const std::int32_t level = world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].level;
        cg::text::FormatTo(buffer, L"{:5}", level);
        if (auto* const element10 = currentWindow->FindElementById(WindowSkillElementId::Element10); element10) element10->data.text->text = buffer;
        for (std::int32_t line = 0; line < 2; ++line) {
            const std::int32_t skill = gSkillWindowIndexD2EA60;
            const std::int32_t hover = gSkillWindowHoverD2F744;
            // IWT01：未悬停(-1)不读前一条记录区域，有效索引直接读取配方说明。
            const auto& recipes = world::gSkillsF1BAFC[skill].recipes8E8;
            if (hover >= 0 && hover < static_cast<std::int32_t>(recipes.size()))
                drawDescription(recipes[hover].description, line, 28);
        }
        break;
    }
    case 5:
    case 6: {
        std::int32_t maximumPosition = world::gBattleCurrentPosition5ABCA4;
        if (mode == 5) {
            maximumPosition = world::GetBattlePetPosition_405B40(maximumPosition);
        }
        const std::int32_t maximum =
            cg::yobi::ReadYobiActionMaxFpOnce(world::gBattleEntries59E980[maximumPosition]);
        std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
        if (mode == 5) {
            currentPosition = world::GetBattlePetPosition_405B40(currentPosition);
        }
        const std::int32_t current =
            cg::yobi::ReadYobiActionFpOnce(world::gBattleEntries59E980[currentPosition]);
        auto* const element8 = currentWindow->FindElementById(WindowSkillElementId::Element8);
        if (element8) {
            cg::text::FormatTo(element8->data.text->text, L"{:4}/{:4}", current, maximum);
        }
        if (gSkillWindowHoverD2F744 >= 0) {
            for (std::int32_t line = 0; line < 4; ++line) {
                std::int32_t pet;
                std::int32_t hover;
                if (mode == 5) {
                    pet = world::FindBattlePetIndex_43C4B0();
                    hover = gSkillWindowHoverD2F744;
                } else {
                    hover = gSkillWindowHoverD2F744;
                    pet = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
                }
                drawDescription(world::gPetRuntimeRecords[pet].techniquesD8[hover].description, line, 26);
            }
        }
        break;
    }
    default:
        break;
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0047B3D0。
static std::int32_t WindowSkill_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        if (gSkillWindowScrollD2D648 > 0) {
            --gSkillWindowScrollD2D648;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gSkillWindowScrollD2D648;
        WindowInstance* const window = currentWindow;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        auto* const handleVerticalScrollElement = window->FindElementById(WindowSkillElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
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

// 原函数：0x0047B470。
static std::int32_t WindowSkill_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        const std::int32_t position = gSkillWindowScrollD2D648;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        if (position < maximum) {
            gSkillWindowScrollD2D648 = position + 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = gSkillWindowScrollD2D648;
        WindowInstance* const window = currentWindow;
        const std::int32_t currentMaximum = gSkillWindowMaximumCA000C;
        auto* const handleVerticalScrollElement = window->FindElementById(WindowSkillElementId::HandleVerticalScroll);
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

// 原函数：0x0047B520。
static std::int32_t WindowSkill_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gSkillWindowScrollD2D648 > 0 ? 51 : 55, 320));
        gSkillWindowScrollD2D648 -= 10;
        if (gSkillWindowScrollD2D648 < 0) {
            gSkillWindowScrollD2D648 = 0;
        }
        const std::int32_t position = gSkillWindowScrollD2D648;
        WindowInstance* const window = currentWindow;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        auto* const handleVerticalScrollElement = window->FindElementById(WindowSkillElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
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

// 原函数：0x0047B5D0。
static std::int32_t WindowSkill_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t initialMaximum = gSkillWindowMaximumCA000C;
        const bool canAdvance = initialMaximum > 0 && gSkillWindowScrollD2D648 < initialMaximum;
        static_cast<void>(audio::PlaySe_52C450(canAdvance ? 51 : 55, 320));
        std::int32_t position = gSkillWindowScrollD2D648;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        position += 10;
        gSkillWindowScrollD2D648 = position;
        if (position > maximum) {
            position = maximum;
            gSkillWindowScrollD2D648 = position;
        }
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowSkillElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position));
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

// 原函数：0x0047B680。
static std::int32_t WindowSkill_Scroll(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        const std::int32_t position = gSkillWindowScrollD2D648;
        auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowSkillElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            gSkillWindowScrollD2D648 =
                WindowScroll_ApplyWheel(handleVerticalScrollElement, position, maximum, wheel);
        }
    }
    return 0;
}

// 原函数：0x00479DA0。
static void WindowSkill_ShowRecipes(WindowInstance* const window, const std::int32_t level)
{
    if (level >= 0) {
        gSkillRecipeLevelC8B1B4 = level;
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    gSkillWindowModeD672AC = 2;
    gSkillWindowScrollD2D648 = 0;
    auto* const handleVerticalScrollElement = window ? window->FindElementById(WindowSkillElementId::HandleVerticalScroll) : nullptr;
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, oldMaximum, 0));
    }
    for (std::int32_t index = 1; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillElementId::Element3) : nullptr; element3) element3->visible = 1;
    if (auto* const buttonBackElement = window ? window->FindElementById(WindowSkillElementId::ButtonBack) : nullptr; buttonBackElement) buttonBackElement->visible = 1;
    if (auto* const element10 = window ? window->FindElementById(WindowSkillElementId::Element10) : nullptr; element10) element10->visible = 1;
    const auto* const recipes = world::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8.data();
    std::int32_t count = 0;
    while (recipes[count].itemId >= 0) {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    for (std::int32_t index = 10; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const element28 = window ? window->FindElementById(WindowSkillElementId::Element28) : nullptr; element28) element28->data.graphic->imageNo = 0x3BA25;
    for (std::int32_t index = 17; index <= 26; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->mouseCallback = WindowSkill_SelectRecipe;
    }
}

// 原函数：0x0047C3A0。
static std::int32_t WindowSkill_SelectRecipe(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t recipe = gSkillWindowScrollD2D648 + index - 17;
    const auto* const record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::uint8_t rebirthMode = gSkillRebirthModeEnabled_5F9754;
    if ((record->rebirthState4C48 != 0) != (rebirthMode != 0)
        || record->recipes8E8[recipe].itemId < 0) {
        return 0;
    }
    std::int8_t visualState = 0;
    const bool clicked = (state & MouseFlag_LDownEvent) != 0;
    if (clicked) {
        bool allowed = (world::ReadPlayerActionFlags() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0;
        if (allowed) {
            const std::int32_t skill = gSkillWindowIndexD2EA60;
            allowed = world::gSkillsF1BAFC[skill].recipes8E8[recipe].value0C != 0;
            if (allowed) {
                bool create;
                if (gWindowStates[WindowId_Craft].window != nullptr) {
                    create = gCraftSkillIndexCC2368 != skill
                        || gCraftRequestValueD30400 != gSkillRecipeLevelC8B1B4
                        || gCraftRecipeIndexD2D638 != recipe;
                } else {
                    create = gWindowActionCA0174 == nullptr;
                }
                if (create) {
                    gWindowActionCA0174 = WindowCraft_OpenRecipe(skill, recipe, gSkillRecipeLevelC8B1B4);
                }
            }
        }
        static_cast<void>(audio::PlaySe_52C450(allowed ? 51 : 55, 320));
    }
    if ((state & MouseFlag_Hover) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = recipe;
        gHoverTextD68AE8 = L"制作此配方内容的物品。" /* 原常量参考：0x00586530。 */;
    }
    if (clicked) {
        visualState = 1;
    }
    if (gWindowStates[WindowId_Craft].window != nullptr
        && gCraftSkillIndexCC2368 == gSkillWindowIndexD2EA60
        && gCraftRequestValueD30400 == gSkillRecipeLevelC8B1B4
        && gCraftRecipeIndexD2D638 == recipe) {
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const auto y = static_cast<std::int16_t>(element->y + window->y);
        const auto x = static_cast<std::int16_t>(element->x + window->x);
        WindowSkill_HandleRecipeRow(x, y, visualState, recipe, 3, 223, 0xFFFFFFFFu);
    }
    return 0;
}

// 原函数：0x0047BC00。
static std::int32_t WindowSkill_SelectSkillType(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t technique = gSkillWindowScrollD2D648 + index - 17;
    const auto* record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = record->techniques3C[technique].value8C != 0 ? L"使用此一等级的技能。" /* 原常量参考：0x00586518。 */ : L"无法使用此一等级的技能。" /* 原常量参考：0x005864FC。 */;
        if (record->canRebirth >= 3 && record->canRebirth <= 4) {
            const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
            if ((world::gPlayerRuntimeSlots.fp < cost)) {
                gHoverTextD68AE8 = kWindow12NoFp5864F0;
            }
        }
        visualState = 2;
        gSkillWindowHoverD2F744 = technique;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        record = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (record->techniques3C[technique].value8C == 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            // 可用技术所有分支最终还播放SE51，包括动作门禁与部分FP失败。
            const auto activate = [&] {
                const auto actionBlocked = [] {
                    return (world::ReadPlayerActionFlags() & 0x80u) != 0
                        || world::IsObservationSceneTransitionActive_440B50() != 0;
                };
                const auto insufficientFp = [&] {
                    const auto* const current = &world::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
                    const std::int32_t cost =
                        current->techniques3C[technique].value7C * current->fpReduce / 100;
                    return (world::gPlayerRuntimeSlots.fp < cost);
                };
                const auto fail = [] {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                };
                switch (record->canRebirth) {
                case 4:
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (insufficientFp()) {
                        world::AppendChatMessage_44B050(kWindow12NoFp5864F0, 4, 3);
                        fail();
                        return;
                    }
                    if (world::CountFreeInventorySlots_518C60() <= 0) {
                        world::AppendChatMessage_44B050(L"物品栏已满" /* 原常量参考：0x005864E4。 */, 4, 3);
                        fail();
                        return;
                    }
                    if (gWindowStates[WindowId_Gather].window != nullptr) {
                        const std::int32_t current = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470);
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (current != skill || gUsingSkillLevelCA0AC4 != technique) {
                            gWindowActionD2E928 = WindowGather_Open(skill, technique);
                        }
                    } else if (gWindowActionD2E928 == nullptr) {
                        gWindowActionD2E928 = WindowGather_Open(gSkillWindowIndexD2EA60, technique);
                    }
                    return;
                case 3: {
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (insufficientFp()) {
                        return;
                    }
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
                    gUsingSkillIndexD2F32C = skill;
                    gUsingSkillLevelCA0AC4 = technique;
                    static_cast<void>(cg::network::GetGameConnection().RequestSkillTargetAttributes(skill));
                    return;
                }
                case 2:
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (gWindowStates[WindowId_Craft].window != nullptr) {
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (gCraftSkillIndexCC2368 != skill || gCraftRequestValueD30400 != technique) {
                            gWindowActionCA0174 = WindowCraft_OpenItemWork(skill, technique);
                        }
                    } else if (gWindowActionCA0174 == nullptr) {
                        gWindowActionCA0174 = WindowCraft_OpenItemWork(gSkillWindowIndexD2EA60, technique);
                    }
                    return;
                case 1: {
                    const std::int32_t skillId = record->skillId;
                    if (skillId == 255 || skillId == 606 || skillId == 2025 || skillId == 2026) {
                        if (actionBlocked()) {
                            fail();
                            return;
                        }
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (skillId == 255 || skillId == 606) {
                            static_cast<void>(WindowCraft_OpenDecoration(skill, technique, skillId == 606 ? 1 : 0));
                        } else if (skillId == 2025) {
                            static_cast<void>(WindowCraft_OpenRefining(skill, technique));
                        } else {
                            static_cast<void>(WindowCraft_OpenMaterials(skill, technique));
                        }
                        return;
                    }
                    WindowRuntime_IncrementGatherOperationCount();
                    const std::uint8_t previous = gSkillOperationCount_D67AFD;
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    gSkillOperationCount_D67AFD = static_cast<std::uint8_t>(previous + 1);
                    const std::int32_t rebirth = world::gSkillsF1BAFC[skill].rebirthState4C48;
                    const bool enabled = gSkillRebirthModeEnabled_5F9754 != 0;
                    if ((rebirth != 0) != enabled) {
                        static_cast<void>(cg::network::GetGameConnection().SendSkillRebirthSelection(skill + (enabled ? 100 : 0)));
                    }
                    WindowSkill_ShowRecipes(currentWindow, technique);
                    return;
                }
                case 5: {
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (cg::yobi::ReadMapActorAttachment44(world::gPlayerRuntimeSlots.action) == nullptr
                        && insufficientFp()) {
                        world::AppendChatMessage_44B050(kWindow12NoFp5864F0, 4, 3);
                        fail();
                        return;
                    }
                    const std::uint8_t rebirth = gSkillRebirthModeEnabled_5F9754;
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
                    static_cast<void>(cg::network::GetGameConnection().SendSkillUseRequest(skill + (rebirth != 0 ? 100 : 0), technique, -1, L""));
                    return;
                }
                default:
                    return;
                }
            };
            activate();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
        visualState = 1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const auto y = static_cast<std::int16_t>(element->y + window->y);
        const auto x = static_cast<std::int16_t>(element->x + window->x);
        WindowSkill_HandleTechniqueRow(x, y, visualState, technique, 3, 223, 0xFFFFFFFFu);
    }
    return consumed;
}

// 原函数：0x0047A140。
void WindowSkill_SyncStates()
{
    if (gWindowStates[WindowId_Skill].window == nullptr || gSkillWindowModeD672AC != 2) {
        return;
    }
    const std::int32_t index = gSkillWindowIndexD2EA60;
    const std::int32_t rebirth = world::gSkillsF1BAFC[index].rebirthState4C48;
    const bool enabled = gSkillRebirthModeEnabled_5F9754 != 0;
    if ((rebirth != 0) != enabled) {
        static_cast<void>(cg::network::GetGameConnection().SendSkillRebirthSelection(index + (enabled ? 100 : 0)));
    }
}

// 原函数：0x00487020。
static std::int32_t WindowSkill_SelectTextTarget(const std::int32_t index, const wchar_t* const text)
{
    if (index == -1) {
        WindowRuntime_DecrementOperationCount();
        const auto count = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD - 1);
        const auto skillCount = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE - 1);
        gSkillOperationCount_D67AFD = count;
        gSkillStatusIconReferences_D67AFE = skillCount;
        if (skillCount == 0 && world::GetPlayerStatusIcon_51BEC0() == 13) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(0));
        }
        return 1;
    }
    if (index == -2) {
        return 1;
    }
    if (index >= 0 && gSkillTargetPendingC8AF5F == 0) {
        if (gSkillTargetKindCF4A10 != 0 && gSkillTargetKindsD2F75C[index] != 3) {
            static_cast<void>(cg::network::GetGameConnection().SelectSkillTextTarget(index));
            gSkillSelectedTargetCC4B1C = index;
            gSkillTargetPendingC8AF5F = 1;
            return 0;
        }
        gSkillSendModeC8A970 = 0;
        char extra[256];
        extra[0] = '\0';
        gSkillTargetNameD2F748 = text;
        // 原509080只有ret；保留调用前的文本和状态写入，不伪造文本生产。
        std::int32_t skill;
        std::int32_t level;
        if (gSkillRebirthModeEnabled_5F9754 != 0) {
            skill = gUsingSkillIndexD2F32C;
            level = gUsingSkillLevelCA0AC4;
            skill += 100;
        } else if (gSkillSendModeC8A970 != 0) {
            skill = gUsingSkillIndexD2F32C;
            level = gUsingSkillLevelCA0AC4;
            skill += 200;
        } else {
            level = gUsingSkillLevelCA0AC4;
            skill = gUsingSkillIndexD2F32C;
        }
        static_cast<void>(cg::network::GetGameConnection().SendSkillUseRequest(skill, level, index, cg::text::FromGbkWide(extra)));
    }
    gSkillTargetPendingC8AF5F = 1;
    return 0;
}

// 原函数：0x004871B0。
static cg::yobi::YobiAction* WindowSkill_OpenTextTargets()
{
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    gSkillSendModeC8A970 = 0;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t kind = gSkillTargetKindsD2F75C[index];
        gSkillTargetNameViewsD45F98[index] = gSkillTargetNamesCB0550[index].data();
        gSkillTargetColorsCB1DE4[index] = static_cast<std::uint8_t>(colors[kind]);
    }
    const std::int32_t skillIndex = gUsingSkillIndexD2F32C;
    ++gSkillStatusIconReferences_D67AFE;
    const std::int32_t kind = WindowSkill_ClassifyAttributeSkill(world::gSkillsF1BAFC[skillIndex].skillId);
    if (kind >= 2 && kind <= 3) {
        if (world::GetPlayerStatusIcon_51BEC0() != 13) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(13));
        }
    } else {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(0));
    }
    WindowRuntime_IncrementOperationCount();
    ++gSkillOperationCount_D67AFD;
    constexpr std::array<std::int32_t, 5> windowIds {11, 12, 45, 46, 13};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    const std::int32_t count = gSkillTargetCountCA0004;
    gSkillTargetPendingC8AF5F = 0;
    return WindowTextTarget_Open(
        gSkillTargetNameViewsD45F98.data(), gSkillTargetColorsCB1DE4.data(), count, WindowSkill_SelectTextTarget);
}

// 原函数：0x004F0200。
static void WindowSkill_ReceiveTextTargets(const char* const text)
{
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    const std::int32_t kind = WindowSkill_ClassifyAttributeSkill(world::gSkillsF1BAFC[gUsingSkillIndexD2F32C].skillId);
    gSkillTargetKindCF4A10 = kind;
    gSkillTargetCountCA0004 = 0;
    gSkillTargetPageC8A974 = 0;
    gSkillTargetPageCountCA0178 = 1;
    for (auto& name : gSkillTargetNamesCB0550) {
        name.clear();
    }
    std::int32_t count = 0;
    if (kind != 0) {
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        count = 1;
        gSkillTargetKindsD2F75C[0] = 0;
        gSkillTargetCountCA0004 = count;
        gSkillTargetNamesCB0550[0] = name;
    }
    for (std::int16_t slot = 0; slot < 5; ++slot) {
        if (world::ReadPetMapFlags(slot) == 0) {
            continue;
        }
        const std::uint32_t ownIndex = world::gPlayerRuntimeSlots.objectIndex;
        if (world::ReadPetMapObjectIndex(slot) == ownIndex) {
            continue;
        }
        cg::yobi::YobiAction* const action = world::ReadPetMapAction(slot);
        if (action != nullptr) {
            gSkillTargetNamesCB0550[count] = cg::yobi::GetYobiName(action);
        }
        gSkillTargetKindsD2F75C[count] = 1;
        gSkillTargetCountCA0004 = ++count;
    }
    char name[256];
    std::int32_t field = 1;
    std::int32_t appended = 0;
    while (count < 10) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name) == 1) {
            count = gSkillTargetCountCA0004;
            break;
        }
        const std::int32_t flag = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        if (gSkillTargetKindCF4A10 == 0 && flag == 1) {
            count = gSkillTargetCountCA0004;
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t index = gSkillTargetCountCA0004;
        gSkillTargetNamesCB0550[index] = cg::text::FromGbkWide(name);
        gSkillTargetKindsD2F75C[index] = flag != 0 ? 3 : 2;
        count = index + 1;
        gSkillTargetCountCA0004 = count;
        if (++appended >= 10) {
            break;
        }
    }
    gSkillTargetPageCountCA0178 = (count + 4) / 5;
    if (count > 0) {
        static_cast<void>(WindowSkill_OpenTextTargets());
    } else if (gSkillTargetKindCF4A10 == 0) {
        world::AppendChatMessage_44B050(L"没有可变装的角色！", 4, 3);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E340。
void WindowSkill_ReceiveTextTargetReply(cg::network::CgConnection&, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    detail::WindowSkill_ReceiveTextTargets(text);
}

// 原函数：0x0050E360。
void WindowSkill_ReceiveAttributeTargetReply(cg::network::CgConnection&, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowSkill_ReceiveAttributeTargets(text));
}

} // namespace cg::ui
