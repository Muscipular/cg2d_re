#include "battle_impact.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/RendererResources.hpp"

#include "animation.hpp"
#include "battle_display.hpp"
#include "battle_effect.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/BattleState.hpp"

#include <bit>
#include <cstdint>
#include <optional>

namespace cg::yobi {

namespace detail {

// 六类命中／携带效果自身首次门禁；与目标BattleAction及全局主场景phase独立。
enum BattleImpactPhase : std::int32_t {
    BattleImpactPhase_WaitForTargetIdle = 0,
    BattleImpactPhase_Running = 1,
};


// 原函数：0x0041A9E0。
void UpdateBattleImpactEffect_41A9E0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto& battle = action->ownerSession->WorldScene().battle;
    const std::int32_t initialPhase = action->phase;
    auto* const payload = action->payload.battleActor;
    if (initialPhase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    action->phase = BattleImpactPhase_Running;
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        static_cast<void>(MarkDeath(action));
        const std::int32_t initialIndex = payload->effectRunningC9C;
        const std::int32_t initialPosition = payload->effects1C[initialIndex].position;
        const std::uint32_t initialFlags = payload->effects1C[initialIndex].flags;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[initialPosition];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((initialFlags & BattleResultFlag_Guard) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & BattleResultFlag_MagicGuard) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & BattleResultFlag_SpecialGuard) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2) + 110001;
        }
        FaceBattleActorTowardFormationPosition_406260(target);
        target->battleOpponent10 = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t resolvedIndex = payload->effectRunningC9C;
        const std::uint32_t resolvedFlags = payload->effects1C[resolvedIndex].flags;
        if ((resolvedFlags & BattleResultFlag_Avoidance) != 0) {
            target->phase = BattleAction_DodgeAndWaitAttacker;
        } else {
            if ((resolvedFlags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[resolvedIndex].position3 != 0) {
                static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = effect.position3;
                (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Death) != 0) {
                    targetPayload->flagsFDC |= BattleActorFlag_Dead;
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_BounceOut) != 0) {
                        action->ownerSession->Yobi().gateMode = YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero;
                        targetPayload->aliveFF8 = 1;
                    }
                }
            }
            const std::uint32_t categoryFlags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((categoryFlags & BattleResultFlag_ReflectMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_NullifyMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_AbsorbMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                if ((payload->effects1C[index].flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[index];
                    const std::int32_t amount = effect.position3;
                    (cg::world::gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t displayIndex = payload->effectRunningC9C;
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_IncreaseHp, 2, displayFlags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else {
                if ((categoryFlags & BattleResultFlag_WakeUp) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->buffEffectAction_1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->buffEffectAction_1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~BattleActorFlag_Sleep;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[index].position3;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_DecreaseHp, 2, flags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
            }
        }
        const std::int32_t finishIndex = payload->effectRunningC9C;
        if ((payload->effects1C[finishIndex].flags & BattleResultFlag_Avoidance) == 0
            && payload->effects1C[finishIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
        }
        action->frameEvent = 0;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            payload->valueFD0 = remaining - 1;
        }
    }
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x0041AF00。
void UpdateBattleImpactSoundEffect_41AF00(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto& battle = action->ownerSession->WorldScene().battle;
    const std::int32_t initialPhase = action->phase;
    auto* const payload = action->payload.battleActor;
    if (initialPhase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    action->phase = BattleImpactPhase_Running;
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        static_cast<void>(MarkDeath(action));
        const std::int32_t initialIndex = payload->effectRunningC9C;
        const std::int32_t initialPosition = payload->effects1C[initialIndex].position;
        const std::uint32_t initialFlags = payload->effects1C[initialIndex].flags;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[initialPosition];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((initialFlags & BattleResultFlag_Guard) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & BattleResultFlag_MagicGuard) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & BattleResultFlag_SpecialGuard) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2) + 110001;
        }
        FaceBattleActorTowardFormationPosition_406260(target);
        target->battleOpponent10 = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t resolvedIndex = payload->effectRunningC9C;
        const std::uint32_t resolvedFlags = payload->effects1C[resolvedIndex].flags;
        if ((resolvedFlags & BattleResultFlag_Avoidance) != 0) {
            target->phase = BattleAction_DodgeAndWaitAttacker;
        } else {
            if ((resolvedFlags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[resolvedIndex].position3 != 0) {
                static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = effect.position3;
                (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Death) != 0) {
                    targetPayload->flagsFDC |= BattleActorFlag_Dead;
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_BounceOut) != 0) {
                        action->ownerSession->Yobi().gateMode = YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero;
                        targetPayload->aliveFF8 = 1;
                    }
                }
            }
            const std::uint32_t categoryFlags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((categoryFlags & BattleResultFlag_ReflectMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_NullifyMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_AbsorbMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                if ((payload->effects1C[index].flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[index];
                    const std::int32_t amount = effect.position3;
                    (cg::world::gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t displayIndex = payload->effectRunningC9C;
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_IncreaseHp, 2, displayFlags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else {
                if ((categoryFlags & BattleResultFlag_WakeUp) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->buffEffectAction_1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->buffEffectAction_1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~BattleActorFlag_Sleep;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[index].position3;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_DecreaseHp, 2, flags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                static_cast<void>(audio::PlaySe_52C450(132, action->x));
            }
        }
        const std::int32_t finishIndex = payload->effectRunningC9C;
        if ((payload->effects1C[finishIndex].flags & BattleResultFlag_Avoidance) == 0
            && payload->effects1C[finishIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
        }
        action->frameEvent = 0;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            payload->valueFD0 = remaining - 1;
        }
    }
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x0041B440。
void UpdateBattleFrameEventEffect_41B440(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto& battle = action->ownerSession->WorldScene().battle;
    const std::int32_t initialPhase = action->phase;
    auto* const payload = action->payload.battleActor;
    if (initialPhase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    const std::int32_t frameEvent = action->frameEvent;
    action->phase = BattleImpactPhase_Running;
    if (frameEvent >= 10000) {
        const std::int32_t initialIndex = payload->effectRunningC9C;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[payload->effects1C[initialIndex].position];
        const std::uint32_t initialFlags = payload->effects1C[initialIndex].flags;
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((initialFlags & BattleResultFlag_Guard) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & BattleResultFlag_MagicGuard) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & BattleResultFlag_SpecialGuard) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2) + 110001;
        }
        FaceBattleActorTowardFormationPosition_406260(target);
        target->battleOpponent10 = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        if (action->frameEvent < 20000 && payload->value1038 != BattleComboFrameHitGate_ForceHitPath) {
            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
            const std::int32_t resolvedIndex = payload->effectRunningC9C;
            const std::uint32_t resolvedFlags = payload->effects1C[resolvedIndex].flags;
            if ((resolvedFlags & BattleResultFlag_Avoidance) != 0) {
                target->phase = BattleAction_DodgeAndWaitAttacker;
            } else {
                if ((resolvedFlags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[resolvedIndex].position3 != 0) {
                    if ((resolvedFlags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness < 0)) {
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(0));
                        const std::int32_t copyIndex = payload->effectRunningC9C;
                        const std::uint32_t encoded = temporary->encoded;
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[copyIndex].position];
                        copyTarget->readiness.encoded = encoded;
                        copyTarget->readiness.mask = temporary->mask;
                        copyTarget->readiness.readCount = temporary->readCount;
                        temporary.reset();
                    }
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Death) != 0) {
                        targetPayload->flagsFDC |= BattleActorFlag_Dead;
                        action->ownerSession->Yobi().gateMode = YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero;
                        targetPayload->aliveFF8 = 1;
                    }
                    const std::int32_t sound = action->frameEvent - 10000;
                    static_cast<void>(audio::PlaySe_52C450(sound, 320));
                }
                const std::uint32_t categoryFlags = payload->effects1C[payload->effectRunningC9C].flags;
                if ((categoryFlags & BattleResultFlag_ReflectMagic) != 0) {
                    AdvanceBattleCastState_439340(target);
                    YobiAction* const scene = battle.sceneAction;
                    if (scene->phase == 8) {
                        ++scene->payload.battleScene->completedEntryCount00;
                    }
                } else if ((categoryFlags & BattleResultFlag_NullifyMagic) != 0) {
                    AdvanceBattleCastState_439340(target);
                    YobiAction* const scene = battle.sceneAction;
                    if (scene->phase == 8) {
                        ++scene->payload.battleScene->completedEntryCount00;
                    }
                } else if ((categoryFlags & BattleResultFlag_AbsorbMagic) != 0) {
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t index = payload->effectRunningC9C;
                    if ((payload->effects1C[index].flags & BattleResultFlag_RebirthGuard) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                    } else {
                        const BattleEntryEffectRecord& effect = payload->effects1C[index];
                        const std::int32_t amount = effect.position3;
                        (cg::world::gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness > compareTarget->maxHp)) {
                            YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                                payload->effects1C[payload->effectRunningC9C].position];
                            copyTarget->readiness = copyTarget->maxHp;
                        }
                        const std::int32_t displayIndex = payload->effectRunningC9C;
                        const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                        const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                        const std::int32_t targetY = target->y;
                        const std::int32_t targetX = target->x;
                        const std::int32_t y = targetY - 80;
                        static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_IncreaseHp, 2, displayFlags));
                        if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(
                            cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                    }
                    YobiAction* const scene = battle.sceneAction;
                    if (scene->phase == 8) {
                        ++scene->payload.battleScene->completedEntryCount00;
                    }
                } else if ((categoryFlags & BattleResultFlag_Barrier) != 0) {
                    AdvanceBattleCastState_439340(target);
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110474, 0));
                } else {
                    if ((categoryFlags & BattleResultFlag_WakeUp) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->buffEffectAction_1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->buffEffectAction_1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~BattleActorFlag_Sleep;
                    }
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t index = payload->effectRunningC9C;
                    const std::uint32_t flags = payload->effects1C[index].flags;
                    if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                    } else {
                        const std::int32_t targetY = target->y;
                        const std::int32_t displayed = payload->effects1C[index].position3;
                        const std::int32_t targetX = target->x;
                        const std::int32_t y = targetY - 80;
                        static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_DecreaseHp, 2, flags));
                        if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(
                            cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                    }
                }
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
            }
        } else {
            const std::int32_t index = payload->effectRunningC9C;
            const std::uint32_t flags = payload->effects1C[index].flags;
            if ((flags & BattleResultFlag_Avoidance) != 0) {
                target->phase = BattleAction_DodgeAndWaitAttacker;
            } else if ((flags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[index].position3 != 0) {
                if ((flags & BattleResultFlag_Critical) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                target->phase = BattleAction_RecoilOnHit;
                const std::int32_t event = action->frameEvent;
                const std::int32_t x = action->x;
                const std::int32_t sound = event >= 20000 ? event - 20000 : event - 10000;
                static_cast<void>(audio::PlaySe_52C450(sound, x));
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
            }
        }
        action->frameEvent = 0;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            payload->valueFD0 = remaining - 1;
        }
        if (payload->valueFD0 == 0) {
            if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
                const std::int32_t image = action->imageNo;
                if (image >= 110140 && image <= 110151) {
                    static_cast<void>(MarkDeath(action));
                }
                const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
                action->flags |= hasDevice ? YobiActionFlag_ForceAlpha80 : YobiActionFlag_SkipDrawWhenParityZero;
                ++action->nameY;
            }
            const std::int32_t previousFade = action->nameY;
            if (previousFade > 0) {
                action->nameY = previousFade + 1;
                if (previousFade >= 20) {
                    static_cast<void>(MarkDeath(action));
                }
            }
        }
    }
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x0041BB00。
void UpdateBattleMovingImpactEffect_41BB00(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto& battle = action->ownerSession->WorldScene().battle;
    const std::int32_t initialPhase = action->phase;
    auto* const payload = action->payload.battleActor;
    if (initialPhase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    action->phase = BattleImpactPhase_Running;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position];
    const std::int32_t targetGraphic = target->graphicIndex;
    auto* const targetPayload = target->payload.battleActor;
    static_cast<void>(render::LoadGraphic_522570(targetGraphic));
    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Barrier) != 0) {
        const std::int32_t halfHeight = render::ReadGraphicDxHeight(target->graphicIndex) / 2;
        const double threshold = static_cast<double>(halfHeight) + target->graphicOffsetY + target->origin_y_offset44 - 50.0;
        if (!(threshold >= action->origin_y_offset44) && action->frameEvent == 0) {
            const std::int32_t halfX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX) / 2;
            action->origin_x_offset40 = static_cast<float>(static_cast<double>(halfX) + action->origin_x_offset40);
            const std::int32_t halfY = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY) / 2;
            action->origin_y_offset44 = static_cast<float>(static_cast<double>(halfY) + action->origin_y_offset44);
            static_cast<void>(MarkDeath(action));
            static_cast<void>(NewBattleRandomEffect_419FC0(action, 110469, 0));
            static_cast<void>(NewBattleRandomEffect_419FC0(target, 110474, 0));
            AdvanceBattleCastState_439340(target);
        }
    }
    const std::int32_t halfHeight = render::ReadGraphicDxHeight(target->graphicIndex) / 2;
    const double threshold = static_cast<double>(halfHeight) + target->graphicOffsetY + target->origin_y_offset44;
    if (!(threshold >= action->origin_y_offset44) && action->frameEvent == 0) {
        const std::uint32_t initialFlags = payload->effects1C[payload->effectRunningC9C].flags;
        std::int32_t imageNo;
        if ((initialFlags & BattleResultFlag_Guard) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & BattleResultFlag_MagicGuard) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & BattleResultFlag_SpecialGuard) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2) + 110001;
        }
        FaceBattleActorTowardFormationPosition_406260(target);
        target->battleOpponent10 = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t resolvedIndex = payload->effectRunningC9C;
        const std::uint32_t resolvedFlags = payload->effects1C[resolvedIndex].flags;
        if ((resolvedFlags & BattleResultFlag_Avoidance) != 0) {
            target->phase = BattleAction_DodgeAndWaitAttacker;
        } else {
            if ((resolvedFlags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[resolvedIndex].position3 != 0) {
                const BattleEntryEffectRecord& effect = payload->effects1C[resolvedIndex];
                const std::int32_t amount = effect.position3;
                (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Death) != 0) {
                    targetPayload->flagsFDC |= BattleActorFlag_Dead;
                    action->ownerSession->Yobi().gateMode = YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t categoryFlags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((categoryFlags & BattleResultFlag_ReflectMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_NullifyMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_AbsorbMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                if ((payload->effects1C[index].flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[index];
                    const std::int32_t amount = effect.position3;
                    (cg::world::gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t displayIndex = payload->effectRunningC9C;
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_IncreaseHp, 2, displayFlags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else {
                if ((categoryFlags & BattleResultFlag_WakeUp) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->buffEffectAction_1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->buffEffectAction_1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~BattleActorFlag_Sleep;
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                AdvanceBattleCastState_439340(target);
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
                } else {
                    const std::int32_t x = target->x;
                    const std::int32_t displayed = payload->effects1C[index].position3;
                    const std::int32_t y = target->y - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, x, y, displayed, BattleNumber_DecreaseHp, 2, flags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                const std::int32_t image = action->imageNo;
                if (image >= 110032 && image <= 110034) {
                    static_cast<void>(audio::PlaySe_52C450(263, action->x));
                    static_cast<void>(audio::PlaySe_52C450(132, action->x));
                } else {
                    std::int32_t sound;
                    if (image >= 110035 && image <= 110037) {
                        sound = 263;
                    } else if (image >= 110038 && image <= 110040) {
                        sound = 132;
                    } else {
                        sound = 131;
                    }
                    static_cast<void>(audio::PlaySe_52C450(sound, action->x));
                }
            }
        }
        const std::int32_t delayIndex = payload->effectRunningC9C;
        if ((payload->effects1C[delayIndex].flags & BattleResultFlag_Avoidance) == 0 && payload->effects1C[delayIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
        }
        action->frameEvent = 1;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        if (previousDelay == 0) {
            static_cast<void>(audio::PlaySe_52C450(262, action->x));
            const std::int32_t frames = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 32);
            for (std::int32_t remaining = frames; remaining > 0; --remaining) {
                static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            }
        }
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            const std::int32_t index = payload->effectRunningC9C;
            payload->valueFD0 = remaining - 1;
            if ((payload->effects1C[index].flags & BattleResultMask_MagicReaction) == 0) {
                const std::int32_t level = payload->castSkillLevel1044;
                action->ownerSession->Yobi().screenOffsetRising = 1;
                action->ownerSession->Yobi().screenOffsetMaximumAmplitude = static_cast<std::uint32_t>(level / 2 + 1);
            }
        }
        if (payload->valueFD0 == 0) {
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            const std::int32_t movingHalfHeight = render::ReadGraphicDxHeight(target->graphicIndex) / 2;
            const float x = target->origin_x_offset40;
            const float centerY = static_cast<float>(
                static_cast<double>(movingHalfHeight) + target->graphicOffsetY + target->origin_y_offset44);
            TurnBattleTowardPosition_4060D0(action, x, centerY, 0, 0.0F);
            MoveBattleByAngle_405EA0(action);
            const double speed = action->motionMagnitude54 + 0.5;
            const std::int32_t hit = action->frameEvent;
            action->motionMagnitude54 = static_cast<float>(speed);
            if (hit == 0) {
                const std::int32_t image = action->imageNo;
                if (image >= 110032 && image <= 110034) {
                    if (action->nameX == 0) {
                        const float y = action->origin_y_offset44;
                        static_cast<void>(NewBattleTrailEffect_41E7C0(action, action->origin_x_offset40, y, 110007, BattleTrailRenderMode_LeaveFlag20000, 200));
                    }
                } else if (image >= 110035 && image <= 110037) {
                    if (action->nameX == 0) {
                        const float y = action->origin_y_offset44;
                        static_cast<void>(NewBattleTrailEffect_41E7C0(action, action->origin_x_offset40, y, 110007, BattleTrailRenderMode_LeaveFlag20000, 200));
                    }
                } else if (image >= 110038 && image <= 110040) {
                    if (action->nameX == 0) {
                        const float y = action->origin_y_offset44;
                        static_cast<void>(NewBattleTrailEffect_41E7C0(action, action->origin_x_offset40, y, 110006, BattleTrailRenderMode_LeaveFlag20000, 200));
                    }
                    ++action->nameX;
                }
            }
            const std::int32_t previousTrail = action->nameX;
            action->nameX = previousTrail < 2 ? previousTrail + 1 : 0;
            if (action->frameEvent == 1) {
                static_cast<void>(MarkDeath(action));
                action->ownerSession->Yobi().screenOffsetRising = 0;
            }
        }
    }
    const std::uint8_t targetPriority = target->displayPriority;
    const float projectedX = action->origin_x_offset40;
    action->displayPriority = static_cast<std::uint8_t>(targetPriority + 20);
    const std::int32_t x = static_cast<std::int32_t>(projectedX);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x0041C3B0。
void UpdateBattleHomingImpactEffect_41C3B0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto& battle = action->ownerSession->WorldScene().battle;
    const std::int32_t initialPhase = action->phase;
    auto* const payload = action->payload.battleActor;
    if (initialPhase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    action->phase = BattleImpactPhase_Running;
    const std::int32_t initialIndex = payload->effectRunningC9C;
    if ((payload->effects1C[initialIndex].flags & BattleResultFlag_Barrier) != 0 && action->frameEvent == 0
        && GetBattleDistance_406300(action, cg::world::gBattleAllEntries59E980[payload->effects1C[initialIndex].position]) <= 70) {
        const std::int32_t index = payload->effectRunningC9C;
        const double x = static_cast<double>(static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX)) + action->origin_x_offset40;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[payload->effects1C[index].position];
        action->origin_x_offset40 = static_cast<float>(x);
        action->origin_y_offset44 = static_cast<float>(
            static_cast<double>(static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY)) + action->origin_y_offset44);
        static_cast<void>(MarkDeath(action));
        static_cast<void>(NewBattleRandomEffect_419FC0(action, 110475, 0));
        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110474, 0));
        AdvanceBattleCastState_439340(target);
    }
    if (action->frameEvent == 0
        && GetBattleDistance_406300(action, cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]) <= 8) {
        const std::int32_t index = payload->effectRunningC9C;
        const std::int32_t position = payload->effects1C[index].position;
        const std::uint32_t initialFlags = payload->effects1C[index].flags;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((initialFlags & BattleResultFlag_Guard) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & BattleResultFlag_MagicGuard) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & BattleResultFlag_SpecialGuard) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2) + 110001;
        }
        target->battleOpponent10 = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t resolvedIndex = payload->effectRunningC9C;
        const std::uint32_t resolvedFlags = payload->effects1C[resolvedIndex].flags;
        if ((resolvedFlags & BattleResultFlag_Avoidance) != 0) {
            target->phase = BattleAction_DodgeAndWaitAttacker;
        } else {
            if ((resolvedFlags & BattleResultMask_MagicReaction) == 0 && payload->effects1C[resolvedIndex].position3 != 0) {
                const BattleEntryEffectRecord& effect = payload->effects1C[resolvedIndex];
                const std::int32_t amount = effect.position3;
                (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Death) != 0) {
                    targetPayload->flagsFDC |= BattleActorFlag_Dead;
                    action->ownerSession->Yobi().gateMode = YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t categoryFlags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((categoryFlags & BattleResultFlag_ReflectMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_NullifyMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else if ((categoryFlags & BattleResultFlag_AbsorbMagic) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t effectIndex = payload->effectRunningC9C;
                if ((payload->effects1C[effectIndex].flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[effectIndex].position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[effectIndex];
                    const std::int32_t amount = effect.position3;
                    (cg::world::gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t displayIndex = payload->effectRunningC9C;
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                    const std::int32_t targetY = target->y;
                    const std::int32_t targetX = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, targetX, y, displayed, BattleNumber_IncreaseHp, 2, displayFlags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                YobiAction* const scene = battle.sceneAction;
                if (scene->phase == 8) {
                    ++scene->payload.battleScene->completedEntryCount00;
                }
            } else {
                if ((categoryFlags & BattleResultFlag_WakeUp) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->buffEffectAction_1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->buffEffectAction_1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~BattleActorFlag_Sleep;
                }
                if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                TurnBattleToward_406190(target, action, 1, 0.0F);
                AdvanceBattleCastState_439340(target);
                const std::int32_t effectIndex = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[effectIndex].flags;
                if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[effectIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t targetY = target->y;
                    const std::int32_t displayed = payload->effects1C[effectIndex].position3;
                    const std::int32_t x = target->x;
                    const std::int32_t y = targetY - 80;
                    static_cast<void>(NewBattleNumber_41F1A0(action, x, y, displayed, BattleNumber_DecreaseHp, 2, flags));
                    if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_Critical) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
                }
                const std::int32_t image = action->imageNo;
                if (image == 111011 || image == 110044 || image == 110045 || image == 110046 || image == 110047) {
                    const std::int32_t sound = image == 110046 || image == 110047 ? 268 : 269;
                    static_cast<void>(audio::PlaySe_52C450(sound, action->x));
                }
            }
        }
        const std::int32_t delayIndex = payload->effectRunningC9C;
        if ((payload->effects1C[delayIndex].flags & BattleResultFlag_Avoidance) == 0 && payload->effects1C[delayIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC & BattleActorFlag_Dead) != 0) + 1u);
        }
        action->frameEvent = 1;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        if (previousDelay == 0) {
            const std::int32_t image = action->imageNo;
            if (image == 111011 || image == 110044 || image == 110045 || image == 110046 || image == 110047) {
                const std::int32_t sound = image == 111011 || image == 110044 ? 269 : 268;
                static_cast<void>(audio::PlaySe_52C450(sound, action->x));
            }
        }
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            payload->valueFD0 = remaining - 1;
        }
        if (payload->valueFD0 == 0) {
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            if (action->frameEvent == 0) {
                const float step = static_cast<float>(static_cast<double>(action->battleTurnStepB4) + 0.1F);
                action->battleTurnStepB4 = step;
                YobiAction* const target = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                TurnBattleToward_406190(action, target, 0, step);
            } else if (!(action->origin_x_offset40 >= -200.0F) || action->origin_x_offset40 > 840.0F
                || !(action->origin_y_offset44 >= -200.0F) || action->origin_y_offset44 > 680.0F) {
                static_cast<void>(MarkDeath(action));
            }
            MoveBattleByAngle_405EA0(action);
            const std::int32_t image = action->imageNo;
            bool emitTrail = false;
            // emitTrail仅在三项均赋值后置true；初值不会进入创建调用。
            float trailY = 0.0F;
            std::int32_t randomX = 0;
            std::int32_t trailImage = 0;
            if (image == 111011) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    emitTrail = true;
                }
            } else if (image == 110044) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    emitTrail = true;
                }
            } else if (image == 110045) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -6, 6);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -6, 6);
                    emitTrail = true;
                }
            } else if (image == 110046) {
                if (action->nameX == 0) {
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -4, 4);
                    const float y = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    const std::int32_t xOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -4, 4);
                    const float x = static_cast<float>(static_cast<double>(xOffset) + action->origin_x_offset40);
                    static_cast<void>(NewBattleTrailEffect_41E7C0(action, x, y, 110006, BattleTrailRenderMode_AddFlag20000, 202));
                }
                ++action->nameX;
            } else if (image == 110047) {
                if (action->nameX == 0) {
                    trailImage = 110006;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -2, 2);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -2, 2);
                    emitTrail = true;
                }
            }
            if (emitTrail) {
                const float x = static_cast<float>(static_cast<double>(randomX) + action->origin_x_offset40);
                static_cast<void>(NewBattleTrailEffect_41E7C0(action, x, trailY, trailImage, BattleTrailRenderMode_AddFlag20000, 202));
            }
            const std::int32_t previousTrail = action->nameX;
            action->nameX = previousTrail < 2 ? previousTrail + 1 : 0;
        }
    }
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x004219D0。
void UpdateBattleCarryingEffect_4219D0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    bool carry = false;
    auto* const payload = action->payload.battleActor;
    if (action->phase == BattleImpactPhase_WaitForTargetIdle) {
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase != BattleAction_Idle) {
            return;
        }
    }
    const std::int32_t initialEvent = action->frameEvent;
    action->phase = BattleImpactPhase_Running;
    if (initialEvent == 0
        && GetBattleDistance_406300(action, cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]) <= 8) {
        const std::int32_t index = payload->effectRunningC9C;
        const std::int32_t sourcePosition = payload->battlePosition18;
        const std::int32_t targetPosition = payload->effects1C[index].position;
        YobiAction* const source = cg::world::gBattleAllEntries59E980[sourcePosition];
        YobiAction* const target = cg::world::gBattleAllEntries59E980[targetPosition];
        auto* const targetPayload = target->payload.battleActor;
        target->battleOpponent10 = source;
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t resolvedIndex = payload->effectRunningC9C;
        const std::uint32_t flags = payload->effects1C[resolvedIndex].flags;
        if ((flags & BattleResultFlag_Avoidance) != 0) {
            target->phase = BattleAction_DodgeAndWaitAttacker;
        } else {
            if ((flags & BattleResultFlag_RebirthGuard) != 0) {
                static_cast<void>(NewBattleFloatingIcon_41F4C0(
                    cg::world::gBattleAllEntries59E980[payload->effects1C[resolvedIndex].position], 22459, 0, 0));
            }
            const std::int32_t image = action->imageNo;
            if (image == 111011 || image == 110044 || image == 110045 || image == 110046 || image == 110047) {
                const std::int32_t sound = image == 110046 || image == 110047 ? 268 : 269;
                static_cast<void>(audio::PlaySe_52C450(sound, action->x));
            }
            if ((payload->effects1C[payload->effectRunningC9C].flags & BattleResultFlag_RebirthGuard) != 0) {
                AdvanceBattleCastState_439340(target);
            }
        }
        action->frameEvent = 1;
    }
    const std::int32_t previousDelay = action->field58;
    action->field58 = previousDelay - 1;
    if (previousDelay <= 0) {
        if (previousDelay == 0) {
            const std::int32_t image = action->imageNo;
            if (image == 111011 || image == 110044 || image == 110045 || image == 110046 || image == 110047) {
                const std::int32_t sound = image == 111011 || image == 110044 ? 269 : 268;
                static_cast<void>(audio::PlaySe_52C450(sound, action->x));
            }
        }
        const std::int32_t remaining = payload->valueFD0;
        if (remaining > 0) {
            payload->valueFD0 = remaining - 1;
        }
        if (payload->valueFD0 == 0) {
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            if (action->frameEvent == 0) {
                const float step = static_cast<float>(static_cast<double>(action->battleTurnStepB4) + 0.1F);
                action->battleTurnStepB4 = step;
                YobiAction* const target = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                TurnBattleToward_406190(action, target, 0, step);
            } else {
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & BattleResultFlag_Avoidance) == 0 && (flags & BattleResultFlag_RebirthGuard) == 0) {
                    carry = true;
                }
                if (!(action->origin_x_offset40 >= -200.0F) || action->origin_x_offset40 > 840.0F
                    || !(action->origin_y_offset44 >= -200.0F) || action->origin_y_offset44 > 680.0F) {
                    if (carry) {
                        AdvanceBattleCastState_439340(cg::world::gBattleAllEntries59E980[payload->effects1C[index].position]);
                    }
                    static_cast<void>(MarkDeath(action));
                }
            }
            MoveBattleByAngle_405EA0(action);
            if (carry) {
                YobiAction* const target = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                target->characterAction = CharacterAnimation_Damage;
                const std::int32_t tick = action->field58 & 0x1F;
                std::int32_t arc;
                if (tick < 16) {
                    const std::int32_t distance = tick - 8;
                    arc = distance * distance / 2;
                } else {
                    const std::int32_t distance = tick - 24;
                    arc = 64 - distance * distance / 2;
                }
                const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
                const std::int32_t direction = target->direction + 1;
                target->origin_x_offset40 = static_cast<float>(static_cast<double>(offsetX) + action->origin_x_offset40);
                const double y = action->origin_y_offset44 - 35.0 - arc
                    + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
                target->direction = direction;
                target->origin_y_offset44 = static_cast<float>(y);
                if (direction >= 8) {
                    target->direction = direction - 8;
                }
            }
            const std::int32_t image = action->imageNo;
            bool emitTrail = false;
            // emitTrail仅在三项均赋值后置true；初值不会进入创建调用。
            float trailY = 0.0F;
            std::int32_t randomX = 0;
            std::int32_t trailImage = 0;
            if (image == 111011) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    emitTrail = true;
                }
            } else if (image == 110044) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -10, 10);
                    emitTrail = true;
                }
            } else if (image == 110045) {
                if (action->nameX == 0) {
                    trailImage = 110007;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -6, 6);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -6, 6);
                    emitTrail = true;
                }
            } else if (image == 110046) {
                if (action->nameX == 0) {
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -4, 4);
                    const float y = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    const std::int32_t xOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -4, 4);
                    const float x = static_cast<float>(static_cast<double>(xOffset) + action->origin_x_offset40);
                    static_cast<void>(NewBattleTrailEffect_41E7C0(action, x, y, 110006, BattleTrailRenderMode_AddFlag20000, 202));
                }
                ++action->nameX;
            } else if (image == 110047) {
                if (action->nameX == 0) {
                    trailImage = 110006;
                    const std::int32_t randomY = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -2, 2);
                    trailY = static_cast<float>(static_cast<double>(randomY) + action->origin_y_offset44);
                    randomX = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -2, 2);
                    emitTrail = true;
                }
            }
            if (emitTrail) {
                const float x = static_cast<float>(static_cast<double>(randomX) + action->origin_x_offset40);
                static_cast<void>(NewBattleTrailEffect_41E7C0(action, x, trailY, trailImage, BattleTrailRenderMode_AddFlag20000, 202));
            }
            const std::int32_t previousTrail = action->nameX;
            action->nameX = previousTrail < 2 ? previousTrail + 1 : 0;
        }
    }
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const float y = action->origin_y_offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
}

