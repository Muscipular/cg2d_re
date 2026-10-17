#include "cg/render/GraphicPendingQueue.hpp"

#include "CanvasInternal.hpp"
#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"
#include "PngImageInternal.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/WorldMapState.hpp"

#include <bit>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <SDL3/SDL_surface.h>

namespace cg::render {

std::int32_t gLastUnrefreshedPaletteFrameVersion = -1;

static constexpr auto kSurfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);

// R03／LUR01／GRT01：直接提交整图缓存，不再先切块再拼接全屏背景。
static void DrawFullscreenBackground(GraphicCacheItem32* const graphic)
{
    if (graphic->imageNo < 0 || gGraphicDxCache103CFA4 == nullptr) { return; }
    static_cast<void>(LoadGraphicIndependentPalette_523140(graphic));
    if (LoadGraphic_522570(graphic->imageNo) == 0) { return; }
    const auto& entry = gGraphicDxCache103CFA4[graphic->imageNo];
    TextureNode* const node = entry.head;
    if (node == nullptr || LoadGraphicTexture_520B10(graphic, node) == 0) { return; }
    node->reuseState = platform::gFrameCount;
    const std::int32_t width = std::bit_cast<std::int16_t>(entry.width);
    const std::int32_t height = std::bit_cast<std::int16_t>(entry.height);
    TextureRectangle source {0, 0, width, height};
    if (graphic->descriptor.clipMode == 1) {
        source[0] = std::max(0, static_cast<std::int32_t>(graphic->descriptor.sourceX));
        source[1] = std::max(0, static_cast<std::int32_t>(graphic->descriptor.sourceY));
        source[2] = std::min(width, graphic->descriptor.sourceX + graphic->descriptor.sourceWidth);
        source[3] = std::min(height, graphic->descriptor.sourceY + graphic->descriptor.sourceHeight);
    }
    if (source[2] <= source[0] || source[3] <= source[1]) { return; }
    if (graphic->batchType == GraphicBatch_FullscreenBackgroundTrimmed) {
        // LUR01：选角／选服背景含透明尾列；只裁外围透明区，不改纹理及内部透明像素。
        const SDL_Surface* const surface = GetTextureSdlSurface(node->surface);
        const auto* const pixels = static_cast<const std::uint32_t*>(surface->pixels);
        const std::int32_t stride = surface->pitch / static_cast<std::int32_t>(sizeof(std::uint32_t));
        const auto transparentColumn = [&](const std::int32_t x) {
            for (std::int32_t y = source[1]; y < source[3]; ++y) {
                if ((pixels[y * stride + x] & 0xFF000000u) != 0) { return false; }
            }
            return true;
        };
        const auto transparentRow = [&](const std::int32_t y) {
            for (std::int32_t x = source[0]; x < source[2]; ++x) {
                if ((pixels[y * stride + x] & 0xFF000000u) != 0) { return false; }
            }
            return true;
        };
        while (source[0] < source[2] && transparentColumn(source[0])) { ++source[0]; }
        while (source[0] < source[2] && transparentColumn(source[2] - 1)) { --source[2]; }
        if (source[0] == source[2]) { return; }
        while (source[1] < source[3] && transparentRow(source[1])) { ++source[1]; }
        while (source[1] < source[3] && transparentRow(source[3] - 1)) { --source[3]; }
        if (source[1] == source[3]) { return; }
    }
    const TextureRectangle destination {0, 0, GetCpuSurfaceWidth(), GetCpuSurfaceHeight()};
    if (RenderImageTexture(node->surface, source, destination,
            graphic->descriptor.mixFlags, graphic->descriptor.color) == kSurfaceBusy) {
        gGraphicDirty = 1;
    }
}

