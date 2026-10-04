#include "cg/render/GraphicPaletteCache.hpp"

#include "GraphicDxCacheState.hpp"
#include "cg/Palette.hpp"

#include <bit>
#include <cstring>
#include <memory>
#include <utility>

namespace cg::render {
namespace {

std::unique_ptr<PaletteCacheItem[]> gOwnedGraphicPaletteCache;

// 原 44E290 的 W32 色项：同步借用 RGB/BGR 源，先写颜色，纯黑再写 1。
void WriteColor32(
    PaletteCacheItem& item, const std::size_t index,
    const std::uint8_t* const source, const bool sourceIsBgr,
    const bool applyTone) noexcept
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    if (applyTone) {
        std::uint8_t* const redMap = gColorChannelMaps.red;
        const std::uint8_t sourceRed = source[sourceIsBgr ? 2 : 0];
        std::uint8_t* const blueMap = gColorChannelMaps.blue;
        red = redMap[sourceRed];
        std::uint8_t* const greenMap = gColorChannelMaps.green;
        green = greenMap[source[1]];
        blue = blueMap[source[sourceIsBgr ? 0 : 2]];
    } else {
        red = source[sourceIsBgr ? 2 : 0];
        blue = source[sourceIsBgr ? 0 : 2];
        green = source[1];
    }
    const std::uint32_t color =
        (static_cast<std::uint32_t>(red) << 16)
        | (static_cast<std::uint32_t>(green) << 8)
        | blue;
    std::uint8_t* const target = item.payload.data() + index * 4;
    std::memcpy(target, &color, sizeof(color));
    if (color == 0) {
        const std::uint32_t nonzeroBlack = 1;
        std::memcpy(target, &nonzeroBlack, sizeof(nonzeroBlack));
    }
}

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
    PaletteCacheItem* const pool = gGraphicDataCacheList;
    std::int32_t index = 0;
    while (index < static_cast<std::int32_t>(kGraphicPaletteDynamicCount)
        && pool[index].imageNo >= 0) {
        ++index;
    }
    if (index < static_cast<std::int32_t>(kGraphicPaletteDynamicCount)) {
        pool[index].imageNo = imageNo;
        gGraphicDataCacheList[index].dirtyCount = 0;
        return &gGraphicDataCacheList[index];
    }

    index = 0;
    while (index < static_cast<std::int32_t>(kGraphicPaletteDynamicCount)
        && pool[index].dirtyCount <= 0) {
        ++index;
    }
    if (index < static_cast<std::int32_t>(kGraphicPaletteDynamicCount)) {
        DxGraphicEntry* const dxBase = gGraphicDxCache103CFA4;
        const std::int32_t previousImageNo = pool[index].imageNo;
        dxBase[previousImageNo].palette = nullptr;
        gGraphicDataCacheList[index].imageNo = -1;
        gGraphicDataCacheList[index].imageNo = imageNo;
        gGraphicDataCacheList[index].dirtyCount = 0;
        return &gGraphicDataCacheList[index];
    }

    const std::int32_t previousImageNo = pool[0].imageNo;
    DxGraphicEntry* const dxBase = gGraphicDxCache103CFA4;
    dxBase[previousImageNo].palette = nullptr;
    gGraphicDataCacheList[0].imageNo = -1;
    gGraphicDataCacheList[0].imageNo = imageNo;
    gGraphicDataCacheList[0].dirtyCount = 0;
    return &gGraphicDataCacheList[0];
}

PaletteCacheItem* LoadGraphicPalette32_44E290(
    const std::int32_t imageNo,
    std::uint8_t mode,
    const std::uint8_t applyTone,
    const std::uint8_t* const paletteTriples,
    const std::uint8_t* const paletteQuads) noexcept
{
    if (paletteTriples == nullptr && paletteQuads == nullptr) {
        mode = 0;
    }

    const PaletteTone neutral {128, 128, 128, 0};
    const PaletteTone* tone = gPaletteTonePointer;
    if (tone == nullptr) {
        tone = &neutral;
    }
    if (applyTone == 0) {
        tone = &neutral;
    }

    PaletteCacheItem* item;
    const PaletteEntry* sourcePalette = nullptr;
    if (mode <= 1) {
        const bool neutralTone =
            tone->blue == 128 && tone->green == 128 && tone->red == 128;
        const std::size_t staticIndex = mode == 0
            ? (neutralTone ? 0u : 2u)
            : (neutralTone ? 1u : 3u);
        item = &gGraphicDataCacheStatic[staticIndex];
        sourcePalette = mode == 0 ? gPaletteBackup : gPalette;

        if (staticIndex == 0) {
            if (item->activePalIndex == 0) {
                return item;
            }
            item->activePalIndex = 0;
            item->frameIndex = static_cast<std::uint32_t>(gPaletteFrameVersion);
        } else {
            const std::int32_t activePalette = gActivePalette;
            const std::int32_t previousPalette = item->activePalIndex;
            if (previousPalette == activePalette) {
                const std::uint32_t previousFrame = item->frameIndex;
                if (previousFrame == static_cast<std::uint32_t>(gPaletteFrameVersion)) {
                    return item;
                }
            }
            item->activePalIndex = activePalette;
            item->frameIndex = static_cast<std::uint32_t>(gPaletteFrameVersion);
        }

        for (std::size_t index = 0; index < 16; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index,
                reinterpret_cast<const std::uint8_t*>(&color), false, false);
        }
        for (std::size_t index = 16; index < 240; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index,
                reinterpret_cast<const std::uint8_t*>(&color), false, applyTone != 0);
        }
        for (std::size_t index = 240; index < 256; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index,
                reinterpret_cast<const std::uint8_t*>(&color), false, false);
        }
    } else {
        item = TakeIndexCacheFromPool_44E150(imageNo);
        if (item == nullptr) {
            return nullptr;
        }
        item->activePalIndex = 0;
        item->frameIndex = 0;
        if (applyTone != 0) {
            item->activePalIndex = gActivePalette;
            item->frameIndex = static_cast<std::uint32_t>(gPaletteFrameVersion);
        }
        const std::uint8_t* const source =
            paletteTriples != nullptr ? paletteTriples : paletteQuads;
        const std::size_t stride = paletteTriples != nullptr ? 3u : 4u;
        for (std::size_t index = 0; index < 256; ++index) {
            const std::uint8_t* const color = source + index * stride;
            WriteColor32(*item, index, color, true, applyTone != 0);
        }
    }

    const std::uint32_t transparent = 0;
    std::memcpy(item->payload.data(), &transparent, sizeof(transparent));
    return item;
}

void AgeGraphicPaletteCache_44E230() noexcept
{
    PaletteCacheItem* pool = gGraphicDataCacheList;
    for (std::int32_t index = 0;
         index < static_cast<std::int32_t>(kGraphicPaletteDynamicCount); ++index) {
        const std::int32_t imageNo = pool[index].imageNo;
        if (imageNo < 0) {
            continue;
        }
        if (pool[index].dirtyCount >= 1) {
            gGraphicDxCache103CFA4[static_cast<std::size_t>(imageNo)].palette = nullptr;
            gGraphicDataCacheList[index].imageNo = -1;
            pool = gGraphicDataCacheList;
        }
        const std::int32_t count = pool[index].dirtyCount;
        pool[index].dirtyCount = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(count) + 1u);
        pool = gGraphicDataCacheList;
    }
}

} // namespace cg::render
