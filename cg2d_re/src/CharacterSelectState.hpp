#pragma once

#include <array>
#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::character_select::state {

// 跨角色选择相关函数共享的原存储；保持唯一实例供后续真实依赖接入。
extern std::uint8_t gPukVersion; // 原 0x9A6E1C，R/W8。
extern std::array<login::YobiAction*, 2> gPreviewActions; // 原 0x9A6E44、0x9A6E48。
extern std::int16_t gPendingCharacterCount; // 原 0xF7CA06，R/W16。
extern std::int32_t gSelectedGraphicNo;      // 原 0xF7C9FC，R/W32。
extern std::uint16_t gCharacterEntryState;  // 原 0x9A6E10，R/W16。
extern std::int32_t gPreviewToggleState;     // 原 0x9A6E14，R/W32。
extern char gSelectedCharacterName[256];     // 原 0xDA391C。
extern char gDeleteCharacterName[132];       // 原 0xDA3A9C。

} // namespace cg::character_select::state
