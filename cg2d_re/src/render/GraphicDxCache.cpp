#include "GraphicDxCacheState.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/core/m_log.hpp"
#include "cg/text/Encoding.hpp"
#include "LegacyTextureDrawing.hpp"
#include "PngImageInternal.hpp"
#include "cg/resource/GraphicDecode.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include <Windows.h>

#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <bit>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <new>
#include <tuple>
#include <vector>

namespace cg::render {

struct TextureSurface {
    std::int32_t width = 0;  // 原 DDS 的实际 W32 尺寸，可能大于低 S16 请求值。
    std::int32_t height = 0;
    std::uint32_t colorKey = 0;
    std::uint32_t references {1}; // 创建引用及独立绑定引用；不是原 COM 对外计数。
    std::unique_ptr<std::uint32_t[]> pixels;
    // 借用 pixels；成员逆序析构保证先释放包装，再释放像素。
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> blitSurface {
        nullptr, SDL_DestroySurface
    };
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> gpuTexture {
        nullptr, SDL_DestroyTexture
    };
    SDL_Renderer* gpuRenderer = nullptr; // 非拥有；销毁 renderer 前清除关联纹理。
    bool gpuDirty = true;
    bool gpuPremultipliedAlpha = false; // 当前GPU像素表示；CPU始终保留普通ARGB。
    bool pngContentDecoded = false; // GRT05：成功解码的 graphic PNG 像素不依赖调色板。
    std::array<std::int32_t, 4> dirtyRectangle {};
    TextureSurface* nextOwned = nullptr;
    TextureSurface* previousOwned = nullptr;
};

// 原5536B8「確認」；正文4B，字面量隐式追加唯一NUL。
static constexpr wchar_t kSurfaceFailureCaption[] =
    L"确认";

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
        surface->dirtyRectangle = {0, 0, width, height};
        surface->colorKey = colorKey;
        surface->blitSurface.reset(SDL_CreateSurfaceFrom(
            width, height, SDL_PIXELFORMAT_ARGB8888, surface->pixels.get(),
            static_cast<int>(width) * static_cast<int>(sizeof(std::uint32_t))));
        if (surface->blitSurface == nullptr
            || !SDL_SetSurfaceBlendMode(surface->blitSurface.get(), SDL_BLENDMODE_NONE)
            || !SDL_SetSurfaceColorKey(surface->blitSurface.get(), false, surface->colorKey)) {
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

    void ReleaseGpuTextures(SDL_Renderer* const renderer) noexcept
    {
        for (TextureSurface* surface = head_; surface != nullptr; surface = surface->nextOwned) {
            if (surface->gpuRenderer == renderer) {
                surface->gpuTexture.reset();
                surface->gpuRenderer = nullptr;
                surface->gpuDirty = true;
            }
        }
    }

private:
    TextureSurface* head_ = nullptr;
};

static TextureSurfaceOwner gTextureSurfaceOwner;

// GRT01：沿用旧 4096 个 64×48 节点对应的像素预算，缓存按整图占用回收。
static constexpr std::size_t kGraphicTexturePixelBudget = kTextureNodeCapacity * 64u * 48u;
static std::size_t gGraphicTexturePixels = 0;

static void ReleaseGraphicNodeSurface(TextureNode& node) noexcept
{
    if (node.surface != nullptr) {
        gGraphicTexturePixels -= static_cast<std::size_t>(node.surface->width) * node.surface->height;
        gTextureSurfaceOwner.Release(node.surface);
        node.surface = nullptr;
    }
}

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

// 唯一活动路径是下方两个原相邻槽；实际静态对象自动构析，指针只供访问。
// std::map 只拥有私有节点，surface 键和值均沿原业务清理借用。
static SurfaceTransformCache* gSurfaceTransforms629300 = nullptr; // 原全局：0x629300；仅指向原入口的唯一静态对象。
static SurfaceTransformCache* gSurfaceTransforms62A630 = nullptr; // 原全局：0x62A630；仅指向原入口的唯一静态对象。

// 原函数：0x0044CC20。
void InitializeSurfaceTransformCache629300_44CC20()
{
    // 原0x0044CC30／0x0044CC60：同一类型按原入口创建，退出由MSVC原生析构承接。
    // 原0x0044CC70共用析构：仅销毁map节点，不释放借用的surface。
    static SurfaceTransformCache storage{};
    gSurfaceTransforms629300 = &storage;
}

// 原函数：0x0044CD30。
void InitializeSurfaceTransformCache62A630_44CD30()
{
    // 原0x0044CD40／0x0044CD70：同一类型按原入口创建，退出由MSVC原生析构承接。
    // 原0x0044CC70共用析构：仅销毁map节点，不释放借用的surface。
    static SurfaceTransformCache storage{};
    gSurfaceTransforms62A630 = &storage;
}

GraphicDxCache* gGraphicDxCache103CFA4 = nullptr; // 原全局：0x0103CFA4；RMO01分页拥有者。

GraphicDxCache::GraphicDxCache(const std::int32_t capacity) noexcept
    : pageCount_((static_cast<std::size_t>(capacity) + kEntriesPerPage - 1) / kEntriesPerPage),
      pages_(new (std::nothrow) std::unique_ptr<DxGraphicEntry[]>[pageCount_] {})
{
}

const DxGraphicEntry& GraphicDxCache::Read(const std::uint32_t imageNo) const noexcept
{
    static const DxGraphicEntry empty {};
    const auto& page = pages_[imageNo / kEntriesPerPage];
    return page ? page[imageNo % kEntriesPerPage] : empty;
}

DxGraphicEntry* GraphicDxCache::Find(const std::uint32_t imageNo) noexcept
{
    auto& page = pages_[imageNo / kEntriesPerPage];
    return page ? &page[imageNo % kEntriesPerPage] : nullptr;
}

DxGraphicEntry* GraphicDxCache::Ensure(const std::uint32_t imageNo) noexcept
{
    auto& page = pages_[imageNo / kEntriesPerPage];
    if (!page) {
        page.reset(new (std::nothrow) DxGraphicEntry[kEntriesPerPage] {});
        if (!page) {
            m_logw(L"[render] DX cache page allocation failed: graphic=%u\n", imageNo);
            return nullptr;
        }
    }
    return &page[imageNo % kEntriesPerPage];
}

void GraphicDxCache::InvalidateReferences(const std::int32_t count) noexcept
{
    const auto limit = static_cast<std::size_t>(count);
    for (std::size_t pageIndex = 0; pageIndex < pageCount_ && pageIndex * kEntriesPerPage < limit; ++pageIndex) {
        if (!pages_[pageIndex]) { continue; }
        const auto entries = std::min(kEntriesPerPage, limit - pageIndex * kEntriesPerPage);
        for (std::size_t index = 0; index < entries; ++index) {
            pages_[pageIndex][index].head = nullptr;
            pages_[pageIndex][index].palette = nullptr;
        }
    }
}
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

void MarkTextureSurfaceModified(TextureSurface* const surface) noexcept
{
    surface->gpuDirty = true;
    surface->dirtyRectangle = {0, 0, surface->width, surface->height};
}

void MarkTextureSurfaceModified(
    TextureSurface* const surface, const std::array<std::int32_t, 4>& rectangle) noexcept
{
    auto& dirty = surface->dirtyRectangle;
    if (surface->gpuDirty) {
        dirty = {std::min(dirty[0], rectangle[0]), std::min(dirty[1], rectangle[1]),
            std::max(dirty[2], rectangle[2]), std::max(dirty[3], rectangle[3])};
    } else {
        dirty = rectangle;
    }
    surface->gpuDirty = true;
}

SDL_Texture* GetTextureGpuTexture(
    TextureSurface* const surface, SDL_Renderer* const renderer, const bool premultiplyAlpha) noexcept
{
    if (surface->gpuTexture == nullptr || surface->gpuRenderer != renderer) {
        // GRT02：只在首次使用／设备重建时创建；所有生产者已经写出 ARGB8888。
        surface->gpuTexture.reset(SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, surface->width, surface->height));
        surface->gpuRenderer = renderer;
        if (surface->gpuTexture == nullptr
            || !SDL_SetTextureScaleMode(surface->gpuTexture.get(), SDL_SCALEMODE_NEAREST)) {
            surface->gpuTexture.reset();
            return nullptr;
        }
        MarkTextureSurfaceModified(surface);
    }
    if (surface->gpuPremultipliedAlpha != premultiplyAlpha) {
        MarkTextureSurfaceModified(surface);
    }
    if (surface->gpuDirty) {
        const auto& dirty = surface->dirtyRectangle;
        const SDL_Rect rectangle {dirty[0], dirty[1], dirty[2] - dirty[0], dirty[3] - dirty[1]};
        const SDL_Surface* const input = surface->blitSurface.get();
        const auto* const pixels = static_cast<const std::uint8_t*>(input->pixels)
            + static_cast<std::size_t>(rectangle.y) * input->pitch
            + static_cast<std::size_t>(rectangle.x) * sizeof(std::uint32_t);
        const void* uploadPixels = pixels;
        int uploadPitch = input->pitch;
        std::unique_ptr<std::uint32_t[]> premultipliedPixels;
        if (premultiplyAlpha) {
            premultipliedPixels.reset(new (std::nothrow) std::uint32_t[
                static_cast<std::size_t>(rectangle.w) * rectangle.h]);
            if (premultipliedPixels == nullptr) {
                SDL_OutOfMemory();
                return nullptr;
            }
            uploadPitch = rectangle.w * static_cast<int>(sizeof(std::uint32_t));
            if (!SDL_PremultiplyAlpha(rectangle.w, rectangle.h, SDL_PIXELFORMAT_ARGB8888,
                    pixels, input->pitch, SDL_PIXELFORMAT_ARGB8888,
                    premultipliedPixels.get(), uploadPitch, false)) {
                return nullptr;
            }
            uploadPixels = premultipliedPixels.get();
        }
        if (!SDL_UpdateTexture(surface->gpuTexture.get(), &rectangle, uploadPixels, uploadPitch)) {
            return nullptr;
        }
        surface->gpuPremultipliedAlpha = premultiplyAlpha;
        surface->gpuDirty = false;
    }
    return surface->gpuTexture.get();
}

