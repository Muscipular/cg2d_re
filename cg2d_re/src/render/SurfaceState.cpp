#include "cg/render/SurfaceState.hpp"

#include "cg/Palette.hpp"

#include "LegacyTextureDrawing.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <memory>
#include <new>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::render {
namespace {

struct CpuBackStorage {
    std::array<std::uint32_t, kCpuSurfacePixelCount + 1> pixels {};
    // SDL 只借用可见像素；成员逆序析构先释放包装。
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> blitSurface {
        nullptr, SDL_DestroySurface
    };
};

// 原 556300..556302，仅描边模式为 256 号色读取；后续地址为独立字符串。
constexpr std::uint8_t kOutlineExtraColors[1][3] {{0xA2, 0xE8, 0xFF}};

struct CpuCacheStorage {
    CpuSurfacePixels pixels {};
    CpuCacheStorage* nextOwned {};
};

class CpuCacheOwner final {
public:
    ~CpuCacheOwner()
    {
        while (head_ != nullptr) {
            CpuCacheStorage* next = head_->nextOwned;
            delete head_;
            head_ = next;
        }
    }

    CpuCacheStorage* Create() noexcept
    {
        auto* surface = new (std::nothrow) CpuCacheStorage {};
        if (surface != nullptr) {
            surface->nextOwned = head_;
            head_ = surface;
        }
        return surface;
    }

    void Release(CpuCacheStorage*& surface) noexcept
    {
        CpuCacheStorage* current = surface;
        surface = nullptr;
        if (current == nullptr) {
            return;
        }
        CpuCacheStorage** link = &head_;
        while (*link != current) {
            link = &(*link)->nextOwned;
        }
        *link = current->nextOwned;
        delete current;
    }

private:
    CpuCacheStorage* head_ {};
};

std::unique_ptr<CpuBackStorage> gCpuBackSurface;
CpuCacheOwner gCpuCacheOwner;
CpuCacheStorage* gCpuAutoMapSurface {};
CpuCacheStorage* gCpuTempSurface {};

} // namespace

std::uint32_t gBackgroundColor = 0;
std::uint32_t gValueFF48E4 = 0;

bool CreateCpuBackSurface() noexcept
{
    gCpuBackSurface.reset(new (std::nothrow) CpuBackStorage {});
    if (gCpuBackSurface == nullptr) {
        return false;
    }
    gCpuBackSurface->blitSurface.reset(SDL_CreateSurfaceFrom(
        kCpuSurfaceWidth, kCpuSurfaceHeight, SDL_PIXELFORMAT_XRGB8888,
        gCpuBackSurface->pixels.data(), static_cast<int>(kCpuSurfacePitchBytes)));
    if (gCpuBackSurface->blitSurface == nullptr) {
        gCpuBackSurface.reset();
        return false;
    }
    return true;
}

bool CreateCpuAutoMapSurface() noexcept
{
    gCpuAutoMapSurface = gCpuCacheOwner.Create();
    return gCpuAutoMapSurface != nullptr;
}

bool CreateCpuTempSurface() noexcept
{
    gCpuTempSurface = gCpuCacheOwner.Create();
    return gCpuTempSurface != nullptr;
}

bool BuildCpuSurfaceBuffers() noexcept
{
    if (!CreateCpuAutoMapSurface()) {
        return false;
    }
    return CreateCpuTempSurface();
}

void DestroyCpuCacheSurfaces() noexcept
{
    gCpuCacheOwner.Release(gCpuTempSurface);
    gCpuCacheOwner.Release(gCpuAutoMapSurface);
}

void DestroyCpuBackSurface() noexcept
{
    gCpuBackSurface.reset();
}

std::span<const std::uint32_t> ReadCpuBackPixels() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return {};
    }
    return {gCpuBackSurface->pixels.data(), kCpuSurfacePixelCount};
}

SDL_Surface* GetCpuBackSdlSurface() noexcept
{
    return gCpuBackSurface == nullptr ? nullptr : gCpuBackSurface->blitSurface.get();
}

std::int32_t ClearSurface_4512C0() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return -1;
    }
    const std::uint32_t color = gBackgroundColor;
    std::fill_n(gCpuBackSurface->pixels.data(), kCpuSurfacePixelCount, color);
    return 0;
}

