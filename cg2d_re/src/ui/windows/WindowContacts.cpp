#include "../../scene/login/LoginSavedState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(std::int32_t value); // 窗口21布局、未读扫描与滚动条初始化。

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(std::int32_t value); // 完整滚动/闪烁/内容刷新后绘制窗口21。

// 原函数：0x004A09E0。
static std::int32_t WindowContacts_Refresh(); // 三行/九行联系人完整内容刷新，不调用公共Draw。

// 原函数：0x004A1690。
static void WindowContacts_SetDisplayMode(std::int32_t mode, std::int32_t category); // 显示模式后接名单类别布局。

// 原函数：0x004A16B0。
static void WindowContacts_ApplyDisplayLayout(std::int32_t mode); // 联系人三行/九行显隐，不触碰末三元素。

// 原函数：0x004A1780。
static void WindowContacts_ApplyCategoryLayout(std::int32_t category); // 普通/公会/迷你名单布局，借原型坐标。

// 原函数：0x004A1C30。
static std::int32_t WindowContacts_ScrollRow(std::int32_t index, std::int32_t state); // 单行滚动、边界音效及保存。

// 原函数：0x004A1EF0。
static std::int32_t WindowContacts_HandleNavigation(std::int32_t index, std::int32_t state); // 显示/排序/翻页及三行记录TLKH发送。

// 原函数：0x004A2610。
static std::int32_t WindowContacts_ButtonGroupMail(std::int32_t index, std::int32_t state); // 群发入口：检查49，动画打开22。

// 原函数：0x004A2870。
static std::int32_t WindowContacts_TabCategory(std::int32_t index, std::int32_t state); // 普通/公会/迷你类别切换。

// 原函数：0x004A2BD0。
static std::int32_t WindowContacts_ButtonGuildDetails(std::int32_t index, std::int32_t state); // 动画打开窗口48公会详情。

// 原函数：0x004A37E0。
static std::int32_t WindowContacts_ResolveRecordIndex(std::int32_t index); // 三行/九行元素编号到同源排序后记录索引。

// 原函数：0x004A40C0。
static std::int32_t WindowContacts_Scroll(std::int32_t unused, std::int32_t state); // 滚轮推进，重读类别写回，恒返回0。

// 原函数：0x004A2530。
static std::int32_t WindowContacts_HandleRankButton(std::int32_t index, std::int32_t state); // 六职级按钮，bit400转交完整4A38C0。

// 原函数：0x004A38C0。
static void WindowContacts_OpenRankSelection(std::int32_t index); // 两次联系人索引读取，生产职级状态后直接打开55。

// 原函数：0x004A2B00。
static std::int32_t WindowContacts_OpenMail(std::int32_t index, std::int32_t state); // 发布同源记录/两类别槽，动画打开40。

// 原函数：0x004A3780。
static std::int32_t WindowContacts_ResolveButtonRecord(std::int32_t index); // 三行/九行分组按钮到实际记录索引。

// 原函数：0x004A39C0。
static std::int32_t WindowContacts_HandleAvatar(std::int32_t index, std::int32_t state); // 九行头像悬停及独立TLKH双击发送。

// 原函数：0x004A3C70。
static std::int32_t WindowContacts_ButtonCompose(std::int32_t index, std::int32_t state); // 十二写信按钮，发布类别/索引后动画打开22。

// 原函数：0x004A32E0。
static std::int32_t WindowContacts_ButtonDelete(std::int32_t index, std::int32_t state); // 三类联系人删除确认分派。

// 原函数：0x004A33C0。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteContact(std::int32_t index); // 普通联系人删除确认，窗口47共享上下文。

// 原函数：0x004A3480。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteGuildMember(std::int32_t index); // 公会成员除名确认。

// 原函数：0x004A3540。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteMiniMail(std::int32_t index); // 迷你邮件联系人删除确认。

// 原函数：0x004A2700。
static void WindowContacts_ClampScroll(); // 修正当前名单位置并更新元素17滚动条。

// 原函数：0x004A3820。
static std::int32_t WindowContacts_DragScroll(std::int32_t unused); // 滚动条反求位置；调用后重读类别写回。

// 原函数：0x004A2C60。
static void WindowContacts_Sort(); // 填200项C9FCE4后按类别依原CRT次序排序60/200/20项。

// 原函数：0x004A2CC0。
static std::int32_t WindowContacts_CompareRecords(const std::int32_t* left, const std::int32_t* right); // MC01-C8标准索引比较，有效性对称，同键返回0。

