#include "cg/render/SurfaceState.hpp"

#include "cg/Palette.hpp"
#include "cg/render/RendererResources.hpp"

#include "BackTextDrawing.hpp"
#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <bit>
#include <memory>
#include <new>

namespace cg::render {
namespace {

struct CpuBackStorage {
    // CPU 视图与 GDI DIB 共用此映射；逻辑容量仍为可见帧加一个额外像素。
    HANDLE mapping = nullptr;
    std::uint32_t* pixels = nullptr;
    HBITMAP textBitmap = nullptr;
    HDC textDc = nullptr;
    HGDIOBJ previousBitmap = nullptr;
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> blitSurface {
        nullptr, SDL_DestroySurface
    };

    void ReleaseTextDc() noexcept
    {
        if (textDc == nullptr) {
            return;
        }
        static_cast<void>(::GdiFlush());
        static_cast<void>(::SelectObject(textDc, previousBitmap));
        static_cast<void>(::DeleteDC(textDc));
        textDc = nullptr;
        previousBitmap = nullptr;
        SDL_UnlockSurface(blitSurface.get());
    }

    ~CpuBackStorage()
    {
        ReleaseTextDc();
        blitSurface.reset();
        if (textBitmap != nullptr) {
            static_cast<void>(::DeleteObject(textBitmap));
        }
        if (pixels != nullptr) {
            static_cast<void>(::UnmapViewOfFile(pixels));
        }
        if (mapping != nullptr) {
            static_cast<void>(::CloseHandle(mapping));
        }
    }
};

// 原 556300..556302，仅描边模式为 256 号色读取；后续地址为独立字符串。
constexpr std::uint8_t kOutlineExtraColors[1][3] {{0xA2, 0xE8, 0xFF}};

std::unique_ptr<CpuBackStorage> gCpuBackSurface;
TextureSurface* gCpuAutoMapSurface {};
TextureSurface* gCpuTempSurface {};

// 两个 32 位 AutoMap 本体各自持有闪烁状态；renderer 重建不重置。
std::uint16_t gIndexedAutoMapBlink62B07C = 0;
std::uint32_t gIndexedAutoMapTick62B080 = 0;
std::uint16_t gColorAutoMapBlink62B094 = 0;
std::uint32_t gColorAutoMapTick62B098 = 0;

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

// 原 44F330/44FC20 的公共投影顺序；取色类型及两组闪烁槽由各入口提供。
template <typename Pixel, typename ReadColor>
std::int32_t DrawAutoMap32(
    const std::int32_t x, const std::int32_t y, const Pixel* source,
    const std::int32_t width, const std::int32_t height,
    std::uint16_t& blink, std::uint32_t& tick, const ReadColor readColor)
{
    constexpr auto failure = std::bit_cast<std::int32_t>(0x80004005u);
    constexpr auto surfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);
    TextureRectangle rectangle {0, 0, 2 * (width + height) + 2, width + height + 2};
    SDL_Surface* const surface = GetTextureSdlSurface(gCpuAutoMapSurface);
    if (surface == nullptr) {
        return failure;
    }
    const SDL_Rect fillRect {0, 0, rectangle[2], rectangle[3]};
    if ((surface->flags & SDL_SURFACE_LOCKED) == 0) {
        static_cast<void>(SDL_FillSurfaceRect(surface, &fillRect, 0));
    }
    if ((surface->flags & SDL_SURFACE_LOCKED) != 0) {
        return surfaceBusy;
    }
    if (!SDL_LockSurface(surface)) {
        return failure;
    }

    const std::int32_t pitch = surface->pitch >> 2;
    auto* const origin = static_cast<std::uint32_t*>(surface->pixels) + pitch * (width + 1) + 1;
    std::uint32_t* row = origin;
    for (std::int32_t rowIndex = 0; rowIndex < height; ++rowIndex) {
        std::uint32_t* point = row;
        for (std::int32_t column = 0; column < width; ++column) {
            const std::uint32_t color = readColor(source[column]);
            point[-1] = color;
            point[0] = color;
            point[1] = color;
            point[-pitch] = color;
            point += 2 - pitch;
        }
        row += pitch + 2;
        source += width;
    }

