#include "battle_attachment.hpp"
#include "base.hpp"
#include "battle_base.hpp"

#include "animation.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/BattleState.hpp"
#include "../world/PlayerState.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原函数：0x00421F90。
void UpdateBattleAttachment_421F90(YobiAction* const action)
{
    const std::int32_t position = action->value1D4;
    YobiAction* const registered = action->ownerSession->WorldScene().battle.allEntries[position];
    YobiAction* const parent = action->battleAttachmentParent10;
    if (registered != parent) {
        static_cast<void>(MarkDeath(action));
    }
}

// 原[421FB0,4221C0)：404DB0公共尾显式同步，不替换421F90身份检查回调。
void SyncBattleRider_421FB0(YobiAction* const action)
{
    const std::int32_t position = action->value1D4;
    YobiAction* const parent = action->battleAttachmentParent10;
    YobiAction* const registered = action->ownerSession->WorldScene().battle.allEntries[position];
    const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (registered != parent) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    const bool followsParent = action->phase == BattleRiderSyncMode_FollowParent;
    if (followsParent) {
        action->deltaX = parent->x;
        action->deltaY = parent->y;
        switch (parent->characterAction) {
        case CharacterAnimation_Walk:
            action->characterAction = CharacterAnimation_Walk;
            break;
        case CharacterAnimation_BeginRun:
        case CharacterAnimation_Run:
        case CharacterAnimation_EndRun:
            action->characterAction = CharacterAnimation_Run;
            break;
        case CharacterAnimation_Attack:
        case CharacterAnimation_Cast:
            action->characterAction = CharacterAnimation_Attack;
            break;
        case CharacterAnimation_Damage:
        case CharacterAnimation_Down:
            action->characterAction = CharacterAnimation_Damage;
            break;
        case CharacterAnimation_Guard:
            action->characterAction = CharacterAnimation_Guard;
            break;
        default:
            action->characterAction = CharacterAnimation_Stand;
            break;
        }
        bool play;
        if (parent->phase != BattleAction_Idle) {
            play = true;
        } else if (action->characterAction != CharacterAnimation_Stand) {
            action->characterAction = CharacterAnimation_Stand;
            play = (*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & BattleActorSleepOrStoneMask) == 0;
        } else {
            play = (*reinterpret_cast<const unsigned char*>(&parent->flags) & YobiActionFlag_BattleIdleAnimation) != 0
                && (*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & BattleActorSleepOrStoneMask) == 0;
        }
        const std::int32_t parentDirection = parent->direction;
        if (action->direction != parentDirection) {
            action->direction = parentDirection;
            play = true;
        }
        if (play) {
            static_cast<void>(PlayFrame_517DA0(
                action, action->characterAction == CharacterAnimation_Attack ? AnimationPlayback_Loop : parent->playRevert));
        }
    }
    std::uint32_t drawFlags = (action->flags & YobiActionMask_AttachmentRetainedFlags) | YobiActionMask_BattleDepthSortOrSortByDeltaY;
    action->flags = drawFlags;
    drawFlags |= parent->flags & YobiActionMask_BlinkAndAlpha;
    action->flags = drawFlags;
    drawFlags |= parent->flags & YobiActionMask_HiddenOrFourPhaseUpdateGate;
    action->flags = drawFlags;
    action->batchType = parent->batchType;
    action->color2 = parent->color2;
    action->mixFlagsOverrideMask = parent->mixFlagsOverrideMask;
    action->mixFlagsOverrideValue = parent->mixFlagsOverrideValue;
    if (followsParent) {
        const std::int32_t frameX = parent->frameX;
        const float xScale = parent->scaleX;
        const std::int32_t xOffset = static_cast<std::int32_t>(
            static_cast<double>(frameX) * xScale);
        action->x = xOffset + parent->x;
        const std::int32_t frameY = parent->frameY;
        const float yScale = parent->scaleY;
        const std::int32_t yOffset = static_cast<std::int32_t>(
            static_cast<double>(frameY) * yScale);
        action->y = yOffset + parent->y;
    }
    action->scaleX = parent->scaleX;
    action->scaleY = parent->scaleY;
}

