#include "map_observation.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "../platform/MapEffectMath.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldObjectState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace cg::yobi {

namespace detail {

// 原 payload +00..+2F；原生指针扩宽后只供三种地图角色及观察链使用。
struct MapCharaPayload {
    std::array<std::uint32_t, 5> values;
    std::array<YobiAction*, 5> partyActions;
    std::int32_t frame;
    YobiAction* next;
};

static_assert(std::is_standard_layout_v<MapCharaPayload>);

static_assert(std::is_trivially_copyable_v<MapCharaPayload>);

static_assert(offsetof(MapCharaPayload, values) == 0);

static_assert(offsetof(MapCharaPayload, partyActions) == 0x18);

static_assert(offsetof(MapCharaPayload, frame) == 0x40);

static_assert(offsetof(MapCharaPayload, next) == 0x48);

static_assert(sizeof(MapCharaPayload) == 0x50);

static constexpr std::size_t kYobiRegistryRowCount = 399;

static constexpr std::size_t kYobiRegistryColumnCount = 399;

static constexpr std::size_t kYobiRegistryEntryCount =
    kYobiRegistryRowCount * kYobiRegistryColumnCount;

// 原 byte_B65A80/dword_B8C864；按行展开，指针不拥有 action。
static std::array<std::uint8_t, kYobiRegistryEntryCount> gYobiRegistryFlags {}; // 原全局：0x00B65A80。

static std::array<YobiAction*, kYobiRegistryEntryCount> gYobiRegistryActions {}; // 原全局：0x00B8C864。

static_assert(
    std::tuple_size_v<decltype(gYobiRegistryFlags)>
    == std::size_t{399} * std::size_t{399});

static_assert(
    std::tuple_size_v<decltype(gYobiRegistryActions)>
    == std::size_t{399} * std::size_t{399});

static YobiAction* gMapCharaHead = nullptr; // 原 0x5F9750，独立于首尾哨兵。

static std::array<std::int32_t, 5> gMapValues5F9720 {}; // 原全局：0x005F9720。

static std::array<std::int32_t, 5> gMapImages5AC4A0 {}; // 原全局：0x005AC4A0。

static std::int32_t gMapImage5F9734 = 0; // 原全局：0x005F9734。

static std::int32_t gMapDirection5F9740 = 0; // 原全局：0x005F9740。

static std::int32_t gMapDuration5F9744 = 0; // 原全局：0x005F9744。

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

YobiAction* NewMapCharaAction()
{
    return NewYobiAction_404AE0(2, sizeof(MapCharaPayload), YobiPayloadKind::MapChara);
}

// 两种观察节点的真实回调，声明供以下生产入口绑定。
// 原函数：0x0043F520。
static void Sub_43F520(YobiAction* action);

// 原函数：0x00440070。
static void Sub_440070(YobiAction* action);

// 原函数：0x0043F330。
YobiAction* Sub_43F330(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int32_t value170)
{
    YobiAction* const action = NewMapCharaAction();
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->callback = Sub_43F520;
    action->characterAction = 0;
    action->direction = direction;
    action->field58 = direction;
    action->displayPriority = 10;
    action->flags = 0x20u;
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

    action->readiness = static_cast<std::int32_t>(value170);
    action->phase = 1;
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    static_cast<void>(Sub_43F180(action));
    return action;
}

// 原函数：0x0043FEB0。
YobiAction* Sub_43FEB0(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction)
{
    YobiAction* const action = NewMapCharaAction();
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = Sub_440070;
    action->imageNo = imageNo;
    action->characterAction = 3;
    action->displayPriority = 10;
    action->flags = 0x20u;
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
    action->phase = 3;
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
    static_cast<void>(Sub_43F180(action));
    return action;
}

// 原函数：0x004408C0。
void Sub_4408C0(
    YobiAction* const action, const std::int32_t first, const std::int32_t second,
    const std::int32_t third, const std::int32_t fourth, const std::int32_t fifth)
{
    auto* const payload = action->payload.mapChara;
    const std::array<std::int32_t, 5> values {first, second, third, fourth, fifth};
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] > 0) {
            payload->values[i] = static_cast<std::uint32_t>(values[i]);
        }
    }
}

