#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPlayerProfileElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    ButtonClose = 18,
    Element19 = 19,
    HandleButtons = 20,
    HandleButtons_21 = 21,
    HandleButtons_22 = 22,
    Element23 = 23,
    HandleButtons_24 = 24,
    HandleButtons_25 = 25,
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
    HandleSideArea = 73,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00474BE0。
static std::int32_t WindowPlayerProfile_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口7资料切页及前后排请求。

// 原函数：0x00474310。
static void WindowPlayerProfile_Release(WindowInstance&);

// 原函数：0x00474540。
static std::int32_t WindowPlayerProfile_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic55F780 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F780。

static WindowGraphicData gWindowGraphic55F798 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F798。

static WindowGraphicData gWindowGraphic55F7B0 {0x3BAC5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7B0。

static WindowGraphicData gWindowGraphic55F7C8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7C8。

static WindowGraphicData gWindowGraphic55F7E0 {0x3BAB6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7E0。

static WindowGraphicData gWindowGraphic55F7F8 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7F8。

static WindowGraphicData gWindowGraphic55F810 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F810。

static WindowGraphicData gWindowGraphic55F828 {0x3BAC0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F828。

static WindowGraphicData gWindowGraphic55F840 {0x3BABD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F840。

static WindowGraphicData gWindowGraphic55F858 {0x3BABB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F858。

static WindowGraphicData gWindowGraphic55F870 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F870。

static WindowGraphicData gWindowGraphic55F960 {0x3BACC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F960。

static WindowGraphicData gWindowGraphic55FA50 {0x3BAD1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FA50。

static WindowGraphicData gWindowGraphic55FB40 {0x3BAD6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FB40。

static WindowGraphicData gWindowGraphic55FC30 {0x3BAC6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC30。

static WindowGraphicData gWindowGraphic55FC48 {0x3BACB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC48。

static WindowGraphicData gWindowGraphic55FC60 {0x3BAD0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC60。

static WindowGraphicData gWindowGraphic55FC78 {0x3BAD5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC78。

static WindowGraphicData gWindowGraphic55FC90 {0x3BAB2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC90。

static WindowGraphicData gWindowGraphic55FCA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCA8。

static WindowGraphicData gWindowGraphic55FCC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x0055FCC0。

static WindowGraphicData gWindowGraphic55FCD8 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCD8。

static WindowTextPrototype gWindowText55FCF0 { // 原全局：0x0055FCF0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText55FCF8 { // 原全局：0x0055FCF8。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText55FD00 { // 原全局：0x0055FD00。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"x5", // 原常量参考：0x00586224。
};

static WindowTextPrototype gWindowText55FD08 { // 原全局：0x0055FD08。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"职业", // 原常量参考：0x0058621C。
};

static WindowTextPrototype gWindowText55FD10 { // 原全局：0x0055FD10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00586214。
};

static WindowTextPrototype gWindowText55FD18 { // 原全局：0x0055FD18。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族称号", // 原常量参考：0x00586208。
};

static WindowTextPrototype gWindowText55FD20 { // 原全局：0x0055FD20。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"特殊称号", // 原常量参考：0x005861FC。
};

static WindowTextPrototype gWindowText55FD28 { // 原全局：0x0055FD28。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自定称号", // 原常量参考：0x005861F0。
};

static WindowTextPrototype gWindowText55FD30 { // 原全局：0x0055FD30。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"种族", // 原常量参考：0x005861E8。
};

static WindowTextPrototype gWindowText55FD38 { // 原全局：0x0055FD38。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"经验值", // 原常量参考：0x005861E0。
};

static WindowTextPrototype gWindowText55FD40 { // 原全局：0x0055FD40。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"下一级经验值", // 原常量参考：0x005861D0。
};

static WindowTextPrototype gWindowText55FD48 { // 原全局：0x0055FD48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"生命值", // 原常量参考：0x005861C8。
};

static WindowTextPrototype gWindowText55FD50 { // 原全局：0x0055FD50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

static WindowTextPrototype gWindowText55FD58 { // 原全局：0x0055FD58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"对战胜利值", // 原常量参考：0x005861B4。
};

static WindowElementPrototype gWindowElements55FD60[73] { // 原全局：0x0055FD60。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText55FCF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element1)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText55FCF8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element2)},
    {WindowElementType_Text, 70, 101, 0, 0, 1, {.text = &gWindowText55FD00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element3)},
    {WindowElementType_Text, 165, 31, 0, 0, 1, {.text = &gWindowText55FD08}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element4)},
    {WindowElementType_Text, 165, 49, 0, 0, 1, {.text = &gWindowText55FD10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element5)},
    {WindowElementType_Text, 165, 68, 0, 0, 1, {.text = &gWindowText55FD18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element6)},
    {WindowElementType_Text, 165, 86, 0, 0, 1, {.text = &gWindowText55FD20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element7)},
    {WindowElementType_Text, 165, 104, 0, 0, 1, {.text = &gWindowText55FD28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element8)},
    {WindowElementType_Text, 89, 123, 0, 0, 1, {.text = &gWindowText55FD30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element9)},
    {WindowElementType_Text, 89, 141, 0, 0, 1, {.text = &gWindowText55FD38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element10)},
    {WindowElementType_Text, 89, 158, 0, 0, 1, {.text = &gWindowText55FD40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element11)},
    {WindowElementType_Text, 89, 176, 0, 0, 1, {.text = &gWindowText55FD48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element12)},
    {WindowElementType_Text, 89, 194, 0, 0, 1, {.text = &gWindowText55FD50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element13)},
    {WindowElementType_Text, 89, 211, 0, 0, 1, {.text = &gWindowText55FD58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element14)},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic55F780}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element15)},
    {WindowElementType_Graphic, 48, 103, 0, 0, 1, {.graphic = &gWindowGraphic55F798}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element16)},
    {WindowElementType_Graphic, 60, 104, 0, 0, 1, {.graphic = &gWindowGraphic55F7B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element17)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic55F7C8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPlayerProfileElementId::ButtonClose)},
    {WindowElementType_Graphic, 151, 230, 0, 0, 1, {.graphic = &gWindowGraphic55F7E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element19)},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic55F7F8}, &WindowPlayerProfile_HandleButtons, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons)},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic55F810}, &WindowPlayerProfile_HandleButtons, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons_21)},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic55FCD8}, &WindowPlayerProfile_HandleButtons, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons_22)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F858}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element23)},
    {WindowElementType_Graphic, 217, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F828}, &WindowPlayerProfile_HandleButtons, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons_24)},
    {WindowElementType_Graphic, 253, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F840}, &WindowPlayerProfile_HandleButtons, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons_25)},
    {WindowElementType_Graphic, 229, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element26)},
    {WindowElementType_Graphic, 237, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element27)},
    {WindowElementType_Graphic, 245, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element28)},
    {WindowElementType_Graphic, 253, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element29)},
    {WindowElementType_Graphic, 261, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element30)},
    {WindowElementType_Graphic, 269, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element31)},
    {WindowElementType_Graphic, 277, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element32)},
    {WindowElementType_Graphic, 285, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element33)},
    {WindowElementType_Graphic, 293, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element34)},
    {WindowElementType_Graphic, 301, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element35)},
    {WindowElementType_Graphic, 229, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element36)},
    {WindowElementType_Graphic, 237, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element37)},
    {WindowElementType_Graphic, 245, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element38)},
    {WindowElementType_Graphic, 253, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element39)},
    {WindowElementType_Graphic, 261, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element40)},
    {WindowElementType_Graphic, 269, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element41)},
    {WindowElementType_Graphic, 277, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element42)},
    {WindowElementType_Graphic, 285, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element43)},
    {WindowElementType_Graphic, 293, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element44)},
    {WindowElementType_Graphic, 301, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element45)},
    {WindowElementType_Graphic, 229, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element46)},
    {WindowElementType_Graphic, 237, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element47)},
    {WindowElementType_Graphic, 245, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element48)},
    {WindowElementType_Graphic, 253, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element49)},
    {WindowElementType_Graphic, 261, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element50)},
    {WindowElementType_Graphic, 269, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element51)},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element52)},
    {WindowElementType_Graphic, 285, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element53)},
    {WindowElementType_Graphic, 293, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element54)},
    {WindowElementType_Graphic, 301, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element55)},
    {WindowElementType_Graphic, 229, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element56)},
    {WindowElementType_Graphic, 237, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element57)},
    {WindowElementType_Graphic, 245, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element58)},
    {WindowElementType_Graphic, 253, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element59)},
    {WindowElementType_Graphic, 261, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element60)},
    {WindowElementType_Graphic, 269, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element61)},
    {WindowElementType_Graphic, 277, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element62)},
    {WindowElementType_Graphic, 285, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element63)},
    {WindowElementType_Graphic, 293, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element64)},
    {WindowElementType_Graphic, 301, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element65)},
    {WindowElementType_Graphic, 195, 130, 0, 0, 1, {.graphic = &gWindowGraphic55FC30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element66)},
    {WindowElementType_Graphic, 195, 147, 0, 0, 1, {.graphic = &gWindowGraphic55FC48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element67)},
    {WindowElementType_Graphic, 195, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FC60}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element68)},
    {WindowElementType_Graphic, 195, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FC78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element69)},
    {WindowElementType_Graphic, 16, 29, 0, 0, 1, {.graphic = &gWindowGraphic55FC90}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element70)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55FCA8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element71)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic55FCC0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerProfileElementId::Element72)},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPlayerProfileElementId::HandleSideArea)},
};

