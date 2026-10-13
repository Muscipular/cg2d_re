#include "GraphicDxCacheState.hpp"
#include "../platform/LegacyFormat.hpp"
#include "LegacyTextureDrawing.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/GraphicRead.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <new>
#include <tuple>

namespace cg::render {

struct TextureSurface {
    std::int32_t width {};  // 原 DDS 的实际 W32 尺寸，可能大于低 S16 请求值。
    std::int32_t height {};
    std::uint32_t colorKey {};
    std::uint32_t references {1}; // 创建引用及独立绑定引用；不是原 COM 对外计数。
    std::unique_ptr<std::uint32_t[]> pixels;
    // 借用 pixels；成员逆序析构保证先释放包装，再释放像素。
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> blitSurface {
        nullptr, SDL_DestroySurface
    };
    TextureSurface* nextOwned {};
    TextureSurface* previousOwned {};
};

namespace {

// 原5536B8「確認」；正文4B，字面量隐式追加唯一NUL。
constexpr char kSurfaceFailureCaption[] =
    "\xB4\x5F\xD5\x4A";
static_assert(sizeof(kSurfaceFailureCaption) == 5);

// 原58D398「AutoMap作業用Surface產生失敗。」；正文30B，字面量隐式追加唯一NUL。
constexpr char kAutoMapCreateFailed[] =
    "\x41\x75\x74\x6F\x4D\x61\x70\xD7\xF7\x98\x49\xD3\xC3\x53\x75\x72\x66\x61\x63\x65\xAE\x61\xC9\xFA\xCA\xA7\x94\xA1\xA1\xA3";
static_assert(sizeof(kAutoMapCreateFailed) == 31);

// 原58D384「建立戰鬥畫面錯誤」；正文16B，字面量隐式追加唯一NUL。
constexpr char kTempSurfaceCreateFailed[] =
    "\xBD\xA8\xC1\xA2\x91\xF0\xF4\x59\xAE\x8B\xC3\xE6\xE5\x65\xD5\x60";
static_assert(sizeof(kTempSurfaceCreateFailed) == 17);

// 原58D374「建立繪圖頁失敗」；正文14B，字面量隐式追加唯一NUL。
constexpr char kTextureNodeCreateFailed[] =
    "\xBD\xA8\xC1\xA2\xC0\x4C\x88\x44\xED\x93\xCA\xA7\x94\xA1";
static_assert(sizeof(kTextureNodeCreateFailed) == 15);

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

    TextureSurface* Create(
        std::int32_t width, std::int32_t height, std::uint32_t colorKey) noexcept
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
        surface->colorKey = colorKey;
        surface->blitSurface.reset(SDL_CreateSurfaceFrom(
            width, height, SDL_PIXELFORMAT_XRGB8888, surface->pixels.get(),
            static_cast<int>(width) * static_cast<int>(sizeof(std::uint32_t))));
        if (surface->blitSurface == nullptr
            || !SDL_SetSurfaceBlendMode(surface->blitSurface.get(), SDL_BLENDMODE_NONE)
            || !SDL_SetSurfaceColorKey(surface->blitSurface.get(), true, surface->colorKey)) {
            delete surface;
            return nullptr;
        }
        surface->nextOwned = head_;
        if (head_ != nullptr) {
            head_->previousOwned = surface;
        }
        head_ = surface;
        return surface;
    }

