#include "battle_actor.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_skill_effect.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "battle_attachment.hpp"
#include "battle_attack.hpp"
#include "battle_cast.hpp"
#include "battle_effect.hpp"
#include "battle_ride.hpp"
#include "battle_status.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/AlbumState.hpp"
#include "../world/BattleState.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

// 原[423800,4238EA)：固定角色记录，名称及四项属性均短路匹配。
std::int32_t MatchBattleAlbum_423800(const YobiAction* const action)
{
    const world::AlbumRecord* record = world::GetSelectedAlbumRecords();
    for (std::int32_t index = 0; index < 960; ++index, ++record) {
        if (record->used == 0) {
            continue;
        }
        if (record->name != action->nameC4) {
            continue;
        }
        std::size_t field = 0;
        for (; field < action->fields288.size(); ++field) {
            const std::int32_t value = record->attributes[field];
            if ((value != 0) != (action->fields288[field] != 0)) {
                break;
            }
        }
        if (field == action->fields288.size()) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x00438290。
YobiAction* NewBattleActor_438290(const std::int32_t position)
{
    const auto priority = static_cast<std::uint8_t>(static_cast<std::uint8_t>(position) * 10 + 2);
    YobiAction* const action = NewYobiAction_404AE0(priority, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleActor_404DB0;
    const std::uint32_t configuredFlags = flags | 0x60054u;
    action->imageNo = 100500;
    action->flags = configuredFlags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 3.0F;
    action->direction = 1;
    action->displayPriority = 9;
    const std::int32_t firstPointIndex = cg::world::gBattlePointIndices5536FC[position];
    auto* const payload = action->payload.battleActor;
    action->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[firstPointIndex][0]);
    action->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][1]);
    payload->x00 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2];
    payload->y04 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3];
    payload->x08 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][0];
    const std::int32_t y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][1];
    payload->battlePosition18 = position;
    payload->y0C = y;
    action->phase = BattleAction_Idle;
    action->pathCountOrDrawOrder_270 = 0;
    PlayFrame_517DA0(action, AnimationPlayback_Loop);
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
    return action;
}

// 原函数：0x00404DB0。
void UpdateBattleActor_404DB0(YobiAction* const action)
{
    UpdateLuaSkillEffect(action); // E10：原生257槽正文由接管器按原时序调用。
}

