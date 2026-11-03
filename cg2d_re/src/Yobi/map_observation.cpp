#include "map_observation.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "../platform/MapEffectMath.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldObjectState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::yobi {

namespace detail {

// 440920观察回复命令；不是角色资源动画81／82。
enum MapObservationReplyAction : std::int32_t {
    MapObservationReplyAction_DepartingGroup = 81,
    MapObservationReplyAction_ArrivingGroup = 82,
};


// 原43F330/43FEB0生产、43F520/440070消费；仅用于观察父组的phase。
enum ObservationGroupPhase : std::int32_t {
    ObservationGroupPhase_WaitingForBoarding = 1, // 等待成员登载完成。
    ObservationGroupPhase_SpiralDeparture = 2, // 起步后沿旋转上升轨迹离开。
    ObservationGroupPhase_ArrivalApproach = 3, // 抵达轨迹，超过原帧阈值后安排落地。
    ObservationGroupPhase_WaitingForLanding = 4, // 等待所有现有成员恢复地图对象。
    ObservationGroupPhase_LeavingAfterLanding = 5, // 成员落地后允许离屏清理。
};

// 原43F290/43FE00生产、43F910/440440消费；同值不等于父组phase。
enum ObservationMemberPhase : std::int32_t {
    ObservationMemberPhase_Boarding = 1, // 从地图对象位置登载到父组槽位。
    ObservationMemberPhase_Boarded = 2, // 已登载，保持父组槽位。
    ObservationMemberPhase_Carried = 3, // 抵达阶段随父组移动，等待落地。
    ObservationMemberPhase_Landing = 4, // 从父组槽位落回地图对象位置。
    ObservationMemberPhase_RestoredToMap = 5, // 已调用玩家／世界对象恢复函数。
};

// 原 payload +00..+2F；原生指针扩宽后只供三种地图角色及观察链使用。
struct MapCharaPayload {
    std::array<std::uint32_t, 5> objectIds00; // 原+00；观察成员对象编号，供FindObjectYobi查找，非表槽或地址。
    std::array<YobiAction*, 5> partyActions; // 原+14；与成员编号对应的非拥有动画节点引用。
    std::int32_t frame; // 原+28；观察组轨迹／入离场推进计数。
    YobiAction* next; // 原+2C；观察组链表的非拥有后继，节点仍由Yobi链管理。
};

static_assert(std::is_standard_layout_v<MapCharaPayload>);

static_assert(std::is_trivially_copyable_v<MapCharaPayload>);

static_assert(offsetof(MapCharaPayload, objectIds00) == 0);

static_assert(offsetof(MapCharaPayload, partyActions) == 0x18);

static_assert(offsetof(MapCharaPayload, frame) == 0x40);

static_assert(offsetof(MapCharaPayload, next) == 0x48);

static_assert(sizeof(MapCharaPayload) == 0x50);

struct MapPartyOffset {
    std::int32_t x;
    std::int32_t y;
};

// 原只读 [54D950,54DD10)：group/slot/direction 扁平索引；保留相邻项。
static constexpr std::array<MapPartyOffset, 120> kMapPartyOffsets54D950 {{ // 原常量参考：0x0054D950。
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {-41, 15}, {-44, 0}, {-19, -9}, {0, -15}, {28, -12}, {44, 6}, {33, 15}, {0, 20},
    {19, -9}, {44, 0}, {41, 15}, {0, 20}, {-33, 15}, {-44, 6}, {-28, -12}, {0, -15},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {29, 9}, {0, 24}, {-29, 9}, {-45, -9}, {-31, -21}, {0, -18}, {31, -21}, {45, -9},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {34, 15}, {0, 21}, {-34, 15}, {-47, 2}, {-33, -11}, {0, -17}, {33, -11}, {47, 2},
    {65, 27}, {0, 38}, {-64, 27}, {-97, 0}, {-64, -31}, {0, -40}, {64, -31}, {97, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

static_assert(sizeof(MapPartyOffset) == 8);

static_assert(offsetof(MapPartyOffset, y) == 4);

static_assert(sizeof(kMapPartyOffsets54D950) == 0x3C0);

YobiAction* NewMapCharaAction(session::ClientSession& session)
{
    return NewYobiAction_404AE0(session, 2, sizeof(MapCharaPayload), YobiPayloadKind::MapChara);
}

// 两种观察节点的真实回调，声明供以下生产入口绑定。
// 原函数：0x0043F520。
static void UpdateDepartingObservationGroup_43F520(YobiAction* action);

// 原函数：0x00440070。
static void UpdateArrivingObservationGroup_440070(YobiAction* action);

// 原函数：0x0043F330。
YobiAction* CreateDepartingObservationGroup_43F330(
    YobiAction* const source, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int32_t travelDuration)
{
    YobiAction* const action = NewMapCharaAction(*source->ownerSession);
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->callback = UpdateDepartingObservationGroup_43F520;
    action->characterAction = CharacterAnimation_Stand;
    action->direction = direction;
    action->field58 = direction;
    action->displayPriority = 10;
    action->flags = YobiActionFlag_PendingBatchFF;
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
    action->worldX = static_cast<float>(scaledX);
    action->worldY = static_cast<float>(mapY * 64.0);

    action->readiness = static_cast<std::int32_t>(travelDuration);
    action->phase = ObservationGroupPhase_WaitingForBoarding;
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    static_cast<void>(AppendObservationGroup_43F180(action));
    return action;
}

// 原函数：0x0043FEB0。
YobiAction* CreateArrivingObservationGroup_43FEB0(
    YobiAction* const source, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction)
{
    YobiAction* const action = NewMapCharaAction(*source->ownerSession);
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = UpdateArrivingObservationGroup_440070;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Run;
    action->displayPriority = 10;
    action->flags = YobiActionFlag_PendingBatchFF;
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
    action->phase = ObservationGroupPhase_ArrivalApproach;
    const float worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const float worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    static_cast<void>(AppendObservationGroup_43F180(action));
    return action;
}

// 原函数：0x004408C0。
void SetObservationGroupMembers_4408C0(
    YobiAction* const action, const std::int32_t first, const std::int32_t second,
    const std::int32_t third, const std::int32_t fourth, const std::int32_t fifth)
{
    auto* const payload = action->payload.mapChara;
    const std::array<std::int32_t, 5> values {first, second, third, fourth, fifth};
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] > 0) {
            payload->objectIds00[i] = static_cast<std::uint32_t>(values[i]);
        }
    }
}

// 原函数：0x0043F290。
static YobiAction* CreateBoardingObservationMember_43F290(YobiAction* const parent, const std::int32_t index)
{
    const std::uint32_t playerIndex = parent->ownerSession->WorldScene().player->objectIndex;
    const auto* const payload = static_cast<const MapCharaPayload*>(parent->payload.mapChara);
    const std::uint32_t objectIndex = payload->objectIds00[index];
    YobiAction* const source = objectIndex == playerIndex
        ? parent->ownerSession->WorldScene().player->action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 3, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = source->imageNo;
    action->characterAction = CharacterAnimation_Stand;
    const std::int32_t direction = source->direction;
    action->displayPriority = 10;
    action->direction = direction;
    action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
    action->x = source->x;
    const std::int32_t y = source->y;
    action->field1D0 = index;
    action->observationParent10 = parent;
    action->y = y;
    action->phase = ObservationMemberPhase_Boarding;
    return action;
}

// 原函数：0x0043FE00。
static YobiAction* CreateLandingObservationMember_43FE00(YobiAction* const parent, const std::int32_t index)
{
    const std::uint32_t playerIndex = parent->ownerSession->WorldScene().player->objectIndex;
    const auto* const payload = static_cast<const MapCharaPayload*>(parent->payload.mapChara);
    const std::uint32_t objectIndex = payload->objectIds00[index];
    YobiAction* const source = objectIndex == playerIndex
        ? parent->ownerSession->WorldScene().player->action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 3, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = source->imageNo;
    action->characterAction = CharacterAnimation_Stand;
    const std::int32_t direction = source->direction;
    action->displayPriority = 10;
    action->direction = direction;
    action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
    action->x = source->x;
    const std::int32_t y = source->y;
    action->value1D4 = 0;
    action->counter28 = 0;
    action->field1D0 = index;
    action->observationParent10 = parent;
    action->y = y;
    action->phase = ObservationMemberPhase_Carried;
    return action;
}

// 43F910/440440共用的原三类动作；不等同于实体骑乘复原的0/11分类。
static void SetObservationChildAction(
    YobiAction* const action, const std::int32_t imageNo)
{
    if ((imageNo >= 0x1BD50 && imageNo <= 0x1BDF9)
        || (imageNo >= 0x1B580 && imageNo <= 0x1B587)
        || (imageNo >= 0x1B58A && imageNo <= 0x1B59C)
        || (imageNo >= 0x1D554 && imageNo <= 0x1D55B)) {
        action->characterAction = CharacterAnimation_Run;
    } else if ((imageNo >= 0x186A0 && imageNo <= 0x18A87)
        || (imageNo >= 0x19A28 && imageNo <= 0x1A130)
        || (imageNo >= 0x197D0 && imageNo <= 0x19848)
        || (imageNo >= 0x1D524 && imageNo <= 0x1D553)
        || (imageNo >= 0x1AF0E && imageNo <= 0x1AF25)) {
        action->characterAction = CharacterAnimation_Sit;
    } else {
        action->characterAction = CharacterAnimation_Stand;
    }
}

[[maybe_unused]] static void SetObservationChildAction(YobiAction* const action)
{
    SetObservationChildAction(action, action->imageNo);
}

// 原函数：0x0043F910。
static void UpdateBoardingObservationMember_43F910(YobiAction* const action)
{
    const std::uint32_t playerIndex = action->ownerSession->WorldScene().player->objectIndex;
    YobiAction* const parent = action->observationParent10;
    const std::int32_t initialSlot = action->field1D0;
    auto* const payload = parent->payload.mapChara;
    const std::uint32_t objectIndex = payload->objectIds00[initialSlot];
    YobiAction* const source = objectIndex == playerIndex
        ? action->ownerSession->WorldScene().player->action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source != nullptr) {
        action->imageNo = world::ResolveRidingCharacterImage_51C0D0(source->imageNo);
        action->direction = parent->direction;
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    } else {
        action->graphicIndex = -1;
    }
    std::int32_t slotX = 0;
    std::int32_t slotY = 0;
    const std::int32_t slotDirection = parent->direction;
    const std::int32_t slot = action->field1D0;
    const std::int32_t parentImageNo = parent->imageNo;
    AddObservationMemberSlotOffset_43FD90(parentImageNo, slot, slotDirection, &slotX, &slotY);
    const std::int32_t parentY = parent->y;
    const std::int32_t initialPhase = action->phase;
    const std::int32_t parentX = parent->x;
    const std::int32_t frameY = parent->frameY;
    const std::int32_t frameX = parent->frameX;
    std::int32_t relativeX;
    std::int32_t relativeY;
    if (initialPhase == ObservationMemberPhase_Boarding) {
        const std::int32_t frame = action->value1D4;
        relativeX = frameX + slotX;
        if (frame < 15) {
            relativeX = frame * relativeX / 15;
        }
        relativeY = (frame - 15) * (frame - 15) - 225;
        if (frame == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1D1, action->x));
        }
        const std::int32_t currentFrame = action->value1D4;
        const std::int32_t targetY = slotY + frameY;
        if (currentFrame > 15 && targetY <= relativeY) {
            relativeY = targetY;
        }
        action->characterAction = CharacterAnimation_Sit;
        if (currentFrame > 15 && targetY <= relativeY) {
            const std::int32_t imageNo = action->imageNo;
            action->phase = ObservationMemberPhase_Boarded;
            action->value1D4 = 0;
            SetObservationChildAction(action, imageNo);
            static_cast<void>(audio::PlaySe_52C450(0x1CE, action->x));
        }
    } else if (initialPhase == ObservationMemberPhase_Boarded) {
        relativeX = frameX + slotX;
        const std::int32_t imageNo = action->imageNo;
        relativeY = frameY + slotY;
        SetObservationChildAction(action, imageNo);
    }
    const std::int32_t overrideAction = parent->counter1DC;
    if (overrideAction >= 0) {
        action->characterAction = overrideAction;
    }
    const std::int32_t index = action->field1D0;
    if (payload->objectIds00[index] != 0 && payload->partyActions[index] != nullptr) {
        std::int32_t shiftX = 0;
        std::int32_t shiftY = 0;
        if (action->phase == ObservationMemberPhase_Boarding) {
            const std::int32_t frame = action->value1D4;
            if (frame < 15) {
                const std::int32_t remaining = 15 - frame;
                shiftX = (action->x - parentX) * remaining / 15;
                shiftY = (action->y - parentY) * remaining / 15;
            }
        }
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
        action->deltaX = offsetX + shiftX + relativeX + parentX;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
        action->deltaY = offsetY + shiftY + relativeY + parentY;
    }
    ++action->value1D4;
}

