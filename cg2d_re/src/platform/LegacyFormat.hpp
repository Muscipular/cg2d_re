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

// 原三个浮点输出槽；初值为545031错误出口，转换器安装另有真实启动链。
using LegacyFloatFormatCallback = void (*)(
    double*, char*, std::int32_t, std::int32_t, std::int32_t);
using LegacyFloatTextCallback = void (*)(char*);
extern LegacyFloatFormatCallback gLegacyFloatFormat58FB50;
extern LegacyFloatTextCallback gLegacyFloatCropZeros58FB54;
extern LegacyFloatTextCallback gLegacyFloatForcePoint58FB5C;
[[noreturn]] void LegacyFloatingPointTrap_545031();

// 原核心及三公共包装；原生va_list只承接参数位置，不使用宿主格式引擎。
std::int32_t FormatLegacyOutput_53EB28(
    LegacyFile* stream, const char* format, std::va_list arguments);
std::int32_t FormatLegacyString_53B304(char* buffer, const char* format, ...);
std::int32_t FormatLegacyBoundedString_53D21D(
    char* buffer, std::uint32_t capacity, const char* format, ...);
std::int32_t FormatLegacyStream_53BC0F(LegacyFile* stream, const char* format, ...);

} // namespace cg::platform
