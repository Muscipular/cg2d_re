#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004990F0。
std::int32_t WindowSpectator_ButtonEnd(const std::int32_t index, const std::int32_t state)
{
    if ((state & 1) != 0) {
        WindowInstance* const window = gCurrentWindow;
        const std::uint32_t image = login::detail::gValueD69768;
        WindowElement* const elements = window->elements;
        const std::int32_t windowY = window->y;
        const std::int32_t windowX = window->x;
        const std::int32_t x = elements[index].x;
        const std::int32_t y = elements[index].y;
        const std::int32_t height = elements[index].height;
        const std::int32_t width = elements[index].width;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(windowX + x - 2),
            static_cast<std::uint16_t>(windowY + y - 2),
            static_cast<std::uint16_t>(width + windowX + x + 2),
            static_cast<std::uint16_t>(height + windowY + y + 2), 223, image, 0);
        gHoverTextD68AE8 = L"結束觀戰。";
    }
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"U"));
    world::gCurrentSceneState = 7;
    return 1;
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowTextPrototype gWindowText56B8D0 {{}, L"觀戰結束"}; // 原全局：0x0056B8D0。 // 原常量参考：0x00554184。

} // namespace

// 原型：窗口29；原全局：0x0054E750。
WindowPrototype gWindowPrototypeSpectator {2, 3, 492, 0, 148, 108, 0x80010101u, 2, gWindowElements56B8D8, &WindowRuntime_ReturnOne, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}; // 29: 54E750

WindowElementPrototype gWindowElements56B8D8[2] { // 原全局：0x0056B8D8。
    {WindowElementType_Text, 32, 46, 0, 0, 1, {.text = &gWindowText56B8D0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 32, 46, 83, 16, 1, {.unusedValue = 0}, &WindowSpectator_ButtonEnd},
};

} // namespace cg::ui::detail
