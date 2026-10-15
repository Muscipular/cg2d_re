#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x0043C950。
void WindowBattleStateNotice_Open()
{
    using namespace world::detail;
    if ((gBattlePlayerFlags5ABCAC & 3u) != 0
        && gBattleEntryNoticeAction5ABFB8 == nullptr && gWindowStates[WindowId_BattleStateNotice].window == nullptr) {
        gBattleEntryNoticeAction5ABFB8 = WindowRuntime_CreateAnimation(WindowId_BattleStateNotice, 1, 0, nullptr);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
    }
}

// 原函数：0x0049BD40。
std::int32_t WindowBattleStateNotice_Update(const std::int32_t)
{
    WindowInstance* const initialWindow = gCurrentWindow;
    world::detail::gBattleEntryNoticeAction5ABFB8 = nullptr;
    initialWindow->elements[0].visible = 0;
    gCurrentWindow->elements[1].visible = 0;
    if (world::gCurrentSceneState != 3) {
        gCurrentWindow->state |= 0x8000u;
    }
    const auto flags = static_cast<std::uint8_t>(world::detail::gBattlePlayerFlags5ABCAC);
    if ((flags & 1u) != 0) {
        gCurrentWindow->elements[0].visible = 1;
    } else if ((flags & 2u) != 0) {
        gCurrentWindow->elements[1].visible = 1;
    } else {
        gCurrentWindow->state |= 0x8000u;
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