void Sub_451350(RECT* const rect, const std::uint16_t colorIndex, const std::int32_t mode)
{
    if (rect->left >= kCpuSurfaceWidth || rect->right < 0
        || rect->top >= kCpuSurfaceHeight || rect->bottom < 0) {
        return;
    }
    if (rect->left < 0) {
        rect->left = 0;
    }
    if (rect->right >= kCpuSurfaceWidth) {
        rect->right = kCpuSurfaceWidth;
    }
    if (rect->top < 0) {
        rect->top = 0;
    }
    if (rect->bottom >= kCpuSurfaceHeight) {
        rect->bottom = kCpuSurfaceHeight;
    }
    const std::int32_t width = rect->right - rect->left;
    const std::int32_t height = rect->bottom - rect->top;
    if (mode == 2) {
        if (width == 0) {
            return;
        }
    } else if (width <= 2 || height <= 2) {
        return;
    }
    if (gCpuBackSurface == nullptr) {
        return;
    }

    const auto pitch = static_cast<std::int32_t>(kCpuSurfacePitchPixels);
    std::uint32_t* const first =
        gCpuBackSurface->pixels.data() + rect->top * pitch + rect->left + 1;
    switch (mode) {
    case 0: {
        std::uint32_t color;
        if (colorIndex < 256) {
            color = gPaletteAutoMap[colorIndex];
        } else {
            const std::uint8_t* const rgb = kOutlineExtraColors[colorIndex - 256];
            color = (static_cast<std::uint32_t>(rgb[0]) << 16)
                | (static_cast<std::uint32_t>(rgb[1]) << 8) | rgb[2];
        }
        const std::int32_t lastRow = pitch * (height - 1);
        for (std::int32_t x = 0; x < width - 2; ++x) {
            first[x] = color;
            first[pitch + x] = color;
            first[lastRow - pitch + x] = color;
            first[lastRow + x] = color;
        }
        first[2 * pitch + 1] = color;
        first[2 * pitch - 4 + width] = color;
        first[lastRow - 2 * pitch + 1] = color;
        first[width + lastRow - (2 * pitch + 4)] = color;
        std::uint32_t* side = first + pitch - 1;
        for (std::int32_t y = 0; y < height - 2; ++y) {
            side[0] = color;
            side[1] = color;
            side[width - 1] = color;
            side[width - 2] = color;
            side += pitch;
        }
        break;
    }
    case 1: {
        const std::uint32_t color = gPaletteAutoMap[colorIndex];
        std::fill_n(first, width - 2, color);
        std::uint32_t* row = first + pitch - 1;
        for (std::int32_t y = 0; y < height - 2; ++y) {
            std::fill_n(row, width, color);
            row += pitch;
        }
        std::fill_n(row + 1, width - 2, color);
        break;
    }
    case 2: {
        const std::uint32_t color = gPaletteAutoMap[colorIndex];
        // 原 32 位单行分支仍以 left+1 起笔，不按常规半开矩形修正到 left。
        if (width > 0) {
            std::fill_n(first, width, color);
        }
        break;
    }
    }
}

bool CopyBackToTemp() noexcept
{
    if (gCpuBackSurface == nullptr || gCpuTempSurface == nullptr) {
        return false;
    }
    std::copy_n(
        gCpuBackSurface->pixels.data(),
        kCpuSurfacePixelCount,
        gCpuTempSurface->pixels.data());
    return true;
}

bool BlitTempToBack(
    const std::int32_t destinationX,
    const std::int32_t destinationY,
    const std::int32_t sourceLeft,
    const std::int32_t sourceTop,
    const std::int32_t sourceRight,
    const std::int32_t sourceBottom) noexcept
{
    if (gCpuTempSurface == nullptr
        || gCpuBackSurface == nullptr
        || destinationX < 0
        || destinationY < 0
        || sourceLeft < 0
        || sourceTop < 0
        || sourceRight > kCpuSurfaceWidth
        || sourceBottom > kCpuSurfaceHeight
        || sourceLeft >= sourceRight
        || sourceTop >= sourceBottom) {
        return false;
    }

    const std::int32_t copyWidth = sourceRight - sourceLeft;
    const std::int32_t copyHeight = sourceBottom - sourceTop;
    if (destinationX > kCpuSurfaceWidth - copyWidth
        || destinationY > kCpuSurfaceHeight - copyHeight) {
        return false;
    }

    const std::size_t width = static_cast<std::size_t>(copyWidth);
    for (std::int32_t row = 0; row < copyHeight; ++row) {
        const std::size_t sourceIndex =
            static_cast<std::size_t>(sourceTop + row)
                * kCpuSurfacePitchPixels
            + static_cast<std::size_t>(sourceLeft);
        const std::size_t destinationIndex =
            static_cast<std::size_t>(destinationY + row)
                * kCpuSurfacePitchPixels
            + static_cast<std::size_t>(destinationX);
        std::copy_n(
            gCpuTempSurface->pixels.data() + sourceIndex,
            width,
            gCpuBackSurface->pixels.data() + destinationIndex);
    }
    return true;
}

} // namespace cg::render
