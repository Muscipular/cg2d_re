#include "LegacyFormat.hpp"

#include <bit>
#include <cstdio>
#include <intrin.h>

#pragma intrinsic(__movsd)

namespace cg::platform {

// 原4D8F5B仅取一个W32；不直接求值未生产命令，也不补默认值。
extern "C" std::int32_t Cg2dReadLegacyShopCommandW32(
    const std::int32_t* const command) noexcept
{
    static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
    unsigned long value;
    __movsd(&value, reinterpret_cast<unsigned long*>(const_cast<std::int32_t*>(command)), 1);
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value));
}

std::int32_t FormatLegacyOutput_53EB28(LegacyFile* const stream,
    const char* format, std::va_list arguments)
{
    return std::vfprintf(stream, format, arguments);
}

std::int32_t FormatLegacyString_53B304(char* const buffer, const char* const format, ...)
{
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = std::vsprintf(buffer, format, arguments);
    va_end(arguments);
    return result;
}

std::int32_t FormatLegacyBoundedString_53D21D(char* const buffer,
    const std::uint32_t capacity, const char* const format, ...)
{
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = ::_vsnprintf(buffer, capacity, format, arguments);
    va_end(arguments);
    return result;
}

std::int32_t FormatLegacyStream_53BC0F(LegacyFile* const stream, const char* const format, ...)
{
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = std::vfprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

std::uint64_t DivideLegacyUnsigned64_53CB10(
    const std::uint64_t dividend, const std::uint64_t divisor)
{
    return dividend / divisor;
}

} // namespace cg::platform
