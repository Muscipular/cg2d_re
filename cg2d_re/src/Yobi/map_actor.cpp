#include "map_actor.hpp"
#include "base.hpp"
#include "map_transition.hpp"
#include "map_render.hpp"
#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../world/WorldObjectState.hpp"
#include "../platform/MapEffectMath.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>

namespace cg::login {

namespace detail {

// 原441DB0/448040回调及实体绘制尾；完整定义位于本文件后段。
// 原函数：0x004417E0。
void Sub_4417E0(YobiAction* action);

// 原函数：0x00441DB0。
YobiAction* Sub_441DB0(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(MapActorPayload), YobiPayloadKind::MapActor);
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = Sub_4417E0;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x34;

    // 本入口的 count/encoded/mask 写序与通用 同类operator= 不同。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX200, mapX);
    assignCoordinate(action->mapY210, mapY);
    action->field270 = 0;
    assignCoordinate(action->mapX1E0, mapX);
    assignCoordinate(action->mapY1F0, mapY);

    const double scaledX = mapX * 64.0;
    action->field27C = 0;
    action->field280 = 0;
    action->phase = 0;
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
    action->flags |= 0x80u;
}

void SetYobiWalkSpeed(YobiAction* const action, const std::int32_t speed) noexcept
{
    action->walkSpeed = speed;
}

YobiAction* ReadMapActorAttachment44(const YobiAction* const action) noexcept
{
    return static_cast<const MapActorPayload*>(action->payload.mapActor)->action44;
}

void BeginMapActorPhase2(YobiAction* const action)
{
    auto* const payload = action->payload.mapActor;
    action->phase = 2;
    payload->counter48 = 0;
}

std::uint16_t ReadYobiDirectionLow16(const YobiAction* const action) noexcept
{
    return static_cast<std::uint16_t>(action->direction);
}

YobiAction* PublishPlayerActionIdentity(
    const char* const name, const char* const titles,
    const std::int32_t originalNameLength, const std::int32_t level,
    const std::int32_t value188)
{
    YobiAction* action = world::detail::gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return nullptr;
    }
    if (originalNameLength <= 16) {
        std::memmove(action->nameC4.data(), name, std::strlen(name) + 1);
    }

    // 原对象阶段重新解析四段；不能使用上一轮已写回的共享称号。
    char first[256];
    char second[256];
    char third[256];
    char fourth[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, first);
    input::UnescapeLegacyTextAndNotify_52D2F0(first);
    if (std::strlen(first) <= 16) {
        action = world::detail::gPlayerRuntimeSlots.action;
        std::strcpy(action->titleE1.data(), first);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, second);
    input::UnescapeLegacyTextAndNotify_52D2F0(second);
    if (std::strlen(second) <= 32) {
        action = world::detail::gPlayerRuntimeSlots.action;
        std::strcpy(action->text102.data(), second);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, third);
    input::UnescapeLegacyTextAndNotify_52D2F0(third);
    if (std::strlen(third) <= 32) {
        action = world::detail::gPlayerRuntimeSlots.action;
        std::strcpy(action->text123.data(), third);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, fourth);
    input::UnescapeLegacyTextAndNotify_52D2F0(fourth);
    if (std::strlen(fourth) <= 16) {
        action = world::detail::gPlayerRuntimeSlots.action;
        std::strcpy(action->text31C.data(), fourth);
    }
    action = world::detail::gPlayerRuntimeSlots.action;
    action->field1D0 = level;
    action = world::detail::gPlayerRuntimeSlots.action;
    action->field1D8 = value188;
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
    const char* const name, const char* const title, const std::int32_t* const level,
    const platform::LegacyCryptInt* const hp, const platform::LegacyCryptInt* const maxHp)
{
    YobiAction* action = world::detail::gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return;
    }
    if (std::strlen(name) <= 16) {
        std::strcpy(action->nameC4.data(), name);
        action = world::detail::gPlayerRuntimeSlots.action;
    }
    if (std::strlen(title) <= 16) {
        std::strcpy(action->titleE1.data(), title);
        action = world::detail::gPlayerRuntimeSlots.action;
    }
    action->field1D0 = *level;
    action = world::detail::gPlayerRuntimeSlots.action;
    action->readiness = *hp;
    action = world::detail::gPlayerRuntimeSlots.action;
    action->maxHp = *maxHp;
}

void UpdateMapActorMode(
    YobiAction*& slot, const std::int32_t mode, const std::int32_t imageNo)
{
    YobiAction* current = slot;
    if (current->imageNo2E8 != imageNo) {
        current->refresh2EC = 1;
        current = slot;
    }
    const bool explicitMode = mode == 1 || mode == 2;
    current->field2E4 = explicitMode ? mode : 0;
    slot->imageNo2E8 = explicitMode ? imageNo : 0;
}

void ApplyMapActorReplyAttachments(YobiAction* const action, const char* const text)
{
    auto* const payload = action->payload.mapActor;
    char buffer[2048];
    network::ExtractDelimitedField_52CED0(text, '|', 17, 2047, buffer);
    if (std::strlen(buffer) != 0) {
        Sub_43DD00(action, text, 17);
    } else if (payload->action2C != nullptr) {
        MarkDeath(payload->action2C);
        payload->action2C = nullptr;
    }
}

void ReadMapActorTarget(
    YobiAction* const action, YobiAction*& slot,
    std::int32_t* const x, std::int32_t* const y)
{
    *x = action->mapX200.value();
    *y = slot->mapY210.value();
}

void ReadMapActorTarget(
    YobiAction*& slot, std::int32_t* const x, std::int32_t* const y)
{
    YobiAction* const action = slot;
    ReadMapActorTarget(action, slot, x, y);
}

void ClearMapActorPathCount(YobiAction* const action) noexcept
{
    action->field270 = 0;
}

bool IsMapActorIdleAtTarget(YobiAction* const action)
{
    if (std::bit_cast<std::int16_t>(action->field270) > 0) {
        return false;
    }
    const double x = action->mapX200.value() * 64.0;
    const float worldX = action->worldX;
    if (x < worldX || x > worldX) {
        return false;
    }
    const double y = action->mapY210.value() * 64.0;
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
        world::detail::gPlayerRuntimeSlots.action->payload.mapActor;
    if (payload == nullptr) {
        return;
    }
    *playerImageNo = character_select::detail::Sub_51B740(imageNo);
    const std::int32_t mappedImage = character_select::detail::Sub_51B740(imageNo);
    world::detail::gPlayerRuntimeSlots.action->imageNo = mappedImage;
    if (gearImage != 0) {
        const std::int32_t mappedGear = character_select::detail::Sub_51B740(gearImage);
        bool hasGear = payload->action30 != nullptr;
        if (!hasGear) {
            YobiAction* const gear = NewYobiGear_4410F0(mappedGear);
            hasGear = gear != nullptr;
            payload->action30 = gear;
        }
        if (hasGear) {
            const std::int32_t converted =
                character_select::detail::Sub_51B740(mappedGear);
            payload->action30->imageNo = converted;
        }
    } else {
        YobiAction* const gear = payload->action30;
        if (gear != nullptr) {
            MarkDeath(gear);
            payload->action30 = nullptr;
        }
    }

    if (petImage != 0) {
        std::int32_t mappedPet = character_select::detail::Sub_51B740(petImage);
        bool hasPet = payload->action34 != nullptr;
        if (!hasPet) {
            const std::int32_t y = world::detail::gPlayerRuntimeSlots.action->mapY1F0.value();
            const std::int32_t x = world::detail::gPlayerRuntimeSlots.action->mapX1E0.value();
            YobiAction* const pet = NewYobiGearPet_441130(mappedPet, x, y);
            hasPet = pet != nullptr;
            payload->action34 = pet;
        }
        if (hasPet) {
            const std::int32_t converted =
                character_select::detail::Sub_51B740(mappedPet);
            payload->action34->imageNo = converted;
        }
        if (kind == 6) {
            YobiAction* const previousSecondPet = payload->action38;
            ++mappedPet;
            bool hasSecondPet = previousSecondPet != nullptr;
            if (!hasSecondPet) {
                const std::int32_t y = world::detail::gPlayerRuntimeSlots.action->mapY1F0.value();
                const std::int32_t x = world::detail::gPlayerRuntimeSlots.action->mapX1E0.value();
                YobiAction* const pet = NewYobiGearPet_441130(mappedPet, x, y);
                hasSecondPet = pet != nullptr;
                payload->action38 = pet;
            }
            if (hasSecondPet) {
                const std::int32_t converted =
                    character_select::detail::Sub_51B740(mappedPet);
                payload->action38->imageNo = converted;
            }
        }
    } else {
        YobiAction* const firstPet = payload->action34;
        if (firstPet != nullptr) {
            MarkDeath(firstPet);
            payload->action34 = nullptr;
        }
        YobiAction* const secondPet = payload->action38;
        if (secondPet != nullptr) {
            MarkDeath(secondPet);
            payload->action38 = nullptr;
        }
    }
    payload->kind3C = kind;
}

