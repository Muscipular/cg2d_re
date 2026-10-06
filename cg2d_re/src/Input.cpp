#include "cg/Input.hpp"
#include "cg/Blacklist.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include "InputKeyDispatchDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"
#include "platform/LegacyMultibyte.hpp"
#include "platform/WindowProcedureState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace cg {

void NullSub_509080() noexcept
{
}

} // namespace cg

namespace cg::input {

InputContext gInputContext626AA0 {};
InputContext gInputContexts5F9758[24] {};
InputContext gInputContextD66CA8 {};
InputContext gInputContextCB03F0 {};
InputContext gInputContextCC46E0 {};
InputContext gInputContextCC2080 {};
InputContext gInputContextD2D828 {};
InputContext gInputContextCA0180 {};
InputContext gInputContextCB0610 {};
InputContext gInputContextCB0028 {};
InputContext gInputContextCC2378 {};
InputContext gInputContextCF4E60 {};
InputContext gInputContextD2DE70 {};
InputContext gInputContextD2DFD0 {};
InputContext gInputContextD2DC10 {};
InputContext gInputContextD2D4A0 {};
InputContext gInputContextD64CB0 {};
InputContext gInputContextD2D670 {};
InputContext gInputContextCAFEC8 {};
InputContext gInputContextC8B1B8 {};
InputContext gInputContextCC21E8 {};
InputContext gInputContextCB0878 {};
InputContext gInputContextCA0018 {};
InputContext gInputContextCC49C0 {};
InputContext gInputContextCA02E0 {};
InputContext gInputContextD2E130 {};
InputContext gInputContextD2E2E0 {};
InputContext gInputContextCB01F8 {};
InputContext gInputContextCF47E0 {};
InputContext gInputContext1050B80 {};
InputContext gInputContext1051348 {};
InputContext gInputContexts1050F18[3] {};
InputContext* gCurrentInputData = nullptr;
std::int32_t gValueC9A96C = 0;
std::int32_t gValue626C0C = 0;
std::int32_t gValue626C14 = 0;
std::int32_t gValue626C20 = 0;
std::int32_t gValue626C28 = 0;

static_assert(sizeof(gInputContexts5F9758) == 0x20A0);

namespace {

struct LegacyInput593850 {
    char prefix[8];
    InputContext context;
};

LegacyInput593850 gLegacyInput593850 {};
static_assert(offsetof(LegacyInput593850, context) == 8);
static_assert(sizeof(LegacyInput593850) == 0x164);

constexpr std::array<std::uint8_t, 8> kEscapePairs58E944 {{
    '\n', 'n', ',', 'c', '|', 'z', '\\', 'y',
}};

constexpr char kBlacklistSelfText554938[] =
    "\xB1\x7A\xA1\x4B\xA6\x70\xA6\xB9\xB9\xBD\xB4\x63\xA6\xDB"
    "\xA4\x76\xAA\xBA\xC1\x6E\xAD\xB5\xB6\xDC\xA1\x48";
constexpr char kBlacklistExistingFormat554928[] =
    "%s\xA4\x77\xA6\x62\xB6\xC2\xA6\x57\xB3\xE6\xA4\xA4";
constexpr char kBlacklistAddedFormat5532C4[] =
    "\xA4\x77\xB1\x4E%s\xA5\x5B\xA4\x4A\xB6\xC2\xA6\x57\xB3\xE6";
constexpr char kBlacklistFailedFormat5532B0[] =
    "%s\xA5\x5B\xA4\x4A\xB6\xC2\xA6\x57\xB3\xE6\xA5\xA2\xB1\xD1";
static_assert(sizeof(kBlacklistSelfText554938) == 27);
static_assert(sizeof(kBlacklistExistingFormat554928) == 15);
static_assert(sizeof(kBlacklistAddedFormat5532C4) == 17);
static_assert(sizeof(kBlacklistFailedFormat5532B0) == 17);

std::int8_t SignedByte(const char value)
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
}

} // namespace

const char* LegacyText593850() noexcept
{
    // 原端文本读取可从前缀延续到紧邻的上下文。
    return reinterpret_cast<const char*>(&gLegacyInput593850);
}

InputContext* BorrowLegacyBlacklistContext593858() noexcept
{
    return &gLegacyInput593850.context;
}

InputContext* Sub_45D720(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    [[maybe_unused]] const std::int32_t unused,
    const std::int32_t color, const std::int32_t renderMode)
{
    context->field154 = 0;
    context->field158 = 0;
    context->field134 = y;
    context->field130 = x;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field140 = color;
    return context;
}

