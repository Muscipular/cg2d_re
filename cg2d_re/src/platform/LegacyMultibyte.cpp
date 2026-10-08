#include "LegacyMultibyte.hpp"

#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyStringCompare.hpp"

#include <bit>
#include <cstring>
#include <malloc.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace cg::platform {

static_assert(sizeof(wchar_t) == 2);

// 原58FE3C已加载S32初值1；不是多字节代码页对象108677C的标志。
std::int32_t gLegacyCtypeMaxBytes58FE3C = 1;

LegacyMultibyteStorage gLegacyMultibyte1086764 {};

std::uint32_t gLegacyMultibyteTypeBackend10862E4 = 0;
std::uint32_t gLegacyMultibyteSystemCodePage10862E8 = 0;
std::uint32_t gLegacyMultibyteInitGuard1087AEC = 0;

namespace {

// 原5923F0的五个48B记录；参数按原三个W32保存，不解释为指针。
struct MultibyteCodePageDefinition {
    std::uint32_t codePage;
    std::uint32_t parameters[3];
    std::uint8_t ranges[4][8];
};
static_assert(sizeof(MultibyteCodePageDefinition) == 48);

constexpr MultibyteCodePageDefinition kMultibyteCodePages5923F0[] {
    {932, {0x82798260u, 0x21u, 0x0u},
        {{0xA6, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0xA1, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x81, 0x9F, 0xE0, 0xFC, 0x00, 0x00, 0x00, 0x00},
         {0x40, 0x7E, 0x80, 0xFC, 0x00, 0x00, 0x00, 0x00}}},
    {936, {0xA3DAA3C1u, 0x20u, 0x0u},
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x81, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x40, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}},
    {949, {0xA3DAA3C1u, 0x20u, 0x0u},
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x81, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x41, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}},
    {950, {0xA2E4A2CFu, 0xA2E5001Au, 0x5BA2E8u},
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x81, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x40, 0x7E, 0xA1, 0xFE, 0x00, 0x00, 0x00, 0x00}}},
    {1361, {0xDA5EDA51u, 0xDA5F0020u, 0x32DA6Au},
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
         {0x81, 0xD3, 0xD8, 0xDE, 0xE0, 0xF9, 0x00, 0x00},
         {0x31, 0x7E, 0x81, 0xFE, 0x00, 0x00, 0x00, 0x00}}}
};
constexpr std::uint8_t kMultibyteRangeMasks5923E8[] {1, 2, 4, 8};

} // namespace

// 原545208完整七参数分类；原S32长度/实际infoType及W32申请-填充/SEH见通用分类SPEC。
std::int32_t GetLegacyStringTypes_545208(const std::uint32_t infoType,
    const char* const source, const std::int32_t sourceCount,
    std::uint16_t* const types, std::uint32_t codePage, std::uint32_t locale,
    const std::int32_t strict)
{
    std::uint32_t backend = gLegacyMultibyteTypeBackend10862E4;
    if (backend == 0) {
        WORD probe;
        if (::GetStringTypeW(CT_CTYPE1, L"", 1, &probe) != 0) {
            backend = 1;
        } else {
            if (::GetStringTypeA(0, CT_CTYPE1, &gNlsProbeD68A7C, 1, &probe) == 0) {
                return 0;
            }
            backend = 2;
        }
        gLegacyMultibyteTypeBackend10862E4 = backend;
    }
    if (backend == 2) {
        if (locale == 0) {
            locale = gLegacyTextLocale1086160.ctypeLocale;
        }
        return ::GetStringTypeA(locale, infoType, source, sourceCount, types);
    }
    if (backend != 1) {
        return 0;
    }
    if (codePage == 0) {
        codePage = gConversionCodePage_1086170;
    }
    const int wideCount = ::MultiByteToWideChar(
        codePage, MB_PRECOMPOSED | (strict != 0 ? MB_ERR_INVALID_CHARS : 0),
        source, sourceCount, nullptr, 0);
    if (wideCount == 0) {
        return 0;
    }
    wchar_t* wide;
    __try {
        // 原申请按4B对齐，原填充只接count*2低W32；两种数量不能混用。
        const std::uint32_t wideBytes = static_cast<std::uint32_t>(wideCount) * 2u;
        const std::uint32_t allocationBytes = (wideBytes + 3u) & ~3u;
        wide = static_cast<wchar_t*>(_alloca(allocationBytes));
        static_cast<void>(FillLegacyMemory_5419D0(wide, 0, wideBytes));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        wide = nullptr;
    }
    if (wide == nullptr) {
        return 0;
    }
    const int converted = ::MultiByteToWideChar(
        codePage, MB_PRECOMPOSED, source, sourceCount, wide, wideCount);
    return converted == 0 ? 0 : ::GetStringTypeW(infoType, wide, converted, types);
}

