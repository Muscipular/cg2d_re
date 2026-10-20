#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowGuildDetailsElementId : ElementId {
    HandleInput = 1,
    HandleInput_2 = 2,
    HandleInput_3 = 3,
    HandleInput_4 = 4,
    HandleInput_5 = 5,
    HandleInput_6 = 6,
    HandleInput_7 = 7,
    HandleInput_8 = 8,
    HandleInput_9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
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
    HandleActionButton = 65,
    HandleActionButton_66 = 66,
    HandleScrollButtons = 67,
    HandleScrollButtons_68 = 68,
    Element69 = 69,
    HandleScrollFocus = 70,
    Scroll = 71,
    ButtonClose = 72,
    HandleActionButton_73 = 73,
    Element74 = 74,
    Element75 = 75,
    HandleSideArea = 76,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextD2E130 {}; // 原全局：0x00D2E130。

struct GuildRankDraft {
    std::int32_t flags;
    std::wstring text; // 原成员偏移：+0x04。
    std::array<std::byte, 3> bytes2D; // 原成员偏移：+0x2D。
};

// CF401C+48*33与CF464C是同一草稿文本；仅保留已证字段及间隔，不解释未消费字节。
struct GuildWindowDraft {
    std::array<GuildRankDraft, 32> ranks;
    std::array<std::byte, 4> bytes600; // 原成员偏移：+0x600。
    std::wstring name604; // 原成员偏移：+0x604。
    std::array<std::byte, 7> bytes62D; // 原成员偏移：+0x62D。
    std::wstring secondText634; // 原成员偏移：+0x634。
};

// 回调与内部函数前置声明。

// 原函数：0x004AF960。
static std::uint32_t WindowGuildDetails_RefreshDraft(); // 窗口48管理权限下复制草稿，返回末文本含NUL长度。

// 原函数：0x004AFE20。
static std::int32_t WindowGuildDetails_HasChangedRank(std::int32_t index); // 公会草稿差异；原职级含80H时忽略纯flags变化。

// 原函数：0x004AFEA0。
static std::int32_t WindowGuildDetails_HasChangedRanks(); // 完整扫描32职级，不首个差异早返。

// 原函数：0x004AFB10。
static std::int32_t WindowGuildDetails_SubmitName(); // 公会名称变更校验/转义/Ufm类型2发送。

// 原函数：0x004AFC00。
static std::int32_t WindowGuildDetails_SubmitDescription(); // 公会第二文本变更校验/转义/Ufm类型3发送。

// 原函数：0x004AFCF0。
static std::int32_t WindowGuildDetails_SubmitRanks(); // 先验证全体变化文本，再逐条GY发送并恢复聊天。

// 原函数：0x004AF180。
static void WindowGuildDetails_RefreshReadOnly(); // 权限分流与只读刷新，原混合返回由caller弃用。

// 原函数：0x004AF250。
static void WindowGuildDetails_RefreshEditor(); // 管理模式焦点/草稿/按钮与颜色刷新。

// 原函数：0x004AF650。
static std::int32_t WindowGuildDetails_RefreshRanks(); // 七职级与三附加文本刷新，返回末行Y推进值304。

// 原函数：0x004AFA00。
static void WindowGuildDetails_SaveInputDraft(std::int32_t index); // 输入原型正文写入当前编辑号对应的同源草稿。

// 原函数：0x004AFA60。
static void WindowGuildDetails_ScrollRow(); // 页码原范围0..25与音效、滑块同步。

// 原函数：0x004AFAE0。
static void WindowGuildDetails_DragScroll(std::int32_t unused); // 拖动反求页码；保留主绘制转发的未用实参。

// 原函数：0x004AFED0。
static std::int32_t WindowGuildDetails_CommitFocusedInput(); // 固定窗口/焦点，扫描九个输入载荷。

// 原函数：0x004AEC30。
static std::int32_t WindowGuildDetails_ToggleRankPermission(std::int32_t index, std::int32_t state);

// 原函数：0x004AED40。
static std::int32_t WindowGuildDetails_HandleActionButton(std::int32_t index, std::int32_t state);

// 原函数：0x004AEE00。
static std::int32_t WindowGuildDetails_HandleInput(std::int32_t index, std::int32_t state);

// 原函数：0x004AF000。
static std::int32_t WindowGuildDetails_HandleScrollButtons(std::int32_t index, std::int32_t state);

// 原函数：0x004AF090。
static std::int32_t WindowGuildDetails_HandleScrollFocus(std::int32_t index, std::int32_t state); // WSP01：滚动恰1时调用既有焦点页依赖。

// 原函数：0x004AF0C0。
static std::int32_t WindowGuildDetails_Scroll(std::int32_t unused, std::int32_t state);

// 原函数：0x004AF100。
static std::int32_t WindowGuildDetails_ButtonClose(std::int32_t index, std::int32_t state);