    const std::uint32_t centerColor = blink != 0 ? 0xFFFFFFu : 1u;
    if (tick + 1000u <= ::GetTickCount()) {
        tick = ::GetTickCount();
        blink = static_cast<std::uint16_t>((static_cast<std::uint8_t>(blink) - 1) & 1);
    }
    std::uint32_t* const center =
        origin + height * (pitch + 2) / 2 - width * (pitch - 2) / 2;
    center[-1] = centerColor;
    center[0] = centerColor;
    center[1] = centerColor;
    center[-pitch] = centerColor;
    SDL_UnlockSurface(surface);

    std::int32_t destinationX = x + 17;
    std::int32_t destinationY = 117 - width + y;
    if (destinationX < 0) {
        rectangle[0] -= destinationX;
        destinationX = 0;
    }
    if (destinationX + rectangle[2] - rectangle[0] > kCpuSurfaceWidth) {
        rectangle[2] = rectangle[0] - destinationX + kCpuSurfaceWidth;
    }
    if (destinationY < 0) {
        rectangle[1] -= destinationY;
        destinationY = 0;
    }
    if (destinationY + rectangle[3] - rectangle[1] > kCpuSurfaceHeight) {
        rectangle[3] = rectangle[1] - destinationY + kCpuSurfaceHeight;
    }
    const TextureRectangle destination {
        destinationX, destinationY,
        destinationX + rectangle[2] - rectangle[0],
        destinationY + rectangle[3] - rectangle[1]
    };
    return BlitTextureToBack(gCpuAutoMapSurface, rectangle, destination);
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
    constexpr std::size_t storageBytes =
        (kCpuSurfacePixelCount + 1) * sizeof(std::uint32_t);
    gCpuBackSurface->mapping = ::CreateFileMappingW(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
        static_cast<DWORD>(storageBytes), nullptr);
    if (gCpuBackSurface->mapping == nullptr) {
        gCpuBackSurface.reset();
        return false;
    }
    gCpuBackSurface->pixels = static_cast<std::uint32_t*>(::MapViewOfFile(
        gCpuBackSurface->mapping, FILE_MAP_ALL_ACCESS, 0, 0, storageBytes));
    if (gCpuBackSurface->pixels == nullptr) {
        gCpuBackSurface.reset();
        return false;
    }
    std::uninitialized_value_construct_n(gCpuBackSurface->pixels, kCpuSurfacePixelCount + 1);
    gCpuBackSurface->blitSurface.reset(SDL_CreateSurfaceFrom(
        kCpuSurfaceWidth, kCpuSurfaceHeight, SDL_PIXELFORMAT_XRGB8888,
        gCpuBackSurface->pixels, static_cast<int>(kCpuSurfacePitchBytes)));
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

std::int32_t DrawIndexedAutoMap32_44F330(
    const std::int32_t x, const std::int32_t y, const std::uint8_t* const source,
    const std::int32_t width, const std::int32_t height)
{
    return DrawAutoMap32(x, y, source, width, height,
        gIndexedAutoMapBlink62B07C, gIndexedAutoMapTick62B080,
        [](const std::uint8_t index) {
            const std::uint32_t color = index == 0 ? 0 : gPaletteAutoMap[index];
            return color == 0 ? 1u : color;
        });
}

std::int32_t DrawColorAutoMap32_44FC20(
    const std::int32_t x, const std::int32_t y, const std::uint32_t* const source,
    const std::int32_t width, const std::int32_t height)
{
    return DrawAutoMap32(x, y, source, width, height,
        gColorAutoMapBlink62B094, gColorAutoMapTick62B098,
        [](const std::uint32_t color) { return color == 0 ? 1u : color; });
}

std::int32_t DrawIndexedAutoMap_44F5C0(
    const std::int32_t x, const std::int32_t y, const std::uint8_t* const source,
    const std::int32_t width, const std::int32_t height)
{
    return DrawIndexedAutoMap32_44F330(x, y, source, width, height);
}

std::int32_t DrawColorAutoMap_44FEB0(
    const std::int32_t x, const std::int32_t y, const std::uint32_t* const source,
    const std::int32_t width, const std::int32_t height)
{
    return DrawColorAutoMap32_44FC20(x, y, source, width, height);
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
    return {gCpuBackSurface->pixels, kCpuSurfacePixelCount};
}

SDL_Surface* GetCpuBackSdlSurface() noexcept
{
    return gCpuBackSurface == nullptr ? nullptr : gCpuBackSurface->blitSurface.get();
}

HDC AcquireCpuBackTextDc() noexcept
{
    CpuBackStorage* const back = gCpuBackSurface.get();
    if (back == nullptr) {
        return nullptr;
    }
    SDL_Surface* const surface = back->blitSurface.get();
    if ((surface->flags & SDL_SURFACE_LOCKED) != 0 || !SDL_LockSurface(surface)) {
        return nullptr;
    }
    if (back->textBitmap == nullptr) {
        BITMAPINFO description {};
        description.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        description.bmiHeader.biWidth = kCpuSurfaceWidth;
        description.bmiHeader.biHeight = -kCpuSurfaceHeight;
        description.bmiHeader.biPlanes = 1;
        description.bmiHeader.biBitCount = 32;
        description.bmiHeader.biCompression = BI_RGB;
        void* bitmapPixels = nullptr;
        back->textBitmap = ::CreateDIBSection(
            nullptr, &description, DIB_RGB_COLORS, &bitmapPixels, back->mapping, 0);
        if (back->textBitmap == nullptr) {
            SDL_UnlockSurface(surface);
            return nullptr;
        }
    }
    const HDC dc = ::CreateCompatibleDC(nullptr);
    if (dc == nullptr) {
        SDL_UnlockSurface(surface);
        return nullptr;
    }
    const HGDIOBJ previousBitmap = ::SelectObject(dc, back->textBitmap);
    if (previousBitmap == nullptr || previousBitmap == HGDI_ERROR) {
        static_cast<void>(::DeleteDC(dc));
        SDL_UnlockSurface(surface);
        return nullptr;
    }
    back->previousBitmap = previousBitmap;
    back->textDc = dc;
    return dc;
}

void ReleaseCpuBackTextDc(const HDC dc) noexcept
{
    if (gCpuBackSurface != nullptr && dc != nullptr && gCpuBackSurface->textDc == dc) {
        gCpuBackSurface->ReleaseTextDc();
    }
}

std::int32_t ClearSurface_4512C0() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return -1;
    }
    const std::uint32_t color = gBackgroundColor;
    std::fill_n(gCpuBackSurface->pixels, kCpuSurfacePixelCount, color);
    return 0;
}

