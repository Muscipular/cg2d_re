#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetSkillChoiceElementId : ElementId {
    HandleConfirmation = 1,
    ButtonClose = 2,
    ButtonLearn = 3,
    ButtonBack = 4,
    Element5 = 5,
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
    HandleVerticalScroll_19 = 19,
    ButtonPreviousRow_20 = 20,
    ButtonNextRow_21 = 21,
    ButtonPreviousPage_22 = 22,
    ButtonNextPage_23 = 23,
    Scroll_24 = 24,
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
    Element47 = 47,
    Element48 = 48,
    Element49 = 49,
    Element50 = 50,
    Element51 = 51,
    Element52 = 52,
    Element53 = 53,
    Element54 = 54,
    Element55 = 55,
    Element56 = 56,
    Element57 = 57,
    Element58 = 58,
    Element59 = 59,
    Element60 = 60,
    Element61 = 61,
    Element62 = 62,
    Element63 = 63,
    Element64 = 64,
    Element65 = 65,
    Element66 = 66,
    Element67 = 67,
    Element68 = 68,
    Element69 = 69,
    Element70 = 70,
    Element71 = 71,
    Element72 = 72,
    Element73 = 73,
    Element74 = 74,
    Element75 = 75,
    Element76 = 76,
    Element77 = 77,
    Element78 = 78,
    Element79 = 79,
    Element80 = 80,
    Element81 = 81,
    Element82 = 82,
    Element83 = 83,
    Element84 = 84,
    Element85 = 85,
    Element86 = 86,
    Element87 = 87,
    Element88 = 88,
    Element89 = 89,
    Element90 = 90,
    Element91 = 91,
    Element92 = 92,
    Element93 = 93,
    Element94 = 94,
    Element95 = 95,
    Element96 = 96,
    Element97 = 97,
    Element98 = 98,
    Element99 = 99,
    Element100 = 100,
    Element101 = 101,
    Element102 = 102,
    Element103 = 103,
    Element104 = 104,
    Element105 = 105,
    Element106 = 106,
    HandleSideArea = 107,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004BE3F0。
static void WindowPetSkillChoice_RefreshLearn(session::ClientSession& session, WindowInstance* window); // 模式0技能栏/说明/花费，reqSlot按低S8。

// 原函数：0x004BE600。
static void WindowPetSkillChoice_RefreshForget(session::ClientSession& session, WindowInstance* window); // 模式1遗忘映射/警告/回调，与原技能表同源。

// 原函数：0x004BE930。
static void WindowPetSkillChoice_RefreshPetNames(session::ClientSession& session, WindowInstance* window); // 固定旧窗的五宠物名称/元素及技能分页刷新。

// 原函数：0x004BEC20。
static void WindowPetSkillChoice_RefreshPetCandidates(session::ClientSession& session, WindowInstance* window); // 玩家索引对转候选映射，绑定真实选择回调。

// 原函数：0x004BEF30。
static void WindowPetSkillChoice_RefreshSelectedPetSkills(session::ClientSession& session, WindowInstance* window); // 技能槽计数/候选原序显示，先右侧再左侧刷新。

