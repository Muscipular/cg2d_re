#include "cg/world/SceneState.hpp"

#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "WorldUpdateDependencies.hpp"

#include <bit>

namespace cg::world {
namespace {

// 原 0x9A7FA8；由 LegacyMouse.cpp 的 CRT 顺序适配显式构造、析构。
platform::LegacyCryptInt gRippleGate {};

constexpr std::array<std::int32_t, 6> kAllowedMapFloors {
    59000,
    59001,
    59100,
    37623,
    37619,
    37631,
};

// 原 0x5541F4；Sub_4493C0 按原顺序匹配的四项地图楼层表。
constexpr std::array<std::int32_t, 4> kMapFloors5541F4 {
    59000,
    59001,
    37623,
    37619,
};

} // namespace

std::int32_t& gCurrentSceneType = platform::gValueFF0D40;
std::int32_t gPendingSceneState = 0;
std::int32_t gCurrentSceneState = 0;
std::int32_t gPendingSceneType = 0;

std::uint8_t gValueC82204 = 0;
std::uint32_t gValue5ABFC4 = 0;
std::uint32_t gValue9A6E0C = 0;
std::uint32_t gValueFF0D5C = 0;
std::uint32_t gValueFF0D58 = 0;
std::uint32_t gValueD67E78 = 0;
std::uint32_t gValueFF0D74 = 0;
std::uint8_t gValue9DAD30 = 0;
std::array<std::uint32_t, 7> gValueD67B08 {};
std::uint32_t gValue58B6D0 = 1;
std::uint8_t gValueD9F2CA = 0;

std::uint32_t gWorldProjectionSourceA = 0;
std::uint32_t gWorldProjectionSourceB = 0;
std::uint32_t gWorldProjectionSourceC = 0;
std::uint32_t gWorldProjectionSourceD = 0;
std::uint32_t gWorldProjectionSnapshotA = 0;
std::uint32_t gWorldProjectionSnapshotB = 0;
std::uint32_t gWorldProjectionSnapshotC = 0;
std::uint32_t gWorldProjectionSnapshotD = 0;

volatile std::int32_t gMapFloor = 0; // 原 0x9EE8F0。
volatile std::uint32_t gWorldCurrentXBits = 0; // 原 0x9DAD4C。
volatile std::uint32_t gWorldCurrentYBits = 0; // 原 0x9DAD50。
volatile std::uint32_t gWorldProjectionReferenceXBits = 0; // 原 0xC751A8。
volatile std::uint32_t gWorldProjectionReferenceYBits = 0; // 原 0xC751A4。

static_assert(sizeof(gValueD67B08) == 7 * sizeof(std::uint32_t));

void ConstructRippleGate()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&gRippleGate);
}

void DestroyRippleGate()
{
    platform::legacy_crypt_int::Destroy_401CF0(&gRippleGate);
}

std::int32_t ReadRippleGateOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gRippleGate);
}

std::int32_t IsMapFloorAllowed_448240() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 41999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kAllowedMapFloors) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

// 原 sub_4493C0@[0x4493C0, 0x4493F2)；先判断闭区间，再按原表顺序匹配。
std::int32_t Sub_4493C0() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 40999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kMapFloors5541F4) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

void ProjectWorldToScreen_46CE30(
    const float worldX,
    const float worldY,
    float* const outX,
    float* const outY) noexcept
{
    const std::uint32_t referenceXBits = gWorldProjectionReferenceXBits;
    const std::uint32_t referenceYBits = gWorldProjectionReferenceYBits;
    const float referenceX = std::bit_cast<float>(referenceXBits);
    const float referenceY = std::bit_cast<float>(referenceYBits);

    const double u =
        (static_cast<double>(worldX) - static_cast<double>(referenceX))
        * 0.015625;
    const double v =
        (static_cast<double>(worldY) - static_cast<double>(referenceY))
        * 0.015625;
    *outX = static_cast<float>((v + u) * 32.0 + 320.0);
    *outY = static_cast<float>((v - u) * 24.0 + 240.0);
}

std::int32_t SetCurrentSceneType(std::int32_t sceneType)
{
    gCurrentSceneState = 0;
    gCurrentSceneType = sceneType;
    return sceneType;
}

std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState)
{
    gCurrentSceneType = sceneType;
    gCurrentSceneState = sceneState;
    return sceneType;
}

std::int32_t SetPendingSceneType(std::int32_t sceneType)
{
    gPendingSceneState = 0;
    gPendingSceneType = sceneType;
    return sceneType;
}

std::uint32_t CopyWorldProjectionState_468FF0()
{
    const std::uint32_t a = gWorldProjectionSourceA;
    const std::uint32_t b = gWorldProjectionSourceB;
    const std::uint32_t c = gWorldProjectionSourceC;
    gWorldProjectionSnapshotA = a;
    const std::uint32_t d = gWorldProjectionSourceD;
    gWorldProjectionSnapshotB = b;
    gWorldProjectionSnapshotC = c;
    gWorldProjectionSnapshotD = d;
    return d;
}

namespace detail {

// 原 0xC82230/0xC82234/0xC82236；供 46A9E0 初始化和后续消费者共享。
std::uint32_t gValueC82230 = 0;
std::int16_t gValueC82234 = 0;
std::int16_t gValueC82236 = 0;

// 原 [0x46A9E0, 0x46A9F4)；依次清三槽并返回完整 32 位零。
std::int32_t __cdecl Sub_46A9E0()
{
    gValueC82230 = 0;
    gValueC82234 = 0;
    gValueC82236 = 0;
    return 0;
}

} // namespace detail

} // namespace cg::world
