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

PaletteCacheItem* TakeIndexCacheFromPool_44E150(const std::int32_t imageNo) noexcept
{
    std::size_t index = 0;
    while (index < kGraphicPaletteDynamicCount
        && gGraphicDataCacheList[index].imageNo >= 0) {
        ++index;
    }
    if (index < kGraphicPaletteDynamicCount) {
        gGraphicDataCacheList[index].imageNo = imageNo;
        gGraphicDataCacheList[index].dirtyCount = 0;
        return &gGraphicDataCacheList[index];
    }

    index = 0;
    while (index < kGraphicPaletteDynamicCount
        && gGraphicDataCacheList[index].dirtyCount <= 0) {
        ++index;
    }
    if (index < kGraphicPaletteDynamicCount) {
        const std::int32_t previousImageNo = gGraphicDataCacheList[index].imageNo;
        gGraphicDxCache103CFA4[static_cast<std::size_t>(previousImageNo)].palette = nullptr;
        gGraphicDataCacheList[index].imageNo = -1;
        gGraphicDataCacheList[index].imageNo = imageNo;
        gGraphicDataCacheList[index].dirtyCount = 0;
        return &gGraphicDataCacheList[index];
    }

    const std::int32_t previousImageNo = gGraphicDataCacheList[0].imageNo;
    gGraphicDxCache103CFA4[static_cast<std::size_t>(previousImageNo)].palette = nullptr;
    gGraphicDataCacheList[0].imageNo = -1;
    gGraphicDataCacheList[0].imageNo = imageNo;
    gGraphicDataCacheList[0].dirtyCount = 0;
    return &gGraphicDataCacheList[0];
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