// 原541955/546B29两个info1域转同一完整体，调用参数/返回消费保持。
int GetLegacyCharacterTypes_545208(const char* const source, const std::int32_t sourceCount,
    std::uint16_t* const types, const std::uint32_t codePage, const std::uint32_t locale,
    const bool strict)
{
    return GetLegacyStringTypes_545208(
        CT_CTYPE1, source, sourceCount, types, codePage, locale, strict ? 1 : 0);
}

// 原541955：-1..255直接查表；其余输入按原高字节lead位构造NLS源。
std::uint32_t ClassifyLegacyCharacter_541955(
    const std::int32_t value, const std::uint32_t mask)
{
    if (static_cast<std::uint32_t>(value) + 1u <= 256u) {
        return gCtypePointer58FC30[value] & mask;
    }
    const auto highByte = static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(value) >> 8);
    const auto lowByte = static_cast<std::uint8_t>(value);
    char source[3];
    int sourceSize;
    if ((gCtypePointer58FC30[highByte] & 0x8000u) != 0) {
        source[0] = std::bit_cast<char>(highByte);
        source[1] = std::bit_cast<char>(lowByte);
        source[2] = '\0';
        sourceSize = 2;
    } else {
        source[0] = std::bit_cast<char>(lowByte);
        source[1] = '\0';
        sourceSize = 1;
    }
    // 原lpCharType指向参数C的高W16，随后紧邻mask；保留两个输出时的别名。
    std::uint16_t types[3];
    types[0] = static_cast<std::uint16_t>(static_cast<std::uint32_t>(value) >> 16);
    types[1] = static_cast<std::uint16_t>(mask);
    types[2] = static_cast<std::uint16_t>(mask >> 16);
    if (GetLegacyCharacterTypes_545208(source, sourceSize, types, 0, 0, true) == 0) {
        return 0;
    }
    const std::uint32_t resultingMask = types[1]
        | (static_cast<std::uint32_t>(types[2]) << 16);
    return types[0] & resultingMask;
}