// 原函数：0x004BF160。
static void WindowPetSkillChoice_RefreshLearnRows(session::ClientSession& session, WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式0十行原技能。

// 原函数：0x004BF2E0。
static void WindowPetSkillChoice_RefreshForgetRows(session::ClientSession& session, WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式1十行协议名称/双数值。

// 原函数：0x004BF460。
static void WindowPetSkillChoice_RefreshPetRows(session::ClientSession& session, WindowInstance* window, std::int32_t scroll); // 五行，存在性与内容索引不同。

// 原函数：0x004BF620。
static void WindowPetSkillChoice_RefreshSelectedSkillRows(session::ClientSession& session, WindowInstance* window, std::int32_t scroll); // 按候选槽数逐轮重读，不加十行钳制。

// 原函数：0x004BFBC0。
static std::int32_t WindowPetSkillChoice_ButtonLearn(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 模式0学习请求，bit2恒消费。

// 原函数：0x004BFCB0。
static std::int32_t WindowPetSkillChoice_ButtonBack(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 分模式关闭/返回，音效后重读模式。

// 原函数：0x004BFF60。
static std::int32_t WindowPetSkillChoice_ButtonPreviousRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // bit400上一行，触边仍刷新。

// 原函数：0x004C0160。
static std::int32_t WindowPetSkillChoice_ButtonNextRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一行，12/13固定左侧且目标index-1。

// 原函数：0x004C0380。
static std::int32_t WindowPetSkillChoice_ButtonPreviousPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 上一页，音效后重读位置。

// 原函数：0x004C0590。
static std::int32_t WindowPetSkillChoice_ButtonNextPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 下一页，先上限后负值修正。

// 原函数：0x004C0800。
static std::int32_t WindowPetSkillChoice_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 两侧滚轮；模式0/1只更新位置。

// 原函数：0x004C0A50。
static std::int32_t WindowPetSkillChoice_SelectPetCandidate(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 候选门禁/提示/发送，hintFlag不阻止发送。

// 原函数：0x004C0910。
static std::int32_t WindowPetSkillChoice_SubmitForget(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 遗忘映射+1发送，发送后关闭窗口。

// 原函数：0x004C0BF0。
static std::int32_t WindowPetSkillChoice_SelectSkillSlot(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 选技能槽并显示元素0，不提前发送。

// 原函数：0x004C0CB0。
static std::int32_t WindowPetSkillChoice_SubmitPetSkill(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 技能选择/请求，发送后对话类型=-2。

// 原函数：0x004C0EA0。
static std::int32_t WindowPetSkillChoice_DrawSelectedSkillFrame(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 已选技能内框，恒0。

// 原函数：0x004C0F20。
static std::int32_t WindowPetSkillChoice_HandleConfirmation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 确认覆盖层，发原选槽文本后请求关闭。

// 原函数：0x004BD500。
static void WindowPetSkillChoice_Release(WindowInstance&);

// 原函数：0x004BF7C0。
static std::int32_t WindowPetSkillChoice_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004BF950。
static std::int32_t WindowPetSkillChoice_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic574668 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574668。

static WindowGraphicData gWindowGraphic574680 {244383, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574680。

static WindowGraphicData gWindowGraphic574698 {244384, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574698。

static WindowGraphicData gWindowGraphic5746B0 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746B0。

static WindowGraphicData gWindowGraphic5746C8 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746C8。

static WindowGraphicData gWindowGraphic5746E0 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746E0。

static WindowGraphicData gWindowGraphic5746F8 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746F8。

static WindowGraphicData gWindowGraphic574710 {245320, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574710。

static WindowGraphicData gWindowGraphic574728 {244387, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574728。

static WindowGraphicData gWindowGraphic574740 {243103, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574740。

static WindowGraphicData gWindowGraphic574758 {244386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574758。

static WindowGraphicData gWindowGraphic574770 {243127, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574770。

static WindowGraphicData gWindowGraphic574788 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574788。

static WindowGraphicData gWindowGraphic5747A0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005747A0。

static WindowTextPrototype gWindowText5747C0 { // 原全局：0x005747C0。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

static WindowTextPrototype gWindowText5747C8 { // 原全局：0x005747C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

static WindowTextPrototype gWindowText5747D0 { // 原全局：0x005747D0。
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

static WindowTextPrototype gWindowText5747D8 { // 原全局：0x005747D8。
    {std::byte{0x8A}, std::byte{8}, std::byte{0}, std::byte{0}}, L"0000000000", // 原常量参考：0x00585DF4。
};

static std::int32_t gWindowAnimationImage5747E0 = 100000; // 原全局：0x005747E0。

static WindowNumberData gWindowNumber5747E8 {nullptr, 0, 3, 1, 0}; // 原全局：0x005747E8。

static WindowNumberData gWindowNumber5747F0 {nullptr, 0, 3, 1, 0}; // 原全局：0x005747F0。

static WindowNumberData gWindowNumber5747F8 {nullptr, 0, 0, 1, 0}; // 原全局：0x005747F8。

static WindowNumberData gWindowNumber574800 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574800。

static WindowNumberData gWindowNumber574808 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574808。

static WindowNumberData gWindowNumber574810 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574810。

static WindowNumberData gWindowNumber574818 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574818。

static WindowNumberData gWindowNumber574820 {nullptr, 0, 3, 1, 0}; // 原全局：0x00574820。

static WindowNumberData gWindowNumber574828 {nullptr, 0, 3, 1, 0}; // 原全局：0x00574828。

static WindowNumberData gWindowNumber574830 {nullptr, 0, 3, 1, 0}; // 原全局：0x00574830。

static WindowNumberData gWindowNumber574838 {nullptr, 0, 3, 1, 0}; // 原全局：0x00574838。

static WindowNumberData gWindowNumber574840 {nullptr, 0, 3, 1, 0}; // 原全局：0x00574840。

static WindowNumberData gWindowNumber574848 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574848。

static WindowNumberData gWindowNumber574850 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574850。

static WindowNumberData gWindowNumber574858 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574858。

static WindowNumberData gWindowNumber574860 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574860。

static WindowNumberData gWindowNumber574868 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574868。

static WindowNumberData gWindowNumber574870 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574870。

static WindowNumberData gWindowNumber574878 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574878。

static WindowNumberData gWindowNumber574880 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574880。

static WindowNumberData gWindowNumber574888 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574888。

static WindowNumberData gWindowNumber574890 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574890。

static WindowNumberData gWindowNumber574898 {nullptr, 0, 0, 1, 0}; // 原全局：0x00574898。

static WindowNumberData gWindowNumber5748A0 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748A0。

static WindowNumberData gWindowNumber5748A8 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748A8。

static WindowNumberData gWindowNumber5748B0 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748B0。

static WindowNumberData gWindowNumber5748B8 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748B8。

static WindowNumberData gWindowNumber5748C0 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748C0。

static WindowNumberData gWindowNumber5748C8 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748C8。

static WindowNumberData gWindowNumber5748D0 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748D0。

static WindowNumberData gWindowNumber5748D8 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748D8。

static WindowNumberData gWindowNumber5748E0 {nullptr, 0, 0, 1, 0}; // 原全局：0x005748E0。

static std::uint32_t gWindowMouseFlagsD67900 = 0; // 原全局：0x00D67900。

// 原全局：0x005748E8。
// 原5748E8的107项；选择回调由模式刷新发布，不替换初始4B0F10。
static WindowElementPrototype gWindowElements5748E8[107] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowPetSkillChoice_HandleConfirmation, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::HandleConfirmation)},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic574668}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonClose)},
    {WindowElementType_Graphic, 203, 326, 66, 17, 1, {.graphic = &gWindowGraphic5746F8}, &WindowPetSkillChoice_ButtonLearn, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonLearn)},
    {WindowElementType_Graphic, 467, 326, 66, 17, 1, {.graphic = &gWindowGraphic574710}, &WindowPetSkillChoice_ButtonBack, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonBack)},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText5747C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element5)},
    {WindowElementType_Text, 78, 52, 0, 14, 1, {.text = &gWindowText5747D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element6)},
    {WindowElementType_Number, 262, 37, 0, 14, 1, {.number = &gWindowNumber5747F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element7)},
    {WindowElementType_Number, 389, 30, 0, 14, 1, {.number = &gWindowNumber5747E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element8)},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5747E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element9)},
    {WindowElementType_Graphic, 502, 146, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element10)},
    {WindowElementType_MouseArea, 502, 146, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 502, 136, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonPreviousRow)},
    {WindowElementType_Graphic, 502, 282, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonNextRow)},
    {WindowElementType_Graphic, 472, 300, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonPreviousPage)},
    {WindowElementType_Graphic, 498, 300, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonNextPage)},
    {WindowElementType_EventArea, 286, 27, 273, 280, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Scroll)},
    {WindowElementType_Graphic, 499, 132, 11, 11, 1, {.graphic = &gWindowGraphic574728}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element17)},
    {WindowElementType_Graphic, 269, 85, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)},
    {WindowElementType_MouseArea, 269, 85, 11, 244, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::HandleVerticalScroll_19)},
    {WindowElementType_Graphic, 269, 75, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonPreviousRow_20)},
    {WindowElementType_Graphic, 269, 327, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonNextRow_21)},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonPreviousPage_22)},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::ButtonNextPage_23)},
    {WindowElementType_EventArea, 17, 28, 274, 325, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Scroll_24)},
    {WindowElementType_Text, 33, 87, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element25)},
    {WindowElementType_Text, 33, 127, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element26)},
    {WindowElementType_Text, 33, 147, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element27)},
    {WindowElementType_Text, 33, 167, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element28)},
    {WindowElementType_Text, 33, 187, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element29)},
    {WindowElementType_Text, 33, 207, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element30)},
    {WindowElementType_Text, 33, 227, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element31)},
    {WindowElementType_Text, 33, 247, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element32)},
    {WindowElementType_Text, 33, 267, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element33)},
    {WindowElementType_Text, 33, 287, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element34)},
    {WindowElementType_Text, 296, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element35)},
    {WindowElementType_Text, 296, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element36)},
    {WindowElementType_Text, 296, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element37)},
    {WindowElementType_Text, 296, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element38)},
    {WindowElementType_Text, 296, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element39)},
    {WindowElementType_Text, 296, 215, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element40)},
    {WindowElementType_Text, 296, 231, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element41)},
    {WindowElementType_Text, 296, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element42)},
    {WindowElementType_Text, 296, 263, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element43)},
    {WindowElementType_Text, 296, 279, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element44)},
    {WindowElementType_Text, 446, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element45)},
    {WindowElementType_Text, 446, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element46)},
    {WindowElementType_Text, 446, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element47)},
    {WindowElementType_Text, 446, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element48)},
    {WindowElementType_Text, 446, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element49)},
    {WindowElementType_Number, 454, 136, 0, 0, 1, {.number = &gWindowNumber574848}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element50)},
    {WindowElementType_Number, 454, 152, 0, 0, 1, {.number = &gWindowNumber574850}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element51)},
    {WindowElementType_Number, 454, 168, 0, 0, 1, {.number = &gWindowNumber574858}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element52)},
    {WindowElementType_Number, 454, 184, 0, 0, 1, {.number = &gWindowNumber574860}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element53)},
    {WindowElementType_Number, 454, 200, 0, 0, 1, {.number = &gWindowNumber574868}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element54)},
    {WindowElementType_Number, 454, 216, 0, 0, 1, {.number = &gWindowNumber574870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element55)},
    {WindowElementType_Number, 454, 232, 0, 0, 1, {.number = &gWindowNumber574878}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element56)},
    {WindowElementType_Number, 454, 248, 0, 0, 1, {.number = &gWindowNumber574880}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element57)},
    {WindowElementType_Number, 454, 264, 0, 0, 1, {.number = &gWindowNumber574888}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element58)},
    {WindowElementType_Number, 454, 280, 0, 0, 1, {.number = &gWindowNumber574890}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element59)},
    {WindowElementType_Number, 492, 136, 0, 0, 1, {.number = &gWindowNumber574898}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element60)},
    {WindowElementType_Number, 492, 152, 0, 0, 1, {.number = &gWindowNumber5748A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element61)},
    {WindowElementType_Number, 492, 168, 0, 0, 1, {.number = &gWindowNumber5748A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element62)},
    {WindowElementType_Number, 492, 184, 0, 0, 1, {.number = &gWindowNumber5748B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element63)},
    {WindowElementType_Number, 492, 200, 0, 0, 1, {.number = &gWindowNumber5748B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element64)},
    {WindowElementType_Number, 492, 216, 0, 0, 1, {.number = &gWindowNumber5748C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element65)},
    {WindowElementType_Number, 492, 232, 0, 0, 1, {.number = &gWindowNumber5748C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element66)},
    {WindowElementType_Number, 492, 248, 0, 0, 1, {.number = &gWindowNumber5748D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element67)},
    {WindowElementType_Number, 492, 264, 0, 0, 1, {.number = &gWindowNumber5748D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element68)},
    {WindowElementType_Number, 492, 280, 0, 0, 1, {.number = &gWindowNumber5748E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element69)},
    {WindowElementType_Graphic, 290, 133, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element70)},
    {WindowElementType_Graphic, 290, 149, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element71)},
    {WindowElementType_Graphic, 290, 165, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element72)},
    {WindowElementType_Graphic, 290, 181, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element73)},
    {WindowElementType_Graphic, 290, 197, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element74)},
    {WindowElementType_Graphic, 290, 213, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element75)},
    {WindowElementType_Graphic, 290, 229, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element76)},
    {WindowElementType_Graphic, 290, 245, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element77)},
    {WindowElementType_Graphic, 290, 261, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element78)},
    {WindowElementType_Graphic, 290, 277, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element79)},
    {WindowElementType_Text, 41, 82, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element80)},
    {WindowElementType_Text, 41, 137, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element81)},
    {WindowElementType_Text, 41, 192, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element82)},
    {WindowElementType_Text, 41, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element83)},
    {WindowElementType_Text, 41, 302, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element84)},
    {WindowElementType_Number, 239, 83, 0, 14, 1, {.number = &gWindowNumber5747F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element85)},
    {WindowElementType_Number, 239, 138, 0, 14, 1, {.number = &gWindowNumber574800}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element86)},
    {WindowElementType_Number, 239, 193, 0, 14, 1, {.number = &gWindowNumber574808}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element87)},
    {WindowElementType_Number, 239, 248, 0, 14, 1, {.number = &gWindowNumber574810}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element88)},
    {WindowElementType_Number, 239, 303, 0, 14, 1, {.number = &gWindowNumber574818}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element89)},
    {WindowElementType_Number, 233, 108, 0, 14, 1, {.number = &gWindowNumber574820}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element90)},
    {WindowElementType_Number, 233, 163, 0, 14, 1, {.number = &gWindowNumber574828}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element91)},
    {WindowElementType_Number, 233, 218, 0, 14, 1, {.number = &gWindowNumber574830}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element92)},
    {WindowElementType_Number, 233, 273, 0, 14, 1, {.number = &gWindowNumber574838}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element93)},
    {WindowElementType_Number, 233, 328, 0, 14, 1, {.number = &gWindowNumber574840}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element94)},
    {WindowElementType_Graphic, 37, 80, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element95)},
    {WindowElementType_Graphic, 37, 135, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element96)},
    {WindowElementType_Graphic, 37, 190, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element97)},
    {WindowElementType_Graphic, 37, 245, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element98)},
    {WindowElementType_Graphic, 37, 300, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element99)},
    {WindowElementType_Graphic, 27, 70, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element100)},
    {WindowElementType_Graphic, 27, 125, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element101)},
    {WindowElementType_Graphic, 27, 180, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element102)},
    {WindowElementType_Graphic, 27, 235, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element103)},
    {WindowElementType_Graphic, 27, 290, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element104)},
    {WindowElementType_Graphic, 17, 28, 274, 325, 1, {.graphic = &gWindowGraphic574698}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element105)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic574680}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::Element106)},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetSkillChoiceElementId::HandleSideArea)},
};

