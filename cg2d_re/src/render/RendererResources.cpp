#include "cg/render/RendererResources.hpp"

#include "GraphicDxCacheState.hpp"
#include "../platform/LegacyFormat.hpp"
#include "LegacyTextureDrawing.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/SurfaceState.hpp"

#include <SDL3/SDL_surface.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <algorithm>
#include <bit>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>
#include <memory>
#include <new>
#include <utility>

namespace cg::render {

struct PaletteBinding {
    PaletteEntry entries[256] {};
};

// 原 +9C 接口的 native 承载；实际 Back 格式由 SDL 管理，设备创建借用它。
struct LegacyTextureInterface {
    const SDL_PixelFormatDetails* pixelFormat;
};

struct RendererResources {
    std::unique_ptr<PaletteBinding> palette;
    std::int32_t logicalWidth = 0; // 原 +94，44B75A 发布。
    std::int32_t logicalHeight = 0; // 原 +98，44B76A 发布。
    // 原纹理/像素格式字段；GPTR 零初值，由 RendererInit 在原时点发布。
    LegacyTextureState textureState {};
    std::unique_ptr<LegacyTextureInterface> textureInterface;
    std::unique_ptr<LegacyTextureDevice, decltype(&DestroyLegacyTextureDevice)> textureDevice {
        nullptr, DestroyLegacyTextureDevice
    };
};

RendererResources* gRenderer = nullptr;
std::uint32_t gLegacyTextureStage = 0; // 原全局：0x0062A648。
std::uint32_t gRendererRedMask = 0; // 原全局：0x00628FE4。
std::uint32_t gRendererGreenMask = 0; // 原全局：0x0062A660。
std::uint32_t gRendererBlueMask = 0; // 原全局：0x00628FC0。
std::int32_t gRendererRedShift = 0; // 原全局：0x006287A8。
std::int32_t gRendererGreenShift = 0; // 原全局：0x0062A658。
std::int32_t gRendererBlueShift = 0; // 原全局：0x00628BB8。
std::int32_t gRendererRedOffset = 0; // 原全局：0x00629314。
std::int32_t gRendererGreenOffset = 0; // 原全局：0x00629718。
std::int32_t gRendererBlueOffset = 0; // 原全局：0x0062971C。

namespace {

constexpr wchar_t kConfirmCaption[] = L"確認";
constexpr wchar_t kMode1Message[] =
    LR"(先前設定的畫面成像模式無法啟動。
將以畫面成像模式1啟動。)";
bool CreateRendererTextureInterface() noexcept
{
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (back == nullptr) {
        return false;
    }
    const SDL_PixelFormatDetails* const format = SDL_GetPixelFormatDetails(back->format);
    if (format == nullptr) {
        return false;
    }
    std::unique_ptr<LegacyTextureInterface> pending(
        new (std::nothrow) LegacyTextureInterface {format});
    if (pending == nullptr) {
        return false;
    }
    gRenderer->textureInterface = std::move(pending);
    return true;
}

// 原两处 CreateFile/WriteFile；正常分支须在成功打开之后读取支持模式。
void WriteRendererModeFile(const bool fallback)
{
    char buffer[11] {};
    if (fallback) {
        buffer[0] = '1'; // 原 55638C 的 31 00。
    }
    const HANDLE file = ::CreateFileA(
        "data\\update.dat", GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        if (!fallback) {
            std::sprintf(buffer, "%d",
                static_cast<int>(std::bit_cast<std::int8_t>(platform::gSupportedRenderMode)));
        }
        DWORD written;
        ::WriteFile(file, buffer, static_cast<DWORD>(std::strlen(buffer)), &written, nullptr);
        ::CloseHandle(file);
    }
}

// 原 44BD6C 公共降级；不是早期 AL=1，也不释放 Back 或 presenter。
std::int8_t DowngradeRenderer()
{
    ReleaseRendererTextureDevice();
    ReleaseRendererTextureInterface();
    gRenderer->textureState.colorKeyEnabled = 0;
    WriteRendererModeFile(true);
    platform::ShowWideMessage(kMode1Message, kConfirmCaption, 0x10);
    platform::gSupportedRenderMode = 1;
    platform::gRendererMode = 1;
    return 0;
}

} // namespace

