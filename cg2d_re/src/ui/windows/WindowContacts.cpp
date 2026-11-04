#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../../scene/login/LoginSavedState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowContactsElementId : ElementId {
    ButtonGuildDetails = 1,
    HandleNavigation = 2,
    HandleNavigation_3 = 3,
    HandleNavigation_4 = 4,
    HandleNavigation_5 = 5,
    HandleNavigation_6 = 6,
    HandleNavigation_7 = 7,
    Element8 = 8,
    Element9 = 9,
    ButtonClose = 10,
    TabCategory = 11,
    TabCategory_12 = 12,
    TabCategory_13 = 13,
    Element14 = 14,
    ScrollRow = 15,
    ScrollRow_16 = 16,
    Element17 = 17,
    HandleVerticalScroll = 18,
    Scroll = 19,
    ButtonGroupMail = 20,
    Element21 = 21,
    HandleRankButton = 22,
    HandleRankButton_23 = 23,
    HandleRankButton_24 = 24,
    HandleRankButton_25 = 25,
    HandleRankButton_26 = 26,
    HandleRankButton_27 = 27,
    OpenMail = 28,
    ButtonCompose = 29,
    ButtonDelete = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    OpenMail_36 = 36,
    ButtonCompose_37 = 37,
    ButtonDelete_38 = 38,
    Element39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
    OpenMail_44 = 44,
    ButtonCompose_45 = 45,
    ButtonDelete_46 = 46,
    Element47 = 47,
    Element48 = 48,
    Element49 = 49,
    Element50 = 50,
    Element51 = 51,
    HandleNavigation_52 = 52,
    HandleNavigation_53 = 53,
    HandleNavigation_54 = 54,
    OpenMail_55 = 55,
    ButtonCompose_56 = 56,
    ButtonDelete_57 = 57,
    Element58 = 58,
    Element59 = 59,
    OpenMail_60 = 60,
    ButtonCompose_61 = 61,
    ButtonDelete_62 = 62,
    Element63 = 63,
    Element64 = 64,
    OpenMail_65 = 65,
    ButtonCompose_66 = 66,
    ButtonDelete_67 = 67,
    Element68 = 68,
    Element69 = 69,
    OpenMail_70 = 70,
    ButtonCompose_71 = 71,
    ButtonDelete_72 = 72,
    Element73 = 73,
    Element74 = 74,
    OpenMail_75 = 75,
    ButtonCompose_76 = 76,
    ButtonDelete_77 = 77,
    Element78 = 78,
    Element79 = 79,
    OpenMail_80 = 80,
    ButtonCompose_81 = 81,
    ButtonDelete_82 = 82,
    Element83 = 83,
    Element84 = 84,
    OpenMail_85 = 85,
    ButtonCompose_86 = 86,
    ButtonDelete_87 = 87,
    Element88 = 88,
    Element89 = 89,
    OpenMail_90 = 90,
    ButtonCompose_91 = 91,
    ButtonDelete_92 = 92,
    Element93 = 93,
    Element94 = 94,
    OpenMail_95 = 95,
    ButtonCompose_96 = 96,
    ButtonDelete_97 = 97,
    Element98 = 98,
    Element99 = 99,
    HandleAvatar = 100,
    HandleAvatar_101 = 101,
    HandleAvatar_102 = 102,
    HandleAvatar_103 = 103,
    HandleAvatar_104 = 104,
    HandleAvatar_105 = 105,
    HandleAvatar_106 = 106,
    HandleAvatar_107 = 107,
    HandleAvatar_108 = 108,
    Element109 = 109,
    Element110 = 110,
    HandleSideArea = 111,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口21布局、未读扫描与滚动条初始化。

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(WindowInstance& callbackWindow, std::int32_t value); // 完整滚动/闪烁/内容刷新后绘制窗口21。

// 原函数：0x004A09E0。
static std::int32_t WindowContacts_Refresh(WindowInstance* callbackWindow); // 三行/九行联系人完整内容刷新，不调用公共Draw。

// 原函数：0x004A1690。
static void WindowContacts_SetDisplayMode(WindowInstance* callbackWindow, std::int32_t mode, std::int32_t category); // 显示模式后接名单类别布局。

// 原函数：0x004A16B0。
static void WindowContacts_ApplyDisplayLayout(WindowInstance* callbackWindow, std::int32_t mode); // 联系人三行/九行显隐，不触碰末三元素。

// 原函数：0x004A1780。
static void WindowContacts_ApplyCategoryLayout(WindowInstance* callbackWindow, std::int32_t category); // 普通/公会/迷你名单布局，借原型坐标。

// 原函数：0x004A1C30。
static std::int32_t WindowContacts_ScrollRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 单行滚动、边界音效及保存。

// 原函数：0x004A1EF0。
static std::int32_t WindowContacts_HandleNavigation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 显示/排序/翻页及三行记录TLKH发送。