namespace {

// 原546CDB；mb分类和固定ctype表独立，后者不能改读可变58FC30指针。
std::int32_t ClassifyMultibyteType_546CDB(const std::uint32_t character,
    const std::uint32_t ctypeMask, const std::uint32_t multibyteMask)
{
    const auto byte = static_cast<std::uint8_t>(character);
    if ((gLegacyMultibyte1086764.classification[byte + 1]
            & static_cast<std::uint8_t>(multibyteMask)) != 0) {
        return 1;
    }
    if (ctypeMask != 0 && (gCtypeTable58FC38[byte + 1] & ctypeMask) != 0) {
        return 1;
    }
    return 0;
}

// 原543AE8完整通用体；八参数/两SEH/SORTKEY与普通字节回转见通用NLS映射SPEC。
std::int32_t MapLegacyString_543AE8(const std::uint32_t locale,
    const std::uint32_t flags, const char* const source, std::int32_t sourceCount,
    char* const destination, const std::int32_t destinationCount,
    std::uint32_t codePage, const std::int32_t strict)
{
    if (gLegacyTextLocale1086160.mapBackend == 0) {
        if (::LCMapStringW(0, LCMAP_LOWERCASE, L"", 1, nullptr, 0) != 0) {
            gLegacyTextLocale1086160.mapBackend = 1;
        } else {
            if (::LCMapStringA(0, LCMAP_LOWERCASE, &gNlsProbeD68A7C, 1, nullptr, 0) == 0) {
                return 0;
            }
            gLegacyTextLocale1086160.mapBackend = 2;
        }
    }
    if (sourceCount > 0) {
        sourceCount = static_cast<std::int32_t>(CountLegacyBytes_543D0C(
            source, static_cast<std::uint32_t>(sourceCount)));
    }
    const std::uint32_t backend = gLegacyTextLocale1086160.mapBackend;
    if (backend == 2) {
        return ::LCMapStringA(locale, flags, source, sourceCount,
            destination, destinationCount);
    }
    if (backend != 1) {
        return 0;
    }
    if (codePage == 0) {
        codePage = gConversionCodePage_1086170;
    }
    const int wideCount = ::MultiByteToWideChar(codePage,
        MB_PRECOMPOSED | (strict != 0 ? MB_ERR_INVALID_CHARS : 0),
        source, sourceCount, nullptr, 0);
    if (wideCount == 0) {
        return 0;
    }
    // 原请求在W32内计算；本机栈承载不模拟旧ESP或扩大数字字段。
    const std::uint32_t wideBytes = (static_cast<std::uint32_t>(wideCount) * 2u + 3u) & ~3u;
    wchar_t* wide;
    __try {
        wide = static_cast<wchar_t*>(_alloca(wideBytes));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        wide = nullptr;
    }
    if (wide == nullptr
        || ::MultiByteToWideChar(codePage, MB_PRECOMPOSED,
            source, sourceCount, wide, wideCount) == 0) {
        return 0;
    }
    const int mappedCount = ::LCMapStringW(locale, flags, wide, wideCount, nullptr, 0);
    if (mappedCount == 0) {
        return 0;
    }
    if ((flags & LCMAP_SORTKEY) != 0) {
        if (destinationCount == 0) {
            return mappedCount;
        }
        if (mappedCount > destinationCount) {
            return 0;
        }
        if (::LCMapStringW(locale, flags, wide, wideCount,
                reinterpret_cast<wchar_t*>(destination), destinationCount) == 0) {
            return 0;
        }
        return mappedCount;
    }
    const std::uint32_t mappedBytes =
        (static_cast<std::uint32_t>(mappedCount) * 2u + 3u) & ~3u;
    wchar_t* mapped;
    __try {
        mapped = static_cast<wchar_t*>(_alloca(mappedBytes));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        mapped = nullptr;
    }
    if (mapped == nullptr
        || ::LCMapStringW(locale, flags, wide, wideCount, mapped, mappedCount) == 0) {
        return 0;
    }
    return ::WideCharToMultiByte(codePage, WC_COMPOSITECHECK | WC_SEPCHARS,
        mapped, mappedCount, destinationCount == 0 ? nullptr : destination,
        destinationCount == 0 ? 0 : destinationCount, nullptr, nullptr);
}

// 原546B29的两固定生产点；数量256、显式CP/strict0实际交完整543AE8。
int MapLegacyCaseTable_543AE8(const std::uint32_t locale, const DWORD flags,
    const char* const source, char* const destination, const std::uint32_t codePage)
{
    return MapLegacyString_543AE8(
        locale, flags, source, 256, destination, 256, codePage, 0);
}

} // namespace


// 原543A1A：保留S32比较、分类门禁、共享NLS状态及失败时返回原值。
std::int32_t UpperLegacyCharacter_543A1A(const std::int32_t value)
{
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        return value >= 'a' && value <= 'z' ? value - 32 : value;
    }
    if (value < 256) {
        const std::uint32_t lowerCase = gLegacyCtypeMaxBytes58FE3C > 1
            ? ClassifyLegacyCharacter_541955(value, 2)
            : gCtypePointer58FC30[value] & 2u;
        if (lowerCase == 0) {
            return value;
        }
    }
    const auto highByte = static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(value) >> 8);
    const auto lowByte = static_cast<std::uint8_t>(value);
    char source[3];
    int sourceSize;
    if ((gCtypePointer58FC30[highByte] & 0x8000u) != 0) {
        source[0] = std::bit_cast<char>(highByte);
        source[1] = std::bit_cast<char>(lowByte);
        source[2] = '\0';
        sourceSize = 2;
    } else {
        source[0] = std::bit_cast<char>(lowByte);
        source[1] = '\0';
        sourceSize = 1;
    }
    char destination[4];
    const int count = MapLegacyCase_543AE8(
        gLegacyTextLocale1086160.ctypeLocale, LCMAP_UPPERCASE,
        source, sourceSize, destination);
    if (count == 0) {
        return value;
    }
    const std::uint32_t first = static_cast<std::uint8_t>(destination[0]);
    return static_cast<std::int32_t>(count == 1 ? first
        : first | (static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(destination[1])) << 8));
}

