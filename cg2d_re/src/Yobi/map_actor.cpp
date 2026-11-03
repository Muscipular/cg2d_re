#include "map_actor.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "map_transition.hpp"
#include "map_render.hpp"
#include "../world/WorldObjectState.hpp"
#include "../platform/MapEffectMath.hpp"
#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>

namespace cg::yobi {

namespace detail {

// 原441DB0/448040回调及实体绘制尾；完整定义位于本文件后段。
// 原函数：0x004417E0。
static void UpdateAndQueueMapActor_4417E0(YobiAction* action);

// 原43E535/43EA41等到格转向分支；每个CryptInt比较只读取一次。
static void ApplyPendingMapActorDirection(YobiAction* const action, const std::int32_t index)
{
    if (!TransitionMovementIdle(action)) {
        return;
    }
    auto& record = world::detail::gWorldObjects[static_cast<std::size_t>(index)];
    if (record.value34 != -1
        && (action->mapX1E0 == static_cast<std::int32_t>(record.value38))
        && (action->mapY1F0 == static_cast<std::int32_t>(record.value3C))) {
        action->direction = record.value34;
        record.value34 = -1;
        record.value38 = 0;
        record.value3C = 0;
    }
}

// 原43E115首次骑乘及43E790/43E8C3结束分类，不使用43EFC0的另一套动作映射。
static void RestoreMountedCharacterAction(YobiAction* const action)
{
    const std::int32_t imageNo = action->imageNo;
    if ((imageNo >= 0x1BD50 && imageNo <= 0x1BDF9)
        || (imageNo >= 0x1B580 && imageNo <= 0x1B587)
        || (imageNo >= 0x1B58A && imageNo <= 0x1B59C)
        || (imageNo >= 0x1D554 && imageNo <= 0x1D55B)) {
        action->characterAction = CharacterAnimation_Stand;
    } else if ((imageNo >= 0x186A0 && imageNo <= 0x18A87)
        || (imageNo >= 0x19A28 && imageNo <= 0x1A130)
        || (imageNo >= 0x197D0 && imageNo <= 0x19848)
        || (imageNo >= 0x1D524 && imageNo <= 0x1D553)
        || (imageNo >= 0x1D55C && imageNo <= 0x1D563)
        || (imageNo >= 0x1AF0E && imageNo <= 0x1AF25)) {
        action->characterAction = CharacterAnimation_Sit;
    } else {
        action->characterAction = CharacterAnimation_Stand;
    }
}

// 原43ECCF/43ED51：先采玩家指针，非当前玩家时才重读payload索引。
static void RestoreActorOriginalImage(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const player = world::gPlayerRuntimeSlots.action;
    if (player != nullptr && player == action) {
        action->imageNo = world::gPlayerRuntimeSlots.imageNo;
        return;
    }
    const std::int32_t index = payload->index;
    action->imageNo = index == -1 ? world::gPlayerRuntimeSlots.imageNo
        : std::bit_cast<std::int32_t>(
            world::detail::gWorldObjects[static_cast<std::size_t>(index)].imageNo);
}

// 原五条附件路径的槽取样时点不同，见 AAS01；只在本文件选择已证顺序。
enum class ActorAttachmentReadOrder {
    AfterYAndXAddition,
    BeforeYAndXAddition,
    BeforeYAddition,
    BeforeXAddition,
};

// 两次偏移查询使用各自当次槽；描述符始终借用初次取样的子对象。
static void DrawActorPhaseAttachment(
    YobiAction* const action, YobiAction** const slot,
    const std::int32_t primaryIndex, const std::int32_t before,
    const ActorAttachmentReadOrder readOrder)
{
    const bool readBeforeY = readOrder == ActorAttachmentReadOrder::BeforeYAndXAddition
        || readOrder == ActorAttachmentReadOrder::BeforeYAddition;
    const bool readBeforeX = readOrder == ActorAttachmentReadOrder::BeforeYAndXAddition
        || readOrder == ActorAttachmentReadOrder::BeforeXAddition;
    YobiAction* const child = *slot;
    const render::GraphicDescriptor16* const descriptor = &child->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(child, YobiGraphicAxis_Y);
    const std::int32_t parentY = action->y;
    YobiAction* nextChild = nullptr; // 下方互补分支在读取前必定覆盖，不改变取样时点。
    if (readBeforeY) {
        nextChild = *slot;
    }
    const std::int32_t drawY = offsetY + parentY;
    if (!readBeforeY) {
        nextChild = *slot;
    }
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(nextChild, YobiGraphicAxis_X);
    const std::int32_t parentX = action->x;
    YobiAction* graphicChild = nullptr; // 同上，保留X加法前后各路径的取样顺序。
    if (readBeforeX) {
        graphicChild = *slot;
    }
    const std::int32_t drawX = offsetX + parentX;
    if (!readBeforeX) {
        graphicChild = *slot;
    }
    const std::int32_t graphicIndex = graphicChild->graphicIndex;
    world::DrawGearNormal_46C060(
        primaryIndex, before, graphicIndex,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), descriptor, 0);
}

// 原两个阶段的transitionEffect08创建/播放；分配后的首写原本不判空，不增加失败门禁。
static void DrawActorPhaseEffect(
    YobiAction* const action, MapActorPayload* const payload,
    const std::int32_t primaryIndex, const std::int32_t imageNo,
    const ActorAttachmentReadOrder readOrder)
{
    if (payload->transitionEffect08 == nullptr) {
        YobiAction* const created = NewYobiAction_404AE0(*action->ownerSession, 2, 0);
        payload->transitionEffect08 = created;
        created->callback = nullptr;
        payload->transitionEffect08->imageNo = imageNo;
        payload->transitionEffect08->characterAction = CharacterAnimation_Stand;
        payload->transitionEffect08->displayPriority = 10;
        payload->transitionEffect08->flags = YobiActionMask_MapActorDefaults;
    }
    YobiAction* const child = payload->transitionEffect08;
    if (child != nullptr) {
        static_cast<void>(AdvanceYobiAnimationFrame_517880(child, 100, AnimationPlayback_Loop, 0));
        DrawActorPhaseAttachment(action, &payload->transitionEffect08, primaryIndex, 1, readOrder);
    }
}

// 原43E591–43EA01：阶段1的上升、骑乘动作恢复及效果附件清理。
static void UpdateActorMountPhase(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const ride = payload->rideActor44;
    if (ride == nullptr) {
        action->phase = MapActorPhase_Normal;
        RestoreMountedCharacterAction(action);
        return;
    }
    ride->direction = action->direction;
    YobiAction* const currentRide = payload->rideActor44;
    const std::int32_t counter = payload->transitionProgress48;
    const std::int32_t horizontal = currentRide->frameX * counter / 30;
    const std::int32_t arc = counter - 15;
    std::int32_t vertical = arc * arc - 225;
    if (counter > 15) {
        const std::int32_t frameY = payload->rideActor44->frameY;
        if (frameY <= vertical) {
            vertical = frameY;
        }
    }
    action->characterAction = CharacterAnimation_Sit;
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + action->y + vertical;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t x = action->x;
    const double sortY = static_cast<double>(action->worldY)
        + 0.001F;
    const std::int32_t drawX = offsetX + x + horizontal;
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::int32_t graphicIndex = action->graphicIndex;
    const auto worldYBits = std::bit_cast<std::uint32_t>(static_cast<float>(sortY));
    const std::int32_t primaryIndex = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    static_cast<void>(AdvanceYobiAnimationFrame_517880(payload->rideActor44, 100, AnimationPlayback_Loop, 0));
    DrawActorPhaseAttachment(action, &payload->rideActor44, primaryIndex, 0,
        ActorAttachmentReadOrder::AfterYAndXAddition);
    DrawActorPhaseEffect(action, payload, primaryIndex, 0x1AE10,
        ActorAttachmentReadOrder::BeforeYAndXAddition);
    if (payload->transitionProgress48 == 0) {
        static_cast<void>(audio::PlaySe_52C450(0x1D1, action->x));
    }
    const std::int32_t nextCounter = payload->transitionProgress48 + 1;
    payload->transitionProgress48 = nextCounter;
    if (nextCounter > 15 && payload->rideActor44->frameY <= vertical) {
        action->phase = MapActorPhase_Normal;
        RestoreMountedCharacterAction(action);
        static_cast<void>(audio::PlaySe_52C450(0x1CE, action->x));
        YobiAction* const effect = payload->transitionEffect08;
        if (effect != nullptr) {
            MarkDeath(effect);
            payload->transitionEffect08 = nullptr;
        }
    }
}

// 原43EAA2–43EDA2：阶段2的下降、骑乘附件退出及原图号恢复。
static void UpdateActorDismountPhase(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const ride = payload->rideActor44;
    if (ride == nullptr) {
        action->phase = MapActorPhase_Normal;
        action->characterAction = CharacterAnimation_Stand;
        MarkDeath(payload->rideActor44);
        payload->rideActor44 = nullptr;
        RestoreActorOriginalImage(action, payload);
        return;
    }
    ride->direction = action->direction;
    YobiAction* const currentRide = payload->rideActor44;
    const std::int32_t initialCounter = payload->transitionProgress48;
    const std::int32_t frameX = currentRide->frameX;
    const std::int32_t horizontal = frameX - frameX * initialCounter / 30;
    if (initialCounter == 0) {
        for (;;) {
            const std::int32_t counter = payload->transitionProgress48;
            const std::int32_t arc = counter - 15;
            const std::int32_t candidate = arc * arc - 225;
            if (payload->rideActor44->frameY >= candidate) {
                break;
            }
            const std::int32_t nextCounter = counter + 1;
            payload->transitionProgress48 = nextCounter;
            if (nextCounter >= 15) {
                break;
            }
        }
    }
    const std::int32_t counter = payload->transitionProgress48;
    const std::int32_t arc = counter - 15;
    std::int32_t vertical = arc * arc - 225;
    if (counter > 15 && vertical >= 0) {
        vertical = 0;
    }
    action->characterAction = CharacterAnimation_Sit;
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + action->y + vertical;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t x = action->x;
    const double sortY = static_cast<double>(action->worldY)
        + 0.001F;
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + x + horizontal;
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const auto worldYBits = std::bit_cast<std::uint32_t>(static_cast<float>(sortY));
    const std::int32_t primaryIndex = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    static_cast<void>(AdvanceYobiAnimationFrame_517880(payload->rideActor44, 100, AnimationPlayback_Loop, 0));
    DrawActorPhaseAttachment(action, &payload->rideActor44, primaryIndex, 0,
        ActorAttachmentReadOrder::BeforeYAddition);
    DrawActorPhaseEffect(action, payload, primaryIndex, 0x1AE11,
        ActorAttachmentReadOrder::BeforeYAddition);
    if (payload->transitionProgress48 == 0) {
        static_cast<void>(audio::PlaySe_52C450(0x1CF, action->x));
    }
    const std::int32_t nextCounter = payload->transitionProgress48 + 1;
    payload->transitionProgress48 = nextCounter;
    if (nextCounter > 15 && vertical >= 0) {
        action->phase = MapActorPhase_Normal;
        action->characterAction = CharacterAnimation_Stand;
        MarkDeath(payload->rideActor44);
        payload->rideActor44 = nullptr;
        RestoreActorOriginalImage(action, payload);
        static_cast<void>(audio::PlaySe_52C450(0x1C9, action->x));
        YobiAction* const effect = payload->transitionEffect08;
        if (effect != nullptr) {
            MarkDeath(effect);
            payload->transitionEffect08 = nullptr;
        }
    }
}

// 原43EDA2–43EF98：阶段3局部螺旋坐标与两次动画调用，不改世界坐标。
static void UpdateActorSpiralPhase(YobiAction* const action, MapActorPayload* const payload)
{
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    constexpr std::array<std::int16_t, 8> angles {45, 0, 315, 270, 225, 180, 135, 90};
    float projectedX;
    float projectedY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &projectedX, &projectedY);
    std::int32_t screenX = static_cast<std::int32_t>(
        static_cast<std::int64_t>(projectedX + 0.5));
    std::int32_t screenY = static_cast<std::int32_t>(
        static_cast<std::int64_t>(projectedY + 0.5));
    const std::int32_t counter = payload->transitionProgress48;
    if (counter <= 360) {
        action->characterAction = CharacterAnimation_Run;
        const std::int32_t progress = 360 - counter;
        screenY += static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(progress)
            * -1.4F));
        if (payload->transitionProgress48 == 0) {
            action->field58 = action->direction;
        }
        const std::int32_t savedDirection = action->field58;
        action->direction = savedDirection - ((progress + 8) % 120) / 15;
        if (action->direction < YobiDirection_UpLeft) {
            action->direction += 8;
        }
        action->direction %= 8;
        const std::int32_t angle =
            angles[static_cast<std::size_t>((savedDirection + 4) % 8)] + progress * 3;
        std::int64_t offsets[2];
        platform::detail::Cg2dMapActorSpiralOffset(progress, angle, offsets);
        screenX += static_cast<std::int32_t>(offsets[0]);
        screenY += static_cast<std::int32_t>(offsets[1]);
    } else {
        action->characterAction = CharacterAnimation_EndRun;
        if (AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_StopAtEnd, 0) == AnimationFrameResult_StoppedOrUnavailable) {
            action->phase = MapActorPhase_Normal;
            payload->transitionProgress48 = 0;
            action->characterAction = CharacterAnimation_Stand;
        }
    }
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + screenY;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + screenX;
    const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    static_cast<void>(world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor));
    ++payload->transitionProgress48;
}

