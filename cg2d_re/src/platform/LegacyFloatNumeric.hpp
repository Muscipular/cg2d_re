#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原扩展数值以固定字节表示，不使用宿主long double布局。
struct LegacyFloat80 final {
    std::uint8_t bytes[10];
};
struct LegacyFloat96 final {
    std::uint8_t bytes[12];
};
static_assert(sizeof(LegacyFloat80) == 10);
static_assert(sizeof(LegacyFloat96) == 12);

// 原正常多字进位；目标4B可能未对齐，因此显式使用字节地址。
std::uint32_t AddLegacyFloatWord_546D95(
    std::uint32_t first, std::uint32_t second, std::uint8_t* output);
std::uint32_t AddLegacyFloat96_546DB6(LegacyFloat96* destination, const LegacyFloat96* source);
LegacyFloat96* ShiftLegacyFloat96Left_546E14(LegacyFloat96* value);
LegacyFloat96* ShiftLegacyFloat96Right_546E42(LegacyFloat96* value);
void MultiplyLegacyFloat96_547971(LegacyFloat96* destination, const LegacyFloat96* source);
void ScaleLegacyFloat96_547B91(
    LegacyFloat96* value, std::int32_t power, std::int32_t preserveExtra);

// 原544F7B按double对象表示转10B扩展值，含原负零归零行为。
void ConvertLegacyDoubleTo80_544F7B(LegacyFloat80* output, const double* source);

// 原I10_OUTPUT结果头及字符区：21个有效数字加舍入位/最终NUL，共22B。
struct LegacyFloatDecimalText final {
    std::int16_t decimal;
    std::int8_t sign;
    std::uint8_t count;
    char digits[22];
};
static_assert(offsetof(LegacyFloatDecimalText, decimal) == 0);
static_assert(offsetof(LegacyFloatDecimalText, sign) == 2);
static_assert(offsetof(LegacyFloatDecimalText, count) == 3);
static_assert(offsetof(LegacyFloatDecimalText, digits) == 4);
static_assert(sizeof(LegacyFloatDecimalText) == 26);

// 原546F36；输入80位值按值消费，输出保留原字段/数字字节发布次序。
std::int32_t GenerateLegacyFloatDecimal_546F36(LegacyFloat80 value,
    std::int32_t precision, std::uint32_t flags, LegacyFloatDecimalText* output);


// 原ld12cvt局部尾数：高/中/低W32顺序，与LegacyFloat96的字节布局不同。
struct LegacyFloatMantissa96 final {
    std::uint8_t bytes[12];
};
static_assert(sizeof(LegacyFloatMantissa96) == 12);

// 原5921C0/5921D8参数；仅数值字段，不随地址拓宽。
struct LegacyFloatBinaryFormat final {
    std::int32_t upperExponent; // 原+0：到达该指数进入上界路径。
    std::int32_t minimumExponent; // 原+4：次正规指数门限。
    std::int32_t precision; // 原+8：传给RoundMan的精度位号。
    std::int32_t exponentBits; // 原+0C：尾数右移及指数拼接位置。
    std::int32_t outputBits; // 原+10：目标32或64位。
    std::int32_t bias; // 原+14：目标指数偏置。
};
static_assert(offsetof(LegacyFloatBinaryFormat, upperExponent) == 0);
static_assert(offsetof(LegacyFloatBinaryFormat, minimumExponent) == 4);
static_assert(offsetof(LegacyFloatBinaryFormat, precision) == 8);
static_assert(offsetof(LegacyFloatBinaryFormat, exponentBits) == 12);
static_assert(offsetof(LegacyFloatBinaryFormat, outputBits) == 16);
static_assert(offsetof(LegacyFloatBinaryFormat, bias) == 20);
static_assert(sizeof(LegacyFloatBinaryFormat) == 24);
extern const LegacyFloatBinaryFormat gLegacyDoubleFormat5921C0;
extern const LegacyFloatBinaryFormat gLegacySingleFormat5921D8;

// 原尾数工具；位号/移位距离为S32，返回原全零标记或最终W32进位。
std::int32_t IsLegacyFloatMantissaTailZero_5449BF(
    const LegacyFloatMantissa96* value, std::int32_t bitIndex);
std::uint32_t IncrementLegacyFloatMantissa_544A08(
    LegacyFloatMantissa96* value, std::int32_t bitIndex);
std::uint32_t RoundLegacyFloatMantissa_544A5E(
    LegacyFloatMantissa96* value, std::int32_t precision);
void CopyLegacyFloatMantissa_544AEA(
    LegacyFloatMantissa96* destination, const LegacyFloatMantissa96* source);
void ClearLegacyFloatMantissa_544B05(LegacyFloatMantissa96* value);
std::int32_t IsLegacyFloatMantissaZero_544B11(const LegacyFloatMantissa96* value);
void ShiftLegacyFloatMantissaRight_544B2C(
    LegacyFloatMantissa96* value, std::int32_t distance);

// 原ld12cvt以字节写出浮点表示，返回0正常、1上界、2下界/次正规状态。
std::int32_t ConvertLegacyFloat96ToBinary_544BB9(const LegacyFloat96* source,
    void* output, const LegacyFloatBinaryFormat* format);
std::int32_t ConvertLegacyFloat96ToDouble_544D25(const LegacyFloat96* source, double* output);
std::int32_t ConvertLegacyFloat96ToSingle_544D3B(const LegacyFloat96* source, float* output);


// 原546E6F；将原数值数字字节（不是ASCII）累加并规范化为12B扩展值。
void ConvertLegacyDecimalMantissa_546E6F(
    const std::uint8_t* digits, std::uint32_t count, LegacyFloat96* output);

} // namespace cg::platform