// 原函数：0x004A2610。
static std::int32_t WindowContacts_ButtonGroupMail(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 群发入口：检查49，动画打开22。

// 原函数：0x004A2870。
static std::int32_t WindowContacts_TabCategory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 普通/公会/迷你类别切换。

// 原函数：0x004A2BD0。
static std::int32_t WindowContacts_ButtonGuildDetails(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 动画打开窗口48公会详情。

// 原函数：0x004A37E0。
static std::int32_t WindowContacts_ResolveRecordIndex(session::ClientSession& session, std::int32_t index); // 三行/九行元素编号到同源排序后记录索引。

// 原函数：0x004A40C0。
static std::int32_t WindowContacts_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 滚轮推进，重读类别写回，恒返回0。

// 原函数：0x004A2530。
static std::int32_t WindowContacts_HandleRankButton(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六职级按钮，bit400转交完整4A38C0。

// 原函数：0x004A38C0。
static void WindowContacts_OpenRankSelection(session::ClientSession& session, std::int32_t index); // 两次联系人索引读取，生产职级状态后直接打开55。

// 原函数：0x004A2B00。
static std::int32_t WindowContacts_OpenMail(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 发布同源记录/两类别槽，动画打开40。

// 原函数：0x004A3780。
static std::int32_t WindowContacts_ResolveButtonRecord(session::ClientSession& session, std::int32_t index); // 三行/九行分组按钮到实际记录索引。

// 原函数：0x004A39C0。
static std::int32_t WindowContacts_HandleAvatar(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 九行头像悬停及独立TLKH双击发送。

// 原函数：0x004A3C70。
static std::int32_t WindowContacts_ButtonCompose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 十二写信按钮，发布类别/索引后动画打开22。

// 原函数：0x004A32E0。
static std::int32_t WindowContacts_ButtonDelete(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 三类联系人删除确认分派。

// 原函数：0x004A33C0。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteContact(session::ClientSession& session, std::int32_t index); // 普通联系人删除确认，窗口47共享上下文。

// 原函数：0x004A3480。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteGuildMember(session::ClientSession& session, std::int32_t index); // 公会成员除名确认。

// 原函数：0x004A3540。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteMiniMail(session::ClientSession& session, std::int32_t index); // 迷你邮件联系人删除确认。

// 原函数：0x004A2700。
static void WindowContacts_ClampScroll(WindowInstance* callbackWindow); // 修正当前名单位置并更新元素17滚动条。

// 原函数：0x004A3820。
static std::int32_t WindowContacts_DragScroll(WindowInstance* callbackWindow, std::int32_t unused); // 滚动条反求位置；调用后重读类别写回。

// 原函数：0x004A2C60。
static void WindowContacts_Sort(session::ClientSession& session); // 填200项C9FCE4后按类别依原CRT次序排序60/200/20项。

// 原函数：0x004A2CC0。
static std::int32_t WindowContacts_CompareRecords(session::ClientSession& session, const std::int32_t* left, const std::int32_t* right); // MC01-C8标准索引比较，有效性对称，同键返回0。

// 原函数：0x004A3230。
static std::int32_t WindowContacts_CompareMiniMailNumber(
    const world::detail::MiniMailContactRecord* left,
    const world::detail::MiniMailContactRecord* right); // 迷你序号按S16比较，零不替换。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr std::array<std::int32_t, 15> kMailSortImages54F4C0 { // 原常量参考：0x0054F4C0。
    0x3B60C, 0x3B60D, 0x3B60F, 0x3B610, 0x3B613,
    0x3B60C, 0x3B60E, 0x3B60F, 0x3B610, 0x3B613,
    0x3BAA6, 0x3BA60, 0x3BAA7, 0x3BA61, 0x3BAA8,
};

static constexpr std::array<std::array<std::int32_t, 2>, 3> kMailImageAnchors56F790 {{ // 原常量参考：0x0056F790。
    {62, 115}, {62, 202}, {62, 289},
}};

static WindowGraphicData gWindowGraphic56D8B0 {0x3B612, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D8B0。

static WindowGraphicData gWindowGraphic56D8E0 {0x3B60C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D8E0。

static WindowGraphicData gWindowGraphic56D940 {0x3B544, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D940。

static WindowGraphicData gWindowGraphic56D958 {0x3B547, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D958。

static WindowGraphicData gWindowGraphic56D970 {0x3B541, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D970。

static WindowGraphicData gWindowGraphic56D988 {0x3B54A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D988。

static WindowGraphicData gWindowGraphic56D9A0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9A0。

static WindowGraphicData gWindowGraphic56D9B8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9B8。

static WindowGraphicData gWindowGraphic56D9D0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9D0。

static WindowGraphicData gWindowGraphic56D9E8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9E8。

static WindowGraphicData gWindowGraphic56DA00 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA00。

static WindowGraphicData gWindowGraphic56DA18 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA18。

static WindowGraphicData gWindowGraphic56DA30 {0x3B57D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA30。

static WindowGraphicData gWindowGraphic56DA48 {0x3B580, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA48。

static WindowGraphicData gWindowGraphic56DA60 {0x3B9E0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA60。

static WindowGraphicData gWindowGraphic56DA78 {0x3B5C6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA78。

static WindowGraphicData gWindowGraphic56DA90 {0x3B9E5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA90。

static WindowGraphicData gWindowGraphic56DAA8 {0x3B9DE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAA8。

static WindowGraphicData gWindowGraphic56DAC0 {0x3B9DF, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056DAC0。

static WindowGraphicData gWindowGraphic56DAD8 {0x3C115, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAD8。

static WindowGraphicData gWindowGraphic56DAF0 {0x3C114, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAF0。

static WindowTextPrototype gWindowText56DB08 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"name"}; // 原全局：0x0056DB08。 // 原常量参考：0x00585F14。

static WindowTextPrototype gWindowText56DB10 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"level"}; // 原全局：0x0056DB10。 // 原常量参考：0x00585F0C。

static WindowTextPrototype gWindowText56DB18 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guild"}; // 原全局：0x0056DB18。 // 原常量参考：0x00585F04。

static WindowTextPrototype gWindowText56DB20 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"title"}; // 原全局：0x0056DB20。 // 原常量参考：0x00585EFC。

static WindowTextPrototype gWindowText56DB28 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"server"}; // 原全局：0x0056DB28。 // 原常量参考：0x00585EF4。

static WindowTextPrototype gWindowText56DB30 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guildserver"}; // 原全局：0x0056DB30。 // 原常量参考：0x00585EE8。

static std::uint32_t gWindowMouseFlagsD67840 = 0; // 原全局：0x00D67840。

// 原全局：0x0056DB38。 窗口21完整元素及原型坐标唯一来源。
static WindowElementPrototype gWindowElements56DB38[111] { // 原全局：0x0056DB38。
    {WindowElementType_Graphic, 259, 45, 41, 17, 0, {.graphic = &gWindowGraphic56DA90}, &WindowContacts_ButtonGuildDetails, -1, static_cast<ElementId>(WindowContactsElementId::ButtonGuildDetails)},
    {WindowElementType_Graphic, 19, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation)},
    {WindowElementType_Graphic, 124, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_3)},
    {WindowElementType_Graphic, 19, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_4)},
    {WindowElementType_Graphic, 124, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_5)},
    {WindowElementType_Graphic, 19, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_6)},
    {WindowElementType_Graphic, 42, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_7)},
    {WindowElementType_Graphic, 42, 33, 0, 0, 1, {.graphic = &gWindowGraphic56D8B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element8)},
    {WindowElementType_Graphic, 42, 52, 0, 0, 1, {.graphic = &gWindowGraphic56D8E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element9)},
    {WindowElementType_Graphic, 310, 9, 11, 11, 1, {.graphic = &gWindowGraphic56D9A0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonClose)},
    {WindowElementType_Graphic, 218, 353, 39, 24, 1, {.graphic = &gWindowGraphic56DA30}, &WindowContacts_TabCategory, -1, static_cast<ElementId>(WindowContactsElementId::TabCategory)},
    {WindowElementType_Graphic, 271, 352, 38, 25, 1, {.graphic = &gWindowGraphic56DA48}, &WindowContacts_TabCategory, -1, static_cast<ElementId>(WindowContactsElementId::TabCategory_12)},
    {WindowElementType_Graphic, 143, 349, 60, 25, 1, {.graphic = &gWindowGraphic56DAD8}, &WindowContacts_TabCategory, -1, static_cast<ElementId>(WindowContactsElementId::TabCategory_13)},
    {WindowElementType_Graphic, 120, 8, 60, 25, 1, {.graphic = &gWindowGraphic56DAF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element14)},
    {WindowElementType_Graphic, 310, 69, 11, 11, 1, {.graphic = &gWindowGraphic56D9B8}, &WindowContacts_ScrollRow, -1, static_cast<ElementId>(WindowContactsElementId::ScrollRow)},
    {WindowElementType_Graphic, 310, 315, 11, 11, 1, {.graphic = &gWindowGraphic56D9E8}, &WindowContacts_ScrollRow, -1, static_cast<ElementId>(WindowContactsElementId::ScrollRow_16)},
    {WindowElementType_Graphic, 310, 80, 10, 14, 1, {.graphic = &gWindowGraphic56D9D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element17)},
    {WindowElementType_MouseArea, 309, 80, 11, 235, 1, {.mouseFlags = &gWindowMouseFlagsD67840}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowContactsElementId::HandleVerticalScroll)},
    {WindowElementType_EventArea, 0, 0, 331, 381, 1, {.unusedValue = 0}, &WindowContacts_Scroll, -1, static_cast<ElementId>(WindowContactsElementId::Scroll)},
    {WindowElementType_Graphic, 219, 45, 19, 19, 1, {.graphic = &gWindowGraphic56D970}, &WindowContacts_ButtonGroupMail, -1, static_cast<ElementId>(WindowContactsElementId::ButtonGroupMail)},
    {WindowElementType_Text, 200, 30, 0, 0, 1, {.text = &gWindowText56DB30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element21)},
    {WindowElementType_Graphic, 142, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton)},
    {WindowElementType_Graphic, 282, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton_23)},
    {WindowElementType_Graphic, 142, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton_24)},
    {WindowElementType_Graphic, 282, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton_25)},
    {WindowElementType_Graphic, 142, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton_26)},
    {WindowElementType_Graphic, 282, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton, -1, static_cast<ElementId>(WindowContactsElementId::HandleRankButton_27)},
    {WindowElementType_Graphic, 222, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail)},
    {WindowElementType_Graphic, 243, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose)},
    {WindowElementType_Graphic, 264, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete)},
    {WindowElementType_Text, 130, 71, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element31)},
    {WindowElementType_Text, 279, 71, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element32)},
    {WindowElementType_Text, 165, 87, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element33)},
    {WindowElementType_Text, 165, 102, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element34)},
    {WindowElementType_Text, 151, 133, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element35)},
    {WindowElementType_Graphic, 222, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_36)},
    {WindowElementType_Graphic, 243, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_37)},
    {WindowElementType_Graphic, 264, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_38)},
    {WindowElementType_Text, 130, 158, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element39)},
    {WindowElementType_Text, 279, 158, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element40)},
    {WindowElementType_Text, 165, 174, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element41)},
    {WindowElementType_Text, 165, 189, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element42)},
    {WindowElementType_Text, 151, 220, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element43)},
    {WindowElementType_Graphic, 222, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_44)},
    {WindowElementType_Graphic, 243, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_45)},
    {WindowElementType_Graphic, 264, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_46)},
    {WindowElementType_Text, 130, 245, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element47)},
    {WindowElementType_Text, 279, 245, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element48)},
    {WindowElementType_Text, 165, 261, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element49)},
    {WindowElementType_Text, 165, 276, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element50)},
    {WindowElementType_Text, 151, 307, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element51)},
    {WindowElementType_Graphic, 16, 67, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_52)},
    {WindowElementType_Graphic, 16, 154, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_53)},
    {WindowElementType_Graphic, 16, 241, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation, -1, static_cast<ElementId>(WindowContactsElementId::HandleNavigation_54)},
    {WindowElementType_Graphic, 242, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_55)},
    {WindowElementType_Graphic, 263, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_56)},
    {WindowElementType_Graphic, 284, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_57)},
    {WindowElementType_Text, 56, 80, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element58)},
    {WindowElementType_Text, 173, 80, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element59)},
    {WindowElementType_Graphic, 242, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_60)},
    {WindowElementType_Graphic, 263, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_61)},
    {WindowElementType_Graphic, 284, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_62)},
    {WindowElementType_Text, 56, 110, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element63)},
    {WindowElementType_Text, 173, 110, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element64)},
    {WindowElementType_Graphic, 242, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_65)},
    {WindowElementType_Graphic, 263, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_66)},
    {WindowElementType_Graphic, 284, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_67)},
    {WindowElementType_Text, 56, 139, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element68)},
    {WindowElementType_Text, 173, 139, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element69)},
    {WindowElementType_Graphic, 242, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_70)},
    {WindowElementType_Graphic, 263, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_71)},
    {WindowElementType_Graphic, 284, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_72)},
    {WindowElementType_Text, 56, 168, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element73)},
    {WindowElementType_Text, 173, 168, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element74)},
    {WindowElementType_Graphic, 242, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_75)},
    {WindowElementType_Graphic, 263, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_76)},
    {WindowElementType_Graphic, 284, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_77)},
    {WindowElementType_Text, 56, 197, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element78)},
    {WindowElementType_Text, 173, 197, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element79)},
    {WindowElementType_Graphic, 242, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_80)},
    {WindowElementType_Graphic, 263, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_81)},
    {WindowElementType_Graphic, 284, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_82)},
    {WindowElementType_Text, 56, 226, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element83)},
    {WindowElementType_Text, 173, 226, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element84)},
    {WindowElementType_Graphic, 242, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_85)},
    {WindowElementType_Graphic, 263, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_86)},
    {WindowElementType_Graphic, 284, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_87)},
    {WindowElementType_Text, 56, 255, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element88)},
    {WindowElementType_Text, 173, 255, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element89)},
    {WindowElementType_Graphic, 242, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_90)},
    {WindowElementType_Graphic, 263, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_91)},
    {WindowElementType_Graphic, 284, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_92)},
    {WindowElementType_Text, 56, 284, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element93)},
    {WindowElementType_Text, 173, 284, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element94)},
    {WindowElementType_Graphic, 242, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail, -1, static_cast<ElementId>(WindowContactsElementId::OpenMail_95)},
    {WindowElementType_Graphic, 263, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose, -1, static_cast<ElementId>(WindowContactsElementId::ButtonCompose_96)},
    {WindowElementType_Graphic, 284, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete, -1, static_cast<ElementId>(WindowContactsElementId::ButtonDelete_97)},
    {WindowElementType_Text, 56, 313, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element98)},
    {WindowElementType_Text, 173, 313, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element99)},
    {WindowElementType_Graphic, 15, 68, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar)},
    {WindowElementType_Graphic, 15, 97, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_101)},
    {WindowElementType_Graphic, 15, 126, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_102)},
    {WindowElementType_Graphic, 15, 155, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_103)},
    {WindowElementType_Graphic, 15, 184, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_104)},
    {WindowElementType_Graphic, 15, 213, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_105)},
    {WindowElementType_Graphic, 15, 242, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_106)},
    {WindowElementType_Graphic, 15, 271, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_107)},
    {WindowElementType_Graphic, 15, 300, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar, -1, static_cast<ElementId>(WindowContactsElementId::HandleAvatar_108)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56DAA8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element109)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic56DAC0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowContactsElementId::Element110)},
    {WindowElementType_EventArea, 331, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowContactsElementId::HandleSideArea)},
};

