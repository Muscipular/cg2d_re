#include "cg/render/GraphicPaletteCache.hpp"

#include "GraphicDxCacheState.hpp"

#include <bit>
#include <memory>
#include <utility>

namespace cg::render {
namespace {

std::unique_ptr<PaletteCacheItem[]> gOwnedGraphicPaletteCache;

} // namespace

PaletteCacheItem* gGraphicDataCacheList = nullptr;
std::array<PaletteCacheItem, kGraphicPaletteStaticCount> gGraphicDataCacheStatic {};

void InitializeGraphicPaletteCache_44E0C0()
{
    std::unique_ptr<PaletteCacheItem[]> pending(
        new PaletteCacheItem[kGraphicPaletteDynamicCount] {});
    gOwnedGraphicPaletteCache = std::move(pending);
    gGraphicDataCacheList = gOwnedGraphicPaletteCache.get();

    for (std::size_t index = 0; index < kGraphicPaletteDynamicCount; ++index) {
        gGraphicDataCacheList[index].imageNo = -1;
    }
    for (auto& item : gGraphicDataCacheStatic) {
        item.activePalIndex = -1;
    }
}

void ReleaseGraphicPaletteCache_44E110() noexcept
{
    gOwnedGraphicPaletteCache.reset();
    gGraphicDataCacheList = nullptr;
}

void AgeGraphicPaletteCache_44E230() noexcept
{
    for (std::size_t index = 0; index < kGraphicPaletteDynamicCount; ++index) {
        const std::int32_t imageNo = gGraphicDataCacheList[index].imageNo;
        if (imageNo < 0) {
            continue;
        }
        if (gGraphicDataCacheList[index].dirtyCount >= 1) {
            gGraphicDxCache103CFA4[static_cast<std::size_t>(imageNo)].palette = nullptr;
            gGraphicDataCacheList[index].imageNo = -1;
        }
        const std::int32_t count = gGraphicDataCacheList[index].dirtyCount;
        gGraphicDataCacheList[index].dirtyCount = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(count) + 1u);
    }
}

} // namespace cg::render
