#include "CharacterSelectSceneDependencies.hpp"

#include <bit>
#include <cstdint>

namespace cg::character_select::detail {
namespace {

std::uint32_t ToBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t AddWrapped(std::int32_t left, std::int32_t right)
{
    return std::bit_cast<std::int32_t>(ToBits(left) + ToBits(right));
}

std::int16_t LowInt16(std::uint32_t value)
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(value));
}

} // namespace

std::int32_t Sub_503990(const CharacterModalUiRecord* uiRecord)
{
    const std::int32_t right = AddWrapped(uiRecord->left, uiRecord->width);
    const std::int32_t bottom = AddWrapped(uiRecord->top, uiRecord->height);
    if (login::detail::Sub_5049A0(
            uiRecord->left,
            uiRecord->top,
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
        const std::int32_t right =
            AddWrapped(uiRecord->left, uiRecord->width);
        const std::int32_t bottom =
            AddWrapped(uiRecord->top, uiRecord->height);
        if (login::detail::Sub_5049A0(
                uiRecord->left,
                uiRecord->top,
                right,
                bottom,
                -1)
            != 0) {
            const std::uint32_t leftBits = ToBits(uiRecord->left);
            const std::uint32_t topBits = ToBits(uiRecord->top);
            const std::uint32_t rightBits =
                leftBits + ToBits(uiRecord->width) + 2U;
            const std::uint32_t bottomBits =
                topBits + ToBits(uiRecord->height) + 1U;
            const std::int32_t step = std::bit_cast<std::int32_t>(
                ToBits(hoverMode) - 1U);
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(leftBits - 2U),
                static_cast<std::uint16_t>(topBits - 3U),
                static_cast<std::uint16_t>(rightBits),
                static_cast<std::uint16_t>(bottomBits),
                layer,
                hoverImageSequence,
                step);
        }
    }

    return login::detail::DrawText(
        LowInt16(ToBits(uiRecord->textX)),
        LowInt16(ToBits(uiRecord->textY)),
        renderMode,
        0,
        color,
        uiRecord->text,
        0,
        0);
}

} // namespace cg::character_select::detail