std::int32_t LowerLegacyCharacter_53DC64(const std::int32_t value)
{
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        return value >= 'A' && value <= 'Z' ? value + 32 : value;
    }
    if (value < 256) {
        const std::uint32_t upperCase = gLegacyCtypeMaxBytes58FE3C > 1
            ? ClassifyLegacyCharacter_541955(value, 1)
            : gCtypePointer58FC30[value] & 1u;
        if (upperCase == 0) {
            return value;
        }
    }
    const auto highByte = static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(value) >> 8);
    const auto lowByte = static_cast<std::uint8_t>(value);
    char source[3];
    int sourceSize;
    if ((gCtypePointer58FC30[highByte] & 0x8000u) != 0) {
        source[0] = std::bit_cast<char>(highByte);
        source[1] = std::bit_cast<char>(lowByte);
        source[2] = '\0';
        sourceSize = 2;
    } else {
        source[0] = std::bit_cast<char>(lowByte);
        source[1] = '\0';
        sourceSize = 1;
    }
    char destination[4];
    const int count = MapLegacyCase_543AE8(
        gLegacyTextLocale1086160.ctypeLocale, LCMAP_LOWERCASE,
        source, sourceSize, destination);
    if (count == 0) {
        return value;
    }
    const std::uint32_t first = static_cast<std::uint8_t>(destination[0]);
    return static_cast<std::int32_t>(count == 1 ? first
        : first | (static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(destination[1])) << 8));
}

std::uint32_t MultibyteCodePageToLocale_546ACD(const std::uint32_t codePage)
{
    switch (codePage) {
    case 932:
        return 1041;
    case 936:
        return 2052;
    case 949:
        return 1042;
    case 950:
        return 1028;
    default:
        return 0;
    }
}

std::int32_t ResetLegacySingleByte_546B00()
{
    gLegacyMultibyte1086764.classification.fill(0);
    gLegacyMultibyte1086764.codePage = 0;
    gLegacyMultibyte1086764.isMultibyte = 0;
    gLegacyMultibyte1086764.locale = 0;
    gLegacyMultibyte1086764.codePageParameters[0] = 0;
    gLegacyMultibyte1086764.codePageParameters[1] = 0;
    gLegacyMultibyte1086764.codePageParameters[2] = 0;
    return 0;
}

// 原543D37完整W32输入；U8表分支与原高低BYTE/lead-NLS和非零后双BYTE实读保持。
std::uint32_t UpperLegacyMultibyteCharacter_543D37(const std::uint32_t value)
{
    if (value <= 0xFFu) {
        if ((gLegacyMultibyte1086764.classification[value + 1] & 0x20u) != 0) {
            return gLegacyMultibyte1086764.caseMap[value];
        }
        return value;
    }
    const auto highByte = static_cast<std::uint8_t>(value >> 8);
    const auto lowByte = static_cast<std::uint8_t>(value);
    if ((gLegacyMultibyte1086764.classification[highByte + 1] & 4u) == 0) {
        return value;
    }
    const char source[2] {std::bit_cast<char>(highByte), std::bit_cast<char>(lowByte)};
    char destination[4];
    if (MapLegacyString_543AE8(gLegacyMultibyte1086764.locale,
            LCMAP_UPPERCASE, source, 2, destination, 2,
            gLegacyMultibyte1086764.codePage, 1) == 0) {
        return value;
    }
    return detail::Cg2dReadLegacyMappedPair(destination);
}

std::uint32_t UpperLegacyDirectoryByte_543D37(const std::uint8_t value)
{
    return UpperLegacyMultibyteCharacter_543D37(value);
}

