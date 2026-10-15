#include "cg/render/GraphicPendingQueue.hpp"

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cmath>
#include <cstring>

namespace cg::render {

std::int32_t gLastUnrefreshedPaletteFrameVersion = -1;

namespace {

constexpr auto kSurfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);
constexpr std::uint8_t kFlipX = 1;
constexpr std::uint8_t kFlipY = 2;

// 原三条纹理路径相同的 left/right/top/bottom 读写顺序；不冻结节点偏移或共享尺寸。
TextureRectangle MakeNodeRectangle(
    const TextureNode* const node, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    TextureRectangle rectangle;
    rectangle[0] = 0;
    const std::int32_t leftBlockX = node->blockX;
    if (leftBlockX < x) {
        rectangle[0] = x - leftBlockX;
    }
    rectangle[2] = gTextureSizeW;
    const std::int32_t rightBlockX = node->blockX;
    if (rightBlockX + rectangle[2] > x + width) {
        rectangle[2] = x + width - rightBlockX;
    }
    rectangle[1] = 0;
    const std::int32_t topBlockY = node->blockY;
    if (topBlockY < y) {
        rectangle[1] = y - topBlockY;
    }
    rectangle[3] = gTextureSizeH;
    const std::int32_t bottomBlockY = node->blockY;
    if (bottomBlockY + rectangle[3] > y + height) {
        rectangle[3] = y + height - bottomBlockY;
    }
    return rectangle;
}

// 原 5211E7 普通纹理链；上传失败仍继续，next 只在当前节点处理后读取。
void DrawIntegerNodes(
    GraphicCacheItem32* const graphic, const std::int32_t imageNo,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    TextureNode* const head)
{
    for (TextureNode* node = head; node != nullptr; node = node->next) {
        const std::uint32_t frame = platform::gFrameCount;
        const std::int32_t blockX = node->blockX;
        node->reuseState = frame;
        if (blockX + gTextureSizeW < x) {
            continue;
        }
        const std::int32_t blockY = node->blockY;
        if (blockY + gTextureSizeH < y
            || node->blockX > x + width || blockY > y + height) {
            continue;
        }
        if (LoadGraphicTexture_520B10(graphic, node) == 0) {
            continue;
        }
        const TextureRectangle rectangle = MakeNodeRectangle(node, x, y, width, height);
        const std::int32_t originX = graphic->x;
        const std::uint8_t mixMode = graphic->descriptor[0x0D];
        std::int32_t offsetX;
        if ((mixMode & kFlipX) != 0) {
            const std::int32_t fullWidth = ReadGraphicDxWidth(imageNo);
            offsetX = fullWidth - node->blockX - rectangle[2] + x;
        } else {
            offsetX = node->blockX - x + rectangle[0];
        }
        const std::int32_t destinationX = originX + offsetX;
        const std::int32_t originY = graphic->y;
        std::int32_t offsetY;
        if ((mixMode & kFlipY) != 0) {
            const std::int32_t fullHeight = ReadGraphicDxHeight(imageNo);
            offsetY = fullHeight - node->blockY - rectangle[3] + y;
        } else {
            offsetY = node->blockY - y + rectangle[1];
        }
        const std::int32_t destinationY = originY + offsetY;
        std::uint32_t color;
        std::memcpy(&color, graphic->descriptor.data() + 0x08, sizeof(color));
        const std::int16_t paletteIndex = node->paletteIndex;
        const std::int32_t nodeImageNo = node->imageNo;
        TextureSurface* const surface = node->surface;
        if (RenderTextureToSurface_44D520(
                destinationX, destinationY,
                rectangle, surface, nodeImageNo, paletteIndex, mixMode, color) == kSurfaceBusy) {
            gGraphicDirty = 1;
        }
    }
}

// 原 520FC0/521522 共用的缩放链；固定比例不读取 action，动态比例按原位置逐轴取值。
void DrawScaledNodes(
    GraphicCacheItem32* const graphic, const std::int32_t imageNo,
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const bool useActionScale, const float fixedScale, TextureNode* const head)
{
    for (TextureNode* node = head; node != nullptr; node = node->next) {
        const std::uint32_t frame = platform::gFrameCount;
        const std::int32_t blockY = node->blockY;
        node->reuseState = frame;
        if (blockY + gTextureSizeH < y || node->blockY > y + height) {
            continue;
        }
        const std::int32_t blockX = node->blockX;
        if (blockX + gTextureSizeW < x || blockX > x + width) {
            continue;
        }
        if (LoadGraphicTexture_520B10(graphic, node) == 0) {
            continue;
        }
        const TextureRectangle rectangle = MakeNodeRectangle(node, x, y, width, height);
        const std::uint8_t mixMode = graphic->descriptor[0x0D];
        const std::int32_t originX = graphic->x;
        const login::YobiAction* const action = useActionScale ? graphic->reference.action : nullptr;
        float scaleX = fixedScale;
        std::int32_t offsetX;
        if ((mixMode & kFlipX) != 0) {
            if (useActionScale) {
                scaleX = login::detail::GetYobiActionScaleX(action);
            }
            const std::int32_t fullWidth = ReadGraphicDxWidth(imageNo);
            offsetX = fullWidth - node->blockX - rectangle[2] + x;
        } else {
            const std::int32_t currentBlockX = node->blockX;
            if (useActionScale) {
                scaleX = login::detail::GetYobiActionScaleX(action);
            }
            offsetX = currentBlockX - x + rectangle[0];
        }
        const auto scaledX = static_cast<std::int64_t>(static_cast<double>(offsetX) * scaleX);
        const std::int32_t destinationX = originX + static_cast<std::int32_t>(scaledX);
        const std::int32_t originY = graphic->y;
        float scaleY = fixedScale;
        std::int32_t offsetY;
        if ((mixMode & kFlipY) != 0) {
            if (useActionScale) {
                scaleY = login::detail::GetYobiActionScaleY(action);
            }
            const std::int32_t fullHeight = ReadGraphicDxHeight(imageNo);
            offsetY = fullHeight - node->blockY - rectangle[3] + y;
        } else {
            const std::int32_t currentBlockY = node->blockY;
            if (useActionScale) {
                scaleY = login::detail::GetYobiActionScaleY(action);
            }
            offsetY = currentBlockY - y + rectangle[1];
        }
        const auto scaledY = static_cast<std::int64_t>(static_cast<double>(offsetY) * scaleY);
        std::uint32_t color;
        std::memcpy(&color, graphic->descriptor.data() + 0x08, sizeof(color));
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
                &rectangle, scaleX, scaleY, surface, nodeImageNo, paletteIndex, mixMode, color)
            == kSurfaceBusy) {
            gGraphicDirty = 1;
        }
    }
}

