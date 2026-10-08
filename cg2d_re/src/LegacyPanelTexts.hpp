#pragma once

// 旧面板共用的原静态字节串；拆分后仍为一个静态地址。
namespace cg::login::detail {

inline constexpr char kLegacyBarterNo588A00[] = "\x20\x20\xB7\xF1";
inline constexpr char kLegacyBarterYes588A08[] = "\x20\x20\xCA\xC7";
inline constexpr char kLegacyItemUseCancel589008[] = "\xC8\xA1\x20\x20\xCF\xFB";
inline constexpr char kLegacyItemUseTitle585F54[] = "\xD5\x88\xDF\x78\x93\xF1\x8C\xA6\xCF\xF3\xA1\xA3";
inline constexpr char kLegacyLogout553FD0[] = "\xEA\x50\xE9\x5D\xD2\x95\xB4\xB0\xA1\xA3";
inline constexpr char kLegacyRecipePage586D2C[] = "\xC7\xD0\x93\x51\xED\x93\x94\xB5\xA1\xA3";
inline constexpr char kLegacySkillTargetNext589360[] = "\xCF\xC2\xED\x93";
inline constexpr char kLegacySkillTargetPrevious589358[] = "\xC7\xB0\xED\x93";
inline constexpr char kLegacyTradeCancel5889AC[] = "\xCD\xA3\xD6\xB9\xBD\xBB\xD2\xD7\xA1\xA3";

} // namespace cg::login::detail