std::uint32_t ResolveLegacyMultibyteCodePage_546A83(const std::uint32_t requested)
{
    gLegacyMultibyteSystemCodePage10862E8 = 0;
    if (requested == 0xFFFFFFFEu) {
        gLegacyMultibyteSystemCodePage10862E8 = 1;
        return ::GetOEMCP();
    }
    if (requested == 0xFFFFFFFDu) {
        gLegacyMultibyteSystemCodePage10862E8 = 1;
        return ::GetACP();
    }
    if (requested == 0xFFFFFFFCu) {
        const std::uint32_t codePage = gConversionCodePage_1086170;
        gLegacyMultibyteSystemCodePage10862E8 = 1;
        return codePage;
    }
    return requested;
}

std::uint32_t RefreshLegacySingleByteCase_546B29()
{
    CPINFO info;
    if (::GetCPInfo(gLegacyMultibyte1086764.codePage, &info) == TRUE) {
        char source[256];
        for (std::uint32_t index = 0; index < 256; ++index) {
            source[index] = std::bit_cast<char>(static_cast<std::uint8_t>(index));
        }
        source[0] = ' ';
        for (const BYTE* range = info.LeadByte; range[0] != 0; range += 2) {
            for (std::uint32_t value = range[0]; value <= range[1]; ++value) {
                source[value] = ' ';
            }
        }

        // 原三个输出不预清零，调用返回均忽略；失败未写内容不声称旧栈等价。
        std::uint16_t types[256];
        char lower[256];
        char upper[256];
        static_cast<void>(GetLegacyCharacterTypes_545208(source, 256, types,
            gLegacyMultibyte1086764.codePage, gLegacyMultibyte1086764.locale, false));
        static_cast<void>(MapLegacyCaseTable_543AE8(gLegacyMultibyte1086764.locale,
            LCMAP_LOWERCASE, source, lower, gLegacyMultibyte1086764.codePage));
        static_cast<void>(MapLegacyCaseTable_543AE8(gLegacyMultibyte1086764.locale,
            LCMAP_UPPERCASE, source, upper, gLegacyMultibyte1086764.codePage));
        for (std::uint32_t index = 0; index < 256; ++index) {
            if ((types[index] & 1u) != 0) {
                gLegacyMultibyte1086764.classification[index + 1] |= 0x10u;
                gLegacyMultibyte1086764.caseMap[index] = static_cast<std::uint8_t>(lower[index]);
            } else if ((types[index] & 2u) != 0) {
                gLegacyMultibyte1086764.classification[index + 1] |= 0x20u;
                gLegacyMultibyte1086764.caseMap[index] = static_cast<std::uint8_t>(upper[index]);
            } else {
                gLegacyMultibyte1086764.caseMap[index] = 0;
            }
        }
        return 256;
    }

    for (std::uint32_t index = 0; index < 256; ++index) {
        if (index >= 'A' && index <= 'Z') {
            gLegacyMultibyte1086764.classification[index + 1] |= 0x10u;
            gLegacyMultibyte1086764.caseMap[index] = static_cast<std::uint8_t>(index + 32);
        } else if (index >= 'a' && index <= 'z') {
            gLegacyMultibyte1086764.classification[index + 1] |= 0x20u;
            gLegacyMultibyte1086764.caseMap[index] = static_cast<std::uint8_t>(index - 32);
        } else {
            gLegacyMultibyte1086764.caseMap[index] = 0;
        }
    }
    return 256;
}

