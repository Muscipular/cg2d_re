#include "cg/render/TextPendingQueue.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::render {
std::array<TextPendingRecord, MAX_QUEUE_SIZE> gTextPendingRecords {}; // 原全局：0x0062ECB8。
std::array<TextFontLayerRecord, 6> gTextFontLayerRecords {{ // 原全局：0x00558F50。
    {0xD2, {}},
    {0xD4, {}},
    {0xD8, {}},
    {0xDF, {}},
    {0xE2, {}},
    {0xE6, {}},
}};
std::int32_t gHoveredTextPendingIndex = 0; // 原全局：0x00D68BEC。

// 原函数：0x0045D7A0。
std::int32_t FindClickedTextIndex_45D7A0(
    const std::int32_t* const values,
    const std::int32_t count)
{
    const std::uint8_t edgeButtons = static_cast<std::uint8_t>(
        platform::BorrowLegacyMouseStateStorage()->edgeButtons);
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

// 原函数：0x00521A30。
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

// 原函数：0x00456560。
std::int32_t DrawText3_456560(
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t color,
    const std::wstring_view text,
    const std::int8_t value7)
{
    return DrawText2_456670(x, y, renderMode, 0, color, text, value7);
}

// 原函数：0x00456670。
std::int32_t DrawText2_456670(
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const std::wstring_view text,
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

    entry.text = text; // UTL02：记录持有完整UTF-16正文。
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

// 原函数：0x00456780。
std::int32_t DrawText_456780(
    const std::int16_t x,
    const std::int16_t y,
    const std::uint8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const std::wstring_view text,
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

    entry.text = text; // UTL02：记录持有完整UTF-16正文。
    const std::size_t queueIndex = static_cast<std::size_t>(renderMode);
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

} // namespace cg::render
