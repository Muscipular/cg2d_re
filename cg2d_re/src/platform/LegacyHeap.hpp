#pragma once

#include "StartupNative.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原大小级双链；地址指向链接字段，实块与组哨兵使用同一原生节点类型。
struct LegacySmallHeap3Link final {
    LegacySmallHeap3Link* next;
    LegacySmallHeap3Link* previous;
};
static_assert(sizeof(LegacySmallHeap3Link) == 16);

// 原实空闲块首W32及+04起的两个地址；首块位于页+0C，链接实际8B对齐。
// 本步只用于4080B初始块；16B拆分块的原生链布局仍待闭合。
#pragma pack(push, 4)
struct LegacySmallHeap3FreeBlock final {
    std::uint32_t boundary;
    LegacySmallHeap3Link links;
};
#pragma pack(pop)
static_assert(offsetof(LegacySmallHeap3FreeBlock, links) == 4);
static_assert(sizeof(LegacySmallHeap3FreeBlock) == 20);
static_assert(alignof(LegacySmallHeap3FreeBlock) == 4);

// 原516B组；活跃分配数保持W32，64个双地址链头随原生指针拓宽。
struct LegacySmallHeap3Group final {
    std::uint32_t allocationCount;
    LegacySmallHeap3Link heads[64];
};
static_assert(offsetof(LegacySmallHeap3Group, heads) == 8);
static_assert(sizeof(LegacySmallHeap3Group) == 1032);

// 原41C4H管理区；当前组/大小级计数/两组位图保持原宽度和前段偏移。
struct LegacySmallHeap3Control final {
    std::int32_t currentGroup;
    std::uint8_t classCounts[64];
    std::uint32_t groupMask0[32];
    std::uint32_t groupMask1[32];
    LegacySmallHeap3Group groups[32];
};
static_assert(offsetof(LegacySmallHeap3Control, classCounts) == 4);
static_assert(offsetof(LegacySmallHeap3Control, groupMask0) == 0x44);
static_assert(offsetof(LegacySmallHeap3Control, groupMask1) == 0xC4);
static_assert(offsetof(LegacySmallHeap3Control, groups) == 0x148);
static_assert(sizeof(LegacySmallHeap3Control) == 0x8248);

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

// 原Win32堆/池元数据保留；mode3通用操作按M3H01为SDK域，mode1/2仍原域。
extern HANDLE gLegacyHeap10869A4;
extern std::int32_t gLegacyHeapMode10869A8;
extern std::uint32_t gLegacySmallHeap3Capacity1086988;
extern LegacySmallHeap3Region* gLegacySmallHeap3Current1086990;
extern LegacySmallHeap3Region* gLegacySmallHeap3EmptyRegion1086994;
extern std::uint32_t gLegacySmallHeap3Count1086998;
extern LegacySmallHeap3Region* gLegacySmallHeap3Regions108699C;
extern std::uint32_t gLegacySmallHeap3Threshold10869A0;

// 原108614C/1086150；真实原堆处理器与门禁，不与宿主窄适配状态混用。
using LegacyHeapNewHandler = std::int32_t (*)(std::uint32_t);
extern LegacyHeapNewHandler gLegacyHeapNewHandler108614C;
extern std::int32_t gLegacyHeapNewMode1086150;
std::int32_t CallLegacyHeapNewHandler_541A28(std::uint32_t byteCount);

// 同域malloc/free：mode3为SDK块，mode1/2为原块；模式须在块存活期稳定，禁止混域。
void* AllocateLegacyHeapBlock_53C3EA(std::uint32_t byteCount);
void* AllocateLegacyHeapWithHandler_53C3BE(std::uint32_t byteCount, std::int32_t enableHandler);
void* AllocateLegacyHeapMemory_53C3AC(std::uint32_t byteCount);
void FreeLegacyHeapMemory_53C0A9(void* memory);

// 原模式3核心仅留身份/参考；通用mode3用SDK，不造旧16B链或同名假体。
void* AllocateLegacySmallHeap3Block_541FB1(std::uint32_t byteCount);
void FreeLegacySmallHeap3Block_541C88(LegacySmallHeap3Region* region, void* memory);
std::int32_t ResizeLegacySmallHeap3Block_542466(
    LegacySmallHeap3Region* region, void* memory, std::uint32_t byteCount);

// 原541BB8；创建后发布模式，子初始化失败销毁堆但不清共享槽。
std::int32_t InitializeLegacyHeap_541BB8(std::int32_t serialized);
// 原541C15；分配16个未清区域头，只初始化原共享管理状态。
std::int32_t InitializeLegacySmallHeap3_541C15(std::uint32_t threshold);
// 原541C5D；按原顺序查100000H所属区域，不改输入或共享槽。
LegacySmallHeap3Region* FindLegacySmallHeap3Region_541C5D(void* memory);

// 原5422BA/54236B；真实区域与组生产者，原分配/释放/拆分消费者尚待。
LegacySmallHeap3Region* CreateLegacySmallHeap3Region_5422BA();
std::int32_t CommitLegacySmallHeap3Group_54236B(LegacySmallHeap3Region* region);

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