    void Release(TextureSurface* surface) noexcept
    {
        if (--surface->references != 0) {
            return;
        }
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

// 原 28B 栈键只拓宽首项地址；未参与比较的原 +0A/+0B 不作为新键。
struct SurfaceTransformKey {
    TextureSurface* surface; // 非拥有引用；数值排序只在比较器内转换。
    std::int32_t imageNo;
    std::int16_t paletteIndex;
    std::array<std::int32_t, 4> rectangle;
};

static_assert(sizeof(std::uintptr_t) == 8);
static_assert(sizeof(TextureSurface*) == 8);
static_assert(sizeof(SurfaceTransformKey) == 0x20);
static_assert(offsetof(SurfaceTransformKey, imageNo) == 0x08);
static_assert(offsetof(SurfaceTransformKey, paletteIndex) == 0x0C);
static_assert(offsetof(SurfaceTransformKey, rectangle) == 0x10);

struct SurfaceTransformKeyLess {
    bool operator()(const SurfaceTransformKey& left, const SurfaceTransformKey& right) const noexcept
    {
        if (left.surface != right.surface) {
            return reinterpret_cast<std::uintptr_t>(left.surface)
                < reinterpret_cast<std::uintptr_t>(right.surface);
        }
        // 原 44D3F0：矩形比较顺序为 left、right、top、bottom，不是内存顺序。
        return std::tie(left.imageNo, left.paletteIndex,
                   left.rectangle[0], left.rectangle[2], left.rectangle[1], left.rectangle[3])
            < std::tie(right.imageNo, right.paletteIndex,
                   right.rectangle[0], right.rectangle[2], right.rectangle[1], right.rectangle[3]);
    }
};

using SurfaceTransformCache =
    std::map<SurfaceTransformKey, TextureSurface*, SurfaceTransformKeyLess>;

// 被动存储不自动构析；唯一活动路径是下方两个原相邻槽的宿主登记。
// std::map 只拥有私有节点，surface 键和值均沿原业务清理借用。
alignas(SurfaceTransformCache)
std::byte gSurfaceTransforms629300[sizeof(SurfaceTransformCache)] {}; // 原全局：0x00629300。
alignas(SurfaceTransformCache)
std::byte gSurfaceTransforms62A630[sizeof(SurfaceTransformCache)] {}; // 原全局：0x0062A630。

SurfaceTransformCache& BorrowSurfaceTransformCache629300() noexcept
{
    return *std::launder(
        reinterpret_cast<SurfaceTransformCache*>(gSurfaceTransforms629300));
}

SurfaceTransformCache& BorrowSurfaceTransformCache62A630() noexcept
{
    return *std::launder(
        reinterpret_cast<SurfaceTransformCache*>(gSurfaceTransforms62A630));
}

// 原 44CC30/44CD40：固定 this 构造并返回同一对象；节点/空节点由宿主 STL 承接。
SurfaceTransformCache* ConstructSurfaceTransformCache629300_44CC30()
{
    return std::construct_at(
        reinterpret_cast<SurfaceTransformCache*>(gSurfaceTransforms629300));
}

// 原函数：0x0044CD40。
SurfaceTransformCache* ConstructSurfaceTransformCache62A630_44CD40()
{
    return std::construct_at(
        reinterpret_cast<SurfaceTransformCache*>(gSurfaceTransforms62A630));
}

// 原 44CC60/44CD70 共用 44CC70：只销毁节点，不调用面释放或场景业务清理。
void DestroySurfaceTransformCache629300_44CC60()
{
    std::destroy_at(&BorrowSurfaceTransformCache629300());
}

// 原函数：0x0044CD70。
void DestroySurfaceTransformCache62A630_44CD70()
{
    std::destroy_at(&BorrowSurfaceTransformCache62A630());
}

} // namespace

// 原函数：0x0044CC20。
std::int32_t InitializeSurfaceTransformCache629300_44CC20()
{
    static_cast<void>(ConstructSurfaceTransformCache629300_44CC30());
    return std::atexit(DestroySurfaceTransformCache629300_44CC60);
}

// 原函数：0x0044CD30。
std::int32_t InitializeSurfaceTransformCache62A630_44CD30()
{
    static_cast<void>(ConstructSurfaceTransformCache62A630_44CD40());
    return std::atexit(DestroySurfaceTransformCache62A630_44CD70);
}

DxGraphicEntry* gGraphicDxCache103CFA4 = nullptr; // 原全局：0x0103CFA4。
std::array<TextureNode, kTextureNodeCapacity> gTextureNodes {}; // 原全局：0x01020F08。
std::int32_t gTextureNodeCount = 0; // 原全局：0x0103CF24。
std::int32_t gTextureScanIndex = 0; // 原全局：0x0103CF20。
std::int32_t gTexturePreferredCount = 0; // 原全局：0x0103CF9C。
std::int32_t gTextureFallbackCount = 0; // 原全局：0x0103CF98。
std::int32_t gTextureSizeW = 0; // 原全局：0x0103CF2C。
std::int32_t gTextureSizeH = 0; // 原全局：0x0103CF28。

// 原函数：0x0044C6D0。
TextureSurface* CreateTextureSurface_44C6D0(
    const std::int16_t requestedWidth, const std::int16_t requestedHeight,
    const std::uint32_t colorKey, std::uint32_t /*unusedMemoryCaps*/)
{
    std::uint32_t width = static_cast<std::uint32_t>(requestedWidth);
    std::uint32_t height = static_cast<std::uint32_t>(requestedHeight);
    if (HasLegacy3DDevice_44B610() != 0) {
        width = static_cast<std::uint32_t>(static_cast<std::int16_t>(requestedWidth + 2));
        height = static_cast<std::uint32_t>(static_cast<std::int16_t>(requestedHeight + 2));
    }
    if (HasLegacy3DDevice_44B610() != 0) {
        if ((GetLegacyTextureState().textureCaps & 2u) != 0) {
            const std::uint32_t requiredWidth = width;
            const std::uint32_t requiredHeight = height;
            width = 1;
            while (width < requiredWidth) {
                width += width;
            }
            height = 1;
            while (height < requiredHeight) {
                height += height;
            }
        }
        if ((GetLegacyTextureState().textureCaps & 0x20u) != 0) {
            if (width <= height) {
                width = height;
            } else {
                height = width;
            }
        }
        if (GetLegacyTextureState().pixelBitCount == 0) {
            return nullptr;
        }
    }
    return gTextureSurfaceOwner.Create(
        std::bit_cast<std::int32_t>(width), std::bit_cast<std::int32_t>(height), colorKey);
}

// 原函数：0x0044C8B0。
TextureSurface* CreatePlainTextureSurface_44C8B0(
    const std::int16_t width, const std::int16_t height,
    const std::uint32_t colorKey, std::uint32_t /*unusedMemoryCaps*/)
{
    return gTextureSurfaceOwner.Create(width, height, colorKey);
}

TextureSurface* RetainTextureSurface(TextureSurface* const surface) noexcept
{
    if (surface != nullptr) {
        ++surface->references;
    }
    return surface;
}

void ReleaseTextureSurface(TextureSurface* const surface) noexcept
{
    if (surface != nullptr) {
        gTextureSurfaceOwner.Release(surface);
    }
}

SDL_Surface* GetTextureSdlSurface(TextureSurface* const surface) noexcept
{
    return surface == nullptr ? nullptr : surface->blitSurface.get();
}

TextureSurfaceBinding::~TextureSurfaceBinding()
{
    ReleaseTextureSurface(surface_);
}

void TextureSurfaceBinding::Bind(TextureSurface* const surface) noexcept
{
    TextureSurface* const retained = RetainTextureSurface(surface);
    ReleaseTextureSurface(surface_);
    surface_ = retained;
}

// 原函数：0x00523310。
void ResetTextureNodes_523310() noexcept
{
    const std::int32_t count = gTextureNodeCount;
    gTextureScanIndex = 0;
    if (count <= 0) {
        return;
    }
    for (std::int32_t index = 0; index < count; ++index) {
        auto& node = gTextureNodes[static_cast<std::size_t>(index)];
        node.imageNo = -1;
        node.reuseState = 0;
        node.next = nullptr;
    }
}

// 原函数：0x00523350。
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

// 原函数：0x00522570。
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