// 原函数：0x0043DD00。
std::int32_t ParseMapActorAttachments_43DD00(
    YobiAction* const action, const char* const text, std::int32_t field)
{
    MapActorPayload* const payload =
        action != nullptr ? action->payload.mapActor : nullptr;
    char buffer[2048];
    network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
    ++field;
    const auto flags = static_cast<std::uint8_t>(
        character_select::ResolveCharacterImageForVersion(std::atoi(buffer)));
    if (payload != nullptr) {
        if (payload->mainGear30 != nullptr) {
            MarkDeath(payload->mainGear30);
            payload->mainGear30 = nullptr;
        }
        if (payload->firstAuxiliaryGear34 != nullptr) {
            MarkDeath(payload->firstAuxiliaryGear34);
            payload->firstAuxiliaryGear34 = nullptr;
        }
        if (payload->secondAuxiliaryGear38 != nullptr) {
            MarkDeath(payload->secondAuxiliaryGear38);
            payload->secondAuxiliaryGear38 = nullptr;
        }
    }

    if ((flags & MapAttachmentInputMask_GearList) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        std::int32_t index = 0;
        while (std::strcmp(buffer, "EF") != 0) {
            const std::int32_t kind =
                character_select::ResolveCharacterImageForVersion(std::atoi(buffer));
            if (kind == MapGearKind_MainGear) {
                network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
                ++field;
                const std::int32_t imageNo =
                    character_select::ResolveCharacterImageForVersion(std::atoi(buffer));
                if (payload != nullptr) {
                    payload->mainGear30 = NewYobiGear_4410F0(action, imageNo);
                }
            } else if ((flags & MapAttachmentInputFlag_AuxiliaryGearEntries) != 0) {
                network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
                ++field;
                const std::int32_t imageNo =
                    character_select::ResolveCharacterImageForVersion(std::atoi(buffer));
                if (payload != nullptr) {
                    if (index == 0) {
                        const std::int32_t y =
                            static_cast<std::int32_t>(action->mapY1F0);
                        const std::int32_t x =
                            static_cast<std::int32_t>(action->mapX1E0);
                        payload->firstAuxiliaryGear34 = NewYobiGearPet_441130(action, imageNo, x, y);
                        payload->firstAuxiliaryGearKind3C = kind;
                    } else if (index == 1) {
                        if (kind >= MapGearKind_DirectionalBatch10 && kind <= MapGearKind_DirectionalBatch12
                            && payload->firstAuxiliaryGearKind3C >= MapGearKind_DirectionalBatch10 && payload->firstAuxiliaryGearKind3C <= MapGearKind_DirectionalBatch12) {
                            MarkDeath(payload->firstAuxiliaryGear34);
                            payload->firstAuxiliaryGear34 = nullptr;
                        }
                        const std::int32_t y =
                            static_cast<std::int32_t>(action->mapY1F0);
                        const std::int32_t x =
                            static_cast<std::int32_t>(action->mapX1E0);
                        payload->secondAuxiliaryGear38 = NewYobiGearPet_441130(action, imageNo, x, y);
                        payload->secondAuxiliaryGearKind40 = kind;
                    }
                }
            }
            network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
            ++field;
            ++index;
        }
    }

    if ((flags & MapAttachmentInputFlag_RidingActor) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t imageNo =
            character_select::ResolveCharacterImageForVersion(std::atoi(buffer));
        if (payload != nullptr) {
            const auto mapMainImage = [&]() {
                YobiAction* const player = world::gPlayerRuntimeSlots.action;
                const std::int32_t original =
                    (player != nullptr && player == action) || payload->index == -1
                    ? world::gPlayerRuntimeSlots.imageNo
                    : std::bit_cast<std::int32_t>(
                        world::detail::gWorldObjects[payload->index].imageNo);
                action->imageNo = world::ResolveRidingCharacterImage_51C0D0(original);
            };
            if (payload->rideActor44 != nullptr) {
                payload->rideActor44->imageNo = imageNo;
                mapMainImage();
            } else {
                YobiAction* const ride = NewYobiAction_404AE0(*action->ownerSession, 2, 0);
                payload->rideActor44 = ride;
                if (ride != nullptr) {
                    ride->callback = nullptr;
                    payload->rideActor44->imageNo = imageNo;
                    payload->rideActor44->characterAction = CharacterAnimation_Stand;
                    payload->rideActor44->displayPriority = 10;
                    payload->rideActor44->flags = YobiActionMask_MapActorDefaults;
                    payload->rideActor44->flags |= YobiActionFlag_PetOrRider;
                    mapMainImage();
                    RestoreMountedCharacterAction(action);
                }
            }
        }
    } else if (payload != nullptr && payload->rideActor44 != nullptr) {
        action->phase = MapActorPhase_Dismounting;
        payload->transitionProgress48 = 0;
    }

    if ((flags & MapAttachmentInputFlag_AttachedActor) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t imageNo =
            character_select::ResolveCharacterImageForVersion(std::atoi(buffer));
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t packedDirection = std::atoi(buffer);
        const std::int32_t direction = packedDirection / 10 - 1;
        const std::int32_t offsetDirection = packedDirection % 10 - 1;
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t level = std::atoi(buffer);
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t hoverTextColor = std::atoi(buffer); // 附属实体悬停文字色，完整S32传递，显示时消费低BYTE。
        char name[256];
        network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        ++field;
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (action == nullptr) {
            return field;
        }
        std::int32_t x = static_cast<std::int32_t>(action->mapX1E0);
        std::int32_t y = static_cast<std::int32_t>(action->mapY1F0);
        if (payload == nullptr) {
            return field;
        }
        if (payload->attachedActor2C != nullptr) {
            MarkDeath(payload->attachedActor2C);
            payload->attachedActor2C = nullptr;
        }
        if (offsetDirection != -1) {
            const std::int16_t dx = world::ReadDirectionDeltaX(
                static_cast<std::int16_t>(offsetDirection));
            const std::int16_t dy = world::ReadDirectionDeltaY(
                static_cast<std::int16_t>(offsetDirection));
            x += dx;
            y += dy;
        }
        YobiAction* const child = CreateMapActorAction_441DB0(*action->ownerSession, imageNo, x, y, direction);
        payload->attachedActor2C = child;
        if (child == nullptr) {
            return field;
        }
        auto* const childPayload = child->payload.mapActor;
        childPayload->index =
            world::gPlayerRuntimeSlots.action == action ? -1 : payload->index;
        payload->attachedActor2C->flags |= YobiActionMask_NamedCharacterOrFollowPetOverlay;
        payload->attachedActor2C->nameC4 = cg::text::FromGbkWide(name);
        payload->attachedActor2C->field1D0 = level;
        payload->attachedActor2C->hoverTextColor1D8 = hoverTextColor;
    } else if (payload != nullptr && payload->attachedActor2C != nullptr) {
        MarkDeath(payload->attachedActor2C);
        payload->attachedActor2C = nullptr;
    }
    return field;
}

