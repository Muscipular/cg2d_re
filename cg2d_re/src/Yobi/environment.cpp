#include "base.hpp"
#include "../platform/MapEffectMath.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"

#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

// 负环境配置的图形类型；只按已证资源图号命名，不是移动mode。
enum SceneEffectGraphicType : std::int32_t {
    SceneEffectGraphicType_Image14645 = 100,
    SceneEffectGraphicType_Image14646 = 101,
    SceneEffectGraphicType_Image301122 = 201,
    SceneEffectGraphicType_Image1002065 = 202,
};

// 漂移帧表行及低两位退出变体；4只证实波纹与变体0退出规则。
enum SceneDriftVariant : std::int32_t {
    SceneDriftVariant_FromPositiveWorldX = 0,
    SceneDriftVariant_FromNegativeWorldX = 1,
    SceneDriftVariant_FromNegativeWorldY = 2,
    SceneDriftVariant_FromPositiveWorldY = 3,
    SceneDriftVariant_RippleWithExitVariant0 = 4,
};

// 449570帧表组索引；覆盖层及漂移对象各用独立表。
enum SceneEffectFrameGroup : std::int32_t {
    SceneEffectFrameGroup_Overlay = 0,
    SceneEffectFrameGroup_Drift = 1,
};

// 原448810/448D90/449700生产，三路回调消费；characterAction在此保存移动模式。
// mode0分别表示计时配置或直接返回，保留各入口的原解释。
enum SceneEffectMode : std::int32_t {
    SceneEffectMode_MovingIn = 1, // 向目标进场，完成后按原递增进入2。
    SceneEffectMode_Stationary = 2, // 停止进出场移动，仍执行投影和原波纹处理。
    SceneEffectMode_MovingOut = 3, // 沿各入口原方向离场。
    SceneEffectMode_RemoveIfPresent = 4, // 有旧节点则标死；无旧节点保留原分配／default路径。
};

// 原449570解释帧duration；只约束指令值，不替换普通帧时长和目标索引。
enum SceneEffectFrameCommand : std::int32_t {
    SceneEffectFrameCommand_End = -1, // 结束并返回1；当前七张表未生产此指令。
    SceneEffectFrameCommand_Jump = -2, // graphic槽为跳转目标帧。
};

struct SceneEffectFrame {
    std::int32_t duration; // -1终止、-2跳转，其他值为帧时长。
    std::int32_t graphic; // 通常mapNo；group1/image4为index，-2记录为目标帧。
};

static_assert(sizeof(SceneEffectFrame) == 8);

