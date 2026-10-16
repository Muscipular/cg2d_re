#include "MapEffectMath.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <emmintrin.h>
#include <float.h>

namespace cg::platform::detail {
namespace {

// 原11个效果叶使用临时027F控制并在写回后恢复；CPP保相同宿主掩码/舍入职责。
class MapMathControlScope final {
public:
    MapMathControlScope() noexcept : previous_(_control87(0, 0))
    {
        static_cast<void>(_control87(_MCW_EM | _RC_NEAR, _MCW_EM | _MCW_RC));
    }
    ~MapMathControlScope() noexcept
    {
        static_cast<void>(_control87(previous_, _MCW_EM | _MCW_RC));
    }
private:
    unsigned int previous_;
};

// COR15/COR16：常量用十进制或等值分数；F后缀保持原F32提升为double的语义。
constexpr double kRadians54DD18 = 0.017453292519444445; // 原常量参考：0x0054DD18。
constexpr float kSpeedScale54E228 = 0.001F; // 原常量参考：0x0054E228。
constexpr float kFallback54E230 = 0.0001F; // 原常量参考：0x0054E230。
constexpr float kSpawnScale54E22C = 720.0F; // 原常量参考：0x0054E22C。
constexpr float kPercentScale54D4E4 = 0.01F; // 原常量参考：0x0054D4E4。
constexpr float kArcScale54DD34 = 1.0F / 30.0F; // 原常量参考：0x0054DD34。
constexpr float kActorRate54DD3C = 1.6F; // 原常量参考：0x0054DD3C。
constexpr float kActorRate54DD40 = 1.2F; // 原常量参考：0x0054DD40。
constexpr float kSpiralY54DD10 = 0.6F; // 原常量参考：0x0054DD10。
constexpr float kProjectionInverse54E218 = 4.0F / 3.0F; // 原常量参考：0x0054E218。
constexpr std::array<double, 8> kObserverAngles {
    2.356194490125,
    1.57079632675,
    0.785398163375,
    0.0,
    5.4977871436250005,
    4.71238898025,
    3.926990816875,
    3.1415926535
};

// ACP01：宿主向零I64转换；旧控制字/T80不是当前数值实现。
std::int64_t TruncateMapIntermediate(const double value) noexcept
{
    return _mm_cvttsd_si64(_mm_set_sd(value));
}

} // namespace

extern "C" void Cg2dMapEffectDirection(
    const std::int32_t angle, const float* const speed,
    const std::uint32_t* const offsetXBits, const std::uint32_t* const offsetYBits,
    double* const direction) noexcept
{
    const MapMathControlScope control;
    const double radians = angle * kRadians54DD18;
    const volatile float* const sampledSpeed = speed;
    const volatile std::uint32_t* const sampledX = offsetXBits;
    const volatile std::uint32_t* const sampledY = offsetYBits;
    const double horizontal = std::cos(radians) * *sampledSpeed;
    direction[0] = horizontal + std::bit_cast<float>(static_cast<std::uint32_t>(*sampledX));
    // 原X写回之后先读中心Y，再重新读幅值。
    const double centerY = std::bit_cast<float>(static_cast<std::uint32_t>(*sampledY));
    direction[1] = centerY - std::sin(radians) * *sampledSpeed;
}

extern "C" void Cg2dMapEffectMotion(
    const double* const direction, const std::int32_t cameraX,
    const std::int32_t cameraY, MapEffectMotion* const result) noexcept
{
    const MapMathControlScope control;
    const volatile double* const sampled = direction;
    const double vertical = cameraY + sampled[1];
    const double horizontal = cameraX + sampled[0];
    result->magnitude = std::sqrt(horizontal * horizontal + vertical * vertical);
    // 原C0/C3分支：正值1，负值或无序-1，零0。
    result->horizontalSign = horizontal > 0.0 ? 1 : (!(horizontal >= 0.0) ? -1 : 0);
    result->verticalSign = vertical > 0.0 ? 1 : (!(vertical >= 0.0) ? -1 : 0);
}

extern "C" std::int32_t Cg2dMapEffectDivisor(
    const std::int32_t exponent, const double* const magnitude) noexcept
{
    const MapMathControlScope control;
    const double power = std::pow(1.25, exponent);
    const volatile double* const sampled = magnitude;
    double divisor = power * *sampled;
    // 原C3判断包含零与无序，保留原F32备用幅值。
    if (divisor == 0.0 || std::isnan(divisor)) divisor = kFallback54E230;
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        TruncateMapIntermediate(kSpawnScale54E22C / divisor)));
}