// 原函数：0x00440440。
static void UpdateLandingObservationMember_440440(YobiAction* const action)
{
    YobiAction* const parent = action->observationParent10;
    const std::int32_t initialSlot = action->field1D0;
    auto* const payload = parent->payload.mapChara;
    const std::uint32_t objectIndex = payload->objectIds00[initialSlot];
    YobiAction* const source = objectIndex == action->ownerSession->WorldScene().player->objectIndex
        ? action->ownerSession->WorldScene().player->action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source != nullptr) {
        action->imageNo = world::ResolveRidingCharacterImage_51C0D0(source->imageNo);
        action->direction = parent->direction;
        if (action->phase == ObservationMemberPhase_Landing) {
            action->direction = source->direction;
        }
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    } else {
        action->graphicIndex = -1;
    }
    std::int32_t slotX = 0;
    std::int32_t slotY = 0;
    const std::int32_t slotDirection = parent->direction;
    const std::int32_t slot = action->field1D0;
    const std::int32_t parentImageNo = parent->imageNo;
    AddObservationMemberSlotOffset_43FD90(parentImageNo, slot, slotDirection, &slotX, &slotY);
    std::int32_t parentX = parent->x;
    const std::int32_t initialPhase = action->phase;
    std::int32_t parentY = parent->y;
    const std::int32_t frameX = parent->frameX;
    const std::int32_t frameY = parent->frameY;
    std::int32_t relativeX;
    std::int32_t relativeY;
    if (initialPhase == ObservationMemberPhase_Landing) {
        const float worldY = parent->worldY;
        const float worldX = parent->worldX;
        float screenX;
        float screenY;
        world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
        parentX = static_cast<std::int32_t>(
            static_cast<std::int64_t>(screenX + 0.5));
        parentY = static_cast<std::int32_t>(
            static_cast<std::int64_t>(screenY + 0.5));
        const std::int32_t travel = action->counter28;
        if (travel < 15) {
            relativeX = frameX + slotX;
            relativeX += travel * relativeX / -15;
        } else {
            relativeX = 0;
        }
        if (action->value1D4 == 0) {
            const std::int32_t targetY = frameY + slotY;
            for (;;) {
                const std::int32_t frame = action->value1D4;
                const std::int32_t arc = (frame - 15) * (frame - 15) - 225;
                if (targetY >= arc) {
                    break;
                }
                const std::int32_t nextFrame = frame + 1;
                action->value1D4 = nextFrame;
                if (nextFrame >= 15) {
                    break;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(0x1CF, action->x));
        }
        const std::int32_t frame = action->value1D4;
        relativeY = (frame - 15) * (frame - 15) - 225;
        if (frame > 15 && relativeY >= 0) {
            relativeY = 0;
        }
        action->characterAction = CharacterAnimation_Sit;
        if (frame > 15 && relativeY >= 0) {
            const std::int32_t soundX = action->x;
            action->phase = ObservationMemberPhase_RestoredToMap;
            action->characterAction = CharacterAnimation_Stand;
            static_cast<void>(audio::PlaySe_52C450(0x1C9, soundX));
            const std::uint32_t currentIndex = payload->objectIds00[action->field1D0];
            if (currentIndex == action->ownerSession->WorldScene().player->objectIndex) {
                world::ResumePlayerAfterObservation_51BEF0(*action->ownerSession);
            } else {
                world::ResumeWorldActorAfterObservation_43DB30(source);
            }
        }
    } else if (initialPhase == ObservationMemberPhase_Carried) {
        relativeY = frameY + slotY;
        const std::int32_t imageNo = action->imageNo;
        relativeX = frameX + slotX;
        SetObservationChildAction(action, imageNo);
    }
    const std::int32_t currentPhase = action->phase;
    if (currentPhase >= ObservationMemberPhase_RestoredToMap) {
        return;
    }
    const std::int32_t index = action->field1D0;
    if (payload->objectIds00[index] != 0 && payload->partyActions[index] != nullptr) {
        std::int32_t shiftX = 0;
        std::int32_t shiftY = 0;
        if (source != nullptr && currentPhase == ObservationMemberPhase_Landing) {
            const std::int32_t travel = action->counter28;
            shiftX = (source->x - parentX) * travel / 15;
            shiftY = (source->y - parentY) * travel / 15;
        }
        const std::int32_t drawDirection = parent->direction;
        const std::int32_t drawImageNo = parent->imageNo;
        const std::int32_t drawSlot = action->field1D0;
        AddObservationMemberSlotOffset_43FD90(drawImageNo, drawSlot, drawDirection, &slotX, &slotY);
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t drawY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y) + shiftY + relativeY + parentY;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
        const std::int32_t primaryIndex = parent->value1D4;
        const std::int32_t drawX = offsetX + shiftX + relativeX + parentX;
        const std::int32_t graphicIndex = action->graphicIndex;
        world::DrawGearNormal_46C060(
            primaryIndex, 1, graphicIndex,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), descriptor, 0);
    }
    if (action->phase == ObservationMemberPhase_Landing) {
        const std::int32_t nextFrame = action->value1D4 + 1;
        const std::int32_t nextTravel = action->counter28 + 1;
        action->value1D4 = nextFrame;
        action->counter28 = nextTravel;
    }
}

