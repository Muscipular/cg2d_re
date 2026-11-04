#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/network/CgConnection.hpp"
#include "map_render.hpp"
#include "base.hpp"
#include "window.hpp"
#include "map_actor.hpp"
#include "battle_base.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../world/WorldObjectState.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <new>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <string_view>

namespace cg::yobi {

namespace detail {

// YMN16：悬停角色名单快捷键使用的Windows虚拟键码。
enum HoverShortcutKey : std::int32_t {
    HoverShortcutKey_Control = 17,
    HoverShortcutKey_Insert = 45,
    HoverShortcutKey_Delete = 46,
};

// YMN16：名单快捷键读取的平台键态；4为首次按下，区别于10h的重复按键脉冲。
enum HoverShortcutKeyStateFlag : std::uint32_t {
    HoverShortcutKeyStateFlag_Down = 0x2u,
    HoverShortcutKeyStateFlag_FirstPress = 0x4u,
};


// 地图头顶文本槽；仅按远端角色字段命名，未证业务名称保留。
enum ActorOverheadTextSlot : std::int16_t {
    ActorOverheadTextSlot_NameC4 = 1,
    ActorOverheadTextSlot_TitleE1 = 2,
    ActorOverheadTextSlot_Text144 = 3,
    ActorOverheadTextSlot_Text102 = 4,
    ActorOverheadTextSlot_Text123 = 5,
    ActorOverheadTextSlot_Text31C = 6,
};


// 原[554204,5542A4)的40项S32初值；4417A0按每25个图号一组读取。
static std::array<std::int32_t, 40> gActorHeightOffsets554204 { // 原全局：0x00554204。
    -78, -88, -88, -81, -88, -90, -99, -88, -88, -88,
    -79, -86, -88, -85, -88, -88, -96, -78, -88, -86,
    -88, -88, -81, -88, -90, -99, -79, -85, -88, -88,
    -96, -99, -88, -88, -88, -88, -88, -88, -88, -88,
};

static_assert(sizeof(gActorHeightOffsets554204) == 0xA0);

// 原5542F4/554338，分别为17项状态图号表和聊天泡开关。
static std::array<std::int32_t, 17> gActorStatusImages5542F4 { // 原全局：0x005542F4。
    0, 0x192BC, 0x1AF41, 0x1AF42, 0x1AF43, 0x1AF44,
    0x1AF45, 0x1AF46, 0x1AF47, 0x1AF48, 0x1AF49, 0x1AF4A,
    0x1AF4B, 0x1AF4C, 0x1AF4D, 0x1AF4E, 0x19708,
};

static std::uint32_t gChatBubbles554338 = 1; // 原全局：0x00554338。

// 原4441A0：一次动画完成时清可选父槽再标死，未完成则走地图入队。
static void UpdateOneShotMapAnimation_4441A0(YobiAction* const action)
{
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*action->ownerSession, worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    if (AdvanceYobiAnimationFrame_517880(action, 100, AnimationPlayback_StopAtEnd, 0) != AnimationFrameResult_Continue) {
        YobiAction** const slot = action->completionSlot10;
        if (slot != nullptr) {
            *slot = nullptr;
        }
        MarkDeath(action);
        return;
    }
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_Y);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, YobiGraphicAxis_X);
    const std::int32_t x = action->x;
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + x;
    const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    static_cast<void>(world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 0, descriptor));
}

// 原444260：无payload附件，仅借用可选的父引用槽，不改变通用释放逻辑。
YobiAction* CreateOneShotMapAnimation_444260(
    YobiAction* const source, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority,
    YobiAction** const completionSlot)
{
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->characterAction = characterAction;
    action->callback = UpdateOneShotMapAnimation_4441A0;
    action->direction = direction;
    action->displayPriority = displayPriority;
    action->flags = YobiActionFlag_PendingBatchFF;
    (action->mapX200 = static_cast<std::int32_t>(mapX));
    (action->mapY210 = static_cast<std::int32_t>(mapY));
    (action->mapX1E0 = static_cast<std::int32_t>(mapX));
    (action->mapY1F0 = static_cast<std::int32_t>(mapY));
    const double scaledX = mapX * 64.0;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*source->ownerSession, worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    action->completionSlot10 = completionSlot;
    return action;
}

// 原函数：0x004417A0。
std::int32_t GetActorOverheadOffset_4417A0(const std::int32_t imageNo)
{
    if (imageNo >= 100000 && imageNo <= 100999) {
        return gActorHeightOffsets554204[static_cast<std::size_t>((imageNo - 100000) / 25)];
    }
    return -88;
}

// 原函数：0x00444000。
YobiAction* CreateMapAttachmentAction_444000(
    YobiAction* const parent, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority)
{
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->characterAction = characterAction;
    action->direction = direction;
    action->displayPriority = displayPriority;
    action->flags = YobiActionFlag_PendingBatchFF;
    // 原四次各先读取临时 encoded/mask/count，再按同序发布目标。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX200, mapX);
    assignCoordinate(action->mapY210, mapY);
    assignCoordinate(action->mapX1E0, mapX);
    assignCoordinate(action->mapY1F0, mapY);
    const double scaledX = mapX * 64.0;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*parent->ownerSession, worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    return action;
}