// 原函数：0x004AEAE0。
static std::int32_t WindowGuildDetails_Update(std::int32_t value);

// 原函数：0x004AEC10。
static std::int32_t WindowGuildDetails_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static GuildWindowDraft gGuildDraftCF4018 {}; // 原全局：0x00CF4018。

static std::int32_t gGuildPageD67B78 = 0; // 原全局：0x00D67B78。

static std::int32_t gGuildEditing57F03C = -1; // 原全局：0x0057F03C。

static std::array<WindowInputPrototype, 9> gGuildInputs57F040 {{ // 原全局：0x0057F040。
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        116, 31, 3, 12, 6, {}, 1, 24, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        116, 49, 3, 12, 6, {}, 1, 24, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 165, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 184, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 203, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 222, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 241, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 260, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&cg::ui::detail::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 279, 3, 12, 6, {}, 1, 16, 0, 0},
}};

static const wchar_t* const kGuildPermissionHints[] {
    L"变更此家族称号所属的加入权限。" /* 原常量参考：0x00587AA8。 */, L"变更此家族称号所属的除名权限。" /* 原常量参考：0x00587A88。 */,
    L"变更此家族称号所属的家族兽喂养权限。" /* 原常量参考：0x00587A60。 */, L"变更此家族称号所属的家族仓库使用权限。" /* 原常量参考：0x00587A38。 */, L"变更此家族称号所属的留言板删除权限。" /* 原常量参考：0x00587A10。 */,
};

static WindowGraphicData gWindowGraphic571DF0 {0x3B9FD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571DF0。

static WindowGraphicData gWindowGraphic571E08 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E08。

static WindowGraphicData gWindowGraphic571E20 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E20。

static WindowGraphicData gWindowGraphic571E38 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E38。

static WindowGraphicData gWindowGraphic571E50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E50。

static WindowGraphicData gWindowGraphic571E68 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E68。

static WindowGraphicData gWindowGraphic571EB0 {0x3B9F4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EB0。

static WindowGraphicData gWindowGraphic571EC8 {0x3B9F5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EC8。

static WindowGraphicData gWindowGraphic571EE0 {0x3B9F2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EE0。

static WindowGraphicData gWindowGraphic571EF8 {0x3B9F3, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00571EF8。

static WindowTextPrototype gWindowText571F10 { // 原全局：0x00571F10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guild", // 原常量参考：0x00585F04。
};

static std::uint32_t gWindowMouseFlagsD678A8 = 0; // 原全局：0x00D678A8。

static WindowElementPrototype gWindowElements571F18[76] { // 原全局：0x00571F18。
    {WindowElementType_Input, 116, 31, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput)},
    {WindowElementType_Input, 116, 49, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_2)},
    {WindowElementType_Input, 38, 163, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_3)},
    {WindowElementType_Input, 38, 182, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_4)},
    {WindowElementType_Input, 38, 201, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_5)},
    {WindowElementType_Input, 38, 220, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_6)},
    {WindowElementType_Input, 38, 239, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_7)},
    {WindowElementType_Input, 38, 258, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_8)},
    {WindowElementType_Input, 38, 277, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleInput_9)},
    {WindowElementType_Text, 116, 31, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element10)},
    {WindowElementType_Text, 116, 49, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element11)},
    {WindowElementType_Text, 38, 165, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element12)},
    {WindowElementType_Text, 38, 184, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element13)},
    {WindowElementType_Text, 38, 203, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element14)},
    {WindowElementType_Text, 38, 222, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element15)},
    {WindowElementType_Text, 38, 241, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element16)},
    {WindowElementType_Text, 38, 260, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element17)},
    {WindowElementType_Text, 38, 279, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element18)},
    {WindowElementType_Graphic, 201, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element19)},
    {WindowElementType_Graphic, 220, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element20)},
    {WindowElementType_Graphic, 239, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element21)},
    {WindowElementType_Graphic, 258, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element22)},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element23)},
    {WindowElementType_Graphic, 201, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element24)},
    {WindowElementType_Graphic, 220, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element25)},
    {WindowElementType_Graphic, 239, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element26)},
    {WindowElementType_Graphic, 258, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element27)},
    {WindowElementType_Graphic, 277, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element28)},
    {WindowElementType_Graphic, 201, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element29)},
    {WindowElementType_Graphic, 220, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element30)},
    {WindowElementType_Graphic, 239, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element31)},
    {WindowElementType_Graphic, 258, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element32)},
    {WindowElementType_Graphic, 277, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element33)},
    {WindowElementType_Graphic, 201, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element34)},
    {WindowElementType_Graphic, 220, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element35)},
    {WindowElementType_Graphic, 239, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element36)},
    {WindowElementType_Graphic, 258, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element37)},
    {WindowElementType_Graphic, 277, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element38)},
    {WindowElementType_Graphic, 201, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element39)},
    {WindowElementType_Graphic, 220, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element40)},
    {WindowElementType_Graphic, 239, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element41)},
    {WindowElementType_Graphic, 258, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element42)},
    {WindowElementType_Graphic, 277, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element43)},
    {WindowElementType_Graphic, 201, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element44)},
    {WindowElementType_Graphic, 220, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element45)},
    {WindowElementType_Graphic, 239, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element46)},
    {WindowElementType_Graphic, 258, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element47)},
    {WindowElementType_Graphic, 277, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element48)},
    {WindowElementType_Graphic, 201, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element49)},
    {WindowElementType_Graphic, 220, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element50)},
    {WindowElementType_Graphic, 239, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element51)},
    {WindowElementType_Graphic, 258, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element52)},
    {WindowElementType_Graphic, 277, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element53)},
    {WindowElementType_Graphic, 18, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element54)},
    {WindowElementType_Text, 116, 70, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element55)},
    {WindowElementType_Text, 116, 90, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element56)},
    {WindowElementType_Text, 116, 110, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element57)},
    {WindowElementType_Graphic, 16, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element58)},
    {WindowElementType_Graphic, 16, 180, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element59)},
    {WindowElementType_Graphic, 16, 199, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element60)},
    {WindowElementType_Graphic, 16, 218, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element61)},
    {WindowElementType_Graphic, 16, 237, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element62)},
    {WindowElementType_Graphic, 16, 256, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element63)},
    {WindowElementType_Graphic, 16, 275, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element64)},
    {WindowElementType_Graphic, 273, 28, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleActionButton)},
    {WindowElementType_Graphic, 273, 46, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleActionButton_66)},
    {WindowElementType_Graphic, 299, 163, 11, 11, 1, {.graphic = &gWindowGraphic571E38}, &WindowGuildDetails_HandleScrollButtons, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleScrollButtons)},
    {WindowElementType_Graphic, 299, 280, 11, 11, 1, {.graphic = &gWindowGraphic571E68}, &WindowGuildDetails_HandleScrollButtons, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleScrollButtons_68)},
    {WindowElementType_Graphic, 299, 173, 0, 0, 1, {.graphic = &gWindowGraphic571E50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element69)},
    {WindowElementType_MouseArea, 298, 173, 10, 94, 1, {.mouseFlags = &gWindowMouseFlagsD678A8}, &WindowGuildDetails_HandleScrollFocus, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleScrollFocus)},
    {WindowElementType_EventArea, 0, 0, 317, 325, 1, {.unusedValue = 0}, &WindowGuildDetails_Scroll, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Scroll)},
    {WindowElementType_Graphic, 299, 9, 11, 11, 1, {.graphic = &gWindowGraphic571E08}, &WindowGuildDetails_ButtonClose, -1, static_cast<ElementId>(WindowGuildDetailsElementId::ButtonClose)},
    {WindowElementType_Graphic, 265, 296, 57, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleActionButton_73)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571EE0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element74)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic571EF8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGuildDetailsElementId::Element75)},
    {WindowElementType_EventArea, 317, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowGuildDetailsElementId::HandleSideArea)},
};

