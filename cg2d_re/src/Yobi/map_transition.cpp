#include "map_transition.hpp"
#include "base.hpp"
#include "map_render.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/world/SceneState.hpp"

#include "map_actor.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldObjectState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::yobi {

namespace detail {

// 原448040的+18/+1C：模式0/1是附件指针，其他模式是S32状态。
union MapTransitionSlots18 {
    struct {
        YobiAction* action18; // 原成员偏移：+0x18。
        YobiAction* action1C; // 原成员偏移：+0x1C。
    } attachments;
    struct {
        std::int32_t state18; // 原成员偏移：+0x18。
        std::int32_t state1C; // 原成员偏移：+0x1C。
    } values;
};

struct MapTransitionPayload {
    std::int16_t mode;
    std::int16_t value02; // 原成员偏移：+0x02。
    std::int16_t value04; // 原成员偏移：+0x04。
    std::uint16_t value06; // 原成员偏移：+0x06。
    std::int32_t mapX08; // 原成员偏移：+0x08。
    std::int32_t mapY0C; // 原成员偏移：+0x0C。
    std::int32_t counter10; // 原成员偏移：+0x10。
    std::uint64_t tick14; // 原成员偏移：+0x14。
    MapTransitionSlots18 slots18; // 原成员偏移：+0x18。
    float value20; // 原成员偏移：+0x20。
    std::int32_t value24; // 原成员偏移：+0x24。
    std::int32_t objectIndex28; // 原成员偏移：+0x28。
    std::uint32_t visible2C; // 原成员偏移：+0x2C。
    std::uint32_t frame30; // 原成员偏移：+0x30。
    std::int32_t step34; // 原成员偏移：+0x34。
    std::int32_t countdown38; // 原成员偏移：+0x38。
    std::int32_t height3C; // 原成员偏移：+0x3C。
    std::int32_t velocity40; // 原成员偏移：+0x40。
    std::array<std::int32_t, 10> trailGraphic44; // 原成员偏移：+0x44。
    std::array<std::int32_t, 10> trailX6C; // 原成员偏移：+0x6C。
    std::array<std::int32_t, 10> trailY94; // 原成员偏移：+0x94。
    std::array<std::uint32_t, 10> trailWorldXBitsBC; // 原成员偏移：+0xBC。
    std::array<std::uint32_t, 10> trailWorldYBitsE4; // 原成员偏移：+0xE4。
    std::uint32_t flags10C; // 原成员偏移：+0x10C。
};

static_assert(std::is_standard_layout_v<MapTransitionPayload>);

static_assert(std::is_trivially_copyable_v<MapTransitionPayload>);

static_assert(sizeof(MapTransitionSlots18) == 0x10);

static_assert(offsetof(MapTransitionPayload, value02) == 0x02);

static_assert(offsetof(MapTransitionPayload, value04) == 0x04);

static_assert(offsetof(MapTransitionPayload, value06) == 0x06);

static_assert(offsetof(MapTransitionPayload, mapX08) == 0x08);

static_assert(offsetof(MapTransitionPayload, mapY0C) == 0x0C);

static_assert(offsetof(MapTransitionPayload, counter10) == 0x10);

// CTM01：载荷按sizeof分配，64位时间槽之后采用本地布局。
static_assert(sizeof(MapTransitionPayload::tick14) == 8);

// 原446970；唯一caller弃EAX，只在偶数时写回S16。
static void Sub_446970(std::int16_t* const value)
{
    const std::int16_t current = *value;
    if (current % 2 == 0) {
        *value = static_cast<std::int16_t>(current + 1);
    }
}

// 原446990：负值局部归一化后写一次，正值超界时逐次写回S16槽。
static void Sub_446990(std::int16_t* const value)
{
    std::int16_t current = *value;
    if (current < 0) {
        do {
            current = static_cast<std::int16_t>(current + 8);
        } while (current < 0);
        *value = current;
        return;
    }
    while (*value > 7) {
        *value = static_cast<std::int16_t>(*value - 8);
    }
}

// 原函数：0x00447E60。
static void Sub_447E60(YobiAction* action);

// 原函数：0x004473B0。
static std::int32_t Sub_4473B0(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->visible2C = 1;
        const std::uint32_t frame = render::gFrameParity628FC8;
        payload->step34 = 20;
        payload->countdown38 = 10;
        payload->frame30 = frame ^ 1u;
    }
    const std::int32_t step = payload->step34;
    if (step <= -20) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (render::gFrameParity628FC8 != payload->frame30) {
        --payload->countdown38;
    }
    if (payload->countdown38 <= 0) {
        payload->visible2C ^= 1u;
        payload->countdown38 = step / 2;
        if (payload->countdown38 < 1) {
            payload->countdown38 = 1;
        }
        payload->step34 = step - 1;
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
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    if (payload->visible2C != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
        const std::int32_t drawY = offsetY + action->y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        const std::int32_t drawX = offsetX + action->x;
        static_cast<void>(render::DrawGraphic2_521DD0(
            drawX, drawY, 11, graphicIndex, 0, 0, descriptor));
    }
    payload->frame30 = render::gFrameParity628FC8;
    return 0;
}

// 原函数：0x004474E0。
static std::int32_t Sub_4474E0(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->visible2C = 1;
        const std::uint32_t frame = render::gFrameParity628FC8;
        payload->step34 = -10;
        payload->countdown38 = 1;
        payload->frame30 = frame ^ 1u;
    }
    const std::int32_t step = payload->step34;
    if (step > 30) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (render::gFrameParity628FC8 != payload->frame30) {
        --payload->countdown38;
    }
    if (payload->countdown38 <= 0) {
        payload->visible2C ^= 1u;
        payload->countdown38 = step / 2;
        if (payload->countdown38 < 1) {
            payload->countdown38 = 1;
        }
        payload->step34 = step + 1;
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
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    if (payload->visible2C != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
        const std::int32_t drawY = offsetY + action->y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        const std::int32_t drawX = offsetX + action->x;
        static_cast<void>(render::DrawGraphic2_521DD0(
            drawX, drawY, 11, graphicIndex, 0, 0, descriptor));
    }
    payload->frame30 = render::gFrameParity628FC8;
    return 0;
}

// 原4469C0：按原递增换向顺序选一格，避开上次格及469810判定的阻挡格。
static void Sub_4469C0(YobiAction* const action)
{
    if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) > 0) {
        return;
    }
    if ((world::gPlayerX != world::gValue9EE8D0)
        || (world::gPlayerY != world::gValue9EE8E0)) {
        return;
    }
    auto* const payload = action->payload.mapTransition;
    std::int32_t attempt = 0;
    std::int16_t direction = payload->value02;
    std::int32_t candidateX;
    std::int32_t candidateY;
    do {
        const std::int16_t deltaX = world::ReadDirectionDeltaX(direction);
        const std::int16_t deltaY = world::ReadDirectionDeltaY(direction);
        candidateX = (action->mapX1E0 + static_cast<std::int32_t>(deltaX));
        candidateY = (action->mapY1F0 + static_cast<std::int32_t>(deltaY));
        if ((candidateX != payload->mapX08 || candidateY != payload->mapY0C)
            && world::Sub_469810(candidateX, candidateY) == 0) {
            break;
        }
        direction = static_cast<std::int16_t>(direction + attempt * 2 + 2);
        Sub_446990(&direction);
        ++attempt;
    } while (attempt < 4);
    if (attempt >= 4) {
        payload->mapX08 = -1;
        payload->mapY0C = -1;
        payload->value02 = static_cast<std::int16_t>(payload->value02 - 2);
        Sub_446990(&payload->value02);
        return;
    }
    if (std::bit_cast<std::int16_t>(payload->value06) >= 2) {
        payload->value06 = 0;
        payload->value02 = static_cast<std::int16_t>(payload->value02 - 6);
        Sub_446990(&payload->value02);
    }
    if (direction != payload->value04) {
        ++payload->value06;
    } else {
        payload->value06 = 0;
    }
    payload->value04 = direction;
    action->pathX220[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = candidateX;
    action->pathY248[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = candidateY;
    ++action->pathCountOrDrawOrder_270;
    payload->mapX08 = static_cast<std::int32_t>(action->mapX1E0);
    payload->mapY0C = static_cast<std::int32_t>(action->mapY1F0);
}

// 原446B50：按载荷方向追加一个目标格，仅447610调用且不消费返回值。
static void Sub_446B50(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) > 0) {
        return;
    }
    if ((world::gPlayerX != world::gValue9EE8D0)
        || (world::gPlayerY != world::gValue9EE8E0)) {
        return;
    }
    const std::int16_t deltaX = world::ReadDirectionDeltaX(payload->value02);
    const std::int32_t x =
        (action->mapX1E0 + static_cast<std::int32_t>(deltaX));
    action->pathX220[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = x;
    const std::int16_t deltaY = world::ReadDirectionDeltaY(payload->value02);
    const std::int32_t y =
        (action->mapY1F0 + static_cast<std::int32_t>(deltaY));
    action->pathY248[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = y;
    ++action->pathCountOrDrawOrder_270;
    payload->mapX08 = static_cast<std::int32_t>(action->mapX1E0);
    payload->mapY0C = static_cast<std::int32_t>(action->mapY1F0);
}

// 两次差值都保留读计数/RNG副作用；第二次结果按首次符号取负，不可换成abs。
static std::int32_t TransitionDistance(
    platform::LegacyCryptInt* const lhs, platform::LegacyCryptInt* const rhs)
{
    const std::int32_t sign = ((*lhs) - (*rhs));
    const std::int32_t value = ((*lhs) - (*rhs));
    return sign < 0 ? -value : value;
}

// 原446C10：玩家快照稳定且距离超过一格时，按各轴符号追加目标格。
static void Sub_446C10(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    std::int32_t stepX = 0;
    std::int32_t stepY = 0;
    if (std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) > 0) {
        return;
    }
    if ((world::gPlayerX != world::gValue9EE8D0)
        || (world::gPlayerY != world::gValue9EE8E0)) {
        return;
    }
    if (TransitionDistance(&world::gPlayerX, &action->mapX1E0) <= 1
        && TransitionDistance(&world::gPlayerY, &action->mapY1F0) <= 1) {
        return;
    }
    if ((world::gPlayerX - action->mapX1E0) > 0) {
        stepX = 1;
    } else if ((world::gPlayerX - action->mapX1E0) < 0) {
        stepX = -1;
    }
    if ((world::gPlayerY - action->mapY1F0) > 0) {
        stepY = 1;
    } else if ((world::gPlayerY - action->mapY1F0) < 0) {
        stepY = -1;
    }
    const std::int32_t x =
        (action->mapX1E0 + static_cast<std::int32_t>(stepX));
    action->pathX220[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = x;
    const std::int32_t y =
        (action->mapY1F0 + static_cast<std::int32_t>(stepY));
    action->pathY248[std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270)] = y;
    ++action->pathCountOrDrawOrder_270;
    payload->mapX08 = static_cast<std::int32_t>(action->mapX1E0);
    payload->mapY0C = static_cast<std::int32_t>(action->mapY1F0);
}

