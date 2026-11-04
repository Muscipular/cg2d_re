#include "battle_skill.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "battle_skill_attack.hpp"
#include "battle_skill_status.hpp"
#include "battle_skill_special.hpp"
#include "battle_skill_effect.hpp"
#include "cg/render/RendererResources.hpp"

#include "battle_scene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/BattleState.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原函数：0x004242B0。
std::int32_t AppendBattleRedirect_4242B0(
    YobiAction* const scene, const std::int32_t source, const std::int32_t target,
    const std::uint32_t flags, const std::int32_t value, const std::int32_t redirect)
{
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[source], BattleAction_AdvanceEffectAndCast);
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[source], redirect, flags, value);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[source], BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[source], BattleAction_AdvanceLinkedCast);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[source], BattleAction_Idle);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[source], BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_Idle);
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[redirect], target, flags, value);
    scene->ownerSession->WorldScene().battle.allEntries[redirect]->payload.battleActor->targetPosition100C = target;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_FaceTargetFormationPoint);
    scene->ownerSession->WorldScene().battle.allEntries[redirect]->payload.battleActor->soundId1018 = 252;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_PlaySound1018);
    scene->ownerSession->WorldScene().battle.allEntries[redirect]->payload.battleActor->reactionImage1010 = 0x5748;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_ShowReactionHitEffect);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_Wait30Ticks);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_EnableFlash);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_PlayBeginRunFacingEffectTarget);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_CloseInOnEffectTarget);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_TurnTargetTowardSelf);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_AdvanceLinkedCast);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_PlayEndRunAndRestoreFacing);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_DisableFlash);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[redirect], BattleAction_AdvanceEffectAndCast);
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[redirect], target, flags, value);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_Idle);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_ReactToHitWithSound);
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[target], source, flags, value);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_AdvanceLinkedCast);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_AdvanceEffectAndCast);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_SetIdlePose);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[target], BattleAction_WaitIdle);
    return redirect;
}

// 原函数：0x00424500。
void AppendBattleSkillTail_424500(
    YobiAction* const scene, const std::int32_t position, const std::uint32_t flags,
    const std::int32_t mode, const std::int32_t target)
{
    if (mode == 0) {
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_Wait60Ticks);
    }
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_AdvanceLinkedCast);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_AdvanceEffectAndCast);
    if ((flags & BattleResultMask_PhysicalReaction) != 0) {
        if (target < 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_ReturnToFormation);
        } else {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_ApproachEffectTarget);
            AppendBattleEffect_439400(scene->ownerSession->WorldScene().battle.allEntries[position], {target, 0, 0, 0});
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_AdvanceEffectAndCast);
        }
    }
    if ((flags & BattleResultFlag_Death) == 0) {
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_FaceFormation);
    }
    // 原4245BF独立尾块，不能与前面的E000h分支合并去重。
    if (mode == 0) {
        if (target < 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_ReturnToFormation);
        } else {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_ApproachEffectTarget);
            AppendBattleEffect_439400(scene->ownerSession->WorldScene().battle.allEntries[position], {target, 0, 0, 0});
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[position], BattleAction_AdvanceEffectAndCast);
        }
    }
}

