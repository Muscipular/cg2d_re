#include "LegacyFloatFormat.hpp"
#include "LegacyFloatEnvironment.hpp"
#include "LegacyFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

LegacyFloatDigits* gLegacyFloatDigits1086048 = nullptr;
std::uint8_t gLegacyFloatReuse108604C = 0;
std::int32_t gLegacyFloatExponent1086050 = 0;
std::uint8_t gLegacyFloatCarry1086054 = 0;
std::uint8_t gLegacyDecimalPoint58FE40 = '.';
LegacyFloatDecimalText gLegacyDecimalText10862B0 {};
LegacyFloatDigits gLegacyProducedFloatDigits10862D0 {};

LegacyFloatDigits* ProduceLegacyFloatDigits_544F17(const double value)
{
    LegacyFloat80 extended;
    ConvertLegacyDoubleTo80_544F7B(&extended, &value);
    const std::int32_t result = GenerateLegacyFloatDecimal_546F36(
        extended, 17, 0, &gLegacyDecimalText10862B0);
    gLegacyProducedFloatDigits10862D0.result = result;
    gLegacyProducedFloatDigits10862D0.sign = gLegacyDecimalText10862B0.sign;
    gLegacyProducedFloatDigits10862D0.decimal = gLegacyDecimalText10862B0.decimal;
    gLegacyProducedFloatDigits10862D0.digits = gLegacyDecimalText10862B0.digits;
    return &gLegacyProducedFloatDigits10862D0;
}

void ShiftLegacyFloatText_5401EB(char* const text, const std::int32_t distance)
{
    if (distance != 0) {
        const auto count = static_cast<std::uint32_t>(StrLen_53F9E0(text)) + 1u;
        static_cast<void>(std::memmove(text + distance, text, count));
    }
}

void RoundLegacyFloatDigits_544EA0(char* const output,
    const std::int32_t precision, LegacyFloatDigits* const digits)
{
    const char* source = digits->digits;
    char* cursor = output + 1;
    *output = '0';
    for (std::int32_t remaining = precision; remaining > 0; --remaining) {
        const char value = *source;
        if (value != '\0') {
            *cursor++ = value;
            ++source;
        } else {
            *cursor++ = '0';
        }
    }
    *cursor = '\0';
    if (precision >= 0
        && std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*source)) >= '5') {
        while (*--cursor == '9') {
            *cursor = '0';
        }
        ++*cursor;
    }
    if (*output == '1') {
        ++digits->decimal;
    } else {
        const auto count = static_cast<std::uint32_t>(StrLen_53F9E0(output + 1)) + 1u;
        static_cast<void>(std::memmove(output, output + 1, count));
    }
}

void ForceLegacyDecimalPoint_53FDD5(char* text)
{
    const auto first = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*text));
    if (LowerLegacyCharacter_53DC64(first) != 'e') {
        std::uint32_t digit;
        do {
            ++text;
            const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*text));
            digit = gLegacyCtypeMaxBytes58FE3C > 1
                ? ClassifyLegacyCharacter_541955(value, 4)
                : gCtypePointer58FC30[value] & 4u;
        } while (digit != 0);
    }
    char displaced = *text;
    *text++ = std::bit_cast<char>(gLegacyDecimalPoint58FE40);
    char written;
    do {
        const char next = *text;
        *text = displaced;
        displaced = next;
        written = *text++;
    } while (written != '\0');
}

void CropLegacyFloatZeros_53FE2F(char* text)
{
    const char point = std::bit_cast<char>(gLegacyDecimalPoint58FE40);
    while (*text != '\0' && *text != point) {
        ++text;
    }
    if (*text++ == '\0') {
        return;
    }
    while (*text != '\0' && *text != 'e' && *text != 'E') {
        ++text;
    }
    const char* suffix = text;
    while (*--text == '0') {
    }
    if (*text == point) {
        --text;
    }
    char value;
    do {
        value = *suffix++;
        *++text = value;
    } while (value != '\0');
}