// 原型：窗口21；原全局：0x0054E990。
WindowPrototype gWindowPrototypeContacts {0, 4, 2, 64, 331, 381, 0x80010101u, 111, gWindowElements56DB38, &WindowContacts_Update, &WindowContacts_Draw, &WindowRuntime_ReleaseDefault}; // 21: 54E990

// 窗口函数实现。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    if (value == -1) {
        const std::int32_t category = business.valuesD67B40[6];
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
        const std::uint32_t sortMode = platform::gContactSortMode_D67CD0;
        business.mailSortModeD67B3C = sortMode;
        WindowContacts_SetDisplayMode(&callbackWindow, mode, category);
        static_cast<void>(world::detail::UpdateUnreadMailPageHint_518DA0(*callbackWindow.ownerSession));
        WindowContacts_ClampScroll(&callbackWindow);
    }
    return 1;
}

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(WindowInstance& callbackWindow, const std::int32_t value)
{
    static_cast<void>(WindowContacts_DragScroll(&callbackWindow, value));
    static_cast<void>(WindowMail_AdvanceUnreadBlink(*callbackWindow.ownerSession));
    static_cast<void>(WindowContacts_Refresh(&callbackWindow));
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x004A09E0。
static std::int32_t WindowContacts_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    const std::int32_t category = business.valuesD67B40[6];
    const std::uint32_t sortMode = business.mailSortModeD67B3C;
    auto* const element9 = currentWindow->FindElementById(WindowContactsElementId::Element9);
    if (element9) {
        auto* const sortGraphic = element9->data.graphic;
        sortGraphic->imageNo = kMailSortImages54F4C0[category * 5 + sortMode];
    }
    WindowContacts_Sort(*callbackWindow->ownerSession);
    if (WindowRuntime_Settings(*callbackWindow->ownerSession).contactsPreviewMode == 0) {
        const std::uint32_t page = callbackWindow->ownerSession->WorldScene().mail.data->contactPage;
        render::GraphicDescriptor16 descriptor {};
        const std::int32_t currentCategory = business.valuesD67B40[6];
        callbackWindow->ownerSession->WorldScene().mail.data->fallbackPage = page;
        if (currentCategory == 1) {
            for (std::int32_t index = 21; index < 27; ++index) {
                const std::int32_t allowed = WindowGuildDetails_ReadRankPermissions(*callbackWindow->ownerSession);
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = allowed != 0 ? 1 : 0;
            }
            if (WindowGuildDetails_ReadRankPermissions(*callbackWindow->ownerSession) != 0) {
                for (std::int32_t row = 0; row < 3; ++row) {
                    const std::int32_t rowCategory = business.valuesD67B40[6];
                    const std::int32_t scroll = business.valuesD67B40[rowCategory];
                    const std::int32_t memberIndex = business.mailIndicesC9FCE4[scroll + row];
                    const std::int32_t rank = callbackWindow->ownerSession->WorldScene().guild.orderedRecords[memberIndex].value00;
                    const std::uint8_t flags = cg::world::ReadGuildRankFlagsByte1051514(*callbackWindow->ownerSession, rank);
                    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(21 + row * 2)); indexedElement) indexedElement->visible = 0;
                    if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(22 + row * 2)); indexedElement2) indexedElement2->visible = (flags & 0x80) == 0 ? 1 : 0;
                }
            } else {
                for (std::int32_t row = 0; row < 3; ++row) {
                    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(21 + row * 2)); indexedElement) indexedElement->visible = 0;
                    if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(22 + row * 2)); indexedElement2) indexedElement2->visible = 0;
                }
            }
            for (std::int32_t index = 29; index < 46; index += 8) {
                const std::int32_t allowed = WindowGuildDetails_CanEdit(*callbackWindow->ownerSession, 2);
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = allowed != 0 ? 1 : 0;
            }
            if (cg::world::ReadGuildStatus10514AC(*callbackWindow->ownerSession) == -1) {
                if (auto* const element21 = currentWindow->FindElementById(WindowContactsElementId::Element21); element21) element21->visible = 0;
            } else {
                if (auto* const element21 = currentWindow->FindElementById(WindowContactsElementId::Element21); element21) element21->visible = 1;
                WindowInstance* const window = currentWindow;
                const std::int32_t status = cg::world::ReadGuildStatus10514AC(*callbackWindow->ownerSession);
                auto* const element212 = window->FindElementById(WindowContactsElementId::Element21);
                if (element212) {
                    auto* const data = element212->data.text;
                    WindowMail_WriteGuildStatus(*callbackWindow->ownerSession, data, status);
                }
            }
        }

        // 原类别未覆盖时沿用未定义/旧借用；正常三类每行只绑定指针，不缓存字段值。
        MailContactView record;
        std::int32_t privileged;
        std::wstring numberText;
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t rowCategory = business.valuesD67B40[6];
            const std::int32_t scroll = business.valuesD67B40[rowCategory];
            WindowInstance* const entryWindow = currentWindow;
            auto* entryElementsWindow = entryWindow;
            const std::int32_t recordIndex = business.mailIndicesC9FCE4[scroll + row];
            if (auto* const indexedElement = entryElementsWindow->FindElementById(NativeElementKey(29 + row * 8)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(28 + row * 8)); indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(27 + row * 8)); indexedElement3) indexedElement3->visible = 1;
            switch (business.valuesD67B40[6]) {
            case 0:
                record = WindowMail_MakeContactView(callbackWindow->ownerSession->WorldScene().mail.data->contacts[recordIndex]);
                if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(51 + row)); indexedElement4) indexedElement4->data.graphic->imageNo = 0x3B9E0;
                break;
            case 1:
                record = WindowMail_MakeContactView(callbackWindow->ownerSession->WorldScene().guild.orderedRecords[recordIndex]);
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(51 + row)); indexedElement5) indexedElement5->data.graphic->imageNo = 0x3B9E8;
                break;
            case 2:
                record = WindowMail_MakeContactView(world::detail::ReadMiniMailContact1075A90(*callbackWindow->ownerSession, recordIndex));
                if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(51 + row)); indexedElement6) indexedElement6->data.graphic->imageNo = 0x3B9E0;
                break;
            }
            const std::int32_t textIndex = 30 + row * 8;
            if (*record.used == 0) {
                if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(29 + row * 8)); indexedElement4) indexedElement4->visible = 0;
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(28 + row * 8)); indexedElement5) indexedElement5->visible = 0;
                if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(27 + row * 8)); indexedElement6) indexedElement6->visible = 0;
                for (std::int32_t index = textIndex; index < textIndex + 5; ++index) {
                    auto* const dataElement = currentWindow->FindElementById(NativeElementKey(index));
                    if (dataElement) {
                        auto* const data = dataElement->data.text;
                        data->text = L"";
                    }
                }
                continue;
            }
            const std::uint32_t color = 0xFFFFFFFFu;
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            const std::int32_t image = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(*record.imageNo);
            const std::int32_t anchorY = kMailImageAnchors56F790[row][1];
            WindowInstance* const window = currentWindow;
            const std::int32_t y = window->y + anchorY - 5;
            const std::int32_t windowX = window->x;
            const std::int32_t anchorX = kMailImageAnchors56F790[row][0];
            static_cast<void>(cg::render::DrawGraphic(
                windowX + anchorX - 7, y, 223, image, 0, &descriptor));
            auto* const nameDataElement = currentWindow->FindElementById(NativeElementKey(textIndex));
            if (nameDataElement) {
                auto* const nameData = nameDataElement->data.text;
                nameData->text = record.name;
            }
            cg::text::FormatTo(numberText, L"{:3}" /* 原常量参考：0x00553FCC。 */, *record.number);
            auto* const numberDataElement = currentWindow->FindElementById(NativeElementKey(textIndex + 1));
            if (numberDataElement) {
                auto* const numberData = numberDataElement->data.text;
                numberData->text = numberText;
            }
            switch (business.valuesD67B40[6]) {
            case 0:
            case 2: {
                auto* const dataElement = currentWindow->FindElementById(NativeElementKey(textIndex + 2));
                if (dataElement) {
                    auto* const data = dataElement->data.text;
                    data->text = record.text32;
                }
                break;
            }
            case 1: {
                const auto* const member = &callbackWindow->ownerSession->WorldScene().guild.orderedRecords[recordIndex];
                privileged = WindowGuildDetails_CanEditRank(*callbackWindow->ownerSession, member->value00, 0x80);
                const std::int32_t rank = member->value00;
                const wchar_t* const text = rank == -1
                    ? L"" : cg::world::GetGuildRankText(*callbackWindow->ownerSession, rank);
                auto* const dataElement = currentWindow->FindElementById(NativeElementKey(textIndex + 2));
                if (dataElement) {
                    auto* const data = dataElement->data.text;
                    data->text = text;
                }
                break;
            }
            }
            auto* const textDataElement = currentWindow->FindElementById(NativeElementKey(textIndex + 3));
            if (textDataElement) {
                auto* const textData = textDataElement->data.text;
                textData->text = record.text21;
            }
            auto* statusElementsWindow = currentWindow;
            const std::int16_t status = record.ReadStatus();
            auto* const statusDataElement = statusElementsWindow->FindElementById(NativeElementKey(textIndex + 4));
            if (statusDataElement) {
                auto* const statusData = statusDataElement->data.text;
                WindowMail_WriteContactStatus(*callbackWindow->ownerSession, record, statusData, status);
            }
            switch (business.valuesD67B40[6]) {
            case 0:
                if (world::detail::HasUnreadContactMail_519090(*callbackWindow->ownerSession, recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(callbackWindow, 27 + row * 8);
                }
                break;
            case 1:
                if (WindowGuildDetails_CanEdit(*callbackWindow->ownerSession, 2) != 0) {
                    if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(29 + row * 8)); indexedElement4) indexedElement4->visible = privileged == 0 ? 1 : 0;
                } else {
                    const std::int32_t ownKey = cg::world::ReadGuildOwnKey10514B8(*callbackWindow->ownerSession);
                    const std::int32_t memberKey = callbackWindow->ownerSession->WorldScene().guild.orderedRecords[recordIndex].value0C;
                    if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(29 + row * 8)); indexedElement4) indexedElement4->visible = memberKey == ownKey ? 1 : 0;
                }
                if (world::detail::HasUnreadGuildMail_5190F0(*callbackWindow->ownerSession, recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(callbackWindow, 27 + row * 8);
                }
                break;
            case 2:
                if (world::detail::HasUnreadMiniMail_519170(*callbackWindow->ownerSession, recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(callbackWindow, 27 + row * 8);
                }
                break;
            }
        }
        return 108;
    }

    if (business.valuesD67B40[6] == 1) {
        if (auto* const element21 = currentWindow->FindElementById(WindowContactsElementId::Element21); element21) element21->visible = 1;
        WindowInstance* const window = currentWindow;
        const std::int32_t status = cg::world::ReadGuildStatus10514AC(*callbackWindow->ownerSession);
        auto* const element212 = window->FindElementById(WindowContactsElementId::Element21);
        if (element212) {
            auto* const data = element212->data.text;
            WindowMail_WriteGuildStatus(*callbackWindow->ownerSession, data, status);
        }
    } else {
        if (auto* const element21 = currentWindow->FindElementById(WindowContactsElementId::Element21); element21) element21->visible = 0;
    }
    MailContactView record;
    for (std::int32_t row = 0; row < 9; ++row) {
        const std::int32_t rowCategory = business.valuesD67B40[6];
        const std::int32_t scroll = business.valuesD67B40[3 + rowCategory];
        WindowInstance* const entryWindow = currentWindow;
        const std::int32_t recordIndex = business.mailIndicesC9FCE4[scroll + row];
        if (auto* const indexedElement = entryWindow->FindElementById(NativeElementKey(56 + row * 5)); indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(55 + row * 5)); indexedElement2) indexedElement2->visible = 1;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(54 + row * 5)); indexedElement3) indexedElement3->visible = 1;
        switch (business.valuesD67B40[6]) {
        case 0:
            record = WindowMail_MakeContactView(callbackWindow->ownerSession->WorldScene().mail.data->contacts[recordIndex]);
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(56 + row * 5)); indexedElement4) indexedElement4->visible = 1;
            break;
        case 1: {
            const auto* const member = &callbackWindow->ownerSession->WorldScene().guild.orderedRecords[recordIndex];
            record = WindowMail_MakeContactView(*member);
            if (WindowGuildDetails_CanEdit(*callbackWindow->ownerSession, 2) != 0) {
                const std::int32_t privileged = WindowGuildDetails_CanEditRank(*callbackWindow->ownerSession, member->value00, 0x80);
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(56 + row * 5)); indexedElement5) indexedElement5->visible = privileged == 0 ? 1 : 0;
            } else {
                const std::int32_t memberKey = member->value0C;
                const std::int32_t ownKey = cg::world::ReadGuildOwnKey10514B8(*callbackWindow->ownerSession);
                if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(56 + row * 5)); indexedElement5) indexedElement5->visible = memberKey == ownKey ? 1 : 0;
            }
            break;
        }
        case 2:
            record = WindowMail_MakeContactView(world::detail::ReadMiniMailContact1075A90(*callbackWindow->ownerSession, recordIndex));
            break;
        }
        const std::int32_t textIndex = 57 + row * 5;
        if (*record.used == 0) {
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(56 + row * 5)); indexedElement4) indexedElement4->visible = 0;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(55 + row * 5)); indexedElement5) indexedElement5->visible = 0;
            if (auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(54 + row * 5)); indexedElement6) indexedElement6->visible = 0;
            for (std::int32_t index = textIndex; index < textIndex + 2; ++index) {
                auto* const dataElement = currentWindow->FindElementById(NativeElementKey(index));
                if (dataElement) {
                    auto* const data = dataElement->data.text;
                    data->text = L"";
                }
            }
            continue;
        }
        auto* const nameDataElement = currentWindow->FindElementById(NativeElementKey(textIndex));
        if (nameDataElement) {
            auto* const nameData = nameDataElement->data.text;
            nameData->text = record.name;
        }
        auto* const statusDataElement = currentWindow->FindElementById(NativeElementKey(textIndex + 1));
        if (statusDataElement) {
            auto* const statusData = statusDataElement->data.text;
            const std::int16_t status = record.ReadStatus();
            WindowMail_WriteContactStatus(*callbackWindow->ownerSession, record, statusData, status);
        }
        switch (business.valuesD67B40[6]) {
        case 0:
            if (world::detail::HasUnreadContactMail_519090(*callbackWindow->ownerSession, recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(callbackWindow, 54 + row * 5);
            }
            break;
        case 1:
            if (world::detail::HasUnreadGuildMail_5190F0(*callbackWindow->ownerSession, recordIndex) != 0) {
                const std::int32_t blink = business.mailBlinkFlagD67B60;
                auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(54 + row * 5));
                if (graphicElement) {
                    auto* const graphic = graphicElement->data.graphic;
                    graphic->imageNo = blink != 0 ? 0x3B549 : 0x3B547;
                }
            }
            break;
        case 2:
            if (world::detail::HasUnreadMiniMail_519170(*callbackWindow->ownerSession, recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(callbackWindow, 54 + row * 5);
            }
            break;
        }
    }
    return 3672;
}