// 原[4225D0,42277A)身份/绘制属性接线；主挂件动作及坐标按AF406，不改变装备自己的缩放。
void SyncBattleGear_4225D0(YobiAction* const action)
{
    using enum resource::animation::AnimeActionFlag;
    const std::int32_t position = action->value1D4;
    YobiAction* const parent = action->battleAttachmentParent10;
    YobiAction* const registered = action->ownerSession->WorldScene().battle.allEntries[position];
    const bool hasRegisteredParent = registered == parent;
    const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (!hasRegisteredParent) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    action->deltaX = parent->x;
    action->deltaY = parent->y;
    const std::uint16_t animeFlags = GetYobiAnimeActionFlags(action);
    const bool wing = (animeFlags & AnimeActionFlag_Wing) != 0;
    const bool fixedPlacement = !wing && (animeFlags & (AnimeActionFlag_Field | AnimeActionFlag_Shield)) != 0;
    const std::int32_t parentAction = parent->characterAction;
    if (wing) {
        action->characterAction = parentAction;
    } else if (!fixedPlacement) {
        action->characterAction = parentAction >= CharacterAnimation_BeginRun && parentAction <= CharacterAnimation_EndRun ? CharacterAnimation_Stand
            : parentAction == CharacterAnimation_Damage ? CharacterAnimation_Run : parentAction;
    }
    const std::uint32_t flags = payload->flagsFDC;
    std::uint32_t extraFlags = 0;
    if ((flags & BattleActorFlag_Rebirth) != 0) {
        extraFlags = 2;
    }
    if (payload->appearanceOverrideActive13B0 != 0) {
        extraFlags = 2;
    }
    if ((flags & BattleActorFlag_Morph) != 0) {
        extraFlags = 2;
    }
    action->direction = parent->direction;
    const std::int32_t image = parent->imageNo;
    const bool suppressWing = wing && parentAction == CharacterAnimation_Cast
        && ((image >= 100250 && image <= 100273) || (image >= 100650 && image <= 100655));
    if (suppressWing || (fixedPlacement && parentAction == CharacterAnimation_Down)) {
        extraFlags = 2;
    } else {
        const std::int32_t stopAtEnd = wing ? parent->playRevert : AnimationPlayback_Loop;
        // 4.06@46EA79持续播放，返回值不作为主挂件隐藏条件。
        static_cast<void>(PlayFrame_517DA0(action, stopAtEnd, wing ? parent : nullptr));
    }
    std::uint32_t drawFlags = (action->flags & YobiActionMask_AttachmentRetainedFlags) | YobiActionMask_BattleDepthSortOrSortByDeltaY;
    action->flags = drawFlags;
    drawFlags |= parent->flags & YobiActionMask_BlinkAndAlpha;
    action->flags = drawFlags;
    drawFlags |= parent->flags & YobiActionMask_HiddenOrFourPhaseUpdateGate;
    drawFlags |= extraFlags;
    action->flags = drawFlags;
    action->batchType = parent->batchType;
    action->color2 = parent->color2;
    action->mixFlagsOverrideMask = parent->mixFlagsOverrideMask;
    action->mixFlagsOverrideValue = parent->mixFlagsOverrideValue;
    // AF406：4.06@46E83A之后只叠加Coordinate，不把人物动画帧偏移重复加入挂件。
    std::int32_t xOffset = 0;
    std::int32_t yOffset = 0;
    if (!fixedPlacement) {
        if ((animeFlags & AnimeActionFlag_LockPos) == 0) {
            GetYobiAttachmentCoordinate(parent, xOffset, yOffset, false);
        }
        YobiAction* const rider = payload->rider13C8;
        if (rider != nullptr) {
            std::int32_t riderX;
            std::int32_t riderY;
            GetYobiAttachmentCoordinate(rider, riderX, riderY);
            xOffset += riderX;
            yOffset += riderY;
        }
    }
    action->x = parent->x + static_cast<std::int16_t>(xOffset);
    action->y = parent->y + static_cast<std::int16_t>(yOffset);
}

