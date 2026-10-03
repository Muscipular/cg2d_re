#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <cstring>

namespace cg::platform {

// 沿既有.data虚拟零合同；不采用IDA对未加载字节返回的FFFFFFFF。
HANDLE gLegacyHeap10869A4 = nullptr;
std::int32_t gLegacyHeapMode10869A8 = 0;
std::uint32_t gLegacySmallHeap3Capacity1086988 = 0;
LegacySmallHeap3Region* gLegacySmallHeap3Current1086990 = nullptr;
LegacySmallHeap3Region* gLegacySmallHeap3EmptyRegion1086994 = nullptr;
std::uint32_t gLegacySmallHeap3Count1086998 = 0;
LegacySmallHeap3Region* gLegacySmallHeap3Regions108699C = nullptr;
std::uint32_t gLegacySmallHeap3Threshold10869A0 = 0;

std::int32_t InitializeLegacyHeap_541BB8(const std::int32_t serialized)
{
    const HANDLE heap = ::HeapCreate(serialized == 0 ? HEAP_NO_SERIALIZE : 0, 0x1000, 0);
    gLegacyHeap10869A4 = heap;
    if (heap == nullptr) {
        return 0;
    }
    const std::int32_t mode = SelectLegacyHeapMode_541A70();
    gLegacyHeapMode10869A8 = mode;
    if (mode == 3) {
        if (InitializeLegacySmallHeap3_541C15(0x3F8) != 0) {
            return 1;
        }
    } else if (mode == 2) {
        if (CreateLegacySmallHeap2Region_54275C() != nullptr) {
            return 1;
        }
    } else {
        return 1;
    }
    // 原541C02子初始化后重读共享句柄；销毁后不清槽。
    static_cast<void>(::HeapDestroy(gLegacyHeap10869A4));
    return 0;
}

std::int32_t InitializeLegacySmallHeap3_541C15(const std::uint32_t threshold)
{
    auto* const regions = static_cast<LegacySmallHeap3Region*>(
        ::HeapAlloc(gLegacyHeap10869A4, 0, 16 * sizeof(LegacySmallHeap3Region)));
    gLegacySmallHeap3Regions108699C = regions;
    if (regions == nullptr) {
        return 0;
    }
    gLegacySmallHeap3EmptyRegion1086994 = nullptr;
    gLegacySmallHeap3Count1086998 = 0;
    gLegacySmallHeap3Current1086990 = regions;
    gLegacySmallHeap3Threshold10869A0 = threshold;
    gLegacySmallHeap3Capacity1086988 = 16;
    return 1;
}

LegacySmallHeap3Region* FindLegacySmallHeap3Region_541C5D(void* const memory)
{
    const std::uint32_t count = gLegacySmallHeap3Count1086998;
    auto* region = gLegacySmallHeap3Regions108699C;
    const auto end = reinterpret_cast<std::uintptr_t>(region)
        + static_cast<std::uintptr_t>(count) * sizeof(LegacySmallHeap3Region);
    while (reinterpret_cast<std::uintptr_t>(region) < end) {
        if (reinterpret_cast<std::uintptr_t>(memory)
                - reinterpret_cast<std::uintptr_t>(region->base) < 0x100000u) {
            return region;
        }
        ++region;
    }
    return nullptr;
}

std::uint32_t GetLegacyAllocationSize_53FCFB(void* const memory)
{
    const std::int32_t mode = gLegacyHeapMode10869A8;
    if (mode == 3) {
        if (FindLegacySmallHeap3Region_541C5D(memory) != nullptr) {
            std::uint32_t size;
            std::memcpy(&size, static_cast<const std::byte*>(memory) - 4, sizeof(size));
            return size - 9u;
        }
    } else if (mode == 2) {
        LegacySmallHeap2Region* region;
        std::byte* page;
        const auto* const marker = FindLegacySmallHeap2Block_5429B8(memory, &region, &page);
        if (marker != nullptr) {
            return static_cast<std::uint32_t>(*marker) << 4;
        }
    }
    return static_cast<std::uint32_t>(::HeapSize(gLegacyHeap10869A4, 0, memory));
}

} // namespace cg::platform