// 原函数：0x004A1690。
static void WindowContacts_SetDisplayMode(WindowInstance* callbackWindow, const std::int32_t mode, const std::int32_t category)
{
    WindowContacts_ApplyDisplayLayout(callbackWindow, mode);
    WindowContacts_ApplyCategoryLayout(callbackWindow, category);
}

// 原函数：0x004A16B0。
static void WindowContacts_ApplyDisplayLayout(WindowInstance* callbackWindow, const std::int32_t mode)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    for (std::int32_t index = 0; index < 21; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
    }
    if (mode == 0) {
        auto* const element8 = currentWindow->FindElementById(WindowContactsElementId::Element8);
        if (element8) {
            auto* const graphic = element8->data.graphic;
            graphic->imageNo = 0x3B612;
        }
        for (std::int32_t index = 21; index < 54; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
    } else if (mode == 1) {
        auto* const element8 = currentWindow->FindElementById(WindowContactsElementId::Element8);
        if (element8) {
            auto* const graphic = element8->data.graphic;
            graphic->imageNo = 0x3B611;
        }
        for (std::int32_t index = 21; index < 54; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        }
    }
}

// 原函数：0x004A1780。
static void WindowContacts_ApplyCategoryLayout(WindowInstance* callbackWindow, const std::int32_t category)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    if (category == 0) {
        for (std::int32_t index = 21; index < 27; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        if (auto* const element14 = currentWindow->FindElementById(WindowContactsElementId::Element14); element14) element14->visible = 0;
        if (auto* const element21 = currentWindow->FindElementById(WindowContactsElementId::Element21); element21) element21->visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = currentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            if (auto* const indexedElement = window->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->x = x - 24;
        }
        auto* const tabCategoryElement = currentWindow->FindElementById(WindowContactsElementId::TabCategory);
        if (tabCategoryElement) {
            auto* const graphic = tabCategoryElement->data.graphic;
            graphic->imageNo = 0x3B57E;
        }
        for (std::int32_t index = 21; index < 27; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
    } else if (category == 1) {
        auto* const tabCategory_12Element = currentWindow->FindElementById(WindowContactsElementId::TabCategory_12);
        if (tabCategory_12Element) {
            auto* const graphic = tabCategory_12Element->data.graphic;
            graphic->imageNo = 0x3B581;
        }
        if (auto* const element14 = currentWindow->FindElementById(WindowContactsElementId::Element14); element14) element14->visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = currentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            if (auto* const indexedElement = window->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->x = x;
        }
    } else if (category == 2) {
        for (std::int32_t index = 21; index < 27; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        if (auto* const element14 = currentWindow->FindElementById(WindowContactsElementId::Element14); element14) element14->visible = 1;
        for (const std::int32_t index : {34, 42, 50, 58, 63, 68, 73, 78, 83, 88, 93, 98, 20}) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        auto* const tabCategory_13Element = currentWindow->FindElementById(WindowContactsElementId::TabCategory_13);
        if (tabCategory_13Element) {
            auto* const graphic = tabCategory_13Element->data.graphic;
            graphic->imageNo = 0x3C116;
        }
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = currentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            if (auto* const indexedElement = window->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->x = x - 24;
        }
        for (const std::int32_t index : {34, 42, 50, 0, 3, 4, 8, 19, 20}) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        if (auto* const handleNavigation_6Element = currentWindow->FindElementById(WindowContactsElementId::HandleNavigation_6); handleNavigation_6Element) handleNavigation_6Element->visible = 1;
        if (auto* const handleNavigation_7Element = currentWindow->FindElementById(WindowContactsElementId::HandleNavigation_7); handleNavigation_7Element) handleNavigation_7Element->visible = 1;
        for (std::int32_t index = 21; index < 27; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
    } else {
        return;
    }

    for (const std::int32_t index : {1, 2, 7}) {
        WindowInstance* const xWindow = currentWindow;
        const std::int32_t x = gWindowElements56DB38[index].x;
        if (auto* const indexedElement = xWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->x = x + (category == 2 ? 110 : 0);
        WindowInstance* const yWindow = currentWindow;
        const std::int32_t y = gWindowElements56DB38[index].y;
        if (auto* const indexedElement2 = yWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->y = y + (category == 2 ? 17 : 0);
    }
}

// 原函数：0x004A1C30。
static std::int32_t WindowContacts_ScrollRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t imageOffset = elementId == static_cast<ElementId>(WindowContactsElementId::ScrollRow) ? 0 : 3;
        graphic->imageNo = 0x3B592 + imageOffset;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B594 + imageOffset;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B593 + imageOffset;
        }
    }
    if ((state & MouseFlag_LRepeatDown) == 0) {
        return 0;
    }

    const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
    const std::int32_t category = business.valuesD67B40[6];
    std::int32_t& position = business.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    position += elementId == static_cast<ElementId>(WindowContactsElementId::ScrollRow) ? -1 : 1;
    if ((mode == 0 || mode == 1) && category >= 0 && category <= 2) {
        std::int32_t maximum;
        if (category == 0) {
            maximum = mode == 0 ? 57 : 51;
        } else if (category == 1) {
            const std::int32_t count = callbackWindow.ownerSession->WorldScene().guild.orderedRecordCount;
            maximum = count < 100 ? (mode == 0 ? 97 : 91) : count - (mode == 0 ? 3 : 9);
        } else {
            maximum = mode == 0 ? 17 : 11;
        }
        std::int32_t sound = 51;
        if (position < 0) {
            position = 0;
            sound = 55;
        } else if (position > maximum) {
            position = maximum;
            sound = 55;
        }
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
    }
    WindowContacts_ClampScroll(&callbackWindow);
    login::detail::SaveCurrentLoginRecord_520200();
    return 1;
}

// 原函数：0x004A1EF0。
static std::int32_t WindowContacts_HandleNavigation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (elementId >= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_52) && elementId <= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_54)) {
        if ((state & MouseFlag_RDownEvent) == 0) {
            return 0;
        }
        switch (business.valuesD67B40[6]) {
        case 0: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
            if (callbackWindow.ownerSession->WorldScene().mail.data->contacts[recordIndex].used == 0) {
                return 1;
            }
            static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(0, recordIndex));
            break;
        }
        case 1: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
            const auto& record = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[recordIndex];
            if (record.value08 == 0) {
                return 1;
            }
            const std::int32_t target = record.value0C;
            static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(1, target));
            break;
        }
        case 2: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
            const auto& record = world::detail::ReadMiniMailContact1075A90(*callbackWindow.ownerSession, recordIndex);
            if (record.used == 0) {
                return 1;
            }
            const std::int32_t target = record.key04;
            static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(2, target));
            break;
        }
        }
        return 0;
    }
    if (elementId < static_cast<ElementId>(WindowContactsElementId::HandleNavigation) || elementId > static_cast<ElementId>(WindowContactsElementId::HandleNavigation_7)) {
        return 0;
    }

    const std::int32_t imageOffset = (index & 1) != 0 ? 0 : 3;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B558 + imageOffset;
            if (elementId <= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_3)) {
                uiFrame.hoverText = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode != 0 ? L"切换为详细显示。" /* 原常量参考：0x00587444。 */ : L"切换为预览显示。" /* 原常量参考：0x00587458。 */;
            } else if (elementId <= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_5)) {
                uiFrame.hoverText = L"切换名片的排序方式。" /* 原常量参考：0x0058746C。 */;
            }
        } else {
            graphic->imageNo = 0x3B556 + imageOffset;
        }
    }

    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (elementId <= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_3)) {
            const std::int32_t oldMode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
            const std::int32_t category = business.valuesD67B40[6];
            const std::int32_t mode = oldMode == 0 ? 1 : 0;
            WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode = mode;
            WindowContacts_SetDisplayMode(&callbackWindow, mode, category);
            const std::int32_t currentCategory = business.valuesD67B40[6];
            business.valuesD67B40[3 + currentCategory] = 0;
            business.valuesD67B40[currentCategory] = 0;
            WindowContacts_ClampScroll(&callbackWindow);
        } else if (elementId <= static_cast<ElementId>(WindowContactsElementId::HandleNavigation_5)) {
            const std::int32_t oldSort = std::bit_cast<std::int32_t>(business.mailSortModeD67B3C);
            const std::int32_t sort = (oldSort + (elementId == static_cast<ElementId>(WindowContactsElementId::HandleNavigation_4) ? 4 : 6)) % 5;
            business.mailSortModeD67B3C = std::bit_cast<std::uint32_t>(sort);
            platform::gContactSortMode_D67CD0 = std::bit_cast<std::uint32_t>(sort);
        } else {
            const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
            const std::int32_t category = business.valuesD67B40[6];
            const std::int32_t step = mode == 0 ? 3 : 9;
            business.valuesD67B40[(mode == 0 ? 0 : 3) + category] += elementId == static_cast<ElementId>(WindowContactsElementId::HandleNavigation_6) ? -step : step;
            WindowContacts_ClampScroll(&callbackWindow);
        }
        login::detail::SaveCurrentLoginRecord_520200();
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        // 业务调用可能改变当前窗口，按原尾路径重新借用载荷。
        auto* const pressedElement = currentWindow->FindElementById(elementId);
        if (pressedElement) {
            auto* const pressed = pressedElement->data.graphic;
            pressed->imageNo = 0x3B557 + imageOffset;
        }
    }
    return consumed;
}

