#include "cg/render/GraphicPendingQueue.hpp"

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include "../LoginMessageDependencies.hpp"

#include <bit>
#include <cstdlib>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::render {

// 原 [0x451350,0x451D78) 的 32 位像素本体在 SurfaceState，使用同一 Back/调色板。
void Sub_451350(RECT* rect, std::uint16_t color, std::int32_t mode);

namespace {

// 原比较器 0x5222A0：layer 按 unsigned8 升序，cacheIndex 按 signed16 降序。
int CompareGraphicCache3_5222A0(const void* const left, const void* const right)
{
    const auto& first = *static_cast<const GraphicCache3*>(left);
    const auto& second = *static_cast<const GraphicCache3*>(right);
    const std::uint8_t firstLayer = first.layer;
    const std::uint8_t secondLayer = second.layer;
    if (firstLayer > secondLayer) {
        return 1;
    }
    if (firstLayer < secondLayer) {
        return -1;
    }
    const std::int32_t firstIndex = first.cacheIndex;
    const std::int32_t secondIndex = second.cacheIndex;
    return secondIndex - firstIndex;
}

// 原波纹坐标差 helper；读取顺序和每个 F32 落点均属于调用语义。
// 原函数：0x005219C0。
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
std::uint16_t AppendReservedGraphic(
    const std::uint16_t indexBits,
    const std::uint8_t layer,
    const std::uint8_t batchType)
{
    const auto index = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(indexBits));
    GraphicCache3& cache3 = gLegacyPendingStorage.cache3[index];
    cache3.layer = layer;
    cache3.cacheIndex = std::bit_cast<std::int16_t>(
        platform::gPendingGraphicCount);

    GraphicCacheItem32& record = gLegacyPendingStorage.records[index];
    record.imageNo = -1;
    record.value4 = 0;
    record.reference.action = nullptr;
    record.batchType = batchType;

    const std::uint16_t countBits = platform::gPendingGraphicCount;
    const auto nextCountBits = static_cast<std::uint16_t>(countBits + 1U);
    platform::gPendingGraphicCount = nextCountBits;
    return nextCountBits;
}

} // namespace

LegacyPendingStorage gLegacyPendingStorage {}; // 原全局：0x00FF4E30。
std::int32_t gHoveredGraphicPendingIndex = 0; // 原全局：0x00D6976C。
std::uint8_t gSpecialGraphicPending = 0; // 原全局：0x01020EDC。
std::int32_t gGraphicDirty = 0; // 原全局：0x01020EE8。
std::uint32_t gRenderQueueMode103CF10 = 0; // 原全局：0x0103CF10。
std::int32_t gGraphicPaletteUploadCount = 0; // 原全局：0x01020ECC。
volatile std::uint32_t gRippleIntegerPhaseBits = 0; // 原 0x1020EEC。
float gFloat1020EF0 = 0.0f; // 原全局：0x01020EF0。
volatile std::uint32_t gRipplePreviousWorldXBits = 0; // 原 0x1020EF4。
volatile std::uint32_t gRipplePreviousWorldYBits = 0; // 原 0x1020EF8。
volatile std::uint32_t gRippleStrengthBits = 0; // 原 0x1020EFC。
volatile std::int32_t gRippleMapFloorCache = 0; // 原 0x1020F00。

const std::array<std::int32_t, 64> gRippleSineTable { // 原全局：0x0058B6E0。
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 11, 11, 12,
    12, 12, 11, 11, 11, 10, 9, 9, 8, 7, 6, 5, 4, 3, 2, 1,
    0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -10, -11, -11, -11, -12,
    -12, -12, -11, -11, -11, -10, -9, -9, -8, -7, -6, -5, -4, -3, -2, -1,
};

// 原函数：0x00507090。
void DrawMatchingGraphicHover_507090(const std::int32_t value4)
{
    std::int32_t index = 0;
    std::int16_t count =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (count <= 0) {
        return;
    }

    // 原平行主记录/层槽遍历，不经排序槽cacheIndex回查主记录。
    const GraphicCache3* layerRecord = gLegacyPendingStorage.cache3.data();
    const GraphicCacheItem32* record = gLegacyPendingStorage.records.data();
    do {
        const std::int32_t itemValue4 = record->value4;
        const std::int32_t requestedValue4 = value4;
        if (itemValue4 == requestedValue4) {
            const std::uint8_t layer = layerRecord->layer;
            const std::int32_t imageNo = record->imageNo;
            const std::uint8_t hoverLayer =
                layer < 0xD4 ? std::uint8_t{0xCE} : std::uint8_t{0xDA};
            const DxGraphicEntry* const dxBase = gGraphicDxCache103CFA4;
            const std::uint32_t hover = login::detail::gValueD69768;
            const DxGraphicEntry* const entry = dxBase + imageNo;
            const std::int32_t y = record->y;
            const std::int32_t x = record->x;
            const std::int16_t height =
                std::bit_cast<std::int16_t>(entry->height);
            const std::int16_t width =
                std::bit_cast<std::int16_t>(entry->width);

            // 原50710A..507117的正常S32计算序，U16只在既有调用边界取得。
            const std::int32_t bottom = y + height + 2;
            const std::int32_t top = y - 2;
            const std::int32_t right = width + x + 2;
            const std::int32_t left = x - 2;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left),
                static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(right),
                static_cast<std::uint16_t>(bottom),
                hoverLayer,
                hover,
                0);
        }

        // 绘框会追加同一队列；先重读计数，再按原序推进两条native游标。
        count = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        ++index;
        ++record;
        ++layerRecord;
    } while (index < count);
}