// 各调用点独立采样，按原FCOMP的C3判定且保持X到Y短路读取。
bool TransitionMovementIdle(const YobiAction* const action)
{
    const float movementX = std::bit_cast<float>(action->movementStepXBits_27C);
    if (movementX < 0.0F || movementX > 0.0F) {
        return false;
    }
    const float movementY = std::bit_cast<float>(action->movementStepYBits_280);
    return !(movementY < 0.0F || movementY > 0.0F);
}

// 仅复用447610/447820的移动后绘制尾；stepChange分别为-1/+1。
static void DrawMovingTransition(
    YobiAction* const action, MapTransitionPayload* const payload,
    const std::int32_t stepChange)
{
    if (render::gFrameParity628FC8 != payload->frame30) {
        --payload->countdown38;
    }
    if (payload->countdown38 <= 0) {
        const std::uint32_t visible = payload->visible2C;
        const std::int32_t step = payload->step34;
        payload->visible2C = visible ^ 1u;
        payload->countdown38 = step / 2;
        if (payload->countdown38 < 1) {
            payload->countdown38 = 1;
        }
        payload->step34 = step + stepChange;
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
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    if (payload->visible2C != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
        const std::int32_t drawY = offsetY + action->y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        const std::int32_t drawX = offsetX + action->x;
        static_cast<void>(render::DrawGraphic2_521DD0(
            drawX, drawY, 11, graphicIndex, 0, 0, descriptor));
    }
    payload->frame30 = render::gFrameParity628FC8;
}

// 原模式2/3共享三点绘制；X/Y舍入点不同，三轮均推进动画并归一化局部角度。
static void DrawTransitionOrbit(
    YobiAction* const action, MapTransitionPayload* const payload,
    float angle, const bool allowMapAtZero)
{
    for (std::int32_t point = 0; point < 3; ++point) {
        const double xFactor = world::Sub_46F950(angle);
        const float offsetX = static_cast<float>(
            xFactor * payload->value24);
        const double yFactor = world::Sub_46F930(angle);
        const double offsetY = yFactor * payload->value24;
        const float projectedY = static_cast<float>(
            offsetY + action->worldY);
        const float projectedX = static_cast<float>(
            static_cast<double>(offsetX) + action->worldX);
        float screenX;
        float screenY;
        world::ProjectWorldToScreen_46CE30(
            projectedX, projectedY, &screenX, &screenY);
        action->x = static_cast<std::int32_t>(
            static_cast<std::int64_t>(screenX + 0.5));
        action->y = static_cast<std::int32_t>(
            static_cast<std::int64_t>(screenY + 0.5));
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
        if (allowMapAtZero && payload->value24 == 0) {
            const render::GraphicDescriptor16* const descriptor = &action->drawMode;
            const std::int32_t yOffset = GetYobiGraphicOriginOffset_5182B0(action, 1);
            const std::int32_t drawY = yOffset + action->y;
            const std::int32_t xOffset = GetYobiGraphicOriginOffset_5182B0(action, 0);
            const std::int32_t x = action->x;
            const std::int32_t graphicIndex = action->graphicIndex;
            const std::int32_t drawX = xOffset + x;
            const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
            const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
            static_cast<void>(world::CachePrimaryMapGraphic_46BD60(
                graphicIndex, worldXBits, worldYBits,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                0, descriptor));
        } else if (render::gFrameParity628FC8 == 1) {
            const std::int32_t graphicIndex = action->graphicIndex;
            const render::GraphicDescriptor16* const descriptor = &action->drawMode;
            const std::int32_t yOffset = GetYobiGraphicOriginOffset_5182B0(action, 1);
            const std::int32_t drawY = yOffset + action->y;
            const std::int32_t xOffset = GetYobiGraphicOriginOffset_5182B0(action, 0);
            const std::int32_t drawX = xOffset + action->x;
            static_cast<void>(render::DrawGraphic2_521DD0(
                drawX, drawY, 11, graphicIndex, 0, 0, descriptor));
        }
        angle = static_cast<float>(angle + 120.0);
        world::Sub_46F8F0(&angle);
    }
}

// 原447A20/447C40仅两处Y偏移轴参数不同；保留相同的五列交错搬移和发布次序。
static std::int32_t UpdateTransitionTrail(
    YobiAction* const action, const std::uint8_t yOffsetAxis)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->counter10 = 0;
        action->movementStepXBits_27C = 0;
        action->movementStepYBits_280 = 0;
        payload->height3C = 0;
        payload->velocity40 = -15;
        payload->trailGraphic44.fill(-1);
    }
    const std::int32_t counter = payload->counter10;
    if (counter >= 40) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (counter >= 30) {
        payload->height3C = 0;
    } else {
        const double deltaX = std::bit_cast<float>(action->movementStepXBits_27C);
        action->worldX = static_cast<float>(deltaX + action->worldX);
        const double deltaY = std::bit_cast<float>(action->movementStepYBits_280);
        action->worldY = static_cast<float>(deltaY + action->worldY);
        const std::int32_t velocity = payload->velocity40;
        const std::int32_t height = payload->height3C;
        payload->height3C = height + velocity;
        payload->velocity40 = velocity + 1;
    }
    ++payload->counter10;
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    if ((action->flags & 2u) == 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, yOffsetAxis);
        const std::int32_t height = payload->height3C;
        const std::int32_t y = action->y;
        const std::int32_t drawY = offsetY + height + y;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        const std::int32_t drawX = offsetX + action->x;
        static_cast<void>(render::DrawGraphic2_521DD0(
            drawX, drawY, 11, graphicIndex, 0, 0, descriptor));
    }
    if (render::gFrameParity628FC8 == 1) {
        for (std::size_t index = 0; index < 10; ++index) {
            const std::int32_t graphicIndex = payload->trailGraphic44[index];
            if (graphicIndex >= 0) {
                const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, yOffsetAxis);
                const std::int32_t drawY = offsetY + payload->trailY94[index];
                const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
                const std::int32_t drawX = offsetX + payload->trailX6C[index];
                static_cast<void>(render::DrawGraphic2_521DD0(
                    drawX, drawY, 11, graphicIndex, 0, 0, nullptr));
            }
        }
    }
    for (std::size_t index = 9; index != 0; --index) {
        payload->trailGraphic44[index] = payload->trailGraphic44[index - 1];
        payload->trailX6C[index] = payload->trailX6C[index - 1];
        payload->trailY94[index] = payload->trailY94[index - 1];
        payload->trailWorldXBitsBC[index] = payload->trailWorldXBitsBC[index - 1];
        payload->trailWorldYBitsE4[index] = payload->trailWorldYBitsE4[index - 1];
    }
    if (payload->counter10 <= 30) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const std::int32_t height = payload->height3C;
        payload->trailGraphic44[0] = graphicIndex;
        payload->trailX6C[0] = action->x;
        payload->trailY94[0] = height + action->y;
        payload->trailWorldXBitsBC[0] = std::bit_cast<std::uint32_t>(action->worldX);
        payload->trailWorldYBitsE4[0] = std::bit_cast<std::uint32_t>(action->worldY);
    } else {
        payload->trailGraphic44[0] = -1;
    }
    return 0;
}