// 原函数：0x004A2610。
static std::int32_t WindowContacts_ButtonGroupMail(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = 0x3B541;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B543;
            uiFrame.hoverText = L"对一群人寄送邮件。" /* 原常量参考：0x0058749C。 */;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            graphic->imageNo = 0x3B542;
            WindowInstance* const window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_GroupSelect].window;
            if (window != nullptr) {
                window->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            } else {
                business.prefixD2F0B8.value10 = 1;
                business.prefixD2F0B8.value00 = 0;
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_MailCompose, 1, 0, nullptr));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B542;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004A2870。
static std::int32_t WindowContacts_TabCategory(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (elementId < static_cast<ElementId>(WindowContactsElementId::TabCategory) || elementId > static_cast<ElementId>(WindowContactsElementId::TabCategory_13)) {
        return 0;
    }
    const std::int32_t category = index - 10;
    if (business.valuesD67B40[6] == category) {
        return 0;
    }
    const std::int32_t idle = category == 0 ? 0x3B57D : (category == 1 ? 0x3B580 : 0x3C115);
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = idle + 2;
        const wchar_t* const hint = category == 0 ? L"切换至名片视窗。" /* 原常量参考：0x005874B0。 */
            : (category == 1 ? L"切换至家族名簿视窗。" /* 原常量参考：0x005874C4。 */ : L"切换到迷你邮件群组。" /* 原常量参考：0x005874DC。 */);
        uiFrame.hoverText = hint;
        consumed = 1;
    } else {
        if (graphic) graphic->imageNo = idle;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (category == 2) {
            auto* const tabCategory_13Element = currentWindow->FindElementById(WindowContactsElementId::TabCategory_13);
            if (tabCategory_13Element) {
                auto* const selected = tabCategory_13Element->data.graphic;
                selected->imageNo = 0x3C116;
            }
        } else {
            auto* const element109 = currentWindow->FindElementById(WindowContactsElementId::Element109);
            if (element109) {
                auto* const title = element109->data.graphic;
                title->imageNo = category == 0 ? 0x3B9DE : 0x3B9E3;
            }
        }
        if (auto* const buttonGuildDetailsElement = currentWindow->FindElementById(WindowContactsElementId::ButtonGuildDetails); buttonGuildDetailsElement) buttonGuildDetailsElement->visible = category == 1 ? 1 : 0;
        business.valuesD67B40[6] = category;
        WindowContacts_ClampScroll(&callbackWindow);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        const std::int32_t currentCategory = business.valuesD67B40[6];
        const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
        WindowContacts_SetDisplayMode(&callbackWindow, mode, currentCategory);
        consumed = 1;
    }
    return consumed;
}

