#include "battle_status.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/Palette.hpp"
#include "cg/render/RendererResources.hpp"

#include "animation.hpp"
#include "battle_attachment.hpp"
#include "battle_display.hpp"
#include "battle_effect.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <array>
#include <bit>
#include <cstdint>

namespace cg::yobi {

namespace detail {



static constexpr std::array<std::array<std::int32_t, 2>, 4> kRebirthExitImages4118BF {{
    {110423, 110424}, {110435, 110436}, {110447, 110448}, {110459, 110460},
}};

static constexpr std::array<std::int32_t, 4> kRebirthExitSounds411901 {426, 428, 430, 432};

// BHRC01：完整FE4后低WORD门禁；角度持样加180/发布/单次减360，移动后播放。
// 原函数：0x0040A9E0。
void UpdateBattleAction26_40A9E0(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    const double angle = action->angle50 + 180.0;
    action->framePlayingRate = 50;
    action->motionMagnitude54 = 5.0F;
    action->angle50 = static_cast<float>(angle);
    if (angle >= 360.0) {
        action->angle50 = static_cast<float>(angle - 360.0);
    }
    MoveBattleByAngle_405EA0(action);
    static_cast<void>(PlayFrame_517DA0(action, 1));
}

// BHRC01：入口FDC持样与阶段fresh门禁分开；计数先于FF8清零，创建后重取周期。
// 原函数：0x0040AA70。
std::int32_t UpdateBattleAction27_40AA70(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::uint32_t initialFlags = payload->flagsFDC;
    const std::int32_t initialBit = static_cast<std::int32_t>((initialFlags >> 1) & 1u);
    const std::int32_t count = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = count;
    if (count < 14 * (initialBit + 1)) {
        const double angle = action->angle50 + 180.0;
        action->motionMagnitude54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & BattleActorFlag_Dead) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count < 14 * initialBit + 34) {
        if ((static_cast<std::uint8_t>(payload->effectFlagsFE4) & 2) != 0) {
            const std::int32_t direction = action->direction + 1;
            action->direction = direction;
            if (direction >= 8) {
                action->direction = direction - 8;
            }
        }
        const std::int32_t threshold = 14 * (static_cast<std::int32_t>((payload->flagsFDC & BattleActorFlag_Dead) != 0) + 1);
        if (payload->actionFrameCountFCC == threshold) {
            action->motionMagnitude54 = 5.0F;
            if ((static_cast<std::uint8_t>(payload->flagsFDC) & BattleActorFlag_Dead) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                payload->aliveFF8 = 0;
            }
            payload->valueFF0 = 0;
        }
        if (payload->valueFF0 == 0) {
            static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
        }
        const std::int32_t previous = payload->valueFF0;
        payload->valueFF0 = previous + 1;
        if (previous >= 2) {
            payload->valueFF0 = 0;
        }
        const double speed = static_cast<double>(action->motionMagnitude54) - 0.3F;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (!(speed >= 0.0)) {
            action->motionMagnitude54 = 0.0F;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 14 * initialBit + 54) {
        payload->actionFrameCountFCC = 0;
        if ((static_cast<std::uint8_t>(initialFlags) & BattleActorFlag_Dead) == 0) {
            action->motionMagnitude54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
            if (action->phase == BattleAction_ReturnToFormation) {
                action->motionMagnitude54 = 4.0F;
                action->framePlayingRate = 100;
            }
        } else if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0x3010) != 0) {
            action->motionMagnitude54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
        } else {
            action->motionMagnitude54 = 1.0F;
            action->framePlayingRate = 250;
            AdvanceBattleCastState_439340(action);
        }
    }
    return PlayFrame_517DA0(action, 1);
}

