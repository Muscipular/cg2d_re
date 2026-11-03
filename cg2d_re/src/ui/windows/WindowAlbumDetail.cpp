#include "../WindowRuntimeInternal.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "../../world/AlbumState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowAlbumDetailElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    HandleNavigation = 10,
    HandleNavigation_11 = 11,
    HandleNavigation_12 = 12,
    HandleNavigation_13 = 13,
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
    HandleNavigation_81 = 81,
    HandleSideArea = 82,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00492880。
static std::int32_t WindowAlbumDetail_HandleNavigation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 图鉴详情四种翻页与旋转。

// 原函数：0x00492410。
static std::int32_t WindowAlbumDetail_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr std::array<std::int32_t, 2> kAlbumMarkImages57DF24 {0x3BB0F, 0x3BB0E}; // 原常量参考：0x0057DF24。

static constexpr std::array<std::int32_t, 3> kAlbumMarkImages57DF2C {0, 0x3BB10, 0x3BB11}; // 原常量参考：0x0057DF2C。

static WindowGraphicData gWindowGraphic5679B0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679B0。

static WindowGraphicData gWindowGraphic5679C8 {0x3BB0A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679C8。

static WindowGraphicData gWindowGraphic5679E0 {0x3BB0B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005679E0。

static WindowGraphicData gWindowGraphic5679F8 {0x3BB0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679F8。

static WindowGraphicData gWindowGraphic567A10 {0x3BB0C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A10。

static WindowGraphicData gWindowGraphic567A40 {0x3B55C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A40。

static WindowGraphicData gWindowGraphic567A58 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A58。

static WindowGraphicData gWindowGraphic567A70 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A70。

static WindowGraphicData gWindowGraphic567A88 {0x3B55F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A88。

static WindowGraphicData gWindowGraphic567AA0 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567AA0。

static WindowTextPrototype gWindowText567AB8 { // 原全局：0x00567AB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"宠物名称", // 原常量参考：0x00586048。
};

static WindowTextPrototype gWindowText567AC0 { // 原全局：0x00567AC0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"000", // 原常量参考：0x00585FEC。
};

static WindowTextPrototype gWindowText567AC8 { // 原全局：0x00567AC8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"00", // 原常量参考：0x00585FE8。
};

static WindowTextPrototype gWindowText567AD0 { // 原全局：0x00567AD0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"种类", // 原常量参考：0x00586018。
};

static WindowTextPrototype gWindowText567AD8 { // 原全局：0x00567AD8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"255/255", // 原常量参考：0x00585FE0。
};

static std::uint32_t gWindowAnimationImage567AE0 = 100000; // 原全局：0x00567AE0。

static std::uint32_t gWindowMouseFlagsD677D0 = 0; // 原全局：0x00D677D0。

static WindowElementPrototype gWindowElements567AE8[82] { // 原全局：0x00567AE8。
    {WindowElementType_Graphic, 235, 8, 11, 11, 1, {.graphic = &gWindowGraphic5679B0}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowAlbumDetailElementId::ButtonClose)},
    {WindowElementType_Animation, 186, 135, 0, 0, 1, {.animationImageNo = reinterpret_cast<std::int32_t*>(&gWindowAnimationImage567AE0)}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element2)},
    {WindowElementType_Text, 60, 28, 0, 0, 1, {.text = &gWindowText567AB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element3)},
    {WindowElementType_Text, 214, 28, 0, 0, 1, {.text = &gWindowText567AC0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element4)},
    {WindowElementType_Text, 95, 47, 0, 0, 1, {.text = &gWindowText567AC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element5)},
    {WindowElementType_Text, 63, 63, 0, 0, 1, {.text = &gWindowText567AD0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element6)},
    {WindowElementType_Text, 48, 262, 0, 0, 1, {.text = &gWindowText567AD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element7)},
    {WindowElementType_Graphic, 18, 175, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element8)},
    {WindowElementType_Graphic, 54, 173, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element9)},
    {WindowElementType_Graphic, 18, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A40}, &WindowAlbumDetail_HandleNavigation, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleNavigation)},
    {WindowElementType_Graphic, 38, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A58}, &WindowAlbumDetail_HandleNavigation, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleNavigation_11)},
    {WindowElementType_Graphic, 90, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A70}, &WindowAlbumDetail_HandleNavigation, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleNavigation_12)},
    {WindowElementType_Graphic, 102, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A88}, &WindowAlbumDetail_HandleNavigation, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleNavigation_13)},
    {WindowElementType_Graphic, 31, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element14)},
    {WindowElementType_Graphic, 37, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element15)},
    {WindowElementType_Graphic, 43, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element16)},
    {WindowElementType_Graphic, 49, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element17)},
    {WindowElementType_Graphic, 55, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element18)},
    {WindowElementType_Graphic, 61, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element19)},
    {WindowElementType_Graphic, 67, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element20)},
    {WindowElementType_Graphic, 73, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element21)},
    {WindowElementType_Graphic, 79, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element22)},
    {WindowElementType_Graphic, 85, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element23)},
    {WindowElementType_Graphic, 31, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element24)},
    {WindowElementType_Graphic, 37, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element25)},
    {WindowElementType_Graphic, 43, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element26)},
    {WindowElementType_Graphic, 49, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element27)},
    {WindowElementType_Graphic, 55, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element28)},
    {WindowElementType_Graphic, 61, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element29)},
    {WindowElementType_Graphic, 67, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element30)},
    {WindowElementType_Graphic, 73, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element31)},
    {WindowElementType_Graphic, 79, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element32)},
    {WindowElementType_Graphic, 85, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element33)},
    {WindowElementType_Graphic, 31, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element34)},
    {WindowElementType_Graphic, 37, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element35)},
    {WindowElementType_Graphic, 43, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element36)},
    {WindowElementType_Graphic, 49, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element37)},
    {WindowElementType_Graphic, 55, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element38)},
    {WindowElementType_Graphic, 61, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element39)},
    {WindowElementType_Graphic, 67, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element40)},
    {WindowElementType_Graphic, 73, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element41)},
    {WindowElementType_Graphic, 79, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element42)},
    {WindowElementType_Graphic, 85, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element43)},
    {WindowElementType_Graphic, 31, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element44)},
    {WindowElementType_Graphic, 37, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element45)},
    {WindowElementType_Graphic, 43, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element46)},
    {WindowElementType_Graphic, 49, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element47)},
    {WindowElementType_Graphic, 55, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element48)},
    {WindowElementType_Graphic, 61, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element49)},
    {WindowElementType_Graphic, 67, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element50)},
    {WindowElementType_Graphic, 73, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element51)},
    {WindowElementType_Graphic, 79, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element52)},
    {WindowElementType_Graphic, 85, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element53)},
    {WindowElementType_Graphic, 145, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element54)},
    {WindowElementType_Graphic, 165, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element55)},
    {WindowElementType_Graphic, 185, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element56)},
    {WindowElementType_Graphic, 205, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element57)},
    {WindowElementType_Graphic, 225, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element58)},
    {WindowElementType_Graphic, 145, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element59)},
    {WindowElementType_Graphic, 165, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element60)},
    {WindowElementType_Graphic, 185, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element61)},
    {WindowElementType_Graphic, 205, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element62)},
    {WindowElementType_Graphic, 225, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element63)},
    {WindowElementType_Graphic, 145, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element64)},
    {WindowElementType_Graphic, 165, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element65)},
    {WindowElementType_Graphic, 185, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element66)},
    {WindowElementType_Graphic, 205, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element67)},
    {WindowElementType_Graphic, 225, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element68)},
    {WindowElementType_Graphic, 145, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element69)},
    {WindowElementType_Graphic, 165, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element70)},
    {WindowElementType_Graphic, 185, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element71)},
    {WindowElementType_Graphic, 205, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element72)},
    {WindowElementType_Graphic, 225, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element73)},
    {WindowElementType_Graphic, 145, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element74)},
    {WindowElementType_Graphic, 165, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element75)},
    {WindowElementType_Graphic, 185, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element76)},
    {WindowElementType_Graphic, 205, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element77)},
    {WindowElementType_Graphic, 225, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element78)},
    {WindowElementType_Graphic, 0, 0, 11, 11, 1, {.graphic = &gWindowGraphic5679C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element79)},
    {WindowElementType_Graphic, 13, 26, 11, 11, 1, {.graphic = &gWindowGraphic5679E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumDetailElementId::Element80)},
    {WindowElementType_MouseArea, 131, 59, 114, 110, 1, {.mouseFlags = &gWindowMouseFlagsD677D0}, &WindowAlbumDetail_HandleNavigation, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleNavigation_81)},
    {WindowElementType_EventArea, 256, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowAlbumDetailElementId::HandleSideArea)},
};

