#include "environment.hpp"
#include "base.hpp"
#include "../platform/MapEffectMath.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"

#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

volatile std::uint32_t gYobiScreenOffsetRising = 0; // 原 0x59F9F4。

volatile std::uint32_t gYobiScreenOffsetX = 0; // 原 0x59F9FC。

volatile std::uint32_t gYobiScreenOffsetY = 0; // 原 0x59FA00。

volatile std::uint32_t gYobiScreenOffsetAmplitude = 0; // 原 0x59FA04。

volatile std::uint32_t gYobiScreenOffsetMaximumAmplitude = 4; // 原 0x553C58。

namespace detail {

// 原448810模式0生产，4483A0消费；两槽均属PE虚拟零区。
static std::int32_t gDuration5AC4B8 = 0; // 原全局：0x005AC4B8。

static std::uint64_t gStartTick5F973C = 0; // 原全局：0x005F973C。

static std::uint8_t gEffectCounter5F9755 = 0; // 原全局：0x005F9755。 原449160独占的U8生成计数，非相邻UI标志。

struct SceneEffectFrame {
    std::int32_t duration; // -1终止、-2跳转，其他值为帧时长。
    std::int32_t graphic; // 通常mapNo；group1/image4为index，-2记录为目标帧。
};

static_assert(sizeof(SceneEffectFrame) == 8);

static constexpr SceneEffectFrame kFrames55433C[] { // 原常量参考：0x0055433C。
    {24, 0x38C54}, {24, 0x38C55}, {24, 0x38C56}, {24, 0x38C57},
    {24, 0x38C58}, {24, 0x38C59}, {24, 0x38C5A}, {24, 0x38C5B},
    {24, 0x38C5C}, {24, 0x38C5D}, {24, 0x38C5E}, {24, 0x38C5F},
    {24, 0x38C60}, {24, 0x38C61}, {24, 0x38C62}, {24, 0x38C63}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames5543C4[] { // 原常量参考：0x005543C4。
    {16, 0x38C54}, {16, 0x38C55}, {16, 0x38C56}, {16, 0x38C57},
    {16, 0x38C58}, {16, 0x38C59}, {16, 0x38C5A}, {16, 0x38C5B},
    {16, 0x38C5C}, {16, 0x38C5D}, {16, 0x38C5E}, {16, 0x38C5F},
    {16, 0x38C60}, {16, 0x38C61}, {16, 0x38C62}, {16, 0x38C63}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames554454[] { // 原常量参考：0x00554454。
    {8, 0x38C3C}, {8, 0x38C3D}, {8, 0x38C3E}, {8, 0x38C3F},
    {4, 0x38C40}, {4, 0x38C41}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames55448C[] { // 原常量参考：0x0055448C。
    {8, 0x38C48}, {8, 0x38C49}, {8, 0x38C4A}, {8, 0x38C4B},
    {4, 0x38C4C}, {4, 0x38C4D}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames5544C4[] { // 原常量参考：0x005544C4。
    {8, 0x38C42}, {8, 0x38C43}, {8, 0x38C44}, {8, 0x38C45},
    {4, 0x38C46}, {4, 0x38C47}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames5544FC[] { // 原常量参考：0x005544FC。
    {8, 0x38C4E}, {8, 0x38C4F}, {8, 0x38C50}, {8, 0x38C51},
    {4, 0x38C52}, {4, 0x38C53}, {-2, 0},
};

static constexpr SceneEffectFrame kFrames554534[] { // 原常量参考：0x00554534。
    {240, 227336}, {8, 227335}, {30, 227334}, {8, 227335}, {-2, 0},
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

// 原函数：0x00448370。
YobiAction* Sub_448370(void (*callback)(YobiAction*))
{
    // 原 0x448370/0x448379/0x448380：先读首槽、缓存尾槽，再读 start->next。
    YobiAction* const start = gYobiActionStart;
    YobiAction* const end = gYobiActionEnd;
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
std::int32_t Sub_449570(YobiAction* const action, const std::int32_t group)
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
        if (frame->duration == -1) {
            return 1;
        }
        if (frame->duration == -2) {
            action->frameIndex = static_cast<std::uint32_t>(frame->graphic);
            frame = &kFrameGroups554570[group][image][action->frameIndex];
        }
        action->frameRemain = frame->duration;
        std::int32_t graphicIndex;
        if (group == 1 && image == 4) {
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
std::uint32_t Sub_4496C0(YobiAction* const action)
{
    const std::uint32_t phase = render::gRippleIntegerPhaseBits;
    const std::int32_t y = action->y;
    const std::uint32_t index = (static_cast<std::uint32_t>(y >> 3) + phase) & 0x3Fu;
    const std::int32_t sine = render::gRippleSineTable[index];
    const std::uint32_t strengthBits = render::gRippleStrengthBits;
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
        action->offset40 = 0.0F;
        action->offset44 = 0.0F;
        action->value54 = 0.25F;
        action->angle50 = 0.0F;
        action->field58 = 0;
    }
}

// 原[448A50,448BEF)：正类型对象回调，保持FST后仍使用寄存器值的边界。
void Sub_448A50(YobiAction* const action)
{
    const std::int32_t floor = world::gMapFloor;
    if (action->mapY1F0.value() != floor) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == 1) {
        if (action->offset40 < 0.0F) {
            if (action->value54 > 0.0F
                && static_cast<double>(action->value54) - 0.11F <= 0.0) {
                PlaySceneEffectSounds();
            }
            const double speed = static_cast<double>(action->value54) - 0.11F;
            action->value54 = static_cast<float>(speed);
            const double x = action->offset40 - speed;
            action->offset40 = static_cast<float>(x);
            action->offset44 = static_cast<float>(
                speed * 0.14F + action->offset44);
            if (x > 0.0) {
                action->offset40 = 0.0F;
                action->offset44 = 0.0F;
                action->value54 = 0.0F;
                ++action->characterAction;
            }
        } else {
            action->offset40 = static_cast<float>(
                static_cast<double>(action->offset40) - action->value54);
            action->offset44 = static_cast<float>(
                static_cast<double>(action->value54) * 0.14F + action->offset44);
        }
    } else if (action->characterAction == 3) {
        const double speed = static_cast<double>(action->value54) + 0.002F;
        action->value54 = static_cast<float>(speed);
        if (speed > 1.5F) {
            action->value54 = 1.5F;
        }
        action->offset40 = static_cast<float>(
            static_cast<double>(action->offset40) + action->value54);
        action->offset44 = static_cast<float>(
            action->offset44
            - static_cast<double>(action->value54) * 0.2F);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(action->worldX, action->worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenX) + action->offset40 + 0.5));
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenY) + action->offset44 + 0.5));
}

// 原[448BF0,448D8F)：负类型对象回调，偏移推进方向与448A50相反。
void Sub_448BF0(YobiAction* const action)
{
    const std::int32_t floor = world::gMapFloor;
    if (action->mapY1F0.value() != floor) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == 1) {
        if (action->offset40 > 0.0F) {
            if (action->value54 > 0.0F
                && static_cast<double>(action->value54) - 0.11F <= 0.0) {
                PlaySceneEffectSounds();
            }
            const double speed = static_cast<double>(action->value54) - 0.11F;
            action->value54 = static_cast<float>(speed);
            const double x = speed + action->offset40;
            action->offset40 = static_cast<float>(x);
            action->offset44 = static_cast<float>(
                action->offset44 - speed * 0.14F);
            if (x < 0.0) {
                action->offset40 = 0.0F;
                action->offset44 = 0.0F;
                action->value54 = 0.0F;
                ++action->characterAction;
            }
        } else {
            action->offset40 = static_cast<float>(
                static_cast<double>(action->offset40) + action->value54);
            action->offset44 = static_cast<float>(
                action->offset44
                - static_cast<double>(action->value54) * 0.14F);
        }
    } else if (action->characterAction == 3) {
        const double speed = static_cast<double>(action->value54) + 0.002F;
        action->value54 = static_cast<float>(speed);
        if (speed > 1.5F) {
            action->value54 = 1.5F;
        }
        action->offset40 = static_cast<float>(
            static_cast<double>(action->offset40) - action->value54);
        action->offset44 = static_cast<float>(
            static_cast<double>(action->value54) * 0.2F + action->offset44);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(action->worldX, action->worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenX) + action->offset40 + 0.5));
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(screenY) + action->offset44 + 0.5));
}

