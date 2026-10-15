#include "scene/login/LoginMessageDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::render {
namespace {

// 原两入口的四W32读写/空参W16-W32-W8默认发布，保正常源目标别名。
void CopyGraphicDescriptorWords(
    std::array<std::uint8_t, 16>& output, const GraphicDescriptor16* const descriptor)
{
    if (descriptor != nullptr) {
        for (std::int32_t word = 0; word < 4; ++word) {
            std::uint32_t value;
            std::memcpy(&value, descriptor->bytes.data() + word * 4, sizeof(value));
            std::memcpy(output.data() + word * 4, &value, sizeof(value));
        }
    } else {
        const std::uint16_t zero = 0;
        for (std::int32_t word = 0; word < 4; ++word) {
            std::memcpy(output.data() + word * 2, &zero, sizeof(zero));
        }
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(output.data() + 8, &color, sizeof(color));
        output[12] = 0;
        output[13] = 0;
        std::memcpy(output.data() + 14, &zero, sizeof(zero));
    }
}

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

// 原函数：0x00520970。
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
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    GraphicCacheItem32* const item =
        gLegacyPendingStorage.records.data() + initialCount;
    // 原复用 isGraphicNo 参数槽；底层未写出时保留入参位模式。
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
            std::bit_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    }

    GraphicCache3* const cache3 =
        gLegacyPendingStorage.cache3.data() + initialCount;
    cache3->layer = layer;
    const std::int16_t cacheIndex =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    const std::int32_t itemY = y;
    cache3->cacheIndex = cacheIndex;
    item->x = x;
    const std::int32_t itemValue4 = value4;
    item->y = itemY;
    item->imageNo = graphicIndex;
    const bool hasDescriptor = descriptor != nullptr;
    item->value4 = itemValue4;
    item->reference.action = nullptr;
    CopyGraphicDescriptorWords(item->descriptor, hasDescriptor ? descriptor : nullptr);

    std::int16_t sourceX;
    std::memcpy(&sourceX, item->descriptor.data(), sizeof(sourceX));
    sourceX = SubtractSigned16Wrapping(sourceX, offsetX);
    std::memcpy(item->descriptor.data(), &sourceX, sizeof(sourceX));
    std::int16_t sourceY;
    std::memcpy(&sourceY, item->descriptor.data() + 2, sizeof(sourceY));
    sourceY = SubtractSigned16Wrapping(sourceY, offsetY);
    std::memcpy(item->descriptor.data() + 2, &sourceY, sizeof(sourceY));

    const std::uint8_t clip = item->descriptor[12];
    item->batchType = batchType;
    if (clip == 1) {
        item->batchType = 1;
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

// 原函数：0x0045F090。
std::int32_t Sub_45F090(
    const std::int32_t x,
    const std::int32_t y,
    const std::uint8_t layer,
    const std::int32_t imageNo,
    const std::int32_t value4,
    const std::int32_t color)
{
    GraphicDescriptor16 descriptor;
    const std::uint32_t zero32 = 0;
    const std::int32_t itemColor = color;
    std::memcpy(descriptor.bytes.data() + 2, &zero32, sizeof(zero32));
    const GraphicDescriptor16* const descriptorAddress = &descriptor;
    std::memcpy(descriptor.bytes.data() + 6, &zero32, sizeof(zero32));
    const std::uint8_t itemLayer = layer;
    std::memcpy(descriptor.bytes.data() + 10, &zero32, sizeof(zero32));

    const std::uint16_t zero16 = 0;
    std::memcpy(descriptor.bytes.data() + 14, &zero16, sizeof(zero16));
    std::memcpy(descriptor.bytes.data(), &zero16, sizeof(zero16));
    std::memcpy(descriptor.bytes.data() + 2, &zero16, sizeof(zero16));
    std::memcpy(descriptor.bytes.data() + 4, &zero16, sizeof(zero16));
    std::memcpy(descriptor.bytes.data() + 6, &zero16, sizeof(zero16));
    descriptor.bytes[12] = 0;
    std::memcpy(descriptor.bytes.data() + 14, &zero16, sizeof(zero16));

    const std::int32_t itemValue4 = value4;
    std::memcpy(descriptor.bytes.data() + 8, &itemColor, sizeof(itemColor));
    const std::int32_t itemImageNo = imageNo;
    const std::int32_t itemY = y;
    const std::int32_t itemX = x;
    descriptor.bytes[13] = 4;
    return DrawGraphic3_520970(
        itemX, itemY, itemLayer, itemImageNo, itemValue4, 1, 0, descriptorAddress);
}

// 原函数：0x00521C80。
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

    GraphicCacheItem32* const item =
        gLegacyPendingStorage.records.data() + initialCount;
    std::int32_t graphicIndex = imageNo;
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (imageNo > 999) {
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    }

    GraphicCache3* const cache3 =
        gLegacyPendingStorage.cache3.data() + initialCount;
    cache3->layer = layer;
    cache3->cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    item->x = x + offsetX;
    const std::int32_t adjustedY = y + offsetY;
    const std::int32_t itemValue4 = value4;
    item->y = adjustedY;
    const bool hasDescriptor = descriptor != nullptr;
    item->imageNo = graphicIndex;
    item->value4 = itemValue4;
    item->reference.action = nullptr;
    CopyGraphicDescriptorWords(item->descriptor, hasDescriptor ? descriptor : nullptr);

    const std::uint8_t clip = item->descriptor[12];
    item->batchType = 0;
    if (clip == 1) {
        item->batchType = 1;
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

// 原函数：0x00521DD0。
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

    GraphicCacheItem32* const item =
        gLegacyPendingStorage.records.data() + initialCount;
    if (graphicIndex >= -1 && graphicIndex <= 999) {
        return -2;
    }
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (graphicIndex > 999) {
        resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    }

    GraphicCache3* const cache3 =
        gLegacyPendingStorage.cache3.data() + initialCount;
    cache3->layer = layer;
    cache3->cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    const std::int32_t itemY = y;
    item->x = x + offsetX;
    const GraphicDescriptor16* const descriptorAddress = descriptor;
    const std::int32_t adjustedY = itemY + offsetY;
    const std::int32_t itemValue4 = value4;
    const bool hasDescriptor = descriptorAddress != nullptr;
    item->y = adjustedY;
    item->imageNo = graphicIndex;
    item->value4 = itemValue4;
    item->reference.action = nullptr;
    CopyGraphicDescriptorWords(item->descriptor, hasDescriptor ? descriptorAddress : nullptr);

    item->batchType = batchType;
    if (item->descriptor[12] == 1) {
        item->batchType = 1;
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
// 原函数：0x00521B10。
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
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    render::GraphicCacheItem32* const item =
        render::gLegacyPendingStorage.records.data() + initialCount;
    std::int32_t finalImageNo = imageNo;
    std::int16_t offsetX = 0;
    std::int16_t offsetY = 0;
    if (imageNo >= -1 && imageNo <= 999) {
        return -2;
    }
    if (imageNo > 999) {
        resource::GetGraphicMapNoIndex(imageNo, &finalImageNo);
        resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(finalImageNo), &offsetX, &offsetY);
    }

    render::GraphicCache3* const cache3 =
        render::gLegacyPendingStorage.cache3.data() + initialCount;
    cache3->layer = layer;
    cache3->cacheIndex =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

    item->x = x + offsetX;
    const std::int32_t adjustedY = y + offsetY;
    const std::int32_t itemValue4 = value4;
    item->y = adjustedY;
    const bool hasDescriptor = descriptor != nullptr;
    item->imageNo = finalImageNo;
    item->value4 = itemValue4;
    item->reference.action = nullptr;
    render::CopyGraphicDescriptorWords(item->descriptor, hasDescriptor ? descriptor : nullptr);

    const std::uint8_t clip = item->descriptor[12];
    item->batchType = 0;
    if (clip == 1) {
        item->batchType = 1;
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
