#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::platform {

inline constexpr std::size_t LegacyKeyStateCount = 256;

// NSM01：仅当前输入目标返回原U8键态；后台返回零，不清原数组或模拟释放。
std::uint8_t ReadSessionKeyState(session::ClientSession& session, std::int32_t key) noexcept;

// 原 sub_459760；逐项保留 0xEA 位，返回处理项数 256。
int ClearKeyFlags_459760(session::ClientSession& session) noexcept;

// 原 resetKeyState@0x459780；逐项保留 0xE8 位，返回处理项数 256。
int ResetKeyState_459780(session::ClientSession& session) noexcept;

// 原 sub_4597B0：完整W32键索引（S32承载）读取同源状态0x10位，返回0/0x10，不清状态。
int ReadKeyPressFlag_4597B0(session::ClientSession& session, std::int32_t key) noexcept;
// 原4597A0：完整S32索引，返回同源键态的4位，不清状态。
int ReadKeyFlag4_4597A0(session::ClientSession& session, std::int32_t key) noexcept;

} // namespace cg::platform
