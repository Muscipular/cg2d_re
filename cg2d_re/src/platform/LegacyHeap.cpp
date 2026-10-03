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


namespace {

std::byte* const kLegacySmallHeap2InvalidAddress =
    reinterpret_cast<std::byte*>(std::intptr_t{-1});

// 三个扫描成功出口共用的原marker→data地址关系；不扩数值单位或页尺寸。
void* SmallHeap2DataForMarker(LegacySmallHeap2Page* const page,
    const std::uint8_t* const marker)
{
    return reinterpret_cast<std::byte*>(page) + 0x100
        + (marker - page->markers) * 16;
}

// 创建/重新提交共同的三次页头写入，保留哨兵、游标、连续空闲数次序。
void InitializeSmallHeap2Page(LegacySmallHeap2Page* const page)
{
    page->markers[240] = 0xFF;
    page->cursor = page->markers;
    page->freeRun = 240;
}

} // namespace

LegacySmallHeap2Region gLegacySmallHeap2Static58FE58 = {
    &gLegacySmallHeap2Static58FE58,
    &gLegacySmallHeap2Static58FE58,
    gLegacySmallHeap2Static58FE58.pages,
    gLegacySmallHeap2Static58FE58.pages,
    reinterpret_cast<std::byte*>(std::intptr_t{-1}),
    reinterpret_cast<std::byte*>(std::intptr_t{-1}),
    {{240, 241}}
};
LegacySmallHeap2Region* gLegacySmallHeap2Current591E78 = &gLegacySmallHeap2Static58FE58;
std::uint32_t gLegacySmallHeap2Threshold591E7C = 480;
std::uint32_t gLegacySmallHeap2EmptyPages1086154 = 0;

LegacySmallHeap2Region* CreateLegacySmallHeap2Region_54275C()
{
    auto* region = &gLegacySmallHeap2Static58FE58;
    if (gLegacySmallHeap2Static58FE58.base != kLegacySmallHeap2InvalidAddress) {
        region = static_cast<LegacySmallHeap2Region*>(
            ::HeapAlloc(gLegacyHeap10869A4, 0, sizeof(LegacySmallHeap2Region)));
        if (region == nullptr) {
            return nullptr;
        }
    }
    auto* const base = static_cast<std::byte*>(
        ::VirtualAlloc(nullptr, 0x400000, MEM_RESERVE, PAGE_READWRITE));
    if (base != nullptr) {
        if (::VirtualAlloc(base, 0x10000, MEM_COMMIT, PAGE_READWRITE) != nullptr) {
            if (region == &gLegacySmallHeap2Static58FE58) {
                if (gLegacySmallHeap2Static58FE58.next == nullptr) {
                    gLegacySmallHeap2Static58FE58.next = &gLegacySmallHeap2Static58FE58;
                }
                if (gLegacySmallHeap2Static58FE58.previous == nullptr) {
                    gLegacySmallHeap2Static58FE58.previous = &gLegacySmallHeap2Static58FE58;
                }
            } else {
                region->next = &gLegacySmallHeap2Static58FE58;
                region->previous = gLegacySmallHeap2Static58FE58.previous;
                gLegacySmallHeap2Static58FE58.previous = region;
                region->previous->next = region;
            }
            region->end = base + 0x400000;
            region->firstUncommitted = region->pages + 16;
            region->base = base;
            region->currentPage = region->pages;
            for (std::int32_t index = 0; index < 1024; ++index) {
                region->pages[index].freeSlots = index < 16 ? 240 : -1;
                region->pages[index].failedRequest = 241;
            }
            std::memset(base, 0, 0x10000);
            for (auto* address = base; reinterpret_cast<std::uintptr_t>(address)
                    < reinterpret_cast<std::uintptr_t>(region->base) + 0x10000;
                 address += 0x1000) {
                InitializeSmallHeap2Page(reinterpret_cast<LegacySmallHeap2Page*>(address));
            }
            return region;
        }
        static_cast<void>(::VirtualFree(base, 0, MEM_RELEASE));
    }
    if (region != &gLegacySmallHeap2Static58FE58) {
        static_cast<void>(::HeapFree(gLegacyHeap10869A4, 0, region));
    }
    return nullptr;
}