// 原函数：0x00446D80。
static std::int32_t Sub_446D80(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (TransitionDistance(&action->mapX1E0, &world::gPlayerX) >= 13
        || TransitionDistance(&action->mapY1F0, &world::gPlayerY) >= 13) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    if (payload->tick14 + 8000u < tick && TransitionMovementIdle(action)) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if ((payload->flags10C & 1u) == 0
        && payload->slots18.attachments.action18 == nullptr) {
        YobiAction** const slot = &payload->slots18.attachments.action18;
        const std::uint64_t firstTick = platform::ReadTickCount64();
        if (payload->tick14 + 7200u < firstTick) {
            const std::uint8_t displayPriority = action->displayPriority;
            const std::int32_t y =
                static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t x =
                static_cast<std::int32_t>(action->mapX1E0);
            *slot = CreateOneShotMapAnimation_444260(0x18902, x, y, 0, 0, displayPriority, slot);
            payload->flags10C |= 1u;
        }
    }
    if ((payload->flags10C & 2u) == 0
        && payload->slots18.attachments.action1C == nullptr) {
        YobiAction** const slot = &payload->slots18.attachments.action1C;
        const std::uint64_t secondTick = platform::ReadTickCount64();
        if (payload->tick14 + 7500u < secondTick) {
            const std::uint8_t displayPriority = action->displayPriority;
            const std::int32_t y =
                static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t x =
                static_cast<std::int32_t>(action->mapX1E0);
            *slot = CreateOneShotMapAnimation_444260(0x18902, x, y, 0, 0, displayPriority, slot);
            payload->flags10C |= 2u;
        }
    }
    if (TransitionMovementIdle(action)) {
        Sub_4469C0(action);
    }
    UpdateMapActorPathMovement_4424B0(action);
    YobiAction* const first = payload->slots18.attachments.action18;
    if (first != nullptr) {
        first->worldX = action->worldX;
        YobiAction* const firstY = payload->slots18.attachments.action18;
        firstY->worldY = action->worldY;
    }
    YobiAction* const second = payload->slots18.attachments.action1C;
    if (second != nullptr) {
        second->worldX = action->worldX;
        YobiAction* const secondY = payload->slots18.attachments.action1C;
        secondY->worldY = action->worldY;
    }
    return 1;
}

