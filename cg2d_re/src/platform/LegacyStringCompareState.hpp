#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原 [1086160,10861A0)；邮件重叠写必须作用于同一对象表示，不建私有副本。
struct LegacyTextLocaleStorage {
    std::uint32_t ctypeLocale;
    std::array<std::uint8_t, 0x0C> remaining04;
    std::uint32_t conversionCodePage;
    std::array<std::uint8_t, 0x24> remaining14;
    std::uint32_t mapBackend;
    std::array<std::uint8_t, 0x04> remaining3C;
};
static_assert(std::is_standard_layout_v<LegacyTextLocaleStorage>);
static_assert(std::is_trivially_copyable_v<LegacyTextLocaleStorage>);
static_assert(sizeof(LegacyTextLocaleStorage) == 0x40);
static_assert(offsetof(LegacyTextLocaleStorage, ctypeLocale) == 0x00);
static_assert(offsetof(LegacyTextLocaleStorage, conversionCodePage) == 0x10);
static_assert(offsetof(LegacyTextLocaleStorage, mapBackend) == 0x38);

extern LegacyTextLocaleStorage gLegacyTextLocale1086160;
// 复用原 LegacyTime 符号，仅把独立数值改为上面同址字段的引用。
extern std::uint32_t& gConversionCodePage_1086170;
extern std::array<std::uint16_t, 257> gCtypeTable58FC38;
extern std::uint16_t* gCtypePointer58FC30;
extern char gNlsProbeD68A7C;

} // namespace cg::platform