// 原函数：0x004A2BD0。
static std::int32_t WindowContacts_ButtonGuildDetails(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = 0x3B9E5;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9E7;
            uiFrame.hoverText = L"开启家族详细资料视窗。" /* 原常量参考：0x00587544。 */;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            graphic->imageNo = 0x3B9E6;
            static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_GuildDetails, 1, 0, nullptr));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B9E6;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004A37E0。
static std::int32_t WindowContacts_ResolveRecordIndex(session::ClientSession& session, const std::int32_t index)
{
    auto& business = session.UiFrame().native.business;
    if (WindowRuntime_Settings(session).contactsPreviewMode == 0) {
        const std::int32_t category = business.valuesD67B40[6];
        const std::int32_t position = business.valuesD67B40[category];
        return business.mailIndicesC9FCE4[position + index - 51];
    }
    const std::int32_t category = business.valuesD67B40[6];
    const std::int32_t position = business.valuesD67B40[3 + category];
    return business.mailIndicesC9FCE4[position + index - 99];
}

// 原函数：0x004A40C0。
static std::int32_t WindowContacts_Scroll(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) == 0) {
        return 0;
    }
    const std::int32_t category = business.valuesD67B40[6];
    std::int32_t count;
    switch (category) {
    case 0:
        count = 60;
        break;
    case 1:
        count = callbackWindow.ownerSession->WorldScene().guild.orderedRecordCount;
        if (count < 100) {
            count = 100;
        }
        break;
    case 2:
        count = 20;
        break;
    default:
        count = state;
        break;
    }
    const std::int32_t mode = WindowRuntime_Settings(*callbackWindow.ownerSession).contactsPreviewMode;
    const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
    const std::int32_t position = business.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowContactsElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        const std::int32_t result = WindowScroll_ApplyWheel(
            handleVerticalScrollElement, position, count - (mode == 0 ? 3 : 9), wheel);
        const std::int32_t currentCategory = business.valuesD67B40[6];
        business.valuesD67B40[(mode == 0 ? 0 : 3) + currentCategory] = result;
    }
    return 0;
}

// 原函数：0x004A2530。
static std::int32_t WindowContacts_HandleRankButton(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t parity = (index - 21) % 2;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t imageOffset = parity != 0 ? 3 : 0;
        graphic->imageNo = 0x3B556 + imageOffset;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B558 + imageOffset;
            uiFrame.hoverText = L"变更此家族成员的称号。" /* 原常量参考：0x00587484。 */;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            graphic->imageNo = 0x3B557 + imageOffset;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            WindowContacts_OpenRankSelection(*callbackWindow.ownerSession, index);
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B557 + imageOffset;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004A38C0。
static void WindowContacts_OpenRankSelection(session::ClientSession& session, const std::int32_t index)
{
    auto& business = session.UiFrame().native.business;
    const std::int32_t category = business.valuesD67B40[6];
    const std::int32_t row = (index - 21) / 2;
    const std::int32_t position = business.valuesD67B40[category];
    const std::int32_t recordIndex = business.mailIndicesC9FCE4[position + row];
    const std::int32_t key = session.WorldScene().guild.orderedRecords[recordIndex].value0C;
    const std::int32_t memberIndex = cg::world::FindActiveOrderedRecordIndex_5195C0(session, key);
    const std::int32_t currentCategory = business.valuesD67B40[6];
    business.mailMemberIndexCB1D18 = memberIndex;
    const std::int32_t currentPosition = business.valuesD67B40[currentCategory];
    const std::int32_t currentIndex = business.mailIndicesC9FCE4[currentPosition + row];
    business.mailMemberKeyD45F88 = session.WorldScene().guild.orderedRecords[currentIndex].value0C;
    business.mailRankCF4A14 = session.WorldScene().guild.orderedRecords[memberIndex].value00 - 1;
    static_cast<void>(WindowRuntime_Open(WindowId_GuildRank));
}

// 原函数：0x004A2B00。
static std::int32_t WindowContacts_OpenMail(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    if (graphic) graphic->imageNo = 0x3B547;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B549;
        switch (business.valuesD67B40[6]) {
        case 0:
            uiFrame.hoverText = L"察看此名片之主人的邮件纪录" /* 原常量参考：0x005874F4。 */;
            break;
        case 1:
            uiFrame.hoverText = L"察看此家族成员的邮件纪录" /* 原常量参考：0x00587510。 */;
            break;
        case 2:
            uiFrame.hoverText = L"浏览此迷你邮件的历程。" /* 原常量参考：0x0058752C。 */;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (graphic) graphic->imageNo = 0x3B548;
        callbackWindow.ownerSession->WorldScene().mail.data->viewedContactId = WindowContacts_ResolveButtonRecord(*callbackWindow.ownerSession, index);
        const std::int32_t category = business.valuesD67B40[6];
        business.mailCategoryD67B64 = category;
        business.mailCategoryCC4D50 = category;
        static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_MailRead, 1, 0, nullptr));
        consumed = 1;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B548;
    }
    return consumed;
}

// 原函数：0x004A3780。
static std::int32_t WindowContacts_ResolveButtonRecord(session::ClientSession& session, const std::int32_t index)
{
    auto& business = session.UiFrame().native.business;
    if (WindowRuntime_Settings(session).contactsPreviewMode == 0) {
        const std::int32_t category = business.valuesD67B40[6];
        const std::int32_t row = (index - 27) / 8;
        const std::int32_t position = business.valuesD67B40[category];
        return business.mailIndicesC9FCE4[position + row];
    }
    const std::int32_t row = (index - 54) / 5;
    const std::int32_t category = business.valuesD67B40[6];
    const std::int32_t position = business.valuesD67B40[3 + category];
    return business.mailIndicesC9FCE4[position + row];
}

