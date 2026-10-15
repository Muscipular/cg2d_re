#pragma once

#include "base.hpp"
#include "battle_base.hpp"

#include <cstdint>

namespace cg::login {

namespace detail {

// 原432ECE..4340FE通用状态生产；不同成功门禁、替换动作和动态技能路径分别保留。
[[maybe_unused]] void ProcessBattleMSkill32Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原43412E/434130..434EDF；常规入口传entryValue=0，3037保留原EDI=1。
[[maybe_unused]] void ProcessBattleMSkill19Family(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t fpUsed, const std::int32_t entryValue,
    const std::uint32_t flags9C, const std::int32_t valueA0);

// 原434EDF..43573F；267保留继承累计值，每目标累计效果与最终累计尾分别生产。
[[maybe_unused]] void ProcessBattleMSkill267(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed,
    const std::uint32_t flags9C, const std::int32_t valueA0);

} // namespace detail

} // namespace cg::login