static constexpr SceneEffectFrame kFrames55433C[] { // 原常量参考：0x0055433C。
    {24, 0x38C54}, {24, 0x38C55}, {24, 0x38C56}, {24, 0x38C57},
    {24, 0x38C58}, {24, 0x38C59}, {24, 0x38C5A}, {24, 0x38C5B},
    {24, 0x38C5C}, {24, 0x38C5D}, {24, 0x38C5E}, {24, 0x38C5F},
    {24, 0x38C60}, {24, 0x38C61}, {24, 0x38C62}, {24, 0x38C63}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames5543C4[] { // 原常量参考：0x005543C4。
    {16, 0x38C54}, {16, 0x38C55}, {16, 0x38C56}, {16, 0x38C57},
    {16, 0x38C58}, {16, 0x38C59}, {16, 0x38C5A}, {16, 0x38C5B},
    {16, 0x38C5C}, {16, 0x38C5D}, {16, 0x38C5E}, {16, 0x38C5F},
    {16, 0x38C60}, {16, 0x38C61}, {16, 0x38C62}, {16, 0x38C63}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames554454[] { // 原常量参考：0x00554454。
    {8, 0x38C3C}, {8, 0x38C3D}, {8, 0x38C3E}, {8, 0x38C3F},
    {4, 0x38C40}, {4, 0x38C41}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames55448C[] { // 原常量参考：0x0055448C。
    {8, 0x38C48}, {8, 0x38C49}, {8, 0x38C4A}, {8, 0x38C4B},
    {4, 0x38C4C}, {4, 0x38C4D}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames5544C4[] { // 原常量参考：0x005544C4。
    {8, 0x38C42}, {8, 0x38C43}, {8, 0x38C44}, {8, 0x38C45},
    {4, 0x38C46}, {4, 0x38C47}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames5544FC[] { // 原常量参考：0x005544FC。
    {8, 0x38C4E}, {8, 0x38C4F}, {8, 0x38C50}, {8, 0x38C51},
    {4, 0x38C52}, {4, 0x38C53}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr SceneEffectFrame kFrames554534[] { // 原常量参考：0x00554534。
    {240, 227336}, {8, 227335}, {30, 227334}, {8, 227335}, {SceneEffectFrameCommand_Jump, 0},
};

static constexpr const SceneEffectFrame* kFrameGroup55444C[] { // 原常量参考：0x0055444C。
    kFrames55433C, kFrames5543C4,
};

static constexpr const SceneEffectFrame* kFrameGroup55455C[] { // 原常量参考：0x0055455C。
    kFrames554454, kFrames55448C, kFrames5544C4, kFrames5544FC, kFrames554534,
};

static constexpr const SceneEffectFrame* const* kFrameGroups554570[] { // 原常量参考：0x00554570。
    kFrameGroup55444C, kFrameGroup55455C,
};

// 与AdvanceSceneEffectTableFrame_449570共用实际帧表，跳转指令不属于图形引用。
static void RetainDirectFrameGraphics()
{
    for (const SceneEffectFrame& frame : kFrames554534) {
        if (frame.duration != SceneEffectFrameCommand_Jump
            && frame.duration != SceneEffectFrameCommand_End) {
            resource::gGraphicInfo.Retain(static_cast<std::uint32_t>(frame.graphic));
        }
    }
}

// 原 sub_448370@[0x448370,0x44839C)；查找首个 callback 匹配且 deathFlag 为零的普通节点。
// 调用前提为已初始化的有效链；允许空 callback，返回非拥有指针，未命中返回空。
static YobiAction* FindLiveActionByCallback(
    session::ClientSession& session, void (*callback)(YobiAction*))
{
    // 原 0x448370/0x448379/0x448380：先读首槽、缓存尾槽，再读 start->next。
    YobiAction* const start = session.Yobi().start;
    YobiAction* const end = session.Yobi().end;
    YobiAction* current = start->next;
    while (current != end) {
        if (current->callback == callback && current->deathFlag == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// 原[449570,44966B)：两组帧表共用播放器，复用action动画字段。
static std::int32_t AdvanceSceneEffectTableFrame_449570(YobiAction* const action, const std::int32_t group)
{
    const std::int32_t image = action->imageNo;
    if (action->lastImageNo != image) {
        action->lastImageNo = image;
        action->frameRemain = 0;
        action->frameIndex = 0;
    }
    if (action->frameRemain == 0) {
        const SceneEffectFrame* frame =
            &kFrameGroups554570[group][image][action->frameIndex];
        if (frame->duration == SceneEffectFrameCommand_End) {
            return 1;
        }
        if (frame->duration == SceneEffectFrameCommand_Jump) {
            action->frameIndex = static_cast<std::uint32_t>(frame->graphic);
            frame = &kFrameGroups554570[group][image][action->frameIndex];
        }
        action->frameRemain = frame->duration;
        std::int32_t graphicIndex;
        if (group == SceneEffectFrameGroup_Drift && image == SceneDriftVariant_RippleWithExitVariant0) {
            graphicIndex = frame->graphic;
        } else {
            resource::GetGraphicMapNoIndex(frame->graphic, &graphicIndex);
        }
        std::int16_t offsetX;
        std::int16_t offsetY;
        resource::GetGraphicXY(
            static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
        action->graphicIndex = graphicIndex;
        const std::uint32_t nextFrame = action->frameIndex + 1;
        action->graphicOffsetY = offsetY;
        action->graphicOffsetX = offsetX;
        action->frameIndex = nextFrame;
    }
    --action->frameRemain;
    return 0;
}

// 原[4496C0,4496FE)：共享波纹对屏幕Y的补偿，返回除法修正位。
static std::uint32_t ApplyRippleOffsetToSceneEffect_4496C0(YobiAction* const action)
{
    const std::uint32_t phase = action->ownerSession->WorldScene().map.rippleIntegerPhaseBits;
    const std::int32_t y = action->y;
    const std::uint32_t index = (static_cast<std::uint32_t>(y >> 3) + phase) & 0x3Fu;
    const std::int32_t sine = render::gRippleSineTable[index];
    const std::uint32_t strengthBits = action->ownerSession->WorldScene().map.rippleStrengthBits;
    const std::int32_t product = sine * std::bit_cast<std::int32_t>(strengthBits);
    action->y = y - product / 100;
    return product < 0 ? 1u : 0u;
}

static void PlaySceneEffectSounds()
{
    static_cast<void>(audio::PlaySe_52C450(0x1BC, 320));
    static_cast<void>(audio::PlaySe_52C450(0x1BB, 320));
    static_cast<void>(audio::PlaySe_52C450(0x194, 320));
}

// 三路生产的相同图形/新对象写序；调用方先发布各自callback。
static void InitializeSceneEffectGraphic(
    YobiAction* const action, const std::int32_t mapNo, const bool created)
{
    std::int32_t graphicIndex;
    resource::GetGraphicMapNoIndex(mapNo, &graphicIndex);
    std::int16_t offsetX;
    std::int16_t offsetY;
    resource::GetGraphicXY(
        static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
    action->graphicIndex = graphicIndex;
    action->graphicOffsetX = offsetX;
    action->graphicOffsetY = offsetY;
    if (created) {
        action->origin_x_offset40 = 0.0F;
        action->origin_y_offset44 = 0.0F;
        action->motionMagnitude54 = 0.25F;
        action->angle50 = 0.0F;
        action->field58 = 0;
    }
}

// 原[448A50,448BEF)：正类型对象回调，保持FST后仍使用寄存器值的边界。
static void UpdateRightEntrySceneEffect_448A50(YobiAction* const action)
{
    const std::int32_t floor = action->ownerSession->WorldScene().map.floor;
    if (static_cast<std::int32_t>(action->mapY1F0) != floor) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == SceneEffectMode_MovingIn) {
        if (action->origin_x_offset40 < 0.0F) {
            if (action->motionMagnitude54 > 0.0F
                && static_cast<double>(action->motionMagnitude54) - 0.11F <= 0.0) {
                PlaySceneEffectSounds();
            }
            const double speed = static_cast<double>(action->motionMagnitude54) - 0.11F;
            action->motionMagnitude54 = static_cast<float>(speed);
            const double x = action->origin_x_offset40 - speed;
            action->origin_x_offset40 = static_cast<float>(x);
            action->origin_y_offset44 = static_cast<float>(
                speed * 0.14F + action->origin_y_offset44);
            if (x > 0.0) {
                action->origin_x_offset40 = 0.0F;
                action->origin_y_offset44 = 0.0F;
                action->motionMagnitude54 = 0.0F;
                ++action->characterAction;
            }
        } else {
            action->origin_x_offset40 = static_cast<float>(
                static_cast<double>(action->origin_x_offset40) - action->motionMagnitude54);
            action->origin_y_offset44 = static_cast<float>(
                static_cast<double>(action->motionMagnitude54) * 0.14F + action->origin_y_offset44);
        }
    } else if (action->characterAction == SceneEffectMode_MovingOut) {
        const double speed = static_cast<double>(action->motionMagnitude54) + 0.002F;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (speed > 1.5F) {
            action->motionMagnitude54 = 1.5F;
        }
        action->origin_x_offset40 = static_cast<float>(
            static_cast<double>(action->origin_x_offset40) + action->motionMagnitude54);
        action->origin_y_offset44 = static_cast<float>(
            action->origin_y_offset44
            - static_cast<double>(action->motionMagnitude54) * 0.2F);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*action->ownerSession, action->worldX, action->worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenX) + action->origin_x_offset40 + 0.5));
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenY) + action->origin_y_offset44 + 0.5));
}

// 原[448BF0,448D8F)：负类型对象回调，偏移推进方向与448A50相反。
static void UpdateLeftEntrySceneEffect_448BF0(YobiAction* const action)
{
    const std::int32_t floor = action->ownerSession->WorldScene().map.floor;
    if (static_cast<std::int32_t>(action->mapY1F0) != floor) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == SceneEffectMode_MovingIn) {
        if (action->origin_x_offset40 > 0.0F) {
            if (action->motionMagnitude54 > 0.0F
                && static_cast<double>(action->motionMagnitude54) - 0.11F <= 0.0) {
                PlaySceneEffectSounds();
            }
            const double speed = static_cast<double>(action->motionMagnitude54) - 0.11F;
            action->motionMagnitude54 = static_cast<float>(speed);
            const double x = speed + action->origin_x_offset40;
            action->origin_x_offset40 = static_cast<float>(x);
            action->origin_y_offset44 = static_cast<float>(
                action->origin_y_offset44 - speed * 0.14F);
            if (x < 0.0) {
                action->origin_x_offset40 = 0.0F;
                action->origin_y_offset44 = 0.0F;
                action->motionMagnitude54 = 0.0F;
                ++action->characterAction;
            }
        } else {
            action->origin_x_offset40 = static_cast<float>(
                static_cast<double>(action->origin_x_offset40) + action->motionMagnitude54);
            action->origin_y_offset44 = static_cast<float>(
                action->origin_y_offset44
                - static_cast<double>(action->motionMagnitude54) * 0.14F);
        }
    } else if (action->characterAction == SceneEffectMode_MovingOut) {
        const double speed = static_cast<double>(action->motionMagnitude54) + 0.002F;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (speed > 1.5F) {
            action->motionMagnitude54 = 1.5F;
        }
        action->origin_x_offset40 = static_cast<float>(
            static_cast<double>(action->origin_x_offset40) - action->motionMagnitude54);
        action->origin_y_offset44 = static_cast<float>(
            static_cast<double>(action->motionMagnitude54) * 0.2F + action->origin_y_offset44);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*action->ownerSession, action->worldX, action->worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenX) + action->origin_x_offset40 + 0.5));
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenY) + action->origin_y_offset44 + 0.5));
}