// 原函数：0x0043E490。
std::int32_t UpdateMapActorTransitionPhase_43E490(YobiAction* const action)
{
    const float worldY = action->worldY;
    auto* const payload = action->payload.mapActor;
    const float worldX = action->worldX;
    const std::int32_t index = payload->index;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    switch (action->phase) {
    case MapActorPhase_Normal:
        return 0;
    case MapActorPhase_Mounting:
        UpdateMapActorPathMovement_4424B0(action);
        ApplyPendingMapActorDirection(action, index);
        UpdateActorMountPhase(action, payload);
        return 1;
    case MapActorPhase_Dismounting:
        UpdateMapActorPathMovement_4424B0(action);
        ApplyPendingMapActorDirection(action, index);
        UpdateActorDismountPhase(action, payload);
        return 1;
    case MapActorPhase_Spiral:
        UpdateActorSpiralPhase(action, payload);
        return 1;
    default:
        action->phase = MapActorPhase_Normal;
        return 0;
    }
}

// 原函数：0x004417E0。
static void UpdateAndQueueMapActor_4417E0(YobiAction* const action)
{
    auto* payload = action->payload.mapActor;
    std::int32_t index = payload->index;
    std::int32_t primaryIndex = -1;
    bool useSharedRate = true;
    if (world::IsObservationSceneTransitionActive_440B50() != 0) {
        return;
    }
    YobiAction* player = world::gPlayerRuntimeSlots.action;
    if ((player != nullptr && player == action) || index == -1) {
        if ((world::ReadPlayerActionFlagsByte() & MapActorActionFlag_SuspendUpdate) != 0) {
            return;
        }
    } else if ((reinterpret_cast<const unsigned char*>(
                    &world::detail::gWorldObjects[
                        static_cast<std::size_t>(index)].flags120)[0] & MapActorActionFlag_SuspendUpdate) != 0) {
        return;
    }
    if (UpdateMapActorTransitionPhase_43E490(action) != 0) {
        return;
    }
    if (world::gPlayerRuntimeSlots.action != action) {
        payload = action->payload.mapActor;
        index = payload->index;
        if ((reinterpret_cast<const unsigned char*>(
                    &world::detail::gWorldObjects[
                        static_cast<std::size_t>(index)].flags42)[1] & MapActorDisplayByte1Flag_SharedMovementAndBars) == 0) {
            UpdateMapActorPathMovement_4424B0(action);
            useSharedRate = false;
        } else {
            useSharedRate = true;
        }
        ApplyPendingMapActorDirection(action, index);
    }
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    player = world::gPlayerRuntimeSlots.action;
    std::int32_t stopAtEnd;
    if (player != action
        && (world::detail::gWorldObjects[
                static_cast<std::size_t>(index)].category & MapActorCategoryFlag_LoopEmoteAnimations) != 0) {
        const std::int32_t characterAction = action->characterAction;
        stopAtEnd = characterAction == CharacterAnimation_Run || characterAction == CharacterAnimation_Stand
            || characterAction == CharacterAnimation_Walk || characterAction == CharacterAnimation_Scissors || characterAction == CharacterAnimation_Paper
            || characterAction == CharacterAnimation_Joy || characterAction == CharacterAnimation_Sad
            || characterAction == CharacterAnimation_Sit ? AnimationPlayback_Loop : AnimationPlayback_StopAtEnd;
    } else {
        YobiAction* const ride = payload->rideActor44;
        const std::int32_t characterAction =
            ride != nullptr ? ride->characterAction : action->characterAction;
        stopAtEnd = characterAction == CharacterAnimation_Wave || characterAction == CharacterAnimation_Joy
            || characterAction == CharacterAnimation_Angry || characterAction == CharacterAnimation_Sad || characterAction == CharacterAnimation_Walk
            || characterAction == CharacterAnimation_Stand || characterAction == CharacterAnimation_Nod || characterAction == CharacterAnimation_Run
            || characterAction == CharacterAnimation_Cast ? AnimationPlayback_Loop : AnimationPlayback_StopAtEnd;
    }
    std::uint32_t flipStatic = 0;
    if (world::gMapResourceMode_9DAD68 == 1) {
        payload = action->payload.mapActor;
        index = payload->index;
        if (player != action) {
            const std::uint32_t flags = world::detail::gWorldObjects[
                static_cast<std::size_t>(index)].flags114;
            if ((flags & MapObjectRenderFlag_NameOnlyCategory) != 0 && (flags & MapObjectRenderMask_StaticFlipExempt) == 0) {
                flipStatic = 1;
            }
        }
    }
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, flipStatic));
    if (useSharedRate) {
        if (std::bit_cast<float>(world::gPartyCatchUpRateBits_9DAD78) >= 1.2F) {
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, flipStatic));
        }
        if (std::bit_cast<float>(world::gPartyCatchUpRateBits_9DAD78) >= 1.6F) {
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, flipStatic));
        }
    } else {
        if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) >= 2) {
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, flipStatic));
        }
        if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) >= 4) {
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, flipStatic));
        }
    }
    const std::int32_t imageNo = action->imageNo;
    if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
        action->drawMode.mixFlags |= cg::render::GraphicMixFlag_OriginalPalette;
    } else {
        action->drawMode.mixFlags &= ~cg::render::GraphicMixFlag_OriginalPalette;
    }
    YobiAction* const ride = payload->rideActor44;
    if (ride != nullptr) {
        ride->direction = action->direction;
        static_cast<void>(AdvanceYobiAnimationFrame_517880(payload->rideActor44, 100, stopAtEnd, 0));
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
        YobiAction* const rideY = payload->rideActor44;
        const std::int32_t y = action->y;
        const std::int32_t drawY = offsetY + rideY->frameY + y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
        YobiAction* const rideX = payload->rideActor44;
        const std::int32_t x = action->x;
        const std::int32_t frameX = rideX->frameX;
        const std::int32_t graphicIndex = action->graphicIndex;
        const std::int32_t drawX = offsetX + frameX + x;
        const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
        const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
        primaryIndex = world::CachePrimaryMapGraphic_46BD60(
            graphicIndex, worldXBits, worldYBits,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
        DrawActorPhaseAttachment(action, &payload->rideActor44, primaryIndex, 0,
            ActorAttachmentReadOrder::BeforeXAddition);
    } else if ((action->flags & YobiActionFlag_GroundItem) != 0) {
        if (world::gPlayerRuntimeSlots.action != action) {
            const std::int32_t lowerX =
                (world::gPlayerX - 16);
            bool outside = (action->mapX1E0 < static_cast<std::int32_t>(lowerX));
            if (!outside) {
                const std::int32_t upperX =
                    (world::gPlayerX + 16);
                outside = (upperX < action->mapX1E0);
            }
            if (!outside) {
                const std::int32_t lowerY =
                    (world::gPlayerY - 16);
                outside = (action->mapY1F0 < static_cast<std::int32_t>(lowerY));
            }
            if (!outside) {
                const std::int32_t upperY =
                    (world::gPlayerY + 16);
                outside = (upperY < action->mapY1F0);
            }
            if (outside) {
                world::RemoveWorldObject_4462E0(world::detail::gWorldObjects[
                    static_cast<std::size_t>(index)].objectIndex);
                return;
            }
        }
        const std::int32_t mapY = static_cast<std::int32_t>(action->mapY1F0);
        const std::int32_t mapX = static_cast<std::int32_t>(action->mapX1E0);
        const std::int32_t firstGraphicIndex = action->graphicIndex;
        if (world::RegisterMapActorDrawKey_446900(firstGraphicIndex, mapX, mapY) == 0) {
            const render::GraphicDescriptor16* const descriptor = &action->drawMode;
            const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
            const std::int32_t drawY = offsetY + action->y;
            const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
            const std::int32_t x = action->x;
            const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
            const std::int32_t drawX = offsetX + x;
            const std::int32_t graphicIndex = action->graphicIndex;
            const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
            primaryIndex = world::CachePrimaryMapGraphic_46BD60(
                graphicIndex, worldXBits, worldYBits,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 0, descriptor);
            action->flags &= ~YobiActionFlag_Hidden;
        } else {
            action->flags |= YobiActionFlag_Hidden;
        }
    } else {
        const std::int32_t mode = world::gPlayerRuntimeSlots.action == action ? 1 : 0;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
        const std::int32_t drawY = offsetY + action->y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
        const std::int32_t x = action->x;
        const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
        const std::int32_t drawX = offsetX + x;
        const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
        const std::int32_t graphicIndex = action->graphicIndex;
        primaryIndex = world::CachePrimaryMapGraphic_46BD60(
            graphicIndex, worldXBits, worldYBits,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), mode, descriptor);
    }
    DrawMapActorOverlaysAndAttachments_442F70(action, primaryIndex);
}