// 原函数：0x004A39C0。
static std::int32_t WindowContacts_HandleAvatar(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const activeWindow = &callbackWindow;
    if (!activeWindow) return 0;
    const std::int32_t category = business.valuesD67B40[6];
    const std::int32_t position = business.valuesD67B40[3 + category] + index - 99;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t* recordImage = nullptr;
    switch (category) {
    case 0:
        if (position < 0 || position > 59) {
            return 0;
        }
        if ((state & MouseFlag_Hover) != 0) {
            const std::int32_t recordIndex = business.mailIndicesC9FCE4[position];
            const auto& record = callbackWindow.ownerSession->WorldScene().mail.data->contacts[recordIndex];
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    case 1:
        if (position < 0) {
            return 0;
        }
        if (position > callbackWindow.ownerSession->WorldScene().guild.orderedRecordCount - 1) {
            return 0;
        }
        if ((state & MouseFlag_Hover) != 0) {
            const std::int32_t recordIndex = business.mailIndicesC9FCE4[position];
            const auto& record = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[recordIndex];
            if (record.value08 != 0) {
                recordImage = &record.value14;
            }
        }
        break;
    case 2:
        if ((state & MouseFlag_Hover) != 0) {
            const std::int32_t recordIndex = business.mailIndicesC9FCE4[position];
            const auto& record = world::detail::ReadMiniMailContact1075A90(*callbackWindow.ownerSession, recordIndex);
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    }
    std::int32_t consumed = 0;
    if (recordImage != nullptr) {
        WindowInstance* const firstWindow = activeWindow;
        const std::uint32_t color = 0xFFFFFFFFu;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        auto* firstElementsWindow = firstWindow;
        const std::int32_t firstWindowY = firstWindow->y;
        auto* const firstElementYElement = firstElementsWindow->FindElementById(elementId);
        if (firstElementYElement) {
            const std::int32_t firstElementY = firstElementYElement->y;
            auto* const firstElementXElement = firstElementsWindow->FindElementById(elementId);
            if (firstElementXElement) {
                const std::int32_t firstElementX = firstElementXElement->x;
                const std::int32_t firstY = firstElementY + firstWindowY + 55;
                const std::int32_t firstWindowX = firstWindow->x;
                static_cast<void>(cg::render::DrawGraphic(
                    firstElementX + firstWindowX + 182, firstY, 223, 0x3BD32, 0, &descriptor));
            }
        }

        // 先固定第一次绘图后的元素；图号转换后仅重取当前窗口，不更换此元素。
        auto* const elementPtr = activeWindow->FindElementById(elementId);
        if (elementPtr) {
            WindowElement* const element = elementPtr;
            const std::int32_t image = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(*recordImage);
            const std::int32_t elementY = element->y;
            WindowInstance* const currentWindow = activeWindow;
            const std::int32_t windowY = currentWindow->y;
            const std::int32_t y = elementY + windowY + 55;
            const std::int32_t elementX = element->x;
            const std::int32_t windowX = currentWindow->x;
            static_cast<void>(cg::render::DrawGraphic(
                elementX + windowX + 182, y, 223, image, 0, &descriptor));
        }
        consumed = 1;
    }
    if ((state & MouseFlag_RDownEvent) == 0) {
        return consumed;
    }
    switch (business.valuesD67B40[6]) {
    case 0: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
        if (callbackWindow.ownerSession->WorldScene().mail.data->contacts[recordIndex].used == 0) {
            return 1;
        }
        static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(0, recordIndex));
        break;
    }
    case 1: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
        const auto& record = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[recordIndex];
        if (record.value08 == 0) {
            return 1;
        }
        const std::int32_t target = record.value0C;
        static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(1, target));
        break;
    }
    case 2: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(*callbackWindow.ownerSession, index);
        const auto& record = world::detail::ReadMiniMailContact1075A90(*callbackWindow.ownerSession, recordIndex);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t target = record.key04;
        static_cast<void>(callbackWindow.ownerSession->Connection().RequestPlayerProfile(2, target));
        break;
    }
    }
    return consumed;
}

// 原函数：0x004A3C70。
static std::int32_t WindowContacts_ButtonCompose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    const bool hover = (state & MouseFlag_Hover) != 0;
    if (graphic) graphic->imageNo = hover ? 0x3B546 : 0x3B544;
    const std::int32_t consumed = hover ? 1 : 0;
    bool nineRows;
    std::int32_t row;
    switch (static_cast<WindowContactsElementId>(elementId)) {
    case WindowContactsElementId::ButtonCompose: nineRows = false; row = 0; break;
    case WindowContactsElementId::ButtonCompose_37: nineRows = false; row = 1; break;
    case WindowContactsElementId::ButtonCompose_45: nineRows = false; row = 2; break;
    case WindowContactsElementId::ButtonCompose_56: nineRows = true; row = 0; break;
    case WindowContactsElementId::ButtonCompose_61: nineRows = true; row = 1; break;
    case WindowContactsElementId::ButtonCompose_66: nineRows = true; row = 2; break;
    case WindowContactsElementId::ButtonCompose_71: nineRows = true; row = 3; break;
    case WindowContactsElementId::ButtonCompose_76: nineRows = true; row = 4; break;
    case WindowContactsElementId::ButtonCompose_81: nineRows = true; row = 5; break;
    case WindowContactsElementId::ButtonCompose_86: nineRows = true; row = 6; break;
    case WindowContactsElementId::ButtonCompose_91: nineRows = true; row = 7; break;
    case WindowContactsElementId::ButtonCompose_96: nineRows = true; row = 8; break;
    default: return consumed;
    }
    const std::int32_t category = business.valuesD67B40[6];
    if (!nineRows && (category < 0 || category > 2)) {
        return consumed;
    }
    if (hover) {
        switch (category) {
        case 0:
            uiFrame.hoverText = L"寄送邮件给此名片的主人。" /* 原常量参考：0x005876B0。 */;
            break;
        case 1:
            uiFrame.hoverText = L"对此家族成员寄送邮件。" /* 原常量参考：0x005876CC。 */;
            break;
        case 2:
            uiFrame.hoverText = L"传送迷你邮件。" /* 原常量参考：0x005876E4。 */;
            break;
        }
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return consumed;
    }
    business.prefixD2F0B8.value10 = 0;
    business.prefixD2F0B8.value00 = category;
    const std::int32_t position = business.valuesD67B40[(nineRows ? 3 : 0) + category];
    business.prefixD2F0B8.value04 = business.mailIndicesC9FCE4[position + row];
    static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_MailCompose, 1, 0, nullptr));
    return 1;
}