// 原型：窗口20；原全局：0x0054E630。
WindowPrototype gWindowPrototypeAlbumDetail {0, 4, 357, 90, 256, 289, 0x80010101u, 82, gWindowElements567AE8, &WindowAlbumDetail_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 20: 54E630

// 窗口函数实现。

// 原评级参数是S32，不是反编译的载荷指针；五个caller均不消费混合返回。
// 原函数：0x004939A0。
static void WindowAlbumDetail_DrawRating(const std::int32_t rating, const std::int32_t base)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(base + slot)); indexedElement) indexedElement->visible = 0;
    }
    const std::int32_t count = rating + 1;
    if (count < 1 || count > 10 || count <= 0) {
        return;
    }
    for (std::int32_t part = 0; part < count; ++part) {
        if (part % 2 == 0) {
            const std::int32_t element = base + part / 2;
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(element)); indexedElement2) indexedElement2->data.graphic->imageNo = 0x3BB0C;
        }
        if (part == count - 1 && count % 2 == 1) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(base + part / 2)); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB0D;
        }
    }
}

// 唯一caller492410不消费原混合返回；保留各消费点独立的角色/记录取样。
// 原函数：0x00492F70。
static void WindowAlbumDetail_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const auto selectedRecord = []() {
        const world::AlbumRecord* const records = world::GetSelectedAlbumRecords();
        const std::int32_t index = gLoginWindowRuntime.valueCB2200;
        return records + index;
    };
    const auto copyBeforeScan = [currentWindow](const std::int32_t index, const std::wstring_view source) {
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(index));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = source;
        }
        return;
    };
    const auto copyAfterScan = [currentWindow](const std::int32_t index, const std::wstring_view source) {
        auto* elementsWindow = currentWindow;
        auto* const dataElement = elementsWindow->FindElementById(NativeElementKey(index));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = source;
        }
        return;
    };
    std::wstring buffer;
    if (selectedRecord()->used != 0) {
        constexpr std::int32_t shown[] {2, 1, 3, 4, 5, 7, 8};
        for (const std::int32_t element : shown) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 1;
        }
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(53 + slot)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(58 + slot)); indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(63 + slot)); indexedElement3) indexedElement3->visible = 1;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(68 + slot)); indexedElement4) indexedElement4->visible = 1;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(73 + slot)); indexedElement5) indexedElement5->visible = 1;
        }
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(13 + slot)); indexedElement) indexedElement->visible = 1;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(23 + slot)); indexedElement2) indexedElement2->visible = 1;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(33 + slot)); indexedElement3) indexedElement3->visible = 1;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(43 + slot)); indexedElement4) indexedElement4->visible = 1;
        }
        const std::int16_t nameCharacter = cg::network::GetGameConnection().LoginRequests().selectedCharacterIndex;
        const std::int32_t nameIndex = gLoginWindowRuntime.valueCB2200;
        const auto& name = world::ClearAlbumNewFlagAndGetName(nameCharacter, nameIndex);
        copyBeforeScan(2, name);

        const std::int32_t image = selectedRecord()->imageNo;
        const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(image);
        auto* const element2 = currentWindow->FindElementById(WindowAlbumDetailElementId::Element2);
        if (element2) {
            auto* const imageData = element2->data.animation;
            cg::yobi::SetYobiActionField2A8(imageData->action, mapped);
        }
        auto* const element22 = currentWindow->FindElementById(WindowAlbumDetailElementId::Element2);
        if (element22) {
            auto* const directionData = element22->data.animation;
            const std::int32_t direction = gLoginWindowRuntime.valueCF4A00;
            cg::yobi::SetYobiActionField2B8(directionData->action, direction);
        }

        buffer = std::format(L"{:3}", gLoginWindowRuntime.valueCB2200 + 1);
        copyAfterScan(3, buffer);
        buffer = std::format(L"{:2}", selectedRecord()->value30);
        copyAfterScan(4, buffer);
        const world::AlbumRecord* const tribeRecord = selectedRecord();
        WindowInstance* const tribeWindow = currentWindow;
        const auto tribe = std::bit_cast<std::int8_t>(tribeRecord->tribe);
        auto* tribeElementsWindow = tribeWindow;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const element6 = tribeElementsWindow->FindElementById(WindowAlbumDetailElementId::Element6);
        if (element6) {
            auto* const tribeData = element6->data.text;
            tribeData->text = tribeName;
        }
        buffer = std::format(L"{:3}/{:3}", gLoginWindowRuntime.valueCB2200 + 1, 960);
        copyBeforeScan(6, buffer);

        const world::AlbumRecord* const firstMarkRecord = selectedRecord();
        WindowInstance* const firstMarkWindow = currentWindow;
        const auto firstMark = std::bit_cast<std::int8_t>(firstMarkRecord->mark1);
        auto* const element8 = firstMarkWindow->FindElementById(WindowAlbumDetailElementId::Element8);
        if (element8) {
            auto* const firstMarkData = element8->data.graphic;
            firstMarkData->imageNo = kAlbumMarkImages57DF24[firstMark];
        }
        WindowInstance* const secondMarkWindow = currentWindow;
        const world::AlbumRecord* const secondMarkRecord = selectedRecord();
        const auto secondMark = std::bit_cast<std::int8_t>(secondMarkRecord->mark2);
        auto* const element9 = secondMarkWindow->FindElementById(WindowAlbumDetailElementId::Element9);
        if (element9) {
            auto* const secondMarkData = element9->data.graphic;
            secondMarkData->imageNo = kAlbumMarkImages57DF2C[secondMark];
        }

        const std::int32_t firstRatingIndex = gLoginWindowRuntime.valueCB2200;
        const world::AlbumRecord* const firstRatingRecords = world::GetSelectedAlbumRecords();
        WindowAlbumDetail_DrawRating(firstRatingRecords[firstRatingIndex].ratings[0], 53);
        for (std::int32_t rating = 1; rating < 5; ++rating) {
            WindowAlbumDetail_DrawRating(selectedRecord()->ratings[rating], 53 + rating * 5);
        }
        const auto showSegment = [currentWindow](const std::int32_t element, const std::int32_t graphic) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->data.graphic->imageNo = graphic;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(element)); indexedElement2) indexedElement2->visible = 1;
        };
        for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
            const std::int32_t base = 13 + attribute * 10;
            for (std::int32_t slot = 0; slot < 10; ++slot) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(base + slot)); indexedElement) indexedElement->visible = 0;
            }
            const std::int32_t segments = selectedRecord()->attributes[attribute];
            const auto& graphics = gPetBarImages57E434[attribute];
            if (segments == 1) {
                showSegment(base, graphics[3]);
            } else {
                for (std::int32_t segment = 0; segment < segments; ++segment) {
                    showSegment(base + segment, graphics[1]);
                    if (segment == 0) {
                        showSegment(base, graphics[0]);
                    }
                    if (segment == segments - 1) {
                        showSegment(base + segment, graphics[2]);
                    }
                }
            }
        }
    } else {
        constexpr std::int32_t hidden[] {2, 1, 4, 5, 7, 8};
        for (const std::int32_t element : hidden) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        }
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(53 + slot)); indexedElement) indexedElement->visible = 0;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(58 + slot)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(63 + slot)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(68 + slot)); indexedElement4) indexedElement4->visible = 0;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(73 + slot)); indexedElement5) indexedElement5->visible = 0;
        }
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(13 + slot)); indexedElement) indexedElement->visible = 0;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(23 + slot)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(33 + slot)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(43 + slot)); indexedElement4) indexedElement4->visible = 0;
        }
        buffer = std::format(L"{:3}", gLoginWindowRuntime.valueCB2200 + 1);
        copyBeforeScan(3, buffer);
        buffer = std::format(L"{:3}/{:3}", gLoginWindowRuntime.valueCB2200 + 1, 960);
        copyBeforeScan(6, buffer);
    }
}