// 原型：窗口48；原全局：0x0054EBA0。
WindowPrototype gWindowPrototypeGuildDetails {0, 4, 100, 60, 317, 325, 0x80010101u, 76, gWindowElements571F18, &WindowGuildDetails_Update, &WindowGuildDetails_Draw, &WindowRuntime_ReleaseDefault}; // 48: 54EBA0

// 窗口函数实现。

// 原函数：0x004AEAE0。
static std::int32_t WindowGuildDetails_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (value == -1) {
        gGuildPageD67B78 = 0;
        gGuildEditing57F03C = -1;
        static_cast<void>(WindowGuildDetails_RefreshDraft());
        WindowGuildDetails_RefreshReadOnly();
        gGuildDraftCF4018.name604 = cg::world::GetGuildName();
        gGuildDraftCF4018.secondText634 = cg::world::GetGuildSecondText();
        for (std::int32_t index = 0; index < 32; ++index) {
            const wchar_t* const source = cg::world::GetGuildRankText(index);
            gGuildDraftCF4018.ranks[index].text = source;
        }
        auto* const element10 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element10);
        if (element10) {
            auto* const name = element10->data.text;
            name->text = cg::world::GetGuildName();
        }
        auto* const element11 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element11);
        if (element11) {
            auto* const second = element11->data.text;
            second->text = cg::world::GetGuildSecondText();
        }
    }
    if (WindowGuildDetails_CommitFocusedInput() == 0) {
        gGuildEditing57F03C = -1;
    }
    return 1;
}