// 43F520/440070共有的五槽降序交换与挂接；不改写孩子坐标。
static void DrawObservationChildren(
    YobiAction* const parent, MapCharaPayload* const payload,
    std::array<std::int8_t, 5>& order)
{
    for (std::size_t index = 0; index < order.size(); ++index) {
        if (order[index] < 0) {
            break;
        }
        for (std::size_t next = index + 1; next < order.size(); ++next) {
            const std::int8_t nextIndex = order[next];
            if (nextIndex < 0) {
                break;
            }
            const std::int8_t currentIndex = order[index];
            YobiAction* const nextChild = payload->partyActions[nextIndex];
            YobiAction* const currentChild = payload->partyActions[currentIndex];
            const std::int32_t nextDepth = nextChild->deltaY;
            const std::int32_t currentDepth = currentChild->deltaY;
            if (currentDepth < nextDepth) {
                order[index] = nextIndex;
                order[next] = currentIndex;
            }
        }
    }
    for (const std::int8_t index : order) {
        if (index < 0) {
            break;
        }
        YobiAction* const child = payload->partyActions[index];
        const std::int32_t graphicIndex = child->graphicIndex;
        if (graphicIndex >= 0) {
            const render::GraphicDescriptor16* const descriptor = &child->drawMode;
            const std::int32_t childY = child->deltaY;
            const std::int32_t childX = child->deltaX;
            const std::int32_t primaryIndex = parent->value1D4;
            world::DrawGearNormal_46C060(
                primaryIndex, 1, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(childX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(childY)),
                descriptor, 0);
        }
    }
}