std::int32_t SetLegacyMultibyteCodePage_5468EA(const std::uint32_t requested)
{
    const std::uint32_t codePage = ResolveLegacyMultibyteCodePage_546A83(requested);
    if (codePage == gLegacyMultibyte1086764.codePage) {
        return 0;
    }
    if (codePage == 0) {
        static_cast<void>(ResetLegacySingleByte_546B00());
    } else {
        const MultibyteCodePageDefinition* definition = nullptr;
        for (const auto& candidate : kMultibyteCodePages5923F0) {
            if (candidate.codePage == codePage) {
                definition = &candidate;
                break;
            }
        }
        if (definition != nullptr) {
            gLegacyMultibyte1086764.classification.fill(0);
            for (std::uint32_t kind = 0; kind < 4; ++kind) {
                for (const std::uint8_t* range = definition->ranges[kind];
                     range[0] != 0; range += 2) {
                    if (range[1] == 0) {
                        break;
                    }
                    for (std::uint32_t value = range[0]; value <= range[1]; ++value) {
                        gLegacyMultibyte1086764.classification[value + 1]
                            |= kMultibyteRangeMasks5923E8[kind];
                    }
                }
            }
            gLegacyMultibyte1086764.isMultibyte = 1;
            gLegacyMultibyte1086764.codePage = codePage;
            const std::uint32_t locale = MultibyteCodePageToLocale_546ACD(codePage);
            gLegacyMultibyte1086764.codePageParameters[0] = definition->parameters[0];
            gLegacyMultibyte1086764.codePageParameters[1] = definition->parameters[1];
            gLegacyMultibyte1086764.locale = locale;
            gLegacyMultibyte1086764.codePageParameters[2] = definition->parameters[2];
        } else {
            CPINFO info;
            if (::GetCPInfo(codePage, &info) != TRUE) {
                if (gLegacyMultibyteSystemCodePage10862E8 == 0) {
                    return -1;
                }
                static_cast<void>(ResetLegacySingleByte_546B00());
            } else {
                gLegacyMultibyte1086764.codePage = codePage;
                gLegacyMultibyte1086764.classification.fill(0);
                gLegacyMultibyte1086764.locale = 0;
                if (info.MaxCharSize <= 1) {
                    gLegacyMultibyte1086764.isMultibyte = 0;
                } else {
                    for (const BYTE* range = info.LeadByte; range[0] != 0; range += 2) {
                        if (range[1] == 0) {
                            break;
                        }
                        for (std::uint32_t value = range[0]; value <= range[1]; ++value) {
                            gLegacyMultibyte1086764.classification[value + 1] |= 4u;
                        }
                    }
                    for (std::uint32_t value = 1; value < 255; ++value) {
                        gLegacyMultibyte1086764.classification[value + 1] |= 8u;
                    }
                    gLegacyMultibyte1086764.locale = MultibyteCodePageToLocale_546ACD(codePage);
                    gLegacyMultibyte1086764.isMultibyte = 1;
                }
                gLegacyMultibyte1086764.codePageParameters[0] = 0;
                gLegacyMultibyte1086764.codePageParameters[1] = 0;
                gLegacyMultibyte1086764.codePageParameters[2] = 0;
            }
        }
    }
    static_cast<void>(RefreshLegacySingleByteCase_546B29());
    return 0;
}

void InitializeLegacyMultibyte_546CAE()
{
    if (gLegacyMultibyteInitGuard1087AEC == 0) {
        static_cast<void>(SetLegacyMultibyteCodePage_5468EA(0xFFFFFFFDu));
        gLegacyMultibyteInitGuard1087AEC = 1;
    }
}

// 原544552；保留lead分支中先有符号、后无符号的两个数量判断。
std::int32_t Mbtowc_544552(wchar_t* const destination, const char* const source,
    const std::uint32_t sourceCount)
{
    if (source == nullptr || sourceCount == 0) {
        return 0;
    }
    const auto first = static_cast<std::uint8_t>(source[0]);
    if (first == 0) {
        if (destination != nullptr) {
            *destination = 0;
        }
        return 0;
    }
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        if (destination != nullptr) {
            *destination = static_cast<wchar_t>(first);
        }
        return 1;
    }
    if ((gCtypePointer58FC30[first] & 0x8000u) != 0) {
        std::int32_t width = gLegacyCtypeMaxBytes58FE3C;
        if (width > 1) {
            if (std::bit_cast<std::int32_t>(sourceCount) < width) {
                gLegacyErrno_1085FE0 = 42;
                return -1;
            }
            const int converted = ::MultiByteToWideChar(gConversionCodePage_1086170,
                MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, source, width,
                destination, destination != nullptr ? 1 : 0);
            width = gLegacyCtypeMaxBytes58FE3C;
            if (converted != 0) {
                return width;
            }
        }
        if (sourceCount >= static_cast<std::uint32_t>(width) && source[1] != '\0') {
            return width;
        }
    } else if (::MultiByteToWideChar(gConversionCodePage_1086170,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, source, 1,
            destination, destination != nullptr ? 1 : 0) != 0) {
        return 1;
    }
    gLegacyErrno_1085FE0 = 42;
    return -1;
}

