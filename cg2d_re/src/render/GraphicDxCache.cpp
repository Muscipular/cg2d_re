#include "GraphicDxCacheState.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/GraphicRead.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>

namespace cg::render {

struct TextureSurface {
    std::int16_t width {};
    std::int16_t height {};
    std::uint32_t colorKey {};
    std::unique_ptr<std::uint32_t[]> pixels;
    TextureSurface* nextOwned {};
    TextureSurface* previousOwned {};
};

namespace {

class TextureSurfaceOwner final {
public:
    ~TextureSurfaceOwner()
    {
        while (head_ != nullptr) {
            TextureSurface* next = head_->nextOwned;
            delete head_;
            head_ = next;
        }
    }

    TextureSurface* Create(std::int16_t width, std::int16_t height) noexcept
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        auto* surface = new (std::nothrow) TextureSurface {};
        if (surface == nullptr) {
            return nullptr;
        }
        const std::size_t pixelCount =
            static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        surface->pixels.reset(new (std::nothrow) std::uint32_t[pixelCount] {});
        if (surface->pixels == nullptr) {
            delete surface;
            return nullptr;
        }
        surface->width = width;
        surface->height = height;
        surface->nextOwned = head_;
        if (head_ != nullptr) {
            head_->previousOwned = surface;
        }
        head_ = surface;
        return surface;
    }

    void Release(TextureSurface* surface) noexcept
    {
        if (surface->previousOwned != nullptr) {
            surface->previousOwned->nextOwned = surface->nextOwned;
        } else {
            head_ = surface->nextOwned;
        }
        if (surface->nextOwned != nullptr) {
            surface->nextOwned->previousOwned = surface->previousOwned;
        }
        delete surface;
    }

private:
    TextureSurface* head_ {};
};

TextureSurfaceOwner gTextureSurfaceOwner;

} // namespace

DxGraphicEntry* gGraphicDxCache103CFA4 = nullptr;
std::array<TextureNode, kTextureNodeCapacity> gTextureNodes {};
std::int32_t gTextureNodeCount = 0;
std::int32_t gTextureScanIndex = 0;
std::int32_t gTexturePreferredCount = 0;
std::int32_t gTextureFallbackCount = 0;
std::int32_t gTextureSizeW = 0;
std::int32_t gTextureSizeH = 0;

void ResetTextureNodes_523310() noexcept
{
    gTextureScanIndex = 0;
    for (std::int32_t index = 0; index < gTextureNodeCount; ++index) {
        auto& node = gTextureNodes[static_cast<std::size_t>(index)];
        node.imageNo = -1;
        node.reuseState = 0;
        node.next = nullptr;
    }
}

std::int32_t InvalidateGraphicRefs_523350() noexcept
{
    const std::int32_t count = resource::gMaxGraphicCount;
    if (count <= 0) {
        return count;
    }

    std::int32_t processed = 0;
    std::uint32_t legacyOffset = 0;
    do {
        const auto index = static_cast<std::size_t>(processed);
        DxGraphicEntry* headBase = gGraphicDxCache103CFA4;
        ++processed;
        headBase[index].head = nullptr;

        DxGraphicEntry* paletteBase = gGraphicDxCache103CFA4;
        paletteBase[index].palette = nullptr;
        const std::int32_t currentCount = resource::gMaxGraphicCount;
        legacyOffset += 0x10;
        if (processed >= currentCount) {
            break;
        }
    } while (true);

    return static_cast<std::int32_t>(legacyOffset);
}

std::int32_t LoadGraphic_522570(const std::int32_t imageNo)
{
    if (imageNo < 0 || imageNo >= resource::gMaxGraphicCount) {
        return 0;
    }
    if (gGraphicDxCache103CFA4[static_cast<std::size_t>(imageNo)].head != nullptr) {
        return 1;
    }
    if (resource::ReadGraphicDataByIndex(
            imageNo,
            &resource::gGraphicDataPtr,
            &resource::gCurrentGraphicWidth,
            &resource::gCurrentGraphicHeight,
            &resource::gCurrentGraphicPaletteSize) == 0) {
        resource::gGraphicDataBuffer = -1;
        resource::gGraphicDataPtr = nullptr;
        resource::gCurrentGraphicHeight = 0;
        resource::gCurrentGraphicWidth = 0;
        resource::gCurrentGraphicPaletteSize = 0;
        return 0;
    }

    resource::gGraphicDataBuffer = imageNo;
    gGraphicDxCache103CFA4[static_cast<std::size_t>(imageNo)].width =
        static_cast<std::uint16_t>(resource::gCurrentGraphicWidth);
    gGraphicDxCache103CFA4[static_cast<std::size_t>(imageNo)].height =
        static_cast<std::uint16_t>(resource::gCurrentGraphicHeight);
    TakeGraphicDxCache_5222D0(imageNo, nullptr);
    return 1;
}

