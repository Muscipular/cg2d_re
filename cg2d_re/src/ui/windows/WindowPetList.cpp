#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetListElementId : ElementId {
    ButtonClose = 1,
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
    Element107 = 107,
    Element108 = 108,
    Element109 = 109,
    Element110 = 110,
    Element111 = 111,
    Element112 = 112,
    Element113 = 113,
    Element114 = 114,
    Element115 = 115,
    Element116 = 116,
    Element117 = 117,
    Element118 = 118,
    Element119 = 119,
    Element120 = 120,
    Element121 = 121,
    Element122 = 122,
    Element123 = 123,
    Element124 = 124,
    Element125 = 125,
    Element126 = 126,
    Element127 = 127,
    Element128 = 128,
    Element129 = 129,
    Element130 = 130,
    Element131 = 131,
    Element132 = 132,
    Element133 = 133,
    Element134 = 134,
    Element135 = 135,
    Element136 = 136,
    Element137 = 137,
    Element138 = 138,
    Element139 = 139,
    Element140 = 140,
    Element141 = 141,
    Element142 = 142,
    Element143 = 143,
    Element144 = 144,
    Element145 = 145,
    Element146 = 146,
    Element147 = 147,
    Element148 = 148,
    Element149 = 149,
    Element150 = 150,
    Element151 = 151,
    Element152 = 152,
    Element153 = 153,
    Element154 = 154,
    Element155 = 155,
    Element156 = 156,
    Element157 = 157,
    Element158 = 158,
    Element159 = 159,
    Element160 = 160,
    Element161 = 161,
    Element162 = 162,
    Element163 = 163,
    Element164 = 164,
    Element165 = 165,
    Element166 = 166,
    Element167 = 167,
    Element168 = 168,
    Element169 = 169,
    Element170 = 170,
    Element171 = 171,
    Element172 = 172,
    Element173 = 173,
    Element174 = 174,
    Element175 = 175,
    Element176 = 176,
    Element177 = 177,
    Element178 = 178,
    Element179 = 179,
    Element180 = 180,
    Element181 = 181,
    Element182 = 182,
    Element183 = 183,
    Element184 = 184,
    Element185 = 185,
    Element186 = 186,
    Element187 = 187,
    Element188 = 188,
    Element189 = 189,
    Element190 = 190,
    Element191 = 191,
    Element192 = 192,
    Element193 = 193,
    Element194 = 194,
    Element195 = 195,
    Element196 = 196,
    Element197 = 197,
    Element198 = 198,
    Element199 = 199,
    Element200 = 200,
    Element201 = 201,
    Element202 = 202,
    Element203 = 203,
    Element204 = 204,
    Element205 = 205,
    Element206 = 206,
    Element207 = 207,
    Element208 = 208,
    Element209 = 209,
    Element210 = 210,
    Element211 = 211,
    Element212 = 212,
    Element213 = 213,
    Element214 = 214,
    Element215 = 215,
    Element216 = 216,
    Element217 = 217,
    Element218 = 218,
    Element219 = 219,
    Element220 = 220,
    Element221 = 221,
    Element222 = 222,
    Element223 = 223,
    Element224 = 224,
    Element225 = 225,
    Element226 = 226,
    HandleButtons = 227,
    HandleButtons_228 = 228,
    HandleButtons_229 = 229,
    HandleButtons_230 = 230,
    HandleButtons_231 = 231,
    HandleButtons_232 = 232,
    HandleButtons_233 = 233,
    HandleButtons_234 = 234,
    HandleButtons_235 = 235,
    HandleButtons_236 = 236,
    HandleButtons_237 = 237,
    HandleButtons_238 = 238,
    HandleButtons_239 = 239,
    HandleButtons_240 = 240,
    HandleButtons_241 = 241,
    HandleButtons_242 = 242,
    HandleButtons_243 = 243,
    HandleButtons_244 = 244,
    HandleButtons_245 = 245,
    HandleButtons_246 = 246,
    Element247 = 247,
    Element248 = 248,
    Element249 = 249,
    Element250 = 250,
    Element251 = 251,
    Element252 = 252,
    Element253 = 253,
    HandleButtons_254 = 254,
    HandleButtons_255 = 255,
    HandleButtons_256 = 256,
    HandleButtons_257 = 257,
    HandleButtons_258 = 258,
    HandleSideArea = 259,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048C1E0。
static std::int32_t WindowPetList_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口15完整五组交互。

// 原函数：0x0048D4E0。
static network::SendResult WindowPetList_SubmitStates(); // 五状态局部快照叠加真实跟随位，透传发送结果。

// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
static std::int32_t WindowPetList_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic563298 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563298。