// 原54472B；真实API写入和默认字符标志均保留，不在失败时清目的区。
std::int32_t Wctomb_54472B(char* const destination, const std::uint16_t character)
{
    if (destination == nullptr) {
        return 0;
    }
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        if (character <= 0xFFu) {
            destination[0] = std::bit_cast<char>(static_cast<std::uint8_t>(character));
            return 1;
        }
    } else {
        const wchar_t wideCharacter = static_cast<wchar_t>(character);
        BOOL usedDefault = FALSE;
        const int converted = ::WideCharToMultiByte(gConversionCodePage_1086170,
            WC_COMPOSITECHECK | WC_SEPCHARS, &wideCharacter, 1, destination,
            gLegacyCtypeMaxBytes58FE3C, nullptr, &usedDefault);
        if (converted != 0 && usedDefault == FALSE) {
            return converted;
        }
    }
    gLegacyErrno_1085FE0 = 42;
    return -1;
}

// 原54A9C3；state按W32判断，低U8续接写和整个W32清理分别执行。
std::int32_t Mbrtowc_54A9C3(wchar_t* const destination, const char* const source,
    const std::uint32_t sourceCount, std::uint32_t* const state,
    const LegacyConversionLocale54A9C3* const locale)
{
    if (source == nullptr || sourceCount == 0) {
        return 0;
    }
    const auto first = static_cast<std::uint8_t>(source[0]);
    if (first == 0) {
        if (destination != nullptr) {
            *destination = 0;
        }
        return 0;
    }
    std::uint32_t ctypeLocale;
    std::uint32_t codePage;
    if (locale == nullptr) {
        ctypeLocale = gLegacyTextLocale1086160.ctypeLocale;
        codePage = gConversionCodePage_1086170;
    } else {
        ctypeLocale = locale->ctypeLocale;
        codePage = locale->codePage;
    }
    if (ctypeLocale == 0) {
        if (destination != nullptr) {
            *destination = static_cast<wchar_t>(first);
        }
        return 1;
    }
    if (*state != 0) {
        reinterpret_cast<unsigned char*>(state)[1] = first;
        if (gLegacyCtypeMaxBytes58FE3C > 1
            && ::MultiByteToWideChar(codePage, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                reinterpret_cast<const char*>(state), 2, destination,
                destination != nullptr ? 1 : 0) != 0) {
            *state = 0;
            return gLegacyCtypeMaxBytes58FE3C;
        }
        *state = 0;
        gLegacyErrno_1085FE0 = 42;
        return -1;
    }
    if ((gCtypePointer58FC30[first] & 0x8000u) != 0) {
        const std::int32_t width = gLegacyCtypeMaxBytes58FE3C;
        if (sourceCount < static_cast<std::uint32_t>(width)) {
            reinterpret_cast<unsigned char*>(state)[0] = first;
            return -2;
        }
        if (width > 1
            && ::MultiByteToWideChar(codePage, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                source, width, destination, destination != nullptr ? 1 : 0) != 0) {
            return gLegacyCtypeMaxBytes58FE3C;
        }
        if (source[1] != '\0') {
            return gLegacyCtypeMaxBytes58FE3C;
        }
        *state = 0;
    } else if (::MultiByteToWideChar(codePage, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            source, 1, destination, destination != nullptr ? 1 : 0) != 0) {
        return 1;
    }
    gLegacyErrno_1085FE0 = 42;
    return -1;
}

// 原54AAD5；只初始化原W32状态，输出沿被调函数真实写入。
std::uint16_t Btowc_54AAD5(const std::int32_t character)
{
    if (character == -1) {
        return 0xFFFFu;
    }
    const char byte = std::bit_cast<char>(static_cast<std::uint8_t>(character));
    std::uint32_t state = 0;
    wchar_t converted;
    const std::int32_t result = Mbrtowc_54A9C3(&converted, &byte, 1, &state, nullptr);
    return result < 0 ? 0xFFFFu : static_cast<std::uint16_t>(converted);
}