// 原函数：0x00523140。
std::uint8_t LoadGraphicIndependentPalette_523140(GraphicCacheItem32* const graphic)
{
    std::int16_t index;
    std::memcpy(&index, graphic->descriptor.data() + 0x0E, sizeof(index));
    const std::uint8_t result = resource::RefreshIndependentPalette32_523140(index);
    if (result == 2) {
        std::memcpy(graphic->descriptor.data() + 0x0E, &index, sizeof(index));
    }
    return result;
}

// 原函数：0x005208F0。
std::uint32_t BackupValues_5208F0()
{
    const std::uint32_t currentXBits = world::gWorldCurrentXBits;
    const std::uint32_t currentYBits = world::gWorldCurrentYBits;
    gRipplePreviousWorldXBits = currentXBits;
    gRipplePreviousWorldYBits = currentYBits;
    return currentXBits;
}

// 原函数：0x00520910。
void Sub_520910()
{
    gFloat1020EF0 += 0.25f;
}

// 原函数：0x00520930。
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

// 原函数：0x00521AA0。
std::int32_t QueueMap_521AA0(
    const std::int32_t x, const std::int32_t y, const std::uint8_t layer)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    if (initialCount >= MAX_QUEUE_SIZE) {
        return -2;
    }

    GraphicCacheItem32* const record =
        gLegacyPendingStorage.records.data() + initialCount;
    GraphicCache3* const cache3 =
        gLegacyPendingStorage.cache3.data() + initialCount;
    cache3->layer = layer;
    cache3->cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    const std::int32_t itemX = x;
    const std::int32_t itemY = y;
    record->x = itemX;
    record->y = itemY;
    record->imageNo = -1;
    record->value4 = 0;
    record->reference.action = nullptr;
    record->batchType = 5;
    const std::uint16_t resultBits = platform::gPendingGraphicCount;
    const auto result = static_cast<std::int32_t>(std::bit_cast<std::int16_t>(resultBits));
    platform::gPendingGraphicCount = static_cast<std::uint16_t>(resultBits + 1u);
    return result;
}

// 原函数：0x00522210。
void Sub_522210(
    const GraphicCacheItem32* const graphic,
    const std::uint32_t imageNoBits)
{
    const std::uint32_t xBits = std::bit_cast<std::uint32_t>(graphic->x);
    const std::uint32_t yBits = std::bit_cast<std::uint32_t>(graphic->y);
    RECT rect;
    rect.right = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(xBits));
    rect.bottom = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(yBits));
    rect.left = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(xBits >> 16));
    rect.top = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(yBits >> 16));

    const std::uint16_t color = static_cast<std::uint16_t>(imageNoBits);
    if ((imageNoBits & 0x20000000u) != 0) {
        Sub_451350(&rect, color, 0);
    } else if ((imageNoBits & 0x10000000u) != 0) {
        Sub_451350(&rect, color, 1);
    } else if ((imageNoBits & 0x40000000u) != 0) {
        Sub_451350(&rect, color, 2);
    }
}

// 原函数：0x00520AD0。
void FillDirtyArea_520AD0(const GraphicCacheItem32* const graphic)
{
    const std::uint8_t mixMode = graphic->descriptor[0x0D];
    std::uint32_t color;
    std::memcpy(&color, graphic->descriptor.data() + 0x08, sizeof(color));

    // 原520ADB/520AE0/520AE5/520AEA：同一记录的四次W16读取，不能合并成W32。
    std::int16_t bottom;
    std::memcpy(
        &bottom,
        reinterpret_cast<const std::uint8_t*>(&graphic->y) + sizeof(bottom),
        sizeof(bottom));
    std::int16_t top;
    std::memcpy(&top, &graphic->y, sizeof(top));
    std::int16_t right;
    std::memcpy(
        &right,
        reinterpret_cast<const std::uint8_t*>(&graphic->x) + sizeof(right),
        sizeof(right));
    std::int16_t left;
    std::memcpy(&left, &graphic->x, sizeof(left));

    if (FillRect_44C9A0(left, right, top, bottom, color, mixMode)
        == 0x887601AEu) {
        gGraphicDirty = 1;
    }
}

