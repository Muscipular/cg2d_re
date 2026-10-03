#include "LegacyFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyRuntimeError.hpp"
#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

namespace {

// 原54F96C：低半为20H..78H字符类别，高半为类别*8+旧状态的转移。
constexpr std::uint8_t kFormatTable54F96C[89] = {
    6, 0, 0, 6, 0, 1, 0, 0, 16, 0, 3, 6, 0, 6, 2, 16, 4, 69, 69, 69, 5, 5, 5, 5, 5, 53, 48, 0, 80, 0, 0, 0, 0, 32, 40, 56, 80, 88, 7, 8, 0, 55, 48, 48, 87, 80, 7, 0, 0, 32, 32, 8, 0, 0, 0, 0, 8, 96, 104, 96, 96, 96, 96, 0, 0, 112, 112, 120, 120, 120, 120, 8, 7, 8, 0, 0, 7, 0, 8, 8, 8, 0, 0, 8, 0, 8, 0, 7, 8
};

enum FormatFlags : std::uint32_t {
    kPlus = 1, kSpace = 2, kLeft = 4, kZero = 8,
    kLong = 0x10, kShort = 0x20, kSigned = 0x40, kAlternate = 0x80,
    kNegative = 0x100, kOctalPrefix = 0x200, kWide = 0x800, kInt64 = 0x8000
};

// 原同一无参trap经不同参数形态调用；原生C++使用精确类型适配入口。
char* TrapFloatFormat(double*, char*, std::int32_t, std::int32_t, std::int32_t)
{
    LegacyFloatingPointTrap_545031();
}

void TrapFloatText(char*)
{
    LegacyFloatingPointTrap_545031();
}

void TrapFloatAssign(std::int32_t, void*, const char*)
{
    LegacyFloatingPointTrap_545031();
}

std::int32_t TrapFloatNonnegative(double*)
{
    LegacyFloatingPointTrap_545031();
}

} // namespace

const char* gLegacyFormatNullText58FB38 = "(null)";
const wchar_t* gLegacyFormatNullWide58FB3C = L"(null)";
LegacyFloatFormatCallback gLegacyFloatFormat58FB50 = TrapFloatFormat;
LegacyFloatTextCallback gLegacyFloatCropZeros58FB54 = TrapFloatText;
LegacyFloatAssignCallback gLegacyFloatAssign58FB58 = TrapFloatAssign;
LegacyFloatTextCallback gLegacyFloatForcePoint58FB5C = TrapFloatText;
LegacyFloatNonnegativeCallback gLegacyFloatNonnegative58FB60 = TrapFloatNonnegative;
LegacyFloatFormatCallback gLegacyFloatFormatAlias58FB64 = TrapFloatFormat;

[[noreturn]] void LegacyFloatingPointTrap_545031()
{
    ExitLegacyRuntimeError_53DEAB(2);
    __assume(0);
}