const YobiAction* GetBattleTitleAnchor(const YobiAction* const action)
{
    if (action->callback != UpdateBattleAttachment_421F90
        || (GetYobiAnimeActionFlags(action) & resource::animation::AnimeActionFlag_Title) == 0) {
        return nullptr;
    }
    const YobiAction* const parent = action->battleAttachmentParent10;
    if (parent == nullptr || parent->payloadKind != YobiPayloadKind::BattleActor
        || parent->payload.battleActor->gear13B4 != action) {
        return nullptr;
    }
    const YobiAction* const rider = parent->payload.battleActor->rider13C8;
    return rider != nullptr ? rider : parent;
}

// AF406：战斗排序完成后承接4.06@46E910后的WING/FIELD/SHIELD层选择。
void ResolveBattleGearLayer406(YobiAction* const action)
{
    using enum resource::animation::AnimeActionFlag;
    if (action->callback != UpdateBattleAttachment_421F90) {
        return;
    }
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent == nullptr || parent->payloadKind != YobiPayloadKind::BattleActor
        || parent->payload.battleActor->gear13B4 != action) {
        return;
    }
    const std::uint16_t flags = GetYobiAnimeActionFlags(action);
    if ((flags & AnimeActionFlag_Title) != 0) {
        return; // AT01：保留排序器分配的本人物挂件组最前景层。
    }
    if ((flags & AnimeActionFlag_Wing) != 0) {
        // 本线每个对象独立分配层；保留正常前景层，只把背向挂件放到人物之前。
        if (parent->direction >= YobiDirection_DownRight && parent->direction <= YobiDirection_DownLeft) {
            action->displayPriority = static_cast<std::uint8_t>(parent->displayPriority - 1);
        }
    } else if ((flags & AnimeActionFlag_Field) != 0) {
        action->displayPriority = 2;
    }
}

// 原函数：0x00422780。
void SyncBattleAuxGear_422780(YobiAction* const action, const std::int32_t index)
{
    if (action == nullptr) {
        return;
    }
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (payload == nullptr) {
        return;
    }
    const std::int32_t parentDirection = parent->direction;
    const std::int32_t position = action->value1D4;
    action->direction = parentDirection;
    if (action->ownerSession->WorldScene().battle.allEntries[position] != parent) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    std::int32_t graphicIndex = action->graphicIndex;
    std::int32_t type = 0;
    if (index == 0) {
        type = payload->auxiliaryGearTypes13C0[0];
    } else if (index == 1) {
        type = payload->auxiliaryGearTypes13C0[1];
    }
    std::int32_t characterAction = CharacterAnimation_Stand;
    std::int32_t stopAtEnd = AnimationPlayback_Loop;
    std::int32_t xOffset = 0;
    std::int32_t yOffset = 0;
    switch (type) {
    case BattleAuxGearType_StandLoop:
        break;
    case BattleAuxGearType_DirectionalQuarterScale:
    case BattleAuxGearType_DirectionalHalfScale:
    case BattleAuxGearType_DirectionalThreeQuarterScale: {
        const std::int32_t parentCharacterAction = parent->characterAction;
        switch (parentCharacterAction) {
        case CharacterAnimation_Stand:
        case CharacterAnimation_Wave:
        case CharacterAnimation_Joy:
        case CharacterAnimation_Angry:
        case CharacterAnimation_Sad:
        case CharacterAnimation_Nod:
            break;
        case CharacterAnimation_Walk:
        case CharacterAnimation_Run:
        case CharacterAnimation_Cast:
            characterAction = parentCharacterAction;
            break;
        case CharacterAnimation_BeginRun:
        case CharacterAnimation_EndRun:
        case CharacterAnimation_Attack:
        case CharacterAnimation_Damage:
        case CharacterAnimation_Guard:
        case CharacterAnimation_Down:
            characterAction = parentCharacterAction;
            stopAtEnd = AnimationPlayback_StopAtEnd;
            break;
        default:
            stopAtEnd = AnimationPlayback_StopAtEnd;
            break;
        }
        switch (parent->direction) {
        case YobiDirection_UpLeft: xOffset = 34; yOffset = -4; break;
        case YobiDirection_Up: xOffset = 36; yOffset = 16; break;
        case YobiDirection_UpRight: xOffset = 16; yOffset = 24; break;
        case YobiDirection_Right: xOffset = -16; yOffset = 26; break;
        case YobiDirection_DownRight: xOffset = -34; yOffset = 8; break;
        case YobiDirection_Down: xOffset = -36; yOffset = -12; break;
        case YobiDirection_DownLeft: xOffset = -20; yOffset = -20; break;
        case YobiDirection_Left: xOffset = 16; yOffset = -26; break;
        default: break;
        }
        break;
    }
    case BattleAuxGearType_ClearGraphicOnDown:
        if (parent->characterAction == CharacterAnimation_Down) {
            characterAction = CharacterAnimation_Down;
            stopAtEnd = AnimationPlayback_StopAtEnd;
            graphicIndex = -1;
        }
        break;
    default:
        if (parent->characterAction == CharacterAnimation_Down) {
            characterAction = CharacterAnimation_Down;
            stopAtEnd = AnimationPlayback_StopAtEnd;
        }
        break;
    }
    action->characterAction = characterAction;
    action->graphicIndex = graphicIndex;
    action->flags = parent->flags | YobiActionFlag_AuxiliaryBattleGear;
    action->deltaX = parent->deltaX;
    action->deltaY = parent->deltaY;
    action->origin_x_offset40 = parent->origin_x_offset40;
    action->origin_y_offset44 = parent->origin_y_offset44;
    const std::int32_t x = static_cast<std::int32_t>(parent->origin_x_offset40);
    const std::int32_t deltaX = parent->deltaX;
    action->x = x + deltaX + xOffset;
    const std::int32_t y = static_cast<std::int32_t>(parent->origin_y_offset44);
    const std::int32_t deltaY = parent->deltaY;
    action->y = y + yOffset + deltaY;
    static_cast<void>(PlayFrame_517DA0(action, stopAtEnd));
}