// BHRC01：音效后fresh计数，14/34/54分段；原减速先发布/负值归零保持。
// 原函数：0x0040ACF0。
std::int32_t UpdateBattleAction28_40ACF0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if (payload->actionFrameCountFCC == 0) {
        static_cast<void>(audio::PlaySe_52C450(132, action->x));
    }
    action->framePlayingRate = 50;
    const std::int32_t count = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = count;
    if (count < 14) {
        const double angle = action->angle50 + 180.0;
        action->motionMagnitude54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count < 34) {
        if (count == 14) {
            action->motionMagnitude54 = 5.0F;
            payload->valueFF0 = 0;
        }
        if (payload->valueFF0 == 0) {
            static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
        }
        const std::int32_t previous = payload->valueFF0;
        payload->valueFF0 = previous + 1;
        if (previous >= 2) {
            payload->valueFF0 = 0;
        }
        const double speed = static_cast<double>(action->motionMagnitude54) - 0.3F;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (!(speed >= 0.0)) {
            action->motionMagnitude54 = 0.0F;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 54) {
        payload->actionFrameCountFCC = 0;
        action->motionMagnitude54 = 4.0F;
        action->framePlayingRate = 100;
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BHRC01：低WORD入口；新计数<14后退，>=44才归零/推进，末播放。
// 原函数：0x0040AE50。
std::int32_t UpdateBattleAction29_40AE50(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::int32_t count = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = count;
    if (count < 14) {
        const double angle = action->angle50 + 180.0;
        action->motionMagnitude54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 44) {
        payload->actionFrameCountFCC = 0;
        action->framePlayingRate = 100;
        action->motionMagnitude54 = 4.0F;
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BHRC01：低WORD入口；计数>=14才fresh完整FDC取14/28阈值，末播放。
// 原函数：0x0040AF30。
std::int32_t UpdateBattleAction30_40AF30(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::int32_t count = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = count;
    if (count < 14) {
        const double angle = action->angle50 + 180.0;
        action->motionMagnitude54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 14 * (static_cast<std::int32_t>((payload->flagsFDC & BattleActorFlag_Dead) != 0) + 1)) {
        payload->actionFrameCountFCC = 0;
        action->framePlayingRate = 100;
        action->motionMagnitude54 = 4.0F;
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BHRC01：持高W32旗/偶数旋转后fresh计数；等待other14，方向后重取低WORD选速度。
// 原函数：0x0040B020。
std::int32_t UpdateBattleAction31_40B020(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->framePlayingRate = 100;
    const std::int32_t previous = payload->actionFrameCountFCC;
    const std::int32_t flags = payload->effectFlagsFE4;
    const std::int32_t count = previous + 1;
    payload->actionFrameCountFCC = count;
    if ((flags & 0x08000000) != 0 && count % 2 == 0) {
        const std::int32_t direction = action->direction + 1;
        action->direction = direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
    }
    const std::int32_t current = payload->actionFrameCountFCC;
    if (current < 14) {
        action->characterAction = 1;
        action->motionMagnitude54 = -4.0F;
        MoveBattleByAngle_405EA0(action);
    } else if (current < 28) {
        action->characterAction = 0;
    } else {
        YobiAction* const other = action->battleOpponent10;
        if (other == nullptr || other->phase != BattleAction_PerformMeleeAttack) {
            if ((payload->effectFlagsFE4 & 0x08000000) != 0) {
                UpdateBattleDirectionFromAngle_405CB0(action);
            }
            action->characterAction = 0;
            action->motionMagnitude54 = (static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0x1010) != 0 ? 6.0F : 4.0F;
            payload->actionFrameCountFCC = 0;
            AdvanceBattleCastState_439340(action);
        }
    }
    return PlayFrame_517DA0(action, 1);
}

// BSA01：仅正cost实际扣FP/比较，负值三字段零归，临时销毁后固定payload记-1。
// 原函数：0x00405B60。
void ConsumeBattleFpCost_405B60(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t cost = payload->fpUsed1008;
    if (cost > 0) {
        (action->barValue1B0 -= static_cast<std::int32_t>(cost));
        if ((action->barValue1B0 < 0)) {
            // 原405BA8/AF/B2先mask后encoded，不使用写序不同的同类operator=。
            // 现由整数赋值统一三字段发布；一次RNG，目标token／reserved保持。
            action->barValue1B0 = std::int32_t{0};
        }
        payload->fpUsed1008 = -1;
    }
}

// 4107C0/410B00排除七处不同收尾后245条规范化指令相同；仅复用数值段。
static void ApplyBattleResourceChange_4107C0_410B00(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t displayType = payload->numberDisplayType1034;
    if (displayType != BattleNumber_Style0) {
        const std::int32_t y = action->y - 80;
        const std::int32_t x = action->x;
        static_cast<void>(NewBattleNumber_41F1A0(x, y, payload->value1030, displayType, 2, 0));
    }
    switch (payload->numberDisplayType1034) {
    case BattleNumber_DecreaseHp:
        (action->readiness -= static_cast<std::int32_t>(payload->value1030));
        if ((action->readiness < 0)) {
            action->readiness = std::int32_t{0};
        }
        break;
    case BattleNumber_IncreaseHp:
        (action->readiness += static_cast<std::int32_t>(payload->value1030));
        if ((action->maxHp < action->readiness)) {
            action->readiness = action->maxHp;
        }
        break;
    case BattleNumber_IncreaseFp:
        (action->barValue1B0 += static_cast<std::int32_t>(payload->value1030));
        if ((action->barMaximum1C0 < action->barValue1B0)) {
            action->barValue1B0 = action->barMaximum1C0;
        }
        break;
    case BattleNumber_DecreaseFp:
        (action->barValue1B0 -= static_cast<std::int32_t>(payload->value1030));
        if ((0 > action->barValue1B0)) {
            action->barValue1B0 = std::int32_t{0};
        }
        break;
    case BattleNumber_Style0:
        static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22459, 0, 0));
        break;
    case BattleNumber_SetHp: {
        action->readiness = static_cast<std::int32_t>(payload->value1030);
        if ((0 > action->readiness)) {
            action->readiness = std::int32_t{0};
        }
        if ((action->maxHp < action->readiness)) {
            action->readiness = action->maxHp;
        }
        break;
    }
    case BattleNumber_SetFp: {
        action->barValue1B0 = static_cast<std::int32_t>(payload->value1030);
        if ((0 > action->barValue1B0)) {
            action->barValue1B0 = std::int32_t{0};
        }
        if ((action->barMaximum1C0 < action->barValue1B0)) {
            action->barValue1B0 = action->barMaximum1C0;
        }
        break;
    }
    default:
        break;
    }
}

// 原函数：0x004107C0。
void UpdateBattleAction159_4107C0(YobiAction* const action)
{
    ApplyBattleResourceChange_4107C0_410B00(action);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00410B00。
void UpdateBattleAction160_410B00(YobiAction* const action)
{
    ApplyBattleResourceChange_4107C0_410B00(action);
    action->phase = BattleAction_Wait60Ticks;
}

// 原函数：0x00410E30。
void UpdateBattleAction162_410E30(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->effectPlaybackState1024 == BattleEffectPlayback_Completed) {
        payload->effectPlaybackState1024 = BattleEffectPlayback_Idle;
        action->hidden = 0;
        action->field2D8 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004113A0。
void UpdateBattleAction163_4113A0(YobiAction* const action)
{
    // 原0x004116DE立即数表；进入阶段音效，保留原项序。
    static constexpr std::array<std::int32_t, 4> kRebirthEnterSounds {425, 427, 429, 431};
    // 原0x00411708立即数表；有效类型1..4，不补第0行。
    static constexpr std::array<std::array<std::int32_t, 2>, 4> kRebirthEnterImages {{
        {110420, 110421}, {110432, 110433}, {110444, 110445}, {110456, 110457},
    }};
    auto* const payload = action->payload.battleActor;
    const std::int32_t initialEffect = payload->effectRunningC9C;
    if ((payload->effects1C[initialEffect].flags & 0x80000u) != 0) {
        switch (payload->valueFD4) {
        case 0: {
            const std::int32_t direction = action->direction;
            const bool side = direction < 6 && static_cast<std::uint32_t>(direction) > 2u;
            const std::int32_t soundX = action->x;
            const std::int32_t sound = kRebirthEnterSounds[payload->effects1C[initialEffect].position];
            static_cast<void>(audio::PlaySe_52C450(sound, soundX));
            SetYobiActionField308(action, 8);
            SetYobiActionField30F(action, 255);
            if (render::HasLegacy3DDevice_44B610() == 0) {
                action->flags |= 0x8000u;
            }
            const std::int32_t effectIndex = payload->effectRunningC9C;
            payload->valueFD4 = 1;
            YobiAction* const effect = NewBattleTransitionEffect_419C10(action,
                kRebirthEnterImages[payload->effects1C[effectIndex].position][side]);
            payload->rebirthEffect106C = effect;
            if (effect != nullptr) {
                SetYobiActionField309(effect, 4);
                SetYobiActionField310(payload->rebirthEffect106C, 4);
            }
            action->characterAction = 6;
            static_cast<void>(PlayFrame_517DA0(action, 1));
            break;
        }
        case 1: {
            const std::int32_t imageNo = action->imageNo;
            if (imageNo == payload->effects1C[initialEffect].position4) {
                YobiAction* const effect = payload->rebirthEffect106C;
                if (effect == nullptr || effect->phase != 0) {
                    payload->valueFD4 = 2;
                }
                static_cast<void>(PlayFrame_517DA0(action, 0));
            } else {
                const std::uint8_t alpha = GetYobiActionField30F(action);
                if (alpha <= 7) {
                    SetYobiActionField30F(action, 0);
                    if (payload->appearanceOverrideActive13B0 == 0) {
                        payload->baseImage13AC = imageNo;
                    }
                    const std::int32_t currentImage = payload->effects1C[payload->effectRunningC9C].position4;
                    action->characterAction = 0;
                    action->imageNo = currentImage;
                    SetYobiActionField309(action, 4);
                    SetYobiActionField310(action, 4);
                    SetYobiActionField308(action, 0);
                    SetYobiActionField30F(action, 255);
                    reinterpret_cast<unsigned char*>(&action->drawMode.color)[3] = 255;
                    payload->flagsFDC |= BattleActorFlag_Rebirth;
                    static_cast<void>(PlayFrame_517DA0(action, 0));
                } else {
                    SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha - 7));
                    action->characterAction = 6;
                    static_cast<void>(PlayFrame_517DA0(action, 1));
                }
            }
            break;
        }
        case 2: {
            YobiAction* const previousEffect = payload->rebirthEffect106C;
            if (previousEffect != nullptr) {
                static_cast<void>(MarkDeath(previousEffect));
            }
            const std::int32_t effectIndex = payload->effectRunningC9C;
            payload->rebirthEffect106C = nullptr;
            const std::uint8_t position = *reinterpret_cast<const std::uint8_t*>(
                &payload->effects1C[effectIndex].position);
            const std::uint8_t type = static_cast<std::uint8_t>(position + 1);
            payload->rebirthEffectType1080 = std::bit_cast<std::int8_t>(type);
            std::int32_t imageNo = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][0];
            if (imageNo != 0) {
                YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(action, 0, 2, imageNo);
                payload->rebirthEffect106C = effect;
                if (effect != nullptr) {
                    SetYobiActionField309(effect, 4);
                    SetYobiActionField310(payload->rebirthEffect106C, 4);
                }
            }
            imageNo = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][1];
            if (imageNo != 0) {
                YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(action, 0, 1, imageNo);
                payload->rebirthAuxiliaryEffects1074[0] = effect;
                if (effect != nullptr) {
                    SetYobiActionField309(effect, 4);
                    SetYobiActionField310(payload->rebirthAuxiliaryEffects1074[0], 4);
                }
            }
            imageNo = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][2];
            if (imageNo != 0) {
                YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(action, 0, -1, imageNo);
                payload->rebirthAuxiliaryEffects1074[1] = effect;
                if (effect != nullptr) {
                    SetYobiActionField309(effect, 4);
                    SetYobiActionField310(payload->rebirthAuxiliaryEffects1074[1], 4);
                }
            }
            imageNo = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][3];
            if (imageNo != 0) {
                YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(action, 0, -2, imageNo);
                YobiAction* const third = payload->rebirthAuxiliaryEffects1074[1];
                payload->rebirthAuxiliaryEffects1074[2] = effect;
                // 原411562按第三槽判定，不改为检查本次创建结果。
                if (third != nullptr) {
                    SetYobiActionField309(effect, 4);
                    SetYobiActionField310(payload->rebirthAuxiliaryEffects1074[2], 4);
                }
            }
            const std::int32_t currentImage = payload->effects1C[payload->effectRunningC9C].position4;
            SetYobiActionField309(action, 4);
            action->imageNo = currentImage;
            const std::uint32_t flags = action->flags & ~0x8000u;
            SetYobiActionField310(action, 4);
            SetYobiActionField308(action, 0);
            SetYobiActionField30F(action, 255);
            action->flags = flags;
            payload->flagsFDC |= BattleActorFlag_Rebirth;
            action->characterAction = 0;
            static_cast<void>(PlayFrame_517DA0(action, 0));
            static_cast<void>(AdvanceBattleEffect_439470(action));
            AdvanceBattleCastState_439340(action);
            action->frameEvent = 0;
            payload->valueFD4 = 0;
            break;
        }
        default:
            break;
        }
        for (YobiAction*& slot : payload->auxiliaryGear13B8) {
            YobiAction* const effect = slot;
            if (effect != nullptr) {
                static_cast<void>(MarkDeath(effect));
                slot = nullptr;
            }
        }
        return;
    }

    action->characterAction = 6;
    static_cast<void>(PlayFrame_517DA0(action, 1));
    if (payload->valueFD4 == 1) {
        const std::uint8_t alpha = GetYobiActionField30F(action);
        if (alpha + 7 > 255) {
            YobiAction* const previousEffect = payload->rebirthEffect106C;
            if (previousEffect != nullptr) {
                static_cast<void>(MarkDeath(previousEffect));
            }
            payload->rebirthEffect106C = nullptr;
            const std::uint32_t flags = action->flags & ~0x10000u;
            SetYobiActionField308(action, 0);
            SetYobiActionField30F(action, 255);
            action->flags = flags;
            payload->rebirthEffectType1080 = 0;
            static_cast<void>(AdvanceBattleEffect_439470(action));
            AdvanceBattleCastState_439340(action);
            action->frameEvent = 0;
            payload->valueFD4 = 0;
        } else {
            SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha + 7));
        }
    } else if (payload->valueFD4 == 0) {
        const std::int8_t initialType = payload->rebirthEffectType1080;
        if (initialType > 0) {
            static_cast<void>(audio::PlaySe_52C450(kRebirthExitSounds411901[initialType - 1], action->x));
        }
        YobiAction* const previousEffect = payload->rebirthEffect106C;
        if (previousEffect != nullptr) {
            static_cast<void>(MarkDeath(previousEffect));
        }
        payload->rebirthEffect106C = nullptr;
        for (YobiAction*& slot : payload->rebirthAuxiliaryEffects1074) {
            YobiAction* const effect = slot;
            if (effect != nullptr) {
                static_cast<void>(MarkDeath(effect));
            }
            slot = nullptr;
        }
        const std::int32_t direction = action->direction;
        const bool side = direction < 6 && static_cast<std::uint32_t>(direction) > 2u;
        const std::int32_t imageNo = payload->effects1C[payload->effectRunningC9C].position4;
        SetYobiActionField308(action, 8);
        action->imageNo = imageNo;
        SetYobiActionField30F(action, 0);
        SetYobiActionField309(action, 0);
        payload->flagsFDC &= ~BattleActorFlag_Rebirth;
        static_cast<void>(PlayFrame_517DA0(action, 1));
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags |= 0x10000u;
        }
        const std::int8_t currentType = payload->rebirthEffectType1080;
        if (currentType > 0) {
            payload->rebirthEffect106C = NewBattleTransitionEffect_419C10(
                action, kRebirthExitImages4118BF[currentType - 1][side]);
        }
        YobiAction* const effect = payload->rebirthEffect106C;
        if (effect != nullptr) {
            SetYobiActionField309(effect, 4);
            SetYobiActionField310(payload->rebirthEffect106C, 4);
        }
        payload->valueFD4 = 1;
    }
}