void ApplyPlayerRideReply(
    const std::int32_t imageNo, const std::int32_t steps,
    std::int32_t* const remainingSteps)
{
    YobiAction* const action = world::detail::gPlayerRuntimeSlots.action;
    if (imageNo >= 0) {
        auto* const payload = action->payload.mapActor;
        if (payload->action44 == nullptr) {
            YobiAction* const ride = NewYobiAction_404AE0(2, 0);
            payload->action44 = ride;
            ride->callback = nullptr;
            payload->action44->characterAction = 0;
            payload->action44->displayPriority = 10;
            payload->action44->flags = 0x34u;
            payload->action44->flags |= 0x200u;
        }
        const std::int32_t mappedRide = character_select::detail::Sub_51B740(imageNo);
        payload->action44->imageNo = mappedRide;
        const std::int32_t mappedPlayer =
            world::detail::Sub_51C0D0(world::detail::gPlayerRuntimeSlots.imageNo);
        world::detail::gPlayerRuntimeSlots.action->imageNo = mappedPlayer;
        world::detail::gPlayerRuntimeSlots.action->phase = 1;
        payload->counter48 = 0;
        *remainingSteps = steps;
        payload->action44->characterAction = 0;
        if (world::detail::gPlayerRuntimeSlots.action->characterAction == 1) {
            payload->action44->characterAction = 1;
        }
        return;
    }
    if (action->phase == 2) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    action->phase = 2;
    YobiAction* const ride = payload->action44;
    payload->counter48 = 0;
    if (ride != nullptr) {
        ride->characterAction = 0;
        if (world::detail::gPlayerRuntimeSlots.action->characterAction == 1) {
            payload->action44->characterAction = 1;
        }
    }
    *remainingSteps = 0;
}

void ApplyPlayerPhase3Reply(const std::int32_t imageNo)
{
    YobiAction* const action = world::detail::gPlayerRuntimeSlots.action;
    auto* const payload = action->payload.mapActor;
    action->phase = 3;
    const std::int32_t mappedImage = character_select::detail::Sub_51B740(imageNo);
    world::detail::gPlayerRuntimeSlots.action->imageNo = mappedImage;
    payload->counter48 = 0;
}

void ResetMapFollowerFromPath(
    YobiAction* const leader, const std::int32_t position, YobiAction*& slot,
    const bool isPlayer, std::int32_t* const playerDirection)
{
    if (isPlayer) {
        YobiAction* const player = world::detail::gPlayerRuntimeSlots.action;
        if (player != nullptr) {
            player->field270 = 0;
        }
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->field270) - position;
        const std::int32_t y = leader->pathY248[index];
        const std::int32_t x = leader->pathX220[index];
        static_cast<void>(world::detail::Sub_519EC0(x, y));
    } else {
        slot->field270 = 0;
        YobiAction* const follower = slot;
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->field270) - position;
        const std::int32_t y = leader->pathY248[index];
        const std::int32_t x = leader->pathX220[index];
        Sub_442AB0(follower, x, y);
    }
    if (!isPlayer || world::detail::gPlayerRuntimeSlots.action != nullptr) {
        const std::int32_t index =
            std::bit_cast<std::int16_t>(leader->field270) - position;
        const std::int32_t dy = leader->pathY248[index - 1] - leader->pathY248[index];
        const std::int32_t dx = leader->pathX220[index - 1] - leader->pathX220[index];
        const float y = static_cast<float>(dy);
        const float x = static_cast<float>(dx);
        const std::int32_t direction =
            static_cast<std::int32_t>(world::detail::Sub_46CDF0(x, y));
        slot->direction = (direction + 4) % 8;
        if (isPlayer) {
            *playerDirection = slot->direction;
        }
    }
    if (isPlayer) {
        world::detail::Sub_519EE0();
        static_cast<void>(world::detail::Sub_4681B0());
    }
}

// 原函数：0x00442AB0。
void Sub_442AB0(
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
    action->field27C = 0;
    action->field280 = 0;
    action->worldX = static_cast<float>(worldX * 64.0F);
    action->worldY = static_cast<float>(targetY * 64.0);
}

// 原函数：0x00441F60。
void Sub_441F60(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    const std::uint16_t count = action->field270;
    if (count >= 10u) {
        action->field270 = 0;
        Sub_442AB0(action, targetX, targetY);
        return;
    }
    const auto xSlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(count));
    action->pathX220[xSlot] = targetX;
    const auto ySlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(action->field270));
    action->pathY248[ySlot] = targetY;
    action->field270 = static_cast<std::uint16_t>(action->field270 + 1u);
}

// 原函数：0x00441FC0。
void Sub_441FC0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    std::int32_t startX;
    std::int32_t startY;
    const std::int16_t count = std::bit_cast<std::int16_t>(action->field270);
    if (count > 0) {
        const auto last = static_cast<std::size_t>(count - 1);
        startX = action->pathX220[last];
        startY = action->pathY248[last];
    } else {
        startX = action->mapX200.value();
        startY = action->mapY210.value();
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
        std::bit_cast<std::int16_t>(action->field270);
    pendingX[static_cast<std::size_t>(pointCount)] = targetX;
    pendingY[static_cast<std::size_t>(pointCount)] = targetY;
    ++pointCount;
    if (static_cast<std::uint32_t>(currentCount + pointCount) > 10u) {
        const auto last = static_cast<std::size_t>(pointCount - 1);
        const std::int32_t finalY = pendingY[last];
        const std::int32_t finalX = pendingX[last];
        action->field270 = 0;
        Sub_442AB0(action, finalX, finalY);
        return;
    }
    for (std::int32_t point = 0; point < pointCount; ++point) {
        const auto xSlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->field270));
        const std::int32_t x = pendingX[static_cast<std::size_t>(point)];
        action->pathX220[xSlot] = x;
        const std::int32_t y = pendingY[static_cast<std::size_t>(point)];
        const auto ySlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->field270));
        action->pathY248[ySlot] = y;
        action->field270 = static_cast<std::uint16_t>(action->field270 + 1u);
    }
}

// 原函数：0x00442110。
double Sub_442110(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY, const std::int32_t speed,
    const float overrideRate)
{
    if (action == nullptr) {
        return 0.0;
    }
    std::int16_t pathCount = 0;
    if (!(overrideRate < 0.0F || overrideRate > 0.0F)) {
        pathCount = std::bit_cast<std::int16_t>(action->field270);
    }
    const platform::detail::MapActorVelocityInput input {
        targetX, targetY, speed, pathCount, 0, overrideRate,
        action->worldX, action->worldY,
    };
    platform::detail::MapActorVelocity velocity;
    platform::detail::Cg2dMapActorVelocity(&input, &velocity);
    action->field27C = std::bit_cast<std::uint32_t>(velocity.stepX);
    action->field280 = std::bit_cast<std::uint32_t>(velocity.stepY);
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
            world::detail::Sub_46CDF0(velocity.directionX, velocity.directionY));
        action->field272 = 1;
    }
    return velocity.rate;
}

// 原函数：0x004422F0。
void Sub_4422F0(
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
    const float percent = std::bit_cast<float>(0x3C23D70Au);
    action->field27C = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.x * 4.0F * percent));
    action->field280 = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.y * 4.0F * percent));

    action->mapX200 = static_cast<std::int32_t>(targetX);
    action->mapY210 = static_cast<std::int32_t>(targetY);
    if (direction.x < 0.0F || direction.x > 0.0F
        || direction.y < 0.0F || direction.y > 0.0F) {
        action->direction = static_cast<std::int32_t>(
            world::detail::Sub_46CDF0(direction.x, direction.y));
        action->field272 = 1;
    }
}

// 原函数：0x00442460。
void Sub_442460(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const std::int16_t count = std::bit_cast<std::int16_t>(action->field270);
    if (count > 0) {
        action->field270 = static_cast<std::uint16_t>(count - 1);
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int16_t>(action->field270); ++index) {
        const auto slot = static_cast<std::size_t>(index);
        action->pathX220[slot] = action->pathX220[slot + 1];
        action->pathY248[slot] = action->pathY248[slot + 1];
    }
}