// 原函数：0x004AEC10。
static std::int32_t WindowGuildDetails_Draw(const std::int32_t value)
{
    WindowGuildDetails_DragScroll(value);
    WindowGuildDetails_RefreshReadOnly();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004AF180。
static void WindowGuildDetails_RefreshReadOnly()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    if (WindowGuildDetails_ReadRankPermissions() != 0) {
        WindowGuildDetails_RefreshEditor();
        return;
    }
    if (auto* const handleActionButtonElement = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton); handleActionButtonElement) handleActionButtonElement->visible = 0;
    if (auto* const handleActionButton_66Element = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_66); handleActionButton_66Element) handleActionButton_66Element->visible = 0;
    if (auto* const handleActionButton_73Element = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_73); handleActionButton_73Element) handleActionButton_73Element->visible = 0;
    for (std::int32_t index = 9; index < 18; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->mouseCallback = &WindowElement_NoOp;
    }
    for (std::int32_t index = 18; index < 53; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->height = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->width = 0;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement3) indexedElement3->mouseCallback = &WindowElement_NoOp;
    }
    if (auto* const handleActionButton_73Element2 = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_73); handleActionButton_73Element2) handleActionButton_73Element2->visible = 0;
    static_cast<void>(WindowGuildDetails_RefreshRanks());
}

// 原函数：0x004AF250。
static void WindowGuildDetails_RefreshEditor()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    if (auto* const handleActionButtonElement = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton); handleActionButtonElement) handleActionButtonElement->visible = 1;
    if (auto* const handleActionButton_66Element = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_66); handleActionButton_66Element) handleActionButton_66Element->visible = 1;
    if (auto* const handleActionButton_73Element = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_73); handleActionButton_73Element) handleActionButton_73Element->visible = 1;
    for (std::int32_t index = 9; index < 18; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
    }
    for (std::int32_t index = 18; index < 53; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->height = 15;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->width = 15;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(index)); indexedElement3) indexedElement3->mouseCallback = &WindowGuildDetails_ToggleRankPermission;
    }
    static_cast<void>(WindowGuildDetails_RefreshRanks());
    WindowInstance* window = currentWindow;
    auto* elementsWindow = window;
    auto* const handleInputElement = elementsWindow->FindElementById(WindowGuildDetailsElementId::HandleInput);
    if (handleInputElement) {
        if (gFocusedElementDataD45F94 == handleInputElement->data.input) {
            char fieldBytes[41]{};
            cg::text::StoreGbk(fieldBytes, sizeof(fieldBytes), input::ContextText(*gGuildInputs57F040[0].input.context));
            gGuildDraftCF4018.name604 = input::ContextText(*gGuildInputs57F040[0].input.context);
            if (auto* const element10 = window->FindElementById(WindowGuildDetailsElementId::Element10); element10) element10->visible = 0;
            WindowInstance* const current = currentWindow;
            const std::int32_t y = current->y;
            const std::int32_t x = current->x;
            static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[0], x, y, 1));
        } else {
            if (auto* const element10 = elementsWindow->FindElementById(WindowGuildDetailsElementId::Element10); element10) element10->visible = 1;
            auto* const element102 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element10);
            if (element102) {
                auto* const text = element102->data.text;
                text->text = gGuildDraftCF4018.name604;
            }
            const bool changed = gGuildDraftCF4018.name604.compare(cg::world::GetGuildName()) != 0;
            auto* const element103 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element10);
            if (element103) {
                element103->data.text->style.color =
                    changed ? std::byte{6} : std::byte{0};
            }
        }
    }
    const bool nameChanged = gGuildDraftCF4018.name604.compare(cg::world::GetGuildName()) != 0;
    if (auto* const handleActionButtonElement2 = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton); handleActionButtonElement2) handleActionButtonElement2->visible = nameChanged ? 1 : 0;

    window = currentWindow;
    elementsWindow = window;
    auto* const handleInput_2Element = elementsWindow->FindElementById(WindowGuildDetailsElementId::HandleInput_2);
    if (handleInput_2Element) {
        if (gFocusedElementDataD45F94 == handleInput_2Element->data.input) {
            if (auto* const element11 = elementsWindow->FindElementById(WindowGuildDetailsElementId::Element11); element11) element11->visible = 0;
            char fieldBytes[41]{};
            cg::text::StoreGbk(fieldBytes, sizeof(fieldBytes), input::ContextText(*gGuildInputs57F040[1].input.context));
            gGuildDraftCF4018.secondText634 = input::ContextText(*gGuildInputs57F040[1].input.context);
            WindowInstance* const current = currentWindow;
            const std::int32_t y = current->y;
            const std::int32_t x = current->x;
            static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[1], x, y, 1));
        } else {
            if (auto* const element11 = elementsWindow->FindElementById(WindowGuildDetailsElementId::Element11); element11) element11->visible = 1;
            auto* const element112 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element11);
            if (element112) {
                auto* const text = element112->data.text;
                text->text = gGuildDraftCF4018.secondText634;
            }
            const bool changed = gGuildDraftCF4018.secondText634.compare(cg::world::GetGuildSecondText()) != 0;
            auto* const element113 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element11);
            if (element113) {
                element113->data.text->style.color =
                    changed ? std::byte{6} : std::byte{0};
            }
        }
    }
    const bool secondChanged = gGuildDraftCF4018.secondText634.compare(cg::world::GetGuildSecondText()) != 0;
    if (auto* const handleActionButton_66Element2 = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_66); handleActionButton_66Element2) handleActionButton_66Element2->visible = secondChanged ? 1 : 0;

    for (std::int32_t row = 0; row < 7; ++row) {
        window = currentWindow;
        elementsWindow = window;
        auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(row + 2));
        if (indexedElement) {
            if (gFocusedElementDataD45F94 == indexedElement->data.input) {
                if (auto* const indexedElement2 = elementsWindow->FindElementById(NativeElementKey(row + 11)); indexedElement2) indexedElement2->visible = 0;
                WindowGuildDetails_SaveInputDraft(row + 2);
                WindowInstance* const current = currentWindow;
                // 原显示源为row，而草稿写回使用row+2，不合并这两个索引。
                auto* const indexedElement3 = current->FindElementById(NativeElementKey(row + 11));
                if (indexedElement3) {
                    indexedElement3->data.text->text =
                        input::ContextText(*gGuildInputs57F040[row].input.context);
                }
            }
        }
        window = currentWindow;
        const std::int32_t rank = gGuildPageD67B78 + row;
        auto* const textElement = window->FindElementById(NativeElementKey(row + 11));
        if (textElement) {
            auto* const text = textElement->data.text;
            const std::int32_t changed = WindowGuildDetails_HasChangedRank(rank);
            text->style.color = changed != 0 ? std::byte{6} : std::byte{0};
        }
    }
    const std::int32_t changed = WindowGuildDetails_HasChangedRanks();
    if (auto* const handleActionButton_73Element2 = currentWindow->FindElementById(WindowGuildDetailsElementId::HandleActionButton_73); handleActionButton_73Element2) handleActionButton_73Element2->visible = changed;
}