InputContext* Sub_45D760(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    const std::int32_t color, const std::int32_t renderMode,
    const std::int32_t fontType)
{
    context->field130 = x;
    context->field134 = y;
    context->field140 = color;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field13C = fontType;
    return context;
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

// 原44B570固定扫描40字节；与通常的双字节分类规则不同。
std::int32_t ClassifyChatSplit_44B570(const char* const text, const std::int32_t)
{
    std::int32_t offset = 0;
    std::int32_t lastPair = 0;
    do {
        if (SignedByte(text[offset]) <= 0) {
            lastPair = offset;
            offset += 2;
        } else {
            ++offset;
        }
    } while (offset < 40);
    return lastPair > 38 ? 0 : 2;
}

// 原 [0x44B440,0x44B49D)：仅 0x81..0x9F 与 0xE0..0xFC 为双字节首字节。
std::uint8_t ClassifyLegacyBoundary_44B440(
    const char* const text,
    const std::int16_t byteOffset)
{
    std::int32_t cursor = 0;
    while (cursor < byteOffset) {
        const auto current =
            static_cast<std::uint8_t>(text[cursor]);
        if ((current >= 0x81u && current <= 0x9Fu)
            || (current >= 0xE0u && current <= 0xFCu)) {
            cursor += 2;
        } else {
            ++cursor;
        }
    }

    if (cursor != byteOffset) {
        return 2;
    }
    const auto current =
        static_cast<std::uint8_t>(text[cursor]);
    return static_cast<std::uint8_t>(
        (current >= 0x81u && current <= 0x9Fu)
        || (current >= 0xE0u && current <= 0xFCu));
}

// 原 [0x44B4A0,0x44B561)：目标首字节经 movsx 后，双字节目标分支对 W8 输入不可达。
char* FindLegacyByte_44B4A0(char* const text, const char* const target)
{
    const std::int32_t wanted = SignedByte(target[0]);
    bool trailingByte = false;
    for (char* current = text; *current != '\0'; ++current) {
        if (trailingByte) {
            trailingByte = false;
            continue;
        }

        const auto byte = static_cast<std::uint8_t>(*current);
        if ((byte >= 0x81u && byte <= 0x9Fu)
            || (byte >= 0xE0u && byte <= 0xFCu)) {
            trailingByte = true;
            continue;
        }
        if (static_cast<std::int32_t>(byte) == wanted) {
            return current;
        }
    }
    return nullptr;
}

namespace {

// 原 sub_44B1D0@[0x44B1D0,0x44B244)。
std::int32_t ClassifyAnsiBoundary_44B1D0(
    const char* source,
    const std::int32_t count)
{
    std::array<char, InputTextCapacity> copied;
    platform::StrNCpy_53BEC0(
        copied.data(),
        source,
        static_cast<std::uint32_t>(count));
    copied.data()[count] = '\0';

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

} // namespace

// 原 sub_44A160。
void LoadInitialText_44A160(const char* const initialText)
{
    if (gCurrentInputData == nullptr) {
        return;
    }

    // 原44A17E/44A18D/44A19E均按S32解释长度及循环门禁。
    std::int32_t length = static_cast<std::int32_t>(std::strlen(initialText));
    if (length >= static_cast<std::int32_t>(InputTextCapacity)) {
        length = static_cast<std::int32_t>(InputTextCapacity - 1);
    }

    for (std::int32_t index = 0; index < length; ++index) {
        AppendChar_44ADB0(initialText[index]);
    }
}

namespace detail {

// 原 [0x44A5D0,0x44A7CD)：按 CR 和最大字节宽度扫描行后上移光标。
void Sub_44A5D0()
{
    InputContext* context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return;
    }

    gValue626C0C = 20;
    if (context->maxLine <= 1) {
        return;
    }

    std::array<std::int32_t, InputTextCapacity> lineLengths;
    std::int32_t offset = 0;
    std::int32_t lineCount = 0;
    std::int32_t targetLine = 0;
    std::int32_t column = 0;
    bool foundCaretLine = false;

    while (true) {
        const std::int32_t start = offset;
        std::int32_t scanned = 0;
        bool foundCarriageReturn = false;
        while (true) {
            const char current = context->text[start + scanned];
            if (current == '\0') {
                break;
            }
            if (current == '\r') {
                offset = start + scanned + 1;
                lineLengths[lineCount++] = offset - start;
                foundCarriageReturn = true;
                break;
            }
            ++scanned;
            if (scanned == context->maxCharPerLine) {
                break;
            }
        }

        if (!foundCarriageReturn) {
            const std::int32_t maxBytes = context->maxCharPerLine;
            const std::uint32_t remaining = static_cast<std::uint32_t>(
                std::strlen(context->text + start));
            if (remaining < static_cast<std::uint32_t>(maxBytes)) {
                break;
            }

            const std::int32_t boundary = ClassifyAnsiBoundary_44B1D0(
                context->text + start, maxBytes);
            context = gCurrentInputData;
            offset += context->maxCharPerLine - (boundary == 3 ? 1 : 0);
            lineLengths[lineCount++] = offset - start;
        }

        if (!foundCaretLine && context->caretByteOffset < offset) {
            foundCaretLine = true;
            targetLine = lineCount - 1;
            column = context->caretByteOffset - start;
        }
    }

    if (!foundCaretLine) {
        targetLine = lineCount;
        if (context->maxLine <= lineCount) {
            --context->caretByteOffset;
            context = gCurrentInputData;
        }
        column = context->caretByteOffset - offset;
    }
    if (targetLine <= 0) {
        return;
    }

    const std::int32_t previousLength = lineLengths[targetLine - 1];
    if (previousLength > column) {
        context->caretByteOffset -= previousLength;
        context = gCurrentInputData;
    } else if (context->maxCharPerLine > 0) {
        std::int32_t stepped = 0;
        do {
            --context->caretByteOffset;
            context = gCurrentInputData;
            if (context->text[context->caretByteOffset] == '\r') {
                break;
            }
            ++stepped;
        } while (stepped < context->maxCharPerLine);
    }

    if (ClassifyAnsiBoundary_44B1D0(
            context->text, context->caretByteOffset) == 3) {
        --gCurrentInputData->caretByteOffset;
    }
}

// 原 [0x44A540,0x44A580)：未移动时返回上下文指针，移动时返回边界分类。
std::uintptr_t Sub_44A540()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return reinterpret_cast<std::uintptr_t>(context);
    }

    const std::int32_t caret = context->caretByteOffset;
    if (caret <= 0) {
        return reinterpret_cast<std::uintptr_t>(context);
    }

    const std::int32_t boundary =
        ClassifyAnsiBoundary_44B1D0(context->text, caret);
    context->caretByteOffset -= boundary;
    gValue626C0C = 20;
    return static_cast<std::uintptr_t>(boundary);
}

// 原 [0x44A580,0x44A5C8)：按光标处字节的最高位移动一或两字节。
std::uintptr_t Sub_44A580()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return reinterpret_cast<std::uintptr_t>(context);
    }

    const std::int32_t caret = context->caretByteOffset;
    if (caret >= context->totalByteLength) {
        return reinterpret_cast<std::uintptr_t>(context);
    }

    const std::int32_t width =
        (static_cast<unsigned char>(context->text[caret]) & 0x80u) != 0u
        ? 2 : 1;
    context->caretByteOffset += width;
    gValue626C0C = 20;
    return static_cast<std::uintptr_t>(width);
}