// 原函数：0x00422E50。
void InitBattleTargetEffect_422E50(
    YobiAction* const action, YobiAction* const source, const YobiCallback callback)
{
    using world::gBattleAllEntries59E980;
    if (action == nullptr || source == nullptr) {
        return;
    }
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
    action->callback = callback;
    action->displayPriority = 0xC9;
    const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
    const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
    const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
    const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
    const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
    if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
        std::int32_t position;
        std::int32_t value3;
        std::int32_t value4;
        std::int32_t value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->framePlayingRate = 100;
    if (sourcePayload->effectRunningC9C != 0) {
        action->field58 = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 30);
    }
    action->imageNo = sourcePayload->image102C;
    switch (action->imageNo) {
    case 111177:
    case 111178:
    case 111179:
    case 111183:
    case 111184:
    case 111185:
    case 111186:
    case 111187:
    case 111188:
        static_cast<void>(audio::PlaySe_52C450(263, action->x));
        break;
    case 111189:
    case 111190:
    case 111191:
        static_cast<void>(audio::PlaySe_52C450(269, action->x));
        break;
    default:
        break;
    }
    payload->castSkill103C = sourcePayload->castSkill103C;
    payload->castSkillLevel1044 = sourcePayload->castSkillLevel1044;
    payload->skillMeta = sourcePayload->skillMeta; // E01／422FB8。
    static_cast<void>(render::LoadGraphic_522570(target->graphicIndex));
    action->origin_x_offset40 = target->origin_x_offset40;
    action->origin_y_offset44 = target->origin_y_offset44;
    action->pathCountOrDrawOrder_270 = 0;
}

