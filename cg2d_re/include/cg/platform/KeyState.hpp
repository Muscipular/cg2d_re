#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

inline constexpr std::size_t LegacyKeyStateCount = 256;

// 原 gKeyState.states@0x73EFF8；窗口过程写入，场景控制器只读。
extern std::uint8_t gKeyStates[LegacyKeyStateCount];

// 原 sub_459760；逐项保留 0xEA 位，返回处理项数 256。
int ClearKeyFlags_459760() noexcept;

// 原 resetKeyState@0x459780；逐项保留 0xE8 位，返回处理项数 256。
int ResetKeyState_459780() noexcept;

// 原 sub_4597B0；读取按键状态的 0x10 位，返回 0 或 0x10，不修改状态。
int ReadKeyPressFlag_4597B0(std::uint8_t key) noexcept;
// 原4597C0：Ctrl bit1短路后查询完整S32键索引bit4，返回0/1，不清状态。
int ReadControlKeyPressFlag_4597C0(std::int32_t key) noexcept;
// 原4597A0：完整S32索引，返回同源键态的4位，不清状态。
int ReadKeyFlag4_4597A0(std::int32_t key) noexcept;

} // namespace cg::platform
