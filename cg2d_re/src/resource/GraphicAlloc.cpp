#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。
#include "cg/resource/GraphicAlloc.hpp"


#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "../render/GraphicDxCacheState.hpp"

#include <cstdlib>
#include <cstring>
#include <new>

namespace cg::resource {

std::uint32_t gAlbumEntryLimit_D2D5FC = 0; // 原全局：0x00D2D5FC。

// 原函数：0x004574E0；BLM01容量取配置；RMO01仅DX改为清零页目录。
std::int32_t GraphicAlloc_4574E0()
{
    animation::gMaxAnimateCount9A4A4C = gBinLoaderAnimeCapacity;
    gMaxGraphicCount = gBinLoaderGraphicCapacity;
    gAlbumEntryLimit_D2D5FC = 0x3C0;

    // RMO01：DX页目录覆盖配置容量，记录页首次写入才分配。
    animation::AnimateRecord* const allocatedAnimate =
        static_cast<animation::AnimateRecord*>(
            std::malloc(static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C)
                * sizeof(animation::AnimateRecord)));
    const std::int32_t cacheCapacity = gMaxGraphicCount;
    animation::gAnimateRecords9A4A58 = allocatedAnimate;
    auto* allocatedDxCache = new (std::nothrow) render::GraphicDxCache(cacheCapacity);
    if (allocatedDxCache != nullptr && !allocatedDxCache->IsAllocated()) {
        delete allocatedDxCache;
        allocatedDxCache = nullptr;
    }
    const std::int32_t graphicCapacity = gMaxGraphicCount;
    render::gGraphicDxCache103CFA4 = allocatedDxCache;
    gGraphicInfo = static_cast<GraphicInfo*>(
        std::malloc(static_cast<std::size_t>(graphicCapacity) * sizeof(GraphicInfo)));
    animation::AnimeCacheRecord* const allocatedAnimeCache =
        static_cast<animation::AnimeCacheRecord*>(std::malloc(
            static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C)
            * sizeof(animation::AnimeCacheRecord)));

    animation::AnimateRecord* const animate = animation::gAnimateRecords9A4A58;
    const bool missingAnimate = animate == nullptr;
    animation::gAnimateCache9A4A5C = allocatedAnimeCache;
    if (missingAnimate
        || render::gGraphicDxCache103CFA4 == nullptr
        || gGraphicInfo == nullptr
        || allocatedAnimeCache == nullptr) {
        if (animate != nullptr) {
            std::free(animate);
        }
        delete render::gGraphicDxCache103CFA4;
        render::gGraphicDxCache103CFA4 = nullptr;
        GraphicInfo* const graphicToFree = gGraphicInfo;
        if (graphicToFree != nullptr) {
            std::free(graphicToFree);
        }
        return 0;
    }

    const std::int32_t animateClearCount = animation::gMaxAnimateCount9A4A4C;
    std::memset(
        animate, 0, static_cast<std::size_t>(animateClearCount)
            * sizeof(animation::AnimateRecord));
    const std::int32_t graphicClearCount = gMaxGraphicCount;
    GraphicInfo* const graphicToClear = gGraphicInfo;
    std::memset(
        graphicToClear, 0,
        static_cast<std::size_t>(graphicClearCount) * sizeof(GraphicInfo));
    const std::int32_t animeCacheClearCount = animation::gMaxAnimateCount9A4A4C;
    animation::AnimeCacheRecord* const animeCacheToClear = animation::gAnimateCache9A4A5C;
    std::memset(
        animeCacheToClear, 0, static_cast<std::size_t>(animeCacheClearCount)
            * sizeof(animation::AnimeCacheRecord));
    return 1;
}

// 原457670：P1/P3沿CRT释放；RMO01的P2逐页销毁并清缓存拥有者。
void FreePrimaryGraphicResourceTables_457670()
{
    animation::AnimateRecord* const animate = animation::gAnimateRecords9A4A58;
    if (animate != nullptr) {
        std::free(animate);
    }
    delete render::gGraphicDxCache103CFA4;
    render::gGraphicDxCache103CFA4 = nullptr;
    GraphicInfo* const graphic = gGraphicInfo;
    if (graphic != nullptr) {
        std::free(graphic);
    }
}

} // namespace cg::resource