// 原427470/SKL的非零子状态；由ProcessBattleMSkillHeader调用，不单独注册消息回调。
static void ProcessBattleMSkillWaitingState(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t subState)
{
    const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
    const std::int32_t skill98Mode88 = scene->ownerSession->WorldScene().battle.mSkill98Mode;
    if (subState == BattleMSkillPhase_WaitForEntryReady) {
        switch (skill) {
        case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicFire:
        case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicEarthStrong:
        case BattleSkill_AttackMagicWaterStrong: case BattleSkill_AttackMagicFireStrong:
        case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicEarthSuper:
        case BattleSkill_AttackMagicWaterSuper: case BattleSkill_AttackMagicFireSuper:
        case BattleSkill_AttackMagicWindSuper: case BattleSkill_Dorain:
        case BattleSkill_HealMagic: case BattleSkill_HealMagicStrong: case BattleSkill_HealMagicSuper: case BattleSkill_Sacrifice:
        case BattleSkill_MonWind: case BattleSkill_Acid: case BattleSkill_Barrier:
        case BattleSkill_EmergencyTransfusion: case 2055: case 2056: case 2057:
        case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
        case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
        case 3037:
            break;
        default:
            return;
        }
        const std::int32_t count = payload->entryCount04;
        if (count == 0) {
            payload->packetSubState58 = BattleMSkillPhase_WaitForTurnReady;
            return;
        }
        std::int32_t readyCount = 0;
        for (std::int32_t index = 0; index < count - 1; ++index) {
            if (scene->ownerSession->WorldScene().battle.allEntries[payload->entryPositions08[index]]->phase == BattleAction_Idle) {
                ++readyCount;
            }
        }
        YobiAction* const last = scene->ownerSession->WorldScene().battle.allEntries[payload->entryPositions08[count - 1]];
        if (last->phase == BattleAction_WaitIdle) {
            ++readyCount;
        }
        if (readyCount < count) {
            return;
        }
        const std::int32_t casterPhase = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->phase;
        if (casterPhase == BattleAction_Idle || casterPhase == BattleAction_WaitIdle) {
            // 原42A930传最后参与者ECX，而不是用于phase门禁的施法者。
            AdvanceBattleCastState_439340(last);
            payload->packetSubState58 = BattleMSkillPhase_WaitForTurnReady;
        }
        return;
    }
    if (subState != BattleMSkillPhase_WaitForTurnReady) {
        return;
    }

    enum class FinishKind {
        Caster, LastCaster, OnlyFinish, ClearCaster, BothCasters, CasterFlash, EffectTargets,
    };
    FinishKind kind;
    switch (skill) {
    case BattleSkill_ContinuationAttack: case BattleSkill_Parameter_DoubleEdgedBlade:
    case BattleSkill_Parameter_AllOrNothing: case BattleSkill_GuardBreak: case BattleSkill_Forcecut: case BattleSkill_Steal:
    case BattleSkill_NormalAttack:
    case BattleSkill_StatusAttackPoison: case BattleSkill_StatusAttackSleep: case BattleSkill_StatusAttackStone:
    case BattleSkill_StatusAttackInebriety: case BattleSkill_StatusAttackConfusion:
    case BattleSkill_StatusAttackForget: case BattleSkill_BloodAttack:
    case BattleSkill_EnergyDrain: case BattleSkill_EquipBreakAttack: case BattleSkill_GoldAttack: case BattleSkill_PanicAttack: case BattleSkill_RandomShot: case BattleSkill_Assassin:
    case BattleSkill_Ranbu: case 110: case 113:
    case BattleSkill_DelayAttack257: case BattleSkill_DelayAttack258: case BattleSkill_StatusAttackPoisonStrike:
    case BattleSkill_Wattack: case BattleSkill_FirstAttack: case BattleSkill_Madmax:
    case BattleSkill_Recklessness: case BattleSkill_Taunt: case BattleSkill_ShortStrike: case BattleSkill_ShadowHand:
    case BattleSkill_MercyStrike: case BattleSkill_KarmaCycle:
    case BattleSkill_StatusAttackPoison2096: case BattleSkill_StatusAttackSleep2097:
    case BattleSkill_StatusAttackStone2098: case BattleSkill_StatusAttackInebriety2099:
    case BattleSkill_StatusAttackConfusion2100: case BattleSkill_StatusAttackForget2101:
    case 3019: case 3038: case BattleSkill_SwallowSlash: case BattleSkill_BladeSoul: case 4002:
        kind = FinishKind::LastCaster;
        break;
    case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicFire:
    case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicEarthStrong: case BattleSkill_AttackMagicWaterStrong:
    case BattleSkill_AttackMagicFireStrong:
    case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicEarthSuper:
    case BattleSkill_AttackMagicWaterSuper: case BattleSkill_AttackMagicFireSuper:
    case BattleSkill_AttackMagicWindSuper: case BattleSkill_Dorain: case BattleSkill_Bomb:
    case BattleSkill_MagicMissile: case BattleSkill_MonWind: case BattleSkill_Acid: case BattleSkill_Barrier:
    case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case BattleSkill_ConfusionShockwave: case 3037:
        kind = FinishKind::OnlyFinish;
        break;
    case BattleSkill_SpiracleShot: case BattleSkill_Consentration: case BattleSkill_TreatTypeEarth:
    case BattleSkill_TreatTypeWater: case BattleSkill_TreatTypeFire: case BattleSkill_TreatTypeWind:
    case BattleSkill_HealMagic: case BattleSkill_HealMagicStrong: case BattleSkill_HealMagicSuper: case BattleSkill_Sacrifice: case BattleSkill_TreatTypeSilence:
    case BattleSkill_Summon: case BattleSkill_EarthQuake:
    case BattleSkill_DetectEnemy: case 108: case 109: case BattleSkill_RecoveryUp: case 117: case BattleSkill_Copy: case BattleSkill_LvSet:
    case BattleSkill_BlastWave: case BattleSkill_WindRendingStrike:
    case BattleSkill_StatusChangePoison: case BattleSkill_StatusChangeSleep: case BattleSkill_StatusChangeStone:
    case BattleSkill_StatusChangeInebriety: case BattleSkill_StatusChangeConfusion:
    case BattleSkill_StatusChangeForget: case BattleSkill_StatusChangePoisonStrong:
    case BattleSkill_StatusChangeSleepStrong:
    case BattleSkill_StatusChangeStoneStrong: case BattleSkill_StatusChangeInebrietyStrong:
    case BattleSkill_StatusChangeConfusionStrong: case BattleSkill_StatusChangeForgetStrong:
    case BattleSkill_StatusChangePoisonSuper: case BattleSkill_StatusChangeSleepSuper:
    case BattleSkill_StatusChangeStoneSuper: case BattleSkill_StatusChangeInebrietySuper:
    case BattleSkill_StatusChangeConfusionSuper: case BattleSkill_StatusChangeForgetSuper:
    case BattleSkill_ReverseType: case BattleSkill_ReflectionPhysics: case BattleSkill_ReflectionMagic:
    case BattleSkill_AbsorbPhysics: case BattleSkill_AbsorbMagic: case BattleSkill_IneffectivePhysics:
    case BattleSkill_IneffectiveMagic:
    case BattleSkill_LpRecovery: case BattleSkill_LpRecoveryStrong: case BattleSkill_LpRecoverySuper:
    case BattleSkill_StatusRecover: case BattleSkill_Revive: case BattleSkill_Death:
    case BattleSkill_DefUp: case BattleSkill_DefDown: case BattleSkill_AtkUp: case BattleSkill_AtkDown:
    case BattleSkill_AglUp: case BattleSkill_AglDown:
    case BattleSkill_Concentration: case BattleSkill_ElementalAmplification: case BattleSkill_Blessing:
    case BattleSkill_DefensiveBarrier: case BattleSkill_VoodooCurse: case BattleSkill_BattleCommand:
    case BattleSkill_BideTime: case BattleSkill_SoloDance: case BattleSkill_Adrenaline:
    case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062: case 2063: case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3001: case 3002: case 3003: case 3004: case 3005: case 3006: case 3007: case 3008:
    case 3009: case 3010: case 3011: case 3012: case 3013: case 3014: case 3015: case 3016:
        kind = FinishKind::Caster;
        break;
    case BattleSkill_AttackAll: case 111: case BattleSkill_ThrowItem:
        kind = FinishKind::ClearCaster;
        break;
    case BattleSkill_RiderBreakAttack: case BattleSkill_RiderBreakMind:
        kind = FinishKind::BothCasters;
        break;
    case BattleSkill_UltimateAttack:
        kind = FinishKind::CasterFlash;
        break;
    case BattleSkill_Dance:
        kind = skill98Mode88 == BattleDanceMode_SwapActorAndPetPositions ? FinishKind::EffectTargets : FinishKind::Caster;
        break;
    default:
        return;
    }
    if (CheckBattleTurnReady_4394A0(scene, skill == BattleSkill_Revive ? BattleTurnReadyPolicy_CheckAllSelectedEntries : BattleTurnReadyPolicy_AllowInactiveSkip) != 1) {
        return;
    }
    switch (kind) {
    case FinishKind::Caster:
        FaceBattleActorTowardFormationPosition_406260(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]);
        break;
    case FinishKind::LastCaster: {
        YobiAction* const last = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mValue5ABA7C];
        if (last != nullptr) {
            auto* const lastPayload = last->payload.battleActor;
            if ((*reinterpret_cast<const unsigned char*>(&lastPayload->flagsFDC) & BattleActorFlag_Dead) == 0) {
                if (scene->ownerSession->WorldScene().battle.mSkill == BattleSkill_Ranbu) {
                    lastPayload->flash1004 = 0;
                }
                FaceBattleActorTowardFormationPosition_406260(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mValue5ABA7C]);
            }
        }
        break;
    }
    case FinishKind::OnlyFinish:
        break;
    case FinishKind::ClearCaster:
        FaceBattleActorTowardFormationPosition_406260(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]);
        FinishBattleEvent_43B6C0(scene);
        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->frameEvent = 0;
        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->flags &= ~YobiActionFlag_Hidden;
        if (render::HasLegacy3DDevice_44B610() != 0) {
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->flags &= ~YobiActionFlag_ForceAlpha80;
        } else {
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->flags &= ~YobiActionFlag_SkipDrawWhenParityZero;
        }
        return;
    case FinishKind::BothCasters: {
        YobiAction* const caster = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition];
        if (caster != nullptr && (*reinterpret_cast<const unsigned char*>(
                &caster->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) == 0) {
            FaceBattleActorTowardFormationPosition_406260(caster);
        }
        YobiAction* const last = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mValue5ABA7C];
        if (last != nullptr && (*reinterpret_cast<const unsigned char*>(
                &last->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) == 0) {
            FaceBattleActorTowardFormationPosition_406260(last);
        }
        break;
    }
    case FinishKind::CasterFlash: {
        YobiAction* const caster = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition];
        if (caster != nullptr && (*reinterpret_cast<const unsigned char*>(
                &caster->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) == 0) {
            FaceBattleActorTowardFormationPosition_406260(caster);
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                ->flash1004 = 0;
        }
        break;
    }
    case FinishKind::EffectTargets: {
        auto* const casterPayload = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor;
        for (BattleEntryEffectRecord* effect = casterPayload->effects1C.data(); effect->position != -1; ++effect) {
            YobiAction* const target = scene->ownerSession->WorldScene().battle.allEntries[effect->position];
            if (target != nullptr && target->characterAction != CharacterAnimation_Down && target->phase != BattleAction_BounceOutAfterKnockout) {
                FaceBattleActorTowardFormationPosition_406260(target);
            }
        }
        break;
    }
    }
    FinishBattleEvent_43B6C0(scene);
}