// 原[448810,448A4F)：mode0写计时槽，此时floor实参表示已过秒数。
void Sub_448810(
    const std::int32_t mode, const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    if (mode == 0) {
        gDuration5AC4B8 = duration;
        const std::uint64_t tick = platform::ReadTickCount64();
        gStartTick5F973C = tick - static_cast<std::uint64_t>(static_cast<std::int64_t>(floor) * 1000);
        return;
    }
    if (world::gCurrentSceneType != 9) {
        return;
    }
    if (mode == 4) {
        YobiAction* const previous = Sub_448370(Sub_448A50);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    YobiAction* action = Sub_448370(Sub_448A50);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = Sub_448A50;
    InitializeSceneEffectGraphic(action, 14647, created);
    action->graphicOffsetX += 86;
    action->graphicOffsetY -= 93;
    action->characterAction = mode;
    action->displayPriority = 11;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case 1:
        action->offset40 = 870.0F;
        action->offset44 = -121.8F;
        action->value54 = 1.0F;
        break;
    case 2:
        action->offset40 = 0.0F;
        action->value54 = 0.0F;
        break;
    case 3:
        PlaySceneEffectSounds();
        action->offset40 = 0.0F;
        action->offset44 = 0.0F;
        action->value54 = 0.0F;
        break;
    default:
        MarkDeath(action);
        return;
    }
    action->worldX = static_cast<float>(mapX * 64.0);
    action->worldY = static_cast<float>(mapY * 64.0);
}

