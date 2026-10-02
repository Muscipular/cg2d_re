#pragma once

#include "LegacyFloatNumeric.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原544F17返回的唯一数字结果；只把+0C的字符串指针拓宽。
struct LegacyFloatDigits final {
    std::int32_t sign;
    std::int32_t decimal;
    std::int32_t result;
    const char* digits;
};
static_assert(offsetof(LegacyFloatDigits, digits) == 16);
static_assert(sizeof(LegacyFloatDigits) == 24);

extern LegacyFloatDigits* gLegacyFloatDigits1086048;
extern std::uint8_t gLegacyFloatReuse108604C;
extern std::int32_t gLegacyFloatExponent1086050;
extern std::uint8_t gLegacyFloatCarry1086054;
extern std::uint8_t gLegacyDecimalPoint58FE40;

// 原两份共享结果；生产者覆盖当前内容，不分配或返回私有副本。
extern LegacyFloatDecimalText gLegacyDecimalText10862B0;
extern LegacyFloatDigits gLegacyProducedFloatDigits10862D0;

// 原生产者已接dtold/I10_OUTPUT，仍须按真实启动顺序安装浮点回调。
LegacyFloatDigits* ProduceLegacyFloatDigits_544F17(double value);
void RoundLegacyFloatDigits_544EA0(char* output, std::int32_t precision, LegacyFloatDigits* digits);
void ShiftLegacyFloatText_5401EB(char* text, std::int32_t distance);
void ForceLegacyDecimalPoint_53FDD5(char* text);
void CropLegacyFloatZeros_53FE2F(char* text);

// 原e/f/g及g缓存包装；参数为原S32精度，返回原目标地址。
char* FormatLegacyFloatScientific_53FED3(
    double* value, char* output, std::int32_t precision, std::int32_t uppercase);
char* FormatLegacyFloatFixed_53FFD7(double* value, char* output, std::int32_t precision);
char* FormatLegacyFloatGeneral_5400B5(
    double* value, char* output, std::int32_t precision, std::int32_t uppercase);
char* FormatLegacyFloatScientificCached_540150(
    double* value, char* output, std::int32_t precision, std::int32_t uppercase);
char* FormatLegacyFloatFixedCached_540177(
    double* value, char* output, std::int32_t precision);
char* ConvertLegacyFloatText_54019A(double* value, char* output,
    std::int32_t format, std::int32_t precision, std::int32_t uppercase);


// 原546419；结束指针先发布，后四个参数保留原S32控制/十进制尺度语义。
std::int32_t ParseLegacyFloat96_546419(LegacyFloat96* output, const char** end,
    const char* source, std::int32_t preserveExtra, std::int32_t exponentAdjustment,
    std::int32_t fractionDigits, std::int32_t allowImplicitExponent);
std::int32_t ParseLegacyDouble_544D51(double* output, const char* source);
std::int32_t ParseLegacySingle_544D7E(float* output, const char* source);

// 原输入赋值/符号判断及六槽安装；安装只提供本体，真实启动父另行接线。
void AssignLegacyFloatText_53FE95(std::int32_t useDouble, void* output, const char* source);
std::int32_t IsLegacyFloatNonnegative_53FE7D(double* value);
void InstallLegacyFloatCallbacks_53B45F();

} // namespace cg::platform
