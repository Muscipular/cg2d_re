#include "cg/Input.hpp"

#include <array>
#include <bit>
#include <cstring>

namespace cg::input {

InputContext* gCurrentInputData = nullptr;
std::int32_t gValue626C0C = 0;
std::int32_t gValue626C20 = 0;
std::int32_t gValue626C28 = 0;

namespace {

inline constexpr std::int32_t kMaximumCaretOffset =
    static_cast<std::int32_t>(InputTextCapacity) - 2;
inline constexpr std::int32_t kMaximumTotalByteLength =
    static_cast<std::int32_t>(InputTextCapacity) - 1;

std::int8_t SignedByte(const char value)
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
}

// 原 sub_44B190@[0x44B190,0x44B1C5)。
std::int32_t ClassifyLastAnsiUnit_44B190(const char* text)
{
    std::int32_t result = 0;
    const char* current = text;
    while (*current != '\0') {
        if (SignedByte(*current) < 0) {
            if (current[1] == '\0') {
                return 3;
            }
            current += 2;
            result = 2;
        } else {
            ++current;
            result = 1;
        }
    }
    return result;
}

// 原 sub_44B1D0@[0x44B1D0,0x44B244)。
std::int32_t ClassifyAnsiBoundary_44B1D0(
    const char* source,
    const std::int32_t count)
{
    std::array<char, InputTextCapacity> copied {};
    std::strncpy(
        copied.data(),
        source,
        static_cast<std::size_t>(count));
    copied[static_cast<std::size_t>(count)] = '\0';

    std::int32_t result = 0;
    std::int32_t consumed = 0;
    const char* current = copied.data();
    while (consumed < count && *current != '\0') {
        if (SignedByte(*current) < 0) {
            if (current[1] == '\0') {
                return 3;
            }
            current += 2;
            result = 2;
            consumed += 2;
        } else {
            ++current;
            result = 1;
            ++consumed;
        }
    }
    return result;
}

bool HasSafeLegacyState(const InputContext& context)
{
    if (context.caretByteOffset < 0
        || context.caretByteOffset > kMaximumCaretOffset
        || context.totalByteLength < 0
        || context.totalByteLength > kMaximumTotalByteLength
        || context.caretByteOffset > context.totalByteLength) {
        return false;
    }

    const auto* const terminator = static_cast<const char*>(
        std::memchr(context.text, '\0', InputTextCapacity));
    if (terminator == nullptr) {
        return false;
    }

    const std::size_t visibleLength =
        static_cast<std::size_t>(terminator - context.text);
    if (visibleLength
        > static_cast<std::size_t>(context.totalByteLength)) {
        return false;
    }

    const std::size_t caret =
        static_cast<std::size_t>(context.caretByteOffset);
    const auto* const tailTerminator = static_cast<const char*>(
        std::memchr(
            context.text + caret,
            '\0',
            InputTextCapacity - caret));
    if (tailTerminator == nullptr) {
        return false;
    }

    const std::size_t tailTerminatorOffset =
        static_cast<std::size_t>(tailTerminator - context.text);
    return caret <= visibleLength
        || tailTerminatorOffset
            <= static_cast<std::size_t>(context.totalByteLength);
}

// 原 sub_44A160。
void LoadInitialText_44A160(const char* const initialText)
{
    if (gCurrentInputData == nullptr) {
        return;
    }

    std::size_t length = std::strlen(initialText);
    if (length >= InputTextCapacity) {
        length = InputTextCapacity - 1;
    }

    for (std::size_t index = 0; index < length; ++index) {
        AppendChar_44ADB0(initialText[index]);
    }
}

} // namespace