// 原函数：0x004424B0。
void UpdateMapActorPathMovement_4424B0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float firstTargetX = static_cast<float>(static_cast<std::int32_t>(action->mapX200) * 64.0);
    const double firstTargetY = static_cast<std::int32_t>(action->mapY210) * 64.0;
    const float firstWorldX = action->worldX;
    // 原 FCOMP/C3；Y在首轮不落F32，只有X比较通过才读取世界Y。
    if (!(firstTargetX < firstWorldX || firstTargetX > firstWorldX)) {
        const float firstWorldY = action->worldY;
        if (!(firstTargetY < firstWorldY || firstTargetY > firstWorldY)
            && std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) > 0) {
            const std::int32_t previousX =
                static_cast<std::int32_t>(action->mapX1E0);
            const std::int32_t previousY =
                static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t nextY = action->pathY248[0];
            const std::int32_t nextX = action->pathX220[0];
            const std::int32_t speed = action->walkSpeed;
            const float rate =
                static_cast<float>(StartMapActorMovement_442110(action, nextX, nextY, speed, 0.0F));
            PopMapActorPathPoint_442460(action);
            if ((action->flags & YobiActionFlag_SuppressAttachedPathFollow) == 0) {
                auto* const payload = action->payload.mapActor;
                YobiAction* const child = payload->attachedActor2C;
                if (child != nullptr) {
                    StartMapActorMovement_442110(child, previousX, previousY, child->walkSpeed, rate);
                }
            }
        }
    }

    const float targetX = static_cast<float>(static_cast<std::int32_t>(action->mapX200) * 64.0);
    const float targetY = static_cast<float>(static_cast<std::int32_t>(action->mapY210) * 64.0);
    const float movementX = std::bit_cast<float>(action->movementStepXBits_27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->movementStepYBits_280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->movementActive_272 != 0) {
            SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Stand, 0);
        }
        action->movementActive_272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->movementStepYBits_280);
        const float stepX = std::bit_cast<float>(action->movementStepXBits_27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::GetSquaredWorldDistance_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->movementStepXBits_27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->movementStepYBits_280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->movementStepXBits_27C = 0;
            action->movementStepYBits_280 = 0;
        }
        SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Walk, 0);
    }

    // 此入口先写count，不使用encoded优先的通用operator=(int32_t)。
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldX * 0.015625));
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldY * 0.015625));
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
}

} // namespace detail