// 原函数：0x00411A50。
void UpdateBattleAction164_411A50(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 0;
    static_cast<void>(PlayFrame_517DA0(action, 1));
    const std::int32_t stage = payload->valueFD4;
    if (stage != 0) {
        if (stage > 0 && stage <= 2) {
            const std::uint8_t alpha = GetYobiActionField30F(action);
            if (alpha + 7 <= 255) {
                SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha + 7));
                return;
            }
            if (stage == 1) {
                static_cast<void>(MarkDeath(payload->rebirthEffect106C));
            }
            payload->rebirthEffect106C = nullptr;
            const std::uint32_t flags = action->flags & ~0x10000u;
            SetYobiActionField308(action, 0);
            SetYobiActionField30F(action, 255);
            action->flags = flags;
            const std::int32_t savedPhase = payload->valueFF0;
            payload->rebirthEffectType1080 = 0;
            action->phase = savedPhase;
            payload->valueFF0 = 0;
            action->frameEvent = 0;
            payload->valueFD4 = 0;
        }
        return;
    }
    const std::int8_t initialType = payload->rebirthEffectType1080;
    if (initialType > 0) {
        static_cast<void>(audio::PlaySe_52C450(kRebirthExitSounds411901[initialType - 1], action->x));
    }
    YobiAction* const previousEffect = payload->rebirthEffect106C;
    if (previousEffect != nullptr) {
        static_cast<void>(MarkDeath(previousEffect));
    }
    payload->rebirthEffect106C = nullptr;
    for (YobiAction*& slot : payload->rebirthAuxiliaryEffects1074) {
        YobiAction* const effect = slot;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
        }
        slot = nullptr;
    }
    const std::int32_t direction = action->direction;
    const bool side = direction < 6 && static_cast<std::uint32_t>(direction) > 2u;
    const std::int32_t imageNo = payload->baseImage13AC;
    SetYobiActionField308(action, 8);
    action->imageNo = imageNo;
    SetYobiActionField30F(action, 0);
    SetYobiActionField309(action, 0);
    payload->flagsFDC &= ~BattleActorFlag_Rebirth;
    payload->appearanceOverrideActive13B0 = 0;
    static_cast<void>(PlayFrame_517DA0(action, 1));
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->flags |= 0x10000u;
    }
    const std::int8_t currentType = payload->rebirthEffectType1080;
    if (currentType > 0) {
        payload->rebirthEffect106C = NewBattleTransitionEffect_419C10(
            action, kRebirthExitImages4118BF[currentType - 1][side]);
    }
    YobiAction* const effect = payload->rebirthEffect106C;
    if (effect != nullptr) {
        SetYobiActionField309(effect, 4);
        SetYobiActionField310(payload->rebirthEffect106C, 4);
    }
    payload->valueFD4 = 1;
}

