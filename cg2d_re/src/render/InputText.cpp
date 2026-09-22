#include "cg/render/TextPendingQueue.hpp"

#include "cg/Input.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::render {
namespace {

inline constexpr std::int32_t kMaximumInputOffset =
    static_cast<std::int32_t>(input::InputTextCapacity) - 1;
inline constexpr std::uint32_t kMaximumDestinationBytes = 255;
inline constexpr std::size_t kTextRecordCapacity = 255;

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        - std::bit_cast<std::uint32_t>(right));
}

std::int16_t LowS16(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value)));
}

std::int8_t LowS8(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(
        std::bit_cast<std::uint32_t>(value)));
}

bool IsInputOffset(const std::int32_t value) noexcept
{
    return value >= 0 && value <= kMaximumInputOffset;
}

bool IsPendingIndex(const std::uint32_t countBits) noexcept
{
    const std::int32_t count = std::bit_cast<std::int32_t>(countBits);
    return count >= 0 && count < MAX_QUEUE_SIZE;
}

bool CanMeasureInputText(
    const input::InputContext& context,
    const std::int32_t start,
    const std::int32_t fontType,
    const std::int32_t maxBytes) noexcept
{
    if (maxBytes <= 0
        || fontType < 0
        || fontType >= static_cast<std::int32_t>(kFontTypeCount)) {
        return true;
    }
    if (!IsInputOffset(start)) {
        return false;
    }

    std::size_t cursor = static_cast<std::size_t>(start);
    std::uint32_t consumed = 0;
    for (;;) {
        if (cursor >= input::InputTextCapacity) {
            return false;
        }

        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(context.text[cursor]));
        if (firstByte == 0) {
            return true;
        }
        if ((firstByte & 0x80U) != 0U) {
            if (cursor + 2U > input::InputTextCapacity) {
                return false;
            }
            cursor += 2U;
            consumed += 2U;
        } else {
            ++cursor;
            ++consumed;
        }

        if (std::bit_cast<std::int32_t>(consumed) >= maxBytes) {
            return true;
        }
    }
}

} // namespace