// 原函数：0x00441DB0。
YobiAction* CreateMapActorAction_441DB0(
    session::ClientSession& session, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction)
{
    YobiAction* const action = NewYobiAction_404AE0(session, 2, sizeof(detail::MapActorPayload), YobiPayloadKind::MapActor);
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = detail::UpdateAndQueueMapActor_4417E0;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->displayPriority = 10;
    action->flags = static_cast<std::int32_t>(YobiActionMask_MapActorDefaults);

    // 本入口的 count/encoded/mask 写序与通用 同类operator= 不同。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX200, mapX);
    assignCoordinate(action->mapY210, mapY);
    action->pathCountOrDrawOrder_270 = 0;
    assignCoordinate(action->mapX1E0, mapX);
    assignCoordinate(action->mapY1F0, mapY);

    const double scaledX = mapX * 64.0;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
    action->phase = MapActorPhase_Normal;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    return action;
}

void SetYobiPlayerFlag(YobiAction* const action) noexcept
{
    action->flags |= YobiActionFlag_LocalPlayer;
}

void SetYobiWalkSpeed(YobiAction* const action, const std::int32_t speed) noexcept
{
    action->walkSpeed = speed;
}

YobiAction* ReadMapActorAttachment44(const YobiAction* const action) noexcept
{
    return static_cast<const detail::MapActorPayload*>(action->payload.mapActor)->rideActor44;
}

void BeginMapActorPhase2(YobiAction* const action)
{
    auto* const payload = action->payload.mapActor;
    action->phase = MapActorPhase_Dismounting;
    payload->transitionProgress48 = 0;
}

std::uint16_t ReadYobiDirectionLow16(const YobiAction* const action) noexcept
{
    return static_cast<std::uint16_t>(action->direction);
}

YobiAction* PublishPlayerActionIdentity(
    const std::wstring_view name, const std::array<std::wstring, 4>& titles,
    const std::int32_t originalNameLength, const std::int32_t level,
    const std::int32_t hoverTextColor)
{
    YobiAction* action = world::gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return nullptr;
    }
    if (originalNameLength <= 16) {
        action->nameC4 = name;
    }

    // 使用原始回复的独立文本快照，不借用上一轮写回的共享称号。
    cg::core::NullSub_509080();
    if (cg::text::ToGbk(titles[0]).size() <= 16) {
        action = world::gPlayerRuntimeSlots.action;
        action->titleE1 = titles[0];
    }
    cg::core::NullSub_509080();
    if (cg::text::ToGbk(titles[1]).size() <= 32) {
        action = world::gPlayerRuntimeSlots.action;
        action->text102 = titles[1];
    }
    cg::core::NullSub_509080();
    if (cg::text::ToGbk(titles[2]).size() <= 32) {
        action = world::gPlayerRuntimeSlots.action;
        action->text123 = titles[2];
    }
    cg::core::NullSub_509080();
    if (cg::text::ToGbk(titles[3]).size() <= 16) {
        action = world::gPlayerRuntimeSlots.action;
        action->text31C = titles[3];
    }
    action = world::gPlayerRuntimeSlots.action;
    action->field1D0 = level;
    action = world::gPlayerRuntimeSlots.action;
    action->hoverTextColor1D8 = hoverTextColor; // 原51A495完整写第四参数，悬停文字消费另按低BYTE取色。
    return action;
}

void CopyYobiPlayerHp(
    YobiAction* const action, const platform::LegacyCryptInt* const source)
{
    action->readiness = *source;
}

void CopyYobiPlayerMaxHp(
    YobiAction* const action, const platform::LegacyCryptInt* const source)
{
    action->maxHp = *source;
}

void CopyInitialPlayerActionIdentity(
    const std::wstring_view name, const std::wstring_view title, const std::int32_t* const level,
    const platform::LegacyCryptInt* const hp, const platform::LegacyCryptInt* const maxHp)
{
    YobiAction* action = world::gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return;
    }
    if (cg::text::ToGbk(name).size() <= 16) {
        action->nameC4 = name;
        action = world::gPlayerRuntimeSlots.action;
    }
    if (cg::text::ToGbk(title).size() <= 16) {
        action->titleE1 = title;
        action = world::gPlayerRuntimeSlots.action;
    }
    action->field1D0 = *level;
    action = world::gPlayerRuntimeSlots.action;
    action->readiness = *hp;
    action = world::gPlayerRuntimeSlots.action;
    action->maxHp = *maxHp;
}

void UpdateMapActorMode(
    YobiAction*& slot, const std::int32_t mode, const std::int32_t imageNo)
{
    YobiAction* current = slot;
    if (current->overheadMarkerImage2E8 != imageNo) {
        current->overheadMarkerRefresh2EC = 1;
        current = slot;
    }
    const bool explicitMode = mode == OverheadMarkerMode_CustomImage || mode == OverheadMarkerMode_CustomImageKeepMarkerFlag;
    current->overheadMarkerMode2E4 = explicitMode ? mode : OverheadMarkerMode_DefaultImage;
    slot->overheadMarkerImage2E8 = explicitMode ? imageNo : 0;
}

void ApplyMapActorReplyAttachments(YobiAction* const action, const char* const text)
{
    auto* const payload = action->payload.mapActor;
    char buffer[2048];
    network::ExtractDelimitedField_52CED0(text, '|', 17, 2047, buffer);
    if (std::strlen(buffer) != 0) {
        detail::ParseMapActorAttachments_43DD00(action, text, 17);
    } else if (payload->attachedActor2C != nullptr) {
        MarkDeath(payload->attachedActor2C);
        payload->attachedActor2C = nullptr;
    }
}

