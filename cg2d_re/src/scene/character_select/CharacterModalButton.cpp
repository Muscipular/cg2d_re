#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>

namespace cg::character_select::detail {
// 原函数：0x00503990。
std::int32_t Sub_503990(const CharacterModalUiRecord* uiRecord)
{
    const std::int32_t top = uiRecord->top;
    const std::int32_t left = uiRecord->left;
    const std::int32_t height = uiRecord->height;
    const std::int32_t width = uiRecord->width;
    const std::int32_t bottom = height + top;
    const std::int32_t right = width + left;
    if (cg::ui::HitTestMouseRect_5049A0(
            left,
            top,
            right,
            bottom,
            -1)
        == 0) {
        return 0;
    }

    const std::uint8_t clickState =
        static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
    std::uint32_t result = 0x40U;
    if ((clickState & 0x01U) != 0) {
        result |= 0x01U;
    }
    if ((clickState & 0x02U) != 0) {
        result |= 0x04U;
    }

    const std::uint8_t buttonState2 =
        static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.heldButtons);
    if ((buttonState2 & 0x01U) != 0) {
        result |= 0x02U;
    }
    if ((buttonState2 & 0x02U) != 0) {
        result |= 0x08U;
    }

    const std::uint8_t buttonState =
        static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.pulseButtons);
    if ((buttonState & 0x01U) != 0) {
        result |= 0x10U;
    }
    if ((buttonState & 0x02U) != 0) {
        result |= 0x20U;
    }
    if ((clickState & 0x10U) != 0) {
        result |= 0x80U;
    }
    if ((clickState & 0x20U) != 0) {
        result |= 0x100U;
    }
    return static_cast<std::int32_t>(result);
}

// 原函数：0x00503A20。
std::int32_t Sub_503A20(
    const CharacterModalUiRecord* uiRecord,
    std::uint8_t renderMode,
    std::int8_t color,
    std::int32_t hoverMode,
    std::uint32_t hoverImageSequence,
    std::uint8_t layer)
{
    if (hoverMode != 0) {
        const std::int32_t hitTop = uiRecord->top;
        const std::int32_t hitHeight = uiRecord->height;
        const std::int32_t hitLeft = uiRecord->left;
        const std::int32_t hitWidth = uiRecord->width;
        const std::int32_t hitBottom = hitHeight + hitTop;
        const std::int32_t hitRight = hitLeft + hitWidth;
        if (cg::ui::HitTestMouseRect_5049A0(
                hitLeft,
                hitTop,
                hitRight,
                hitBottom,
                -1)
            != 0) {
            const std::int32_t hoverTop = uiRecord->top;
            const std::int32_t step = hoverMode - 1;
            const std::int32_t hoverLeft = uiRecord->left;
            const std::int32_t hoverHeight = uiRecord->height;
            const std::int32_t bottom = hoverHeight + hoverTop + 1;
            const std::int32_t top = hoverTop - 3;
            const std::int32_t hoverWidth = uiRecord->width;
            const std::int32_t right = hoverLeft + hoverWidth + 2;
            const std::int32_t left = hoverLeft - 2;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left),
                static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(right),
                static_cast<std::uint16_t>(bottom),
                layer,
                hoverImageSequence,
                step);
        }
    }

    const wchar_t* const text = uiRecord->text;
    const std::int16_t textY = static_cast<std::int16_t>(uiRecord->textY);
    const std::int16_t textX = static_cast<std::int16_t>(uiRecord->textX);
    return cg::render::DrawText_456780(
        textX,
        textY,
        renderMode,
        0,
        color,
        text,
        0,
        0);
}

} // namespace cg::character_select::detail