void DestroyLegacySmallHeap2Region_5428A0(LegacySmallHeap2Region* const region)
{
    static_cast<void>(::VirtualFree(region->base, 0, MEM_RELEASE));
    if (gLegacySmallHeap2Current591E78 == region) {
        gLegacySmallHeap2Current591E78 = region->previous;
    }
    if (region == &gLegacySmallHeap2Static58FE58) {
        gLegacySmallHeap2Static58FE58.base = kLegacySmallHeap2InvalidAddress;
    } else {
        region->previous->next = region->next;
        region->next->previous = region->previous;
        static_cast<void>(::HeapFree(gLegacyHeap10869A4, 0, region));
    }
}

void DecommitLegacySmallHeap2Pages_5428F6(std::int32_t pageCount)
{
    auto* region = gLegacySmallHeap2Static58FE58.previous;
    do {
        if (region->base != kLegacySmallHeap2InvalidAddress) {
            std::int32_t released = 0;
            for (std::int32_t index = 1023; index >= 0; --index) {
                auto* const info = region->pages + index;
                if (info->freeSlots == 240
                    && ::VirtualFree(region->base + index * 0x1000,
                        0x1000, MEM_DECOMMIT) != 0) {
                    info->freeSlots = -1;
                    --gLegacySmallHeap2EmptyPages1086154;
                    auto* const first = region->firstUncommitted;
                    if (first == nullptr || first > info) {
                        region->firstUncommitted = info;
                    }
                    ++released;
                    if (--pageCount == 0) {
                        break;
                    }
                }
            }
            auto* const finished = region;
            region = region->previous;
            if (released != 0 && finished->pages[0].freeSlots == -1) {
                std::int32_t index = 1;
                while (index < 1024 && finished->pages[index].freeSlots == -1) {
                    ++index;
                }
                if (index == 1024) {
                    DestroyLegacySmallHeap2Region_5428A0(finished);
                }
            }
        }
    } while (region != gLegacySmallHeap2Static58FE58.previous && pageCount > 0);
}

std::uint8_t* FindLegacySmallHeap2Block_5429B8(void* const memory,
    LegacySmallHeap2Region** const outputRegion, std::byte** const outputPage)
{
    const auto address = reinterpret_cast<std::uintptr_t>(memory);
    auto* region = &gLegacySmallHeap2Static58FE58;
    while (address <= reinterpret_cast<std::uintptr_t>(region->base)
        || address >= reinterpret_cast<std::uintptr_t>(region->end)) {
        region = region->next;
        if (region == &gLegacySmallHeap2Static58FE58) {
            return nullptr;
        }
    }
    if ((address & 0xFu) != 0 || (address & 0xFFFu) < 0x100u) {
        return nullptr;
    }
    *outputRegion = region;
    auto* const page = reinterpret_cast<std::byte*>(address & ~std::uintptr_t{0xFFF});
    *outputPage = page;
    auto* const header = reinterpret_cast<LegacySmallHeap2Page*>(page);
    return header->markers + ((address - reinterpret_cast<std::uintptr_t>(page) - 0x100) >> 4);
}

void FreeLegacySmallHeap2Block_542A0F(LegacySmallHeap2Region* const region,
    std::byte* const page, std::uint8_t* const marker)
{
    auto* const info = region->pages + ((page - region->base) / 0x1000);
    info->freeSlots += *marker;
    *marker = 0;
    const bool empty = info->freeSlots == 240;
    info->failedRequest = 241;
    if (empty && ++gLegacySmallHeap2EmptyPages1086154 == 32) {
        DecommitLegacySmallHeap2Pages_5428F6(16);
    }
}