void ReleaseRendererGpuTextures(SDL_Renderer* const renderer) noexcept
{
    gTextureSurfaceOwner.ReleaseGpuTextures(renderer);
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

// NSM01：共享色盘切换不重建节点，仅使实际着色结果失效。
void InvalidateGraphicTexturePalettes() noexcept
{
    for (std::int32_t index = 0; index < gTextureNodeCount; ++index) {
        auto& node = gTextureNodes[static_cast<std::size_t>(index)];
        node.paletteChoice = -1;
        node.paletteVersion = -1;
    }
}

// 原函数：0x00523350。
std::int32_t InvalidateGraphicRefs_523350() noexcept
{
    const std::int32_t count = resource::gMaxGraphicCount;
    if (count <= 0) {
        return count;
    }

    gGraphicDxCache103CFA4->InvalidateReferences(count);
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(count) * 0x10u);
}

// 原函数：0x00522570。
std::int32_t LoadGraphic_522570(const std::int32_t imageNo)
{
    if (imageNo < 0 || imageNo >= resource::gMaxGraphicCount) {
        return 0;
    }
    if (gGraphicDxCache103CFA4->Read(imageNo).head != nullptr) {
        return 1;
    }
    DxGraphicEntry* const entry = gGraphicDxCache103CFA4->Ensure(imageNo);
    if (entry == nullptr) { return 0; }
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

    std::uint16_t width;
    std::memcpy(&width, &resource::gCurrentGraphicWidth, sizeof(width));
    resource::gGraphicDataBuffer = imageNo;
    entry->width = width;

    std::uint16_t height;
    std::memcpy(&height, &resource::gCurrentGraphicHeight, sizeof(height));
    entry->height = height;
    TakeGraphicDxCache_5222D0(imageNo, nullptr);
    return entry->head != nullptr ? 1 : 0;
}

