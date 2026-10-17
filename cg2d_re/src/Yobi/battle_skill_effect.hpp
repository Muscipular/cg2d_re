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
void CaptureBattleSkillMeta(std::int32_t techId, std::int32_t skillId);
void BindBattleSkillMeta(YobiAction* action);
void InitializeBattleEffects();
void ShutdownBattleEffects();
std::int32_t ResolveSkillEffectImage(
    YobiAction* source, SkillEffectKind kind, std::int32_t originalImage);
std::int32_t ResolveSkillCastingImage(YobiAction* source, std::int32_t originalImage);

// E10：技能演绎独立 Lua 状态；仅接管已注册的 SKL，原生正文供状态机直接调用。
enum class LuaSkillPacketResult { Native, Handled, Finished };
void InitializeLuaSkillEffects();
void ShutdownLuaSkillEffects();
LuaSkillPacketResult ProcessLuaSkillPacket(YobiAction* scene);
bool LuaSkillEffectBindMoveState(YobiAction* action, BattleSkillMeta& meta);
void UpdateLuaSkillEffect(YobiAction* action);
void AdvanceLuaSkillEffect(YobiAction* action);
bool HandleLuaSkillEffectDeath(YobiAction* action);
void FreeLuaSkillEffectAction(YobiAction* action, YobiCallback releaseStorage);
void UpdateNativeBattleActor(YobiAction* action);
void AdvanceNativeBattleCastState(YobiAction* action);

} // namespace cg::yobi::detail