void TakeGraphicDxCache_5222D0(const std::int32_t imageNo, void* /*unusedSource*/)
{
    std::int32_t scan = gTextureScanIndex;
    const std::int32_t scanStart = scan;
    const auto width = static_cast<std::int32_t>(resource::gCurrentGraphicWidth);
    const auto height = static_cast<std::int32_t>(resource::gCurrentGraphicHeight);
    const std::int32_t columns = width / 64 + (width % 64 != 0 ? 1 : 0);
    const std::int32_t rows = height / 48 + (height % 48 != 0 ? 1 : 0);
    const std::int32_t required = columns * rows;
    if ((resource::gCurrentGraphicWidth & 3u) != 0) {
        resource::gCurrentGraphicWidth += (0u - resource::gCurrentGraphicWidth) & 3u;
    }

    DxGraphicEntry& entry = gGraphicDxCache103CFA4[imageNo];
    if (entry.head != nullptr) {
        TextureNode* old = entry.head;
        do {
            old->imageNo = -1;
            old = old->next;
        } while (old != nullptr);
        entry.head = nullptr;
        scan = gTextureScanIndex;
    }

    TextureNode* previous = nullptr;
    std::int32_t blockX = 0;
    std::int32_t remainingHeight = height;
    std::int32_t taken = 0;
    for (;;) {
        const TextureNode& candidate = gTextureNodes[static_cast<std::size_t>(scan)];
        if (candidate.reuseState < platform::gFrameCount - 2u) {
            if (candidate.imageNo != -1) {
                DxGraphicEntry& evicted = gGraphicDxCache103CFA4[candidate.imageNo];
                TextureNode* old = evicted.head;
                evicted.head = nullptr;
                while (old != nullptr) {
                    old->imageNo = -1;
                    old = old->next;
                }
                scan = gTextureScanIndex;
            }
            TextureNode* const selected = &gTextureNodes[static_cast<std::size_t>(scan)];
            if (entry.head == nullptr) {
                entry.head = selected;
            } else {
                previous->next = selected;
            }
            scan = gTextureScanIndex;
            TextureNode& node = gTextureNodes[static_cast<std::size_t>(scan)];
            node.paletteChoice = -1;
            node.paletteVersion = -1;
            node.paletteIndex = 0;
            node.imageNo = imageNo;
            node.blockX = static_cast<std::int16_t>(blockX);
            node.blockY = static_cast<std::int16_t>(height - remainingHeight);
            ++taken;
            if (taken >= required) {
                ++scan;
                gTextureScanIndex = scan;
                node.next = nullptr;
                if (scan >= gTextureNodeCount) {
                    gTextureScanIndex = 0;
                }
                return;
            }
            previous = &node;
            if (blockX < width - 64) {
                blockX += 64;
            } else {
                blockX = 0;
                remainingHeight -= 48;
            }
        }
        ++scan;
        gTextureScanIndex = scan;
        if (scan >= gTextureNodeCount) {
            scan = 0;
            gTextureScanIndex = 0;
        }
        if (scan == scanStart) {
            // 原 58D348 消息字节及 52253F 路径；不足时不清除部分链或补写尾 next。
            char message[256];
            std::sprintf(message,
                "\x83\x54\x81\x5B\x83\x74\x83\x46\x83\x58\x95\x73"
                "\x91\xAB\x82\xC5\x82\xB7\x81\x42%d\n", scan);
            platform::WriteLegacyDebugString(message);
            return;
        }
    }
}