// 原[448810,448A4F)：mode0写计时槽，此时floor实参表示已过秒数。
static void ConfigureRightEntrySceneEffect_448810(
    session::ClientSession& session,
    const std::int32_t mode, const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    if (mode == 0) {
        session.Yobi().mapEffectDurationSeconds = duration;
        const std::uint64_t tick = platform::ReadTickCount64();
        session.Yobi().mapEffectStartTick = tick - static_cast<std::uint64_t>(static_cast<std::int64_t>(floor) * 1000);
        return;
    }
    if (session.WorldScene().sceneType != 9) {
        return;
    }
    if (mode == SceneEffectMode_RemoveIfPresent) {
        YobiAction* const previous = FindLiveActionByCallback(session, UpdateRightEntrySceneEffect_448A50);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    YobiAction* action = FindLiveActionByCallback(session, UpdateRightEntrySceneEffect_448A50);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(session, 3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = UpdateRightEntrySceneEffect_448A50;
    InitializeSceneEffectGraphic(action, 14647, created);
    action->graphicOffsetX += 86;
    action->graphicOffsetY -= 93;
    action->characterAction = mode;
    action->displayPriority = 11;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case SceneEffectMode_MovingIn:
        action->origin_x_offset40 = 870.0F;
        action->origin_y_offset44 = -121.8F;
        action->motionMagnitude54 = 1.0F;
        break;
    case SceneEffectMode_Stationary:
        action->origin_x_offset40 = 0.0F;
        action->motionMagnitude54 = 0.0F;
        break;
    case SceneEffectMode_MovingOut:
        PlaySceneEffectSounds();
        action->origin_x_offset40 = 0.0F;
        action->origin_y_offset44 = 0.0F;
        action->motionMagnitude54 = 0.0F;
        break;
    default:
        MarkDeath(action);
        return;
    }
    action->worldX = static_cast<float>(mapX * 64.0);
    action->worldY = static_cast<float>(mapY * 64.0);
}

// 原[448D90,44905A)：四种负类型图形生产，先按mode4查重再筛类型。
static void ConfigureLeftEntrySceneEffect_448D90(
    session::ClientSession& session,
    const std::int32_t value, const std::int32_t /*unused*/,
    const std::int32_t floor, const std::int32_t mapX, const std::int32_t mapY)
{
    const std::int32_t type = value / -100;
    const std::int32_t mode = -value % 100;
    if (mode == 0 || session.WorldScene().sceneType != 9) {
        return;
    }
    if (mode == SceneEffectMode_RemoveIfPresent) {
        YobiAction* const previous = FindLiveActionByCallback(session, UpdateLeftEntrySceneEffect_448BF0);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    std::int32_t image;
    switch (type) {
    case SceneEffectGraphicType_Image14645: image = 14645; break;
    case SceneEffectGraphicType_Image14646: image = 14646; break;
    case SceneEffectGraphicType_Image301122: image = 301122; break;
    case SceneEffectGraphicType_Image1002065: image = 1002065; break;
    default: return;
    }
    YobiAction* action = FindLiveActionByCallback(session, UpdateLeftEntrySceneEffect_448BF0);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(session, 3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = UpdateLeftEntrySceneEffect_448BF0;
    InitializeSceneEffectGraphic(action, image, created);
    switch (type) {
    case SceneEffectGraphicType_Image14645:
        action->graphicOffsetX -= 189;
        action->graphicOffsetY += 217;
        break;
    case SceneEffectGraphicType_Image14646:
        action->graphicOffsetX -= 215;
        action->graphicOffsetY -= 220;
        break;
    case SceneEffectGraphicType_Image301122:
        action->graphicOffsetX -= 253;
        action->graphicOffsetY += 217;
        break;
    case SceneEffectGraphicType_Image1002065:
        action->graphicOffsetX -= 279;
        action->graphicOffsetY -= 124;
        break;
    }
    action->characterAction = mode;
    action->displayPriority = 4;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case SceneEffectMode_MovingIn:
        action->origin_x_offset40 = -870.0F;
        action->origin_y_offset44 = 121.8F;
        action->motionMagnitude54 = 1.0F;
        break;
    case SceneEffectMode_Stationary:
        action->origin_x_offset40 = 0.0F;
        action->motionMagnitude54 = 0.0F;
        break;
    case SceneEffectMode_MovingOut:
        PlaySceneEffectSounds();
        action->origin_x_offset40 = 0.0F;
        action->origin_y_offset44 = 0.0F;
        action->motionMagnitude54 = 0.0F;
        break;
    default:
        MarkDeath(action);
        return;
    }
    action->worldX = static_cast<float>(mapX * 64.0);
    action->worldY = static_cast<float>(mapY * 64.0);
}

// 原[449A00,449BBD)：普通路移动/投影，首帧只置CryptInt标记。
static void UpdateMovingSceneEffect_449A00(YobiAction* const action)
{
    if (world::IsMapFloorAllowed_448240(*action->ownerSession) == 0) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = action->ownerSession->WorldScene().map.floor;
    if (static_cast<std::int32_t>(action->mapY1F0) != floor
        || action->ownerSession->WorldScene().player->action == nullptr) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == SceneEffectMode_MovingIn) {
        const double x = action->worldX - 0.75;
        action->worldX = static_cast<float>(x);
        action->worldY = static_cast<float>(action->worldY + 0.75);
        const auto targetX = static_cast<std::int32_t>(static_cast<std::int64_t>(x));
        if ((action->mapX200 >= static_cast<std::int32_t>(targetX))) {
            const auto targetY = static_cast<std::int32_t>(
                static_cast<std::int64_t>(action->worldY));
            if ((action->mapY210 <= static_cast<std::int32_t>(targetY))) {
                action->worldX = static_cast<float>(
                    static_cast<std::int32_t>(action->mapX200));
                action->worldY = static_cast<float>(
                    static_cast<std::int32_t>(action->mapY210));
                ++action->characterAction;
            }
        }
    } else if (action->characterAction == SceneEffectMode_MovingOut) {
        action->worldX = static_cast<float>(action->worldX - 0.5);
        action->worldY = static_cast<float>(action->worldY - 0.5);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*action->ownerSession, action->worldX, action->worldY, &screenX, &screenY);
    const auto offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->origin_x_offset40 + 0.5));
    const auto projectedX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenX + 0.5));
    action->x = offsetX + projectedX;
    const auto offsetY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->origin_y_offset44 + 0.5));
    const auto projectedY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenY + 0.5));
    action->y = offsetY + projectedY;
    if ((action->mapX1E0 == 0)) {
        action->mapX1E0 = std::int32_t{1};
    } else {
        static_cast<void>(ApplyRippleOffsetToSceneEffect_4496C0(action));
    }
}

