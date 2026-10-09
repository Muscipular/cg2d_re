#include "cg/platform/LegacyStringCompare.hpp"

#include "LegacyStringCompareState.hpp"
#include "LegacyMultibyte.hpp"

#include <cstring>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {

LegacyTextLocaleStorage gLegacyTextLocale1086160 {};
std::uint32_t& gConversionCodePage_1086170 =
    gLegacyTextLocale1086160.conversionCodePage;
char gNlsProbeD68A7C = 0;

// 原 58FC38 的 EOF 槽及 ASCII 分类；其余 128 项原字节均为零。
std::array<std::uint16_t, 257> gCtypeTable58FC38 {
    0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0048, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084,
    0x0084, 0x0084, 0x0084, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0082, 0x0082, 0x0082, 0x0082, 0x0082, 0x0082,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0020
};
std::uint16_t* gCtypePointer58FC30 = gCtypeTable58FC38.data() + 1;
static_assert(sizeof(gCtypePointer58FC30) == 8);

// MC01：有界长度由MSVC strnlen提供，不复刻原末端额外读取。
std::uint32_t CountLegacyBytes_543D0C(const char* const source, const std::uint32_t limit)
{
    return static_cast<std::uint32_t>(::strnlen(source, limit));
}

// 53DC64/543A1A调用域：LOWERCASE/UPPERCASE、目的长3、代码页参数0、strict=1。
std::int32_t MapLegacyCase_543AE8(const std::uint32_t locale, const std::uint32_t flags,
    const char* source, const int sourceSize, char* destination)
{
    return MapLegacyString_543AE8(
        locale, flags, source, sourceSize, destination, 3, 0, 1);
}

std::uint16_t ClassifyLegacyByte(const std::uint8_t value, const std::uint16_t mask)
{
    return gCtypePointer58FC30[value] & mask;
}

std::int32_t CompareInsensitive_53CF50(const char* left, const char* right)
{
    return ::_stricmp(left, right);
}

std::int32_t CompareInsensitiveN_54AB30(
    const char* left, const char* right, std::uint32_t count)
{
    return ::_strnicmp(left, right, count);
}

} // namespace cg::platform
