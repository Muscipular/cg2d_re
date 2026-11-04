#include "battle_skill_status.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "battle_skill.hpp"

#include "battle_scene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/BattleState.hpp"

#include <Windows.h>

#include <array>
#include <format>
#include <bit>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::yobi {

namespace detail {

// 原2006/2010/2011/2012/2013/2015/2014/2027有效档1..5；各行原地址见共享战斗明细。
static constexpr std::array<std::array<std::int32_t, 5>, 8> kBattleMStatusBandImages {{
    {111063, 111064, 111065, 111066, 111067},
    {111078, 111079, 111080, 111081, 111082},
    {111088, 111089, 111090, 111091, 111092},
    {111083, 111084, 111085, 111086, 111087},
    {111093, 111094, 111095, 111096, 111097},
    {111111, 111112, 111113, 111114, 111115},
    {111106, 111107, 111108, 111109, 111110},
    {111142, 111143, 111144, 111145, 111146},
}};

// 原54D738..54D864，按真实等级直接索引，重复图号不取消列。
// BSI01：各行对应独立技能；仅凭图号不能确认名称，2058～2067／2089～2095仍保留技能编号。
// 2091～2095当前使用111083～111087；用户资料为111183～111187，尚未消解差异，不改表值。
static constexpr std::array<std::array<std::int32_t, 5>, 15> kBattleMStatusImages54D738 {{ // 原常量参考：0x0054D738。
    {111167, 111167, 111167, 111167, 111167},
    {111168, 111168, 111168, 111168, 111168},
    {111169, 111169, 111169, 111169, 111169},
    {111170, 111170, 111170, 111170, 111170},
    {111171, 111171, 111171, 111171, 111171},
    {111172, 111172, 111172, 111172, 111172},
    {111173, 111173, 111173, 111173, 111173},
    {111174, 111174, 111174, 111174, 111174},
    {111175, 111175, 111175, 111175, 111175},
    {111176, 111176, 111176, 111176, 111176},
    {111083, 111084, 111085, 111086, 111087},
    {111083, 111084, 111085, 111086, 111087},
    {111083, 111084, 111085, 111086, 111087},
    {111078, 111079, 111080, 111081, 111082},
    {111078, 111079, 111080, 111081, 111082},
}};

// 原54D864..54D93C，前三行2050..2052，后十五行3002..3016。
// BSI01：仅有独立技能的播放图号依据，业务名称未确认；不得按共享常驻状态图猜名。
static constexpr std::array<std::array<std::int32_t, 3>, 18> kBattleMStatusImages54D864 {{ // 原常量参考：0x0054D864。
    {111153, 111154, 111155},
    {111156, 111157, 111158},
    {111159, 111160, 111161},
    {111247, 111248, 111249},
    {111197, 111198, 111199},
    {111200, 111201, 111202},
    {111203, 111204, 111205},
    {111206, 111207, 111208},
    {111209, 111210, 111211},
    {111212, 111213, 111214},
    {111215, 111216, 111217},
    {111218, 111219, 111220},
    {111221, 111222, 111223},
    {111224, 111225, 111226},
    {111227, 111228, 111229},
    {111230, 111231, 111232},
    {111233, 111234, 111235},
    {111236, 111237, 111238},
}};

static_assert(sizeof(kBattleMStatusBandImages) + sizeof(kBattleMStatusImages54D738)
    + sizeof(kBattleMStatusImages54D864) == 676);

// 原432ECE..4340FE通用状态生产；不同成功门禁、替换动作和动态技能路径分别保留。
[[maybe_unused]] void ProcessBattleMSkill32Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    scene->ownerSession->WorldScene().battle.mCasterPosition = parsedCasterPosition;
    YobiAction* const caster = scene->ownerSession->WorldScene().battle.allEntries[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = scene->ownerSession->WorldScene().battle.mSkill;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
    YobiAction* const levelCaster = scene->ownerSession->WorldScene().battle.allEntries[levelCasterPosition];
    const std::int32_t casterLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ConsumeFp);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_FaceFormation);
    const std::int32_t initialSkill = scene->ownerSession->WorldScene().battle.mSkill;
    const std::int32_t level = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    if (initialSkill == BattleSkill_Death) {
        if (level < 12) {
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t imageCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            const std::int32_t initialImage = imageOffset + 110052;
            YobiAction* const imageCaster = scene->ownerSession->WorldScene().battle.allEntries[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                ->image1028 = 111001;
        }
    } else if ((initialSkill >= BattleSkill_LpRecovery && initialSkill <= BattleSkill_LpRecoverySuper)
        || initialSkill == BattleSkill_Revive) {
        if (level < 12) {
            const std::int32_t imageCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t initialImage = imageOffset + 110081;
            YobiAction* const imageCaster = scene->ownerSession->WorldScene().battle.allEntries[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                ->image1028 = 111014;
        }
    } else {
        if (level < 12) {
            const std::int32_t imageCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t initialImage = imageOffset + 110056;
            YobiAction* const imageCaster = scene->ownerSession->WorldScene().battle.allEntries[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                ->image1028 = 111018;
        }
    }
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_CastSpellAndTriggerTargets);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_SetIdlePose);
    std::int32_t statusType;
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        scene->ownerSession->WorldScene().battle.mTargetPosition = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (scene->ownerSession->WorldScene().battle.allEntries[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        scene->ownerSession->WorldScene().battle.mResultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        std::uint32_t flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        if ((flags & BattleResultFlag_Success) != 0) {
            const std::int32_t appendTargetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
            const std::int32_t appendLevel = scene->ownerSession->WorldScene().battle.mSkillLevel;
            const std::int32_t appendSkill = scene->ownerSession->WorldScene().battle.mSkill;
            AddBattleAppendEffect_43BE00(scene, appendSkill, appendLevel, appendTargetPosition);
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        const std::int32_t effectCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t effectTargetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
        YobiAction* const effectCaster = scene->ownerSession->WorldScene().battle.allEntries[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, flags, 0);
        const std::int32_t skillTargetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
        YobiAction* const skillTarget = scene->ownerSession->WorldScene().battle.allEntries[skillTargetPosition];
        const std::int32_t targetSkill = scene->ownerSession->WorldScene().battle.mSkill;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
        YobiAction* const levelTarget = scene->ownerSession->WorldScene().battle.allEntries[levelTargetPosition];
        const std::int32_t targetLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor
            ->numberDisplayType1034 = BattleNumber_SkipAfterEffect;
        const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
        if ((skill >= BattleSkill_ReflectionPhysics && skill <= BattleSkill_IneffectiveMagic)
            || (skill >= BattleSkill_LpRecovery && skill <= BattleSkill_StatusRecover)) {
            const std::int32_t sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            const std::int32_t targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
            const bool sameSide = (sourcePosition == 20 && targetPosition < 10)
                || (sourcePosition == 21 && targetPosition >= 10);
            const std::int32_t state = skill <= BattleSkill_IneffectiveMagic
                ? (sameSide ? BattleAction_PlayBarrierSpellEffectAndFadeIn : BattleAction_PlayBarrierSpellEffect)
                : skill <= BattleSkill_LpRecoverySuper
                    ? (sameSide ? BattleAction_PlayRecoveryEffectAndFadeIn : BattleAction_PlayRecoveryEffect)
                    : (sameSide ? BattleAction_PlayStatusRecoveryEffectAndFadeIn : BattleAction_PlayStatusRecoveryEffect);
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], state);
            if (skill == BattleSkill_StatusRecover && (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ClearDebuff);
            }
        } else if (skill >= BattleSkill_StatusChangePoison && skill <= BattleSkill_StatusChangeForgetSuper) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayDebuffSpellEffect);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) == 0) {
                continue;
            }
            const std::int32_t successfulSkillPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
            YobiAction* const successfulSkillActor = scene->ownerSession->WorldScene().battle.allEntries[successfulSkillPosition];
            const std::int32_t successfulSkill = scene->ownerSession->WorldScene().battle.mSkill;
            successfulSkillActor->payload.battleActor->castSkill103C = successfulSkill;
            const std::int32_t currentSkill = scene->ownerSession->WorldScene().battle.mSkill;
            const std::int32_t type = currentSkill >= BattleSkill_StatusChangePoison
                && currentSkill <= BattleSkill_StatusChangeForgetSuper
                ? (currentSkill - BattleSkill_StatusChangePoison) % CG_BATTLE_DEBUFF_TYPE_COUNT + CG_BATTLE_DEBUFF_POISON
                : CG_BATTLE_DEBUFF_NONE;
            YobiAction* const target = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition];
            auto* const targetPayload = target->payload.battleActor;
            if (targetPayload->currentDebuffState_13CC != CG_BATTLE_STATUS_MODE_DEBUFF || targetPayload->currentDebuffType_13D0 != type) {
                AppendBattleCast_4392A0(scene, target, BattleAction_ClearDebuff);
            }
            targetPayload->currentDebuffType_13D0 = type;
            targetPayload->currentDebuffState_13CC = CG_BATTLE_STATUS_MODE_DEBUFF;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ApplyDebuff);
        } else if (skill == BattleSkill_ReverseType) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayAttributeReversalEffect);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ToggleAttributeReversal);
            }
        } else if (skill == BattleSkill_Revive) {
            const std::int32_t amount = TakeBattleMInteger_438F70(scene);
            ResetBattleCastList_4391E0(*scene->ownerSession, scene->ownerSession->WorldScene().battle.mTargetPosition);
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->phase = BattleAction_Idle;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayReviveEffect);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) != 0) {
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->motionMagnitude54 = 8.0F;
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ResetDeathState);
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ApplyResourceChangeAndWait);
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor
                    ->value1030 = amount;
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor
                    ->numberDisplayType1034 = BattleNumber_IncreaseHp;
            }
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_FaceFormation);
        } else if (skill >= BattleSkill_DefUp && skill <= BattleSkill_AglDown) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayParameterChangeEffect);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) == 0) {
                continue;
            }
            const std::int32_t successfulSkillPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
            YobiAction* const successfulSkillActor = scene->ownerSession->WorldScene().battle.allEntries[successfulSkillPosition];
            const std::int32_t successfulSkill = scene->ownerSession->WorldScene().battle.mSkill;
            successfulSkillActor->payload.battleActor->castSkill103C = successfulSkill;
            switch (scene->ownerSession->WorldScene().battle.mSkill) {
            case BattleSkill_DefUp: statusType = CG_BATTLE_PARAMETER_EFFECT_DEFENSE_UP; break;
            case BattleSkill_DefDown: statusType = CG_BATTLE_PARAMETER_EFFECT_DEFENSE_DOWN; break;
            case BattleSkill_AtkUp: statusType = CG_BATTLE_PARAMETER_EFFECT_ATTACK_UP; break;
            case BattleSkill_AtkDown: statusType = CG_BATTLE_PARAMETER_EFFECT_ATTACK_DOWN; break;
            case BattleSkill_AglUp: statusType = CG_BATTLE_PARAMETER_EFFECT_AGILITY_UP; break;
            case BattleSkill_AglDown: statusType = CG_BATTLE_PARAMETER_EFFECT_AGILITY_DOWN; break;
            case BattleSkill_RecoveryUp: statusType = CG_BATTLE_PARAMETER_EFFECT_RECOVERY_UP; break;
            default: break;
            }
            auto* const targetPayload =
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor;
            if (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_PARAMETER || targetPayload->statusEffectType13DC != statusType) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ClearStatusEffect);
            }
            targetPayload->statusEffectType13DC = statusType;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_PARAMETER;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ApplyStatusEffect);
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_Wait60Ticks);
        } else if (skill == BattleSkill_Death) {
            const std::int32_t technique = scene->ownerSession->WorldScene().battle.mTechnique / 100;
            const std::int32_t state = technique == 2053 ? BattleAction_PlayDeathScytheTechnique2053Effect : technique == 2054 ? BattleAction_PlayDeathScytheTechnique2054Effect : BattleAction_PlayInstantDeathEffect;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], state);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_MarkDead);
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_SetHpZero);
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_HandleDeath);
            }
        } else if (skill == BattleSkill_Concentration || (skill >= BattleSkill_ElementalAmplification && skill <= BattleSkill_BideTime)
            || skill == BattleSkill_Adrenaline) {
            if (skill == BattleSkill_VoodooCurse) {
                const auto output = std::format(L"performIndex = {}", scene->ownerSession->WorldScene().battle.mSkillLevelIndex);
                ::OutputDebugStringW(output.c_str());
                if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) == 0) {
                    continue;
                }
            }
            std::int32_t row = 0;
            switch (skill) {
            case BattleSkill_ElementalAmplification: row = 1; break;
            case BattleSkill_Blessing: row = 2; break;
            case BattleSkill_DefensiveBarrier: row = 3; break;
            case BattleSkill_VoodooCurse: row = 4; break;
            case BattleSkill_BideTime: row = 5; break;
            case BattleSkill_BattleCommand: row = 6; break;
            case BattleSkill_Adrenaline: row = 7; break;
            default: break;
            }
            const std::int32_t band = GetBattleValueBand_43BF00(scene->ownerSession->WorldScene().battle.mSkillLevelIndex + 1);
            const std::int32_t bandTargetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
            if (skill == BattleSkill_Concentration || skill == BattleSkill_Blessing || skill == BattleSkill_VoodooCurse
                || skill == BattleSkill_Adrenaline) {
                const std::int32_t image = kBattleMStatusBandImages[row][band - 1];
                YobiAction* const bandTarget = scene->ownerSession->WorldScene().battle.allEntries[bandTargetPosition];
                bandTarget->payload.battleActor->image102C = image;
            } else {
                YobiAction* const bandTarget = scene->ownerSession->WorldScene().battle.allEntries[bandTargetPosition];
                auto* const bandPayload = bandTarget->payload.battleActor;
                const std::int32_t image = kBattleMStatusBandImages[row][band - 1];
                bandPayload->image102C = image;
            }
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayConfiguredEffect42);
            if (skill == BattleSkill_Blessing || skill == BattleSkill_VoodooCurse || (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) == 0) {
                continue;
            }
            const std::int32_t type = skill == BattleSkill_Concentration ? CG_BATTLE_EXT_EFFECT_SKILL_2006
                : skill == BattleSkill_ElementalAmplification ? CG_BATTLE_EXT_EFFECT_SKILL_2010
                : skill == BattleSkill_DefensiveBarrier ? CG_BATTLE_EXT_EFFECT_SKILL_2012
                : skill == BattleSkill_BattleCommand ? CG_BATTLE_EXT_EFFECT_SKILL_2014
                : skill == BattleSkill_BideTime ? CG_BATTLE_EXT_EFFECT_SKILL_2015 : CG_BATTLE_EXT_EFFECT_SKILL_2027;
            auto* const targetPayload =
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor;
            const bool replaceState = skill != BattleSkill_BideTime && skill != BattleSkill_Adrenaline;
            if (replaceState && (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_EXTENDED || targetPayload->statusEffectType13DC != type)) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ClearStatusEffect);
            }
            targetPayload->statusEffectType13DC = type;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_EXTENDED;
            if (replaceState) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ApplyStatusEffect);
            }
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_Wait60Ticks);
        } else if ((skill >= 2050 && skill <= 2052) || (skill >= 2058 && skill <= 2067)
            || (skill >= 2089 && skill <= 2095) || (skill >= 3002 && skill <= 3016)) {
            std::int32_t image;
            std::int32_t type;
            bool useDebuffActions = false;
            if (skill >= 2050 && skill <= 2052) {
                image = kBattleMStatusImages54D864[skill - 2050][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = skill - 2050 + CG_BATTLE_EXT_EFFECT_SKILL_2050;
            } else if (skill >= 2058 && skill <= 2067) {
                image = kBattleMStatusImages54D738[skill - 2058][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = (skill - 2058) / 2 + CG_BATTLE_EXT_EFFECT_SKILL_2058_2059;
            } else if (skill == 2089) {
                image = kBattleMStatusImages54D738[13][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2089;
                useDebuffActions = true;
            } else if (skill == 2090) {
                image = kBattleMStatusImages54D738[14][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2090;
                useDebuffActions = true;
            } else if (skill == 2091) {
                image = kBattleMStatusImages54D738[10][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2091;
            } else if (skill == 2092 || skill == 2093) {
                image = kBattleMStatusImages54D738[11][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2092_2093;
            } else if (skill == 2094 || skill == 2095) {
                image = kBattleMStatusImages54D738[12][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2094_2095;
            } else {
                image = kBattleMStatusImages54D864[skill - 3002 + 3][scene->ownerSession->WorldScene().battle.mSkillLevelIndex];
                type = skill - 3002 + CG_BATTLE_EXT_EFFECT_SKILL_3002;
            }
            scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor
                ->image102C = image;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayConfiguredEffect42);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Success) == 0) {
                continue;
            }
            auto* const targetPayload =
                scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition]->payload.battleActor;
            if (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_EXTENDED || targetPayload->statusEffectType13DC != type) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition],
                    useDebuffActions ? BattleAction_ClearDebuff : BattleAction_ClearStatusEffect);
            }
            targetPayload->statusEffectType13DC = type;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_EXTENDED;
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition],
                useDebuffActions ? BattleAction_ApplyDebuff : BattleAction_ApplyStatusEffect);
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_Wait60Ticks);
        }
    }
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], -1, 0, 0);
    MoveBattleCastState_439380(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原5A4A30仅本生产段六处读写；入口覆盖，不属于场景13项恢复/回存。
static std::int32_t gBattleMHalfValue5A4A30 = 0;

// 原43412E/434130..434EDF；常规入口传entryValue=0，3037保留原EDI=1。
[[maybe_unused]] void ProcessBattleMSkill19Family(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t fpUsed, const std::int32_t entryValue,
    const std::uint32_t flags9C, const std::int32_t feedbackAlternateImageA0)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    std::int32_t halfApplied = entryValue;
    payload->completedEntryCount00 = entryValue;
    payload->entryCount04 = entryValue;
    scene->ownerSession->WorldScene().battle.mMessageValue = entryValue;
    gBattleMHalfValue5A4A30 = entryValue;
    const std::int32_t parsedCaster = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    scene->ownerSession->WorldScene().battle.mCasterPosition = parsedCaster;
    YobiAction* const caster = scene->ownerSession->WorldScene().battle.allEntries[parsedCaster];
    if (reinterpret_cast<std::uintptr_t>(caster) == static_cast<std::uintptr_t>(entryValue)) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    casterPayload->castSkill103C = scene->ownerSession->WorldScene().battle.mSkill;
    const std::int32_t casterLevelPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
    YobiAction* const casterLevelActor = scene->ownerSession->WorldScene().battle.allEntries[casterLevelPosition];
    const std::int32_t casterLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    casterLevelActor->payload.battleActor->castSkillLevel1044 = casterLevel;
    scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ConsumeFp);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_FaceFormation);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_CastSpellAndTriggerTargets);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_SetIdlePose);
    const std::int32_t level = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    if (level < 12) {
        const std::int32_t casterImagePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t offset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = offset + 110052;
        YobiAction* const casterImageActor = scene->ownerSession->WorldScene().battle.allEntries[casterImagePosition];
        casterImageActor->payload.battleActor->image1028 = image;
    } else {
        YobiAction* const casterImageActor = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition];
        casterImageActor->payload.battleActor->image1028 = 111001;
    }
    std::int32_t* const effectImage =
        &scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor->image102C;
    switch (scene->ownerSession->WorldScene().battle.mSkill) {
    case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicEarthStrong: case BattleSkill_AttackMagicEarthSuper: {
        const std::int32_t index = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110032 : 111002;
        break;
    }
    case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicWaterStrong: case BattleSkill_AttackMagicWaterSuper: {
        const std::int32_t index = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110020 : 111005;
        break;
    }
    case BattleSkill_AttackMagicFire: case BattleSkill_AttackMagicFireStrong: case BattleSkill_AttackMagicFireSuper: {
        const std::int32_t index = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110140 : 111008;
        break;
    }
    case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicWindSuper: {
        const std::int32_t index = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        *effectImage = index < 12 ? kBattleMSkillEffectOffsets553E58[index] + 110044 : 111011;
        break;
    }
    case BattleSkill_Dorain: case 3037: {
        const std::int32_t index = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        *effectImage = index < 12 ? kBattleMSkillEffectOffsets553E58[index] + 110063 : 111012;
        break;
    }
    case 2055: *effectImage = 111164; break;
    case 2056: *effectImage = 111165; break;
    case 2057: *effectImage = 111166; break;
    case 2068: case 2070: *effectImage = 111177; break;
    case 2069: *effectImage = 111178; break;
    case 2071: *effectImage = 111183; break;
    case 2072: *effectImage = 111184; break;
    case 2073: *effectImage = 111185; break;
    case 2074: case 2076: *effectImage = 111186; break;
    case 2075: *effectImage = 111187; break;
    case 2077: case 2079: *effectImage = 111189; break;
    case 2078: *effectImage = 111190; break;
    default: break;
    }
    bool seenCaster = false;
    std::int32_t lastAmount;
    std::int32_t reflectedSecondImage = std::bit_cast<std::int32_t>(flags9C);
    std::int32_t reflectedAlternateImage = feedbackAlternateImageA0;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTarget = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetFinished = parsedTarget == 255;
        scene->ownerSession->WorldScene().battle.mTargetPosition = parsedTarget;
        if (targetFinished) {
            break;
        }
        if (parsedTarget == scene->ownerSession->WorldScene().battle.mCasterPosition) {
            seenCaster = true;
        }
        YobiAction* const parsedTargetActor = scene->ownerSession->WorldScene().battle.allEntries[parsedTarget];
        if (parsedTargetActor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        scene->ownerSession->WorldScene().battle.mResultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        lastAmount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, BattleMAuxGraphicsReadMode_ReadObjectFields, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        if ((flags & BattleResultFlag_ReflectMagic) != 0) {
            const std::int32_t previousTotal = scene->ownerSession->WorldScene().battle.mMessageValue;
            const std::int32_t target = scene->ownerSession->WorldScene().battle.mTargetPosition;
            scene->ownerSession->WorldScene().battle.mMessageValue = previousTotal + lastAmount;
            scene->ownerSession->WorldScene().battle.mFeedbackEffectImage = firstImage;
            reflectedSecondImage = secondImage;
            scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition = target;
            scene->ownerSession->WorldScene().battle.mFlags = flags;
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            reflectedAlternateImage = firstImages[imageIndex];
            flags &= BattleResultMask_WithoutDeathInjuryOrEquipmentResults;
            scene->ownerSession->WorldScene().battle.mResultFlags = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        const std::int32_t effectTarget = scene->ownerSession->WorldScene().battle.mTargetPosition;
        const std::int32_t effectCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const effectCasterActor = scene->ownerSession->WorldScene().battle.allEntries[effectCaster];
        AppendBattleEffect_439400(effectCasterActor, {effectTarget, flags, lastAmount, firstImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t selectedImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = scene->ownerSession->WorldScene().battle.mResultFlags;
        const std::int32_t auxiliaryTarget = scene->ownerSession->WorldScene().battle.mTargetPosition;
        const std::int32_t auxiliaryCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const auxiliaryActor = scene->ownerSession->WorldScene().battle.allEntries[auxiliaryCaster];
        AppendBattleAuxEffects_43BC10(auxiliaryActor, auxiliaryTarget, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, selectedImage);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_Idle);
        flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        if ((flags & BattleResultMask_AvoidanceOrFlashDodge) == 0) {
            if ((flags & BattleResultFlag_BounceOut) != 0 && (flags & BattleResultFlag_ReflectMagic) == 0) {
                const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
                std::int32_t targetPosition;
                if (skill == BattleSkill_Dorain || skill == 3037) {
                    if ((flags & BattleResultMask_SpecialOrMagicGuard) != 0) {
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayGuardAnimation);
                    }
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_BounceOutAfterKnockout);
                    std::int32_t sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                    targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                    if (sourcePosition != targetPosition) {
                        const std::uint32_t effectListFlags = scene->ownerSession->WorldScene().battle.mResultFlags;
                        YobiAction* const effectListActor = scene->ownerSession->WorldScene().battle.allEntries[targetPosition];
                        AppendBattleEffectList_4393A0(
                            effectListActor, sourcePosition, effectListFlags, lastAmount);
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_WaitTargetReady);
                        targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                    }
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], BattleAction_AdvanceLinkedCast);
                    sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                    targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                    if (sourcePosition != targetPosition) {
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], BattleAction_AdvanceEffectAndCast);
                        sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                        targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                    }
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[sourcePosition], BattleAction_Idle);
                    const std::uint8_t halfFlags =
                        *reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mResultFlags);
                    targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                    sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                    if ((halfFlags & BattleResultFlag_Death) == 0 || sourcePosition != targetPosition) {
                        if ((sourcePosition == 20 && targetPosition < 10)
                            || (sourcePosition == 21 && targetPosition >= 10)) {
                            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[sourcePosition], BattleAction_PlayMagicAttackEffectAndFadeIn);
                            targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                        }
                        halfApplied = 1;
                        gBattleMHalfValue5A4A30 += lastAmount / 2;
                    }
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_BounceOutAfterKnockout);
                    targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                }
                const bool currentTarget = targetPosition == scene->ownerSession->WorldScene().battle.currentPosition;
                if (currentTarget) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], BattleAction_RequestBattleExit);
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], BattleAction_DismissPlayerPet);
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_RemoveFromBattle);
                }
                flags = scene->ownerSession->WorldScene().battle.mResultFlags;
            } else if (lastAmount != 0 && (flags & BattleResultMask_NormalOrCritical) != 0) {
                if ((flags & BattleResultFlag_NullifyMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicNullifyEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_AbsorbMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicAbsorbEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_ReflectMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicReflectEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_Barrier) == 0) {
                    const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
                    if (skill != BattleSkill_Dorain && skill != 3037) {
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition],
                            (flags & BattleResultFlag_Critical) != 0 ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit);
                        flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                    } else {
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition],
                            (flags & BattleResultMask_SpecialOrMagicGuard) != 0 ? BattleAction_PlayGuardAnimation : BattleAction_PlayHitAnimation);
                        std::int32_t sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                        std::int32_t targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                        if (sourcePosition != targetPosition) {
                            const std::uint32_t effectListFlags = scene->ownerSession->WorldScene().battle.mResultFlags;
                            YobiAction* const effectListActor = scene->ownerSession->WorldScene().battle.allEntries[targetPosition];
                            AppendBattleEffectList_4393A0(
                                effectListActor, sourcePosition, effectListFlags, lastAmount);
                            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_WaitTargetReady);
                            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_AdvanceLinkedCast);
                            sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                            targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                            if (sourcePosition != targetPosition) {
                                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[targetPosition], BattleAction_AdvanceEffectAndCast);
                                sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                                targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                            }
                        }
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[sourcePosition], BattleAction_Idle);
                        flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                        sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                        targetPosition = scene->ownerSession->WorldScene().battle.mTargetPosition;
                        if ((flags & BattleResultFlag_Death) == 0 || sourcePosition != targetPosition) {
                            if ((sourcePosition == 20 && targetPosition < 10)
                                || (sourcePosition == 21 && targetPosition >= 10)) {
                                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[sourcePosition], BattleAction_PlayMagicAttackEffectAndFadeIn);
                                flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                            }
                            halfApplied = 1;
                            gBattleMHalfValue5A4A30 += lastAmount / 2;
                        }
                    }
                }
            }
        }
        if ((lastAmount != 0 || (flags & BattleResultMask_AvoidanceOrFlashDodge) != 0) && (flags & BattleResultMask_MagicReactionOrBarrier) == 0) {
            const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
            if (skill != BattleSkill_Dorain && skill != 3037) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ReturnToFormation);
                flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                if ((flags & BattleResultFlag_FlashDodge) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_DisableFlash);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                }
            }
        }
        if ((flags & BattleResultFlag_Death) != 0 && (flags & BattleResultFlag_ReflectMagic) == 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_HandleDeath);
        }
    }
    if (halfApplied == 1) {
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_PlayMagicAttackEffect);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ApplyResourceChangeAndWait);
        const std::int32_t halfCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const halfCasterActor = scene->ownerSession->WorldScene().battle.allEntries[halfCaster];
        const std::int32_t halfAmount = gBattleMHalfValue5A4A30;
        halfCasterActor->payload.battleActor->value1030 = halfAmount;
        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
            ->numberDisplayType1034 = BattleNumber_IncreaseHp;
    }
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], -1, 0, 0);
    if (scene->ownerSession->WorldScene().battle.mMessageValue != 0) {
        const std::int32_t reflectedSkillPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const reflectedSkillActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedSkillPosition];
        const std::int32_t reflectedSkill = scene->ownerSession->WorldScene().battle.mSkill;
        reflectedSkillActor->payload.battleActor->castSkill103C = reflectedSkill;
        const std::int32_t reflectedLevelPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const reflectedLevelActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedLevelPosition];
        const std::int32_t reflectedLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
        reflectedLevelActor->payload.battleActor->castSkillLevel1044 = reflectedLevel;
        const std::int32_t imageSourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const imageSourceActor = scene->ownerSession->WorldScene().battle.allEntries[imageSourcePosition];
        const std::int32_t imageReflectedPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        auto* const sourcePayload = imageSourceActor->payload.battleActor;
        YobiAction* const imageReflectedActor = scene->ownerSession->WorldScene().battle.allEntries[imageReflectedPosition];
        auto* const reflectedPayload = imageReflectedActor->payload.battleActor;
        reflectedPayload->image102C = sourcePayload->image102C;
        scene->ownerSession->WorldScene().battle.mFlags &= ~BattleResultFlag_ReflectMagic;
        if (seenCaster) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_Wait60Ticks);
        }
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_WaitIdle);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_TriggerSpellEffectOrHealTarget);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_Idle);
        const std::uint8_t reflectedFlags =
            *reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags);
        if ((reflectedFlags & BattleResultFlag_Avoidance) == 0) {
            if ((reflectedFlags & BattleResultFlag_BounceOut) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_BounceOutAfterKnockout);
                const std::int32_t reflectedCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                const bool currentCaster = reflectedCasterPosition == scene->ownerSession->WorldScene().battle.currentPosition;
                if (currentCaster) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[reflectedCasterPosition], BattleAction_RequestBattleExit);
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[reflectedCasterPosition], BattleAction_DismissPlayerPet);
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_RemoveFromBattle);
                }
            } else if (lastAmount != 0) {
                const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
                if (skill != BattleSkill_Dorain && skill != 3037) {
                    if ((reflectedFlags & BattleResultMask_NormalOrCritical) != 0) {
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition],
                            (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_Critical) != 0 ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit);
                    }
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition],
                        (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultMask_SpecialOrMagicGuard) != 0 ? BattleAction_PlayGuardAnimation : BattleAction_PlayHitAnimation);
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_SetIdlePose);
                    const std::uint8_t reflectedHalfFlags =
                        *reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags);
                    if ((reflectedHalfFlags & BattleResultFlag_Death) == 0) {
                        const std::int32_t sourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                        const bool sameSide = (sourcePosition == 20 && scene->ownerSession->WorldScene().battle.mTargetPosition < 10)
                            || (sourcePosition == 21 && scene->ownerSession->WorldScene().battle.mTargetPosition >= 10);
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[sourcePosition], sameSide ? BattleAction_PlayMagicAttackEffectAndFadeIn : BattleAction_PlayMagicAttackEffect);
                        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ApplyResourceChangeAndWait);
                        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                            ->value1030 = lastAmount / 2;
                        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
                            ->numberDisplayType1034 = BattleNumber_IncreaseHp;
                    }
                }
            }
        }
        const std::int32_t reflectedEffectImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
        const std::int32_t reflectedEffectAmount = scene->ownerSession->WorldScene().battle.mMessageValue;
        const std::uint32_t reflectedEffectFlags = scene->ownerSession->WorldScene().battle.mFlags;
        const std::int32_t reflectedEffectCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t reflectedEffectSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const reflectedEffectActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedEffectSource];
        AppendBattleEffect_439400(reflectedEffectActor,
            {reflectedEffectCaster, reflectedEffectFlags, reflectedEffectAmount, reflectedEffectImage});
        const std::int32_t reflectedAuxSecondImage = reflectedSecondImage;
        const std::int32_t reflectedAuxFirstImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
        const std::uint32_t reflectedAuxFlags = scene->ownerSession->WorldScene().battle.mFlags;
        const std::int32_t reflectedAuxCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t reflectedAuxSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const reflectedAuxActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedAuxSource];
        AppendBattleAuxEffects_43BC10(reflectedAuxActor, reflectedAuxCaster, reflectedAuxFlags,
            reflectedAuxFirstImage, reflectedAuxSecondImage, reflectedAlternateImage);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_AdvanceEffectAndCast);
        if (lastAmount != 0 || (*reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags) & BattleResultFlag_Avoidance) != 0
            || (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_FlashDodge) != 0) {
            const std::int32_t skill = scene->ownerSession->WorldScene().battle.mSkill;
            if (skill != BattleSkill_Dorain && skill != 3037) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ReturnToFormation);
                if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_FlashDodge) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_DisableFlash);
                }
            }
        }
        if ((*reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags) & BattleResultFlag_Death) != 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_HandleDeath);
        }
    }
    MoveBattleCastState_439380(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = BattleMSkillPhase_WaitForEntryReady;
}