// 原函数：0x004A32E0。
static std::int32_t WindowContacts_ButtonDelete(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    if (graphic) graphic->imageNo = 0x3B54A;
    if ((state & MouseFlag_Hover) != 0) {
        if (graphic) graphic->imageNo = 0x3B54C;
        switch (business.valuesD67B40[6]) {
        case 0:
            uiFrame.hoverText = L"删除此名片。" /* 原常量参考：0x0058755C。 */;
            break;
        case 1:
            uiFrame.hoverText = L"对此家族成员进行除名。" /* 原常量参考：0x0058756C。 */;
            break;
        case 2:
            uiFrame.hoverText = L"删除此迷你邮件的名片。" /* 原常量参考：0x00587584。 */;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        const std::int32_t recordIndex = WindowContacts_ResolveButtonRecord(*callbackWindow.ownerSession, index);
        const std::int32_t category = business.valuesD67B40[6];
        business.mailDeleteIndexD2DA8C = recordIndex;
        switch (category) {
        case 0: static_cast<void>(WindowContacts_ConfirmDeleteContact(*callbackWindow.ownerSession, recordIndex)); break;
        case 1: static_cast<void>(WindowContacts_ConfirmDeleteGuildMember(*callbackWindow.ownerSession, recordIndex)); break;
        case 2: static_cast<void>(WindowContacts_ConfirmDeleteMiniMail(*callbackWindow.ownerSession, recordIndex)); break;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (graphic) graphic->imageNo = 0x3B54B;
    }
    return consumed;
}

// 原函数：0x004A33C0。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteContact(session::ClientSession& session, const std::int32_t index)
{
    auto& confirmation = session.UiFrame().native.confirmation;
    WindowMail_InitializeDeleteConfirmation(session, &WindowMail_DeleteContact);
    confirmation.textCC426C = std::format(L"" LR"(
)" L"     确定将" LR"(
)" L"　" LR"(
)" L"　{}" LR"(
)" L"　" LR"(
)" L"　从名片里删除吗？" LR"(
)" L" " LR"(
)" L" (确定删除时，请按住" LR"(
)" L"  CTRL键再选择「是」)" /* 原常量参考：0x0058759C。 */, session.WorldScene().mail.data->contacts[index].name10);
    return WindowRuntime_CreateAnimation(session, WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3480。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteGuildMember(session::ClientSession& session, const std::int32_t index)
{
    auto& confirmation = session.UiFrame().native.confirmation;
    WindowMail_InitializeDeleteConfirmation(session, &WindowMail_DeleteGuildMember);
    confirmation.textCC426C = std::format(LR"(
     确定将
　
　{}
　
从家族会员名单中删除吗？

 (确定删除时，请按住
  CTRL键再选择「是」))" /* 原常量参考：0x005875F4。 */, session.WorldScene().guild.orderedRecords[index].name18.c_str());
    return WindowRuntime_CreateAnimation(session, WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3540。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteMiniMail(session::ClientSession& session, const std::int32_t index)
{
    auto& confirmation = session.UiFrame().native.confirmation;
    WindowMail_InitializeDeleteConfirmation(session, &WindowMail_DeleteMiniMailContact);
    confirmation.textCC426C = std::format(LR"(
     确定将
　
　{}
　
从迷你邮件中删除吗？

 (确定删除时，请按住
  CTRL键再选择「是」))" /* 原常量参考：0x00587654。 */, world::detail::ReadMiniMailContact1075A90(session, index).name10);
    return WindowRuntime_CreateAnimation(session, WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A2700。
static void WindowContacts_ClampScroll(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    // 原无效模式/类别读取入口寄存器暂存；不补默认计数或模拟异常结果。
    std::int32_t maximum;
    std::int32_t position;
    const std::int32_t mode = WindowRuntime_Settings(*callbackWindow->ownerSession).contactsPreviewMode;
    if (mode == 0) {
        const std::int32_t category = business.valuesD67B40[6];
        if (business.valuesD67B40[category] < 0) {
            business.valuesD67B40[category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 57;
            if (business.valuesD67B40[0] > maximum) {
                business.valuesD67B40[0] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = callbackWindow->ownerSession->WorldScene().guild.orderedRecordCount;
            maximum = count < 100 ? 97 : count - 3;
            if (business.valuesD67B40[1] > maximum) {
                business.valuesD67B40[1] = maximum;
            }
            break;
        }
        case 2:
            maximum = 17;
            if (business.valuesD67B40[2] > maximum) {
                business.valuesD67B40[2] = maximum;
            }
            break;
        }
        position = business.valuesD67B40[category];
    } else if (mode == 1) {
        const std::int32_t category = business.valuesD67B40[6];
        if (business.valuesD67B40[3 + category] < 0) {
            business.valuesD67B40[3 + category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 51;
            if (business.valuesD67B40[3] > maximum) {
                business.valuesD67B40[3] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = callbackWindow->ownerSession->WorldScene().guild.orderedRecordCount;
            maximum = count < 100 ? 91 : count - 9;
            if (business.valuesD67B40[4] > maximum) {
                business.valuesD67B40[4] = maximum;
            }
            break;
        }
        case 2:
            maximum = 11;
            if (business.valuesD67B40[5] > maximum) {
                business.valuesD67B40[5] = maximum;
            }
            break;
        }
        position = business.valuesD67B40[3 + category];
    }
    auto* const handleVerticalScrollElement = currentWindow->FindElementById(WindowContactsElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        WindowScroll_SetPosition(handleVerticalScrollElement, maximum, position);
    }
    return;
}

// 原函数：0x004A2C60。
static void WindowContacts_Sort(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    for (std::int32_t index = 0; index < 200; ++index) {
        business.mailIndicesC9FCE4[index] = index;
    }
    // 原异常类别的入口寄存器暂存不专项模拟，不补默认项数。
    std::uint32_t count;
    switch (business.valuesD67B40[6]) {
    case 0:
        count = 60;
        break;
    case 1:
        count = 200;
        break;
    case 2:
        count = 20;
        break;
    }
    WindowMail_SortIndices(session, business.mailIndicesC9FCE4.data(), count, WindowContacts_CompareRecords);
}

// 原函数：0x004A2CC0。
static std::int32_t WindowContacts_CompareRecords(session::ClientSession& session, const std::int32_t* const left, const std::int32_t* const right)
{
    auto& business = session.UiFrame().native.business;
    const std::int32_t category = business.valuesD67B40[6];
    if (category == 0 || category == 1 || category == 2) {
        bool leftUsed;
        bool rightUsed;
        if (category == 0) {
            rightUsed = session.WorldScene().mail.data->contacts[*right].used != 0;
            leftUsed = session.WorldScene().mail.data->contacts[*left].used != 0;
        } else if (category == 1) {
            rightUsed = session.WorldScene().guild.orderedRecords[*right].value08 != 0;
            leftUsed = session.WorldScene().guild.orderedRecords[*left].value08 != 0;
        } else {
            rightUsed = world::detail::ReadMiniMailContact1075A90(session, *right).used != 0;
            leftUsed = world::detail::ReadMiniMailContact1075A90(session, *left).used != 0;
        }
        if (leftUsed != rightUsed) {
            return leftUsed ? -1 : 1;
        }
        if (!leftUsed) {
            return 0;
        }
        if (category == 2) {
            const std::int32_t rightIndex = *right;
            const std::int32_t leftIndex = *left;
            const auto* const other = &world::detail::ReadMiniMailContact1075A90(session, rightIndex);
            const auto* const current = &world::detail::ReadMiniMailContact1075A90(session, leftIndex);
            return WindowContacts_CompareMiniMailNumber(current, other);
        }
    }

    const std::uint32_t mode = platform::gContactSortMode_D67CD0;
    if (mode > 4) {
        return 0;
    }
    const std::int32_t rightIndex = *right;
    if (category == 0) {
        const std::int32_t leftIndex = *left;
        const auto* const other = &session.WorldScene().mail.data->contacts[rightIndex];
        const auto* const current = &session.WorldScene().mail.data->contacts[leftIndex];
        switch (mode) {
        case 0:
            return WindowContacts_CompareContactName(current, other);
        case 1:
            return WindowContacts_CompareContactTitle(current, other);
        case 2:
            return WindowContacts_CompareContactValue(current, other);
        case 3:
            return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4:
            return WindowContacts_CompareContactNumber(current, other);
        }
    } else {
        const std::int32_t leftIndex = *left;
        const auto* const other = &session.WorldScene().guild.orderedRecords[rightIndex];
        const auto* const current = &session.WorldScene().guild.orderedRecords[leftIndex];
        switch (mode) {
        case 0:
            return WindowContacts_CompareGuildName(current, other);
        case 1:
            return WindowContacts_CompareGuildValue(current, other);
        case 2:
            return WindowContacts_CompareGuildStatus(current, other);
        case 3:
            return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4:
            return WindowContacts_CompareGuildNumber(current, other);
        }
    }
    return 0;
}

// 原函数：0x004A3010。
std::int32_t WindowContacts_CompareContactName(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const int order = left->name10.compare(right->name10);
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3070。
std::int32_t WindowContacts_CompareGuildName(
    const cg::world::OrderedRecord1051B14* const left,
    const cg::world::OrderedRecord1051B14* const right)
{
    const int order = left->name18.compare(right->name18);
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A30D0。
std::int32_t WindowContacts_CompareGuildValue(
    const cg::world::OrderedRecord1051B14* const left,
    const cg::world::OrderedRecord1051B14* const right)
{
    const std::int32_t leftValue = left->value00;
    const std::int32_t rightValue = right->value00;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3100。
std::int32_t WindowContacts_CompareContactTitle(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const int order = left->text32.compare(right->text32);
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3160。
std::int32_t WindowContacts_CompareContactValue(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const std::int32_t leftValue = left->value08;
    const std::int32_t rightValue = right->value08;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3190。
std::int32_t WindowContacts_CompareGuildStatus(
    const cg::world::OrderedRecord1051B14* const left,
    const cg::world::OrderedRecord1051B14* const right)
{
    const std::int32_t leftValue = left->value10;
    const std::int32_t rightValue = right->value10;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A31C0。
std::int32_t WindowContacts_CompareRecordKey(
    const std::int32_t* const leftValue04, const std::int32_t* const rightValue04)
{
    const std::int32_t leftSequence = *leftValue04;
    const std::int32_t rightSequence = *rightValue04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A31E0。
std::int32_t WindowContacts_CompareContactNumber(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const std::int16_t leftShort = left->value02;
    const std::int32_t leftValue = leftShort == 0 ? 0x7FFFFFFF : leftShort;
    const std::int16_t rightShort = right->value02;
    const std::int32_t rightValue = rightShort == 0 ? 0x7FFFFFFF : rightShort;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3230。
static std::int32_t WindowContacts_CompareMiniMailNumber(
    const world::detail::MiniMailContactRecord* const left,
    const world::detail::MiniMailContactRecord* const right)
{
    const std::int32_t leftValue = std::bit_cast<std::int16_t>(left->sequence02);
    const std::int32_t rightValue = std::bit_cast<std::int16_t>(right->sequence02);
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->key04;
    const std::int32_t rightSequence = right->key04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3260。
std::int32_t WindowContacts_CompareGuildNumber(
    const cg::world::OrderedRecord1051B14* const left,
    const cg::world::OrderedRecord1051B14* const right)
{
    const std::int16_t leftShort = left->value0A;
    const std::int32_t leftValue = leftShort == 0 ? 0x7FFFFFFF : leftShort;
    const std::int16_t rightShort = right->value0A;
    const std::int32_t rightValue = rightShort == 0 ? 0x7FFFFFFF : rightShort;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3820。
static std::int32_t WindowContacts_DragScroll(WindowInstance* callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    // 类别0/1/2均有真实计数；无效类别的入口寄存器值不专项模拟。
    std::int32_t count;
    switch (business.valuesD67B40[6]) {
    case 0:
        count = 60;
        break;
    case 1:
        count = callbackWindow->ownerSession->WorldScene().guild.orderedRecordCount;
        if (count < 100) {
            count = 100;
        }
        break;
    case 2:
        count = 20;
        break;
    }
    auto* elementsWindow = currentWindow;
    auto* const handleVerticalScrollElement = elementsWindow->FindElementById(WindowContactsElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        auto* const mouse = handleVerticalScrollElement->data.mouse;
        if ((mouse->flags & 1) != 0) {
            if (WindowRuntime_Settings(*callbackWindow->ownerSession).contactsPreviewMode == 0) {
                count = WindowScroll_ReadPosition(handleVerticalScrollElement, count - 3);
                const std::int32_t category = business.valuesD67B40[6];
                business.valuesD67B40[category] = count;
            } else {
                count = WindowScroll_ReadPosition(handleVerticalScrollElement, count - 9);
                const std::int32_t category = business.valuesD67B40[6];
                business.valuesD67B40[3 + category] = count;
            }
        }
    }
    return count;
}

} // namespace detail

} // namespace cg::ui