extern "C" void Cg2dMapEffectStep(
    const std::int32_t speed, const double* const direction, std::int64_t* const delta) noexcept
{
    const MapMathControlScope control;
    const double scaled = static_cast<double>(speed) * kSpeedScale54E228;
    const volatile double* const sampled = direction;
    delta[0] = TruncateMapIntermediate(scaled * sampled[0] * 65536.0);
    delta[1] = TruncateMapIntermediate(scaled * sampled[1] * 65536.0);
}

extern "C" void Cg2dMapObserverProjection(
    const std::int32_t frame, const std::int32_t* const duration,
    const std::int32_t direction, MapObserverProjection* const result) noexcept
{
    const MapMathControlScope control;
    const double progress = frame * 100.0 / *duration;
    result->progress = static_cast<float>(progress);
    const double radius = progress * 480.0 * kPercentScale54D4E4 - 240.0;
    const double angle = kObserverAngles[direction];
    result->x = TruncateMapIntermediate(std::cos(angle) * radius);
    result->y = TruncateMapIntermediate(-(std::sin(angle) * radius));
}

extern "C" std::int64_t Cg2dMapObserverArc(const float* const progress) noexcept
{
    const MapMathControlScope control;
    const double difference = *progress - 40.0;
    return TruncateMapIntermediate(difference * difference * kArcScale54DD34);
}

extern "C" void Cg2dMapActorVelocity(
    const MapActorVelocityInput* const input, MapActorVelocity* const result) noexcept
{
    const MapMathControlScope control;
    const volatile MapActorVelocityInput* const sampled = input;
    const float overrideRate = sampled->overrideRate;
    if (overrideRate != 0.0f && !std::isnan(overrideRate)) {
        result->rate = sampled->overrideRate;
    } else {
        const std::int16_t count = sampled->pathCount;
        const double rate = count > 5 ? 2.0 :
            (count >= 4 ? kActorRate54DD3C :
                (count >= 2 ? kActorRate54DD40 : 1.0));
        result->rate = static_cast<float>(sampled->speed * rate * kPercentScale54D4E4);
    }
    const std::int32_t targetX = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(sampled->targetX) << 6);
    const double horizontal = static_cast<double>(targetX) - sampled->worldX;
    const std::int32_t targetY = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(sampled->targetY) << 6);
    const double vertical = static_cast<double>(targetY) - sampled->worldY;
    const float roundedY = static_cast<float>(vertical);
    const double length = std::sqrt(vertical * roundedY + horizontal * horizontal);
    const float roundedLength = static_cast<float>(length);
    if (length > 0.0) {
        result->directionX = static_cast<float>(horizontal / roundedLength);
        result->directionY = static_cast<float>(static_cast<double>(roundedY) / roundedLength);
    } else {
        result->directionX = 0.0f;
        result->directionY = 0.0f;
    }
    result->stepX = static_cast<float>(static_cast<double>(result->directionX) * result->rate * 4.0);
    result->stepY = static_cast<float>(static_cast<double>(result->directionY) * result->rate * 4.0);
}

