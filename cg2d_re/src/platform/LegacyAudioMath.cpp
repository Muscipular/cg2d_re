#include "LegacyAudioMath.hpp"

#include <bit>
#include <cmath>
#include <emmintrin.h>

namespace cg::platform {

// 原FISTP I64后只消费低W32；宿主向零转换也定义无效值的整数返回。
static std::int32_t TruncateSoundIntermediate(const double value) noexcept
{
    const std::int64_t integer = _mm_cvttsd_si64(_mm_set_sd(value));
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(integer));
}

// 原52BA38..52BBC8；只借128项表，保持正向→反向→端点→音量的覆盖序。
extern "C" void Cg2dInitializeSoundTablesX87(
    std::int32_t* const pan, std::int32_t* const volume) noexcept
{
    const double logarithmBase = std::log(100.0);
    const std::int32_t panMaximum = TruncateSoundIntermediate(
        std::log(127.0) / logarithmBase * 5000.0);
    std::int32_t position = 64;
    for (std::int32_t value = 127; value >= 0; --value) {
        if ((value & 1) == 0) continue;
        std::int32_t difference = panMaximum - TruncateSoundIntermediate(
            std::log(value) / logarithmBase * 5000.0);
        if (difference > 5000) difference = 10000;
        pan[position++] = difference;
    }
    position = 64;
    for (std::int32_t value = 127; value >= 0; --value) {
        if ((value & 1) == 0) continue;
        std::int32_t difference = panMaximum - TruncateSoundIntermediate(
            std::log(value) / logarithmBase * 5000.0);
        if (difference > 5000) difference = 10000;
        pan[position--] = -difference;
    }
    pan[0] = -10000;
    pan[127] = 10000;

    const std::int32_t volumeBase = TruncateSoundIntermediate(
        std::log(127.0) / logarithmBase * -10000.0);
    for (std::int32_t value = 127; value > 0; --value) {
        std::int32_t difference = -volumeBase - TruncateSoundIntermediate(
            std::log(value) / logarithmBase * 10000.0);
        if (difference > 10000) difference = 10000;
        volume[value] = -difference;
    }
    volume[0] = volume[1];
}

// 原454E49..454EBC独立表，包含log(0)及原截断/上限路径，不借音效0槽。
extern "C" void Cg2dInitializeFixedMusicVolumeX87(std::int32_t* const volume) noexcept
{
    const double logarithmBase = std::log(100.0);
    const std::int32_t base = TruncateSoundIntermediate(
        std::log(127.0) / logarithmBase * -10000.0);
    for (std::int32_t value = 127; value >= 0; --value) {
        std::int32_t difference = -base - TruncateSoundIntermediate(
            std::log(value) / logarithmBase * 10000.0);
        if (difference > 10000) difference = 10000;
        volume[value] = -difference;
    }
}

// 原52CA07..52CA4E：先整数中心差，再以double平方；商算完才发布距离。
extern "C" std::int32_t Cg2dComputeSpatialSoundX87(
    const std::int32_t x, const std::int32_t y, std::int32_t* const distance) noexcept
{
    const double horizontal = x - 320;
    const double vertical = y - 240;
    const std::int32_t length = TruncateSoundIntermediate(
        std::sqrt(vertical * vertical + horizontal * horizontal));
    const std::int32_t scaled = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(length) << 4);
    const std::int32_t quotient = scaled / 106;
    *distance = length;
    return quotient;
}

// 原52CD77..52CDA6：W32平方和完成后才转double，区别于直接距离。
extern "C" std::int32_t Cg2dComputeSpatialScanQuotientX87(
    const std::int32_t x, const std::int32_t y) noexcept
{
    const std::uint32_t horizontal = static_cast<std::uint32_t>(x);
    const std::uint32_t vertical = static_cast<std::uint32_t>(y);
    const std::int32_t square = std::bit_cast<std::int32_t>(
        vertical * vertical + horizontal * horizontal);
    const std::int32_t length = TruncateSoundIntermediate(std::sqrt(square));
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(length) << 4) / 106;
}

// 原52CDE0..52CE05：F32结果按Y→X发布。
extern "C" void Cg2dScaleSpatialSourceX87(
    const std::int32_t x, const std::int32_t y,
    float* const worldX, float* const worldY) noexcept
{
    *worldY = static_cast<float>(y * 64.0);
    *worldX = static_cast<float>(x * 64.0);
}

// 原52CE0D..52CE8A：距离采样与发布采样分开；未发布返回索引，发布返回Y。
extern "C" std::int32_t Cg2dPublishSpatialProjectionX87(
    audio::SpatialAudioRecord* const records, const std::int32_t index,
    const float* const screenX, const float* const screenY) noexcept
{
    const volatile float* const sampledX = screenX;
    const volatile float* const sampledY = screenY;
    const double horizontal = *sampledX - 320.0;
    const double vertical = *sampledY - 240.0;
    const std::int32_t distance = TruncateSoundIntermediate(
        std::sqrt(vertical * vertical + horizontal * horizontal));
    audio::SpatialAudioRecord& record = records[index];
    if (record.value00 == 0 && record.value04 != -1 && distance >= record.value10) return index;

    const double projectedX = *sampledX;
    record.value04 = index;
    const std::int32_t truncatedX = TruncateSoundIntermediate(projectedX);
    const double projectedY = *sampledY;
    record.value08 = truncatedX;
    const std::int32_t truncatedY = TruncateSoundIntermediate(projectedY);
    record.value0C = truncatedY;
    record.value10 = distance;
    record.value00 = 0;
    return truncatedY;
}

} // namespace cg::platform
