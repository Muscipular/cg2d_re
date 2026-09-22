#include "cg/render/GraphicPendingQueue.hpp"

#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/MainLoop.hpp"

#include "../LoginMessageDependencies.hpp"

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
float gFloat1020EF0 = 0.0f;

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