// 原 [0x44A7D0,0x44A989)：按 CR 和最大字节宽度扫描行后下移光标。
void Sub_44A7D0()
{
    InputContext* context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return;
    }

    gValue626C0C = 20;
    if (context->maxLine <= 1) {
        return;
    }

    std::array<std::int32_t, InputTextCapacity> lineLengths;
    std::int32_t offset = 0;
    std::int32_t lineCount = 0;
    std::int32_t targetLine = 0;
    std::int32_t column = 0;
    bool foundCaretLine = false;

    while (true) {
        const std::int32_t start = offset;
        std::int32_t scanned = 0;
        bool foundCarriageReturn = false;
        while (true) {
            const char current = context->text[start + scanned];
            if (current == '\0') {
                break;
            }
            if (current == '\r') {
                offset = start + scanned + 1;
                lineLengths[lineCount++] = offset - start;
                foundCarriageReturn = true;
                break;
            }
            ++scanned;
            if (scanned == context->maxCharPerLine) {
                break;
            }
        }

        if (!foundCarriageReturn) {
            const std::int32_t maxBytes = context->maxCharPerLine;
            const std::uint32_t remaining = static_cast<std::uint32_t>(
                std::strlen(context->text + start));
            if (remaining < static_cast<std::uint32_t>(maxBytes)) {
                if (!foundCaretLine) {
                    targetLine = lineCount;
                    column = context->caretByteOffset - offset;
                }
                const std::int32_t terminalLength = static_cast<std::int32_t>(
                    std::strlen(context->text + offset));
                lineLengths[lineCount] = terminalLength + 1;
                break;
            }

            const std::int32_t boundary = ClassifyAnsiBoundary_44B1D0(
                context->text + start, maxBytes);
            context = gCurrentInputData;
            offset += context->maxCharPerLine - (boundary == 3 ? 1 : 0);
            lineLengths[lineCount++] = offset - start;
        }

        if (!foundCaretLine && context->caretByteOffset < offset) {
            foundCaretLine = true;
            targetLine = lineCount - 1;
            column = context->caretByteOffset - start;
        }
    }

    if (targetLine >= lineCount) {
        return;
    }

    const std::int32_t nextLength = lineLengths[targetLine + 1];
    const std::int32_t delta = column < nextLength
        ? lineLengths[targetLine]
        : lineLengths[targetLine] - column + nextLength - 1;
    context->caretByteOffset += delta;
    InputContext* const current = gCurrentInputData;
    if (ClassifyAnsiBoundary_44B1D0(
            current->text, current->caretByteOffset) == 3) {
        --gCurrentInputData->caretByteOffset;
    }
}

// 原 [0x44A990,0x44AA8D)：删除光标前一个旧 ANSI 单元。
void Sub_44A990()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || context->caretByteOffset <= 0) {
        return;
    }

    const std::int32_t width =
        ClassifyAnsiBoundary_44B1D0(context->text, context->caretByteOffset);
    InputContext* const copyContext = gCurrentInputData;
    const std::int32_t caret = copyContext->caretByteOffset;
    std::array<char, InputTextCapacity> tail;
    std::strcpy(tail.data(), copyContext->text + caret);
    copyContext->text[caret - width] = '\0';
    std::strcat(gCurrentInputData->text, tail.data());

    gCurrentInputData->totalByteLength -= width;
    gCurrentInputData->caretByteOffset -= width;
    InputContext* result = gCurrentInputData;
    if (result->caretByteOffset < 0) {
        result->caretByteOffset = 0;
        result = gCurrentInputData;
    }
    if (result->text[0] == '\0') {
        result->totalByteLength = 0;
    }
    gValue626C0C = 20;
}

// 原 [0x44AA90,0x44AB59)：删除光标处一个旧 ANSI 单元，返回当前上下文。
InputContext* Sub_44AA90()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr
        || context->caretByteOffset >= context->totalByteLength) {
        return context;
    }

    const std::int32_t caret = context->caretByteOffset;
    const std::int32_t width =
        (static_cast<unsigned char>(context->text[caret]) & 0x80u) != 0u
        ? 2 : 1;
    InputContext* const copyContext = gCurrentInputData;
    const std::int32_t copyCaret = copyContext->caretByteOffset;
    std::array<char, InputTextCapacity> tail;
    std::strcpy(tail.data(), copyContext->text + copyCaret + width);
    std::strcpy(copyContext->text + copyCaret, tail.data());

    gCurrentInputData->totalByteLength -= width;
    InputContext* const result = gCurrentInputData;
    if (result->text[0] == '\0') {
        result->totalByteLength = 0;
    }
    gValue626C0C = 20;
    return result;
}

} // namespace detail

bool IsRejectedInputText_404060(const char* const legacyText)
{
    // 原EDI/ESI为S32；控制分类在空白分类调用后独立重读源字节。
    const std::int32_t length = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(std::strlen(legacyText)));
    if (platform::StrStr_53BE40(legacyText, "\xA1\x40") != nullptr) {
        return true;
    }
    if (length == 0) {
        return true;
    }

    for (std::int32_t index = 0; index < length; ++index) {
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
        if (platform::ClassifyLegacySpace_53BDEF(firstByte) != 0) {
            return true;
        }
        if (platform::ClassifyLegacyControl_53BE17(SignedByte(legacyText[index])) != 0) {
            return true;
        }
    }

    return platform::StrStr_53BE40(legacyText, "[GP]") != nullptr;
}