// 原 520E54 的全图/裁剪入口；早退不清调色板 dirtyCount。
void DrawSprite(GraphicCacheItem32* const graphic)
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
        std::memset(graphic->descriptor.data(), 0, 4);
        const std::int16_t fullWidth = ReadGraphicDxWidth(imageNo);
        std::memcpy(graphic->descriptor.data() + 4, &fullWidth, sizeof(fullWidth));
        const std::int16_t fullHeight = ReadGraphicDxHeight(imageNo);
        std::memcpy(graphic->descriptor.data() + 6, &fullHeight, sizeof(fullHeight));
    }
    std::int16_t sourceWidth;
    std::int16_t sourceX;
    std::int16_t sourceY;
    std::int16_t sourceHeight;
    std::memcpy(&sourceWidth, graphic->descriptor.data() + 4, sizeof(sourceWidth));
    std::memcpy(&sourceX, graphic->descriptor.data(), sizeof(sourceX));
    std::memcpy(&sourceY, graphic->descriptor.data() + 2, sizeof(sourceY));
    std::memcpy(&sourceHeight, graphic->descriptor.data() + 6, sizeof(sourceHeight));
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
    const login::YobiAction* const action = graphic->reference.action;
    if (action != nullptr) {
        const float scaleX = login::detail::GetYobiActionScaleX(action);
        // 原只测 x87 C3；unordered 与相等同支，X 未命中时不得读取 Y。
        integerPath = scaleX == 1.0f || std::isnan(scaleX);
        if (integerPath) {
            const float scaleY = login::detail::GetYobiActionScaleY(action);
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
void DrawFixedScaleSprite(GraphicCacheItem32* const graphic, const float scale)
{
    const std::int32_t imageNo = graphic->imageNo;
    if (imageNo < 0 || imageNo >= resource::gMaxGraphicCount) {
        return;
    }
    static_cast<void>(LoadGraphicIndependentPalette_523140(graphic));
    if (LoadGraphic_522570(imageNo) == 0) {
        return;
    }
    std::memset(graphic->descriptor.data(), 0, 4);
    const std::int16_t fullWidth = ReadGraphicDxWidth(imageNo);
    std::memcpy(graphic->descriptor.data() + 4, &fullWidth, sizeof(fullWidth));
    std::int32_t width = fullWidth;
    const std::int16_t fullHeight = ReadGraphicDxHeight(imageNo);
    const std::int32_t height = fullHeight;
    std::memcpy(graphic->descriptor.data() + 6, &fullHeight, sizeof(fullHeight));
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

} // namespace

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
                if (HasLegacy3DDevice_44B610() != 0) {
                    static_cast<void>(EndLegacyTextureScene(GetRendererTextureDevice()));
                }
                RenderTextToSurface_456D70(graphic->reference.textToken);
                if (HasLegacy3DDevice_44B610() != 0) {
                    static_cast<void>(BeginLegacyTextureScene(GetRendererTextureDevice()));
                }
                break;
            case 5: {
                const std::int32_t y = graphic->y;
                const std::int32_t x = graphic->x;
                static_cast<void>(world::detail::Sub_46C760(x, y));
                break;
            }
            case 6:
                if (world::gValueC82204 != 0) {
                    const auto offsetX = std::bit_cast<std::int32_t>(world::detail::gValueC3500C);
                    std::int32_t destinationX = 0;
                    std::int32_t destinationY = 0;
                    TextureRectangle rectangle {0, 0, kCpuSurfaceWidth, kCpuSurfaceHeight};
                    if (offsetX > 0) {
                        rectangle[2] = kCpuSurfaceWidth - offsetX;
                        destinationX = offsetX;
                    } else if (offsetX < 0) {
                        rectangle[0] = -offsetX;
                    }
                    const auto offsetY = std::bit_cast<std::int32_t>(world::detail::gValueC75010);
                    if (offsetY > 0) {
                        rectangle[3] = kCpuSurfaceHeight - offsetY;
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
                world::detail::Sub_46E480();
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