// 原函数：0x00411D30。
std::int32_t UpdateBattleAction165_411D30(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((payload->effectFlagsFE4 & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::uint32_t initialFlags = payload->flagsFDC;
    const std::int32_t initialBit = static_cast<std::int32_t>((initialFlags >> 1) & 1u);
    const std::int32_t count = payload->actionFrameCountFCC + 1;
    payload->actionFrameCountFCC = count;
    if (count < 14 * (initialBit + 1)) {
        const double angle = action->angle50 + 180.0;
        action->motionMagnitude54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        const std::uint8_t movingFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
        if ((movingFlags & BattleActorFlag_Dead) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count < 14 * initialBit + 34) {
        const std::int32_t direction = action->direction + 1;
        action->direction = direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
        const std::int32_t threshold = 14 * (static_cast<std::int32_t>((payload->flagsFDC & BattleActorFlag_Dead) != 0) + 1);
        if (payload->actionFrameCountFCC == threshold) {
            action->motionMagnitude54 = 5.0F;
            const std::uint8_t thresholdFlags =
                *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
            if ((thresholdFlags & BattleActorFlag_Dead) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                payload->aliveFF8 = 0;
            }
            payload->valueFF0 = 0;
        }
        if (payload->valueFF0 == 0) {
            static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
        }
        const std::int32_t previous = payload->valueFF0;
        payload->valueFF0 = previous + 1;
        if (previous >= 2) {
            payload->valueFF0 = 0;
        }
        const double speed = static_cast<double>(action->motionMagnitude54) - 0.3F;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (!(speed >= 0.0)) {
            action->motionMagnitude54 = 0.0F;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 14 * initialBit + 54) {
        payload->actionFrameCountFCC = 0;
        if ((initialFlags & BattleActorFlag_Dead) == 0) {
            action->motionMagnitude54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
            if (action->phase == BattleAction_ReturnToFormation) {
                action->motionMagnitude54 = 4.0F;
                action->framePlayingRate = 100;
            }
        } else if ((payload->effectFlagsFE4 & 0x3010) != 0) {
            action->motionMagnitude54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
        } else {
            action->motionMagnitude54 = 1.0F;
            action->framePlayingRate = 250;
            AdvanceBattleCastState_439340(action);
        }
    }
    return PlayFrame_517DA0(action, 1);
}

// 原函数：0x00411CB0。
void UpdateBattleAction166_411CB0(YobiAction* const action)
{
    const std::int32_t playerPosition = world::gBattleCurrentPosition5ABCA4;
    auto* const payload = action->payload.battleActor;
    if (payload->battlePosition18 == playerPosition) {
        world::ConsumeBattlePlayerTimer(payload->queuedTargets1084[payload->queuedTargetIndex13A4]);
    }
    const std::int32_t index = payload->queuedTargetIndex13A4;
    const std::int32_t y = action->y;
    const std::int32_t value = payload->queuedTargets1084[index];
    const std::int32_t x = action->x;
    static_cast<void>(NewBattleTimeNumber_4206C0(x, y, -value));
    static_cast<void>(AdvanceBattleQueuedTarget_43B830(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00411FA0。
void UpdateBattleAction167_411FA0(YobiAction* const action)
{
    SetYobiActionField308(action, 8);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00411FC0。
void UpdateBattleAction168_411FC0(YobiAction* const action)
{
    SetYobiActionField308(action, 0);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00411FE0。
void UpdateBattleAction169_411FE0(YobiAction* const action)
{
    SetYobiActionField309(action, 255);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412000。
void UpdateBattleAction170_412000(YobiAction* const action)
{
    SetYobiActionField309(action, 0);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412020。
void UpdateBattleAction171_412020(YobiAction* const action)
{
    action->flags &= ~2u;
    SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0, 2);
    const std::uint8_t alpha = GetYobiActionField30F(action);
    if (alpha + 3 < 255) {
        SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha + 3));
        SyncBattleAttachmentAlpha_420F10(action, -1, 3, 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags |= 0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0x8000, 0);
        }
    } else {
        SetYobiActionField30F(action, 255);
        SyncBattleAttachmentAlpha_420F10(action, 255, 0, 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags &= ~0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0, 0x8000);
        }
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004120F0。
void UpdateBattleActions172_173_4120F0(YobiAction* const action)
{
    const std::int32_t step = action->phase == BattleAction_FadeOutStep2 ? BattleAction_EnterFormation : BattleAction_ApproachEffectTarget;
    const std::uint8_t alpha = GetYobiActionField30F(action);
    if (alpha - step > 0) {
        SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha - step));
        SyncBattleAttachmentAlpha_420F10(action, -1, static_cast<std::int8_t>(-step), 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags |= 0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0x8000, 0);
        }
    } else {
        SetYobiActionField30F(action, 0);
        SyncBattleAttachmentAlpha_420F10(action, 0, 0, 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags &= ~0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0, 0x8000);
        }
        action->flags |= 2u;
        SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0x8000, 0);
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004121D0。
void UpdateBattleAction174_4121D0(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t index = payload->effectRunningC9C;
    const std::int32_t steps = payload->effects1C[index].position3;
    const std::int32_t palette = payload->effects1C[index].position;
    static_cast<void>(render::SelectPalette(palette, steps));
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412210。
void UpdateBattleAction175_412210(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->effect1060 == nullptr) {
        payload->effect1060 = NewBattleBackgroundFade_421570(
            payload->effects1C[payload->effectRunningC9C].position4, 2, 0);
    }
    payload->effect1060->phase = 0;
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412260。
void UpdateBattleAction176_412260(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const effect = payload->effect1060;
    if (effect != nullptr) {
        effect->phase = 2;
        payload->effect1060 = nullptr;
    }
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412290。
void UpdateBattleAction177_412290(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    static_cast<void>(NewBattleBackgroundFade_421570(
        payload->effects1C[payload->effectRunningC9C].position4, 2, 1));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x004122C0。
void UpdateBattleAction178_4122C0(YobiAction* const action)
{
    AdvanceBattleCastState_439340(action);
    action->phase = BattleAction_Idle;
}

// 原函数：0x004122E0。
void UpdateBattleAction179_4122E0(YobiAction* const action)
{
    action->characterAction = 6;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00412310。
void UpdateBattleActions180_203_412310(YobiAction* const action)
{
    // BAN01：本组转204渐显并按结果结算，不能与普通转162等待的同type动作合并。
    // 188无创建分支；未证业务名的工厂项保留type号，不依据图号猜技能名。
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    if ((payload->flagsFDC & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->effectPlaybackState1024 == BattleEffectPlayback_Idle) {
        std::int32_t type = -1;
        switch (action->phase) {
        case BattleAction_PlayRecoveryEffectAndFadeIn: type = 13; break;
        case BattleAction_PlayEffectType1AndFadeIn: type = 1; break;
        case BattleAction_PlayMagicAttackEffectAndFadeIn: type = 2; break;
        case BattleAction_PlayStatusRecoveryEffectAndFadeIn: type = 25; break;
        case BattleAction_PlayHealEffectAndFadeIn: type = 16; break;
        case BattleAction_PlayLevelScaledEffect21AndFadeIn: type = 21; break;
        case BattleAction_PlayParentBoundEffect22AndFadeIn: type = 22; break;
        case BattleAction_PlayParentBoundEffect23AndFadeIn: type = 23; break;
        case BattleAction_PlayDebuffSpellEffectAndFadeIn: type = 4; break;
        case BattleAction_PlayBarrierSpellEffectAndFadeIn: type = 5; break;
        case BattleAction_PlayPhysicalReflectEffectAndFadeIn: type = 7; break;
        case BattleAction_PlayPhysicalAbsorbEffectAndFadeIn: type = 8; break;
        case BattleAction_PlayPhysicalNullifyEffectAndFadeIn: type = 9; break;
        case BattleAction_PlayMagicReflectEffectAndFadeIn: type = 10; break;
        case BattleAction_PlayMagicAbsorbEffectAndFadeIn: type = 11; break;
        case BattleAction_PlayMagicNullifyEffectAndFadeIn: type = 12; break;
        case BattleAction_PlayEffectType26AndFadeIn: type = 26; break;
        case BattleAction_PlayHealingCastEffectAndFadeIn: type = 29; break;
        case BattleAction_PlayEffectType14AndFadeIn: type = 14; break;
        case BattleAction_PlayEffectType15AndFadeIn: type = 15; break;
        case BattleAction_PlayAttributeReversalEffectAndFadeIn: type = 6; break;
        case BattleAction_PlayParameterChangeEffectAndFadeIn: type = 37; break;
        case BattleAction_PlayBombLevelEffectAndFadeIn: type = 39; break;
        default: break;
        }
        if (type >= 0) {
            static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, type, 0, 1));
        }
    }
    action->phase = BattleAction_FadeInAndResolveEffect;
}

// 原函数：0x004124E0。
void UpdateBattleAction204_4124E0(YobiAction* const action)
{
    // BAN01：透明度每次加3；播放状态2时按免疫位或数字类型结算一次，转3累计60次。
    // 状态4且完全不透明才清状态／计数并推进；不将渐显完成单独视为动作完成。
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & BattleActorSleepOrStoneMask) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    action->flags &= ~2u;
    SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0, 2);
    const std::uint8_t alpha = GetYobiActionField30F(action);
    if (alpha + 3 < 255) {
        SetYobiActionField30F(action, static_cast<std::uint8_t>(alpha + 3));
        SyncBattleAttachmentAlpha_420F10(action, -1, 3, 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags |= 0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0x8000, 0);
        }
    } else {
        SetYobiActionField30F(action, 255);
        SyncBattleAttachmentAlpha_420F10(action, 255, 0, 0, 0);
        if (render::HasLegacy3DDevice_44B610() == 0) {
            action->flags &= ~0x8000u;
            SyncBattleAttachmentAlpha_420F10(action, -1, 0, 0, 0x8000);
        }
    }
    if (payload->effectPlaybackState1024 == BattleEffectPlayback_Completed) {
        action->hidden = 0;
        action->field2D8 = 0;
        const std::int32_t index = payload->effectRunningC9C;
        if ((payload->effects1C[index].flags & 0x10000000u) != 0) {
            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
        } else {
            const std::int32_t displayType = payload->numberDisplayType1034;
            if (displayType >= 0) {
                const std::int32_t value = payload->value1030;
                const std::int32_t x = action->x;
                const std::int32_t y = action->y - 80;
                static_cast<void>(NewBattleNumber_41F1A0(x, y, value, displayType, 2, 0));
            }
            switch (payload->numberDisplayType1034) {
            case BattleNumber_DecreaseHp:
                (action->readiness -= static_cast<std::int32_t>(payload->value1030));
                if ((action->readiness < 0)) {
                    action->readiness = std::int32_t{0};
                }
                break;
            case BattleNumber_IncreaseHp:
                (action->readiness += static_cast<std::int32_t>(payload->value1030));
                if ((action->maxHp < action->readiness)) {
                    action->readiness = action->maxHp;
                }
                break;
            case BattleNumber_IncreaseFp:
                (action->barValue1B0 += static_cast<std::int32_t>(payload->value1030));
                if ((action->barMaximum1C0 < action->barValue1B0)) {
                    action->barValue1B0 = action->barMaximum1C0;
                }
                break;
            case BattleNumber_DecreaseFp:
                (action->barValue1B0 -= static_cast<std::int32_t>(payload->value1030));
                if ((0 > action->barValue1B0)) {
                    action->barValue1B0 = std::int32_t{0};
                }
                break;
            default:
                break;
            }
        }
        ++payload->effectPlaybackState1024;
    }
    if (payload->effectPlaybackState1024 == BattleEffectPlayback_HoldAfterResult) {
        const std::int32_t count = payload->valueFF0 + 1;
        payload->valueFF0 = count;
        if (count >= 60) {
            payload->effectPlaybackState1024 = BattleEffectPlayback_WaitFullOpacity;
        }
    }
    if (payload->effectPlaybackState1024 == BattleEffectPlayback_WaitFullOpacity && GetYobiActionField30F(action) == 255) {
        payload->effectPlaybackState1024 = BattleEffectPlayback_Idle;
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004127B0。
void UpdateBattleAction205_4127B0(YobiAction* const action)
{
    action->characterAction = 16;
    if (PlayFrame_517DA0(action, 1) == 1) {
        action->frameIndex = 0;
        action->frameRemain = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004127F0。
void UpdateBattleAction206_4127F0(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    YobiAction* const target = world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position];
    TurnBattleToward_406190(action, target, 1, 0.0F);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00412830。
void UpdateBattleAction207_412830(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->effectRunningC9C;
    payload->moveTargetXFE8 = payload->effects1C[index].position;
    payload->moveTargetYFEC = std::bit_cast<std::int32_t>(payload->effects1C[index].flags);
    static_cast<void>(AdvanceBattleEffect_439470(action));
    action->phase = BattleAction_MoveToTargetPosition;
}

} // namespace detail

} // namespace cg::yobi
