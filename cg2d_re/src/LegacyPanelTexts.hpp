#pragma once

// 旧面板共用的原静态字节串；拆分后仍为一个静态地址。
namespace cg::login::detail {

inline constexpr char kLegacyBarterNo588A00[] = "\x20\x20\xA7\x5F";
inline constexpr char kLegacyBarterYes588A08[] = "\x20\x20\xAC\x4F";
inline constexpr char kLegacyItemUseCancel589008[] = "\xA8\xFA\x20\x20\xAE\xF8";
inline constexpr char kLegacyItemUseTitle585F54[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xB9\xEF\xB6\x48\xA1\x43";
inline constexpr char kLegacyLogout553FD0[] = "\xC3\xF6\xB3\xAC\xB5\xF8\xB5\xA1\xA1\x43";
inline constexpr char kLegacyRecipePage586D2C[] = "\xA4\xC1\xB4\xAB\xAD\xB6\xBC\xC6\xA1\x43";
inline constexpr char kLegacySkillTargetNext589360[] = "\xA4\x55\xAD\xB6";
inline constexpr char kLegacySkillTargetPrevious589358[] = "\xAB\x65\xAD\xB6";
inline constexpr char kLegacyTradeCancel5889AC[] = "\xB0\xB1\xA4\xEE\xA5\xE6\xA9\xF6\xA1\x43";

} // namespace cg::login::detail
