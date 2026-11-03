#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowProfileCategoryElementId : ElementId {
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
    SelectCategory = 80,
    SelectCategory_81 = 81,
    SelectCategory_82 = 82,
    SelectCategory_83 = 83,
    SelectCategory_84 = 84,
    SelectCategory_85 = 85,
    SelectCategory_86 = 86,
    SelectCategory_87 = 87,
    SelectCategory_88 = 88,
    SelectCategory_89 = 89,
    SelectCategory_90 = 90,
    SelectCategory_91 = 91,
    SelectCategory_92 = 92,
    SelectCategory_93 = 93,
    SelectCategory_94 = 94,
    SelectCategory_95 = 95,
    SelectCategory_96 = 96,
    SelectCategory_97 = 97,
    SelectCategory_98 = 98,
    SelectCategory_99 = 99,
    SelectCategory_100 = 100,
    SelectCategory_101 = 101,
    SelectCategory_102 = 102,
    SelectCategory_103 = 103,
    SelectCategory_104 = 104,
    SelectCategory_105 = 105,
    SelectCategory_106 = 106,
    SelectCategory_107 = 107,
    SelectCategory_108 = 108,
    SelectCategory_109 = 109,
    SelectCategory_110 = 110,
    SelectCategory_111 = 111,
    SelectCategory_112 = 112,
    SelectCategory_113 = 113,
    SelectCategory_114 = 114,
    SelectCategory_115 = 115,
    SelectCategory_116 = 116,
    SelectCategory_117 = 117,
    SelectCategory_118 = 118,
    SelectCategory_119 = 119,
    SelectCategory_120 = 120,
    SelectCategory_121 = 121,
    SelectCategory_122 = 122,
    SelectCategory_123 = 123,
    SelectCategory_124 = 124,
    SelectCategory_125 = 125,
    SelectCategory_126 = 126,
    SelectCategory_127 = 127,
    SelectCategory_128 = 128,
    SelectCategory_129 = 129,
    SelectCategory_130 = 130,
    SelectCategory_131 = 131,
    SelectCategory_132 = 132,
    SelectCategory_133 = 133,
    SelectCategory_134 = 134,
    SelectCategory_135 = 135,
    SelectCategory_136 = 136,
    SelectCategory_137 = 137,
    SelectCategory_138 = 138,
    SelectCategory_139 = 139,
    SelectCategory_140 = 140,
    SelectCategory_141 = 141,
    SelectCategory_142 = 142,
    SelectCategory_143 = 143,
    SelectCategory_144 = 144,
    SelectCategory_145 = 145,
    SelectCategory_146 = 146,
    SelectCategory_147 = 147,
    SelectCategory_148 = 148,
    SelectCategory_149 = 149,
    SelectCategory_150 = 150,
    SelectCategory_151 = 151,
    SelectCategory_152 = 152,
    SelectCategory_153 = 153,
    SelectCategory_154 = 154,
    SelectCategory_155 = 155,
    SelectCategory_156 = 156,
    SelectCategory_157 = 157,
    SelectCategory_158 = 158,
    ButtonClose = 159,
    Element160 = 160,
    Element161 = 161,
    HandleSideArea = 162,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
static std::int32_t WindowProfileCategory_Update(std::int32_t value); // 窗口60仅初始化刷新，随后检查距离。

// 原函数：0x004C9F20。
static void WindowProfileCategory_Refresh(); // 固定32/32/15项分类的显隐、文本和选择颜色。

// 原函数：0x004CA1A0。
static std::int32_t WindowProfileCategory_SelectCategory(ElementId elementId, std::int32_t state); // 分类点击发送后复制共享默认关键词。

// 原函数：0x004CA300。
static void WindowProfileCategory_Release(WindowInstance&); // 先清对话类型，再标记61/62/63。

