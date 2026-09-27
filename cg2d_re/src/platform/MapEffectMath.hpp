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

// 私有 MASM 叶保留 x87 指令/存储边界，恢复控制字；见场景 SPEC §20.19。
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

} // namespace cg::platform::detail