namespace {

// 原43E535/43EA41等到格转向分支；每个CryptInt比较只读取一次。
void ApplyPendingMapActorDirection(YobiAction* const action, const std::int32_t index)
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
void RestoreMountedCharacterAction(YobiAction* const action)
{
    const std::int32_t imageNo = action->imageNo;
    if ((imageNo >= 0x1BD50 && imageNo <= 0x1BDF9)
        || (imageNo >= 0x1B580 && imageNo <= 0x1B587)
        || (imageNo >= 0x1B58A && imageNo <= 0x1B59C)
        || (imageNo >= 0x1D554 && imageNo <= 0x1D55B)) {
        action->characterAction = 0;
    } else if ((imageNo >= 0x186A0 && imageNo <= 0x18A87)
        || (imageNo >= 0x19A28 && imageNo <= 0x1A130)
        || (imageNo >= 0x197D0 && imageNo <= 0x19848)
        || (imageNo >= 0x1D524 && imageNo <= 0x1D553)
        || (imageNo >= 0x1D55C && imageNo <= 0x1D563)
        || (imageNo >= 0x1AF0E && imageNo <= 0x1AF25)) {
        action->characterAction = 11;
    } else {
        action->characterAction = 0;
    }
}

// 原43ECCF/43ED51：先采玩家指针，非当前玩家时才重读payload索引。
void RestoreActorOriginalImage(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const player = world::detail::gPlayerRuntimeSlots.action;
    if (player != nullptr && player == action) {
        action->imageNo = world::detail::gPlayerRuntimeSlots.imageNo;
        return;
    }
    const std::int32_t index = payload->index;
    action->imageNo = index == -1 ? world::detail::gPlayerRuntimeSlots.imageNo
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
void DrawActorPhaseAttachment(
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
    const std::int32_t offsetY = Sub_5182B0(child, 1);
    const std::int32_t parentY = action->y;
    YobiAction* nextChild;
    if (readBeforeY) {
        nextChild = *slot;
    }
    const std::int32_t drawY = offsetY + parentY;
    if (!readBeforeY) {
        nextChild = *slot;
    }
    const std::int32_t offsetX = Sub_5182B0(nextChild, 0);
    const std::int32_t parentX = action->x;
    YobiAction* graphicChild;
    if (readBeforeX) {
        graphicChild = *slot;
    }
    const std::int32_t drawX = offsetX + parentX;
    if (!readBeforeX) {
        graphicChild = *slot;
    }
    const std::int32_t graphicIndex = graphicChild->graphicIndex;
    world::detail::DrawGearNormal_46C060(
        primaryIndex, before, graphicIndex,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), descriptor, 0);
}

// 原两个阶段的action08创建/播放；分配后的首写原本不判空，不增加失败门禁。
void DrawActorPhaseEffect(
    YobiAction* const action, MapActorPayload* const payload,
    const std::int32_t primaryIndex, const std::int32_t imageNo,
    const ActorAttachmentReadOrder readOrder)
{
    if (payload->action08 == nullptr) {
        YobiAction* const created = NewYobiAction_404AE0(2, 0);
        payload->action08 = created;
        created->callback = nullptr;
        payload->action08->imageNo = imageNo;
        payload->action08->characterAction = 0;
        payload->action08->displayPriority = 10;
        payload->action08->flags = 0x34u;
    }
    YobiAction* const child = payload->action08;
    if (child != nullptr) {
        static_cast<void>(Sub_517880(child, 100, 0, 0));
        DrawActorPhaseAttachment(action, &payload->action08, primaryIndex, 1, readOrder);
    }
}

// 原43E591–43EA01：阶段1的上升、骑乘动作恢复及效果附件清理。
void UpdateActorMountPhase(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const ride = payload->action44;
    if (ride == nullptr) {
        action->phase = 0;
        RestoreMountedCharacterAction(action);
        return;
    }
    ride->direction = action->direction;
    YobiAction* const currentRide = payload->action44;
    const std::int32_t counter = payload->counter48;
    const std::int32_t horizontal = currentRide->frameX * counter / 30;
    const std::int32_t arc = counter - 15;
    std::int32_t vertical = arc * arc - 225;
    if (counter > 15) {
        const std::int32_t frameY = payload->action44->frameY;
        if (frameY <= vertical) {
            vertical = frameY;
        }
    }
    action->characterAction = 11;
    static_cast<void>(Sub_517880(action, 100, 0, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = Sub_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y + vertical;
    const std::int32_t offsetX = Sub_5182B0(action, 0);
    const std::int32_t x = action->x;
    const double sortY = static_cast<double>(action->worldY)
        + std::bit_cast<float>(std::uint32_t{0x3A83126F});
    const std::int32_t drawX = offsetX + x + horizontal;
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const std::int32_t graphicIndex = action->graphicIndex;
    const auto worldYBits = std::bit_cast<std::uint32_t>(static_cast<float>(sortY));
    const std::int32_t primaryIndex = world::detail::Sub_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    static_cast<void>(Sub_517880(payload->action44, 100, 0, 0));
    DrawActorPhaseAttachment(action, &payload->action44, primaryIndex, 0,
        ActorAttachmentReadOrder::AfterYAndXAddition);
    DrawActorPhaseEffect(action, payload, primaryIndex, 0x1AE10,
        ActorAttachmentReadOrder::BeforeYAndXAddition);
    if (payload->counter48 == 0) {
        static_cast<void>(audio::PlaySe_52C450(0x1D1, action->x));
    }
    const std::int32_t nextCounter = payload->counter48 + 1;
    payload->counter48 = nextCounter;
    if (nextCounter > 15 && payload->action44->frameY <= vertical) {
        action->phase = 0;
        RestoreMountedCharacterAction(action);
        static_cast<void>(audio::PlaySe_52C450(0x1CE, action->x));
        YobiAction* const effect = payload->action08;
        if (effect != nullptr) {
            MarkDeath(effect);
            payload->action08 = nullptr;
        }
    }
}

// 原43EAA2–43EDA2：阶段2的下降、骑乘附件退出及原图号恢复。
void UpdateActorDismountPhase(YobiAction* const action, MapActorPayload* const payload)
{
    YobiAction* const ride = payload->action44;
    if (ride == nullptr) {
        action->phase = 0;
        action->characterAction = 0;
        MarkDeath(payload->action44);
        payload->action44 = nullptr;
        RestoreActorOriginalImage(action, payload);
        return;
    }
    ride->direction = action->direction;
    YobiAction* const currentRide = payload->action44;
    const std::int32_t initialCounter = payload->counter48;
    const std::int32_t frameX = currentRide->frameX;
    const std::int32_t horizontal = frameX - frameX * initialCounter / 30;
    if (initialCounter == 0) {
        for (;;) {
            const std::int32_t counter = payload->counter48;
            const std::int32_t arc = counter - 15;
            const std::int32_t candidate = arc * arc - 225;
            if (payload->action44->frameY >= candidate) {
                break;
            }
            const std::int32_t nextCounter = counter + 1;
            payload->counter48 = nextCounter;
            if (nextCounter >= 15) {
                break;
            }
        }
    }
    const std::int32_t counter = payload->counter48;
    const std::int32_t arc = counter - 15;
    std::int32_t vertical = arc * arc - 225;
    if (counter > 15 && vertical >= 0) {
        vertical = 0;
    }
    action->characterAction = 11;
    static_cast<void>(Sub_517880(action, 100, 0, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = Sub_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y + vertical;
    const std::int32_t offsetX = Sub_5182B0(action, 0);
    const std::int32_t x = action->x;
    const double sortY = static_cast<double>(action->worldY)
        + std::bit_cast<float>(std::uint32_t{0x3A83126F});
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + x + horizontal;
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    const auto worldYBits = std::bit_cast<std::uint32_t>(static_cast<float>(sortY));
    const std::int32_t primaryIndex = world::detail::Sub_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
    static_cast<void>(Sub_517880(payload->action44, 100, 0, 0));
    DrawActorPhaseAttachment(action, &payload->action44, primaryIndex, 0,
        ActorAttachmentReadOrder::BeforeYAddition);
    DrawActorPhaseEffect(action, payload, primaryIndex, 0x1AE11,
        ActorAttachmentReadOrder::BeforeYAddition);
    if (payload->counter48 == 0) {
        static_cast<void>(audio::PlaySe_52C450(0x1CF, action->x));
    }
    const std::int32_t nextCounter = payload->counter48 + 1;
    payload->counter48 = nextCounter;
    if (nextCounter > 15 && vertical >= 0) {
        action->phase = 0;
        action->characterAction = 0;
        MarkDeath(payload->action44);
        payload->action44 = nullptr;
        RestoreActorOriginalImage(action, payload);
        static_cast<void>(audio::PlaySe_52C450(0x1C9, action->x));
        YobiAction* const effect = payload->action08;
        if (effect != nullptr) {
            MarkDeath(effect);
            payload->action08 = nullptr;
        }
    }
}

// 原43EDA2–43EF98：阶段3局部螺旋坐标与两次动画调用，不改世界坐标。
void UpdateActorSpiralPhase(YobiAction* const action, MapActorPayload* const payload)
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
    const std::int32_t counter = payload->counter48;
    if (counter <= 360) {
        action->characterAction = 3;
        const std::int32_t progress = 360 - counter;
        screenY += static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(progress)
            * std::bit_cast<float>(std::uint32_t{0xBFB33333})));
        if (payload->counter48 == 0) {
            action->field58 = action->direction;
        }
        const std::int32_t savedDirection = action->field58;
        action->direction = savedDirection - ((progress + 8) % 120) / 15;
        if (action->direction < 0) {
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
        action->characterAction = 4;
        if (Sub_517880(action, 100, 1, 0) == 1) {
            action->phase = 0;
            payload->counter48 = 0;
            action->characterAction = 0;
        }
    }
    static_cast<void>(Sub_517880(action, 100, 0, 0));
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = Sub_5182B0(action, 1);
    const std::int32_t drawY = offsetY + screenY;
    const std::int32_t offsetX = Sub_5182B0(action, 0);
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + screenX;
    const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    static_cast<void>(world::detail::Sub_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor));
    ++payload->counter48;
}

} // namespace

// 原函数：0x0043DD00。
std::int32_t Sub_43DD00(
    YobiAction* const action, const char* const text, std::int32_t field)
{
    MapActorPayload* const payload =
        action != nullptr ? action->payload.mapActor : nullptr;
    char buffer[2048];
    network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
    ++field;
    const auto flags = static_cast<std::uint8_t>(
        character_select::detail::Sub_51B740(std::atoi(buffer)));
    if (payload != nullptr) {
        if (payload->action30 != nullptr) {
            MarkDeath(payload->action30);
            payload->action30 = nullptr;
        }
        if (payload->action34 != nullptr) {
            MarkDeath(payload->action34);
            payload->action34 = nullptr;
        }
        if (payload->action38 != nullptr) {
            MarkDeath(payload->action38);
            payload->action38 = nullptr;
        }
    }

    if ((flags & 9u) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        std::int32_t index = 0;
        while (std::strcmp(buffer, "EF") != 0) {
            const std::int32_t kind =
                character_select::detail::Sub_51B740(std::atoi(buffer));
            if (kind == 0) {
                network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
                ++field;
                const std::int32_t imageNo =
                    character_select::detail::Sub_51B740(std::atoi(buffer));
                if (payload != nullptr) {
                    payload->action30 = NewYobiGear_4410F0(imageNo);
                }
            } else if ((flags & 0x10u) != 0) {
                network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
                ++field;
                const std::int32_t imageNo =
                    character_select::detail::Sub_51B740(std::atoi(buffer));
                if (payload != nullptr) {
                    if (index == 0) {
                        const std::int32_t y =
                            action->mapY1F0.value();
                        const std::int32_t x =
                            action->mapX1E0.value();
                        payload->action34 = NewYobiGearPet_441130(imageNo, x, y);
                        payload->kind3C = kind;
                    } else if (index == 1) {
                        if (kind >= 3 && kind <= 5
                            && payload->kind3C >= 3 && payload->kind3C <= 5) {
                            MarkDeath(payload->action34);
                            payload->action34 = nullptr;
                        }
                        const std::int32_t y =
                            action->mapY1F0.value();
                        const std::int32_t x =
                            action->mapX1E0.value();
                        payload->action38 = NewYobiGearPet_441130(imageNo, x, y);
                        payload->kind40 = kind;
                    }
                }
            }
            network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
            ++field;
            ++index;
        }
    }

    if ((flags & 2u) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t imageNo =
            character_select::detail::Sub_51B740(std::atoi(buffer));
        if (payload != nullptr) {
            const auto mapMainImage = [&]() {
                YobiAction* const player = world::detail::gPlayerRuntimeSlots.action;
                const std::int32_t original =
                    (player != nullptr && player == action) || payload->index == -1
                    ? world::detail::gPlayerRuntimeSlots.imageNo
                    : std::bit_cast<std::int32_t>(
                        world::detail::gWorldObjects[payload->index].imageNo);
                action->imageNo = world::detail::Sub_51C0D0(original);
            };
            if (payload->action44 != nullptr) {
                payload->action44->imageNo = imageNo;
                mapMainImage();
            } else {
                YobiAction* const ride = NewYobiAction_404AE0(2, 0);
                payload->action44 = ride;
                if (ride != nullptr) {
                    ride->callback = nullptr;
                    payload->action44->imageNo = imageNo;
                    payload->action44->characterAction = 0;
                    payload->action44->displayPriority = 10;
                    payload->action44->flags = 0x34u;
                    payload->action44->flags |= 0x200u;
                    mapMainImage();
                    RestoreMountedCharacterAction(action);
                }
            }
        }
    } else if (payload != nullptr && payload->action44 != nullptr) {
        action->phase = 2;
        payload->counter48 = 0;
    }

    if ((flags & 4u) != 0) {
        network::ExtractDelimitedField_52CED0(text, '|', field, 2047, buffer);
        ++field;
        const std::int32_t imageNo =
            character_select::detail::Sub_51B740(std::atoi(buffer));
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
        const std::int32_t value1D8 = std::atoi(buffer);
        char name[256];
        network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        ++field;
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (action == nullptr) {
            return field;
        }
        std::int32_t x = action->mapX1E0.value();
        std::int32_t y = action->mapY1F0.value();
        if (payload == nullptr) {
            return field;
        }
        if (payload->action2C != nullptr) {
            MarkDeath(payload->action2C);
            payload->action2C = nullptr;
        }
        if (offsetDirection != -1) {
            const std::int16_t dx = world::detail::ReadDirectionDeltaX(
                static_cast<std::int16_t>(offsetDirection));
            const std::int16_t dy = world::detail::ReadDirectionDeltaY(
                static_cast<std::int16_t>(offsetDirection));
            x += dx;
            y += dy;
        }
        YobiAction* const child = Sub_441DB0(imageNo, x, y, direction);
        payload->action2C = child;
        if (child == nullptr) {
            return field;
        }
        auto* const childPayload = child->payload.mapActor;
        childPayload->index =
            world::detail::gPlayerRuntimeSlots.action == action ? -1 : payload->index;
        payload->action2C->flags |= 0x80100u;
        std::strcpy(payload->action2C->nameC4.data(), name);
        payload->action2C->field1D0 = level;
        payload->action2C->field1D8 = value1D8;
    } else if (payload != nullptr && payload->action2C != nullptr) {
        MarkDeath(payload->action2C);
        payload->action2C = nullptr;
    }
    return field;
}

// 原函数：0x0043E490。
std::int32_t Sub_43E490(YobiAction* const action)
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
    case 0:
        return 0;
    case 1:
        Sub_4424B0(action);
        ApplyPendingMapActorDirection(action, index);
        UpdateActorMountPhase(action, payload);
        return 1;
    case 2:
        Sub_4424B0(action);
        ApplyPendingMapActorDirection(action, index);
        UpdateActorDismountPhase(action, payload);
        return 1;
    case 3:
        UpdateActorSpiralPhase(action, payload);
        return 1;
    default:
        action->phase = 0;
        return 0;
    }
}