// 原[448D90,44905A)：四种负类型图形生产，先按mode4查重再筛类型。
void Sub_448D90(
    const std::int32_t value, const std::int32_t /*unused*/,
    const std::int32_t floor, const std::int32_t mapX, const std::int32_t mapY)
{
    const std::int32_t type = value / -100;
    const std::int32_t mode = -value % 100;
    if (mode == 0 || world::gCurrentSceneType != 9) {
        return;
    }
    if (mode == 4) {
        YobiAction* const previous = Sub_448370(Sub_448BF0);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    std::int32_t image;
    switch (type) {
    case 100: image = 14645; break;
    case 101: image = 14646; break;
    case 201: image = 301122; break;
    case 202: image = 1002065; break;
    default: return;
    }
    YobiAction* action = Sub_448370(Sub_448BF0);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = Sub_448BF0;
    InitializeSceneEffectGraphic(action, image, created);
    switch (type) {
    case 100:
        action->graphicOffsetX -= 189;
        action->graphicOffsetY += 217;
        break;
    case 101:
        action->graphicOffsetX -= 215;
        action->graphicOffsetY -= 220;
        break;
    case 201:
        action->graphicOffsetX -= 253;
        action->graphicOffsetY += 217;
        break;
    case 202:
        action->graphicOffsetX -= 279;
        action->graphicOffsetY -= 124;
        break;
    }
    action->characterAction = mode;
    action->displayPriority = 4;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case 1:
        action->offset40 = -870.0F;
        action->offset44 = 121.8F;
        action->value54 = 1.0F;
        break;
    case 2:
        action->offset40 = 0.0F;
        action->value54 = 0.0F;
        break;
    case 3:
        PlaySceneEffectSounds();
        action->offset40 = 0.0F;
        action->offset44 = 0.0F;
        action->value54 = 0.0F;
        break;
    default:
        MarkDeath(action);
        return;
    }
    action->worldX = static_cast<float>(mapX * 64.0);
    action->worldY = static_cast<float>(mapY * 64.0);
}

// 原[449A00,449BBD)：普通路移动/投影，首帧只置CryptInt标记。
void Sub_449A00(YobiAction* const action)
{
    if (world::IsMapFloorAllowed_448240() == 0) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = world::gMapFloor;
    if (action->mapY1F0.value() != floor
        || world::gPlayerRuntimeSlots.action == nullptr) {
        MarkDeath(action);
        return;
    }
    if (action->characterAction == 1) {
        const double x = action->worldX - 0.75;
        action->worldX = static_cast<float>(x);
        action->worldY = static_cast<float>(action->worldY + 0.75);
        const auto targetX = static_cast<std::int32_t>(static_cast<std::int64_t>(x));
        if ((action->mapX200 >= static_cast<std::int32_t>(targetX))) {
            const auto targetY = static_cast<std::int32_t>(
                static_cast<std::int64_t>(action->worldY));
            if ((action->mapY210 <= static_cast<std::int32_t>(targetY))) {
                action->worldX = static_cast<float>(
                    action->mapX200.value());
                action->worldY = static_cast<float>(
                    action->mapY210.value());
                ++action->characterAction;
            }
        }
    } else if (action->characterAction == 3) {
        action->worldX = static_cast<float>(action->worldX - 0.5);
        action->worldY = static_cast<float>(action->worldY - 0.5);
    }
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(action->worldX, action->worldY, &screenX, &screenY);
    const auto offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->offset40 + 0.5));
    const auto projectedX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenX + 0.5));
    action->x = offsetX + projectedX;
    const auto offsetY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->offset44 + 0.5));
    const auto projectedY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenY + 0.5));
    action->y = offsetY + projectedY;
    if ((action->mapX1E0 == 0)) {
        action->mapX1E0 = std::int32_t{1};
    } else {
        static_cast<void>(Sub_4496C0(action));
    }
}