void* AllocateLegacySmallHeap2Block_542A54(const std::int32_t units)
{
    auto* region = gLegacySmallHeap2Current591E78;
    do {
        if (region->base != kLegacySmallHeap2InvalidAddress) {
            auto* info = region->currentPage;
            auto* address = region->base + (info - region->pages) * 0x1000;
            auto* const end = region->pages + 1024;
            while (info < end) {
                if (info->freeSlots >= units
                    && info->failedRequest > static_cast<std::uint32_t>(units)) {
                    void* const result = AllocateLegacySmallHeap2Page_542C5C(
                        reinterpret_cast<LegacySmallHeap2Page*>(address),
                        static_cast<std::uint32_t>(info->freeSlots),
                        static_cast<std::uint32_t>(units));
                    if (result != nullptr) {
                        gLegacySmallHeap2Current591E78 = region;
                        info->freeSlots -= units;
                        region->currentPage = info;
                        return result;
                    }
                    info->failedRequest = static_cast<std::uint32_t>(units);
                }
                ++info;
                address += 0x1000;
            }
            auto* const stop = region->currentPage;
            info = region->pages;
            address = region->base;
            while (info < stop) {
                if (info->freeSlots >= units
                    && info->failedRequest > static_cast<std::uint32_t>(units)) {
                    void* const result = AllocateLegacySmallHeap2Page_542C5C(
                        reinterpret_cast<LegacySmallHeap2Page*>(address),
                        static_cast<std::uint32_t>(info->freeSlots),
                        static_cast<std::uint32_t>(units));
                    if (result != nullptr) {
                        gLegacySmallHeap2Current591E78 = region;
                        info->freeSlots -= units;
                        region->currentPage = info;
                        return result;
                    }
                    info->failedRequest = static_cast<std::uint32_t>(units);
                }
                address += 0x1000;
                ++info;
            }
        }
        region = region->next;
    } while (region != gLegacySmallHeap2Current591E78);

    region = &gLegacySmallHeap2Static58FE58;
    while (region->base == kLegacySmallHeap2InvalidAddress
        || region->firstUncommitted == nullptr) {
        region = region->next;
        if (region == &gLegacySmallHeap2Static58FE58) {
            auto* const created = CreateLegacySmallHeap2Region_54275C();
            if (created == nullptr) {
                return nullptr;
            }
            auto* const page = reinterpret_cast<LegacySmallHeap2Page*>(created->base);
            page->markers[0] = static_cast<std::uint8_t>(units);
            gLegacySmallHeap2Current591E78 = created;
            page->cursor = page->markers + units;
            page->freeRun = 240u - static_cast<std::uint32_t>(units);
            created->pages[0].freeSlots -= static_cast<std::uint8_t>(units);
            return created->base + 0x100;
        }
    }

    auto* const first = region->firstUncommitted;
    std::int32_t count = 0;
    auto* probe = first;
    auto* const address = region->base + (first - region->pages) * 0x1000;
    if (first->freeSlots == -1) {
        while (count < 16) {
            ++probe;
            ++count;
            if (probe->freeSlots != -1) {
                break;
            }
        }
    }
    const std::uint32_t commitBytes = static_cast<std::uint32_t>(count) << 12;
    if (::VirtualAlloc(address, commitBytes, MEM_COMMIT, PAGE_READWRITE) != address) {
        return nullptr;
    }
    // 原542B9D..542BA3：第三参数为0，不能改为清commitBytes字节。
    std::memset(address, static_cast<int>(commitBytes), 0);
    auto* info = first;
    for (std::int32_t index = 0; index < count; ++index) {
        InitializeSmallHeap2Page(
            reinterpret_cast<LegacySmallHeap2Page*>(address + index * 0x1000));
        info->freeSlots = 240;
        info->failedRequest = 241;
        ++info;
    }
    gLegacySmallHeap2Current591E78 = region;
    while (info < region->pages + 1024 && info->freeSlots != -1) {
        ++info;
    }
    region->firstUncommitted = info < region->pages + 1024 ? info : nullptr;
    auto* const page = reinterpret_cast<LegacySmallHeap2Page*>(address);
    page->markers[0] = static_cast<std::uint8_t>(units);
    region->currentPage = first;
    first->freeSlots -= units;
    page->freeRun -= static_cast<std::uint32_t>(units);
    page->cursor = page->markers + units;
    return address + 0x100;
}