// 原[449C10,449C8B)：每轮重查普通路父节点，播放后复制父屏幕坐标。
static void UpdateSceneEffectOverlay_449C10(YobiAction* const action)
{
    YobiAction* const parent = FindLiveActionByCallback(*action->ownerSession, UpdateMovingSceneEffect_449A00);
    if (parent == nullptr || world::IsMapFloorAllowed_448240(*action->ownerSession) == 0) {
        MarkDeath(action);
        return;
    }
    action->imageNo = parent->characterAction == SceneEffectMode_MovingIn ? 0 : 1;
    static_cast<void>(AdvanceSceneEffectTableFrame_449570(action, SceneEffectFrameGroup_Overlay));
    action->x = parent->x;
    action->y = parent->y;
}

// 原[449BC0,449C0A)：按callback去重附属节点，不拥有父引用。
static YobiAction* EnsureSceneEffectOverlay_449BC0(session::ClientSession& session)
{
    YobiAction* action = FindLiveActionByCallback(session, UpdateSceneEffectOverlay_449C10);
    if (action != nullptr) {
        return action;
    }
    YobiAction* const parent = FindLiveActionByCallback(session, UpdateMovingSceneEffect_449A00);
    if (parent == nullptr) {
        return nullptr;
    }
    action = NewYobiAction_404AE0(session, static_cast<std::uint8_t>(parent->priority + 1), 0);
    if (action != nullptr) {
        action->callback = UpdateSceneEffectOverlay_449C10;
        action->displayPriority = static_cast<std::uint8_t>(parent->displayPriority + 1);
    }
    return action;
}