void ReadMapActorTarget(
    YobiAction* const action, YobiAction*& slot,
    std::int32_t* const x, std::int32_t* const y)
{
    *x = static_cast<std::int32_t>(action->mapX200);
    *y = static_cast<std::int32_t>(slot->mapY210);
}

void ReadMapActorTarget(
    YobiAction*& slot, std::int32_t* const x, std::int32_t* const y)
{
    YobiAction* const action = slot;
    ReadMapActorTarget(action, slot, x, y);
}

void ClearMapActorPathCount(YobiAction* const action) noexcept
{
    action->pathCountOrDrawOrder_270 = 0;
}

bool IsMapActorIdleAtTarget(YobiAction* const action)
{
    if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) > 0) {
        return false;
    }
    const double x = static_cast<std::int32_t>(action->mapX200) * 64.0;
    const float worldX = action->worldX;
    if (x < worldX || x > worldX) {
        return false;
    }
    const double y = static_cast<std::int32_t>(action->mapY210) * 64.0;
    const float worldY = action->worldY;
    return !(y < worldY || y > worldY);
}

bool AreMapActorTargetsAdjacent(YobiAction*& previous, YobiAction*& current)
{
    YobiAction* const currentSignX = current;
    YobiAction* const previousSignX = previous;
    const std::int32_t signX = (previousSignX->mapX200 - currentSignX->mapX200);
    const bool negativeX = signX < 0;
    YobiAction* const currentMagnitudeX = current;
    YobiAction* const previousMagnitudeX = previous;
    std::int32_t distanceX = (previousMagnitudeX->mapX200 - currentMagnitudeX->mapX200);
    if (negativeX) {
        distanceX = -distanceX;
    }
    if (distanceX >= 2) {
        return false;
    }
    YobiAction* const currentSignY = current;
    YobiAction* const previousSignY = previous;
    const std::int32_t signY = (previousSignY->mapY210 - currentSignY->mapY210);
    const bool negativeY = signY < 0;
    YobiAction* const currentMagnitudeY = current;
    YobiAction* const previousMagnitudeY = previous;
    std::int32_t distanceY = (previousMagnitudeY->mapY210 - currentMagnitudeY->mapY210);
    if (negativeY) {
        distanceY = -distanceY;
    }
    return distanceY < 2;
}

void ApplyPlayerAppearanceReply(
    const std::int32_t imageNo, const std::int32_t gearImage,
    const std::int32_t petImage, const std::int32_t kind,
    std::int32_t* const playerImageNo)
{
    auto* const payload =
        world::gPlayerRuntimeSlots.action->payload.mapActor;
    if (payload == nullptr) {
        return;
    }
    *playerImageNo = character_select::ResolveCharacterImageForVersion(imageNo);
    const std::int32_t mappedImage = character_select::ResolveCharacterImageForVersion(imageNo);
    world::gPlayerRuntimeSlots.action->imageNo = mappedImage;
    if (gearImage != 0) {
        const std::int32_t mappedGear = character_select::ResolveCharacterImageForVersion(gearImage);
        bool hasGear = payload->mainGear30 != nullptr;
        if (!hasGear) {
            YobiAction* const gear = NewYobiGear_4410F0(world::gPlayerRuntimeSlots.action, mappedGear);
            hasGear = gear != nullptr;
            payload->mainGear30 = gear;
        }
        if (hasGear) {
            const std::int32_t converted =
                character_select::ResolveCharacterImageForVersion(mappedGear);
            payload->mainGear30->imageNo = converted;
        }
    } else {
        YobiAction* const gear = payload->mainGear30;
        if (gear != nullptr) {
            MarkDeath(gear);
            payload->mainGear30 = nullptr;
        }
    }

    if (petImage != 0) {
        std::int32_t mappedPet = character_select::ResolveCharacterImageForVersion(petImage);
        bool hasPet = payload->firstAuxiliaryGear34 != nullptr;
        if (!hasPet) {
            const std::int32_t y = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.action->mapY1F0);
            const std::int32_t x = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.action->mapX1E0);
            YobiAction* const pet = NewYobiGearPet_441130(world::gPlayerRuntimeSlots.action, mappedPet, x, y);
            hasPet = pet != nullptr;
            payload->firstAuxiliaryGear34 = pet;
        }
        if (hasPet) {
            const std::int32_t converted =
                character_select::ResolveCharacterImageForVersion(mappedPet);
            payload->firstAuxiliaryGear34->imageNo = converted;
        }
        if (kind == MapGearKind_AdjacentImagePair) {
            YobiAction* const previousSecondPet = payload->secondAuxiliaryGear38;
            ++mappedPet;
            bool hasSecondPet = previousSecondPet != nullptr;
            if (!hasSecondPet) {
                const std::int32_t y = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.action->mapY1F0);
                const std::int32_t x = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.action->mapX1E0);
                YobiAction* const pet = NewYobiGearPet_441130(world::gPlayerRuntimeSlots.action, mappedPet, x, y);
                hasSecondPet = pet != nullptr;
                payload->secondAuxiliaryGear38 = pet;
            }
            if (hasSecondPet) {
                const std::int32_t converted =
                    character_select::ResolveCharacterImageForVersion(mappedPet);
                payload->secondAuxiliaryGear38->imageNo = converted;
            }
        }
    } else {
        YobiAction* const firstPet = payload->firstAuxiliaryGear34;
        if (firstPet != nullptr) {
            MarkDeath(firstPet);
            payload->firstAuxiliaryGear34 = nullptr;
        }
        YobiAction* const secondPet = payload->secondAuxiliaryGear38;
        if (secondPet != nullptr) {
            MarkDeath(secondPet);
            payload->secondAuxiliaryGear38 = nullptr;
        }
    }
    payload->firstAuxiliaryGearKind3C = kind;
}

void ApplyPlayerRideReply(
    const std::int32_t imageNo, const std::int32_t steps,
    std::int32_t* const remainingSteps)
{
    YobiAction* const action = world::gPlayerRuntimeSlots.action;
    if (imageNo >= 0) {
        auto* const payload = action->payload.mapActor;
        if (payload->rideActor44 == nullptr) {
            YobiAction* const ride = NewYobiAction_404AE0(*action->ownerSession, 2, 0);
            payload->rideActor44 = ride;
            ride->callback = nullptr;
            payload->rideActor44->characterAction = CharacterAnimation_Stand;
            payload->rideActor44->displayPriority = 10;
            payload->rideActor44->flags = YobiActionMask_MapActorDefaults;
            payload->rideActor44->flags |= YobiActionFlag_PetOrRider;
        }
        const std::int32_t mappedRide = character_select::ResolveCharacterImageForVersion(imageNo);
        payload->rideActor44->imageNo = mappedRide;
        const std::int32_t mappedPlayer =
            world::ResolveRidingCharacterImage_51C0D0(world::gPlayerRuntimeSlots.imageNo);
        world::gPlayerRuntimeSlots.action->imageNo = mappedPlayer;
        world::gPlayerRuntimeSlots.action->phase = MapActorPhase_Mounting;
        payload->transitionProgress48 = 0;
        *remainingSteps = steps;
        payload->rideActor44->characterAction = CharacterAnimation_Stand;
        if (world::gPlayerRuntimeSlots.action->characterAction == CharacterAnimation_Walk) {
            payload->rideActor44->characterAction = CharacterAnimation_Walk;
        }
        return;
    }
    if (action->phase == MapActorPhase_Dismounting) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    action->phase = MapActorPhase_Dismounting;
    YobiAction* const ride = payload->rideActor44;
    payload->transitionProgress48 = 0;
    if (ride != nullptr) {
        ride->characterAction = CharacterAnimation_Stand;
        if (world::gPlayerRuntimeSlots.action->characterAction == CharacterAnimation_Walk) {
            payload->rideActor44->characterAction = CharacterAnimation_Walk;
        }
    }
    *remainingSteps = 0;
}

