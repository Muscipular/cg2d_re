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

// 模式2真实依赖声明；页内指针/计数/标记的x64整体布局尚待完成。
struct LegacySmallHeap2Region;
LegacySmallHeap2Region* CreateLegacySmallHeap2Region_54275C();
std::uint8_t* FindLegacySmallHeap2Block_5429B8(
    void* memory, LegacySmallHeap2Region** region, std::byte** page);

} // namespace cg::platform
