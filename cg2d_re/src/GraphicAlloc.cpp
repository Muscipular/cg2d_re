#include "cg/GraphicAlloc.hpp"

#include "platform/LegacyStdio.hpp"

#include "cg/AnimationRuntime.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/ResourcePaths.hpp"
#include "render/GraphicDxCacheState.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource {

std::uint32_t gValue9A4A50 = 0;
std::uint32_t gValueD2D5FC = 0;
std::uint32_t gValueCC3E68 = 0;

std::int32_t GraphicAlloc_4574E0()
{
    for (const char* path : gGraphicInfoPaths) {
        platform::LegacyFile* file = platform::OpenLegacyStream_53B2F1(path, "rb");
        if (file != nullptr) {
            platform::CloseLegacyStream_53B1D0(file);
        }
    }

    animation::gMaxAnimateCount9A4A4C = 20500;
    gValue9A4A50 = 10412000;
    gMaxGraphicCount = 2200000;
    gValueD2D5FC = 0x3C0;
    gValueCC3E68 = 0x40;

    // 原45750F固定分配20500项；x64只按三处原生指针调整记录步长。
    animation::gAnimateRecords9A4A58 = static_cast<animation::AnimateRecord*>(
        std::malloc(20500U * sizeof(animation::AnimateRecord)));
    render::gGraphicDxCache103CFA4 = static_cast<render::DxGraphicEntry*>(
        std::malloc(
            static_cast<std::size_t>(gMaxGraphicCount)
            * sizeof(render::DxGraphicEntry)));
    gGraphicInfo = static_cast<GraphicInfo*>(
        std::malloc(
            static_cast<std::size_t>(gMaxGraphicCount) * sizeof(GraphicInfo)));
    animation::gAnimateCache9A4A5C =
        static_cast<animation::AnimeCacheRecord*>(std::malloc(
            static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C)
            * sizeof(animation::AnimeCacheRecord)));

    animation::AnimateRecord* const animate = animation::gAnimateRecords9A4A58;
    if (animate == nullptr
        || render::gGraphicDxCache103CFA4 == nullptr
        || gGraphicInfo == nullptr
        || animation::gAnimateCache9A4A5C == nullptr) {
        if (animate != nullptr) {
            std::free(animate);
        }
        if (render::gGraphicDxCache103CFA4 != nullptr) {
            std::free(render::gGraphicDxCache103CFA4);
        }
        if (gGraphicInfo != nullptr) {
            std::free(gGraphicInfo);
        }
        return 0;
    }

    std::memset(
        animate, 0,
        static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C)
            * sizeof(animation::AnimateRecord));
    std::memset(
        render::gGraphicDxCache103CFA4, 0,
        static_cast<std::size_t>(gMaxGraphicCount)
            * sizeof(render::DxGraphicEntry));
    std::memset(
        gGraphicInfo, 0,
        static_cast<std::size_t>(gMaxGraphicCount) * sizeof(GraphicInfo));
    std::memset(
        animation::gAnimateCache9A4A5C, 0,
        static_cast<std::size_t>(animation::gMaxAnimateCount9A4A4C)
            * sizeof(animation::AnimeCacheRecord));
    return 1;
}

void Sub_457670()
{
    if (animation::gAnimateRecords9A4A58 != nullptr) {
        std::free(animation::gAnimateRecords9A4A58);
    }
    if (render::gGraphicDxCache103CFA4 != nullptr) {
        std::free(render::gGraphicDxCache103CFA4);
    }
    if (gGraphicInfo != nullptr) {
        std::free(gGraphicInfo);
    }
}

} // namespace cg::resource
