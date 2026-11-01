#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。
#include "cg/resource/GraphicAlloc.hpp"


#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "../render/GraphicDxCacheState.hpp"

#include <cstdlib>
#include <memory>
#include <new>

namespace cg::resource {

std::uint32_t gAlbumEntryLimit_D2D5FC = 0; // 原全局：0x00D2D5FC。

// 原函数：0x004574E0；RAC01在索引覆盖完成后整理引用，再按实际容量发布缓存。
std::int32_t GraphicAlloc_4574E0()
{
    gAlbumEntryLimit_D2D5FC = 0x3C0;
    try {
        gGraphicInfo.Retain(0);
        for (std::int32_t mapNo = 1; mapNo < gGraphicMapNoCapacity; ++mapNo) {
            if (gGraphicMapNoIndex[mapNo] != 0) {
                gGraphicInfo.Retain(static_cast<std::uint32_t>(gGraphicMapNoIndex[mapNo]));
            }
        }
        if (!animation::RetainAnimationGraphicReferences()) {
            return 0;
        }
        gGraphicInfo.Compact();
        std::unique_ptr<render::GraphicDxCache> dxCache(
            new (std::nothrow) render::GraphicDxCache(gMaxGraphicCount));
        if (!dxCache || !dxCache->IsAllocated()) {
            return 0;
        }
        auto* const animeCache = static_cast<animation::AnimeCacheRecord*>(std::calloc(
            static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C),
            sizeof(animation::AnimeCacheRecord)));
        if (animeCache == nullptr) {
            return 0;
        }
        render::gGraphicDxCache103CFA4 = dxCache.release();
        animation::gAnimateCache9A4A5C = animeCache;
        return 1;
    } catch (const std::bad_alloc&) {
        return 0;
    }
}

// 原457670；RAC01在活动槽清理后释放四块真实owner并撤销借用。
void FreePrimaryGraphicResourceTables_457670()
{
    std::free(animation::gAnimateRecords9A4A58);
    animation::gAnimateRecords9A4A58 = nullptr;
    animation::gAnimateCount9A4A60 = 0;
    animation::gMaxAnimateCount9A4A4C = 0;
    std::free(animation::gAnimateCache9A4A5C);
    animation::gAnimateCache9A4A5C = nullptr;
    delete render::gGraphicDxCache103CFA4;
    render::gGraphicDxCache103CFA4 = nullptr;
    gGraphicInfo.Clear();
    gMaxGraphicCount = 0;
}

} // namespace cg::resource