// 原型：窗口54；原全局：0x0054EC90。
WindowPrototype gWindowPrototypePetSkillChoice {0, 4, 10, 30, 549, 366, 0x80010101u, 107, gWindowElements5748E8, &WindowPetSkillChoice_Update, &WindowPetSkillChoice_Draw, &WindowPetSkillChoice_Release}; // 54: 54EC90

// 窗口函数实现。

// 原函数：0x004BD500。
static void WindowPetSkillChoice_Release(WindowInstance& releasedWindow)
{
    auto& npcDialog = releasedWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = releasedWindow.ownerSession->UiFrame().native.petSkillChoice;
    if (choice.lifecycle != 2
        && choice.dialogType == npcDialog.type) {
        npcDialog.type = -1;
    }
    choice.lifecycle = 0;
}

// 原函数：0x004BD540。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenLearn(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    std::array<char, 1536> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(npcLines[row + 1].text, buffer.data(), row, 21));
    }
    char headingGbkBuffer[3072]{};
    char* const heading = headingGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 4, 80, heading));
    input::UnescapeLegacyTextAndNotify_52D2F0(heading);
    npcLines[9].text = cg::text::FromGbkWide(heading);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 5, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 8; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(npcLines[row + 10].text, buffer.data(), row, 32));
    }
    choice.valueCC2E68 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    choice.valueCF4A3C = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    choice.firstCount = 0;
    choice.secondCount = 0;
    choice.mode = 0;
    return WindowRuntime_CreateAnimation(session, WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 原函数：0x004BD680。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenForget(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    std::array<char, 1536> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(npcLines[row + 1].text, buffer.data(), row, 21));
    }
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::int32_t field = 4 + row * 3;
        char nameGbkBuffer[3072]{};
    char* const name = nameGbkBuffer;
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field, 21, name));
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
    npcLines[row + 10].text = cg::text::FromGbkWide(name);
        std::wstring& destination = choice.names[row].text;
        destination = cg::text::FromGbkWide(name);
        const char first = name[0];
        // 原分段字节复制已并入上面的UTF-16赋值。
        if (first != '\0') {
            choice.firstValues[row] =
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
            choice.secondValues[row] =
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        } else {
            choice.firstValues[row] = -1;
            choice.secondValues[row] = -1;
        }
    }
    choice.mode = 1;
    choice.firstCount = 0;
    choice.secondCount = 0;
    return WindowRuntime_CreateAnimation(session, WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 两原创建仅模式值不同，保留原窗快照和生命周期标记的分支次序。
static cg::yobi::YobiAction* WindowPetSkillChoice_CreateSkillList(
    session::ClientSession& session,
    const char* const text, const std::uint8_t mode)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    std::array<char, 768> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(npcLines[1 + group * 4 + row].text, buffer.data(), row, 20));
        }
    }
    choice.skills.fill({});
    choice.secondCount = 0;
    std::int32_t field = 6;
    for (Window54SkillRecord& record : choice.skills) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        record.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        record.name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 24 ? "???" : buffer.data());
        record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(
                record.description[row], buffer.data(), row, 28));
        }
        ++choice.secondCount;
    }
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_PetSkillChoice].window;
    choice.firstCount = 0;
    choice.mode = mode;
    if (window != nullptr) {
        if (choice.lifecycle != 0) {
            choice.lifecycle = 0;
            WindowPetSkillChoice_RefreshPetNames(session, window);
            return nullptr;
        }
        window->state |= 0x8000u;
        choice.lifecycle = 2;
    } else {
        choice.lifecycle = 0;
    }
    return WindowRuntime_CreateAnimation(session, WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 原函数：0x004BD810。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenPetSkills(session::ClientSession& session, const char* const text)
{
    return WindowPetSkillChoice_CreateSkillList(session, text, 2);
}

// 原函数：0x004BDAD0。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenPetSkillsByTribe(session::ClientSession& session, const char* const text)
{
    return WindowPetSkillChoice_CreateSkillList(session, text, 12);
}

// 原函数：0x004BDD90。
void WindowPetSkillChoice_ReceivePetCandidates(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    if (WindowRuntime_States(session)[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 768> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 255, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(npcLines[row + 1].text, buffer.data(), row, 20));
    }
    choice.petOptions.fill({});
    const std::int32_t threshold = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    std::int32_t field = 5;
    for (Window54PetOption& option : choice.petOptions) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        option.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        option.name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        option.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.slots = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.tribe = -1;
        option.hintFlag = 1;
        if (threshold <= option.value04) {
            option.allowed = 1;
        }
    }
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_PetSkillChoice].window;
    choice.tribe = -1;
    choice.firstCount = 0;
    choice.secondCount = 0;
    choice.mode = 3;
    choice.lifecycle = 0;
    WindowPetSkillChoice_RefreshPetCandidates(session, window);
}

// 原函数：0x004BDF70。
void WindowPetSkillChoice_ReceiveTribeCandidates(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    if (WindowRuntime_States(session)[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 768> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(npcLines[1 + group * 4 + row].text, buffer.data(), row, 20));
        }
    }
    choice.petOptions.fill({});
    const std::int32_t threshold = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    choice.tribe = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    std::int32_t field = 8;
    for (Window54PetOption& option : choice.petOptions) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        option.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        option.name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        option.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.slots = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.tribe = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.hintFlag = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        if (threshold <= option.value04) {
            option.allowed = 1;
        }
    }
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_PetSkillChoice].window;
    choice.firstCount = 0;
    choice.secondCount = 0;
    choice.mode = 13;
    choice.lifecycle = 0;
    WindowPetSkillChoice_RefreshPetCandidates(session, window);
}

// 原函数：0x004BE200。
void WindowPetSkillChoice_ReceiveSelectedPetSkills(session::ClientSession& session, const char* const text)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    if (WindowRuntime_States(session)[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 1536> buffer;
    choice.dialogType = npcDialog.type;
    choice.imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(npcLines[row + 1].text, buffer.data(), row, 21));
    }
    choice.techniqueSlots.fill({});
    std::int32_t field = 4;
    for (Window54TechniqueSlot& slot : choice.techniqueSlots) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 511, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (buffer[0] == '\0') {
            slot.used = 0;
            field += 2;
            continue;
        }
        slot.used = 1;
        slot.name = cg::text::FromGbkWide(std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        slot.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 511, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(
                slot.description[row], buffer.data(), row, 28));
        }
    }
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_PetSkillChoice].window;
    choice.firstCount = 0;
    choice.secondCount = 0;
    choice.mode = 4;
    choice.lifecycle = 0;
    WindowPetSkillChoice_RefreshSelectedPetSkills(session, window);
}