// 原三条纹理路径相同的 left/right/top/bottom 读写顺序；不冻结节点偏移或共享尺寸。
static TextureRectangle MakeNodeRectangle(
    const TextureNode* const node, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    TextureRectangle rectangle;
    rectangle[0] = 0;
    const std::int32_t leftBlockX = node->blockX;
    if (leftBlockX < x) {
        rectangle[0] = x - leftBlockX;
    }
    rectangle[2] = GetTextureSdlSurface(node->surface)->w;
    const std::int32_t rightBlockX = node->blockX;
    if (rightBlockX + rectangle[2] > x + width) {
        rectangle[2] = x + width - rightBlockX;
    }
    rectangle[1] = 0;
    const std::int32_t topBlockY = node->blockY;
    if (topBlockY < y) {
        rectangle[1] = y - topBlockY;
    }
    rectangle[3] = GetTextureSdlSurface(node->surface)->h;
    const std::int32_t bottomBlockY = node->blockY;
    if (bottomBlockY + rectangle[3] > y + height) {
        rectangle[3] = y + height - bottomBlockY;
    }
    return rectangle;
}

// 原 5211E7 普通纹理链；上传失败仍继续，next 只在当前节点处理后读取。
static void DrawIntegerNodes(
    GraphicCacheItem32* const graphic, const std::int32_t imageNo,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    TextureNode* const head)
{
    for (TextureNode* node = head; node != nullptr; node = node->next) {
        const std::uint32_t frame = platform::gFrameCount;
        const std::int32_t blockX = node->blockX;
        node->reuseState = frame;
        if (blockX + GetTextureSdlSurface(node->surface)->w < x) {
            continue;
        }
        const std::int32_t blockY = node->blockY;
        if (blockY + GetTextureSdlSurface(node->surface)->h < y
            || node->blockX > x + width || blockY > y + height) {
            continue;
        }
        if (LoadGraphicTexture_520B10(graphic, node) == 0) {
            continue;
        }
        const TextureRectangle rectangle = MakeNodeRectangle(node, x, y, width, height);
        const std::int32_t originX = graphic->x;
        const std::uint8_t mixFlags = graphic->descriptor.mixFlags;
        std::int32_t offsetX;
        if ((mixFlags & GraphicMixFlag_FlipX) != 0) {
            const std::int32_t fullWidth = ReadGraphicDxWidth(imageNo);
            offsetX = fullWidth - node->blockX - rectangle[2] + x;
        } else {
            offsetX = node->blockX - x + rectangle[0];
        }
        const std::int32_t destinationX = originX + offsetX;
        const std::int32_t originY = graphic->y;
        std::int32_t offsetY;
        if ((mixFlags & GraphicMixFlag_FlipY) != 0) {
            const std::int32_t fullHeight = ReadGraphicDxHeight(imageNo);
            offsetY = fullHeight - node->blockY - rectangle[3] + y;
        } else {
            offsetY = node->blockY - y + rectangle[1];
        }
        const std::int32_t destinationY = originY + offsetY;
        std::uint32_t color;
        color = graphic->descriptor.color;
        const std::int16_t paletteIndex = node->paletteIndex;
        const std::int32_t nodeImageNo = node->imageNo;
        TextureSurface* const surface = node->surface;
        if (RenderTextureToSurface_44D520(
                destinationX, destinationY,
                rectangle, surface, nodeImageNo, paletteIndex, mixFlags, color) == kSurfaceBusy) {
            gGraphicDirty = 1;
        }
    }
}

