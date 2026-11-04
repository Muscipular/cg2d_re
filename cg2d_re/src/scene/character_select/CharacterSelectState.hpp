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

} // namespace cg::character_select::state