std::int32_t FormatLegacyOutput_53EB28(LegacyFile* const stream,
    const char* format, std::va_list arguments)
{
    char scratch[512];
    char prefix[2];
    std::int32_t textLength = 0;
    // 初始长度0不消费该指针；随后保留前一字段正文，不在%入口重置。
    const char* text = scratch;
    std::int32_t written = 0;
    std::int32_t state = 0;
    char* allocated = nullptr;
    std::uint32_t flags;
    std::int32_t width;
    std::int32_t precision;
    std::int32_t prefixLength;
    std::int32_t uppercase;
    bool suppress;
    bool wideText;
    char character = *format++;
    while (character != '\0') {
        if (written < 0) {
            break;
        }
        const auto signedCharacter =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(character));
        const std::uint32_t category = signedCharacter >= 0x20 && signedCharacter <= 0x78
            ? kFormatTable54F96C[static_cast<std::uint8_t>(character) - 0x20] & 0xFu : 0u;
        state = std::bit_cast<std::int8_t>(kFormatTable54F96C[category * 8 + state]) >> 4;
        switch (state) {
        case 0:
        writeOrdinaryCharacter:
            wideText = false;
            if ((gCtypePointer58FC30[static_cast<std::uint8_t>(character)] & 0x8000u) != 0) {
                WriteLegacyFormatByte_53F2C6(signedCharacter, stream, &written);
                character = *format++;
            }
            WriteLegacyFormatByte_53F2C6(
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(character)),
                stream, &written);
            break;
        case 1:
            precision = -1;
            uppercase = 0;
            suppress = false;
            width = 0;
            prefixLength = 0;
            flags = 0;
            wideText = false;
            break;
        case 2:
            switch (character) {
            case ' ': flags |= kSpace; break;
            case '#': flags |= kAlternate; break;
            case '+': flags |= kPlus; break;
            case '-': flags |= kLeft; break;
            case '0': flags |= kZero; break;
            }
            break;
        case 3:
            if (character == '*') {
                width = va_arg(arguments, int);
                if (width < 0) {
                    flags |= kLeft;
                    width = -width;
                }
            } else {
                width = width * 10 + signedCharacter - '0';
            }
            break;
        case 4:
            precision = 0;
            break;
        case 5:
            if (character == '*') {
                precision = va_arg(arguments, int);
                if (precision < 0) {
                    precision = -1;
                }
            } else {
                precision = precision * 10 + signedCharacter - '0';
            }
            break;
        case 6:
            switch (character) {
            case 'h': flags |= kShort; break;
            case 'l': flags |= kLong; break;
            case 'w': flags |= kWide; break;
            case 'I':
                if (format[0] == '6' && format[1] == '4') {
                    format += 2;
                    flags |= kInt64;
                } else {
                    state = 0;
                    goto writeOrdinaryCharacter;
                }
                break;
            }
            break;
        case 7: {
            bool integer = false;
            bool pointer = false;
            std::uint32_t radix = 10;
            std::int32_t digitOffset = 0;
            switch (character) {
            case 'C':
                if ((flags & (kWide | kLong | kShort)) == 0) {
                    flags |= kWide;
                }
                [[fallthrough]];
            case 'c':
                if ((flags & (kWide | kLong)) != 0) {
                    const auto value = static_cast<std::uint16_t>(va_arg(arguments, int));
                    textLength = Wctomb_54472B(scratch, value);
                    if (textLength < 0) {
                        suppress = true;
                    }
                } else {
                    scratch[0] = std::bit_cast<char>(
                        static_cast<std::uint8_t>(va_arg(arguments, int)));
                    textLength = 1;
                }
                text = scratch;
                break;
            case 'S':
                if ((flags & (kWide | kLong | kShort)) == 0) {
                    flags |= kWide;
                }
                [[fallthrough]];
            case 's': {
                std::int32_t limit = precision == -1 ? 0x7FFFFFFF : precision;
                if ((flags & (kWide | kLong)) != 0) {
                    const wchar_t* value = va_arg(arguments, const wchar_t*);
                    if (value == nullptr) {
                        value = gLegacyFormatNullWide58FB3C;
                    }
                    text = reinterpret_cast<const char*>(value);
                    wideText = true;
                    const char* cursor = text;
                    while (limit != 0) {
                        std::uint16_t codeUnit;
                        std::memcpy(&codeUnit, cursor, sizeof(codeUnit));
                        if (codeUnit == 0) {
                            break;
                        }
                        --limit;
                        cursor += 2;
                    }
                    textLength = static_cast<std::int32_t>((cursor - text) / 2);
                } else {
                    const char* value = va_arg(arguments, const char*);
                    if (value == nullptr) {
                        value = gLegacyFormatNullText58FB38;
                    }
                    text = value;
                    const char* cursor = value;
                    while (limit != 0 && *cursor != '\0') {
                        --limit;
                        ++cursor;
                    }
                    textLength = static_cast<std::int32_t>(cursor - value);
                }
                break;
            }
            case 'Z': {
                const auto* value = va_arg(arguments, const LegacyCountedFormatString*);
                const void* source = value != nullptr ? value->text : nullptr;
                if (source == nullptr) {
                    text = gLegacyFormatNullText58FB38;
                    textLength = static_cast<std::int32_t>(StrLen_53F9E0(text));
                } else {
                    text = static_cast<const char*>(source);
                    textLength = value->length;
                    if ((flags & kWide) != 0) {
                        textLength = static_cast<std::int32_t>(
                            static_cast<std::uint32_t>(textLength) >> 1);
                        wideText = true;
                    } else {
                        wideText = false;
                    }
                }
                break;
            }
            case 'n':
                if ((flags & kShort) != 0) {
                    auto* const destination = va_arg(arguments, std::int16_t*);
                    const auto value = static_cast<std::uint16_t>(written);
                    std::memcpy(destination, &value, sizeof(value));
                } else if ((flags & kLong) != 0) {
                    auto* const destination = va_arg(arguments, long*);
                    std::memcpy(destination, &written, sizeof(written));
                } else {
                    auto* const destination = va_arg(arguments, std::int32_t*);
                    std::memcpy(destination, &written, sizeof(written));
                }
                suppress = true;
                break;
            case 'E':
            case 'G':
                uppercase = 1;
                character = static_cast<char>(character + 0x20);
                [[fallthrough]];
            case 'e':
            case 'f':
            case 'g': {
                flags |= kSigned;
                char* output = scratch;
                text = output;
                if (precision < 0) {
                    precision = 6;
                } else if (precision == 0) {
                    if (character == 'g') {
                        precision = 1;
                    }
                } else {
                    if (precision > 512) {
                        precision = 512;
                    }
                    if (precision > 163) {
                        allocated = static_cast<char*>(Malloc_53C3AC(
                            static_cast<std::uint32_t>(precision + 349)));
                        if (allocated != nullptr) {
                            output = allocated;
                            text = output;
                        } else {
                            precision = 163;
                        }
                    }
                }
                double value = va_arg(arguments, double);
                gLegacyFloatFormat58FB50(&value, output, character, precision, uppercase);
                const bool alternate = (flags & kAlternate) != 0;
                if (alternate && precision == 0) {
                    gLegacyFloatForcePoint58FB5C(output);
                }
                if (character == 'g' && !alternate) {
                    gLegacyFloatCropZeros58FB54(output);
                }
                if (*output == '-') {
                    flags |= kNegative;
                    ++output;
                    text = output;
                }
                textLength = static_cast<std::int32_t>(StrLen_53F9E0(output));
                break;
            }
            case 'd':
            case 'i':
                flags |= kSigned;
                integer = true;
                break;
            case 'u':
                integer = true;
                break;
            case 'o':
                radix = 8;
                integer = true;
                if ((flags & kAlternate) != 0) {
                    flags |= kOctalPrefix;
                }
                break;
            case 'p':
                precision = static_cast<std::int32_t>(sizeof(void*) * 2);
                pointer = true;
                [[fallthrough]];
            case 'X':
                digitOffset = 7;
                radix = 16;
                integer = true;
                break;
            case 'x':
                digitOffset = 0x27;
                radix = 16;
                integer = true;
                break;
            }
            if (integer) {
                if (radix == 16 && (flags & kAlternate) != 0) {
                    prefix[0] = '0';
                    prefix[1] = static_cast<char>(digitOffset + 0x51);
                    prefixLength = 2;
                }
                std::uint64_t value;
                if (pointer) {
                    value = reinterpret_cast<std::uintptr_t>(va_arg(arguments, void*));
                } else if ((flags & kInt64) != 0) {
                    value = (flags & kSigned) != 0
                        ? static_cast<std::uint64_t>(va_arg(arguments, std::int64_t))
                        : va_arg(arguments, std::uint64_t);
                } else if ((flags & kShort) != 0) {
                    const auto word = static_cast<std::uint16_t>(va_arg(arguments, int));
                    value = (flags & kSigned) != 0
                        ? static_cast<std::uint64_t>(std::bit_cast<std::int16_t>(word)) : word;
                } else if ((flags & kLong) != 0) {
                    value = (flags & kSigned) != 0
                        ? static_cast<std::uint64_t>(va_arg(arguments, long))
                        : va_arg(arguments, unsigned long);
                } else {
                    value = (flags & kSigned) != 0
                        ? static_cast<std::uint64_t>(va_arg(arguments, int))
                        : va_arg(arguments, unsigned int);
                }
                if ((flags & kSigned) != 0 && std::bit_cast<std::int64_t>(value) < 0) {
                    value = 0u - value;
                    flags |= kNegative;
                }
                if (!pointer && (flags & kInt64) == 0) {
                    value = static_cast<std::uint32_t>(value);
                }
                if (precision < 0) {
                    precision = 1;
                } else {
                    flags &= ~kZero;
                    if (precision > 512) {
                        precision = 512;
                    }
                }
                if (value == 0) {
                    prefixLength = 0;
                }
                std::int32_t index = 512;
                for (;;) {
                    const std::int32_t previousPrecision = precision--;
                    if (previousPrecision <= 0 && value == 0) {
                        break;
                    }
                    std::int32_t digit = static_cast<std::int32_t>(value % radix) + '0';
                    value /= radix;
                    if (digit > '9') {
                        digit += digitOffset;
                    }
                    scratch[--index] = static_cast<char>(digit);
                }
                textLength = 512 - index;
                if ((flags & kOctalPrefix) != 0 && (textLength == 0 || scratch[index] != '0')) {
                    scratch[--index] = '0';
                    ++textLength;
                }
                text = scratch + index;
            }
            if (!suppress) {
                if ((flags & kSigned) != 0) {
                    if ((flags & kNegative) != 0) {
                        prefix[0] = '-';
                        prefixLength = 1;
                    } else if ((flags & kPlus) != 0) {
                        prefix[0] = '+';
                        prefixLength = 1;
                    } else if ((flags & kSpace) != 0) {
                        prefix[0] = ' ';
                        prefixLength = 1;
                    }
                }
                const std::int32_t padding = width - prefixLength - textLength;
                if ((flags & (kLeft | kZero)) == 0) {
                    RepeatLegacyFormatByte_53F2FB(' ', padding, stream, &written);
                }
                WriteLegacyFormatString_53F32C(prefix, prefixLength, stream, &written);
                if ((flags & kZero) != 0 && (flags & kLeft) == 0) {
                    RepeatLegacyFormatByte_53F2FB('0', padding, stream, &written);
                }
                if (wideText && textLength > 0) {
                    const char* cursor = text;
                    for (std::int32_t remaining = textLength; remaining != 0; --remaining) {
                        std::uint16_t value;
                        std::memcpy(&value, cursor, sizeof(value));
                        cursor += 2;
                        char bytes[4];
                        const std::int32_t count = Wctomb_54472B(bytes, value);
                        if (count <= 0) {
                            break;
                        }
                        WriteLegacyFormatString_53F32C(bytes, count, stream, &written);
                    }
                } else {
                    WriteLegacyFormatString_53F32C(text, textLength, stream, &written);
                }
                if ((flags & kLeft) != 0) {
                    RepeatLegacyFormatByte_53F2FB(' ', padding, stream, &written);
                }
            }
            if (allocated != nullptr) {
                Free_53C0A9(allocated);
                allocated = nullptr;
            }
            break;
        }
        }
        character = *format++;
    }
    return written;
}