// 原函数：0x004221F0。
YobiAction* NewBattleRider_4221F0(YobiAction* const parent, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->imageNo = imageNo;
    const std::uint32_t configuredFlags = flags | YobiActionMask_BattleAttachmentDefaults;
    action->callback = UpdateBattleAttachment_421F90;
    action->flags = configuredFlags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 3.0F;
    const std::int32_t direction = parent->direction;
    action->displayPriority = 9;
    action->direction = direction;
    action->phase = BattleRiderSyncMode_FollowParent;
    action->pathCountOrDrawOrder_270 = 1;
    action->battleAttachmentParent10 = parent;
    action->value1D4 = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->origin_x_offset40);
    action->x = x + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->origin_y_offset44);
    action->y = y + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
    return action;
}

// 原函数：0x00422A30。
YobiAction* NewBattleGear_422A30(YobiAction* const parent, const std::int32_t imageNo)
{
    if (parent == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | YobiActionMask_BattleAttachmentDefaults;
    action->callback = UpdateBattleAttachment_421F90;
    action->flags = flags;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 3.0F;
    action->direction = parent->direction;
    action->displayPriority = 9;
    action->phase = 0;
    action->pathCountOrDrawOrder_270 = 2;
    action->battleAttachmentParent10 = parent;
    action->value1D4 = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->origin_x_offset40);
    action->x = x + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->origin_y_offset44);
    action->y = y + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
    return action;
}

// 原函数：0x00422B00。
YobiAction* NewBattleAuxGear_422B00(
    YobiAction* const parent, const std::int32_t imageNo, const std::int16_t sortIndex)
{
    if (parent == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleAttachment_421F90;
    const std::uint32_t flags = parent->flags;
    action->imageNo = imageNo;
    action->flags = flags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 3.0F;
    const std::int32_t direction = parent->direction;
    action->displayPriority = 9;
    action->direction = direction;
    const std::uint16_t pathCountOrDrawOrder_270 = static_cast<std::uint16_t>(sortIndex);
    action->phase = 0;
    action->battleAttachmentParent10 = parent;
    const std::int32_t position = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    action->pathCountOrDrawOrder_270 = pathCountOrDrawOrder_270;
    action->value1D4 = position;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->origin_x_offset40);
    action->x = x + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->origin_y_offset44);
    action->y = y + static_cast<std::int32_t>(action->ownerSession->Yobi().screenOffsetY);
    return action;
}