// 原函数：0x00446FA0。
static std::int32_t Sub_446FA0(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.attachments.action18 == nullptr) {
        const std::uint64_t firstTick = platform::ReadTickCount64();
        if (payload->tick14 < firstTick) {
            const std::uint8_t displayPriority = action->displayPriority;
            const std::int32_t y =
                static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t x =
                static_cast<std::int32_t>(action->mapX1E0);
            payload->slots18.attachments.action18 =
                CreateOneShotMapAnimation_444260(0x18902, x, y, 0, 0, displayPriority, nullptr);
        }
    }
    if (payload->slots18.attachments.action1C == nullptr) {
        const std::uint64_t secondTick = platform::ReadTickCount64();
        if (payload->tick14 + 500u < secondTick) {
            const std::uint8_t displayPriority = action->displayPriority;
            const std::int32_t y =
                static_cast<std::int32_t>(action->mapY1F0);
            const std::int32_t x =
                static_cast<std::int32_t>(action->mapX1E0);
            payload->slots18.attachments.action1C =
                CreateOneShotMapAnimation_444260(0x18902, x, y, 0, 0, displayPriority, nullptr);
        }
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    if (payload->tick14 + 1200u < tick) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
    }
    return 0;
}

// 原函数：0x00447070。
static std::int32_t Sub_447070(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->value20 = 0.0F;
        payload->value24 = 0;
    }
    if (payload->value24 > 800) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (payload->slots18.values.state18 != 0) {
        const std::uint64_t now = platform::ReadTickCount64();
        const std::uint64_t deadline = payload->tick14 + 1000u;
        if (deadline < now) {
            const double angle = payload->value20 + 6.0;
            payload->value24 += 4;
            payload->value20 = static_cast<float>(angle);
        }
    }
    DrawTransitionOrbit(action, payload, payload->value20, true);
    return 0;
}

