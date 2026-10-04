#pragma once

#include "LegacyStdio.hpp"

#include <cstdarg>
#include <cstdint>

namespace cg::platform {

// 原Z记录仅地址拓宽；长度是S16字节数，remaining02不由输出核心消费。
struct LegacyCountedFormatString final {
    std::int16_t length;
    std::uint16_t remaining02;
    const void* text;
};
static_assert(offsetof(LegacyCountedFormatString, text) == 8);
static_assert(sizeof(LegacyCountedFormatString) == 16);

extern const char* gLegacyFormatNullText58FB38;
extern const wchar_t* gLegacyFormatNullWide58FB3C;

// 原六个浮点槽；初值均为545031，53B45F按原序安装，启动父仍另行接线。
using LegacyFloatFormatCallback = char* (*)(
    double*, char*, std::int32_t, std::int32_t, std::int32_t);
using LegacyFloatTextCallback = void (*)(char*);
using LegacyFloatAssignCallback = void (*)(std::int32_t, void*, const char*);
using LegacyFloatNonnegativeCallback = std::int32_t (*)(double*);
extern LegacyFloatFormatCallback gLegacyFloatFormat58FB50;
extern LegacyFloatTextCallback gLegacyFloatCropZeros58FB54;
extern LegacyFloatAssignCallback gLegacyFloatAssign58FB58;
extern LegacyFloatTextCallback gLegacyFloatForcePoint58FB5C;
extern LegacyFloatNonnegativeCallback gLegacyFloatNonnegative58FB60;
extern LegacyFloatFormatCallback gLegacyFloatFormatAlias58FB64;
[[noreturn]] void LegacyFloatingPointTrap_545031();

// 原核心及三公共包装；原生va_list只承接参数位置，不使用宿主格式引擎。
std::int32_t FormatLegacyOutput_53EB28(
    LegacyFile* stream, const char* format, std::va_list arguments);
std::int32_t FormatLegacyString_53B304(char* buffer, const char* format, ...);
std::int32_t FormatLegacyBoundedString_53D21D(
    char* buffer, std::uint32_t capacity, const char* format, ...);
std::int32_t FormatLegacyStream_53BC0F(LegacyFile* stream, const char* format, ...);


// 原输入核心/字符串包装；目标地址按Win64 va_list传递，数值写入保持原宽度。
std::int32_t ScanLegacyInput_53DEF4(
    LegacyFile* stream, const char* format, std::va_list arguments);
std::int32_t ScanLegacyString_53CADC(const char* buffer, const char* format, ...);

// 原U64两helper，参数/返回各64位而非地址；原生无符号算符，不增除零保护或替代值。
std::uint64_t RemainderLegacyUnsigned64_5447A0(
    std::uint64_t dividend, std::uint64_t divisor);
std::uint64_t DivideLegacyUnsigned64_53CB10(
    std::uint64_t dividend, std::uint64_t divisor);

} // namespace cg::platform