std::int32_t Sub_476740(const char* const text)
{
    const auto length = static_cast<std::int32_t>(std::strlen(text));
    if (length == 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        const auto current = static_cast<std::uint8_t>(text[index]);
        if ((current & 0x80u) != 0) {
            index += 2;
        } else {
            if (current == ',' || current == '|' || current == '\\') {
                return 0;
            }
            ++index;
        }
    } while (index < length);

    const auto secondLength = static_cast<std::int32_t>(std::strlen(text));
    index = 0;
    do {
        const auto current = static_cast<std::uint8_t>(text[index]);
        if ((current & 0x80u) != 0) {
            if (current != 0x81u || text[index + 1] != '@') {
                return 1;
            }
            index += 2;
        } else {
            if (current != ' ') {
                return 1;
            }
            ++index;
        }
    } while (index < secondLength);
    return 0;
}

std::int32_t Sub_45E770(const char* const text)
{
    bool trailingByte = false;
    for (const char* cursor = text; *cursor != '\0'; ++cursor) {
        const auto current = static_cast<std::uint8_t>(*cursor);
        if (trailingByte) {
            trailingByte = false;
        } else if (current >= 0x81u) {
            trailingByte = true;
        } else if (current == '|' || current == '\\') {
            return 1;
        }
    }
    return 0;
}

// 原 [0x52D1B0,0x52D2C6)：原字节表按输入/转义字母交错存放。
char* EscapeLegacyText_52D1B0(
    const char* const source,
    char* const destination,
    const std::int32_t capacity)
{
    const std::int32_t sourceLength =
        static_cast<std::int32_t>(std::strlen(source));
    std::int32_t sourceIndex = 0;
    std::int32_t destinationIndex = 0;
    std::int32_t used = 1;
    std::int32_t nextWidth = 2;
    std::int32_t adjustedLength = sourceLength;

    if (sourceLength > 0) {
        while (used < capacity) {
            const std::uint8_t current =
                static_cast<std::uint8_t>(source[sourceIndex]);
            if ((current & 0x80u) != 0u) {
                if (nextWidth >= capacity || used >= adjustedLength) {
                    break;
                }
                destination[destinationIndex++] = source[sourceIndex++];
                destination[destinationIndex++] = source[sourceIndex];
                nextWidth += 2;
                used += 2;
            } else {
                std::size_t pair = 0;
                while (pair < 4
                    && current != kEscapePairs58E944[pair * 2]) {
                    ++pair;
                }
                if (pair == 4) {
                    destination[destinationIndex++] = source[sourceIndex];
                    ++nextWidth;
                    ++used;
                } else {
                    if (nextWidth >= capacity) {
                        break;
                    }
                    destination[destinationIndex++] = '\\';
                    destination[destinationIndex++] = static_cast<char>(
                        kEscapePairs58E944[pair * 2 + 1]);
                    nextWidth += 2;
                    used += 2;
                    ++adjustedLength;
                }
            }
            if (++sourceIndex >= sourceLength) {
                break;
            }
        }
    }

    destination[destinationIndex] = '\0';
    return destination;
}

// 原 [0x52D2D0,0x52D2F0)：空叶不消费上下文，后继完整透传缓冲指针。
char* EscapeLegacyTextAndNotify_52D2D0(
    const char* const source,
    char* const destination,
    const std::int32_t capacity)
{
    NullSub_509080();
    return EscapeLegacyText_52D1B0(source, destination, capacity);
}

char* EscapeInputText_52D2D0(
    const InputContext* const context,
    char* const destination,
    const std::int32_t capacity)
{
    return EscapeLegacyTextAndNotify_52D2D0(context->text, destination, capacity);
}

namespace {

// 原 [0x52D120,0x52D144)：未命中表的转义字母原样返回。
char DecodeEscapeLetter_52D120(const char letter)
{
    for (std::size_t pair = 0; pair < 4; ++pair) {
        if (static_cast<std::uint8_t>(letter)
            == kEscapePairs58E944[pair * 2 + 1]) {
            return static_cast<char>(kEscapePairs58E944[pair * 2]);
        }
    }
    return letter;
}

} // namespace

// 原 [0x52D150,0x52D1A6)：以入口长度为界，原位压缩旧字节转义。
char* UnescapeLegacyText_52D150(char* const text)
{
    const std::int32_t length =
        static_cast<std::int32_t>(std::strlen(text));
    std::int32_t sourceIndex = 0;
    std::int32_t destinationIndex = 0;
    while (sourceIndex < length) {
        const auto current =
            static_cast<std::uint8_t>(text[sourceIndex]);
        if ((current & 0x80u) != 0u) {
            text[destinationIndex++] = text[sourceIndex++];
            text[destinationIndex++] = text[sourceIndex];
        } else if (current == '\\') {
            ++sourceIndex;
            text[destinationIndex++] =
                DecodeEscapeLetter_52D120(text[sourceIndex]);
        } else {
            text[destinationIndex++] = text[sourceIndex];
        }
        ++sourceIndex;
    }
    text[destinationIndex] = '\0';
    return text;
}

// 原 [0x52D2F0,0x52D308)：空叶调用不改变转义后的文本或原指针。
char* UnescapeLegacyTextAndNotify_52D2F0(char* const text)
{
    UnescapeLegacyText_52D150(text);
    NullSub_509080();
    return text;
}

// 原[503AB0,503B6D)：旧字节分行，参数失败不写目标，正常尾始终写NUL。
std::int32_t ExtractLegacyLine_503AB0(
    char* destination, const std::int32_t capacity, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    if (requestedLine < 0 || width <= 0 || capacity <= width) {
        return 0;
    }
    std::int32_t line = 0;
    std::int32_t column = 0;
    for (;;) {
        // 原503ADD/503B55每轮只取一次首U8，分流和写出均复用该值。
        const volatile char* const sampledSource = source;
        const char value = *sampledSource;
        if (value == '\0') {
            break;
        }
        const auto byte = static_cast<std::uint8_t>(value);
        if ((byte & 0x80u) != 0) {
            column += 2;
            if (column > width) {
                ++line;
                column = 0;
                continue;
            }
            if (line == requestedLine) {
                destination[0] = value;
                // 原503AFD先写首字节，再503AFF取尾；保持源/目标可能重叠的原序。
                destination[1] = sampledSource[1];
                destination += 2;
            }
            source += 2;
        } else if (value == '\r' || value == '\n' || value == 1) {
            if (value == 1) {
                if (line >= requestedLine) {
                    break;
                }
                line = 8 * (line / 8) + 8;
            } else {
                if (line == requestedLine) {
                    break;
                }
                ++line;
            }
            column = 0;
            ++source;
            continue;
        } else {
            if (line == requestedLine) {
                *destination++ = value;
            }
            ++source;
            ++column;
        }
        if (column >= width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
        }
    }
    *destination = '\0';
    return 1;
}

