#include "LegacyFormat.hpp"

#include <cstdio>

namespace cg::platform {

std::int32_t ScanLegacyInput_53DEF4(
    LegacyFile* const stream, const char* format, std::va_list arguments)
{
    return std::vfscanf(stream, format, arguments);
}

std::int32_t ScanLegacyString_53CADC(const char* const buffer, const char* const format, ...)
{
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = std::vsscanf(buffer, format, arguments);
    va_end(arguments);
    return result;
}

} // namespace cg::platform
