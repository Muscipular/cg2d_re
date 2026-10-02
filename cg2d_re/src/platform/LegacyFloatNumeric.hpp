#pragma once

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

} // namespace cg::platform