// 原函数：0x00521870。
std::int32_t Sub_521870()
{
    // 原四个W32栈矩形字段先发布；只原有字段写入，不整块清零。
    std::array<std::int32_t, 4> sourceRectangle;
    sourceRectangle[0] = 0;
    sourceRectangle[1] = 0;
    sourceRectangle[2] = kCpuSurfaceWidth;
    sourceRectangle[3] = kCpuSurfaceHeight;

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

    static_cast<void>(CopyBackToTemp());

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
    std::int32_t strip = 0;
    sourceRectangle[2] = kCpuSurfaceWidth;
    gRippleIntegerPhaseBits = std::bit_cast<std::uint32_t>(integerPhase);
    sourceRectangle[0] = 0;
    std::int32_t destinationY = 0;

    do {
        const std::uint32_t phaseBits = gRippleIntegerPhaseBits;
        const std::int32_t phaseValue = std::bit_cast<std::int32_t>(phaseBits);
        const std::int32_t tableIndex = (phaseValue + strip) & 63;
        const std::int32_t tableValue =
            gRippleSineTable[static_cast<std::size_t>(tableIndex)];
        const std::uint32_t strengthBits = gRippleStrengthBits;
        const std::int32_t strength = std::bit_cast<std::int32_t>(strengthBits);
        const std::int32_t product = tableValue * strength;
        const std::int32_t offset = product / 100;

        // 原每次提交完整8行；不把矩形预裁成部分条带或跳过失败提交。
        const std::int32_t sourceTop = offset + destinationY + 12;
        sourceRectangle[1] = sourceTop;
        const std::int32_t sourceBottom = sourceTop + 8;
        sourceRectangle[3] = sourceBottom;
        TextureSurface* const temp = BorrowCpuTempSurface();
        static_cast<void>(BlitSurfaceRectangleToBack(
            0, destinationY, sourceRectangle, temp));
        destinationY += 8;
        ++strip;
    } while (destinationY < kCpuSurfaceHeight);

    const std::uint32_t strengthBits = gRippleStrengthBits;
    const std::int32_t strength = std::bit_cast<std::int32_t>(strengthBits);
    if (strength < 100) {
        const std::int32_t updatedStrength = strength + 2;
        gRippleStrengthBits = std::bit_cast<std::uint32_t>(updatedStrength);
        return updatedStrength;
    }
    return strength;
}

// 原函数：0x00520790。
void SortTextures_520790()
{
    const bool hasSpecialGraphic = gSpecialGraphicPending != 0;
    const std::int32_t reserveCount = hasSpecialGraphic ? 4 : 3;
    std::uint16_t countBits = platform::gPendingGraphicCount;
    const std::int16_t initialCount = std::bit_cast<std::int16_t>(countBits);
    if (static_cast<std::int32_t>(initialCount) + reserveCount
        >= MAX_QUEUE_SIZE) {
        countBits = static_cast<std::uint16_t>(MAX_QUEUE_SIZE - reserveCount);
        platform::gPendingGraphicCount = countBits;
    }

    if (hasSpecialGraphic) {
        countBits = AppendReservedGraphic(countBits, 2, 8);
    }
    countBits = AppendReservedGraphic(countBits, 5, 6);
    countBits = AppendReservedGraphic(countBits, 6, 7);
    countBits = AppendReservedGraphic(countBits, 20, 9);

    const std::int32_t sortCount = std::bit_cast<std::int16_t>(countBits);
    std::qsort(
        gLegacyPendingStorage.cache3.data(),
        static_cast<std::uint32_t>(sortCount),
        sizeof(GraphicCache3),
        CompareGraphicCache3_5222A0);
}

} // namespace cg::render

namespace cg::platform {

char (&gTextFF4E30)[0x88] = render::gLegacyPendingStorage.prefix; // 原全局：0x00FF4E30。

} // namespace cg::platform

namespace cg::login::detail {

void SetPendingGraphicBatchTypeAtLegacyIndex(
    const std::int32_t index,
    const std::uint8_t type)
{
    if (index == -2) {
        // 原 records[-2].batchType 落到 prefix+0x5C；不能按新端 48B 步长推算。
        render::gLegacyPendingStorage.prefix[0x5C] = std::bit_cast<char>(type);
        return;
    }

    render::GraphicCacheItem32* const record =
        render::gLegacyPendingStorage.records.data() + index;
    record->batchType = type;
}

} // namespace cg::login::detail
