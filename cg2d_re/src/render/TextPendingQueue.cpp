#include "cg/session/ClientSession.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/Font.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::render {
std::array<TextFontLayerRecord, 6> gTextFontLayerRecords {{ // 原全局：0x00558F50。
    {0xD2, {}},
    {0xD4, {}},
    {0xD8, {}},
    {0xDF, {}},
    {0xE2, {}},
    {0xE6, {}},
}};

// 原函数：0x0045D7A0。
std::int32_t FindClickedTextIndex_45D7A0(
    session::ClientSession& session,
    const std::int32_t* const values,
    const std::int32_t count)
{
    const std::uint8_t edgeButtons = static_cast<std::uint8_t>(
        platform::ReadSessionMouseEdgeButtons(session));
    if ((edgeButtons & 1U) == 0U) {
        return -1;
    }
    if (count <= 0) {
        return -1;
    }

    const std::int32_t hoveredTextIndex = session.UiFrame().hoveredTextPendingIndex;
    for (std::int32_t index = 0; index < count; ++index) {
        if (values[index] == hoveredTextIndex) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x00521A30。
std::int32_t QueueText_521A30(
    session::ClientSession& session,
    const std::uint32_t headToken,
    const std::uint8_t layer)
{
    const std::uint16_t initialCountBits = session.GraphicQueue().count;
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(initialCountBits);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    const std::size_t queueIndex = static_cast<std::size_t>(initialCount);
    GraphicCache3& cache3 = session.GraphicQueue().pending.cache3[queueIndex];
    cache3.layer = layer;
    cache3.cacheIndex =
        std::bit_cast<std::int16_t>(session.GraphicQueue().count);

    GraphicCacheItem32& record = session.GraphicQueue().pending.records[queueIndex];
    record.imageNo = -1;
    record.value4 = 0;
    record.batchType = 4;
    record.reference.textToken = headToken;

    const std::uint16_t resultBits = session.GraphicQueue().count;
    const std::int32_t result = static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(resultBits));
    session.GraphicQueue().count = static_cast<std::uint16_t>(resultBits + 1U);
    return result;
}

// 原函数：0x00456560。
std::int32_t DrawText3_456560(
    session::ClientSession& session,
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t color,
    const std::wstring_view text,
    const std::int8_t value7)
{
    return DrawText2_456670(session, x, y, renderMode, 0, color, text, value7);
}

// 原函数：0x00456670。
std::int32_t DrawText2_456670(
    session::ClientSession& session,
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const std::wstring_view text,
    const std::int8_t value7,
    const std::array<std::int32_t, 4>* const clipRectangle)
{
    const std::uint32_t initialCountBits = session.TextQueue().count;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
    TextPendingRecord& entry = session.TextQueue().records[entryIndex];
    entry.x = x;
    entry.y = y;
    entry.renderMode = std::bit_cast<std::uint8_t>(renderMode);
    entry.color = std::bit_cast<std::uint8_t>(color);
    entry.fontType = static_cast<std::uint8_t>(ResolveFontId(fontType));
    entry.value7 = std::bit_cast<std::uint8_t>(value7);
    entry.marker = 0xF9;
    entry.clipRectangle.reset(); // 队列槽跨帧复用，不继承上一条文字的裁剪。
    if (clipRectangle) entry.clipRectangle = *clipRectangle;

    entry.text = text; // UTL02：记录持有完整UTF-16正文。
    const std::size_t queueIndex =
        static_cast<std::size_t>(static_cast<std::int32_t>(renderMode));
    const std::uint32_t tailToken = session.TextQueue().tails[queueIndex];
    entry.nextToken = 0;

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        // 原空链先读layer，再写尾槽，不改变QueueText弃返回行为。
        const std::uint8_t layer = gTextFontLayerRecords[queueIndex].layer;
        session.TextQueue().tails[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(session, currentToken, layer));

        const std::uint32_t resultBits = session.TextQueue().count;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        session.TextQueue().count = resultBits + 1U;
        return result;
    }

    session.TextQueue().records[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = session.TextQueue().count;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    session.TextQueue().tails[queueIndex] = resultBits + 1U;
    session.TextQueue().count = resultBits + 1U;
    return result;
}

// 原函数：0x00456780。
std::int32_t DrawText_456780(
    session::ClientSession& session,
    const std::int16_t x,
    const std::int16_t y,
    const std::uint8_t renderMode,
    const std::int8_t fontType,
    const std::int8_t color,
    const std::wstring_view text,
    const std::int8_t value7,
    const std::int8_t marker)
{
    const std::uint32_t initialCountBits = session.TextQueue().count;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
    TextPendingRecord& entry = session.TextQueue().records[entryIndex];
    entry.x = x;
    entry.y = y;
    entry.renderMode = renderMode;
    entry.color = std::bit_cast<std::uint8_t>(color);
    entry.fontType = static_cast<std::uint8_t>(ResolveFontId(fontType));
    entry.value7 = std::bit_cast<std::uint8_t>(value7);
    entry.marker = std::bit_cast<std::uint8_t>(marker);
    entry.clipRectangle.reset();

    entry.text = text; // UTL02：记录持有完整UTF-16正文。
    const std::size_t queueIndex = static_cast<std::size_t>(renderMode);
    const std::uint32_t tailToken = session.TextQueue().tails[queueIndex];
    entry.nextToken = 0;

    const std::uint32_t currentToken = initialCountBits + 1U;
    if (tailToken == 0) {
        // 原空链先读layer，再写尾槽，不改变QueueText弃返回行为。
        const std::uint8_t layer = gTextFontLayerRecords[queueIndex].layer;
        session.TextQueue().tails[queueIndex] = currentToken;
        static_cast<void>(QueueText_521A30(session, currentToken, layer));

        const std::uint32_t resultBits = session.TextQueue().count;
        const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
        session.TextQueue().count = resultBits + 1U;
        return result;
    }

    session.TextQueue().records[static_cast<std::size_t>(tailToken - 1U)].nextToken =
        currentToken;

    const std::uint32_t resultBits = session.TextQueue().count;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    session.TextQueue().tails[queueIndex] = resultBits + 1U;
    session.TextQueue().count = resultBits + 1U;
    return result;
}

// 绘制和度量使用相同的可见分段，避免色码/转义美元符占用对齐宽度。
template <typename Submit>
static void ForEachColoredTextSegment(
    const std::wstring_view text, const std::int8_t color, const Submit& submit)
{
    std::int8_t currentColor = color;
    std::wstring segment;
    const auto submitSegment = [&] {
        if (segment.empty()) { return; }
        submit(std::wstring_view(segment), currentColor);
        segment.clear();
    };
    for (std::size_t offset = 0; offset < text.size();) {
        if (text[offset] == L'$' && offset + 1 < text.size()) {
            const wchar_t code = text[offset + 1];
            if (code == L'$') {
                segment.push_back(L'$');
                offset += 2;
                continue;
            }
            const std::int32_t colorIndex = cg::text::InlineColorIndex(code);
            if (colorIndex >= 0) {
                submitSegment();
                currentColor = static_cast<std::int8_t>(colorIndex);
                offset += 2;
                continue;
            }
        }
        const std::size_t next = cg::text::Next(text, offset);
        segment.append(text.substr(offset, next - offset));
        offset = next;
    }
    submitSegment();
}

std::int32_t MeasureColoredText(const std::wstring_view text, const std::int8_t fontType)
{
    std::int32_t width = 0;
    ForEachColoredTextSegment(text, 0,
        [&](const std::wstring_view segment, const std::int8_t) {
            width += TextMeasure(segment, fontType);
        });
    return width;
}

void DrawColoredText(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y, const std::uint8_t renderMode,
    const std::int8_t fontType, const std::int8_t color, const std::wstring_view text,
    const std::int8_t value7, const std::int8_t marker,
    const std::array<std::int32_t, 4>* const clipRectangle)
{
    std::int32_t currentX = x;
    ForEachColoredTextSegment(text, color,
        [&](const std::wstring_view segment, const std::int8_t segmentColor) {
            const auto index = DrawText_456780(session, static_cast<std::int16_t>(currentX), y,
                renderMode, fontType, segmentColor, segment, value7, marker);
            if (index >= 0 && clipRectangle) {
                session.TextQueue().records[static_cast<std::size_t>(index)].clipRectangle = *clipRectangle;
            }
            currentX += TextMeasure(segment, fontType);
        });
}

} // namespace cg::render
