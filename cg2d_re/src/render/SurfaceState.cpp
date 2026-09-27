#include "cg/render/SurfaceState.hpp"

#include "cg/Palette.hpp"

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <bit>
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

std::unique_ptr<CpuBackStorage> gCpuBackSurface;
TextureSurface* gCpuAutoMapSurface {};
TextureSurface* gCpuTempSurface {};

// 原等待型 BltFast 不启用源色键；只在同步复制范围内改变 SDL 包装标志。
bool BlitWithoutSourceColorKey(
    SDL_Surface* source, const SDL_Rect& sourceRect,
    SDL_Surface* destination, const SDL_Rect& destinationRect) noexcept
{
    if (((source->flags | destination->flags) & SDL_SURFACE_LOCKED) != 0) {
        return false;
    }
    const bool hadColorKey = SDL_SurfaceHasColorKey(source);
    std::uint32_t colorKey = 0;
    if (hadColorKey) {
        if (!SDL_GetSurfaceColorKey(source, &colorKey)
            || !SDL_SetSurfaceColorKey(source, false, colorKey)) {
            return false;
        }
    }
    const bool copied = SDL_BlitSurfaceUnchecked(source, &sourceRect, destination, &destinationRect);
    if (hadColorKey && !SDL_SetSurfaceColorKey(source, true, colorKey)) {
        return false;
    }
    return copied;
}

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

std::int32_t DrawMapEffects32_452840()
{
    constexpr auto failure = std::bit_cast<std::int32_t>(0x80004005u);
    constexpr auto surfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (back == nullptr) {
        return failure;
    }
    if ((back->flags & SDL_SURFACE_LOCKED) != 0) {
        return surfaceBusy;
    }
    if (!SDL_LockSurface(back)) {
        return failure;
    }

    const std::int32_t pitch = back->pitch >> 2;
    auto* const pixels = static_cast<std::uint32_t*>(back->pixels);
    const world::detail::MapEffectRecord* record = world::detail::BorrowActiveMapEffectHead();
    while (record != nullptr) {
        const auto readInt = [record](const std::size_t offset) {
            return std::bit_cast<std::int32_t>(world::detail::ReadMapEffectWord(*record, offset));
        };
        const std::int32_t x = readInt(0);
        if (x >= 0 && x < 636) {
            const std::int32_t y = readInt(4);
            if (y >= 0 && y < 468) {
                std::uint32_t* const point = pixels + y * pitch + x;
                const std::uint16_t kind = world::detail::ReadMapEffectHalf(*record, 0x3C);
                switch (kind) {
                case 1:
                    point[0] = gPaletteAutoMap[143];
                    point[pitch] = gPaletteAutoMap[143];
                    point[2 * pitch + 1] = gPaletteAutoMap[143];
                    point[3 * pitch + 1] = gPaletteAutoMap[143];
                    break;
                case 2: {
                    const std::uint16_t variant = world::detail::ReadMapEffectHalf(*record, 0x40);
                    if (variant == 0) {
                        point[0] = gPaletteAutoMap[161];
                        point[1] = gPaletteAutoMap[159];
                        point[2] = gPaletteAutoMap[161];
                        point[pitch] = gPaletteAutoMap[159];
                        point[pitch + 1] = gPaletteAutoMap[159];
                        point[pitch + 2] = gPaletteAutoMap[159];
                        point[2 * pitch] = gPaletteAutoMap[161];
                        point[2 * pitch + 1] = gPaletteAutoMap[159];
                        point[2 * pitch + 2] = gPaletteAutoMap[161];
                    } else if (variant == 1) {
                        point[0] = gPaletteAutoMap[162];
                        point[1] = gPaletteAutoMap[159];
                        point[2] = gPaletteAutoMap[159];
                        point[3] = gPaletteAutoMap[162];
                        point[pitch] = gPaletteAutoMap[159];
                        point[pitch + 1] = gPaletteAutoMap[159];
                        point[pitch + 2] = gPaletteAutoMap[159];
                        point[pitch + 3] = gPaletteAutoMap[159];
                        point[2 * pitch] = gPaletteAutoMap[159];
                        point[2 * pitch + 1] = gPaletteAutoMap[159];
                        point[2 * pitch + 2] = gPaletteAutoMap[159];
                        point[2 * pitch + 3] = gPaletteAutoMap[159];
                        point[3 * pitch] = gPaletteAutoMap[162];
                        point[3 * pitch + 1] = gPaletteAutoMap[159];
                        point[3 * pitch + 2] = gPaletteAutoMap[159];
                        point[3 * pitch + 3] = gPaletteAutoMap[162];
                    }
                    break;
                }
                case 3:
                    switch (world::detail::ReadMapEffectHalf(*record, 0x40)) {
                    case 0:
                        point[-pitch - 1] = gPaletteAutoMap[8];
                        point[-pitch] = gPaletteAutoMap[255];
                        point[-pitch + 1] = gPaletteAutoMap[8];
                        point[-1] = gPaletteAutoMap[255];
                        point[0] = gPaletteAutoMap[255];
                        point[1] = gPaletteAutoMap[255];
                        point[pitch - 1] = gPaletteAutoMap[8];
                        point[pitch] = gPaletteAutoMap[255];
                        point[pitch + 1] = gPaletteAutoMap[8];
                        break;
                    case 1:
                        point[-pitch] = gPaletteAutoMap[251];
                        point[-1] = gPaletteAutoMap[251];
                        point[0] = gPaletteAutoMap[251];
                        point[1] = gPaletteAutoMap[251];
                        point[pitch] = gPaletteAutoMap[251];
                        break;
                    case 2:
                        point[0] = gPaletteAutoMap[251];
                        point[1] = gPaletteAutoMap[255];
                        point[pitch] = gPaletteAutoMap[255];
                        point[pitch + 1] = gPaletteAutoMap[251];
                        break;
                    case 3:
                        point[0] = gPaletteAutoMap[255];
                        point[1] = gPaletteAutoMap[251];
                        point[pitch] = gPaletteAutoMap[251];
                        point[pitch + 1] = gPaletteAutoMap[255];
                        break;
                    case 4:
                        point[0] = gPaletteAutoMap[251];
                        point[1] = gPaletteAutoMap[193];
                        point[pitch] = gPaletteAutoMap[193];
                        point[pitch + 1] = gPaletteAutoMap[193];
                        break;
                    case 5:
                        point[0] = gPaletteAutoMap[198];
                        break;
                    case 6:
                        point[0] = gPaletteAutoMap[193];
                        break;
                    case 7:
                        point[0] = gPaletteAutoMap[208];
                        break;
                    case 8:
                        point[0] = gPaletteAutoMap[251];
                        break;
                    }
                    break;
                case 4: {
                    const std::uint16_t variant = world::detail::ReadMapEffectHalf(*record, 0x40);
                    const std::uint16_t phase = world::detail::ReadMapEffectHalf(*record, 0x3E);
                    std::uint32_t color;
                    if (variant == 0) {
                        color = gPaletteAutoMap[phase == 0 ? 223 : phase == 1 ? 217 : 159];
                    } else if (variant == 1) {
                        color = gPaletteAutoMap[phase == 0 ? 212 : phase == 1 ? 208 : 159];
                    } else if (variant == 2) {
                        color = gPaletteAutoMap[phase == 0 ? 130 : phase == 1 ? 147 : 159];
                    } else if (phase == 0) {
                        color = gPaletteAutoMap[116];
                    } else {
                        color = gPaletteAutoMap[99];
                        if (phase != 1) {
                            color = gPaletteAutoMap[159];
                        }
                    }
                    std::int32_t height = readInt(0x2C);
                    std::int32_t row = 0;
                    if (height > 0) {
                        do {
                            std::int32_t width = readInt(0x28);
                            std::int32_t column = 0;
                            if (width > 0) {
                                do {
                                    const std::int32_t localX = readInt(0x20) + column;
                                    if (localX >= 0 && localX < kCpuSurfaceWidth) {
                                        const std::int32_t localY = readInt(0x24) + row;
                                        if (localY >= 0 && localY < kCpuSurfaceHeight) {
                                            // 原目标基址已带记录原点；门禁只检查局部坐标。
                                            point[localY * pitch + localX] = color;
                                        }
                                    }
                                    width = readInt(0x28);
                                    ++column;
                                } while (column < width);
                            }
                            height = readInt(0x2C);
                            ++row;
                        } while (row < height);
                    }
                    break;
                }
                }
            }
        }
        record = world::detail::BorrowNextMapEffect(record);
    }
    SDL_UnlockSurface(back);
    return 0;
}

