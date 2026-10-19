#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 窗口函数实现。

// 原函数：0x004C2460。
std::int32_t WindowCrystalTrade_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        gHoverTextD68AE8 = L"还是算了。" /* 原常量参考：0x00585AD0。 */;
    }
    if ((state & 2) != 0) {
        const std::int32_t target = world::gDialogTargetD2DE6C;
        const std::int32_t request = world::gDialogRequestCC21E0;
        const std::int32_t y =
            static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x =
            static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(network::SendDialogResponse_512620(
            platform::gConnectionSocket, x, y, request, target, 2, L""));
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 245320;
    if (hover) {
        graphic->imageNo = 245322;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 245321;
    }
    return consumed;
}

// 原函数：0x004C4680。
std::int32_t WindowCrystalTrade_DrawButton(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 243203;
    if ((state & 1) != 0) {
        graphic->imageNo = 243205;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243204;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
