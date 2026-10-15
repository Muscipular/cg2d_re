#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x00495D70。
std::int32_t WindowMiniMap_Update(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        data->text.clear();
        return 1;
    }
    if (world::detail::Sub_440B50() == 0) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        std::wstring& destination = data->text;
        if (destination[0] == '\0') {
            const char* const name = world::ReadText9DAD30();
            destination = cg::text::FromGbkWide(name);
            const std::int32_t measured = login::detail::TextMeasure(cg::text::FromGbkWide(name).c_str(), 12);
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t width = window->width;
            WindowElement* const elements = window->elements;
            elements[3].x = ((width - measured) >> 1) + 2;
        }
    }
    return 1;
}

// 原函数：0x00495E10。
std::int32_t WindowMiniMap_DrawMap(const std::int32_t index, const std::int32_t)
{
    if (world::detail::Sub_440B50() != 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const elements = window->elements;
    const std::int32_t windowY = window->y;
    const std::int32_t elementY = elements[index].y;
    const std::int32_t elementX = elements[index].x;
    const std::int32_t y = elementY + windowY - 117;
    const std::int32_t x = elementX + window->x - 14;
    static_cast<void>(render::QueueMap_521AA0(x, y, 0xDF));
    return 0;
}

// 原函数：0x00495E60。
std::int32_t WindowMiniMap_RefreshPlayerX(const std::int32_t index, const std::int32_t)
{
    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
    if (world::detail::Sub_440B50() != 0) {
        data->text.clear();
        return 0;
    }
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    cg::text::FormatTo(data->text, L"{:3}", x);
    return 0;
}

// 原函数：0x00495EB0。
std::int32_t WindowMiniMap_RefreshPlayerY(const std::int32_t index, const std::int32_t)
{
    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
    if (world::detail::Sub_440B50() != 0) {
        data->text.clear();
        return 0;
    }
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    cg::text::FormatTo(data->text, L"{:3}", y);
    return 0;
}

// 原函数：0x00495F00。
login::YobiAction* WindowMiniMap_Open(
    const std::uint32_t flags, const std::uint32_t mode,
    login::YobiAction** const completionSlot)
{
    WindowInstance* const window = gWindowStates[WindowId_MiniMap].window;
    std::int32_t x;
    std::int32_t y;
    if (window != nullptr) {
        window->state |= 0x8000u;
        WindowInstance* const current = gWindowStates[WindowId_MiniMap].window;
        x = current->x;
        y = current->y;
    } else {
        x = gWindowStates[WindowId_MiniMap].x;
        y = gWindowStates[WindowId_MiniMap].y;
    }
    return login::detail::Sub_475950(x, y, 184, 159, 33, flags, mode, completionSlot);
}

} // namespace detail

} // namespace cg::ui