// 原函数：0x004AF650。
static std::int32_t WindowGuildDetails_RefreshRanks()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const std::int32_t editable = WindowGuildDetails_ReadRankPermissions();
    if (auto* const element54 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element54); element54) element54->visible = 0;
    auto* const element10 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element10);
    if (element10) {
        auto* const name = element10->data.text;
        name->text = editable != 0 ? gGuildDraftCF4018.name604.data()
            : cg::world::GetGuildName();
    }
    auto* const element11 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element11);
    if (element11) {
        auto* const second = element11->data.text;
        second->text = editable != 0 ? gGuildDraftCF4018.secondText634.data()
            : cg::world::GetGuildSecondText();
    }
    for (std::int32_t index = 0; index < 3; ++index) {
        WindowInstance* const window = currentWindow;
        const wchar_t* const source = cg::world::GetGuildAuxText(index);
        if (auto* const indexedElement = window->FindElementById(NativeElementKey(index + 54)); indexedElement) indexedElement->data.text->text = source;
    }
    std::int32_t y = 164;
    for (std::int32_t row = 0; row < 7; ++row) {
        const std::int32_t rank = gGuildPageD67B78 + row;
        const wchar_t* const source = editable != 0 ? gGuildDraftCF4018.ranks[rank].text.data()
            : cg::world::GetGuildRankText(rank);
        auto* const textElement = currentWindow->FindElementById(NativeElementKey(row + 11));
        if (textElement) {
            auto* const text = textElement->data.text;
            text->text = source;
        }
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t flags = editable != 0 ? gGuildDraftCF4018.ranks[rank].flags
                : cg::world::ReadGuildRankFlags1051514(rank);
            auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(18 + row * 5 + column));
            if (graphicElement) {
                auto* const graphic = graphicElement->data.graphic;
                graphic->imageNo = (flags & (1 << column)) != 0 ? 0x3B9F6 : 0x3B9FD;
            }
        }
        const std::uint8_t flags = editable != 0
            ? *reinterpret_cast<const unsigned char*>(&gGuildDraftCF4018.ranks[rank].flags)
            : cg::world::ReadGuildRankFlagsByte1051514(rank);
        if ((flags & 0x80) != 0) {
            if (auto* const element542 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element54); element542) element542->visible = 1;
            if (auto* const element543 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element54); element543) element543->x = 23;
            if (auto* const element544 = currentWindow->FindElementById(WindowGuildDetailsElementId::Element54); element544) element544->y = y;
        }
        y += 20;
    }
    return y;
}