const LegacyTextureState& GetLegacyTextureState()
{
    return gRenderer->textureState;
}

// 原函数：0x0044B610。
std::int32_t HasLegacy3DDevice_44B610()
{
    return gRenderer->textureInterface != nullptr ? 1 : 0;
}

std::int32_t GetRendererLogicalWidth() noexcept
{
    return gRenderer->logicalWidth;
}

std::int32_t GetRendererLogicalHeight() noexcept
{
    return gRenderer->logicalHeight;
}

bool CreateRendererTextureDevice() noexcept
{
    if (gRenderer == nullptr || gRenderer->textureInterface == nullptr) {
        return false;
    }
    LegacyTextureDevice* const device = CreateLegacyTextureDevice(
        gRenderer->textureInterface->pixelFormat, GetCpuBackSdlSurface());
    if (device == nullptr) {
        return false;
    }
    gRenderer->textureDevice.reset(device);
    return true;
}

void ReleaseRendererTextureInterface() noexcept
{
    if (gRenderer != nullptr) {
        gRenderer->textureInterface.reset();
    }
}

void ReleaseRendererTextureDevice() noexcept
{
    if (gRenderer != nullptr) {
        gRenderer->textureDevice.reset();
    }
}

LegacyTextureDevice* GetRendererTextureDevice() noexcept
{
    return gRenderer == nullptr ? nullptr : gRenderer->textureDevice.get();
}

void PublishRendererPixelFormat32_44BB1E() noexcept
{
    gRenderer->textureState.pixelBitCount = 32;
    gRendererRedMask = 0x00FF0000u;
    gRendererGreenMask = 0x0000FF00u;
    gRendererBlueMask = 0x000000FFu;
    gRendererRedShift = 8 - CountMaskBits_453DE0(gRendererRedMask);
    gRendererGreenShift = 8 - CountMaskBits_453DE0(gRendererGreenMask);
    gRendererBlueShift = 8 - CountMaskBits_453DE0(gRendererBlueMask);
    gRendererRedOffset = CountMaskBits_453DE0(gRendererBlueMask)
        + CountMaskBits_453DE0(gRendererGreenMask);
    gRendererGreenOffset = CountMaskBits_453DE0(gRendererBlueMask);
    gRendererBlueOffset = 0;
}

