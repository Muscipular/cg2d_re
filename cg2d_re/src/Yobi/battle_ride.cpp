#include "battle_ride.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "cg/audio/LegacyAudio.hpp"

#include "animation.hpp"
#include "battle_actor.hpp"
#include "battle_attachment.hpp"
#include "battle_effect.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/PlayerState.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

// 三种拆分动作的固定载荷清理段；不含骑乘/1060，也不重取对象表。
static void ClearBattleRideAttachments(BattleActorPayload* const payload)
{
    YobiAction* const reverseEffect = payload->attributeReversalEffect105C;
    if (reverseEffect != nullptr) {
        static_cast<void>(MarkDeath(reverseEffect));
        payload->attributeReversalEffect105C = nullptr;
    }
    YobiAction* const debuffEffect = payload->buffEffectAction_1054;
    if (debuffEffect != nullptr) {
        static_cast<void>(MarkDeath(debuffEffect));
        payload->buffEffectAction_1054 = nullptr;
    }
    YobiAction* const statusEffect = payload->statusEffect1068;
    if (statusEffect != nullptr) {
        static_cast<void>(MarkDeath(statusEffect));
        payload->statusEffect1068 = nullptr;
    }
    YobiAction* const attachedEffect = payload->attachedEffect1058;
    if (attachedEffect != nullptr) {
        static_cast<void>(MarkDeath(attachedEffect));
        payload->attachedEffect1058 = nullptr;
    }
    YobiAction* const gear = payload->gear13B4;
    if (gear != nullptr) {
        static_cast<void>(MarkDeath(gear));
        payload->gear13B4 = nullptr;
    }
    YobiAction* const firstAuxiliaryGear = payload->auxiliaryGear13B8[0];
    if (firstAuxiliaryGear != nullptr) {
        static_cast<void>(MarkDeath(firstAuxiliaryGear));
        payload->auxiliaryGear13B8[0] = nullptr;
    }
    YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
    if (secondAuxiliaryGear != nullptr) {
        static_cast<void>(MarkDeath(secondAuxiliaryGear));
        payload->auxiliaryGear13B8[1] = nullptr;
    }
}

// 原恢复首位置的装备替换段；每次调用后重取表内对象及载荷。
static void RestoreBattleRideGear(YobiAction** const slot, const BattleRideCarrierPayload* const carrier)
{
    auto* payload = (*slot)->payload.battleActor;
    YobiAction* const gear = payload->gear13B4;
    if (gear != nullptr) {
        static_cast<void>(MarkDeath(gear));
        (*slot)->payload.battleActor->gear13B4 = nullptr;
    }
    const std::int32_t image = carrier->gearImage0C;
    if (image > 0) {
        YobiAction* const newGear = NewBattleGear_422A30(*slot, image);
        (*slot)->payload.battleActor->gear13B4 = newGear;
    }
    payload = (*slot)->payload.battleActor;
    YobiAction* const firstAuxiliaryGear = payload->auxiliaryGear13B8[0];
    if (firstAuxiliaryGear != nullptr) {
        static_cast<void>(MarkDeath(firstAuxiliaryGear));
        (*slot)->payload.battleActor->auxiliaryGear13B8[0] = nullptr;
    }
    payload = (*slot)->payload.battleActor;
    YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
    if (secondAuxiliaryGear != nullptr) {
        static_cast<void>(MarkDeath(secondAuxiliaryGear));
        (*slot)->payload.battleActor->auxiliaryGear13B8[1] = nullptr;
    }
}