// 原函数：0x00423040。
void InitBattleOffsetEffect_423040(
    YobiAction* const action, YobiAction* const source, const YobiCallback callback)
{
    using world::gBattleAllEntries59E980;
    if (action == nullptr || source == nullptr) {
        return;
    }
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
    action->callback = callback;
    action->flags |= YobiActionFlag_FourPhaseUpdateGate;
    const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
    const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
    const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
    const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
    const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
    if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
        std::int32_t position;
        std::int32_t value3;
        std::int32_t value4;
        std::int32_t value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->framePlayingRate = 100;
    if (sourcePayload->effectRunningC9C != 0) {
        action->field58 = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 30);
    }
    const std::int32_t imageOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2);
    action->imageNo = imageOffset + sourcePayload->image102C;
    payload->castSkill103C = sourcePayload->castSkill103C;
    payload->castSkillLevel1044 = sourcePayload->castSkillLevel1044;
    payload->skillMeta = sourcePayload->skillMeta; // E01／423152。
    static_cast<void>(render::LoadGraphic_522570(target->graphicIndex));
    const std::int32_t positionIndex = sourcePayload->effectRunningC9C;
    const double x = target->origin_x_offset40;
    const std::int32_t position = sourcePayload->effects1C[positionIndex].position;
    action->origin_x_offset40 = static_cast<float>(position >= 10 ? x + 100.0 : x - 100.0);
    const std::int32_t graphic = target->graphicIndex;
    const double targetY = target->origin_y_offset44;
    const std::int32_t halfHeight = render::ReadGraphicDxHeight(graphic) / 2;
    const double y = targetY - 200.0 - halfHeight;
    action->pathCountOrDrawOrder_270 = 0;
    action->origin_y_offset44 = static_cast<float>(y);
}

