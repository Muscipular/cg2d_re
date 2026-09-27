#pragma once

#include "cg/YobiRuntime.hpp"

#include <array>
#include <cstdint>

namespace cg::character_select::state {

// 两个角色选择场景共享的原版本载荷，不建立第二份语义存储。
inline constexpr char kVersion20[] = "20031229_20"; // 原 0x55C80C。
inline constexpr char kVersion30[] = "20031229_30"; // 原 0x55C800。

// 跨角色选择相关函数共享的原存储；保持唯一实例供后续真实依赖接入。
extern std::uint8_t gPukVersion; // 原 0x9A6E1C，R/W8。
extern std::uint8_t gGraphicVersionOverride58B6A0; // 原覆盖W8，FF时才读取独立Puk版本。
extern std::uint8_t gValue9A57E0; // 原 0x9A57E0，创建界面版本门禁 W8。
extern std::uint8_t gValue9A4CFC; // 原 0x9A4CFC，创建界面选择 W8。
extern std::uint8_t gValue9A5B00; // 原 0x9A5B00，创建界面状态 W8。
extern std::array<login::YobiAction*, 2> gCreateActions; // 原 0x9A4CB4/0x9A4CB8，非拥有。
extern std::array<login::YobiAction*, 2> gPreviewActions; // 原 0x9A6E44、0x9A6E48。
extern login::YobiAction* gCharacterModalAction; // 原 0x9A6E64，原生指针。
extern std::int16_t gPendingCharacterCount; // 原 0xF7CA06，R/W16。
extern std::int32_t gSelectedGraphicNo;      // 原 0xF7C9FC，R/W32。
extern std::uint16_t gCharacterEntryState;  // 原 0x9A6E10，R/W16。
extern std::int32_t gPreviewToggleState;     // 原 0x9A6E14，R/W32。
extern char gSelectedCharacterName[256];     // 原 0xDA391C。
extern char gDeleteCharacterName[132];       // 原 0xDA3A9C。

} // namespace cg::character_select::state