// 原型：窗口7；原全局：0x0054E360。
WindowPrototype gWindowPrototypePlayerProfile {0, 4, 15, 97, 315, 261, 0x80000080u, 73, gWindowElements55FD60, &WindowPlayerProfile_Update, &WindowRuntime_DrawDefault, &WindowPlayerProfile_Release}; // 7: 54E360

// 窗口函数实现。

// 原474FD1..4753A6：每组先清显隐后读属性，图号与显隐各自重取窗口。
static void WindowPlayerProfile_RefreshAttributeBar(
    const std::int32_t attribute, const std::int32_t firstElement,
    const std::int32_t middleImage, const std::int32_t firstImage,
    const std::int32_t lastImage, const std::int32_t singleImage)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    for (std::int32_t index = 0; index < 10; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(firstElement + index)); indexedElement) indexedElement->visible = 0;
    }
    const std::int32_t count = world::gPlayerRuntimeSlots.valuesE4[static_cast<std::int32_t>(attribute)] / 10;
    if (count == 1) {
        auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(firstElement));
        if (graphicElement) {
            auto* const graphic =
                graphicElement->data.graphic;
            graphic->imageNo = singleImage;
        }
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(firstElement)); indexedElement) indexedElement->visible = 1;
    } else {
        for (std::int32_t index = 0; index < count; ++index) {
            auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(firstElement + index));
            if (graphicElement) {
                auto* const graphic = graphicElement->data.graphic;
                graphic->imageNo = middleImage;
            }
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(firstElement + index)); indexedElement) indexedElement->visible = 1;
            if (index == 0) {
                auto* const firstElement2 = currentWindow->FindElementById(NativeElementKey(firstElement));
                if (firstElement2) {
                    auto* const first = firstElement2->data.graphic;
                    first->imageNo = firstImage;
                }
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(firstElement)); indexedElement2) indexedElement2->visible = 1;
            }
            if (index == count - 1) {
                auto* const lastElement = currentWindow->FindElementById(NativeElementKey(firstElement + index));
                if (lastElement) {
                    auto* const last = lastElement->data.graphic;
                    last->imageNo = lastImage;
                }
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(firstElement + index)); indexedElement2) indexedElement2->visible = 1;
            }
        }
    }
}