// 原[449C10,449C8B)：每轮重查普通路父节点，播放后复制父屏幕坐标。
void Sub_449C10(YobiAction* const action)
{
    YobiAction* const parent = Sub_448370(Sub_449A00);
    if (parent == nullptr || world::IsMapFloorAllowed_448240() == 0) {
        MarkDeath(action);
        return;
    }
    action->imageNo = parent->characterAction == 1 ? 0 : 1;
    static_cast<void>(Sub_449570(action, 0));
    action->x = parent->x;
    action->y = parent->y;
}

// 原[449BC0,449C0A)：按callback去重附属节点，不拥有父引用。
YobiAction* Sub_449BC0()
{
    YobiAction* action = Sub_448370(Sub_449C10);
    if (action != nullptr) {
        return action;
    }
    YobiAction* const parent = Sub_448370(Sub_449A00);
    if (parent == nullptr) {
        return nullptr;
    }
    action = NewYobiAction_404AE0(static_cast<std::uint8_t>(parent->priority + 1), 0);
    if (action != nullptr) {
        action->callback = Sub_449C10;
        action->displayPriority = static_cast<std::uint8_t>(parent->displayPriority + 1);
    }
    return action;
}

// 原[4483A0,448810)：计时偏移与100块背景入队，关联对象缺失不补零。
void Sub_4483A0(YobiAction* const action)
{
    if (world::gCurrentSceneType != 9) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = world::gMapFloor;
    if (floor != 59529
        && action->mapY1F0.value() != floor
        && world::gMapFloor != 38251) {
        MarkDeath(action);
        return;
    }
    YobiAction* const associated = Sub_448370(Sub_448BF0);
    const std::uint64_t tick = platform::ReadTickCount64();
    const auto elapsed = std::bit_cast<std::int64_t>(tick - gStartTick5F973C);
    std::int32_t duration = gDuration5AC4B8;
    const std::int32_t total = duration * 1000;
    std::int32_t progress = 0;
    if (total != 0) {
        progress = static_cast<std::int32_t>(elapsed * 130 / total);
    }
    if (elapsed > total || elapsed < 0) {
        world::gMapEffectOffsetXBits = 0;
        world::gMapEffectOffsetYBits = 0;
    } else if (elapsed > 200 * (5 * duration - 75)) {
        if (elapsed < 200 * (5 * duration - 10) && elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            world::gMapEffectOffsetXBits =
                std::bit_cast<std::uint32_t>(associated->value54);
            world::gMapEffectOffsetYBits = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->value54) * -0.14F));
        }
    } else if (elapsed > 200 * (5 * duration - 80)) {
        world::gMapEffectOffsetXBits = 0;
        world::gMapEffectOffsetYBits = 0;
    } else if (elapsed > 200 * (5 * duration - 85)) {
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        const double amplitude = static_cast<double>(action->value54) - 0.025F;
        action->value54 = static_cast<float>(amplitude);
        if (amplitude < 0.0) {
            action->value54 = 0.0F;
        }
        platform::detail::Cg2dMapTimedOffset(
            (progress + 250) % 360, &action->value54,
            &world::gMapEffectOffsetXBits, &world::gMapEffectOffsetYBits);
    } else if (elapsed <= 14000) {
        if (elapsed >= 13000 && elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            world::gMapEffectOffsetXBits =
                std::bit_cast<std::uint32_t>(-associated->value54);
            world::gMapEffectOffsetYBits = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->value54) * 0.14F));
        }
    } else if (elapsed <= 15000) {
        if (elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        if (elapsed % 40 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
        }
        if (associated != nullptr) {
            const double amplitude = static_cast<double>(associated->value54) - 0.025F;
            associated->value54 = static_cast<float>(amplitude);
            if (amplitude < 0.0) {
                associated->value54 = 0.0F;
            }
            world::gMapEffectOffsetXBits =
                std::bit_cast<std::uint32_t>(-associated->value54);
            world::gMapEffectOffsetYBits = std::bit_cast<std::uint32_t>(
                static_cast<float>(static_cast<double>(associated->value54) * 0.14F));
        }
    } else if (elapsed <= 16000) {
        if (elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
        }
        world::gMapEffectOffsetXBits = 0;
        world::gMapEffectOffsetYBits = 0;
    } else {
        if (elapsed <= 200 * (5 * duration - 150) && elapsed % 120 == 0) {
            static_cast<void>(audio::PlaySe_52C450(0x1C7, 320));
            duration = gDuration5AC4B8;
        }
        if (elapsed % 120 == 80) {
            static_cast<void>(audio::PlaySe_52C450(0x1C6, 320));
            duration = gDuration5AC4B8;
        }
        const bool increasing = elapsed <= 200 * (5 * duration - 125);
        platform::detail::Cg2dMapTimedOffset(
            (progress + 250) % 360, &action->value54,
            &world::gMapEffectOffsetXBits, &world::gMapEffectOffsetYBits);
        if (increasing) {
            const double amplitude = static_cast<double>(action->value54) + 0.1F;
            action->value54 = static_cast<float>(amplitude);
            if (amplitude > 15.0F) {
                action->value54 = 15.0F;
            }
        } else {
            const double amplitude = static_cast<double>(action->value54) - 0.15F;
            action->value54 = static_cast<float>(amplitude);
            if (amplitude < 1.0F) {
                action->value54 = 1.0F;
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
void Sub_448290()
{
    if (world::gCurrentSceneType != 9 || world::gPlayerRuntimeSlots.action == nullptr) {
        return;
    }
    const std::int32_t floor = world::gMapFloor;
    if (floor != 59529 && floor != 38251) {
        return;
    }
    if (Sub_448370(Sub_4483A0) != nullptr) {
        return;
    }
    YobiAction* const action = NewYobiAction_404AE0(3, 0);
    if (action == nullptr) {
        return;
    }
    action->callback = Sub_4483A0;
    action->displayPriority = 1;
    action->mapY1F0 = static_cast<std::int32_t>(world::gMapFloor);
    action->flags = 2;
    action->value54 = 0.0F;
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
void Sub_449670(YobiAction* const action)
{
    float angle = static_cast<float>(270.0 - action->angle50);
    world::Sub_46F8F0(&angle);
    const double xFactor = world::Sub_46F950(angle);
    action->offset40 = static_cast<float>(
        action->offset40 - xFactor * action->value54);
    const double yFactor = world::Sub_46F930(angle);
    action->offset44 = static_cast<float>(
        yFactor * action->value54 + action->offset44);
}

// 原[449400,449552)：漂移对象的移动、帧表、投影及四方向退出。
void Sub_449400(YobiAction* const action)
{
    if (world::IsMapFloorAllowed_448240() == 0) {
        MarkDeath(action);
        return;
    }
    const std::int32_t floor = world::gMapFloor;
    if (action->mapY1F0.value() != floor) {
        MarkDeath(action);
        return;
    }
    Sub_449670(action);
    static_cast<void>(Sub_449570(action, 1));
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(action->worldX, action->worldY, &screenX, &screenY);
    const auto offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->offset40 + 0.5));
    const auto projectedX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenX + 0.5));
    action->x = offsetX + projectedX;
    const auto offsetY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->offset44 + 0.5));
    const auto projectedY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        screenY + 0.5));
    const std::int32_t image = action->imageNo;
    action->y = offsetY + projectedY;
    if (image == 4) {
        static_cast<void>(Sub_4496C0(action));
    }
    switch (action->imageNo & 3) {
    case 0:
        if (action->x < -150 || action->y > 630) {
            MarkDeath(action);
        }
        break;
    case 1:
        if (action->x > 790 || action->y < -150) {
            MarkDeath(action);
        }
        break;
    case 2:
        if (action->x > 790 || action->y > 630) {
            MarkDeath(action);
        }
        break;
    case 3:
        if (action->x < -150 || action->y < -150) {
            MarkDeath(action);
        }
        break;
    }
}

} // namespace detail

