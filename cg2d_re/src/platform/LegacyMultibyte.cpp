#include "LegacyMultibyte.hpp"

#include "LegacyStringCompareState.hpp"

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

// 原545208：infoType=1；表生产用256/false，541955字符分类用1或2/true。
int GetLegacyCharacterTypes_545208(const char* const source, const std::int32_t sourceCount,
    std::uint16_t* const types, std::uint32_t codePage, std::uint32_t locale,
    const bool strict)
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
        return ::GetStringTypeA(locale, CT_CTYPE1, source, sourceCount, types);
    }
    if (backend != 1) {
        return 0;
    }
    if (codePage == 0) {
        codePage = gConversionCodePage_1086170;
    }
    const int wideCount = ::MultiByteToWideChar(
        codePage, MB_PRECOMPOSED | (strict ? MB_ERR_INVALID_CHARS : 0),
        source, sourceCount, nullptr, 0);
    if (wideCount == 0) {
        return 0;
    }
    wchar_t* wide;
    __try {
        wide = static_cast<wchar_t*>(_alloca(sizeof(wchar_t) * wideCount));
        std::memset(wide, 0, sizeof(wchar_t) * wideCount);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        wide = nullptr;
    }
    if (wide == nullptr) {
        return 0;
    }
    const int converted = ::MultiByteToWideChar(
        codePage, MB_PRECOMPOSED, source, sourceCount, wide, wideCount);
    return converted == 0 ? 0 : ::GetStringTypeW(CT_CTYPE1, wide, converted, types);
}

namespace {

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

// 原543AE8的256项生产域；仅LOWERCASE/UPPERCASE，不进入SORTKEY分支。
int MapLegacyCaseTable_543AE8(const std::uint32_t locale, const DWORD flags,
    const char* const source, char* const destination, std::uint32_t codePage)
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
    int length = 0;
    while (length < 256 && source[length] != '\0') {
        ++length;
    }
    const std::uint32_t backend = gLegacyTextLocale1086160.mapBackend;
    if (backend == 2) {
        return ::LCMapStringA(locale, flags, source, length, destination, 256);
    }
    if (backend != 1) {
        return 0;
    }
    if (codePage == 0) {
        codePage = gConversionCodePage_1086170;
    }
    const int wideCount = ::MultiByteToWideChar(
        codePage, MB_PRECOMPOSED, source, length, nullptr, 0);
    if (wideCount == 0) {
        return 0;
    }
    wchar_t* wide;
    __try {
        wide = static_cast<wchar_t*>(_alloca(sizeof(wchar_t) * wideCount));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        wide = nullptr;
    }
    if (wide == nullptr
        || ::MultiByteToWideChar(codePage, MB_PRECOMPOSED, source, length, wide, wideCount) == 0) {
        return 0;
    }
    const int mappedCount = ::LCMapStringW(locale, flags, wide, wideCount, nullptr, 0);
    if (mappedCount == 0) {
        return 0;
    }
    wchar_t* mapped;
    __try {
        mapped = static_cast<wchar_t*>(_alloca(sizeof(wchar_t) * mappedCount));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        mapped = nullptr;
    }
    if (mapped == nullptr
        || ::LCMapStringW(locale, flags, wide, wideCount, mapped, mappedCount) == 0) {
        return 0;
    }
    return ::WideCharToMultiByte(codePage, WC_COMPOSITECHECK | WC_SEPCHARS,
        mapped, mappedCount, destination, 256, nullptr, nullptr);
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

std::uint32_t UpperLegacyDirectoryByte_543D37(const std::uint8_t value)
{
    if ((gLegacyMultibyte1086764.classification[value + 1] & 0x20u) != 0) {
        return gLegacyMultibyte1086764.caseMap[value];
    }
    return value;
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

} // namespace cg::platform