// 原函数：0x004A3230。
static std::int32_t WindowContacts_CompareMiniMailNumber(
    const world::detail::MiniMailContactRecord* left,
    const world::detail::MiniMailContactRecord* right); // 迷你序号按S16比较，零不替换。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(std::int32_t value);

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(std::int32_t value);

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
    {WindowElementType_Graphic, 259, 45, 41, 17, 0, {.graphic = &gWindowGraphic56DA90}, &WindowContacts_ButtonGuildDetails},
    {WindowElementType_Graphic, 19, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 124, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 19, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 124, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 19, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 42, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 42, 33, 0, 0, 1, {.graphic = &gWindowGraphic56D8B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 42, 52, 0, 0, 1, {.graphic = &gWindowGraphic56D8E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 310, 9, 11, 11, 1, {.graphic = &gWindowGraphic56D9A0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 218, 353, 39, 24, 1, {.graphic = &gWindowGraphic56DA30}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 271, 352, 38, 25, 1, {.graphic = &gWindowGraphic56DA48}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 143, 349, 60, 25, 1, {.graphic = &gWindowGraphic56DAD8}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 120, 8, 60, 25, 1, {.graphic = &gWindowGraphic56DAF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 310, 69, 11, 11, 1, {.graphic = &gWindowGraphic56D9B8}, &WindowContacts_ScrollRow},
    {WindowElementType_Graphic, 310, 315, 11, 11, 1, {.graphic = &gWindowGraphic56D9E8}, &WindowContacts_ScrollRow},
    {WindowElementType_Graphic, 310, 80, 10, 14, 1, {.graphic = &gWindowGraphic56D9D0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 309, 80, 11, 235, 1, {.mouseFlags = &gWindowMouseFlagsD67840}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_EventArea, 0, 0, 331, 381, 1, {.unusedValue = 0}, &WindowContacts_Scroll},
    {WindowElementType_Graphic, 219, 45, 19, 19, 1, {.graphic = &gWindowGraphic56D970}, &WindowContacts_ButtonGroupMail},
    {WindowElementType_Text, 200, 30, 0, 0, 1, {.text = &gWindowText56DB30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 142, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 142, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 142, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 222, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 71, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 71, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 87, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 102, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 133, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 222, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 158, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 158, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 174, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 189, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 220, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 222, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 245, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 245, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 261, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 276, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 307, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 67, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 16, 154, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 16, 241, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 242, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 80, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 80, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 110, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 110, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 139, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 139, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 168, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 168, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 197, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 197, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 226, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 226, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 255, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 255, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 284, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 284, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 313, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 313, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 68, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 97, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 126, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 155, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 184, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 213, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 242, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 271, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 300, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56DAA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic56DAC0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 331, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口21；原全局：0x0054E990。
WindowPrototype gWindowPrototypeContacts {0, 4, 2, 64, 331, 381, 0x80010101u, 111, gWindowElements56DB38, &WindowContacts_Update, &WindowContacts_Draw, &WindowRuntime_ReleaseDefault}; // 21: 54E990

// 窗口函数实现。

// 原函数：0x004A0970。
static std::int32_t WindowContacts_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t mode = gValueD67B38;
        const std::uint32_t sortMode = platform::gValueD67CD0;
        gLoginWindowRuntime.mailSortModeD67B3C = sortMode;
        WindowContacts_SetDisplayMode(mode, category);
        static_cast<void>(world::detail::Sub_518DA0());
        WindowContacts_ClampScroll();
    }
    return 1;
}

// 原函数：0x004A09B0。
static std::int32_t WindowContacts_Draw(const std::int32_t value)
{
    static_cast<void>(WindowContacts_DragScroll(value));
    static_cast<void>(WindowMail_AdvanceUnreadBlink());
    static_cast<void>(WindowContacts_Refresh());
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004A09E0。
static std::int32_t WindowContacts_Refresh()
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::uint32_t sortMode = gLoginWindowRuntime.mailSortModeD67B3C;
    auto* const sortGraphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[8].data);
    sortGraphic->imageNo = kMailSortImages54F4C0[category * 5 + sortMode];
    WindowContacts_Sort();
    if (gValueD67B38 == 0) {
        const std::uint32_t page = world::detail::gValueCB1384;
        render::GraphicDescriptor16 descriptor {};
        const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
        world::detail::gValueCB0354 = page;
        if (currentCategory == 1) {
            for (std::int32_t index = 21; index < 27; ++index) {
                const std::int32_t allowed = WindowGuildDetails_ReadRankPermissions();
                gCurrentWindow->elements[index].visible = allowed != 0 ? 1 : 0;
            }
            if (WindowGuildDetails_ReadRankPermissions() != 0) {
                for (std::int32_t row = 0; row < 3; ++row) {
                    const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
                    const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[rowCategory];
                    const std::int32_t memberIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
                    const std::int32_t rank = cg::world::gOrderedRecords1051B14[memberIndex].value00;
                    const std::uint8_t flags = cg::world::ReadGuildRankFlagsByte1051514(rank);
                    gCurrentWindow->elements[21 + row * 2].visible = 0;
                    gCurrentWindow->elements[22 + row * 2].visible = (flags & 0x80) == 0 ? 1 : 0;
                }
            } else {
                for (std::int32_t row = 0; row < 3; ++row) {
                    gCurrentWindow->elements[21 + row * 2].visible = 0;
                    gCurrentWindow->elements[22 + row * 2].visible = 0;
                }
            }
            for (std::int32_t index = 29; index < 46; index += 8) {
                const std::int32_t allowed = WindowGuildDetails_CanEdit(2);
                gCurrentWindow->elements[index].visible = allowed != 0 ? 1 : 0;
            }
            if (cg::world::ReadGuildStatus10514AC() == -1) {
                gCurrentWindow->elements[20].visible = 0;
            } else {
                gCurrentWindow->elements[20].visible = 1;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t status = cg::world::ReadGuildStatus10514AC();
                auto* const data = static_cast<WindowTextData*>(window->elements[20].data);
                WindowMail_WriteGuildStatus(data, status);
            }
        }

        // 原类别未覆盖时沿用未定义/旧借用；正常三类每行只绑定指针，不缓存字段值。
        MailContactView record;
        std::int32_t privileged;
        std::wstring numberText;
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[rowCategory];
            WindowInstance* const entryWindow = gCurrentWindow;
            WindowElement* const entryElements = entryWindow->elements;
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
            entryElements[29 + row * 8].visible = 1;
            gCurrentWindow->elements[28 + row * 8].visible = 1;
            gCurrentWindow->elements[27 + row * 8].visible = 1;
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
                record = WindowMail_MakeContactView(world::detail::gContactsF19FB0[recordIndex]);
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E0;
                break;
            case 1:
                record = WindowMail_MakeContactView(cg::world::gOrderedRecords1051B14[recordIndex]);
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E8;
                break;
            case 2:
                record = WindowMail_MakeContactView(world::detail::ReadMiniMailContact1075A90(recordIndex));
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E0;
                break;
            }
            const std::int32_t textIndex = 30 + row * 8;
            if (*record.used == 0) {
                gCurrentWindow->elements[29 + row * 8].visible = 0;
                gCurrentWindow->elements[28 + row * 8].visible = 0;
                gCurrentWindow->elements[27 + row * 8].visible = 0;
                for (std::int32_t index = textIndex; index < textIndex + 5; ++index) {
                    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
                    data->text = L"";
                }
                continue;
            }
            const std::uint32_t color = 0xFFFFFFFFu;
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            const std::int32_t image = character_select::detail::Sub_51BC50(*record.imageNo);
            const std::int32_t anchorY = kMailImageAnchors56F790[row][1];
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t y = window->y + anchorY - 5;
            const std::int32_t windowX = window->x;
            const std::int32_t anchorX = kMailImageAnchors56F790[row][0];
            static_cast<void>(cg::render::DrawGraphic(
                windowX + anchorX - 7, y, 223, image, 0, &descriptor));
            auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data);
            nameData->text = cg::text::FromGbkWide(record.name);
            cg::text::FormatTo(numberText, L"{:3}" /* 原常量参考：0x00553FCC。 */, *record.number);
            auto* const numberData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 1].data);
            numberData->text = numberText;
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
            case 2: {
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 2].data);
                data->text = cg::text::FromGbkWide(record.text32);
                break;
            }
            case 1: {
                const auto* const member = &cg::world::gOrderedRecords1051B14[recordIndex];
                privileged = WindowGuildDetails_CanEditRank(member->value00, 0x80);
                const std::int32_t rank = member->value00;
                const char* const text = rank == -1
                    ? "" : cg::world::GetGuildRankText(rank);
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 2].data);
                data->text = cg::text::FromGbkWide(text);
                break;
            }
            }
            auto* const textData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 3].data);
            textData->text = cg::text::FromGbkWide(record.text21);
            WindowElement* const statusElements = gCurrentWindow->elements;
            const std::int16_t status = record.ReadStatus();
            auto* const statusData = static_cast<WindowTextData*>(statusElements[textIndex + 4].data);
            WindowMail_WriteContactStatus(record, statusData, status);
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
                if (world::detail::Sub_519090(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            case 1:
                if (WindowGuildDetails_CanEdit(2) != 0) {
                    gCurrentWindow->elements[29 + row * 8].visible = privileged == 0 ? 1 : 0;
                } else {
                    const std::int32_t ownKey = cg::world::ReadGuildOwnKey10514B8();
                    const std::int32_t memberKey = cg::world::gOrderedRecords1051B14[recordIndex].value0C;
                    gCurrentWindow->elements[29 + row * 8].visible = memberKey == ownKey ? 1 : 0;
                }
                if (world::detail::Sub_5190F0(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            case 2:
                if (world::detail::Sub_519170(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            }
        }
        return 108;
    }

    if (gLoginWindowRuntime.valuesD67B40[6] == 1) {
        gCurrentWindow->elements[20].visible = 1;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t status = cg::world::ReadGuildStatus10514AC();
        auto* const data = static_cast<WindowTextData*>(window->elements[20].data);
        WindowMail_WriteGuildStatus(data, status);
    } else {
        gCurrentWindow->elements[20].visible = 0;
    }
    MailContactView record;
    for (std::int32_t row = 0; row < 9; ++row) {
        const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[3 + rowCategory];
        WindowInstance* const entryWindow = gCurrentWindow;
        const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
        entryWindow->elements[56 + row * 5].visible = 1;
        gCurrentWindow->elements[55 + row * 5].visible = 1;
        gCurrentWindow->elements[54 + row * 5].visible = 1;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            record = WindowMail_MakeContactView(world::detail::gContactsF19FB0[recordIndex]);
            gCurrentWindow->elements[56 + row * 5].visible = 1;
            break;
        case 1: {
            const auto* const member = &cg::world::gOrderedRecords1051B14[recordIndex];
            record = WindowMail_MakeContactView(*member);
            if (WindowGuildDetails_CanEdit(2) != 0) {
                const std::int32_t privileged = WindowGuildDetails_CanEditRank(member->value00, 0x80);
                gCurrentWindow->elements[56 + row * 5].visible = privileged == 0 ? 1 : 0;
            } else {
                const std::int32_t memberKey = member->value0C;
                const std::int32_t ownKey = cg::world::ReadGuildOwnKey10514B8();
                gCurrentWindow->elements[56 + row * 5].visible = memberKey == ownKey ? 1 : 0;
            }
            break;
        }
        case 2:
            record = WindowMail_MakeContactView(world::detail::ReadMiniMailContact1075A90(recordIndex));
            break;
        }
        const std::int32_t textIndex = 57 + row * 5;
        if (*record.used == 0) {
            gCurrentWindow->elements[56 + row * 5].visible = 0;
            gCurrentWindow->elements[55 + row * 5].visible = 0;
            gCurrentWindow->elements[54 + row * 5].visible = 0;
            for (std::int32_t index = textIndex; index < textIndex + 2; ++index) {
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
                data->text = L"";
            }
            continue;
        }
        auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data);
        nameData->text = cg::text::FromGbkWide(record.name);
        auto* const statusData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 1].data);
        const std::int16_t status = record.ReadStatus();
        WindowMail_WriteContactStatus(record, statusData, status);
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            if (world::detail::Sub_519090(recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(54 + row * 5);
            }
            break;
        case 1:
            if (world::detail::Sub_5190F0(recordIndex) != 0) {
                const std::int32_t blink = gLoginWindowRuntime.mailBlinkFlagD67B60;
                auto* const graphic = static_cast<WindowGraphicData*>(
                    gCurrentWindow->elements[54 + row * 5].data);
                graphic->imageNo = blink != 0 ? 0x3B549 : 0x3B547;
            }
            break;
        case 2:
            if (world::detail::Sub_519170(recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(54 + row * 5);
            }
            break;
        }
    }
    return 3672;
}

