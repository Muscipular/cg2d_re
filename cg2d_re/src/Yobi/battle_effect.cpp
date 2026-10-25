#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/platform/Startup.hpp"
#include "battle_effect.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_skill_effect.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "battle_actor.hpp"
#include "battle_display.hpp"
#include "battle_impact.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/WorldMapState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

// 原41DA04分配的132B数值载荷，41A590逐槽生成；不与战斗角色载荷混用。
struct BattleRingEmitterPayload {
    std::array<std::int32_t, 32> counts00; // 原成员偏移：+0x00。
    std::int32_t lastIndex80; // 原成员偏移：+0x80。
};

static_assert(sizeof(BattleRingEmitterPayload) == 0x84);

static_assert(offsetof(BattleRingEmitterPayload, lastIndex80) == 0x80);

// 原419CF0的12B载荷：仅父引用拓宽，419CB0按屏幕坐标和S32偏移跟随。
struct BattleFollowOffsetPayload {
    YobiAction* parent00; // 原成员偏移：+0x00。
    std::int32_t offsetX04; // 原成员偏移：+0x04。
    std::int32_t offsetY08; // 原成员偏移：+0x08。
};

static_assert(sizeof(BattleFollowOffsetPayload) == sizeof(YobiAction*) + 2 * sizeof(std::int32_t));

// 原419C10请求12B；父引用原生拓宽，尾8B保留分配足迹，419BE0不消费载荷。
struct BattleTransitionEffectPayload {
    YobiAction* parent00; // 原成员偏移：+0x00。
    std::array<std::byte, 8> bytes04; // 原+4..+BH：用途及地址属性未定，只沿通用清零保留存储。
};

static_assert(sizeof(BattleTransitionEffectPayload) == sizeof(YobiAction*) + 8);

// 角度到16方向图号的比例；编译期按double计算，保持原常量精度。
static constexpr double kBattleDirectionFrameScale = 2.0 / 45.0;

// 原立即数坐标表；顺序发射器Y的两项44/-44不同于随机上升粒子的33/-33。
static constexpr std::array<std::int32_t, 16> kBattleRadialXOffsets {
    48, 44, 34, 18, 0, -18, -34, -44, -48, -44, -34, -18, 0, 18, 34, 44,
};

static constexpr std::array<std::int32_t, 16> kBattleRisingYOffsets {
    0, 14, 26, 44, 47, 33, 26, 14, 0, -14, -26, -44, -47, -33, -26, -14,
};

static constexpr std::array<std::int32_t, 16> kBattleEmitterYOffsets {
    0, 14, 26, 44, 47, 44, 26, 14, 0, -14, -26, -44, -47, -44, -26, -14,
};

// 原553A38的12项S32等级映射；59F9E8只有工厂type34/43发布，不拥有或释放对象。
static constexpr std::array<std::int32_t, 12> kBattleEffectTierOffsets553A38 {3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0}; // 原常量参考：0x00553A38。

// BCVI01：现1025个S32位置槽的映像覆盖区为零；114按效果索引存位置编号。
std::array<std::int32_t, 1025> gBattleEffectPositions59E9D8 {}; // 原全局：0x0059E9D8。

static YobiAction* gBattleFactoryEffect59F9E8 = nullptr; // 原全局：0x0059F9E8。

// 原553A74已证六型、每型16项图号；41E890共享相位，41E920另用对象内相位。
static constexpr std::array<std::int32_t, 96> kBattleTiledEffectImages553A74 {
    28881, 28881, 28882, 28882, 28883, 28883, 28884, 28884,
    28885, 28885, 28886, 28886, 28887, 28887, 28888, 28888,
    28889, 28889, 28890, 28890, 28891, 28891, 28892, 28892,
    28893, 28893, 28894, 28894, 28895, 28895, 28896, 28896,
    28897, 28897, 28898, 28898, 28899, 28899, 28900, 28900,
    28901, 28901, 28902, 28902, 28903, 28903, 28904, 28904,
    28905, 28905, 28906, 28906, 28907, 28907, 28908, 28908,
    28909, 28909, 28910, 28910, 28911, 28911, 28912, 28912,
    28895, 28904, 28895, 28904, 28895, 28904, 28895, 28904,
    28895, 28904, 28895, 28904, 28895, 28904, 28895, 28904,
    28913, 28913, 28913, 28913, 28914, 28914, 28914, 28914,
    28915, 28915, 28915, 28915, 28916, 28916, 28916, 28916,
};

static std::int32_t gBattleTiledEffectPhase59F9EC = 0; // 原全局：0x0059F9EC。

static std::int32_t gBattleTiledEffectReverse59F9F0 = 0; // 原全局：0x0059F9F0。

// 原函数：0x0041EAB0。
YobiAction* CloneBattleAction_41EAB0(YobiAction* const source)
{
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    YobiAction* const previous = action->prev;
    YobiAction* const next = action->next;
    auto* const payload = action->payload.battleActor;
    *action = *source;
    action->next = next;
    action->prev = previous;
    action->payload.battleActor = payload;
    action->payloadKind = YobiPayloadKind::BattleActor;
    // 原载荷及E01技能元数据按成员浅复制，含用途未定字节；不回绑其中的T*。
    *payload = *sourcePayload;
    return action;
}