// 原函数：0x004BE3F0。
static void WindowPetSkillChoice_RefreshLearn(session::ClientSession& session, WindowInstance* const window)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    if (auto* const element105 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element105) : nullptr; element105) element105->data.graphic->imageNo = 244384;
    if (auto* const buttonLearnElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::ButtonLearn) : nullptr; buttonLearnElement) buttonLearnElement->visible = 1;
    if (auto* const element6 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element6) element6->visible = 1;
    choice.firstCount = 0;
    std::int32_t usedSlots = 0;
    std::int32_t count = 0;
    for (const world::SkillPairRecord& pair : world::gSkillPairsF63370) {
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            usedSlots += std::bit_cast<std::int8_t>(
                world::gSkillsF1BAFC[index].requiredSlots19[0]);
            ++count;
        }
    }
    const std::int32_t totalSlots = world::gPlayerRuntimeSlots.value16C;
    choice.firstCount = count;
    auto* const element62 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr;
    if (element62) {
        cg::text::FormatTo(element62->data.text->text, L"{:2}/{:2}", usedSlots, totalSlots);
    }
    std::int32_t freeSlots = world::gPlayerRuntimeSlots.value16C - usedSlots;
    if (freeSlots < 0) {
        freeSlots = 0;
    }
    const std::int32_t displayed = choice.firstCount + freeSlots;
    choice.firstCount = displayed;
    for (std::int32_t index = 9; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = displayed > 10 ? 1 : 0;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element25) : nullptr; element25) element25->visible = 1;
    if (auto* const element252 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element25) : nullptr; element252) element252->data.text->text = npcLines[9].text;
    for (std::int32_t row = 0; row < 8; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 25)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 25)) : nullptr; indexedElement2) indexedElement2->data.text->text = npcLines[row + 10].text;
    }
    if (auto* const element34 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr; element34) element34->visible = 1;
    auto* const element342 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr;
    if (element342) {
        cg::text::FormatTo(element342->data.text->text, L"所需的技能栏位数量：{:2}栏", choice.valueCC2E68);
    }
    if (auto* const element7 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element7) : nullptr; element7) element7->visible = 1;
    cg::text::FormatTo(choice.amountText, L"{:10}", choice.valueCF4A3C);
    for (std::int32_t row = 0; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->mouseCallback = &WindowElement_NoOp;
    }
    const std::int32_t currentCount = choice.firstCount;
    const std::int32_t scroll = choice.petScroll;
    WindowPetSkillChoice_RefreshLearnRows(session, window, scroll, currentCount);
    for (std::int32_t index = 17; index <= 23; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    for (std::int32_t index = 79; index <= 103; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
}

// 原函数：0x004BE600。
static void WindowPetSkillChoice_RefreshForget(session::ClientSession& session, WindowInstance* const window)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    if (auto* const element105 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element105) : nullptr; element105) element105->data.graphic->imageNo = 244384;
    if (auto* const buttonLearnElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::ButtonLearn) : nullptr; buttonLearnElement) buttonLearnElement->visible = 0;
    if (auto* const element6 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element6) element6->visible = 1;
    for (std::int32_t row = 0; row < world::kSkillSlotCount; ++row) {
        const world::SkillPairRecord& pair = world::gSkillPairsF63370[row];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            std::int32_t empty = 0;
            for (std::int32_t skill = 0; skill < index; ++skill) {
                if (world::gSkillsF1BAFC[skill].name[0] == '\0') {
                    ++empty;
                }
            }
            const auto lowIndex = static_cast<std::uint8_t>(pair.index);
            choice.skillMap[row] =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(lowIndex - empty));
        } else {
            choice.skillMap[row] = -1;
        }
    }
    choice.firstCount = 0;
    std::int32_t usedSlots = 0;
    std::int32_t count = 0;
    for (const world::SkillPairRecord& pair : world::gSkillPairsF63370) {
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            usedSlots += std::bit_cast<std::int8_t>(
                world::gSkillsF1BAFC[index].requiredSlots19[0]);
            ++count;
        }
    }
    const std::int32_t totalSlots = world::gPlayerRuntimeSlots.value16C;
    choice.firstCount = count;
    auto* const element62 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr;
    if (element62) {
        cg::text::FormatTo(element62->data.text->text, L"{:2}/{:2}", usedSlots, totalSlots);
    }
    std::int32_t freeSlots = world::gPlayerRuntimeSlots.value16C - usedSlots;
    if (freeSlots < 0) {
        freeSlots = 0;
    }
    const std::int32_t displayed = choice.firstCount + freeSlots;
    choice.firstCount = displayed;
    for (std::int32_t index = 9; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = displayed > 10 ? 1 : 0;
    }
    if (auto* const element25 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element25) : nullptr; element25) element25->visible = 0;
    if (auto* const element26 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element26) : nullptr; element26) element26->data.text->text = L"请选择";
    if (auto* const element27 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element27) : nullptr; element27) element27->data.text->text = L"想遗忘的技能";
    if (auto* const element28 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element28) : nullptr; element28) element28->data.text->text = L"";
    if (auto* const element29 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element29) : nullptr; element29) element29->data.text->text = L"＊＊＊＊＊＊　注意　＊＊＊＊＊＊";
    if (auto* const element30 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element30) : nullptr; element30) element30->data.text->text = L"";
    if (auto* const element31 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element31) : nullptr; element31) element31->data.text->text = L"　一旦遗忘技能后";
    if (auto* const element32 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element32) : nullptr; element32) element32->data.text->text = L"　就不能再恢复原状了。";
    if (auto* const element33 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element33) : nullptr; element33) element33->data.text->text = L"　请特别注意。";
    for (std::int32_t index = 25; index <= 32; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    if (auto* const element34 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr; element34) element34->visible = 0;
    if (auto* const element7 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element7) : nullptr; element7) element7->visible = 1;
    cg::text::FormatTo(choice.amountText, L"{:10}", 0);
    for (std::int32_t row = 0; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->mouseCallback = &WindowPetSkillChoice_SubmitForget;
    }
    const std::int32_t currentCount = choice.firstCount;
    const std::int32_t scroll = choice.petScroll;
    WindowPetSkillChoice_RefreshForgetRows(session, window, scroll, currentCount);
    for (std::int32_t index = 17; index <= 23; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    for (std::int32_t index = 79; index <= 103; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
}

// 原函数：0x004BE930。
static void WindowPetSkillChoice_RefreshPetNames(session::ClientSession& session, WindowInstance* const window)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    if (auto* const element105 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element105) : nullptr; element105) element105->data.graphic->imageNo = 244385;
    if (auto* const buttonLearnElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::ButtonLearn) : nullptr; buttonLearnElement) buttonLearnElement->visible = 0;
    if (auto* const element6 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element62 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element62) element62->data.text->text = L"";
    for (std::int32_t index = 9; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element34 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr; element34) element34->visible = 0;
    if (auto* const element7 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element7) : nullptr; element7) element7->visible = 1;
    cg::text::FormatTo(choice.amountText, L"{:10}", 0);
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t petIndex =
            std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[row].index);
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement2) indexedElement2->mouseCallback = &WindowElement_NoOp;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement3) indexedElement3->data.graphic->imageNo = 243100;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement4) indexedElement4->visible = 1;
        if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement5) indexedElement5->visible = 0;
        if (choice.mode == 12) {
            if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement6) indexedElement6->visible = 1;
        }
        if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement6) indexedElement6->visible = 1;
        if (auto* const indexedElement7 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement7) indexedElement7->visible = 1;
        auto* const indexedElement8 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr;
        if (indexedElement8) {
            indexedElement8->data.text->text.clear();
        }
        auto* const indexedElement9 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr;
        if (indexedElement9) {
            indexedElement9->data.text->text.clear();
        }
        const world::PetRuntimeRecord& pet = world::gPetRuntimeRecords[petIndex];
        const bool used = pet.used != 0;
        choice.firstValueTexts[row].clear();
        choice.secondValueTexts[row].clear();
        if (used) {
            const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            if (auto* const indexedElement10 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement10) indexedElement10->data.text->text = name;
            const std::int32_t tribe = std::bit_cast<std::int16_t>(pet.value04);
            if (auto* const indexedElement11 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement11) indexedElement11->data.text->text = gTribeNames57CB54[tribe];
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement4) indexedElement4->visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(session, window, choice.skillScroll);
}

// 原函数：0x004BEC20。
static void WindowPetSkillChoice_RefreshPetCandidates(session::ClientSession& session, WindowInstance* const window)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    if (auto* const element105 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element105) : nullptr; element105) element105->data.graphic->imageNo = 244385;
    if (auto* const buttonLearnElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::ButtonLearn) : nullptr; buttonLearnElement) buttonLearnElement->visible = 0;
    if (auto* const element6 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element62 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element62) element62->data.text->text = L"";
    for (std::int32_t index = 9; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element34 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr; element34) element34->visible = 0;
    if (auto* const element7 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element7) : nullptr; element7) element7->visible = 1;
    cg::text::FormatTo(choice.amountText, L"{:10}", choice.skills[choice.selected].price);
    for (std::int32_t row = 0; row < 5; ++row) {
        const world::PetPairRecord& pair = world::gPetPairsDE0B08[row];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            std::int32_t empty = 0;
            for (std::int32_t pet = 0; pet < index; ++pet) {
                if (world::gPetRuntimeRecords[pet].used == 0) {
                    ++empty;
                }
            }
            const auto lowIndex = static_cast<std::uint8_t>(pair.index);
            choice.petMap[row] =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(lowIndex - empty));
        } else {
            choice.petMap[row] = -1;
        }
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t selected = choice.petMap[row];
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement2) indexedElement2->mouseCallback = &WindowPetSkillChoice_SelectPetCandidate;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement3) indexedElement3->visible = 1;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement4) indexedElement4->visible = 0;
        if (choice.mode == 13) {
            if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement5) indexedElement5->visible = 1;
        }
        if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement5) indexedElement5->visible = 1;
        if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement6) indexedElement6->visible = 1;
        auto* const indexedElement7 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr;
        if (indexedElement7) {
            indexedElement7->data.text->text.clear();
        }
        auto* const indexedElement8 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr;
        if (indexedElement8) {
            indexedElement8->data.text->text.clear();
        }
        choice.firstValueTexts[row].clear();
        choice.secondValueTexts[row].clear();
        if (selected >= 0 && choice.petOptions[selected].used != 0) {
            const Window54PetOption& option = choice.petOptions[selected];
            if (auto* const indexedElement9 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement9) indexedElement9->data.text->text = option.name;
            const std::int32_t tribe = option.tribe;
            if (tribe >= 0) {
                if (auto* const indexedElement10 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement10) indexedElement10->data.text->text = gTribeNames57CB54[tribe];
            }
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement4) indexedElement4->visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(session, window, choice.skillScroll);
}