// 原函数：0x0043F520。
static void UpdateDepartingObservationGroup_43F520(YobiAction* const action)
{
    const float worldY = action->worldY;
    auto* const payload = action->payload.mapChara;
    const float worldX = action->worldX;
    constexpr std::array<std::int16_t, 8> angles {45, 0, 315, 270, 225, 180, 135, 90};
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    std::int32_t x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    std::int32_t y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    const std::int32_t initialPhase = action->phase;
    action->counter1DC = -1;
    if (initialPhase == ObservationGroupPhase_WaitingForBoarding) {
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    } else if (initialPhase == ObservationGroupPhase_SpiralDeparture) {
        const std::int32_t frame = payload->frame;
        if (frame < 0) {
            action->characterAction = CharacterAnimation_BeginRun;
            if (AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_StopAtEnd, 0) == AnimationFrameResult_StoppedOrUnavailable) {
                payload->frame = 0;
            }
            action->counter1DC = CharacterAnimation_Stand;
        } else {
            action->characterAction = CharacterAnimation_Run;
            action->counter1DC = CharacterAnimation_Run;
            y += static_cast<std::int32_t>(static_cast<std::int64_t>(
                static_cast<double>(frame)
                * -1.4F));
            const std::int32_t initialDirection = action->field58;
            const std::int32_t direction = initialDirection - ((frame + 8) % 120) / 15;
            action->direction = direction;
            if (direction < YobiDirection_UpLeft) {
                action->direction = direction + 8;
            }
            const std::int32_t angle = angles[initialDirection] + frame * 3;
            std::int64_t offset[2];
            platform::detail::Cg2dMapActorSpiralOffset(frame, angle, offset);
            x += static_cast<std::int32_t>(offset[0]);
            y += static_cast<std::int32_t>(offset[1]);
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
            if (FindObservationGroupByMember_43F1F0(*action->ownerSession, action->ownerSession->WorldScene().player->objectIndex) == nullptr
                && frame > 300 && (x < -300 || x > 940)) {
                UnlinkObservationGroup_43F1B0(action);
                for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
                    YobiAction* const child = payload->partyActions[index];
                    if (child != nullptr) {
                        MarkDeath(child);
                    }
                }
                MarkDeath(action);
            }
        }
    }
    action->x = x;
    action->y = y;
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t drawX = offsetX + action->x;
    const std::uint32_t worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::uint32_t worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const std::int32_t graphicIndex = action->graphicIndex;
    action->value1D4 = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    if (action->phase == ObservationGroupPhase_WaitingForBoarding) {
        std::int8_t ready = 0;
        for (std::int32_t index = 0; index < 5; ++index) {
            if (payload->objectIds00[index] != 0) {
                if (payload->partyActions[index] == nullptr) {
                    payload->partyActions[index] = CreateBoardingObservationMember_43F290(action, index);
                }
                YobiAction* const child = payload->partyActions[index];
                if (child == nullptr || child->phase != ObservationMemberPhase_Boarded) {
                    continue;
                }
            }
            ++ready;
        }
        if (payload->frame > 10 && ready >= 5) {
            action->phase = ObservationGroupPhase_SpiralDeparture;
            payload->frame = -1;
        }
    }
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        YobiAction* const child = payload->partyActions[index];
        if (child != nullptr) {
            UpdateBoardingObservationMember_43F910(child);
            order[count++] = static_cast<std::int8_t>(index);
        }
    }
    for (std::size_t index = count; index < order.size(); ++index) {
        order[index] = -1;
    }
    DrawObservationChildren(action, payload, order);
    const std::int32_t currentFrame = payload->frame;
    if (currentFrame >= 0) {
        payload->frame = currentFrame + 1;
    }
}

