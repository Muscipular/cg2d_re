#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048C1E0。
static std::int32_t WindowPetList_HandleButtons(std::int32_t index, std::int32_t state); // 窗口15完整五组交互。

// 原函数：0x0048D4E0。
static std::int32_t WindowPetList_SubmitStates(); // 五状态局部快照叠加真实跟随位，透传发送结果。

// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
static std::int32_t WindowPetList_Update(std::int32_t value);

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
    {WindowElementType_Graphic, 302, 8, 11, 11, 1, {.graphic = &gWindowGraphic563298}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 23, 34, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 34, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 47, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 60, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 49, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 82, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 82, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 95, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 108, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 97, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 130, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 130, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 143, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 156, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 145, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 178, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 178, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 191, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 204, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 193, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 226, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 226, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 239, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 252, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 241, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 259, 54, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 102, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 150, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 198, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 246, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 54, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 102, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 150, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 198, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 246, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 76, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 124, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 172, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 220, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5632B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 26, 0, 0, 1, {.graphic = &gWindowGraphic5632C8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 15, 28, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 76, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 124, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 172, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 220, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_EventArea, 323, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
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
static std::int32_t WindowPetList_SendStates(const std::array<std::int32_t, 5>& values)
{
    const std::int32_t fifth = values[4];
    const std::int32_t fourth = values[3];
    const std::int32_t third = values[2];
    const std::int32_t second = values[1];
    const std::int32_t first = values[0];
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::SendPetStates_511AE0(socket, first, second, third, fourth, fifth);
}

// 原函数：0x0048D4E0。
static std::int32_t WindowPetList_SubmitStates()
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
static std::int32_t WindowPetList_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    auto& states = gLoginWindowRuntime.valuesCC1A18;
    const auto petIndex = [](const std::int32_t row) {
        return std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[row].index);
    };
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };

    if (index >= 253 && index <= 257) {
        const std::int32_t row = index - 253;
        std::uint32_t events = static_cast<std::uint32_t>(state);
        if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3) {
            consumed = 1;
            events &= ~0x60u;
        }
        if ((events & 0x40u) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            const std::int16_t target = petIndex(row);
            WindowItem_RequestPetTransfer(15, index, source, target);
            WindowDrag_Reset();
        }
        if ((events & 1u) != 0) {
            gHoverTextD68AE8 = L"打开此宠物的能力视窗。" /* 原常量参考：0x00586D0C。 */;
            consumed = 1;
        }
        if ((events & 2u) != 0) {
            const std::int16_t selected = petIndex(row);
            const auto& pet = world::gPetRuntimeRecords[selected];
            if (pet.used != 0) {
                WindowInstance* const detailsWindow = gWindowStates[WindowId_PetDetail].window;
                gLoginWindowRuntime.selectedPetCC3E5C = row;
                gLoginWindowRuntime.valueD673D0 = 5;
                if (detailsWindow != nullptr || gWindowStates[WindowId_PetAttributes].window != nullptr
                    || gWindowStates[WindowId_PetSkills].window != nullptr) {
                    const char* const name =
                        pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    gPetNameInputPrototype57DCF8.text = cg::text::FromGbkWide(name);
                    WindowInstance* const window = gCurrentWindow;
                    const std::int32_t y = window->y;
                    const std::int32_t x = window->x;
                    static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetDetail, 1, 0, nullptr));
                }
            }
            static_cast<void>(WindowDrag_ResetMouse());
            consumed = 1;
        } else if ((events & 1u) != 0 && (events & 4u) != 0) {
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

    if (index >= 226 && index <= 230) {
        const std::int32_t row = index - 226;
        if (character_select::state::gPukVersion < 7u) {
            gCurrentWindow->elements[index].visible = 0;
            return 0;
        }
        if (WindowPet_HasSkill25600(petIndex(row)) != 1) {
            return 0;
        }
        if ((state & 1) != 0) {
            setImage(0x3B588);
            const std::int32_t follow = world::gPetRuntimeRecords[petIndex(row)].follow6D4;
            gHoverTextD68AE8 = follow != 0 ? L"无法将此宠物带在身后散步。" /* 原常量参考：0x00586CF0。 */ : L"将此宠物带在身后散步。" /* 原常量参考：0x00586CD8。 */;
            consumed = 1;
        }
        if ((state & 2) == 0) {
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
            world::Sub_44B050(L"宠物忠诚度需达到60以上才可以带它散步喔!" /* 原常量参考：0x00586CB0。 */, 4, 3);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            return consumed;
        }
        states[selected] |= 0x10;
        static_cast<void>(WindowPetList_SendStates(states));
        states[petIndex(row)] &= ~0x10;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (index >= 231 && index <= 235) {
        const std::int32_t row = index - 231;
        if ((state & 1) != 0) {
            setImage(0x3B58B);
            gHoverTextD68AE8 = states[petIndex(row)] == 1 ? L"这只宠物正处于待命状态。" /* 原常量参考：0x00586C94。 */ : L"使这只宠物处于待命状态。" /* 原常量参考：0x00586C78。 */;
            consumed = 1;
        }
        if ((state & 2) == 0) {
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

    if (index >= 236 && index <= 240) {
        const std::int32_t row = index - 236;
        if ((state & 1) != 0) {
            setImage(0x3B58E);
            gHoverTextD68AE8 = states[petIndex(row)] == 2 ? L"这只宠物正处于战斗状态。" /* 原常量参考：0x00586C5C。 */ : L"使这只宠物处于战斗状态。" /* 原常量参考：0x00586C40。 */;
            consumed = 1;
        }
        if ((state & 2) == 0) {
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

    if (index >= 241 && index <= 245) {
        const std::int32_t row = index - 241;
        if ((state & 1) != 0) {
            setImage(0x3B591);
            gHoverTextD68AE8 = states[petIndex(row)] == 3 ? L"无法将此宠物放在地上。" /* 原常量参考：0x00586C28。 */ : kPetPlaceHint586C14;
            consumed = 1;
        }
        if ((state & 2) == 0) {
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
            && world::Sub_440B50() == 0) {
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
    const std::int32_t base = 26 + row * 40 + attribute * 10;
    for (std::int32_t segment = 0; segment < 10; ++segment) {
        gCurrentWindow->elements[base + segment].visible = 0;
    }
    const std::int16_t petIndex = std::bit_cast<std::int16_t>(pair->index);
    const std::int32_t segments =
        world::gPetRuntimeRecords[petIndex].values70[21 + attribute] / 10;
    const auto show = [](const std::int32_t element, const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = image;
        gCurrentWindow->elements[element].visible = 1;
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
    WindowPetList_RefreshStates();
    std::wstring buffer;
    const auto copyNumber = [&buffer](const std::int32_t element) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[element].data);
        data->text = buffer;
    };
    const auto setImage = [](const std::int32_t element, const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = image;
    };
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto* const pair = &world::gPetPairsDE0B08[row];
        // 只固定索引对地址，技能查询和各次CryptInt读取之间重新取编号。
        const auto petIndex = [pair]() {
            return std::bit_cast<std::int16_t>(pair->index);
        };
        const std::int16_t initialIndex = petIndex();
        WindowInstance* const initialWindow = gCurrentWindow;
        const std::int32_t textBase = 1 + row * 5;
        if (world::gPetRuntimeRecords[initialIndex].used == 0) {
            initialWindow->elements[textBase].visible = 0;
            for (std::int32_t column = 1; column < 5; ++column) {
                gCurrentWindow->elements[textBase + column].visible = 0;
            }
            for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
                for (std::int32_t segment = 0; segment < 10; ++segment) {
                    gCurrentWindow->elements[26 + row * 40 + attribute * 10 + segment].visible = 0;
                }
            }
            gCurrentWindow->elements[231 + row].visible = 0;
            gCurrentWindow->elements[236 + row].visible = 0;
            gCurrentWindow->elements[226 + row].visible = 0;
            gCurrentWindow->elements[241 + row].visible = 0;
            continue;
        }

        initialWindow->elements[textBase].visible = 1;
        for (std::int32_t column = 1; column < 5; ++column) {
            gCurrentWindow->elements[textBase + column].visible = 1;
        }
        gCurrentWindow->elements[226 + row].visible = 1;
        gCurrentWindow->elements[231 + row].visible = 1;
        gCurrentWindow->elements[236 + row].visible = 1;
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            gCurrentWindow->elements[226 + row].visible = 1;
        }
        const std::int16_t loyaltyIndex = petIndex();
        const bool canSelect = world::gPetRuntimeRecords[loyaltyIndex].values70[10] == 100
            || gLoginWindowRuntime.valuesCC1A18[loyaltyIndex] == 3;
        gCurrentWindow->elements[241 + row].visible = canSelect ? 1 : 0;

        const auto& namePet = world::gPetRuntimeRecords[petIndex()];
        const char* const name =
            namePet.name6BD[0] != '\0' ? namePet.name6BD.data() : namePet.name6AC.data();
        WindowElement* const nameElements = gCurrentWindow->elements;
        auto* const nameData = static_cast<WindowTextData*>(nameElements[textBase].data);
        nameData->text = cg::text::FromGbkWide(name);

        const std::int16_t colorIndex = petIndex();
        WindowInstance* const colorWindow = gCurrentWindow;
        const std::int32_t colorState = gLoginWindowRuntime.valuesCC1A18[colorIndex];
        static_cast<WindowTextData*>(colorWindow->elements[textBase].data)->style.color =
            colorState == 3 ? std::byte{7} : std::byte{0};
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
        WindowInstance* const tribeWindow = gCurrentWindow;
        const std::int16_t tribe =
            std::bit_cast<std::int16_t>(world::gPetRuntimeRecords[tribeIndex].value04);
        WindowElement* const tribeElements = tribeWindow->elements;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const tribeData = static_cast<WindowTextData*>(tribeElements[textBase + 4].data);
        tribeData->text = tribeName;

        const std::int32_t state = gLoginWindowRuntime.valuesCC1A18[petIndex()];
        if (state >= 0 && state <= 3) {
            setImage(231 + row, state == 1 ? 0x3B589 : 0x3B58A);
            setImage(236 + row, state == 2 ? 0x3B58C : 0x3B58D);
            setImage(241 + row, state == 3 ? 0x3B58F : 0x3B590);
        }
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            const std::int16_t followIndex = petIndex();
            WindowInstance* const followWindow = gCurrentWindow;
            const std::int32_t follow = world::gPetRuntimeRecords[followIndex].follow6D4;
            static_cast<WindowGraphicData*>(followWindow->elements[226 + row].data)->imageNo =
                follow != 0 ? 0x3B586 : 0x3B587;
        } else {
            gCurrentWindow->elements[226 + row].visible = 0;
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
static std::int32_t WindowPetList_Update(const std::int32_t value)
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