// 原函数：0x004BEF30。
static void WindowPetSkillChoice_RefreshSelectedPetSkills(session::ClientSession& session, WindowInstance* const window)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    if (auto* const element105 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element105) : nullptr; element105) element105->data.graphic->imageNo = 244385;
    if (auto* const buttonLearnElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::ButtonLearn) : nullptr; buttonLearnElement) buttonLearnElement->visible = 0;
    if (auto* const element6 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr; element6) element6->visible = 1;
    const std::int32_t capacity = std::min(choice.petOptions[choice.petChoice].slots, world::kPetTechniqueSlotCount);
    choice.firstCount = capacity;
    choice.petScroll = std::clamp(choice.petScroll, 0, std::max(0, capacity - 10));
    std::int32_t used = 0;
    for (std::int32_t index = 0; index < capacity; ++index) {
        if (choice.techniqueSlots[index].used != 0) {
            ++used;
        }
    }
    auto* const element62 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element6) : nullptr;
    if (element62) {
        cg::text::FormatTo(element62->data.text->text, L"{:2}/{:2}", used, capacity);
    }
    for (std::int32_t index = 9; index <= 16; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = capacity > 10 ? 1 : 0;
    }
    auto* const handleVerticalScrollElement = window ? window->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll) : nullptr;
    if (handleVerticalScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, std::max(0, capacity - 10), choice.petScroll));
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element34 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element34) : nullptr; element34) element34->visible = 0;
    if (auto* const element7 = window ? window->FindElementById(WindowPetSkillChoiceElementId::Element7) : nullptr; element7) element7->visible = 1;
    cg::text::FormatTo(choice.amountText, L"{:10}", choice.skills[choice.selected].price);
    for (std::int32_t row = 0; row < 5; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement2) indexedElement2->mouseCallback = &WindowPetSkillChoice_SelectPetCandidate;
        auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr;
        if (indexedElement3) {
            indexedElement3->data.text->text.clear();
        }
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 44)) : nullptr; indexedElement4) indexedElement4->visible = 0;
        const bool usedOption = choice.petOptions[row].used != 0;
        choice.firstValueTexts[row].clear();
        choice.secondValueTexts[row].clear();
        if (usedOption) {
            if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement5) indexedElement5->data.text->text = choice.petOptions[row].name;
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement4) indexedElement4->visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(session, window, choice.skillScroll);
    WindowPetSkillChoice_RefreshSelectedSkillRows(session, window, choice.petScroll);
}

// 原函数：0x004BF160。
static void WindowPetSkillChoice_RefreshLearnRows(
    session::ClientSession& session,
    WindowInstance* const window, const std::int32_t scroll, const std::int32_t count)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement4) indexedElement4->visible = 0;
        choice.firstValueTexts[row].clear();
        choice.secondValueTexts[row].clear();
        if (scroll + row < count) {
            if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement5) indexedElement5->visible = 1;
            const world::SkillPairRecord& pair = world::gSkillPairsF63370[scroll + row];
            if (pair.used != 0) {
                const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
                if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement6) indexedElement6->visible = 1;
                if (auto* const indexedElement7 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement7) indexedElement7->visible = 1;
                if (auto* const indexedElement8 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement8) indexedElement8->visible = 1;
                const world::SkillRuntimeRecord& skill = world::gSkillsF1BAFC[index];
                if (auto* const indexedElement9 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement9) indexedElement9->data.text->text = skill.name;
                cg::text::FormatTo(choice.firstValueTexts[row], L"{:2}", skill.level);
                const std::int32_t requiredSlots = std::bit_cast<std::int8_t>(skill.requiredSlots19[0]);
                cg::text::FormatTo(choice.secondValueTexts[row], L"{:2}", requiredSlots);
            }
        }
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
}

// 原函数：0x004BF2E0。
static void WindowPetSkillChoice_RefreshForgetRows(
    session::ClientSession& session,
    WindowInstance* const window, const std::int32_t scroll, const std::int32_t count)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement4) indexedElement4->visible = 0;
        choice.firstValueTexts[row].clear();
        choice.secondValueTexts[row].clear();
        if (scroll + row < count) {
            if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement5) indexedElement5->visible = 1;
            const std::int32_t selected = choice.skillMap[scroll + row];
            if (selected >= 0 && choice.firstValues[selected] >= 0) {
                if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement6) indexedElement6->visible = 1;
                if (auto* const indexedElement7 = window ? window->FindElementById(NativeElementKey(row + 49)) : nullptr; indexedElement7) indexedElement7->visible = 1;
                if (auto* const indexedElement8 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement8) indexedElement8->visible = 1;
                if (auto* const indexedElement9 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement9) indexedElement9->data.text->text = choice.names[selected].text;
                cg::text::FormatTo(choice.firstValueTexts[row], L"{:2}", choice.firstValues[selected]);
                cg::text::FormatTo(choice.secondValueTexts[row], L"{:2}", choice.secondValues[selected]);
            }
        }
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
}

// 原函数：0x004BF460。
static void WindowPetSkillChoice_RefreshPetRows(session::ClientSession& session, WindowInstance* const window, const std::int32_t scroll)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    for (std::int32_t row = 0; row < 5; ++row) {
        const Window54SkillRecord& record = choice.skills[scroll + row];
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 94)) : nullptr; indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 79)) : nullptr; indexedElement2) indexedElement2->visible = 1;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 84)) : nullptr; indexedElement3) indexedElement3->visible = 1;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 89)) : nullptr; indexedElement4) indexedElement4->visible = 1;
        if (auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 99)) : nullptr; indexedElement5) indexedElement5->visible = 1;
        if (choice.mode == 2) {
            if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 99)) : nullptr; indexedElement6) indexedElement6->mouseCallback = &WindowPetSkillChoice_SubmitPetSkill;
        }
        if (choice.mode == 3) {
            if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 99)) : nullptr; indexedElement6) indexedElement6->mouseCallback = &WindowPetSkillChoice_DrawSelectedSkillFrame;
        }
        if (choice.mode == 12) {
            if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 99)) : nullptr; indexedElement6) indexedElement6->mouseCallback = &WindowPetSkillChoice_SubmitPetSkill;
        }
        if (choice.mode == 13) {
            if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 99)) : nullptr; indexedElement6) indexedElement6->mouseCallback = &WindowPetSkillChoice_DrawSelectedSkillFrame;
        }
        auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 79)) : nullptr;
        if (indexedElement6) {
            indexedElement6->data.text->text.clear();
        }
        choice.skillValueTexts[row].clear();
        choice.skillPriceTexts[row].clear();
        // 原存在性检查不加scroll，显示内容仍取分页后的记录。
        if (choice.skills[row].name[0] != '\0') {
            if (auto* const indexedElement7 = window ? window->FindElementById(NativeElementKey(row + 79)) : nullptr; indexedElement7) indexedElement7->data.text->text = record.name;
            cg::text::FormatTo(choice.skillValueTexts[row], L"{:5}", record.value04);
            cg::text::FormatTo(choice.skillPriceTexts[row], L"{:10}", record.price);
            const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
            const std::int32_t price = record.price;
            auto* const indexedElement8 = window ? window->FindElementById(NativeElementKey(row + 89)) : nullptr;
            if (indexedElement8) {
                indexedElement8->data.number->color =
                    money >= price ? 0 : 6;
            }
        }
    }
}

