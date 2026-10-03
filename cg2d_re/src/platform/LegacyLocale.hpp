#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyNarrowString.hpp"

namespace cg::platform {

struct LegacyLocaleFacet;

// 原facet/_Locimp本组只调用vft+00；实际显式this，未接宿主SDK对象。
struct LegacyLocaleFacetMethods final {
    LegacyLocaleFacet* (*deletingDestructor)(LegacyLocaleFacet* storage, std::uint32_t flags);
};

// 原facet前缀+00/+04；引用保持U32/0与FFFFFFFF门禁，不自动管理生命周期。
struct LegacyLocaleFacet final {
    const LegacyLocaleFacetMethods* methods; // 原+00。
    std::uint32_t references;                // 原+04。
};
static_assert(offsetof(LegacyLocaleFacet, references) == 8);
static_assert(sizeof(LegacyLocaleFacet) == 16);

// 原54A505/54A404分配40B的_Locimp；地址拓宽，其他数值/首字节仍原位型。
struct LegacyLocaleImplementation final {
    LegacyLocaleFacet facet;         // 原+00；实际首成员。
    LegacyLocaleFacet** facets;       // 原+08；实际facet向量，析构每轮重取。
    std::uint32_t facetCount;         // 原+0C；析构入口取一次，U32逆序。
    std::uint32_t categories;         // 原+10。
    std::uint8_t transparent;         // 原+14；原参数低字节，不归一化bool。
    LegacyNarrowString name;          // 原+18；首前缀取参数同W32第4字节。
};
static_assert(offsetof(LegacyLocaleImplementation, facet) == 0);
static_assert(offsetof(LegacyLocaleImplementation, facets) == 16);
static_assert(offsetof(LegacyLocaleImplementation, facetCount) == 24);
static_assert(offsetof(LegacyLocaleImplementation, categories) == 28);
static_assert(offsetof(LegacyLocaleImplementation, transparent) == 32);
static_assert(offsetof(LegacyLocaleImplementation, name) == 40);
static_assert(sizeof(LegacyLocaleImplementation) == 64);

// 不初始化全局locale/登记退出或宿主迁移；所有调用只接真实原生字段/原堆域。
LegacyLocaleImplementation* ConstructLegacyLocaleImplementation_54A505(
    LegacyLocaleImplementation* storage, std::uint32_t transparencyWord);
LegacyLocaleFacet* DecrementLegacyLocaleFacet_548695(LegacyLocaleFacet* storage);
void DestroyLegacyLocale_54867D(LegacyLocaleImplementation** implementationSlot);
LegacyLocaleFacet* DeleteLegacyLocaleFacet_54A57E(
    LegacyLocaleFacet* storage, std::uint32_t flags);
void DestroyLegacyLocaleImplementation_54A5B7(LegacyLocaleImplementation* storage);
LegacyLocaleImplementation* DeleteLegacyLocaleImplementation_54A59B(
    LegacyLocaleImplementation* storage, std::uint32_t flags);

} // namespace cg::platform