// 原 [0x52D570,0x52D5BB)：每个源字节依序与删除集合比较，再原位压缩。
char RemoveLegacyBytes_52D570(
    char* const text,
    const char* const excluded)
{
    const std::size_t excludedLength = std::strlen(excluded);
    std::size_t source = 0;
    std::size_t destination = 0;
    while (text[source] != '\0') {
        std::size_t index = 0;
        while (index < excludedLength && text[source] != excluded[index]) {
            ++index;
        }
        if (index == excludedLength) {
            text[destination++] = text[source];
        }
        ++source;
    }
    text[destination] = '\0';
    return 0;
}

void AppendChar_44ADB0(const char character)
{
    InputContext* context = gCurrentInputData;
    if (context == nullptr) {
        return;
    }

    const std::int8_t signedCharacter = SignedByte(character);
    if (signedCharacter >= 1
        && signedCharacter <= 0x1F
        && signedCharacter != '\r') {
        return;
    }
    std::array<char, InputTextCapacity + 1> tail;
    std::array<char, InputTextCapacity> oldText;
    platform::StrNCpy_53BEC0(
        tail.data(),
        context->text + context->caretByteOffset,
        static_cast<std::uint32_t>(tail.size()));
    context = gCurrentInputData;
    platform::StrNCpy_53BEC0(oldText.data(), context->text, static_cast<std::uint32_t>(oldText.size()));

    // 当前对象按原阶段重取，保存/回写次序见doc/cg2d_input_append.md。
    context = gCurrentInputData;
    const std::int32_t oldTotalByteLength = context->totalByteLength;
    const std::int32_t oldCaretByteOffset = context->caretByteOffset;
    context->text[context->caretByteOffset] = character;
    context = gCurrentInputData;
    context->text[context->caretByteOffset + 1] = '\0';
    context = gCurrentInputData;
    ++context->totalByteLength;
    context = gCurrentInputData;
    ++context->caretByteOffset;

    context = gCurrentInputData;
    const std::uint32_t insertedLength = static_cast<std::uint32_t>(std::strlen(context->text));
    const std::uint32_t remainingCapacity =
        static_cast<std::uint32_t>(InputTextCapacity - 1) - insertedLength;
    platform::StrNCat_53C790(context->text, tail.data(), remainingCapacity);
    context = gCurrentInputData;

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
            const std::int32_t characterBudget = context->maxCharPerLine;
            ++scanned;
            if (scanned == characterBudget) {
                break;
            }
        }

        if (!foundCarriageReturn) {
            const std::int32_t lineBudget = context->maxCharPerLine;
            const std::uint32_t remainingLength =
                static_cast<std::uint32_t>(std::strlen(context->text + lineStart));
            if (remainingLength <= static_cast<std::uint32_t>(lineBudget)) {
                break;
            }

            const std::int32_t boundary = ClassifyAnsiBoundary_44B1D0(
                context->text + lineStart, lineBudget);
            context = gCurrentInputData;
            const bool splitDoubleByte = boundary == 3;
            const std::int32_t nextLineBudget = context->maxCharPerLine;
            if (splitDoubleByte) {
                lineStart = nextLineBudget + lineStart - 1;
            } else {
                lineStart += nextLineBudget;
            }
            lineEndedWithCarriageReturn = 0;
            ++lineCount;
        }

        if (lineCount >= context->maxLine) {
            break;
        }
    }

    const std::int32_t maximumLines = context->maxLine;
    const std::int32_t overflowControl = gValue626C20;
    if (lineCount >= maximumLines && overflowControl == 0) {
        const std::int32_t truncateOffset =
            lineStart - lineEndedWithCarriageReturn;
        context->totalByteLength -= static_cast<std::int32_t>(
            std::strlen(context->text + truncateOffset));
        context = gCurrentInputData;
        context->text[truncateOffset] = '\0';

        context = gCurrentInputData;
        if (ClassifyLastAnsiUnit_44B190(context->text) == 3) {
            context = gCurrentInputData;
            --context->totalByteLength;
            context = gCurrentInputData;
            context->text[context->totalByteLength] = '\0';
        }
        context = gCurrentInputData;
        const std::int32_t totalByteLength = context->totalByteLength;
        if (totalByteLength < context->caretByteOffset) {
            context->caretByteOffset = totalByteLength;
        }
    } else if (lineCount >= maximumLines && overflowControl != 0) {
        const std::int32_t totalByteLength = context->totalByteLength;
        const std::int32_t caretByteOffset = context->caretByteOffset;
        if (caretByteOffset == totalByteLength - 1) {
            platform::StrNCpy_53BEC0(context->text, oldText.data(), static_cast<std::uint32_t>(oldText.size()));
            context = gCurrentInputData;
            context->totalByteLength = oldTotalByteLength;
            context = gCurrentInputData;
            context->caretByteOffset = oldCaretByteOffset;
            gValue626C28 = 1;
        }
    }

    gValue626C0C = 20;
}

