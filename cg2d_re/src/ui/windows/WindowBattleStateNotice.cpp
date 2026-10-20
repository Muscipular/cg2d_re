#include "../WindowRuntimeInternal.hpp"
#include "../../world/BattleState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowBattleStateNoticeElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049BD40。
static std::int32_t WindowBattleStateNotice_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowTextPrototype gWindowText56C9A8 {{}, L"偷袭"}; // 原全局：0x0056C9A8。 // 原常量参考：0x00585F70。

static WindowTextPrototype gWindowText56C9B0 {{}, L"被偷袭"}; // 原全局：0x0056C9B0。 // 原常量参考：0x00585F68。

static WindowElementPrototype gWindowElements56C9B8[2] { // 原全局：0x0056C9B8。
    {WindowElementType_Text, 78, 40, 0, 0, 1, {.text = &gWindowText56C9A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleStateNoticeElementId::Element1)},
    {WindowElementType_Text, 68, 40, 0, 0, 1, {.text = &gWindowText56C9B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleStateNoticeElementId::Element2)},
};

// 原型：窗口32；原全局：0x0054E840。
WindowPrototype gWindowPrototypeBattleStateNotice {2, 3, 224, 162, 192, 96, 0x80010101u, 2, gWindowElements56C9B8, &WindowBattleStateNotice_Update, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}; // 32: 54E840

// 窗口函数实现。

// 原函数：0x0043C950。
void WindowBattleStateNotice_Open()
{
    if ((cg::world::gBattlePlayerFlags5ABCAC & 3u) != 0
        && cg::world::gBattleEntryNoticeAction5ABFB8 == nullptr && gWindowStates[WindowId_BattleStateNotice].window == nullptr) {
        cg::world::gBattleEntryNoticeAction5ABFB8 = WindowRuntime_CreateAnimation(WindowId_BattleStateNotice, 1, 0, nullptr);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
    }
}

// 原函数：0x0049BD40。
static std::int32_t WindowBattleStateNotice_Update(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const initialWindow = currentWindow;
    world::gBattleEntryNoticeAction5ABFB8 = nullptr;
    if (auto* const element1 = initialWindow->FindElementById(WindowBattleStateNoticeElementId::Element1); element1) element1->visible = 0;
    if (auto* const element2 = currentWindow->FindElementById(WindowBattleStateNoticeElementId::Element2); element2) element2->visible = 0;
    if (world::gCurrentSceneState != 3) {
        currentWindow->state |= 0x8000u;
    }
    const auto flags = static_cast<std::uint8_t>(world::gBattlePlayerFlags5ABCAC);
    if ((flags & 1u) != 0) {
        if (auto* const element12 = currentWindow->FindElementById(WindowBattleStateNoticeElementId::Element1); element12) element12->visible = 1;
    } else if ((flags & 2u) != 0) {
        if (auto* const element22 = currentWindow->FindElementById(WindowBattleStateNoticeElementId::Element2); element22) element22->visible = 1;
    } else {
        currentWindow->state |= 0x8000u;
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
