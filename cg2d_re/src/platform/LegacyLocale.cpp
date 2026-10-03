#include "LegacyLocale.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyTime.hpp"

namespace cg::platform {
namespace {

LegacyLocaleFacet* DeleteLegacyLocaleImplementationFromFacet(
    LegacyLocaleFacet* const storage, const std::uint32_t flags)
{
    DeleteLegacyLocaleImplementation_54A59B(
        reinterpret_cast<LegacyLocaleImplementation*>(storage), flags);
    return storage;
}

// 两原表已证的槽0；真实本体地址，不生成SDK vft/RTTI或未知facet槽。
const LegacyLocaleFacetMethods gLegacyLocaleFacetMethods55015C{
    DeleteLegacyLocaleFacet_54A57E
};
const LegacyLocaleFacetMethods gLegacyLocaleImplementationMethods550154{
    DeleteLegacyLocaleImplementationFromFacet
};
const char gLegacyLocaleStar55014C[2] = "*"; // 原已加载2B含NUL。

// 原54A577是54A505/清理共用尾，仅改facet方法表，不新增原函数身份。
void RestoreLegacyLocaleFacetMethods_54A577(LegacyLocaleFacet* const storage)
{
    storage->methods = &gLegacyLocaleFacetMethods55015C;
}

} // namespace

LegacyLocaleImplementation* ConstructLegacyLocaleImplementation_54A505(
    LegacyLocaleImplementation* const storage, const std::uint32_t transparencyWord)
{
    storage->facet.references = 1;
    storage->facet.methods = &gLegacyLocaleFacetMethods55015C;
    storage->transparent = static_cast<std::uint8_t>(transparencyWord);
    const std::uint8_t namePrefix = static_cast<std::uint8_t>(transparencyWord >> 24);
    storage->facets = nullptr;
    storage->facetCount = 0;
    storage->categories = 0;
    // 原8A4D0B读EBP+0B，同一个4B实参的第4字节，不是新实参/固定0。
    storage->name.prefix00[0] = namePrefix;
    try {
        TidyLegacyNarrowString_548E29(&storage->name, false);
        const std::uint32_t length = StrLen_53F9E0(gLegacyLocaleStar55014C);
        AssignLegacyNarrowString_549055(&storage->name, gLegacyLocaleStar55014C, length);
    } catch (...) {
        // 原state0仅54A577，不额外清名称/回滚已写字段。
        RestoreLegacyLocaleFacetMethods_54A577(&storage->facet);
        throw;
    }
    storage->facet.methods = &gLegacyLocaleImplementationMethods550154;
    return storage;
}

LegacyLocaleFacet* DecrementLegacyLocaleFacet_548695(LegacyLocaleFacet* const storage)
{
    const std::uint32_t references = storage->references;
    if (references > 0 && references < 0xFFFFFFFFu) {
        storage->references = references - 1u;
    }
    // 原5486A5重新取计数，0输入同样返回this，FFFFFFFF不递减且返回空。
    return storage->references == 0 ? storage : nullptr;
}

void DestroyLegacyLocale_54867D(LegacyLocaleImplementation** const implementationSlot)
{
    LegacyLocaleImplementation* const implementation = *implementationSlot;
    if (implementation != nullptr) {
        LegacyLocaleFacet* const facet = DecrementLegacyLocaleFacet_548695(&implementation->facet);
        if (facet != nullptr) {
            facet->methods->deletingDestructor(facet, 1);
        }
    }
}

LegacyLocaleFacet* DeleteLegacyLocaleFacet_54A57E(
    LegacyLocaleFacet* const storage, const std::uint32_t flags)
{
    // 原第一条已test flags，先保存判定，再写基类方法表。
    const bool deleteObject = (flags & 1u) != 0;
    storage->methods = &gLegacyLocaleFacetMethods55015C;
    if (deleteObject) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

void DestroyLegacyLocaleImplementation_54A5B7(LegacyLocaleImplementation* const storage)
{
    storage->facet.methods = &gLegacyLocaleImplementationMethods550154;
    std::uint32_t remaining = storage->facetCount;
    try {
        try {
            while (remaining != 0) {
                // 原每轮先读向量地址再退index，facet析构可能改下一轮向量。
                LegacyLocaleFacet** const facets = storage->facets;
                --remaining;
                LegacyLocaleFacet* const facet = facets[remaining];
                if (facet != nullptr) {
                    const std::uint32_t references = facet->references;
                    if (references > 0 && references < 0xFFFFFFFFu) {
                        facet->references = references - 1u;
                    }
                    if (facet->references == 0) {
                        facet->methods->deletingDestructor(facet, 1);
                    }
                }
            }
            // 原53C0A9直接free；取当时向量，空也传入，不清原字段。
            FreeLegacyHeapMemory_53C0A9(storage->facets);
        } catch (...) {
            // 原state2→1：空清理后名称析构；继而state0只恢复facet表。
            TidyLegacyNarrowString_548E29(&storage->name, true);
            throw;
        }
        // 原进入名称正常析构前已回state0，不能失败后再析构名称一次。
        TidyLegacyNarrowString_548E29(&storage->name, true);
    } catch (...) {
        RestoreLegacyLocaleFacetMethods_54A577(&storage->facet);
        throw;
    }
    RestoreLegacyLocaleFacetMethods_54A577(&storage->facet);
}

LegacyLocaleImplementation* DeleteLegacyLocaleImplementation_54A59B(
    LegacyLocaleImplementation* const storage, const std::uint32_t flags)
{
    DestroyLegacyLocaleImplementation_54A5B7(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

} // namespace cg::platform
