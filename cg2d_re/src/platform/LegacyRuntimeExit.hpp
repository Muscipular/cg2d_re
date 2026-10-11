#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C45E/53C4E0/53C560的元素this回调；新端显式传非拥有地址，返回不消费。
using LegacyArrayElementCallback = void (*)(void*);
// storage为首元素，elementBytes保持U32，elementCount保持S32；无空回调/数量补保护。
// 原函数：0x0053C45E。
void ConstructLegacyArray_53C45E(
    void* storage, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback constructor, LegacyArrayElementCallback destructor);
// 原函数：0x0053C4E0。
void DestroyLegacyArray_53C4E0(
    void* storage, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback destructor);
// onePast为已完成/剩余前缀的尾地址，先递减数量/地址再析构；C++二次异常交MSVC terminate。
// 原函数：0x0053C560。
void UnwindLegacyArray_53C560(
    void* onePast, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback destructor);

} // namespace cg::platform
