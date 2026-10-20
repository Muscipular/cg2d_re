#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowSpectatorElementId : ElementId {
    Element1 = 1,
    ButtonEnd = 2,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004990F0。
static std::int32_t WindowSpectator_ButtonEnd(std::int32_t index, std::int32_t state); // 窗口29观战结束，hover不消费。

// 控件载荷、窗口状态、组件表与原型。

static WindowTextPrototype gWindowText56B8D0 {{}, L"观战结束"}; // 原全局：0x0056B8D0。 // 原常量参考：0x00554184。

static WindowElementPrototype gWindowElements56B8D8[2] { // 原全局：0x0056B8D8。
    {WindowElementType_Text, 32, 46, 0, 0, 1, {.text = &gWindowText56B8D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSpectatorElementId::Element1)},
    {WindowElementType_EventArea, 32, 46, 83, 16, 1, {.unusedValue = 0}, &WindowSpectator_ButtonEnd, -1, static_cast<ElementId>(WindowSpectatorElementId::ButtonEnd)},
};

// 原型：窗口29；原全局：0x0054E750。
WindowPrototype gWindowPrototypeSpectator {2, 3, 492, 0, 148, 108, 0x80010101u, 2, gWindowElements56B8D8, &WindowRuntime_ReturnOne, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}; // 29: 54E750

// 窗口函数实现。

// 原函数：0x004990F0。
static std::int32_t WindowSpectator_ButtonEnd(const std::int32_t index, const std::int32_t state)
{
    if ((state & 1) != 0) {
        WindowInstance* const window = gCurrentWindow;
        const std::uint32_t image = cg::render::gHoverFramePaletteIndex_D69768;
        auto* elementsWindow = window;
        const std::int32_t windowY = window->y;
        const std::int32_t windowX = window->x;
        if (!((elementsWindow && (*elementsWindow)[NativeElementKey(index)]))) { return {}; }
        const std::int32_t x = (*elementsWindow)[NativeElementKey(index)]->x;
        if (!((elementsWindow && (*elementsWindow)[NativeElementKey(index)]))) { return {}; }
        const std::int32_t y = (*elementsWindow)[NativeElementKey(index)]->y;
        if (!((elementsWindow && (*elementsWindow)[NativeElementKey(index)]))) { return {}; }
        const std::int32_t height = (*elementsWindow)[NativeElementKey(index)]->height;
        if (!((elementsWindow && (*elementsWindow)[NativeElementKey(index)]))) { return {}; }
        const std::int32_t width = (*elementsWindow)[NativeElementKey(index)]->width;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(windowX + x - 2),
            static_cast<std::uint16_t>(windowY + y - 2),
            static_cast<std::uint16_t>(width + windowX + x + 2),
            static_cast<std::uint16_t>(height + windowY + y + 2), 223, image, 0);
        gHoverTextD68AE8 = L"结束观战。";
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