std::uint16_t ReadGraphicAnimeActionFlags(const std::int32_t imageNo) noexcept
{
    return imageNo >= 0 && imageNo < resource::gMaxGraphicCount
        ? gGraphicDxCache103CFA4->Read(imageNo).animeActionFlags : 0;
}

bool SetGraphicAnimeActionFlags(const std::uint32_t imageNo, const std::uint16_t flags) noexcept
{
    if (imageNo < static_cast<std::uint32_t>(resource::gMaxGraphicCount)) {
        auto* entry = gGraphicDxCache103CFA4->Find(imageNo);
        if (entry == nullptr && flags == 0) { return true; }
        if (entry == nullptr) { entry = gGraphicDxCache103CFA4->Ensure(imageNo); }
        if (entry == nullptr) { return false; }
        entry->animeActionFlags = flags;
    }
    return true;
}

void ReadGraphicCoordinate(
    const std::int32_t imageNo, std::int16_t& x, std::int16_t& y) noexcept
{
    x = 0;
    y = 0;
    if (imageNo >= 0 && imageNo < resource::gMaxGraphicCount) {
        const auto& entry = gGraphicDxCache103CFA4->Read(imageNo);
        x = entry.coordinateX;
        y = entry.coordinateY;
    }
}

bool SetGraphicCoordinate(
    const std::uint32_t imageNo, const std::int16_t x, const std::int16_t y) noexcept
{
    if (imageNo < static_cast<std::uint32_t>(resource::gMaxGraphicCount)) {
        auto* entry = gGraphicDxCache103CFA4->Find(imageNo);
        if (entry == nullptr && x == 0 && y == 0) { return true; }
        if (entry == nullptr) { entry = gGraphicDxCache103CFA4->Ensure(imageNo); }
        if (entry == nullptr) { return false; }
        entry->coordinateX = x;
        entry->coordinateY = y;
    }
    return true;
}