char* FormatLegacyFloatScientific_53FED3(double* const value, char* const output,
    const std::int32_t precision, const std::int32_t uppercase)
{
    LegacyFloatDigits* digits;
    if (gLegacyFloatReuse108604C != 0) {
        digits = gLegacyFloatDigits1086048;
        ShiftLegacyFloatText_5401EB(output + (digits->sign == '-' ? 1 : 0),
            precision > 0 ? 1 : 0);
    } else {
        digits = ProduceLegacyFloatDigits_544F17(*value);
        RoundLegacyFloatDigits_544EA0(
            output + (digits->sign == '-' ? 1 : 0) + (precision > 0 ? 1 : 0),
            precision + 1, digits);
    }
    char* cursor = output;
    if (digits->sign == '-') {
        *cursor++ = '-';
    }
    if (precision > 0) {
        *cursor = cursor[1];
        *++cursor = std::bit_cast<char>(gLegacyDecimalPoint58FE40);
    }
    cursor += (gLegacyFloatReuse108604C == 0 ? 1 : 0) + precision;
    // 原54FA0C的6B模板，含三位指数和终止字节。
    StrCpy_544DB0(cursor, "e+000");
    if (uppercase != 0) {
        *cursor = 'E';
    }
    ++cursor;
    if (digits->digits[0] != '0') {
        std::int32_t exponent = digits->decimal - 1;
        if (exponent < 0) {
            exponent = -exponent;
            *cursor = '-';
        }
        ++cursor;
        if (exponent >= 100) {
            *cursor = static_cast<char>(*cursor + exponent / 100);
            exponent %= 100;
        }
        ++cursor;
        if (exponent >= 10) {
            *cursor = static_cast<char>(*cursor + exponent / 10);
            exponent %= 10;
        }
        cursor[1] = static_cast<char>(cursor[1] + exponent);
    }
    return output;
}

char* FormatLegacyFloatFixed_53FFD7(double* const value, char* const output,
    std::int32_t precision)
{
    LegacyFloatDigits* digits;
    if (gLegacyFloatReuse108604C != 0) {
        const std::int32_t exponent = gLegacyFloatExponent1086050;
        digits = gLegacyFloatDigits1086048;
        if (exponent == precision) {
            char* const tail = output + (digits->sign == '-' ? 1 : 0) + exponent;
            tail[0] = '0';
            tail[1] = '\0';
        }
    } else {
        digits = ProduceLegacyFloatDigits_544F17(*value);
        RoundLegacyFloatDigits_544EA0(output + (digits->sign == '-' ? 1 : 0),
            digits->decimal + precision, digits);
    }
    char* cursor = output;
    if (digits->sign == '-') {
        *cursor++ = '-';
    }
    const std::int32_t decimal = digits->decimal;
    if (decimal <= 0) {
        ShiftLegacyFloatText_5401EB(cursor, 1);
        *cursor++ = '0';
    } else {
        cursor += decimal;
    }
    if (precision > 0) {
        ShiftLegacyFloatText_5401EB(cursor, 1);
        *cursor++ = std::bit_cast<char>(gLegacyDecimalPoint58FE40);
        const std::int32_t currentDecimal = digits->decimal;
        if (currentDecimal < 0) {
            const std::int32_t zeros = -currentDecimal;
            if (gLegacyFloatReuse108604C != 0 || precision >= zeros) {
                precision = zeros;
            }
            ShiftLegacyFloatText_5401EB(cursor, precision);
            std::memset(cursor, '0', static_cast<std::uint32_t>(precision));
        }
    }
    return output;
}

char* FormatLegacyFloatScientificCached_540150(double* const value, char* const output,
    const std::int32_t precision, const std::int32_t uppercase)
{
    gLegacyFloatReuse108604C = 1;
    char* const result = FormatLegacyFloatScientific_53FED3(value, output, precision, uppercase);
    gLegacyFloatReuse108604C = 0;
    return result;
}

char* FormatLegacyFloatFixedCached_540177(double* const value, char* const output,
    const std::int32_t precision)
{
    gLegacyFloatReuse108604C = 1;
    char* const result = FormatLegacyFloatFixed_53FFD7(value, output, precision);
    gLegacyFloatReuse108604C = 0;
    return result;
}