    DxGraphicEntry* const widthBase = gGraphicDxCache103CFA4;
    std::uint16_t width;
    std::memcpy(&width, &resource::gCurrentGraphicWidth, sizeof(width));
    resource::gGraphicDataBuffer = imageNo;
    widthBase[static_cast<std::size_t>(imageNo)].width = width;

    DxGraphicEntry* const heightBase = gGraphicDxCache103CFA4;
    std::uint16_t height;
    std::memcpy(&height, &resource::gCurrentGraphicHeight, sizeof(height));
    heightBase[static_cast<std::size_t>(imageNo)].height = height;
    TakeGraphicDxCache_5222D0(imageNo, nullptr);
    return 1;
}

std::int16_t ReadGraphicDxWidth(const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(gGraphicDxCache103CFA4[imageNo].width);
}

std::int16_t ReadGraphicDxHeight(const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(gGraphicDxCache103CFA4[imageNo].height);
}

const DxGraphicEntry* BorrowGraphicDxCacheEntries() noexcept
{
    return gGraphicDxCache103CFA4;
}

std::int16_t ReadGraphicDxHeightFromEntries(
    const DxGraphicEntry* const entries, const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(entries[imageNo].height);
}

std::int16_t ReadGraphicDxWidthFromEntries(
    const DxGraphicEntry* const entries, const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(entries[imageNo].width);
}

