#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

// 共享原状态，只在本组实现中定义；原全局：0x0059E9D8。
extern std::array<std::int32_t, 1025> gBattleEffectPositions59E9D8;

} // namespace detail

} // namespace cg::yobi