char* FormatLegacyFloatGeneral_5400B5(double* const value, char* const output,
    const std::int32_t precision, const std::int32_t uppercase)
{
    LegacyFloatDigits* const produced = ProduceLegacyFloatDigits_544F17(*value);
    gLegacyFloatDigits1086048 = produced;
    gLegacyFloatExponent1086050 = produced->decimal - 1;
    char* const text = output + (produced->sign == '-' ? 1 : 0);
    RoundLegacyFloatDigits_544EA0(text, precision, produced);
    LegacyFloatDigits* const digits = gLegacyFloatDigits1086048;
    const bool carry = gLegacyFloatExponent1086050 < digits->decimal - 1;
    gLegacyFloatCarry1086054 = carry ? 1 : 0;
    const std::int32_t exponent = digits->decimal - 1;
    gLegacyFloatExponent1086050 = exponent;
    if (exponent >= -4 && exponent < precision) {
        if (carry) {
            char* cursor = text;
            while (*cursor++ != '\0') {
            }
            cursor[-2] = '\0';
        }
        return FormatLegacyFloatFixedCached_540177(value, output, precision);
    }
    return FormatLegacyFloatScientificCached_540150(value, output, precision, uppercase);
}

char* ConvertLegacyFloatText_54019A(double* const value, char* const output,
    const std::int32_t format, const std::int32_t precision, const std::int32_t uppercase)
{
    if (format == 'e' || format == 'E') {
        return FormatLegacyFloatScientific_53FED3(value, output, precision, uppercase);
    }
    if (format == 'f') {
        return FormatLegacyFloatFixed_53FFD7(value, output, precision);
    }
    return FormatLegacyFloatGeneral_5400B5(value, output, precision, uppercase);
}