std::int32_t LoadGraphicTexture_520B10(
    const GraphicCacheItem32* const graphic, TextureNode* const texture)
{
    std::int32_t paletteChoice = gActivePalette;
    std::int32_t paletteVersion = gPaletteFrameVersion;
    DxGraphicEntry& entry = gGraphicDxCache103CFA4[graphic->imageNo];
    const std::uint8_t mixMode = graphic->descriptor[0x0D];
    if ((mixMode & 4u) != 0) {
        paletteChoice = 0;
        paletteVersion = 0;
    }
    std::int16_t paletteIndex;
    std::memcpy(&paletteIndex, graphic->descriptor.data() + 0x0E, sizeof(paletteIndex));
    if (texture->paletteChoice == paletteChoice
        && texture->paletteVersion == paletteVersion
        && texture->paletteIndex == paletteIndex) {
        return 1;
    }
    if (gRenderQueueMode103CF10 != 0 && entry.flag != 0 && texture->paletteChoice >= 0) {
        if (gGraphicPaletteUploadCount >= 10) {
            return 1;
        }
        ++gGraphicPaletteUploadCount;
    }
    // 原 520B9F：限流分支之后再次核对同一组三字段。
    if (texture->paletteChoice == paletteChoice
        && texture->paletteVersion == paletteVersion
        && texture->paletteIndex == paletteIndex) {
        return 1;
    }
    if (resource::gGraphicDataBuffer != graphic->imageNo) {
        if (resource::ReadGraphicDataByIndex(
                graphic->imageNo, &resource::gGraphicDataPtr,
                &resource::gCurrentGraphicWidth, &resource::gCurrentGraphicHeight,
                &resource::gCurrentGraphicPaletteSize) == 0) {
            resource::gGraphicDataPtr = nullptr;
            resource::gCurrentGraphicHeight = 0;
            resource::gCurrentGraphicWidth = 0;
            resource::gCurrentGraphicPaletteSize = 0;
            resource::gGraphicDataBuffer = -1;
            return 0;
        }
        resource::gGraphicDataBuffer = graphic->imageNo;
    }

    const auto graphicWidth = static_cast<std::int16_t>(entry.width);
    const auto graphicHeight = static_cast<std::int16_t>(entry.height);
    std::int32_t width = gTextureSizeW;
    if (texture->blockX + width > graphicWidth) {
        width = graphicWidth - texture->blockX;
    }
    std::int32_t height = gTextureSizeH;
    if (texture->blockY + height > graphicHeight) {
        height = graphicHeight - texture->blockY;
    }
    if (width < gTextureSizeW || height < gTextureSizeH) {
        FillTextureBlack_451310(texture->surface);
    }

    const bool applyTone = (graphic->descriptor[0x0D] & 4u) == 0;
    std::memcpy(&paletteIndex, graphic->descriptor.data() + 0x0E, sizeof(paletteIndex));
    if (paletteIndex != 0) {
        const std::uint32_t* const colors =
            resource::BorrowIndependentPaletteColors32(paletteIndex, applyTone);
        if (colors != nullptr) {
            LoadTextureFromBuffer32_450400(
                texture->surface, texture->blockX, graphicHeight - texture->blockY - 1,
                width, height, resource::gGraphicDataPtr,
                static_cast<std::int32_t>(resource::gCurrentGraphicWidth),
                reinterpret_cast<const std::uint8_t*>(colors));
        }
    } else {
        if (entry.palette != nullptr
            && (entry.palette->activePalIndex != paletteChoice
                || entry.palette->frameIndex != static_cast<std::uint32_t>(paletteVersion))) {
            entry.palette->imageNo = -1;
            entry.palette = nullptr;
        }
        if (entry.palette == nullptr) {
            if (static_cast<std::int32_t>(resource::gCurrentGraphicPaletteSize) > 0) {
                entry.palette = LoadGraphicPalette32_44E290(
                    graphic->imageNo, 2, applyTone ? 1 : 0,
                    resource::gGraphicDataPtr
                        + resource::gCurrentGraphicWidth * resource::gCurrentGraphicHeight,
                    nullptr);
            } else if (!applyTone) {
                entry.palette = LoadGraphicPalette32_44E290(
                    graphic->imageNo, 0, 0, nullptr, nullptr);
            }
        }
        const std::uint8_t* const colors =
            entry.palette == nullptr ? nullptr : entry.palette->payload.data();
        LoadTextureFromBuffer32_450400(
            texture->surface, texture->blockX, graphicHeight - texture->blockY - 1,
            width, height, resource::gGraphicDataPtr,
            static_cast<std::int32_t>(resource::gCurrentGraphicWidth), colors);
        entry.flag = 0;
        if (static_cast<std::int32_t>(resource::gCurrentGraphicPaletteSize) > 0) {
            entry.flag = 1;
        }
    }
    texture->paletteChoice = static_cast<std::int16_t>(paletteChoice);
    texture->paletteVersion = static_cast<std::int16_t>(paletteVersion);
    std::memcpy(&texture->paletteIndex, graphic->descriptor.data() + 0x0E,
        sizeof(texture->paletteIndex));
    return 1;
}