void UpdateNativeBattleActor(YobiAction* const action)
{
    const std::int32_t scene = world::gCurrentSceneState;
    const bool isSceneSeven = scene == 7;
    auto* const payload = action->payload.battleActor;
    if (isSceneSeven) {
        return;
    }
    switch (action->phase) {
    case BattleAction_Idle:
        static_cast<void>(UpdateBattleAction0_406650(action));
        break;
    case BattleAction_WaitIdle:
        static_cast<void>(UpdateBattleAction1_406720(action));
        break;
    case BattleAction_EnterFormation:
        static_cast<void>(UpdateBattleAction2_406760(action));
        break;
    case BattleAction_ApproachEffectTarget:
        static_cast<void>(UpdateBattleAction3_406C30(action));
        break;
    case BattleAction_CloseInOnEffectTarget:
        static_cast<void>(UpdateBattleAction4_406D70(action));
        break;
    case BattleAction_MoveForward16Ticks:
        static_cast<void>(UpdateBattleAction5_40CFA0(action));
        break;
    case BattleAction_MoveToTargetPosition:
        static_cast<void>(UpdateBattleAction6_406CD0(action));
        break;
    case BattleAction_PlayBeginRunFacingEffectTarget:
        static_cast<void>(UpdateBattleAction7_409410(action));
        break;
    case BattleAction_PlayBeginRunFacingMoveTarget:
        static_cast<void>(UpdateBattleAction8_4094A0(action));
        break;
    case BattleAction_MoveTowardTargetOrFormation:
        static_cast<void>(UpdateBattleAction9_409510(action));
        break;
    case BattleAction_PlayEndRunAndRestoreFacing:
        static_cast<void>(UpdateBattleAction10_409610(action));
        break;
    case BattleAction_ReturnToFormation:
        static_cast<void>(UpdateBattleAction11_4067F0(action));
        break;
    case BattleAction_PrepareEscape:
        static_cast<void>(UpdateBattleAction12_406930(action));
        break;
    case BattleAction_MoveToExitPoint:
        static_cast<void>(UpdateBattleAction13_406A80(action));
        break;
    case BattleAction_PerformMeleeAttack:
        static_cast<void>(UpdateBattleAction14_409680(action));
        break;
    case BattleAction_LaunchRangedAttack:
        static_cast<void>(UpdateBattleAction15_406E10(action));
        break;
    case BattleAction_ResolveProjectileHit:
        static_cast<void>(UpdateBattleAction16_4072E0(action));
        break;
    case BattleAction_MoveOutOfBounds:
        static_cast<void>(UpdateBattleAction17_4093B0(action));
        break;
    case BattleAction_FireBowShots:
        static_cast<void>(UpdateBattleAction18_408170(action));
        break;
    case BattleAction_FireThrowingShots:
        static_cast<void>(UpdateBattleAction19_4082F0(action));
        break;
    case BattleAction_ResolveCloseRangeHit:
        static_cast<void>(UpdateBattleAction20_408410(action));
        break;
    case BattleAction_MoveBoomerangToTurnPoint:
        static_cast<void>(UpdateBattleAction21_408750(action));
        break;
    case BattleAction_ResolveBoomerangHits:
        static_cast<void>(UpdateBattleAction22_4087C0(action));
        break;
    case BattleAction_ReturnBoomerang:
        static_cast<void>(UpdateBattleAction23_4090F0(action));
        break;
    case BattleAction_Wait21Ticks:
        static_cast<void>(UpdateBattleAction24_409380(action));
        break;
    case BattleAction_WaitTargetReady:
        static_cast<void>(UpdateBattleAction25_406BA0(action));
        break;
    case BattleAction_RecoilOnHit:
        static_cast<void>(UpdateBattleAction26_40A9E0(action));
        break;
    case BattleAction_ReactToHit:
        static_cast<void>(UpdateBattleAction27_40AA70(action));
        break;
    case BattleAction_ReactToHitWithSound:
        static_cast<void>(UpdateBattleAction28_40ACF0(action));
        break;
    case BattleAction_RecoilAndWait44Ticks:
        static_cast<void>(UpdateBattleAction29_40AE50(action));
        break;
    case BattleAction_RecoilAndWait14Or28Ticks:
        static_cast<void>(UpdateBattleAction30_40AF30(action));
        break;
    case BattleAction_DodgeAndWaitAttacker:
        static_cast<void>(UpdateBattleAction31_40B020(action));
        break;
    case BattleAction_PlayHitAnimation:
        static_cast<void>(UpdateBattleAction32_40B410(action));
        break;
    case BattleAction_PlayGuardAnimation:
        static_cast<void>(UpdateBattleAction33_40B450(action));
        break;
    case BattleAction_BounceOutAfterKnockout:
        static_cast<void>(UpdateBattleAction34_40B5E0(action));
        break;
    case BattleAction_FlyOutAfterKnockout:
        static_cast<void>(UpdateBattleAction35_40B930(action));
        break;
    case BattleAction_Wait30Ticks:
        static_cast<void>(UpdateBattleAction36_40BAD0(action));
        break;
    case BattleAction_Wait60Ticks:
        static_cast<void>(UpdateBattleAction37_40BB20(action));
        break;
    case BattleAction_Wait120Ticks:
        static_cast<void>(UpdateBattleAction38_40BB70(action));
        break;
    case BattleAction_AlternateHitAndGuardAnimations:
        static_cast<void>(UpdateBattleAction39_40BBC0(action));
        break;
    case BattleAction_FaceFormation:
        static_cast<void>(UpdateBattleAction40_40BD40(action));
        break;
    case BattleAction_FaceTargetFormationPoint:
        static_cast<void>(UpdateBattleAction41_40BD60(action));
        break;
    case BattleAction_FaceSelectedTarget:
        static_cast<void>(UpdateBattleAction42_40BDB0(action));
        break;
    case BattleAction_TrackOpponentFacing:
        static_cast<void>(UpdateBattleAction43_40BE30(action));
        break;
    case BattleAction_TurnTargetTowardSelf:
        static_cast<void>(UpdateBattleAction44_40BDF0(action));
        break;
    case BattleAction_SetHpZero:
        static_cast<void>(UpdateBattleAction45_40B130(action));
        break;
    case BattleAction_HandleDeath:
        static_cast<void>(UpdateBattleAction46_40B200(action));
        break;
    case BattleAction_MarkDead:
        static_cast<void>(UpdateBattleAction47_40B3F0(action));
        break;
    case BattleAction_FadeOutForRemoval:
        static_cast<void>(UpdateBattleAction48_40B490(action));
        break;
    case BattleAction_WaitRemovalEffect:
        static_cast<void>(UpdateBattleAction49_40B5B0(action));
        break;
    case BattleAction_PlayDeathAnimation:
        static_cast<void>(UpdateBattleAction50_40B3C0(action));
        break;
    case BattleAction_RemoveFromBattle:
        static_cast<void>(UpdateBattleAction51_40BE70(action));
        break;
    case BattleAction_DismissPlayerPet:
        static_cast<void>(UpdateBattleAction52_40BE90(action));
        break;
    case BattleAction_ShrinkOut:
        static_cast<void>(UpdateBattleAction53_40BF20(action));
        break;
    case BattleAction_GrowIn:
        static_cast<void>(UpdateBattleAction54_40BFE0(action));
        break;
    case BattleAction_ConsumeFp:
        static_cast<void>(UpdateBattleAction55_40C0C0(action));
        break;
    case BattleAction_PlayAttackAnimationAndResetFrames:
        static_cast<void>(UpdateBattleAction56_40C160(action));
        break;
    case BattleAction_PlayJoyAnimationAndResetFrames:
        static_cast<void>(UpdateBattleAction57_40C1A0(action));
        break;
    case BattleAction_PlayAngryAnimationAndResetFrames:
        static_cast<void>(UpdateBattleAction58_40C1E0(action));
        break;
    case BattleAction_PlaySadAnimationAndResetFrames:
        static_cast<void>(UpdateBattleAction59_40C240(action));
        break;
    case BattleAction_SetSadAnimation:
        static_cast<void>(UpdateBattleAction60_40C220(action));
        break;
    case BattleAction_PlayWaveAnimation:
        static_cast<void>(UpdateBattleAction61_40C280(action));
        break;
    case BattleAction_PlayScissorsAnimation:
        static_cast<void>(UpdateBattleAction62_40C2B0(action));
        break;
    case BattleAction_TurnLeftThenRight:
        static_cast<void>(UpdateBattleAction63_40C2E0(action));
        break;
    case BattleAction_SetIdlePose:
        static_cast<void>(UpdateBattleAction64_40CBB0(action));
        break;
    case BattleAction_OscillateFacing:
        static_cast<void>(UpdateBattleAction65_40C380(action));
        break;
    case BattleAction_PlayConfiguredImage:
        static_cast<void>(UpdateBattleAction66_40C480(action));
        break;
    case BattleAction_TriggerTargetsOnFrameEvent:
        static_cast<void>(UpdateBattleAction67_4106B0(action));
        break;
    case BattleAction_ShowReactionIcon:
        static_cast<void>(UpdateBattleAction68_40C0E0(action));
        break;
    case BattleAction_ShowReactionHitEffect:
        static_cast<void>(UpdateBattleAction69_40C120(action));
        break;
    case BattleAction_FireSpiracleShots:
        static_cast<void>(UpdateBattleAction70_4070E0(action));
        break;
    case BattleAction_StartEffectTargets:
        static_cast<void>(UpdateBattleAction71_40C4D0(action));
        break;
    case BattleAction_NotifyCasterEntryComplete:
        static_cast<void>(UpdateBattleAction72_40CAA0(action));
        break;
    case BattleAction_AdvanceLinkedCast:
        static_cast<void>(AdvanceBattleLinkedCast_40CAC0(action));
        break;
    case BattleAction_AdvanceEffectAndCast:
        static_cast<void>(AdvanceBattleEffectAndCast_40CAF0(action));
        break;
    case BattleAction_EnableFlash:
        static_cast<void>(UpdateBattleAction75_40CB10(action));
        break;
    case BattleAction_DisableFlash:
        static_cast<void>(UpdateBattleAction76_40CB30(action));
        break;
    case BattleAction_EnableBackendDrawFlags:
        static_cast<void>(UpdateBattleAction77_40CB50(action));
        break;
    case BattleAction_DisableBackendDrawFlags:
        static_cast<void>(UpdateBattleAction78_40CB80(action));
        break;
    case BattleAction_ResetDeathState:
        static_cast<void>(UpdateBattleAction79_40CBD0(action));
        break;
    case BattleAction_PlaySound1018:
        static_cast<void>(UpdateBattleAction80_40CC80(action));
        break;
    case BattleAction_PlaySound101C:
        static_cast<void>(UpdateBattleAction81_40CCB0(action));
        break;
    case BattleAction_PlaySound1020:
        static_cast<void>(UpdateBattleAction82_40CCE0(action));
        break;
    case BattleAction_ApplyDamageUp:
        static_cast<void>(UpdateBattleAction83_40CD10(action));
        break;
    case BattleAction_ClearDamageUp:
        static_cast<void>(UpdateBattleAction84_40CD70(action));
        break;
    case BattleAction_SpinAndEnlarge:
        static_cast<void>(UpdateBattleAction85_40CDE0(action));
        break;
    case BattleAction_ApplyDamageToTargets:
        static_cast<void>(UpdateBattleAction86_40C520(action));
        break;
    case BattleAction_PlayEarthquakeEffects:
        static_cast<void>(UpdateBattleAction87_40C780(action));
        break;
    case BattleAction_StartScreenShake:
        static_cast<void>(UpdateBattleAction88_40C8D0(action));
        break;
    case BattleAction_EndScreenShakeRise:
        static_cast<void>(UpdateBattleAction89_40C8F0(action));
        break;
    case BattleAction_SweepEffectTargetsAndExit:
        static_cast<void>(UpdateBattleAction90_40C910(action));
        break;
    case BattleAction_SetObjectFlag2AndPlay:
        static_cast<void>(UpdateBattleAction91_40CF30(action));
        break;
    case BattleAction_ClearObjectFlag2AndPlay:
        static_cast<void>(UpdateBattleAction92_40CF50(action));
        break;
    case BattleAction_ReleaseUpdateGate:
        static_cast<void>(UpdateBattleAction93_40CF70(action));
        break;
    case BattleAction_OffsetFromEffectTarget:
        static_cast<void>(UpdateBattleAction94_40D010(action));
        break;
    case BattleAction_RushAndZeroTargetHp:
        static_cast<void>(UpdateBattleAction95_40D0B0(action));
        break;
    case BattleAction_ShowEffectTarget:
        static_cast<void>(UpdateBattleAction96_40D170(action));
        break;
    case BattleAction_HideEffectTarget:
        static_cast<void>(UpdateBattleAction97_40D1A0(action));
        break;
    case BattleAction_CastWithTargetMarkers:
        static_cast<void>(UpdateBattleAction98_40D1D0(action));
        break;
    case BattleAction_SetAnimationRate25:
        static_cast<void>(UpdateBattleAction99_40D390(action));
        break;
    case BattleAction_PlayAttackAnimationSequence:
        static_cast<void>(UpdateBattleAction100_40D3C0(action));
        break;
    case BattleAction_MoveToSideCenter:
        static_cast<void>(UpdateBattleAction101_40D580(action));
        break;
    case BattleAction_RushThroughTargets:
        static_cast<void>(UpdateBattleAction102_40D6C0(action));
        break;
    case BattleAction_SpawnMeleeAttackCopies:
        static_cast<void>(UpdateBattleAction103_40DBE0(action));
        break;
    case BattleAction_ShowInspectionResults:
        static_cast<void>(UpdateBattleAction104_40DC90(action));
        break;
    case BattleAction_StepAroundEffectTarget:
        static_cast<void>(UpdateBattleAction105_40DE40(action));
        break;
    case BattleAction_SetJoyAnimation:
        static_cast<void>(UpdateBattleAction106_40DF50(action));
        break;
    case BattleAction_MarkForRemoval:
        static_cast<void>(UpdateBattleAction107_40DF70(action));
        break;
    case BattleAction_AttackAllTargetsOnFrameEvent:
        static_cast<void>(UpdateBattleAction108_40DF80(action));
        break;
    case BattleAction_SpawnChargedStrikeCopy:
        static_cast<void>(UpdateBattleAction109_40E4B0(action));
        break;
    case BattleAction_ChargeAndStrike:
        static_cast<void>(UpdateBattleAction110_40E560(action));
        break;
    case BattleAction_FlyOutFastAndShakeScreen:
        static_cast<void>(UpdateBattleAction111_40E970(action));
        break;
    case BattleAction_SpawnArcAttackCopies:
        static_cast<void>(UpdateBattleAction112_40EB00(action));
        break;
    case BattleAction_MoveAlongArc:
        static_cast<void>(UpdateBattleAction113_40EFE0(action));
        break;
    case BattleAction_ResolveArcCopyHits:
        static_cast<void>(UpdateBattleAction114_40F0A0(action));
        break;
    case BattleAction_PlayAttackClashEffect:
        static_cast<void>(UpdateBattleAction115_40FB80(action));
        break;
    case BattleAction_PlayEffectType1:
    case BattleAction_PlayMagicAttackEffect:
    case BattleAction_EnterEffectWait:
    case BattleAction_PlayDebuffSpellEffect:
    case BattleAction_PlayBarrierSpellEffect:
    case BattleAction_PlayPhysicalReflectEffect:
    case BattleAction_PlayPhysicalAbsorbEffect:
    case BattleAction_PlayPhysicalNullifyEffect:
    case BattleAction_PlayMagicReflectEffect:
    case BattleAction_PlayMagicAbsorbEffect:
    case BattleAction_PlayMagicNullifyEffect:
    case BattleAction_PlayRecoveryEffect:
    case BattleAction_PlayEffectType14:
    case BattleAction_PlayEffectType15:
    case BattleAction_PlayStatusRecoveryEffect:
    case BattleAction_PlayEffectType26:
    case BattleAction_PlayEffectType27:
    case BattleAction_PlayEffectType28:
    case BattleAction_PlayHealingCastEffect:
    case BattleAction_PlayAttributeReversalEffect:
    case BattleAction_PlayHealEffect:
    case BattleAction_PlayAmountTieredRecoveryEffect:
    case BattleAction_PlayLevelScaledEffect21:
    case BattleAction_PlayParentBoundEffect22:
    case BattleAction_PlayParentBoundEffect23:
    case BattleAction_PlayParameterChangeEffect:
    case BattleAction_PlayEffectType38:
    case BattleAction_PlayBombLevelEffect:
    case BattleAction_PlayRandomOffsetEffect40:
    case BattleAction_PlayConfiguredEffect41:
    case BattleAction_PlayConfiguredEffect42:
        static_cast<void>(UpdateBattleFactoryAction_40FC10(action));
        break;
    case BattleAction_StartPhysicalReflectEffect:
    case BattleAction_StartPhysicalAbsorbEffect:
    case BattleAction_StartPhysicalNullifyEffect:
        static_cast<void>(UpdateBattleActions127To129_40FF90(action));
        break;
    case BattleAction_PlayItemUseEffect:
        static_cast<void>(UpdateBattleAction138_40FF40(action));
        break;
    case BattleAction_CastSpellAndTriggerTargets:
        static_cast<void>(UpdateBattleAction139_410000(action));
        break;
    case BattleAction_TriggerSpellEffectOrHealTarget:
        static_cast<void>(UpdateBattleAction140_410730(action));
        break;
    case BattleAction_ApplyDebuff:
        static_cast<void>(UpdateBattleAction141_410E90(action));
        break;
    case BattleAction_ClearDebuff:
        static_cast<void>(UpdateBattleAction142_410F70(action));
        break;
    case BattleAction_ApplyStatusEffect:
        static_cast<void>(UpdateBattleAction143_410FC0(action));
        break;
    case BattleAction_ClearStatusEffect:
        static_cast<void>(UpdateBattleAction144_411240(action));
        break;
    case BattleAction_CreatePrayerField:
        static_cast<void>(UpdateBattleAction145_411290(action));
        break;
    case BattleAction_ToggleAttributeReversal:
        static_cast<void>(UpdateBattleAction147_411320(action));
        break;
    case BattleAction_PlayReviveEffect:
        static_cast<void>(UpdateBattleAction148_40FEF0(action));
        break;
    case BattleAction_StartScalingEffect24:
        static_cast<void>(UpdateBattleAction154_40F790(action));
        break;
    case BattleAction_PulseScaleWithParentEffect:
        static_cast<void>(UpdateBattleAction155_40F800(action));
        break;
    case BattleAction_WaitScalingEffect24End:
        static_cast<void>(UpdateBattleAction156_40FB30(action));
        break;
    case BattleAction_ApplyResourceChange:
        static_cast<void>(UpdateBattleAction159_4107C0(action));
        break;
    case BattleAction_ApplyResourceChangeAndWait:
        static_cast<void>(UpdateBattleAction160_410B00(action));
        break;
    case BattleAction_WaitEffectComplete:
        static_cast<void>(UpdateBattleAction162_410E30(action));
        break;
    case BattleAction_TransitionRebirthForm:
        static_cast<void>(UpdateBattleAction163_4113A0(action));
        break;
    case BattleAction_RestoreRebirthBaseForm:
        static_cast<void>(UpdateBattleAction164_411A50(action));
        break;
    case BattleAction_ReactToCriticalHit:
        static_cast<void>(UpdateBattleAction165_411D30(action));
        break;
    case BattleAction_ConsumeQueuedTime:
        static_cast<void>(UpdateBattleAction166_411CB0(action));
        break;
    case BattleAction_EnableAlphaOverride:
        static_cast<void>(UpdateBattleAction167_411FA0(action));
        break;
    case BattleAction_ClearColorOverrides:
        static_cast<void>(UpdateBattleAction168_411FC0(action));
        break;
    case BattleAction_OverrideAllMixFlags:
        static_cast<void>(UpdateBattleAction169_411FE0(action));
        break;
    case BattleAction_ClearMixFlagOverride:
        static_cast<void>(UpdateBattleAction170_412000(action));
        break;
    case BattleAction_FadeIn:
        static_cast<void>(UpdateBattleAction171_412020(action));
        break;
    case BattleAction_FadeOutStep3:
    case BattleAction_FadeOutStep2:
        static_cast<void>(UpdateBattleActions172_173_4120F0(action));
        break;
    case BattleAction_SelectPalette:
        static_cast<void>(UpdateBattleAction174_4121D0(action));
        break;
    case BattleAction_StartBackgroundFade:
        static_cast<void>(UpdateBattleAction175_412210(action));
        break;
    case BattleAction_EndBackgroundFade:
        static_cast<void>(UpdateBattleAction176_412260(action));
        break;
    case BattleAction_PlayBackgroundFade:
        static_cast<void>(UpdateBattleAction177_412290(action));
        break;
    case BattleAction_AdvanceAndIdle:
        static_cast<void>(UpdateBattleAction178_4122C0(action));
        break;
    case BattleAction_PlayCastAnimation:
        static_cast<void>(UpdateBattleAction179_4122E0(action));
        break;
    case BattleAction_PlayRecoveryEffectAndFadeIn:
    case BattleAction_PlayEffectType1AndFadeIn:
    case BattleAction_PlayMagicAttackEffectAndFadeIn:
    case BattleAction_PlayStatusRecoveryEffectAndFadeIn:
    case BattleAction_PlayHealEffectAndFadeIn:
    case BattleAction_PlayLevelScaledEffect21AndFadeIn:
    case BattleAction_PlayParentBoundEffect22AndFadeIn:
    case BattleAction_PlayParentBoundEffect23AndFadeIn:
    case BattleAction_EnterFadeInAndResolveEffect:
    case BattleAction_PlayDebuffSpellEffectAndFadeIn:
    case BattleAction_PlayBarrierSpellEffectAndFadeIn:
    case BattleAction_PlayPhysicalReflectEffectAndFadeIn:
    case BattleAction_PlayPhysicalAbsorbEffectAndFadeIn:
    case BattleAction_PlayPhysicalNullifyEffectAndFadeIn:
    case BattleAction_PlayMagicReflectEffectAndFadeIn:
    case BattleAction_PlayMagicAbsorbEffectAndFadeIn:
    case BattleAction_PlayMagicNullifyEffectAndFadeIn:
    case BattleAction_PlayEffectType26AndFadeIn:
    case BattleAction_PlayHealingCastEffectAndFadeIn:
    case BattleAction_PlayEffectType14AndFadeIn:
    case BattleAction_PlayEffectType15AndFadeIn:
    case BattleAction_PlayAttributeReversalEffectAndFadeIn:
    case BattleAction_PlayParameterChangeEffectAndFadeIn:
    case BattleAction_PlayBombLevelEffectAndFadeIn:
        static_cast<void>(UpdateBattleActions180_203_412310(action));
        break;
    case BattleAction_FadeInAndResolveEffect:
        static_cast<void>(UpdateBattleAction204_4124E0(action));
        break;
    case BattleAction_PlayNodAnimationAndResetFrames:
        static_cast<void>(UpdateBattleAction205_4127B0(action));
        break;
    case BattleAction_FaceEffectTarget:
        static_cast<void>(UpdateBattleAction206_4127F0(action));
        break;
    case BattleAction_MoveToEffectPosition:
        static_cast<void>(UpdateBattleAction207_412830(action));
        break;
    case BattleAction_MoveBesideEffectTarget:
        static_cast<void>(SetBattleAction208Target_412870(action));
        break;
    case BattleAction_ReactToHitAndStrikeLinkedTarget:
        static_cast<void>(UpdateBattleAction209_412900(action));
        break;
    case BattleAction_RushWithTrail:
        static_cast<void>(UpdateBattleAction210_412F70(action));
        break;
    case BattleAction_ApplySceneEffects:
        static_cast<void>(ApplyBattleSceneEffects_413070(action));
        break;
    case BattleAction_RepeatOffsetEffect:
        static_cast<void>(UpdateBattleAction212_413110(action));
        break;
    case BattleAction_HandleDeathAndContinue:
        static_cast<void>(UpdateBattleAction213_4131B0(action));
        break;
    case BattleAction_FadeOutAndContinue:
        static_cast<void>(UpdateBattleAction214_4132B0(action));
        break;
    case BattleAction_WaitFadeEffectAndContinue:
        static_cast<void>(UpdateBattleAction215_4133D0(action));
        break;
    case BattleAction_CastMonsterSpellAndTriggerTargets:
        static_cast<void>(UpdateBattleAction216_413400(action));
        break;
    case BattleAction_ChangeAppearanceWithCrossFade:
        static_cast<void>(UpdateBattleAction218_413550(action));
        break;
    case BattleAction_RestoreAppearanceAfterDeath:
        static_cast<void>(UpdateBattleAction219_413750(action));
        break;
    case BattleAction_RestoreAppearanceBeforeFade:
        static_cast<void>(UpdateBattleAction220_413980(action));
        break;
    case BattleAction_ApplyFollowupHit:
        static_cast<void>(UpdateBattleAction221_413B40(action));
        break;
    case BattleAction_MoveLinkedProjectileToTarget:
        static_cast<void>(UpdateBattleAction222_413E90(action));
        break;
    case BattleAction_UpdatePetSnapshot:
        static_cast<void>(UpdateBattleAction223_413FE0(action));
        break;
    case BattleAction_TransitionRideState:
        static_cast<void>(UpdateBattleAction224_414050(action));
        break;
    case BattleAction_FailToMountAndReturn:
        static_cast<void>(UpdateBattleAction225_4150F0(action));
        break;
    case BattleAction_DismountAndRestoreActors:
        static_cast<void>(UpdateBattleAction226_4154C0(action));
        break;
    case BattleAction_DismountWithActorAnimation:
        static_cast<void>(UpdateBattleAction227_415940(action));
        break;
    case BattleAction_StartAnimationSlowdown:
        static_cast<void>(UpdateBattleAction228_416000(action));
        break;
    case BattleAction_AnimateSlowdown:
        static_cast<void>(UpdateBattleAction229_416020(action));
        break;
    case BattleAction_MoveAlongEffectPath:
        static_cast<void>(UpdateBattleAction230_4160A0(action));
        break;
    case BattleAction_DecelerateWithTrail:
        static_cast<void>(UpdateBattleAction231_416150(action));
        break;
    case BattleAction_RushIntoTargetWithTrail:
        static_cast<void>(UpdateBattleAction232_416220(action));
        break;
    case BattleAction_ReactToHitWithoutDisplacement:
        static_cast<void>(UpdateBattleAction233_416860(action));
        break;
    case BattleAction_BounceOutImmediately:
        static_cast<void>(UpdateBattleAction234_416AB0(action));
        break;
    case BattleAction_ApplyEffectFlags:
        static_cast<void>(ApplyBattleCastEffects_416DB0(action));
        break;
    case BattleAction_ResetBusyAndIdle:
        static_cast<void>(UpdateBattleAction236_406630(action));
        break;
    case BattleAction_RushAndResolveHitWithin16Ticks:
        static_cast<void>(UpdateBattleAction237_416DD0(action));
        break;
    case BattleAction_ResolveDeceleratingProjectileHit:
    case BattleAction_ResolveAcceleratingProjectileHit:
        static_cast<void>(UpdateBattleActions238_239_417410(action));
        break;
    case BattleAction_PrepareArcShots:
        static_cast<void>(UpdateBattleAction240_417AC0(action));
        break;
    case BattleAction_FireArcShotsSequentially:
        static_cast<void>(UpdateBattleAction241_417B80(action));
        break;
    case BattleAction_FireArcShotsTogether:
        static_cast<void>(UpdateBattleAction242_417CC0(action));
        break;
    case BattleAction_ResolveStraightProjectileHit:
        static_cast<void>(UpdateBattleAction243_417D80(action));
        break;
    case BattleAction_MarkDeadAndAcquireGate:
        static_cast<void>(UpdateBattleAction244_418550(action));
        break;
    case BattleAction_CreateTiledEffect:
        static_cast<void>(UpdateBattleAction245_418580(action));
        break;
    case BattleAction_SetSpeedFromEffect:
        static_cast<void>(UpdateBattleAction246_4185C0(action));
        break;
    case BattleAction_SavePosition:
        static_cast<void>(UpdateBattleAction247_4185F0(action));
        break;
    case BattleAction_ReturnToSavedPosition:
        static_cast<void>(UpdateBattleAction248_418620(action));
        break;
    case BattleAction_FireBlastWaves:
        static_cast<void>(UpdateBattleAction249_4071C0(action));
        break;
    case BattleAction_SetHpFpOne:
        static_cast<void>(UpdateBattleAction251_40B180(action));
        break;
    case BattleAction_CreateCastingEffect:
        static_cast<void>(UpdateBattleAction253_40FE50(action));
        break;
    case BattleAction_ClearCastingEffect:
        static_cast<void>(UpdateBattleAction254_40FEB0(action));
        break;
    case BattleAction_ApplyAreaAttackOnFrameEvent:
        static_cast<void>(UpdateBattleAction255_4186B0(action));
        break;
    case BattleAction_RequestBattleExit:
        static_cast<void>(UpdateBattleAction256_418B50(action));
        break;
    default:
        break;
    }
    // 公共尾沿用入口载荷；动作可能改写对象表，不在此重取载荷。
    if (payload->flash1004 == 1) {
        const std::int32_t count = payload->flashCounter1000 + 1;
        payload->flashCounter1000 = count;
        if (count >= 2) {
            payload->flashCounter1000 = 0;
        }
        if (payload->flashCounter1000 == 1) {
            static_cast<void>(NewBattleAfterimage_41E5F0(action));
        }
    }
    if ((action->flags & 0x800000u) == 0) {
        const std::int32_t x = static_cast<std::int32_t>(action->offset40);
        const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
        const float y = action->offset44;
        action->x = x + offsetX;
        const std::int32_t yCoordinate = static_cast<std::int32_t>(y);
        const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
        action->y = yCoordinate + offsetY;
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        SyncBattleRider_421FB0(rider);
    }
    YobiAction* const gear = payload->gear13B4;
    if (gear != nullptr) {
        SyncBattleGear_4225D0(gear);
    }
    const bool hasRider = payload->rider13C8 != nullptr;
    YobiAction* const firstAuxiliaryGear = payload->auxiliaryGear13B8[0];
    if (hasRider) {
        if (firstAuxiliaryGear != nullptr) {
            static_cast<void>(MarkDeath(firstAuxiliaryGear));
            payload->auxiliaryGear13B8[0] = nullptr;
        }
        YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
        if (secondAuxiliaryGear != nullptr) {
            static_cast<void>(MarkDeath(secondAuxiliaryGear));
            payload->auxiliaryGear13B8[1] = nullptr;
        }
    } else {
        if (firstAuxiliaryGear != nullptr) {
            SyncBattleAuxGear_422780(firstAuxiliaryGear, 0);
        }
        YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
        if (secondAuxiliaryGear != nullptr) {
            SyncBattleAuxGear_422780(secondAuxiliaryGear, 1);
        }
    }
}