// 原函数：0x004AFA00。
static void WindowGuildDetails_SaveInputDraft(const std::int32_t index)
{
    const std::int32_t editing = gGuildEditing57F03C;
    if (editing < 0 || editing > 33 || index < 0 || index >= 9) {
        return;
    }
    // 原4+48*editing：0..31为职级，32为名称，33为会馆名称。
    std::wstring& destination = editing < 32 ? gGuildDraftCF4018.ranks[editing].text
        : editing == 32 ? gGuildDraftCF4018.name604 : gGuildDraftCF4018.secondText634;
    const auto& text = input::ContextText(*gGuildInputs57F040[index].input.context);
    char fieldBytes[41]{};
    cg::text::StoreGbk(fieldBytes, sizeof(fieldBytes), text);
    destination = text;
}

// 原函数：0x004AFED0。
static std::int32_t WindowGuildDetails_CommitFocusedInput()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    void* const focused = gFocusedElementDataD45F94;
    auto* elementsWindow = window;
    for (std::int32_t index = 0; index < 9; ++index) {
        auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(index));
        if (indexedElement) {
            if (indexedElement->data.input == focused) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x004AFAE0。
static void WindowGuildDetails_DragScroll(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    auto* elementsWindow = currentWindow;
    auto* const handleScrollFocusElement = elementsWindow->FindElementById(WindowGuildDetailsElementId::HandleScrollFocus);
    if (handleScrollFocusElement) {
        auto* const mouse = handleScrollFocusElement->data.mouse;
        if ((mouse->flags & 1) != 0) {
            auto* const handleScrollFocusElement2 = elementsWindow->FindElementById(WindowGuildDetailsElementId::HandleScrollFocus);
            if (handleScrollFocusElement2) {
                gGuildPageD67B78 = WindowScroll_ReadPosition(handleScrollFocusElement2, 25);
            }
        }
    }
    return;
}

// 原函数：0x004AFA60。
static void WindowGuildDetails_ScrollRow()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const std::int32_t page = gGuildPageD67B78;
    std::int32_t sound = 51;
    if (page < 0) {
        gGuildPageD67B78 = 0;
        sound = 55;
    } else if (page > 25) {
        gGuildPageD67B78 = 25;
        sound = 55;
    }
    static_cast<void>(audio::PlaySe_52C450(sound, 320));
    WindowInstance* const window = currentWindow;
    const std::int32_t position = gGuildPageD67B78;
    auto* const handleScrollFocusElement = window->FindElementById(WindowGuildDetailsElementId::HandleScrollFocus);
    if (handleScrollFocusElement) {
        static_cast<void>(WindowScroll_SetPosition(handleScrollFocusElement, 25, position));
    }
    return;
}

// 原函数：0x004AEC30。
static std::int32_t WindowGuildDetails_ToggleRankPermission(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t row = (index - 18) / 5;
    const std::int32_t column = (index - 18) % 5;
    if (row == 0 && gGuildPageD67B78 == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kGuildPermissionHints[column];
    }
    if ((state & 2) != 0) {
        const std::int32_t rank = gGuildPageD67B78 + row;
        if (gGuildDraftCF4018.ranks[rank].text.empty()) {
            return 0;
        }
        gGuildDraftCF4018.ranks[rank].flags ^= 1 << column;
        WindowGuildDetails_RefreshEditor();
        return 1;
    }
    return 0;
}

// 原函数：0x004AED40。
static std::int32_t WindowGuildDetails_HandleActionButton(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    if (graphic) graphic->imageNo = 0x3B56E;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B570;
        switch (index) {
        case 64: gHoverTextD68AE8 = L"变更家族名称。" /* 原常量参考：0x00587AC8。 */; break;
        case 65: gHoverTextD68AE8 = L"变更家族会馆名称。" /* 原常量参考：0x00587AD8。 */; break;
        case 72: gHoverTextD68AE8 = L"变更已设定的家族称号。" /* 原常量参考：0x00587AEC。 */; break;
        }
    }
    if ((state & 2) != 0) {
        if (graphic) graphic->imageNo = 0x3B56F;
        switch (index) {
        case 64: static_cast<void>(WindowGuildDetails_SubmitName()); break;
        case 65: static_cast<void>(WindowGuildDetails_SubmitDescription()); break;
        case 72: static_cast<void>(WindowGuildDetails_SubmitRanks()); break;
        }
        if (graphic) graphic->imageNo = 0x3B56F;
        return 1;
    }
    return 0;
}