// 原函数：0x00440070。
static void UpdateArrivingObservationGroup_440070(YobiAction* const action)
{
    const float worldY = action->worldY;
    auto* const payload = action->payload.mapChara;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    std::int32_t x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    std::int32_t y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    const std::int32_t initialPhase = action->phase;
    if (initialPhase >= ObservationGroupPhase_ArrivalApproach && initialPhase <= ObservationGroupPhase_LeavingAfterLanding) {
        if (initialPhase == ObservationGroupPhase_ArrivalApproach && payload->frame > 50) {
            action->phase = ObservationGroupPhase_WaitingForLanding;
            for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
                YobiAction* const child = payload->partyActions[index];
                if (child != nullptr) {
                    child->phase = ObservationMemberPhase_Landing;
                }
            }
        }
        const std::int32_t direction = action->direction;
        const std::int32_t frame = payload->frame;
        std::int64_t offset[2];
        platform::detail::Cg2dMapObserverExitOffset(frame, direction, offset);
        x += static_cast<std::int32_t>(offset[0]);
        const std::int32_t elapsed = frame - 50;
        y += static_cast<std::int32_t>(offset[1]) - elapsed * elapsed / 30;
    }
    if (action->phase == ObservationGroupPhase_WaitingForLanding) {
        std::int32_t index = 0;
        for (; index < 5; ++index) {
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr && child->phase != ObservationMemberPhase_RestoredToMap) {
                break;
            }
        }
        if (index >= 5) {
            action->phase = ObservationGroupPhase_LeavingAfterLanding;
        }
    }
    if (action->phase == ObservationGroupPhase_LeavingAfterLanding && (x < -300 || x > 940 || y < -300 || y > 780)) {
        UnlinkObservationGroup_43F1B0(action);
        for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr) {
                MarkDeath(child);
            }
        }
        MarkDeath(action);
    }
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    action->x = x;
    action->y = y;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t drawX = offsetX + action->x;
    const std::uint32_t worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::uint32_t worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    action->value1D4 = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    for (std::int32_t index = 0; index < 5; ++index) {
        if (payload->objectIds00[index] != 0 && payload->partyActions[index] == nullptr) {
            payload->partyActions[index] = CreateLandingObservationMember_43FE00(action, index);
        }
    }
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        YobiAction* const child = payload->partyActions[index];
        if (child != nullptr) {
            UpdateLandingObservationMember_440440(child);
            order[count++] = static_cast<std::int8_t>(index);
        }
    }
    for (std::size_t index = count; index < order.size(); ++index) {
        order[index] = -1;
    }
    DrawObservationChildren(action, payload, order);
    ++payload->frame;
}

// 原函数：0x0043F180。
YobiAction* AppendObservationGroup_43F180(YobiAction* const action)
{
    auto& head = action->ownerSession->Yobi().mapCharaHead;
    YobiAction* current = head;
    if (current == nullptr) {
        head = action;
        return nullptr;
    }
    for (;;) {
        auto* const payload = current->payload.mapChara;
        YobiAction* const next = payload->next;
        if (next == nullptr) {
            payload->next = action;
            return action;
        }
        current = next;
    }
}

