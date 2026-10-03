#pragma once

#include "StartupNative.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原模式3管理区；内部链布局待与分配/释放一起还原，不按旧字节偏移写原生指针。
struct LegacySmallHeap3Control;

// 原14H区域头；仅+0C/+10地址拓宽，供541C15/541C5D及后续区域生产者共用。
struct LegacySmallHeap3Region final {
    std::uint32_t classMask0;            // 原+00，第一组分类掩码。
    std::uint32_t classMask1;            // 原+04，第二组分类掩码。
    std::uint32_t uncommittedGroups;     // 原+08，32个组的未提交掩码。
    std::byte* base;                     // 原+0C，保留的100000H区域。
    LegacySmallHeap3Control* control;    // 原+10，管理区；所有权随原区域流程。
};
static_assert(offsetof(LegacySmallHeap3Region, classMask0) == 0);
static_assert(offsetof(LegacySmallHeap3Region, classMask1) == 4);
static_assert(offsetof(LegacySmallHeap3Region, uncommittedGroups) == 8);
static_assert(offsetof(LegacySmallHeap3Region, base) == 16);
static_assert(offsetof(LegacySmallHeap3Region, control) == 24);
static_assert(sizeof(LegacySmallHeap3Region) == 32);

// 仅内部原堆实现可用；不能用于释放既有宿主malloc或裸字节适配分配的内存。
extern HANDLE gLegacyHeap10869A4;
extern std::int32_t gLegacyHeapMode10869A8;
extern std::uint32_t gLegacySmallHeap3Capacity1086988;
extern LegacySmallHeap3Region* gLegacySmallHeap3Current1086990;
extern LegacySmallHeap3Region* gLegacySmallHeap3EmptyRegion1086994;
extern std::uint32_t gLegacySmallHeap3Count1086998;
extern LegacySmallHeap3Region* gLegacySmallHeap3Regions108699C;
extern std::uint32_t gLegacySmallHeap3Threshold10869A0;

// 原541BB8；创建后发布模式，子初始化失败销毁堆但不清共享槽。
std::int32_t InitializeLegacyHeap_541BB8(std::int32_t serialized);
// 原541C15；分配16个未清区域头，只初始化原共享管理状态。
std::int32_t InitializeLegacySmallHeap3_541C15(std::uint32_t threshold);
// 原541C5D；按原顺序查100000H所属区域，不改输入或共享槽。
LegacySmallHeap3Region* FindLegacySmallHeap3Region_541C5D(void* memory);

// 原模式2页头；游标拓宽，连续空闲数移至原未使用尾区，标记/载荷几何保持。
struct LegacySmallHeap2Page final {
    std::uint8_t* cursor;                // 原+00，指向markers内。
    std::uint8_t markers[241];           // 原+08..+F8，末项为FF哨兵。
    std::byte padding[3];
    std::uint32_t freeRun;               // 原+04，连续空闲缓存；新+FC。
};
static_assert(offsetof(LegacySmallHeap2Page, cursor) == 0);
static_assert(offsetof(LegacySmallHeap2Page, markers) == 8);
static_assert(offsetof(LegacySmallHeap2Page, freeRun) == 252);
static_assert(sizeof(LegacySmallHeap2Page) == 256);

// 原双W32页元数据；-1表示未提交，failedRequest为失败请求门槛。
struct LegacySmallHeap2PageInfo final {
    std::int32_t freeSlots;
    std::uint32_t failedRequest;
};
static_assert(sizeof(LegacySmallHeap2PageInfo) == 8);

// 原2020H描述区；六地址拓宽，1024逻辑项及末尾8B原存储范围保持。
struct LegacySmallHeap2Region final {
    LegacySmallHeap2Region* next;            // 原+00。
    LegacySmallHeap2Region* previous;        // 原+04。
    LegacySmallHeap2PageInfo* currentPage;    // 原+08。
    LegacySmallHeap2PageInfo* firstUncommitted; // 原+0C。
    std::byte* base;                         // 原+10；-1表示静态头未分配。
    std::byte* end;                          // 原+14。
    LegacySmallHeap2PageInfo pages[1025];     // 原+18；最后8B不作为可用页。
};
static_assert(offsetof(LegacySmallHeap2Region, pages) == 48);
static_assert(sizeof(LegacySmallHeap2Region) == 0x2038);

extern LegacySmallHeap2Region gLegacySmallHeap2Static58FE58;
extern LegacySmallHeap2Region* gLegacySmallHeap2Current591E78;
extern std::uint32_t gLegacySmallHeap2Threshold591E7C;
extern std::uint32_t gLegacySmallHeap2EmptyPages1086154;

// 原八个模式2本体；所有页面布局生产/消费统一使用上面的原生字段。
LegacySmallHeap2Region* CreateLegacySmallHeap2Region_54275C();
void DestroyLegacySmallHeap2Region_5428A0(LegacySmallHeap2Region* region);
void DecommitLegacySmallHeap2Pages_5428F6(std::int32_t pageCount);
std::uint8_t* FindLegacySmallHeap2Block_5429B8(
    void* memory, LegacySmallHeap2Region** region, std::byte** page);
void FreeLegacySmallHeap2Block_542A0F(
    LegacySmallHeap2Region* region, std::byte* page, std::uint8_t* marker);
void* AllocateLegacySmallHeap2Block_542A54(std::int32_t units);
void* AllocateLegacySmallHeap2Page_542C5C(
    LegacySmallHeap2Page* page, std::uint32_t remaining, std::uint32_t units);
std::int32_t ResizeLegacySmallHeap2Block_542D80(
    LegacySmallHeap2Region* region, std::byte* page, std::uint8_t* marker,
    std::uint32_t units);

} // namespace cg::platform