// 原42ACFD..42AE37；返回ECX技能，FP输出承接caller局部ESI，不接具体技能生产。
static std::int32_t ReadBattleMSkillHeader(
    YobiAction* const scene, std::int32_t& fpUsed)
{
    const std::int32_t initialTechnique = TakeBattleMInteger_438F70(scene);
    scene->ownerSession->WorldScene().battle.mTechnique = initialTechnique;
    fpUsed = TakeBattleMInteger_438F70(scene);
    scene->ownerSession->WorldScene().battle.mSkill = TakeBattleMInteger_438F70(scene);
    // E01／42AD06、42AD1A：在原技能／等级归一化前保留报文双 ID。
    CaptureBattleSkillMeta(initialTechnique, scene->ownerSession->WorldScene().battle.mSkill);
    scene->ownerSession->WorldScene().battle.mSkillLevel = TakeBattleMInteger_438F70(scene);
    std::int32_t requestedLevel = TakeBattleMInteger_438F70(scene);
    if (scene->ownerSession->WorldScene().battle.mTechnique >= 205000) {
        const std::int32_t remainder = scene->ownerSession->WorldScene().battle.mSkillLevel % 30;
        const bool belowOne = remainder < 1;
        scene->ownerSession->WorldScene().battle.mSkillLevel = remainder;
        if (belowOne) {
            scene->ownerSession->WorldScene().battle.mSkillLevel = 1;
        } else if (remainder >= 15) {
            scene->ownerSession->WorldScene().battle.mSkillLevel = 15;
        }
        if (requestedLevel < 1) {
            requestedLevel = 1;
        } else if (requestedLevel >= 15) {
            requestedLevel = 15;
        }
        scene->ownerSession->WorldScene().battle.mSkillLevelIndex = requestedLevel - 1;
        return scene->ownerSession->WorldScene().battle.mSkill;
    }
    const std::int32_t skill = initialTechnique / 100;
    std::int32_t technique = initialTechnique % 100;
    scene->ownerSession->WorldScene().battle.mSkill = skill;
    const bool reduceByFifty = technique >= 50;
    scene->ownerSession->WorldScene().battle.mTechnique = technique;
    scene->ownerSession->WorldScene().battle.mSkillLevelIndex = technique;
    if (reduceByFifty) {
        technique %= 50;
        scene->ownerSession->WorldScene().battle.mTechnique = technique;
        scene->ownerSession->WorldScene().battle.mSkillLevelIndex = technique;
    } else if (technique >= 30) {
        technique %= 30;
        scene->ownerSession->WorldScene().battle.mTechnique = technique;
        scene->ownerSession->WorldScene().battle.mSkillLevelIndex = technique;
    }
    if (technique >= 14) {
        scene->ownerSession->WorldScene().battle.mTechnique = 14;
        technique = 14;
        scene->ownerSession->WorldScene().battle.mSkillLevelIndex = 14;
    } else if (technique <= 0) {
        scene->ownerSession->WorldScene().battle.mTechnique = 0;
        technique = 0;
        scene->ownerSession->WorldScene().battle.mSkillLevelIndex = 0;
    }
    scene->ownerSession->WorldScene().battle.mSkillLevel = technique + 1;
    return skill;
}

