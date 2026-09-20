#include "LoginMessageDependencies.hpp"

#include <cstdint>

namespace cg::login::detail {

std::int32_t Sub_5049A0(
    const std::int32_t left,
    const std::int32_t top,
    const std::int32_t right,
    const std::int32_t bottom,
    const std::int32_t layer)
{
    if (ReadLegacyMouseX() >= right) {
        return 0;
    }
    if (left > ReadLegacyMouseX()) {
        return 0;
    }
    if (ReadLegacyMouseY() >= bottom) {
        return 0;
    }
    if (top > ReadLegacyMouseY()) {
        return 0;
    }

    if (layer >= 0) {
        DrawGraphicHover(
            static_cast<std::uint16_t>(left),
            static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(right),
            static_cast<std::uint16_t>(bottom),
            static_cast<std::uint8_t>(layer),
            gValueD69768,
            0);
    }
    return 1;
}

} // namespace cg::login::detail