// 原函数：0x004417E0。
void Sub_4417E0(YobiAction* const action)
{
    auto* payload = action->payload.mapActor;
    std::int32_t index = payload->index;
    std::int32_t primaryIndex = -1;
    bool useSharedRate = true;
    if (world::detail::Sub_440B50() != 0) {
        return;
    }
    YobiAction* player = world::detail::gPlayerRuntimeSlots.action;
    if ((player != nullptr && player == action) || index == -1) {
        if ((world::detail::ReadPlayerActionFlagsByte() & 0x80u) != 0) {
            return;
        }
    } else if ((reinterpret_cast<const unsigned char*>(
                    &world::detail::gWorldObjects[
                        static_cast<std::size_t>(index)].flags120)[0] & 0x80u) != 0) {
        return;
    }
    if (Sub_43E490(action) != 0) {
        return;
    }
    if (world::detail::gPlayerRuntimeSlots.action != action) {
        payload = action->payload.mapActor;
        index = payload->index;
        if ((reinterpret_cast<const unsigned char*>(
                    &world::detail::gWorldObjects[
                        static_cast<std::size_t>(index)].flags42)[1] & 2u) == 0) {
            Sub_4424B0(action);
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
    player = world::detail::gPlayerRuntimeSlots.action;
    std::int32_t stopAtEnd;
    if (player != action
        && (world::detail::gWorldObjects[
                static_cast<std::size_t>(index)].category & 0x100u) != 0) {
        const std::int32_t characterAction = action->characterAction;
        stopAtEnd = characterAction == 3 || characterAction == 0
            || characterAction == 1 || characterAction == 18 || characterAction == 19
            || characterAction == 13 || characterAction == 15 || characterAction == 11 ? 0 : 1;
    } else {
        YobiAction* const ride = payload->action44;
        const std::int32_t characterAction =
            ride != nullptr ? ride->characterAction : action->characterAction;
        stopAtEnd = characterAction == 12 || characterAction == 13
            || characterAction == 14 || characterAction == 15 || characterAction == 1
            || characterAction == 0 || characterAction == 16 || characterAction == 3
            || characterAction == 6 ? 0 : 1;
    }
    std::uint32_t flipStatic = 0;
    if (world::detail::gValue9DAD68 == 1) {
        payload = action->payload.mapActor;
        index = payload->index;
        if (player != action) {
            const std::uint32_t flags = world::detail::gWorldObjects[
                static_cast<std::size_t>(index)].flags114;
            if ((flags & 0x1000u) != 0 && (flags & 0x400380u) == 0) {
                flipStatic = 1;
            }
        }
    }
    static_cast<void>(Sub_517880(action, 100, stopAtEnd, flipStatic));
    if (useSharedRate) {
        if (std::bit_cast<float>(world::detail::gValue9DAD78) >= 1.2F) {
            static_cast<void>(Sub_517880(action, 100, stopAtEnd, flipStatic));
        }
        if (std::bit_cast<float>(world::detail::gValue9DAD78) >= 1.6F) {
            static_cast<void>(Sub_517880(action, 100, stopAtEnd, flipStatic));
        }
    } else {
        if (std::bit_cast<std::int16_t>(action->field270) >= 2) {
            static_cast<void>(Sub_517880(action, 100, stopAtEnd, flipStatic));
        }
        if (std::bit_cast<std::int16_t>(action->field270) >= 4) {
            static_cast<void>(Sub_517880(action, 100, stopAtEnd, flipStatic));
        }
    }
    const std::int32_t imageNo = action->imageNo;
    if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
        action->drawMode.mixFlags |= cg::render::GraphicMixFlag_OriginalPalette;
    } else {
        action->drawMode.mixFlags &= ~cg::render::GraphicMixFlag_OriginalPalette;
    }
    YobiAction* const ride = payload->action44;
    if (ride != nullptr) {
        ride->direction = action->direction;
        static_cast<void>(Sub_517880(payload->action44, 100, stopAtEnd, 0));
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = Sub_5182B0(action, 1);
        YobiAction* const rideY = payload->action44;
        const std::int32_t y = action->y;
        const std::int32_t drawY = offsetY + rideY->frameY + y;
        const std::int32_t offsetX = Sub_5182B0(action, 0);
        YobiAction* const rideX = payload->action44;
        const std::int32_t x = action->x;
        const std::int32_t frameX = rideX->frameX;
        const std::int32_t graphicIndex = action->graphicIndex;
        const std::int32_t drawX = offsetX + frameX + x;
        const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
        const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
        primaryIndex = world::detail::Sub_46BD60(
            graphicIndex, worldXBits, worldYBits,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 1, descriptor);
        DrawActorPhaseAttachment(action, &payload->action44, primaryIndex, 0,
            ActorAttachmentReadOrder::BeforeXAddition);
    } else if ((action->flags & 0x400u) != 0) {
        if (world::detail::gPlayerRuntimeSlots.action != action) {
            const std::int32_t lowerX =
                (world::detail::gPlayerX - 16);
            bool outside = (action->mapX1E0 < static_cast<std::int32_t>(lowerX));
            if (!outside) {
                const std::int32_t upperX =
                    (world::detail::gPlayerX + 16);
                outside = (upperX < action->mapX1E0);
            }
            if (!outside) {
                const std::int32_t lowerY =
                    (world::detail::gPlayerY - 16);
                outside = (action->mapY1F0 < static_cast<std::int32_t>(lowerY));
            }
            if (!outside) {
                const std::int32_t upperY =
                    (world::detail::gPlayerY + 16);
                outside = (upperY < action->mapY1F0);
            }
            if (outside) {
                world::detail::Sub_4462E0(world::detail::gWorldObjects[
                    static_cast<std::size_t>(index)].objectIndex);
                return;
            }
        }
        const std::int32_t mapY = action->mapY1F0.value();
        const std::int32_t mapX = action->mapX1E0.value();
        const std::int32_t firstGraphicIndex = action->graphicIndex;
        if (world::detail::Sub_446900(firstGraphicIndex, mapX, mapY) == 0) {
            const render::GraphicDescriptor16* const descriptor = &action->drawMode;
            const std::int32_t offsetY = Sub_5182B0(action, 1);
            const std::int32_t drawY = offsetY + action->y;
            const std::int32_t offsetX = Sub_5182B0(action, 0);
            const std::int32_t x = action->x;
            const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
            const std::int32_t drawX = offsetX + x;
            const std::int32_t graphicIndex = action->graphicIndex;
            const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
            primaryIndex = world::detail::Sub_46BD60(
                graphicIndex, worldXBits, worldYBits,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 0, descriptor);
            action->flags &= ~2u;
        } else {
            action->flags |= 2u;
        }
    } else {
        const std::int32_t mode = world::detail::gPlayerRuntimeSlots.action == action ? 1 : 0;
        const render::GraphicDescriptor16* const descriptor = &action->drawMode;
        const std::int32_t offsetY = Sub_5182B0(action, 1);
        const std::int32_t drawY = offsetY + action->y;
        const std::int32_t offsetX = Sub_5182B0(action, 0);
        const std::int32_t x = action->x;
        const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
        const std::int32_t drawX = offsetX + x;
        const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
        const std::int32_t graphicIndex = action->graphicIndex;
        primaryIndex = world::detail::Sub_46BD60(
            graphicIndex, worldXBits, worldYBits,
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), mode, descriptor);
    }
    Sub_442F70(action, primaryIndex);
}