// 原函数：0x004A1690。
static void WindowContacts_SetDisplayMode(const std::int32_t mode, const std::int32_t category)
{
    WindowContacts_ApplyDisplayLayout(mode);
    WindowContacts_ApplyCategoryLayout(category);
}

// 原函数：0x004A16B0。
static void WindowContacts_ApplyDisplayLayout(const std::int32_t mode)
{
    for (std::int32_t index = 0; index < 21; ++index) {
        gCurrentWindow->elements[index].visible = 1;
    }
    if (mode == 0) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[7].data);
        graphic->imageNo = 0x3B612;
        for (std::int32_t index = 21; index < 54; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else if (mode == 1) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[7].data);
        graphic->imageNo = 0x3B611;
        for (std::int32_t index = 21; index < 54; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
    }
}

// 原函数：0x004A1780。
static void WindowContacts_ApplyCategoryLayout(const std::int32_t category)
{
    if (category == 0) {
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 0;
        gCurrentWindow->elements[20].visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x - 24;
        }
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[10].data);
        graphic->imageNo = 0x3B57E;
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else if (category == 1) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[11].data);
        graphic->imageNo = 0x3B581;
        gCurrentWindow->elements[13].visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x;
        }
    } else if (category == 2) {
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 1;
        for (const std::int32_t index : {34, 42, 50, 58, 63, 68, 73, 78, 83, 88, 93, 98, 20}) {
            gCurrentWindow->elements[index].visible = 0;
        }
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data);
        graphic->imageNo = 0x3C116;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x - 24;
        }
        for (const std::int32_t index : {34, 42, 50, 0, 3, 4, 8, 19, 20}) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[5].visible = 1;
        gCurrentWindow->elements[6].visible = 1;
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else {
        return;
    }

    for (const std::int32_t index : {1, 2, 7}) {
        WindowInstance* const xWindow = gCurrentWindow;
        const std::int32_t x = gWindowElements56DB38[index].x;
        xWindow->elements[index].x = x + (category == 2 ? 110 : 0);
        WindowInstance* const yWindow = gCurrentWindow;
        const std::int32_t y = gWindowElements56DB38[index].y;
        yWindow->elements[index].y = y + (category == 2 ? 17 : 0);
    }
}

