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

} // namespace cg::platform::detail