std::int16_t ReadGraphicDxWidth(const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(gGraphicDxCache103CFA4->Read(imageNo).width);
}

std::int16_t ReadGraphicDxHeight(const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(gGraphicDxCache103CFA4->Read(imageNo).height);
}

std::int16_t ReadGraphicDxHeightFromEntries(
    const GraphicDxCache* const entries, const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(entries->Read(imageNo).height);
}

std::int16_t ReadGraphicDxWidthFromEntries(
    const GraphicDxCache* const entries, const std::int32_t imageNo) noexcept
{
    return static_cast<std::int16_t>(entries->Read(imageNo).width);
}

std::int16_t ReadGraphicDxHeightFromField(const std::int32_t& imageNo) noexcept
{
    const GraphicDxCache* const entries = gGraphicDxCache103CFA4;
    const std::int32_t image = imageNo;
    return static_cast<std::int16_t>(entries->Read(image).height);
}

// 原函数：0x005222D0。
void TakeGraphicDxCache_5222D0(const std::int32_t imageNo, void* /*unusedSource*/)
{
    const auto width = static_cast<std::int32_t>(resource::gCurrentGraphicWidth);
    const auto height = static_cast<std::int32_t>(resource::gCurrentGraphicHeight);
    const std::size_t pixels = static_cast<std::size_t>(width) * height;
    DxGraphicEntry* const allocatedEntry = gGraphicDxCache103CFA4->Ensure(imageNo);
    if (allocatedEntry == nullptr) { return; }
    DxGraphicEntry& entry = *allocatedEntry;
    if (pixels > kGraphicTexturePixelBudget) {
        // 负尺寸转为 size_t 后也会进入既有容量拒绝分支，需区分资源尺寸错误。
        if (width < 0 || height < 0) {
            const auto& info = resource::gGraphicInfo[imageNo];
            m_logw(L"[render] graphic %d has invalid resource dimensions: %dx%d (mapNo=%d, fileIndex=%d, offset=%u, length=%u)\n",
                imageNo, width, height, info.mapNo, static_cast<int>(info.fileIndex),
                static_cast<unsigned int>(info.offset), static_cast<unsigned int>(info.length));
        } else {
            m_logw(L"[render] graphic %d exceeds texture cache capacity: %dx%d\n", imageNo, width, height);
        }
        return;
    }
    const auto evict = [](TextureNode& node) {
        if (node.imageNo != -1) {
            auto* previous = gGraphicDxCache103CFA4->Find(node.imageNo);
            if (previous != nullptr && previous->head == &node) { previous->head = nullptr; }
        }
        ReleaseGraphicNodeSurface(node);
        node.imageNo = -1;
        node.next = nullptr;
    };
    if (entry.head != nullptr) { evict(*entry.head); }
    TextureNode* selected = nullptr;
    const std::uint32_t reuseThreshold = platform::gFrameCount - 2u;
    for (std::int32_t checked = 0; checked < gTextureNodeCount; ++checked) {
        TextureNode& node = gTextureNodes[static_cast<std::size_t>(gTextureScanIndex)];
        gTextureScanIndex = (gTextureScanIndex + 1) % gTextureNodeCount;
        if (node.imageNo == -1 || node.reuseState < reuseThreshold) {
            evict(node);
            selected = &node;
            break;
        }
    }
    if (selected == nullptr) {
        m_logw(L"[render] graphic %d has no reusable texture cache entry\n", imageNo);
        return;
    }
    for (std::int32_t checked = 0;
         gGraphicTexturePixels + pixels > kGraphicTexturePixelBudget && checked < gTextureNodeCount;
         ++checked) {
        TextureNode& node = gTextureNodes[static_cast<std::size_t>(gTextureScanIndex)];
        gTextureScanIndex = (gTextureScanIndex + 1) % gTextureNodeCount;
        if (&node != selected && (node.imageNo == -1 || node.reuseState < reuseThreshold)) { evict(node); }
    }
    if (gGraphicTexturePixels + pixels > kGraphicTexturePixelBudget) {
        m_logw(L"[render] graphic %d cannot fit while recent textures are in use\n", imageNo);
        return;
    }
    selected->surface = gTextureSurfaceOwner.Create(width, height, 0);
    if (selected->surface == nullptr) {
        m_logw(L"[render] graphic %d surface allocation failed: %dx%d\n", imageNo, width, height);
        return;
    }
    gGraphicTexturePixels += pixels;
    selected->imageNo = imageNo;
    selected->reuseState = platform::gFrameCount;
    selected->blockX = 0;
    selected->blockY = 0;
    selected->paletteChoice = -1;
    selected->paletteVersion = -1;
    selected->paletteIndex = 0;
    selected->next = nullptr;
    entry.head = selected;
}