// 原函数：0x00414050。
void UpdateBattleAction224_414050(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    using world::gBattleGraphicPoints553758;
    using world::gBattlePointIndices5536FC;
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->effectRunningC9C;
    if (payload->effects1C[index].flags != 0) {
        YobiAction* const effect = payload->effect1060;
        const std::int32_t position = payload->effects1C[index].position;
        const std::int32_t first = payload->effects1C[index].position3;
        const std::int32_t second = payload->effects1C[index].position4;
        auto* const carrier = effect->payload.battleRideCarrier;
        if (payload->rider13C8 == nullptr) {
            const std::int32_t image = world::ResolveRidingCharacterImage_51C0D0(effect->imageNo);
            YobiAction* const rider = NewBattleRider_4221F0(action, image);
            payload->rider13C8 = rider;
            rider->phase = BattleRiderSyncMode_DrawAttributesOnly;
            payload->rider13C8->field58 = 0;
            payload->rider13C8->characterAction = CharacterAnimation_Sit;
            YobiAction* const nameSource = cg::world::gBattleAllEntries59E980[first];
            YobiAction* const nameRider = payload->rider13C8;
            nameRider->nameC4 = nameSource->nameC4;
            YobiAction* const valueSource = cg::world::gBattleAllEntries59E980[first];
            YobiAction* const valueRider = payload->rider13C8;
            valueRider->field1D0 = valueSource->field1D0;
            YobiAction* cryptSource = cg::world::gBattleAllEntries59E980[first];
            YobiAction* cryptRider = payload->rider13C8;
            cryptRider->readiness = cryptSource->readiness;
            cryptSource = cg::world::gBattleAllEntries59E980[first];
            cryptRider = payload->rider13C8;
            cryptRider->maxHp = cryptSource->maxHp;
            cryptSource = cg::world::gBattleAllEntries59E980[first];
            cryptRider = payload->rider13C8;
            cryptRider->barValue1B0 = cryptSource->barValue1B0;
            cryptSource = cg::world::gBattleAllEntries59E980[first];
            cryptRider = payload->rider13C8;
            cryptRider->barMaximum1C0 = cryptSource->barMaximum1C0;
            YobiAction* coordinateSource = cg::world::gBattleAllEntries59E980[first];
            YobiAction* coordinateRider = payload->rider13C8;
            coordinateRider->x = coordinateSource->x;
            coordinateSource = cg::world::gBattleAllEntries59E980[first];
            coordinateRider = payload->rider13C8;
            coordinateRider->y = coordinateSource->y;
            coordinateSource = cg::world::gBattleAllEntries59E980[first];
            coordinateRider = payload->rider13C8;
            coordinateRider->origin_x_offset40 = coordinateSource->origin_x_offset40;
            coordinateSource = cg::world::gBattleAllEntries59E980[first];
            coordinateRider = payload->rider13C8;
            coordinateRider->origin_y_offset44 = coordinateSource->origin_y_offset44;
            payload->rider13C8->flags |= YobiActionMask_PendingValue1OrHoverDescription;
            action->nameC4 = cg::world::gBattleAllEntries59E980[second]->nameC4;
            const std::int32_t nextImage = cg::world::gBattleAllEntries59E980[second]->imageNo;
            action->characterAction = CharacterAnimation_Stand;
            action->imageNo = nextImage;
            action->field1D0 = cg::world::gBattleAllEntries59E980[second]->field1D0;
            action->readiness = cg::world::gBattleAllEntries59E980[second]->readiness;
            action->maxHp = cg::world::gBattleAllEntries59E980[second]->maxHp;
            action->barValue1B0 = cg::world::gBattleAllEntries59E980[second]->barValue1B0;
            action->barMaximum1C0 = cg::world::gBattleAllEntries59E980[second]->barMaximum1C0;
            action->x = cg::world::gBattleAllEntries59E980[second]->x;
            action->y = cg::world::gBattleAllEntries59E980[second]->y;
            action->origin_x_offset40 = cg::world::gBattleAllEntries59E980[second]->origin_x_offset40;
            action->origin_y_offset44 = cg::world::gBattleAllEntries59E980[second]->origin_y_offset44;
            ClearBattleRideAttachments(cg::world::gBattleAllEntries59E980[first]->payload.battleActor);
            ClearBattleRideAttachments(cg::world::gBattleAllEntries59E980[second]->payload.battleActor);
            FaceBattleActorTowardFormationPosition_406260(action);
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            const std::int32_t frameX = action->frameX;
            const float targetY = effect->origin_y_offset44;
            const float targetX = effect->origin_x_offset40;
            effect->deltaX = frameX;
            if (GetBattleDistanceToPosition_4062A0(action, targetX, targetY) < 5) {
                action->origin_x_offset40 = effect->origin_x_offset40;
                action->origin_y_offset44 = effect->origin_y_offset44;
                FaceBattleActorTowardFormationPosition_406260(action);
                action->characterAction = CharacterAnimation_Stand;
            } else {
                const float turnY = effect->origin_y_offset44;
                const float turnX = effect->origin_x_offset40;
                TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
            }
            const std::int32_t riderDeltaX = effect->deltaX;
            const float riderY = effect->origin_y_offset44;
            YobiAction* const turningRider = payload->rider13C8;
            const float riderX = static_cast<float>(static_cast<double>(riderDeltaX) + effect->origin_x_offset40);
            TurnBattleTowardPosition_4060D0(turningRider, riderX, riderY, 1, 0.0F);
            action->motionMagnitude54 = 8.0F;
            payload->rider13C8->motionMagnitude54 = 8.0F;
            static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
            static_cast<void>(PlayFrame_517DA0(payload->rider13C8, AnimationPlayback_Loop));
            static_cast<void>(MarkDeath(cg::world::gBattleAllEntries59E980[second]));
            cg::world::gBattleAllEntries59E980[second] = nullptr;
            static_cast<void>(audio::PlaySe_52C450(465, payload->rider13C8->x));
            return;
        }
        action->characterAction = CharacterAnimation_Run;
        const float distanceY = effect->origin_y_offset44;
        const float distanceX = effect->origin_x_offset40;
        if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
            action->origin_x_offset40 = effect->origin_x_offset40;
            action->origin_y_offset44 = effect->origin_y_offset44;
            FaceBattleActorTowardFormationPosition_406260(action);
            action->characterAction = CharacterAnimation_Stand;
        } else {
            MoveBattleByAngle_405EA0(action);
        }
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        payload->rider13C8->characterAction = CharacterAnimation_Sit;
        const float riderY = effect->origin_y_offset44;
        const std::int32_t riderDeltaX = effect->deltaX;
        YobiAction* const distanceRider = payload->rider13C8;
        const float riderX = static_cast<float>(static_cast<double>(riderDeltaX) + effect->origin_x_offset40);
        if (GetBattleDistanceToPosition_4062A0(distanceRider, riderX, riderY) < 5) {
            const std::int32_t snapDeltaX = effect->deltaX;
            YobiAction* const snapXRider = payload->rider13C8;
            snapXRider->origin_x_offset40 = static_cast<float>(static_cast<double>(snapDeltaX) + effect->origin_x_offset40);
            YobiAction* const snapYRider = payload->rider13C8;
            snapYRider->origin_y_offset44 = effect->origin_y_offset44;
        } else {
            MoveBattleByAngle_405EA0(payload->rider13C8);
        }
        YobiAction* const xRider = payload->rider13C8;
        xRider->deltaX = static_cast<std::int32_t>(xRider->origin_x_offset40) + static_cast<std::int32_t>(gYobiScreenOffsetX);
        YobiAction* const yRider = payload->rider13C8;
        yRider->deltaY = static_cast<std::int32_t>(yRider->origin_y_offset44) + static_cast<std::int32_t>(gYobiScreenOffsetY);
        YobiAction* const positionXRider = payload->rider13C8;
        positionXRider->x = positionXRider->deltaX;
        YobiAction* const positionYRider = payload->rider13C8;
        positionYRider->y = positionYRider->deltaY;
        YobiAction* const movingRider = payload->rider13C8;
        const std::int32_t time = movingRider->field58;
        const std::int32_t baseY = movingRider->y;
        const std::int32_t t = time - 15;
        const std::int32_t offset = t * t - 225;
        movingRider->y = baseY + offset;
        YobiAction* const currentRider = payload->rider13C8;
        const std::int32_t oldTime = currentRider->field58;
        const bool pastMinimum = oldTime > 15;
        currentRider->field58 = oldTime + 1;
        if (pastMinimum && action->frameY <= offset) {
            payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
            payload->rider13C8->field58 = 0;
            payload->rider13C8->flags &= ~YobiActionMask_PendingValue1OrHoverDescription;
            action->nameC4 = effect->nameC4;
            action->characterAction = CharacterAnimation_Stand;
            action->field1D0 = effect->field1D0;
            action->readiness = effect->readiness;
            action->maxHp = effect->maxHp;
            action->barValue1B0 = effect->barValue1B0;
            action->barMaximum1C0 = effect->barMaximum1C0;
            const std::uint32_t restoredFlags = carrier->flags08;
            payload->stateFE0 = 0;
            payload->flagsFDC = restoredFlags;
            action->origin_x_offset40 = effect->origin_x_offset40;
            action->origin_y_offset44 = effect->origin_y_offset44;
            const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
            const float y = action->origin_y_offset44;
            action->x = x;
            action->y = static_cast<std::int32_t>(y);
            YobiAction* const oldGear = payload->gear13B4;
            if (oldGear != nullptr) {
                static_cast<void>(MarkDeath(oldGear));
                payload->gear13B4 = nullptr;
            }
            const std::int32_t gearImage = carrier->gearImage0C;
            if (gearImage > 0) {
                payload->gear13B4 = NewBattleGear_422A30(action, gearImage);
            }
            YobiAction* const oldAuxiliary0 = payload->auxiliaryGear13B8[0];
            if (oldAuxiliary0 != nullptr) {
                static_cast<void>(MarkDeath(oldAuxiliary0));
                payload->auxiliaryGear13B8[0] = nullptr;
            }
            YobiAction* const oldAuxiliary1 = payload->auxiliaryGear13B8[1];
            if (oldAuxiliary1 != nullptr) {
                static_cast<void>(MarkDeath(oldAuxiliary1));
                payload->auxiliaryGear13B8[1] = nullptr;
            }
            if (position != first) {
                YobiAction* const oldSecond = cg::world::gBattleAllEntries59E980[second];
                if (oldSecond != nullptr) {
                    static_cast<void>(MarkDeath(oldSecond));
                    cg::world::gBattleAllEntries59E980[second] = nullptr;
                }
                YobiAction** const slot = &cg::world::gBattleAllEntries59E980[position];
                YobiAction* const oldPosition = *slot;
                if (oldPosition != nullptr) {
                    static_cast<void>(MarkDeath(oldPosition));
                    *slot = nullptr;
                }
                *slot = cg::world::gBattleAllEntries59E980[first];
                const std::uint8_t priority = static_cast<std::uint8_t>(10 * position + 2);
                cg::world::gBattleAllEntries59E980[first] = nullptr;
                action->priority = priority;
                payload->battlePosition18 = position;
                action->origin_x_offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2]);
                action->origin_y_offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3]);
                action->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2];
                action->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3];
                payload->x00 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2];
                payload->y04 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3];
                payload->x08 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][0];
                payload->y0C = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][1];
                const std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
                if (currentPosition == first || currentPosition == second) {
                    world::gBattleCurrentPosition5ABCA4 = position;
                }
            }
            RestoreBattleStateEffects_422370(action);
            action->fields288[3] = 0;
            action->fields288[2] = 0;
            action->fields288[1] = 0;
            action->fields288[0] = 0;
            if ((payload->flagsFDC & BattleActorFlag_Earth) != 0) { action->fields288[0] = 1; }
            if ((payload->flagsFDC & BattleActorFlag_Water) != 0) { action->fields288[1] = 1; }
            if ((payload->flagsFDC & BattleActorFlag_Fire) != 0) { action->fields288[2] = 1; }
            if ((payload->flagsFDC & BattleActorFlag_Wind) != 0) { action->fields288[3] = 1; }
            FaceBattleActorTowardFormationPosition_406260(action);
            UpdateBattleAlbumFlag_4238F0(action);
            static_cast<void>(MarkDeath(payload->effect1060));
            YobiAction* const soundRider = payload->rider13C8;
            payload->effect1060 = nullptr;
            static_cast<void>(audio::PlaySe_52C450(462, soundRider->x));
            static_cast<void>(AdvanceBattleEffect_439470(action));
            AdvanceBattleCastState_439340(action);
            static_cast<void>(PlayFrame_517DA0(payload->rider13C8, AnimationPlayback_Loop));
            return;
        }
        static_cast<void>(PlayFrame_517DA0(payload->rider13C8, AnimationPlayback_Loop));
        return;
    }
    auto* const carrier = payload->effect1060->payload.battleRideCarrier;
    const std::int32_t first = payload->effects1C[index].position3;
    const std::int32_t second = payload->effects1C[index].position4;
    action->characterAction = CharacterAnimation_Run;
    action->motionMagnitude54 = 8.0F;
    const std::int32_t distancePoint = cg::world::gBattlePointIndices5536FC[second];
    const float distanceY = static_cast<float>(cg::world::gBattleGraphicPoints553758[distancePoint][3]);
    const float distanceX = static_cast<float>(cg::world::gBattleGraphicPoints553758[distancePoint][2]);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->origin_x_offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[second]][2]);
        action->origin_y_offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[second]][3]);
        FaceBattleActorTowardFormationPosition_406260(action);
        action->characterAction = CharacterAnimation_Stand;
    } else {
        const std::int32_t turnPoint = cg::world::gBattlePointIndices5536FC[second];
        const float turnY = static_cast<float>(cg::world::gBattleGraphicPoints553758[turnPoint][3]);
        const float turnX = static_cast<float>(cg::world::gBattleGraphicPoints553758[turnPoint][2]);
        TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
        MoveBattleByAngle_405EA0(action);
    }
    payload->rider13C8->phase = BattleRiderSyncMode_DrawAttributesOnly;
    payload->rider13C8->characterAction = CharacterAnimation_Sit;
    payload->rider13C8->flags |= YobiActionMask_PendingValue1OrHoverDescription;
    const std::int32_t riderDistancePoint = cg::world::gBattlePointIndices5536FC[first];
    const float riderDistanceY = static_cast<float>(cg::world::gBattleGraphicPoints553758[riderDistancePoint][3]);
    const std::int32_t riderDistanceX = cg::world::gBattleGraphicPoints553758[riderDistancePoint][2];
    YobiAction* const distanceRider = payload->rider13C8;
    if (GetBattleDistanceToPosition_4062A0(
        distanceRider, static_cast<float>(riderDistanceX), riderDistanceY) < 5) {
        const std::int32_t snapXPoint = cg::world::gBattlePointIndices5536FC[first];
        YobiAction* const snapXRider = payload->rider13C8;
        snapXRider->origin_x_offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[snapXPoint][2]);
        const std::int32_t snapYPoint = cg::world::gBattlePointIndices5536FC[first];
        YobiAction* const snapYRider = payload->rider13C8;
        snapYRider->origin_y_offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[snapYPoint][3]);
    } else {
        const std::int32_t turnPoint = cg::world::gBattlePointIndices5536FC[first];
        const float turnY = static_cast<float>(cg::world::gBattleGraphicPoints553758[turnPoint][3]);
        const std::int32_t turnX = cg::world::gBattleGraphicPoints553758[turnPoint][2];
        YobiAction* const turningRider = payload->rider13C8;
        TurnBattleTowardPosition_4060D0(turningRider, static_cast<float>(turnX), turnY, 1, 0.0F);
        MoveBattleByAngle_405EA0(payload->rider13C8);
    }
    if (payload->rider13C8->field58 == 0) {
        ClearBattleRideAttachments(payload);
        action->nameC4 = carrier->second04->nameC4;
        action->field1D0 = carrier->second04->field1D0;
        action->readiness = carrier->second04->readiness;
        action->maxHp = carrier->second04->maxHp;
        action->barValue1B0 = carrier->second04->barValue1B0;
        action->barMaximum1C0 = carrier->second04->barMaximum1C0;
        action->x = carrier->second04->x;
        action->y = carrier->second04->y;
        FaceBattleActorTowardFormationPosition_406260(action);
        YobiAction* const directionRider = payload->rider13C8;
        directionRider->direction = action->direction;
        YobiAction* rider = payload->rider13C8;
        std::int32_t warmTime = rider->field58;
        while (warmTime < 15) {
            const std::int32_t t = warmTime - 15;
            const std::int32_t square = t * t;
            const std::int32_t frameY = action->frameY;
            if (frameY >= square - 225) {
                break;
            }
            rider->field58 = warmTime + 1;
            rider = payload->rider13C8;
            warmTime = rider->field58;
        }
        static_cast<void>(audio::PlaySe_52C450(463, payload->rider13C8->x));
    }
    YobiAction* const xRider = payload->rider13C8;
    xRider->deltaX = static_cast<std::int32_t>(xRider->origin_x_offset40) + static_cast<std::int32_t>(gYobiScreenOffsetX);
    YobiAction* const yRider = payload->rider13C8;
    yRider->deltaY = static_cast<std::int32_t>(yRider->origin_y_offset44) + static_cast<std::int32_t>(gYobiScreenOffsetY);
    YobiAction* const positionXRider = payload->rider13C8;
    positionXRider->x = positionXRider->deltaX;
    YobiAction* const positionYRider = payload->rider13C8;
    positionYRider->y = positionYRider->deltaY;
    YobiAction* const movingRider = payload->rider13C8;
    const std::int32_t time = movingRider->field58;
    const std::int32_t baseY = movingRider->y;
    const std::int32_t t = time - 15;
    const std::int32_t offset = t * t - 225;
    movingRider->y = baseY + offset;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    YobiAction* const currentRider = payload->rider13C8;
    const std::int32_t oldTime = currentRider->field58;
    const bool finish = oldTime > 15 && offset >= 0;
    currentRider->field58 = oldTime + 1;
    if (finish) {
        YobiAction** const secondSlot = &cg::world::gBattleAllEntries59E980[second];
        YobiAction* const oldSecond = *secondSlot;
        if (oldSecond != nullptr) {
            static_cast<void>(MarkDeath(oldSecond));
            *secondSlot = nullptr;
        }
        YobiAction* const restoredSecond = carrier->second04;
        *secondSlot = restoredSecond;
        restoredSecond->callback = UpdateBattleActor_404DB0;
        (*secondSlot)->flags &= ~YobiActionFlag_Hidden;
        carrier->second04 = nullptr;
        RestoreBattleStateEffects_422370(*secondSlot);
        FaceBattleActorTowardFormationPosition_406260(*secondSlot);
        static_cast<void>(PlayFrame_517DA0(*secondSlot, AnimationPlayback_Loop));
        MoveBattleCastState_439380(*secondSlot);
        YobiAction** const firstSlot = &cg::world::gBattleAllEntries59E980[first];
        YobiAction* const oldFirst = *firstSlot;
        if (oldFirst != nullptr) {
            static_cast<void>(MarkDeath(oldFirst));
            *firstSlot = nullptr;
        }
        YobiAction* const restoredFirst = carrier->first00;
        *firstSlot = restoredFirst;
        restoredFirst->callback = UpdateBattleActor_404DB0;
        (*firstSlot)->flags &= ~YobiActionFlag_Hidden;
        carrier->first00 = nullptr;
        RestoreBattleRideGear(firstSlot, carrier);
        RestoreBattleStateEffects_422370(*firstSlot);
        FaceBattleActorTowardFormationPosition_406260(*firstSlot);
        static_cast<void>(PlayFrame_517DA0(*firstSlot, AnimationPlayback_Loop));
        MoveBattleCastState_439380(*firstSlot);
        payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        static_cast<void>(MarkDeath(payload->rider13C8));
        YobiAction* const oldEffect = payload->effect1060;
        payload->rider13C8 = nullptr;
        static_cast<void>(MarkDeath(oldEffect));
        payload->effect1060 = nullptr;
        static_cast<void>(audio::PlaySe_52C450(457, (*firstSlot)->x));
        static_cast<void>(AdvanceBattleEffect_439470(action));
        AdvanceBattleCastState_439340(action);
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        static_cast<void>(PlayFrame_517DA0(rider, AnimationPlayback_Loop));
    }
}

