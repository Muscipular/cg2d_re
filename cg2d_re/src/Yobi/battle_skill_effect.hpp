#pragma once

#include "battle_base.hpp"

namespace cg::yobi::detail {

// E02–E08：effectHook.yml 各类图号配置；匹配只使用原始 techId。
enum class SkillEffectKind {
    SpiracleShot, BlastWave, Magic, Heal, Recovery, StatusRecover, Revive,
    Bow, Knife, Boomerang, Count,
};

// E01：仅 BattleActor 载荷拥有元数据，控制器／场景不可按旧偏移访问。
BattleSkillMeta* GetBattleSkillMeta(YobiAction* action);
void CaptureBattleSkillMeta(session::ClientSession& session, std::int32_t techId, std::int32_t skillId);
void BindBattleSkillMeta(session::ClientSession& session, YobiAction* action);
std::int32_t ResolveSkillEffectImage(
    YobiAction* source, SkillEffectKind kind, std::int32_t originalImage);
std::int32_t ResolveSkillCastingImage(YobiAction* source, std::int32_t originalImage);

} // namespace cg::yobi::detail