// 原53C9DD：整串转换；数量/返回保持U32，目标可空，不使用宿主locale。
std::uint32_t Mbstowcs_53C9DD(wchar_t* const destination, const char* const source,
    const std::uint32_t destinationCount)
{
    if (destination == nullptr) {
        if (gLegacyTextLocale1086160.ctypeLocale == 0) {
            return StrLen_53F9E0(source);
        }
        const std::int32_t converted = ::MultiByteToWideChar(
            gConversionCodePage_1086170, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            source, -1, nullptr, 0);
        if (converted != 0) {
            return static_cast<std::uint32_t>(converted - 1);
        }
        gLegacyErrno_1085FE0 = 42;
        return 0xFFFFFFFFu;
    }
    if (destinationCount == 0) {
        return 0;
    }
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        std::uint32_t completed = 0;
        wchar_t* target = destination;
        do {
            *target = static_cast<std::uint8_t>(source[completed]);
            // 原53CA19在W16写入之后重读同一源字节，保源/目标可能的别名。
            if (source[completed] == '\0') {
                break;
            }
            ++completed;
            ++target;
        } while (completed < destinationCount);
        return completed;
    }

    const std::int32_t converted = ::MultiByteToWideChar(
        gConversionCodePage_1086170, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
        source, -1, destination, std::bit_cast<std::int32_t>(destinationCount));
    if (converted != 0) {
        return static_cast<std::uint32_t>(converted - 1);
    }
    if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        gLegacyErrno_1085FE0 = 42;
        return 0xFFFFFFFFu;
    }

    const char* cursor = source;
    std::uint32_t remaining = destinationCount - 1u;
    for (;;) {
        const std::uint8_t current = static_cast<std::uint8_t>(*cursor);
        if (current == 0) {
            break;
        }
        if ((gCtypePointer58FC30[current] & 0x8000u) != 0) {
            ++cursor;
        }
        ++cursor;
        const std::uint32_t previous = remaining;
        --remaining;
        if (previous == 0) {
            break;
        }
    }
    // 原53CA93以W32字节差传API，截取转换的结果不再减去NUL。
    const std::uint32_t sourceBytes = static_cast<std::uint32_t>(cursor - source);
    const std::int32_t prefixConverted = ::MultiByteToWideChar(
        gConversionCodePage_1086170, MB_PRECOMPOSED, source,
        std::bit_cast<std::int32_t>(sourceBytes), destination,
        std::bit_cast<std::int32_t>(destinationCount));
    if (prefixConverted != 0) {
        return static_cast<std::uint32_t>(prefixConverted);
    }
    gLegacyErrno_1085FE0 = 42;
    return 0xFFFFFFFFu;
}

// 原53BDEF；完整入口保留共享最大宽度门禁，0..255/EOF域沿同一分类表。
std::int32_t ClassifyLegacySpace_53BDEF(const std::int32_t character)
{
    if (gLegacyCtypeMaxBytes58FE3C > 1) {
        return static_cast<std::int32_t>(ClassifyLegacyCharacter_541955(character, 8));
    }
    return gCtypePointer58FC30[character] & 8;
}

// 原53BE17；控制字符mask20H，在输入校验中位于空白分类之后。
std::int32_t ClassifyLegacyControl_53BE17(const std::int32_t character)
{
    if (gLegacyCtypeMaxBytes58FE3C > 1) {
        return static_cast<std::int32_t>(ClassifyLegacyCharacter_541955(character, 0x20));
    }
    return gCtypePointer58FC30[character] & 0x20;
}

// 原53E919；仅数字类别保持输入，其余照原位运算，不替caller减去字符0。
std::int32_t NormalizeLegacyHexCharacter_53E919(const std::int32_t character)
{
    const std::uint32_t digit = gLegacyCtypeMaxBytes58FE3C > 1
        ? ClassifyLegacyCharacter_541955(character, 4)
        : gCtypePointer58FC30[character] & 4u;
    if (digit != 0) {
        return character;
    }
    return std::bit_cast<std::int32_t>(
        (static_cast<std::uint32_t>(character) & 0xFFFFFFDFu) - 7u);
}

// 原546CCA只有546CDB委托；保持固定mask及0/1返回。
std::int32_t IsLegacyMultibyteLead_546CCA(const std::uint32_t character)
{
    return ClassifyMultibyteType_546CDB(character, 0, 4);
}

} // namespace cg::platform