// 原函数：0x004154C0。
void UpdateBattleAction226_4154C0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->effectRunningC9C;
    YobiAction* const entryRider = payload->rider13C8;
    auto* const carrier = payload->effect1060->payload.battleRideCarrier;
    const std::int32_t first = payload->effects1C[index].position3;
    const std::int32_t second = payload->effects1C[index].position4;
    if (entryRider->field58 == 0) {
        YobiAction* const restoredSecond = carrier->second04;
        cg::world::gBattleAllEntries59E980[second] = restoredSecond;
        restoredSecond->callback = UpdateBattleActor_404DB0;
        cg::world::gBattleAllEntries59E980[second]->phase = BattleAction_WaitIdle;
        cg::world::gBattleAllEntries59E980[second]->flags &= ~YobiActionFlag_Hidden;
        YobiAction* const secondXTarget = cg::world::gBattleAllEntries59E980[second];
        secondXTarget->origin_x_offset40 = action->origin_x_offset40;
        YobiAction* const secondYTarget = cg::world::gBattleAllEntries59E980[second];
        secondYTarget->origin_y_offset44 = action->origin_y_offset44;
        carrier->second04 = nullptr;
        action->graphicIndex = -1;
        action->lastImageNo = 0;
        ClearBattleRideAttachments(payload);
        FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[second]);
        YobiAction* const directionSource = cg::world::gBattleAllEntries59E980[second];
        YobiAction* const directionRider = payload->rider13C8;
        directionRider->direction = directionSource->direction;
        static_cast<void>(audio::PlaySe_52C450(463, payload->rider13C8->x));
    }
    action->characterAction = CharacterAnimation_Run;
    payload->rider13C8->phase = BattleRiderSyncMode_DrawAttributesOnly;
    payload->rider13C8->characterAction = CharacterAnimation_Damage;
    payload->rider13C8->flags |= YobiActionMask_PendingValue1OrHoverDescription;
    const std::int32_t x = static_cast<std::int32_t>(cg::world::gBattleAllEntries59E980[second]->origin_x_offset40);
    const std::int32_t screenX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    YobiAction* const deltaXRider = payload->rider13C8;
    deltaXRider->deltaX = x + screenX;
    const std::int32_t y = static_cast<std::int32_t>(cg::world::gBattleAllEntries59E980[second]->origin_y_offset44);
    const std::int32_t screenY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    YobiAction* const deltaYRider = payload->rider13C8;
    deltaYRider->deltaY = y + screenY;
    YobiAction* const positionXRider = payload->rider13C8;
    positionXRider->x = positionXRider->deltaX;
    YobiAction* const positionYRider = payload->rider13C8;
    positionYRider->y = positionYRider->deltaY;
    static_cast<void>(PlayFrame_517DA0(cg::world::gBattleAllEntries59E980[second], AnimationPlayback_Loop));
    YobiAction* const movingRider = payload->rider13C8;
    const std::int32_t time = movingRider->field58;
    const std::int32_t offset = time * time / 5 + cg::world::gBattleAllEntries59E980[second]->frameY;
    movingRider->y += offset;
    YobiAction* const counterRider = payload->rider13C8;
    const std::int32_t nextTime = counterRider->field58 + 1;
    const bool restoreEntries = offset >= 0;
    counterRider->field58 = nextTime;
    if (restoreEntries) {
        YobiAction* const oldFirst = cg::world::gBattleAllEntries59E980[first];
        if (oldFirst != nullptr) {
            static_cast<void>(MarkDeath(oldFirst));
            cg::world::gBattleAllEntries59E980[first] = nullptr;
        }
        YobiAction* const restoredFirst = carrier->first00;
        cg::world::gBattleAllEntries59E980[first] = restoredFirst;
        restoredFirst->callback = UpdateBattleActor_404DB0;
        cg::world::gBattleAllEntries59E980[first]->flags &= ~YobiActionFlag_Hidden;
        YobiAction* const firstXTarget = cg::world::gBattleAllEntries59E980[first];
        firstXTarget->origin_x_offset40 = action->origin_x_offset40;
        YobiAction* const firstYTarget = cg::world::gBattleAllEntries59E980[first];
        firstYTarget->origin_y_offset44 = action->origin_y_offset44;
        RestoreBattleRideGear(&cg::world::gBattleAllEntries59E980[first], carrier);
        carrier->first00 = nullptr;
        FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[first]);
        static_cast<void>(PlayFrame_517DA0(cg::world::gBattleAllEntries59E980[first], AnimationPlayback_Loop));
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[first]);
        payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        static_cast<void>(MarkDeath(payload->rider13C8));
        YobiAction* const oldEffect = payload->effect1060;
        payload->rider13C8 = nullptr;
        static_cast<void>(MarkDeath(oldEffect));
        payload->effect1060 = nullptr;
        RestoreBattleStateEffects_422370(cg::world::gBattleAllEntries59E980[first]);
        RestoreBattleStateEffects_422370(cg::world::gBattleAllEntries59E980[second]);
        static_cast<void>(audio::PlaySe_52C450(457, cg::world::gBattleAllEntries59E980[first]->x));
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[second]);
        static_cast<void>(MarkDeath(action));
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        static_cast<void>(PlayFrame_517DA0(rider, AnimationPlayback_Loop));
    }
}

