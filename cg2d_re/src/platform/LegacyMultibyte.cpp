#include "LegacyMultibyte.hpp"

namespace cg::platform {

LegacyMultibyteStorage gLegacyMultibyte1086764 {};

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

} // namespace cg::platform
