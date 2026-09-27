#include "LoginMessageDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace cg::render {
namespace {

struct GraphicModeFields {
    std::int16_t sourceX;
    std::int16_t sourceY;
    std::int16_t width;
    std::int16_t height;
    std::int32_t color;
    std::uint8_t clip;
    std::uint8_t mixMode;
    std::uint16_t paletteCacheIndex;
};

static_assert(std::is_standard_layout_v<GraphicModeFields>);
static_assert(std::is_trivially_copyable_v<GraphicModeFields>);
static_assert(sizeof(GraphicModeFields) == 0x10);
static_assert(offsetof(GraphicModeFields, sourceX) == 0x00);
static_assert(offsetof(GraphicModeFields, sourceY) == 0x02);
static_assert(offsetof(GraphicModeFields, width) == 0x04);
static_assert(offsetof(GraphicModeFields, height) == 0x06);
static_assert(offsetof(GraphicModeFields, color) == 0x08);
static_assert(offsetof(GraphicModeFields, clip) == 0x0C);
static_assert(offsetof(GraphicModeFields, mixMode) == 0x0D);
static_assert(offsetof(GraphicModeFields, paletteCacheIndex) == 0x0E);

std::int16_t SubtractSigned16Wrapping(
    const std::int16_t value,
    const std::int16_t offset)
{
    const std::uint16_t result = static_cast<std::uint16_t>(
        std::bit_cast<std::uint16_t>(value)
        - std::bit_cast<std::uint16_t>(offset));
    return std::bit_cast<std::int16_t>(result);
}

} // namespace

std::int32_t DrawGraphic3_520970(
    const std::int32_t x,
    const std::int32_t y,
    const std::uint8_t layer,
    const std::int32_t imageNo,
    const std::int32_t value4,
    const std::int32_t isGraphicNo,
    const std::uint8_t batchType,
    const GraphicDescriptor16* const descriptor)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount < 0 || initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    // 原函数复用 isGraphicNo 参数槽作为 map 输出；若底层不写出，保留入参位模式。
    std::int32_t graphicIndex = isGraphicNo;
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (imageNo < -1) {
        graphicIndex = imageNo;
    } else {
        if (imageNo <= 999) {
            return -2;
        }

        if (static_cast<std::uint8_t>(isGraphicNo) != 0) {
            resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        } else {
            graphicIndex = imageNo;
        }
        resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(graphicIndex),
            &offsetX,
            &offsetY);
    }

    const std::size_t queueIndex = static_cast<std::size_t>(initialCount);
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

    GraphicCacheItem32& item = gLegacyPendingStorage.records[queueIndex];
    item.x = x;
    item.y = y;
    item.imageNo = graphicIndex;
    item.value4 = value4;
    item.reference.action = nullptr;

    GraphicModeFields mode {};
    if (descriptor != nullptr) {
        std::memcpy(&mode, descriptor->bytes.data(), sizeof(mode));
    } else {
        mode.color = -1;
    }
    mode.sourceX = SubtractSigned16Wrapping(mode.sourceX, offsetX);
    mode.sourceY = SubtractSigned16Wrapping(mode.sourceY, offsetY);
    std::memcpy(item.descriptor.data(), &mode, sizeof(mode));

    item.batchType = batchType;
    if (mode.clip == 1) {
        item.batchType = 1;
    }

    if (graphicIndex >= 0xC18 && graphicIndex <= 0xC21) {
        gSpecialGraphicPending = 1;
    }

    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const std::int32_t result = static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1U);
    return result;
}

std::int32_t Sub_45F090(
    const std::int32_t x,
    const std::int32_t y,
    const std::uint8_t layer,
    const std::int32_t imageNo,
    const std::int32_t value4,
    const std::int32_t color)
{
    GraphicModeFields mode {};
    mode.color = color;
    mode.mixMode = 4;
    const GraphicDescriptor16 descriptor =
        std::bit_cast<GraphicDescriptor16>(mode);
    return DrawGraphic3_520970(
        x,
        y,
        layer,
        imageNo,
        value4,
        1,
        0,
        &descriptor);
}

std::int32_t Sub_521C80(
    const std::int32_t x, const std::int32_t y, const std::uint8_t layer,
    const std::int32_t imageNo, const std::int32_t value4,
    const GraphicDescriptor16* const descriptor)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    std::int32_t graphicIndex = imageNo;
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (imageNo > 999) {
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    }

    const auto queueIndex = static_cast<std::size_t>(initialCount);
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    GraphicCacheItem32& item = gLegacyPendingStorage.records[queueIndex];
    item.x = x + offsetX;
    item.y = y + offsetY;
    item.imageNo = graphicIndex;
    item.value4 = value4;
    item.reference.action = nullptr;
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
    if ((graphicIndex >= 0xC18 && graphicIndex <= 0xC21)
        || (imageNo >= 0x1DF0 && imageNo <= 0x1DFF)) {
        gSpecialGraphicPending = 1;
    }
    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const auto result = static_cast<std::int32_t>(std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1u);
    return result;
}

std::int32_t DrawGraphic2_521DD0(
    const std::int32_t x, const std::int32_t y, const std::uint8_t layer,
    const std::int32_t graphicIndex, const std::int32_t value4,
    const std::uint8_t batchType, const GraphicDescriptor16* const descriptor)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }
    if (graphicIndex >= -1 && graphicIndex <= 999) {
        return -2;
    }
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (graphicIndex > 999) {
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    }

    const auto queueIndex = static_cast<std::size_t>(initialCount);
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    GraphicCacheItem32& item = gLegacyPendingStorage.records[queueIndex];
    item.x = x + offsetX;
    item.y = y + offsetY;
    item.imageNo = graphicIndex;
    item.value4 = value4;
    item.reference.action = nullptr;
    if (descriptor != nullptr) {
        item.descriptor = descriptor->bytes;
    } else {
        item.descriptor.fill(0);
        item.descriptor[8] = 0xFF;
        item.descriptor[9] = 0xFF;
        item.descriptor[10] = 0xFF;
        item.descriptor[11] = 0xFF;
    }
    item.batchType = batchType;
    if (item.descriptor[12] == 1) {
        item.batchType = 1;
    }
    if ((graphicIndex >= 0xC18 && graphicIndex <= 0xC21)
        || (graphicIndex >= 0x1DF0 && graphicIndex <= 0x1DFF)) {
        gSpecialGraphicPending = 1;
    }
    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const auto result = static_cast<std::int32_t>(std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1u);
    return result;
}

} // namespace cg::render

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
    if (initialCount < 0 || initialCount >= MAX_QUEUE_SIZE) {
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
    item.reference.action = nullptr;

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