// 原函数：0x004231D0。
void InitBattleCenterEffect_4231D0(
    YobiAction* const action, YobiAction* const source, const YobiCallback callback)
{
    using world::gBattleAllEntries59E980;
    if (action == nullptr || source == nullptr) {
        return;
    }
    const std::uint32_t initialFlags = action->flags;
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    action->callback = callback;
    action->displayPriority = 0xC9;
    action->flags = initialFlags | YobiActionMask_BattleDepthSortOrFourPhaseUpdateGate;
    const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
    auto* const payload = action->payload.battleActor;
    const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
    const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
    const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
    const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
    if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
        std::int32_t position;
        std::int32_t value3;
        std::int32_t value4;
        std::int32_t value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->framePlayingRate = 100;
    if (sourcePayload->effectRunningC9C != 0) {
        action->field58 = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 30);
    }
    action->imageNo = sourcePayload->image102C;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
    action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
    const std::int32_t speed = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 4, 8);
    action->motionMagnitude54 = static_cast<float>(speed);
    payload->castSkill103C = sourcePayload->castSkill103C;
    payload->castSkillLevel1044 = sourcePayload->castSkillLevel1044;
    payload->skillMeta = sourcePayload->skillMeta; // E01／4232E9。
    action->origin_x_offset40 = 320.0F;
    action->origin_y_offset44 = 260.0F;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
    TurnBattleToward_406190(action, target, 0, 0.0F);
    const std::int32_t angleOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), -90, 90);
    action->pathCountOrDrawOrder_270 = 0;
    action->angle50 = static_cast<float>(static_cast<double>(angleOffset) + action->angle50);
}