// 原函数：0x004CB4A0。
static std::int32_t WindowProfileCategory_ResolveCategory(std::int32_t index, std::int32_t* group, std::int32_t* value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic577740 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577740。

static WindowGraphicData gWindowGraphic577758 {0x3C119, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577758。

static WindowGraphicData gWindowGraphic577770 {0x3BB3D, 0, 0, 0, 0, 0x8FFFFFFFu}; // 原全局：0x00577770。

static WindowGraphicData gWindowGraphic577788 {0x3C11A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577788。

static WindowTextPrototype gWindowText5777A0 { // 原全局：0x005777A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"类别",
};

static WindowElementPrototype gWindowElements5777A8[162] { // 原全局：0x005777A8。
    {WindowElementType_Text, 50, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element1)},
    {WindowElementType_Text, 50, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element2)},
    {WindowElementType_Text, 50, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element3)},
    {WindowElementType_Text, 50, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element4)},
    {WindowElementType_Text, 50, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element5)},
    {WindowElementType_Text, 50, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element6)},
    {WindowElementType_Text, 50, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element7)},
    {WindowElementType_Text, 50, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element8)},
    {WindowElementType_Text, 50, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element9)},
    {WindowElementType_Text, 50, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element10)},
    {WindowElementType_Text, 50, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element11)},
    {WindowElementType_Text, 50, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element12)},
    {WindowElementType_Text, 50, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element13)},
    {WindowElementType_Text, 50, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element14)},
    {WindowElementType_Text, 50, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element15)},
    {WindowElementType_Text, 50, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element16)},
    {WindowElementType_Text, 150, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element17)},
    {WindowElementType_Text, 150, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element18)},
    {WindowElementType_Text, 150, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element19)},
    {WindowElementType_Text, 150, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element20)},
    {WindowElementType_Text, 150, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element21)},
    {WindowElementType_Text, 150, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element22)},
    {WindowElementType_Text, 150, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element23)},
    {WindowElementType_Text, 150, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element24)},
    {WindowElementType_Text, 150, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element25)},
    {WindowElementType_Text, 150, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element26)},
    {WindowElementType_Text, 150, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element27)},
    {WindowElementType_Text, 150, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element28)},
    {WindowElementType_Text, 150, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element29)},
    {WindowElementType_Text, 150, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element30)},
    {WindowElementType_Text, 150, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element31)},
    {WindowElementType_Text, 150, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element32)},
    {WindowElementType_Text, 268, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element33)},
    {WindowElementType_Text, 268, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element34)},
    {WindowElementType_Text, 268, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element35)},
    {WindowElementType_Text, 268, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element36)},
    {WindowElementType_Text, 268, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element37)},
    {WindowElementType_Text, 268, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element38)},
    {WindowElementType_Text, 268, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element39)},
    {WindowElementType_Text, 268, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element40)},
    {WindowElementType_Text, 268, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element41)},
    {WindowElementType_Text, 268, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element42)},
    {WindowElementType_Text, 268, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element43)},
    {WindowElementType_Text, 268, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element44)},
    {WindowElementType_Text, 268, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element45)},
    {WindowElementType_Text, 268, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element46)},
    {WindowElementType_Text, 268, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element47)},
    {WindowElementType_Text, 268, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element48)},
    {WindowElementType_Text, 368, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element49)},
    {WindowElementType_Text, 368, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element50)},
    {WindowElementType_Text, 368, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element51)},
    {WindowElementType_Text, 368, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element52)},
    {WindowElementType_Text, 368, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element53)},
    {WindowElementType_Text, 368, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element54)},
    {WindowElementType_Text, 368, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element55)},
    {WindowElementType_Text, 368, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element56)},
    {WindowElementType_Text, 368, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element57)},
    {WindowElementType_Text, 368, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element58)},
    {WindowElementType_Text, 368, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element59)},
    {WindowElementType_Text, 368, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element60)},
    {WindowElementType_Text, 368, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element61)},
    {WindowElementType_Text, 368, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element62)},
    {WindowElementType_Text, 368, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element63)},
    {WindowElementType_Text, 368, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element64)},
    {WindowElementType_Text, 484, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element65)},
    {WindowElementType_Text, 484, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element66)},
    {WindowElementType_Text, 484, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element67)},
    {WindowElementType_Text, 484, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element68)},
    {WindowElementType_Text, 484, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element69)},
    {WindowElementType_Text, 484, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element70)},
    {WindowElementType_Text, 484, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element71)},
    {WindowElementType_Text, 484, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element72)},
    {WindowElementType_Text, 484, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element73)},
    {WindowElementType_Text, 484, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element74)},
    {WindowElementType_Text, 484, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element75)},
    {WindowElementType_Text, 484, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element76)},
    {WindowElementType_Text, 484, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element77)},
    {WindowElementType_Text, 484, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element78)},
    {WindowElementType_Text, 484, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element79)},
    {WindowElementType_Graphic, 48, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory)},
    {WindowElementType_Graphic, 48, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_81)},
    {WindowElementType_Graphic, 48, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_82)},
    {WindowElementType_Graphic, 48, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_83)},
    {WindowElementType_Graphic, 48, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_84)},
    {WindowElementType_Graphic, 48, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_85)},
    {WindowElementType_Graphic, 48, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_86)},
    {WindowElementType_Graphic, 48, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_87)},
    {WindowElementType_Graphic, 48, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_88)},
    {WindowElementType_Graphic, 48, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_89)},
    {WindowElementType_Graphic, 48, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_90)},
    {WindowElementType_Graphic, 48, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_91)},
    {WindowElementType_Graphic, 48, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_92)},
    {WindowElementType_Graphic, 48, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_93)},
    {WindowElementType_Graphic, 48, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_94)},
    {WindowElementType_Graphic, 48, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_95)},
    {WindowElementType_Graphic, 148, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_96)},
    {WindowElementType_Graphic, 148, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_97)},
    {WindowElementType_Graphic, 148, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_98)},
    {WindowElementType_Graphic, 148, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_99)},
    {WindowElementType_Graphic, 148, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_100)},
    {WindowElementType_Graphic, 148, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_101)},
    {WindowElementType_Graphic, 148, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_102)},
    {WindowElementType_Graphic, 148, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_103)},
    {WindowElementType_Graphic, 148, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_104)},
    {WindowElementType_Graphic, 148, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_105)},
    {WindowElementType_Graphic, 148, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_106)},
    {WindowElementType_Graphic, 148, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_107)},
    {WindowElementType_Graphic, 148, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_108)},
    {WindowElementType_Graphic, 148, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_109)},
    {WindowElementType_Graphic, 148, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_110)},
    {WindowElementType_Graphic, 148, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_111)},
    {WindowElementType_Graphic, 266, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_112)},
    {WindowElementType_Graphic, 266, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_113)},
    {WindowElementType_Graphic, 266, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_114)},
    {WindowElementType_Graphic, 266, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_115)},
    {WindowElementType_Graphic, 266, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_116)},
    {WindowElementType_Graphic, 266, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_117)},
    {WindowElementType_Graphic, 266, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_118)},
    {WindowElementType_Graphic, 266, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_119)},
    {WindowElementType_Graphic, 266, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_120)},
    {WindowElementType_Graphic, 266, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_121)},
    {WindowElementType_Graphic, 266, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_122)},
    {WindowElementType_Graphic, 266, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_123)},
    {WindowElementType_Graphic, 266, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_124)},
    {WindowElementType_Graphic, 266, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_125)},
    {WindowElementType_Graphic, 266, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_126)},
    {WindowElementType_Graphic, 266, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_127)},
    {WindowElementType_Graphic, 366, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_128)},
    {WindowElementType_Graphic, 366, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_129)},
    {WindowElementType_Graphic, 366, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_130)},
    {WindowElementType_Graphic, 366, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_131)},
    {WindowElementType_Graphic, 366, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_132)},
    {WindowElementType_Graphic, 366, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_133)},
    {WindowElementType_Graphic, 366, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_134)},
    {WindowElementType_Graphic, 366, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_135)},
    {WindowElementType_Graphic, 366, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_136)},
    {WindowElementType_Graphic, 366, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_137)},
    {WindowElementType_Graphic, 366, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_138)},
    {WindowElementType_Graphic, 366, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_139)},
    {WindowElementType_Graphic, 366, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_140)},
    {WindowElementType_Graphic, 366, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_141)},
    {WindowElementType_Graphic, 366, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_142)},
    {WindowElementType_Graphic, 366, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_143)},
    {WindowElementType_Graphic, 482, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_144)},
    {WindowElementType_Graphic, 482, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_145)},
    {WindowElementType_Graphic, 482, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_146)},
    {WindowElementType_Graphic, 482, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_147)},
    {WindowElementType_Graphic, 482, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_148)},
    {WindowElementType_Graphic, 482, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_149)},
    {WindowElementType_Graphic, 482, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_150)},
    {WindowElementType_Graphic, 482, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_151)},
    {WindowElementType_Graphic, 482, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_152)},
    {WindowElementType_Graphic, 482, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_153)},
    {WindowElementType_Graphic, 482, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_154)},
    {WindowElementType_Graphic, 482, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_155)},
    {WindowElementType_Graphic, 482, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_156)},
    {WindowElementType_Graphic, 482, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_157)},
    {WindowElementType_Graphic, 482, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory, -1, static_cast<ElementId>(WindowProfileCategoryElementId::SelectCategory_158)},
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic577740}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowProfileCategoryElementId::ButtonClose)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element160)},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic577770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileCategoryElementId::Element161)},
    {WindowElementType_EventArea, 600, 0, 34, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowProfileCategoryElementId::HandleSideArea)},
};

