
#include "cg/resource/GraphicInfo.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "WorldMapState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

namespace cg::world::detail {

// 原 52B 记录的前 44B 不含地址；只有两条挂件链拓宽，均不拥有节点。
struct MapDrawRecord {
    std::int32_t imageNo;
    std::int32_t sourceIndex;
    std::uint32_t value08; // 原成员偏移：+0x08。
    std::uint32_t worldXBits;
    std::uint32_t worldYBits;
    std::int16_t x;
    std::int16_t y;
    std::uint8_t batchType;
    std::array<std::uint8_t, 3> unwritten19; // 原成员偏移：+0x19。
    render::GraphicDescriptor16 descriptor;
    MapDrawRecord* before;
    MapDrawRecord* after;
};

static_assert(std::is_standard_layout_v<MapDrawRecord>);
static_assert(std::is_trivially_copyable_v<MapDrawRecord>);
static_assert(offsetof(MapDrawRecord, imageNo) == 0x00);
static_assert(offsetof(MapDrawRecord, sourceIndex) == 0x04);
static_assert(offsetof(MapDrawRecord, value08) == 0x08);
static_assert(offsetof(MapDrawRecord, worldXBits) == 0x0C);
static_assert(offsetof(MapDrawRecord, worldYBits) == 0x10);
static_assert(offsetof(MapDrawRecord, x) == 0x14);
static_assert(offsetof(MapDrawRecord, y) == 0x16);
static_assert(offsetof(MapDrawRecord, batchType) == 0x18);
static_assert(offsetof(MapDrawRecord, unwritten19) == 0x19);
static_assert(offsetof(MapDrawRecord, descriptor) == 0x1C);
static_assert(offsetof(MapDrawRecord, before) == 0x30);
static_assert(offsetof(MapDrawRecord, after) == 0x38);
static_assert(sizeof(MapDrawRecord) == 0x40);
static_assert(alignof(MapDrawRecord) == 8);

static constexpr std::int32_t kSourceCapacity = 1024;
static constexpr std::int32_t kMergedCapacity = 3072;
static std::array<MapDrawRecord, kSourceCapacity> gMapDrawPrimary9EE918 {}; // 原全局：0x009EE918。
static std::array<MapDrawRecord, kSourceCapacity> gMapDrawAttachments9E0438 {}; // 原全局：0x009E0438。
static std::array<MapDrawRecord, kSourceCapacity> gMapDrawObjectsC751D0 {}; // 原全局：0x00C751D0。
static std::array<MapDrawRecord, kSourceCapacity> gMapDrawSpecialC28008 {}; // 原全局：0x00C28008。
static std::array<MapDrawRecord, kMergedCapacity> gMapDrawMerged9A86E8 {}; // 原全局：0x009A86E8。
static std::int32_t gMapDrawMergedCount9EE914 = 0; // 原全局：0x009EE914。

// 各生产者共用完整描述符及空参默认值，不清记录的其他字段。
static void WriteMapDrawDescriptor(
    render::GraphicDescriptor16& destination, const render::GraphicDescriptor16* const source)
{
    if (source != nullptr) {
        destination = *source;
        return;
    }
    destination = {0, 0, 0, 0, 0xFFFFFFFFu, 0, render::GraphicMixFlag_None, 0};
}

// 原46C4D0六段复制：前44B逐W32读写，再按原序复制两条非拥有链接。
static void CopyMapDrawRecord(MapDrawRecord* const destination, const MapDrawRecord* const source)
{
    auto* const destinationBytes = reinterpret_cast<std::byte*>(destination);
    const auto* const sourceBytes = reinterpret_cast<const std::byte*>(source);
    for (std::size_t offset = 0; offset < 0x2C; offset += sizeof(std::uint32_t)) {
        std::uint32_t value;
        std::memcpy(&value, sourceBytes + offset, sizeof(value));
        std::memcpy(destinationBytes + offset, &value, sizeof(value));
    }
    MapDrawRecord* const before = source->before;
    destination->before = before;
    MapDrawRecord* const after = source->after;
    destination->after = after;
}

// 原 46C300：两项深度落 F32；图形查询之后按原 x87 门禁及 S16 屏幕阈值比较。
static std::int32_t CompareMapObject_46C300(
    const MapDrawRecord* const primary, const MapDrawRecord* const object)
{
    const float primaryDepth = static_cast<float>(
        static_cast<double>(std::bit_cast<float>(primary->worldXBits))
        - std::bit_cast<float>(primary->worldYBits));
    const float objectDepth = static_cast<float>(
        static_cast<double>(std::bit_cast<float>(object->worldXBits))
        - std::bit_cast<float>(object->worldYBits));
    const std::int32_t sizeImageNo = object->imageNo;
    std::uint16_t sx;
    std::uint16_t sy;
    static_cast<void>(resource::GetGraphicSxSy(
        std::bit_cast<std::uint32_t>(sizeImageNo), &sx, &sy));
    const std::int32_t mapNo = resource::GetGraphicMapNo(object->imageNo);
    if (mapNo == 1 || (sx == 1 && sy == 1)) {
        return primaryDepth > objectDepth ? 1 : 0;
    }

    const float primaryX = std::bit_cast<float>(primary->worldXBits);
    const float objectX = std::bit_cast<float>(object->worldXBits);
    if (primaryX > objectX) {
        const float primaryY = std::bit_cast<float>(primary->worldYBits);
        const float objectY = std::bit_cast<float>(object->worldYBits);
        if (!(primaryY >= objectY)) {
            return 1;
        }
    }
    const std::int16_t primaryScreenX = primary->x;
    const std::int16_t objectScreenX = object->x;
    if (primaryScreenX > objectScreenX) {
        const std::int32_t offset = (48 - 48 * std::bit_cast<std::int16_t>(sx)) / 2;
        const std::int32_t primaryY = primary->y;
        const std::int32_t objectY = object->y;
        return offset + objectY > primaryY ? 1 : 0;
    }
    if (primaryScreenX < objectScreenX) {
        const std::int32_t objectY = object->y;
        const std::int32_t offset = (48 - 48 * std::bit_cast<std::int16_t>(sy)) / 2;
        const std::int32_t primaryY = primary->y;
        return offset + objectY > primaryY ? 1 : 0;
    }
    const std::int16_t objectY = object->y;
    const std::int16_t primaryY = primary->y;
    return objectY > primaryY ? 1 : 0;
}

// MC01-C8：原F64深度主键；同深度统一批次优先，再按sourceIndex，同键返回0。
// 原函数：0x0046C440。
static std::int32_t CompareMapPrimary_46C440(
    const MapDrawRecord* const first, const MapDrawRecord* const second)
{
    const double firstDepth = static_cast<double>(std::bit_cast<float>(first->worldXBits))
        - std::bit_cast<float>(first->worldYBits);
    const double secondDepth = static_cast<double>(std::bit_cast<float>(second->worldXBits))
        - std::bit_cast<float>(second->worldYBits);
    if (firstDepth < secondDepth) {
        return -1;
    }
    if (firstDepth > secondDepth) {
        return 1;
    }
    const bool firstBatch = first->batchType != 0;
    const bool secondBatch = second->batchType != 0;
    if (firstBatch != secondBatch) {
        return firstBatch ? -1 : 1;
    }
    return (first->sourceIndex > second->sourceIndex)
        - (first->sourceIndex < second->sourceIndex);
}

// MC01-C8：原数组/数量接std::sort，比较交换和等价项排列采用标准库。
// 原函数：0x0046C420。
static void SortMapPrimary_46C420()
{
    const std::uint32_t count = gValue9FB934;
    if (count < 2) {
        return;
    }
    std::sort(gMapDrawPrimary9EE918.begin(), gMapDrawPrimary9EE918.begin() + count,
        [](const MapDrawRecord& left, const MapDrawRecord& right) {
            return CompareMapPrimary_46C440(&left, &right) < 0;
        });
}

// 原 46C4D0：计数在各复制段的发布时点不同，不合并成统一 append。
static void MergeMapDrawCache_46C4D0()
{
    std::int32_t objectLimit = std::bit_cast<std::int32_t>(gValue9FB928);
    std::int32_t merged = 0;
    std::int32_t objectIndex = 0;
    std::int32_t primaryIndex = 0;
    gMapDrawMergedCount9EE914 = 0;
    while (objectIndex < objectLimit) {
        if (primaryIndex < std::bit_cast<std::int32_t>(gValue9FB934)) {
            const MapDrawRecord* const object = gMapDrawObjectsC751D0.data() + objectIndex;
            bool objectFirst = false;
            for (;;) {
                const MapDrawRecord* primary = gMapDrawPrimary9EE918.data() + primaryIndex;
                if (CompareMapObject_46C300(primary, object) != 0) {
                    objectFirst = true;
                    break;
                }
                if (primaryIndex < std::bit_cast<std::int32_t>(gValue9FB934)) {
                    for (;;) {
                        const std::int32_t mapNo = resource::GetGraphicMapNo(primary->imageNo);
                        merged = gMapDrawMergedCount9EE914;
                        if (mapNo != 1) {
                            MapDrawRecord* const destination = gMapDrawMerged9A86E8.data() + merged;
                            gMapDrawMergedCount9EE914 = ++merged;
                            CopyMapDrawRecord(destination, primary);
                        }
                        const float currentX = std::bit_cast<float>(primary->worldXBits);
                        const float nextX = std::bit_cast<float>((primary + 1)->worldXBits);
                        if (currentX != nextX && !std::isunordered(currentX, nextX)) {
                            break;
                        }
                        const float currentY = std::bit_cast<float>(primary->worldYBits);
                        const float nextY = std::bit_cast<float>((primary + 1)->worldYBits);
                        if (currentY != nextY && !std::isunordered(currentY, nextY)) {
                            break;
                        }
                        const std::int32_t limit = std::bit_cast<std::int32_t>(gValue9FB934);
                        ++primaryIndex;
                        ++primary;
                        if (primaryIndex >= limit) {
                            break;
                        }
                    }
                } else {
                    merged = gMapDrawMergedCount9EE914;
                }
                const std::int32_t limit = std::bit_cast<std::int32_t>(gValue9FB934);
                ++primaryIndex;
                if (primaryIndex >= limit) {
                    break;
                }
            }
            if (objectFirst) {
                merged = gMapDrawMergedCount9EE914;
                MapDrawRecord* const destination = gMapDrawMerged9A86E8.data() + merged;
                objectLimit = std::bit_cast<std::int32_t>(gValue9FB928);
                ++merged;
                ++objectIndex;
                gMapDrawMergedCount9EE914 = merged;
                CopyMapDrawRecord(destination, object);
                if (objectIndex < objectLimit) {
                    const MapDrawRecord* const primary =
                        gMapDrawPrimary9EE918.data() + primaryIndex;
                    const MapDrawRecord* nextObject = gMapDrawObjectsC751D0.data() + objectIndex;
                    for (;;) {
                        if (CompareMapObject_46C300(primary, nextObject) == 0) {
                            merged = gMapDrawMergedCount9EE914;
                            --objectIndex;
                            break;
                        }
                        merged = gMapDrawMergedCount9EE914;
                        MapDrawRecord* const nextDestination =
                            gMapDrawMerged9A86E8.data() + merged;
                        ++merged;
                        ++objectIndex;
                        objectLimit = std::bit_cast<std::int32_t>(gValue9FB928);
                        CopyMapDrawRecord(nextDestination, nextObject);
                        gMapDrawMergedCount9EE914 = merged;
                        ++nextObject;
                        if (objectIndex >= objectLimit) {
                            break;
                        }
                    }
                }
            }
        }
        if (primaryIndex >= std::bit_cast<std::int32_t>(gValue9FB934)) {
            objectLimit = std::bit_cast<std::int32_t>(gValue9FB928);
            if (objectIndex < objectLimit) {
                const std::int32_t remaining = objectLimit - objectIndex;
                MapDrawRecord* const destination = gMapDrawMerged9A86E8.data() + merged;
                const MapDrawRecord* const source = gMapDrawObjectsC751D0.data() + objectIndex;
                objectIndex += remaining;
                merged += remaining;
                for (std::int32_t recordIndex = 0; recordIndex < remaining; ++recordIndex) {
                    CopyMapDrawRecord(destination + recordIndex, source + recordIndex);
                }
                gMapDrawMergedCount9EE914 = merged;
            }
        }
        objectLimit = std::bit_cast<std::int32_t>(gValue9FB928);
        ++objectIndex;
    }
    if (primaryIndex < std::bit_cast<std::int32_t>(gValue9FB934)) {
        const MapDrawRecord* primary = gMapDrawPrimary9EE918.data() + primaryIndex;
        for (;;) {
            const std::int32_t mapNo = resource::GetGraphicMapNo(primary->imageNo);
            merged = gMapDrawMergedCount9EE914;
            if (mapNo != 1) {
                MapDrawRecord* const destination = gMapDrawMerged9A86E8.data() + merged;
                gMapDrawMergedCount9EE914 = ++merged;
                CopyMapDrawRecord(destination, primary);
            }
            const std::int32_t limit = std::bit_cast<std::int32_t>(gValue9FB934);
            ++primaryIndex;
            ++primary;
            if (primaryIndex >= limit) {
                break;
            }
        }
    }
    if (merged < kMergedCapacity) {
        std::int32_t specialIndex = 0;
        const MapDrawRecord* source = gMapDrawSpecialC28008.data();
        MapDrawRecord* destination = gMapDrawMerged9A86E8.data() + merged;
        do {
            if (specialIndex >= std::bit_cast<std::int32_t>(gValue9A86C0)) {
                break;
            }
            MapDrawRecord* const currentDestination = destination;
            const MapDrawRecord* const currentSource = source;
            ++merged;
            ++destination;
            ++specialIndex;
            ++source;
            CopyMapDrawRecord(currentDestination, currentSource);
        } while (destination < gMapDrawMerged9A86E8.data() + kMergedCapacity);
        gMapDrawMergedCount9EE914 = merged;
    }
}

// 原 46C1C0 的三处提交；分流始终读取主项 clip，挂件不能覆盖此选择。
static void QueueMapDrawRecord(
    const MapDrawRecord* const record, const MapDrawRecord* const primary, const bool attachment)
{
    const bool clipped = primary->descriptor.clipMode == 1;
    const std::uint8_t batch = attachment ? record->batchType : 0;
    const std::int32_t imageNo = record->imageNo;
    const std::int32_t y = record->y;
    const std::int32_t x = record->x;
    if (clipped) {
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 10, imageNo, 0, 0, batch, &record->descriptor));
    } else {
        static_cast<void>(render::DrawGraphic2_521DD0(
            x, y, 10, imageNo, 0, batch, &record->descriptor));
    }
}