void DrawInputText_4568A0(input::InputContext* const context)
{
    const std::uint32_t initialCountBits =
        platform::gPendingGraphicCount2;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return;
    }
    if (initialCount < 0 || context == nullptr) {
        return;
    }

    std::int32_t caretLineStart = 0;
    std::int32_t caretLineYOffset = 0;

    if (context->maxLine >= 2) {
        std::int32_t renderMode = 0;
        std::memcpy(&renderMode, context->field138, sizeof(renderMode));

        const auto* const terminator = static_cast<const char*>(std::memchr(
            context->text,
            '\0',
            input::InputTextCapacity));
        if (terminator == nullptr) {
            return;
        }
        const std::size_t textLength =
            static_cast<std::size_t>(terminator - context->text);
        const std::uint32_t maxCharBits = std::bit_cast<std::uint32_t>(
            context->maxCharPerLine);

        std::array<char, 256> destination {};
        std::int32_t lineStart = 0;
        std::int32_t nextStart = 0;
        std::int32_t yOffset = 0;
        std::int32_t lineCount = 0;
        bool caretLineKnown = false;

        for (;;) {
            if (lineStart < 0
                || static_cast<std::size_t>(lineStart) > textLength) {
                return;
            }

            const std::size_t lineStartIndex =
                static_cast<std::size_t>(lineStart);
            std::uint32_t scan = 0;
            bool foundCarriageReturn = false;
            for (;;) {
                const std::size_t scanIndex = lineStartIndex
                    + static_cast<std::size_t>(scan);
                if (scanIndex > textLength) {
                    return;
                }
                const char current = context->text[scanIndex];
                if (current == '\0') {
                    break;
                }
                if (current == '\r') {
                    if (scan > kMaximumDestinationBytes) {
                        return;
                    }
                    std::memcpy(
                        destination.data(),
                        context->text + lineStartIndex,
                        static_cast<std::size_t>(scan));
                    destination[static_cast<std::size_t>(scan)] = '\0';
                    nextStart = AddW32(
                        AddW32(lineStart, static_cast<std::int32_t>(scan)),
                        1);
                    lineCount = AddW32(lineCount, 1);
                    foundCarriageReturn = true;
                    break;
                }

                ++scan;
                if (scan == maxCharBits) {
                    break;
                }
            }

            if (!foundCarriageReturn) {
                const std::size_t remainingLength =
                    textLength - lineStartIndex;
                if (static_cast<std::uint32_t>(remainingLength)
                    < maxCharBits) {
                    break;
                }
                if (maxCharBits > kMaximumDestinationBytes) {
                    return;
                }

                const std::size_t copyLength =
                    static_cast<std::size_t>(maxCharBits);
                std::memcpy(
                    destination.data(),
                    context->text + lineStartIndex,
                    copyLength);
                destination[copyLength] = '\0';

                std::int32_t advance = static_cast<std::int32_t>(
                    maxCharBits);
                if (input::ClassifyLastAnsiUnit_44B190(
                        destination.data()) == 3) {
                    if (copyLength == 0) {
                        return;
                    }
                    destination[copyLength - 1U] = '\0';
                    advance = SubW32(advance, 1);
                }
                if (advance <= 0) {
                    return;
                }

                nextStart = AddW32(lineStart, advance);
                lineCount = AddW32(lineCount, 1);
            }

            if (!caretLineKnown) {
                const bool ownsCaret = context->maxLine > lineCount
                    ? context->caretByteOffset < nextStart
                    : context->caretByteOffset <= nextStart;
                if (ownsCaret) {
                    caretLineStart = lineStart;
                    caretLineYOffset = yOffset;
                    caretLineKnown = true;
                }
            }

            static_cast<void>(DrawText2_456670(
                LowS16(context->field130),
                LowS16(AddW32(context->field134, yOffset)),
                LowS8(renderMode),
                LowS8(context->field13C),
                LowS8(context->field140),
                destination.data(),
                0));

            lineStart = nextStart;
            yOffset = AddW32(yOffset, context->field14C);
        }

        if (!caretLineKnown) {
            caretLineStart = nextStart;
            caretLineYOffset = yOffset;
        }
        static_cast<void>(DrawText2_456670(
            LowS16(context->field130),
            LowS16(AddW32(context->field134, yOffset)),
            LowS8(renderMode),
            LowS8(context->field13C),
            LowS8(context->field140),
            context->text + nextStart,
            0));
    } else {
        std::int32_t renderMode = 0;
        std::memcpy(&renderMode, context->field138, sizeof(renderMode));
        if (renderMode < 0
            || renderMode >= static_cast<std::int32_t>(
                gTextFontLayerRecords.size())) {
            return;
        }
        const std::size_t modeIndex = static_cast<std::size_t>(renderMode);

        std::size_t outputLength = 0;
        const char* originalSuffix = nullptr;
        bool originalSuffixValidated = false;
        if (context->encrypt >= 1) {
            const std::int32_t encryptedLength = SubW32(
                context->totalByteLength,
                context->field158);
            if (encryptedLength > 0) {
                if (encryptedLength
                    >= static_cast<std::int32_t>(kTextRecordCapacity)) {
                    return;
                }
                outputLength = static_cast<std::size_t>(encryptedLength);
            }
        } else {
            if (!IsInputOffset(context->field158)) {
                return;
            }
            const std::size_t visibleStart = static_cast<std::size_t>(
                context->field158);
            originalSuffix = context->text + visibleStart;
            const auto* const visibleTerminator = static_cast<const char*>(
                std::memchr(
                    originalSuffix,
                    '\0',
                    input::InputTextCapacity - visibleStart));
            if (visibleTerminator == nullptr) {
                return;
            }
            originalSuffixValidated = true;
            outputLength = static_cast<std::size_t>(
                visibleTerminator - originalSuffix);
        }
        if (outputLength >= kTextRecordCapacity) {
            return;
        }

        const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
        TextPendingRecord& entry = gTextPendingRecords[entryIndex];
        entry.x = LowS16(context->field130);
        entry.y = LowS16(context->field134);
        entry.renderMode = std::bit_cast<std::uint8_t>(LowS8(renderMode));
        entry.color = std::bit_cast<std::uint8_t>(
            LowS8(context->field140));
        entry.fontType = std::bit_cast<std::uint8_t>(
            LowS8(context->field13C));
        entry.value7 = 0;

        if (context->encrypt >= 1) {
            for (std::size_t index = 0; index < outputLength; ++index) {
                entry.text[index] = '*';
            }
            entry.text[outputLength] = '\0';
        } else {
            for (std::size_t index = 0; index <= outputLength; ++index) {
                entry.text[index] = originalSuffix[index];
            }
        }

        std::uint32_t queueCountBits = initialCountBits;
        if (context->field154 != 0 && outputLength > 0) {
            std::int32_t clippedLength = static_cast<std::int32_t>(
                outputLength);
            for (;;) {
                if (!IsPendingIndex(queueCountBits)) {
                    return;
                }
                const std::size_t measureIndex = static_cast<std::size_t>(
                    std::bit_cast<std::int32_t>(queueCountBits));
                if (Font::measure(
                        gTextPendingRecords[measureIndex].text,
                        context->field13C,
                        clippedLength) <= context->field154) {
                    queueCountBits = platform::gPendingGraphicCount2;
                    if (!IsPendingIndex(queueCountBits)) {
                        return;
                    }
                    const std::size_t clippedEntryIndex =
                        static_cast<std::size_t>(
                            std::bit_cast<std::int32_t>(queueCountBits));
                    gTextPendingRecords[clippedEntryIndex].text[
                        static_cast<std::size_t>(clippedLength)] = '\0';
                    break;
                }

                clippedLength = SubW32(clippedLength, 1);
                if (!originalSuffixValidated) {
                    if (!IsInputOffset(context->field158)) {
                        return;
                    }
                    const std::size_t visibleStart =
                        static_cast<std::size_t>(context->field158);
                    originalSuffix = context->text + visibleStart;
                    if (std::memchr(
                            originalSuffix,
                            '\0',
                            input::InputTextCapacity - visibleStart)
                        == nullptr) {
                        return;
                    }
                    originalSuffixValidated = true;
                }
                if (input::ClassifyLastAnsiUnit_44B190(
                        originalSuffix) == 3) {
                    clippedLength = SubW32(clippedLength, 1);
                }

                queueCountBits = platform::gPendingGraphicCount2;
                if (!IsPendingIndex(queueCountBits)) {
                    return;
                }
                if (clippedLength <= 0) {
                    break;
                }
            }
        }

        const std::size_t queueEntryIndex = static_cast<std::size_t>(
            std::bit_cast<std::int32_t>(queueCountBits));
        TextPendingRecord& queueEntry = gTextPendingRecords[queueEntryIndex];
        queueEntry.nextToken = 0;

        const std::uint32_t tailToken = gGraphicCache4Queue[modeIndex];
        if (tailToken > static_cast<std::uint32_t>(MAX_QUEUE_SIZE)) {
            return;
        }
        const std::uint32_t currentToken = queueCountBits + 1U;
        if (tailToken == 0) {
            gGraphicCache4Queue[modeIndex] = currentToken;
            static_cast<void>(QueueText_521A30(
                currentToken,
                gTextFontLayerRecords[modeIndex].layer));
        } else {
            gTextPendingRecords[
                static_cast<std::size_t>(tailToken - 1U)].nextToken =
                currentToken;

            const std::uint32_t tailCountBits =
                platform::gPendingGraphicCount2;
            if (!IsPendingIndex(tailCountBits)) {
                return;
            }
            gGraphicCache4Queue[modeIndex] = tailCountBits + 1U;
        }
        platform::gPendingGraphicCount2 =
            platform::gPendingGraphicCount2 + 1U;
    }

    if (input::gCurrentInputData != context) {
        return;
    }

    if (context->maxCharPerLine > 0 && context->maxLine >= 2) {
        const std::int32_t measureBytes = SubW32(
            context->caretByteOffset,
            caretLineStart);
        if (!IsInputOffset(caretLineStart)) {
            return;
        }
        std::int32_t width = 0;
        if (measureBytes > 0
            && context->field13C >= 0
            && context->field13C
                < static_cast<std::int32_t>(kFontTypeCount)) {
            if (!CanMeasureInputText(
                    *context,
                    caretLineStart,
                    context->field13C,
                    measureBytes)) {
                return;
            }
            width = Font::measure(
                context->text + caretLineStart,
                context->field13C,
                measureBytes);
        }
        context->x = AddW32(context->field130, width);
        context->y = AddW32(context->field134, caretLineYOffset);
        return;
    }

    std::int32_t visibleStart = context->field158;
    std::int32_t count = SubW32(
        context->caretByteOffset,
        visibleStart);
    if (count < 0) {
        visibleStart = context->caretByteOffset;
        context->field158 = visibleStart;
        count = 0;
    }
    std::int32_t width = 0;
    if (count > 0
        && context->field13C >= 0
        && context->field13C < static_cast<std::int32_t>(kFontTypeCount)) {
        if (!IsInputOffset(visibleStart)) {
            return;
        }
        if (!CanMeasureInputText(
                *context,
                visibleStart,
                context->field13C,
                count)) {
            return;
        }
        const std::size_t visibleStartIndex = static_cast<std::size_t>(
            visibleStart);
        width = Font::measure(
            context->text + visibleStartIndex,
            context->field13C,
            count);
    }
    context->x = AddW32(context->field130, width);
    context->y = context->field134;
}

} // namespace cg::render