// 原函数：0x0043EFC0。
void Sub_43EFC0(
    YobiAction* const action, const std::int32_t characterAction,
    const std::int32_t restart)
{
    auto* const payload = action->payload.mapActor;
    YobiAction* const ride = payload->action44;
    if (ride != nullptr) {
        ride->characterAction = characterAction;
        const std::int32_t imageNo = action->imageNo;
        if ((imageNo >= 114000 && imageNo <= 114169)
            || (imageNo >= 112000 && imageNo <= 112007)
            || (imageNo >= 112010 && imageNo <= 112028)
            || (imageNo >= 120148 && imageNo <= 120155)) {
            switch (payload->action44->characterAction) {
            case 0: action->characterAction = 0; break;
            case 1: action->characterAction = 1; break;
            case 2:
            case 3:
            case 4: action->characterAction = 3; break;
            case 5:
            case 6: action->characterAction = 5; break;
            case 8:
            case 10: action->characterAction = 8; break;
            case 9: action->characterAction = 9; break;
            default:
                action->characterAction = 0;
                payload->action44->characterAction = 0;
                break;
            }
        } else if ((imageNo >= 100000 && imageNo <= 100999)
            || (imageNo >= 105000 && imageNo <= 106800)
            || (imageNo >= 104400 && imageNo <= 104520)
            || (imageNo >= 120100 && imageNo <= 120147)
            || imageNo == 100900 || imageNo == 100901) {
            action->characterAction = 11;
        } else {
            action->characterAction = 0;
        }
    } else {
        action->characterAction = characterAction;
    }

    if (restart != 0) {
        action->lastCharacterAction = -1;
        YobiAction* attachment = payload->action44;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->action34;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->action38;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
    }
}

// 原函数：0x004424B0。
void Sub_4424B0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float firstTargetX = static_cast<float>(action->mapX200.value() * 64.0);
    const double firstTargetY = action->mapY210.value() * 64.0;
    const float firstWorldX = action->worldX;
    // 原 FCOMP/C3；Y在首轮不落F32，只有X比较通过才读取世界Y。
    if (!(firstTargetX < firstWorldX || firstTargetX > firstWorldX)) {
        const float firstWorldY = action->worldY;
        if (!(firstTargetY < firstWorldY || firstTargetY > firstWorldY)
            && std::bit_cast<std::int16_t>(action->field270) > 0) {
            const std::int32_t previousX =
                action->mapX1E0.value();
            const std::int32_t previousY =
                action->mapY1F0.value();
            const std::int32_t nextY = action->pathY248[0];
            const std::int32_t nextX = action->pathX220[0];
            const std::int32_t speed = action->walkSpeed;
            const float rate =
                static_cast<float>(Sub_442110(action, nextX, nextY, speed, 0.0F));
            Sub_442460(action);
            if ((action->flags & 0x200000u) == 0) {
                auto* const payload = action->payload.mapActor;
                YobiAction* const child = payload->action2C;
                if (child != nullptr) {
                    Sub_442110(child, previousX, previousY, child->walkSpeed, rate);
                }
            }
        }
    }

    const float targetX = static_cast<float>(action->mapX200.value() * 64.0);
    const float targetY = static_cast<float>(action->mapY210.value() * 64.0);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->field280);
        const float stepX = std::bit_cast<float>(action->field27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::detail::Sub_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
    }

    // 此入口先写count，不使用encoded优先的通用operator=(int32_t)。
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldX * 0.015625));
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldY * 0.015625));
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
}

// 原函数：0x00442730。
void Sub_442730(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(action->mapX200.value() * 64.0);
    const float targetY = static_cast<float>(action->mapY210.value() * 64.0);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->field280);
        const float stepX = std::bit_cast<float>(action->field27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::detail::Sub_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
    }

    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldX * 0.015625));
    action->mapX1E0 = static_cast<std::int32_t>(mapX);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        action->worldY * 0.015625));
    action->mapY1F0 = static_cast<std::int32_t>(mapY);
}

// 原函数：0x004428F0。
void Sub_4428F0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(action->mapX200.value() * 64.0);
    const float targetY = static_cast<float>(action->mapY210.value() * 64.0);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float rateX = std::bit_cast<float>(world::detail::gValue9DAD78);
        const float rawStepX = std::bit_cast<float>(action->field27C);
        const double scaledX = static_cast<double>(rateX) * rawStepX;
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const float stepX = static_cast<float>(scaledX);
        const float rateY = std::bit_cast<float>(world::detail::gValue9DAD78);
        const float rawStepY = std::bit_cast<float>(action->field280);
        const float stepY = static_cast<float>(static_cast<double>(rateY) * rawStepY);
        const double distance = world::detail::Sub_46FB80(
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
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
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
        &payload->action04, &payload->action08, &payload->action0C,
        &payload->action10, &payload->action2C, &payload->action4C,
        &payload->action30, &payload->action44,
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

} // namespace detail

} // namespace cg::login