static WindowGraphicData gWindowGraphic5632B0 {0x3BAE2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632B0。

static WindowGraphicData gWindowGraphic5632C8 {0x3BAE3, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005632C8。

static WindowGraphicData gWindowGraphic5632E0 {0x3B5C4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632E0。

static WindowGraphicData gWindowGraphic5632F8 {0x3B5CF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632F8。

static WindowGraphicData gWindowGraphic563478 {0x3B586, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563478。

static WindowGraphicData gWindowGraphic563490 {0x3B589, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563490。

static WindowGraphicData gWindowGraphic5634A8 {0x3B58C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005634A8。

static WindowGraphicData gWindowGraphic5634C0 {0x3B58F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005634C0。

static WindowTextPrototype gWindowText5634D8 { // 原全局：0x005634D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"宠物名称", // 原常量参考：0x00586048。
};

static WindowTextPrototype gWindowText5634E0 { // 原全局：0x005634E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"300", // 原常量参考：0x00586044。
};

static WindowTextPrototype gWindowText5634E8 { // 原全局：0x005634E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};

static WindowTextPrototype gWindowText5634F0 { // 原全局：0x005634F0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};

static WindowTextPrototype gWindowText5634F8 { // 原全局：0x005634F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"种族", // 原常量参考：0x005861E8。
};

static std::uint32_t gWindowMouseFlagsD677A0 = 0; // 原全局：0x00D677A0。

static WindowElementPrototype gWindowElements563500[259] { // 原全局：0x00563500。
    {WindowElementType_Graphic, 302, 8, 11, 11, 1, {.graphic = &gWindowGraphic563298}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetListElementId::ButtonClose)},
    {WindowElementType_Text, 23, 34, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element2)},
    {WindowElementType_Text, 156, 34, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element3)},
    {WindowElementType_Text, 52, 47, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element4)},
    {WindowElementType_Text, 52, 60, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element5)},
    {WindowElementType_Text, 133, 49, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element6)},
    {WindowElementType_Text, 23, 82, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element7)},
    {WindowElementType_Text, 156, 82, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element8)},
    {WindowElementType_Text, 52, 95, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element9)},
    {WindowElementType_Text, 52, 108, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element10)},
    {WindowElementType_Text, 133, 97, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element11)},
    {WindowElementType_Text, 23, 130, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element12)},
    {WindowElementType_Text, 156, 130, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element13)},
    {WindowElementType_Text, 52, 143, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element14)},
    {WindowElementType_Text, 52, 156, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element15)},
    {WindowElementType_Text, 133, 145, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element16)},
    {WindowElementType_Text, 23, 178, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element17)},
    {WindowElementType_Text, 156, 178, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element18)},
    {WindowElementType_Text, 52, 191, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element19)},
    {WindowElementType_Text, 52, 204, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element20)},
    {WindowElementType_Text, 133, 193, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element21)},
    {WindowElementType_Text, 23, 226, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element22)},
    {WindowElementType_Text, 156, 226, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element23)},
    {WindowElementType_Text, 52, 239, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element24)},
    {WindowElementType_Text, 52, 252, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element25)},
    {WindowElementType_Text, 133, 241, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element26)},
    {WindowElementType_Graphic, 190, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element27)},
    {WindowElementType_Graphic, 196, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element28)},
    {WindowElementType_Graphic, 202, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element29)},
    {WindowElementType_Graphic, 208, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element30)},
    {WindowElementType_Graphic, 214, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element31)},
    {WindowElementType_Graphic, 220, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element32)},
    {WindowElementType_Graphic, 226, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element33)},
    {WindowElementType_Graphic, 232, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element34)},
    {WindowElementType_Graphic, 238, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element35)},
    {WindowElementType_Graphic, 244, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element36)},
    {WindowElementType_Graphic, 190, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element37)},
    {WindowElementType_Graphic, 196, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element38)},
    {WindowElementType_Graphic, 202, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element39)},
    {WindowElementType_Graphic, 208, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element40)},
    {WindowElementType_Graphic, 214, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element41)},
    {WindowElementType_Graphic, 220, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element42)},
    {WindowElementType_Graphic, 226, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element43)},
    {WindowElementType_Graphic, 232, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element44)},
    {WindowElementType_Graphic, 238, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element45)},
    {WindowElementType_Graphic, 244, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element46)},
    {WindowElementType_Graphic, 190, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element47)},
    {WindowElementType_Graphic, 196, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element48)},
    {WindowElementType_Graphic, 202, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element49)},
    {WindowElementType_Graphic, 208, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element50)},
    {WindowElementType_Graphic, 214, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element51)},
    {WindowElementType_Graphic, 220, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element52)},
    {WindowElementType_Graphic, 226, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element53)},
    {WindowElementType_Graphic, 232, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element54)},
    {WindowElementType_Graphic, 238, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element55)},
    {WindowElementType_Graphic, 244, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element56)},
    {WindowElementType_Graphic, 190, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element57)},
    {WindowElementType_Graphic, 196, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element58)},
    {WindowElementType_Graphic, 202, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element59)},
    {WindowElementType_Graphic, 208, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element60)},
    {WindowElementType_Graphic, 214, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element61)},
    {WindowElementType_Graphic, 220, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element62)},
    {WindowElementType_Graphic, 226, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element63)},
    {WindowElementType_Graphic, 232, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element64)},
    {WindowElementType_Graphic, 238, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element65)},
    {WindowElementType_Graphic, 244, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element66)},
    {WindowElementType_Graphic, 190, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element67)},
    {WindowElementType_Graphic, 196, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element68)},
    {WindowElementType_Graphic, 202, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element69)},
    {WindowElementType_Graphic, 208, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element70)},
    {WindowElementType_Graphic, 214, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element71)},
    {WindowElementType_Graphic, 220, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element72)},
    {WindowElementType_Graphic, 226, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element73)},
    {WindowElementType_Graphic, 232, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element74)},
    {WindowElementType_Graphic, 238, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element75)},
    {WindowElementType_Graphic, 244, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element76)},
    {WindowElementType_Graphic, 190, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element77)},
    {WindowElementType_Graphic, 196, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element78)},
    {WindowElementType_Graphic, 202, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element79)},
    {WindowElementType_Graphic, 208, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element80)},
    {WindowElementType_Graphic, 214, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element81)},
    {WindowElementType_Graphic, 220, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element82)},
    {WindowElementType_Graphic, 226, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element83)},
    {WindowElementType_Graphic, 232, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element84)},
    {WindowElementType_Graphic, 238, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element85)},
    {WindowElementType_Graphic, 244, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element86)},
    {WindowElementType_Graphic, 190, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element87)},
    {WindowElementType_Graphic, 196, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element88)},
    {WindowElementType_Graphic, 202, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element89)},
    {WindowElementType_Graphic, 208, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element90)},
    {WindowElementType_Graphic, 214, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element91)},
    {WindowElementType_Graphic, 220, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element92)},
    {WindowElementType_Graphic, 226, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element93)},
    {WindowElementType_Graphic, 232, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element94)},
    {WindowElementType_Graphic, 238, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element95)},
    {WindowElementType_Graphic, 244, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element96)},
    {WindowElementType_Graphic, 190, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element97)},
    {WindowElementType_Graphic, 196, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element98)},
    {WindowElementType_Graphic, 202, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element99)},
    {WindowElementType_Graphic, 208, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element100)},
    {WindowElementType_Graphic, 214, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element101)},
    {WindowElementType_Graphic, 220, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element102)},
    {WindowElementType_Graphic, 226, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element103)},
    {WindowElementType_Graphic, 232, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element104)},
    {WindowElementType_Graphic, 238, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element105)},
    {WindowElementType_Graphic, 244, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element106)},
    {WindowElementType_Graphic, 190, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element107)},
    {WindowElementType_Graphic, 196, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element108)},
    {WindowElementType_Graphic, 202, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element109)},
    {WindowElementType_Graphic, 208, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element110)},
    {WindowElementType_Graphic, 214, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element111)},
    {WindowElementType_Graphic, 220, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element112)},
    {WindowElementType_Graphic, 226, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element113)},
    {WindowElementType_Graphic, 232, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element114)},
    {WindowElementType_Graphic, 238, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element115)},
    {WindowElementType_Graphic, 244, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element116)},
    {WindowElementType_Graphic, 190, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element117)},
    {WindowElementType_Graphic, 196, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element118)},
    {WindowElementType_Graphic, 202, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element119)},
    {WindowElementType_Graphic, 208, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element120)},
    {WindowElementType_Graphic, 214, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element121)},
    {WindowElementType_Graphic, 220, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element122)},
    {WindowElementType_Graphic, 226, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element123)},
    {WindowElementType_Graphic, 232, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element124)},
    {WindowElementType_Graphic, 238, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element125)},
    {WindowElementType_Graphic, 244, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element126)},
    {WindowElementType_Graphic, 190, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element127)},
    {WindowElementType_Graphic, 196, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element128)},
    {WindowElementType_Graphic, 202, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element129)},
    {WindowElementType_Graphic, 208, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element130)},
    {WindowElementType_Graphic, 214, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element131)},
    {WindowElementType_Graphic, 220, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element132)},
    {WindowElementType_Graphic, 226, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element133)},
    {WindowElementType_Graphic, 232, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element134)},
    {WindowElementType_Graphic, 238, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element135)},
    {WindowElementType_Graphic, 244, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element136)},
    {WindowElementType_Graphic, 190, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element137)},
    {WindowElementType_Graphic, 196, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element138)},
    {WindowElementType_Graphic, 202, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element139)},
    {WindowElementType_Graphic, 208, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element140)},
    {WindowElementType_Graphic, 214, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element141)},
    {WindowElementType_Graphic, 220, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element142)},
    {WindowElementType_Graphic, 226, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element143)},
    {WindowElementType_Graphic, 232, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element144)},
    {WindowElementType_Graphic, 238, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element145)},
    {WindowElementType_Graphic, 244, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element146)},
    {WindowElementType_Graphic, 190, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element147)},
    {WindowElementType_Graphic, 196, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element148)},
    {WindowElementType_Graphic, 202, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element149)},
    {WindowElementType_Graphic, 208, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element150)},
    {WindowElementType_Graphic, 214, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element151)},
    {WindowElementType_Graphic, 220, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element152)},
    {WindowElementType_Graphic, 226, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element153)},
    {WindowElementType_Graphic, 232, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element154)},
    {WindowElementType_Graphic, 238, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element155)},
    {WindowElementType_Graphic, 244, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element156)},
    {WindowElementType_Graphic, 190, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element157)},
    {WindowElementType_Graphic, 196, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element158)},
    {WindowElementType_Graphic, 202, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element159)},
    {WindowElementType_Graphic, 208, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element160)},
    {WindowElementType_Graphic, 214, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element161)},
    {WindowElementType_Graphic, 220, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element162)},
    {WindowElementType_Graphic, 226, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element163)},
    {WindowElementType_Graphic, 232, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element164)},
    {WindowElementType_Graphic, 238, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element165)},
    {WindowElementType_Graphic, 244, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element166)},
    {WindowElementType_Graphic, 190, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element167)},
    {WindowElementType_Graphic, 196, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element168)},
    {WindowElementType_Graphic, 202, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element169)},
    {WindowElementType_Graphic, 208, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element170)},
    {WindowElementType_Graphic, 214, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element171)},
    {WindowElementType_Graphic, 220, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element172)},
    {WindowElementType_Graphic, 226, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element173)},
    {WindowElementType_Graphic, 232, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element174)},
    {WindowElementType_Graphic, 238, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element175)},
    {WindowElementType_Graphic, 244, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element176)},
    {WindowElementType_Graphic, 190, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element177)},
    {WindowElementType_Graphic, 196, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element178)},
    {WindowElementType_Graphic, 202, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element179)},
    {WindowElementType_Graphic, 208, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element180)},
    {WindowElementType_Graphic, 214, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element181)},
    {WindowElementType_Graphic, 220, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element182)},
    {WindowElementType_Graphic, 226, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element183)},
    {WindowElementType_Graphic, 232, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element184)},
    {WindowElementType_Graphic, 238, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element185)},
    {WindowElementType_Graphic, 244, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element186)},
    {WindowElementType_Graphic, 190, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element187)},
    {WindowElementType_Graphic, 196, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element188)},
    {WindowElementType_Graphic, 202, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element189)},
    {WindowElementType_Graphic, 208, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element190)},
    {WindowElementType_Graphic, 214, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element191)},
    {WindowElementType_Graphic, 220, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element192)},
    {WindowElementType_Graphic, 226, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element193)},
    {WindowElementType_Graphic, 232, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element194)},
    {WindowElementType_Graphic, 238, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element195)},
    {WindowElementType_Graphic, 244, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element196)},
    {WindowElementType_Graphic, 190, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element197)},
    {WindowElementType_Graphic, 196, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element198)},
    {WindowElementType_Graphic, 202, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element199)},
    {WindowElementType_Graphic, 208, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element200)},
    {WindowElementType_Graphic, 214, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element201)},
    {WindowElementType_Graphic, 220, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element202)},
    {WindowElementType_Graphic, 226, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element203)},
    {WindowElementType_Graphic, 232, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element204)},
    {WindowElementType_Graphic, 238, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element205)},
    {WindowElementType_Graphic, 244, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element206)},
    {WindowElementType_Graphic, 190, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element207)},
    {WindowElementType_Graphic, 196, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element208)},
    {WindowElementType_Graphic, 202, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element209)},
    {WindowElementType_Graphic, 208, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element210)},
    {WindowElementType_Graphic, 214, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element211)},
    {WindowElementType_Graphic, 220, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element212)},
    {WindowElementType_Graphic, 226, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element213)},
    {WindowElementType_Graphic, 232, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element214)},
    {WindowElementType_Graphic, 238, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element215)},
    {WindowElementType_Graphic, 244, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element216)},
    {WindowElementType_Graphic, 190, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element217)},
    {WindowElementType_Graphic, 196, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element218)},
    {WindowElementType_Graphic, 202, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element219)},
    {WindowElementType_Graphic, 208, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element220)},
    {WindowElementType_Graphic, 214, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element221)},
    {WindowElementType_Graphic, 220, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element222)},
    {WindowElementType_Graphic, 226, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element223)},
    {WindowElementType_Graphic, 232, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element224)},
    {WindowElementType_Graphic, 238, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element225)},
    {WindowElementType_Graphic, 244, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element226)},
    {WindowElementType_Graphic, 259, 54, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons)},
    {WindowElementType_Graphic, 259, 102, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_228)},
    {WindowElementType_Graphic, 259, 150, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_229)},
    {WindowElementType_Graphic, 259, 198, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_230)},
    {WindowElementType_Graphic, 259, 246, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_231)},
    {WindowElementType_Graphic, 285, 54, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_232)},
    {WindowElementType_Graphic, 285, 102, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_233)},
    {WindowElementType_Graphic, 285, 150, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_234)},
    {WindowElementType_Graphic, 285, 198, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_235)},
    {WindowElementType_Graphic, 285, 246, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_236)},
    {WindowElementType_Graphic, 259, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_237)},
    {WindowElementType_Graphic, 259, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_238)},
    {WindowElementType_Graphic, 259, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_239)},
    {WindowElementType_Graphic, 259, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_240)},
    {WindowElementType_Graphic, 259, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_241)},
    {WindowElementType_Graphic, 285, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_242)},
    {WindowElementType_Graphic, 285, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_243)},
    {WindowElementType_Graphic, 285, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_244)},
    {WindowElementType_Graphic, 285, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_245)},
    {WindowElementType_Graphic, 285, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_246)},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element247)},
    {WindowElementType_Graphic, 15, 76, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element248)},
    {WindowElementType_Graphic, 15, 124, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element249)},
    {WindowElementType_Graphic, 15, 172, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element250)},
    {WindowElementType_Graphic, 15, 220, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element251)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5632B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element252)},
    {WindowElementType_Graphic, 14, 26, 0, 0, 1, {.graphic = &gWindowGraphic5632C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetListElementId::Element253)},
    {WindowElementType_MouseArea, 15, 28, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_254)},
    {WindowElementType_MouseArea, 15, 76, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_255)},
    {WindowElementType_MouseArea, 15, 124, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_256)},
    {WindowElementType_MouseArea, 15, 172, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_257)},
    {WindowElementType_MouseArea, 15, 220, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons, -1, static_cast<ElementId>(WindowPetListElementId::HandleButtons_258)},
    {WindowElementType_EventArea, 323, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetListElementId::HandleSideArea)},
};