// 原434EDF..43573F；267保留继承累计值，每目标累计效果与最终累计尾分别生产。
[[maybe_unused]] void ProcessBattleMSkill267(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed,
    const std::uint32_t flags9C, const std::int32_t feedbackAlternateImageA0)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCaster = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    scene->ownerSession->WorldScene().battle.mCasterPosition = parsedCaster;
    YobiAction* const caster = scene->ownerSession->WorldScene().battle.allEntries[parsedCaster];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    casterPayload->castSkill103C = scene->ownerSession->WorldScene().battle.mSkill;
    const std::int32_t casterLevelPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
    YobiAction* const casterLevelActor = scene->ownerSession->WorldScene().battle.allEntries[casterLevelPosition];
    const std::int32_t casterLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    casterLevelActor->payload.battleActor->castSkillLevel1044 = casterLevel;
    scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ConsumeFp);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_FaceFormation);
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_PrepareArcShots);
    const std::int32_t effectCountLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    const std::int32_t effectCountPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
    const std::int32_t effectOffset = kBattleMSkillEffectOffsets553E58[effectCountLevel];
    YobiAction* const effectCountActor = scene->ownerSession->WorldScene().battle.allEntries[effectCountPosition];
    const std::int32_t effectCount = 4 - effectOffset;
    effectCountActor->payload.battleActor->image102C = effectCount;
    const std::int32_t level = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
    if (level < 12) {
        const std::int32_t casterImagePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t offset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = offset + 110052;
        YobiAction* const casterImageActor = scene->ownerSession->WorldScene().battle.allEntries[casterImagePosition];
        casterImageActor->payload.battleActor->image1028 = image;
    } else {
        YobiAction* const casterImageActor = scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition];
        casterImageActor->payload.battleActor->image1028 = 111001;
    }
    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_SetIdlePose);
    scene->ownerSession->WorldScene().battle.mValue5ABA7C = 0;
    scene->ownerSession->WorldScene().battle.mFlags = 0;
    std::int32_t lastAmount;
    std::int32_t accumulatedSecondImage = std::bit_cast<std::int32_t>(flags9C);
    std::int32_t accumulatedAlternateImage = feedbackAlternateImageA0;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTarget = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetFinished = parsedTarget == 255;
        scene->ownerSession->WorldScene().battle.mTargetPosition = parsedTarget;
        if (targetFinished) {
            break;
        }
        YobiAction* const parsedTargetActor = scene->ownerSession->WorldScene().battle.allEntries[parsedTarget];
        if (parsedTargetActor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        scene->ownerSession->WorldScene().battle.mResultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        lastAmount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, BattleMAuxGraphicsReadMode_ReadObjectFields, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        if ((flags & BattleResultMask_Reflection) != 0) {
            const std::uint32_t savedFlags = scene->ownerSession->WorldScene().battle.mFlags;
            const std::int32_t previousTotal = scene->ownerSession->WorldScene().battle.mMessageValue;
            const std::int32_t target = scene->ownerSession->WorldScene().battle.mTargetPosition;
            accumulatedSecondImage = secondImage;
            const std::uint32_t combinedFlags = savedFlags | flags;
            const std::int32_t total = previousTotal + lastAmount;
            scene->ownerSession->WorldScene().battle.mFlags = combinedFlags;
            scene->ownerSession->WorldScene().battle.mMessageValue = total;
            scene->ownerSession->WorldScene().battle.mFeedbackEffectImage = firstImage;
            scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition = target;
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            accumulatedAlternateImage = firstImages[imageIndex];
            flags &= BattleResultMask_WithoutDeathInjuryOrEquipmentResults;
            scene->ownerSession->WorldScene().battle.mResultFlags = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        const std::int32_t effectTarget = scene->ownerSession->WorldScene().battle.mTargetPosition;
        const std::int32_t effectCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const effectCasterActor = scene->ownerSession->WorldScene().battle.allEntries[effectCaster];
        AppendBattleEffect_439400(effectCasterActor, {effectTarget, flags, lastAmount, firstImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t selectedImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = scene->ownerSession->WorldScene().battle.mResultFlags;
        const std::int32_t auxiliaryTarget = scene->ownerSession->WorldScene().battle.mTargetPosition;
        const std::int32_t auxiliaryCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const auxiliaryActor = scene->ownerSession->WorldScene().battle.allEntries[auxiliaryCaster];
        AppendBattleAuxEffects_43BC10(auxiliaryActor, auxiliaryTarget, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, selectedImage);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_Idle);
        flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        if ((flags & BattleResultMask_AvoidanceOrFlashDodge) == 0) {
            if ((flags & BattleResultFlag_BounceOut) != 0 && (flags & BattleResultMask_Reflection) == 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_BounceOutAfterKnockout);
                const std::int32_t responseTarget = scene->ownerSession->WorldScene().battle.mTargetPosition;
                const bool currentTarget = responseTarget == scene->ownerSession->WorldScene().battle.currentPosition;
                if (currentTarget) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[responseTarget], BattleAction_RequestBattleExit);
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[responseTarget], BattleAction_DismissPlayerPet);
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_RemoveFromBattle);
                }
                flags = scene->ownerSession->WorldScene().battle.mResultFlags;
            } else if (lastAmount != 0 && (flags & BattleResultMask_NormalOrCritical) != 0) {
                if ((flags & BattleResultFlag_NullifyPhysical) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayPhysicalNullifyEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_NullifyMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicNullifyEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_AbsorbPhysical) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayPhysicalAbsorbEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_AbsorbMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicAbsorbEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_ReflectPhysical) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayPhysicalReflectEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_ReflectMagic) != 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_PlayMagicReflectEffect);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                } else if ((flags & BattleResultFlag_Barrier) == 0) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition],
                        (flags & BattleResultFlag_Critical) != 0 ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit);
                    flags = scene->ownerSession->WorldScene().battle.mResultFlags;
                }
            }
        }
        if ((lastAmount != 0 || (flags & BattleResultMask_AvoidanceOrFlashDodge) != 0) && (flags & BattleResultMask_AnyReactionOrBarrier) == 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_ReturnToFormation);
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
            if ((flags & BattleResultFlag_FlashDodge) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_DisableFlash);
                flags = scene->ownerSession->WorldScene().battle.mResultFlags;
            }
        }
        if ((flags & BattleResultFlag_Death) != 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_HandleDeath);
            flags = scene->ownerSession->WorldScene().battle.mResultFlags;
        }
        if ((flags & BattleResultMask_Reflection) != 0) {
            const std::int32_t reflectedSkillPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
            YobiAction* const reflectedSkillActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedSkillPosition];
            const std::int32_t reflectedSkill = scene->ownerSession->WorldScene().battle.mSkill;
            reflectedSkillActor->payload.battleActor->castSkill103C = reflectedSkill;
            const std::int32_t reflectedLevelPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
            YobiAction* const reflectedLevelActor = scene->ownerSession->WorldScene().battle.allEntries[reflectedLevelPosition];
            const std::int32_t reflectedLevel = scene->ownerSession->WorldScene().battle.mSkillLevelIndex;
            reflectedLevelActor->payload.battleActor->castSkillLevel1044 = reflectedLevel;
            const std::int32_t imageSourcePosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            YobiAction* const imageSourceActor = scene->ownerSession->WorldScene().battle.allEntries[imageSourcePosition];
            const std::int32_t imageAccumulatedPosition = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
            auto* const sourcePayload = imageSourceActor->payload.battleActor;
            YobiAction* const imageAccumulatedActor = scene->ownerSession->WorldScene().battle.allEntries[imageAccumulatedPosition];
            auto* const accumulatedPayload = imageAccumulatedActor->payload.battleActor;
            accumulatedPayload->image102C = sourcePayload->image102C;
            const std::uint32_t accumulatedEffectFlags = scene->ownerSession->WorldScene().battle.mFlags;
            const std::int32_t accumulatedEffectImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
            const std::int32_t accumulatedEffectAmount = scene->ownerSession->WorldScene().battle.mMessageValue;
            const std::uint32_t clearedEffectFlags = accumulatedEffectFlags & ~BattleResultMask_Reflection;
            const std::int32_t accumulatedEffectSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
            scene->ownerSession->WorldScene().battle.mFlags = clearedEffectFlags;
            const std::int32_t accumulatedEffectCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
            YobiAction* const accumulatedEffectActor = scene->ownerSession->WorldScene().battle.allEntries[accumulatedEffectSource];
            AppendBattleEffect_439400(accumulatedEffectActor,
                {accumulatedEffectCaster, clearedEffectFlags, accumulatedEffectAmount, accumulatedEffectImage});
            const std::int32_t accumulatedAuxImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
            const std::uint32_t accumulatedAuxFlags = scene->ownerSession->WorldScene().battle.mFlags;
            const std::int32_t accumulatedAuxCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
            const std::int32_t accumulatedAuxSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
            YobiAction* const accumulatedAuxActor = scene->ownerSession->WorldScene().battle.allEntries[accumulatedAuxSource];
            AppendBattleAuxEffects_43BC10(accumulatedAuxActor, accumulatedAuxCaster,
                accumulatedAuxFlags, accumulatedAuxImage, accumulatedSecondImage, accumulatedAlternateImage);
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_FireArcShotsTogether);
            AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], -1, 0, 0);
            const bool repeatedAccumulatedCast = scene->ownerSession->WorldScene().battle.mValue5ABA7C != 0;
            const std::int32_t accumulatedCastPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
            YobiAction* const accumulatedCastActor = scene->ownerSession->WorldScene().battle.allEntries[accumulatedCastPosition];
            AppendBattleCast_4392A0(scene, accumulatedCastActor, repeatedAccumulatedCast ? BattleAction_RecoilOnHit : BattleAction_WaitIdle);
            scene->ownerSession->WorldScene().battle.mValue5ABA7C = 1;
        }
    }
    AppendBattleEffectList_4393A0(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], -1, 0, 0);
    if (scene->ownerSession->WorldScene().battle.mMessageValue != 0) {
        if ((*reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags) & BattleResultFlag_Death) != 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_MarkDeadAndAcquireGate);
        }
        const std::int32_t amountCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        YobiAction* const amountCasterActor = scene->ownerSession->WorldScene().battle.allEntries[amountCaster];
        const std::int32_t accumulatedAmount = scene->ownerSession->WorldScene().battle.mMessageValue;
        amountCasterActor->payload.battleActor->value1030 = accumulatedAmount;
        scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]->payload.battleActor
            ->numberDisplayType1034 = BattleNumber_DecreaseHp;
        std::uint32_t flags = scene->ownerSession->WorldScene().battle.mFlags;
        if ((flags & BattleResultFlag_RebirthGuard) == 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ApplyResourceChange);
            flags = scene->ownerSession->WorldScene().battle.mFlags;
        }
        if ((flags & BattleResultFlag_Avoidance) == 0) {
            if ((flags & BattleResultFlag_BounceOut) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_BounceOutAfterKnockout);
                const std::int32_t reflectedCasterPosition = scene->ownerSession->WorldScene().battle.mCasterPosition;
                const bool currentCaster = reflectedCasterPosition == scene->ownerSession->WorldScene().battle.currentPosition;
                if (currentCaster) {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[reflectedCasterPosition], BattleAction_RequestBattleExit);
                } else {
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[reflectedCasterPosition], BattleAction_DismissPlayerPet);
                    AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_RemoveFromBattle);
                }
                flags = scene->ownerSession->WorldScene().battle.mFlags;
            } else if ((flags & BattleResultFlag_RebirthGuard) == 0 && lastAmount != 0 && (flags & BattleResultMask_NormalOrCritical) != 0) {
                const std::uint32_t responseFlags = scene->ownerSession->WorldScene().battle.mResultFlags;
                const std::int32_t responseCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
                const std::int32_t responseState = (responseFlags & BattleResultFlag_Critical) != 0 ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit;
                YobiAction* const responseCasterActor = scene->ownerSession->WorldScene().battle.allEntries[responseCaster];
                AppendBattleCast_4392A0(scene, responseCasterActor, responseState);
                flags = scene->ownerSession->WorldScene().battle.mFlags;
            }
        }
        const std::int32_t finalEffectImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
        const std::int32_t finalEffectAmount = scene->ownerSession->WorldScene().battle.mMessageValue;
        const std::int32_t finalEffectCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t finalEffectSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const finalEffectActor = scene->ownerSession->WorldScene().battle.allEntries[finalEffectSource];
        AppendBattleEffect_439400(finalEffectActor,
            {finalEffectCaster, flags, finalEffectAmount, finalEffectImage});
        const std::int32_t finalAuxImage = scene->ownerSession->WorldScene().battle.mFeedbackEffectImage;
        const std::uint32_t finalAuxFlags = scene->ownerSession->WorldScene().battle.mFlags;
        const std::int32_t finalAuxCaster = scene->ownerSession->WorldScene().battle.mCasterPosition;
        const std::int32_t finalAuxSource = scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition;
        YobiAction* const finalAuxActor = scene->ownerSession->WorldScene().battle.allEntries[finalAuxSource];
        AppendBattleAuxEffects_43BC10(finalAuxActor, finalAuxCaster, finalAuxFlags,
            finalAuxImage, accumulatedSecondImage, accumulatedAlternateImage);
        AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mFeedbackSourcePosition], BattleAction_AdvanceEffectAndCast);
        flags = scene->ownerSession->WorldScene().battle.mFlags;
        if ((flags & BattleResultFlag_RebirthGuard) == 0
            && (lastAmount != 0 || (flags & BattleResultFlag_Avoidance) != 0 || (scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_FlashDodge) != 0)) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_ReturnToFormation);
            if ((scene->ownerSession->WorldScene().battle.mResultFlags & BattleResultFlag_FlashDodge) != 0) {
                AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mTargetPosition], BattleAction_DisableFlash);
            }
        }
        if ((*reinterpret_cast<const std::uint8_t*>(&scene->ownerSession->WorldScene().battle.mFlags) & BattleResultFlag_Death) != 0) {
            AppendBattleCast_4392A0(scene, scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition], BattleAction_HandleDeath);
        }
    }
    MoveBattleCastState_439380(scene->ownerSession->WorldScene().battle.allEntries[scene->ownerSession->WorldScene().battle.mCasterPosition]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = BattleMSkillPhase_WaitForTurnReady;
}

} // namespace detail

} // namespace cg::yobi