// 原函数：0x004412D0。
void ResolveGearPetAnimationState_4412D0(
    YobiAction* const action, [[maybe_unused]] const std::int32_t primaryIndex,
    std::int32_t* const characterAction, std::int32_t* const revert)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->firstAuxiliaryGearKind3C : payload->secondAuxiliaryGearKind40;
        if (kind == MapGearKind_StandLoop) {
            *characterAction = CharacterAnimation_Stand;
            *revert = AnimationPlayback_Loop;
        } else if (kind >= MapGearKind_DirectionalBatch10 && kind <= MapGearKind_DirectionalBatch12) {
            switch (action->characterAction) {
            case CharacterAnimation_Walk: *characterAction = CharacterAnimation_Walk; *revert = AnimationPlayback_Loop; break;
            case CharacterAnimation_BeginRun: *characterAction = CharacterAnimation_BeginRun; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Run: *characterAction = CharacterAnimation_Run; *revert = AnimationPlayback_Loop; break;
            case CharacterAnimation_EndRun: *characterAction = CharacterAnimation_EndRun; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Attack: *characterAction = CharacterAnimation_Attack; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Cast: *characterAction = CharacterAnimation_Cast; *revert = AnimationPlayback_Loop; break;
            case CharacterAnimation_Damage: *characterAction = CharacterAnimation_Damage; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Guard: *characterAction = CharacterAnimation_Guard; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Down: *characterAction = CharacterAnimation_Down; *revert = AnimationPlayback_StopAtEnd; break;
            case CharacterAnimation_Stand:
            case CharacterAnimation_Wave:
            case CharacterAnimation_Joy:
            case CharacterAnimation_Angry:
            case CharacterAnimation_Sad:
            case CharacterAnimation_Nod:
                *characterAction = CharacterAnimation_Stand;
                *revert = AnimationPlayback_Loop;
                break;
            default:
                *characterAction = CharacterAnimation_Stand;
                *revert = AnimationPlayback_StopAtEnd;
                break;
            }
        } else if (action->characterAction == CharacterAnimation_Down) {
            *characterAction = CharacterAnimation_Down;
            *revert = AnimationPlayback_StopAtEnd;
        } else {
            *characterAction = CharacterAnimation_Stand;
            *revert = AnimationPlayback_Loop;
        }
    }
}

// 原函数：0x00441430。
void ResolveGearPetDrawPlacement_441430(
    YobiAction* const action, [[maybe_unused]] const std::int32_t primaryIndex,
    std::int32_t* const x, std::int32_t* const y, std::uint8_t* const before)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->firstAuxiliaryGearKind3C : payload->secondAuxiliaryGearKind40;
        if (kind < MapGearKind_DirectionalBatch10 || kind > MapGearKind_DirectionalBatch12) {
            continue;
        }
        switch (action->direction) {
        case YobiDirection_UpLeft: *x = 34; *y = -4; break;
        case YobiDirection_Up: *x = 36; *y = 16; break;
        case YobiDirection_UpRight: *x = 16; *y = 24; break;
        case YobiDirection_Right: *x = -16; *y = 26; break;
        case YobiDirection_DownRight: *x = -34; *y = 8; break;
        case YobiDirection_Down: *x = -36; *y = -12; break;
        case YobiDirection_DownLeft: *x = -20; *y = -20; break;
        case YobiDirection_Left: *x = 16; *y = -26; break;
        default: break;
        }
        switch (action->direction) {
        case YobiDirection_Up:
        case YobiDirection_UpRight:
        case YobiDirection_Right:
        case YobiDirection_DownRight:
            *before = 1;
            break;
        case YobiDirection_UpLeft:
        case YobiDirection_Down:
        case YobiDirection_DownLeft:
        case YobiDirection_Left:
            *before = 0;
            break;
        default: break;
        }
    }
}

// YGOS01：两借用槽/六kind完整消费，1919由CAC07数据关联补正常父合同，正确绘制正文保持。
// 原函数：0x00441570。
void DrawGear_441570(
    YobiAction* const action, const std::int32_t primaryIndex,
    const std::int32_t offsetX, const std::int32_t offsetY,
    const std::uint8_t before)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->firstAuxiliaryGearKind3C : payload->secondAuxiliaryGearKind40;
        YobiAction* const child = slot == 0 ? payload->firstAuxiliaryGear34 : payload->secondAuxiliaryGear38;
        if (child == nullptr) {
            continue;
        }
        if (kind >= MapGearKind_DirectionalBatch10 && kind <= MapGearKind_DirectionalBatch12) {
            const render::GraphicDescriptor16* const descriptor = &child->drawMode;
            const std::int32_t childY = GetScaledYobiGraphicOriginOffset_518300(child, YobiGraphicAxis_Y, kind);
            const std::int32_t drawY = childY + action->y + offsetY;
            const std::int32_t childX = GetScaledYobiGraphicOriginOffset_518300(child, YobiGraphicAxis_X, kind);
            const std::int32_t drawX = childX + action->x + offsetX;
            const std::int32_t graphicIndex = child->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, before, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                descriptor, static_cast<std::uint8_t>(kind + 7));
        } else if (kind == MapGearKind_StandOrDown || kind == MapGearKind_StandLoop || kind == MapGearKind_AdjacentImagePair) {
            const render::GraphicDescriptor16* const descriptor = &child->drawMode;
            const std::int32_t childY = GetYobiGraphicOriginOffset_5182B0(child, YobiGraphicAxis_Y);
            const std::int32_t drawY = childY + action->y;
            const std::int32_t childX = GetYobiGraphicOriginOffset_5182B0(child, YobiGraphicAxis_X);
            const std::int32_t drawX = childX + action->x;
            const std::int32_t graphicIndex = child->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, kind == MapGearKind_StandOrDown ? 0 : 1, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                descriptor, 0);
        }
    }
}

