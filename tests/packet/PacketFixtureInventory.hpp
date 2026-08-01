#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace cg::packet::test {

enum class FixturePayloadAuditKind
{
    StructuredPayload,
    NoStructuredPayload,
};

struct FixturePayloadAudit
{
    std::string_view code;
    FixturePayloadAuditKind kind = FixturePayloadAuditKind::NoStructuredPayload;
    std::size_t fieldIndex = 0U;
};

inline constexpr std::array<std::string_view, 91U> HistoricalTypedBaselineCodes = {{
    "JFVf", "Gp", "CCL", "aSEl", "UUN", "Ot", "RXr", "Flvp", "kjSK", "HJQp", "ycX",
    "CV", "uSr", "xD", "IcE", "rMsv", "PfqV", "woz", "Ufm", "GY", "uId", "wD", "GH",
    "aiAb", "ylGN",
    "whV", "Daj", "DZ", "oy", "cJzI", "HNP", "aPc", "UU", "Sc", "eZT", "IUn", "DpSY",
    "cNB", "ow", "KyN", "TNT", "JJQj", "Rspn", "cNE", "rU", "iKe", "TgX", "ELoy",
    "xmgQ", "Ilz", "dM", "Dn", "lDuJ", "IsSy", "AU", "JgO", "OBee", "Yo", "ysVc", "LY",
    "ND", "nFV", "DyAG", "Zd", "zzd", "Gn", "hB", "LYIm", "Mhbt", "zdr", "BZ", "ft",
    "jv", "TeD", "gnxd", "vI", "aDn", "znv", "yuD", "Kka", "Rll", "Cxo", "ms", "nUbJ",
    "RwK", "Yq", "PhdQ", "nsY", "NI", "uSB", "aBS",
}};

inline constexpr std::array<std::string_view, 34U> FixtureDebtCodes = {{
    "aiAb",
    "JJQj",
    "LY",
    "ND", "nFV", "DyAG", "Zd", "zzd", "Gn", "hB", "LYIm", "Mhbt", "zdr", "BZ", "ft",
    "jv", "TeD", "gnxd", "vI", "aDn", "znv", "yuD", "Kka", "Rll", "Cxo", "ms", "nUbJ",
    "RwK", "Yq", "PhdQ", "nsY", "NI", "uSB", "aBS",
}};

inline constexpr std::array<FixturePayloadAudit, 172U> FixturePayloadAudits = {{
    {"WA", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"PfM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"emRV", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"lz", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Or", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"DE", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"pg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"KeOX", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ik", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"eL", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"EZ", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"dAgg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Rg", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"as", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ixH", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"QeuC", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"OU", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"zGp", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"tj", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ACwB", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"IHw", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Noeb", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Godf", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"rvb", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Xoee", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ed", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Tmrx", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"KXhH", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Yx", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"kiG", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"QpfE", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"PGXE", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ElVN", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"wAo", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"IPy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"rh", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Cpa", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"SWITM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"SWITMP", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"SWITMP2", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"oRut", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"frQp", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"bJg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"PVfz", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"murD", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Bi", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ftaN", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"suI", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ih", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"lO", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"JMh", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"hPUd", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"PudO", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"GmNM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"OaHi", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"SkBD", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"sZWu", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"bNY", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"NdM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"hJq", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"xEg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"BHe", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"kg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"sA", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"TFCu", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"wiB", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"jmb", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"MTg", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Ov", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"lSZ", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"tq", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"HMNy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"LOh", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"QBt", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"zA", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"cZt", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"We", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"nkda", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"SWITMM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"GC", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"fIM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"NgMy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"IMv", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"cEj", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"FvWm", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Ak", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"iVfo", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"yi", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"fS", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"LaR", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"FPkR", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"vcAz", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"CV", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"nB", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"rMsv", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"PfqV", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"woz", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"CZ", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Ufm", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"uHb", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"npzi", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"XIsR", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"dbQC", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"HoW", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"TLKH", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"dcMv", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"FVm", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"vWpn", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"IYo", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"zn", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"LKQy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Ukuy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"iET", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"McVa", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"mjCv", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"BG", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"MIS", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"klF", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"sM", FixturePayloadAuditKind::StructuredPayload, 3U},
    {"kza", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"JFVf", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Gp", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"CCL", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"aSEl", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"UUN", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Ot", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"RXr", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Flvp", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"kjSK", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"HJQp", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ycX", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"uSr", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"xD", FixturePayloadAuditKind::StructuredPayload, 5U},
    {"IcE", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"GY", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"uId", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"wD", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"GH", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"ylGN", FixturePayloadAuditKind::StructuredPayload, 2U},
    {"whV", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Daj", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"DZ", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"oy", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"cJzI", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"HNP", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"aPc", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"UU", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"Sc", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"eZT", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"IUn", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"DpSY", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"cNB", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"ow", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"KyN", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"TNT", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"Rspn", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"cNE", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"rU", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"iKe", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"TgX", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"ELoy", FixturePayloadAuditKind::StructuredPayload, 0U},
    {"xmgQ", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"Ilz", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"dM", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"Dn", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"lDuJ", FixturePayloadAuditKind::StructuredPayload, 7U},
    {"IsSy", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"AU", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"JgO", FixturePayloadAuditKind::StructuredPayload, 2U},
    {"OBee", FixturePayloadAuditKind::StructuredPayload, 1U},
    {"Yo", FixturePayloadAuditKind::NoStructuredPayload, 0U},
    {"ysVc", FixturePayloadAuditKind::StructuredPayload, 0U},
}};

template <std::size_t Size>
consteval bool AreUnique(const std::array<std::string_view, Size>& codes)
{
    for (std::size_t lhs = 0U; lhs < Size; ++lhs) {
        for (std::size_t rhs = lhs + 1U; rhs < Size; ++rhs) {
            if (codes[lhs] == codes[rhs]) {
                return false;
            }
        }
    }
    return true;
}

template <std::size_t SubsetSize, std::size_t SetSize>
consteval bool IsSubset(
    const std::array<std::string_view, SubsetSize>& subset,
    const std::array<std::string_view, SetSize>& set)
{
    for (const std::string_view code : subset) {
        bool found = false;
        for (const std::string_view candidate : set) {
            if (code == candidate) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

static_assert(AreUnique(HistoricalTypedBaselineCodes));
static_assert(AreUnique(FixtureDebtCodes));
static_assert(IsSubset(FixtureDebtCodes, HistoricalTypedBaselineCodes));

} // namespace cg::packet::test