void Sub_451350(RECT* const rect, const std::uint16_t colorIndex, const std::int32_t mode)
{
    if (rect->left >= GetRendererLogicalWidth() || rect->right < 0
        || rect->top >= GetRendererLogicalHeight() || rect->bottom < 0) {
        return;
    }
    if (rect->left < 0) {
        rect->left = 0;
    }
    const std::int32_t logicalWidth = GetRendererLogicalWidth();
    if (rect->right >= logicalWidth) {
        rect->right = logicalWidth;
    }
    if (rect->top < 0) {
        rect->top = 0;
    }
    const std::int32_t logicalHeight = GetRendererLogicalHeight();
    if (rect->bottom >= logicalHeight) {
        rect->bottom = logicalHeight;
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
        gCpuBackSurface->pixels + rect->top * pitch + rect->left + 1;
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
    SDL_Surface* const source = gCpuBackSurface->blitSurface.get();
    SDL_Surface* const destination = GetTextureSdlSurface(gCpuTempSurface);
    // 原空源 RECT 复制完整 Back；BltFast 不裁剪，目标无法容纳时失败。
    if (source == nullptr || destination == nullptr
        || source->w > destination->w || source->h > destination->h) {
        return false;
    }
    const SDL_Rect rectangle {0, 0, source->w, source->h};
    return BlitWithoutSourceColorKey(source, rectangle, destination, rectangle);
}

TextureSurface* BorrowCpuTempSurface() noexcept
{
    return gCpuTempSurface;
}

bool BlitSurfaceRectangleToBack(
    const std::int32_t destinationX, const std::int32_t destinationY,
    const std::array<std::int32_t, 4>& rectangle, TextureSurface* const source) noexcept
{
    SDL_Surface* const sourceSurface = GetTextureSdlSurface(source);
    if (sourceSurface == nullptr || gCpuBackSurface == nullptr
        || gCpuBackSurface->blitSurface == nullptr) {
        return false;
    }
    SDL_Surface* const destination = gCpuBackSurface->blitSurface.get();
    // 原直接BltFast不裁剪；先完成资源层范围判定，避免Unchecked越界访问。
    if (rectangle[0] < 0 || rectangle[1] < 0
        || rectangle[2] <= rectangle[0] || rectangle[3] <= rectangle[1]
        || rectangle[2] > sourceSurface->w || rectangle[3] > sourceSurface->h) {
        return false;
    }
    const std::int32_t width = rectangle[2] - rectangle[0];
    const std::int32_t height = rectangle[3] - rectangle[1];
    if (destinationX < 0 || destinationY < 0
        || width > destination->w || height > destination->h
        || destinationX > destination->w - width
        || destinationY > destination->h - height) {
        return false;
    }
    const SDL_Rect sourceRect {
        rectangle[0], rectangle[1],
        width, height
    };
    const SDL_Rect destinationRect {
        destinationX, destinationY, sourceRect.w, sourceRect.h
    };
    return BlitWithoutSourceColorKey(
        sourceSurface, sourceRect, destination, destinationRect);
}

std::int32_t BlitSurfaceToBack_4535B0(
    const std::int16_t x, const std::int16_t y,
    std::array<std::int32_t, 4>& rectangle, TextureSurface* const source) noexcept
{
    const std::int32_t top = rectangle[1];
    const std::int32_t right = rectangle[2];
    const std::int32_t left = rectangle[0];
    const std::int32_t bottom = rectangle[3];
    const std::int32_t copyHeight = bottom - top;
    const std::int32_t copyWidth = right - left;
    if (x >= GetRendererLogicalWidth() || x + copyWidth <= 0
        || y >= GetRendererLogicalHeight() || y + copyHeight <= 0) {
        return 0;
    }

    std::int32_t destinationX = x;
    if (x < 0) {
        destinationX = 0;
        rectangle[0] = left - x;
    }
    const std::int32_t width = GetRendererLogicalWidth();
    if (x + copyWidth > width) {
        rectangle[2] = width - x - copyWidth + right;
    }
    std::int32_t destinationY = y;
    if (y < 0) {
        destinationY = 0;
        rectangle[1] = top - y;
    }
    const std::int32_t height = GetRendererLogicalHeight();
    if (y + copyHeight > height) {
        rectangle[3] = height - y - copyHeight + bottom;
    }

    SDL_Surface* const sourceSurface = GetTextureSdlSurface(source);
    if (sourceSurface == nullptr || gCpuBackSurface == nullptr
        || gCpuBackSurface->blitSurface == nullptr) {
        return -1;
    }
    const SDL_Rect sourceRect {
        rectangle[0], rectangle[1],
        rectangle[2] - rectangle[0], rectangle[3] - rectangle[1]
    };
    const SDL_Rect destinationRect {
        destinationX, destinationY, sourceRect.w, sourceRect.h
    };
    return BlitWithoutSourceColorKey(
        sourceSurface, sourceRect, gCpuBackSurface->blitSurface.get(), destinationRect) ? 0 : -1;
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

    return BlitTempRectangleToBack(
        destinationX, destinationY, {sourceLeft, sourceTop, sourceRight, sourceBottom});
}

bool BlitTempRectangleToBack(
    const std::int32_t destinationX, const std::int32_t destinationY,
    const TextureRectangle& source) noexcept
{
    if (gCpuTempSurface == nullptr || gCpuBackSurface == nullptr) {
        return false;
    }
    const SDL_Rect sourceRect {source[0], source[1], source[2] - source[0], source[3] - source[1]};
    const SDL_Rect destinationRect {destinationX, destinationY, sourceRect.w, sourceRect.h};
    return BlitWithoutSourceColorKey(
        GetTextureSdlSurface(gCpuTempSurface), sourceRect,
        gCpuBackSurface->blitSurface.get(), destinationRect);
}

} // namespace cg::render