// 原函数：0x00415940。
void UpdateBattleAction227_415940(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->effectRunningC9C;
    const std::int32_t first = payload->effects1C[index].position3;
    const std::int32_t second = payload->effects1C[index].position4;
    const std::uint32_t flags = payload->effects1C[index].flags;
    YobiAction* const entryRider = payload->rider13C8;
    YobiAction* const carrierAction = payload->effect1060;
    const std::int32_t entryTime = entryRider->field58;
    auto* const carrier = carrierAction->payload.battleRideCarrier;
    if (entryTime == 0) {
        YobiAction* const restoredSecond = carrier->second04;
        cg::world::gBattleAllEntries59E980[second] = restoredSecond;
        restoredSecond->callback = UpdateBattleActor_404DB0;
        cg::world::gBattleAllEntries59E980[second]->phase = BattleAction_Idle;
        cg::world::gBattleAllEntries59E980[second]->flags &= ~YobiActionFlag_Hidden;
        YobiAction* const secondXTarget = cg::world::gBattleAllEntries59E980[second];
        secondXTarget->origin_x_offset40 = action->origin_x_offset40;
        YobiAction* const secondYTarget = cg::world::gBattleAllEntries59E980[second];
        secondYTarget->origin_y_offset44 = action->origin_y_offset44;
        cg::world::gBattleAllEntries59E980[second]->x = static_cast<std::int32_t>(action->origin_x_offset40);
        cg::world::gBattleAllEntries59E980[second]->y = static_cast<std::int32_t>(action->origin_y_offset44);
        carrier->second04 = nullptr;
        YobiAction* animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->graphicIndex = action->graphicIndex;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->framePlayingRate = action->framePlayingRate;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->lastImageNo = action->lastImageNo;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->characterAction = action->characterAction;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->lastCharacterAction = action->lastCharacterAction;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->direction = action->direction;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->lastDirection = action->lastDirection;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->frameIndex = action->frameIndex;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->frameRemain = action->frameRemain;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->graphicOffsetX = action->graphicOffsetX;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->graphicOffsetY = action->graphicOffsetY;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->frameEvent = action->frameEvent;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->hidden = action->hidden;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->field2D8 = action->field2D8;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->completedLoops = action->completedLoops;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->playRevert = action->playRevert;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->frameX = action->frameX;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->frameY = action->frameY;
        YobiAction* const descriptorTarget = cg::world::gBattleAllEntries59E980[second];
        descriptorTarget->drawMode = action->drawMode;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->batchType = action->batchType;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->mixFlagsOverrideMask = action->mixFlagsOverrideMask;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->color2 = action->color2;
        animationTarget = cg::world::gBattleAllEntries59E980[second];
        animationTarget->mixFlagsOverrideValue = action->mixFlagsOverrideValue;
        RestoreBattleStateEffects_422370(cg::world::gBattleAllEntries59E980[second]);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[second]);
        static_cast<void>(PlayFrame_517DA0(cg::world::gBattleAllEntries59E980[second], AnimationPlayback_Loop));
        action->graphicIndex = -1;
        action->lastImageNo = 0;
        ClearBattleRideAttachments(payload);
        static_cast<void>(audio::PlaySe_52C450(463, payload->rider13C8->x));
    }
    payload->rider13C8->phase = BattleRiderSyncMode_DrawAttributesOnly;
    payload->rider13C8->characterAction = flags != 0 ? CharacterAnimation_Damage : CharacterAnimation_Sit;
    payload->rider13C8->flags |= YobiActionMask_PendingValue1OrHoverDescription;
    const std::int32_t x = static_cast<std::int32_t>(action->origin_x_offset40);
    const std::int32_t screenX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    YobiAction* const deltaXRider = payload->rider13C8;
    deltaXRider->deltaX = x + screenX;
    const std::int32_t y = static_cast<std::int32_t>(action->origin_y_offset44);
    const std::int32_t screenY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    YobiAction* const deltaYRider = payload->rider13C8;
    deltaYRider->deltaY = y + screenY;
    YobiAction* const positionXRider = payload->rider13C8;
    positionXRider->x = positionXRider->deltaX;
    YobiAction* const positionYRider = payload->rider13C8;
    positionYRider->y = positionYRider->deltaY;
    YobiAction* const movingRider = payload->rider13C8;
    const std::int32_t time = movingRider->field58;
    const std::int32_t offset = flags != 0 ? time * time / 5 + cg::world::gBattleAllEntries59E980[second]->frameY
        : (time - 15) * (time - 15) - 225;
    movingRider->y += offset;
    const bool finish = flags != 0 || payload->rider13C8->field58 > 15;
    YobiAction* const counterRider = payload->rider13C8;
    const std::int32_t nextTime = counterRider->field58 + 1;
    counterRider->field58 = nextTime;
    if (finish && offset >= 0) {
        YobiAction* const oldFirst = cg::world::gBattleAllEntries59E980[first];
        if (oldFirst != nullptr) {
            static_cast<void>(MarkDeath(oldFirst));
            cg::world::gBattleAllEntries59E980[first] = nullptr;
        }
        YobiAction* const restoredFirst = carrier->first00;
        cg::world::gBattleAllEntries59E980[first] = restoredFirst;
        restoredFirst->callback = UpdateBattleActor_404DB0;
        cg::world::gBattleAllEntries59E980[first]->flags &= ~YobiActionFlag_Hidden;
        YobiAction* firstTarget = cg::world::gBattleAllEntries59E980[first];
        firstTarget->origin_x_offset40 = action->origin_x_offset40;
        firstTarget = cg::world::gBattleAllEntries59E980[first];
        firstTarget->origin_y_offset44 = action->origin_y_offset44;
        firstTarget = cg::world::gBattleAllEntries59E980[first];
        firstTarget->x = action->x;
        firstTarget = cg::world::gBattleAllEntries59E980[first];
        firstTarget->y = action->y;
        firstTarget = cg::world::gBattleAllEntries59E980[first];
        firstTarget->direction = action->direction;
        cg::world::gBattleAllEntries59E980[first]->motionMagnitude54 = 6.0F;
        RestoreBattleRideGear(&cg::world::gBattleAllEntries59E980[first], carrier);
        carrier->first00 = nullptr;
        static_cast<void>(PlayFrame_517DA0(cg::world::gBattleAllEntries59E980[first], AnimationPlayback_Loop));
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[first]);
        payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        static_cast<void>(MarkDeath(payload->rider13C8));
        YobiAction* const oldEffect = payload->effect1060;
        payload->rider13C8 = nullptr;
        static_cast<void>(MarkDeath(oldEffect));
        payload->effect1060 = nullptr;
        RestoreBattleStateEffects_422370(cg::world::gBattleAllEntries59E980[first]);
        static_cast<void>(audio::PlaySe_52C450(457, cg::world::gBattleAllEntries59E980[first]->x));
        static_cast<void>(MarkDeath(action));
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        static_cast<void>(PlayFrame_517DA0(rider, AnimationPlayback_Loop));
    }
}