// 原函数：0x004A1C30。
static std::int32_t WindowContacts_ScrollRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t imageOffset = index == 14 ? 0 : 3;
    graphic->imageNo = 0x3B592 + imageOffset;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594 + imageOffset;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593 + imageOffset;
    }
    if ((state & 0x400) == 0) {
        return 0;
    }

    const std::int32_t mode = gValueD67B38;
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    std::int32_t& position = gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    position += index == 14 ? -1 : 1;
    if ((mode == 0 || mode == 1) && category >= 0 && category <= 2) {
        std::int32_t maximum;
        if (category == 0) {
            maximum = mode == 0 ? 57 : 51;
        } else if (category == 1) {
            const std::int32_t count = cg::world::gOrderedRecordCount1051510;
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
    WindowContacts_ClampScroll();
    login::detail::Sub_520200();
    return 1;
}

// 原函数：0x004A1EF0。
static std::int32_t WindowContacts_HandleNavigation(const std::int32_t index, const std::int32_t state)
{
    if (index >= 51 && index <= 53) {
        if ((state & 4) == 0) {
            return 0;
        }
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            if (world::detail::gContactsF19FB0[recordIndex].used == 0) {
                return 1;
            }
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 0, recordIndex));
            break;
        }
        case 1: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            const auto& record = cg::world::gOrderedRecords1051B14[recordIndex];
            if (record.value08 == 0) {
                return 1;
            }
            const std::int32_t target = record.value0C;
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 1, target));
            break;
        }
        case 2: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
            if (record.used == 0) {
                return 1;
            }
            const std::int32_t target = record.key04;
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 2, target));
            break;
        }
        }
        return 0;
    }
    if (index < 1 || index > 6) {
        return 0;
    }

    const std::int32_t imageOffset = (index & 1) != 0 ? 0 : 3;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558 + imageOffset;
        if (index <= 2) {
            gHoverTextD68AE8 = gValueD67B38 != 0 ? L"切换为详细显示。" /* 原常量参考：0x00587444。 */ : L"切换为预览显示。" /* 原常量参考：0x00587458。 */;
        } else if (index <= 4) {
            gHoverTextD68AE8 = L"切换名片的排序方式。" /* 原常量参考：0x0058746C。 */;
        }
    } else {
        graphic->imageNo = 0x3B556 + imageOffset;
    }

    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        if (index <= 2) {
            const std::int32_t oldMode = gValueD67B38;
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t mode = oldMode == 0 ? 1 : 0;
            gValueD67B38 = mode;
            WindowContacts_SetDisplayMode(mode, category);
            const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[3 + currentCategory] = 0;
            gLoginWindowRuntime.valuesD67B40[currentCategory] = 0;
            WindowContacts_ClampScroll();
        } else if (index <= 4) {
            const std::int32_t oldSort = std::bit_cast<std::int32_t>(gLoginWindowRuntime.mailSortModeD67B3C);
            const std::int32_t sort = (oldSort + (index == 3 ? 4 : 6)) % 5;
            gLoginWindowRuntime.mailSortModeD67B3C = std::bit_cast<std::uint32_t>(sort);
            platform::gValueD67CD0 = std::bit_cast<std::uint32_t>(sort);
        } else {
            const std::int32_t mode = gValueD67B38;
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t step = mode == 0 ? 3 : 9;
            gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category] += index == 5 ? -step : step;
            WindowContacts_ClampScroll();
        }
        login::detail::Sub_520200();
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        // 业务调用可能改变当前窗口，按原尾路径重新借用载荷。
        auto* const pressed = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        pressed->imageNo = 0x3B557 + imageOffset;
    }
    return consumed;
}