// 原函数：0x00492410。
static std::int32_t WindowAlbumDetail_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueCF4A00 = 5;
    }
    WindowAlbumDetail_Refresh();
    return 1;
}

// 原函数：0x00492880。
static std::int32_t WindowAlbumDetail_HandleNavigation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t delta;
    std::int32_t idle;
    const wchar_t* hint;
    switch (static_cast<WindowAlbumDetailElementId>(elementId)) {
    case WindowAlbumDetailElementId::HandleNavigation:
        delta = -10;
        idle = 0x3B55C;
        hint = L"快速切换至后10页视窗。" /* 原常量参考：0x00586DC8。 */;
        break;
    case WindowAlbumDetailElementId::HandleNavigation_11:
        delta = -1;
        idle = 0x3BE5D;
        hint = L"切换至后一页视窗。" /* 原常量参考：0x00586DB4。 */;
        break;
    case WindowAlbumDetailElementId::HandleNavigation_12:
        delta = 1;
        idle = 0x3BE5A;
        hint = L"切换至前一页视窗。" /* 原常量参考：0x00586DA0。 */;
        break;
    case WindowAlbumDetailElementId::HandleNavigation_13:
        delta = 10;
        idle = 0x3B55F;
        hint = L"快速切换至前10页视窗。" /* 原常量参考：0x00586D88。 */;
        break;
    case WindowAlbumDetailElementId::HandleNavigation_81:
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueCF4A00 + 1;
            gLoginWindowRuntime.valueCF4A00 = direction;
            if (direction > 7) {
                gLoginWindowRuntime.valueCF4A00 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_RRepeatDown) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueCF4A00 - 1;
            gLoginWindowRuntime.valueCF4A00 = direction;
            if (direction < 0) {
                gLoginWindowRuntime.valueCF4A00 = 7;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        return 0;
    default:
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = hint;
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = idle + 2;
        consumed = 1;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = idle;
    }
    if ((state & MouseFlag_LRepeatDown) != 0) {
        const std::int32_t selected = gLoginWindowRuntime.valueCB2200 + delta;
        gLoginWindowRuntime.valueCB2200 = selected;
        if (delta < 0) {
            if (selected < 0) {
                gLoginWindowRuntime.valueCB2200 = 0;
            }
        } else if (selected > 959) {
            gLoginWindowRuntime.valueCB2200 = 959;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = idle + 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
