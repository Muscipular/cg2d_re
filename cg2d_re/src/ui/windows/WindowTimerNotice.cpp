#include "cg/render/Font.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049E750。
static std::int32_t WindowTimerNotice_Update(std::int32_t value); // 窗口39文本居中/到时关闭请求。

// 控件载荷、窗口状态、组件表与原型。

// 原56CCC8的四字节元数据和空文本；界面直接使用宽字面量。
static WindowTextPrototype gWindowText56CCC8 {{}, L""}; // 原全局：0x0056CCC8。

static WindowElementPrototype gWindowElements56CCD0[1] { // 原全局：0x0056CCD0。
    {WindowElementType_Text, 0, 44, 0, 0, 1, {.text = &gWindowText56CCC8}, &WindowElement_NoOp},
};

// 原型：窗口39；原全局：0x0054E8A0。
WindowPrototype gWindowPrototypeTimerNotice {3, 3, 196, 180, 256, 96, 0x80010101u, 1, gWindowElements56CCD0, &WindowTimerNotice_Update, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}; // 39: 54E8A0

// 窗口函数实现。

// 原函数：0x0049E750。
static std::int32_t WindowTimerNotice_Update(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = gCurrentWindow->elements[0].data.text;
        const std::wstring& text = world::GetMapName_9DAD30();
        data->text = text;
        const std::int32_t textWidth = cg::render::TextMeasure(text, 0);
        WindowInstance* const window = gCurrentWindow;
        window->elements[0].x = (window->width - textWidth) >> 1;
    } else {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (world::gMapNoticeStartTick_62B5B0 + 2000u <= tick) {
            gCurrentWindow->state |= 0x8000u;
            world::gMapNoticeStartTick_62B5B0 = 0;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