// 原[4483A0,448810)：计时偏移与100块背景入队，关联对象缺失不补零。
static void UpdateTimedMapBackgroundEffect_4483A0(YobiAction* const action)
{
    if (action->ownerSession->WorldScene().sceneType != 9) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = action->ownerSession->WorldScene().map.floor;
    if (floor != 59529
        && static_cast<std::int32_t>(action->mapY1F0) != floor
        && action->ownerSession->WorldScene().map.floor != 38251) {
        MarkDeath(action);
        return;
    }
    YobiAction* const associated = FindLiveActionByCallback(*action->ownerSession, UpdateLeftEntrySceneEffect_448BF0);
    const std::uint64_t tick = platform::ReadTickCount64();
    const auto elapsed = std::bit_cast<std::int64_t>(tick - action->ownerSession->Yobi().mapEffectStartTick);
    std::int32_t duration = action->ownerSession->Yobi().mapEffectDurationSeconds;
    const std::int32_t total = duration * 1000;
    std::int32_t progress = 0;
    if (total != 0) {
        progress = static_cast<std::int32_t>(elapsed * 130 / total);
    }
    if (elapsed > total || elapsed < 0) {
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 = 0;
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = 0;
    } else if (elapsed > 200 * (5 * duration - 75)) {
        if (elapsed < 200 * (5 * duration - 10) && elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 =
                std::bit_cast<std::uint32_t>(associated->motionMagnitude54);
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->motionMagnitude54) * -0.14F));
        }
    } else if (elapsed > 200 * (5 * duration - 80)) {
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 = 0;
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = 0;
    } else if (elapsed > 200 * (5 * duration - 85)) {
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        const double amplitude = static_cast<double>(action->motionMagnitude54) - 0.025F;
        action->motionMagnitude54 = static_cast<float>(amplitude);
        if (amplitude < 0.0) {
            action->motionMagnitude54 = 0.0F;
        }
        platform::detail::Cg2dMapTimedOffset(
            (progress + 250) % 360, &action->motionMagnitude54,
            &action->ownerSession->WorldScene().mapEffects.scene.valueC8A758, &action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C);
    } else if (elapsed <= 14000) {
        if (elapsed >= 13000 && elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 =
                std::bit_cast<std::uint32_t>(-associated->motionMagnitude54);
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->motionMagnitude54) * 0.14F));
        }
    } else if (elapsed <= 15000) {
        if (elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            const double amplitude = static_cast<double>(associated->motionMagnitude54) - 0.025F;
            associated->motionMagnitude54 = static_cast<float>(amplitude);
            if (amplitude < 0.0) {
                associated->motionMagnitude54 = 0.0F;
            }
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 =
                std::bit_cast<std::uint32_t>(-associated->motionMagnitude54);
            action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->motionMagnitude54) * 0.14F));
        }
    } else if (elapsed <= 16000) {
        if (elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A758 = 0;
        action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C = 0;
    } else {
        if (elapsed <= 200 * (5 * duration - 150) && elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
            duration = action->ownerSession->Yobi().mapEffectDurationSeconds;
        }
        if (elapsed % 120 == 80) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
            duration = action->ownerSession->Yobi().mapEffectDurationSeconds;
        }
        const bool increasing = elapsed <= 200 * (5 * duration - 125);
        platform::detail::Cg2dMapTimedOffset(
            (progress + 250) % 360, &action->motionMagnitude54,
            &action->ownerSession->WorldScene().mapEffects.scene.valueC8A758, &action->ownerSession->WorldScene().mapEffects.scene.valueC8A75C);
        if (increasing) {
            const double amplitude = static_cast<double>(action->motionMagnitude54) + 0.1F;
            action->motionMagnitude54 = static_cast<float>(amplitude);
            if (amplitude > 15.0F) {
                action->motionMagnitude54 = 15.0F;
            }
        } else {
            const double amplitude = static_cast<double>(action->motionMagnitude54) - 0.15F;
            action->motionMagnitude54 = static_cast<float>(amplitude);
            if (amplitude < 1.0F) {
                action->motionMagnitude54 = 1.0F;
            }
        }
    }
    for (std::int32_t y = 0; y < 480; y += 48) {
        for (std::int32_t x = 0; x < 640; x += 64) {
            static_cast<void>(render::DrawGraphic3_520970(
                x, y, 0, 14648, 0, 1, 0, &action->drawMode));
        }
    }
}