// 原474BE0：五按钮及默认分支；位置请求不写玩家状态，返回仍为悬停值。
// 原函数：0x00474BE0。
static std::int32_t WindowPlayerProfile_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t idleImage;
    std::int32_t hoverImage;
    std::int32_t targetWindow = 0;
    const wchar_t* hint;
    switch (static_cast<WindowPlayerProfileElementId>(elementId)) {
    case WindowPlayerProfileElementId::HandleButtons:
        idleImage = 0x3B583;
        hoverImage = 0x3B585;
        targetWindow = 8;
        hint = L"切换成玩家明细视窗。";
        break;
    case WindowPlayerProfileElementId::HandleButtons_21:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切换成玩家称号视窗。";
        break;
    case WindowPlayerProfileElementId::HandleButtons_22:
        if (callbackWindow.ownerSession->Connection().PukVersion() < 8u) {
            auto* const indexedElement = currentWindow->FindElementById(elementId);
            if (indexedElement) {
                indexedElement->data.graphic->imageNo =
                    0x3C111;
            }
            return 0;
        }
        idleImage = 0x3C111;
        hoverImage = 0x3C112;
        targetWindow = 58;
        hint = L"切换成玩家简介视窗";
        break;
    case WindowPlayerProfileElementId::HandleButtons_24:
        idleImage = 0x3BAC0;
        hoverImage = 0x3BAC1;
        hint = L"把战斗时的位置变更为前排。";
        break;
    case WindowPlayerProfileElementId::HandleButtons_25:
        idleImage = 0x3BABD;
        hoverImage = 0x3BABE;
        hint = L"把战斗时的位置变更为后排。";
        break;
    default:
        return 0;
    }

    std::int32_t consumed = 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = hoverImage;
            gHoverTextD68AE8 = hint;
            consumed = 1;
        } else {
            graphic->imageNo = idleImage;
        }
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (targetWindow != 0) {
            if (WindowRuntime_States(*callbackWindow.ownerSession)[targetWindow].window == nullptr) {
                currentWindow->state |= 0x8000u;
                static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, targetWindow));
                WindowInstance* const xSource = currentWindow;
                WindowInstance* const xTarget = WindowRuntime_States(*callbackWindow.ownerSession)[targetWindow].window;
                xTarget->x = xSource->x;
                WindowInstance* const ySource = currentWindow;
                WindowInstance* const yTarget = WindowRuntime_States(*callbackWindow.ownerSession)[targetWindow].window;
                yTarget->y = ySource->y;
                static_cast<void>(audio::PlaySe_52C450(56, 320));
                WindowRuntime_QueuePagePresentation(*currentWindow, *yTarget);
                consumed = 1;
            }
        } else if (world::gPlayerRuntimeSlots.value1A0 == (elementId == static_cast<ElementId>(WindowPlayerProfileElementId::HandleButtons_24) ? 0 : 1)) {
            static_cast<void>(callbackWindow.ownerSession->Connection().ToggleBattleRowRequest());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return consumed;
}

