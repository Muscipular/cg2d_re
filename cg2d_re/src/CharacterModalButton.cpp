#include "CharacterSelectSceneDependencies.hpp"

#include <bit>
#include <cstdint>

namespace cg::character_select::detail {
namespace {

std::int16_t LowInt16(std::int32_t value)
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(value));
}

} // namespace

std::int32_t Sub_503990(const CharacterModalUiRecord* uiRecord)
{
    const std::int32_t top = uiRecord->top;
    const std::int32_t left = uiRecord->left;
    const std::int32_t height = uiRecord->height;
    const std::int32_t width = uiRecord->width;
    const std::int32_t bottom = height + top;
    const std::int32_t right = width + left;
    if (login::detail::Sub_5049A0(
            left,
            top,
            right,
            bottom,
            -1)
        == 0) {
        return 0;
    }

    const std::uint8_t clickState =
        static_cast<std::uint8_t>(login::detail::gValueD68D9C);
    std::uint32_t result = 0x40U;
    if ((clickState & 0x01U) != 0) {
        result |= 0x01U;
    }
    if ((clickState & 0x02U) != 0) {
        result |= 0x04U;
    }

    const std::uint8_t buttonState2 =
        static_cast<std::uint8_t>(login::detail::gValueD68D98);
    if ((buttonState2 & 0x01U) != 0) {
        result |= 0x02U;
    }
    if ((buttonState2 & 0x02U) != 0) {
        result |= 0x08U;
    }

    const std::uint8_t buttonState =
        static_cast<std::uint8_t>(login::detail::gValueD68DA0);
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
        if (login::detail::Sub_5049A0(
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
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left),
                static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(right),
                static_cast<std::uint16_t>(bottom),
                layer,
                hoverImageSequence,
                step);
        }
    }

    const char* const text = uiRecord->text;
    const std::int16_t textY = LowInt16(uiRecord->textY);
    const std::int16_t textX = LowInt16(uiRecord->textX);
    return login::detail::DrawText(
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
