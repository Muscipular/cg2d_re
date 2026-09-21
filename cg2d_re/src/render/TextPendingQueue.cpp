#include "cg/render/TextPendingQueue.hpp"

#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::render {
namespace {

constexpr std::int32_t kPendingCapacity = 4096;
constexpr std::size_t kTextCapacity = 255;

} // namespace

std::array<TextPendingRecord, 4096> gTextPendingRecords {};
std::array<TextFontLayerRecord, 6> gTextFontLayerRecords {{
    {0xD2, {}},
    {0xD4, {}},
    {0xD8, {}},
    {0xDF, {}},
    {0xE2, {}},
    {0xE6, {}},
}};
std::int32_t gHoveredTextPendingIndex = 0;

std::int32_t FindClickedTextIndex_45D7A0(
    const std::int32_t* const values,
    const std::int32_t count)
{
    const std::uint8_t edgeButtons = static_cast<std::uint8_t>(
        platform::gLegacyMouseState.edgeButtons);
    if ((edgeButtons & 1U) == 0U) {
        return -1;
    }
    if (count <= 0) {
        return -1;
    }

    const std::int32_t hoveredTextIndex = gHoveredTextPendingIndex;
    for (std::int32_t index = 0; index < count; ++index) {
        if (values[index] == hoveredTextIndex) {
            return index;
        }
    }
    return -1;
}

std::int32_t QueueText_521A30(
    const std::uint32_t headToken,
    const std::uint8_t layer)
{
    const std::uint16_t initialCountBits = platform::gPendingGraphicCount;
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(initialCountBits);
    if (initialCount < 0 || initialCount >= kPendingCapacity) {
        return -2;
    }
    if (headToken == 0
        || headToken > static_cast<std::uint32_t>(kPendingCapacity)) {
        return -2;
    }

    const std::size_t queueIndex = static_cast<std::size_t>(initialCount);
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

    GraphicCacheItem32& record = gLegacyPendingStorage.records[queueIndex];
    record.imageNo = -1;
    record.value4 = 0;
    record.batchType = 4;
    record.dataBits = headToken;

    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const std::int32_t result = static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1U);
    return result;
}

std::int32_t DrawText3_456560(
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t color,
    const char* const text,
    const std::int8_t value7)
{
    return DrawText2_456670(x, y, renderMode, 0, color, text, value7);
}

std::int32_t DrawText2_456670(
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const char* const text,
    const std::int8_t value7)
{
    const std::uint32_t initialCountBits = platform::gPendingGraphicCount2;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount < 0 || initialCount >= kPendingCapacity) {
        return -2;
    }

    const std::int32_t modeIndex = static_cast<std::int32_t>(renderMode);
    if (modeIndex < 0 || modeIndex >= 6 || text == nullptr) {
        return -2;
    }

    std::size_t textLength = 0;
    while (textLength < kTextCapacity && text[textLength] != '\0') {
        ++textLength;
    }
    if (textLength == kTextCapacity) {
        return -2;
    }

    const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
    TextPendingRecord& entry = gTextPendingRecords[entryIndex];
    entry.x = x;
    entry.y = y;
    entry.renderMode = std::bit_cast<std::uint8_t>(renderMode);
    entry.color = std::bit_cast<std::uint8_t>(color);
    entry.fontType = std::bit_cast<std::uint8_t>(fontType);
    entry.value7 = std::bit_cast<std::uint8_t>(value7);
    entry.marker = 0xF9;
    for (std::size_t byteIndex = 0; byteIndex <= textLength; ++byteIndex) {
        entry.text[byteIndex] = text[byteIndex];
    }

    const std::size_t queueIndex = static_cast<std::size_t>(modeIndex);
    const std::uint32_t tailToken = gGraphicCache4Queue[queueIndex];
    entry.nextToken = 0;
    if (tailToken > static_cast<std::uint32_t>(kPendingCapacity)) {
        return -2;
    }

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        gGraphicCache4Queue[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(
            currentToken,
            gTextFontLayerRecords[queueIndex].layer));

        const std::uint32_t resultBits = platform::gPendingGraphicCount2;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        platform::gPendingGraphicCount2 = resultBits + 1U;
        return result;
    }

    gTextPendingRecords[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = platform::gPendingGraphicCount2;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    if (result < 0 || result >= kPendingCapacity) {
        return -2;
    }

    gGraphicCache4Queue[queueIndex] = resultBits + 1U;
    platform::gPendingGraphicCount2 = resultBits + 1U;
    return result;
}

std::int32_t DrawText_456780(
    const std::int16_t x,
    const std::int16_t y,
    const std::uint8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const char* const text,
    const std::int8_t value7,
    const std::int8_t marker)
{
    const std::uint32_t initialCountBits = platform::gPendingGraphicCount2;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount < 0 || initialCount >= kPendingCapacity) {
        return -2;
    }

    if (renderMode >= 6U || text == nullptr) {
        return -2;
    }

    std::size_t textLength = 0;
    while (textLength < kTextCapacity && text[textLength] != '\0') {
        ++textLength;
    }
    if (textLength == kTextCapacity) {
        return -2;
    }

    const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
    TextPendingRecord& entry = gTextPendingRecords[entryIndex];
    entry.x = x;
    entry.y = y;
    entry.renderMode = renderMode;
    entry.color = std::bit_cast<std::uint8_t>(color);
    entry.fontType = std::bit_cast<std::uint8_t>(fontType);
    entry.value7 = std::bit_cast<std::uint8_t>(value7);
    entry.marker = std::bit_cast<std::uint8_t>(marker);
    for (std::size_t byteIndex = 0; byteIndex <= textLength; ++byteIndex) {
        entry.text[byteIndex] = text[byteIndex];
    }

    const std::size_t queueIndex = static_cast<std::size_t>(renderMode);
    const std::uint32_t tailToken = gGraphicCache4Queue[queueIndex];
    entry.nextToken = 0;
    if (tailToken > static_cast<std::uint32_t>(kPendingCapacity)) {
        return -2;
    }

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        gGraphicCache4Queue[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(
            currentToken,
            gTextFontLayerRecords[queueIndex].layer));

        const std::uint32_t resultBits = platform::gPendingGraphicCount2;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        platform::gPendingGraphicCount2 = resultBits + 1U;
        return result;
    }

    gTextPendingRecords[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = platform::gPendingGraphicCount2;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    if (result < 0 || result >= kPendingCapacity) {
        return -2;
    }

    gGraphicCache4Queue[queueIndex] = resultBits + 1U;
    platform::gPendingGraphicCount2 = resultBits + 1U;
    return result;
}

} // namespace cg::render
