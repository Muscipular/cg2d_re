#include "LoginMessageDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::login::detail {
namespace {

std::int32_t AddSigned16Wrapping(
    const std::int32_t value,
    const std::int16_t offset)
{
    const std::uint32_t result =
        std::bit_cast<std::uint32_t>(value)
        + static_cast<std::uint32_t>(static_cast<std::int32_t>(offset));
    return std::bit_cast<std::int32_t>(result);
}

} // namespace

std::int32_t DrawGraphic(
    const std::int32_t x,
    const std::int32_t y,
    const std::uint8_t layer,
    const std::int32_t imageNo,
    const std::int32_t value4,
    const GraphicDescriptor16* const descriptor)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount < 0 || initialCount >= 4096) {
        return -2;
    }

    std::int32_t finalImageNo = imageNo;
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (imageNo >= -1 && imageNo <= 999) {
        return -2;
    }
    if (imageNo > 999) {
        resource::GetGraphicMapNoIndex(imageNo, &finalImageNo);
        resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(finalImageNo),
            &offsetX,
            &offsetY);
    }

    const std::size_t queueIndex = static_cast<std::size_t>(initialCount);
    render::GraphicCache3& cache3 = render::gLegacyPendingStorage.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

    render::GraphicCacheItem32& item =
        render::gLegacyPendingStorage.records[queueIndex];
    item.x = AddSigned16Wrapping(x, offsetX);
    item.y = AddSigned16Wrapping(y, offsetY);
    item.imageNo = finalImageNo;
    item.value4 = value4;
    item.dataBits = 0;

    if (descriptor != nullptr) {
        item.descriptor = descriptor->bytes;
    } else {
        item.descriptor.fill(0);
        item.descriptor[8] = 0xFF;
        item.descriptor[9] = 0xFF;
        item.descriptor[10] = 0xFF;
        item.descriptor[11] = 0xFF;
    }

    item.batchType = 0;
    if (item.descriptor[12] == 1) {
        item.batchType = 1;
    }

    if ((finalImageNo >= 0xC18 && finalImageNo <= 0xC21)
        || (imageNo >= 0x1DF0 && imageNo <= 0x1DFF)) {
        render::gSpecialGraphicPending = 1;
    }

    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const std::int32_t result = static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1U);
    return result;
}

} // namespace cg::login::detail
