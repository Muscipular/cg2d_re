#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::login {

namespace detail {

// BCPY01：原5538F4十行S8序列；后八行全0，59F9DC按已证PE映像静态零初值。
inline constexpr std::array<std::int8_t, 200> kBattleActionSequence5538F4 { // 原常量参考：0x005538F4。
    7, 8, 16, 15, 10, 11, 13, 12, 14, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// 共享原状态，只在本组实现中定义；原全局：0x0059F9DC。
extern std::int32_t gBattleActionSequenceIndex59F9DC;

} // namespace detail

} // namespace cg::login
