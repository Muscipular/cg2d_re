#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform::detail {

// 46F243–46F2B9 的局部结果；不是共享状态或原记录布局。
struct MapEffectMotion {
    double magnitude;
    std::int32_t horizontalSign;
    std::int32_t verticalSign;
};

static_assert(offsetof(MapEffectMotion, magnitude) == 0);
static_assert(offsetof(MapEffectMotion, horizontalSign) == 8);
static_assert(offsetof(MapEffectMotion, verticalSign) == 12);
static_assert(sizeof(MapEffectMotion) == 16);

// ACP01：CPP/double/std数学近似替换；保留场景SPEC§20.19的字段与发布序。
extern "C" void Cg2dMapEffectDirection(
    std::int32_t angle,
    const float* speed,
    const std::uint32_t* offsetXBits,
    const std::uint32_t* offsetYBits,
    double* direction) noexcept;

extern "C" void Cg2dMapEffectMotion(
    const double* direction,
    std::int32_t cameraX,
    std::int32_t cameraY,
    MapEffectMotion* result) noexcept;

// 原46F38D/46F43A：S32指数经std::pow后重读借用幅值，返回向零I64低S32。
extern "C" std::int32_t Cg2dMapEffectDivisor(
    std::int32_t exponent,
    const double* magnitude) noexcept;

extern "C" void Cg2dMapEffectStep(
    std::int32_t speed,
    const double* direction,
    std::int64_t* delta) noexcept;

// 440C50 在 FrameMove 之前计算两轴截断值，并单独保留 FST 的 F32 progress。
struct MapObserverProjection {
    std::int64_t x;
    std::int64_t y;
    float progress;
};

static_assert(offsetof(MapObserverProjection, x) == 0);
static_assert(offsetof(MapObserverProjection, y) == 8);
static_assert(offsetof(MapObserverProjection, progress) == 16);
static_assert(sizeof(MapObserverProjection) == 24);

extern "C" void Cg2dMapObserverProjection(
    std::int32_t frame,
    const std::int32_t* duration,
    std::int32_t direction,
    MapObserverProjection* result) noexcept;

// 原 440D41–440D61；在 FrameMove 之后重新读取已存 F32 progress。
extern "C" std::int64_t Cg2dMapObserverArc(const float* progress) noexcept;

// 442110 的同步数值快照；既非 action 布局，也非共享状态。
struct MapActorVelocityInput {
    std::int32_t targetX;
    std::int32_t targetY;
    std::int32_t speed;
    std::int16_t pathCount;
    std::uint16_t reserved;
    float overrideRate;
    float worldX;
    float worldY;
};

struct MapActorVelocity {
    float rate;
    float directionX;
    float directionY;
    float stepX;
    float stepY;
};

static_assert(offsetof(MapActorVelocityInput, targetX) == 0);
static_assert(offsetof(MapActorVelocityInput, targetY) == 4);
static_assert(offsetof(MapActorVelocityInput, speed) == 8);
static_assert(offsetof(MapActorVelocityInput, pathCount) == 12);
static_assert(offsetof(MapActorVelocityInput, overrideRate) == 16);
static_assert(offsetof(MapActorVelocityInput, worldX) == 20);
static_assert(offsetof(MapActorVelocityInput, worldY) == 24);
static_assert(sizeof(MapActorVelocityInput) == 28);
static_assert(offsetof(MapActorVelocity, rate) == 0);
static_assert(offsetof(MapActorVelocity, directionX) == 4);
static_assert(offsetof(MapActorVelocity, directionY) == 8);
static_assert(offsetof(MapActorVelocity, stepX) == 12);
static_assert(offsetof(MapActorVelocity, stepY) == 16);
static_assert(sizeof(MapActorVelocity) == 20);

extern "C" void Cg2dMapActorVelocity(
    const MapActorVelocityInput* input, MapActorVelocity* result) noexcept;

// 46A410先将X差落F32，4422F0不落；调用方分别形成实际差值后借用此数值叶。
struct MapMoveDifference {
    double x;
    double y;
};

struct MapMoveDirection {
    double unroundedX; // X方向在F32落存之前的double值；ACP01不承诺原PC53数值。
    float x;
    float y;
};

static_assert(offsetof(MapMoveDifference, x) == 0);
static_assert(offsetof(MapMoveDifference, y) == 8);
static_assert(sizeof(MapMoveDifference) == 16);
static_assert(offsetof(MapMoveDirection, unroundedX) == 0);
static_assert(offsetof(MapMoveDirection, x) == 8);
static_assert(offsetof(MapMoveDirection, y) == 12);
static_assert(sizeof(MapMoveDirection) == 16);

extern "C" void Cg2dMapMoveDirection(
    const MapMoveDifference* difference, MapMoveDirection* result) noexcept;

// 原43EED2–43EF01：std::cos/sin与progress按原序乘法，输出两个截断I64偏移。
extern "C" void Cg2dMapActorSpiralOffset(
    std::int32_t progress, std::int32_t angle, std::int64_t* result) noexcept;

// 原440188-4401D2：固定方向的退出轨迹；整数抛物线仍由父回调计算。
extern "C" void Cg2dMapObserverExitOffset(
    std::int32_t frame, std::int32_t direction, std::int64_t* result) noexcept;

// 原448542-448560/448795-4487B3：两次重读幅值，直接按X→Y写共享F32位槽。
extern "C" void Cg2dMapTimedOffset(
    std::int32_t angle, const float* amplitude,
    std::uint32_t* xBits, std::uint32_t* yBits) noexcept;

} // namespace cg::platform::detail

namespace cg::platform {

// 原46CE30/46CE80；历史X87符号由CPP提供，保留volatile参考采样和返回outX。
extern "C" float* Cg2dProjectWorldToScreenX87(
    float x, float y, float* outX, float* outY,
    const volatile std::uint32_t* referenceX, const volatile std::uint32_t* referenceY) noexcept;
extern "C" float* Cg2dProjectScreenToWorldX87(
    float x, float y, float* outX, float* outY,
    const volatile std::uint32_t* referenceX, const volatile std::uint32_t* referenceY) noexcept;

} // namespace cg::platform