// 原函数：0x0043F1B0。
void* UnlinkObservationGroup_43F1B0(YobiAction* const action)
{
    auto& head = action->ownerSession->Yobi().mapCharaHead;
    YobiAction* current = head;
    if (current == action) {
        auto* const payload = action->payload.mapChara;
        head = payload->next;
        return payload;
    }
    while (current != nullptr) {
        auto* const payload = current->payload.mapChara;
        YobiAction* const next = payload->next;
        if (next == action) {
            YobiAction* const removedAction = payload->next;
            auto* const removed = removedAction->payload.mapChara;
            payload->next = removed->next;
            return removed;
        }
        current = next;
    }
    return nullptr;
}

// 原函数：0x0043FD90。
std::int32_t* AddObservationMemberSlotOffset_43FD90(
    const std::int32_t imageNo, const std::int32_t slot,
    const std::int32_t direction, std::int32_t* const x, std::int32_t* const y)
{
    std::int32_t group = 0;
    if (imageNo >= 110782 && imageNo <= 110787) {
        group = 1;
    } else if (imageNo >= 110788 && imageNo <= 110793) {
        group = 2;
    }
    const std::int32_t index = (group * 5 + slot) * 8 + direction;
    const std::int32_t oldX = *x;
    *x = oldX + kMapPartyOffsets54D950[index].x;
    const std::int32_t offsetY = kMapPartyOffsets54D950[index].y;
    *y += offsetY;
    return y;
}

// 原 440C50；唯一 callback 写者为 440B70，见 Yobi 观察回调 SPEC。
static void UpdateObservationTravelProjection_440C50(YobiAction* const action)
{
    auto* const payload = action->payload.mapChara;
    const std::int32_t initialFrame = payload->frame;
    const std::int32_t initialDirection = action->direction;
    platform::detail::MapObserverProjection projection;
    platform::detail::Cg2dMapObserverProjection(
        initialFrame, &action->ownerSession->Yobi().mapObservationDuration, initialDirection, &projection);
    AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0);
    action->x = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(projection.x)) + 320;
    const auto arc = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        platform::detail::Cg2dMapObserverArc(&projection.progress)));
    action->y = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(projection.y)) - arc + 290;

    std::array<std::int32_t, 5> drawX;
    std::array<std::int32_t, 5> drawY;
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
        if (payload->partyActions[index] == nullptr) {
            continue;
        }
        std::int32_t offsetX = 0;
        std::int32_t offsetY = 0;
        AdvanceYobiAnimationFrame_517880(payload->partyActions[index], 100, AnimationPlayback_Loop, 0);
        YobiAction* const child = payload->partyActions[index];
        const std::int32_t direction = child->direction;
        const std::int32_t slot = child->field1D0;
        AddObservationMemberSlotOffset_43FD90(action->imageNo, slot, direction, &offsetX, &offsetY);
        const std::int32_t graphicX = GetYobiGraphicOriginOffset_5182B0(payload->partyActions[index], YobiGraphicAxis_X);
        const std::int32_t frameX = action->frameX;
        const std::int32_t parentX = action->x;
        const std::int32_t frameOffsetX = graphicX + frameX;
        const std::int32_t slotOffsetX = offsetX;
        drawX[index] = frameOffsetX + parentX + slotOffsetX;
        const std::int32_t graphicY = GetYobiGraphicOriginOffset_5182B0(payload->partyActions[index], YobiGraphicAxis_Y);
        const std::int32_t frameY = action->frameY;
        const std::int32_t parentY = action->y;
        const std::int32_t frameOffsetY = graphicY + frameY;
        const std::int32_t slotOffsetY = offsetY;
        const std::int32_t y = frameOffsetY + parentY + slotOffsetY;
        order[count] = static_cast<std::int8_t>(index);
        drawY[index] = y;
        ++count;
    }
    for (std::size_t index = count; index < order.size(); ++index) {
        order[index] = -1;
    }

    // 原 440E41–440EB9 按子对象 +3C 降序换索引，不按 drawY 排序。
    for (std::size_t index = 0; index < order.size(); ++index) {
        if (order[index] < 0) {
            break;
        }
        for (std::size_t next = index + 1; next < order.size(); ++next) {
            const std::int8_t nextIndex = order[next];
            if (nextIndex < 0) {
                break;
            }
            const std::int8_t currentIndex = order[index];
            YobiAction* const currentChild = payload->partyActions[currentIndex];
            YobiAction* const nextChild = payload->partyActions[nextIndex];
            const std::int32_t currentDepth = currentChild->deltaY;
            if (currentDepth < nextChild->deltaY) {
                order[index] = nextIndex;
                order[next] = currentIndex;
            }
        }
    }
    for (const std::int8_t index : order) {
        if (index < 0) {
            break;
        }
        YobiAction* const child = payload->partyActions[index];
        const render::GraphicDescriptor16* const childMode = &child->drawMode;
        const std::int32_t childGraphicIndex = child->graphicIndex;
        const std::int32_t childY = drawY[index];
        const std::int32_t childX = drawX[index];
        render::DrawGraphic2_521DD0(
            childX, childY, 10, childGraphicIndex, 0, 0, childMode);
    }
    const std::int32_t graphicIndex = action->graphicIndex;
    const render::GraphicDescriptor16* const mode = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t y = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t x = offsetX + action->x;
    render::DrawGraphic2_521DD0(x, y, 10, graphicIndex, 0, 0, mode);
    ++payload->frame;
}