// 原函数：0x00520B10。
std::int32_t LoadGraphicTexture_520B10(
    const GraphicCacheItem32* const graphic, TextureNode* const texture)
{
    std::int32_t paletteChoice = gActivePalette;
    std::int32_t paletteVersion = gPaletteFrameVersion;
    const std::int32_t initialImageNo = graphic->imageNo;
    const std::uint8_t initialMixFlags = graphic->descriptor.mixFlags;
    DxGraphicEntry* const existingEntry = gGraphicDxCache103CFA4->Find(initialImageNo);
    if (existingEntry == nullptr) { return 0; }
    DxGraphicEntry& entry = *existingEntry;
    if ((initialMixFlags & GraphicMixFlag_OriginalPalette) != 0) {
        paletteChoice = 0;
        paletteVersion = 0;
    }

    if (texture->surface->pngContentDecoded) {
        // PNG 像素未变；同步元数据供 CompositeGraphicPixels 校验，不触发重读或上传。
        texture->paletteChoice = static_cast<std::int16_t>(paletteChoice);
        texture->paletteVersion = static_cast<std::int16_t>(paletteVersion);
        texture->paletteIndex = graphic->descriptor.paletteIndex;
        return 1;
    }

    const std::int16_t oldPaletteChoice = texture->paletteChoice;
    if (oldPaletteChoice == paletteChoice
        && texture->paletteVersion == paletteVersion) {
        const std::int16_t oldPaletteIndex = texture->paletteIndex;
        std::int16_t paletteIndex;
        paletteIndex = graphic->descriptor.paletteIndex;
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
        paletteIndex = graphic->descriptor.paletteIndex;
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

    // GRT01：每图只有一个实际尺寸表面，不上传小块或边缘 padding。
    const std::int32_t width = std::bit_cast<std::int16_t>(entry.width);
    const std::int32_t height = std::bit_cast<std::int16_t>(entry.height);

    std::int16_t paletteIndex;
    paletteIndex = graphic->descriptor.paletteIndex;
    if (paletteIndex != 0) {
        const std::uint32_t* originalColors;
        const std::uint32_t* tonedColors;
        resource::GetIndependentPaletteColors32(paletteIndex, originalColors, tonedColors);
        if (originalColors != nullptr) {
            const std::uint8_t mixFlags = graphic->descriptor.mixFlags;
            const std::uint32_t* const colors =
                (mixFlags & GraphicMixFlag_OriginalPalette) == 0 ? tonedColors : originalColors;
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
                const std::uint8_t mixFlags = graphic->descriptor.mixFlags;
                const std::uint32_t sourceHeight = resource::gCurrentGraphicHeight;
                const std::uint8_t* const source = resource::gGraphicDataPtr;
                const std::uint8_t* const paletteTriples =
                    source + sourceWidth * sourceHeight;
                const std::int32_t imageNo = graphic->imageNo;
                entry.palette = LoadGraphicPalette32_44E290(
                    imageNo, 2, (mixFlags & GraphicMixFlag_OriginalPalette) == 0 ? 1 : 0, paletteTriples, nullptr);
            } else {
                const std::uint8_t mixFlags = graphic->descriptor.mixFlags;
                if ((mixFlags & GraphicMixFlag_OriginalPalette) != 0) {
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
    texture->paletteIndex = graphic->descriptor.paletteIndex;
    return 1;
}

// 原函数：0x005231A0。
std::int32_t BuildSurfaceCache_5231A0()
{
    gTextureNodeCount = 0;
    gTexturePreferredCount = 0;
    gTextureFallbackCount = 0;

    if (!CreateCpuAutoMapSurface()) {
        platform::ShowWideMessage(L"AutoMap作业用Surface产生失败。" /* 原58D398「AutoMap作業用Surface產生失敗。」；正文30B，字面量隐式追加唯一NUL。 */, kSurfaceFailureCaption, 0x10);
        return 0;
    }

    if (!CreateCpuTempSurface() && !CreateCpuTempSurfaceFallback()) {
        platform::ShowWideMessage(L"建立战斗画面错误" /* 原58D384「建立戰鬥畫面錯誤」；正文16B，字面量隐式追加唯一NUL。 */, kSurfaceFailureCaption, 0x10);
        return 0;
    }

    // GRT01：只初始化索引槽；真实尺寸的整图表面在首次加载时分配。
    gTextureNodeCount = static_cast<std::int32_t>(kTextureNodeCapacity);
    ResetTextureNodes_523310();
    static_cast<void>(InvalidateGraphicRefs_523350());
    return 1;
}

// 原函数：0x00451310。
void FillTextureBlack_451310(TextureSurface* const surface) noexcept
{
    surface->pngContentDecoded = false;
    MarkTextureSurfaceModified(surface);
    const std::size_t pixelCount =
        static_cast<std::size_t>(surface->width) * static_cast<std::size_t>(surface->height);
    std::fill_n(surface->pixels.get(), pixelCount, 0u);
}

std::int32_t BlitTextureToBack(
    TextureSurface* const surface, const TextureRectangle& source,
    const TextureRectangle& destination, const bool alphaBlend)
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
    const bool blend = alphaBlend || SDL_SurfaceHasColorKey(input);
    return DrawGpuTexture(surface,
        {static_cast<float>(source[0]), static_cast<float>(source[1]),
         static_cast<float>(source[2]), static_cast<float>(source[3])},
        {static_cast<float>(destination[0]), static_cast<float>(destination[1]),
         static_cast<float>(destination[2]), static_cast<float>(destination[3])}, 0xFFFFFFFFu,
        blend ? LegacyTextureBlend::SourceAlpha : LegacyTextureBlend::One,
        blend ? LegacyTextureBlend::InverseSourceAlpha : LegacyTextureBlend::Zero) ? 0 : failure;
}

// 原函数：0x00450400。
void LoadTextureFromBuffer32_450400(
    TextureSurface* const surface, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::uint8_t* const source, const std::int32_t sourceWidth,
    const std::uint8_t* const palette) noexcept
{
    surface->pngContentDecoded = false;
    MarkTextureSurfaceModified(surface);
    if (resource::IsPngGraphicData(source)) {
        const auto readBe = [](const std::uint8_t* bytes) {
            return (static_cast<std::uint32_t>(bytes[0]) << 24)
                | (static_cast<std::uint32_t>(bytes[1]) << 16)
                | (static_cast<std::uint32_t>(bytes[2]) << 8)
                | static_cast<std::uint32_t>(bytes[3]);
        };
        std::int32_t pngWidth = 0;
        std::int32_t pngHeight = 0;
        if (std::memcmp(source + 12, "IHDR", 4) == 0) {
            pngWidth = static_cast<std::int32_t>(readBe(source + 16));
            pngHeight = static_cast<std::int32_t>(readBe(source + 20));
        }
        const std::int32_t sourceY = pngHeight - y - 1;
        const std::int32_t copyWidth = std::min(width, pngWidth - x);
        const std::int32_t copyHeight = std::min(height, pngHeight - sourceY);
        if (copyWidth <= 0 || copyHeight <= 0) {
            return;
        }
        std::vector<std::uint32_t> pixels(
            static_cast<std::size_t>(copyWidth) * static_cast<std::size_t>(copyHeight));
        const std::size_t length = resource::gGraphicInfo[resource::gGraphicDataBuffer].length;
        const bool decoded = ReadPngRectangle({source, length}, x, sourceY, copyWidth, copyHeight, pixels)
            == pixels.size() * sizeof(std::uint32_t);
        // 参考不以解码返回值改变上传标记；失败区域仍为清零像素。
        FillTextureBlack_451310(surface);
        for (std::int32_t row = 0; row < copyHeight; ++row) {
            std::memcpy(surface->pixels.get() + row * surface->width, pixels.data() + row * copyWidth,
                static_cast<std::size_t>(copyWidth) * sizeof(std::uint32_t));
        }
        surface->pngContentDecoded = decoded;
        return;
    }
    if (height <= 0) {
        return;
    }

    // GRT04：原 0x523278／0x44C896 使用零色色键，与 GraphicInfo.flag 无关。
    // 查表后的 RGB 零色转透明 Alpha；调色板已将普通黑色映射为非零值。
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
            const std::uint32_t color = target[column] & 0x00FFFFFFu;
            target[column] = color | (color != 0 ? 0xFF000000u : 0u);
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
            ReleaseGraphicNodeSurface(*node);
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
    session::ClientSession& session,
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
    auto& cache = session.WorldScene().sceneType == 10
        ? (*gSurfaceTransforms62A630) : (*gSurfaceTransforms629300);
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
    auto& publishedCache = session.WorldScene().sceneType == 10
        ? (*gSurfaceTransforms62A630) : (*gSurfaceTransforms629300);
    publishedCache[publishedKey] = transformed;
    return publishedCache[publishedKey];
}

// 原函数：0x0044D490。
void ClearSurfaceTransformCache_44D490(session::ClientSession& session)
{
    auto& cache = session.WorldScene().sceneType == 10
        ? (*gSurfaceTransforms62A630) : (*gSurfaceTransforms629300);
    auto current = cache.begin();
    while (current != cache.end()) {
        gTextureSurfaceOwner.Release(current->second);
        current->second = nullptr;
        current = cache.erase(current);
    }
}

} // namespace cg::render