// 原型：窗口15；原全局：0x0054E540。
WindowPrototype gWindowPrototypePetList {0, 4, 3, 85, 323, 281, 0x80010101u, 259, gWindowElements563500, &WindowPetList_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 15: 54E540

// 窗口函数实现。

// 原函数：0x0048C150。
std::int32_t WindowPetList_ResetSelection()
{
    gLoginWindowRuntime.valueD673D0 = 5;
    gLoginWindowRuntime.selectedPetCC3E5C = 0;
    for (std::int32_t& value : gLoginWindowRuntime.valuesCC1A18) {
        value = 0;
    }
    gLoginWindowRuntime.valuesCC4CE8[0] = 0;
    gLoginWindowRuntime.valuesCC4CE8[2] = 0;
    gLoginWindowRuntime.valuesCC4CE8[3] = 0;
    gLoginWindowRuntime.valuesCC4CE8[4] = 0;
    gLoginWindowRuntime.valuesCC4CE8[1] = 0;
    gLoginWindowRuntime.valuesD2EAA0[0] = 0;
    gLoginWindowRuntime.valuesD2EAA0[1] = -1;
    gLoginWindowRuntime.valuesD2EAA0[2] = -1;
    return -1;
}

// 原511AE0三处调用均先逆序读取五状态，最后读取socket。
static network::SendResult WindowPetList_SendStates(const std::array<std::int32_t, 5>& values)
{
    const std::int32_t fifth = values[4];
    const std::int32_t fourth = values[3];
    const std::int32_t third = values[2];
    const std::int32_t second = values[1];
    const std::int32_t first = values[0];
    const auto networkSession = cg::network::GetGameConnection().CaptureSession();
    return cg::network::GetGameConnection().SendPetStates(first, second, third, fourth, fifth);
}

// 原函数：0x0048D4E0。
static network::SendResult WindowPetList_SubmitStates()
{
    std::array<std::int32_t, 5> values;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        const std::int32_t follow = world::gPetRuntimeRecords[slot].follow6D4;
        const std::int32_t state = gLoginWindowRuntime.valuesCC1A18[slot];
        values[slot] = state;
        if (follow != 0) {
            values[slot] = state | 0x10;
        }
    }
    return WindowPetList_SendStates(values);
}