// 原函数：0x004A2610。
static std::int32_t WindowContacts_ButtonGroupMail(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B541;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B543;
        gHoverTextD68AE8 = L"对一群人寄送邮件。" /* 原常量参考：0x0058749C。 */;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B542;
        WindowInstance* const window = gWindowStates[WindowId_GroupSelect].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        } else {
            gLoginWindowRuntime.prefixD2F0B8.value10 = 1;
            gLoginWindowRuntime.prefixD2F0B8.value00 = 0;
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B542;
    }
    return consumed;
}

// 原函数：0x004A2870。
static std::int32_t WindowContacts_TabCategory(const std::int32_t index, const std::int32_t state)
{
    if (index < 10 || index > 12) {
        return 0;
    }
    const std::int32_t category = index - 10;
    if (gLoginWindowRuntime.valuesD67B40[6] == category) {
        return 0;
    }
    const std::int32_t idle = category == 0 ? 0x3B57D : (category == 1 ? 0x3B580 : 0x3C115);
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        const wchar_t* const hint = category == 0 ? L"切换至名片视窗。" /* 原常量参考：0x005874B0。 */
            : (category == 1 ? L"切换至家族名簿视窗。" /* 原常量参考：0x005874C4。 */ : L"切换到迷你邮件群组。" /* 原常量参考：0x005874DC。 */);
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idle;
    }
    if ((state & 2) != 0) {
        if (category == 2) {
            auto* const selected = static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data);
            selected->imageNo = 0x3C116;
        } else {
            auto* const title = static_cast<WindowGraphicData*>(gCurrentWindow->elements[108].data);
            title->imageNo = category == 0 ? 0x3B9DE : 0x3B9E3;
        }
        gCurrentWindow->elements[0].visible = category == 1 ? 1 : 0;
        gLoginWindowRuntime.valuesD67B40[6] = category;
        WindowContacts_ClampScroll();
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t mode = gValueD67B38;
        WindowContacts_SetDisplayMode(mode, currentCategory);
        consumed = 1;
    }
    return consumed;
}

// 原函数：0x004A2BD0。
static std::int32_t WindowContacts_ButtonGuildDetails(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B9E5;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9E7;
        gHoverTextD68AE8 = L"开启家族详细资料视窗。" /* 原常量参考：0x00587544。 */;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B9E6;
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_GuildDetails, 1, 0, nullptr));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B9E6;
    }
    return consumed;
}

// 原函数：0x004A37E0。
static std::int32_t WindowContacts_ResolveRecordIndex(const std::int32_t index)
{
    if (gValueD67B38 == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
        return gLoginWindowRuntime.mailIndicesC9FCE4[position + index - 51];
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category];
    return gLoginWindowRuntime.mailIndicesC9FCE4[position + index - 99];
}