namespace cg::world::detail {

// 原函数：0x0050AEF0。
void Sub_50AEF0(const std::uintptr_t, const char* const payload)
{
    char text[2048];
    for (std::int32_t field = 3; field < 28; field += 5) {
        network::ExtractDelimitedField_52CED0(payload, '|', field - 2, 2047, text);
        if (std::strlen(text) == 0) {
            return;
        }
        const std::int32_t objectIndex = network::Base62Decode_52D050(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field - 1, 2047, text);
        const std::int32_t hp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field, 2047, text);
        const std::int32_t maxHp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 2047, text);
        const std::int32_t fp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 2, 2047, text);
        const std::int32_t maxFp = std::atoi(text);
        login::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action == nullptr) {
            continue;
        }

        // 四个目标的发布顺序不同，保留每次构造/复制/析构，不统一同类operator=。
        action->readiness = static_cast<std::int32_t>(hp);

        action->maxHp = static_cast<std::int32_t>(maxHp);

        action->barValue1B0 = static_cast<std::int32_t>(fp);

        action->barMaximum1C0 = static_cast<std::int32_t>(maxFp);
    }
}

// 原函数：0x00444410。
void Sub_444410(
    login::YobiAction* const action, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction, const std::int32_t characterAction,
    const std::int32_t option1, const std::int32_t option2,
    const std::int32_t option3, const std::int32_t option4)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    const std::int32_t index = payload->index;
    switch (characterAction) {
    case 0:
        action->field270 = 0;
        [[fallthrough]];
    case 31:
        login::detail::Sub_43EFC0(action, 0, 1);
        action->direction = direction;
        return;
    case 50: {
        auto& record = gWorldObjects[index];
        record.value34 = direction;
        record.value38 = x;
        record.value3C = y;
        if ((action->mapX200 == static_cast<std::int32_t>(x))
            && (action->mapY210 == static_cast<std::int32_t>(y))) {
            return;
        }
        const auto distance = [](const std::int32_t target,
                                 platform::LegacyCryptInt* const coordinate) {
            const std::int32_t sign =
                (static_cast<std::int32_t>(target) - (*coordinate));
            const std::int32_t value =
                (static_cast<std::int32_t>(target) - (*coordinate));
            return sign < 0 ? -value : value;
        };
        const std::int32_t firstX = distance(x, &action->mapX200);
        const std::int32_t firstY = distance(y, &action->mapY210);
        const bool yLarger = firstX - firstY < 0;
        const std::int32_t secondX = distance(x, &action->mapX200);
        const std::int32_t secondY = distance(y, &action->mapY210);
        const std::int32_t difference = yLarger ? secondY - secondX : secondX - secondY;
        if (difference > 1
            || (reinterpret_cast<const unsigned char*>(&record.flags42)[1] & 2u) != 0) {
            return;
        }
        [[fallthrough]];
    }
    case 1: {
        const std::uint16_t flags = gWorldObjects[index].flags42;
        if ((flags & 0x300u) == 0x300u) {
            if (action->field270 < 10) {
                login::detail::Sub_441FC0(action, x, y);
            } else {
                ResetMapFollowersFromPath(action);
                login::detail::Sub_441F60(action, x, y);
                gWorldObjects[index].value34 = -1;
            }
        } else {
            login::detail::Sub_441F60(action, x, y);
        }
        if (characterAction == 1) {
            action->walkSpeed = option1;
        }
        login::detail::Sub_43EFC0(action, 1, 0);
        return;
    }
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
    case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
    case 18: case 19:
        login::detail::Sub_43EFC0(action, characterAction, 1);
        if (characterAction == 18 || characterAction == 19) {
            action->lastCharacterAction = -1;
        }
        action->direction = direction;
        return;
    case 30:
        login::detail::Sub_43EFC0(action, 1, 1);
        action->direction = direction;
        return;
    case 40:
        if (option1 >= 0) {
            Sub_442D60(action, option1);
            login::detail::Sub_43EFC0(action, 0, 1);
        } else {
            Sub_442D90(action);
        }
        login::detail::Sub_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 41:
        if (option1 == 1) {
            Sub_442BD0(action);
        } else {
            Sub_442C00(action);
        }
        action->direction = direction;
        return;
    case 42:
        if (option1 >= 0) {
            Sub_442CA0(action, option1);
        } else {
            Sub_442CD0(action);
        }
        login::detail::Sub_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 44:
        if (option1 >= 0) {
            Sub_442D00(action, option1);
        } else {
            Sub_442D30(action);
        }
        login::detail::Sub_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 45:
        if (option1 == 1) {
            Sub_43D970(action);
        } else {
            Sub_43D9A0(action);
        }
        action->direction = direction;
        return;
    case 46:
        if (option1 != 0) {
            Sub_43D9D0(action, option1);
        } else {
            Sub_43DA10(action);
        }
        action->direction = direction;
        return;
    case 47:
        Sub_43DA40(action);
        action->direction = direction;
        return;
    case 48:
        if (option1 == 1) {
            Sub_43DA70(action);
        } else {
            Sub_43DAA0(action);
        }
        action->direction = direction;
        return;
    case 49:
        Sub_43DAD0(action);
        action->direction = direction;
        return;
    case 51:
        login::detail::Sub_442AB0(action, x, y);
        action->field270 = 0;
        login::detail::Sub_43EFC0(action, 0, 1);
        action->direction = direction;
        gWorldObjects[index].value34 = -1;
        return;
    case 60: {
        action->imageNo = character_select::detail::Sub_51B740(option1);
        const std::int32_t recordImage = character_select::detail::Sub_51B740(option1);
        gWorldObjects[payload->index].imageNo = static_cast<std::uint32_t>(recordImage);
        if (option2 > 0) {
            if (payload->action30 == nullptr) {
                const std::int32_t image = character_select::detail::Sub_51B740(option2);
                payload->action30 = login::detail::NewYobiGear_4410F0(image);
            }
            if (payload->action30 != nullptr) {
                const std::int32_t image = character_select::detail::Sub_51B740(option2);
                payload->action30->imageNo = image;
            }
        } else {
            login::YobiAction* const gear = payload->action30;
            if (gear != nullptr) {
                login::detail::MarkDeath(gear);
                payload->action30 = nullptr;
            }
        }
        if (option3 > 0) {
            std::int32_t image = character_select::detail::Sub_51B740(option3);
            if (payload->action34 == nullptr) {
                const std::int32_t petY =
                    action->mapY1F0.value();
                const std::int32_t petX =
                    action->mapX1E0.value();
                payload->action34 = login::detail::NewYobiGearPet_441130(image, petX, petY);
            }
            login::YobiAction* const firstPet = payload->action34;
            if (firstPet != nullptr) {
                firstPet->imageNo = image;
            }
            if (option4 == 6) {
                login::YobiAction* const previousSecondPet = payload->action38;
                ++image;
                if (previousSecondPet == nullptr) {
                    const std::int32_t petY =
                        action->mapY1F0.value();
                    const std::int32_t petX =
                        action->mapX1E0.value();
                    payload->action38 = login::detail::NewYobiGearPet_441130(image, petX, petY);
                }
                login::YobiAction* const secondPet = payload->action38;
                if (secondPet != nullptr) {
                    secondPet->imageNo = image;
                }
            }
        } else {
            login::YobiAction* const firstPet = payload->action34;
            if (firstPet != nullptr) {
                login::detail::MarkDeath(firstPet);
                payload->action34 = nullptr;
            }
            login::YobiAction* const secondPet = payload->action38;
            if (secondPet != nullptr) {
                login::detail::MarkDeath(secondPet);
                payload->action38 = nullptr;
            }
        }
        payload->kind3C = option4;
        return;
    }
    case 63:
        Sub_43D9D0(action, option1);
        action->direction = direction;
        return;
    case 64:
        Sub_43DA10(action);
        action->direction = direction;
        return;
    case 70:
        if (option1 >= 0) {
            if (payload->action44 == nullptr) {
                login::YobiAction* const ride = login::detail::NewYobiAction_404AE0(2, 0);
                payload->action44 = ride;
                ride->callback = nullptr;
                payload->action44->characterAction = 0;
                payload->action44->displayPriority = 10;
                payload->action44->flags = 0x34u;
                payload->action44->flags |= 0x200u;
            }
            const std::int32_t image = character_select::detail::Sub_51B740(option1);
            payload->action44->imageNo = image;
            const std::int32_t original =
                std::bit_cast<std::int32_t>(gWorldObjects[payload->index].imageNo);
            action->imageNo = Sub_51C0D0(original);
            action->phase = 1;
        } else {
            action->phase = 2;
        }
        {
            login::YobiAction* const ride = payload->action44;
            const bool resetRide = option1 >= 0 || ride != nullptr;
            payload->counter48 = 0;
            if (resetRide) {
                ride->characterAction = 0;
                if (action->characterAction == 1) {
                    payload->action44->characterAction = 1;
                }
            }
        }
        return;
    case 80:
        action->phase = 3;
        action->imageNo = character_select::detail::Sub_51B740(option1);
        payload->counter48 = 0;
        return;
    case 200:
        if (option1 != 0) {
            reinterpret_cast<unsigned char*>(&gWorldObjects[payload->index].flags120)[1] |= 0x10u;
        } else {
            gWorldObjects[payload->index].flags120 &= 0xEFFFu;
        }
        return;
    default:
        return;
    }
}