// 原函数：0x004BF620。
static void WindowPetSkillChoice_RefreshSelectedSkillRows(session::ClientSession& session, WindowInstance* const window, const std::int32_t scroll)
{
    auto& choice = session.UiFrame().native.petSkillChoice;
    std::int32_t row = 0;
    const std::int32_t visibleCount = std::min(10, std::min(choice.petOptions[choice.petChoice].slots, world::kPetTechniqueSlotCount) - scroll);
    if (visibleCount > 0) {
        do {
            const Window54TechniqueSlot& slot = choice.techniqueSlots[scroll + row];
            if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement3) indexedElement3->visible = 1;
            if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement4) indexedElement4->mouseCallback = &WindowPetSkillChoice_SelectSkillSlot;
            auto* const indexedElement5 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr;
            if (indexedElement5) {
                indexedElement5->data.text->text.clear();
            }
            const bool used = slot.used != 0;
            choice.firstValueTexts[row].clear();
            choice.secondValueTexts[row].clear();
            if (used) {
                if (auto* const indexedElement6 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement6) indexedElement6->data.text->text = slot.name;
                cg::text::FormatTo(choice.secondValueTexts[row], L"{:3}", slot.value04);
            }
            ++row;
        } while (row < visibleCount);
    }
    for (; row < 10; ++row) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(row + 34)) : nullptr; indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = window ? window->FindElementById(NativeElementKey(row + 59)) : nullptr; indexedElement3) indexedElement3->visible = 0;
        if (auto* const indexedElement4 = window ? window->FindElementById(NativeElementKey(row + 69)) : nullptr; indexedElement4) indexedElement4->mouseCallback = &WindowElement_NoOp;
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
}

// 原函数：0x004BF7C0。
static std::int32_t WindowPetSkillChoice_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        // 数字原型只留样式，实例借所属会话的稳定文本。
        if (auto* const element = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element7); element && element->data.number) {
            element->data.number->text = &choice.amountText;
        }
        if (auto* const element = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element8); element && element->data.number) {
            element->data.number->text = &choice.moneyText;
        }
        for (std::int32_t row = 0; row < 10; ++row) {
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(49 + row)); element && element->data.number) {
                element->data.number->text = &choice.firstValueTexts[row];
            }
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(59 + row)); element && element->data.number) {
                element->data.number->text = &choice.secondValueTexts[row];
            }
        }
        for (std::int32_t row = 0; row < 5; ++row) {
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(84 + row)); element && element->data.number) {
                element->data.number->text = &choice.skillValueTexts[row];
            }
            if (auto* const element = currentWindow->FindElementById(NativeElementKey(89 + row)); element && element->data.number) {
                element->data.number->text = &choice.skillPriceTexts[row];
            }
        }
        if (auto* const handleConfirmationElement = currentWindow->FindElementById(WindowPetSkillChoiceElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 0;
        if (auto* const element5 = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element5); element5) element5->data.text->text = npcLines[0].text;
        auto* const element9 = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element9);
        if (element9) {
            auto* animation = element9->data.animation;
            cg::yobi::SetYobiActionField2A8(animation->action, choice.imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
        choice.petScroll = 0;
        const auto mode = std::bit_cast<std::int8_t>(choice.mode);
        choice.skillScroll = 0;
        switch (mode) {
        case 0:
            WindowPetSkillChoice_RefreshLearn(*callbackWindow.ownerSession, currentWindow);
            break;
        case 1:
            WindowPetSkillChoice_RefreshForget(*callbackWindow.ownerSession, currentWindow);
            break;
        case 2:
        case 12:
            WindowPetSkillChoice_RefreshPetNames(*callbackWindow.ownerSession, currentWindow);
            break;
        default:
            break;
        }
        choice.message = -1;
        choice.hover = -1;
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    const std::int32_t dialogType = npcDialog.type;
    if (dialogType != -2 && choice.dialogType != dialogType) {
        currentWindow->state |= 0x8000u;
    }
    cg::text::FormatTo(choice.moneyText, L"{:10}", world::gPlayerRuntimeSlots.valueFC);
    if (choice.message >= 0) {
        choice.message = -1;
    }
    return 1;
}

// 原函数：0x004BF950。
static std::int32_t WindowPetSkillChoice_Draw(WindowInstance& callbackWindow, const std::int32_t unused)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& npcLines = callbackWindow.ownerSession->UiFrame().native.npcDialogLines;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unused);
    WindowInstance* window = currentWindow;
    const auto mode = std::bit_cast<std::int8_t>(choice.mode);
    const std::int32_t x = window->x + 297;
    std::int32_t y = window->y + 52;
    switch (mode) {
    case 0:
    case 1: {
        if (choice.firstCount - 10 <= 0) {
            if (auto* const element10 = window->FindElementById(WindowPetSkillChoiceElementId::Element10); element10) element10->visible = 0;
            window = currentWindow;
        }
        auto* elementsWindow = window;
        auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
                auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll);
                if (handleVerticalScrollElement2) {
                    const std::int32_t position = WindowScroll_ReadPosition(handleVerticalScrollElement2, choice.firstCount - 10);
                    const std::uint8_t currentMode = choice.mode;
                    WindowInstance* const current = currentWindow;
                    choice.petScroll = position;
                    const std::int32_t count = choice.firstCount;
                    if (currentMode == 0) {
                        WindowPetSkillChoice_RefreshLearnRows(*callbackWindow.ownerSession, current, position, count);
                    } else {
                        WindowPetSkillChoice_RefreshForgetRows(*callbackWindow.ownerSession, current, position, count);
                    }
                }
            }
        }
        break;
    }
    case 2:
    case 3:
    case 4:
    case 12:
    case 13: {
        if (auto* const element10 = window->FindElementById(WindowPetSkillChoiceElementId::Element10); element10) element10->visible = 1;
        if (choice.firstCount - 10 <= 0) {
            if (auto* const element102 = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element10); element102) element102->visible = 0;
        }
        auto* elementsWindow = currentWindow;
        auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll);
        if (handleVerticalScrollElement) {
            if ((handleVerticalScrollElement->data.mouse->flags & 1u) != 0) {
                auto* const handleVerticalScrollElement2 = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll);
                if (handleVerticalScrollElement2) {
                    const std::int32_t position = WindowScroll_ReadPosition(handleVerticalScrollElement2, choice.firstCount - 10);
                    WindowInstance* const current = currentWindow;
                    choice.petScroll = position;
                    WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, current, position);
                }
            }
        }
        if (auto* const element18 = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element18); element18) element18->visible = 1;
        if (choice.secondCount - 5 <= 0) {
            if (auto* const element182 = currentWindow->FindElementById(WindowPetSkillChoiceElementId::Element18); element182) element182->visible = 0;
        }
        elementsWindow = currentWindow;
        auto* const handleVerticalScroll_19Element = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll_19);
        if (handleVerticalScroll_19Element) {
            if ((handleVerticalScroll_19Element->data.mouse->flags & 1u) != 0) {
                auto* const handleVerticalScroll_19Element2 = elementsWindow->FindElementById(WindowPetSkillChoiceElementId::HandleVerticalScroll_19);
                if (handleVerticalScroll_19Element2) {
                    const std::int32_t position = WindowScroll_ReadPosition(handleVerticalScroll_19Element2, choice.secondCount - 5);
                    choice.skillScroll = position;
                    WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, position);
                }
            }
        }
        break;
    }
    default:
        break;
    }
    std::int32_t message = choice.message;
    gTooltipColorD67D30 = 0;
    if (message >= 0) {
        for (std::int32_t row = 0; row < 2; ++row) {
            if (row != 0) {
                message = choice.message;
            }
            static_cast<void>(WindowItem_SplitTooltipLines(
                x, y, 3, 8, 0, choice.skills[message].description[row], 0, 0));
            y += 10;
        }
    } else {
        const WindowDialogTextLine* line = npcLines.data() + 1 + 4 * (-1 - message);
        for (std::int32_t row = 0; row < 4; ++row) {
            if (!line->text.empty()) {
                static_cast<void>(WindowItem_SplitTooltipLines(x, y, 3, 8, 0, line->text.data(), 0, 0));
            }
            ++line;
            y += 10;
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004BFBC0。
static std::int32_t WindowPetSkillChoice_ButtonLearn(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = L"学习此技能。";
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (choice.mode == 0) {
                std::wstring escaped;
                escaped = input::EscapeTextAndNotify(L"1", 15);
                const std::int32_t target = npcDialog.target;
                const std::int32_t request = npcDialog.request;
                const std::int32_t y =
                    static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x =
                    static_cast<std::int32_t>(world::gPlayerX);
                static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
                currentWindow->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(79, 320));
            }
            consumed = 1;
        }
        graphic->imageNo = 243048;
        if (hover) {
            graphic->imageNo = 243050;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243049;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004BFCB0。
static std::int32_t WindowPetSkillChoice_ButtonBack(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    auto mode = std::bit_cast<std::int8_t>(choice.mode);
    const bool hover = (state & MouseFlag_Hover) != 0;
    if (hover) {
        switch (mode) {
        case 0:
        case 1:
        case 2:
        case 12:
            uiFrame.hoverText = L"还是算了。";
            break;
        case 3:
        case 4:
        case 13:
            uiFrame.hoverText = L"回前一页视窗";
            break;
        default:
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        switch (mode) {
        case 0: {
            std::wstring escaped;
            escaped = input::EscapeTextAndNotify(L"2", 15);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
            currentWindow->state |= 0x8000u;
            break;
        }
        case 1: {
            std::wstring text;
            std::wstring escaped;
            cg::text::FormatTo(text, L"{}", 99);
            escaped = input::EscapeTextAndNotify(text, 15);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
            currentWindow->state |= 0x8000u;
            break;
        }
        case 2:
        case 12:
            window->state |= 0x8000u;
            break;
        case 3:
        case 4:
        case 13: {
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::wstring text = L"";
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 2, text));
            npcDialog.type = -2;
            break;
        }
        default:
            break;
        }
        choice.lifecycle = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        mode = std::bit_cast<std::int8_t>(choice.mode);
        consumed = 1;
    }
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 12:
        if (graphic) graphic->imageNo = 245320;
        if (hover) {
            if (graphic) graphic->imageNo = 245322;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (graphic) graphic->imageNo = 245321;
        }
        break;
    case 3:
    case 4:
    case 13:
        if (graphic) graphic->imageNo = 245332;
        if (hover) {
            if (graphic) graphic->imageNo = 245334;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (graphic) graphic->imageNo = 245333;
        }
        break;
    default:
        break;
    }
    return consumed;
}

