#include "battle_attachment.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <cstdint>

namespace cg::login {

namespace detail {

// 原函数：0x00421F90。
void UpdateBattleAttachment_421F90(YobiAction* const action)
{
    const std::int32_t position = action->value1D4;
    YobiAction* const registered = world::detail::gBattleAllEntries59E980[position];
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
    YobiAction* const registered = world::detail::gBattleAllEntries59E980[position];
    const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (registered != parent) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    const bool followsParent = action->phase == 0;
    if (followsParent) {
        action->deltaX = parent->x;
        action->deltaY = parent->y;
        switch (parent->characterAction) {
        case 1:
            action->characterAction = 1;
            break;
        case 2:
        case 3:
        case 4:
            action->characterAction = 3;
            break;
        case 5:
        case 6:
            action->characterAction = 5;
            break;
        case 8:
        case 10:
            action->characterAction = 8;
            break;
        case 9:
            action->characterAction = 9;
            break;
        default:
            action->characterAction = 0;
            break;
        }
        bool play;
        if (parent->phase != 0) {
            play = true;
        } else if (action->characterAction != 0) {
            action->characterAction = 0;
            play = (*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & 0x60u) == 0;
        } else {
            play = (*reinterpret_cast<const unsigned char*>(&parent->flags) & 0x40u) != 0
                && (*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & 0x60u) == 0;
        }
        const std::int32_t parentDirection = parent->direction;
        if (action->direction != parentDirection) {
            action->direction = parentDirection;
            play = true;
        }
        if (play) {
            static_cast<void>(PlayFrame_517DA0(
                action, action->characterAction == 5 ? 0 : parent->playRevert));
        }
    }
    std::uint32_t drawFlags = (action->flags & 0xFFEA7FFDu) | 0x820000u;
    action->flags = drawFlags;
    drawFlags |= parent->flags & 0x118000u;
    action->flags = drawFlags;
    drawFlags |= parent->flags & 0x40002u;
    action->flags = drawFlags;
    action->batchType = parent->batchType;
    action->color2 = parent->color2;
    action->rebirthValue1 = parent->rebirthValue1;
    action->rebirthValue2 = parent->rebirthValue2;
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

// 原[4225D0,42277A)：播放非零返回会隐藏主装备，不改变装备自己的缩放。
void SyncBattleGear_4225D0(YobiAction* const action)
{
    const std::int32_t position = action->value1D4;
    YobiAction* const parent = action->battleAttachmentParent10;
    YobiAction* const registered = world::detail::gBattleAllEntries59E980[position];
    const bool hasRegisteredParent = registered == parent;
    const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (!hasRegisteredParent) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    action->deltaX = parent->x;
    action->deltaY = parent->y;
    action->characterAction = parent->characterAction == 8 ? 3 : 0;
    const std::uint32_t flags = payload->flagsFDC;
    std::uint32_t extraFlags = 0;
    if ((flags & 0x10000000u) != 0) {
        extraFlags = 2;
    }
    if (payload->value13B0 != 0) {
        extraFlags = 2;
    }
    if ((flags & 0x2000u) != 0) {
        extraFlags = 2;
    }
    bool play;
    if (parent->phase != 0) {
        play = true;
    } else if (action->characterAction != 0) {
        action->characterAction = 0;
        play = (*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & 0x60u) == 0;
    } else {
        play = (*reinterpret_cast<const unsigned char*>(&parent->flags) & 0x40u) != 0 && (flags & 0x60u) == 0;
    }
    const std::int32_t parentDirection = parent->direction;
    if (action->direction != parentDirection) {
        action->direction = parentDirection;
        play = true;
    }
    if (play && PlayFrame_517DA0(action, 0) != 0) {
        extraFlags = 2;
    }
    std::uint32_t drawFlags = (action->flags & 0xFFEA7FFDu) | 0x820000u;
    action->flags = drawFlags;
    drawFlags |= parent->flags & 0x118000u;
    action->flags = drawFlags;
    drawFlags |= parent->flags & 0x40002u;
    drawFlags |= extraFlags;
    action->flags = drawFlags;
    action->batchType = parent->batchType;
    action->color2 = parent->color2;
    action->rebirthValue1 = parent->rebirthValue1;
    action->rebirthValue2 = parent->rebirthValue2;
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        const std::int32_t frameX = rider->frameX;
        const float xScale = parent->scaleX;
        const std::int32_t xOffset = static_cast<std::int32_t>(
            static_cast<double>(frameX) * xScale);
        action->x = xOffset + rider->x;
        YobiAction* const currentRider = payload->rider13C8;
        const std::int32_t frameY = currentRider->frameY;
        const float yScale = parent->scaleX;
        const std::int32_t yOffset = static_cast<std::int32_t>(
            static_cast<double>(frameY) * yScale);
        action->y = yOffset + currentRider->y;
    } else {
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
    if (world::detail::gBattleAllEntries59E980[position] != parent) {
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
    std::int32_t characterAction = 0;
    std::int32_t stopAtEnd = 0;
    std::int32_t xOffset = 0;
    std::int32_t yOffset = 0;
    switch (type) {
    case 2:
        break;
    case 3:
    case 4:
    case 5: {
        const std::int32_t parentCharacterAction = parent->characterAction;
        switch (parentCharacterAction) {
        case 0:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            break;
        case 1:
        case 3:
        case 6:
            characterAction = parentCharacterAction;
            break;
        case 2:
        case 4:
        case 5:
        case 8:
        case 9:
        case 10:
            characterAction = parentCharacterAction;
            stopAtEnd = 1;
            break;
        default:
            stopAtEnd = 1;
            break;
        }
        switch (parent->direction) {
        case 0: xOffset = 34; yOffset = -4; break;
        case 1: xOffset = 36; yOffset = 16; break;
        case 2: xOffset = 16; yOffset = 24; break;
        case 3: xOffset = -16; yOffset = 26; break;
        case 4: xOffset = -34; yOffset = 8; break;
        case 5: xOffset = -36; yOffset = -12; break;
        case 6: xOffset = -20; yOffset = -20; break;
        case 7: xOffset = 16; yOffset = -26; break;
        default: break;
        }
        break;
    }
    case 6:
        if (parent->characterAction == 10) {
            characterAction = 10;
            stopAtEnd = 1;
            graphicIndex = -1;
        }
        break;
    default:
        if (parent->characterAction == 10) {
            characterAction = 10;
            stopAtEnd = 1;
        }
        break;
    }
    action->characterAction = characterAction;
    action->graphicIndex = graphicIndex;
    action->flags = parent->flags | 0x2000000u;
    action->deltaX = parent->deltaX;
    action->deltaY = parent->deltaY;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    const std::int32_t x = static_cast<std::int32_t>(parent->offset40);
    const std::int32_t deltaX = parent->deltaX;
    action->x = x + deltaX + xOffset;
    const std::int32_t y = static_cast<std::int32_t>(parent->offset44);
    const std::int32_t deltaY = parent->deltaY;
    action->y = y + yOffset + deltaY;
    static_cast<void>(PlayFrame_517DA0(action, stopAtEnd));
}

// 原函数：0x004221F0。
YobiAction* NewBattleRider_4221F0(YobiAction* const parent, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->imageNo = imageNo;
    const std::uint32_t configuredFlags = flags | 0x860040u;
    action->callback = UpdateBattleAttachment_421F90;
    action->flags = configuredFlags;
    action->characterAction = 0;
    action->framePlayingRate = 100;
    action->value54 = 3.0F;
    const std::int32_t direction = parent->direction;
    action->displayPriority = 9;
    action->direction = direction;
    action->phase = 0;
    action->field270 = 1;
    action->battleAttachmentParent10 = parent;
    action->value1D4 = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->offset40);
    action->x = x + static_cast<std::int32_t>(gYobiScreenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->offset44);
    action->y = y + static_cast<std::int32_t>(gYobiScreenOffsetY);
    return action;
}

// 原函数：0x00422A30。
YobiAction* NewBattleGear_422A30(YobiAction* const parent, const std::int32_t imageNo)
{
    if (parent == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | 0x860040u;
    action->callback = UpdateBattleAttachment_421F90;
    action->flags = flags;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->framePlayingRate = 100;
    action->value54 = 3.0F;
    action->direction = parent->direction;
    action->displayPriority = 9;
    action->phase = 0;
    action->field270 = 2;
    action->battleAttachmentParent10 = parent;
    action->value1D4 = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->offset40);
    action->x = x + static_cast<std::int32_t>(gYobiScreenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->offset44);
    action->y = y + static_cast<std::int32_t>(gYobiScreenOffsetY);
    return action;
}

// 原函数：0x00422B00。
YobiAction* NewBattleAuxGear_422B00(
    YobiAction* const parent, const std::int32_t imageNo, const std::int16_t sortIndex)
{
    if (parent == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleAttachment_421F90;
    const std::uint32_t flags = parent->flags;
    action->imageNo = imageNo;
    action->flags = flags;
    action->characterAction = 0;
    action->framePlayingRate = 100;
    action->value54 = 3.0F;
    const std::int32_t direction = parent->direction;
    action->displayPriority = 9;
    action->direction = direction;
    const std::uint16_t field270 = static_cast<std::uint16_t>(sortIndex);
    action->phase = 0;
    action->battleAttachmentParent10 = parent;
    const std::int32_t position = static_cast<const BattleActorPayload*>(parent->payload.battleActor)->battlePosition18;
    action->field270 = field270;
    action->value1D4 = position;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    YobiAction* const source = action->battleAttachmentParent10;
    const std::int32_t x = static_cast<std::int32_t>(source->offset40);
    action->x = x + static_cast<std::int32_t>(gYobiScreenOffsetX);
    const std::int32_t y = static_cast<std::int32_t>(source->offset44);
    action->y = y + static_cast<std::int32_t>(gYobiScreenOffsetY);
    return action;
}

// 原函数：0x004222B0。
std::int32_t SetBattleRider_4222B0(
    YobiAction* const action, const std::int32_t imageNo, const std::int32_t ridingImageNo)
{
    auto* const payload = action->payload.battleActor;
    if (ridingImageNo > 0) {
        if (payload->rider13C8 == nullptr) {
            const std::int32_t mapped = world::detail::Sub_51C0D0(imageNo);
            YobiAction* const rider = NewBattleRider_4221F0(action, mapped);
            payload->rider13C8 = rider;
            rider->phase = 0;
            payload->rider13C8->field58 = 0;
            payload->rider13C8->characterAction = 0;
        }
        if (payload->rider13C8 != nullptr) {
            const std::int32_t mapped = world::detail::Sub_51C0D0(imageNo);
            payload->rider13C8->imageNo = mapped;
        }
        action->imageNo = ridingImageNo;
        return ridingImageNo;
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        rider->phase = 0;
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
    if (index == 1 && type >= 3 && type <= 5) {
        const std::int32_t firstType = payload->auxiliaryGearTypes13C0[0];
        if (firstType >= 3 && firstType <= 5) {
            static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[0]));
            payload->auxiliaryGear13B8[0] = nullptr;
        }
    }
    // 原两caller只传0/1；不模拟其他index把图号当地址的异常路径。
    YobiAction** const slot = &payload->auxiliaryGear13B8[index];
    payload->auxiliaryGearTypes13C0[index] = type;
    switch (type) {
    case 1: {
        if (*slot == nullptr) {
            *slot = NewBattleAuxGear_422B00(action, imageNo, -1);
        }
        YobiAction* const gear = *slot;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
        return;
    }
    case 2: {
        if (*slot == nullptr) {
            *slot = NewBattleAuxGear_422B00(action, imageNo, 3);
        }
        YobiAction* const gear = *slot;
        if (gear != nullptr) {
            gear->imageNo = imageNo;
        }
        return;
    }
    case 3:
    case 4:
    case 5: {
        if (*slot == nullptr) {
            YobiAction* const gear = NewBattleAuxGear_422B00(action, imageNo, 0);
            const bool isTypeThree = type == 3;
            *slot = gear;
            if (isTypeThree) {
                gear->scaleX = 0.25F;
                (*slot)->scaleY = 0.25F;
            } else if (type == 4) {
                gear->scaleX = 0.5F;
                (*slot)->scaleY = 0.5F;
            } else if (type == 5) {
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
    case 6:
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
        &payload->effect1054, &payload->attachedEffect1058, &payload->effect105C,
        &payload->centeredEffect1064, &payload->effect1068, &payload->rebirthEffect106C,
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
        SetYobiActionField308(*slot, 0);
        YobiAction* const batched = *slot;
        if (GetYobiActionField30F(batched) != 255) {
            SetYobiActionField308(batched, 8);
        }
        (*slot)->flags &= ~2u;
        YobiAction* const visible = *slot;
        if (GetYobiActionField30F(visible) == 0) {
            visible->flags |= 2u;
        }
    }
}

} // namespace detail

} // namespace cg::login