// 原函数：0x004222B0。
std::int32_t SetBattleRider_4222B0(
    YobiAction* const action, const std::int32_t imageNo, const std::int32_t ridingImageNo)
{
    auto* const payload = action->payload.battleActor;
    if (ridingImageNo > 0) {
        if (payload->rider13C8 == nullptr) {
            const std::int32_t mapped = world::ResolveRidingCharacterImage_51C0D0(imageNo);
            YobiAction* const rider = NewBattleRider_4221F0(action, mapped);
            payload->rider13C8 = rider;
            rider->phase = BattleRiderSyncMode_FollowParent;
            payload->rider13C8->field58 = 0;
            payload->rider13C8->characterAction = CharacterAnimation_Stand;
        }
        if (payload->rider13C8 != nullptr) {
            const std::int32_t mapped = world::ResolveRidingCharacterImage_51C0D0(imageNo);
            payload->rider13C8->imageNo = mapped;
        }
        action->imageNo = ridingImageNo;
        return ridingImageNo;
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        rider->phase = BattleRiderSyncMode_FollowParent;
        payload->rider13C8->field58 = 0;
        static_cast<void>(MarkDeath(payload->rider13C8));
        payload->rider13C8 = nullptr;
    }
    action->imageNo = imageNo;
    return imageNo;
}

// 原函数：0x00422BC0。
void SetBattleGear_422BC0(YobiAction* const action, const std::int32_t imageNo)
{
    auto* const payload = action->payload.battleActor;
    if (imageNo > 0) {
        if (payload->gear13B4 == nullptr) {
            payload->gear13B4 = NewBattleGear_422A30(action, imageNo);
        }
        YobiAction* const gear = payload->gear13B4;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
    } else {
        YobiAction* const gear = payload->gear13B4;
        if (gear != nullptr) {
            static_cast<void>(MarkDeath(gear));
            payload->gear13B4 = nullptr;
        }
    }
}

// 原函数：0x00422C20。
void SetBattleGear_422C20(
    YobiAction* const action, const std::int32_t imageNo, const std::int32_t index)
{
    auto* const payload = action->payload.battleActor;
    if (imageNo > 0) {
        if (payload->gear13B4 == nullptr) {
            payload->gear13B4 = NewBattleGear_422A30(action, imageNo);
        }
        YobiAction* const gear = payload->gear13B4;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
    } else {
        YobiAction* const gear = payload->gear13B4;
        if (gear != nullptr && index == 0) {
            static_cast<void>(MarkDeath(gear));
            payload->gear13B4 = nullptr;
        }
    }
}