// 原函数：0x004150F0。
void UpdateBattleAction225_4150F0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const initialRider = payload->rider13C8;
    if (initialRider == nullptr) {
        payload->baseImage13AC = action->imageNo;
        const std::int32_t imageNo = world::ResolveRidingCharacterImage_51C0D0(action->imageNo);
        YobiAction* const rider = NewBattleRider_4221F0(action, imageNo);
        payload->rider13C8 = rider;
        rider->phase = BattleRiderSyncMode_DrawAttributesOnly;
        payload->rider13C8->field58 = 0;
        payload->rider13C8->characterAction = CharacterAnimation_Sit;
        payload->rider13C8->nameC4 = action->nameC4;
        YobiAction* const levelRider = payload->rider13C8;
        const std::int32_t level = action->field1D0;
        levelRider->field1D0 = level;
        payload->rider13C8->readiness = action->readiness;
        payload->rider13C8->maxHp = action->maxHp;
        payload->rider13C8->barValue1B0 = action->barValue1B0;
        payload->rider13C8->barMaximum1C0 = action->barMaximum1C0;
        YobiAction* const copyXRider = payload->rider13C8;
        const std::int32_t sourceX = action->x;
        copyXRider->x = sourceX;
        YobiAction* const copyYRider = payload->rider13C8;
        const std::int32_t sourceY = action->y;
        copyYRider->y = sourceY;
        YobiAction* const copyOffsetXRider = payload->rider13C8;
        const float sourceOffsetX = action->origin_x_offset40;
        copyOffsetXRider->origin_x_offset40 = sourceOffsetX;
        YobiAction* const copyOffsetYRider = payload->rider13C8;
        const float sourceOffsetY = action->origin_y_offset44;
        copyOffsetYRider->origin_y_offset44 = sourceOffsetY;
        payload->rider13C8->flags |= YobiActionMask_PendingValue1OrHoverDescription;
        action->graphicIndex = -1;
        action->lastImageNo = 0;
        FaceBattleActorTowardFormationPosition_406260(action);
        YobiAction* const frameEffect = payload->effect1060;
        const std::int32_t frameX = action->frameX;
        frameEffect->deltaX = frameX;
        YobiAction* const effect = payload->effect1060;
        YobiAction* const currentRider = payload->rider13C8;
        const std::int32_t targetDeltaX = effect->deltaX;
        const float targetY = effect->origin_y_offset44;
        const float targetX = static_cast<float>(static_cast<double>(targetDeltaX) + effect->origin_x_offset40);
        TurnBattleTowardPosition_4060D0(currentRider, targetX, targetY, 1, 0.0F);
        payload->rider13C8->motionMagnitude54 = 8.0F;
        static_cast<void>(PlayFrame_517DA0(payload->rider13C8, AnimationPlayback_Loop));
        YobiAction* const soundRider = payload->rider13C8;
        static_cast<void>(audio::PlaySe_52C450(465, soundRider->x));
        return;
    }
    initialRider->characterAction = CharacterAnimation_Sit;
    YobiAction* const effect = payload->effect1060;
    YobiAction* const currentRider = payload->rider13C8;
    const std::int32_t targetDeltaX = effect->deltaX;
    const float targetY = effect->origin_y_offset44;
    const float targetX = static_cast<float>(static_cast<double>(targetDeltaX) + effect->origin_x_offset40);
    if (GetBattleDistanceToPosition_4062A0(currentRider, targetX, targetY) >= 5) {
        MoveBattleByAngle_405EA0(payload->rider13C8);
    } else {
        YobiAction* const positionEffect = payload->effect1060;
        const double x = static_cast<double>(positionEffect->deltaX) + positionEffect->origin_x_offset40;
        payload->rider13C8->origin_x_offset40 = static_cast<float>(x);
        YobiAction* const yEffect = payload->effect1060;
        YobiAction* const yRider = payload->rider13C8;
        yRider->origin_y_offset44 = yEffect->origin_y_offset44;
    }
    YobiAction* const xRider = payload->rider13C8;
    const std::int32_t projectedX = static_cast<std::int32_t>(xRider->origin_x_offset40);
    xRider->deltaX = projectedX + static_cast<std::int32_t>(gYobiScreenOffsetX);
    YobiAction* const yRider = payload->rider13C8;
    const std::int32_t projectedY = static_cast<std::int32_t>(yRider->origin_y_offset44);
    yRider->deltaY = projectedY + static_cast<std::int32_t>(gYobiScreenOffsetY);
    YobiAction* const screenXRider = payload->rider13C8;
    screenXRider->x = screenXRider->deltaX;
    YobiAction* const screenYRider = payload->rider13C8;
    screenYRider->y = screenYRider->deltaY;
    YobiAction* const arcRider = payload->rider13C8;
    const std::int32_t arcTime = arcRider->field58;
    const std::int32_t baseY = arcRider->y;
    const std::int32_t arc = arcTime - 15;
    const std::int32_t displacement = arc * arc - 225;
    arcRider->y = baseY + displacement;
    YobiAction* const counterRider = payload->rider13C8;
    const std::int32_t previous = counterRider->field58;
    counterRider->field58 = previous + 1;
    if (previous > 15 && action->frameY <= displacement) {
        payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        payload->rider13C8->flags &= ~YobiActionMask_PendingValue1OrHoverDescription;
        payload->rider13C8->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        static_cast<void>(MarkDeath(payload->rider13C8));
        YobiAction* const restoreEffect = payload->effect1060;
        payload->rider13C8 = nullptr;
        action->origin_x_offset40 = restoreEffect->origin_x_offset40;
        YobiAction* const restoreYEffect = payload->effect1060;
        const float x = action->origin_x_offset40;
        action->origin_y_offset44 = restoreYEffect->origin_y_offset44;
        const std::int32_t screenX = static_cast<std::int32_t>(x);
        const float y = action->origin_y_offset44;
        action->x = screenX;
        action->y = static_cast<std::int32_t>(y);
        action->imageNo = payload->baseImage13AC;
        static_cast<void>(MarkDeath(payload->effect1060));
        payload->effect1060 = nullptr;
        static_cast<void>(audio::PlaySe_52C450(462, action->x));
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        static_cast<void>(AdvanceBattleEffect_439470(action));
        AdvanceBattleCastState_439340(action);
    }
    YobiAction* const finalRider = payload->rider13C8;
    if (finalRider != nullptr) {
        static_cast<void>(PlayFrame_517DA0(finalRider, AnimationPlayback_Loop));
    }
}