// 原函数：0x0046BE50。
void Sub_46BE50(
    const std::int32_t imageNo, const std::uint32_t worldXBits, const std::uint32_t worldYBits,
    const std::int16_t x, const std::int16_t y, const render::GraphicDescriptor16* const descriptor)
{
    const std::int32_t mapNo = resource::GetGraphicMapNo(imageNo);
    if (mapNo == 1) {
        static_cast<void>(Sub_46BD60(imageNo, worldXBits, worldYBits, x, y, 0, nullptr));
        return;
    }
    if (mapNo <= 999) {
        return;
    }
    std::uint16_t flag;
    static_cast<void>(resource::GetGraphicFlag2(std::bit_cast<std::uint32_t>(imageNo), &flag));
    std::uint32_t& countSlot = flag == 3 ? gValue9A86C0 : gValue9FB928;
    const auto count = std::bit_cast<std::int32_t>(countSlot);
    if (count >= kSourceCapacity) {
        return;
    }
    MapDrawRecord& record = flag == 3 ? gMapDrawSpecialC28008[count] : gMapDrawObjectsC751D0[count];
    record.imageNo = imageNo;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.sourceIndex = count;
    record.batchType = 0;
    WriteMapDrawDescriptor(record.descriptor, descriptor);
    record.before = nullptr;
    record.after = nullptr;
    countSlot = static_cast<std::uint32_t>(count + 1);
}

