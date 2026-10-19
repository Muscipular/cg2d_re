#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "UiControlRecord.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"

#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"

#include <cstdint>

namespace cg::ui {

// RHL01：四次CryptInt比较按X上界/X下界/Y上界/Y下界短路，命中可描边并返回S32 1。
// 原函数：0x005049A0。
std::int32_t HitTestMouseRect_5049A0(
    const std::int32_t left,
    const std::int32_t top,
    const std::int32_t right,
    const std::int32_t bottom,
    const std::int32_t layer)
{
    if (!(platform::gLegacyMouseState.mouseX < right + LegacyUiLayoutScope::GetOffsetX())) {
        return 0;
    }
    if (!(left + LegacyUiLayoutScope::GetOffsetX() <= platform::gLegacyMouseState.mouseX)) {
        return 0;
    }
    if (!(platform::gLegacyMouseState.mouseY < bottom + LegacyUiLayoutScope::GetOffsetY())) {
        return 0;
    }
    if (!(top + LegacyUiLayoutScope::GetOffsetY() <= platform::gLegacyMouseState.mouseY)) {
        return 0;
    }

    if (layer >= 0) {
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left),
            static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(right),
            static_cast<std::uint16_t>(bottom),
            static_cast<std::uint8_t>(layer),
            cg::render::gHoverFramePaletteIndex_D69768,
            0);
    }
    return 1;
}

} // namespace cg::ui
