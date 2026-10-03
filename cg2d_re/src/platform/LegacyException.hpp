#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

struct LegacyExceptionObject;

// 原5501C8的两个真实方法槽；原生显式this，不是宿主C++虚表/RTTI/ThrowInfo。
struct LegacyExceptionMethodTable final {
    LegacyExceptionObject* (*deletingDestructor)(
        LegacyExceptionObject* storage, std::uint32_t flags);
    const char* (*what)(const LegacyExceptionObject* storage);
};

// 原exception已证+00/+04/+08；仅地址拓宽，拥有标志是完整W32，不是U8。
struct LegacyExceptionObject final {
    const LegacyExceptionMethodTable* methods; // 原+00；当前真实两槽原生方法表。
    char* text;                               // 原+04；借用或原堆拥有文本。
    std::uint32_t ownsText;                    // 原+08；保留任意非零位型。
};
static_assert(offsetof(LegacyExceptionObject, text) == 8);
static_assert(offsetof(LegacyExceptionObject, ownsText) == 16);
static_assert(sizeof(LegacyExceptionObject) == 24);
static_assert(sizeof(LegacyExceptionMethodTable) == 16);

// 不自动构析/抛出；实际对象/文本必须沿原堆域与显式生命周期使用。
LegacyExceptionObject* ConstructLegacyExceptionText_54B079(
    LegacyExceptionObject* storage, const char* const* sourceText);
LegacyExceptionObject* CopyConstructLegacyException_54B0B6(
    LegacyExceptionObject* storage, const LegacyExceptionObject* source);
void DestroyLegacyException_54B100(LegacyExceptionObject* storage);
LegacyExceptionObject* DeleteLegacyException_54B05D(
    LegacyExceptionObject* storage, std::uint32_t flags);
const char* GetLegacyExceptionText_54B116(const LegacyExceptionObject* storage);

} // namespace cg::platform