void ApplyPlayerPhase3Reply(const std::int32_t imageNo)
{
    YobiAction* const action = world::gPlayerRuntimeSlots.action;
    auto* const payload = action->payload.mapActor;
    action->phase = MapActorPhase_Spiral;
    const std::int32_t mappedImage = character_select::ResolveCharacterImageForVersion(imageNo);
    world::gPlayerRuntimeSlots.action->imageNo = mappedImage;
    payload->transitionProgress48 = 0;
}

void ResetMapFollowerFromPath(
    YobiAction* const leader, const std::int32_t position, YobiAction*& slot,
    const bool isPlayer, std::int32_t* const playerDirection)
{
    if (isPlayer) {
        YobiAction* const player = world::gPlayerRuntimeSlots.action;
        if (player != nullptr) {
            player->pathCountOrDrawOrder_270 = 0;
        }
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->pathCountOrDrawOrder_270) - position;
        const std::int32_t y = leader->pathY248[index];
        const std::int32_t x = leader->pathX220[index];
        static_cast<void>(world::SetPlayerMapCoordinates_519EC0(x, y));
    } else {
        slot->pathCountOrDrawOrder_270 = 0;
        YobiAction* const follower = slot;
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->pathCountOrDrawOrder_270) - position;
        const std::int32_t y = leader->pathY248[index];
        const std::int32_t x = leader->pathX220[index];
        SnapMapActorToPosition_442AB0(follower, x, y);
    }
    if (!isPlayer || world::gPlayerRuntimeSlots.action != nullptr) {
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->pathCountOrDrawOrder_270) - position;
        const std::int32_t dy = leader->pathY248[index - 1] - leader->pathY248[index];
        const std::int32_t dx = leader->pathX220[index - 1] - leader->pathX220[index];
        const float y = static_cast<float>(dy);
        const float x = static_cast<float>(dx);
        const std::int32_t direction =
            static_cast<std::int32_t>(world::GetEightWayMapDirection_46CDF0(x, y));
        slot->direction = (direction + 4) % 8;
        if (isPlayer) {
            *playerDirection = slot->direction;
        }
    }
    if (isPlayer) {
        world::SynchronizePlayerMapActionPosition_519EE0();
        static_cast<void>(world::RestorePlayerMovementSnapshot_4681B0());
    }
}

// 原函数：0x00442AB0。
void SnapMapActorToPosition_442AB0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    // 本入口以mask开头，既不同于通用赋值，也不同于441DB0的count优先顺序。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX1E0, targetX);
    assignCoordinate(action->mapY1F0, targetY);
    assignCoordinate(action->mapX200, targetX);
    assignCoordinate(action->mapY210, targetY);
    const double worldX = targetX;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
    action->worldX = static_cast<float>(worldX * 64.0F);
    action->worldY = static_cast<float>(targetY * 64.0);
}

// 原函数：0x00441F60。
void AppendMapActorPathPoint_441F60(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    const std::uint16_t count = action->pathCountOrDrawOrder_270;
    if (count >= 10u) {
        action->pathCountOrDrawOrder_270 = 0;
        SnapMapActorToPosition_442AB0(action, targetX, targetY);
        return;
    }
    const auto xSlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(count));
    action->pathX220[xSlot] = targetX;
    const auto ySlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270));
    action->pathY248[ySlot] = targetY;
    action->pathCountOrDrawOrder_270 = static_cast<std::uint16_t>(action->pathCountOrDrawOrder_270 + 1u);
}

// 原函数：0x00441FC0。
void AppendInterpolatedMapActorPath_441FC0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    std::int32_t startX;
    std::int32_t startY;
    const std::int16_t count = std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270);
    if (count > 0) {
        const auto last = static_cast<std::size_t>(count - 1);
        startX = action->pathX220[last];
        startY = action->pathY248[last];
    } else {
        startX = static_cast<std::int32_t>(action->mapX200);
        startY = static_cast<std::int32_t>(action->mapY210);
    }
    const std::int32_t differenceX = targetX - startX;
    const std::int32_t differenceY = targetY - startY;
    const std::int32_t absoluteX = differenceX < 0 ? -differenceX : differenceX;
    const std::int32_t absoluteY = differenceY < 0 ? -differenceY : differenceY;
    std::array<std::int32_t, 2> pendingX;
    std::array<std::int32_t, 2> pendingY;
    std::int32_t pointCount = 0;
    if (absoluteX == 2 && absoluteY == 2) {
        pendingX[0] = startX + differenceX / 2;
        pendingY[0] = startY + differenceY / 2;
        pointCount = 1;
    } else if (absoluteX == 2) {
        pendingX[0] = startX + differenceX / 2;
        pendingY[0] = targetY;
        pointCount = 1;
    } else if (absoluteY == 2) {
        pendingX[0] = targetX;
        pendingY[0] = startY + differenceY / 2;
        pointCount = 1;
    }
    const std::int32_t currentCount =
        std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270);
    pendingX[static_cast<std::size_t>(pointCount)] = targetX;
    pendingY[static_cast<std::size_t>(pointCount)] = targetY;
    ++pointCount;
    if (static_cast<std::uint32_t>(currentCount + pointCount) > 10u) {
        const auto last = static_cast<std::size_t>(pointCount - 1);
        const std::int32_t finalY = pendingY[last];
        const std::int32_t finalX = pendingX[last];
        action->pathCountOrDrawOrder_270 = 0;
        SnapMapActorToPosition_442AB0(action, finalX, finalY);
        return;
    }
    for (std::int32_t point = 0; point < pointCount; ++point) {
        const auto xSlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270));
        const std::int32_t x = pendingX[static_cast<std::size_t>(point)];
        action->pathX220[xSlot] = x;
        const std::int32_t y = pendingY[static_cast<std::size_t>(point)];
        const auto ySlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270));
        action->pathY248[ySlot] = y;
        action->pathCountOrDrawOrder_270 = static_cast<std::uint16_t>(action->pathCountOrDrawOrder_270 + 1u);
    }
}

// 原函数：0x00442110。
double StartMapActorMovement_442110(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY, const std::int32_t speed,
    const float overrideRate)
{
    if (action == nullptr) {
        return 0.0;
    }
    std::int16_t pathCount = 0;
    if (!(overrideRate < 0.0F || overrideRate > 0.0F)) {
        pathCount = std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270);
    }
    const platform::detail::MapActorVelocityInput input {
        targetX, targetY, speed, pathCount, 0, overrideRate,
        action->worldX, action->worldY,
    };
    platform::detail::MapActorVelocity velocity;
    platform::detail::Cg2dMapActorVelocity(&input, &velocity);
    action->movementStepXBits_27C = std::bit_cast<std::uint32_t>(velocity.stepX);
    action->movementStepYBits_280 = std::bit_cast<std::uint32_t>(velocity.stepY);
    // 本入口先取齐encoded/mask/readCount，再按该顺序发布；不同于4422F0。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX200, targetX);
    assignCoordinate(action->mapY210, targetY);
    if (velocity.directionX < 0.0F || velocity.directionX > 0.0F
        || velocity.directionY < 0.0F || velocity.directionY > 0.0F) {
        action->direction = static_cast<std::int32_t>(
            world::GetEightWayMapDirection_46CDF0(velocity.directionX, velocity.directionY));
        action->movementActive_272 = 1;
    }
    return velocity.rate;
}