// 原函数：0x00423350。
void InitBattleLowerEffect_423350(
    YobiAction* const action, YobiAction* const source, const YobiCallback callback)
{
    using world::gBattleAllEntries59E980;
    if (action == nullptr || source == nullptr) {
        return;
    }
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
    action->callback = callback;
    action->displayPriority = 0xC9;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t blendFlags = action->flags | (hasDevice ? YobiActionFlag_ForceAlpha80 : YobiActionFlag_SkipDrawWhenParityZero);
    action->flags = blendFlags;
    action->flags = blendFlags | YobiActionFlag_FourPhaseUpdateGate;
    const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
    const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
    const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
    const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
    const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
    if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
        std::int32_t position;
        std::int32_t value3;
        std::int32_t value4;
        std::int32_t value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->framePlayingRate = 100;
    if (sourcePayload->effectRunningC9C != 0) {
        action->field58 = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 30);
    }
    const std::int32_t imageOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2);
    action->imageNo = imageOffset + sourcePayload->image102C;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
    action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
    action->origin_x_offset40 = target->origin_x_offset40;
    const std::int32_t halfOffset = target->graphicOffsetY / 2;
    const double y = static_cast<double>(halfOffset) + target->origin_y_offset44;
    action->pathCountOrDrawOrder_270 = 0;
    action->origin_y_offset44 = static_cast<float>(y);
}