std::int32_t DrawMapEffects_453580()
{
    return DrawMapEffects32_452840();
}

bool CreateCpuAutoMapSurface() noexcept
{
    gCpuAutoMapSurface = CreatePlainTextureSurface_44C8B0(
        kCpuSurfaceWidth, kCpuSurfaceHeight, 0, 0x800);
    return gCpuAutoMapSurface != nullptr;
}

bool CreateCpuTempSurface() noexcept
{
    gCpuTempSurface = CreatePlainTextureSurface_44C8B0(
        kCpuSurfaceWidth, kCpuSurfaceHeight, 0, 0x4000);
    return gCpuTempSurface != nullptr;
}

bool CreateCpuTempSurfaceFallback() noexcept
{
    gCpuTempSurface = CreateTextureSurface_44C6D0(
        kCpuSurfaceWidth, kCpuSurfaceHeight, 0, 0x800);
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
    ReleaseTextureSurface(gCpuTempSurface);
    gCpuTempSurface = nullptr;
    ReleaseTextureSurface(gCpuAutoMapSurface);
    gCpuAutoMapSurface = nullptr;
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
    const SDL_Rect rectangle {0, 0, kCpuSurfaceWidth, kCpuSurfaceHeight};
    return BlitWithoutSourceColorKey(
        gCpuBackSurface->blitSurface.get(), rectangle,
        GetTextureSdlSurface(gCpuTempSurface), rectangle);
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

    const SDL_Rect sourceRect {sourceLeft, sourceTop, copyWidth, copyHeight};
    const SDL_Rect destinationRect {destinationX, destinationY, copyWidth, copyHeight};
    return BlitWithoutSourceColorKey(
        GetTextureSdlSurface(gCpuTempSurface), sourceRect,
        gCpuBackSurface->blitSurface.get(), destinationRect);
}

} // namespace cg::render
