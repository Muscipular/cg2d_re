#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。
#include "cg/resource/GraphicAlloc.hpp"


#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "../render/GraphicDxCacheState.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource {

std::uint32_t gValue9A4A50 = 0; // 原全局：0x009A4A50。
std::uint32_t gValueD2D5FC = 0; // 原全局：0x00D2D5FC。
std::uint32_t gValueCC3E68 = 0; // 原全局：0x00CC3E68。

// GAP01：原4574E0完整116条复用ACB01；返回/容量先取再pub，原失败清理与四块清零序保持。
std::int32_t GraphicAlloc_4574E0()
{
    for (const char* path : gGraphicInfoPaths) {
        std::FILE* file = std::fopen(path, "rb");
        if (file != nullptr) {
            std::fclose(file);
        }
    }

    animation::gMaxAnimateCount9A4A4C = 20500;
    gValue9A4A50 = 10412000;
    gMaxGraphicCount = 2200000;
    gValueD2D5FC = 0x3C0;
    gValueCC3E68 = 0x40;

    // 原45750F固定分配20500项；x64只按三处原生指针调整记录步长。
    animation::AnimateRecord* const allocatedAnimate =
        static_cast<animation::AnimateRecord*>(
            std::malloc(20500U * sizeof(animation::AnimateRecord)));
    const std::int32_t cacheCapacity = gMaxGraphicCount;
    animation::gAnimateRecords9A4A58 = allocatedAnimate;
    render::DxGraphicEntry* const allocatedDxCache =
        static_cast<render::DxGraphicEntry*>(std::malloc(
            static_cast<std::size_t>(cacheCapacity) * sizeof(render::DxGraphicEntry)));
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
        render::DxGraphicEntry* const cacheToFree = render::gGraphicDxCache103CFA4;
        if (cacheToFree != nullptr) {
            std::free(cacheToFree);
        }
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
    const std::int32_t cacheClearCount = gMaxGraphicCount;
    render::DxGraphicEntry* const cacheToClear = render::gGraphicDxCache103CFA4;
    std::memset(
        cacheToClear, 0, static_cast<std::size_t>(cacheClearCount)
            * sizeof(render::DxGraphicEntry));
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

// GAP01：原457670完整19条；P1/P2/P3逐次一次取样/同快照free，不清槽或新增释放。
void Sub_457670()
{
    animation::AnimateRecord* const animate = animation::gAnimateRecords9A4A58;
    if (animate != nullptr) {
        std::free(animate);
    }
    render::DxGraphicEntry* const cache = render::gGraphicDxCache103CFA4;
    if (cache != nullptr) {
        std::free(cache);
    }
    GraphicInfo* const graphic = gGraphicInfo;
    if (graphic != nullptr) {
        std::free(graphic);
    }
}

} // namespace cg::resource