// 原函数：0x004234B0。
void InitBattleScreenYEffect_4234B0(
    YobiAction* const action, YobiAction* const source, const YobiCallback callback,
    std::int32_t* const side)
{
    using world::gBattleAllEntries59E980;
    if (action == nullptr || source == nullptr) {
        return;
    }
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    YobiAction* const target = cg::world::gBattleAllEntries59E980[
        sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
    const std::uint32_t initialFlags = action->flags;
    action->callback = callback;
    action->displayPriority = 0xCC;
    action->flags = initialFlags | YobiActionMask_BattleDepthSortOrFourPhaseUpdateGate;
    const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
    const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
    const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
    const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
    const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
    if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
        std::int32_t position;
        std::int32_t value3;
        std::int32_t value4;
        std::int32_t value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->framePlayingRate = 100;
    if (sourcePayload->effectRunningC9C != 0) {
        action->field58 = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 30);
    }
    const std::int32_t skill = sourcePayload->castSkill103C;
    std::int32_t image;
    if (skill == BattleSkill_AttackMagicWater || skill == BattleSkill_AttackMagicWaterStrong
        || skill == BattleSkill_AttackMagicWaterSuper) {
        image = sourcePayload->image102C;
    } else {
        const std::int32_t imageOffset = platform::RandomRange_46FAF0(action->ownerSession->Connection(), 0, 2);
        image = imageOffset + sourcePayload->image102C;
    }
    action->imageNo = image;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
    action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
    action->origin_x_offset40 = target->origin_x_offset40;
    if (side != nullptr) {
        *side = sourcePayload->effects1C[sourcePayload->effectRunningC9C].position >= 10 ? 1 : -1;
    }
    const std::int32_t y = target->y + 1;
    action->pathCountOrDrawOrder_270 = 0;
    action->origin_y_offset44 = static_cast<float>(y);
}

