#include "LegacyFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyRuntimeError.hpp"
#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstdio>
#include <cstring>

namespace cg::platform {

namespace {

char* TrapFloatFormat(double*, char*, std::int32_t, std::int32_t, std::int32_t)
{
    LegacyFloatingPointTrap_545031();
}

void TrapFloatText(char*)
{
    LegacyFloatingPointTrap_545031();
}

void TrapFloatAssign(std::int32_t, void*, const char*)
{
    LegacyFloatingPointTrap_545031();
}

std::int32_t TrapFloatNonnegative(double*)
{
    LegacyFloatingPointTrap_545031();
}

} // namespace

const char* gLegacyFormatNullText58FB38 = "(null)";
const wchar_t* gLegacyFormatNullWide58FB3C = L"(null)";
LegacyFloatFormatCallback gLegacyFloatFormat58FB50 = TrapFloatFormat;
LegacyFloatTextCallback gLegacyFloatCropZeros58FB54 = TrapFloatText;
LegacyFloatAssignCallback gLegacyFloatAssign58FB58 = TrapFloatAssign;
LegacyFloatTextCallback gLegacyFloatForcePoint58FB5C = TrapFloatText;
LegacyFloatNonnegativeCallback gLegacyFloatNonnegative58FB60 = TrapFloatNonnegative;
LegacyFloatFormatCallback gLegacyFloatFormatAlias58FB64 = TrapFloatFormat;

[[noreturn]] void LegacyFloatingPointTrap_545031()
{
    ExitLegacyRuntimeError_53DEAB(2);
    __assume(0);
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

std::uint64_t MultiplyLegacyUnsigned64_542F10(
    const std::uint64_t first, const std::uint64_t second)
{
    const auto firstHigh = static_cast<std::uint32_t>(first >> 32);
    const auto secondHigh = static_cast<std::uint32_t>(second >> 32);
    const auto secondLow = static_cast<std::uint32_t>(second);
    const auto firstLow = static_cast<std::uint32_t>(first);
    if ((firstHigh | secondHigh) == 0) {
        return static_cast<std::uint64_t>(firstLow) * secondLow;
    }
    std::uint32_t cross = firstHigh * secondLow;
    cross += firstLow * secondHigh;
    const std::uint64_t lowProduct =
        static_cast<std::uint64_t>(firstLow) * secondLow;
    return lowProduct + (static_cast<std::uint64_t>(cross) << 32);
}

std::uint64_t ShiftLegacyUnsigned64_544620(
    const std::uint64_t value, const std::uint8_t count)
{
    if (count >= 64) {
        return 0;
    }
    if (count >= 32) {
        const auto low = static_cast<std::uint32_t>(value);
        const std::uint32_t high = low << (count & 0x1Fu);
        return static_cast<std::uint64_t>(high) << 32;
    }
    return value << count;
}

std::uint64_t RemainderLegacyUnsigned64_5447A0(
    const std::uint64_t dividend, const std::uint64_t divisor)
{
    return dividend % divisor;
}

std::uint64_t DivideLegacyUnsigned64_53CB10(
    const std::uint64_t dividend, const std::uint64_t divisor)
{
    return dividend / divisor;
}

} // namespace cg::platform
