#include "cg/render/GraphicPendingQueue.hpp"

#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include "../LoginMessageDependencies.hpp"

#include <algorithm>
#include <bit>
#include <cstdlib>

namespace cg::render {
namespace {

// 原比较器 0x5222A0：layer 按 unsigned8 升序，cacheIndex 按 signed16 降序。
int CompareGraphicCache3_5222A0(const void* const left, const void* const right)
{
    const auto& first = *static_cast<const GraphicCache3*>(left);
    const auto& second = *static_cast<const GraphicCache3*>(right);
    if (first.layer < second.layer) {
        return -1;
    }
    if (first.layer > second.layer) {
        return 1;
    }
    return static_cast<std::int32_t>(second.cacheIndex)
        - static_cast<std::int32_t>(first.cacheIndex);
}

// 原波纹坐标差 helper；读取顺序和每个 F32 落点均属于调用语义。
void CalculateRippleScreenDelta(float* const outX, float* const outY) noexcept
{
    const std::uint32_t previousYBits = gRipplePreviousWorldYBits;
    const std::uint32_t previousXBits = gRipplePreviousWorldXBits;
    float previousScreenX;
    float previousScreenY;
    world::ProjectWorldToScreen_46CE30(
        std::bit_cast<float>(previousXBits),
        std::bit_cast<float>(previousYBits),
        &previousScreenX,
        &previousScreenY);

    const std::uint32_t currentYBits = world::gWorldCurrentYBits;
    const std::uint32_t currentXBits = world::gWorldCurrentXBits;
    float currentScreenX;
    float currentScreenY;
    world::ProjectWorldToScreen_46CE30(
        std::bit_cast<float>(currentXBits),
        std::bit_cast<float>(currentYBits),
        &currentScreenX,
        &currentScreenY);

    const double xDifference =
        static_cast<double>(previousScreenX)
        - static_cast<double>(currentScreenX);
    const float yDifference = static_cast<float>(
        static_cast<double>(previousScreenY)
        - static_cast<double>(currentScreenY));

    *outX = static_cast<float>(
        xDifference * static_cast<double>(0.125f));
    *outY = static_cast<float>(
        static_cast<double>(yDifference) * static_cast<double>(0.125f));
}

// 原 SortTextures_520790 的单个保留槽；未列出的记录字段保持原值。
void AppendReservedGraphic(const std::uint8_t layer, const std::uint8_t batchType)
{
    const std::uint16_t countBits = platform::gPendingGraphicCount;
    const std::size_t index = static_cast<std::size_t>(countBits);
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[index];
    cache3.layer = layer;
    cache3.cacheIndex = std::bit_cast<std::int16_t>(countBits);

    GraphicCacheItem32& record = gLegacyPendingStorage.records[index];
    record.imageNo = -1;
    record.value4 = 0;
    record.dataBits = 0;
    record.batchType = batchType;

    platform::gPendingGraphicCount = static_cast<std::uint16_t>(countBits + 1U);
}

} // namespace

LegacyPendingStorage gLegacyPendingStorage {};
std::uint8_t gSpecialGraphicPending = 0;
std::int32_t gGraphicDirty = 0;
volatile std::uint32_t gRippleIntegerPhaseBits = 0; // 原 0x1020EEC。
float gFloat1020EF0 = 0.0f;
volatile std::uint32_t gRipplePreviousWorldXBits = 0; // 原 0x1020EF4。
volatile std::uint32_t gRipplePreviousWorldYBits = 0; // 原 0x1020EF8。
volatile std::uint32_t gRippleStrengthBits = 0; // 原 0x1020EFC。
volatile std::int32_t gRippleMapFloorCache = 0; // 原 0x1020F00。

const std::array<std::int32_t, 64> gRippleSineTable {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 11, 11, 12,
    12, 12, 11, 11, 11, 10, 9, 9, 8, 7, 6, 5, 4, 3, 2, 1,
    0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -10, -11, -11, -11, -12,
    -12, -12, -11, -11, -11, -10, -9, -9, -8, -7, -6, -5, -4, -3, -2, -1,
};

void Sub_520910()
{
    gFloat1020EF0 += 0.25f;
}

void Sub_520930(float* value)
{
    for (;;) {
        if (*value >= 64.0f) {
            *value = *value - 64.0f;
            continue;
        }
        if (*value >= 0.0f) {
            return;
        }
        *value = *value + 64.0f;
    }
}

std::int32_t Sub_521870()
{
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        gRippleStrengthBits = 0;
        return gate;
    }