// AF406：4.06@40AE4E..40B171，地图主挂件与坐骑共享人物的世界排序位置。
static void DrawMapMainGear406(YobiAction* const parent, const std::int32_t primaryIndex)
{
    using enum resource::animation::AnimeActionFlag;
    auto* const payload = parent->payload.mapActor;
    YobiAction* const gear = payload->mainGear30;
    if (gear == nullptr) {
        return;
    }
    gear->x = parent->x;
    gear->y = parent->y;
    gear->direction = parent->direction;
    if (gear->graphicIndex < 0) {
        // 4.06创建时先取首帧发布动作属性，本次尚不提交挂件。
        static_cast<void>(AdvanceYobiAnimationFrame_517880(gear, 100, AnimationPlayback_StopAtEnd, 0));
        return;
    }
    const std::uint16_t flags = GetYobiAnimeActionFlags(gear);
    YobiAction* const ride = payload->rideActor44;
    std::int32_t offsetX = 0;
    std::int32_t offsetY = 0;
    if ((flags & AnimeActionFlag_LockPos) == 0) {
        GetYobiAttachmentCoordinate(parent, offsetX, offsetY);
    }
    if (ride != nullptr) {
        std::int32_t rideX;
        std::int32_t rideY;
        GetYobiAttachmentCoordinate(ride, rideX, rideY);
        offsetX += rideX;
        offsetY += rideY;
    }
    std::int32_t before = 1; // 同层逆序消费：before链在人物之后实际绘制。
    std::uint8_t layer = 10;
    std::int32_t stopAtEnd = AnimationPlayback_Loop;
    const YobiAction* syncSource = nullptr;
    if ((flags & AnimeActionFlag_Wing) != 0) {
        gear->characterAction = parent->characterAction;
        const std::int32_t image = parent->imageNo;
        if (parent->characterAction == CharacterAnimation_Cast
            && ((image >= 100250 && image <= 100273) || (image >= 100650 && image <= 100655))) {
            return;
        }
        syncSource = ride != nullptr ? ride : parent;
        before = gear->direction >= YobiDirection_DownRight && gear->direction <= YobiDirection_DownLeft ? 0 : 1;
        if (gear->characterAction == CharacterAnimation_Down) {
            layer = 9; // 4.06层5，对应本线普通地图层10的下一层。
        }
    } else if ((flags & (AnimeActionFlag_Field | AnimeActionFlag_Shield)) != 0) {
        stopAtEnd = AnimationPlayback_LoopMode3;
        layer = (flags & AnimeActionFlag_Field) != 0 ? 8 : 10;
        offsetX = 0;
        offsetY = 0;
    } else {
        const std::int32_t parentAction = parent->characterAction;
        gear->characterAction = parentAction >= CharacterAnimation_BeginRun && parentAction <= CharacterAnimation_EndRun ? CharacterAnimation_Stand
            : parentAction == CharacterAnimation_Damage ? CharacterAnimation_Run : parentAction;
    }
    static_cast<void>(AdvanceYobiAnimationFrame_517880(gear, 100, stopAtEnd, 0, syncSource));
    if ((flags & AnimeActionFlag_Wing) == 0
        && (parent->phase != MapActorPhase_Normal
            || ((flags & (AnimeActionFlag_Field | AnimeActionFlag_Shield)) != 0 && parent->characterAction == CharacterAnimation_Down))) {
        return;
    }
    if ((flags & AnimeActionFlag_Title) != 0) {
        std::int32_t titleX, titleY;
        if (!GetYobiTitleTopLeft(parent, gear, false, titleX, titleY)) {
            return;
        }
        std::int16_t graphicX, graphicY;
        resource::GetGraphicXY(gear->graphicIndex, &graphicX, &graphicY);
        world::DrawGearNormal_46C060(primaryIndex, world::kMapAttachmentFrontmost, gear->graphicIndex,
            static_cast<std::int16_t>(titleX - graphicX), static_cast<std::int16_t>(titleY - graphicY), &gear->drawMode, 0);
        return;
    }
    // 4.06@4365A6/43695F：WING共享关联主体的绘制基点，不叠加翅膀动画帧偏移。
    YobiAction* const origin = (flags & AnimeActionFlag_Wing) != 0 ? (ride != nullptr ? ride : parent) : gear;
    const std::int32_t x = origin->x + static_cast<std::int16_t>(offsetX) + GetYobiGraphicOriginOffset_5182B0(origin, YobiGraphicAxis_X);
    const std::int32_t y = origin->y + static_cast<std::int16_t>(offsetY) + GetYobiGraphicOriginOffset_5182B0(origin, YobiGraphicAxis_Y);
    world::DrawGearNormal_46C060(primaryIndex, before, gear->graphicIndex,
        static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), &gear->drawMode, 0, layer);
}