void* AllocateLegacySmallHeap2Page_542C5C(LegacySmallHeap2Page* const page,
    std::uint32_t remaining, const std::uint32_t units)
{
    const std::uint32_t cached = page->freeRun;
    auto* const originalCursor = page->cursor;
    auto* const end = page->markers + 240;
    auto* candidate = originalCursor;
    if (cached >= units) {
        *candidate = static_cast<std::uint8_t>(units);
        if (candidate + units < end) {
            page->cursor += units;
            page->freeRun -= units;
        } else {
            page->freeRun = 0;
            page->cursor = page->markers;
        }
        return SmallHeap2DataForMarker(page, candidate);
    }
    auto* const afterCached = originalCursor + cached;
    if (*afterCached != 0) {
        candidate = afterCached;
    }
    while (candidate + units < end) {
        const std::uint8_t allocated = *candidate;
        if (allocated != 0) {
            candidate += allocated;
            continue;
        }
        auto* after = candidate + 1;
        std::uint32_t run = 1;
        while (*after == 0) {
            ++after;
            ++run;
        }
        if (run >= units) {
            if (candidate + units < end) {
                page->cursor = candidate + units;
                page->freeRun = run - units;
            } else {
                page->freeRun = 0;
                page->cursor = page->markers;
            }
            *candidate = static_cast<std::uint8_t>(units);
            return SmallHeap2DataForMarker(page, candidate);
        }
        if (candidate == originalCursor) {
            page->freeRun = run;
        } else {
            remaining -= run;
            if (remaining < units) {
                return nullptr;
            }
        }
        candidate = after;
    }

    candidate = page->markers;
    while (candidate < originalCursor && candidate + units < end) {
        const std::uint8_t allocated = *candidate;
        if (allocated != 0) {
            candidate += allocated;
            continue;
        }
        auto* after = candidate + 1;
        std::uint32_t run = 1;
        while (*after == 0) {
            ++after;
            ++run;
        }
        if (run >= units) {
            if (candidate + units < end) {
                page->cursor = candidate + units;
                page->freeRun = run - units;
            } else {
                page->freeRun = 0;
                page->cursor = page->markers;
            }
            *candidate = static_cast<std::uint8_t>(units);
            return SmallHeap2DataForMarker(page, candidate);
        }
        remaining -= run;
        if (remaining < units) {
            return nullptr;
        }
        candidate = after;
    }
    return nullptr;
}

std::int32_t ResizeLegacySmallHeap2Block_542D80(LegacySmallHeap2Region* const region,
    std::byte* const address, std::uint8_t* const marker, const std::uint32_t units)
{
    const std::uint32_t previous = *marker;
    auto* const info = region->pages + ((address - region->base) / 0x1000);
    if (previous > units) {
        *marker = static_cast<std::uint8_t>(units);
        info->freeSlots += static_cast<std::int32_t>(previous - units);
        info->failedRequest = 241;
        return 1;
    }
    if (previous < units) {
        auto* const page = reinterpret_cast<LegacySmallHeap2Page*>(address);
        auto* const end = marker + units;
        if (end <= page->markers + 240) {
            auto* cursor = marker + previous;
            while (cursor < end && *cursor == 0) {
                ++cursor;
            }
            if (cursor == end) {
                *marker = static_cast<std::uint8_t>(units);
                auto* const cachedCursor = page->cursor;
                if (marker <= cachedCursor && end > cachedCursor) {
                    if (end < page->markers + 240) {
                        std::uint32_t run = 0;
                        page->cursor = end;
                        while (end[run] == 0) {
                            ++run;
                        }
                        page->freeRun = run;
                    } else {
                        page->freeRun = 0;
                        page->cursor = page->markers;
                    }
                }
                info->freeSlots += static_cast<std::int32_t>(previous)
                    - static_cast<std::int32_t>(units);
                return 1;
            }
        }
    }
    return 0;
}


