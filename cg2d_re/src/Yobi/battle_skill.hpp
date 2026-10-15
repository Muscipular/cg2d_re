#pragma once

#include "base.hpp"
#include "battle_base.hpp"

#include <array>
#include <cstdint>

namespace cg::login {

namespace detail {

inline constexpr std::array<std::int32_t, 12> kBattleMSkillEffectOffsets553E58 { // 原常量参考：0x00553E58。
    3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0,
}; // 原553E58，独立于效果工厂553A38的同值表。

void ProcessBattleMSkillHeader(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::uint32_t flags9C, const std::int32_t valueA0);

} // namespace detail

} // namespace cg::login