// 原[448290,44836B)：限定场景与地图的计时回调注册，不清既有偏移。
static void EnsureTimedMapBackgroundEffect_448290(session::ClientSession& session)
{
    if (session.WorldScene().sceneType != 9 || session.WorldScene().player->action == nullptr) {
        return;
    }
    const std::int32_t floor = session.WorldScene().map.floor;
    if (floor != 59529 && floor != 38251) {
        return;
    }
    if (FindLiveActionByCallback(session, UpdateTimedMapBackgroundEffect_4483A0) != nullptr) {
        return;
    }
    YobiAction* const action = NewYobiAction_404AE0(session, 3, 0);
    if (action == nullptr) {
        return;
    }
    action->callback = UpdateTimedMapBackgroundEffect_4483A0;
    action->displayPriority = 1;
    action->mapY1F0 = static_cast<std::int32_t>(session.WorldScene().map.floor);
    action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
    action->motionMagnitude54 = 0.0F;
    action->drawMode.clipMode = 1;
    const std::int16_t sourceX = -320;
    const std::int16_t sourceY = 340;
    const std::int16_t width = 64;
    const std::int16_t height = 48;
    action->drawMode.sourceX = sourceX;
    action->drawMode.sourceY = sourceY;
    action->drawMode.sourceWidth = width;
    action->drawMode.sourceHeight = height;
}

