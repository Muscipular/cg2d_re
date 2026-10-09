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

// MC01-B：流输出核心/包装直接vfprintf，全部文件均为实际MSVC FILE。
std::int32_t FormatLegacyOutput_53EB28(
    LegacyFile* stream, const char* format, std::va_list arguments);
// 原4D8DA0@4D8F5B单W32读取；只借对应命令元素地址，经CPP intrinsic取样后给%d。
// 命令仍仅按原分支写入；CPP该处不直接求值未生产整数，不补值/模拟原栈或外推原值等价。
extern "C" std::int32_t Cg2dReadLegacyShopCommandW32(
    const std::int32_t* command) noexcept;

// vsprintf/_vsnprintf：实际返回及容量耗尽行为采用MSVC合同，调用方保证参数匹配。
std::int32_t FormatLegacyString_53B304(char* buffer, const char* format, ...);
std::int32_t FormatLegacyBoundedString_53D21D(
    char* buffer, std::uint32_t capacity, const char* format, ...);
std::int32_t FormatLegacyStream_53BC0F(LegacyFile* stream, const char* format, ...);


// MC01-B：流/字符串扫描直接vfscanf/vsscanf，目标类型由格式串与调用方保证。
std::int32_t ScanLegacyInput_53DEF4(
    LegacyFile* stream, const char* format, std::va_list arguments);
std::int32_t ScanLegacyString_53CADC(const char* buffer, const char* format, ...);

// 原542F10合成乘积低64位，保原高半零快路与交叉W32乘加；不是地址扩宽。
std::uint64_t MultiplyLegacyUnsigned64_542F10(
    std::uint64_t first, std::uint64_t second);
// 原544620只消费CL计数BYTE，保>=64原返回0/32..63低半搬高/0..31双字移位。
std::uint64_t ShiftLegacyUnsigned64_544620(
    std::uint64_t value, std::uint8_t count);

// 原U64两helper，参数/返回各64位而非地址；原生无符号算符，不增除零保护或替代值。
std::uint64_t RemainderLegacyUnsigned64_5447A0(
    std::uint64_t dividend, std::uint64_t divisor);
std::uint64_t DivideLegacyUnsigned64_53CB10(
    std::uint64_t dividend, std::uint64_t divisor);

} // namespace cg::platform