// 原函数：0x004422F0。
void StartPartyMemberMovement_4422F0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY, const std::int32_t speed)
{
    if (action == nullptr) {
        return;
    }
    const std::int32_t pixelX = targetX * 64;
    const std::int32_t pixelY = targetY * 64;
    const platform::detail::MapMoveDifference difference {
        static_cast<double>(pixelX) - action->worldX,
        static_cast<double>(pixelY) - action->worldY,
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    const double rate = speed;
    const float percent = 0.01F;
    action->movementStepXBits_27C = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.x * 4.0F * percent));
    action->movementStepYBits_280 = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.y * 4.0F * percent));

    action->mapX200 = static_cast<std::int32_t>(targetX);
    action->mapY210 = static_cast<std::int32_t>(targetY);
    if (direction.x < 0.0F || direction.x > 0.0F
        || direction.y < 0.0F || direction.y > 0.0F) {
        action->direction = static_cast<std::int32_t>(
            world::GetEightWayMapDirection_46CDF0(direction.x, direction.y));
        action->movementActive_272 = 1;
    }
}

// 原函数：0x00442460。
void PopMapActorPathPoint_442460(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const std::int16_t count = std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270);
    if (count > 0) {
        action->pathCountOrDrawOrder_270 = static_cast<std::uint16_t>(count - 1);
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270); ++index) {
        const auto slot = static_cast<std::size_t>(index);
        action->pathX220[slot] = action->pathX220[slot + 1];
        action->pathY248[slot] = action->pathY248[slot + 1];
    }
}

// 原函数：0x0043EFC0。
void SetMapActorAnimationState_43EFC0(
    YobiAction* const action, const std::int32_t characterAction,
    const std::int32_t restart)
{
    auto* const payload = action->payload.mapActor;
    YobiAction* const ride = payload->rideActor44;
    if (ride != nullptr) {
        ride->characterAction = characterAction;
        const std::int32_t imageNo = action->imageNo;
        if ((imageNo >= 114000 && imageNo <= 114169)
            || (imageNo >= 112000 && imageNo <= 112007)
            || (imageNo >= 112010 && imageNo <= 112028)
            || (imageNo >= 120148 && imageNo <= 120155)) {
            switch (payload->rideActor44->characterAction) {
            case CharacterAnimation_Stand: action->characterAction = CharacterAnimation_Stand; break;
            case CharacterAnimation_Walk: action->characterAction = CharacterAnimation_Walk; break;
            case CharacterAnimation_BeginRun:
            case CharacterAnimation_Run:
            case CharacterAnimation_EndRun: action->characterAction = CharacterAnimation_Run; break;
            case CharacterAnimation_Attack:
            case CharacterAnimation_Cast: action->characterAction = CharacterAnimation_Attack; break;
            case CharacterAnimation_Damage:
            case CharacterAnimation_Down: action->characterAction = CharacterAnimation_Damage; break;
            case CharacterAnimation_Guard: action->characterAction = CharacterAnimation_Guard; break;
            default:
                action->characterAction = CharacterAnimation_Stand;
                payload->rideActor44->characterAction = CharacterAnimation_Stand;
                break;
            }
        } else if ((imageNo >= 100000 && imageNo <= 100999)
            || (imageNo >= 105000 && imageNo <= 106800)
            || (imageNo >= 104400 && imageNo <= 104520)
            || (imageNo >= 120100 && imageNo <= 120147)
            || imageNo == 100900 || imageNo == 100901) {
            action->characterAction = CharacterAnimation_Sit;
        } else {
            action->characterAction = CharacterAnimation_Stand;
        }
    } else {
        action->characterAction = characterAction;
    }

    if (restart != 0) {
        action->lastCharacterAction = -1;
        YobiAction* attachment = payload->rideActor44;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->firstAuxiliaryGear34;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->secondAuxiliaryGear38;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
    }
}

// 原函数：0x00442730。
void StepMapActorMovement_442730(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(static_cast<std::int32_t>(action->mapX200) * 64.0);
    const float targetY = static_cast<float>(static_cast<std::int32_t>(action->mapY210) * 64.0);
    const float movementX = std::bit_cast<float>(action->movementStepXBits_27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->movementStepYBits_280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->movementActive_272 != 0) {
            SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Stand, 0);
        }
        action->movementActive_272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->movementStepYBits_280);
        const float stepX = std::bit_cast<float>(action->movementStepXBits_27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::GetSquaredWorldDistance_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->movementStepXBits_27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->movementStepYBits_280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->movementStepXBits_27C = 0;
            action->movementStepYBits_280 = 0;
        }
        SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Walk, 0);
    }

    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldX * 0.015625));
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldY * 0.015625));
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
}

// 原函数：0x004428F0。
void StepPartyMemberMovement_4428F0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(static_cast<std::int32_t>(action->mapX200) * 64.0);
    const float targetY = static_cast<float>(static_cast<std::int32_t>(action->mapY210) * 64.0);
    const float movementX = std::bit_cast<float>(action->movementStepXBits_27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->movementStepYBits_280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->movementActive_272 != 0) {
            SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Stand, 0);
        }
        action->movementActive_272 = 0;
    } else {
        const float rateX = std::bit_cast<float>(world::gPartyCatchUpRateBits_9DAD78);
        const float rawStepX = std::bit_cast<float>(action->movementStepXBits_27C);
        const double scaledX = static_cast<double>(rateX) * rawStepX;
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const float stepX = static_cast<float>(scaledX);
        const float rateY = std::bit_cast<float>(world::gPartyCatchUpRateBits_9DAD78);
        const float rawStepY = std::bit_cast<float>(action->movementStepYBits_280);
        const float stepY = static_cast<float>(static_cast<double>(rateY) * rawStepY);
        const double distance = world::GetSquaredWorldDistance_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX =
                static_cast<float>(static_cast<double>(stepX) + action->worldX);
            action->worldY =
                static_cast<float>(static_cast<double>(stepY) + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->movementStepXBits_27C = 0;
            action->movementStepYBits_280 = 0;
        }
        SetMapActorAnimationState_43EFC0(action, CharacterAnimation_Walk, 0);
    }

    // 倍率入口与442730不同：mask→encoded→count，不提前复制目标载荷。
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldX * 0.015625));
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldY * 0.015625));
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
}

// WLR01：玩家519D50、单项删除4462E0与整表4464D0共用八槽/text清理；正确正文保持。
void ClearMapActorAttachments(YobiAction* const action)
{
    auto* const payload = action->payload.mapActor;
    if (payload == nullptr) {
        return;
    }

    // 原清理不包含 +34/+38；只保存槽地址，每轮调用前才读取对象指针。
    YobiAction** const slots[] {
        &payload->overheadMarker04, &payload->transitionEffect08, &payload->statusIcon1AF4D_0C,
        &payload->statusIcon1AF4E_10, &payload->attachedActor2C, &payload->statusIcon4C,
        &payload->mainGear30, &payload->rideActor44,
    };
    for (YobiAction** const slot : slots) {
        YobiAction* const child = *slot;
        if (child != nullptr) {
            MarkDeath(child);
            *slot = nullptr;
        }
    }
    auto* const text = payload->text1C;
    if (text != nullptr) {
        delete text;
        payload->text1C = nullptr;
    }
}

} // namespace cg::yobi