extern "C" void Cg2dMapMoveDirection(
    const MapMoveDifference* const difference, MapMoveDirection* const result) noexcept
{
    const MapMathControlScope control;
    const volatile MapMoveDifference* const sampled = difference;
    const double vertical = sampled->y;
    const float roundedY = static_cast<float>(vertical);
    const double firstX = sampled->x;
    const double squareX = firstX * sampled->x;
    const double length = std::sqrt(vertical * roundedY + squareX);
    const float roundedLength = static_cast<float>(length);
    if (length > 0.0) {
        const double horizontal = sampled->x / roundedLength;
        result->x = static_cast<float>(horizontal);
        result->unroundedX = horizontal;
        result->y = static_cast<float>(static_cast<double>(roundedY) / roundedLength);
    } else {
        result->unroundedX = 0.0;
        result->x = 0.0f;
        result->y = 0.0f;
    }
}

extern "C" void Cg2dMapActorSpiralOffset(
    const std::int32_t progress, const std::int32_t angle, std::int64_t* const result) noexcept
{
    const MapMathControlScope control;
    const double radians = angle * kRadians54DD18;
    result[0] = TruncateMapIntermediate(std::cos(radians) * (progress * 1.75));
    result[1] = TruncateMapIntermediate(-std::sin(radians) * (static_cast<double>(progress) * kSpiralY54DD10));
}

extern "C" void Cg2dMapObserverExitOffset(
    const std::int32_t frame, const std::int32_t direction, std::int64_t* const result) noexcept
{
    const MapMathControlScope control;
    const double angle = kObserverAngles[direction];
    const std::int32_t horizontal = (frame - 50) * 10;
    result[0] = TruncateMapIntermediate(std::cos(angle) * horizontal);
    const std::int32_t vertical = (50 - frame) * 10;
    result[1] = TruncateMapIntermediate(std::sin(angle) * vertical);
}

extern "C" void Cg2dMapTimedOffset(
    const std::int32_t angle, const float* const amplitude,
    std::uint32_t* const xBits, std::uint32_t* const yBits) noexcept
{
    const MapMathControlScope control;
    const double radians = angle * kRadians54DD18;
    const volatile float* const sampled = amplitude;
    *xBits = std::bit_cast<std::uint32_t>(static_cast<float>(std::cos(radians) * *sampled));
    *yBits = std::bit_cast<std::uint32_t>(static_cast<float>(-(std::sin(radians) * *sampled)));
}

} // namespace cg::platform::detail

namespace cg::platform {

// 原46CE30：两参考值在X输出前取样；原四F32比例均为精确二进制值。
extern "C" float* Cg2dProjectWorldToScreenX87(
    const float x, const float y, float* const outX, float* const outY,
    const volatile std::uint32_t* const referenceX,
    const volatile std::uint32_t* const referenceY) noexcept
{
    const double horizontal = (static_cast<double>(x) - std::bit_cast<float>(static_cast<std::uint32_t>(*referenceX))) * 0.015625;
    const double vertical = (static_cast<double>(y) - std::bit_cast<float>(static_cast<std::uint32_t>(*referenceY))) * 0.015625;
    *outX = static_cast<float>((vertical + horizontal) * 32.0 + 320.0);
    *outY = static_cast<float>((vertical - horizontal) * 24.0 + 240.0);
    return outX;
}

// 原46CE80：X发布之后才fresh读取参考Y，不能提前缓存。
extern "C" float* Cg2dProjectScreenToWorldX87(
    const float x, const float y, float* const outX, float* const outY,
    const volatile std::uint32_t* const referenceX,
    const volatile std::uint32_t* const referenceY) noexcept
{
    const double horizontal = x - 320.0;
    const double vertical = (y - 240.0) * detail::kProjectionInverse54E218;
    *outX = static_cast<float>(horizontal - vertical + std::bit_cast<float>(static_cast<std::uint32_t>(*referenceX)));
    *outY = static_cast<float>(vertical + horizontal + std::bit_cast<float>(static_cast<std::uint32_t>(*referenceY)));
    return outX;
}

} // namespace cg::platform