// 原函数：0x00440B70。
YobiAction* CreateObservationTravelFromSnapshot_440B70(session::ClientSession& session)
{
    auto& snapshot = session.Yobi();
    YobiAction* const action = NewMapCharaAction(session);
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = UpdateObservationTravelProjection_440C50;
    action->imageNo = snapshot.mapObservationImage;
    action->characterAction = CharacterAnimation_Run;
    const std::int32_t initialDirection = snapshot.mapObservationDirection;
    auto* const payload = action->payload.mapChara;
    action->direction = initialDirection;
    action->displayPriority = 10;
    action->flags = static_cast<std::int32_t>(YobiActionFlag_PendingBatchFF);
    action->x = -100;
    action->y = -100;
    static_cast<void>(AppendObservationGroup_43F180(action));
    for (std::size_t index = 0; index < payload->objectIds00.size(); ++index) {
        payload->objectIds00[index] = 0;
        payload->partyActions[index] = nullptr;
        const std::int32_t value = snapshot.mapObservationValues[index];
        if (value <= 0) {
            continue;
        }

        payload->objectIds00[index] = static_cast<std::uint32_t>(value);
        YobiAction* const child = NewYobiAction_404AE0(*action->ownerSession, 2, 0);
        payload->partyActions[index] = child;
        if (child != nullptr) {
            child->imageNo = snapshot.mapObservationImages[index];
            YobiAction* const directionChild = payload->partyActions[index];
            const std::int32_t direction = snapshot.mapObservationDirection;
            directionChild->direction = direction;
            YobiAction* const actionChild = payload->partyActions[index];
            actionChild->characterAction = CharacterAnimation_Run;
            YobiAction* const indexChild = payload->partyActions[index];
            indexChild->field1D0 = static_cast<std::int32_t>(index);
        }
    }
    return action;
}

} // namespace detail

// 原函数：0x0046DD40。
void ClearRegisteredMapActions_46DD40(session::ClientSession& session)
{
    auto& registry = session.Yobi();
    for (std::size_t row = 0; row < kMapRegistryRows; ++row) {
        const std::size_t rowOffset = row * kMapRegistryColumns;
        for (std::size_t column = 0;
             column < kMapRegistryColumns;
             ++column) {
            const std::size_t index = rowOffset + column;
            if (registry.mapRegistryFlags[index] == std::uint8_t{1}) {
                registry.mapRegistryFlags[index] = std::uint8_t{0};
                YobiAction* const action = registry.mapRegistryActions[index];
                MarkDeath(action);
            }
        }
    }
}

std::uint8_t& MapYobiRegistryFlagAt(session::ClientSession& session, const std::int32_t index) noexcept
{
    return session.Yobi().mapRegistryFlags.data()[index];
}

YobiAction*& MapYobiRegistryActionAt(session::ClientSession& session, const std::int32_t index) noexcept
{
    return session.Yobi().mapRegistryActions.data()[index];
}

void SetYobiMapOffsetX(YobiAction* const action, const std::int32_t value) noexcept
{
    action->mapOffsetX = value;
}

void SetYobiMapOffsetY(YobiAction* const action, const std::int32_t value) noexcept
{
    action->mapOffsetY = value;
}

// 原函数：0x00440920。
std::int32_t HandleMapObservationActionReply_440920(
    session::ClientSession& session,
    const std::int32_t objectIndex, const std::int32_t x, const std::int32_t y,
    [[maybe_unused]] const std::int32_t unusedDirection,
    const std::int32_t characterAction, const char* const row)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    if (characterAction < detail::MapObservationReplyAction_DepartingGroup || characterAction > detail::MapObservationReplyAction_ArrivingGroup) {
        return 0;
    }
    YobiAction* const target =
        static_cast<std::uint32_t>(objectIndex) == session.WorldScene().player->objectIndex
        ? session.WorldScene().player->action
        : world::FindObjectYobi_4462B0(objectIndex);
    if (target == nullptr) {
        return 1;
    }
    std::array<std::int32_t, 7> values;
    for (std::int32_t index = 0; index < 7; ++index) {
        values[index] = network::ParseDelimitedInteger_52CFA0(row, '|', index + 6);
    }
    if (characterAction == detail::MapObservationReplyAction_ArrivingGroup) {
        const std::int32_t imageNo = character_select::ResolveCharacterImageForVersion(values[0]);
        YobiAction* const observer = detail::CreateArrivingObservationGroup_43FEB0(target, imageNo, x, y, values[1]);
        detail::SetObservationGroupMembers_4408C0(observer, values[2], values[3], values[4], values[5], values[6]);
        for (std::size_t index = 2; index < values.size(); ++index) {
            const std::int32_t member = values[index];
            if (static_cast<std::uint32_t>(member) == session.WorldScene().player->objectIndex) {
                world::SuspendPlayerForObservation_51BEE0(session);
            } else {
                world::SuspendWorldActorForObservation_43DB00(world::FindObjectYobi_4462B0(member));
            }
        }
        return 1;
    }
    YobiAction* const source = world::FindObjectYobi_4462B0(values[0]);
    if (source != nullptr) {
        const std::int32_t travelDuration = values[1];
        const std::int32_t direction = source->direction;
        const std::int32_t sourceY =
            static_cast<std::int32_t>(source->mapY1F0);
        const std::int32_t sourceX =
            static_cast<std::int32_t>(source->mapX1E0);
        YobiAction* const observer = detail::CreateDepartingObservationGroup_43F330(
            source, source->imageNo, sourceX, sourceY, direction, travelDuration);
        detail::SetObservationGroupMembers_4408C0(observer, values[2], values[3], values[4], values[5], values[6]);
        for (std::size_t index = 2; index < values.size(); ++index) {
            const std::int32_t member = values[index];
            if (static_cast<std::uint32_t>(member) == session.WorldScene().player->objectIndex) {
                ui::WindowRuntime_CloseByCategory(session, 3);
                npcDialog.type = -1;
                world::SuspendPlayerForObservation_51BEE0(session);
                ui::WindowRuntime_ApplyObservationInputGate(session);
            } else {
                world::SuspendWorldActorForObservation_43DB00(world::FindObjectYobi_4462B0(member));
            }
        }
        source->imageNo = 0;
    }
    return 1;
}

