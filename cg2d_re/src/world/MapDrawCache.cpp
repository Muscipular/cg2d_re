#include "WorldUpdateDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

namespace cg::world::detail {
namespace {

// 原 52B 记录的前 44B 不含地址；只有两条挂件链拓宽，均不拥有节点。
struct MapDrawRecord {
    std::int32_t imageNo;
    std::int32_t sourceIndex;
    std::uint32_t value08;
    std::uint32_t worldXBits;
    std::uint32_t worldYBits;
    std::int16_t x;
    std::int16_t y;
    std::uint8_t batchType;
    std::array<std::uint8_t, 3> unwritten19;
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

constexpr std::int32_t kSourceCapacity = 1024;
constexpr std::int32_t kMergedCapacity = 3072;
std::array<MapDrawRecord, kSourceCapacity> gMapDrawPrimary9EE918 {};
std::array<MapDrawRecord, kSourceCapacity> gMapDrawAttachments9E0438 {};
std::array<MapDrawRecord, kSourceCapacity> gMapDrawObjectsC751D0 {};
std::array<MapDrawRecord, kSourceCapacity> gMapDrawSpecialC28008 {};
std::array<MapDrawRecord, kMergedCapacity> gMapDrawMerged9A86E8 {};
std::int32_t gMapDrawMergedCount9EE914 = 0;

// 各生产者共同的逐字段默认值/逐 W32 复制，不清记录的其他字段。
void WriteMapDrawDescriptor(
    render::GraphicDescriptor16& destination, const render::GraphicDescriptor16* const source)
{
    if (source != nullptr) {
        for (std::size_t offset = 0; offset < 16; offset += 4) {
            std::uint32_t value;
            std::memcpy(&value, source->bytes.data() + offset, sizeof(value));
            std::memcpy(destination.bytes.data() + offset, &value, sizeof(value));
        }
        return;
    }
    const std::uint16_t zero = 0;
    for (std::size_t offset = 0; offset < 8; offset += 2) {
        std::memcpy(destination.bytes.data() + offset, &zero, sizeof(zero));
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(destination.bytes.data() + 8, &color, sizeof(color));
    destination.bytes[12] = 0;
    destination.bytes[13] = 0;
    std::memcpy(destination.bytes.data() + 14, &zero, sizeof(zero));
}

// 原46C4D0六段复制：前44B逐W32读写，再按原序复制两条非拥有链接。
void CopyMapDrawRecord(MapDrawRecord* const destination, const MapDrawRecord* const source)
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
std::int32_t CompareMapObject_46C300(
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

// 原 46C440：差值保持 F64，比较器从不返回 0，不改成严格弱序。
std::int32_t CompareMapPrimary_46C440(
    const MapDrawRecord* const first, const MapDrawRecord* const second)
{
    const double firstDepth = static_cast<double>(std::bit_cast<float>(first->worldXBits))
        - std::bit_cast<float>(first->worldYBits);
    const double secondDepth = static_cast<double>(std::bit_cast<float>(second->worldXBits))
        - std::bit_cast<float>(second->worldYBits);
    if (!(firstDepth >= secondDepth)) {
        return -1;
    }
    if (firstDepth > secondDepth) {
        return 1;
    }
    const float firstX = std::bit_cast<float>(first->worldXBits);
    const float secondX = std::bit_cast<float>(second->worldXBits);
    if (firstX == secondX || std::isunordered(firstX, secondX)) {
        const float firstY = std::bit_cast<float>(first->worldYBits);
        const float secondY = std::bit_cast<float>(second->worldYBits);
        if (firstY == secondY || std::isunordered(firstY, secondY)) {
            if (second->batchType != 0) {
                return 1;
            }
            if (first->batchType != 0) {
                return -1;
            }
            const std::int32_t firstIndex = first->sourceIndex;
            const std::int32_t secondIndex = second->sourceIndex;
            return firstIndex > secondIndex ? 1 : -1;
        }
    }
    const std::int32_t firstIndex = first->sourceIndex;
    const std::int32_t secondIndex = second->sourceIndex;
    return firstIndex > secondIndex ? 1 : -1;
}

// 原 46C420 调用的 53C112/53C266 算法，仅承接本表；不交给另一 CRT 改变比较次序。
void SortMapPrimary_46C420()
{
    const std::uint32_t count = gValue9FB934;
    if (count < 2) {
        return;
    }
    MapDrawRecord* first = gMapDrawPrimary9EE918.data();
    MapDrawRecord* last = first + count - 1;
    std::array<MapDrawRecord*, 30> firstStack;
    std::array<MapDrawRecord*, 30> lastStack;
    std::int32_t stackSize = 0;
    for (;;) {
        const auto length = last - first + 1;
        if (length <= 8) {
            while (last > first) {
                MapDrawRecord* maximum = first;
                for (MapDrawRecord* item = first + 1; item <= last; ++item) {
                    if (CompareMapPrimary_46C440(item, maximum) > 0) {
                        maximum = item;
                    }
                }
                if (maximum != last) {
                    std::swap(*maximum, *last);
                }
                --last;
            }
        } else {
            std::swap(*(first + length / 2), *first);
            MapDrawRecord* left = first;
            MapDrawRecord* right = last + 1;
            for (;;) {
                do {
                    ++left;
                } while (left <= last && CompareMapPrimary_46C440(left, first) <= 0);
                do {
                    --right;
                } while (right > first && CompareMapPrimary_46C440(right, first) >= 0);
                if (right < left) {
                    break;
                }
                if (left != right) {
                    std::swap(*left, *right);
                }
            }
            if (first != right) {
                std::swap(*first, *right);
            }
            // 原比较 (right-first 的字节数)-1 >= (last-left 的字节数)。
            if (right - first > last - left) {
                if (first + 1 < right) {
                    firstStack[stackSize] = first;
                    lastStack[stackSize] = right - 1;
                    ++stackSize;
                }
                if (left < last) {
                    first = left;
                    continue;
                }
            } else {
                if (left < last) {
                    firstStack[stackSize] = left;
                    lastStack[stackSize] = last;
                    ++stackSize;
                }
                if (first + 1 < right) {
                    last = right - 1;
                    continue;
                }
            }
        }
        if (--stackSize < 0) {
            return;
        }
        first = firstStack[stackSize];
        last = lastStack[stackSize];
    }
}

// 原 46C4D0：计数在各复制段的发布时点不同，不合并成统一 append。
void MergeMapDrawCache_46C4D0()
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
void QueueMapDrawRecord(
    const MapDrawRecord* const record, const MapDrawRecord* const primary, const bool attachment)
{
    const bool clipped = primary->descriptor.bytes[12] == 1;
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

} // namespace

std::int32_t Sub_46BD60(
    const std::int32_t imageNo, const std::uint32_t worldXBits, const std::uint32_t worldYBits,
    const std::int16_t x, const std::int16_t y, const std::int32_t mode,
    const render::GraphicDescriptor16* const descriptor)
{
    const auto count = std::bit_cast<std::int32_t>(gValue9FB934);
    if (count >= kSourceCapacity) {
        return -1;
    }
    MapDrawRecord& record = gMapDrawPrimary9EE918[count];
    record.imageNo = imageNo;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.batchType = mode != 0 ? 1 : 0;
    record.sourceIndex = count;
    WriteMapDrawDescriptor(record.descriptor, descriptor);
    record.before = nullptr;
    record.after = nullptr;
    gValue9FB934 = static_cast<std::uint32_t>(count + 1);
    return count;
}

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

void Sub_46DFF0(
    const std::int32_t imageNo, const std::uint32_t worldXBits, const std::uint32_t worldYBits,
    const std::int16_t x, const std::int16_t y, const render::GraphicDescriptor16* const descriptor)
{
    const auto count = std::bit_cast<std::int32_t>(gValue9FB928);
    if (count >= kSourceCapacity) {
        return;
    }
    MapDrawRecord& record = gMapDrawObjectsC751D0[count];
    record.imageNo = imageNo;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.sourceIndex = count;
    record.batchType = 0;
    if (descriptor != nullptr) {
        for (std::size_t offset = 0; offset < 12; offset += 4) {
            std::uint32_t value;
            std::memcpy(&value, descriptor->bytes.data() + offset, sizeof(value));
            std::memcpy(record.descriptor.bytes.data() + offset, &value, sizeof(value));
        }
        std::uint32_t tail;
        std::memcpy(&tail, descriptor->bytes.data() + 12, sizeof(tail));
        gValue9FB928 = static_cast<std::uint32_t>(count + 1);
        std::memcpy(record.descriptor.bytes.data() + 12, &tail, sizeof(tail));
    } else {
        WriteMapDrawDescriptor(record.descriptor, nullptr);
        gValue9FB928 = static_cast<std::uint32_t>(count + 1);
    }
}

void DrawGearNormal_46C060(
    const std::int32_t primaryIndex, const std::int32_t before,
    const std::int32_t imageNo, const std::int16_t x, const std::int16_t y,
    const render::GraphicDescriptor16* const descriptor, const std::uint8_t batchType)
{
    if (primaryIndex < 0) {
        return;
    }
    const auto initialCount = std::bit_cast<std::int32_t>(gValue9A7F78);
    if (initialCount >= kSourceCapacity) {
        return;
    }
    MapDrawRecord* tail = gMapDrawPrimary9EE918.data() + primaryIndex;
    if (before != 0) {
        while (tail->before != nullptr) {
            tail = tail->before;
        }
        tail->before = gMapDrawAttachments9E0438.data() + initialCount;
    } else {
        while (tail->after != nullptr) {
            tail = tail->after;
        }
        tail->after = gMapDrawAttachments9E0438.data() + initialCount;
    }
    const auto count = std::bit_cast<std::int32_t>(gValue9A7F78);
    MapDrawRecord& record = gMapDrawAttachments9E0438[count];
    record.imageNo = imageNo;
    const std::uint32_t worldXBits = gMapDrawPrimary9EE918[primaryIndex].worldXBits;
    const std::uint32_t worldYBits = gMapDrawPrimary9EE918[primaryIndex].worldYBits;
    record.worldXBits = worldXBits;
    record.worldYBits = worldYBits;
    record.x = x;
    record.y = y;
    record.batchType = batchType;
    record.sourceIndex = primaryIndex;
    WriteMapDrawDescriptor(record.descriptor, descriptor);
    record.before = nullptr;
    record.after = nullptr;
    gValue9A7F78 = static_cast<std::uint32_t>(count + 1);
}

void Sub_46C2F0()
{
    SortMapPrimary_46C420();
    MergeMapDrawCache_46C4D0();
}

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
