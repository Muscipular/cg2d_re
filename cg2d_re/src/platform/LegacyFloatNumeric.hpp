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

} // namespace cg::platform
