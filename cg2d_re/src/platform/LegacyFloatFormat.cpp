#include "LegacyFloatFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyStringCompareState.hpp"

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
        const auto count = static_cast<std::uint32_t>(std::strlen(text)) + 1u;
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
        const auto count = static_cast<std::uint32_t>(std::strlen(output + 1)) + 1u;
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
    std::strcpy(cursor, "e+000");
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

} // namespace cg::platform