// 原函数：0x00442F70。
void DrawMapActorOverlaysAndAttachments_442F70(YobiAction* const action, const std::int32_t primaryIndex)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    std::int32_t stateIndex = payload->index;

    std::uint16_t flags;
    std::uint16_t state;
    std::int32_t value;
    bool drawBars = false;
    YobiAction* const player = action->ownerSession->WorldScene().player->action;
    if ((player != nullptr && player == action) || stateIndex == -1) {
        flags = static_cast<std::uint16_t>(world::ReadPlayerFlags18A(*action->ownerSession));
        state = world::ReadPlayerActionFlags(*action->ownerSession);
        value = action->ownerSession->WorldScene().player->value190;
        if ((action->flags & YobiActionFlag_FollowPetOverlay) != 0) {
            flags &= MapActorDisplayMask_FollowPetStatusImages;
            state = static_cast<std::uint16_t>(
                ((state & MapActorActionFlag_FollowPetStatusImage1AF4E) != 0 ? MapActorActionFlag_StatusImage1AF4E : 0u)
                | ((state & MapActorActionFlag_FollowPetStatusImage1AF4D) != 0 ? MapActorActionFlag_StatusImage1AF4D : 0u)
                | ((state & MapActorActionFlag_FollowPetStatusImage1AF4DSound) != 0 ? MapActorActionFlag_StatusImage1AF4DSound : 0u));
            for (const world::PetRuntimeRecord& pet : (*action->ownerSession->WorldScene().petRecords)) {
                if (pet.used != 0 && pet.follow6D4 != 0) {
                    action->readiness = pet.values0C[0];
                    action->maxHp = pet.values0C[1];
                    action->barValue1B0 = pet.values0C[2];
                    action->barMaximum1C0 = pet.values0C[3];
                    drawBars = true;
                    break;
                }
            }
        } else {
            world::CopyPlayerBarValues(*action->ownerSession,
                &action->readiness, &action->maxHp, &action->barValue1B0, &action->barMaximum1C0);
            drawBars = true;
        }
    } else {
        const std::uint16_t originalFlags = (*action->ownerSession->WorldScene().objects)[stateIndex].flags42;
        state = (*action->ownerSession->WorldScene().objects)[stateIndex].flags120;
        value = (*action->ownerSession->WorldScene().objects)[stateIndex].value124;
        flags = originalFlags;
        if ((action->flags & YobiActionFlag_FollowPetOverlay) != 0) {
            flags &= MapActorDisplayMask_FollowPetStatusImages;
            state = static_cast<std::uint16_t>(
                ((state & MapActorActionFlag_FollowPetStatusImage1AF4E) != 0 ? MapActorActionFlag_StatusImage1AF4E : 0u)
                | ((state & MapActorActionFlag_FollowPetStatusImage1AF4D) != 0 ? MapActorActionFlag_StatusImage1AF4D : 0u)
                | ((state & MapActorActionFlag_FollowPetStatusImage1AF4DSound) != 0 ? MapActorActionFlag_StatusImage1AF4DSound : 0u));
        }
        drawBars = (originalFlags & MapActorDisplayFlag_SharedMovementAndBars) != 0;
    }
    if (action->ownerSession->UiFrame().native.settings.actorBarsEnabled == 0) {
        drawBars = false;
    }

    bool createFirst = action->overheadMarkerRefresh2EC != 0 && (flags & MapActorDisplayFlag_OverheadMarker) != 0;
    if (!createFirst && (flags & MapActorDisplayFlag_OverheadMarker) != 0 && payload->overheadMarker04 == nullptr) {
        createFirst = (flags & MapActorDisplayFlag_StatusImage1AF41) == 0
            || (flags & MapActorDisplayFlag_StatusImage1AF43) == 0 || (flags & MapActorDisplayFlag_StatusImage1AF42) != 0;
    }
    if (createFirst) {
        const std::int32_t kind = action->overheadMarkerMode2E4;
        action->overheadMarkerRefresh2EC = 0;
        const std::int32_t mapY = static_cast<std::int32_t>(action->mapY1F0);
        const std::int32_t mapX = static_cast<std::int32_t>(action->mapX1E0);
        const std::int32_t imageNo = kind == OverheadMarkerMode_CustomImage || kind == OverheadMarkerMode_CustomImageKeepMarkerFlag ? action->overheadMarkerImage2E8 : 0x192BC;
        payload->overheadMarker04 = CreateMapAttachmentAction_444000(action, imageNo, mapX, mapY, CharacterAnimation_Stand, 0, 10);
    } else if (((flags & MapActorDisplayFlag_OverheadMarker) == 0 || (flags & MapActorDisplayFlag_StatusImage1AF41) != 0)
               && payload->overheadMarker04 != nullptr) {
        MarkDeath(payload->overheadMarker04);
        payload->overheadMarker04 = nullptr;
    } else if ((flags & MapActorDisplayFlag_OverheadMarker) != 0) {
        YobiAction* const initialChild = payload->overheadMarker04;
        if (initialChild != nullptr) {
            const std::int32_t parentX = action->x;
            initialChild->x = parentX;
            YobiAction* const yChild = payload->overheadMarker04;
            const std::int32_t parentY = action->y;
            yChild->y = parentY;
            YobiAction* const worldXChild = payload->overheadMarker04;
            const float worldX = action->worldX;
            worldXChild->worldX = worldX;
            YobiAction* const worldYChild = payload->overheadMarker04;
            const float worldY = action->worldY;
            worldYChild->worldY = worldY;
            YobiAction* const animatedChild = payload->overheadMarker04;
            AdvanceYobiAnimationFrame_517880(animatedChild, 100, AnimationPlayback_Loop, 0);
            const std::int32_t imageNo = action->imageNo;
            const std::int32_t height = GetActorOverheadOffset_4417A0(imageNo);
            YobiAction* const yOffsetChild = payload->overheadMarker04;
            const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(yOffsetChild, YobiGraphicAxis_Y);
            const std::int32_t currentY = action->y;
            const std::int32_t heightOffsetY = height + offsetY;
            YobiAction* const xOffsetChild = payload->overheadMarker04;
            const std::int32_t drawY = heightOffsetY + currentY;
            const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(xOffsetChild, YobiGraphicAxis_X);
            const std::int32_t currentX = action->x;
            YobiAction* const graphicChild = payload->overheadMarker04;
            const std::int32_t drawX = offsetX + currentX;
            const std::int32_t graphicIndex = graphicChild->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, 1, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), nullptr, 0);
        }
    }

    if ((flags & MapActorDisplayFlag_ChatBubbleText) != 0) {
        const std::uint64_t now = platform::ReadTickCount64();
        const std::array<std::wstring, 2>* drawableText = nullptr;
        if (payload->chatDeadline14 <= now) {
            auto* const expiredText = payload->text1C;
            flags &= MapActorDisplayMask_WithoutChatBubbleText;
            if (expiredText != nullptr) {
                delete expiredText;
                payload->text1C = nullptr;
            }
        } else if (gChatBubbles554338 != 0 && (action->flags & YobiActionMask_LocalPlayerOrNamedCharacter) != 0
                   && (drawableText = payload->text1C) != nullptr) {
            // 原var_114与对象索引别名；包括零长度，后面的共享状态清位仍用此值。
            stateIndex = payload->textLength24;
            if (stateIndex != 0) {
                const std::int32_t firstMeasureFont = payload->font28;
                std::int32_t width = cg::render::TextMeasure((*drawableText)[0], firstMeasureFont);
                const std::int32_t height = GetActorOverheadOffset_4417A0(action->imageNo);
                std::int32_t y = height + action->y - 32;
                const std::int32_t metricFont = payload->font28;
                const std::int32_t bottom =
                    render::GetFontMetrics(metricFont).doubleHeight + y + 8;
                if (!(*payload->text1C)[1].empty()) {
                    const std::wstring& secondMeasureText = (*payload->text1C)[1];
                    const std::int32_t secondWidth =
                        cg::render::TextMeasure(secondMeasureText, metricFont);
                    if (width < secondWidth) {
                        width = secondWidth;
                    }
                    const std::wstring& text = (*payload->text1C)[1];
                    const auto color = std::bit_cast<std::int8_t>(
                        *reinterpret_cast<const std::uint8_t*>(&payload->color20));
                    const auto font = std::bit_cast<std::int8_t>(
                        *reinterpret_cast<const std::uint8_t*>(&payload->font28));
                    const std::int32_t drawY = y + 4;
                    std::int16_t actionX;
                    std::memcpy(&actionX, &action->x, sizeof(actionX));
                    const auto x = static_cast<std::int16_t>(
                        static_cast<std::int32_t>(actionX) - width / 2);
                    cg::render::DrawText_456780(x, static_cast<std::int16_t>(drawY), 0, font, color, text, 0, 0);
                    y -= render::GetFontMetrics(payload->font28).doubleHeight;
                }
                const std::int32_t halfWidth = width / 2;
                const std::wstring& text = (*payload->text1C)[0];
                const auto color = std::bit_cast<std::int8_t>(
                    *reinterpret_cast<const std::uint8_t*>(&payload->color20));
                const auto font = std::bit_cast<std::int8_t>(
                    *reinterpret_cast<const std::uint8_t*>(&payload->font28));
                std::int16_t actionX;
                std::memcpy(&actionX, &action->x, sizeof(actionX));
                const std::int32_t drawY = y + 4;
                const auto x = static_cast<std::int16_t>(
                    static_cast<std::int32_t>(actionX) - halfWidth);
                cg::render::DrawText_456780(x, static_cast<std::int16_t>(drawY), 0, font, color, text, 0, 0);
                const std::int32_t borderActionX = action->x;
                DrawNineGrid_49BE00(
                    0x3BE84, static_cast<std::int16_t>(borderActionX - halfWidth - 5),
                    static_cast<std::int16_t>(y - 1), static_cast<std::int16_t>(width + 10),
                    static_cast<std::int16_t>(bottom - y + 2), 0xCE, -1,
                    std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
            }
        } else {
            const std::int32_t y = action->y - 84;
            const std::int32_t x = action->x - 20;
            cg::render::DrawGraphic(x, y, 0xCE, 0x573F, 0, nullptr);
        }
    }

    const std::int16_t nameMode = platform::gActorOverheadTextMode_5F974C;
    if (nameMode > 0) {
        const wchar_t* text = nullptr;
        std::int8_t color = 0;
        if (action == action->ownerSession->WorldScene().player->action) {
            if (nameMode <= ActorOverheadTextSlot_Text31C) {
                constexpr std::array<std::int8_t, 6> colors {1, 4, 5, 2, 0, 0};
                text = world::GetPlayerDisplayText(*action->ownerSession, nameMode);
                color = colors[nameMode - 1];
            }
        } else {
            const std::uint32_t actionFlags = action->flags;
            if ((actionFlags & YobiActionFlag_NamedCharacterWithTitle) != 0 && (actionFlags & YobiActionFlag_FollowPetOverlay) == 0) {
                if (action->imageNo != 0) {
                    switch (nameMode) {
                    case ActorOverheadTextSlot_NameC4: text = action->nameC4.data(); color = 0; break;
                    case ActorOverheadTextSlot_TitleE1: text = action->titleE1.data(); color = 1; break;
                    case ActorOverheadTextSlot_Text144: text = action->text144.data(); color = 4; break;
                    case ActorOverheadTextSlot_Text102: text = action->text102.data(); color = 5; break;
                    case ActorOverheadTextSlot_Text123: text = action->text123.data(); color = 2; break;
                    case ActorOverheadTextSlot_Text31C: text = action->text31C.data(); color = 2; break;
                    default: break;
                    }
                }
            } else if ((actionFlags & YobiActionMask_PetOrRiderOrFollowPetOverlay) != 0) {
                text = (actionFlags & YobiActionFlag_PetOrRider) != 0 && !action->titleE1.empty()
                    ? action->titleE1.data() : action->nameC4.data();
            } else if ((actionFlags & YobiActionFlag_OtherMapObjectName) != 0) {
                if (!action->nameC4.empty() && action->imageNo > 0 && !(action->flags & YobiActionFlag_Hidden)) {
                    text = action->nameC4.data();
                }
            }
        }
        if (text != nullptr) {
            std::wstring buffer;
            cg::text::FormatTo(buffer, L"{}", text);
            const auto y = static_cast<std::int16_t>(action->y - 98);
            const std::int32_t width = cg::render::TextMeasure(buffer, 1);
            const auto x = static_cast<std::int16_t>(action->x - width / 2);
            cg::render::DrawText_456780(x, y, 0, 1, color, buffer, 0, 0);
        }
    }

    // 三图标的共同定位按每个原取槽点重读，不捕获一份子指针跨越查询调用。
    const auto positionStatus = [&](YobiAction*& slot) {
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(slot, YobiGraphicAxis_X);
        slot->x = offsetX + action->x;
        const std::int32_t height = GetActorOverheadOffset_4417A0(action->imageNo);
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(slot, YobiGraphicAxis_Y);
        slot->y = height + offsetY + action->y - 20;
        const std::int32_t worldX = GetYobiGraphicOriginOffset_5182B0(slot, YobiGraphicAxis_X);
        slot->worldX = static_cast<float>(static_cast<double>(worldX) + action->worldX);
        const std::int32_t worldY = GetYobiGraphicOriginOffset_5182B0(slot, YobiGraphicAxis_Y);
        slot->worldY = static_cast<float>(
            static_cast<double>(worldY) + action->worldY + 0.1F);
    };
    const auto drawStatus = [&](YobiAction*& slot) {
        AdvanceYobiAnimationFrame_517880(slot, 100, AnimationPlayback_Loop, 0);
        slot->drawMode.mixFlags |= cg::render::GraphicMixFlag_OriginalPalette;
        YobiAction* const child = slot;
        const std::int32_t y = child->y;
        const render::GraphicDescriptor16* const descriptor = &child->drawMode;
        const std::int32_t x = child->x;
        const std::int32_t graphicIndex = child->graphicIndex;
        world::DrawGearNormal_46C060(
            primaryIndex, 1, graphicIndex,
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), descriptor, 0);
    };
    const auto clearActionState = [&](const std::uint16_t bits) {
        YobiAction* const currentPlayer = action->ownerSession->WorldScene().player->action;
        if ((currentPlayer != nullptr && currentPlayer == action) || stateIndex == -1) {
            world::ClearPlayerActionFlags(*action->ownerSession, bits);
        } else {
            (*action->ownerSession->WorldScene().objects)[stateIndex].flags120 &=
                static_cast<std::uint16_t>(~bits);
        }
    };

    const std::uint16_t statusMask = flags & MapActorDisplayMask_PrimaryStatusImages;
    if (statusMask != 0 || (state & MapActorActionFlag_DynamicStatusImage) != 0 || (state & MapActorActionFlag_StatusImage1824F) != 0) {
        std::int32_t statusImage = 0;
        if ((state & MapActorActionFlag_DynamicStatusImage) != 0) {
            if (value < 0 || value >= world::GetActorStatusIconCount_4410E0()) {
                value = 0;
            }
            statusImage = gActorStatusImages5542F4[value];
        }
        if ((flags & MapActorDisplayFlag_StatusImage1AF41) != 0) {
            statusImage = 0x1AF41;
        }
        if ((flags & MapActorDisplayFlag_StatusImage1AF42) != 0) {
            statusImage = 0x1AF42;
        }
        if ((flags & MapActorDisplayFlag_StatusImage1AF43) != 0) {
            statusImage = 0x1AF43;
        }
        if ((state & MapActorActionFlag_StatusImage1824F) != 0) {
            statusImage = 0x1824F;
        }
        if (payload->statusIcon4C == nullptr) {
            const std::int32_t mapY = static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t mapX = static_cast<std::int32_t>(action->mapX1E0);
            YobiAction* const child = CreateMapAttachmentAction_444000(action, statusImage, mapX, mapY, CharacterAnimation_Stand, 0, 10);
            payload->statusIcon4C = child;
            child->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
            payload->statusIcon4C->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
        }
        if (payload->statusIcon4C != nullptr) {
            payload->statusIcon4C->imageNo = statusImage;
            positionStatus(payload->statusIcon4C);
            if ((state & MapActorActionFlag_StatusImage1AF4E) != 0 && statusMask == 0) {
                payload->statusIcon4C->x -= 17;
            }
            drawStatus(payload->statusIcon4C);
        }
    } else if (payload->statusIcon4C != nullptr) {
        MarkDeath(payload->statusIcon4C);
        payload->statusIcon4C = nullptr;
    }

    if ((state & MapActorActionFlag_StatusImage1AF4D) != 0) {
        if (payload->statusIcon4C == nullptr) {
            if (payload->statusIcon1AF4D_0C == nullptr) {
                const std::int32_t mapY = static_cast<std::int32_t>(action->mapY1F0);
                const std::int32_t mapX = static_cast<std::int32_t>(action->mapX1E0);
                payload->statusIcon1AF4D_0C = CreateMapAttachmentAction_444000(action, 0x1AF4D, mapX, mapY, CharacterAnimation_Stand, 0, 10);
                if ((state & MapActorActionFlag_StatusImage1AF4DSound) != 0) {
                    clearActionState((action->flags & YobiActionFlag_FollowPetOverlay) != 0 ? static_cast<std::int32_t>(MapActorActionFlag_FollowPetStatusImage1AF4DSound) : static_cast<std::int32_t>(MapActorActionFlag_StatusImage1AF4DSound));
                    static_cast<void>(audio::PlaySe_52C450(0x1B5, action->x));
                }
                payload->statusIcon1AF4D_0C->counter1DC = 0;
                payload->statusIcon1AF4D_0C->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
                payload->statusIcon1AF4D_0C->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
            }
            if (payload->statusIcon1AF4D_0C != nullptr) {
                positionStatus(payload->statusIcon1AF4D_0C);
                drawStatus(payload->statusIcon1AF4D_0C);
            }
        }
        ++payload->statusIcon1AF4D_0C->counter1DC;
        if (payload->statusIcon1AF4D_0C->counter1DC > 600) {
            clearActionState((action->flags & YobiActionFlag_FollowPetOverlay) != 0 ? static_cast<std::int32_t>(MapActorActionFlag_FollowPetStatusImage1AF4D) : static_cast<std::int32_t>(MapActorActionFlag_StatusImage1AF4D));
        }
    } else if (payload->statusIcon1AF4D_0C != nullptr) {
        MarkDeath(payload->statusIcon1AF4D_0C);
        payload->statusIcon1AF4D_0C = nullptr;
    }

    if ((state & MapActorActionFlag_StatusImage1AF4E) != 0 && statusMask == 0) {
        if (payload->statusIcon1AF4E_10 == nullptr) {
            const std::int32_t mapY = static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t mapX = static_cast<std::int32_t>(action->mapX1E0);
            YobiAction* const child = CreateMapAttachmentAction_444000(action, 0x1AF4E, mapX, mapY, CharacterAnimation_Stand, 0, 10);
            payload->statusIcon1AF4E_10 = child;
            child->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
            payload->statusIcon1AF4E_10->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
        }
        if (payload->statusIcon1AF4E_10 != nullptr) {
            positionStatus(payload->statusIcon1AF4E_10);
            if (payload->statusIcon4C != nullptr) {
                payload->statusIcon1AF4E_10->x += 17;
            }
            drawStatus(payload->statusIcon1AF4E_10);
        }
    } else if (payload->statusIcon1AF4E_10 != nullptr) {
        MarkDeath(payload->statusIcon1AF4E_10);
        payload->statusIcon1AF4E_10 = nullptr;
    }

    DrawMapMainGear406(action, primaryIndex);
    if ((payload->firstAuxiliaryGear34 != nullptr || payload->secondAuxiliaryGear38 != nullptr)
        && payload->rideActor44 == nullptr) {
        std::int32_t characterAction = CharacterAnimation_Stand;
        std::int32_t revert = AnimationPlayback_StopAtEnd;
        std::int32_t x = 0;
        std::int32_t y = 0;
        std::uint8_t before = 1;
        if (payload->firstAuxiliaryGear34 != nullptr) {
            payload->firstAuxiliaryGear34->direction = action->direction;
        }
        if (payload->secondAuxiliaryGear38 != nullptr) {
            payload->secondAuxiliaryGear38->direction = action->direction;
        }
        ResolveGearPetAnimationState_4412D0(action, primaryIndex, &characterAction, &revert);
        ResolveGearPetDrawPlacement_441430(action, primaryIndex, &x, &y, &before);
        if (payload->firstAuxiliaryGearKind3C == MapGearKind_AdjacentImagePair && payload->firstAuxiliaryGear34->characterAction == CharacterAnimation_Down) {
            payload->firstAuxiliaryGear34->graphicIndex = -1;
        }
        if (payload->secondAuxiliaryGearKind40 == MapGearKind_AdjacentImagePair && payload->secondAuxiliaryGear38->characterAction == CharacterAnimation_Down) {
            payload->secondAuxiliaryGear38->graphicIndex = -1;
        }
        DrawGear_441570(action, primaryIndex, x, y, before);
        if (payload->firstAuxiliaryGear34 != nullptr) {
            payload->firstAuxiliaryGear34->characterAction = characterAction;
        }
        if (payload->secondAuxiliaryGear38 != nullptr) {
            payload->secondAuxiliaryGear38->characterAction = characterAction;
        }
        PlayFrame_517DA0(payload->firstAuxiliaryGear34, revert);
        PlayFrame_517DA0(payload->secondAuxiliaryGear38, revert);
    }
    if (drawBars) {
        ShowHpBar_43DB60(action, -10, 3, -1);
    }
}