void AppendChar_44ADB0(const char character)
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr) {
        return;
    }

    const std::int8_t signedCharacter = SignedByte(character);
    if (signedCharacter >= 1
        && signedCharacter <= 0x1F
        && signedCharacter != '\r') {
        return;
    }
    if (!HasSafeLegacyState(*context)) {
        return;
    }

    std::array<char, InputTextCapacity + 1> tail {};
    std::array<char, InputTextCapacity> oldText {};
    std::strncpy(
        tail.data(),
        context->text + context->caretByteOffset,
        tail.size());
    std::strncpy(oldText.data(), context->text, oldText.size());

    const std::int32_t oldCaretByteOffset = context->caretByteOffset;
    const std::int32_t oldTotalByteLength = context->totalByteLength;
    context->text[context->caretByteOffset] = character;
    context->text[context->caretByteOffset + 1] = '\0';
    ++context->totalByteLength;
    ++context->caretByteOffset;

    const std::size_t insertedLength = std::strlen(context->text);
    std::strncat(
        context->text,
        tail.data(),
        InputTextCapacity - 1 - insertedLength);

    std::int32_t lineStart = 0;
    std::int32_t lineCount = 0;
    std::int32_t lineEndedWithCarriageReturn = 0;
    while (true) {
        std::int32_t scanned = 0;
        bool foundCarriageReturn = false;
        while (true) {
            const char current = context->text[lineStart + scanned];
            if (current == '\0') {
                break;
            }
            if (current == '\r') {
                lineStart += scanned + 1;
                lineEndedWithCarriageReturn = 1;
                ++lineCount;
                foundCarriageReturn = true;
                break;
            }
            ++scanned;
            if (scanned == context->maxCharPerLine) {
                break;
            }
        }

        if (!foundCarriageReturn) {
            if (std::strlen(context->text + lineStart)
                <= static_cast<std::size_t>(context->maxCharPerLine)) {
                break;
            }

            const std::int32_t boundary = ClassifyAnsiBoundary_44B1D0(
                context->text + lineStart,
                context->maxCharPerLine);
            if (boundary == 3) {
                lineStart += context->maxCharPerLine - 1;
            } else {
                lineStart += context->maxCharPerLine;
            }
            lineEndedWithCarriageReturn = 0;
            ++lineCount;
        }

        if (lineCount >= context->maxLine) {
            break;
        }
    }

    const std::int32_t overflowControl = gValue626C20;
    if (lineCount >= context->maxLine && overflowControl == 0) {
        const std::int32_t truncateOffset =
            lineStart - lineEndedWithCarriageReturn;
        context->totalByteLength -= static_cast<std::int32_t>(
            std::strlen(context->text + truncateOffset));
        context->text[truncateOffset] = '\0';

        if (ClassifyLastAnsiUnit_44B190(context->text) == 3) {
            --context->totalByteLength;
            context->text[context->totalByteLength] = '\0';
        }
        if (context->totalByteLength < context->caretByteOffset) {
            context->caretByteOffset = context->totalByteLength;
        }
    } else if (lineCount >= context->maxLine
               && overflowControl != 0
               && context->caretByteOffset
                   == context->totalByteLength - 1) {
        std::strncpy(context->text, oldText.data(), oldText.size());
        context->totalByteLength = oldTotalByteLength;
        context->caretByteOffset = oldCaretByteOffset;
        gValue626C28 = 1;
    }

    gValue626C0C = 20;
}

void SetContext_44B330(InputContext* const context)
{
    gCurrentInputData = context;
}

void InitializeContext_449CF0(
    InputContext* const context,
    const std::int32_t field130,
    const std::int32_t field134,
    const std::int32_t renderMode,
    const std::int32_t fontType,
    const std::int32_t color,
    const char* const initialText,
    const std::int32_t maxLine,
    const std::int32_t maxCharPerLine,
    const std::int32_t field14C,
    const std::int32_t encrypt)
{
    context->field130 = field130;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field140 = color;
    context->field134 = field134;
    context->maxCharPerLine = maxCharPerLine;
    context->field13C = fontType;
    context->encrypt = encrypt;
    context->maxLine = maxLine;
    context->field14C = field14C;
    context->text[0] = '\0';
    context->totalByteLength = 0;
    context->caretByteOffset = 0;
    context->field154 = 0;
    context->field158 = 0;

    if (initialText != nullptr) {
        InputContext* const savedContext = gCurrentInputData;
        SetContext_44B330(context);
        LoadInitialText_44A160(initialText);
        SetContext_44B330(savedContext);
    }
}

} // namespace cg::input