// 原函数：0x004AEE00。
static std::int32_t WindowGuildDetails_HandleInput(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const std::int32_t editable = WindowGuildDetails_ReadRankPermissions();
    if (editable == 0) {
        return editable;
    }
    WindowInstance* const window = currentWindow;
    auto* elementsWindow = window;
    auto* const indexedElement = elementsWindow->FindElementById(NativeElementKey(index));
    if (indexedElement) {
        if (gFocusedElementDataD45F94 == indexedElement->data.input) {
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[index], x, y, 2));
            return 0;
        }
    }
    if (auto* const indexedElement2 = elementsWindow->FindElementById(NativeElementKey(index + 9)); indexedElement2) indexedElement2->visible = 1;
    if ((state & 2) == 0) {
        return 0;
    }
    switch (index) {
    case 0:
        world::AppendChatMessage_44B050(L"家族名称不得更改喔!!" /* 原常量参考：0x00587B04。 */, 4, 3);
        break;
    case 1: {
        gGuildInputs57F040[1].text = gGuildDraftCF4018.secondText634;
        WindowInstance* const current = currentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[1], x, y, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextD2E130);
        WindowInstance* const focused = currentWindow;
        auto* const indexedElement3 = focused->FindElementById(NativeElementKey(index));
        if (indexedElement3) {
            gFocusedElementDataD45F94 = indexedElement3->data.input;
        }
        auto* const indexedElement4 = focused->FindElementById(NativeElementKey(index));
        if (indexedElement4) {
            indexedElement4->data.input->context =
                &cg::ui::detail::gInputContextD2E130;
        }
        gGuildEditing57F03C = 33;
        break;
    }
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: {
        auto* const sourceElement = currentWindow->FindElementById(NativeElementKey(index + 9));
        if (sourceElement) {
            const std::wstring& source = sourceElement->data.text->text;
            gGuildInputs57F040[index].text = source;
        }
        WindowInstance* const current = currentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[index], x, y, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextD2E130);
        WindowInstance* const focused = currentWindow;
        auto* const indexedElement3 = focused->FindElementById(NativeElementKey(index));
        if (indexedElement3) {
            gFocusedElementDataD45F94 = indexedElement3->data.input;
        }
        auto* const indexedElement4 = focused->FindElementById(NativeElementKey(index));
        if (indexedElement4) {
            indexedElement4->data.input->context =
                &cg::ui::detail::gInputContextD2E130;
        }
        gGuildEditing57F03C = gGuildPageD67B78 + index - 2;
        break;
    }
    }
    return 1;
}

// 原函数：0x004AF000。
static std::int32_t WindowGuildDetails_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t offset = index == 66 ? 0 : 3;
        graphic->imageNo = 0x3B592 + offset;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B594 + offset;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 0x3B593 + offset;
        }
    }
    if ((state & 0x400) != 0) {
        gGuildPageD67B78 += index == 66 ? -1 : 1;
        WindowGuildDetails_ScrollRow();
        static_cast<void>(WindowChat_RestoreInput());
        return 1;
    }
    return 0;
}

// WSP01：滚动结果恰1才调用既有焦点页依赖，保留原S32结果。
// 原函数：0x004AF090。
static std::int32_t WindowGuildDetails_HandleScrollFocus(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result == 1) {
        static_cast<void>(WindowChat_RestoreInput());
    }
    return result;
}

// 原函数：0x004AF0C0。
static std::int32_t WindowGuildDetails_Scroll(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        if (wheel != 0) {
            WindowInstance* const window = currentWindow;
            const std::int32_t page = gGuildPageD67B78;
            auto* const handleScrollFocusElement = window->FindElementById(WindowGuildDetailsElementId::HandleScrollFocus);
            if (handleScrollFocusElement) {
                gGuildPageD67B78 = WindowScroll_ApplyWheel(handleScrollFocusElement, page, 25, wheel);
            }
            static_cast<void>(WindowChat_RestoreInput());
        }
    }
    return 0;
}

// 原函数：0x004AF100。
static std::int32_t WindowGuildDetails_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = 0x3B538;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B53A;
            gHoverTextD68AE8 = L"关闭此视窗" /* 原常量参考：0x005868EC。 */;
        }
        if ((state & 2) != 0) {
            graphic->imageNo = 0x3B539;
            gWindowStates[WindowId_GuildDetails].window->state = 0x8000;
            return 1;
        }
    }
    return 0;
}

// 原函数：0x004AF960。
static std::uint32_t WindowGuildDetails_RefreshDraft()
{
    if (WindowGuildDetails_ReadRankPermissions() == 0) {
        return 0;
    }
    for (std::int32_t index = 0; index < 32; ++index) {
        const std::int32_t flags = cg::world::ReadGuildRankFlags1051514(index);
        const wchar_t* const text = cg::world::GetGuildRankText(index);
        gGuildDraftCF4018.ranks[index].flags = flags;
        gGuildDraftCF4018.ranks[index].text = text;
    }
    gGuildDraftCF4018.name604 = cg::world::GetGuildName();
    const wchar_t* const text = cg::world::GetGuildSecondText();
    const auto length = static_cast<std::uint32_t>(cg::text::ToGbk(text).size() + 1);
    gGuildDraftCF4018.secondText634 = text;
    return length;
}

// 原函数：0x004AFE20。
static std::int32_t WindowGuildDetails_HasChangedRank(const std::int32_t index)
{
    std::int32_t changes = gGuildDraftCF4018.ranks[index].text.compare(cg::world::GetGuildRankText(index)) != 0 ? 1 : 0;
    const std::int32_t flags = cg::world::ReadGuildRankFlags1051514(index);
    if ((flags & 0x80) == 0 && gGuildDraftCF4018.ranks[index].flags != flags) {
        ++changes;
    }
    return changes > 0 ? 1 : 0;
}