void ProcessBattleMSkillHeader(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::uint32_t flags9C, const std::int32_t feedbackAlternateImageA0)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMSkillPhase_ReadAndQueue) {
        ProcessBattleMSkillWaitingState(scene, payload, subState);
        return;
    }
    std::int32_t fpUsed;
    const std::int32_t skill = ReadBattleMSkillHeader(scene, fpUsed);
    switch (skill) {
    case BattleSkill_SpiracleShot:
        ProcessBattleMSkill4(scene, payload, fpUsed);
        break;
    case BattleSkill_HealMagic: case BattleSkill_HealMagicStrong: case BattleSkill_HealMagicSuper: case BattleSkill_Sacrifice:
        ProcessBattleMSkill61Family(scene, payload, fpUsed);
        break;
    case BattleSkill_TreatTypeEarth: case BattleSkill_TreatTypeWater: case BattleSkill_TreatTypeFire:
    case BattleSkill_TreatTypeWind: case BattleSkill_TreatTypeSilence:
        ProcessBattleMSkill50Family(scene, payload, fpUsed);
        break;
    case BattleSkill_Consentration:
        ProcessBattleMSkill12(scene, payload, fpUsed);
        break;
    case BattleSkill_Bomb:
        ProcessBattleMSkill82(scene, payload, fpUsed);
        break;
    case BattleSkill_EarthQuake:
        ProcessBattleMSkill97(scene, payload, fpUsed);
        break;
    case BattleSkill_Summon:
        ProcessBattleMSkill93(scene, payload, fpUsed);
        break;
    case BattleSkill_Dance:
        ProcessBattleMSkill98(scene, payload, fpUsed);
        break;
    case BattleSkill_DetectEnemy:
        ProcessBattleMSkill107(scene, payload, fpUsed);
        break;
    case 108:
        ProcessBattleMSkill108(scene, payload, fpUsed);
        break;
    case 109:
        ProcessBattleMSkill109(scene, payload, fpUsed);
        break;
    case BattleSkill_UltimateAttack:
        ProcessBattleMSkill112(scene, payload, fpUsed);
        break;
    case BattleSkill_RecoveryUp:
        ProcessBattleMSkill114(scene, payload, fpUsed);
        break;
    case 117:
        ProcessBattleMSkill117(scene, payload, fpUsed);
        break;
    case BattleSkill_AttackAll: case 111: case BattleSkill_ThrowItem:
        ProcessBattleMSkill106Family(scene, payload, fpUsed);
        break;
    case BattleSkill_DefUp: case BattleSkill_DefDown: case BattleSkill_AtkUp: case BattleSkill_AtkDown:
    case BattleSkill_AglUp: case BattleSkill_AglDown:
    case BattleSkill_SoloDance: case 3001:
        ProcessBattleMSkill99Family(scene, payload, fpUsed);
        break;
    case BattleSkill_Copy:
        ProcessBattleMSkill263(scene, payload, fpUsed);
        break;
    case BattleSkill_MonWind:
        ProcessBattleMSkill400(scene, payload, fpUsed);
        break;
    case BattleSkill_Acid:
        ProcessBattleMSkill401(scene, payload, fpUsed);
        break;
    case BattleSkill_Barrier:
        ProcessBattleMSkill402(scene, payload, fpUsed);
        break;
    case BattleSkill_LvSet:
        ProcessBattleMSkill405(scene, payload, fpUsed);
        break;
    case BattleSkill_RiderBreakAttack: case BattleSkill_RiderBreakMind:
        ProcessBattleMSkill404Family(scene, payload);
        break;
    case BattleSkill_MagicMissile: case BattleSkill_ConfusionShockwave:
        ProcessBattleMSkill267(scene, payload, fpUsed, flags9C, feedbackAlternateImageA0);
        break;
    case BattleSkill_BlastWave: case BattleSkill_WindRendingStrike:
        ProcessBattleMSkill2005Family(scene, payload, fpUsed);
        break;
    case BattleSkill_EmergencyTransfusion:
        ProcessBattleMSkill2028(scene, payload, fpUsed);
        break;
    case BattleSkill_BladeSoul:
        ProcessBattleMSkill3101(scene, payload, fpUsed);
        break;
    case BattleSkill_StatusChangePoison: case BattleSkill_StatusChangeSleep: case BattleSkill_StatusChangeStone:
    case BattleSkill_StatusChangeInebriety: case BattleSkill_StatusChangeConfusion:
    case BattleSkill_StatusChangeForget: case BattleSkill_StatusChangePoisonStrong:
    case BattleSkill_StatusChangeSleepStrong: case BattleSkill_StatusChangeStoneStrong:
    case BattleSkill_StatusChangeInebrietyStrong: case BattleSkill_StatusChangeConfusionStrong:
    case BattleSkill_StatusChangeForgetStrong: case BattleSkill_StatusChangePoisonSuper:
    case BattleSkill_StatusChangeSleepSuper:
    case BattleSkill_StatusChangeStoneSuper: case BattleSkill_StatusChangeInebrietySuper:
    case BattleSkill_StatusChangeConfusionSuper: case BattleSkill_StatusChangeForgetSuper:
    case BattleSkill_ReverseType: case BattleSkill_ReflectionPhysics: case BattleSkill_ReflectionMagic:
    case BattleSkill_AbsorbPhysics: case BattleSkill_AbsorbMagic: case BattleSkill_IneffectivePhysics:
    case BattleSkill_IneffectiveMagic:
    case BattleSkill_LpRecovery: case BattleSkill_LpRecoveryStrong: case BattleSkill_LpRecoverySuper:
    case BattleSkill_StatusRecover: case BattleSkill_Revive: case BattleSkill_Death:
    case BattleSkill_Concentration: case BattleSkill_ElementalAmplification: case BattleSkill_Blessing:
    case BattleSkill_DefensiveBarrier: case BattleSkill_VoodooCurse: case BattleSkill_BattleCommand:
    case BattleSkill_BideTime:
    case BattleSkill_Adrenaline: case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062:
    case 2063: case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3002: case 3003: case 3004: case 3005: case 3006:
    case 3007: case 3008: case 3009: case 3010: case 3011:
    case 3012: case 3013: case 3014: case 3015: case 3016:
        ProcessBattleMSkill32Family(scene, payload, fpUsed);
        break;
    case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicFire:
    case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicEarthStrong: case BattleSkill_AttackMagicWaterStrong:
    case BattleSkill_AttackMagicFireStrong:
    case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicEarthSuper:
    case BattleSkill_AttackMagicWaterSuper: case BattleSkill_AttackMagicFireSuper:
    case BattleSkill_AttackMagicWindSuper: case BattleSkill_Dorain:
    case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
        ProcessBattleMSkill19Family(scene, payload, fpUsed, 0, flags9C, feedbackAlternateImageA0);
        break;
    case 3037:
        ProcessBattleMSkill19Family(scene, payload, fpUsed, 1, flags9C, feedbackAlternateImageA0);
        break;
    case BattleSkill_NormalAttack: {
        const std::int32_t mode = TakeBattleMInteger_438F70(scene);
        if (mode == BattleAttackMode_Melee) {
            const std::int32_t attackSkillLevelIndex = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
            const std::int32_t attackSkillLevel = scene->ownerSession->WorldScene().battle.mSkillLevel;
            const std::int32_t attackSkill = scene->ownerSession->WorldScene().battle.mSkill;
            ProcessBattleMSkillAttackTargets(
                scene, payload, attackSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, false);
        } else {
            ProcessBattleMSkill73NonzeroMode(scene, payload, mode);
        }
        break;
    }
    case BattleSkill_SwallowSlash: {
        TakeBattleMInteger_438F70(scene);
        const std::int32_t attackSkillLevelIndex = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        const std::int32_t attackSkillLevel = scene->ownerSession->WorldScene().battle.mSkillLevel;
        const std::int32_t attackSkill = scene->ownerSession->WorldScene().battle.mSkill;
        ProcessBattleMSkillAttackTargets(
            scene, payload, attackSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, true);
        break;
    }
    case BattleSkill_ContinuationAttack: case BattleSkill_Parameter_DoubleEdgedBlade:
    case BattleSkill_Parameter_AllOrNothing: case BattleSkill_GuardBreak: case BattleSkill_Forcecut: case BattleSkill_Steal:
    case BattleSkill_StatusAttackPoison: case BattleSkill_StatusAttackSleep: case BattleSkill_StatusAttackStone:
    case BattleSkill_StatusAttackInebriety: case BattleSkill_StatusAttackConfusion:
    case BattleSkill_StatusAttackForget: case BattleSkill_BloodAttack:
    case BattleSkill_EnergyDrain: case BattleSkill_EquipBreakAttack: case BattleSkill_GoldAttack: case BattleSkill_PanicAttack: case BattleSkill_RandomShot: case BattleSkill_Assassin:
    case BattleSkill_Ranbu: case 110: case 113:
    case BattleSkill_DelayAttack257: case BattleSkill_DelayAttack258: case BattleSkill_StatusAttackPoisonStrike:
    case BattleSkill_Wattack: case BattleSkill_FirstAttack: case BattleSkill_Madmax:
    case BattleSkill_Recklessness: case BattleSkill_Taunt: case BattleSkill_ShortStrike: case BattleSkill_ShadowHand:
    case BattleSkill_MercyStrike: case BattleSkill_KarmaCycle:
    case BattleSkill_StatusAttackPoison2096: case BattleSkill_StatusAttackSleep2097:
    case BattleSkill_StatusAttackStone2098: case BattleSkill_StatusAttackInebriety2099:
    case BattleSkill_StatusAttackConfusion2100: case BattleSkill_StatusAttackForget2101:
    case 3019: case 3038: case 4002: {
        const std::int32_t mode = TakeBattleMInteger_438F70(scene);
        const std::int32_t currentSkill = scene->ownerSession->WorldScene().battle.mSkill;
        if (currentSkill == BattleSkill_Steal || currentSkill == 113 || currentSkill == BattleSkill_ShadowHand || mode == BattleAttackMode_Melee) {
            const std::int32_t attackSkillLevelIndex = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
            const std::int32_t attackSkillLevel = scene->ownerSession->WorldScene().battle.mSkillLevel;
            ProcessBattleMSkillAttackTargets(
                scene, payload, currentSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, false);
        } else {
            ProcessBattleMSkillAttackNonzeroMode(scene, payload, fpUsed, mode);
        }
        break;
    }
    default:
        break;
    }
}

} // namespace detail

} // namespace cg::yobi