// 原[449670,4496B7)：先归一化角度，两次查表后各自重读幅值。
static void MoveSceneEffectAlongAngle_449670(YobiAction* const action)
{
    float angle = static_cast<float>(270.0 - action->angle50);
    world::NormalizeAngleDegrees_46F8F0(&angle);
    const double xFactor = world::LookupCosineDegrees_46F950(angle);
    action->origin_x_offset40 = static_cast<float>(
        action->origin_x_offset40 - xFactor * action->motionMagnitude54);
    const double yFactor = world::LookupSineDegrees_46F930(angle);
    action->origin_y_offset44 = static_cast<float>(
        yFactor * action->motionMagnitude54 + action->origin_y_offset44);
}

// 原[449400,449552)：漂移对象的移动、帧表、投影及四方向退出。
static void UpdateDriftingSceneEffect_449400(YobiAction* const action)
{
    if (world::IsMapFloorAllowed_448240(*action->ownerSession) == 0) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = action->ownerSession->WorldScene().map.floor;
    if (static_cast<std::int32_t>(action->mapY1F0) != floor) {
        MarkDeath(action);
        return;
    }
    MoveSceneEffectAlongAngle_449670(action);
    static_cast<void>(AdvanceSceneEffectTableFrame_449570(action, SceneEffectFrameGroup_Drift));
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(*action->ownerSession, action->worldX, action->worldY, &screenX, &screenY);
    const auto offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->origin_x_offset40 + 0.5));
    const auto projectedX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenX + 0.5));
    action->x = offsetX + projectedX;
    const auto offsetY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->origin_y_offset44 + 0.5));
    const auto projectedY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenY + 0.5));
    const std::int32_t image = action->imageNo;
    action->y = offsetY + projectedY;
    if (image == SceneDriftVariant_RippleWithExitVariant0) {
        static_cast<void>(ApplyRippleOffsetToSceneEffect_4496C0(action));
    }
    switch (action->imageNo & 3) {
    case SceneDriftVariant_FromPositiveWorldX:
        if (action->x < -150 || action->y > 630) {
            MarkDeath(action);
        }
        break;
    case SceneDriftVariant_FromNegativeWorldX:
        if (action->x > 790 || action->y < -150) {
            MarkDeath(action);
        }
        break;
    case SceneDriftVariant_FromNegativeWorldY:
        if (action->x > 790 || action->y > 630) {
            MarkDeath(action);
        }
        break;
    case SceneDriftVariant_FromPositiveWorldY:
        if (action->x < -150 || action->y < -150) {
            MarkDeath(action);
        }
        break;
    }
}

} // namespace detail

void RetainSceneEffectGraphicReferences()
{
    detail::RetainDirectFrameGraphics();
}

