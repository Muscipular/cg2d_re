#include "cg/render/TextPendingQueue.hpp"

#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::render {
namespace {

// 三个原绘制入口均按W32前移；无符号入口在余W8阶段前读取链尾。
void CopyTextWords(
    char*& destination,
    const char*& source,
    const std::uint32_t byteCount) noexcept
{
    for (std::uint32_t index = 0; index < (byteCount >> 2U); ++index) {
        std::uint32_t word;
        std::memcpy(&word, source, sizeof(word));
        std::memcpy(destination, &word, sizeof(word));
        source += sizeof(word);
        destination += sizeof(word);
    }
}

void CopyTextRemainder(
    char* destination,
    const char* source,
    const std::uint32_t byteCount) noexcept
{
    for (std::uint32_t index = 0; index < (byteCount & 3U); ++index) {
        *destination = *source;
        ++source;
        ++destination;
    }
}

} // namespace

std::array<TextPendingRecord, MAX_QUEUE_SIZE> gTextPendingRecords {};
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
    if (initialCount >= MAX_QUEUE_SIZE) {
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
    record.reference.textToken = headToken;

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
    if (initialCount >= MAX_QUEUE_SIZE) {
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

    const std::uint32_t copyCount =
        static_cast<std::uint32_t>(std::strlen(text)) + 1U;
    const char* source = text;
    char* destination = entry.text;
    CopyTextWords(destination, source, copyCount);
    CopyTextRemainder(destination, source, copyCount);
    const std::size_t queueIndex =
        static_cast<std::size_t>(static_cast<std::int32_t>(renderMode));
    const std::uint32_t tailToken = gGraphicCache4Queue[queueIndex];
    entry.nextToken = 0;

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        // 原空链先读layer，再写尾槽，不改变QueueText弃返回行为。
        const std::uint8_t layer = gTextFontLayerRecords[queueIndex].layer;
        gGraphicCache4Queue[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(currentToken, layer));

        const std::uint32_t resultBits = platform::gPendingGraphicCount2;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        platform::gPendingGraphicCount2 = resultBits + 1U;
        return result;
    }

    gTextPendingRecords[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = platform::gPendingGraphicCount2;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
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
    if (initialCount >= MAX_QUEUE_SIZE) {
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

    const std::uint32_t copyCount =
        static_cast<std::uint32_t>(std::strlen(text)) + 1U;
    const char* source = text;
    char* destination = entry.text;
    CopyTextWords(destination, source, copyCount);
    const std::size_t queueIndex = static_cast<std::size_t>(renderMode);
    const std::uint32_t tailToken = gGraphicCache4Queue[queueIndex];
    CopyTextRemainder(destination, source, copyCount);
    entry.nextToken = 0;

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        // 原空链先读layer，再写尾槽，不改变QueueText弃返回行为。
        const std::uint8_t layer = gTextFontLayerRecords[queueIndex].layer;
        gGraphicCache4Queue[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(currentToken, layer));

        const std::uint32_t resultBits = platform::gPendingGraphicCount2;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        platform::gPendingGraphicCount2 = resultBits + 1U;
        return result;
    }

    gTextPendingRecords[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = platform::gPendingGraphicCount2;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    gGraphicCache4Queue[queueIndex] = resultBits + 1U;
    platform::gPendingGraphicCount2 = resultBits + 1U;
    return result;
}

} // namespace cg::render
