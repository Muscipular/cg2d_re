#pragma once

#include "LegacyStdio.hpp"

#include <cstdarg>
#include <cstdint>

namespace cg::platform {

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

// 原53CB10；MainLoop真实计时除法，U64参数/返回和现调用保持。
std::uint64_t DivideLegacyUnsigned64_53CB10(
    std::uint64_t dividend, std::uint64_t divisor);

} // namespace cg::platform
