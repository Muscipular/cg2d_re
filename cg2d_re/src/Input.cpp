#include "cg/Input.hpp"
#include "cg/platform/LegacyTime.hpp"

#include "world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace cg {

void NullSub_509080() noexcept
{
}

} // namespace cg

namespace cg::input {

InputContext gInputContext626AA0 {};
InputContext gInputContexts5F9758[24] {};
InputContext* gCurrentInputData = nullptr;
std::int32_t gValueC9A96C = 0;
std::int32_t gValue626C0C = 0;
std::int32_t gValue626C20 = 0;
std::int32_t gValue626C28 = 0;

static_assert(sizeof(gInputContexts5F9758) == 0x20A0);

namespace {

inline constexpr std::int32_t kMaximumCaretOffset =
    static_cast<std::int32_t>(InputTextCapacity) - 2;
inline constexpr std::int32_t kMaximumTotalByteLength =
    static_cast<std::int32_t>(InputTextCapacity) - 1;

std::int8_t SignedByte(const char value)
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
}

} // namespace

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

namespace {

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

bool IsRejectedInputText_404060(const char* const legacyText)
{
    const std::size_t length = std::strlen(legacyText);
    if (std::strstr(legacyText, "\xA1\x40") != nullptr) {
        return true;
    }
    if (length == 0) {
        return true;
    }

    for (std::size_t index = 0; index < length; ++index) {
        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(legacyText[index]));
        if ((firstByte & 0x80U) != 0U) {
            const std::uint8_t secondByte = static_cast<std::uint8_t>(
                static_cast<unsigned char>(legacyText[index + 1]));
            const std::uint16_t legacyUnit = static_cast<std::uint16_t>(
                (static_cast<std::uint16_t>(firstByte) << 8U)
                | static_cast<std::uint16_t>(secondByte));
            if ((legacyUnit >= 0xA140U && legacyUnit <= 0xA1FEU)
                || (legacyUnit >= 0xA240U && legacyUnit <= 0xA2FEU)
                || (legacyUnit >= 0xA340U && legacyUnit <= 0xA3E0U)) {
                return true;
            }
            ++index;
            continue;
        }

        if (firstByte == static_cast<std::uint8_t>('|')) {
            return true;
        }
        if (firstByte == static_cast<std::uint8_t>('\\')) {
            return true;
        }
        if (firstByte == static_cast<std::uint8_t>('/')) {
            return true;
        }
        if (std::isspace(static_cast<unsigned char>(firstByte)) != 0) {
            return true;
        }
        if (std::iscntrl(static_cast<unsigned char>(firstByte)) != 0) {
            return true;
        }
    }

    return std::strstr(legacyText, "[GP]") != nullptr;
}

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

namespace cg::world::detail {

Record5FB7F8 gRecords5FB7F8[512] {};
volatile std::int32_t gWriteIndex626C08 = 0;
char gLogPath62699C[260] {};
std::FILE* volatile gLogStream626C1C = nullptr;
volatile std::int32_t gCountC9A944 = 0;
volatile std::uint32_t gValue626C10 = 0;

static_assert(sizeof(gRecords5FB7F8) == 512 * 0x134);

// 原 sub_4A76E0@[0x4A76E0,0x4A76FD)：先写未封顶值，超过 512 再写封顶值。
std::int32_t Sub_4A76E0()
{
    const std::int32_t increased = gCountC9A944 + 1;
    gCountC9A944 = increased;
    if (increased > 512) {
        gCountC9A944 = 512;
    }
    return increased;
}

// 原 sub_44B050@[0x44B050,0x44B170)。
void Sub_44B050(
    const char* const text,
    const std::int32_t color,
    const std::int32_t fontType)
{
    if (color >= 13) {
        return;
    }
    if (fontType >= 14) {
        return;
    }

    std::int32_t index = gWriteIndex626C08;
    char* const destination = gRecords5FB7F8[index].text;
    const std::size_t length = std::strlen(text) + 1;
    const std::size_t wordCount = length / sizeof(std::uint32_t);
    for (std::size_t word = 0; word < wordCount; ++word) {
        const std::size_t offset = word * sizeof(std::uint32_t);
        std::uint32_t copied;
        std::memcpy(&copied, text + offset, sizeof(copied));
        std::memcpy(destination + offset, &copied, sizeof(copied));
    }

    const std::FILE* const gate = gLogStream626C1C;
    for (std::size_t offset = wordCount * sizeof(std::uint32_t);
         offset < length; ++offset) {
        destination[offset] = text[offset];
    }

    if (gate != nullptr) {
        std::int32_t timeValue = color;
        platform::ReadLegacyTime32_53BA26(&timeValue);
        const platform::LegacyTm53B8C6* const legacyTime =
            platform::LocalTime_53B8C6(&timeValue);
        index = gWriteIndex626C08;
        const auto* const timeBytes =
            reinterpret_cast<const unsigned char*>(legacyTime);
        std::int32_t second;
        std::int32_t minute;
        std::int32_t hour;
        std::memcpy(&second, timeBytes, sizeof(second));
        std::memcpy(&minute, timeBytes + 4, sizeof(minute));
        std::memcpy(&hour, timeBytes + 8, sizeof(hour));

        std::fprintf(
            gLogStream626C1C,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, gRecords5FB7F8[index].text);
        std::fclose(gLogStream626C1C);
        std::FILE* const newStream = std::fopen(gLogPath62699C, "a");
        index = gWriteIndex626C08;
        gLogStream626C1C = newStream;
    }

    Record5FB7F8& record = gRecords5FB7F8[index];
    record.color = color;
    record.fontType = fontType;
    record.state = 0;
    const std::int32_t nextIndex = index + 1;
    gWriteIndex626C08 = nextIndex;
    if (nextIndex >= 512) {
        gWriteIndex626C08 = 0;
    }
    Sub_4A76E0();
    gValue626C10 = 20;
}

} // namespace cg::world::detail