std::int32_t ParseLegacyFloat96_546419(LegacyFloat96* const output, const char** const end,
    const char* const source, const std::int32_t preserveExtra,
    const std::int32_t exponentAdjustment, const std::int32_t fractionDigits,
    const std::int32_t allowImplicitExponent)
{
    // 原5468BA跳表的状态值；D/E及无标记指数各自保留回退起点。
    enum class ParseState : std::int32_t {
        Start = 0, LeadingZeros = 1, AfterSign = 2, Integer = 3,
        Fraction = 4, RequireFraction = 5, ExponentStart = 6,
        ExponentSign = 7, ExponentZeros = 8, ExponentDigits = 9,
        Done = 10, ImplicitExponent = 11
    };
    const char* cursor = source;
    const char* rollback = source;
    std::uint8_t digits[25];
    auto* digitEnd = digits;
    std::uint32_t digitCount = 0;
    std::uint16_t sign = 0;
    std::int32_t exponentSign = 1;
    std::int32_t exponent = 0;
    std::int32_t decimalAdjustment = 0;
    bool haveNumber = false;
    bool havePoint = false;
    bool haveExponent = false;
    ParseState state = ParseState::Start;
    const auto readByte = [&cursor]() {
        return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*cursor++));
    };
    const auto isDigit = [](const std::int8_t character) {
        const auto value = static_cast<std::uint8_t>(character);
        if (gLegacyCtypeMaxBytes58FE3C > 1) {
            return ClassifyLegacyCharacter_541955(value, 4) != 0;
        }
        const auto* const types = reinterpret_cast<const std::uint8_t*>(gCtypePointer58FC30);
        return (types[value * 2] & 4u) != 0;
    };
    for (;;) {
        const auto character = static_cast<std::uint8_t>(*cursor);
        if (character != ' ' && character != '\t' && character != '\n' && character != '\r') {
            break;
        }
        ++cursor;
    }
    for (;;) {
        std::int8_t character = readByte();
        switch (state) {
        case ParseState::Start:
            if (character >= '1' && character <= '9') {
                state = ParseState::Integer;
                --cursor;
                continue;
            }
            if (static_cast<std::uint8_t>(character) == gLegacyDecimalPoint58FE40) {
                state = ParseState::RequireFraction;
                continue;
            }
            if (character == '+') {
                sign = 0;
                state = ParseState::AfterSign;
                continue;
            }
            if (character == '-') {
                sign = 0x8000u;
                state = ParseState::AfterSign;
                continue;
            }
            if (character == '0') {
                state = ParseState::LeadingZeros;
                continue;
            }
            --cursor;
            goto finish;
        case ParseState::LeadingZeros:
            haveNumber = true;
            if (character >= '1' && character <= '9') {
                state = ParseState::Integer;
                --cursor;
                continue;
            }
            if (static_cast<std::uint8_t>(character) == gLegacyDecimalPoint58FE40) {
                state = ParseState::Fraction;
                continue;
            }
            if (character == '0') {
                state = ParseState::LeadingZeros;
                continue;
            }
            goto suffix;
        case ParseState::AfterSign:
            if (character >= '1' && character <= '9') {
                state = ParseState::Integer;
                --cursor;
                continue;
            }
            if (static_cast<std::uint8_t>(character) == gLegacyDecimalPoint58FE40) {
                state = ParseState::RequireFraction;
                continue;
            }
            if (character == '0') {
                state = ParseState::LeadingZeros;
                continue;
            }
            cursor = rollback;
            goto finish;
        case ParseState::Integer:
            haveNumber = true;
            while (isDigit(character)) {
                if (digitCount < 25) {
                    ++digitCount;
                    *digitEnd++ = static_cast<std::uint8_t>(character - '0');
                } else {
                    ++decimalAdjustment;
                }
                character = readByte();
            }
            if (static_cast<std::uint8_t>(character) == gLegacyDecimalPoint58FE40) {
                state = ParseState::Fraction;
                continue;
            }
            goto suffix;
        case ParseState::Fraction:
            haveNumber = true;
            havePoint = true;
            if (digitCount == 0) {
                while (character == '0') {
                    --decimalAdjustment;
                    character = readByte();
                }
            }
            while (isDigit(character)) {
                if (digitCount < 25) {
                    ++digitCount;
                    --decimalAdjustment;
                    *digitEnd++ = static_cast<std::uint8_t>(character - '0');
                }
                character = readByte();
            }
            goto suffix;
        case ParseState::RequireFraction:
            havePoint = true;
            if (!isDigit(character)) {
                cursor = rollback;
                goto finish;
            }
            state = ParseState::Fraction;
            --cursor;
            continue;
        case ParseState::ExponentStart:
            rollback = cursor - 2;
            if (character >= '1' && character <= '9') {
                state = ParseState::ExponentDigits;
                --cursor;
                continue;
            }
            if (character == '+') {
                state = ParseState::ExponentSign;
                continue;
            }
            if (character == '-') {
                exponentSign = -1;
                state = ParseState::ExponentSign;
                continue;
            }
            if (character == '0') {
                state = ParseState::ExponentZeros;
                continue;
            }
            cursor = rollback;
            goto finish;
        case ParseState::ExponentSign:
            if (character >= '1' && character <= '9') {
                state = ParseState::ExponentDigits;
                --cursor;
                continue;
            }
            if (character == '0') {
                state = ParseState::ExponentZeros;
                continue;
            }
            cursor = rollback;
            goto finish;
        case ParseState::ExponentZeros:
            haveExponent = true;
            while (character == '0') {
                character = readByte();
            }
            if (character >= '1' && character <= '9') {
                state = ParseState::ExponentDigits;
                --cursor;
                continue;
            }
            --cursor;
            goto finish;
        case ParseState::ExponentDigits:
            haveExponent = true;
            exponent = 0;
            while (isDigit(character)) {
                exponent = character + exponent * 10 - '0';
                if (exponent > 5200) {
                    exponent = 5201;
                    break;
                }
                character = readByte();
            }
            // 原停止累计后再次分类当前字节，再继续消费剩余指数数字。
            while (isDigit(character)) {
                character = readByte();
            }
            --cursor;
            goto finish;
        case ParseState::ImplicitExponent:
            if (allowImplicitExponent == 0) {
                --cursor;
                goto finish;
            }
            rollback = cursor - 1;
            if (character == '+') {
                state = ParseState::ExponentSign;
                continue;
            }
            if (character == '-') {
                exponentSign = -1;
                state = ParseState::ExponentSign;
                continue;
            }
            cursor = rollback;
            goto finish;
        case ParseState::Done:
            goto finish;
        default:
            continue;
        }
    suffix:
        if (character == '+' || character == '-') {
            --cursor;
            state = ParseState::ImplicitExponent;
            continue;
        }
        if ((character > 'C' && character <= 'E') || (character > 'c' && character <= 'e')) {
            state = ParseState::ExponentStart;
            continue;
        }
        --cursor;
        goto finish;
    }
