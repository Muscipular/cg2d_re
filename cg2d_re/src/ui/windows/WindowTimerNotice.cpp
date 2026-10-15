#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x0049E750。
std::int32_t WindowTimerNotice_Update(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
        const char* const text = world::ReadText9DAD30();
        data->text = cg::text::FromGbkWide(text);
        const std::int32_t textWidth = login::detail::TextMeasure(cg::text::FromGbkWide(text).c_str(), 0);
        WindowInstance* const window = gCurrentWindow;
        window->elements[0].x = (window->width - textWidth) >> 1;
    } else {
        const std::uint32_t tick = platform::ReadTickCount32();
        if (world::detail::gValue62B5B0 + 2000u <= tick) {
            gCurrentWindow->state |= 0x8000u;
            world::detail::gValue62B5B0 = 0;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
