#pragma once

#include "base.hpp"
#include "battle_base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

inline constexpr std::array<std::int32_t, 12> kBattleMSkillEffectOffsets553E58 { // 原常量参考：0x00553E58。
    3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0,
}; // 原553E58，独立于效果工厂553A38的同值表。

void ProcessBattleMSkillHeader(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::uint32_t flags9C, const std::int32_t valueA0);

// 原4242B0：三方动作/效果按序追加，返回redirect，不缓存对象表中的T*。
std::int32_t AppendBattleRedirect_4242B0(
    YobiAction* scene, std::int32_t source, std::int32_t target,
    std::uint32_t flags, std::int32_t value, std::int32_t redirect);
// 原424500：两处目标动作可重复追加；flags保持W32，只消费E000h/20h。
void AppendBattleSkillTail_424500(
    YobiAction* scene, std::int32_t position, std::uint32_t flags,
    std::int32_t mode, std::int32_t target);

} // namespace detail

} // namespace cg::yobi