finish:
    *end = cursor;
    std::uint16_t extra;
    std::uint16_t binaryExponent;
    std::uint32_t low;
    std::uint32_t high;
    std::int32_t result = 0;
    if (!haveNumber) {
        extra = 0;
        binaryExponent = 0;
        high = 0;
        low = 0;
        result = 4;
    } else {
        if (digitCount > 24) {
            // 原缓冲+23（var_45），不是第25位；只增加该U8，不传播十进制进位。
            if (std::bit_cast<std::int8_t>(digits[23]) >= 5) {
                ++digits[23];
            }
            digitCount = 24;
            --digitEnd;
            ++decimalAdjustment;
        }
        if (digitCount != 0) {
            while (*--digitEnd == 0) {
                --digitCount;
                ++decimalAdjustment;
            }
            LegacyFloat96 value;
            ConvertLegacyDecimalMantissa_546E6F(digits, digitCount, &value);
            if (exponentSign < 0) {
                exponent = -exponent;
            }
            exponent += decimalAdjustment;
            if (!haveExponent) {
                exponent += exponentAdjustment;
            }
            if (!havePoint) {
                exponent -= fractionDigits;
            }
            if (exponent > 5200) {
                low = 0;
                binaryExponent = 0x7FFFu;
                high = 0x80000000u;
                extra = 0;
                result = 2;
            } else if (exponent < -5200) {
                extra = 0;
                binaryExponent = 0;
                high = 0;
                low = 0;
                result = 1;
            } else {
                ScaleLegacyFloat96_547B91(&value, exponent, preserveExtra);
                std::memcpy(&extra, value.bytes, sizeof(extra));
                std::memcpy(&low, value.bytes + 2, sizeof(low));
                std::memcpy(&high, value.bytes + 6, sizeof(high));
                std::memcpy(&binaryExponent, value.bytes + 10, sizeof(binaryExponent));
            }
        } else {
            extra = 0;
            binaryExponent = 0;
            high = 0;
            low = 0;
        }
    }
    binaryExponent = static_cast<std::uint16_t>(binaryExponent | sign);
    std::memcpy(output->bytes + 6, &high, sizeof(high));
    std::memcpy(output->bytes + 2, &low, sizeof(low));
    std::memcpy(output->bytes + 10, &binaryExponent, sizeof(binaryExponent));
    std::memcpy(output->bytes, &extra, sizeof(extra));
    return result;
}

std::int32_t ParseLegacyDouble_544D51(double* const output, const char* source)
{
    LegacyFloat96 value;
    static_cast<void>(ParseLegacyFloat96_546419(&value, &source, source, 0, 0, 0, 0));
    return ConvertLegacyFloat96ToDouble_544D25(&value, output);
}

std::int32_t ParseLegacySingle_544D7E(float* const output, const char* source)
{
    LegacyFloat96 value;
    static_cast<void>(ParseLegacyFloat96_546419(&value, &source, source, 0, 0, 0, 0));
    return ConvertLegacyFloat96ToSingle_544D3B(&value, output);
}

void AssignLegacyFloatText_53FE95(const std::int32_t useDouble, void* const output,
    const char* const source)
{
    if (useDouble != 0) {
        double value;
        static_cast<void>(ParseLegacyDouble_544D51(&value, source));
        const auto* const bytes = reinterpret_cast<const std::uint8_t*>(&value);
        std::memcpy(output, bytes, 4);
        std::memcpy(static_cast<std::uint8_t*>(output) + 4, bytes + 4, 4);
    } else {
        float value;
        static_cast<void>(ParseLegacySingle_544D7E(&value, source));
        std::memcpy(output, &value, 4);
    }
}

std::int32_t IsLegacyFloatNonnegative_53FE7D(double* const value)
{
    return detail::Cg2dLegacyFloatNonnegative(value);
}

void InstallLegacyFloatCallbacks_53B45F()
{
    gLegacyFloatCropZeros58FB54 = CropLegacyFloatZeros_53FE2F;
    gLegacyFloatFormat58FB50 = ConvertLegacyFloatText_54019A;
    gLegacyFloatAssign58FB58 = AssignLegacyFloatText_53FE95;
    gLegacyFloatForcePoint58FB5C = ForceLegacyDecimalPoint_53FDD5;
    gLegacyFloatNonnegative58FB60 = IsLegacyFloatNonnegative_53FE7D;
    gLegacyFloatFormatAlias58FB64 = ConvertLegacyFloatText_54019A;
}

} // namespace cg::platform
