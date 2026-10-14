#pragma once

// 旧面板共用的原静态字节串；拆分后仍为一个静态地址。
namespace cg::login::detail {

inline constexpr wchar_t kLegacyBarterNo588A00[] = L"  否";
inline constexpr wchar_t kLegacyBarterYes588A08[] = L"  是";
inline constexpr wchar_t kLegacyItemUseCancel589008[] = L"取  消";
inline constexpr wchar_t kLegacyItemUseTitle585F54[] = L"請選擇對象。";
inline constexpr wchar_t kLegacyLogout553FD0[] = L"關閉視窗。";
inline constexpr wchar_t kLegacyRecipePage586D2C[] = L"切換頁數。";
inline constexpr wchar_t kLegacySkillTargetNext589360[] = L"下頁";
inline constexpr wchar_t kLegacySkillTargetPrevious589358[] = L"前頁";
inline constexpr wchar_t kLegacyTradeCancel5889AC[] = L"停止交易。";

} // namespace cg::login::detail