// 原函数：0x004A40C0。
static std::int32_t WindowContacts_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) == 0) {
        return 0;
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    std::int32_t count;
    switch (category) {
    case 0:
        count = 60;
        break;
    case 1:
        count = cg::world::gOrderedRecordCount1051510;
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
    const std::int32_t mode = gValueD67B38;
    const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    const std::int32_t result = WindowScroll_ApplyWheel(
        &gCurrentWindow->elements[17], position, count - (mode == 0 ? 3 : 9), wheel);
    const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
    gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + currentCategory] = result;
    return 0;
}

// 原函数：0x004A2530。
static std::int32_t WindowContacts_HandleRankButton(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t parity = (index - 21) % 2;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t imageOffset = parity != 0 ? 3 : 0;
    graphic->imageNo = 0x3B556 + imageOffset;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558 + imageOffset;
        gHoverTextD68AE8 = L"变更此家族成员的称号。" /* 原常量参考：0x00587484。 */;
    }
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        graphic->imageNo = 0x3B557 + imageOffset;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        WindowContacts_OpenRankSelection(index);
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557 + imageOffset;
    }
    return consumed;
}

// 原函数：0x004A38C0。
static void WindowContacts_OpenRankSelection(const std::int32_t index)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t row = (index - 21) / 2;
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
    const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    const std::int32_t key = cg::world::gOrderedRecords1051B14[recordIndex].value0C;
    const std::int32_t memberIndex = cg::world::FindActiveOrderedRecordIndex_5195C0(key);
    const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
    gLoginWindowRuntime.mailMemberIndexCB1D18 = memberIndex;
    const std::int32_t currentPosition = gLoginWindowRuntime.valuesD67B40[currentCategory];
    const std::int32_t currentIndex = gLoginWindowRuntime.mailIndicesC9FCE4[currentPosition + row];
    gLoginWindowRuntime.mailMemberKeyD45F88 = cg::world::gOrderedRecords1051B14[currentIndex].value0C;
    gLoginWindowRuntime.mailRankCF4A14 = cg::world::gOrderedRecords1051B14[memberIndex].value00 - 1;
    static_cast<void>(WindowRuntime_Open(WindowId_GuildRank));
}

// 原函数：0x004A2B00。
static std::int32_t WindowContacts_OpenMail(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B547;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B549;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            gHoverTextD68AE8 = L"察看此名片之主人的邮件纪录" /* 原常量参考：0x005874F4。 */;
            break;
        case 1:
            gHoverTextD68AE8 = L"察看此家族成员的邮件纪录" /* 原常量参考：0x00587510。 */;
            break;
        case 2:
            gHoverTextD68AE8 = L"浏览此迷你邮件的历程。" /* 原常量参考：0x0058752C。 */;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B548;
        world::detail::gValueCF4FC0 = WindowContacts_ResolveButtonRecord(index);
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        gLoginWindowRuntime.mailCategoryD67B64 = category;
        gLoginWindowRuntime.mailCategoryCC4D50 = category;
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailRead, 1, 0, nullptr));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B548;
    }
    return consumed;
}

// 原函数：0x004A3780。
static std::int32_t WindowContacts_ResolveButtonRecord(const std::int32_t index)
{
    if (gValueD67B38 == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t row = (index - 27) / 8;
        const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
        return gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    }
    const std::int32_t row = (index - 54) / 5;
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category];
    return gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
}

// 原函数：0x004A39C0。
static std::int32_t WindowContacts_HandleAvatar(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category] + index - 99;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t* recordImage = nullptr;
    switch (category) {
    case 0:
        if (position < 0 || position > 59) {
            return 0;
        }
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = world::detail::gContactsF19FB0[recordIndex];
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    case 1:
        if (position < 0) {
            return 0;
        }
        if (position > cg::world::gOrderedRecordCount1051510 - 1) {
            return 0;
        }
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = cg::world::gOrderedRecords1051B14[recordIndex];
            if (record.value08 != 0) {
                recordImage = &record.value14;
            }
        }
        break;
    case 2:
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    }
    std::int32_t consumed = 0;
    if (recordImage != nullptr) {
        WindowInstance* const firstWindow = gCurrentWindow;
        const std::uint32_t color = 0xFFFFFFFFu;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        WindowElement* const firstElements = firstWindow->elements;
        const std::int32_t firstWindowY = firstWindow->y;
        const std::int32_t firstElementY = firstElements[index].y;
        const std::int32_t firstElementX = firstElements[index].x;
        const std::int32_t firstY = firstElementY + firstWindowY + 55;
        const std::int32_t firstWindowX = firstWindow->x;
        static_cast<void>(cg::render::DrawGraphic(
            firstElementX + firstWindowX + 182, firstY, 223, 0x3BD32, 0, &descriptor));

        // 先固定第一次绘图后的元素；图号转换后仅重取当前窗口，不更换此元素。
        WindowElement* const element = &gCurrentWindow->elements[index];
        const std::int32_t image = character_select::detail::Sub_51BC50(*recordImage);
        const std::int32_t elementY = element->y;
        WindowInstance* const currentWindow = gCurrentWindow;
        const std::int32_t windowY = currentWindow->y;
        const std::int32_t y = elementY + windowY + 55;
        const std::int32_t elementX = element->x;
        const std::int32_t windowX = currentWindow->x;
        static_cast<void>(cg::render::DrawGraphic(
            elementX + windowX + 182, y, 223, image, 0, &descriptor));
        consumed = 1;
    }
    if ((state & 4) == 0) {
        return consumed;
    }
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
    case 0: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        if (world::detail::gContactsF19FB0[recordIndex].used == 0) {
            return 1;
        }
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 0, recordIndex));
        break;
    }
    case 1: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        const auto& record = cg::world::gOrderedRecords1051B14[recordIndex];
        if (record.value08 == 0) {
            return 1;
        }
        const std::int32_t target = record.value0C;
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 1, target));
        break;
    }
    case 2: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t target = record.key04;
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 2, target));
        break;
    }
    }
    return consumed;
}