// BIDLE01：完整FDC持样/CL消费、归零后fresh BYTE与上一动作门禁见doc/cg2d_battle_idle_dispatch_spec.md。
// 原函数：0x00406650。
void UpdateBattleAction0_406650(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->castPlayIndexFC4;
    if (index <= payload->castListIndexFC8 && payload->castListCA4[index] == BattleAction_ConsumeQueuedTime) {
        AdvanceBattleCastState_439340(action);
    }
    action->framePlayingRate = 100;
    const std::uint32_t flags = payload->flagsFDC;
    const std::uint8_t cachedFlags = static_cast<std::uint8_t>(flags);
    if ((cachedFlags & BattleActorFlag_Dead) != 0) {
        action->characterAction = CharacterAnimation_Down;
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
        return;
    }
    const std::int32_t skill = payload->castSkill103C;
    if (skill == 72 || skill == 113) {
        const std::int32_t previous = payload->castPlayIndexFC4 - 1;
        if (previous >= 0) {
            const std::int32_t previousAction = payload->castListCA4[previous];
            if (previousAction == BattleAction_PlayJoyAnimationAndResetFrames || previousAction == BattleAction_PlaySadAnimationAndResetFrames) {
                return;
            }
        }
    }
    if (action->characterAction != CharacterAnimation_Stand) {
        action->characterAction = CharacterAnimation_Stand;
        const std::uint8_t currentFlags = static_cast<std::uint8_t>(payload->flagsFDC);
        if ((currentFlags & BattleActorSleepOrStoneMask) == 0) {
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        }
    } else if ((static_cast<std::uint8_t>(action->flags) & 0x40u) != 0
        && (cachedFlags & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    }
}

// BIDLE01：保存动作/固定payload后归站立，再读FDC低BYTE门禁，见待机SPEC。
// 原函数：0x00406720。
void UpdateBattleAction1_406720(YobiAction* const action)
{
    const std::int32_t characterAction = action->characterAction;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->framePlayingRate = 100;
    if (characterAction != CharacterAnimation_Stand) {
        action->characterAction = CharacterAnimation_Stand;
    }
    const std::uint8_t flags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((flags & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    }
}

// BRCP01：回00/04，先X发布后Y取样/phase0；FDC置1后才播放，不推进队列。
// 原函数：0x00406760。
std::int32_t UpdateBattleAction2_406760(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Walk;
    action->motionMagnitude54 = 4.0F;
    action->framePlayingRate = 100;
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y04);
    const float distanceX = static_cast<float>(payload->x00);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x00);
        const double y = payload->y04;
        action->phase = BattleAction_Idle;
        action->offset44 = static_cast<float>(y);
        payload->flagsFDC |= BattleActorFlag_EntryComplete;
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BRCP01：转向后重取C9C/position/目标；距离<70推进，否则移动，再播放。
// 原函数：0x00406C30。
std::int32_t UpdateBattleAction3_406C30(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Walk;
    action->motionMagnitude54 = 8.0F;
    action->framePlayingRate = 100;
    TurnBattleToward_406190(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    if (GetBattleDistance_406300(action,
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
        AdvanceBattleCastState_439340(action);
    } else {
        MoveBattleByAngle_405EA0(action);
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BRCP01：转向后重取效果目标；距离<8推进，否则移动，再播放。
// 原函数：0x00406D70。
std::int32_t UpdateBattleAction4_406D70(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Run;
    action->motionMagnitude54 = 8.0F;
    action->framePlayingRate = 100;
    TurnBattleToward_406190(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    if (GetBattleDistance_406300(action,
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 8) {
        AdvanceBattleCastState_439340(action);
    } else {
        MoveBattleByAngle_405EA0(action);
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BPAW01：FF0先发布增量再按旧值<16分流，推进后清零，再播放。
// 原函数：0x0040CFA0。
std::int32_t UpdateBattleAction5_40CFA0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Run;
    action->motionMagnitude54 = 8.0F;
    action->framePlayingRate = 100;
    const std::int32_t count = payload->valueFF0;
    payload->valueFF0 = count + 1;
    if (count < 16) {
        MoveBattleByAngle_405EA0(action);
    } else {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BRCP01：两组FE8/FEC先Y后X取样；抵达先X发布再取Y，随后推进/播放。
// 原函数：0x00406CD0。
std::int32_t UpdateBattleAction6_406CD0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Run;
    action->motionMagnitude54 = 8.0F;
    action->framePlayingRate = 100;
    const float turnY = static_cast<float>(payload->moveTargetYFEC);
    const float turnX = static_cast<float>(payload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->moveTargetYFEC);
    const float distanceX = static_cast<float>(payload->moveTargetXFE8);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->moveTargetXFE8);
        action->offset44 = static_cast<float>(payload->moveTargetYFEC);
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BPAW01：持一次position选转向；严格终帧后fresh低BYTE调速，再推进。
// 原函数：0x00409410。
void UpdateBattleAction7_409410(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_BeginRun;
    action->framePlayingRate = 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position == -1) {
        const float y = static_cast<float>(payload->y04);
        const float x = static_cast<float>(payload->x00);
        TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    } else {
        TurnBattleToward_406190(action, world::gBattleAllEntries59E980[position], 1, 0.0F);
    }
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & BattleActorFlag_Dead) != 0) {
            action->framePlayingRate = 200;
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BPAW01：FEC/FE8先Y后X转向；严格终帧后fresh低BYTE调速/推进。
// 原函数：0x004094A0。
void UpdateBattleAction8_4094A0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_BeginRun;
    action->framePlayingRate = 100;
    const float y = static_cast<float>(payload->moveTargetYFEC);
    const float x = static_cast<float>(payload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & BattleActorFlag_Dead) != 0) {
            action->framePlayingRate = 200;
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BPAW01：FC4-2为14走回位距离5，否则两次fresh目标距离70；另一支才移动。
// 原函数：0x00409510。
std::int32_t UpdateBattleAction9_409510(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Run;
    action->motionMagnitude54 = 8.0F;
    action->framePlayingRate = 100;
    const std::int32_t previous = payload->castPlayIndexFC4 - 2;
    if (previous >= 0 && payload->castListCA4[previous] == BattleAction_PerformMeleeAttack) {
        const float turnY = static_cast<float>(payload->y04);
        const float turnX = static_cast<float>(payload->x00);
        TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
        const float distanceY = static_cast<float>(payload->y04);
        const float distanceX = static_cast<float>(payload->x00);
        if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
            AdvanceBattleCastState_439340(action);
            return PlayFrame_517DA0(action, AnimationPlayback_Loop);
        }
    } else {
        TurnBattleToward_406190(action,
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
        if (GetBattleDistance_406300(action,
                cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
            AdvanceBattleCastState_439340(action);
            return PlayFrame_517DA0(action, AnimationPlayback_Loop);
        }
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// BPAW01：严格终帧才取上一项11/低BYTE门禁，按本位重查全局方向叶后推进。
// 原函数：0x00409610。
void UpdateBattleAction10_409610(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_EndRun;
    action->framePlayingRate = 100;
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        const std::int32_t previous = payload->castPlayIndexFC4 - 1;
        if (previous >= 0 && payload->castListCA4[previous] == BattleAction_ReturnToFormation && (static_cast<std::uint8_t>(payload->flagsFDC) & BattleActorFlag_Dead) == 0) {
            FaceBattleActorTowardFormationPosition_406260(world::gBattleAllEntries59E980[payload->battlePosition18]);
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BRCP01：骑乘两低BYTE死亡短路/持W32门禁；推进后phase及末播放动作重取。
// 原函数：0x004067F0。
void UpdateBattleAction11_4067F0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->rider13C8 != nullptr && payload->effect1060 != nullptr) {
        const auto* const carrier = static_cast<const BattleRideCarrierPayload*>(payload->effect1060->payload.battleRideCarrier);
        YobiAction* const first = carrier->first00;
        bool dead = false;
        if (first != nullptr) {
            const auto* const firstPayload = static_cast<const BattleActorPayload*>(first->payload.battleActor);
            dead = firstPayload != nullptr && (static_cast<std::uint8_t>(firstPayload->flagsFDC) & BattleActorFlag_Dead) != 0;
        }
        if (!dead) {
            YobiAction* const second = carrier->second04;
            if (second != nullptr) {
                const auto* const secondPayload = static_cast<const BattleActorPayload*>(second->payload.battleActor);
                dead = secondPayload != nullptr && (static_cast<std::uint8_t>(secondPayload->flagsFDC) & BattleActorFlag_Dead) != 0;
            }
        }
        if (dead) {
            AdvanceBattleCastState_439340(action);
            if (action->phase == BattleAction_HandleDeath) {
                AdvanceBattleCastState_439340(action);
            }
            return;
        }
    }
    const std::uint32_t flags = payload->flagsFDC;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(flags);
    if ((entryFlags & BattleActorFlag_Dead) != 0 && (entryFlags & BattleActorPlayerOrPetMask) == 0) {
        if (action->characterAction != CharacterAnimation_Down) {
            action->phase = BattleAction_HandleDeath;
            return;
        }
    } else if (action->characterAction != CharacterAnimation_Down) {
        action->characterAction = CharacterAnimation_Walk;
    }
    const std::int32_t previous = payload->castPlayIndexFC4 - 1;
    if (previous >= 0 && payload->castListCA4[previous] == BattleAction_PlayBeginRunFacingEffectTarget) {
        action->characterAction = CharacterAnimation_Run;
    }
    const std::int32_t updateDirection = action->characterAction != CharacterAnimation_Down ? 1 : 0;
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, updateDirection, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y04);
    const float distanceX = static_cast<float>(payload->x00);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x00);
        action->offset44 = static_cast<float>(payload->y04);
        AdvanceBattleCastState_439340(action);
    }
    static_cast<void>(PlayFrame_517DA0(action, action->characterAction == CharacterAnimation_Down ? AnimationPlayback_StopAtEnd : AnimationPlayback_Loop));
}

// B12P01：原入口低BYTE、周期特效/计数与特殊直接推进见doc/cg2d_battle_coordinate_action_producer_spec.md。
// 原函数：0x00406930。
void UpdateBattleAction12_406930(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((entryFlags & BattleActorFlag_Dead) != 0) {
        action->characterAction = CharacterAnimation_Down;
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
        const float y = static_cast<float>(payload->y0C);
        const float x = static_cast<float>(payload->x08);
        TurnBattleTowardPosition_4060D0(action, x, y, 0, 0.0F);
        action->motionMagnitude54 = 6.0F;
        AdvanceBattleCastState_439340(action);
        return;
    }
    if (payload->valueFF0 == 0) {
        const std::int32_t randomY = platform::RandomRange_46FAF0(-10, 10);
        const float y = static_cast<float>(static_cast<double>(randomY) + action->offset44);
        const std::int32_t randomX = platform::RandomRange_46FAF0(-10, 10);
        const float x = static_cast<float>(static_cast<double>(randomX) + action->offset40);
        static_cast<void>(NewBattleCoordinateEffect_41E720(x, y, 110006));
    }
    const std::int32_t count = payload->valueFF0;
    payload->valueFF0 = count + 1;
    if (count >= 2) {
        payload->valueFF0 = 0;
    }
    action->characterAction = CharacterAnimation_Walk;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const float y = static_cast<float>(payload->y0C);
    const float x = static_cast<float>(payload->x08);
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    const std::int32_t frames = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = frames;
    action->framePlayingRate = frames > 70 ? 30 : 100 - frames;
    if (payload->actionFrameCountFCC >= 120) {
        payload->actionFrameCountFCC = 0;
        payload->valueFF0 = 0;
        action->motionMagnitude54 = 6.0F;
        AdvanceBattleCastState_439340(action);
    }
}

// B12P01：原入口低BYTE、播放后移动及到位X发布再读Y，见坐标动作SPEC。
// 原函数：0x00406A80。
void UpdateBattleAction13_406A80(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    std::int32_t updateDirection;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((entryFlags & BattleActorFlag_Dead) != 0) {
        action->characterAction = CharacterAnimation_Down;
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
        updateDirection = 0;
    } else {
        if (payload->valueFF0 == 0) {
            const std::int32_t randomY = platform::RandomRange_46FAF0(-10, 10);
            const float y = static_cast<float>(static_cast<double>(randomY) + action->offset44);
            const std::int32_t randomX = platform::RandomRange_46FAF0(-10, 10);
            const float x = static_cast<float>(static_cast<double>(randomX) + action->offset40);
            static_cast<void>(NewBattleCoordinateEffect_41E720(x, y, 110006));
        }
        const std::int32_t count = payload->valueFF0;
        payload->valueFF0 = count + 1;
        if (count >= 2) {
            payload->valueFF0 = 0;
        }
        action->characterAction = CharacterAnimation_Walk;
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        updateDirection = 1;
    }
    const float turnY = static_cast<float>(payload->y0C);
    const float turnX = static_cast<float>(payload->x08);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, updateDirection, 0.0F);
    action->motionMagnitude54 = 6.0F;
    action->framePlayingRate = 20;
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y0C);
    const float distanceX = static_cast<float>(payload->x08);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x08);
        action->offset44 = static_cast<float>(payload->y0C);
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00406630。
void UpdateBattleAction236_406630(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->phase = BattleAction_Idle;
    payload->busyFD8 = 0;
    UpdateBattleAction0_406650(action);
}

} // namespace detail



// 原[4238F0,42395B)：11个直接caller均不消费原混合返回值。
void UpdateBattleAlbumFlag_4238F0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::uint32_t state = payload->stateFE0;
    const std::int32_t position = payload->battlePosition18;
    const std::uint32_t cleared = state & ~std::uint32_t{1};
    payload->stateFE0 = cleared;
    const std::int32_t playerPosition = cg::world::gBattleCurrentPosition5ABCA4;
    if (position == playerPosition) {
        payload->stateFE0 = cleared | 1u;
        return;
    }
    if (position != cg::world::GetBattlePetPosition_405B40(playerPosition)) {
        if (cg::world::gBattleCurrentPosition5ABCA4 < 10) {
            if (position < 10) {
                return;
            }
        } else if (position >= 10) {
            return;
        }
        if (detail::MatchBattleAlbum_423800(action) == 0) {
            return;
        }
    }
    payload->stateFE0 = payload->stateFE0 | 1u;
}

} // namespace cg::yobi