std::int16_t ReadGraphicDxHeightFromField(const std::int32_t& imageNo) noexcept
{
    DxGraphicEntry* const entries = gGraphicDxCache103CFA4;
    const std::int32_t image = imageNo;
    return static_cast<std::int16_t>(entries[image].height);
}

// 原函数：0x005222D0。
void TakeGraphicDxCache_5222D0(const std::int32_t imageNo, void* /*unusedSource*/)
{
    std::int32_t scan = gTextureScanIndex;
    const std::int32_t scanStart = scan;
    const auto width = static_cast<std::int32_t>(resource::gCurrentGraphicWidth);
    const auto height = static_cast<std::int32_t>(resource::gCurrentGraphicHeight);
    const std::int32_t columns = width / 64 + (width % 64 != 0 ? 1 : 0);
    const std::int32_t rows = height / 48 + (height % 48 != 0 ? 1 : 0);
    const std::int32_t required = columns * rows;
    const std::uint32_t alignmentWidth = resource::gCurrentGraphicWidth;
    if ((alignmentWidth & 3u) != 0) {
        resource::gCurrentGraphicWidth =
            alignmentWidth + ((0u - alignmentWidth) & 3u);
    }

    TextureNode* const oldHead = gGraphicDxCache103CFA4[imageNo].head;
    if (oldHead != nullptr) {
        TextureNode* old = oldHead;
        do {
            old->imageNo = -1;
            old = old->next;
        } while (old != nullptr);
        gGraphicDxCache103CFA4[imageNo].head = nullptr;
        scan = gTextureScanIndex;
    }

    TextureNode* previous = nullptr;
    std::int32_t blockX = 0;
    std::int32_t remainingHeight = height;
    std::int32_t taken = 0;
    for (;;) {
        const std::uint32_t reuseThreshold = platform::gFrameCount - 2u;
        const TextureNode& candidate = gTextureNodes[static_cast<std::size_t>(scan)];
        if (candidate.reuseState < reuseThreshold) {
            const std::int32_t previousImageNo = candidate.imageNo;
            if (previousImageNo != -1) {
                DxGraphicEntry& evicted = gGraphicDxCache103CFA4[previousImageNo];
                TextureNode* old = evicted.head;
                evicted.head = nullptr;
                while (old != nullptr) {
                    old->imageNo = -1;
                    old = old->next;
                }
                scan = gTextureScanIndex;
            }
            DxGraphicEntry& entry = gGraphicDxCache103CFA4[imageNo];
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
        const std::int32_t nodeCount = gTextureNodeCount;
        ++scan;
        gTextureScanIndex = scan;
        if (scan >= nodeCount) {
            scan = 0;
            gTextureScanIndex = 0;
        }
        if (scan == scanStart) {
            // 原 58D348 消息字节及 52253F 路径；不足时不清除部分链或补写尾 next。
            char message[256];
            platform::FormatLegacyString_53B304(message,
                "\x83\x54\x81\x5B\x83\x74\x83\x46\x83\x58\x95\x73"
                "\x91\xAB\x82\xC5\x82\xB7\x81\x42%d\n", scan);
            platform::WriteLegacyDebugString(message);
            return;
        }
    }
}

// 原函数：0x00520B10。
std::int32_t LoadGraphicTexture_520B10(
    const GraphicCacheItem32* const graphic, TextureNode* const texture)
{
    std::int32_t paletteChoice = gActivePalette;
    std::int32_t paletteVersion = gPaletteFrameVersion;
    const std::int32_t initialImageNo = graphic->imageNo;
    const std::uint8_t initialMixMode = graphic->descriptor[0x0D];
    DxGraphicEntry* const dxBase = gGraphicDxCache103CFA4;
    DxGraphicEntry& entry = dxBase[initialImageNo];
    if ((initialMixMode & 4u) != 0) {
        paletteChoice = 0;
        paletteVersion = 0;
    }

    const std::int16_t oldPaletteChoice = texture->paletteChoice;
    if (oldPaletteChoice == paletteChoice
        && texture->paletteVersion == paletteVersion) {
        const std::int16_t oldPaletteIndex = texture->paletteIndex;
        std::int16_t paletteIndex;
        std::memcpy(&paletteIndex, graphic->descriptor.data() + 0x0E, sizeof(paletteIndex));
        if (oldPaletteIndex == paletteIndex) {
            return 1;
        }
    }
    if (gRenderQueueMode103CF10 != 0 && entry.flag != 0 && oldPaletteChoice >= 0) {
        const std::int32_t uploadCount = gGraphicPaletteUploadCount;
        if (uploadCount >= 10) {
            return 1;
        }
        gGraphicPaletteUploadCount = uploadCount + 1;
    }

    // 原 520B9F：限流分支后重新核三字段，索引只在前两项相同时读取。
    if (texture->paletteChoice == paletteChoice
        && texture->paletteVersion == paletteVersion) {
        const std::int16_t oldPaletteIndex = texture->paletteIndex;
        std::int16_t paletteIndex;
        std::memcpy(&paletteIndex, graphic->descriptor.data() + 0x0E, sizeof(paletteIndex));
        if (oldPaletteIndex == paletteIndex) {
            return 1;
        }
    }

    const std::int32_t requestedImageNo = graphic->imageNo;
    const std::int32_t bufferedImageNo = resource::gGraphicDataBuffer;
    if (bufferedImageNo != requestedImageNo) {
        if (resource::ReadGraphicDataByIndex(
                requestedImageNo, &resource::gGraphicDataPtr,
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

    const std::int32_t blockX = texture->blockX;
    std::int32_t width = gTextureSizeW;
    const std::int32_t graphicWidth = std::bit_cast<std::int16_t>(entry.width);
    if (blockX + width > graphicWidth) {
        width = graphicWidth - blockX;
    }
    const std::int32_t textureHeight = gTextureSizeH;
    const std::int32_t blockY = texture->blockY;
    const std::int32_t graphicHeight = std::bit_cast<std::int16_t>(entry.height);
    std::int32_t height = textureHeight;
    if (blockY + height > graphicHeight) {
        height = graphicHeight - blockY;
    }
    const std::int32_t textureWidth = gTextureSizeW;
    if (width < textureWidth || height < textureHeight) {
        FillTextureBlack_451310(texture->surface);
    }

    std::int16_t paletteIndex;
    std::memcpy(&paletteIndex, graphic->descriptor.data() + 0x0E, sizeof(paletteIndex));
    if (paletteIndex != 0) {
        const std::uint32_t* originalColors;
        const std::uint32_t* tonedColors;
        resource::BorrowIndependentPaletteColors32(paletteIndex, originalColors, tonedColors);
        if (originalColors != nullptr) {
            const std::uint8_t mixMode = graphic->descriptor[0x0D];
            const std::uint32_t* const colors =
                (mixMode & 4u) == 0 ? tonedColors : originalColors;
            const std::int32_t sourceHeight = std::bit_cast<std::int16_t>(entry.height);
            const std::int32_t sourceBlockY = texture->blockY;
            const std::int32_t sourceBlockX = texture->blockX;
            TextureSurface* const surface = texture->surface;
            LoadTextureFromBuffer32_450400(
                surface, sourceBlockX, sourceHeight - sourceBlockY - 1,
                width, height, resource::gGraphicDataPtr,
                static_cast<std::int32_t>(resource::gCurrentGraphicWidth),
                reinterpret_cast<const std::uint8_t*>(colors));
        }
    } else {
        PaletteCacheItem* const oldPalette = entry.palette;
        if (oldPalette != nullptr
            && (oldPalette->activePalIndex != paletteChoice
                || oldPalette->frameIndex != static_cast<std::uint32_t>(paletteVersion))) {
            oldPalette->imageNo = -1;
            entry.palette = nullptr;
        }
        if (entry.palette == nullptr) {
            if (static_cast<std::int32_t>(resource::gCurrentGraphicPaletteSize) > 0) {
                const std::uint32_t sourceWidth = resource::gCurrentGraphicWidth;
                const std::uint8_t mixMode = graphic->descriptor[0x0D];
                const std::uint32_t sourceHeight = resource::gCurrentGraphicHeight;
                const std::uint8_t* const source = resource::gGraphicDataPtr;
                const std::uint8_t* const paletteTriples =
                    source + sourceWidth * sourceHeight;
                const std::int32_t imageNo = graphic->imageNo;
                entry.palette = LoadGraphicPalette32_44E290(
                    imageNo, 2, (mixMode & 4u) == 0 ? 1 : 0, paletteTriples, nullptr);
            } else {
                const std::uint8_t mixMode = graphic->descriptor[0x0D];
                if ((mixMode & 4u) != 0) {
                    const std::int32_t imageNo = graphic->imageNo;
                    entry.palette = LoadGraphicPalette32_44E290(
                        imageNo, 0, 0, nullptr, nullptr);
                }
            }
        }
        PaletteCacheItem* const uploadPalette = entry.palette;
        const std::uint8_t* const colors =
            uploadPalette == nullptr ? nullptr : uploadPalette->payload.data();
        const std::int32_t sourceHeight = std::bit_cast<std::int16_t>(entry.height);
        const std::int32_t sourceBlockY = texture->blockY;
        const std::int32_t sourceBlockX = texture->blockX;
        TextureSurface* const surface = texture->surface;
        LoadTextureFromBuffer32_450400(
            surface, sourceBlockX, sourceHeight - sourceBlockY - 1,
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

// 原函数：0x005231A0。
std::int32_t BuildSurfaceCache_5231A0()
{
    gTextureNodeCount = 0;
    gTexturePreferredCount = 0;
    gTextureFallbackCount = 0;

    if (!CreateCpuAutoMapSurface()) {
        platform::ShowLegacyMessage(
            kAutoMapCreateFailed, kSurfaceFailureCaption, 0x10);
        return 0;
    }

    if (!CreateCpuTempSurface() && !CreateCpuTempSurfaceFallback()) {
        platform::ShowLegacyMessage(
            kTempSurfaceCreateFailed, kSurfaceFailureCaption, 0x10);
        return 0;
    }

    bool fallback = false;
    std::int32_t completed = 0;
    for (; completed < static_cast<std::int32_t>(kTextureNodeCapacity); ++completed) {
        TextureNode& node = gTextureNodes[static_cast<std::size_t>(completed)];
        if (!fallback) {
            std::int16_t height;
            std::memcpy(&height, &gTextureSizeH, sizeof(height));
            std::int16_t width;
            std::memcpy(&width, &gTextureSizeW, sizeof(width));
            node.surface = CreateTextureSurface_44C6D0(width, height, 0, 0x4000);
            if (node.surface != nullptr) {
                ++gTexturePreferredCount;
                continue;
            }
            fallback = true;
        }

        std::int16_t height;
        std::memcpy(&height, &gTextureSizeH, sizeof(height));
        std::int16_t width;
        std::memcpy(&width, &gTextureSizeW, sizeof(width));
        node.surface = CreateTextureSurface_44C6D0(width, height, 0, 0x800);
        if (node.surface == nullptr) {
            platform::ShowLegacyMessage(
                kTextureNodeCreateFailed, kSurfaceFailureCaption, 0x10);
            return 0;
        }
        ++gTextureFallbackCount;
    }

    gTextureNodeCount = completed;
    ResetTextureNodes_523310();
    static_cast<void>(InvalidateGraphicRefs_523350());
    return 1;
}

// 原函数：0x00451310。
void FillTextureBlack_451310(TextureSurface* const surface) noexcept
{
    const std::size_t pixelCount =
        static_cast<std::size_t>(surface->width) * static_cast<std::size_t>(surface->height);
    std::fill_n(surface->pixels.get(), pixelCount, 0u);
}

std::int32_t BlitTextureToBack(
    TextureSurface* const surface, const TextureRectangle& source,
    const TextureRectangle& destination)
{
    constexpr auto failure = std::bit_cast<std::int32_t>(0x80004005u);
    constexpr auto surfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (surface == nullptr || back == nullptr) {
        return failure;
    }
    SDL_Surface* const input = surface->blitSurface.get();
    if (((input->flags | back->flags) & SDL_SURFACE_LOCKED) != 0) {
        return surfaceBusy;
    }
    const SDL_Rect sourceRect {
        source[0], source[1], source[2] - source[0], source[3] - source[1]
    };
    const SDL_Rect destinationRect {
        destination[0], destination[1],
        destination[2] - destination[0], destination[3] - destination[1]
    };
    // 上层已按原分支裁剪；不在 native 边界重新改变矩形。
    const bool copied = sourceRect.w == destinationRect.w && sourceRect.h == destinationRect.h
        ? SDL_BlitSurfaceUnchecked(input, &sourceRect, back, &destinationRect)
        : SDL_BlitSurfaceUncheckedScaled(
            input, &sourceRect, back, &destinationRect, SDL_SCALEMODE_NEAREST);
    return copied ? 0 : failure;
}

// 原函数：0x00450400。
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
    const std::int32_t initialCount = gTextureNodeCount;
    if (initialCount <= 0) {
        return;
    }

    std::int32_t index = 0;
    TextureNode* node = gTextureNodes.data();
    for (;;) {
        TextureSurface* const surface = node->surface;
        if (surface != nullptr) {
            gTextureSurfaceOwner.Release(surface);
            node->surface = nullptr;
        }
        const std::int32_t currentCount = gTextureNodeCount;
        ++index;
        ++node;
        if (index >= currentCount) {
            break;
        }
    }
}

// 原函数：0x0044CD80。
TextureSurface* TransformTextureSurface_44CD80(
    TextureSurface* const source, const std::int32_t imageNo, const std::int16_t paletteIndex,
    const std::uint8_t flipX, const std::uint8_t flipY,
    const std::int32_t* const sourceRectangle, const std::int32_t* const keyRectangle)
{
    if (source == nullptr) {
        return nullptr;
    }
    const SurfaceTransformKey key {
        source, imageNo, paletteIndex,
        {keyRectangle[0], keyRectangle[1], keyRectangle[2], keyRectangle[3]}
    };
    auto& cache = world::gCurrentSceneType == 10
        ? BorrowSurfaceTransformCache62A630() : BorrowSurfaceTransformCache629300();
    const auto found = cache.find(key);
    if (found != cache.end()) {
        if (found->second != nullptr) {
            return found->second;
        }
        found->second = nullptr;
    }

    const std::int32_t surfaceWidth = source->width;
    std::int32_t sourceX = sourceRectangle[0];
    if (sourceX < 0) {
        sourceX = 0;
    }
    const std::int32_t width = sourceRectangle[2];
    const std::int32_t height = sourceRectangle[3];
    TextureSurface* const transformed = CreateTextureSurface_44C6D0(
        static_cast<std::int16_t>(source->width),
        static_cast<std::int16_t>(source->height), 0, 0x4000);
    if (transformed == nullptr) {
        return source;
    }
    for (std::int32_t y = 0; y < height; ++y) {
        const std::int32_t sourceY = flipY != 0 ? height - 1 - y : y;
        const std::uint32_t* const sourceRow = source->pixels.get() + sourceY * surfaceWidth;
        std::uint32_t* const targetRow = transformed->pixels.get() + y * transformed->width;
        for (std::int32_t x = 0; x < width; ++x) {
            std::int32_t sourceColumn = flipX != 0 ? width - 1 - x : x;
            // 原 44D1F7–44D205：仅在和小于面宽时加起点，否则仍取未加起点的列。
            if (sourceX + sourceColumn < surfaceWidth) {
                sourceColumn += sourceX;
            }
            targetRow[x] = sourceRow[sourceColumn];
        }
    }

    // 原尾部重新读取矩形与场景后插入；翻转位本身不进入缓存键。
    const SurfaceTransformKey publishedKey {
        source, imageNo, paletteIndex,
        {keyRectangle[0], keyRectangle[1], keyRectangle[2], keyRectangle[3]}
    };
    auto& publishedCache = world::gCurrentSceneType == 10
        ? BorrowSurfaceTransformCache62A630() : BorrowSurfaceTransformCache629300();
    publishedCache[publishedKey] = transformed;
    return publishedCache[publishedKey];
}

// 原函数：0x0044D490。
void ClearSurfaceTransformCache_44D490()
{
    auto& cache = world::gCurrentSceneType == 10
        ? BorrowSurfaceTransformCache62A630() : BorrowSurfaceTransformCache629300();
    auto current = cache.begin();
    while (current != cache.end()) {
        gTextureSurfaceOwner.Release(current->second);
        current->second = nullptr;
        current = cache.erase(current);
    }
}

} // namespace cg::render