// 原函数：0x0046DE50。
YobiAction* CreateRegisteredMapGraphicAction_46DE50(
    session::ClientSession& session,
    const std::int32_t imageNo, const std::int32_t mapX, const std::int32_t mapY)
{
    YobiAction* const action = NewYobiAction_404AE0(session, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->direction = YobiDirection_UpLeft;
    action->displayPriority = 10;
    action->flags = static_cast<std::int32_t>(YobiActionMask_HoverDescriptionOrPendingBatchFF);
    action->mapX200 = static_cast<std::int32_t>(mapX);
    action->mapY210 = static_cast<std::int32_t>(mapY);
    action->pathCountOrDrawOrder_270 = 0;
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
    const double scaledX = mapX * 64.0;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
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

// 原函数：0x0046DD90。
void UpdateRegisteredMapGraphicAction_46DD90(YobiAction* const action)
{
    const float sceneY = action->worldY;
    const float sceneX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(sceneX, sceneY, &screenX, &screenY);
    const auto x = static_cast<std::int64_t>(screenX + 0.5);
    const double y = screenY + 0.5;
    action->x = static_cast<std::int32_t>(x);
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(y));
    const auto worldX = static_cast<float>(
        (action->mapX1E0 * 64));
    const auto worldY = static_cast<float>(
        (action->mapY1F0 * 64));
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_Loop, 0));
    const std::int32_t offsetY = action->mapOffsetY;
    const std::int32_t offsetX = action->mapOffsetX;
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t drawX = offsetX + action->x;
    const std::int32_t imageNo = action->graphicIndex;
    world::CacheMapObjectGraphic_46DFF0(
        imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
        static_cast<std::int16_t>(drawX), static_cast<std::int16_t>(drawY), &action->drawMode);
}

// 原函数：0x0043F1F0。
YobiAction* FindObservationGroupByMember_43F1F0(
    session::ClientSession& session, const std::uint32_t value)
{
    YobiAction* current = session.Yobi().mapCharaHead;
    while (current != nullptr) {
        const auto* const payload =
            static_cast<const detail::MapCharaPayload*>(current->payload.mapChara);
        YobiAction* const next = payload->next;
        for (const std::uint32_t entry : payload->objectIds00) {
            if (entry == value) {
                return current;
            }
        }
        current = next;
    }
    return nullptr;
}

// 原函数：0x0043F220。
void ResetObservationGroupList_43F220(session::ClientSession& session)
{
    session.Yobi().mapCharaHead = nullptr;
}

// 原函数：0x0043F230。
void* ClearObservationGroups_43F230(session::ClientSession& session)
{
    YobiAction* current = session.Yobi().mapCharaHead;
    YobiAction* last = nullptr;
    while (current != nullptr) {
        const auto* const payload =
            static_cast<const detail::MapCharaPayload*>(current->payload.mapChara);
        YobiAction* const next = payload->next;
        for (YobiAction* const child : payload->partyActions) {
            if (child != nullptr) {
                MarkDeath(child);
            }
        }
        last = MarkDeath(current);
        current = next;
    }
    session.Yobi().mapCharaHead = nullptr;
    // 空链时原 EAX 未定义；新端返回空值，不作为原端等价结果验收。
    return last;
}

// 原函数：0x00440F50。
void* SavePlayerObservationTravelSnapshot_440F50(session::ClientSession& session)
{
    YobiAction* const action =
        FindObservationGroupByMember_43F1F0(session, session.WorldScene().player->objectIndex);
    if (action == nullptr) {
        return nullptr;
    }

    const std::int32_t imageNo = action->imageNo;
    auto* const payload = action->payload.mapChara;
    auto& snapshot = session.Yobi();
    snapshot.mapObservationImage = imageNo;
    snapshot.mapObservationDirection = action->field58;
    snapshot.mapObservationDuration =
        (action->readiness * 60);
    for (std::size_t index = 0; index < payload->objectIds00.size(); ++index) {
        snapshot.mapObservationValues[index] = 0;
        snapshot.mapObservationImages[index] = 0;
        const std::int32_t value =
            std::bit_cast<std::int32_t>(payload->objectIds00[index]);
        if (value > 0) {
            snapshot.mapObservationValues[index] = value;
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr) {
                snapshot.mapObservationImages[index] = child->imageNo;
            }
        }
    }
    return payload->partyActions.data();
}

std::int32_t ReadMapObservationDirection(session::ClientSession& session) noexcept
{
    return session.Yobi().mapObservationDirection;
}

// 原函数：0x00440FE0。
std::int32_t BeginObservationTravelScene_440FE0(session::ClientSession& session)
{
    static_cast<void>(detail::CreateObservationTravelFromSnapshot_440B70(session));
    static_cast<void>(cg::audio::BeginBgmFadeOut_52B990());
    return 0x25;
}

} // namespace cg::yobi