// 原 520FC0/521522 共用的缩放链；固定比例不读取 action，动态比例按原位置逐轴取值。
static void DrawScaledNodes(
    GraphicCacheItem32* const graphic, const std::int32_t imageNo,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const bool useActionScale, const float fixedScale, TextureNode* const head)
{
    for (TextureNode* node = head; node != nullptr; node = node->next) {
        const std::uint32_t frame = platform::gFrameCount;
        const std::int32_t blockY = node->blockY;
        node->reuseState = frame;
        if (blockY + GetTextureSdlSurface(node->surface)->h < y || node->blockY > y + height) {
            continue;
        }
        const std::int32_t blockX = node->blockX;
        if (blockX + GetTextureSdlSurface(node->surface)->w < x || blockX > x + width) {
            continue;
        }
        if (LoadGraphicTexture_520B10(graphic, node) == 0) {
            continue;
        }
        const TextureRectangle rectangle = MakeNodeRectangle(node, x, y, width, height);
        const std::uint8_t mixFlags = graphic->descriptor.mixFlags;
        const std::int32_t originX = graphic->x;
        const cg::yobi::YobiAction* const action = useActionScale ? graphic->reference.action : nullptr;
        float scaleX = fixedScale;
        std::int32_t offsetX;
        if ((mixFlags & GraphicMixFlag_FlipX) != 0) {
            if (useActionScale) {
                scaleX = cg::yobi::GetYobiActionScaleX(action);
            }
            const std::int32_t fullWidth = ReadGraphicDxWidth(imageNo);
            offsetX = fullWidth - node->blockX - rectangle[2] + x;
        } else {
            const std::int32_t currentBlockX = node->blockX;
            if (useActionScale) {
                scaleX = cg::yobi::GetYobiActionScaleX(action);
            }
            offsetX = currentBlockX - x + rectangle[0];
        }
        const auto scaledX = static_cast<std::int64_t>(static_cast<double>(offsetX) * scaleX);
        const std::int32_t destinationX = originX + static_cast<std::int32_t>(scaledX);
        const std::int32_t originY = graphic->y;
        float scaleY = fixedScale;
        std::int32_t offsetY;
        if ((mixFlags & GraphicMixFlag_FlipY) != 0) {
            if (useActionScale) {
                scaleY = cg::yobi::GetYobiActionScaleY(action);
            }
            const std::int32_t fullHeight = ReadGraphicDxHeight(imageNo);
            offsetY = fullHeight - node->blockY - rectangle[3] + y;
        } else {
            const std::int32_t currentBlockY = node->blockY;
            if (useActionScale) {
                scaleY = cg::yobi::GetYobiActionScaleY(action);
            }
            offsetY = currentBlockY - y + rectangle[1];
        }
        const auto scaledY = static_cast<std::int64_t>(static_cast<double>(offsetY) * scaleY);
        std::uint32_t color;
        color = graphic->descriptor.color;
        const std::int32_t nodeImageNo = node->imageNo;
        const std::int32_t destinationY = originY + static_cast<std::int32_t>(scaledY);
        // 两条原分支在 paletteIndex/surface 的读取次序不同。
        TextureSurface* surface;
        std::int16_t paletteIndex;
        if (useActionScale) {
            surface = node->surface;
            paletteIndex = node->paletteIndex;
        } else {
            paletteIndex = node->paletteIndex;
            surface = node->surface;
        }
        if (RenderTexture_44DA90(
                static_cast<std::int16_t>(destinationX), static_cast<std::int16_t>(destinationY),
                &rectangle, scaleX, scaleY, surface, nodeImageNo, paletteIndex, mixFlags, color)
            == kSurfaceBusy) {
            gGraphicDirty = 1;
        }
    }
}

