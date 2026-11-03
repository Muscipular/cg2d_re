#pragma once

#include "cg/Yobi/YobiRuntime.hpp"

#include <array>
#include <cstdint>

namespace cg::input {
class InputContext;
}

namespace cg::character_select::state {

// 两个角色选择场景共享的原版本载荷，不建立第二份语义存储。
inline constexpr wchar_t kVersion20[] = L"20031229_20"; // 原 0x55C80C。
inline constexpr wchar_t kVersion30[] = L"20031229_30"; // 原 0x55C800。

// 跨角色选择相关函数共享的原存储；保持唯一实例供后续真实依赖接入。
extern std::uint8_t gGraphicVersionOverride58B6A0; // 原全局：0x0058B6A0。 原覆盖W8，FF时才读取独立Puk版本。
extern cg::yobi::YobiAction* gCharacterModalAction; // 原 0x9A6E64，原生指针。
extern std::wstring gSelectedCharacterName;     // 原 0xDA391C。
extern std::wstring gDeleteCharacterName;       // 原 0xDA3A9C。

} // namespace cg::character_select::state