// 原函数：0x0044B630。
std::int8_t RendererInit_44B630()
{
    platform::gRendererLost = 0;
    const HDC dc = ::GetDC(reinterpret_cast<HWND>(platform::gLegacyWindow));
    platform::gDeviceBitsPerPixel = ::GetDeviceCaps(dc, BITSPIXEL);
    ::ReleaseDC(reinterpret_cast<HWND>(platform::gLegacyWindow), dc);
    if ((platform::gRendererFlags & 8u) != 0 && platform::gDeviceBitsPerPixel < 16) {
        platform::ShowWideMessage(L"因螢幕顏色不是16Bit，顯示畫面將改為全螢幕顯示。", kConfirmCaption, 0x10);
        return 1;
    }
    if (!AllocateRendererResources()) {
        platform::ShowWideMessage(L"GlobalAllocPtr Error ( DIRECT_DRAW )", kConfirmCaption, 0x10);
        return 1;
    }
    gRenderer->logicalWidth = kCpuSurfaceWidth;
    gRenderer->logicalHeight = kCpuSurfaceHeight;
    gTextureSizeW = 64;
    gTextureSizeH = 48;
    if (!CreateRendererPresenter()) {
        platform::ShowWideMessage(L"DirectDrawCreateEx Error", kConfirmCaption, 0x10);
        return 1;
    }
    if (!SetRendererPresenterFullscreen(platform::gWindowMode == 0)) {
        platform::ShowWideMessage(L"SetCooperativeLevel Error", kConfirmCaption, 0x10);
        return 1;
    }
    if (!CreateCpuBackSurface()) {
        platform::ShowWideMessage(L"Back Buffer產生失敗。", kConfirmCaption, 0x10);
        return 1;
    }
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (back == nullptr) {
        return 1;
    }
    const SDL_PixelFormatDetails* const format = SDL_GetPixelFormatDetails(back->format);
    if (format == nullptr) {
        return 1;
    }
    // 早期门禁保留桌面采样，此后 BITSPIXEL 对应已批准的实际 32 位 CPU Back。
    platform::gDeviceBitsPerPixel = format->bits_per_pixel;
    PublishRendererPixelFormat32_44BB1E();

    std::int8_t mode = std::bit_cast<std::int8_t>(platform::gRendererMode);
    if (mode <= 1) {
        platform::gSupportedRenderMode = platform::gRendererMode;
        if (mode < 1) {
            platform::ShowWideMessage(kMode1Message, kConfirmCaption, 0x10);
            mode = 1;
            platform::gSupportedRenderMode = 1;
            platform::gRendererMode = 1;
        }
    }
    if ((platform::gRendererFlags & 1u) != 0) {
        mode = 1;
        platform::gSupportedRenderMode = 1;
        platform::gRendererMode = 1;
    }
    if (format->Amask != 0 || SDL_ISPIXELFORMAT_FOURCC(back->format)
        || gRenderer->textureState.pixelBitCount
            != static_cast<std::uint32_t>(platform::gDeviceBitsPerPixel)) {
        platform::ShowWideMessage(kMode1Message, kConfirmCaption, 0x10);
        platform::gSupportedRenderMode = 1;
        platform::gRendererMode = 1;
        return 0;
    }
    if (mode <= 1) {
        return 0;
    }
    if (!CreateRendererTextureInterface()) {
        platform::ShowWideMessage(L"Direct3D Object產生失敗。", kConfirmCaption, 0x10);
        return DowngradeRenderer();
    }
    // 保留三个原创建分支；native 均执行实际 CPU 设备分配，不冒称旧硬件 GUID。
    if (std::bit_cast<std::int8_t>(platform::gRendererMode) >= 4
        && CreateRendererTextureDevice()) {
        platform::gSupportedRenderMode = 4;
    } else if (std::bit_cast<std::int8_t>(platform::gRendererMode) >= 3
        && CreateRendererTextureDevice()) {
        platform::gSupportedRenderMode = 3;
    } else if (std::bit_cast<std::int8_t>(platform::gRendererMode) >= 2
        && CreateRendererTextureDevice()) {
        platform::gSupportedRenderMode = 2;
    } else {
        return DowngradeRenderer();
    }
    const SDL_PixelFormatDetails* const deviceFormat =
        GetLegacyTextureDeviceFormat(GetRendererTextureDevice());
    if (deviceFormat == nullptr) {
        return DowngradeRenderer();
    }
    gRenderer->textureState.colorKeyEnabled = 0;
    if (deviceFormat->format == SDL_PIXELFORMAT_XRGB8888) {
        if (!EnableLegacyTextureColorKey(GetRendererTextureDevice())) {
            return DowngradeRenderer();
        }
        gRenderer->textureState.colorKeyEnabled = 1;
    }
    if (!ConfigureLegacyTexturePipeline(GetRendererTextureDevice())) {
        return DowngradeRenderer();
    }
    if (GetLegacyTextureDeviceFormat(GetRendererTextureDevice()) == nullptr) {
        return DowngradeRenderer();
    }
    // CPU 采样使用实际尺寸，既不要求幂次方也不要求正方形。
    gRenderer->textureState.textureCaps = 0;
    gRenderer->textureState.width = static_cast<std::int16_t>(gTextureSizeW + 2);
    gRenderer->textureState.height = static_cast<std::int16_t>(gTextureSizeH + 2);
    if ((gRenderer->textureState.textureCaps & 2u) != 0) {
        const std::int16_t width = gRenderer->textureState.width;
        const std::int16_t height = gRenderer->textureState.height;
        gRenderer->textureState.width = 1;
        while (gRenderer->textureState.width < width) {
            gRenderer->textureState.width =
                static_cast<std::int16_t>(gRenderer->textureState.width * 2);
        }
        gRenderer->textureState.height = 1;
        while (gRenderer->textureState.height < height) {
            gRenderer->textureState.height =
                static_cast<std::int16_t>(gRenderer->textureState.height * 2);
        }
    }
    if ((gRenderer->textureState.textureCaps & 0x20u) != 0) {
        if (gRenderer->textureState.width <= gRenderer->textureState.height) {
            gRenderer->textureState.width = gRenderer->textureState.height;
        } else {
            gRenderer->textureState.height = gRenderer->textureState.width;
        }
    }

    TextureSurface* queried = nullptr;
    TextureSurface* const probe = CreateTextureSurface_44C6D0(256, 256, 0, 0x800);
    if (probe != nullptr) {
        std::uint32_t stage = 0;
        for (;;) {
            if (BindLegacyTexture(GetRendererTextureDevice(), 0, probe) != 0
                || QueryLegacyTexture(GetRendererTextureDevice(), 0, &queried) != 0) {
                ReleaseTextureSurface(probe);
                return DowngradeRenderer();
            }
            if (queried != nullptr) {
                ReleaseTextureSurface(queried);
                break;
            }
            // 原 null 返回后有无效虚调用；RPLAN.24 不模拟该故障现场。
            ReleaseTextureSurface(queried);
            if (++stage >= 8) {
                break;
            }
        }
        ReleaseTextureSurface(probe);
        if (stage >= 8) {
            return DowngradeRenderer();
        }
        gLegacyTextureStage = stage;
    }
    if (platform::gSupportedRenderMode == platform::gRendererMode) {
        return 0;
    }
    if (platform::gSupportedRenderMode == 3) {
        platform::ShowWideMessage(LR"(先前設定的畫面成像模式無法啟動。
將以畫面成像模式3啟動。)", kConfirmCaption, 0x10);
    }
    if (platform::gSupportedRenderMode == 2) {
        platform::ShowWideMessage(LR"(先前設定的畫面成像模式無法啟動。
將以畫面成像模式2啟動。)", kConfirmCaption, 0x10);
    }
    WriteRendererModeFile(false);
    platform::gRendererMode = platform::gSupportedRenderMode;
    return 0;
}