// 原函数：0x0041E5C0。
void UpdateBattleAfterimage_41E5C0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t count = payload->valueFF0 + 1;
    const bool expired = count >= 12;
    payload->valueFF0 = count;
    if (expired) {
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x0041E5F0。
YobiAction* NewBattleAfterimage_41E5F0(YobiAction* const source)
{
    YobiAction* const action = CloneBattleAction_41EAB0(source);
    auto* const payload = action->payload.battleActor;
    action->callback = UpdateBattleAfterimage_41E5C0;
    payload->valueFF0 = 0;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags;
    action->flags = flags | (hasDevice ? 0x100000u : 0x8000u);
    return action;
}

// 原函数：0x0041A160。
void UpdateBattleCompletingEffect_41A160(YobiAction* const action)
{
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        YobiAction* const parent = action->battleAttachmentParent10;
        if (parent != nullptr) {
            auto* const payload = parent->payload.battleActor;
            if (payload->effectPlaybackState1024 != BattleEffectPlayback_Idle) {
                payload->effectPlaybackState1024 = BattleEffectPlayback_Completed;
            }
        }
        static_cast<void>(MarkDeath(action));
    }
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// 原函数：0x0041A1C0。
void UpdateBattleParentEffect_41A1C0(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent != nullptr && parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// 原函数：0x0041A200。
void UpdateBattleRisingEffect_41A200(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    const double speed = static_cast<double>(std::bit_cast<float>(action->verticalVelocityBits4C))
        - action->effectAcceleration48;
    action->verticalVelocityBits4C = std::bit_cast<std::uint32_t>(static_cast<float>(speed));
    const double y = speed + action->offset44;
    action->offset44 = static_cast<float>(y);
    if (!(y >= action->phase)) {
        ++parent->field58;
        static_cast<void>(MarkDeath(action));
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// 原函数：0x0041A270。
void UpdateBattleScalingEffect_41A270(YobiAction* const action)
{
    constexpr double step = 0.02F;
    if (action->phase == BattleSealCardPhase_Expanding) {
        const double scaleX = action->scaleX + step;
        action->scaleX = static_cast<float>(scaleX);
        action->scaleY = static_cast<float>(action->scaleY + step);
        if (scaleX > 1.0) {
            action->phase = BattleSealCardPhase_Expanded;
            action->scaleX = 1.0F;
            action->scaleY = 1.0F;
        }
    } else if (action->phase == BattleSealCardPhase_Shrinking) {
        const double scaleX = action->scaleX - step;
        action->scaleX = static_cast<float>(scaleX);
        action->scaleY = static_cast<float>(action->scaleY - step);
        if (!(scaleX >= 0.0)) {
            action->scaleX = 0.0F;
            action->scaleY = 0.0F;
            action->phase = BattleSealCardPhase_Collapsed;
        }
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// 原函数：0x0041A310。
void UpdateBattleRandomEmitter_41A310(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
        return;
    }
    const std::int32_t oldPhase = action->phase;
    action->phase = oldPhase + 1;
    if (oldPhase < 50) {
        if (action->phase == 40) {
            parent->flags |= 2u;
        }
        const std::int32_t index = platform::RandomRange_46FAF0(0, 15);
        YobiAction* const particle = NewYobiAction_404AE0(2, 0);
        if (particle != nullptr) {
            particle->callback = UpdateBattleRisingEffect_41A200;
            std::uint32_t flags;
            if (render::HasLegacy3DDevice_44B610() != 0) {
                flags = particle->flags | 0x100000u;
            } else {
                const std::int32_t mode = platform::RandomRange_46FAF0(0, 1);
                flags = particle->flags | (mode == 0 ? 0x10000u : 0x8000u);
            }
            particle->flags = flags;
            particle->displayPriority = 0xCC;
            particle->flags = flags | 0x40000u;
            particle->framePlayingRate = 100;
            particle->imageNo = 110004;
            particle->rebirthValue1 = 4;
            particle->rebirthValue2 = 4;
            const std::int32_t xOffset = kBattleRadialXOffsets[index];
            const std::int32_t particleX = action->x + xOffset;
            const std::int32_t yOffset = kBattleRisingYOffsets[index];
            particle->offset40 = static_cast<float>(particleX);
            particle->offset44 = static_cast<float>(action->y + yOffset);
            const std::int32_t parentY = action->y;
            particle->effectAcceleration48 = 0.2F;
            particle->battleAttachmentParent10 = action;
            particle->phase = parentY + yOffset - 100;
            action->pathCountOrDrawOrder_270 = 0;
        }
    }
    if (action->field58 >= 50) {
        YobiAction* const completionParent = action->battleAttachmentParent10;
        if (completionParent != nullptr) {
            auto* const payload = completionParent->payload.battleActor;
            if (payload->effectPlaybackState1024 != BattleEffectPlayback_Idle) {
                payload->effectPlaybackState1024 = BattleEffectPlayback_Completed;
            }
        }
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x0041A530。
void UpdateBattleFallingEffect_41A530(YobiAction* const action)
{
    const double y = action->offset44 + 3.0;
    action->offset44 = static_cast<float>(y);
    if (y > action->nameY) {
        YobiAction* const parent = action->battleAttachmentParent10;
        ++parent->field58;
        static_cast<void>(MarkDeath(action));
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    action->x = static_cast<std::int32_t>(action->offset40);
    action->y = static_cast<std::int32_t>(action->offset44);
}

// 原函数：0x0041A590。
void UpdateBattleReverseEmitter_41A590(YobiAction* const action)
{
    auto* const payload = action->payload.battleRingEmitter;
    const std::int32_t oldPhase = action->phase;
    action->phase = oldPhase + 1;
    if (oldPhase > 1) {
        action->phase = 0;
        for (std::int32_t i = 0; i <= payload->lastIndex80; ++i) {
            if (payload->counts00[i] >= 4) {
                continue;
            }
            YobiAction* const particle = NewYobiAction_404AE0(2, 0);
            if (particle == nullptr) {
                continue;
            }
            const std::uint32_t flags = particle->flags;
            particle->callback = UpdateBattleFallingEffect_41A530;
            particle->displayPriority = 0xCC;
            particle->flags = flags | 0x40000u;
            particle->framePlayingRate = 100;
            // E06／41A6FA：控制器关联受术角色，粒子数量与运动沿原流程。
            particle->imageNo = ResolveSkillEffectImage(
                action->battleAttachmentParent10, SkillEffectKind::Revive, 110060);
            particle->rebirthValue1 = 4;
            particle->rebirthValue2 = 4;
            const std::int32_t index = 15 - (i % 16);
            const std::int32_t xOffset = kBattleRadialXOffsets[index];
            const std::int32_t yOffset = kBattleEmitterYOffsets[index];
            particle->offset40 = static_cast<float>(action->x + xOffset);
            particle->offset44 = static_cast<float>((action->y + yOffset) - 100.0);
            const std::int32_t parentY = action->y;
            particle->battleAttachmentParent10 = action;
            particle->nameY = parentY + yOffset - 40;
            action->pathCountOrDrawOrder_270 = 0;
            ++payload->counts00[i];
        }
        const std::int32_t lastIndex = payload->lastIndex80 + 1;
        payload->lastIndex80 = lastIndex;
        if (lastIndex >= 32) {
            payload->lastIndex80 = 31;
        }
    }
    if (action->field58 >= 128) {
        YobiAction* const parent = action->battleAttachmentParent10;
        if (parent != nullptr) {
            auto* const parentPayload = parent->payload.battleActor;
            if (parentPayload->effectPlaybackState1024 != BattleEffectPlayback_Idle) {
                parentPayload->effectPlaybackState1024 = BattleEffectPlayback_Completed;
            }
        }
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x0041A7D0。
void UpdateBattleIndexedEmitter_41A7D0(YobiAction* const action)
{
    const std::int32_t oldPhase = action->phase;
    action->phase = oldPhase + 1;
    if (oldPhase > 0 && action->nameX < 32) {
        action->phase = 0;
        YobiAction* const particle = NewYobiAction_404AE0(2, 0);
        if (particle != nullptr) {
            const std::uint32_t flags = particle->flags;
            particle->callback = UpdateBattleRisingEffect_41A200;
            particle->displayPriority = 0xCC;
            particle->flags = flags | 0x40000u;
            particle->framePlayingRate = 100;
            // E06／41A913：控制器本身没有战斗元数据。
            particle->imageNo = ResolveSkillEffectImage(
                action->battleAttachmentParent10, SkillEffectKind::StatusRecover, 110011);
            particle->rebirthValue1 = 4;
            particle->rebirthValue2 = 4;
            const std::int32_t xOffset = kBattleRadialXOffsets[action->nameX % 16];
            particle->offset40 = static_cast<float>(action->x + xOffset);
            const std::int32_t yOffset = kBattleEmitterYOffsets[action->nameX % 16];
            particle->offset44 = static_cast<float>(action->y + yOffset);
            const std::int32_t endOffset = kBattleEmitterYOffsets[action->nameX % 16];
            const std::int32_t parentY = action->y;
            particle->effectAcceleration48 = 0.1F;
            particle->battleAttachmentParent10 = action;
            particle->phase = endOffset + parentY - 100;
            action->pathCountOrDrawOrder_270 = 0;
        }
        ++action->nameX;
    }
    if (action->field58 >= 32) {
        YobiAction* const parent = action->battleAttachmentParent10;
        if (parent != nullptr) {
            auto* const payload = parent->payload.battleActor;
            if (payload->effectPlaybackState1024 != BattleEffectPlayback_Idle) {
                payload->effectPlaybackState1024 = BattleEffectPlayback_Completed;
            }
        }
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x00420D50。
YobiAction* NewBattleRisingEffect_420D50(YobiAction* const parent)
{
    const std::int32_t index = platform::RandomRange_46FAF0(0, 15);
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleRisingEffect_41A200;
    std::uint32_t flags;
    if (render::HasLegacy3DDevice_44B610() != 0) {
        flags = action->flags | 0x100000u;
    } else {
        const std::int32_t mode = platform::RandomRange_46FAF0(0, 1);
        flags = action->flags | (mode == 0 ? 0x10000u : 0x8000u);
    }
    const std::int32_t xOffset = kBattleRadialXOffsets[index];
    action->flags = flags;
    action->displayPriority = 0xCC;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->framePlayingRate = 100;
    action->imageNo = 110004;
    const std::int32_t yOffset = kBattleRisingYOffsets[index];
    action->offset40 = static_cast<float>(parent->x + xOffset);
    action->offset44 = static_cast<float>(parent->y + yOffset);
    const std::int32_t parentY = parent->y;
    action->effectAcceleration48 = 0.2F;
    action->battleAttachmentParent10 = parent;
    action->pathCountOrDrawOrder_270 = 0;
    action->phase = yOffset + parentY - 100;
    return action;
}

// 原函数：0x0041CD00。
YobiAction* NewBattleEffect_41CD00(
    YobiAction* const source, const std::int32_t offsetX, const BattleEffectThirdArgument third,
    const std::uint32_t type, const std::int32_t blend, const std::int32_t link)
{
    using world::gBattleAllEntries59E980;
    auto* const sourcePayload = source->payload.battleActor;
    YobiAction* action;
    if (type > BattleEffectType_AreaAttackImpactAtPosition) {
        action = third.existingAction;
    } else {
        switch (type) {
        case BattleEffectType_AreaAttackImpactAtPosition: {
            action = NewYobiAction_404AE0(2, 0);
            if (action == nullptr) {
                return nullptr;
            }
            action->callback = UpdateBattleCompletingEffect_41A160;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            const std::uint32_t initialFlags = action->flags;
            action->displayPriority = 204;
            action->offset40 = static_cast<float>(offsetX);
            action->framePlayingRate = 100;
            action->flags = initialFlags | 0x40000u;
            action->pathCountOrDrawOrder_270 = 0;
            action->offset44 = static_cast<float>(third.yValue);
            action->scaleX = static_cast<float>(action->scaleX * 1.5);
            action->scaleY = static_cast<float>(action->scaleY * 1.5);
            action->imageNo = sourcePayload->image102C;
            break;
        }
        case BattleEffectType_Recovery: {
            action = NewYobiAction_404AE0(2, 0);
            if (action == nullptr) {
                return nullptr;
            }
            action->callback = UpdateBattleCompletingEffect_41A160;
            action->displayPriority = 204;
            action->framePlayingRate = 100;
            const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
            const std::uint32_t deviceFlags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
            action->flags = deviceFlags;
            action->flags = deviceFlags | 0x40000u;
            const std::int32_t level = sourcePayload->castSkillLevel1044;
            action->imageNo = level >= 12 ? 111019 : kBattleEffectTierOffsets553A38[level] + 110176;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            action->offset40 = source->offset40;
            const std::int32_t halfOffset = source->graphicOffsetY / 2;
            action->offset44 = static_cast<float>(static_cast<double>(halfOffset) + source->offset44);
            action->pathCountOrDrawOrder_270 = 0;
            break;
        }
        case BattleEffectType_FixedImage110012:
        case BattleEffectType_FixedImage110014:
        case BattleEffectType_FixedImage110013: {
            action = NewYobiAction_404AE0(2, 0);
            if (action == nullptr) {
                return nullptr;
            }
            const std::uint32_t initialFlags = action->flags;
            action->callback = UpdateBattleCompletingEffect_41A160;
            action->displayPriority = 204;
            action->flags = initialFlags | 0x40000u;
            action->framePlayingRate = 100;
            action->imageNo = type == BattleEffectType_FixedImage110012 ? 110012 : (type == BattleEffectType_FixedImage110014 ? 110014 : 110013);
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            action->offset40 = source->offset40;
            action->offset44 = static_cast<float>(static_cast<double>(third.yValue) + source->offset44);
            action->pathCountOrDrawOrder_270 = 0;
            break;
        }
        case BattleEffectType_RandomEmitter: {
            const auto position = static_cast<std::uint8_t>(sourcePayload->battlePosition18);
            action = NewYobiAction_404AE0(static_cast<std::uint8_t>(position * 10 + 1), 0);
            if (action == nullptr) {
                return nullptr;
            }
            const std::uint32_t initialFlags = action->flags;
            action->callback = UpdateBattleRandomEmitter_41A310;
            action->displayPriority = 204;
            action->flags = initialFlags | 0x40000u;
            action->framePlayingRate = 100;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            const std::int32_t graphic = source->graphicIndex;
            const std::int32_t x = source->x;
            const std::int32_t halfWidth = render::ReadGraphicDxWidth(graphic) / 2;
            const std::int32_t projectedX = halfWidth + source->graphicOffsetX + x;
            action->offset40 = static_cast<float>(projectedX);
            const std::int32_t projectedY = source->y + third.yValue;
            action->pathCountOrDrawOrder_270 = 0;
            action->offset44 = static_cast<float>(projectedY);
            break;
        }
        case BattleEffectType_StatusRecoveryEmitter:
        case BattleEffectType_ReviveEmitter: {
            action = NewYobiAction_404AE0(2, sizeof(BattleRingEmitterPayload), YobiPayloadKind::BattleRingEmitter);
            if (action == nullptr) {
                return nullptr;
            }
            if (type == BattleEffectType_ReviveEmitter) {
                static_cast<void>(audio::PlaySe_52C450(280, source->x));
                action->callback = UpdateBattleReverseEmitter_41A590;
            } else {
                static_cast<void>(audio::PlaySe_52C450(281, source->x));
                action->callback = UpdateBattleIndexedEmitter_41A7D0;
            }
            const std::uint32_t initialFlags = action->flags;
            action->displayPriority = 204;
            action->flags = initialFlags | 0x40000u;
            action->framePlayingRate = 100;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            const std::int32_t width = render::ReadGraphicDxWidth(source->graphicIndex);
            const std::int32_t x = source->x;
            const std::int32_t projectedX = width / 2 + source->graphicOffsetX + x;
            action->offset40 = static_cast<float>(projectedX);
            const std::int32_t projectedY = third.yValue + source->y;
            action->pathCountOrDrawOrder_270 = 0;
            action->offset44 = static_cast<float>(projectedY);
            break;
        }
        case BattleEffectType_SpellCasting:
        case BattleEffectType_CastingAttachment: {
            action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
            gBattleFactoryEffect59F9E8 = action;
            // 原34/43先写1C再走判空分支；RPLAN.24不改为安全失败。
            action->deathFlag = 0;
            if (action == nullptr) {
                return nullptr;
            }
            const std::uint32_t initialFlags = action->flags;
            action->callback = UpdateBattleParentEffect_41A1C0;
            action->displayPriority = 204;
            action->flags = initialFlags | 0x40000u;
            action->framePlayingRate = 100;
            // E07／41DB2C：只替换施法图，音效及定位继续读取原 image1028。
            action->imageNo = type == BattleEffectType_SpellCasting ? ResolveSkillCastingImage(source, sourcePayload->image1028)
                : source->payload.battleActor->image13E4;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            if (type == BattleEffectType_SpellCasting) {
                const std::int32_t image = sourcePayload->image1028;
                std::int32_t sound = 0; // playSound为true的路径均覆盖此值。
                bool playSound = true;
                if ((image >= 110052 && image <= 110055) || image == 111001) {
                    sound = 259;
                } else if ((image >= 110056 && image <= 110059) || image == 111018) {
                    sound = 260;
                } else if ((image >= 110081 && image <= 110084) || image == 111014
                    || (image >= 110180 && image <= 110183) || image == 111017) {
                    sound = 261;
                } else {
                    playSound = false;
                }
                if (playSound) {
                    static_cast<void>(audio::PlaySe_52C450(sound, source->x));
                }
                action->offset40 = source->offset40;
                const std::int32_t currentImage = sourcePayload->image1028;
                if (currentImage >= 110180 && currentImage <= 110183) {
                    const std::int32_t halfOffset = source->graphicOffsetY / 2;
                    const double y = static_cast<double>(halfOffset) + source->offset44 + third.yValue;
                    action->pathCountOrDrawOrder_270 = 0;
                    action->offset44 = static_cast<float>(y);
                } else {
                    const float y = source->offset44;
                    action->pathCountOrDrawOrder_270 = 0;
                    action->offset44 = y;
                }
            } else {
                action->offset40 = source->offset40;
                action->offset44 = source->offset44;
                action->pathCountOrDrawOrder_270 = 0;
            }
            break;
        }
        case BattleEffectType_SkillImpact: {
            const std::int32_t skill = sourcePayload->castSkill103C;
            const bool fixedTargetMode = skill == BattleSkill_Dorain || skill == 3037 || skill == BattleSkill_VoodooCurse;
            YobiAction* fixedTarget = nullptr; // 仅fixedTargetMode路径读取，且该路径先赋值。
            if (fixedTargetMode) {
                fixedTarget = cg::world::gBattleAllEntries59E980[
                    sourcePayload->effects1C[sourcePayload->effectRunningC9C].position];
            }
            action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
            if (action == nullptr) {
                return nullptr;
            }
            if (skill == BattleSkill_MonWind) {
                const std::uint32_t initialFlags = action->flags;
                auto* const payload = action->payload.battleActor;
                action->callback = UpdateBattleCarryingEffect_4219D0;
                action->displayPriority = 201;
                action->flags = initialFlags | 0x60000u;
                const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
                const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
                const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
                const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
                const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
                AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
                const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
                if ((flags & 0x800000u) != 0) {
                    std::int32_t position;
                    std::int32_t value3;
                    std::int32_t value4;
                    std::int32_t value5;
                    static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
                    AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
                }
                action->framePlayingRate = 100;
                if (sourcePayload->effectRunningC9C != 0) {
                    action->field58 = platform::RandomRange_46FAF0(0, 30);
                }
                action->imageNo = sourcePayload->image102C;
                action->rebirthValue1 = 4;
                action->rebirthValue2 = 4;
                const std::int32_t speed = platform::RandomRange_46FAF0(4, 8);
                action->motionMagnitude54 = static_cast<float>(speed);
                payload->castSkill103C = sourcePayload->castSkill103C;
                payload->castSkillLevel1044 = sourcePayload->castSkillLevel1044;
                payload->skillMeta = sourcePayload->skillMeta; // E01／41DD50。
                action->offset40 = 320.0F;
                action->offset44 = 260.0F;
                TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[
                    sourcePayload->effects1C[sourcePayload->effectRunningC9C].position], 0, 0.0F);
                const std::int32_t angle = platform::RandomRange_46FAF0(-90, 90);
                action->pathCountOrDrawOrder_270 = 0;
                action->angle50 = static_cast<float>(static_cast<double>(angle) + action->angle50);
            } else if (fixedTargetMode) {
                auto* const payload = action->payload.battleActor;
                action->callback = UpdateBattleImpactEffect_41A9E0;
                action->displayPriority = 201;
                const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
                const std::uint32_t deviceFlags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
                action->flags = deviceFlags;
                action->flags = deviceFlags | 0x40000u;
                const std::int32_t effectIndex = sourcePayload->effectRunningC9C;
                const std::uint32_t effectFlags = sourcePayload->effects1C[effectIndex].flags;
                const std::int32_t effectValue4 = sourcePayload->effects1C[effectIndex].position4;
                const std::int32_t effectValue3 = sourcePayload->effects1C[effectIndex].position3;
                const std::int32_t effectPosition = sourcePayload->effects1C[effectIndex].position;
                AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
                const std::uint32_t flags = sourcePayload->effects1C[sourcePayload->effectRunningC9C].flags;
                if ((flags & 0x800000u) != 0) {
                    std::int32_t position;
                    std::int32_t value3;
                    std::int32_t value4;
                    std::int32_t value5;
                    static_cast<void>(TakeBattleAuxEffects_43BC90(source, &position, &value3, &value4, &value5));
                    AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
                }
                action->framePlayingRate = 100;
                if (sourcePayload->effectRunningC9C != 0) {
                    action->field58 = platform::RandomRange_46FAF0(0, 30);
                }
                action->imageNo = sourcePayload->image102C;
                action->rebirthValue1 = 4;
                action->rebirthValue2 = 4;
                payload->castSkill103C = sourcePayload->castSkill103C;
                payload->castSkillLevel1044 = sourcePayload->castSkillLevel1044;
                payload->skillMeta = sourcePayload->skillMeta; // E01／41E114。
                action->offset40 = fixedTarget->offset40;
                action->offset44 = fixedTarget->offset44;
                action->pathCountOrDrawOrder_270 = 0;
            } else if (skill == BattleSkill_AttackMagicEarth || skill == BattleSkill_AttackMagicEarthStrong
                || skill == BattleSkill_AttackMagicEarthSuper) {
                InitBattleOffsetEffect_423040(action, source, UpdateBattleMovingImpactEffect_41BB00);
            } else if (skill == BattleSkill_AttackMagicWind || skill == BattleSkill_AttackMagicWindStrong
                || skill == BattleSkill_AttackMagicWindSuper) {
                InitBattleCenterEffect_4231D0(action, source, UpdateBattleHomingImpactEffect_41C3B0);
            } else if (skill == BattleSkill_AttackMagicFire || skill == BattleSkill_AttackMagicFireStrong
                || skill == BattleSkill_AttackMagicFireSuper) {
                InitBattleLowerEffect_423350(action, source, UpdateBattleFrameEventEffect_41B440);
            } else if (skill >= 2068 && skill <= 2079) {
                InitBattleTargetEffect_422E50(action, source, UpdateBattleImpactSoundEffect_41AF00);
            } else if (skill >= 2055 && skill <= 2057) {
                InitBattleTargetEffect_422E50(action, source, UpdateBattleImpactEffect_41A9E0);
            } else {
                std::int32_t side;
                InitBattleScreenYEffect_4234B0(action, source, UpdateBattleFrameEventEffect_41B440, &side);
            }
            break;
        }
        case BattleEffectType_ParentBoundImage110184:
        case BattleEffectType_ParentImageTierAura: {
            action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
            if (action == nullptr) {
                return nullptr;
            }
            const std::uint32_t initialFlags = action->flags;
            action->callback = UpdateBattleParentEffect_41A1C0;
            action->displayPriority = 203;
            action->flags = initialFlags | 0x40000u;
            action->battleAttachmentParent10 = source;
            action->framePlayingRate = 100;
            if (type == BattleEffectType_ParentBoundImage110184) {
                action->imageNo = 110184;
            } else {
                const std::int32_t image = source->imageNo;
                if (image >= 110085 && image <= 110088) {
                    const std::int32_t level = source->field1D0;
                    action->imageNo = level >= 12 && level <= 15 ? 111015 : 110184;
                } else if (image >= 110165 && image <= 110168) {
                    const std::int32_t level = source->field1D0;
                    action->imageNo = level >= 12 && level <= 15 ? 111020 : 110184;
                } else {
                    action->imageNo = 0;
                }
            }
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            action->offset40 = source->offset40;
            action->offset44 = source->offset44;
            action->pathCountOrDrawOrder_270 = 0;
            break;
        }
        default: {
            action = NewYobiAction_404AE0(2, 0);
            if (action == nullptr) {
                return nullptr;
            }
            const std::uint32_t initialFlags = action->flags;
            action->callback = UpdateBattleCompletingEffect_41A160;
            action->displayPriority = 204;
            action->framePlayingRate = 100;
            action->rebirthValue1 = 4;
            action->rebirthValue2 = 4;
            action->flags = initialFlags | 0x40000u;
            action->offset40 = static_cast<float>(static_cast<double>(offsetX) + source->offset40);
            const double initialY = static_cast<double>(third.yValue) + source->offset44;
            action->pathCountOrDrawOrder_270 = 0;
            action->offset44 = static_cast<float>(initialY);
            bool deviceBlend = false;
            bool centerY = false;
            bool childAura = false;
            switch (type) {
            case BattleEffectType_AttackClash: action->imageNo = 110009; break;
            case BattleEffectType_FixedImage110063: action->imageNo = 110063; break;
            case BattleEffectType_MagicAttack:
            case BattleEffectType_DebuffSpell:
            case BattleEffectType_BarrierSpell:
            case BattleEffectType_AttributeReversal:
            case BattleEffectType_CenteredLevelScaledImage110170: {
                const std::int32_t level = sourcePayload->castSkillLevel1044;
                std::int32_t base;
                std::int32_t highImage;
                switch (type) {
                case BattleEffectType_MagicAttack: base = 110067; highImage = 111013; break;
                case BattleEffectType_DebuffSpell: base = 110071; highImage = 111063; break;
                case BattleEffectType_BarrierSpell: base = 110105; highImage = 111021; break;
                case BattleEffectType_AttributeReversal: base = 110161; highImage = 111001; break;
                default: base = 110170; highImage = 111001; break;
                }
                action->imageNo = level >= 12 ? highImage : kBattleEffectTierOffsets553A38[level] + base;
                deviceBlend = type != BattleEffectType_BarrierSpell;
                centerY = type == BattleEffectType_CenteredLevelScaledImage110170;
                break;
            }
            case BattleEffectType_SkillActivation: {
                const std::int32_t skill = sourcePayload->castSkill103C;
                std::int32_t* level = &sourcePayload->castSkillLevel1044;
                if (skill == BattleSkill_SwallowSlash || skill == BattleSkill_BladeSoul) {
                    level = &sourcePayload->castLevel1040;
                    --*level;
                }
                const std::int32_t value = *level;
                action->imageNo = value >= 12 ? 111016 : kBattleEffectTierOffsets553A38[value] + 110100;
                centerY = true;
                break;
            }
            case BattleEffectType_Seal:
            case BattleEffectType_Summon:
                action->imageNo = type == BattleEffectType_Seal ? 110097 : 110096;
                action->callback = UpdateBattleParentEffect_41A1C0;
                deviceBlend = true;
                break;
            case BattleEffectType_ItemUse: action->imageNo = 110104; break;
            case BattleEffectType_CrossCounter:
                action->imageNo = FindBattleAppendEffectTechnique_43BEA0(11, sourcePayload->battlePosition18) >= 12
                    ? 111053 : 110153;
                break;
            case BattleEffectType_PhysicalReflect:
            case BattleEffectType_PhysicalAbsorb:
            case BattleEffectType_PhysicalNullify:
            case BattleEffectType_MagicReflect:
            case BattleEffectType_MagicAbsorb:
            case BattleEffectType_MagicNullify: {
                std::int32_t skill;
                std::int32_t lowImage;
                std::int32_t highImage;
                switch (type) {
                case BattleEffectType_PhysicalReflect: skill = BattleSkill_ReflectionPhysics; lowImage = 110109; highImage = 111025; break;
                case BattleEffectType_PhysicalAbsorb: skill = BattleSkill_AbsorbPhysics; lowImage = 110117; highImage = 111027; break;
                case BattleEffectType_PhysicalNullify: skill = BattleSkill_IneffectivePhysics; lowImage = 110125; highImage = 111023; break;
                case BattleEffectType_MagicReflect: skill = BattleSkill_ReflectionMagic; lowImage = 110113; highImage = 111024; break;
                case BattleEffectType_MagicAbsorb: skill = BattleSkill_AbsorbMagic; lowImage = 110121; highImage = 110121; break;
                default: skill = BattleSkill_IneffectiveMagic; lowImage = 110129; highImage = 111022; break;
                }
                const std::int32_t technique = FindBattleAppendEffectTechnique_43BEA0(
                    skill, source->payload.battleActor->battlePosition18);
                action->imageNo = technique >= 12 ? highImage : lowImage;
                deviceBlend = true;
                centerY = true;
                break;
            }
            case BattleEffectType_SealCard:
                action->imageNo = 110095;
                action->callback = UpdateBattleScalingEffect_41A270;
                action->displayPriority = 8;
                action->scaleX = 0.0F;
                action->scaleY = 0.0F;
                break;
            case BattleEffectType_InstantDeath:
            case BattleEffectType_DeathScytheTechnique2053:
            case BattleEffectType_DeathScytheTechnique2054:
                action->imageNo = type == BattleEffectType_InstantDeath ? 110156 : (type == BattleEffectType_DeathScytheTechnique2053 ? 111162 : 111163);
                centerY = true;
                break;
            case BattleEffectType_HealingCast:
                action->imageNo = 110157;
                deviceBlend = true;
                centerY = true;
                break;
            case BattleEffectType_StatusHpRecovery:
            case BattleEffectType_Heal: {
                const std::int32_t level = sourcePayload->castSkillLevel1044;
                const std::int32_t base = type == BattleEffectType_StatusHpRecovery ? 110165 : 110085;
                action->imageNo = level >= 12 ? base : kBattleEffectTierOffsets553A38[level] + base;
                deviceBlend = true;
                childAura = true;
                break;
            }
            case BattleEffectType_AmountTieredHpRecovery: {
                const std::int32_t value = sourcePayload->value1030;
                action->imageNo = value >= 3500 ? 111196 : (value >= 2000 ? 111195 : 111194);
                deviceBlend = true;
                break;
            }
            case BattleEffectType_ParameterChange:
                action->imageNo = sourcePayload->castSkillLevel1044 < 12 ? 110189 : 111057;
                break;
            case BattleEffectType_LevelIndexedImage111239: {
                const std::int32_t level = sourcePayload->castSkillLevel1044;
                action->imageNo = level >= 1 && level <= 7 ? 111239 + level : 111239;
                break;
            }
            case BattleEffectType_BombLevel: {
                const std::int32_t level = world::gBattleBombLevelD67B2C;
                if (level > 0 && level <= 3) {
                    action->imageNo = 110103;
                } else if (level > 3 && level <= 6) {
                    action->imageNo = 110102;
                } else if (level > 6 && level <= 9) {
                    action->imageNo = 110101;
                } else if (level > 9 && level <= 12) {
                    action->imageNo = 110100;
                } else {
                    action->imageNo = 111016;
                }
                centerY = true;
                break;
            }
            case BattleEffectType_RandomOffsetImage110474: {
                action->imageNo = 110474;
                const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
                action->flags |= hasDevice ? 0x100000u : 0x8000u;
                const std::int32_t halfWidth = render::ReadGraphicDxWidth(source->graphicIndex) / 2;
                const std::int32_t randomX = platform::RandomRange_46FAF0(0, halfWidth);
                const std::int32_t quarterWidth = render::ReadGraphicDxWidth(source->graphicIndex) / 4;
                const std::int32_t x = source->x - quarterWidth + randomX;
                action->offset40 = static_cast<float>(x);
                const std::int32_t randomY = platform::RandomRange_46FAF0(0, source->graphicOffsetY / 3);
                const std::int32_t quarterOffset = source->graphicOffsetY / 4;
                const std::int32_t y = source->y + quarterOffset - randomY;
                action->offset44 = static_cast<float>(y);
                break;
            }
            case BattleEffectType_PayloadImage102CType41:
            case BattleEffectType_PayloadImage102CType42: action->imageNo = sourcePayload->image102C; break;
            case BattleEffectType_FixedImage111179: action->imageNo = 111179; break;
            case BattleEffectType_FixedImage111188: action->imageNo = 111188; break;
            case BattleEffectType_FixedImage111191: action->imageNo = 111191; break;
            }
            if (deviceBlend) {
                const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
                action->flags |= hasDevice ? 0x100000u : 0x8000u;
            }
            if (centerY) {
                const std::int32_t halfOffset = source->graphicOffsetY / 2;
                action->offset44 = static_cast<float>(static_cast<double>(halfOffset) + source->offset44);
            }
            if (childAura) {
                const std::int32_t level = sourcePayload->castSkillLevel1044;
                if (level >= 9) {
                    action->field1D0 = level;
                    static_cast<void>(NewBattleEffect_41CD00(action, 0, {.yValue = 0}, BattleEffectType_ParentImageTierAura, 1, 0));
                }
            }
            break;
        }
        }
    }
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    if (hasDevice) {
        if (blend == 1 || blend == 2) {
            action->flags |= 0x100000u;
        }
    } else if (blend == 1) {
        action->flags |= 0x8000u;
    } else if (blend == 2) {
        action->flags |= 0x10000u;
    }
    if (link == 1) {
        action->battleAttachmentParent10 = source;
        source->payload.battleActor->effectPlaybackState1024 = BattleEffectPlayback_Playing;
    }
    // E05／41E368：附加层已按原图生成，再替换受术动画。
    if (type == BattleEffectType_Heal) {
        action->imageNo = ResolveSkillEffectImage(source, SkillEffectKind::Heal, action->imageNo);
    } else if (type == BattleEffectType_Recovery) {
        action->imageNo = ResolveSkillEffectImage(source, SkillEffectKind::Recovery, action->imageNo);
    }
    const float x = action->offset40;
    action->direction = 0;
    const std::int32_t projectedX = static_cast<std::int32_t>(x);
    const float y = action->offset44;
    action->x = projectedX;
    action->y = static_cast<std::int32_t>(y);
    if (type == BattleEffectType_ParentBoundImage110184 || type == BattleEffectType_ParentImageTierAura || type == BattleEffectType_SpellCasting || type == BattleEffectType_SkillImpact || type == BattleEffectType_CastingAttachment) {
        auto* const payload = action->payload.battleActor;
        if (payload != nullptr) {
            const auto* const currentSource = static_cast<const BattleActorPayload*>(source->payload.battleActor);
            if (currentSource != nullptr) {
                payload->battlePosition18 = currentSource->battlePosition18;
            }
        }
    }
    // E04：原创建／定位完成后替换通用魔法图号。
    if (type == BattleEffectType_SkillImpact) {
        action->imageNo = ResolveSkillEffectImage(source, SkillEffectKind::Magic, action->imageNo);
    }
    return action;
}

// BSTL01：共享相位先推进/15和0反向；X外Y内100格，每格fresh相位，返回末结果。
// 原函数：0x0041E890。
std::int32_t DrawBattleTiledEffect_41E890(const std::int32_t type)
{
    if (gBattleTiledEffectReverse59F9F0 == 0) {
        const std::int32_t phase = gBattleTiledEffectPhase59F9EC + 1;
        gBattleTiledEffectPhase59F9EC = phase;
        if (phase >= 15) {
            gBattleTiledEffectReverse59F9F0 = 1;
        }
    } else {
        const std::int32_t phase = gBattleTiledEffectPhase59F9EC - 1;
        gBattleTiledEffectPhase59F9EC = phase;
        if (phase <= 0) {
            gBattleTiledEffectReverse59F9F0 = 0;
        }
    }
    const std::int32_t row = type * 16;
    std::int32_t result;
    for (std::int32_t x = 0; x < platform::gWindowWidth; x += 64) {
        for (std::int32_t y = 0; y < platform::gWindowHeight; y += 48) {
            const std::int32_t image = kBattleTiledEffectImages553A74[row + gBattleTiledEffectPhase59F9EC];
            result = cg::render::DrawGraphic(x + 32, y + 24, 3, image, 0, nullptr);
        }
    }
    return result;
}

// 原函数：0x0041E920。
void UpdateBattleTiledEffect_41E920(YobiAction* const action)
{
    const std::int32_t reverse = action->value1D4;
    const std::int32_t initialPhase = action->field58;
    if (reverse == 0) {
        const std::int32_t phase = initialPhase + 1;
        action->field58 = phase;
        if (phase >= 15) {
            action->value1D4 = 1;
        }
    } else {
        const std::int32_t phase = initialPhase - 1;
        action->field58 = phase;
        if (phase <= 0) {
            action->value1D4 = 0;
        }
    }
    for (std::int32_t x = 0; x < 640; x += 64) {
        for (std::int32_t y = 0; y < 480; y += 48) {
            const std::int32_t type = action->phase;
            const std::int32_t phase = action->field58;
            const std::int32_t image = kBattleTiledEffectImages553A74[type * 16 + phase];
            static_cast<void>(cg::render::DrawGraphic(x + 32, y + 24, 3, image, 0, nullptr));
        }
    }
    auto* const count = &action->readiness;
    if (((*count) >= action->maxHp)) {
        static_cast<void>(MarkDeath(action));
    }
    static_cast<void>((++(*count)));
}

// 原函数：0x0041E9D0。
YobiAction* NewBattleTiledEffect_41E9D0(const std::int32_t type, const std::int32_t lifetime)
{
    if (lifetime <= 0) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleTiledEffect_41E920;
    action->displayPriority = 0;
    action->framePlayingRate = 100;
    action->flags = 0x40002u;
    action->imageNo = 0;
    action->direction = 0;
    action->phase = type;
    (action->maxHp = static_cast<std::int32_t>(lifetime));
    (action->readiness = 0);
    action->value1D4 = 0;
    action->field58 = 0;
    return action;
}

// 原函数：0x00418F10。
void UpdateBattleArcProjection_418F10(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    }
    action->displayPriority = parent->displayPriority;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    if (payload->flash1004 == 1) {
        static_cast<void>(NewBattleAfterimage_41E5F0(action));
    }
    const std::int32_t parentX = static_cast<std::int32_t>(parent->offset40);
    action->x = parentX + action->deltaX;
    const std::int32_t parentY = static_cast<std::int32_t>(parent->offset44);
    const std::int32_t y = parentY + action->deltaY;
    const std::int32_t previous = action->field58;
    action->y = y;
    const std::int32_t duration = payload->valueFF0;
    const std::int32_t squared = duration * duration - previous * previous;
    const std::int32_t direction = action->direction + 1;
    action->direction = direction;
    if (direction > 7) {
        action->direction = 0;
    }
    const float amplitude = static_cast<float>(squared);
    const double xFactor = world::LookupCosineDegrees_46F950(action->angle50);
    const std::int32_t xStep = static_cast<std::int32_t>(xFactor * amplitude);
    const std::int32_t x = action->x + xStep;
    const float yAngle = action->angle50;
    action->x = x;
    const double yFactor = world::LookupSineDegrees_46F930(yAngle);
    const std::int32_t yStep = static_cast<std::int32_t>(yFactor * amplitude);
    const std::int32_t nextY = action->y + yStep;
    const std::int32_t next = action->field58 + 1;
    action->y = nextY;
    action->field58 = next;
}

// 原函数：0x00418FF0。
YobiAction* NewBattleArcProjection_418FF0(
    YobiAction* const parent, const std::int32_t offsetX, const std::int32_t offsetY,
    YobiAction* const distanceSource, YobiAction* const target, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleArcProjection_418F10;
    action->flags = flags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 10;
    action->displayPriority = parent->displayPriority;
    action->imageNo = imageNo;
    action->battleAttachmentParent10 = parent;
    action->deltaX = offsetX;
    action->deltaY = offsetY;
    const double x = static_cast<double>(offsetX) + parent->offset40;
    auto* const payload = action->payload.battleActor;
    action->offset40 = static_cast<float>(x);
    action->offset44 = static_cast<float>(static_cast<double>(offsetY) + parent->offset44);
    payload->x00 = static_cast<std::int32_t>(target->offset40) + offsetX;
    payload->y04 = static_cast<std::int32_t>(target->offset44) + offsetY;
    TurnBattleToward_406190(action, target, 0, 0.0F);
    const std::int32_t randomAngle = platform::Rand16() % 90;
    const std::int32_t angle = static_cast<std::int32_t>(
        static_cast<double>(randomAngle) + action->angle50 + 135.0) % 360;
    action->angle50 = static_cast<float>(angle);
    if (angle < 0) {
        action->angle50 = static_cast<float>(angle + 360);
    }
    const std::int32_t distance = GetBattleDistance_406300(distanceSource, target);
    const std::int32_t duration = ((distance - 20) / 10) / 2;
    payload->valueFF0 = duration;
    action->field58 = 2 - duration;
    action->motionMagnitude54 = 10.0F;
    action->pathCountOrDrawOrder_270 = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    payload->flash1004 = 1;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
    return action;
}

// BPR01：移动/标死后各自重取非拥有父；标死后仍播放并发布坐标，见doc/cg2d_battle_projection_spec.md。
// 原函数：0x00418D80。
void UpdateBattleProjection_418D80(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    action->motionMagnitude54 = 10.0F;
    const auto* const parentPayload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
    if (parentPayload->valueFD0 == 0) {
        MoveBattleByAngle_405EA0(action);
    }
    if (action->battleAttachmentParent10->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    }
    action->displayPriority = action->battleAttachmentParent10->displayPriority;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
}

// BPR01：目标先截S32，播放前后分别取图号；三个原caller均不消费创建返回，保持void接口。
// 原函数：0x00418DF0。
void NewBattleProjection_418DF0(
    YobiAction* const parent, const float x, const float y,
    const float targetX, const float targetY, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleProjection_418D80;
    action->flags = flags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 10;
    const std::uint8_t priority = parent->displayPriority;
    action->battleAttachmentParent10 = parent;
    action->displayPriority = priority;
    auto* const payload = action->payload.battleActor;
    action->imageNo = imageNo;
    action->offset40 = x;
    action->offset44 = y;
    const std::int32_t integerX = static_cast<std::int32_t>(targetX);
    payload->x00 = integerX;
    const std::int32_t integerY = static_cast<std::int32_t>(targetY);
    payload->y04 = integerY;
    const float turnY = static_cast<float>(integerY);
    const float turnX = static_cast<float>(integerX);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 0, 0.0F);
    const std::int32_t initialImage = action->imageNo;
    action->motionMagnitude54 = 10.0F;
    action->pathCountOrDrawOrder_270 = 0;
    if (initialImage == 110016 || initialImage == 111055) {
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
    }
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t currentImage = action->imageNo;
    if (currentImage == 110016 || currentImage == 111055) {
        MoveBattleByAngle_405EA0(action);
        MoveBattleByAngle_405EA0(action);
    }
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
}

// 原函数：0x00418B70。
YobiAction* NewBattleSpiracleShot_418B70(
    YobiAction* const parent, const BattleEntryEffectRecord* const effect, std::int32_t imageNo)
{
    using world::gBattleAllEntries59E980;
    imageNo = ResolveSkillEffectImage(parent, SkillEffectKind::SpiracleShot, imageNo); // E02。
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    action->flags |= 0x60002u;
    action->callback = UpdateBattleActor_404DB0;
    action->imageNo = 0;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 10.0F;
    action->displayPriority = 203;
    action->offset40 = parent->offset40;
    const float initialY = parent->offset44;
    auto* const payload = action->payload.battleActor;
    action->offset44 = initialY;
    action->pathCountOrDrawOrder_270 = 0;
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[effect->position], 0, 0.0F);
    const std::int32_t effectValue4 = effect->position4;
    const std::int32_t effectValue3 = effect->position3;
    const std::uint32_t effectFlags = effect->flags;
    const std::int32_t effectPosition = effect->position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = effect->flags;
    if ((flags & 0x800000u) != 0) {
        std::int32_t position, value3, value4, value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(parent, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->phase = BattleAction_ResolveProjectileHit;
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    static_cast<void>(render::LoadGraphic_522570(cg::world::gBattleAllEntries59E980[effect->position]->graphicIndex));
    payload->battlePosition18 = parent->payload.battleActor->battlePosition18;
    const std::int32_t targetPosition = effect->position;
    const render::GraphicDxCache* const graphics = render::gGraphicDxCache103CFA4;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[targetPosition];
    const std::int16_t targetHeight = render::ReadGraphicDxHeightFromEntries(graphics, target->graphicIndex);
    const std::int32_t parentGraphicIndex = parent->graphicIndex;
    const std::int32_t targetGraphicOffsetY = target->graphicOffsetY;
    const float targetBaseY = target->offset44;
    const float targetX = target->offset40;
    const float targetY = static_cast<float>(targetHeight
        * 0.5 + targetGraphicOffsetY + targetBaseY);
    const std::int16_t parentHeight = render::ReadGraphicDxHeightFromEntries(graphics, parentGraphicIndex);
    const float projectionX = action->offset40;
    const std::int32_t parentGraphicOffsetY = parent->graphicOffsetY;
    const float projectionBaseY = action->offset44;
    const float projectionY = static_cast<float>(parentHeight
        * 0.5 + parentGraphicOffsetY + projectionBaseY);
    NewBattleProjection_418DF0(action, projectionX, projectionY, targetX, targetY, imageNo);
    if (action->imageNo == 110016 || action->imageNo == 111055) {
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
    }
    payload->castSkill103C = parent->payload.battleActor->castSkill103C;
    payload->attackModeFF4 = parent->payload.battleActor->attackModeFF4;
    payload->castSkillLevel1044 = parent->payload.battleActor->castSkillLevel1044;
    payload->skillMeta = parent->payload.battleActor->skillMeta; // E01／418D24。
    MoveBattleByAngle_405EA0(action);
    MoveBattleByAngle_405EA0(action);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(screenY);
    return action;
}

// 原函数：0x00419180。
YobiAction* NewBattleBlastWave_419180(
    YobiAction* const parent, const BattleEntryEffectRecord* const effect, std::int32_t imageNo)
{
    using world::gBattleAllEntries59E980;
    imageNo = ResolveSkillEffectImage(parent, SkillEffectKind::BlastWave, imageNo); // E03。
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t initialFlags = action->flags | 0x60002u;
    action->callback = UpdateBattleActor_404DB0;
    action->flags = initialFlags;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 10.0F;
    action->displayPriority = 203;
    action->offset40 = parent->offset40;
    const float initialY = parent->offset44;
    auto* const payload = action->payload.battleActor;
    action->offset44 = initialY;
    action->pathCountOrDrawOrder_270 = 0;
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[effect->position], 0, 0.0F);
    action->imageNo = static_cast<std::int32_t>((action->angle50 + 11.25)
        * kBattleDirectionFrameScale);
    if (action->imageNo < 0) {
        action->imageNo += 16;
    }
    if (action->imageNo >= 16) {
        action->imageNo -= 16;
    }
    action->imageNo += imageNo;
    const std::int32_t effectValue4 = effect->position4;
    const std::int32_t effectValue3 = effect->position3;
    const std::uint32_t effectFlags = effect->flags;
    const std::int32_t effectPosition = effect->position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = effect->flags;
    if ((flags & 0x800000u) != 0) {
        std::int32_t position, value3, value4, value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(parent, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->phase = BattleAction_ResolveProjectileHit;
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    static_cast<void>(render::LoadGraphic_522570(cg::world::gBattleAllEntries59E980[effect->position]->graphicIndex));
    payload->battlePosition18 = parent->payload.battleActor->battlePosition18;
    const std::int32_t targetPosition = effect->position;
    const std::int32_t projectionImage = action->imageNo;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[targetPosition];
    const float targetY = target->offset44;
    const float targetX = target->offset40;
    const float projectionY = action->offset44;
    const float projectionX = action->offset40;
    NewBattleProjection_418DF0(action, projectionX, projectionY, targetX, targetY, projectionImage);
    if (action->imageNo == 110016 || action->imageNo == 111055) {
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
    }
    payload->castSkill103C = parent->payload.battleActor->castSkill103C;
    payload->attackModeFF4 = parent->payload.battleActor->attackModeFF4;
    payload->castSkillLevel1044 = parent->payload.battleActor->castSkillLevel1044;
    payload->skillMeta = parent->payload.battleActor->skillMeta; // E01／419333。
    MoveBattleByAngle_405EA0(action);
    MoveBattleByAngle_405EA0(action);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
    return action;
}

// 原函数：0x00419390。
YobiAction* NewBattleRangeHit_419390(
    YobiAction* const parent, const BattleEntryEffectRecord* const effect, std::int32_t imageNo)
{
    using world::gBattleAllEntries59E980;
    // E08：只识别原弓箭／小刀图号，不影响其他弹道。
    if (imageNo == 22050) imageNo = ResolveSkillEffectImage(parent, SkillEffectKind::Bow, imageNo);
    else if (imageNo == 22090) imageNo = ResolveSkillEffectImage(parent, SkillEffectKind::Knife, imageNo);
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t initialFlags = action->flags | 0x60000u;
    action->callback = UpdateBattleActor_404DB0;
    action->flags = initialFlags;
    action->framePlayingRate = 10;
    action->displayPriority = 203;
    action->offset40 = parent->offset40;
    const float initialY = parent->offset44;
    auto* const payload = action->payload.battleActor;
    action->offset44 = initialY;
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[effect->position], 1, 0.0F);
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    static_cast<void>(render::LoadGraphic_522570(cg::world::gBattleAllEntries59E980[effect->position]->graphicIndex));
    action->motionMagnitude54 = static_cast<float>(render::ReadGraphicDxWidth(parent->graphicIndex) * 0.5);
    MoveBattleByAngle_405EA0(action);
    const double adjustedAngle = action->angle50 + 11.25;
    action->motionMagnitude54 = 10.0F;
    action->imageNo = static_cast<std::int32_t>(adjustedAngle * kBattleDirectionFrameScale);
    if (action->imageNo < 0) {
        action->imageNo += 16;
    }
    if (action->imageNo >= 16) {
        action->imageNo -= 16;
    }
    action->imageNo += imageNo + 16;
    const std::int32_t effectValue4 = effect->position4;
    const std::int32_t effectValue3 = effect->position3;
    const std::uint32_t effectFlags = effect->flags;
    const std::int32_t effectPosition = effect->position;
    AppendBattleEffect_439400(action, {effectPosition, effectFlags, effectValue3, effectValue4});
    const std::uint32_t flags = effect->flags;
    if ((flags & 0x800000u) != 0) {
        std::int32_t position, value3, value4, value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(parent, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->phase = BattleAction_ResolveProjectileHit;
    action->pathCountOrDrawOrder_270 = 0;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    payload->castSkill103C = parent->payload.battleActor->castSkill103C;
    payload->attackModeFF4 = parent->payload.battleActor->attackModeFF4;
    payload->castSkillLevel1044 = parent->payload.battleActor->castSkillLevel1044;
    payload->skillMeta = parent->payload.battleActor->skillMeta; // E01／41951F。
    payload->battlePosition18 = parent->payload.battleActor->battlePosition18;
    const std::int32_t targetPosition = effect->position;
    const std::int32_t projectileImage = action->imageNo;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[targetPosition];
    const std::int32_t projectionImage = projectileImage - 16;
    const render::GraphicDxCache* const graphics = render::gGraphicDxCache103CFA4;
    const std::int16_t targetHeight = render::ReadGraphicDxHeightFromEntries(graphics, target->graphicIndex);
    const std::int32_t parentGraphicIndex = parent->graphicIndex;
    const std::int32_t targetGraphicOffsetY = target->graphicOffsetY;
    const float targetBaseY = target->offset44;
    const float targetX = target->offset40;
    const float targetY = static_cast<float>(targetHeight
        * 0.5 + targetGraphicOffsetY + targetBaseY);
    const std::int16_t parentHeight = render::ReadGraphicDxHeightFromEntries(graphics, parentGraphicIndex);
    const float projectionX = action->offset40;
    const std::int32_t parentGraphicOffsetY = parent->graphicOffsetY;
    const float projectionBaseY = action->offset44;
    const float projectionY = static_cast<float>(parentHeight
        * 0.5 + parentGraphicOffsetY + projectionBaseY);
    NewBattleProjection_418DF0(action, projectionX, projectionY, targetX, targetY, projectionImage);
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(screenY);
    return action;
}

// 原函数：0x00419DA0。
YobiAction* NewBattleArcShot_419DA0(
    YobiAction* const parent, const BattleEntryEffectRecord* const effect,
    const std::int32_t imageNo, const std::int32_t count, const std::int32_t mode)
{
    using world::gBattleAllEntries59E980;
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    auto* const payload = action->payload.battleActor;
    action->flags |= 0x60002u;
    action->callback = UpdateBattleActor_404DB0;
    action->imageNo = 0;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 10.0F;
    action->displayPriority = 203;
    action->offset40 = parent->offset40;
    action->pathCountOrDrawOrder_270 = 0;
    action->offset44 = parent->offset44;
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[effect->position], 0, 0.0F);
    AppendBattleEffect_439400(action, *effect);
    const std::uint32_t flags = effect->flags;
    if ((flags & 0x800000u) != 0) {
        std::int32_t position, value3, value4, value5;
        static_cast<void>(TakeBattleAuxEffects_43BC90(parent, &position, &value3, &value4, &value5));
        AppendBattleAuxEffects_43BC10(action, position, flags, value3, value4, value5);
    }
    action->phase = mode != 0 ? BattleAction_ResolveStraightProjectileHit : BattleAction_ResolveProjectileHit;
    const std::uint32_t hiddenFlags = action->flags | 2u;
    action->imageNo = imageNo;
    action->flags = hiddenFlags;
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    static_cast<void>(render::LoadGraphic_522570(cg::world::gBattleAllEntries59E980[effect->position]->graphicIndex));
    payload->battlePosition18 = parent->payload.battleActor->battlePosition18;
    for (std::int32_t remaining = count; remaining > 0; --remaining) {
        YobiAction* const target = cg::world::gBattleAllEntries59E980[effect->position];
        const std::int32_t y = render::ReadGraphicDxHeight(parent->graphicIndex) / 2 + parent->graphicOffsetY;
        static_cast<void>(NewBattleArcProjection_418FF0(action, 0, y, parent, target, imageNo));
    }
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    payload->castSkill103C = parent->payload.battleActor->castSkill103C;
    payload->attackModeFF4 = parent->payload.battleActor->attackModeFF4;
    payload->castSkillLevel1044 = parent->payload.battleActor->castSkillLevel1044;
    payload->skillMeta = parent->payload.battleActor->skillMeta; // E01／419F44。
    MoveBattleByAngle_405EA0(action);
    MoveBattleByAngle_405EA0(action);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
    return action;
}

// BCPY01：新独立载荷/空返，三次父载荷重取与播放后Y先X发布，不浅克隆。
// 原函数：0x004205F0。
YobiAction* NewBattleCastCopy_4205F0(YobiAction* const parent)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleActor_404DB0;
    auto* const payload = action->payload.battleActor;
    action->displayPriority = parent->displayPriority;
    action->imageNo = parent->imageNo;
    const std::uint32_t flags = action->flags | 0x20000u;
    action->battleAttachmentParent10 = parent;
    action->flags = flags;
    action->motionMagnitude54 = 0.0F;
    action->angle50 = parent->angle50;
    action->direction = parent->direction;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    action->pathCountOrDrawOrder_270 = 0;
    action->phase = BattleAction_Idle;
    payload->castSkill103C = parent->payload.battleActor->castSkill103C;
    payload->attackModeFF4 = parent->payload.battleActor->attackModeFF4;
    payload->battlePosition18 = parent->payload.battleActor->battlePosition18;
    payload->skillMeta = parent->payload.battleActor->skillMeta; // E01／420678。
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
    return action;
}

// B22C01：效果/坐标取样、几何发布与真实同步callback循环见doc/cg2d_battle_boomerang_creator_spec.md。
// 原函数：0x004195F0。
YobiAction* NewBattleBoomerang_4195F0(YobiAction* const parent)
{
    using world::gBattleAllEntries59E980;
    auto* const parentPayload = parent->payload.battleActor;
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleActor_404DB0;
    action->characterAction = CharacterAnimation_Stand;
    action->flags = flags;
    action->framePlayingRate = 100;
    action->displayPriority = 205;
    action->battleAttachmentParent10 = parent;
    const float parentX = parent->offset40;
    auto* const payload = action->payload.battleActor;
    action->offset40 = parentX;
    const float parentY = parent->offset44;
    const float initialFloatX = action->offset40;
    action->offset44 = parentY;
    const std::int32_t initialX = static_cast<std::int32_t>(initialFloatX);
    const float initialY = action->offset44;
    action->x = initialX;
    action->y = static_cast<std::int32_t>(initialY);
    action->imageNo = parentPayload->castSkill103C == BattleSkill_KarmaCycle ? 111141 : 103000;
    action->imageNo = ResolveSkillEffectImage(parent, SkillEffectKind::Boomerang, action->imageNo); // E08／419692。
    action->direction = 0;
    action->pathCountOrDrawOrder_270 = 0;
    payload->battlePosition18 = parentPayload->battlePosition18;
    do {
        if (parentPayload->castSkill103C == BattleSkill_DelayAttack258) {
            const std::int32_t level = parentPayload->castSkillLevel1044;
            const std::int32_t image = level < 9 ? 29105 : (level < 12 ? 29106 : 104217);
            static_cast<void>(NewBattleHitEffect_41F5B0(
                cg::world::gBattleAllEntries59E980[parentPayload->effects1C[parentPayload->effectRunningC9C].position],
                0, 0, 30, image));
        }
        const std::int32_t effectIndex = parentPayload->effectRunningC9C;
        const std::int32_t effectValue4 = parentPayload->effects1C[effectIndex].position4;
        const std::uint32_t appendedFlags = parentPayload->effects1C[effectIndex].flags;
        const std::int32_t effectValue3 = parentPayload->effects1C[effectIndex].position3;
        const std::int32_t effectPosition = parentPayload->effects1C[effectIndex].position;
        AppendBattleEffect_439400(action, {effectPosition, appendedFlags, effectValue3, effectValue4});
        const std::uint32_t effectFlags = parentPayload->effects1C[parentPayload->effectRunningC9C].flags;
        if ((effectFlags & 0x800000u) != 0) {
            std::int32_t position, value3, value4, value5;
            static_cast<void>(TakeBattleAuxEffects_43BC90(parent, &position, &value3, &value4, &value5));
            AppendBattleAuxEffects_43BC10(action, position, effectFlags, value3, value4, value5);
        }
    } while (AdvanceBattleEffect_439470(parent) != 1);
    const auto* const initialEffect = &payload->effects1C[payload->effectRunningC9C];
    payload->x00 = cg::world::gBattleAllEntries59E980[initialEffect->position]->x;
    const std::int32_t initialTargetY = cg::world::gBattleAllEntries59E980[initialEffect->position]->y;
    const std::int32_t firstPosition = payload->effects1C[0].position;
    payload->y04 = initialTargetY;
    payload->caster1070 = parentPayload->caster1070;
    YobiAction* target = cg::world::gBattleAllEntries59E980[firstPosition];
    const float firstTargetY = target->offset44;
    const float firstTargetX = target->offset40;
    const float firstSourceY = action->offset44;
    const float firstSourceX = action->offset40;
    const std::int8_t firstDirection = GetBattleRelativeDirection_405ED0(
        firstSourceX, firstSourceY, firstTargetX, firstTargetY, 5.0F);
    if (payload->effectCountCA0 >= 2) {
        const std::int32_t first = payload->effects1C[0].position;
        const std::int32_t second = payload->effects1C[1].position;
        if (firstDirection == 1) {
            if (cg::world::gBattleAllEntries59E980[first]->offset40 > cg::world::gBattleAllEntries59E980[second]->offset40) {
                const std::int32_t waypointY = payload->y04;
                const std::int32_t waypointX = payload->x00;
                payload->y04 = waypointY - 100;
                payload->x00 = waypointX + 20;
            }
            if (!(cg::world::gBattleAllEntries59E980[first]->offset40 >= cg::world::gBattleAllEntries59E980[second]->offset40)) {
                payload->x00 -= 100;
            }
        } else if (firstDirection == 0) {
            if (cg::world::gBattleAllEntries59E980[first]->offset40 > cg::world::gBattleAllEntries59E980[second]->offset40
                && payload->x00 >= action->x) {
                payload->x00 += 150;
            }
            if (!(cg::world::gBattleAllEntries59E980[first]->offset40 >= cg::world::gBattleAllEntries59E980[second]->offset40)
                && payload->x00 <= action->x) {
                payload->y04 += 150;
            }
        } else if (firstDirection == -1) {
            if (!(cg::world::gBattleAllEntries59E980[first]->offset40 >= cg::world::gBattleAllEntries59E980[second]->offset40)) {
                const std::int32_t waypointY = payload->y04;
                const std::int32_t waypointX = payload->x00;
                payload->y04 = waypointY + 100;
                payload->x00 = waypointX + 20;
            }
            if (cg::world::gBattleAllEntries59E980[first]->offset40 > cg::world::gBattleAllEntries59E980[second]->offset40) {
                payload->x00 += 100;
            }
        }
        target = cg::world::gBattleAllEntries59E980[payload->effects1C[0].position];
        const float relativeTargetY = target->offset44;
        const float relativeTargetX = target->offset40;
        const float relativeSourceY = action->offset44;
        const float relativeSourceX = action->offset40;
        const std::int8_t direction = GetBattleRelativeDirection_405ED0(
            relativeSourceX, relativeSourceY, relativeTargetX, relativeTargetY, 5.0F);
        const bool firstOnRight = cg::world::gBattleAllEntries59E980[payload->effects1C[0].position]->offset40
            >= cg::world::gBattleAllEntries59E980[payload->effects1C[1].position]->offset40;
        if ((direction < 0 && firstOnRight) || (direction > 0 && !firstOnRight)) {
            std::int32_t ratio = 0; // adjust仅在ratio计算完成后置true。
            bool adjust = false;
            if (payload->y04 < action->y) {
                if (payload->x00 > action->x) {
                    ratio = 100 * (payload->x00 - action->x) / (action->y - payload->y04);
                    adjust = true;
                }
            } else if (payload->x00 < action->x) {
                ratio = 100 * (action->x - payload->x00) / (payload->y04 - action->y);
                adjust = true;
            }
            if (adjust) {
                if (ratio > 100) {
                    ratio = 100;
                }
                if (direction < 0) {
                    const std::int32_t adjustedY = payload->y04 + ratio;
                    const std::int32_t waypointX = payload->x00;
                    payload->y04 = adjustedY;
                    payload->x00 = waypointX + ratio / 2;
                }
                if (direction > 0) {
                    const std::int32_t adjustedY = payload->y04 - ratio;
                    const std::int32_t waypointX = payload->x00;
                    payload->y04 = adjustedY;
                    payload->x00 = waypointX + -(ratio / 2);
                }
            }
        } else {
            std::int32_t ratio = 0; // adjust仅在ratio计算完成后置true。
            bool adjust = false;
            if (payload->x00 < action->x) {
                if (payload->y04 < action->y) {
                    ratio = 100 * (action->y - payload->y04) / (action->x - payload->x00);
                    adjust = true;
                }
            } else if (payload->y04 > action->y) {
                ratio = 100 * (payload->y04 - action->y) / (payload->x00 - action->x);
                adjust = true;
            }
            if (adjust) {
                if (ratio > 100) {
                    ratio = 100;
                }
                if (direction < 0) {
                    payload->x00 -= ratio;
                    payload->y04 += ratio / 2;
                }
                if (direction > 0) {
                    const std::int32_t adjustedX = payload->x00 + ratio;
                    const std::int32_t waypointY = payload->y04;
                    payload->x00 = adjustedX;
                    payload->y04 = waypointY + -(ratio / 2);
                }
            }
        }
    }
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 0, 0.0F);
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    action->phase = BattleAction_MoveBoomerangToTurnPoint;
    action->motionMagnitude54 = 10.0F;
    action->effectAcceleration48 = 10.0F;
    action->field1D0 = 0;
    const float halfWidth = static_cast<float>(render::ReadGraphicDxWidth(parent->graphicIndex) * 0.5);
    do {
        action->callback(action);
        if (action->phase != BattleAction_MoveBoomerangToTurnPoint) {
            break;
        }
    } while (static_cast<double>(GetBattleDistance_406300(action, parent)) < halfWidth);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    payload->castSkill103C = parentPayload->castSkill103C;
    payload->attackModeFF4 = parentPayload->attackModeFF4;
    payload->castSkillLevel1044 = parentPayload->castSkillLevel1044;
    payload->skillMeta = parentPayload->skillMeta; // E01／419BB2。
    return action;
}

// BSEX01：移动/严格终帧标死后仍投影；X转换、Y取样先于X发布。
// 原函数：0x0041E4D0。
void UpdateBattleScatterEffect_41E4D0(YobiAction* const action)
{
    MoveBattleByAngle_405EA0(action);
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        static_cast<void>(MarkDeath(action));
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
}

// BSEX01：分配空不消耗RNG；三原参数依序，X发布后Y取样/低W16再Y发布。
// 原函数：0x0041E510。
YobiAction* NewBattleScatterEffect_41E510(YobiAction* const parent, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleScatterEffect_41E4D0;
    action->displayPriority = 0xCC;
    const std::int32_t rate = platform::RandomRange_46FAF0(10, 200);
    const std::uint32_t flags = action->flags;
    action->framePlayingRate = rate;
    action->flags = flags | 0x40000u;
    action->imageNo = imageNo;
    action->direction = 0;
    action->angle50 = static_cast<float>(platform::RandomRange_46FAF0(0, 359));
    action->motionMagnitude54 = static_cast<float>(platform::RandomRange_46FAF0(5, 20));
    action->offset40 = parent->offset40;
    const std::int32_t halfOffsetY = parent->graphicOffsetY / 2;
    const double y = static_cast<double>(halfOffsetY) + parent->offset44;
    action->pathCountOrDrawOrder_270 = 0;
    action->offset44 = static_cast<float>(y);
    return action;
}

// 原函数：0x0041E640。
void UpdateBattlePositionEffect_41E640(YobiAction* const action)
{
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        static_cast<void>(MarkDeath(action));
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
}

// 原函数：0x0041E680。
YobiAction* NewBattlePositionEffect_41E680(YobiAction* const parent, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattlePositionEffect_41E640;
    action->displayPriority = 0xCC;
    action->framePlayingRate = 100;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
    action->flags = flags;
    action->flags = flags | 0x60000u;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    const float screenX = action->offset40;
    action->imageNo = imageNo;
    action->direction = 0;
    const std::int32_t x = static_cast<std::int32_t>(screenX);
    const float screenY = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(screenY);
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x0041E720。
YobiAction* NewBattleCoordinateEffect_41E720(const float x, const float y, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattlePositionEffect_41E640;
    action->displayPriority = 0xCC;
    action->framePlayingRate = 100;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
    action->flags = flags;
    action->flags = flags | 0x60000u;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->offset44 = y;
    action->offset40 = x;
    action->imageNo = imageNo;
    action->direction = 0;
    action->x = static_cast<std::int32_t>(x);
    action->y = static_cast<std::int32_t>(y);
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x0041E7C0。
YobiAction* NewBattleTrailEffect_41E7C0(
    const float x, const float y, const std::int32_t imageNo,
    const std::int32_t mode, const std::uint8_t priority)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattlePositionEffect_41E640;
    action->displayPriority = priority;
    action->framePlayingRate = 200;
    if (render::HasLegacy3DDevice_44B610() != 0) {
        action->flags |= 0x100000u;
    } else {
        const std::int32_t modeFlag = platform::RandomRange_46FAF0(0, 1);
        action->flags |= modeFlag == 0 ? 0x10000u : 0x8000u;
    }
    if (mode == 1) {
        action->flags |= 0x20000u;
    }
    action->flags |= 0x40000u;
    action->offset40 = x;
    action->offset44 = y;
    action->imageNo = imageNo;
    action->direction = 0;
    action->x = static_cast<std::int32_t>(x);
    action->y = static_cast<std::int32_t>(y);
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x00419FA0。
void UpdateBattleOneShotEffect_419FA0(YobiAction* const action)
{
    if (PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x00419FC0。
YobiAction* NewBattleRandomEffect_419FC0(
    YobiAction* const parent, const std::int32_t imageNo, const std::int32_t direction)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleOneShotEffect_419FA0;
    action->displayPriority = 0xCC;
    action->framePlayingRate = 100;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x10000u);
    action->flags = flags;
    const std::uint32_t configuredFlags = flags | 0x40000u;
    action->imageNo = imageNo;
    action->flags = configuredFlags;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    const std::int32_t randomX = platform::RandomRange_46FAF0(
        0, render::ReadGraphicDxWidth(parent->graphicIndex) / 2);
    const std::int32_t quarterWidth = render::ReadGraphicDxWidth(parent->graphicIndex) / 4;
    action->x = parent->x - quarterWidth + randomX;
    const std::int32_t randomY = platform::RandomRange_46FAF0(0, parent->graphicOffsetY / 3);
    const std::int32_t quarterHeight = parent->graphicOffsetY / 4;
    const std::int32_t y = quarterHeight + parent->y - randomY;
    action->direction = direction;
    action->y = y;
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x0041A0D0。
YobiAction* NewBattleScreenEffect_41A0D0(
    const std::int32_t x, const std::int32_t y, const std::int32_t imageNo, const std::int32_t direction)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleOneShotEffect_419FA0;
    action->displayPriority = 0xCC;
    action->framePlayingRate = 100;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
    action->flags = flags;
    action->flags = flags | 0x40000u;
    action->x = x;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->y = y;
    action->direction = direction;
    action->imageNo = imageNo;
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// BRAE01：固定两载荷；首段创建后再取目标S8类型，依次发布三个BYTE。
// 原函数：0x004208F0。
void NewBattleRebirthEffects_4208F0(YobiAction* const source, YobiAction* const target)
{
    if (source == nullptr || target == nullptr) {
        return;
    }
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    const auto* const targetPayload = static_cast<const BattleActorPayload*>(target->payload.battleActor);
    constexpr std::array<std::int32_t, 8> images {
        0x1AF5F, 0x1AF6B, 0x1AF77, 0x1AF83, 0x1AF5E, 0x1AF6A, 0x1AF76, 0x1AF82};
    const std::int8_t sourceType = sourcePayload->rebirthEffectType1080;
    if (sourceType != 0) {
        YobiAction* const effect = NewBattleRandomEffect_419FC0(target, images[sourceType - 1], 0);
        reinterpret_cast<unsigned char*>(&effect->drawMode.color)[3] = 0x96;
        effect->rebirthValue1 = 4;
        effect->rebirthValue2 = 4;
    }
    const std::int8_t targetType = targetPayload->rebirthEffectType1080;
    if (targetType != 0) {
        YobiAction* const effect = NewBattleRandomEffect_419FC0(target, images[targetType + 3], 0);
        reinterpret_cast<unsigned char*>(&effect->drawMode.color)[3] = 0x96;
        effect->rebirthValue1 = 4;
        effect->rebirthValue2 = 4;
    }
}

// BRAE01：门禁/四W16向量先取齐；八配对依次调用，失败仍置位并占纵位置。
// 原函数：0x004209C0。
std::uint8_t NewBattleAttributeEffects_4209C0(YobiAction* const source, YobiAction* const target)
{
    const auto* const sourcePayload = static_cast<const BattleActorPayload*>(source->payload.battleActor);
    if ((static_cast<std::uint8_t>(sourcePayload->stateFE0) & 1u) == 0) {
        return 0;
    }
    const auto* const targetPayload = static_cast<const BattleActorPayload*>(target->payload.battleActor);
    if ((static_cast<std::uint8_t>(targetPayload->stateFE0) & 1u) == 0) {
        return 0;
    }
    const std::uint8_t flags = cg::world::gBattleAttributeEffectFlags59F9E0;
    const std::int32_t battleType = cg::world::gBattleType5ABFC8;
    if ((flags & 1u) == 0 && battleType != 5 && battleType != 6) {
        return 0;
    }
    if ((flags & 2u) == 0 && cg::world::gBattleModeFlag5ABFCC == 1) {
        return 0;
    }
    if ((flags & 4u) == 0 && (battleType == 5 || battleType == 6)) {
        return 0;
    }
    if (source == nullptr || target == nullptr || sourcePayload == nullptr || targetPayload == nullptr) {
        return 0;
    }
    std::array<std::int32_t, 4> sourceAttributes {};
    switch (sourcePayload->castSkill103C) {
    case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicEarthStrong: case BattleSkill_AttackMagicEarthSuper:
    case 2068: case 2069: case 2070:
        sourceAttributes[0] = 1;
        break;
    case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicWaterStrong: case BattleSkill_AttackMagicWaterSuper:
    case 2071: case 2072: case 2073:
        sourceAttributes[1] = 1;
        break;
    case BattleSkill_AttackMagicFire: case BattleSkill_AttackMagicFireStrong: case BattleSkill_AttackMagicFireSuper:
    case 2074: case 2075: case 2076:
        sourceAttributes[2] = 1;
        break;
    case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicWindSuper:
    case 2077: case 2078: case 2079:
        sourceAttributes[3] = 1;
        break;
    case 2055: case 2056: case 2057:
        break;
    default:
        for (std::size_t index = 0; index < sourceAttributes.size(); ++index) {
            sourceAttributes[index] = source->fields288[index] != 0 ? 1 : 0;
        }
        if ((sourcePayload->flagsFDC & BattleActorFlag_ReverseType) != 0) {
            sourceAttributes = {sourceAttributes[2], sourceAttributes[3], sourceAttributes[0], sourceAttributes[1]};
        }
        break;
    }
    std::array<std::int32_t, 4> targetAttributes {};
    for (std::size_t index = 0; index < targetAttributes.size(); ++index) {
        targetAttributes[index] = target->fields288[index] != 0 ? 1 : 0;
    }
    if ((targetPayload->flagsFDC & BattleActorFlag_ReverseType) != 0) {
        targetAttributes = {targetAttributes[2], targetAttributes[3], targetAttributes[0], targetAttributes[1]};
    }
    struct AttributeEffect {
        std::size_t sourceIndex;
        std::size_t targetIndex;
        std::int32_t imageNo;
    };
    constexpr std::array<AttributeEffect, 8> effects {{
        {0, 1, 22455}, {1, 2, 22456}, {2, 3, 22457}, {3, 0, 22458},
        {0, 3, 22450}, {1, 0, 22451}, {2, 1, 22452}, {3, 2, 22453}}};
    std::uint8_t result = 0;
    std::int32_t y = -20;
    for (std::size_t index = 0; index < effects.size(); ++index) {
        const AttributeEffect& effect = effects[index];
        if (sourceAttributes[effect.sourceIndex] != 0 && targetAttributes[effect.targetIndex] != 0) {
            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, effect.imageNo, 0, y));
            result |= static_cast<std::uint8_t>(1u << index);
            y -= 22;
        }
    }
    return result;
}

// BAS01：六图号先改FDC/13D0/13CC，分配失败不回滚；播放后fresh父载荷写1054再绑定父。
// 原函数：0x0041F660。
YobiAction* NewBattleDebuffEffect_41F660(YobiAction* const parent, const std::int32_t imageNo)
{
    auto* const payload = parent->payload.battleActor;
    if (payload->debuffEffect1054 != nullptr) {
        return nullptr;
    }
    std::int32_t type = CG_BATTLE_DEBUFF_NONE;
    switch (imageNo) {
    case 110075: payload->flagsFDC |= BattleActorFlag_Poison; type = CG_BATTLE_DEBUFF_POISON; break;
    case 110076: payload->flagsFDC |= BattleActorFlag_Sleep; type = CG_BATTLE_DEBUFF_SLEEP; break;
    case 110077: payload->flagsFDC |= BattleActorFlag_Stone; type = CG_BATTLE_DEBUFF_STONE; break;
    case 110078: payload->flagsFDC |= BattleActorFlag_Inebriety; type = CG_BATTLE_DEBUFF_INEBRIETY; break;
    case 110079: payload->flagsFDC |= BattleActorFlag_Confusion; type = CG_BATTLE_DEBUFF_CONFUSION; break;
    case 110080: payload->flagsFDC |= BattleActorFlag_Forget; type = CG_BATTLE_DEBUFF_FORGET; break;
    default: break;
    }
    if (type != CG_BATTLE_DEBUFF_NONE) {
        auto* const currentPayload = parent->payload.battleActor;
        currentPayload->debuffType13D0 = type;
        currentPayload->debuffState13CC = CG_BATTLE_STATUS_MODE_DEBUFF;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleDebuffEffect_41F800;
    action->displayPriority = 9;
    action->framePlayingRate = 100;
    action->flags = flags | 0x60000u;
    action->offset40 = parent->offset40;
    const double y = parent->offset44 + 1.0;
    action->imageNo = imageNo;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->offset44 = static_cast<float>(y);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    parent->payload.battleActor->debuffEffect1054 = action;
    action->battleAttachmentParent10 = parent;
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float currentY = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(currentY);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->pathCountOrDrawOrder_270 = 0;
    action->y = projectedY + offsetY;
    return action;
}

// BAS01：1054/105C共用借父回调，Play后重借父跟随；标死仍投影，Y转换后取屏幕偏移。
// 原函数：0x0041F800。
void UpdateBattleDebuffEffect_41F800(YobiAction* const action)
{
    if (action->battleAttachmentParent10->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    } else {
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        YobiAction* const parent = action->battleAttachmentParent10;
        action->offset40 = parent->offset40;
        action->offset44 = static_cast<float>(parent->offset44 + 1.0);
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(y);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->y = projectedY + offsetY;
}

// BAS01：105C已有返空，设备后持flags两次发布；播放后fresh父载荷写105C再绑定父。
// 原函数：0x0041F860。
YobiAction* NewBattleReverseEffect_41F860(YobiAction* const parent, const std::int32_t imageNo)
{
    if (parent->payload.battleActor->attributeReversalEffect105C != nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleDebuffEffect_41F800;
    action->displayPriority = 0xC8;
    action->framePlayingRate = 100;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
    action->flags = flags;
    action->flags = flags | 0x40000u;
    action->offset40 = parent->offset40;
    const double y = parent->offset44 + 1.0;
    action->imageNo = imageNo;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->offset44 = static_cast<float>(y);
    action->rebirthValue2 = 4;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    parent->payload.battleActor->attributeReversalEffect105C = action;
    action->battleAttachmentParent10 = parent;
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float currentY = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(currentY);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->pathCountOrDrawOrder_270 = 0;
    action->y = projectedY + offsetY;
    return action;
}

// 原函数：0x00419BE0。
void UpdateBattleTransitionEffect_419BE0(YobiAction* const action)
{
    if (action->phase == BattleTransitionPhase_Playing && PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd) == AnimationFrameResult_StoppedOrUnavailable) {
        const std::uint32_t flags = action->flags;
        action->phase = BattleTransitionPhase_HiddenAfterStop;
        action->flags = flags | 2u;
    }
}

// 原函数：0x00419C10。
YobiAction* NewBattleTransitionEffect_419C10(YobiAction* const parent, const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleTransitionEffectPayload), YobiPayloadKind::BattleTransitionEffect);
    if (action == nullptr) {
        return nullptr;
    }
    action->payload.battleTransitionEffect->parent00 = parent;
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleTransitionEffect_419BE0;
    action->flags = flags;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 0.0F;
    action->displayPriority = 204;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    action->pathCountOrDrawOrder_270 = 0;
    action->phase = BattleTransitionPhase_Playing;
    action->field1D0 = 0;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
    return action;
}

// 原函数：0x00419CB0。
void UpdateBattleOffsetFollowEffect_419CB0(YobiAction* const action)
{
    auto* const payload = action->payload.battleFollowOffset;
    YobiAction* const parent = payload->parent00;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    }
    const std::int32_t parentX = parent->x;
    const std::int32_t offsetX = payload->offsetX04;
    action->x = parentX + offsetX;
    const std::int32_t parentY = parent->y;
    const std::int32_t offsetY = payload->offsetY08;
    action->y = parentY + offsetY;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
}

// 原函数：0x00419CF0。
YobiAction* NewBattleOffsetFollowEffect_419CF0(
    YobiAction* const parent, const std::int32_t offsetX, const std::int32_t offsetY,
    const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleFollowOffsetPayload), YobiPayloadKind::BattleFollowOffset);
    if (action == nullptr) {
        return nullptr;
    }
    auto* const payload = action->payload.battleFollowOffset;
    payload->parent00 = parent;
    const std::uint32_t flags = action->flags | 0x60000u;
    action->callback = UpdateBattleOffsetFollowEffect_419CB0;
    action->flags = flags;
    action->imageNo = imageNo;
    action->characterAction = CharacterAnimation_Stand;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 0.0F;
    action->displayPriority = 9;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    payload->offsetX04 = offsetX;
    payload->offsetY08 = offsetY;
    action->pathCountOrDrawOrder_270 = 0;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    const double x = static_cast<double>(payload->offsetX04) + action->offset40;
    action->x = static_cast<std::int32_t>(x);
    const double y = static_cast<double>(payload->offsetY08) + action->offset44;
    action->y = static_cast<std::int32_t>(y);
    return action;
}

// BDE01：固定parent严格标死仍执行共同投影尾，不清1064或立即释放。
// 原函数：0x0041F950。
void UpdateBattleCenteredEffect_41F950(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    } else {
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
        const std::int32_t halfWidth = render::ReadGraphicDxWidth(parent->graphicIndex) / 2;
        action->offset40 = static_cast<float>(
            static_cast<double>(halfWidth) + parent->graphicOffsetX + parent->offset40);
        const std::int32_t halfHeight = render::ReadGraphicDxHeight(parent->graphicIndex) / 2;
        action->offset44 = static_cast<float>(
            static_cast<double>(halfHeight) + parent->graphicOffsetY + parent->offset44);
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(gYobiScreenOffsetY);
}

// BDE01：已有1064/分配空早返；两S16向零除2、播放后发布与投影读齐保持。
// 原函数：0x0041FA00。
YobiAction* NewBattleCenteredEffect_41FA00(YobiAction* const parent, const std::int32_t imageNo)
{
    if (parent->payload.battleActor->centeredEffect1064 != nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleCenteredEffect_41F950;
    action->displayPriority = 0xC8;
    action->framePlayingRate = 100;
    action->flags = flags | 0x40000u;
    static_cast<void>(render::LoadGraphic_522570(parent->graphicIndex));
    const std::int32_t halfWidth = render::ReadGraphicDxWidth(parent->graphicIndex) / 2;
    action->offset40 = static_cast<float>(
        static_cast<double>(halfWidth) + parent->graphicOffsetX + parent->offset40);
    const std::int32_t halfHeight = render::ReadGraphicDxHeight(parent->graphicIndex) / 2;
    const double centerY = static_cast<double>(halfHeight) + parent->graphicOffsetY + parent->offset44;
    action->imageNo = imageNo;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->offset44 = static_cast<float>(centerY);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    parent->payload.battleActor->centeredEffect1064 = action;
    action->battleAttachmentParent10 = parent;
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(y);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->pathCountOrDrawOrder_270 = 0;
    action->y = projectedY + offsetY;
    return action;
}

// 原函数：0x0041FB20。
void UpdateBattleAttachedEffect_41FB20(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    } else {
        const auto* const payload = static_cast<const BattleActorPayload*>(parent->payload.battleActor);
        YobiAction* const effect = payload->statusEffect1068;
        const std::int32_t count = action->deltaX;
        if (effect != nullptr) {
            if (count > -10) {
                action->deltaX = count - 1;
            }
        } else if (count < 0) {
            action->deltaX = count + 1;
        }
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        if (world::gCurrentSceneType == 10 && world::gCurrentSceneState == 6) {
            action->flags &= ~2u;
        } else {
            action->flags |= 2u;
        }
        if ((action->batchType & 8u) != 0
            && (static_cast<std::uint32_t>(action->color2) >> 24) == 0) {
            action->flags = 2;
        }
        const std::int32_t position = action->deltaX;
        YobiAction* const source = action->battleAttachmentParent10;
        action->offset40 = static_cast<float>(static_cast<double>(position) + source->offset40);
        action->offset44 = source->offset44;
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(gYobiScreenOffsetY);
}

// 原函数：0x0041FBE0。
YobiAction* NewBattleAttachedEffect_41FBE0(YobiAction* const parent, const std::int32_t mode)
{
    YobiAction* const existing = parent->payload.battleActor->attachedEffect1058;
    if (existing != nullptr) {
        existing->imageNo = mode == 1 ? 22326 : 22327;
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleAttachedEffect_41FB20;
    action->displayPriority = 0xC8;
    action->framePlayingRate = 100;
    action->flags = flags;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    action->imageNo = mode == 1 ? 22326 : 22327;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    parent->payload.battleActor->attachedEffect1058 = action;
    action->battleAttachmentParent10 = parent;
    action->x = static_cast<std::int32_t>(action->offset40);
    action->y = static_cast<std::int32_t>(action->offset44);
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x0041FCD0。
void UpdateBattleStatusEffect_41FCD0(YobiAction* const action)
{
    YobiAction* const parent = action->battleAttachmentParent10;
    if (parent->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    } else {
        YobiAction* const attached = parent->payload.battleActor->attachedEffect1058;
        if (attached != nullptr) {
            const std::uint8_t flags = static_cast<std::uint8_t>(attached->flags);
            const std::int32_t count = action->deltaX;
            if ((flags & 2u) != 0) {
                if (count > 0) {
                    action->deltaX = count - 1;
                }
            } else if (count < 10) {
                action->deltaX = count + 1;
            }
        } else {
            const std::int32_t count = action->deltaX;
            if (count > 0) {
                action->deltaX = count - 1;
            }
        }
        const std::int32_t initialY = action->deltaY;
        if (initialY != 0) {
            constexpr double growth = 0.04F;
            const float scaleX = static_cast<float>(action->scaleX + growth);
            action->scaleX = scaleX;
            const double scaleY = action->scaleY + growth;
            action->scaleY = static_cast<float>(scaleY);
            if (scaleX > 1.0F) {
                action->scaleX = 1.0F;
            }
            if (scaleY > 1.0) {
                action->scaleY = 1.0F;
            }
            if (action->scaleY >= 1.0F) {
                action->effectAcceleration48 = static_cast<float>(
                    action->effectAcceleration48 + 1.0);
            }
            if (action->effectAcceleration48 >= 30.0F) {
                action->deltaY = initialY + 7;
            }
            if (action->deltaY > 0) {
                action->deltaY = 0;
            }
        }
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
        const std::int32_t position = action->deltaX;
        YobiAction* const source = action->battleAttachmentParent10;
        action->offset40 = static_cast<float>(static_cast<double>(position) + source->offset40);
        action->offset44 = static_cast<float>(source->offset44 - 5.0 + action->deltaY);
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    action->y = static_cast<std::int32_t>(y) + static_cast<std::int32_t>(gYobiScreenOffsetY);
}

// 原函数：0x0041FE00。
YobiAction* NewBattleStatusEffect_41FE00(YobiAction* const parent, const std::int32_t imageNo,
    const std::int32_t mode)
{
    if (parent->payload.battleActor->statusEffect1068 != nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleStatusEffect_41FCD0;
    auto* const payload = parent->payload.battleActor;
    std::int32_t displayImage = imageNo;
    if (payload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_EXTENDED) {
        switch (imageNo) {
        case 103018: payload->flagsFDC |= BattleActorFlag_DefenseUp; break;
        case 103019: payload->flagsFDC |= BattleActorFlag_DefenseDown; break;
        case 103020: payload->flagsFDC |= BattleActorFlag_AttackUp; break;
        case 103021: payload->flagsFDC |= BattleActorFlag_AttackDown; break;
        case 103022: payload->flagsFDC |= BattleActorFlag_AgilityUp; break;
        case 103023: payload->flagsFDC |= BattleActorFlag_AgilityDown; break;
        case 103033: payload->flagsFDC |= BattleActorFlag_RecoveryUp; displayImage = 103018; break;
        default: break;
        }
    }
    const std::uint32_t flags = action->flags;
    action->displayPriority = 0xC8;
    action->framePlayingRate = 100;
    action->flags = flags | 0x40000u;
    action->offset40 = parent->offset40;
    action->offset44 = parent->offset44;
    action->imageNo = displayImage;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    parent->payload.battleActor->statusEffect1068 = action;
    action->battleAttachmentParent10 = parent;
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const float y = action->offset44;
    action->x = x;
    action->y = static_cast<std::int32_t>(y);
    action->pathCountOrDrawOrder_270 = 0;
    if (mode == 1) {
        action->deltaY = -50;
        action->scaleX = 0.3F;
        action->scaleY = 0.3F;
    } else {
        action->deltaY = 0;
    }
    return action;
}

// BFL01：当前场景death严格1标死自身，仍投影；Y转换后再取屏幕Y偏移。
// 原函数：0x0041FF90。
void UpdateBattleFieldEffect_41FF90(YobiAction* const action)
{
    if (world::gBattleSceneAction5ABA94->deathFlag == 1) {
        static_cast<void>(MarkDeath(action));
    } else {
        static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_Loop));
    }
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(y);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->y = projectedY + offsetY;
}

// BFL01：B0已有只换图返空；两组索引先齐读坐标，播放后fresh场景发布，原投影尾。
// 原函数：0x0041FFE0。
YobiAction* NewBattleFieldEffect_41FFE0(const std::int32_t imageNo)
{
    using world::gBattleSceneAction5ABA94;
    using world::gBattleGraphicPoints553758;
    using world::gBattlePointIndices5536FC;
    YobiAction* const existing = cg::world::gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0;
    if (existing != nullptr) {
        existing->imageNo = imageNo;
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleFieldEffect_41FF90;
    action->displayPriority = 7;
    action->framePlayingRate = 100;
    action->flags = flags | 0x40000u;
    const std::int32_t x15Index = cg::world::gBattlePointIndices5536FC[15];
    const std::int32_t x5Index = cg::world::gBattlePointIndices5536FC[5];
    const double x15 = cg::world::gBattleGraphicPoints553758[x15Index][2];
    const double x5 = cg::world::gBattleGraphicPoints553758[x5Index][2];
    action->offset40 = static_cast<float>((x5 - x15) * 0.5 + x15);
    const std::int32_t y15Index = cg::world::gBattlePointIndices5536FC[15];
    const std::int32_t y5Index = cg::world::gBattlePointIndices5536FC[5];
    const double y15 = cg::world::gBattleGraphicPoints553758[y15Index][3];
    const double y5 = cg::world::gBattleGraphicPoints553758[y5Index][3];
    action->imageNo = imageNo;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->offset44 = static_cast<float>((y5 - y15) * 0.5 + y15);
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    cg::world::gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0 = action;
    const std::int32_t x = static_cast<std::int32_t>(action->offset40);
    const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
    const float y = action->offset44;
    action->x = x + offsetX;
    const std::int32_t projectedY = static_cast<std::int32_t>(y);
    const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
    action->pathCountOrDrawOrder_270 = 0;
    action->y = projectedY + offsetY;
    return action;
}

// 原函数：0x00421480。
void UpdateBattleBackgroundFade_421480(YobiAction* const action)
{
    using world::gBattleBackgroundImage59F9F8;
    switch (action->phase) {
    case BattleBackgroundFadePhase_IncreasingAlpha: {
        const std::uint8_t alpha = GetYobiActionField30F(action);
        const std::int32_t step = action->field58;
        if (alpha + step < 255) {
            SetYobiActionField30F(action,
                static_cast<std::uint8_t>(alpha + static_cast<std::uint8_t>(action->field58)));
            break;
        }
        const std::int32_t publish = action->counter28;
        SetYobiActionField30F(action, 255);
        if (publish != 0) {
            cg::world::gBattleBackgroundImage59F9F8 = action->imageNo;
            action->phase = BattleBackgroundFadePhase_Finishing;
        }
        [[fallthrough]];
    }
    case BattleBackgroundFadePhase_ShowingWithAlphaOverride: {
        const std::uint32_t flags = action->flags & ~2u;
        const std::int32_t publish = action->counter28;
        SetYobiActionField308(action, 8);
        action->flags = flags;
        if (publish != 0) {
            cg::world::gBattleBackgroundImage59F9F8 = action->imageNo;
            action->phase = BattleBackgroundFadePhase_Finishing;
        }
        break;
    }
    case BattleBackgroundFadePhase_DecreasingAlpha: {
        const std::uint8_t alpha = GetYobiActionField30F(action);
        const std::int32_t step = action->field58;
        SetYobiActionField308(action, 8);
        if (alpha - step > 0) {
            SetYobiActionField30F(action,
                static_cast<std::uint8_t>(alpha - static_cast<std::uint8_t>(action->field58)));
            break;
        }
        SetYobiActionField30F(action, 0);
        [[fallthrough]];
    }
    case BattleBackgroundFadePhase_HidingAndFinishing: {
        const std::uint32_t flags = action->flags;
        SetYobiActionField308(action, 0);
        action->flags = flags | 2u;
        [[fallthrough]];
    }
    case BattleBackgroundFadePhase_Finishing:
        static_cast<void>(MarkDeath(action));
        break;
    default:
        break;
    }
}

// 原函数：0x00421570。
YobiAction* NewBattleBackgroundFade_421570(
    const std::int32_t imageNo, const std::uint8_t step, const std::int32_t publishBackground)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->callback = UpdateBattleBackgroundFade_421480;
    action->flags = flags;
    action->imageNo = imageNo;
    action->graphicIndex = -1;
    if (imageNo >= 0) {
        static_cast<void>(resource::GetGraphicMapNoIndex(imageNo, &action->graphicIndex));
    }
    action->characterAction = CharacterAnimation_Stand;
    action->field58 = step;
    action->framePlayingRate = 100;
    action->motionMagnitude54 = 0.0F;
    action->displayPriority = 3;
    action->offset40 = 0.0F;
    action->offset44 = 0.0F;
    action->phase = BattleBackgroundFadePhase_IncreasingAlpha;
    action->counter28 = publishBackground;
    SetYobiActionField308(action, 8);
    SetYobiActionField30F(action, 0);
    action->x = 0;
    action->y = 0;
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

// 原函数：0x00422370。
void RestoreBattleStateEffects_422370(YobiAction* const parent)
{
    auto* const payload = parent->payload.battleActor;
    const std::uint32_t flags = payload->flagsFDC;
    if ((flags & BattleActorFlag_Poison) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110075));
    } else {
        payload->flagsFDC = flags & ~BattleActorFlag_Poison;
    }
    if ((flags & BattleActorFlag_Sleep) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110076));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_Sleep;
    }
    if ((flags & BattleActorFlag_Stone) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110077));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_Stone;
    }
    if ((flags & BattleActorFlag_Inebriety) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110078));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_Inebriety;
    }
    if ((flags & BattleActorFlag_Confusion) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110079));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_Confusion;
    }
    if ((flags & BattleActorFlag_Forget) != 0) {
        static_cast<void>(NewBattleDebuffEffect_41F660(parent, 110080));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_Forget;
    }
    const float scale = (flags & BattleActorFlag_DamageUp) != 0 ? 1.3F : 1.0F;
    const bool hasAttachedEffect = (flags & BattleActorFlag_TwoActions) != 0;
    parent->scaleX = scale;
    parent->scaleY = scale;
    if (hasAttachedEffect) {
        static_cast<void>(NewBattleAttachedEffect_41FBE0(parent, 1));
    } else {
        static_cast<void>(MarkDeath(parent->payload.battleActor->attachedEffect1058));
        parent->payload.battleActor->attachedEffect1058 = nullptr;
    }
    const std::uint32_t currentFlags = payload->flagsFDC;
    if ((flags & BattleActorFlag_ReverseType) != 0) {
        payload->flagsFDC = currentFlags | BattleActorFlag_ReverseType;
        static_cast<void>(NewBattleReverseEffect_41F860(parent, 110152));
    } else {
        YobiAction* const effect = payload->attributeReversalEffect105C;
        payload->flagsFDC = currentFlags & ~BattleActorFlag_ReverseType;
        static_cast<void>(MarkDeath(effect));
        payload->attributeReversalEffect105C = nullptr;
    }
    if ((flags & BattleActorFlag_Morph) != 0) {
        payload->flagsFDC |= BattleActorFlag_Morph;
    }
    // 原端每对状态共用清位掩码；不能改为清除各自的选择位。
    if ((flags & BattleActorFlag_DefenseUp) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103018, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_DefenseUp;
    }
    if ((flags & BattleActorFlag_DefenseDown) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103019, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_DefenseUp;
    }
    if ((flags & BattleActorFlag_AttackUp) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103020, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_AttackUp;
    }
    if ((flags & BattleActorFlag_AttackDown) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103021, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_AttackUp;
    }
    if ((flags & BattleActorFlag_AgilityUp) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103022, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_AgilityUp;
    }
    if ((flags & BattleActorFlag_AgilityDown) != 0) {
        static_cast<void>(NewBattleStatusEffect_41FE00(parent, 103023, 0));
    } else {
        payload->flagsFDC &= ~BattleActorFlag_AgilityUp;
    }
}

// 原函数：0x00421610。
YobiAction* NewBattleScreenEffect_421610(const std::int32_t x, const std::int32_t y)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = UpdateBattleOneShotEffect_419FA0;
    action->displayPriority = 0xCC;
    action->framePlayingRate = 100;
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->rebirthValue2 = 0;
        const std::uint32_t flags = action->flags;
        action->rebirthValue1 = 8;
        action->flags = flags | 0x8000u;
    }
    const std::uint32_t flags = action->flags | 0x40000u;
    action->imageNo = 110468;
    action->flags = flags;
    action->rebirthValue1 = 4;
    action->rebirthValue2 = 4;
    action->direction = 0;
    action->x = x;
    action->pathCountOrDrawOrder_270 = 0;
    action->y = y;
    return action;
}

// 原函数：0x004216B0。
void UpdateBattleScreenEmitter_4216B0(YobiAction* const action)
{
    if ((static_cast<std::uint8_t>(action->phase) & 7u) == 0) {
        const std::int32_t y = platform::Rand16() % 480;
        const std::int32_t x = platform::Rand16() % 640;
        static_cast<void>(NewBattleScreenEffect_421610(x, y));
    }
    const std::int32_t phase = action->phase - 1;
    action->phase = phase;
    if (phase < 0) {
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x00421700。
YobiAction* NewBattleScreenEmitter_421700(const std::int32_t phase)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleScreenEmitter_4216B0;
    action->characterAction = CharacterAnimation_Stand;
    action->flags = flags | 2u;
    action->framePlayingRate = 100;
    action->displayPriority = 3;
    action->phase = phase;
    action->pathCountOrDrawOrder_270 = 0;
    return action;
}

} // namespace detail

} // namespace cg::yobi
