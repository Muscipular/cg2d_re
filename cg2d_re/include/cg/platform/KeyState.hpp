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

} // namespace cg::platform
