#pragma once

#include "base.hpp"
#include "battle_base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原435B63..43695E及42B26D..42B282；caller先分流mode0和72/113/2017至完整424620攻击生产。
[[maybe_unused]] void ProcessBattleMSkillAttackNonzeroMode(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t fpUsed, const std::int32_t mode);

// 原43699B..4369F9；3101沿426FC0生产，收尾重取initial输出而非source。
[[maybe_unused]] void ProcessBattleMSkill3101(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42AE7A..42B229及42B26D..42B282；caller已读mode并分流0，不提前首对象的后置空检查。
[[maybe_unused]] void ProcessBattleMSkill73NonzeroMode(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t mode);

// 42B253/43576F/436983共用九参生产尾；3100写1，其余两路递增。
void ProcessBattleMSkillAttackTargets(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t skill, const std::int32_t skillLevel, const std::int32_t skillLevelIndex,
    const std::int32_t fpUsed, const bool fixedState);

} // namespace detail

} // namespace cg::yobi
