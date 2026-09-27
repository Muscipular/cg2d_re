#include "WindowRuntimeState.hpp"

namespace cg::ui {
namespace detail {

std::array<WindowInstance*, kWindowCount> gWindows {};
std::array<WindowState, kWindowCount> gWindowStates {};
WindowInstance* gCurrentWindow = nullptr;
WindowState* gCurrentWindowState = nullptr;
std::uint8_t gValueD67BE0 = 0;
std::int32_t gDragItemIndex584DA4 = -1;

} // namespace detail

std::uint8_t ReadValueD67BE0() noexcept
{
    return detail::gValueD67BE0;
}

std::int32_t ReadDragItemIndex_584DA4() noexcept
{
    return detail::gDragItemIndex584DA4;
}

WindowInstance* Sub_4CD3E0()
{
    WindowInstance* const window = detail::gWindowStates[68].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

WindowInstance* Sub_4CD400()
{
    WindowInstance* const window = detail::gWindowStates[67].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

} // namespace cg::ui