// 原函数：0x00447230。
static std::int32_t Sub_447230(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->slots18.values.state1C = 0;
        payload->value20 = 0.0F;
        payload->value24 = 800;
    } else if (payload->value24 <= 0) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 1;
    }
    if (payload->slots18.values.state18 != 0) {
        const double angle = payload->value20 + 6.0;
        payload->value24 -= 4;
        payload->value20 = static_cast<float>(angle);
        DrawTransitionOrbit(action, payload, static_cast<float>(angle), false);
    }
    return 0;
}

// 原函数：0x00447610。
static std::int32_t Sub_447610(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->visible2C = 1;
        const std::uint32_t frame = render::gFrameParity628FC8;
        payload->step34 = 20;
        payload->countdown38 = 10;
        payload->frame30 = frame ^ 1u;
    }
    if (TransitionDistance(&action->mapX1E0, &world::gPlayerX) >= 13
        || TransitionDistance(&action->mapY1F0, &world::gPlayerY) >= 13) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    if (payload->tick14 + 6000u < tick && TransitionMovementIdle(action)) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (TransitionMovementIdle(action)) {
        Sub_446B50(action);
    }
    UpdateMapActorPathMovement_4424B0(action);
    DrawMovingTransition(action, payload, -1);
    return 0;
}

// 原函数：0x00447820。
static std::int32_t Sub_447820(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    if (payload->slots18.values.state18 == 0) {
        payload->slots18.values.state18 = 1;
        payload->visible2C = 1;
        const std::uint32_t frame = render::gFrameParity628FC8;
        payload->step34 = -10;
        payload->countdown38 = 1;
        payload->frame30 = frame ^ 1u;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    if (payload->tick14 + 6000u < tick && TransitionMovementIdle(action)) {
        world::RecreateWorldObjectAction_4467D0(payload->objectIndex28);
        MarkDeath(action);
        return 0;
    }
    if (TransitionMovementIdle(action)
        && (TransitionDistance(&action->mapX1E0, &world::gPlayerX) > 1
            || TransitionDistance(&action->mapY1F0, &world::gPlayerY) > 1)) {
        Sub_446C10(action);
    }
    UpdateMapActorPathMovement_4424B0(action);
    DrawMovingTransition(action, payload, 1);
    return 0;
}

// 原函数：0x00447A20。
static std::int32_t Sub_447A20(YobiAction* const action)
{
    return UpdateTransitionTrail(action, 1);
}

// 原函数：0x00447C40。
static std::int32_t Sub_447C40(YobiAction* const action)
{
    return UpdateTransitionTrail(action, 0);
}

// 原函数：0x00447E60。
static void Sub_447E60(YobiAction* const action)
{
    auto* const payload = action->payload.mapTransition;
    std::int32_t result = 1;
    switch (payload->mode) {
    case 0:
        result = Sub_446D80(action);
        break;
    case 1:
        result = Sub_446FA0(action);
        break;
    case 2:
        result = Sub_447070(action);
        break;
    case 3:
        result = Sub_447230(action);
        break;
    case 4:
        result = Sub_4473B0(action);
        break;
    case 5:
        result = Sub_4474E0(action);
        break;
    case 6:
        result = Sub_447610(action);
        break;
    case 7:
        result = Sub_447820(action);
        break;
    case 8:
        result = Sub_447A20(action);
        break;
    case 9:
        result = Sub_447C40(action);
        break;
    default:
        break;
    }
    if (result == 0) {
        return;
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
    const std::int32_t characterAction = action->characterAction;
    const std::int32_t stopAtEnd =
        characterAction == 12 || characterAction == 13 || characterAction == 14
        || characterAction == 15 || characterAction == 1 || characterAction == 0
        || characterAction == 16 ? 0 : 1;
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, stopAtEnd, 0));
    if ((action->flags & 2u) != 0) {
        return;
    }
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
    const std::int32_t x = action->x;
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + x;
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    static_cast<void>(world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 0, descriptor));
}

} // namespace detail

// LDT01：四次构造复用同一临时槽，count/encoded/mask全部取齐后再同序发布。
// 原函数：0x00448040。
YobiAction* Sub_448040(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int16_t mode, const std::int16_t value02,
    const std::int32_t objectIndex)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(detail::MapTransitionPayload), YobiPayloadKind::MapTransition);
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = detail::Sub_447E60;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x200034u;
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
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    const auto y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    auto* const payload = action->payload.mapTransition;
    action->y = y;
    payload->mode = mode;
    payload->value02 = value02;
    detail::Sub_446970(&payload->value02);
    payload->value04 = payload->value02;
    payload->value06 = 0;
    payload->mapX08 = mapX;
    payload->mapY0C = mapY;
    payload->counter10 = 0;
    payload->tick14 = platform::ReadTickCount64();
    if (mode == 0 || mode == 1) {
        payload->slots18.attachments.action18 = nullptr;
        payload->slots18.attachments.action1C = nullptr;
    } else {
        payload->slots18.values.state18 = 0;
        payload->slots18.values.state1C = 0;
    }
    payload->value20 = 0.0F;
    payload->value24 = 0;
    payload->flags10C = 0;
    payload->objectIndex28 = objectIndex;
    return action;
}

} // namespace cg::yobi