// WLR01：仅已占用/空action恢复，方向→第二/第一坐标→图号，创建后按原借用/字节复制/flags发布。
// 原函数：0x004467D0。
void Sub_4467D0(const std::int32_t objectIndex)
{
    if (objectIndex < 0) {
        return;
    }
    const std::int32_t index = Sub_444FA0(objectIndex);
    if (index < 0) {
        return;
    }
    auto& record = gWorldObjects[static_cast<std::size_t>(index)];
    if (record.used == 0 || record.action != nullptr) {
        return;
    }
    const std::int32_t direction = record.direction;
    const std::int32_t mapY =
        record.coordinate1C.value();
    const std::int32_t mapX =
        record.coordinate0C.value();
    const auto imageNo = std::bit_cast<std::int32_t>(record.imageNo);
    login::YobiAction* action =
        login::detail::Sub_441DB0(imageNo, mapX, mapY, direction);
    record.action = action;
    if (action == nullptr) {
        return;
    }
    record.used = 2;
    record.action->payload.mapActor->index = index;
    action = record.action;
    const std::int32_t level = record.level2C;
    action->field1D0 = level;
    char* const name = record.action->nameC4.data();
    std::memmove(name, record.name54.data(), std::strlen(record.name54.data()) + 1);
    char* const title = record.action->titleE1.data();
    std::memmove(title, record.title65.data(), std::strlen(record.title65.data()) + 1);
    action = record.action;
    const std::uint32_t flags = record.flags114;
    action->flags |= flags;
}

// WBR01：批量补建空action；未知类别仍保创建，仅跳过名字/flags尾，原主体保持。
// 原函数：0x00446640。
void Sub_446640()
{
    for (std::size_t index = 0; index < kWorldObjectCapacity; ++index) {
        auto& record = gWorldObjects[index];
        if (record.used == 0 || record.action != nullptr) {
            continue;
        }
        const std::int32_t direction = record.direction;
        const std::int32_t mapY =
            record.coordinate1C.value();
        const std::int32_t mapX =
            record.coordinate0C.value();
        const auto imageNo = std::bit_cast<std::int32_t>(record.imageNo);
        login::YobiAction* action =
            login::detail::Sub_441DB0(imageNo, mapX, mapY, direction);
        record.action = action;
        if (action == nullptr) {
            continue;
        }
        record.used = 2;
        record.action->payload.mapActor->index =
            static_cast<std::int32_t>(index);
        const std::uint16_t category = record.category;
        if (category == 1 || category == 8) {
            action = record.action;
            const std::int32_t level = record.level2C;
            action->field1D0 = level;
            char* const destination = record.action->nameC4.data();
            std::memmove(destination, record.name54.data(), std::strlen(record.name54.data()) + 1);
        } else if (category == 2 || category == 4) {
            const std::size_t limit = category == 2 ? 28u : 16u;
            if (std::strlen(record.nameC9.data()) <= limit) {
                char* const destination = record.action->nameC4.data();
                std::memmove(destination, record.nameC9.data(), std::strlen(record.nameC9.data()) + 1);
            } else {
                std::memmove(record.action->nameC4.data(), "???", 4);
            }
        } else {
            continue;
        }
        action = record.action;
        const std::uint32_t flags = record.flags114;
        action->flags |= flags;
    }
}

// MOP01：完整对象生产/更新；两坐标三值先取、四256B文本槽原复用关系沿SPEC，原失败/字段宽度保持。
// 原函数：0x00445350。
void Sub_445350(
    const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const char* const name,
    const char* const titles, const char* const extraText,
    const std::int32_t level, const std::int32_t value40,
    const std::int32_t flag1, const std::int32_t flag2,
    const std::int32_t type)
{
    std::int32_t index = Sub_444FA0(objectIndex);
    std::int32_t selectedDirection = direction;
    if (index < 0) {
        index = Sub_446130();
        if (index < 0) {
            return;
        }
        if (type > 3 && gValue9DAD68 == 0 && Sub_5183B0(imageNo) == 0) {
            selectedDirection = 0;
        }
        login::YobiAction* const action =
            login::detail::Sub_441DB0(imageNo, mapX, mapY, selectedDirection);
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        record.action = action;
        if (action == nullptr) {
            return;
        }
        record.used = 2;
        record.action->payload.mapActor->index = index;
        record.category = static_cast<std::uint16_t>(Sub_446210(type));
        record.objectIndex = objectIndex;
        record.value34 = -1;
    }

    auto& record = gWorldObjects[static_cast<std::size_t>(index)];
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value40 = static_cast<std::uint16_t>(value40);
    record.level2C = level;
    record.coordinate0C = static_cast<std::int32_t>(mapX);
    record.coordinate1C = static_cast<std::int32_t>(mapY);
    record.direction = selectedDirection;
    if (flag1 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x40u;
    }
    if (flag2 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x80u;
    }
    if (std::strlen(name) <= 16) {
        std::memmove(record.name54.data(), name, std::strlen(name) + 1);
    }

    char text100[256]; // 原var_100：action第4段/type3第1段。
    char text200[256]; // 原var_200：记录/action第2段共用。
    char text300[256]; // 原var_300：记录第1段/action第3段共用。
    char text400[256]; // 原var_400：记录第4段/type3第1段、action普通第1段共用。
    if (type != 3) {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text300);
        input::UnescapeLegacyTextAndNotify_52D2F0(text300);
        if (std::strlen(text300) <= 16) {
            std::memmove(record.title65.data(), text300, std::strlen(text300) + 1);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text200);
        input::UnescapeLegacyTextAndNotify_52D2F0(text200);
        if (std::strlen(text200) <= 32) {
            std::memmove(record.text76.data(), text200, std::strlen(text200) + 1);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            std::memmove(record.text128.data(), text400, std::strlen(text400) + 1);
        }
        if (std::strlen(extraText) <= 16) {
            std::memmove(record.textB8.data(), extraText, std::strlen(extraText) + 1);
        }
    } else {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            std::memmove(record.title65.data(), text400, std::strlen(text400) + 1);
        }
        record.textB8[0] = '\0';
    }
    const std::uint32_t flags = Sub_4461E0(type);
    record.flags114 = flags;
    const auto levelBits = static_cast<std::uint32_t>(level);
    if ((flags & 0x1000u) != 0
        && (levelBits & 0x100u) != 0 && (levelBits & 0x40u) != 0) {
        record.flags114 = flags | 0x400000u;
    }
    record.value118 = 1;
    login::YobiAction* action = record.action;
    if (action == nullptr) {
        return;
    }
    action->imageNo = imageNo;
    record.action->field1D0 = level;
    action = record.action;
    const std::uint32_t actionFlags = record.flags114;
    action->flags |= actionFlags;
    if (std::strlen(name) <= 16) {
        char* const destination = record.action->nameC4.data();
        std::memmove(destination, name, std::strlen(name) + 1);
    }

    // 原action阶段独立重新解析，不能复用记录阶段的文本或补写记录第3段。
    if (type != 3) {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            char* const destination = record.action->titleE1.data();
            std::memmove(destination, text400, std::strlen(text400) + 1);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text200);
        input::UnescapeLegacyTextAndNotify_52D2F0(text200);
        if (std::strlen(text200) <= 32) {
            char* const destination = record.action->text102.data();
            std::memmove(destination, text200, std::strlen(text200) + 1);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, text300);
        input::UnescapeLegacyTextAndNotify_52D2F0(text300);
        if (std::strlen(text300) <= 32) {
            char* const destination = record.action->text123.data();
            std::memmove(destination, text300, std::strlen(text300) + 1);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, text100);
        input::UnescapeLegacyTextAndNotify_52D2F0(text100);
        if (std::strlen(text100) <= 16) {
            char* const destination = record.action->text31C.data();
            std::memmove(destination, text100, std::strlen(text100) + 1);
        }
        if (std::strlen(extraText) <= 16) {
            char* const destination = record.action->text144.data();
            std::memmove(destination, extraText, std::strlen(extraText) + 1);
        }
    } else {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text100);
        input::UnescapeLegacyTextAndNotify_52D2F0(text100);
        if (std::strlen(text100) <= 16) {
            char* const destination = record.action->titleE1.data();
            std::memmove(destination, text100, std::strlen(text100) + 1);
        }
    }
    record.action->field1D8 = value40;
}

// WGQ01：X→fresh Y→类别/必需flags120，返对象编号或-1；保比较读取/RNG与动态上界。
// 原函数：0x00445030。
std::int32_t Sub_445030(
    const std::int32_t x, const std::int32_t y,
    const std::uint16_t category, const std::uint16_t flags)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return -1;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            login::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (record.flags120 & flags) != 0) {
                return record.objectIndex;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return -1;
}

// WGQ01：X→fresh Y→类别，返完整0/1，无额外flags门禁；原主体保持。
// 原函数：0x004450D0。
std::int32_t Sub_4450D0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            login::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// WGQ01：X→fresh Y→类别，返对象编号或-1，无标志门禁；保动态上界。
// 原函数：0x00445160。
std::int32_t Sub_445160(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return -1;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            login::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0) {
                return record.objectIndex;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return -1;
}

// WGQ01：包含掩码0分别跳过相应W16字段，原X/fresh Y/类别后再检查，返完整0/1。
// 原函数：0x004451F0。
std::int32_t Sub_4451F0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category,
    const std::uint16_t flags42Mask, const std::uint16_t flags120Mask)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            login::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (flags42Mask == 0 || (record.flags42 & flags42Mask) != 0)
                && (flags120Mask == 0 || (record.flags120 & flags120Mask) != 0)) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// WGQ01：排除掩码0仍沿原W16字段读取，42失败不取120，返完整0/1；主体保持。