// 原函数：0x0046C2F0。
void Sub_46C2F0()
{
    SortMapPrimary_46C420();
    MergeMapDrawCache_46C4D0();
}

// 原函数：0x0046C1C0。
std::int32_t Sub_46C1C0()
{
    std::int32_t count = gMapDrawMergedCount9EE914;
    std::int32_t index = 0;
    if (count > 0) {
        MapDrawRecord* primary = gMapDrawMerged9A86E8.data();
        do {
            for (MapDrawRecord* record = primary->before; record != nullptr; record = record->before) {
                QueueMapDrawRecord(record, primary, true);
            }
            QueueMapDrawRecord(primary, primary, false);
            for (MapDrawRecord* record = primary->after; record != nullptr; record = record->after) {
                QueueMapDrawRecord(record, primary, true);
            }
            count = gMapDrawMergedCount9EE914;
            ++index;
            ++primary;
        } while (index < count);
    }
    return count;
}

} // namespace cg::world::detail

namespace cg::world {

// 原函数：0x0046BD60。
std::int32_t Sub_46BD60(
    const std::int32_t imageNo, const std::uint32_t worldXBits, const std::uint32_t worldYBits,
    const std::int16_t x, const std::int16_t y, const std::int32_t mode,
    const render::GraphicDescriptor16* const descriptor)
{
    const auto count = std::bit_cast<std::int32_t>(detail::gValue9FB934);
    if (count >= detail::kSourceCapacity) {
        return -1;
    }
    detail::MapDrawRecord& record = detail::gMapDrawPrimary9EE918[count];
    record.imageNo = imageNo;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.batchType = mode != 0 ? 1 : 0;
    record.sourceIndex = count;
    detail::WriteMapDrawDescriptor(record.descriptor, descriptor);
    record.before = nullptr;
    record.after = nullptr;
    detail::gValue9FB934 = static_cast<std::uint32_t>(count + 1);
    return count;
}

// 原函数：0x0046DFF0。
void Sub_46DFF0(
    const std::int32_t imageNo, const std::uint32_t worldXBits, const std::uint32_t worldYBits,
    const std::int16_t x, const std::int16_t y, const render::GraphicDescriptor16* const descriptor)
{
    const auto count = std::bit_cast<std::int32_t>(detail::gValue9FB928);
    if (count >= detail::kSourceCapacity) {
        return;
    }
    detail::MapDrawRecord& record = detail::gMapDrawObjectsC751D0[count];
    record.imageNo = imageNo;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.sourceIndex = count;
    record.batchType = 0;
    if (descriptor != nullptr) {
        record.descriptor.sourceX = descriptor->sourceX;
        record.descriptor.sourceY = descriptor->sourceY;
        record.descriptor.sourceWidth = descriptor->sourceWidth;
        record.descriptor.sourceHeight = descriptor->sourceHeight;
        record.descriptor.color = descriptor->color;
        const std::uint8_t clipMode = descriptor->clipMode;
        const std::uint8_t mixFlags = descriptor->mixFlags;
        const std::int16_t paletteIndex = descriptor->paletteIndex;
        detail::gValue9FB928 = static_cast<std::uint32_t>(count + 1);
        record.descriptor.clipMode = clipMode;
        record.descriptor.mixFlags = mixFlags;
        record.descriptor.paletteIndex = paletteIndex;
    } else {
        detail::WriteMapDrawDescriptor(record.descriptor, nullptr);
        detail::gValue9FB928 = static_cast<std::uint32_t>(count + 1);
    }
}

// 原函数：0x0046C060。
void DrawGearNormal_46C060(
    const std::int32_t primaryIndex, const std::int32_t before,
    const std::int32_t imageNo, const std::int16_t x, const std::int16_t y,
    const render::GraphicDescriptor16* const descriptor, const std::uint8_t batchType)
{
    if (primaryIndex < 0) {
        return;
    }
    const auto initialCount = std::bit_cast<std::int32_t>(detail::gValue9A7F78);
    if (initialCount >= detail::kSourceCapacity) {
        return;
    }
    detail::MapDrawRecord* tail = detail::gMapDrawPrimary9EE918.data() + primaryIndex;
    if (before != 0) {
        while (tail->before != nullptr) {
            tail = tail->before;
        }
        tail->before = detail::gMapDrawAttachments9E0438.data() + initialCount;
    } else {
        while (tail->after != nullptr) {
            tail = tail->after;
        }
        tail->after = detail::gMapDrawAttachments9E0438.data() + initialCount;
    }
    const auto count = std::bit_cast<std::int32_t>(detail::gValue9A7F78);
    detail::MapDrawRecord& record = detail::gMapDrawAttachments9E0438[count];
    record.imageNo = imageNo;
    const std::uint32_t worldXBits = detail::gMapDrawPrimary9EE918[primaryIndex].worldXBits;
    const std::uint32_t worldYBits = detail::gMapDrawPrimary9EE918[primaryIndex].worldYBits;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.batchType = batchType;
    record.sourceIndex = primaryIndex;
    detail::WriteMapDrawDescriptor(record.descriptor, descriptor);
    record.before = nullptr;
    record.after = nullptr;
    detail::gValue9A7F78 = static_cast<std::uint32_t>(count + 1);
}

} // namespace cg::world