std::int32_t ProcessPresetHotkeys_44B340()
{
    std::int32_t base = 0;
    if ((platform::gKeyStates[0x10] & 2u) != 0) {
        base = 16;
    } else if ((platform::gKeyStates[0x11] & 2u) != 0) {
        base = 8;
    }
    std::int32_t selected = -1;
    std::int32_t result = 0;
    for (std::int32_t index = 0; index < 8; ++index) {
        result = platform::ReadKeyPressFlag_4597B0(0x70 + index);
        if (result != 0) {
            selected = base + index;
        }
    }
    if (selected >= 0) {
        LoadInitialText_44A160(gInputContexts5F9758[selected].text);
        return audio::PlaySe_52C450(51, 320);
    }
    return result;
}

void SetContext_44B330(InputContext* const context)
{
    gCurrentInputData = context;
}

std::int32_t IsCurrentContext_503E40(const InputContext* const context)
{
    return gCurrentInputData == context ? 1 : 0;
}

std::int32_t IsChatContext_503E60()
{
    return gCurrentInputData == &gInputContext626AA0 ? 1 : 0;
}

InputContext* BorrowLegacyPresetContext5F9758(const std::int32_t index) noexcept
{
    if (index < 24) {
        return &gInputContexts5F9758[index];
    }
    // 原5FB7F8紧随24项预设；旧6后续15CH视图必须与真实聊天字节共用，不能复制。
    static_assert(54 * sizeof(InputContext) <= sizeof(world::detail::gRecords5FB7F8));
    auto* const chatBytes = reinterpret_cast<std::byte*>(world::detail::gRecords5FB7F8);
    return reinterpret_cast<InputContext*>(chatBytes + static_cast<std::ptrdiff_t>(index - 24) * sizeof(InputContext));
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
platform::LegacyFile* volatile gLogStream626C1C = nullptr;
volatile std::int32_t gCountC9A944 = 0;
volatile std::uint32_t gValue626C10 = 0;
input::InputContext gInputContext626840 {};
std::uint32_t gValue626C18 = 0;
std::uint32_t gValue626BFC = 0;
char gLogFileName554648[16] = "chat_000000.txt";

// 原 [0x621FF8,0x62683C)：64 项记录与尾槽一次传输，游标不在文件块内。
struct ChatHistory621FF8 {
    char records[64][0x121];
    std::uint32_t tail626838;
};

ChatHistory621FF8 gChatHistory621FF8 {};
std::int32_t gCursor62683C = 0;

static_assert(sizeof(gRecords5FB7F8) == 512 * 0x134);
static_assert(sizeof(gInputContext626840) == 0x15C);
static_assert(sizeof(gLogFileName554648) == 16);
static_assert(std::is_standard_layout_v<ChatHistory621FF8>);
static_assert(std::is_trivially_copyable_v<ChatHistory621FF8>);
static_assert(sizeof(gChatHistory621FF8.records[0]) == 0x121);
static_assert(sizeof(gChatHistory621FF8.records) == 0x4840);
static_assert(offsetof(ChatHistory621FF8, tail626838) == 0x4840);
static_assert(sizeof(ChatHistory621FF8) == 0x4844);

namespace {

constexpr char kLogDirectory_554658[] = "Log";
constexpr char kFileFormat_55351C[] = "chat_%02d%02d%02d.txt";
constexpr char kPathFormat_553514[] = "%s\\%s";
constexpr char kReadMode_554908[] = "r";
constexpr char kAppendMode_554904[] = "a";
constexpr char kChatHistoryName554910[] = "chathis.dat";
constexpr char kChatHistoryReadMode[] = "rb";
constexpr char kChatHistoryWriteMode[] = "wb";
constexpr char kChatHistoryUpdateMode[] = "r+b";

constexpr auto kFirstSeparator_5548CC = [] {
    std::array<char, 56> text {};
    for (std::size_t i = 0; i < 8; ++i) {
        text[i] = ' ';
    }
    for (std::size_t i = 8; i < 31; ++i) {
        text[i] = '>';
    }
    for (std::size_t i = 31; i < 54; ++i) {
        text[i] = '<';
    }
    text[54] = '\n';
    return text;
}();

constexpr char kDateTitle_55488C[] =
    "        >>  \xC5\x5D \xA4\x4F \xC4\x5F \xA8\xA9 \xAB\x5F "
    "\xC0\x49 \xAA\xCC \xA4\xE9 \xBB\x78 [ %02d/%02d/%02d ]  <<\n";

constexpr auto kBeforeSession_554848 = [] {
    std::array<char, 65> text {};
    text[0] = '\n';
    text[1] = ' ';
    text[2] = ' ';
    for (std::size_t i = 3; i < 63; ++i) {
        text[i] = '-';
    }
    text[63] = '\n';
    return text;
}();

constexpr char kSessionTime_554820[] =
    " \xA1\xBD %02d/%02d/%02d %02d:%02d:%02d \xAC\xF6\xBF\xFD\n";

constexpr auto kAfterSession_5547DC = [] {
    std::array<char, 65> text {};
    text[0] = ' ';
    text[1] = ' ';
    for (std::size_t i = 2; i < 63; ++i) {
        text[i] = '-';
    }
    text[63] = '\n';
    return text;
}();

static_assert(sizeof(kLogDirectory_554658) == 4);
static_assert(sizeof(kFileFormat_55351C) == 22);
static_assert(sizeof(kPathFormat_553514) == 6);
static_assert(sizeof(kReadMode_554908) == 2);
static_assert(sizeof(kAppendMode_554904) == 2);
static_assert(sizeof(kChatHistoryName554910) == 12);
static_assert(sizeof(kChatHistoryReadMode) == 3);
static_assert(sizeof(kChatHistoryWriteMode) == 3);
static_assert(sizeof(kChatHistoryUpdateMode) == 4);
static_assert(sizeof(kDateTitle_55488C) == 63);
static_assert(sizeof(kSessionTime_554820) == 40);

} // namespace

// 原 [0x44A090,0x44A15B)：保留短读已写入的字节及 rb 失败后创建文件的顺序。
std::int32_t Sub_44A090()
{
    gCursor62683C = -1;
    char path[128];
    platform::FormatLegacyString_53B304(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const readStream = platform::OpenLegacyStream_53B2F1(path, kChatHistoryReadMode);
    if (readStream != nullptr) {
        const std::uint32_t count =
            platform::ReadLegacyStream_53BCCD(&gChatHistory621FF8, 0x4844, 1, readStream);
        platform::CloseLegacyStream_53B1D0(readStream);
        return count >= 1 ? 1 : 0;
    }

    platform::LegacyFile* const writeStream = platform::OpenLegacyStream_53B2F1(path, kChatHistoryWriteMode);
    if (writeStream != nullptr) {
        platform::WriteLegacyStream_53C680(&gChatHistory621FF8, 0x4844, 1, writeStream);
        platform::CloseLegacyStream_53B1D0(writeStream);
    }
    gCursor62683C = 63;
    return 0;
}

// 原 [0x449FA0,0x44A084)：按索引原位写入聊天历史记录及尾部 W32。
std::int32_t Sub_449FA0(const std::int32_t index)
{
    char path[128];
    platform::FormatLegacyString_53B304(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const stream = platform::OpenLegacyStream_53B2F1(path, kChatHistoryUpdateMode);
    if (stream == nullptr) {
        return 0;
    }

    platform::SeekLegacyStream_53BC41(stream, index * 0x121, 0);
    if (platform::WriteLegacyStream_53C680(gChatHistory621FF8.records[index], 0x121, 1, stream) < 1) {
        platform::CloseLegacyStream_53B1D0(stream);
        return 0;
    }

    platform::SeekLegacyStream_53BC41(stream, 0x4840, 0);
    const std::uint32_t written = platform::WriteLegacyStream_53C680(&index, sizeof(index), 1, stream);
    platform::CloseLegacyStream_53B1D0(stream);
    return written >= 1 ? 1 : 0;
}

// 原 sub_449DA0@[0x449DA0,0x449F9C)：输入记录与聊天日志初始化。
void __cdecl Sub_449DA0()
{
    std::memset(gRecords5FB7F8, 0, sizeof(gRecords5FB7F8));
    std::memset(&input::gInputContext626AA0, 0, sizeof(input::gInputContext626AA0));
    input::InitializeContext_449CF0(
        &input::gInputContext626AA0, 8, 440, 0, 1, 0, nullptr, 1, 78, 0, 0);
    std::memcpy(
        &gInputContext626840,
        &input::gInputContext626AA0,
        sizeof(gInputContext626840));

    for (std::size_t index = 0; index < 19; ++index) {
        gRecords5FB7F8[index].state = 1;
    }
    gValue626C18 = 0;
    gValue626BFC = 0;

    std::int32_t timeValue = 0;
    platform::LegacyFile* const existingStream = gLogStream626C1C;
    if (existingStream != nullptr) {
        return;
    }

    platform::CreateLegacyDirectory_53C654(kLogDirectory_554658);
    platform::ReadLegacyTime32_53BA26(&timeValue);
    const platform::LegacyTm53B8C6* const legacyTime =
        platform::LocalTime_53B8C6(&timeValue);
    const auto* const timeBytes =
        reinterpret_cast<const unsigned char*>(legacyTime);
    const auto readTimeField = [timeBytes](const std::size_t offset) {
        std::int32_t value;
        std::memcpy(&value, timeBytes + offset, sizeof(value));
        return value;
    };

    const std::int32_t fileMonth = readTimeField(0x10);
    const std::int32_t fileDay = readTimeField(0x0C);
    const std::int32_t fileYear = readTimeField(0x14);
    platform::FormatLegacyString_53B304(
        gLogFileName554648,
        kFileFormat_55351C,
        fileYear % 100, fileMonth + 1, fileDay);
    platform::FormatLegacyString_53B304(
        gLogPath62699C,
        kPathFormat_553514,
        kLogDirectory_554658,
        gLogFileName554648);

    platform::LegacyFile* const readStream =
        platform::OpenLegacyStream_53B2F1(gLogPath62699C, kReadMode_554908);
    gLogStream626C1C = readStream;
    platform::LegacyFile* gate;
    if (readStream == nullptr) {
        platform::LegacyFile* const appendStream =
            platform::OpenLegacyStream_53B2F1(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        platform::FormatLegacyStream_53BC0F(appendStream, kFirstSeparator_5548CC.data());

        const std::int32_t titleMonth = readTimeField(0x10);
        const std::int32_t titleDay = readTimeField(0x0C);
        const std::int32_t titleYear = readTimeField(0x14);
        platform::FormatLegacyStream_53BC0F(
            gLogStream626C1C,
            kDateTitle_55488C,
            titleYear % 100, titleMonth + 1, titleDay);
        platform::FormatLegacyStream_53BC0F(gLogStream626C1C, kFirstSeparator_5548CC.data());
        gate = gLogStream626C1C;
    } else {
        platform::CloseLegacyStream_53B1D0(readStream);
        platform::LegacyFile* const appendStream =
            platform::OpenLegacyStream_53B2F1(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        gate = appendStream;
    }

    if (gate != nullptr) {
        platform::FormatLegacyStream_53BC0F(gate, kBeforeSession_554848.data());

        const std::int32_t sessionSecond = readTimeField(0x00);
        const std::int32_t sessionMinute = readTimeField(0x04);
        const std::int32_t sessionHour = readTimeField(0x08);
        const std::int32_t sessionDay = readTimeField(0x0C);
        const std::int32_t sessionMonth = readTimeField(0x10);
        const std::int32_t sessionYear = readTimeField(0x14);
        platform::FormatLegacyStream_53BC0F(
            gLogStream626C1C,
            kSessionTime_554820,
            sessionYear % 100, sessionMonth + 1, sessionDay,
            sessionHour, sessionMinute, sessionSecond);
        platform::FormatLegacyStream_53BC0F(gLogStream626C1C, kAfterSession_5547DC.data());
    }
    platform::FlushLegacyStream_53BB02(gLogStream626C1C);
}

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

    const platform::LegacyFile* const gate = gLogStream626C1C;
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

        platform::FormatLegacyStream_53BC0F(
            gLogStream626C1C,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, gRecords5FB7F8[index].text);
        platform::CloseLegacyStream_53B1D0(gLogStream626C1C);
        platform::LegacyFile* const newStream = platform::OpenLegacyStream_53B2F1(gLogPath62699C, "a");
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

namespace cg::input {

void BrowseChatHistory_44A1B0()
{
    using world::detail::gChatHistory621FF8;
    using world::detail::gCursor62683C;
    NullSub_509080();
    InputContext* const initialContext = gCurrentInputData;
    if (initialContext != &gInputContext626AA0 || platform::gImeComposition[0] != '\0') {
        return;
    }
    const std::uint8_t control = platform::gKeyStates[17] & 2u;
    if (control == 0 && (platform::gKeyStates[16] & 2u) == 0
        && (platform::gKeyStates[38] & 0x10u) != 0) {
        const std::int32_t previous = gCursor62683C;
        const auto tail = std::bit_cast<std::int32_t>(gChatHistory621FF8.tail626838);
        std::int32_t selected = previous == -1 ? tail : previous - 1;
        gCursor62683C = selected;
        if (selected < 0) {
            selected = 63;
            gCursor62683C = selected;
        }
        if (gChatHistory621FF8.records[selected][0] == '\0'
            || (selected == tail && previous != -1)) {
            gCursor62683C = previous;
            return;
        }
    } else {
        // 向下复用已读Ctrl位，不增加Shift门禁或空记录检查。
        if (control != 0 || (platform::gKeyStates[40] & 0x10u) == 0) {
            return;
        }
        const std::int32_t selected = gCursor62683C;
        if (selected == -1) {
            return;
        }
        if (selected == std::bit_cast<std::int32_t>(gChatHistory621FF8.tail626838)) {
            gCursor62683C = -1;
            initialContext->totalByteLength = 0;
            gCurrentInputData->text[0] = '\0';
            gCurrentInputData->caretByteOffset = 0;
            return;
        }
        const std::int32_t next = selected + 1;
        gCursor62683C = next;
        if (next >= 64) {
            gCursor62683C = 0;
        }
    }
    initialContext->totalByteLength = 0;
    gCurrentInputData->text[0] = '\0';
    gCurrentInputData->caretByteOffset = 0;
    LoadInitialText_44A160(gChatHistory621FF8.records[gCursor62683C]);
    std::array<char, 256> text;
    std::strcpy(text.data(), gChatHistory621FF8.records[gCursor62683C]);
    const auto length = static_cast<std::int32_t>(std::strlen(text.data()));
    const std::int32_t width = ui::ReadChatInputByteWidth_4A80E0();
    std::int32_t offset = length - width;
    if (ClassifyLegacyBoundary_44B440(text.data(),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(offset))) == 2) {
        ++offset;
    }
    if (offset < 0) {
        offset = 0;
    }
    gCurrentInputData->field158 = offset;
}

} // namespace cg::input

namespace cg::input::detail {

void Sub_44AB60()
{
    if (platform::gImeComposition[0] != '\0') {
        return;
    }
    InputContext* context = gCurrentInputData;
    if (context == &gInputContext626AA0) {
        if (context->totalByteLength == 0) {
            return;
        }
        auto& history = world::detail::gChatHistory621FF8;
        const std::uint32_t oldTail = history.tail626838;
        const std::int32_t oldIndex = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(oldTail));
        std::int32_t nextIndex = std::bit_cast<std::int32_t>(oldTail) + 1;
        history.tail626838 = static_cast<std::uint32_t>(nextIndex);
        if (nextIndex >= 64) {
            nextIndex = 0;
            history.tail626838 = 0;
        }
        if (std::strcmp(context->text, history.records[oldIndex]) == 0) {
            history.tail626838 = static_cast<std::uint32_t>(oldIndex);
        } else {
            std::strcpy(history.records[nextIndex], context->text);
            static_cast<void>(world::detail::Sub_449FA0(
                std::bit_cast<std::int32_t>(history.tail626838)));
            context = gCurrentInputData;
        }
        const std::int32_t color = gInputContext626AA0.field140;
        world::detail::gCursor62683C = -1;
        static_cast<void>(world::detail::Sub_50C940(context, color));
        gCurrentInputData->totalByteLength = 0;
        gCurrentInputData->caretByteOffset = 0;
        gCurrentInputData->text[0] = '\0';
        context = gCurrentInputData;
    } else if (context == &gInputContextD66CA8
               || context == &gInputContextCB03F0
               || context == &gInputContextCC46E0
               || context == &gInputContextCC2080
               || context == &gInputContextD2D828
               || context == &gInputContextCA0180) {
        AppendChar_44ADB0('\r');
        context = gCurrentInputData;
    }
    if (context == &gInputContextC8B1B8) {
        char message[256];
        const char* const text = gInputContextC8B1B8.text;
        if (std::strcmp(text, world::detail::BorrowPlayerDisplayText(1)) == 0) {
            platform::FormatLegacyString_53B304(message, kBlacklistSelfText554938);
        } else if (blacklist::ContainsBlacklistRecord_4013D0(text) != 0) {
            platform::FormatLegacyString_53B304(message, kBlacklistExistingFormat554928, text);
        } else {
            const char* const format = blacklist::AddBlacklistRecord_401270(text) != 0
                ? kBlacklistAddedFormat5532C4 : kBlacklistFailedFormat5532B0;
            platform::FormatLegacyString_53B304(message, format, text);
        }
        gInputContextC8B1B8.text[0] = '\0';
        gInputContextC8B1B8.caretByteOffset = 0;
        gInputContextC8B1B8.totalByteLength = 0;
        world::detail::Sub_44B050(message, 4, 3);
        SetContext_44B330(&gInputContext626AA0);
    }
    gValue626C0C = 20;
}

} // namespace cg::input::detail