// 原函数：0x004452A0。
std::int32_t Sub_4452A0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category,
    const std::uint16_t excluded42, const std::uint16_t excluded120)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            login::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (record.flags42 & excluded42) == 0
                && (record.flags120 & excluded120) == 0) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// 原函数：0x0046B4B0。
void Sub_46B4B0(const std::int32_t direction)
{
    if (direction < 0 || direction > 7) {
        return;
    }
    login::YobiAction* const action = cg::world::detail::gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return;
    }
    const float movementX = std::bit_cast<float>(gValue9EE8FC);
    if (movementX < 0.0F || movementX > 0.0F) {
        return;
    }
    const float movementY = std::bit_cast<float>(gValue9EE900);
    if (movementY < 0.0F || movementY > 0.0F
        || gValue62B5AA != 0 || gValueC82220 != 0
        || action->direction == direction) {
        return;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gValueC8223C + 500u >= now) {
        return;
    }
    static_cast<void>(Sub_51A040(direction));
    const std::uint32_t encoded = Sub_46BD20(direction, 1);
    const char text[2] {
        std::bit_cast<char>(static_cast<std::uint8_t>(encoded)), '\0',
    };
    const std::int32_t y = gPlayerY.value();
    const std::int32_t x = gPlayerX.value();
    static_cast<void>(Sub_50C8D0(x, y, text));
    gValueC8223C = platform::ReadTickCount32();
}

// 原函数：0x0046A190。
std::uint32_t Sub_46A190()
{
    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = cg::world::detail::gPlayerRuntimeSlots.objectIndex;
        login::YobiAction* const action = ReadPetMapAction(index);
        const bool isPlayer = ReadPetMapObjectIndex(index) == playerIndex;
        if (isPlayer) {
            const double targetX = gPlayerSnapshotX.value() * 64.0;
            const std::uint32_t worldXBits = gWorldCurrentXBits;
            const float worldX = std::bit_cast<float>(worldXBits);
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = gPlayerSnapshotY.value() * 64.0;
            const std::uint32_t worldYBits = gWorldCurrentYBits;
            const float worldY = std::bit_cast<float>(worldYBits);
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->field270) <= 0) {
                continue;
            }
        } else {
            const double targetX = action->mapX200.value() * 64.0;
            const float worldX = action->worldX;
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = action->mapY210.value() * 64.0;
            const float worldY = action->worldY;
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->field270) <= 0) {
                continue;
            }
            if (position == 0) {
                gValue9DAD78 = 0x3F800000u;
                const std::int16_t count =
                    std::bit_cast<std::int16_t>(action->field270);
                if (count > 5) {
                    gValue9DAD78 = 0x40000000u;
                } else if (count >= 4) {
                    gValue9DAD78 = 0x3FCCCCCDu;
                } else if (count >= 2) {
                    gValue9DAD78 = 0x3F99999Au;
                }
            }
        }

        login::YobiAction* const first = ReadPetMapAction(ReadPetFollowIndex(0));
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t speed = first->walkSpeed;
        const std::int32_t nextX = action->pathX220[0];
        if (isPlayer) {
            Sub_46A410(nextX, nextY, speed);
        } else {
            login::detail::Sub_4422F0(action, nextX, nextY, speed);
        }
        login::detail::Sub_442460(action);
        // 两路末项前视均在消费前丢弃，不扩展原五槽顺序表。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        login::YobiAction* const next = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            action->mapY1F0.value();
        const std::int32_t x =
            action->mapX1E0.value();
        login::detail::Sub_441F60(next, x, y);
    }

    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = cg::world::detail::gPlayerRuntimeSlots.objectIndex;
        if (ReadPetMapObjectIndex(index) == playerIndex) {
            Sub_46A700();
        } else {
            login::detail::Sub_4428F0(ReadPetMapAction(index));
        }
    }
    static_cast<void>(Sub_468200());
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = worldXBits;
    gWorldProjectionReferenceYBits = worldYBits;
    return worldXBits;
}

// 原函数：0x0046B1B0。
void MovePlayerOneStep_46B1B0(
    const std::int32_t x, const std::int32_t y, const std::int32_t speed)
{
    login::YobiAction* const player = cg::world::detail::gPlayerRuntimeSlots.action;
    auto* const payload = player == nullptr
        ? nullptr : player->payload.mapActor;
    const float rate = static_cast<float>(
        static_cast<double>(speed) * std::bit_cast<float>(0x3C23D70Au));
    if (payload != nullptr && payload->action2C != nullptr) {
        const std::int32_t previousY =
            player->mapY1F0.value();
        const std::int32_t previousX =
            cg::world::detail::gPlayerRuntimeSlots.action->mapX1E0.value();
        static_cast<void>(login::detail::Sub_442110(
            payload->action2C, previousX, previousY, speed, rate));
    }

    gPlayerSnapshotX = static_cast<std::int32_t>(x);
    gPlayerSnapshotY = static_cast<std::int32_t>(y);

    const std::int32_t pixelX =
        (gPlayerSnapshotX * 64);
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float differenceX = static_cast<float>(
        static_cast<double>(pixelX) - std::bit_cast<float>(worldXBits));
    const std::int32_t pixelY =
        (gPlayerSnapshotY * 64);
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const platform::detail::MapMoveDifference difference {
        differenceX,
        static_cast<double>(pixelY) - std::bit_cast<float>(worldYBits),
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    gValue9EE8FC = std::bit_cast<std::uint32_t>(
        static_cast<float>(direction.unroundedX * rate * 4.0F));
    gValue9EE900 = std::bit_cast<std::uint32_t>(
        static_cast<float>(static_cast<double>(direction.y) * rate * 4.0F));
    if (direction.unroundedX < 0.0 || direction.unroundedX > 0.0
        || direction.y < 0.0F || direction.y > 0.0F) {
        const auto value = static_cast<std::int32_t>(
            Sub_46CDF0(direction.x, direction.y));
        static_cast<void>(Sub_51A040(value));
        static_cast<void>(Sub_51A640());
    }
    Sub_519EE0();
}

// 原函数：0x0046A5F0。
void Sub_46A5F0()
{
    if (gValueFF0D36 == 0 || (ReadPlayerFlags18A() & 0x100u) == 0) {
        return;
    }
    for (std::size_t position = 1; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        login::YobiAction* const action = ReadPetMapAction(index);
        const double targetX = action->mapX200.value() * 64.0;
        const float worldX = action->worldX;
        if (targetX < worldX || targetX > worldX) {
            continue;
        }
        const double targetY = action->mapY210.value() * 64.0;
        const float worldY = action->worldY;
        if (targetY < worldY || targetY > worldY
            || std::bit_cast<std::int16_t>(action->field270) <= 0) {
            continue;
        }

        const std::int32_t speed = cg::world::detail::gPlayerRuntimeSlots.walkSpeed;
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t nextX = action->pathX220[0];
        static_cast<void>(
            login::detail::Sub_442110(action, nextX, nextY, speed, 0.0F));
        login::detail::Sub_442460(action);
        // 原末位置的9FB942前视值无论正负都被丢弃；不虚构第六槽。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        login::YobiAction* const nextAction = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            action->mapY1F0.value();
        const std::int32_t x =
            action->mapX1E0.value();
        login::detail::Sub_441F60(nextAction, x, y);
    }
}

// 原函数：0x0051A640。
login::YobiAction* Sub_51A640()
{
    login::YobiAction* const action = cg::world::detail::gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        action->field272 = 1;
    }
    return action;
}

// 原函数：0x0051A660。
login::YobiAction* Sub_51A660()
{
    login::YobiAction* const action = cg::world::detail::gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        action->field272 = 0;
    }
    return action;
}

// 原函数：0x0051A680。
std::int32_t Sub_51A680()
{
    login::YobiAction* const action = cg::world::detail::gPlayerRuntimeSlots.action;
    return action == nullptr ? 0 : std::bit_cast<std::int16_t>(action->field272);
}

// 原函数：0x00519EE0。
void Sub_519EE0()
{
    if (cg::world::detail::gPlayerRuntimeSlots.action == nullptr) {
        return;
    }
    const std::int32_t y = gPlayerY.value();
    const std::int32_t x = gPlayerX.value();
    const std::int32_t blocked = Sub_469810(x, y);
    login::YobiAction* action = cg::world::detail::gPlayerRuntimeSlots.action;
    if (blocked != 0) {
        gPlayerX = action->mapX1E0;
        gPlayerY = action->mapY1F0;
    }

    const std::uint32_t worldXBits = gWorldCurrentXBits;
    action->worldX = std::bit_cast<float>(worldXBits);
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    action->worldY = std::bit_cast<float>(worldYBits);
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->mapX1E0 = gPlayerX;
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->mapY1F0 = gPlayerY;
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->field27C = gValue9EE8FC;
    const std::uint32_t movementY = gValue9EE900;
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->field280 = movementY;
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->mapX200 = gPlayerSnapshotX;
    action = cg::world::detail::gPlayerRuntimeSlots.action;
    action->mapY210 = gPlayerSnapshotY;
}

} // namespace cg::world::detail