// 原函数：0x004AFEA0。
static std::int32_t WindowGuildDetails_HasChangedRanks()
{
    std::int32_t changes = 0;
    for (std::int32_t index = 0; index < 32; ++index) {
        if (WindowGuildDetails_HasChangedRank(index) != 0) {
            ++changes;
        }
    }
    return changes > 0 ? 1 : 0;
}

// 原函数：0x004AFB10。
static std::int32_t WindowGuildDetails_SubmitName()
{
    std::int32_t result = gGuildDraftCF4018.name604.compare(cg::world::GetGuildName());
    if (result == 0) {
        return result;
    }
    result = input::IsValidDelimitedFieldText_476740(gGuildDraftCF4018.name604);
    if (result == 0) {
        return result;
    }
    result = input::ContainsPipeOrBackslash_45E770(gGuildDraftCF4018.name604);
    if (result == 1) {
        return result;
    }
    const std::wstring escapedText = input::EscapeTextAndNotify(gGuildDraftCF4018.name604, 256);
    static_cast<void>(network::SendGuildTextUpdate_511250(
        platform::gConnectionSocket, 2, escapedText));
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    return WindowChat_RestoreInput();
}

// 原函数：0x004AFC00。
static std::int32_t WindowGuildDetails_SubmitDescription()
{
    std::int32_t result = gGuildDraftCF4018.secondText634.compare(cg::world::GetGuildSecondText());
    if (result == 0) {
        return result;
    }
    result = input::IsValidDelimitedFieldText_476740(gGuildDraftCF4018.secondText634);
    if (result == 0) {
        return result;
    }
    result = input::ContainsPipeOrBackslash_45E770(gGuildDraftCF4018.secondText634);
    if (result == 1) {
        return result;
    }
    const std::wstring escapedText = input::EscapeTextAndNotify(gGuildDraftCF4018.secondText634, 256);
    static_cast<void>(network::SendGuildTextUpdate_511250(
        platform::gConnectionSocket, 3, escapedText));
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    return WindowChat_RestoreInput();
}

// 原函数：0x004AFCF0。
static std::int32_t WindowGuildDetails_SubmitRanks()
{
    for (std::int32_t index = 0; index < 32; ++index) {
        const wchar_t* const text = gGuildDraftCF4018.ranks[index].text.data();
        if (WindowGuildDetails_HasChangedRank(index) != 0) {
            const std::int32_t valid = input::IsValidDelimitedFieldText_476740(text);
            if (valid == 0) {
                return valid;
            }
            const std::int32_t rejected = input::ContainsPipeOrBackslash_45E770(text);
            if (rejected == 1) {
                return rejected;
            }
        }
    }
    for (std::int32_t index = 0; index < 32; ++index) {
        const wchar_t* const draft = gGuildDraftCF4018.ranks[index].text.data();
        if (std::wstring_view(cg::world::GetGuildRankText(index)).compare(draft) != 0
            || cg::world::ReadGuildRankFlags1051514(index) != gGuildDraftCF4018.ranks[index].flags) {
            const std::wstring escapedText = input::EscapeTextAndNotify(draft, 256);
            const std::int32_t flags = gGuildDraftCF4018.ranks[index].flags;
            const std::uintptr_t socket = platform::gConnectionSocket;
            static_cast<void>(network::SendGuildRankDefinition_5112C0(socket, index, flags, escapedText));
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    return WindowChat_RestoreInput();
}

// 原函数：0x004A3950。
std::int32_t WindowGuildDetails_ReadRankPermissions()
{
    return WindowGuildDetails_CanEdit(0x80);
}

// 原函数：0x004A3960。
std::int32_t WindowGuildDetails_CanEdit(const std::int32_t mask)
{
    const std::int32_t index = cg::world::ReadGuildTextIndex10514B0();
    if (index == -1) {
        return 0;
    }
    const std::int32_t flags = cg::world::ReadGuildRankFlags1051514(index);
    return (flags & mask) != 0 ? 1 : 0;
}

// 原函数：0x004A3990。
std::int32_t WindowGuildDetails_CanEditRank(const std::int32_t index, const std::int32_t mask)
{
    if (cg::world::ReadGuildTextIndex10514B0() == -1) {
        return 0;
    }
    const std::int32_t flags = cg::world::ReadGuildRankFlags1051514(index);
    return (flags & mask) != 0 ? 1 : 0;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x004AFF00。
std::int32_t WindowGuildDetails_RequestClose()
{
    WindowInstance* const window = detail::gWindowStates[WindowId_GuildDetails].window;
    if (window == nullptr) {
        return 0;
    }
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
}

} // namespace cg::ui