// 原474FB0：窗口7完整刷新，复制阶段的elements/data快照差异见window_7.md。
// 原函数：0x00474FB0。
static void WindowPlayerProfile_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    auto* portraitElementsWindow = currentWindow;
    const std::int32_t portrait = world::gPlayerRuntimeSlots.value17C;
    if (auto* const element15 = portraitElementsWindow->FindElementById(WindowPlayerProfileElementId::Element15); element15) element15->data.graphic->imageNo = portrait;
    WindowPlayerProfile_RefreshAttributeBar(0, 25, 0x3BAC7, 0x3BACA, 0x3BAC8, 0x3BAC9);
    WindowPlayerProfile_RefreshAttributeBar(1, 35, 0x3BACC, 0x3BACF, 0x3BACD, 0x3BACE);
    WindowPlayerProfile_RefreshAttributeBar(2, 45, 0x3BAD1, 0x3BAD4, 0x3BAD2, 0x3BAD3);
    WindowPlayerProfile_RefreshAttributeBar(3, 55, 0x3BAD6, 0x3BAD9, 0x3BAD7, 0x3BAD8);

    auto* const element1 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element1);
    if (element1) {
        auto* const nameData = element1->data.text;
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        nameData->text = name;
    }

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::gPlayerRuntimeSlots.level);
    auto* const element2 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element2);
    if (element2) {
        auto* const levelData = element2->data.text;
        levelData->text = buffer;
    }

    auto* jobElementsWindow = currentWindow;
    const auto& job = world::GetPlayerJobName();
    auto* const element4 = jobElementsWindow->FindElementById(WindowPlayerProfileElementId::Element4);
    if (element4) {
        auto* const jobData = element4->data.text;
        jobData->text = job;
    }

    WindowInstance* const guildWindow = currentWindow;
    const std::int32_t guildIndex = cg::world::ReadGuildTextIndex10514B0();
    auto* const element5 = guildWindow->FindElementById(WindowPlayerProfileElementId::Element5);
    if (guildIndex != -1) {
        const wchar_t* const guild = cg::world::GetGuildName();
        if (element5) element5->data.text->text = guild;
        const std::int32_t rankIndex = cg::world::ReadGuildTextIndex10514B0();
        auto* const element6 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element6);
        if (element6) {
            auto* const rankData = element6->data.text;
            const wchar_t* const rank = cg::world::GetGuildRankText(rankIndex);
            rankData->text = rank;
        }
    } else {
        const wchar_t* const guild = L"";
        if (element5) element5->data.text->text = guild;
        auto* const element6 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element6);
        if (element6) {
            auto* const rankData = element6->data.text;
            const wchar_t* const rank = L"";
            rankData->text = rank;
        }
    }

    const std::int32_t titleIndex = world::gPlayerRuntimeSlots.titleIndex100;
    auto* const element7 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element7);
    if (element7) {
        auto* const titleData = element7->data.text;
        const wchar_t* const title = world::GetTitleText(titleIndex);
        titleData->text = title;
    }

    auto* ownTitleElementsWindow = currentWindow;
    const wchar_t* const ownTitle = world::GetPlayerDisplayText(2);
    auto* const element8 = ownTitleElementsWindow->FindElementById(WindowPlayerProfileElementId::Element8);
    if (element8) {
        auto* const ownTitleData = element8->data.text;
        ownTitleData->text = ownTitle;
    }

    WindowInstance* const tribeWindow = currentWindow;
    const std::int16_t tribeIndex = world::ReadPlayerTribe();
    auto* tribeElementsWindow = tribeWindow;
    const wchar_t* const tribe = gTribeNames57CB54[tribeIndex];
    auto* const element9 = tribeElementsWindow->FindElementById(WindowPlayerProfileElementId::Element9);
    if (element9) {
        auto* const tribeData = element9->data.text;
        tribeData->text = tribe;
    }

    cg::text::FormatTo(buffer, L"{:9}", static_cast<std::int32_t>(world::gPlayerRuntimeSlots.value58));
    auto* const element10 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element10);
    if (element10) {
        auto* const experienceData = element10->data.text;
        experienceData->text = buffer;
    }

    const std::int32_t maxHp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxHp);
    const std::int32_t hp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.hp);
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    auto* const element12 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element12);
    if (element12) {
        auto* const hpData = element12->data.text;
        hpData->text = buffer;
    }

    const std::int32_t maxFp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxFp);
    const std::int32_t fp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.fp);
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    auto* const element13 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element13);
    if (element13) {
        auto* const fpData = element13->data.text;
        fpData->text = buffer;
    }

    std::wstring_view nextExperience;
    if ((world::gPlayerRuntimeSlots.value68 == -1)) {
        nextExperience = L"        -";
    } else {
        cg::text::FormatTo(buffer, L"{:9}", static_cast<std::int32_t>(world::gPlayerRuntimeSlots.value68));
        nextExperience = buffer;
    }
    auto* const element11 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element11);
    if (element11) {
        auto* const nextData = element11->data.text;
        nextData->text = nextExperience;
    }

    cg::text::FormatTo(buffer, L"{:9}", world::gPlayerRuntimeSlots.value104);
    auto* const element14 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element14);
    if (element14) {
        auto* const duelData = element14->data.text;
        duelData->text = buffer;
    }

    if (world::gPlayerRuntimeSlots.value1A0 == 1) {
        if (auto* const element23 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element23); element23) element23->x = 227;
    } else {
        if (auto* const element23 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element23); element23) element23->x = 263;
    }
    if (auto* const element23 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element23); element23) element23->y = 203;

    const std::int32_t injury = world::gPlayerRuntimeSlots.valueF4;
    std::int32_t injuryImage;
    if (injury == 0) {
        injuryImage = 0x3B614;
    } else if (injury <= 25) {
        injuryImage = 0x3B615;
    } else if (injury <= 50) {
        injuryImage = 0x3B616;
    } else if (injury <= 75) {
        injuryImage = 0x3B617;
    } else {
        injuryImage = 0x3B618;
    }
    if (auto* const element16 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element16); element16) element16->data.graphic->imageNo = injuryImage;

    const std::int32_t penalty = world::gPlayerRuntimeSlots.valueF8;
    if (penalty > 0) {
        cg::text::FormatTo(buffer, L"x{}", penalty);
        auto* penaltyElementsWindow = currentWindow;
        auto* const element3 = penaltyElementsWindow->FindElementById(WindowPlayerProfileElementId::Element3);
        if (element3) {
            auto* const penaltyData = element3->data.text;
            penaltyData->text = buffer;
        }
        if (auto* const element32 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element3); element32) element32->visible = 1;
        if (auto* const element17 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element17); element17) element17->visible = 1;
    } else {
        if (auto* const element3 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element3); element3) element3->visible = 0;
        if (auto* const element17 = currentWindow->FindElementById(WindowPlayerProfileElementId::Element17); element17) element17->visible = 0;
    }
}

// 原函数：0x00474540。
static std::int32_t WindowPlayerProfile_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowPlayerProfile_Refresh();
    return 1;
}

// 原函数：0x00474310。
static void WindowPlayerProfile_Release(WindowInstance& releasedWindow)
{
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerProfile].window;
    if (window != nullptr) {
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerAttributes].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerAttributes].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerTitles].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerTitles].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