// 原函数：0x00416000。
std::int32_t UpdateBattleAction228_416000(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    payload->valueFF0 = 0;
    action->phase = BattleAction_AnimateSlowdown;
    return UpdateBattleAction229_416020(action);
}

// 原函数：0x00416020。
std::int32_t UpdateBattleAction229_416020(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Run;
    if (payload->valueFF0 < 60) {
        action->framePlayingRate = 250;
        action->framePlayingRate = 250 - 4 * payload->valueFF0;
        ++payload->valueFF0;
    } else {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, AnimationPlayback_Loop);
}

// 原函数：0x004160A0。
void UpdateBattleAction230_4160A0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t initialIndex = payload->effectRunningC9C;
    const float initialY = static_cast<float>(std::bit_cast<std::int32_t>(payload->effects1C[initialIndex].flags));
    const float initialX = static_cast<float>(payload->effects1C[initialIndex].position);
    TurnBattleTowardPosition_4060D0(action, initialX, initialY, 1, 10.0F);
    MoveBattleByAngle_405EA0(action);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t currentIndex = payload->effectRunningC9C;
    const float targetY = static_cast<float>(std::bit_cast<std::int32_t>(payload->effects1C[currentIndex].flags));
    const float targetX = static_cast<float>(payload->effects1C[currentIndex].position);
    if (GetBattleDistanceToPosition_4062A0(action, targetX, targetY) < 8) {
        action->origin_x_offset40 = static_cast<float>(payload->effects1C[payload->effectRunningC9C].position);
        action->origin_y_offset44 = static_cast<float>(
            std::bit_cast<std::int32_t>(payload->effects1C[payload->effectRunningC9C].flags));
        static_cast<void>(AdvanceBattleEffect_439470(action));
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00416150。
std::int32_t UpdateBattleAction231_416150(YobiAction* const action)
{
    const double speed = static_cast<double>(action->motionMagnitude54) - 0.45F;
    auto* const payload = action->payload.battleActor;
    action->characterAction = CharacterAnimation_Guard;
    action->motionMagnitude54 = static_cast<float>(speed);
    if (!(speed >= 0.0)) {
        action->motionMagnitude54 = 0.0F;
    }
    action->framePlayingRate = 100;
    MoveBattleByAngle_405EA0(action);
    if (!(action->motionMagnitude54 > 0.0F)) {
        AdvanceBattleCastState_439340(action);
        action->motionMagnitude54 = 6.0F;
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    if (payload->valueFF0 == 0) {
        static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
    }
    if (payload->valueFF0 == 1) {
        static_cast<void>(NewBattlePositionEffect_41E680(action, 110007));
    }
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous >= 2) {
        payload->valueFF0 = 0;
    }
    return previous;
}

} // namespace detail

} // namespace cg::yobi