LegacySmallHeap3Region* CreateLegacySmallHeap3Region_5422BA()
{
    std::uint32_t count = gLegacySmallHeap3Count1086998;
    const std::uint32_t capacity = gLegacySmallHeap3Capacity1086988;
    if (count == capacity) {
        const std::uint32_t bytes = (capacity + 16u)
            * static_cast<std::uint32_t>(sizeof(LegacySmallHeap3Region));
        auto* const resized = static_cast<LegacySmallHeap3Region*>(
            ::HeapReAlloc(gLegacyHeap10869A4, 0, gLegacySmallHeap3Regions108699C, bytes));
        if (resized == nullptr) {
            return nullptr;
        }
        gLegacySmallHeap3Capacity1086988 += 16;
        gLegacySmallHeap3Regions108699C = resized;
        count = gLegacySmallHeap3Count1086998;
    }
    auto* const region = gLegacySmallHeap3Regions108699C + count;
    auto* const control = static_cast<LegacySmallHeap3Control*>(
        ::HeapAlloc(gLegacyHeap10869A4, HEAP_ZERO_MEMORY, sizeof(LegacySmallHeap3Control)));
    region->control = control;
    if (control == nullptr) {
        return nullptr;
    }
    auto* const base = static_cast<std::byte*>(
        ::VirtualAlloc(nullptr, 0x100000, MEM_RESERVE, PAGE_READWRITE));
    region->base = base;
    if (base == nullptr) {
        static_cast<void>(::HeapFree(gLegacyHeap10869A4, 0, region->control));
        return nullptr;
    }
    region->uncommittedGroups = 0xFFFFFFFFu;
    region->classMask0 = 0;
    region->classMask1 = 0;
    ++gLegacySmallHeap3Count1086998;
    region->control->currentGroup = -1;
    return region;
}

std::int32_t CommitLegacySmallHeap3Group_54236B(LegacySmallHeap3Region* const region)
{
    auto* const control = region->control;
    std::uint32_t mask = region->uncommittedGroups;
    std::int32_t index = 0;
    while ((mask & 0x80000000u) == 0) {
        mask <<= 1;
        ++index;
    }
    auto* const group = control->groups + index;
    for (std::int32_t bucket = 0; bucket < 63; ++bucket) {
        auto* const head = group->heads + bucket;
        head->previous = head;
        head->next = head;
    }
    auto* const base = region->base + (index << 15);
    if (::VirtualAlloc(base, 0x8000, MEM_COMMIT, PAGE_READWRITE) == nullptr) {
        return -1;
    }
    for (std::int32_t page = 0; page < 8; ++page) {
        auto* const address = base + page * 0x1000;
        auto* const block = reinterpret_cast<LegacySmallHeap3FreeBlock*>(address + 0x0C);
        *reinterpret_cast<std::uint32_t*>(address + 8) = 0xFFFFFFFFu;
        *reinterpret_cast<std::uint32_t*>(address + 0xFFC) = 0xFFFFFFFFu;
        block->boundary = 4080;
        // 原边端地址先指向相邻页；这里只形成整数地址，提交范围外不解引用。
        const auto pageAddress = reinterpret_cast<std::uintptr_t>(address);
        block->links.next = reinterpret_cast<LegacySmallHeap3Link*>(pageAddress + 0x1010);
        block->links.previous = reinterpret_cast<LegacySmallHeap3Link*>(pageAddress - 0xFF0);
        *reinterpret_cast<std::uint32_t*>(address + 0xFF8) = 4080;
    }
    auto* const head = group->heads + 63;
    auto* const first = &reinterpret_cast<LegacySmallHeap3FreeBlock*>(base + 0x0C)->links;
    auto* const last =
        &reinterpret_cast<LegacySmallHeap3FreeBlock*>(base + 0x700C)->links;
    head->next = first;
    first->previous = head;
    head->previous = last;
    last->next = head;
    control->groupMask0[index] = 0;
    control->groupMask1[index] = 1;
    const std::uint8_t oldCount = control->classCounts[63];
    control->classCounts[63] = static_cast<std::uint8_t>(oldCount + 1);
    if (oldCount == 0) {
        region->classMask1 |= 1u;
    }
    region->uncommittedGroups &= ~(0x80000000u >> index);
    return index;
}

} // namespace cg::platform