// 原函数：0x004A3C70。
static std::int32_t WindowContacts_ButtonCompose(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    graphic->imageNo = hover ? 0x3B546 : 0x3B544;
    const std::int32_t consumed = hover ? 1 : 0;
    bool nineRows;
    std::int32_t row;
    switch (index) {
    case 28: nineRows = false; row = 0; break;
    case 36: nineRows = false; row = 1; break;
    case 44: nineRows = false; row = 2; break;
    case 55: nineRows = true; row = 0; break;
    case 60: nineRows = true; row = 1; break;
    case 65: nineRows = true; row = 2; break;
    case 70: nineRows = true; row = 3; break;
    case 75: nineRows = true; row = 4; break;
    case 80: nineRows = true; row = 5; break;
    case 85: nineRows = true; row = 6; break;
    case 90: nineRows = true; row = 7; break;
    case 95: nineRows = true; row = 8; break;
    default: return consumed;
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    if (!nineRows && (category < 0 || category > 2)) {
        return consumed;
    }
    if (hover) {
        switch (category) {
        case 0:
            gHoverTextD68AE8 = L"寄送邮件给此名片的主人。" /* 原常量参考：0x005876B0。 */;
            break;
        case 1:
            gHoverTextD68AE8 = L"对此家族成员寄送邮件。" /* 原常量参考：0x005876CC。 */;
            break;
        case 2:
            gHoverTextD68AE8 = L"传送迷你邮件。" /* 原常量参考：0x005876E4。 */;
            break;
        }
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    gLoginWindowRuntime.prefixD2F0B8.value10 = 0;
    gLoginWindowRuntime.prefixD2F0B8.value00 = category;
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[(nineRows ? 3 : 0) + category];
    gLoginWindowRuntime.prefixD2F0B8.value04 = gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
    return 1;
}

// 原函数：0x004A32E0。
static std::int32_t WindowContacts_ButtonDelete(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B54A;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B54C;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            gHoverTextD68AE8 = L"删除此名片。" /* 原常量参考：0x0058755C。 */;
            break;
        case 1:
            gHoverTextD68AE8 = L"对此家族成员进行除名。" /* 原常量参考：0x0058756C。 */;
            break;
        case 2:
            gHoverTextD68AE8 = L"删除此迷你邮件的名片。" /* 原常量参考：0x00587584。 */;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t recordIndex = WindowContacts_ResolveButtonRecord(index);
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        gLoginWindowRuntime.mailDeleteIndexD2DA8C = recordIndex;
        switch (category) {
        case 0: static_cast<void>(WindowContacts_ConfirmDeleteContact(recordIndex)); break;
        case 1: static_cast<void>(WindowContacts_ConfirmDeleteGuildMember(recordIndex)); break;
        case 2: static_cast<void>(WindowContacts_ConfirmDeleteMiniMail(recordIndex)); break;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B54B;
    }
    return consumed;
}

// 原函数：0x004A33C0。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteContact(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteContact);
    gWindow47Runtime.textCC426C = std::format(L"" LR"(
)" L"     确定将" LR"(
)" L"　" LR"(
)" L"　{}" LR"(
)" L"　" LR"(
)" L"　从名片里删除吗？" LR"(
)" L" " LR"(
)" L" (确定删除时，请按住" LR"(
)" L"  CTRL键再选择「是」)" /* 原常量参考：0x0058759C。 */, cg::text::FromGbkWide(world::detail::gContactsF19FB0[index].name10.data()).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3480。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteGuildMember(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteGuildMember);
    gWindow47Runtime.textCC426C = std::format(LR"(
     确定将
　
　{}
　
从家族会员名单中删除吗？

 (确定删除时，请按住
  CTRL键再选择「是」))" /* 原常量参考：0x005875F4。 */, cg::text::FromGbkWide(cg::world::gOrderedRecords1051B14[index].text18).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3540。
static cg::yobi::YobiAction* WindowContacts_ConfirmDeleteMiniMail(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteMiniMailContact);
    gWindow47Runtime.textCC426C = std::format(LR"(
     确定将
　
　{}
　
从迷你邮件中删除吗？

 (确定删除时，请按住
  CTRL键再选择「是」))" /* 原常量参考：0x00587654。 */, cg::text::FromGbkWide(world::detail::ReadMiniMailContact1075A90(index).name10.data()).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A2700。
static void WindowContacts_ClampScroll()
{
    // 原无效模式/类别读取入口寄存器暂存；不补默认计数或模拟异常结果。
    std::int32_t maximum;
    std::int32_t position;
    const std::int32_t mode = gValueD67B38;
    if (mode == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        if (gLoginWindowRuntime.valuesD67B40[category] < 0) {
            gLoginWindowRuntime.valuesD67B40[category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 57;
            if (gLoginWindowRuntime.valuesD67B40[0] > maximum) {
                gLoginWindowRuntime.valuesD67B40[0] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = cg::world::gOrderedRecordCount1051510;
            maximum = count < 100 ? 97 : count - 3;
            if (gLoginWindowRuntime.valuesD67B40[1] > maximum) {
                gLoginWindowRuntime.valuesD67B40[1] = maximum;
            }
            break;
        }
        case 2:
            maximum = 17;
            if (gLoginWindowRuntime.valuesD67B40[2] > maximum) {
                gLoginWindowRuntime.valuesD67B40[2] = maximum;
            }
            break;
        }
        position = gLoginWindowRuntime.valuesD67B40[category];
    } else if (mode == 1) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        if (gLoginWindowRuntime.valuesD67B40[3 + category] < 0) {
            gLoginWindowRuntime.valuesD67B40[3 + category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 51;
            if (gLoginWindowRuntime.valuesD67B40[3] > maximum) {
                gLoginWindowRuntime.valuesD67B40[3] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = cg::world::gOrderedRecordCount1051510;
            maximum = count < 100 ? 91 : count - 9;
            if (gLoginWindowRuntime.valuesD67B40[4] > maximum) {
                gLoginWindowRuntime.valuesD67B40[4] = maximum;
            }
            break;
        }
        case 2:
            maximum = 11;
            if (gLoginWindowRuntime.valuesD67B40[5] > maximum) {
                gLoginWindowRuntime.valuesD67B40[5] = maximum;
            }
            break;
        }
        position = gLoginWindowRuntime.valuesD67B40[3 + category];
    }
    WindowScroll_SetPosition(&gCurrentWindow->elements[17], maximum, position);
}

// 原函数：0x004A2C60。
static void WindowContacts_Sort()
{
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailIndicesC9FCE4[index] = index;
    }
    // 原异常类别的入口寄存器暂存不专项模拟，不补默认项数。
    std::uint32_t count;
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
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
    WindowMail_SortIndices(gLoginWindowRuntime.mailIndicesC9FCE4.data(), count, WindowContacts_CompareRecords);
}

// 原函数：0x004A2CC0。
static std::int32_t WindowContacts_CompareRecords(const std::int32_t* const left, const std::int32_t* const right)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    if (category == 0 || category == 1 || category == 2) {
        bool leftUsed;
        bool rightUsed;
        if (category == 0) {
            rightUsed = world::detail::gContactsF19FB0[*right].used != 0;
            leftUsed = world::detail::gContactsF19FB0[*left].used != 0;
        } else if (category == 1) {
            rightUsed = cg::world::gOrderedRecords1051B14[*right].value08 != 0;
            leftUsed = cg::world::gOrderedRecords1051B14[*left].value08 != 0;
        } else {
            rightUsed = world::detail::ReadMiniMailContact1075A90(*right).used != 0;
            leftUsed = world::detail::ReadMiniMailContact1075A90(*left).used != 0;
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
            const auto* const other = &world::detail::ReadMiniMailContact1075A90(rightIndex);
            const auto* const current = &world::detail::ReadMiniMailContact1075A90(leftIndex);
            return WindowContacts_CompareMiniMailNumber(current, other);
        }
    }

    const std::uint32_t mode = platform::gValueD67CD0;
    if (mode > 4) {
        return 0;
    }
    const std::int32_t rightIndex = *right;
    if (category == 0) {
        const std::int32_t leftIndex = *left;
        const auto* const other = &world::detail::gContactsF19FB0[rightIndex];
        const auto* const current = &world::detail::gContactsF19FB0[leftIndex];
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
        const auto* const other = &cg::world::gOrderedRecords1051B14[rightIndex];
        const auto* const current = &cg::world::gOrderedRecords1051B14[leftIndex];
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
    const int order = std::strcmp(left->name10.data(), right->name10.data());
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
    const int order = std::strcmp(left->text18, right->text18);
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
    const int order = std::strcmp(left->text32.data(), right->text32.data());
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
static std::int32_t WindowContacts_DragScroll(const std::int32_t)
{
    // 类别0/1/2均有真实计数；无效类别的入口寄存器值不专项模拟。
    std::int32_t count;
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
    case 0:
        count = 60;
        break;
    case 1:
        count = cg::world::gOrderedRecordCount1051510;
        if (count < 100) {
            count = 100;
        }
        break;
    case 2:
        count = 20;
        break;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const mouse = static_cast<WindowMouseData*>(elements[17].data);
    if ((mouse->flags & 1) != 0) {
        if (gValueD67B38 == 0) {
            count = WindowScroll_ReadPosition(&elements[17], count - 3);
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[category] = count;
        } else {
            count = WindowScroll_ReadPosition(&elements[17], count - 9);
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[3 + category] = count;
        }
    }
    return count;
}

} // namespace detail

} // namespace cg::ui