// 原型：窗口60；原全局：0x0054ED90。
WindowPrototype gWindowPrototypeProfileCategory {0, 4, 5, 20, 634, 424, 0x80000080u, 162, gWindowElements5777A8, &WindowProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowProfileCategory_Release}; // 60: 54ED90

// 窗口函数实现。

// 原函数：0x004C9F20。
static void WindowProfileCategory_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    constexpr std::array<std::int32_t, 3> counts {32, 32, 15};
    constexpr std::array<std::int32_t, 3> textBases {0, 32, 64};
    for (std::int32_t group = 0; group < 3; ++group) {
        for (std::int32_t index = 1; index <= counts[group]; ++index) {
            const auto& record = world::detail::ReadCategoryRecord105FA24(group, index);
            const std::int32_t textIndex = textBases[group] + index - 1;
            if (record.used != 0) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(textIndex)); indexedElement) indexedElement->visible = 1;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(textIndex + 79)); indexedElement2) indexedElement2->visible = 1;
                if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(textIndex)); indexedElement3) indexedElement3->data.text->text = record.text0C;
                auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(textIndex));
                if (indexedElement4) {
                    indexedElement4->data.text->style.color =
                        record.selected10C != 0 ? std::byte{0} : std::byte{7};
                }
            } else {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(textIndex)); indexedElement) indexedElement->visible = 0;
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(textIndex + 79)); indexedElement2) indexedElement2->visible = 0;
            }
        }
    }
}

// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
static std::int32_t WindowProfileCategory_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowProfileCategory_Refresh();
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004CA300。
static void WindowProfileCategory_Release(WindowInstance& releasedWindow)
{
    world::gCurrentDialogType_581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(*releasedWindow.ownerSession, 2));
    static_cast<void>(WindowProfileView_RequestCategoryClose(*releasedWindow.ownerSession, 3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(*releasedWindow.ownerSession, 4));
}

// 原函数：0x004CB4A0。
static std::int32_t WindowProfileCategory_ResolveCategory(
    const std::int32_t index, std::int32_t* const group, std::int32_t* const value)
{
    if (index - 79 < 32) {
        *group = 0;
        *value = world::detail::ReadCategoryRecord105FA24(0, index - 78).value04;
    } else if (index - 79 < 64) {
        *group = 1;
        *value = world::detail::ReadCategoryRecord105FA24(1, index - 110).value04;
    } else {
        *group = 2;
        *value = world::detail::ReadCategoryRecord105FA24(2, index - 142).value04;
    }
    return 1;
}

// 原函数：0x004CA1A0。
static std::int32_t WindowProfileCategory_SelectCategory(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        gHoverTextD68AE8 = L"在此目录中搜寻";
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 246042;
        consumed = 1;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0;
    }
    if ((state & MouseFlag_LDownEvent) == 0 || WindowProfileView_ReadSelection(index) == 0) {
        return consumed;
    }
    std::int32_t group;
    std::int32_t value;
    if (WindowProfileCategory_ResolveCategory(index, &group, &value) != 0) {
        std::array<std::wstring, 2> buffers;
        cg::text::FormatTo(buffers[0], L"{}|{}|", group, value);
        buffers[1] = input::EscapeTextAndNotify(buffers[0], 255);
        const std::int32_t target = world::gDialogTargetD2DE6C;
        const std::int32_t request = world::gDialogRequestCC21E0;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(cg::network::GetGameConnection().SendDialogResponse(x, y, request, target, 0, buffers[1]));
        gProfileSearchKeyword1075988.clear();
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