// BHE01：计数与上限持样保持，标死后仍取Y再发布X，见命中特效SPEC。
// 原函数：0x0041F570。
void UpdateBattleHitEffect_41F570(YobiAction* const action)
{
    const std::int32_t previous = action->deltaX;
    const std::int32_t maximum = action->deltaY;
    const std::int32_t count = previous + 1;
    action->deltaX = count;
    if (count >= maximum) {
        static_cast<void>(MarkDeath(action));
    }
    const std::int32_t screenX = static_cast<std::int32_t>(action->origin_x_offset40);
    const float screenY = action->origin_y_offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// BHE01：创建字段/播放后YX投影取样见doc/cg2d_battle_hit_effect_lifecycle_spec.md。
// 原函数：0x0041F5B0。
YobiAction* NewBattleHitEffect_41F5B0(
    YobiAction* const parent, const std::int32_t offsetX, const std::int32_t offsetY,
    const std::int32_t lifetime, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleHitEffect_41F570;
    action->displayPriority = 0xCC;
    action->flags = flags | YobiActionFlag_FourPhaseUpdateGate;
    action->framePlayingRate = 100;
    action->origin_x_offset40 = static_cast<float>(static_cast<double>(offsetX) + parent->origin_x_offset40);
    const double y = static_cast<double>(offsetY) + parent->origin_y_offset44;
    action->verticalVelocityBits4C = 0xC1000000u;
    action->imageNo = imageNo;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
    action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
    action->origin_y_offset44 = static_cast<float>(y);
    action->deltaY = lifetime;
    action->direction = YobiDirection_UpLeft;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    const std::int32_t screenX = static_cast<std::int32_t>(action->origin_x_offset40);
    const float screenY = action->origin_y_offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

} // namespace detail

} // namespace cg::yobi