// 原函数：0x0048C1E0。
static std::int32_t WindowPetList_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    auto& states = gLoginWindowRuntime.valuesCC1A18;
    const auto petIndex = [](const std::int32_t row) {
        return std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[row].index);
    };
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };

    if (elementId >= static_cast<ElementId>(WindowPetListElementId::HandleButtons_254) && elementId <= static_cast<ElementId>(WindowPetListElementId::HandleButtons_258)) {
        const std::int32_t row = index - 253;
        std::uint32_t events = static_cast<std::uint32_t>(state);
        if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 3) {
            consumed = 1;
            events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        }
        if ((events & MouseFlag_DragFinish) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            const std::int16_t target = petIndex(row);
            WindowItem_RequestPetTransfer(15, index, source, target);
            WindowDrag_Reset();
        }
        if ((events & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = L"打开此宠物的能力视窗。" /* 原常量参考：0x00586D0C。 */;
            consumed = 1;
        }
        if ((events & MouseFlag_LDownEvent) != 0) {
            const std::int16_t selected = petIndex(row);
            const auto& pet = world::gPetRuntimeRecords[selected];
            if (pet.used != 0) {
                WindowInstance* const detailsWindow = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetDetail].window;
                gLoginWindowRuntime.selectedPetCC3E5C = row;
                gLoginWindowRuntime.valueD673D0 = 5;
                if (WindowInstance* const equipment = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetEquipment].window) {
                    equipment->state |= 0x2000u;
                } else if (detailsWindow != nullptr || WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetAttributes].window != nullptr
                    || WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetSkills].window != nullptr) {
                    const wchar_t* const name =
                        pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    gPetNameInputPrototype57DCF8.text = name;
                    WindowInstance* const window = currentWindow;
                    const std::int32_t y = window->y;
                    const std::int32_t x = window->x;
                    static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PetDetail, 1, 0, nullptr));
                }
            }
            static_cast<void>(WindowDrag_ResetMouse());
            consumed = 1;
        } else if ((events & MouseFlag_Hover) != 0 && (events & MouseFlag_RDownEvent) != 0) {
            const std::int16_t selected = petIndex(row);
            if (world::gPetRuntimeRecords[selected].used != 0) {
                static_cast<void>(WindowPetDrag_Start(selected, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        gLoginWindowRuntime.valuesD2EAA0[0] = 0;
        gLoginWindowRuntime.valuesD2EAA0[1] = -1;
        if (WindowDrag_ReadKind() == 3) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            gLoginWindowRuntime.valuesD2EAA0[0] = 1;
            gLoginWindowRuntime.valuesD2EAA0[1] = WindowItem_FindSortedPetIndex(source);
        }
        return consumed;
    }

    if (elementId >= static_cast<ElementId>(WindowPetListElementId::HandleButtons) && elementId <= static_cast<ElementId>(WindowPetListElementId::HandleButtons_231)) {
        const std::int32_t row = index - 226;
        if (callbackWindow.ownerSession->Connection().PukVersion() < 7u) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->visible = 0;
            return 0;
        }
        if (WindowPet_HasSkill25600(petIndex(row)) != 1) {
            return 0;
        }
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B588);
            const std::int32_t follow = world::gPetRuntimeRecords[petIndex(row)].follow6D4;
            gHoverTextD68AE8 = follow != 0 ? L"无法将此宠物带在身后散步。" /* 原常量参考：0x00586CF0。 */ : L"将此宠物带在身后散步。" /* 原常量参考：0x00586CD8。 */;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        if (world::gPetRuntimeRecords[selected].follow6D4 != 0) {
            static_cast<void>(WindowPetList_SendStates(states));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            return consumed;
        }
        std::int32_t followCount = 0;
        for (std::int32_t position = 0; position < 5; ++position) {
            if (world::gPetRuntimeRecords[petIndex(position)].follow6D4 != 0) {
                ++followCount;
            }
        }
        if (followCount > 0) {
            return consumed;
        }
        if (world::gPetRuntimeRecords[selected].values70[10] < 60) {
            world::AppendChatMessage_44B050(L"宠物忠诚度需达到60以上才可以带它散步喔!" /* 原常量参考：0x00586CB0。 */, 4, 3);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            return consumed;
        }
        states[selected] |= 0x10;
        static_cast<void>(WindowPetList_SendStates(states));
        states[petIndex(row)] &= ~0x10;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (elementId >= static_cast<ElementId>(WindowPetListElementId::HandleButtons_232) && elementId <= static_cast<ElementId>(WindowPetListElementId::HandleButtons_236)) {
        const std::int32_t row = index - 231;
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B58B);
            gHoverTextD68AE8 = states[petIndex(row)] == 1 ? L"这只宠物正处于待命状态。" /* 原常量参考：0x00586C94。 */ : L"使这只宠物处于待命状态。" /* 原常量参考：0x00586C78。 */;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        const std::int32_t previous = states[selected];
        if (previous == 3) {
            return consumed;
        }
        if (previous == 1) {
            states[selected] = 0;
            static_cast<void>(WindowPetList_SubmitStates());
            return consumed;
        }
        if (previous == 2) {
            WindowCraft_ResetItemSelection();
        }
        std::int32_t count = 0;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            const std::int32_t value = states[slot];
            if (value == 1) {
                ++count;
            }
            if (value == 2) {
                ++count;
            }
        }
        const std::int16_t target = petIndex(row);
        if (count >= 3 && states[target] != 2) {
            const std::int32_t firstReady = gLoginWindowRuntime.valuesCC4CE8[2];
            states[firstReady] &= ~1;
        }
        states[target] = 1;
        static_cast<void>(WindowPetList_SubmitStates());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (elementId >= static_cast<ElementId>(WindowPetListElementId::HandleButtons_237) && elementId <= static_cast<ElementId>(WindowPetListElementId::HandleButtons_241)) {
        const std::int32_t row = index - 236;
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B58E);
            gHoverTextD68AE8 = states[petIndex(row)] == 2 ? L"这只宠物正处于战斗状态。" /* 原常量参考：0x00586C5C。 */ : L"使这只宠物处于战斗状态。" /* 原常量参考：0x00586C40。 */;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        const std::int32_t previous = states[selected];
        if (previous == 3) {
            return consumed;
        }
        if (previous == 2) {
            states[selected] = 0;
        } else {
            const std::int32_t oldBattle = gLoginWindowRuntime.valuesCC4CE8[0];
            if (oldBattle != -1) {
                states[oldBattle] &= ~2;
            }
            states[selected] = 2;
        }
        static_cast<void>(WindowPetList_SubmitStates());
        WindowCraft_ResetItemSelection();
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (elementId >= static_cast<ElementId>(WindowPetListElementId::HandleButtons_242) && elementId <= static_cast<ElementId>(WindowPetListElementId::HandleButtons_246)) {
        const std::int32_t row = index - 241;
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B591);
            gHoverTextD68AE8 = states[petIndex(row)] == 3 ? L"无法将此宠物放在地上。" /* 原常量参考：0x00586C28。 */ : kPetPlaceHint586C14;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) == 0) {
            return consumed;
        }
        if (states[petIndex(row)] == 2) {
            WindowCraft_ResetItemSelection();
        }
        const std::int16_t selected = petIndex(row);
        if (states[selected] == 3) {
            states[selected] = 0;
            static_cast<void>(WindowPetList_SubmitStates());
        } else if ((world::ReadPlayerActionFlags() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0) {
            states[petIndex(row)] = 3;
            static_cast<void>(WindowPetList_SubmitStates());
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }
    return 0;
}