// 原 520E54 的全图/裁剪入口；早退不清调色板 dirtyCount。
static void DrawSprite(GraphicCacheItem32* const graphic)
{
    const std::int32_t imageNo = graphic->imageNo;
    if (imageNo < 0 || imageNo >= resource::gMaxGraphicCount) {
        return;
    }
    static_cast<void>(LoadGraphicIndependentPalette_523140(graphic));
    if (LoadGraphic_522570(imageNo) == 0) {
        return;
    }
    if (graphic->batchType == 0) {
        graphic->descriptor.sourceX = 0;
        graphic->descriptor.sourceY = 0;
        const std::int16_t fullWidth = ReadGraphicDxWidth(imageNo);
        graphic->descriptor.sourceWidth = fullWidth;
        const std::int16_t fullHeight = ReadGraphicDxHeight(imageNo);
        graphic->descriptor.sourceHeight = fullHeight;
    }
    std::int16_t sourceWidth;
    std::int16_t sourceX;
    std::int16_t sourceY;
    std::int16_t sourceHeight;
    sourceWidth = graphic->descriptor.sourceWidth;
    sourceX = graphic->descriptor.sourceX;
    sourceY = graphic->descriptor.sourceY;
    sourceHeight = graphic->descriptor.sourceHeight;
    std::int32_t width = sourceWidth;
    std::int32_t x = sourceX;
    std::int32_t y = sourceY;
    std::int32_t height = sourceHeight;
    if (x + width < 0 || y + height < 0) {
        return;
    }
    DxGraphicEntry* dxBase = gGraphicDxCache103CFA4;
    const std::int32_t initialWidth =
        std::bit_cast<std::int16_t>(dxBase[imageNo].width);
    if (x > initialWidth) {
        return;
    }
    const std::int32_t initialHeight =
        std::bit_cast<std::int16_t>(dxBase[imageNo].height);
    if (y > initialHeight) {
        return;
    }
    if (x < 0) {
        const std::int32_t originX = graphic->x;
        width += x;
        graphic->x = originX - x;
        x = 0;
        dxBase = gGraphicDxCache103CFA4;
    }
    if (y < 0) {
        const std::int32_t originY = graphic->y;
        height += y;
        graphic->y = originY - y;
        y = 0;
        dxBase = gGraphicDxCache103CFA4;
    }
    const std::int32_t fullWidth =
        std::bit_cast<std::int16_t>(dxBase[imageNo].width);
    if (x + width > fullWidth) {
        width = fullWidth - x;
    }
    const std::int32_t fullHeight =
        std::bit_cast<std::int16_t>(dxBase[imageNo].height);
    if (y + height > fullHeight) {
        height = fullHeight - y;
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    bool integerPath = true;
    const cg::yobi::YobiAction* const action = graphic->reference.action;
    if (action != nullptr) {
        const float scaleX = cg::yobi::GetYobiActionScaleX(action);
        // 原只测 x87 C3；unordered 与相等同支，X 未命中时不得读取 Y。
        integerPath = scaleX == 1.0f || std::isnan(scaleX);
        if (integerPath) {
            const float scaleY = cg::yobi::GetYobiActionScaleY(action);
            integerPath = scaleY == 1.0f || std::isnan(scaleY);
        }
    }
    TextureNode* const head = dxBase[imageNo].head;
    if (head != nullptr) {
        if (integerPath) {
            DrawIntegerNodes(graphic, imageNo, x, y, width, height, head);
        } else {
            DrawScaledNodes(graphic, imageNo, x, y, width, height, true, 1.0f, head);
        }
        dxBase = gGraphicDxCache103CFA4;
    }
    PaletteCacheItem* const palette = dxBase[imageNo].palette;
    if (palette != nullptr) {
        palette->dirtyCount = 0;
    }
}

// 原 52143C：固定三比例，不以构造虚拟 action 复用对象入口。
static void DrawFixedScaleSprite(GraphicCacheItem32* const graphic, const float scale)
{
    const std::int32_t imageNo = graphic->imageNo;
    if (imageNo < 0 || imageNo >= resource::gMaxGraphicCount) {
        return;
    }
    static_cast<void>(LoadGraphicIndependentPalette_523140(graphic));
    if (LoadGraphic_522570(imageNo) == 0) {
        return;
    }
    graphic->descriptor.sourceX = 0;
    graphic->descriptor.sourceY = 0;
    const std::int16_t fullWidth = ReadGraphicDxWidth(imageNo);
    graphic->descriptor.sourceWidth = fullWidth;
    std::int32_t width = fullWidth;
    const std::int16_t fullHeight = ReadGraphicDxHeight(imageNo);
    const std::int32_t height = fullHeight;
    graphic->descriptor.sourceHeight = fullHeight;
    if (width < 0 || height < 0) {
        return;
    }
    DxGraphicEntry* dxBase = gGraphicDxCache103CFA4;
    const std::int32_t currentWidth =
        std::bit_cast<std::int16_t>(dxBase[imageNo].width);
    if (currentWidth < 0 || height < 0) {
        return;
    }
    if (width > currentWidth) {
        width = currentWidth;
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    TextureNode* const head = dxBase[imageNo].head;
    if (head != nullptr) {
        DrawScaledNodes(graphic, imageNo, 0, 0, width, height, false, scale, head);
        dxBase = gGraphicDxCache103CFA4;
    }
    PaletteCacheItem* const palette = dxBase[imageNo].palette;
    if (palette != nullptr) {
        palette->dirtyCount = 0;
    }
}

// 原函数：0x00520DD0。
void DrawAllQueued_520DD0()
{
    gGraphicPaletteUploadCount = 0;
    GraphicCache3* order = gLegacyPendingStorage.cache3.data();
    if (GetCpuBackSdlSurface() == nullptr) {
        return;
    }
    if (HasLegacy3DDevice_44B610() != 0) {
        static_cast<void>(BeginLegacyTextureScene(GetRendererTextureDevice()));
    }
    std::int32_t index = 0;
    if (std::bit_cast<std::int16_t>(platform::gPendingGraphicCount) > 0) {
        for (;;) {
            GraphicCacheItem32* const graphic =
                gLegacyPendingStorage.records.data() + order->cacheIndex;
            const std::uint8_t batchType = graphic->batchType;
            switch (batchType) {
            case 0:
            case 1:
                DrawSprite(graphic);
                break;
            case 2:
                Sub_522210(graphic, std::bit_cast<std::uint32_t>(graphic->imageNo));
                break;
            case 3:
                FillDirtyArea_520AD0(graphic);
                break;
            case 4:
                RenderTextToSurface_456D70(graphic->reference.textToken);
                break;
            case 5: {
                const std::int32_t y = graphic->y;
                const std::int32_t x = graphic->x;
                static_cast<void>(world::Sub_46C760(x, y));
                break;
            }
            case 6:
                if (world::gValueC82204 != 0) {
                    const auto offsetX = std::bit_cast<std::int32_t>(world::gValueC3500C);
                    std::int32_t destinationX = 0;
                    std::int32_t destinationY = 0;
                    TextureRectangle rectangle {0, 0, GetCpuSurfaceWidth(), GetCpuSurfaceHeight()};
                    if (offsetX > 0) {
                        rectangle[2] = GetCpuSurfaceWidth() - offsetX;
                        destinationX = offsetX;
                    } else if (offsetX < 0) {
                        rectangle[0] = -offsetX;
                    }
                    const auto offsetY = std::bit_cast<std::int32_t>(world::gValueC75010);
                    if (offsetY > 0) {
                        rectangle[3] = GetCpuSurfaceHeight() - offsetY;
                        destinationY = offsetY;
                    } else if (offsetY < 0) {
                        rectangle[1] = -offsetY;
                    }
                    static_cast<void>(BlitTempRectangleToBack(destinationX, destinationY, rectangle));
                }
                break;
            case 7:
                if (world::gValueC82204 != 0) {
                    static_cast<void>(CopyBackToTemp());
                }
                break;
            case 8:
                static_cast<void>(Sub_521870());
                break;
            case 9:
                world::Sub_46E480();
                break;
            case 10:
                DrawFixedScaleSprite(graphic, 0.25f);
                break;
            case 11:
                DrawFixedScaleSprite(graphic, 0.5f);
                break;
            case 12:
                DrawFixedScaleSprite(graphic, 0.75f);
                break;
            case GraphicBatch_Rectangle: {
                const auto xBits = std::bit_cast<std::uint32_t>(graphic->x);
                const auto yBits = std::bit_cast<std::uint32_t>(graphic->y);
                const TextureRectangle rectangle {
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(xBits)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(yBits)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(xBits >> 16)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(yBits >> 16))
                };
                static_cast<void>(RenderColorRectangle(rectangle, graphic->descriptor.color));
                break;
            }
            case GraphicBatch_PngImage:
                DrawPngImageBatch(*graphic->reference.pngImage);
                break;
            case GraphicBatch_FullscreenBackground:
            case GraphicBatch_FullscreenBackgroundTrimmed:
                DrawFullscreenBackground(graphic);
                break;
            case GraphicBatch_Canvas:
                DrawCanvasBatch(*graphic->reference.canvas);
                break;
            default:
                break;
            }
            const std::int16_t count = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
            ++index;
            ++order;
            if (index >= count) {
                break;
            }
        }
    }
    if (HasLegacy3DDevice_44B610() != 0) {
        static_cast<void>(EndLegacyTextureScene(GetRendererTextureDevice()));
    }
    if (gGraphicPaletteUploadCount == 0) {
        gLastUnrefreshedPaletteFrameVersion = gPaletteFrameVersion;
    }
    AgeGraphicPaletteCache_44E230();
    gSpecialGraphicPending = 0;
}

} // namespace cg::render
