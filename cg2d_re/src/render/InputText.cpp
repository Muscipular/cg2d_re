#include "cg/render/TextPendingQueue.hpp"

#include "cg/Input.hpp"
#include "cg/platform/LegacyTime.hpp"
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

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left + right;
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left - right;
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

// 原456BED/456C0B/456C2F分别读取完整W32模式，不能共用入口快照。
std::int32_t ReadRenderMode(const input::InputContext& context) noexcept
{
    std::int32_t mode;
    std::memcpy(&mode, context.field138, sizeof(mode));
    return mode;
}

} // namespace

// 原函数：0x004568A0。
void DrawInputText_4568A0(input::InputContext* const context)
{
    const std::uint32_t initialCountBits =
        platform::gPendingGraphicCount2;
    const std::int32_t initialCount =
        std::bit_cast<std::int32_t>(initialCountBits);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return;
    }

    std::int32_t caretLineStart = 0;
    std::int32_t caretLineYOffset = 0;

    if (context->maxLine >= 2) {
        std::array<char, 256> destination;
        std::int32_t lineStart = 0;
        std::int32_t nextStart = 0;
        std::int32_t yOffset = 0;
        std::int32_t lineCount = 0;
        bool caretLineKnown = false;

        for (;;) {
            const char* const lineSource = context->text + lineStart;
            std::uint32_t scan = 0;
            bool foundCarriageReturn = false;
            for (;;) {
                const char current = lineSource[scan];
                if (current == '\0') {
                    break;
                }
                if (current == '\r') {
                    platform::StrNCpy_53BEC0(
                        destination.data(), lineSource, scan);
                    destination[scan] = '\0';
                    nextStart = AddW32(
                        AddW32(lineStart, std::bit_cast<std::int32_t>(scan)),
                        1);
                    lineCount = AddW32(lineCount, 1);
                    foundCarriageReturn = true;
                    break;
                }

                const std::uint32_t scanBudget =
                    std::bit_cast<std::uint32_t>(context->maxCharPerLine);
                ++scan;
                if (scan == scanBudget) {
                    break;
                }
            }

            if (!foundCarriageReturn) {
                // 原45693B先取复制预算；复制后及分类后各自再取+148。
                const std::int32_t copyBudget = context->maxCharPerLine;
                const std::uint32_t remainingLength =
                    static_cast<std::uint32_t>(std::strlen(lineSource));
                if (remainingLength
                    < std::bit_cast<std::uint32_t>(copyBudget)) {
                    break;
                }

                platform::StrNCpy_53BEC0(
                    destination.data(),
                    lineSource,
                    std::bit_cast<std::uint32_t>(copyBudget));
                const std::int32_t terminateAt = context->maxCharPerLine;
                destination.data()[terminateAt] = '\0';
                const std::int32_t boundary =
                    input::ClassifyLastAnsiUnit_44B190(destination.data());
                const bool splitDoubleByte = boundary == 3;
                const std::int32_t advance = context->maxCharPerLine;
                if (splitDoubleByte) {
                    destination.data()[SubW32(advance, 1)] = '\0';
                    nextStart = AddW32(lineStart, SubW32(advance, 1));
                } else {
                    nextStart = AddW32(lineStart, advance);
                }
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

            // 原4569E9..456A19当前颜色/字体/模式/Y/X取样后才绘制。
            const std::int32_t color = context->field140;
            const std::int32_t fontType = context->field13C;
            const std::int8_t renderMode =
                std::bit_cast<std::int8_t>(context->field138[0]);
            const std::int32_t y = context->field134;
            const std::int32_t x = context->field130;
            static_cast<void>(DrawText2_456670(
                LowS16(x),
                LowS16(AddW32(y, yOffset)),
                renderMode,
                LowS8(fontType),
                LowS8(color),
                destination.data(),
                0));

            const std::int32_t lineHeight = context->field14C;
            lineStart = nextStart;
            yOffset = AddW32(yOffset, lineHeight);
        }

        if (!caretLineKnown) {
            caretLineStart = nextStart;
            caretLineYOffset = yOffset;
        }
        const std::int32_t color = context->field140;
        const std::int32_t fontType = context->field13C;
        const std::int8_t renderMode =
            std::bit_cast<std::int8_t>(context->field138[0]);
        const std::int32_t y = context->field134;
        const std::int32_t x = context->field130;
        static_cast<void>(DrawText2_456670(
            LowS16(x),
            LowS16(AddW32(y, yOffset)),
            renderMode,
            LowS8(fontType),
            LowS8(color),
            context->text + nextStart,
            0));
    } else {
        const std::size_t entryIndex = static_cast<std::size_t>(initialCount);
        TextPendingRecord& entry = gTextPendingRecords[entryIndex];
        entry.x = LowS16(context->field130);
        entry.y = LowS16(context->field134);
        entry.renderMode = context->field138[0];
        entry.color = std::bit_cast<std::uint8_t>(
            LowS8(context->field140));
        entry.fontType = std::bit_cast<std::uint8_t>(
            LowS8(context->field13C));
        entry.value7 = 0;

        if (context->encrypt >= 1) {
            std::int32_t totalBytes = context->totalByteLength;
            std::int32_t visibleStart = context->field158;
            std::int32_t index = 0;
            if (SubW32(totalBytes, visibleStart) > 0) {
                do {
                    entry.text[index] = '*';
                    totalBytes = context->totalByteLength;
                    visibleStart = context->field158;
                    index = AddW32(index, 1);
                } while (index < SubW32(totalBytes, visibleStart));
            }
            entry.text[index] = '\0';
        } else {
            const char* source = context->text + context->field158;
            const std::uint32_t copyCount =
                static_cast<std::uint32_t>(std::strlen(source)) + 1U;
            char* destination = entry.text;
            // 原456B46..456B50按W32前移，再复制含NUL的余W8。
            for (std::uint32_t index = 0; index < (copyCount >> 2U); ++index) {
                std::uint32_t word;
                std::memcpy(&word, source, sizeof(word));
                std::memcpy(destination, &word, sizeof(word));
                source += sizeof(word);
                destination += sizeof(word);
            }
            for (std::uint32_t index = 0; index < (copyCount & 3U); ++index) {
                *destination = *source;
                ++source;
                ++destination;
            }
        }

        std::uint32_t queueCountBits = initialCountBits;
        if (context->field154 != 0) {
            std::int32_t clippedLength = std::bit_cast<std::int32_t>(
                static_cast<std::uint32_t>(std::strlen(entry.text)));
            if (clippedLength > 0) {
                for (;;) {
                    const std::int32_t fontType = context->field13C;
                    const std::size_t measureIndex = static_cast<std::size_t>(
                        std::bit_cast<std::int32_t>(queueCountBits));
                    const std::int32_t measuredWidth = Font::measure(
                        gTextPendingRecords[measureIndex].text,
                        fontType,
                        clippedLength);
                    const std::int32_t widthLimit = context->field154;
                    if (measuredWidth <= widthLimit) {
                        queueCountBits = platform::gPendingGraphicCount2;
                        const std::size_t clippedEntryIndex =
                            static_cast<std::size_t>(
                                std::bit_cast<std::int32_t>(queueCountBits));
                        gTextPendingRecords[clippedEntryIndex].text[
                            clippedLength] = '\0';
                        break;
                    }

                    const std::int32_t sourceOffset = context->field158;
                    clippedLength = SubW32(clippedLength, 1);
                    if (input::ClassifyLastAnsiUnit_44B190(
                            context->text + sourceOffset) == 3) {
                        clippedLength = SubW32(clippedLength, 1);
                    }
                    queueCountBits = platform::gPendingGraphicCount2;
                    if (clippedLength <= 0) {
                        break;
                    }
                }
            }
        }

        const std::size_t queueEntryIndex = static_cast<std::size_t>(
            std::bit_cast<std::int32_t>(queueCountBits));
        TextPendingRecord& queueEntry = gTextPendingRecords[queueEntryIndex];
        queueEntry.nextToken = 0;

        const std::size_t modeIndex =
            static_cast<std::size_t>(ReadRenderMode(*context));
        const std::uint32_t tailToken = gGraphicCache4Queue[modeIndex];
        const std::uint32_t currentToken = queueCountBits + 1U;
        if (tailToken == 0) {
            gGraphicCache4Queue[modeIndex] = currentToken;
            const std::size_t layerModeIndex =
                static_cast<std::size_t>(ReadRenderMode(*context));
            static_cast<void>(QueueText_521A30(
                currentToken,
                gTextFontLayerRecords[layerModeIndex].layer));
        } else {
            gTextPendingRecords[
                static_cast<std::size_t>(tailToken - 1U)].nextToken =
                currentToken;

            const std::uint32_t tailCountBits =
                platform::gPendingGraphicCount2;
            const std::size_t tailModeIndex =
                static_cast<std::size_t>(ReadRenderMode(*context));
            gGraphicCache4Queue[tailModeIndex] = tailCountBits + 1U;
        }
        platform::gPendingGraphicCount2 =
            platform::gPendingGraphicCount2 + 1U;
    }

    if (input::gCurrentInputData != context) {
        return;
    }

    if (context->maxCharPerLine > 0 && context->maxLine >= 2) {
        const std::int32_t measureBytes = SubW32(
            context->caretByteOffset, caretLineStart);
        const std::int32_t fontType = context->field13C;
        const std::int32_t width = Font::measure(
            context->text + caretLineStart, fontType, measureBytes);
        const std::int32_t x = AddW32(context->field130, width);
        const std::int32_t y = context->field134;
        context->x = x;
        context->y = AddW32(y, caretLineYOffset);
        return;
    }

    const std::int32_t caret = context->caretByteOffset;
    const std::int32_t visibleStart = context->field158;
    std::int32_t count = SubW32(caret, visibleStart);
    if (count < 0) {
        context->field158 = caret;
        count = 0;
    }
    const std::int32_t fontType = context->field13C;
    const char* const source = context->text + context->field158;
    const std::int32_t width = Font::measure(source, fontType, count);
    const std::int32_t x = AddW32(context->field130, width);
    const std::int32_t y = context->field134;
    context->x = x;
    context->y = y;
}

} // namespace cg::render