// 原函数：0x004BFF60。
static std::int32_t WindowPetSkillChoice_ButtonPreviousRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        switch (std::bit_cast<std::int8_t>(choice.mode)) {
        case 0:
        case 1: {
            if (choice.petScroll > 0) {
                --choice.petScroll;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = choice.petScroll;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = choice.firstCount - 10;
            auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            const std::uint8_t mode = choice.mode;
            const std::int32_t count = choice.firstCount;
            WindowInstance* const current = currentWindow;
            const std::int32_t scroll = choice.petScroll;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(*callbackWindow.ownerSession, current, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(*callbackWindow.ownerSession, current, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (elementId <= static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)) {
                if (choice.petScroll > 0) {
                    --choice.petScroll;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = choice.petScroll;
                WindowInstance* const window = currentWindow;
                const std::int32_t maximum = choice.firstCount - 10;
                auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.petScroll;
                WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, currentWindow, scroll);
            } else {
                if (choice.skillScroll > 0) {
                    --choice.skillScroll;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = choice.skillScroll;
                WindowInstance* const window = currentWindow;
                const std::int32_t maximum = choice.secondCount - 5;
                auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.skillScroll;
                WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, scroll);
            }
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

// 原函数：0x004C0160。
static std::int32_t WindowPetSkillChoice_ButtonNextRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LRepeatDown) != 0) {
        switch (std::bit_cast<std::int8_t>(choice.mode)) {
        case 0:
        case 1: {
            const std::int32_t limit = choice.firstCount - 10;
            if (choice.petScroll < limit) {
                ++choice.petScroll;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = choice.petScroll;
            const std::int32_t maximum = choice.firstCount - 10;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 2));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            const std::uint8_t mode = choice.mode;
            const std::int32_t scroll = choice.petScroll;
            WindowInstance* const window = currentWindow;
            const std::int32_t count = choice.firstCount;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(*callbackWindow.ownerSession, window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(*callbackWindow.ownerSession, window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
            if (elementId > static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)) {
                const std::int32_t limit = choice.secondCount - 5;
                if (choice.skillScroll < limit) {
                    ++choice.skillScroll;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = choice.skillScroll;
                const std::int32_t maximum = choice.secondCount - 5;
                auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 2));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.skillScroll;
                WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, scroll);
                break;
            }
            [[fallthrough]];
        case 12:
        case 13: {
            const std::int32_t limit = choice.firstCount - 10;
            if (choice.petScroll < limit) {
                ++choice.petScroll;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = choice.petScroll;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = choice.firstCount - 10;
            // 原4C0314为index-1；模式12/13也不按右侧元素改走另一分支。
            auto* const indexedElement = window->FindElementById(NativeElementKey(index - 1));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            const std::int32_t scroll = choice.petScroll;
            WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, currentWindow, scroll);
            break;
        }
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

// 原函数：0x004C0380。
static std::int32_t WindowPetSkillChoice_ButtonPreviousPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        switch (std::bit_cast<std::int8_t>(choice.mode)) {
        case 0:
        case 1: {
            static_cast<void>(audio::PlaySe_52C450(choice.petScroll > 0 ? 51 : 55, 320));
            choice.petScroll -= 10;
            if (choice.petScroll < 0) {
                choice.petScroll = 0;
            }
            const std::int32_t position = choice.petScroll;
            const std::int32_t maximum = choice.firstCount - 10;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            const std::uint8_t mode = choice.mode;
            const std::int32_t count = choice.firstCount;
            WindowInstance* const window = currentWindow;
            const std::int32_t scroll = choice.petScroll;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(*callbackWindow.ownerSession, window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(*callbackWindow.ownerSession, window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (elementId <= static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)) {
                static_cast<void>(audio::PlaySe_52C450(choice.petScroll > 0 ? 51 : 55, 320));
                choice.petScroll -= 10;
                if (choice.petScroll < 0) {
                    choice.petScroll = 0;
                }
                const std::int32_t position = choice.petScroll;
                const std::int32_t maximum = choice.firstCount - 10;
                auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.petScroll;
                WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, currentWindow, scroll);
            } else {
                static_cast<void>(audio::PlaySe_52C450(choice.skillScroll > 0 ? 51 : 55, 320));
                choice.skillScroll -= 5;
                if (choice.skillScroll < 0) {
                    choice.skillScroll = 0;
                }
                const std::int32_t position = choice.skillScroll;
                const std::int32_t maximum = choice.secondCount - 5;
                auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 3));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.skillScroll;
                WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, scroll);
            }
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

// 原函数：0x004C0590。
static std::int32_t WindowPetSkillChoice_ButtonNextPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        switch (std::bit_cast<std::int8_t>(choice.mode)) {
        case 0:
        case 1: {
            const std::int32_t limit = choice.firstCount - 10;
            static_cast<void>(audio::PlaySe_52C450(
                limit > 0 && choice.petScroll < limit ? 51 : 55, 320));
            std::int32_t position = choice.petScroll;
            const std::int32_t maximum = choice.firstCount - 10;
            position += 10;
            choice.petScroll = position;
            if (position > maximum) {
                position = maximum;
                choice.petScroll = position;
            }
            if (position < 0) {
                position = 0;
                choice.petScroll = position;
            }
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
            if (indexedElement) {
                static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
            }
            const std::uint8_t mode = choice.mode;
            const std::int32_t scroll = choice.petScroll;
            WindowInstance* const window = currentWindow;
            const std::int32_t count = choice.firstCount;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(*callbackWindow.ownerSession, window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(*callbackWindow.ownerSession, window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (elementId <= static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)) {
                const std::int32_t limit = choice.firstCount - 10;
                static_cast<void>(audio::PlaySe_52C450(
                    limit > 0 && choice.petScroll < limit ? 51 : 55, 320));
                std::int32_t position = choice.petScroll;
                const std::int32_t maximum = choice.firstCount - 10;
                position += 10;
                choice.petScroll = position;
                if (position > maximum) {
                    position = maximum;
                    choice.petScroll = position;
                }
                if (position < 0) {
                    position = 0;
                    choice.petScroll = position;
                }
                auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.petScroll;
                WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, currentWindow, scroll);
            } else {
                const std::int32_t limit = choice.secondCount - 5;
                static_cast<void>(audio::PlaySe_52C450(
                    limit > 0 && choice.skillScroll < limit ? 51 : 55, 320));
                std::int32_t position = choice.skillScroll;
                const std::int32_t maximum = choice.secondCount - 5;
                position += 5;
                choice.skillScroll = position;
                if (position > maximum) {
                    position = maximum;
                    choice.skillScroll = position;
                }
                if (position < 0) {
                    position = 0;
                    choice.skillScroll = position;
                }
                auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 4));
                if (indexedElement) {
                    static_cast<void>(WindowScroll_SetPosition(indexedElement, maximum, position));
                }
                const std::int32_t scroll = choice.skillScroll;
                WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, scroll);
            }
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

// 原函数：0x004C0800。
static std::int32_t WindowPetSkillChoice_Scroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        switch (std::bit_cast<std::int8_t>(choice.mode)) {
        case 0:
        case 1: {
            const std::int32_t count = choice.firstCount;
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            const std::int32_t position = choice.petScroll;
            auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 5));
            if (indexedElement) {
                choice.petScroll =
                    WindowScroll_ApplyWheel(indexedElement, position, count - 10, wheel);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13: {
            const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
            if (elementId <= static_cast<ElementId>(WindowPetSkillChoiceElementId::Element18)) {
                const std::int32_t count = choice.firstCount;
                const std::int32_t position = choice.petScroll;
                auto* const scrollElement = currentWindow->FindElementById(NativeElementKey(index - 5));
                if (scrollElement) {
                    const std::int32_t scroll =
                        WindowScroll_ApplyWheel(scrollElement, position, count - 10, wheel);
                    choice.petScroll = scroll;
                    WindowPetSkillChoice_RefreshSelectedSkillRows(*callbackWindow.ownerSession, currentWindow, scroll);
                }
            } else {
                const std::int32_t count = choice.secondCount;
                const std::int32_t position = choice.skillScroll;
                auto* const scrollElement = currentWindow->FindElementById(NativeElementKey(index - 5));
                if (scrollElement) {
                    const std::int32_t scroll =
                        WindowScroll_ApplyWheel(scrollElement, position, count - 5, wheel);
                    choice.skillScroll = scroll;
                    WindowPetSkillChoice_RefreshPetRows(*callbackWindow.ownerSession, currentWindow, scroll);
                }
            }
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

// 原函数：0x004C0910。
static std::int32_t WindowPetSkillChoice_SubmitForget(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t selected = choice.skillMap[choice.petScroll + index - 69];
        if (selected < 0 || choice.firstValues[selected] < 0) {
            graphic->imageNo = 243104;
            return 0;
        }
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = L"遗忘此技能。";
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            std::wstring text;
            std::wstring escaped;
            cg::text::FormatTo(text, L"{}", selected + 1);
            escaped = input::EscapeTextAndNotify(text, 15);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
            currentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = 243103;
        if (hover) {
            graphic->imageNo = 243105;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243104;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C0A50。
static std::int32_t WindowPetSkillChoice_SelectPetCandidate(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t selected = choice.petMap[choice.petScroll + index - 69];
        if (selected == 0) {
            choice.message = -1;
        }
        // 原4C0A97：候选-1先读D67420的U16，别名为商店价格文本D673EC+34。
        std::uint16_t used;
        if (selected == -1) {
            used = shop.priceTail;
        } else {
            used = choice.petOptions[selected].used;
        }
        if (used == 0) {
            graphic->imageNo = 243101;
            return 0;
        }
        const Window54PetOption& option = choice.petOptions[selected];
        if (option.allowed == 0) {
            graphic->imageNo = 243101;
            return 0;
        }
        const bool hover = (state & MouseFlag_Hover) != 0;
        const std::int32_t requiredTribe = choice.tribe;
        if (requiredTribe >= 0 && option.tribe != requiredTribe) {
            if (hover) {
                choice.message = -2;
            }
            graphic->imageNo = 243101;
            return 0;
        }
        // 此标记只改变悬停消息，原端仍允许下面的点击发送。
        if (option.hintFlag == 0 && hover) {
            choice.message = -3;
        }
        if (hover) {
            uiFrame.hoverText = L"让宠物学习此技能。";
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            choice.petChoice = selected;
            std::wstring text;
            std::wstring escaped;
            cg::text::FormatTo(text, L"{}", selected);
            escaped = input::EscapeTextAndNotify(text, 15);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
            npcDialog.type = -2;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = 243100;
        if (hover) {
            graphic->imageNo = 243102;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 243101;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004C0BF0。
static std::int32_t WindowPetSkillChoice_SelectSkillSlot(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    const bool hover = (state & MouseFlag_Hover) != 0;
    auto* const graphicElement = window->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const std::int32_t slot = choice.petScroll + index - 69;
    if (hover) {
        uiFrame.hoverText = L"把要学习的技能放在这一栏。";
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        choice.replacement = slot;
        if (auto* const handleConfirmationElement = window->FindElementById(WindowPetSkillChoiceElementId::HandleConfirmation); handleConfirmationElement) handleConfirmationElement->visible = 1;
        currentWindow->state |= 2u;
        static_cast<void>(audio::PlaySe_52C450(78, 320));
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 243127;
    if (hover) {
        if (graphic) graphic->imageNo = 243129;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 243128;
    }
    return consumed;
}

// 原函数：0x004C0CB0。
static std::int32_t WindowPetSkillChoice_SubmitPetSkill(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t selected = choice.skillScroll + index - 99;
    const Window54SkillRecord& record = choice.skills[selected];
    if (record.name[0] == '\0') {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        WindowInstance* const window = currentWindow;
        const std::int32_t hoverImage = cg::render::gHoverFramePaletteIndex_D69768;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + element.width),
                static_cast<std::uint16_t>(y + element.height), 223, hoverImage, 0);
        }
        if (choice.message >= -1 || choice.hover != selected) {
            choice.message = selected;
        }
        choice.hover = selected;
        uiFrame.hoverText = L"把此技能让宠物学习。";
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
    if (money < record.price) {
        choice.message = -2;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return 1;
    }
    std::int32_t petCount = 0;
    for (const world::PetRuntimeRecord& pet : world::gPetRuntimeRecords) {
        if (pet.used != 0) {
            ++petCount;
        }
    }
    if (petCount == 0) {
        choice.message = -3;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return 1;
    }
    choice.selected = selected;
    std::wstring text;
    std::wstring escaped;
    cg::text::FormatTo(text, L"{}", selected);
    escaped = input::EscapeTextAndNotify(text, 15);
    const std::int32_t target = npcDialog.target;
    const std::int32_t request = npcDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, escaped));
    npcDialog.type = -2;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// 原函数：0x004C0EA0。
static std::int32_t WindowPetSkillChoice_DrawSelectedSkillFrame(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t unusedState)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unusedState);
    const std::int32_t selected = choice.skillScroll + index - 99;
    if (choice.skills[selected].name[0] != '\0' && choice.selected == selected) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = window->x + element.x;
            const std::int32_t y = window->y + element.y;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
                static_cast<std::uint16_t>(x + element.width - 2),
                static_cast<std::uint16_t>(y + element.height - 2), 223, 254, 0);
        }
    }
    return 0;
}

// 原函数：0x004C0F20。
static std::int32_t WindowPetSkillChoice_HandleConfirmation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t unusedState)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.petSkillChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& shop = callbackWindow.ownerSession->UiFrame().native.shop;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    static_cast<void>(unusedState);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const bool acceptHover = cg::ui::HitTestMouseRect_5049A0(240, 310, 306, 327, -1) != 0;
    const bool cancelHover = cg::ui::HitTestMouseRect_5049A0(334, 310, 400, 327, -1) != 0;
    std::int32_t acceptImage = 245323;
    if (acceptHover) {
        acceptImage = 245325;
        const auto buttons = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
        uiFrame.hoverText = shop.confirmationAcceptHint;
        if ((buttons & platform::LegacyMouseEdge_LDown) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            std::wstring requestText;
            cg::text::FormatTo(requestText, L"{}", choice.replacement);
            const std::int32_t target = npcDialog.target;
            const std::int32_t request = npcDialog.request;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(callbackWindow.ownerSession->Connection().SendDialogResponse(x, y, request, target, 0, requestText));
            currentWindow->state &= ~2u;
            acceptImage = 245324;
            currentWindow->state |= 0x8000u;
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
            cancelImage = 245327;
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            currentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(240, 310, 226, acceptImage, 0, 1, 0, &descriptor));
    static_cast<void>(render::DrawGraphic3_520970(334, 310, 226, cancelImage, 0, 1, 0, &descriptor));
    constexpr wchar_t learnSuffix5882E8[] = L"来学习。";
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"学习『{}』" /* 原常量参考：0x00588344。 */, choice.skills[choice.selected].name.c_str());
    std::int32_t width = cg::render::TextMeasure(buffer, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 130, 4, 0, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"让『{}』" /* 原常量参考：0x00588338。 */, choice.petOptions[choice.petChoice].name.c_str());
    // 原第二行按固定16位示例名测宽，不按实际候选名测量。
    width = cg::render::TextMeasure(L"让『1234567890123456』" /* 原常量参考：0x00588320。 */, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 154, 4, 0, 0, buffer, 0, 0));
    const Window54TechniqueSlot& slot = choice.techniqueSlots[choice.replacement];
    if (slot.used != 0) {
        cg::text::FormatTo(buffer, L"使用『{}』的位置" /* 原常量参考：0x0058830C。 */, slot.name.c_str());
    } else {
        buffer = L"使用『空栏位』的位置" /* 原常量参考：0x005882F4。 */;
    }
    width = cg::render::TextMeasure(buffer, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 178, 4, 0, 0, buffer, 0, 0));
    width = cg::render::TextMeasure(learnSuffix5882E8, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 198, 4, 0, 0, learnSuffix5882E8, 0, 0));
    cg::text::FormatTo(buffer, L"所持金　 {:7}金币" /* 原常量参考：0x005882D4。 */, world::gPlayerRuntimeSlots.valueFC);
    static_cast<void>(render::DrawText_456780(245, 230, 4, 0, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"购入金额 {:7}金币" /* 原常量参考：0x005882C0。 */, choice.skills[choice.selected].price);
    static_cast<void>(render::DrawText_456780(245, 250, 4, 0, 0, buffer, 0, 0));
    static_cast<void>(render::DrawText_456780(252, 282, 4, 0, 0, L"这样可以吗？" /* 原常量参考：0x005882B0。 */, 0, 0));
    cg::yobi::DrawNineGrid_49BE00(
        245300, 160, 110, 320, 240, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

} // namespace detail

} // namespace cg::ui