std::int32_t BuildSurfaceCache_5231A0()
{
    gTextureNodeCount = 0;
    gTexturePreferredCount = 0;
    gTextureFallbackCount = 0;

    if (!CreateCpuAutoMapSurface()) {
        platform::ShowLegacyMessage(
            "AutoMap作業用Surface產生失敗。", "確認", 0x10);
        return 0;
    }

    if (!CreateCpuTempSurface() && !CreateCpuTempSurface()) {
        platform::ShowLegacyMessage(
            "建立戰鬥畫面錯誤", "確認", 0x10);
        return 0;
    }

    bool fallback = false;
    for (std::size_t index = 0; index < kTextureNodeCapacity; ++index) {
        if (!fallback) {
            const auto width = static_cast<std::int16_t>(gTextureSizeW);
            const auto height = static_cast<std::int16_t>(gTextureSizeH);
            gTextureNodes[index].surface = gTextureSurfaceOwner.Create(width, height);
            if (gTextureNodes[index].surface != nullptr) {
                ++gTexturePreferredCount;
                continue;
            }
            fallback = true;
        }

        const auto width = static_cast<std::int16_t>(gTextureSizeW);
        const auto height = static_cast<std::int16_t>(gTextureSizeH);
        gTextureNodes[index].surface = gTextureSurfaceOwner.Create(width, height);
        if (gTextureNodes[index].surface == nullptr) {
            platform::ShowLegacyMessage(
                "建立繪圖頁失敗", "確認", 0x10);
            return 0;
        }
        ++gTextureFallbackCount;
    }

    gTextureNodeCount = static_cast<std::int32_t>(kTextureNodeCapacity);
    ResetTextureNodes_523310();
    static_cast<void>(InvalidateGraphicRefs_523350());
    return 1;
}

void FillTextureBlack_451310(TextureSurface* const surface) noexcept
{
    const std::size_t pixelCount =
        static_cast<std::size_t>(surface->width) * static_cast<std::size_t>(surface->height);
    std::fill_n(surface->pixels.get(), pixelCount, 0u);
}

void LoadTextureFromBuffer32_450400(
    TextureSurface* const surface, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::uint8_t* const source, const std::int32_t sourceWidth,
    const std::uint8_t* const palette) noexcept
{
    if (height <= 0) {
        return;
    }

    const std::uint8_t* row = source + y * sourceWidth + x;
    std::uint32_t* target = surface->pixels.get();
    for (std::int32_t rowIndex = 0; rowIndex < height; ++rowIndex) {
        for (std::int32_t column = 0; column < width; ++column) {
            const std::uint8_t index = row[column];
            if (palette != nullptr) {
                std::memcpy(&target[column], palette + static_cast<std::size_t>(index) * 4, 4);
            } else {
                target[column] = gPaletteAutoMap[index];
            }
        }
        if (rowIndex + 1 < height) {
            row -= sourceWidth;
        }
        target += surface->width;
    }
}

void ReleaseTextureNodeSurfaces_44C408() noexcept
{
    if (gTextureNodeCount <= 0) {
        return;
    }
    std::int32_t index = 0;
    do {
        TextureNode& node = gTextureNodes[static_cast<std::size_t>(index)];
        if (node.surface != nullptr) {
            gTextureSurfaceOwner.Release(node.surface);
            node.surface = nullptr;
        }
        ++index;
    } while (index < gTextureNodeCount);
}

} // namespace cg::render