std::int32_t FormatLegacyString_53B304(char* const buffer, const char* const format, ...)
{
    LegacyFile stream;
    stream.base = buffer;
    stream.current = buffer;
    stream.flags = 0x42;
    stream.remaining = 0x7FFFFFFF;
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = FormatLegacyOutput_53EB28(&stream, format, arguments);
    va_end(arguments);
    --stream.remaining;
    if (stream.remaining >= 0) {
        *stream.current = '\0';
    } else {
        static_cast<void>(FlushAndWriteLegacyStreamByte_53F8C0(0, &stream));
    }
    return result;
}

std::int32_t FormatLegacyBoundedString_53D21D(char* const buffer,
    const std::uint32_t capacity, const char* const format, ...)
{
    LegacyFile stream;
    stream.base = buffer;
    stream.current = buffer;
    stream.flags = 0x42;
    stream.remaining = std::bit_cast<std::int32_t>(capacity);
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = FormatLegacyOutput_53EB28(&stream, format, arguments);
    va_end(arguments);
    --stream.remaining;
    if (stream.remaining >= 0) {
        *stream.current = '\0';
    } else {
        static_cast<void>(FlushAndWriteLegacyStreamByte_53F8C0(0, &stream));
    }
    return result;
}

std::int32_t FormatLegacyStream_53BC0F(LegacyFile* const stream, const char* const format, ...)
{
    const std::int32_t started = BeginLegacyTemporaryBuffer_53EA5E(stream);
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = FormatLegacyOutput_53EB28(stream, format, arguments);
    va_end(arguments);
    EndLegacyTemporaryBuffer_53EAEB(started, stream);
    return result;
}

} // namespace cg::platform