// 原函数：0x00449700。
void Sub_449700(
    const std::int32_t mode, const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    if (mode >= 100) {
        detail::Sub_448810(mode % 100, duration, floor, mapX, mapY);
        return;
    }
    if (mode < 0) {
        detail::Sub_448D90(mode, duration, floor, mapX, mapY);
        return;
    }
    if (mode == 0) {
        return;
    }
    if (mode == 4) {
        YobiAction* const previous = detail::Sub_448370(detail::Sub_449A00);
        if (previous != nullptr) {
            MarkDeath(previous);
            return;
        }
    }
    YobiAction* action = detail::Sub_448370(detail::Sub_449A00);
    const bool created = action == nullptr;
    if (created) {
        action = NewYobiAction_404AE0(3, 0);
        if (action == nullptr) {
            return;
        }
    }
    action->callback = detail::Sub_449A00;
    detail::InitializeSceneEffectGraphic(action, 230058, created);
    action->characterAction = mode;
    action->displayPriority = 3;
    action->mapX1E0 = std::int32_t{0};
    action->mapY1F0 = static_cast<std::int32_t>(floor);
    switch (mode) {
    case 1: {
        action->mapX200 = static_cast<std::int32_t>(mapX * 64);
        action->mapY210 = static_cast<std::int32_t>(mapY * 64);
        action->worldX = static_cast<float>((mapX + 10) * 64.0);
        action->worldY = static_cast<float>((mapY - 10) * 64.0);
        break;
    }
    case 3: {
        action->mapX200 = static_cast<std::int32_t>((mapX - 10) * 64);
        action->mapY210 = static_cast<std::int32_t>((mapY - 10) * 64);
        [[fallthrough]];
    }
    case 2:
        action->worldX = static_cast<float>(mapX * 64.0);
        action->worldY = static_cast<float>(mapY * 64.0);
        break;
    default:
        MarkDeath(action);
        return;
    }
    static_cast<void>(detail::Sub_449BC0());
}