// 原函数：0x00449700。
void ConfigureMapSceneEffect_449700(
    session::ClientSession& session,
    const std::int32_t mode, const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    if (mode >= 100) {
        detail::ConfigureRightEntrySceneEffect_448810(session, mode % 100, duration, floor, mapX, mapY);
        return;
    }
    if (mode < 0) {
        detail::ConfigureLeftEntrySceneEffect_448D90(session, mode, duration, floor, mapX, mapY);
        return;
    }
    if (mode == 0) {
        return;
    }
    if (mode == detail::SceneEffectMode_RemoveIfPresent) {
        YobiAction* const previous = detail::FindLiveActionByCallback(session, detail::UpdateMovingSceneEffect_449A00);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    YobiAction* action = detail::FindLiveActionByCallback(session, detail::UpdateMovingSceneEffect_449A00);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(session, 3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = detail::UpdateMovingSceneEffect_449A00;
    detail::InitializeSceneEffectGraphic(action, 230058, created);
    action->characterAction = mode;
    action->displayPriority = 3;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case detail::SceneEffectMode_MovingIn: {
        action->mapX200 = static_cast<std::int32_t>(mapX * 64);
        action->mapY210 = static_cast<std::int32_t>(mapY * 64);
        action->worldX = static_cast<float>((mapX + 10) * 64.0);
        action->worldY = static_cast<float>((mapY - 10) * 64.0);
        break;
    }
    case detail::SceneEffectMode_MovingOut: {
        action->mapX200 = static_cast<std::int32_t>((mapX - 10) * 64);
        action->mapY210 = static_cast<std::int32_t>((mapY - 10) * 64);
        [[fallthrough]];
    }
    case detail::SceneEffectMode_Stationary:
        action->worldX = static_cast<float>(mapX * 64.0);
        action->worldY = static_cast<float>(mapY * 64.0);
        break;
    default:
        MarkDeath(action);
        return;
    }
    static_cast<void>(detail::EnsureSceneEffectOverlay_449BC0(session));
}

// 原[449160,4493AA)：共享玩家只作入口门禁，生成时按轴重取玩家T*。
void UpdateMapEnvironmentEffects_449160(session::ClientSession& session)
{
    if (session.WorldScene().player->action == nullptr) {
        return;
    }
    detail::EnsureTimedMapBackgroundEffect_448290(session);
    render::AdvanceRipplePhase_520910(session);
    if (world::IsMapDriftEffectAllowed_4493C0(session) == 0) {
        return;
    }
    const auto counter = static_cast<std::uint8_t>(session.Yobi().mapEffectSpawnCounter + 1);
    session.Yobi().mapEffectSpawnCounter = counter;
    if ((counter & 0x3Fu) != 0) {
        return;
    }
    YobiAction* const action = NewYobiAction_404AE0(session, 3, 0);
    if (action == nullptr) {
        return;
    }
    action->callback = detail::UpdateDriftingSceneEffect_449400;
    action->displayPriority = 30;
    action->mapY1F0 = static_cast<std::int32_t>(session.WorldScene().map.floor);
    const auto timeCategory = static_cast<std::uint8_t>(session.WorldScene().map.currentDayPeriod);
    action->imageNo = (timeCategory & 1u) != 0
        ? platform::RandomRange_46FAF0(session.Connection(), detail::SceneDriftVariant_FromPositiveWorldX, detail::SceneDriftVariant_FromNegativeWorldX) : platform::RandomRange_46FAF0(session.Connection(), detail::SceneDriftVariant_FromNegativeWorldY, detail::SceneDriftVariant_FromPositiveWorldY);
    const std::int32_t speed = platform::RandomRange_46FAF0(session.Connection(), 0, 200);
    action->motionMagnitude54 = static_cast<float>(static_cast<double>(speed) * 0.01F + 2.0F);
    const std::int32_t offset = platform::RandomRange_46FAF0(session.Connection(), 0, 2048) - 1024;
    switch (action->imageNo & 3) {
    case detail::SceneDriftVariant_FromPositiveWorldX:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(session.Connection(), 0, 40) + 213);
        action->worldX = static_cast<float>(
            session.WorldScene().player->action->worldX + 550.0);
        action->worldY = static_cast<float>(
            static_cast<double>(offset) + session.WorldScene().player->action->worldY);
        break;
    case detail::SceneDriftVariant_FromNegativeWorldX:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(session.Connection(), 0, 40) + 33);
        action->worldX = static_cast<float>(
            session.WorldScene().player->action->worldX - 400.0 - 150.0F);
        action->worldY = static_cast<float>(
            static_cast<double>(offset) + session.WorldScene().player->action->worldY);
        break;
    case detail::SceneDriftVariant_FromNegativeWorldY:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(session.Connection(), 0, 40) + 107);
        action->worldX = static_cast<float>(
            static_cast<double>(offset) + session.WorldScene().player->action->worldX);
        action->worldY = static_cast<float>(
            session.WorldScene().player->action->worldY - 400.0 - 150.0F);
        break;
    case detail::SceneDriftVariant_FromPositiveWorldY:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(session.Connection(), 0, 40) + 287);
        action->worldX = static_cast<float>(
            static_cast<double>(offset) + session.WorldScene().player->action->worldX);
        action->worldY = static_cast<float>(
            session.WorldScene().player->action->worldY + 550.0);
        break;
    }
}

} // namespace cg::yobi
