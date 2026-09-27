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

void WriteColor32(
    PaletteCacheItem& item, const std::size_t index,
    std::uint8_t blue, std::uint8_t green, std::uint8_t red,
    const bool applyTone) noexcept
{
    if (applyTone) {
        blue = gColorChannelMaps.blue[blue];
        green = gColorChannelMaps.green[green];
        red = gColorChannelMaps.red[red];
    }
    const std::uint32_t color =
        (static_cast<std::uint32_t>(red) << 16)
        | (static_cast<std::uint32_t>(green) << 8)
        | blue;
    const std::uint32_t stored = color == 0 ? 1u : color;
    std::memcpy(item.payload.data() + index * 4, &stored, sizeof(stored));
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

    PaletteCacheItem* item;
    const PaletteEntry* sourcePalette = nullptr;
    if (mode <= 1) {
        const PaletteTone neutral {128, 128, 128, 0};
        const PaletteTone* const tone =
            applyTone != 0 && gPaletteTonePointer != nullptr
            ? gPaletteTonePointer : &neutral;
        const bool neutralTone =
            tone->red == 128 && tone->green == 128 && tone->blue == 128;
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
            if (item->activePalIndex == gActivePalette
                && item->frameIndex == static_cast<std::uint32_t>(gPaletteFrameVersion)) {
                return item;
            }
            item->activePalIndex = gActivePalette;
            item->frameIndex = static_cast<std::uint32_t>(gPaletteFrameVersion);
        }

        for (std::size_t index = 0; index < 16; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index, color.blue, color.green, color.red, false);
        }
        for (std::size_t index = 16; index < 240; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index, color.blue, color.green, color.red, applyTone != 0);
        }
        for (std::size_t index = 240; index < 256; ++index) {
            const PaletteEntry& color = sourcePalette[index];
            WriteColor32(*item, index, color.blue, color.green, color.red, false);
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
            WriteColor32(*item, index, color[0], color[1], color[2], applyTone != 0);
        }
    }

    const std::uint32_t transparent = 0;
    std::memcpy(item->payload.data(), &transparent, sizeof(transparent));
    return item;
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