// 原函数：0x0043F290。
static YobiAction* Sub_43F290(YobiAction* const parent, const std::int32_t index)
{
    const std::uint32_t playerIndex = world::gPlayerRuntimeSlots.objectIndex;
    const auto* const payload = static_cast<const MapCharaPayload*>(parent->payload.mapChara);
    const std::uint32_t objectIndex = payload->values[index];
    YobiAction* const source = objectIndex == playerIndex
        ? world::gPlayerRuntimeSlots.action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(3, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = source->imageNo;
    action->characterAction = 0;
    const std::int32_t direction = source->direction;
    action->displayPriority = 10;
    action->direction = direction;
    action->flags = 2;
    action->x = source->x;
    const std::int32_t y = source->y;
    action->field1D0 = index;
    action->observationParent10 = parent;
    action->y = y;
    action->phase = 1;
    return action;
}

// 原函数：0x0043FE00。
static YobiAction* Sub_43FE00(YobiAction* const parent, const std::int32_t index)
{
    const std::uint32_t playerIndex = world::gPlayerRuntimeSlots.objectIndex;
    const auto* const payload = static_cast<const MapCharaPayload*>(parent->payload.mapChara);
    const std::uint32_t objectIndex = payload->values[index];
    YobiAction* const source = objectIndex == playerIndex
        ? world::gPlayerRuntimeSlots.action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source == nullptr) {
        return nullptr;
    }
    YobiAction* const action = NewYobiAction_404AE0(3, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = source->imageNo;
    action->characterAction = 0;
    const std::int32_t direction = source->direction;
    action->displayPriority = 10;
    action->direction = direction;
    action->flags = 2;
    action->x = source->x;
    const std::int32_t y = source->y;
    action->value1D4 = 0;
    action->counter28 = 0;
    action->field1D0 = index;
    action->observationParent10 = parent;
    action->y = y;
    action->phase = 3;
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
        action->characterAction = 3;
    } else if ((imageNo >= 0x186A0 && imageNo <= 0x18A87)
        || (imageNo >= 0x19A28 && imageNo <= 0x1A130)
        || (imageNo >= 0x197D0 && imageNo <= 0x19848)
        || (imageNo >= 0x1D524 && imageNo <= 0x1D553)
        || (imageNo >= 0x1AF0E && imageNo <= 0x1AF25)) {
        action->characterAction = 11;
    } else {
        action->characterAction = 0;
    }
}

[[maybe_unused]] static void SetObservationChildAction(YobiAction* const action)
{
    SetObservationChildAction(action, action->imageNo);
}

// 原函数：0x0043F910。
static void Sub_43F910(YobiAction* const action)
{
    const std::uint32_t playerIndex = world::gPlayerRuntimeSlots.objectIndex;
    YobiAction* const parent = action->observationParent10;
    const std::int32_t initialSlot = action->field1D0;
    auto* const payload = parent->payload.mapChara;
    const std::uint32_t objectIndex = payload->values[initialSlot];
    YobiAction* const source = objectIndex == playerIndex
        ? world::gPlayerRuntimeSlots.action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source != nullptr) {
        action->imageNo = world::Sub_51C0D0(source->imageNo);
        action->direction = parent->direction;
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    } else {
        action->graphicIndex = -1;
    }
    std::int32_t slotX = 0;
    std::int32_t slotY = 0;
    const std::int32_t slotDirection = parent->direction;
    const std::int32_t slot = action->field1D0;
    const std::int32_t parentImageNo = parent->imageNo;
    Sub_43FD90(parentImageNo, slot, slotDirection, &slotX, &slotY);
    const std::int32_t parentY = parent->y;
    const std::int32_t initialPhase = action->phase;
    const std::int32_t parentX = parent->x;
    const std::int32_t frameY = parent->frameY;
    const std::int32_t frameX = parent->frameX;
    std::int32_t relativeX;
    std::int32_t relativeY;
    if (initialPhase == 1) {
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
        action->characterAction = 11;
        if (currentFrame > 15 && targetY <= relativeY) {
            const std::int32_t imageNo = action->imageNo;
            action->phase = 2;
            action->value1D4 = 0;
            SetObservationChildAction(action, imageNo);
            static_cast<void>(audio::PlaySe_52C450(0x1CE, action->x));
        }
    } else if (initialPhase == 2) {
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
    if (payload->values[index] != 0 && payload->partyActions[index] != nullptr) {
        std::int32_t shiftX = 0;
        std::int32_t shiftY = 0;
        if (action->phase == 1) {
            const std::int32_t frame = action->value1D4;
            if (frame < 15) {
                const std::int32_t remaining = 15 - frame;
                shiftX = (action->x - parentX) * remaining / 15;
                shiftY = (action->y - parentY) * remaining / 15;
            }
        }
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        action->deltaX = offsetX + shiftX + relativeX + parentX;
        const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
        action->deltaY = offsetY + shiftY + relativeY + parentY;
    }
    ++action->value1D4;
}