// 原[449160,4493AA)：共享玩家只作入口门禁，生成时按轴重取玩家T*。
void Sub_449160()
{
    if (world::gPlayerRuntimeSlots.action == nullptr) {
        return;
    }
    detail::Sub_448290();
    render::Sub_520910();
    if (world::Sub_4493C0() == 0) {
        return;
    }
    const auto counter = static_cast<std::uint8_t>(detail::gEffectCounter5F9755 + 1);
    detail::gEffectCounter5F9755 = counter;
    if ((counter & 0x3Fu) != 0) {
        return;
    }
    YobiAction* const action = NewYobiAction_404AE0(3, 0);
    if (action == nullptr) {
        return;
    }
    action->callback = detail::Sub_449400;
    action->displayPriority = 30;
    action->mapY1F0 = static_cast<std::int32_t>(world::gMapFloor);
    const auto timeCategory = static_cast<std::uint8_t>(world::gValue73ED48);
    action->imageNo = (timeCategory & 1u) != 0
        ? platform::RandomRange_46FAF0(0, 1) : platform::RandomRange_46FAF0(2, 3);
    const std::int32_t speed = platform::RandomRange_46FAF0(0, 200);
    action->value54 = static_cast<float>(static_cast<double>(speed) * 0.01F + 2.0F);
    const std::int32_t offset = platform::RandomRange_46FAF0(0, 2048) - 1024;
    switch (action->imageNo & 3) {
    case 0:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(0, 40) + 213);
        action->worldX = static_cast<float>(
            world::gPlayerRuntimeSlots.action->worldX + 550.0);
        action->worldY = static_cast<float>(
            static_cast<double>(offset) + world::gPlayerRuntimeSlots.action->worldY);
        break;
    case 1:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(0, 40) + 33);
        action->worldX = static_cast<float>(
            world::gPlayerRuntimeSlots.action->worldX - 400.0 - 150.0F);
        action->worldY = static_cast<float>(
            static_cast<double>(offset) + world::gPlayerRuntimeSlots.action->worldY);
        break;
    case 2:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(0, 40) + 107);
        action->worldX = static_cast<float>(
            static_cast<double>(offset) + world::gPlayerRuntimeSlots.action->worldX);
        action->worldY = static_cast<float>(
            world::gPlayerRuntimeSlots.action->worldY - 400.0 - 150.0F);
        break;
    case 3:
        action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(0, 40) + 287);
        action->worldX = static_cast<float>(
            static_cast<double>(offset) + world::gPlayerRuntimeSlots.action->worldX);
        action->worldY = static_cast<float>(
            world::gPlayerRuntimeSlots.action->worldY + 550.0);
        break;
    }
}

} // namespace cg::yobi