// 原函数：0x0043DB60。
std::int32_t ShowHpBar_43DB60(
    YobiAction* const action, const std::int16_t offsetX,
    const std::int16_t offsetY, const std::int32_t color)
{
    NineGridDescriptorFields fields {};
    std::int32_t graphicIndex;
    resource::GetGraphicMapNoIndex(0x3BDF0, &graphicIndex);
    std::int16_t graphicX;
    std::int16_t graphicY;
    resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
    fields.color = color;
    fields.mixFlags = render::GraphicMixFlag_OriginalPalette;
    fields.clip = 1;
    fields.sourceX = -16;
    fields.sourceY = -1;
    fields.height = 2;
    render::GraphicDescriptor16 descriptor = ToGraphicDescriptor(fields);
    if (static_cast<std::int32_t>(action->maxHp) != 0) {
        const std::int32_t numerator =
            static_cast<std::int32_t>(action->readiness) * 32;
        const std::int32_t maximum =
            static_cast<std::int32_t>(action->maxHp);
        fields.width = static_cast<std::int16_t>(numerator / maximum);
        const std::int32_t y = action->y + offsetY - 2;
        const std::int32_t x = action->x + offsetX - 12;
        descriptor = ToGraphicDescriptor(fields);
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 0xCF, 0x3BDF0, 0, 1, 0, &descriptor));
    }
    if (static_cast<std::int32_t>(action->barMaximum1C0) != 0) {
        const std::int32_t numerator =
            static_cast<std::int32_t>(action->barValue1B0) * 32;
        const std::int32_t maximum =
            static_cast<std::int32_t>(action->barMaximum1C0);
        fields.width = static_cast<std::int16_t>(numerator / maximum);
        const std::int32_t y = action->y + offsetY + 1;
        const std::int32_t x = action->x + offsetX - 12;
        descriptor = ToGraphicDescriptor(fields);
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 0xCF, 0x3BDEF, 0, 1, 0, &descriptor));
    }
    const std::int32_t y = action->y + offsetY;
    const std::int32_t x = action->x + offsetX;
    fields.clip = 0;
    descriptor = ToGraphicDescriptor(fields);
    return cg::render::DrawGraphic(x, y, 0xCF, 0x3BDEE, 0, &descriptor);
}

} // namespace detail