// 原函数：0x0048D460。
static void WindowPetList_RefreshStates()
{
    gLoginWindowRuntime.valuesCC4CE8[0] = -1;
    gLoginWindowRuntime.valuesCC4CE8[2] = -1;
    gLoginWindowRuntime.valuesCC4CE8[3] = -1;
    gLoginWindowRuntime.valuesCC4CE8[4] = -1;
    gLoginWindowRuntime.valuesCC4CE8[1] = -1;
    gLoginWindowRuntime.countCC4CFC = 0;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        const auto& pet = world::gPetRuntimeRecords[slot];
        const std::int32_t state = std::bit_cast<std::int16_t>(pet.flags6AA);
        gLoginWindowRuntime.valuesCC1A18[slot] = state;
        if (state == 2) {
            gLoginWindowRuntime.valuesCC4CE8[0] = slot;
        } else if (state == 1) {
            const std::int32_t count = gLoginWindowRuntime.countCC4CFC;
            gLoginWindowRuntime.valuesCC4CE8[count + 2] = slot;
            gLoginWindowRuntime.countCC4CFC = gLoginWindowRuntime.countCC4CFC + 1;
        }
        if (pet.follow6D4 != 0) {
            gLoginWindowRuntime.valuesCC4CE8[1] = slot;
        }
    }
}

