#include "scene/login/LoginMessageDependencies.hpp"

#include <bit>
#include <cstdint>

namespace cg::login::detail {

// 原函数：0x00522180。
void DrawGraphicHover(
    const std::uint16_t left,
    const std::uint16_t top,
    const std::uint16_t right,
    const std::uint16_t bottom,
    const std::uint8_t layer,
    const std::uint32_t imageNo,
    const std::int32_t step)
{
    const std::uint32_t packedX =
        (static_cast<std::uint32_t>(left) << 16U)
        | static_cast<std::uint32_t>(right);
    const std::uint32_t packedY =
        (static_cast<std::uint32_t>(top) << 16U)
        | static_cast<std::uint32_t>(bottom);

    std::uint32_t encodedImageNo = imageNo;
    switch (step) {
    case 0:
        encodedImageNo |= 0xA0000000U;
        break;
    case 1:
        encodedImageNo |= 0x90000000U;
        break;
    case 2:
        encodedImageNo |= 0xC0000000U;
        break;
    default:
        break;
    }

    const std::int32_t index = DrawGraphic(
        std::bit_cast<std::int32_t>(packedX),
        std::bit_cast<std::int32_t>(packedY),
        layer,
        std::bit_cast<std::int32_t>(encodedImageNo),
        0,
        nullptr);
    SetPendingGraphicBatchTypeAtLegacyIndex(index, std::uint8_t{2});
}

} // namespace cg::login::detail