bool AllocateRendererResources() noexcept
{
    if (gRenderer != nullptr) {
        return false;
    }
    gRenderer = new (std::nothrow) RendererResources;
    return gRenderer != nullptr;
}

void ReleaseRendererResources() noexcept
{
    delete gRenderer;
    gRenderer = nullptr;
}

bool CreateRendererPaletteBinding(const PaletteEntry* entries) noexcept
{
    if (gRenderer == nullptr || entries == nullptr) {
        return false;
    }
    std::unique_ptr<PaletteBinding> pending(new (std::nothrow) PaletteBinding);
    if (!pending) {
        return false;
    }
    std::copy_n(entries, 256, pending->entries);
    gRenderer->palette = std::move(pending);
    return true;
}

void ReleaseRendererPaletteBinding() noexcept
{
    if (gRenderer != nullptr) {
        gRenderer->palette.reset();
    }
}

PaletteBinding* GetRendererPalette(RendererResources* renderer) noexcept
{
    return renderer == nullptr ? nullptr : renderer->palette.get();
}

std::int32_t SetPaletteEntries(
    PaletteBinding* palette, std::uint32_t flags, std::uint32_t first,
    std::uint32_t count, const PaletteEntry* entries) noexcept
{
    if (palette == nullptr || entries == nullptr || flags != 0
        || first > 256 || count > 256 - first) {
        return -1;
    }
    std::copy_n(entries, count, palette->entries + first);
    return 0;
}

} // namespace cg::render