    if (world::IsMapFloorAllowed_448240() == 0) {
        gRippleStrengthBits = 0;
        return 0;
    }

    const std::int32_t mapFloor = world::gMapFloor;
    const std::int32_t cachedMapFloor = gRippleMapFloorCache;
    if (mapFloor != cachedMapFloor) {
        gRippleMapFloorCache = mapFloor;
        gRippleStrengthBits = 0;
    }

    gCpuTempSurface = gCpuBackSurface;

    float xDelta;
    float yDelta;
    CalculateRippleScreenDelta(&xDelta, &yDelta);
    static_cast<void>(xDelta);

    const float phase = gFloat1020EF0;
    gFloat1020EF0 = static_cast<float>(
        static_cast<double>(phase) - static_cast<double>(yDelta));
    Sub_520930(&gFloat1020EF0);
    const std::int32_t integerPhase =
        static_cast<std::int32_t>(gFloat1020EF0);
    gRippleIntegerPhaseBits = std::bit_cast<std::uint32_t>(integerPhase);

    for (std::int32_t strip = 0; strip < 60; ++strip) {
        const std::uint32_t phaseBits = gRippleIntegerPhaseBits;
        const std::uint32_t tableIndex =
            (phaseBits + static_cast<std::uint32_t>(strip)) & 63U;
        const std::int32_t tableValue =
            gRippleSineTable[static_cast<std::size_t>(tableIndex)];
        const std::uint32_t strengthBits = gRippleStrengthBits;
        const std::uint32_t productBits =
            strengthBits * std::bit_cast<std::uint32_t>(tableValue);
        const std::int32_t offset =
            std::bit_cast<std::int32_t>(productBits) / 100;

        const std::int32_t destinationY = 8 * strip;
        const std::int32_t sourceTop = destinationY + 12 + offset;
        const std::int32_t begin =
            std::max({0, -sourceTop, -destinationY});
        const std::int32_t end = std::min({
            8,
            kCpuSurfaceHeight - sourceTop,
            kCpuSurfaceHeight - destinationY,
        });
        if (begin >= end) {
            continue;
        }

        static_cast<void>(BlitTempToBack(
            0,
            destinationY + begin,
            0,
            sourceTop + begin,
            kCpuSurfaceWidth,
            sourceTop + end));
    }

    const std::uint32_t strengthBits = gRippleStrengthBits;
    const std::int32_t strength = std::bit_cast<std::int32_t>(strengthBits);
    if (strength < 100) {
        const std::uint32_t updatedStrengthBits = strengthBits + 2U;
        gRippleStrengthBits = updatedStrengthBits;
        return std::bit_cast<std::int32_t>(updatedStrengthBits);
    }
    return strength;
}

void SortTextures_520790()
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount < 0) {
        // 新端拒绝负位型；原函数会越界，不把此保护归于原版。
        return;
    }

    const bool hasSpecialGraphic = gSpecialGraphicPending != 0;
    const std::int32_t reserveCount = hasSpecialGraphic ? 4 : 3;
    if (static_cast<std::int32_t>(initialCount) + reserveCount
        >= MAX_QUEUE_SIZE) {
        platform::gPendingGraphicCount = static_cast<std::uint16_t>(
            MAX_QUEUE_SIZE - reserveCount);
    }

    if (hasSpecialGraphic) {
        AppendReservedGraphic(2, 8);
    }
    AppendReservedGraphic(5, 6);
    AppendReservedGraphic(6, 7);
    AppendReservedGraphic(20, 9);

    std::qsort(
        gLegacyPendingStorage.cache3.data(),
        static_cast<std::size_t>(platform::gPendingGraphicCount),
        sizeof(GraphicCache3),
        CompareGraphicCache3_5222A0);
}

} // namespace cg::render

namespace cg::platform {

char (&gTextFF4E30)[0x88] = render::gLegacyPendingStorage.prefix;

} // namespace cg::platform

namespace cg::login::detail {

void SetPendingGraphicBatchTypeAtLegacyIndex(
    const std::int32_t index,
    const std::uint8_t type)
{
    if (index == -2) {
        // 原 records[-2].batchType 正好落到 0xFF4E8C，即 prefix+0x5C。
        render::gLegacyPendingStorage.prefix[0x5C] = std::bit_cast<char>(type);
        return;
    }
    if (index < 0 || index >= MAX_QUEUE_SIZE) {
        return;
    }

    render::gLegacyPendingStorage.records[static_cast<std::size_t>(index)].batchType = type;
}

} // namespace cg::login::detail