// 原函数：0x00422C90。
void SetBattleAuxGear_422C90(
    YobiAction* const action, const std::int32_t imageNo,
    const std::int32_t type, const std::int32_t index)
{
    auto* const payload = action->payload.battleActor;
    if (imageNo <= 0) {
        if (type > 0) {
            YobiAction* const firstGear = payload->auxiliaryGear13B8[0];
            if (firstGear != nullptr) {
                static_cast<void>(MarkDeath(firstGear));
                payload->auxiliaryGear13B8[0] = nullptr;
            }
            YobiAction* const secondGear = payload->auxiliaryGear13B8[1];
            if (secondGear != nullptr) {
                static_cast<void>(MarkDeath(secondGear));
                payload->auxiliaryGear13B8[1] = nullptr;
            }
        }
        return;
    }
    if (index == 1 && type >= BattleAuxGearType_DirectionalQuarterScale && type <= BattleAuxGearType_DirectionalThreeQuarterScale) {
        const std::int32_t firstType = payload->auxiliaryGearTypes13C0[0];
        if (firstType >= BattleAuxGearType_DirectionalQuarterScale && firstType <= BattleAuxGearType_DirectionalThreeQuarterScale) {
            static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[0]));
            payload->auxiliaryGear13B8[0] = nullptr;
        }
    }
    // 原两caller只传0/1；不模拟其他index把图号当地址的异常路径。
    YobiAction** const slot = &payload->auxiliaryGear13B8[index];
    payload->auxiliaryGearTypes13C0[index] = type;
    switch (type) {
    case BattleAuxGearType_StandOrDown: {
        if (*slot == nullptr) {
            *slot = NewBattleAuxGear_422B00(action, imageNo, -1);
        }
        YobiAction* const gear = *slot;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
        return;
    }
    case BattleAuxGearType_StandLoop: {
        if (*slot == nullptr) {
            *slot = NewBattleAuxGear_422B00(action, imageNo, 3);
        }
        YobiAction* const gear = *slot;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
        return;
    }
    case BattleAuxGearType_DirectionalQuarterScale:
    case BattleAuxGearType_DirectionalHalfScale:
    case BattleAuxGearType_DirectionalThreeQuarterScale: {
        if (*slot == nullptr) {
            YobiAction* const gear = NewBattleAuxGear_422B00(action, imageNo, 0);
            const bool isTypeThree = type == BattleAuxGearType_DirectionalQuarterScale;
            *slot = gear;
            if (isTypeThree) {
                gear->scaleX = 0.25F;
                (*slot)->scaleY = 0.25F;
            } else if (type == BattleAuxGearType_DirectionalHalfScale) {
                gear->scaleX = 0.5F;
                (*slot)->scaleY = 0.5F;
            } else if (type == BattleAuxGearType_DirectionalThreeQuarterScale) {
                gear->scaleX = 0.75F;
                (*slot)->scaleY = 0.75F;
            }
        }
        YobiAction* const gear = *slot;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
        return;
    }
    case BattleAuxGearType_ClearGraphicOnDown:
        if (*slot == nullptr) {
            *slot = NewBattleAuxGear_422B00(action, imageNo, 3);
        }
        return;
    default:
        return;
    }
}

// 原函数：0x00420F10。
void SyncBattleAttachmentAlpha_420F10(
    YobiAction* const action, const std::int32_t alpha, const std::int8_t delta,
    const std::uint32_t addMask, const std::uint32_t removeMask)
{
    auto* const payload = action->payload.battleActor;
    // 保存槽地址而非对象快照，保留每个操作后的重取及多个槽指向同一对象的重复处理。
    const std::array<YobiAction**, 9> slots {
        &payload->buffEffectAction_1054, &payload->attachedEffect1058, &payload->attributeReversalEffect105C,
        &payload->centeredEffect1064, &payload->statusEffect1068, &payload->rebirthEffect106C,
        &payload->rebirthAuxiliaryEffects1074[0], &payload->rebirthAuxiliaryEffects1074[1],
        &payload->rebirthAuxiliaryEffects1074[2],
    };
    for (YobiAction** const slot : slots) {
        if (*slot == nullptr) {
            continue;
        }
        if (alpha >= 0) {
            SetYobiActionField30F(*slot, static_cast<std::uint8_t>(alpha));
        }
        YobiAction* const adjusted = *slot;
        SetYobiActionField30F(adjusted,
            static_cast<std::uint8_t>(GetYobiActionField30F(adjusted) + delta));
        (*slot)->flags &= ~removeMask;
        (*slot)->flags |= addMask;
        SetYobiActionField308(*slot, static_cast<std::int32_t>(YobiColorChannelFlag_None));
        YobiAction* const batched = *slot;
        if (GetYobiActionField30F(batched) != 255) {
            SetYobiActionField308(batched, static_cast<std::int32_t>(YobiColorChannelFlag_Alpha));
        }
        (*slot)->flags &= ~YobiActionFlag_Hidden;
        YobiAction* const visible = *slot;
        if (GetYobiActionField30F(visible) == 0) {
            visible->flags |= YobiActionFlag_Hidden;
        }
    }
}

} // namespace detail

} // namespace cg::yobi