// 原函数：0x00441130。
YobiAction* NewYobiGearPet_441130(
    YobiAction* const source, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY)
{
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->displayPriority = 10;
    action->flags = YobiActionMask_MapActorDefaults;
    (action->mapX200 = static_cast<std::int32_t>(mapX));
    (action->mapY210 = static_cast<std::int32_t>(mapY));
    action->pathCountOrDrawOrder_270 = 0;
    (action->mapX1E0 = static_cast<std::int32_t>(mapX));
    (action->mapY1F0 = static_cast<std::int32_t>(mapY));
    const double scaledX = mapX * 64.0;
    action->movementStepXBits_27C = 0;
    action->movementStepYBits_280 = 0;
    action->phase = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*source->ownerSession, worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    return action;
}

// 原函数：0x004410F0。
YobiAction* NewYobiGear_4410F0(YobiAction* const source, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->displayPriority = 10;
    action->flags = static_cast<std::int32_t>(YobiActionMask_MapActorDefaults);
    return action;
}

// 原函数：0x00507140。
void DescribeHoveredYobi_507140(session::ClientSession& session, const render::GraphicCacheItem32* const graphic)
{
    constexpr wchar_t kFull58AD04[] =
        L"等级 {:<3} {:<16}  生命 {:4}/{:<4}  魔力 {:4}/{:<4}";
    constexpr wchar_t kLevel58AD5C[] = L"等级 {:<3} {:<16}";

    YobiAction* action = graphic->reference.action;
    if (action == nullptr || (action->flags & YobiActionFlag_HoverDescription) == 0) {
        return;
    }
    auto& interaction = action->ownerSession->UiFrame().native.interaction;
    const std::int32_t scene = action->ownerSession->WorldScene().sceneType;
    std::int32_t companion = action->hoverTextColor1D8;
    interaction.hoverObjectValue = companion;
    if (scene == 9) {
        action = graphic->reference.action;
        if (action->imageNo <= 0) {
            return;
        }
        const std::uint32_t flags = action->flags;
        if ((flags & YobiActionFlag_LocalPlayer) != 0) {
            interaction.hoverObjectValue = action->hoverTextColor1D8;
            YobiAction* const fixed = graphic->reference.action;
            const std::int32_t maxFp = static_cast<std::int32_t>(action->ownerSession->WorldScene().player->maxFp);
            const std::int32_t fp = static_cast<std::int32_t>(action->ownerSession->WorldScene().player->fp);
            const std::int32_t maxHp = static_cast<std::int32_t>(action->ownerSession->WorldScene().player->maxHp);
            const std::int32_t hp = static_cast<std::int32_t>(action->ownerSession->WorldScene().player->hp);
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kFull58AD04, fixed->field1D0, fixed->nameC4.c_str(), hp, maxHp, fp, maxFp);
        } else if ((flags & YobiActionFlag_FollowPetOverlay) != 0) {
            interaction.hoverObjectValue = action->hoverTextColor1D8;
            YobiAction* const fixed = graphic->reference.action;
            // 原额外字符串实参不被本格式消费，不增加显示内容。
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kLevel58AD5C, fixed->field1D0, fixed->nameC4.c_str(), fixed->titleE1.c_str());
        } else if ((flags & YobiActionFlag_NamedCharacterWithTitle) != 0) {
            interaction.hoverObjectValue = action->hoverTextColor1D8;
            YobiAction* const fixed = graphic->reference.action;
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, L"等级 {:<3} {:<16} [{:<16}]" /* 原常量参考：0x0058AD44。 */, fixed->field1D0, fixed->nameC4.c_str(), fixed->titleE1.c_str());
            const wchar_t* const title = graphic->reference.action->text144.data();
            if (*title != L'\0') {
                interaction.hoverTitle = std::format(L"{:<16}", title);
                interaction.hoverTitleValue = 4;
            }
            const std::uint8_t control = platform::gKeyStates[detail::HoverShortcutKey_Control] & detail::HoverShortcutKeyStateFlag_Down;
            if (control != 0 && (platform::gKeyStates[detail::HoverShortcutKey_Insert] & detail::HoverShortcutKeyStateFlag_FirstPress) != 0) {
                if (cg::world::blacklist::GetBlacklistCount_401190(session) >= 72) {
                    cg::world::AppendChatMessage_44B050(*action->ownerSession, L"你的黑名单已满，无法再加入" /* 原常量参考：0x00553294。 */, 4, 3);
                    return;
                }
                const wchar_t* const name = graphic->reference.action->nameC4.data();
                if (name == nullptr) {
                    return;
                }
                std::wstring message;
                if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(session, name) == 0) {
                    const std::int8_t added = cg::world::blacklist::AddBlacklistRecord_401270(session, graphic->reference.action->nameC4);
                    cg::text::FormatTo(message, added != 0 ? L"已将{}加入黑名单" /* 原常量参考：0x005532C4。 */ : L"{}加入黑名单失败" /* 原常量参考：0x005532B0。 */, graphic->reference.action->nameC4.c_str());
                } else {
                    cg::text::FormatTo(message, L"{}已在黑名单中" /* 原常量参考：0x00554928。 */, graphic->reference.action->nameC4.c_str());
                }
                cg::world::AppendChatMessage_44B050(*action->ownerSession, message, 4, 3);
            } else if (control != 0 && (platform::gKeyStates[detail::HoverShortcutKey_Delete] & detail::HoverShortcutKeyStateFlag_FirstPress) != 0) {
                const wchar_t* const name = graphic->reference.action->nameC4.data();
                if (name == nullptr) {
                    return;
                }
                std::wstring message;
                if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(session, name) != 0) {
                    const std::int8_t removed = cg::world::blacklist::RemoveBlacklistRecord_4012F0(session, graphic->reference.action->nameC4);
                    cg::text::FormatTo(message, removed != 0 ? L"已将{}从黑名单中移除" /* 原常量参考：0x005532EC。 */ : L"从黑名单移除{}失败" /* 原常量参考：0x005532D8。 */, graphic->reference.action->nameC4.c_str());
                } else {
                    cg::text::FormatTo(message, L"{}不在黑名单中" /* 原常量参考：0x0058AD34。 */, graphic->reference.action->nameC4.c_str());
                }
                cg::world::AppendChatMessage_44B050(*action->ownerSession, message, 4, 3);
            }
        } else if ((flags & YobiActionFlag_PetOrRider) != 0) {
            const wchar_t* name = action->titleE1.data();
            if (*name == '\0') {
                name = action->nameC4.data();
            }
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kLevel58AD5C, action->field1D0, name);
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & YobiActionFlag_GroundItem) != 0) {
            interaction.hoverObjectValue = action->hoverTextColor1D8;
            YobiAction* const fixed = graphic->reference.action;
            const std::int32_t count = fixed->payload.mapActor->itemCount18;
            if (count > 0) {
                cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, L"{}({})", fixed->nameC4.c_str(), count);
            } else {
                cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, L"{}", fixed->nameC4.c_str());
            }
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & YobiActionFlag_GroundGold) != 0) {
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, L"{}", action->nameC4.c_str());
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & YobiActionFlag_OtherMapObjectName) != 0) {
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, L"{}", action->nameC4.c_str());
        }
        return;
    }
    if (scene != 10) {
        return;
    }

    const std::int32_t position = action->ownerSession->WorldScene().battle.currentPosition;
    action = graphic->reference.action;
    const auto* const payload = static_cast<const detail::BattleActorPayload*>(action->payload.battleActor);
    if (position < 10) {
        // G02：只对应参考首匹配507AD4，其他位置分支的门禁保持。
        if (payload != nullptr
            && (action->ownerSession->Connection().GameplaySettings().showEnemyHpBars
                || payload->battlePosition18 < 10)) {
            if ((payload->flagsFDC & detail::BattleActorFlag_Morph) != 0) {
                companion = 4;
            }
            interaction.hoverObjectValue = companion;
        } else {
            if ((action->flags & YobiActionFlag_AuxiliaryBattleGear) == 0) {
                cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kLevel58AD5C, action->field1D0, action->nameC4.c_str());
            }
            return;
        }
    } else if (position < 20) {
        if (payload != nullptr && payload->battlePosition18 < 10) {
            if ((payload->flagsFDC & detail::BattleActorFlag_Morph) != 0) {
                companion = 4;
            }
            interaction.hoverObjectValue = companion;
            YobiAction* const fixed = graphic->reference.action;
            cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kLevel58AD5C, fixed->field1D0, fixed->nameC4.c_str());
            return;
        }
        if ((action->flags & YobiActionFlag_AuxiliaryBattleGear) != 0) {
            return;
        }
    } else {
        if (payload != nullptr && payload->battlePosition18 < 10) {
            if ((payload->flagsFDC & detail::BattleActorFlag_Morph) != 0) {
                companion = 4;
            }
            interaction.hoverObjectValue = companion;
        }
        YobiAction* const fixed = graphic->reference.action;
        cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kLevel58AD5C, fixed->field1D0, fixed->nameC4.c_str());
        return;
    }

    // 原507AEA/507B8F固定首对象供名称/等级，后续三个CryptInt仍逐次重取对象。
    YobiAction* const fixed = graphic->reference.action;
    const std::int32_t maxFp = static_cast<std::int32_t>(fixed->barMaximum1C0);
    const std::int32_t fp = static_cast<std::int32_t>(graphic->reference.action->barValue1B0);
    const std::int32_t maxHp = static_cast<std::int32_t>(graphic->reference.action->maxHp);
    const std::int32_t hp = static_cast<std::int32_t>(graphic->reference.action->readiness);
    cg::text::FormatTo(action->ownerSession->UiFrame().hoverText, kFull58AD04, fixed->field1D0, fixed->nameC4.c_str(), hp, maxHp, fp, maxFp);
}

} // namespace cg::yobi