// 原函数：0x00440440。
static void Sub_440440(YobiAction* const action)
{
    YobiAction* const parent = action->observationParent10;
    const std::int32_t initialSlot = action->field1D0;
    auto* const payload = parent->payload.mapChara;
    const std::uint32_t objectIndex = payload->values[initialSlot];
    YobiAction* const source = objectIndex == world::gPlayerRuntimeSlots.objectIndex
        ? world::gPlayerRuntimeSlots.action
        : world::FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
    if (source != nullptr) {
        action->imageNo = world::Sub_51C0D0(source->imageNo);
        action->direction = parent->direction;
        if (action->phase == 4) {
            action->direction = source->direction;
        }
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    } else {
        action->graphicIndex = -1;
    }
    std::int32_t slotX = 0;
    std::int32_t slotY = 0;
    const std::int32_t slotDirection = parent->direction;
    const std::int32_t slot = action->field1D0;
    const std::int32_t parentImageNo = parent->imageNo;
    Sub_43FD90(parentImageNo, slot, slotDirection, &slotX, &slotY);
    std::int32_t parentX = parent->x;
    const std::int32_t initialPhase = action->phase;
    std::int32_t parentY = parent->y;
    const std::int32_t frameX = parent->frameX;
    const std::int32_t frameY = parent->frameY;
    std::int32_t relativeX;
    std::int32_t relativeY;
    if (initialPhase == 4) {
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
        action->characterAction = 11;
        if (frame > 15 && relativeY >= 0) {
            const std::int32_t soundX = action->x;
            action->phase = 5;
            action->characterAction = 0;
            static_cast<void>(audio::PlaySe_52C450(0x1C9, soundX));
            const std::uint32_t currentIndex = payload->values[action->field1D0];
            if (currentIndex == world::gPlayerRuntimeSlots.objectIndex) {
                world::Sub_51BEF0();
            } else {
                world::ResumeWorldActorAfterObservation_43DB30(source);
            }
        }
    } else if (initialPhase == 3) {
        relativeY = frameY + slotY;
        const std::int32_t imageNo = action->imageNo;
        relativeX = frameX + slotX;
        SetObservationChildAction(action, imageNo);
    }
    const std::int32_t currentPhase = action->phase;
    if (currentPhase >= 5) {
        return;
    }
    const std::int32_t index = action->field1D0;
    if (payload->values[index] != 0 && payload->partyActions[index] != nullptr) {
        std::int32_t shiftX = 0;
        std::int32_t shiftY = 0;
        if (source != nullptr && currentPhase == 4) {
            const std::int32_t travel = action->counter28;
            shiftX = (source->x - parentX) * travel / 15;
            shiftY = (source->y - parentY) * travel / 15;
        }
        const std::int32_t drawDirection = parent->direction;
        const std::int32_t drawImageNo = parent->imageNo;
        const std::int32_t drawSlot = action->field1D0;
        Sub_43FD90(drawImageNo, drawSlot, drawDirection, &slotX, &slotY);
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t drawY = GetYobiGraphicOriginOffset_5182B0(action, 1) + shiftY + relativeY + parentY;
        const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
        const std::int32_t primaryIndex = parent->value1D4;
        const std::int32_t drawX = offsetX + shiftX + relativeX + parentX;
        const std::int32_t graphicIndex = action->graphicIndex;
        world::DrawGearNormal_46C060(
            primaryIndex, 1, graphicIndex,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), descriptor, 0);
    }
    if (action->phase == 4) {
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
static void Sub_43F520(YobiAction* const action)
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
    if (initialPhase == 1) {
        static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    } else if (initialPhase == 2) {
        const std::int32_t frame = payload->frame;
        if (frame < 0) {
            action->characterAction = 2;
            if (AdvanceYobiAnimationFrame_517880(action, 100, 1, 0) == 1) {
                payload->frame = 0;
            }
            action->counter1DC = 0;
        } else {
            action->characterAction = 3;
            action->counter1DC = 3;
            y += static_cast<std::int32_t>(static_cast<std::int64_t>(
                static_cast<double>(frame)
                * -1.4F));
            const std::int32_t initialDirection = action->field58;
            const std::int32_t direction = initialDirection - ((frame + 8) % 120) / 15;
            action->direction = direction;
            if (direction < 0) {
                action->direction = direction + 8;
            }
            const std::int32_t angle = angles[initialDirection] + frame * 3;
            std::int64_t offset[2];
            platform::detail::Cg2dMapActorSpiralOffset(frame, angle, offset);
            x += static_cast<std::int32_t>(offset[0]);
            y += static_cast<std::int32_t>(offset[1]);
            static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
            if (Sub_43F1F0(world::gPlayerRuntimeSlots.objectIndex) == nullptr
                && frame > 300 && (x < -300 || x > 940)) {
                Sub_43F1B0(action);
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
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
    const std::int32_t drawX = offsetX + action->x;
    const std::uint32_t worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::uint32_t worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const std::int32_t graphicIndex = action->graphicIndex;
    action->value1D4 = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    if (action->phase == 1) {
        std::int8_t ready = 0;
        for (std::int32_t index = 0; index < 5; ++index) {
            if (payload->values[index] != 0) {
                if (payload->partyActions[index] == nullptr) {
                    payload->partyActions[index] = Sub_43F290(action, index);
                }
                YobiAction* const child = payload->partyActions[index];
                if (child == nullptr || child->phase != 2) {
                    continue;
                }
            }
            ++ready;
        }
        if (payload->frame > 10 && ready >= 5) {
            action->phase = 2;
            payload->frame = -1;
        }
    }
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        YobiAction* const child = payload->partyActions[index];
        if (child != nullptr) {
            Sub_43F910(child);
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
static void Sub_440070(YobiAction* const action)
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
    if (initialPhase >= 3 && initialPhase <= 5) {
        if (initialPhase == 3 && payload->frame > 50) {
            action->phase = 4;
            for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
                YobiAction* const child = payload->partyActions[index];
                if (child != nullptr) {
                    child->phase = 4;
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
    if (action->phase == 4) {
        std::int32_t index = 0;
        for (; index < 5; ++index) {
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr && child->phase != 5) {
                break;
            }
        }
        if (index >= 5) {
            action->phase = 5;
        }
    }
    if (action->phase == 5 && (x < -300 || x > 940 || y < -300 || y > 780)) {
        Sub_43F1B0(action);
        for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr) {
                MarkDeath(child);
            }
        }
        MarkDeath(action);
    }
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    action->x = x;
    action->y = y;
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
    const std::int32_t drawX = offsetX + action->x;
    const std::uint32_t worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::uint32_t worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    action->value1D4 = world::CachePrimaryMapGraphic_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    for (std::int32_t index = 0; index < 5; ++index) {
        if (payload->values[index] != 0 && payload->partyActions[index] == nullptr) {
            payload->partyActions[index] = Sub_43FE00(action, index);
        }
    }
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        YobiAction* const child = payload->partyActions[index];
        if (child != nullptr) {
            Sub_440440(child);
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
YobiAction* Sub_43F180(YobiAction* const action)
{
    YobiAction* current = gMapCharaHead;
    if (current == nullptr) {
        gMapCharaHead = action;
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
void* Sub_43F1B0(YobiAction* const action)
{
    YobiAction* current = gMapCharaHead;
    if (current == action) {
        auto* const payload = action->payload.mapChara;
        gMapCharaHead = payload->next;
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
std::int32_t* Sub_43FD90(
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
static void Sub_440C50(YobiAction* const action)
{
    auto* const payload = action->payload.mapChara;
    const std::int32_t initialFrame = payload->frame;
    const std::int32_t initialDirection = action->direction;
    platform::detail::MapObserverProjection projection;
    platform::detail::Cg2dMapObserverProjection(
        initialFrame, &gMapDuration5F9744, initialDirection, &projection);
    AdvanceYobiAnimationFrame_517880(action, 100, 0, 0);
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
        AdvanceYobiAnimationFrame_517880(payload->partyActions[index], 100, 0, 0);
        YobiAction* const child = payload->partyActions[index];
        const std::int32_t direction = child->direction;
        const std::int32_t slot = child->field1D0;
        Sub_43FD90(action->imageNo, slot, direction, &offsetX, &offsetY);
        const std::int32_t graphicX = GetYobiGraphicOriginOffset_5182B0(payload->partyActions[index], 0);
        const std::int32_t frameX = action->frameX;
        const std::int32_t parentX = action->x;
        const std::int32_t frameOffsetX = graphicX + frameX;
        const std::int32_t slotOffsetX = offsetX;
        drawX[index] = frameOffsetX + parentX + slotOffsetX;
        const std::int32_t graphicY = GetYobiGraphicOriginOffset_5182B0(payload->partyActions[index], 1);
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
    const std::int32_t offsetY = GetYobiGraphicOriginOffset_5182B0(action, 1);
    const std::int32_t y = offsetY + action->y;
    const std::int32_t offsetX = GetYobiGraphicOriginOffset_5182B0(action, 0);
    const std::int32_t x = offsetX + action->x;
    render::DrawGraphic2_521DD0(x, y, 10, graphicIndex, 0, 0, mode);
    ++payload->frame;
}

// 原函数：0x00440B70。
YobiAction* Sub_440B70()
{
    YobiAction* const action = NewMapCharaAction();
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = Sub_440C50;
    action->imageNo = gMapImage5F9734;
    action->characterAction = 3;
    const std::int32_t initialDirection = gMapDirection5F9740;
    auto* const payload = action->payload.mapChara;
    action->direction = initialDirection;
    action->displayPriority = 10;
    action->flags = 0x20;
    action->x = -100;
    action->y = -100;
    static_cast<void>(Sub_43F180(action));
    for (std::size_t index = 0; index < payload->values.size(); ++index) {
        payload->values[index] = 0;
        payload->partyActions[index] = nullptr;
        const std::int32_t value = gMapValues5F9720[index];
        if (value <= 0) {
            continue;
        }

        payload->values[index] = static_cast<std::uint32_t>(value);
        YobiAction* const child = NewYobiAction_404AE0(2, 0);
        payload->partyActions[index] = child;
        if (child != nullptr) {
            child->imageNo = gMapImages5AC4A0[index];
            YobiAction* const directionChild = payload->partyActions[index];
            const std::int32_t direction = gMapDirection5F9740;
            directionChild->direction = direction;
            YobiAction* const actionChild = payload->partyActions[index];
            actionChild->characterAction = 3;
            YobiAction* const indexChild = payload->partyActions[index];
            indexChild->field1D0 = static_cast<std::int32_t>(index);
        }
    }
    return action;
}

} // namespace detail

// 原函数：0x0046DD40。
void Sub_46DD40()
{
    for (std::size_t row = 0; row < detail::kYobiRegistryRowCount; ++row) {
        const std::size_t rowOffset = row * detail::kYobiRegistryColumnCount;
        for (std::size_t column = 0;
             column < detail::kYobiRegistryColumnCount;
             ++column) {
            const std::size_t index = rowOffset + column;
            if (detail::gYobiRegistryFlags[index] == std::uint8_t{1}) {
                detail::gYobiRegistryFlags[index] = std::uint8_t{0};
                YobiAction* const action = detail::gYobiRegistryActions[index];
                MarkDeath(action);
            }
        }
    }
}

std::uint8_t& MapYobiRegistryFlagAt(const std::int32_t index) noexcept
{
    return detail::gYobiRegistryFlags.data()[index];
}

YobiAction*& MapYobiRegistryActionAt(const std::int32_t index) noexcept
{
    return detail::gYobiRegistryActions.data()[index];
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
    const std::int32_t objectIndex, const std::int32_t x, const std::int32_t y,
    [[maybe_unused]] const std::int32_t unusedDirection,
    const std::int32_t characterAction, const char* const row)
{
    if (characterAction < 81 || characterAction > 82) {
        return 0;
    }
    YobiAction* const target =
        static_cast<std::uint32_t>(objectIndex) == world::gPlayerRuntimeSlots.objectIndex
        ? world::gPlayerRuntimeSlots.action
        : world::FindObjectYobi_4462B0(objectIndex);
    if (target == nullptr) {
        return 1;
    }
    std::array<std::int32_t, 7> values;
    for (std::int32_t index = 0; index < 7; ++index) {
        values[index] = network::ParseDelimitedInteger_52CFA0(row, '|', index + 6);
    }
    if (characterAction == 82) {
        const std::int32_t imageNo = character_select::ResolveCharacterImageForVersion(values[0]);
        YobiAction* const observer = detail::Sub_43FEB0(imageNo, x, y, values[1]);
        detail::Sub_4408C0(observer, values[2], values[3], values[4], values[5], values[6]);
        for (std::size_t index = 2; index < values.size(); ++index) {
            const std::int32_t member = values[index];
            if (static_cast<std::uint32_t>(member) == world::gPlayerRuntimeSlots.objectIndex) {
                world::Sub_51BEE0();
            } else {
                world::SuspendWorldActorForObservation_43DB00(world::FindObjectYobi_4462B0(member));
            }
        }
        return 1;
    }
    YobiAction* const source = world::FindObjectYobi_4462B0(values[0]);
    if (source != nullptr) {
        const std::int32_t value170 = values[1];
        const std::int32_t direction = source->direction;
        const std::int32_t sourceY =
            static_cast<std::int32_t>(source->mapY1F0);
        const std::int32_t sourceX =
            static_cast<std::int32_t>(source->mapX1E0);
        YobiAction* const observer = detail::Sub_43F330(
            source->imageNo, sourceX, sourceY, direction, value170);
        detail::Sub_4408C0(observer, values[2], values[3], values[4], values[5], values[6]);
        for (std::size_t index = 2; index < values.size(); ++index) {
            const std::int32_t member = values[index];
            if (static_cast<std::uint32_t>(member) == world::gPlayerRuntimeSlots.objectIndex) {
                ui::WindowRuntime_CloseByCategory(3);
                world::gValue581800 = -1;
                world::Sub_51BEE0();
                ui::WindowRuntime_ApplyObservationInputGate();
            } else {
                world::SuspendWorldActorForObservation_43DB00(world::FindObjectYobi_4462B0(member));
            }
        }
        source->imageNo = 0;
    }
    return 1;
}

// 原函数：0x0046DE50。
YobiAction* Sub_46DE50(
    const std::int32_t imageNo, const std::int32_t mapX, const std::int32_t mapY)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->direction = 0;
    action->displayPriority = 10;
    action->flags = 0x30;
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
void Sub_46DD90(YobiAction* const action)
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
    static_cast<void>(AdvanceYobiAnimationFrame_517880(action, 100, 0, 0));
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
YobiAction* Sub_43F1F0(const std::uint32_t value)
{
    YobiAction* current = detail::gMapCharaHead;
    while (current != nullptr) {
        const auto* const payload =
            static_cast<const detail::MapCharaPayload*>(current->payload.mapChara);
        YobiAction* const next = payload->next;
        for (const std::uint32_t entry : payload->values) {
            if (entry == value) {
                return current;
            }
        }
        current = next;
    }
    return nullptr;
}

// 原函数：0x0043F220。
void Sub_43F220()
{
    detail::gMapCharaHead = nullptr;
}

// 原函数：0x0043F230。
void* Sub_43F230()
{
    YobiAction* current = detail::gMapCharaHead;
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
    detail::gMapCharaHead = nullptr;
    // 空链时原 EAX 未定义；新端返回空值，不作为原端等价结果验收。
    return last;
}

// 原函数：0x00440F50。
void* Sub_440F50()
{
    YobiAction* const action =
        Sub_43F1F0(world::gPlayerRuntimeSlots.objectIndex);
    if (action == nullptr) {
        return nullptr;
    }

    const std::int32_t imageNo = action->imageNo;
    auto* const payload = action->payload.mapChara;
    detail::gMapImage5F9734 = imageNo;
    detail::gMapDirection5F9740 = action->field58;
    detail::gMapDuration5F9744 =
        (action->readiness * 60);
    for (std::size_t index = 0; index < payload->values.size(); ++index) {
        detail::gMapValues5F9720[index] = 0;
        detail::gMapImages5AC4A0[index] = 0;
        const std::int32_t value =
            std::bit_cast<std::int32_t>(payload->values[index]);
        if (value > 0) {
            detail::gMapValues5F9720[index] = value;
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr) {
                detail::gMapImages5AC4A0[index] = child->imageNo;
            }
        }
    }
    return payload->partyActions.data();
}

std::int32_t ReadMapObservationDirection() noexcept
{
    return detail::gMapDirection5F9740;
}

// 原函数：0x00440FE0。
std::int32_t Sub_440FE0()
{
    static_cast<void>(detail::Sub_440B70());
    static_cast<void>(cg::audio::BeginBgmFadeOut_52B990());
    return 0x25;
}

} // namespace cg::yobi