// 原48C920四段同构属性条：先逐格隐藏，再取S16宠物索引，图号/显隐分别重取窗口。
static void WindowPetList_RefreshAttributeBar(
    const std::int32_t row, const world::PetPairRecord* const pair,
    const std::int32_t attribute, const std::int32_t singleImage,
    const std::int32_t middleImage, const std::int32_t firstImage,
    const std::int32_t lastImage)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const std::int32_t base = 26 + row * 40 + attribute * 10;
    for (std::int32_t segment = 0; segment < 10; ++segment) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(base + segment)); indexedElement) indexedElement->visible = 0;
    }
    const std::int16_t petIndex = std::bit_cast<std::int16_t>(pair->index);
    const std::int32_t segments =
        world::gPetRuntimeRecords[petIndex].values70[21 + attribute] / 10;
    const auto show = [currentWindow](const std::int32_t element, const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->data.graphic->imageNo = image;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(element)); indexedElement2) indexedElement2->visible = 1;
    };
    if (segments == 1) {
        show(base, singleImage);
    } else {
        for (std::int32_t segment = 0; segment < segments; ++segment) {
            show(base + segment, middleImage);
            if (segment == 0) {
                show(base, firstImage);
            }
            if (segment == segments - 1) {
                show(base + segment, lastImage);
            }
        }
    }
}

// 原函数：0x0048C920。
static void WindowPetList_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    WindowPetList_RefreshStates();
    std::wstring buffer;
    const auto copyNumber = [&buffer, currentWindow](const std::int32_t element) {
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(element));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = buffer;
        }
        return;
    };
    const auto setImage = [currentWindow](const std::int32_t element, const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto* const pair = &world::gPetPairsDE0B08[row];
        // 只固定索引对地址，技能查询和各次CryptInt读取之间重新取编号。
        const auto petIndex = [pair]() {
            return std::bit_cast<std::int16_t>(pair->index);
        };
        const std::int16_t initialIndex = petIndex();
        WindowInstance* const initialWindow = currentWindow;
        const std::int32_t textBase = 1 + row * 5;
        if (world::gPetRuntimeRecords[initialIndex].used == 0) {
            if (auto* const indexedElement = initialWindow->FindElementById(NativeElementKey(textBase)); indexedElement) indexedElement->visible = 0;
            for (std::int32_t column = 1; column < 5; ++column) {
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(textBase + column)); indexedElement2) indexedElement2->visible = 0;
            }
            for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
                for (std::int32_t segment = 0; segment < 10; ++segment) {
                    if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(26 + row * 40 + attribute * 10 + segment)); indexedElement2) indexedElement2->visible = 0;
                }
            }
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(231 + row)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(236 + row)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(226 + row)); indexedElement4) indexedElement4->visible = 0;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(241 + row)); indexedElement5) indexedElement5->visible = 0;
            continue;
        }

        if (auto* const indexedElement = initialWindow->FindElementById(NativeElementKey(textBase)); indexedElement) indexedElement->visible = 1;
        for (std::int32_t column = 1; column < 5; ++column) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(textBase + column)); indexedElement2) indexedElement2->visible = 1;
        }
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(226 + row)); indexedElement2) indexedElement2->visible = 1;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(231 + row)); indexedElement3) indexedElement3->visible = 1;
        if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(236 + row)); indexedElement4) indexedElement4->visible = 1;
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(226 + row)); indexedElement5) indexedElement5->visible = 1;
        }
        const std::int16_t loyaltyIndex = petIndex();
        const bool canSelect = world::gPetRuntimeRecords[loyaltyIndex].values70[10] == 100
            || gLoginWindowRuntime.valuesCC1A18[loyaltyIndex] == 3;
        if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(241 + row)); indexedElement5) indexedElement5->visible = canSelect ? 1 : 0;

        const auto& namePet = world::gPetRuntimeRecords[petIndex()];
        const wchar_t* const name =
            namePet.name6BD[0] != '\0' ? namePet.name6BD.data() : namePet.name6AC.data();
        auto* nameElementsWindow = currentWindow;
        auto* const nameDataElement = nameElementsWindow->FindElementById(NativeElementKey(textBase));
        if (nameDataElement) {
            auto* const nameData = nameDataElement->data.text;
            nameData->text = name;
        }

        const std::int16_t colorIndex = petIndex();
        WindowInstance* const colorWindow = currentWindow;
        const std::int32_t colorState = gLoginWindowRuntime.valuesCC1A18[colorIndex];
        auto* const indexedElement6 = colorWindow->FindElementById(NativeElementKey(textBase));
        if (indexedElement6) {
            indexedElement6->data.text->style.color =
                colorState == 3 ? std::byte{7} : std::byte{0};
        }
        cg::text::FormatTo(buffer, L"{:3}", world::gPetRuntimeRecords[petIndex()].value08);
        copyNumber(textBase + 1);
        const std::int32_t maxHp = static_cast<std::int32_t>(world::gPetRuntimeRecords[petIndex()].values0C[1]);
        const std::int32_t hp = static_cast<std::int32_t>(world::gPetRuntimeRecords[petIndex()].values0C[0]);
        cg::text::FormatTo(buffer, L"{:6}/{:6}", hp, maxHp);
        copyNumber(textBase + 2);
        const std::int32_t maxFp = static_cast<std::int32_t>(world::gPetRuntimeRecords[petIndex()].values0C[3]);
        const std::int32_t fp = static_cast<std::int32_t>(world::gPetRuntimeRecords[petIndex()].values0C[2]);
        cg::text::FormatTo(buffer, L"{:6}/{:6}", fp, maxFp);
        copyNumber(textBase + 3);

        const std::int16_t tribeIndex = petIndex();
        WindowInstance* const tribeWindow = currentWindow;
        const std::int16_t tribe =
            std::bit_cast<std::int16_t>(world::gPetRuntimeRecords[tribeIndex].value04);
        auto* tribeElementsWindow = tribeWindow;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const tribeDataElement = tribeElementsWindow->FindElementById(NativeElementKey(textBase + 4));
        if (tribeDataElement) {
            auto* const tribeData = tribeDataElement->data.text;
            tribeData->text = tribeName;
        }

        const std::int32_t state = gLoginWindowRuntime.valuesCC1A18[petIndex()];
        if (state >= 0 && state <= 3) {
            setImage(231 + row, state == 1 ? 0x3B589 : 0x3B58A);
            setImage(236 + row, state == 2 ? 0x3B58C : 0x3B58D);
            setImage(241 + row, state == 3 ? 0x3B58F : 0x3B590);
        }
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            const std::int16_t followIndex = petIndex();
            WindowInstance* const followWindow = currentWindow;
            const std::int32_t follow = world::gPetRuntimeRecords[followIndex].follow6D4;
            auto* const indexedElement7 = followWindow->FindElementById(NativeElementKey(226 + row));
            if (indexedElement7) {
                indexedElement7->data.graphic->imageNo =
                    follow != 0 ? 0x3B586 : 0x3B587;
            }
        } else {
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(226 + row)); indexedElement7) indexedElement7->visible = 0;
        }
        WindowPetList_RefreshAttributeBar(row, pair, 0, 0x3B5D2, 0x3B5D0, 0x3B5CF, 0x3B5D1);
        WindowPetList_RefreshAttributeBar(row, pair, 1, 0x3B5D7, 0x3B5D5, 0x3B5D4, 0x3B5D6);
        WindowPetList_RefreshAttributeBar(row, pair, 2, 0x3B5DC, 0x3B5DA, 0x3B5D9, 0x3B5DB);
        WindowPetList_RefreshAttributeBar(row, pair, 3, 0x3B5E1, 0x3B5DF, 0x3B5DE, 0x3B5E0);
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        const bool selected = gLoginWindowRuntime.valuesD2EAA0[0] == 1
            && gLoginWindowRuntime.valuesD2EAA0[1] == row;
        setImage(246 + row, selected ? 0x3BAE4 : 0x3B5C4);
    }
}

// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
static std::int32_t WindowPetList_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valuesD2EAA0[0] = 0;
        gLoginWindowRuntime.valuesD2EAA0[1] = -1;
        gLoginWindowRuntime.valuesD2EAA0[2] = -1;
    }
    WindowPetList_Refresh();
    return 1;
}

} // namespace detail

} // namespace cg::ui
