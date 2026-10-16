#include "LegacyTextureDrawing.hpp"

#include "GraphicDxCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <bit>
#include <cmath>
#include <memory>
#include <new>

namespace cg::render {

struct LegacyTextureDevice {
    SDL_Surface* back; // 借用唯一 Back，设备必须先于 Back 销毁。
    std::array<TextureSurfaceBinding, 8> textures;
    LegacyTextureBlend sourceBlend {LegacyTextureBlend::One};
    LegacyTextureBlend destinationBlend {LegacyTextureBlend::Zero};
    bool sceneActive = false; // 由 Begin/End 修改，四顶点绘制实际消费。
};

namespace {

constexpr auto kNativeFailure = std::bit_cast<std::int32_t>(0x80004005u);
constexpr auto kSurfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);

// GRT01：矩形只提交一次 GPU 绘制，CPU 不再逐像素计算 UV 或逐行混合。
std::int32_t DrawTextureRectangle(
    LegacyTextureDevice& device, const std::array<LegacyTextureVertex, 4>& vertices)
{
    TextureSurface* const surface = device.textures[0].Get();
    SDL_Surface* const input = GetTextureSdlSurface(surface);
    if (((device.back->flags | (input == nullptr ? 0u : input->flags)) & SDL_SURFACE_LOCKED) != 0) {
        return kSurfaceBusy;
    }
    if (!device.sceneActive) { return kNativeFailure; }
    const float width = input == nullptr ? 1.0f : static_cast<float>(input->w);
    const float height = input == nullptr ? 1.0f : static_cast<float>(input->h);
    return DrawGpuTexture(surface,
        {vertices[0].u * width, vertices[0].v * height, vertices[1].u * width, vertices[3].v * height},
        {vertices[0].x, vertices[0].y, vertices[1].x, vertices[3].y},
        vertices[0].color, device.sourceBlend, device.destinationBlend) ? 0 : kNativeFailure;
}

// 原 __ftol 在 PC53 下先转 S64，caller 取低 W32；沿用既有局部承接。
std::int32_t TruncateTextureCoordinate(const double value)
{
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value));
}

// 三个入口相同的四顶点、混合选择及 native 提交段；FillRect 绑定空纹理。
std::int32_t SubmitTexturedRectangle(
    TextureSurface* surface, const TextureRectangle& destination,
    const float u0, const float v0, const float u1, const float v1,
    const std::uint8_t mixFlags, const std::uint32_t color,
    LegacyTextureDevice* const imageDevice = nullptr)
{
    const auto left = static_cast<float>(destination[0]);
    const auto top = static_cast<float>(destination[1]);
    const auto right = static_cast<float>(destination[2]);
    const auto bottom = static_cast<float>(destination[3]);
    const std::array<LegacyTextureVertex, 4> vertices {{
        {left, top, 0.0f, 1.0f, color, u0, v0},
        {right, top, 0.0f, 1.0f, color, u1, v0},
        {right, bottom, 0.0f, 1.0f, color, u1, v1},
        {left, bottom, 0.0f, 1.0f, color, u0, v1}
    }};
    LegacyTextureBlend sourceBlend = LegacyTextureBlend::One;
    LegacyTextureBlend destinationBlend = LegacyTextureBlend::One;
    if ((mixFlags & GraphicMixFlag_Additive) == 0) {
        if (surface == nullptr && (color & 0xFF000000u) == 0xFF000000u) {
            destinationBlend = LegacyTextureBlend::Zero;
        } else {
            sourceBlend = LegacyTextureBlend::SourceAlpha;
            destinationBlend = LegacyTextureBlend::InverseSourceAlpha;
        }
    }
    if (imageDevice != nullptr) {
        imageDevice->sourceBlend = sourceBlend;
        imageDevice->destinationBlend = destinationBlend;
        static_cast<void>(BindLegacyTexture(imageDevice, gLegacyTextureStage, surface));
        return DrawTextureRectangle(*imageDevice, vertices);
    }
    return SubmitLegacyTextureFan(surface, vertices, sourceBlend, destinationBlend);
}

} // namespace

LegacyTextureDevice* CreateLegacyTextureDevice(
    const SDL_PixelFormatDetails* const format, SDL_Surface* const back) noexcept
{
    if (format == nullptr || back == nullptr) {
        return nullptr;
    }
    auto* const device = new (std::nothrow) LegacyTextureDevice {};
    if (device == nullptr) {
        return nullptr;
    }
    device->back = back;
    return device;
}

void DestroyLegacyTextureDevice(LegacyTextureDevice* const device) noexcept
{
    delete device;
}

std::int32_t BeginLegacyTextureScene(LegacyTextureDevice* const device) noexcept
{
    if (device == nullptr || device->sceneActive) {
        return kNativeFailure;
    }
    if ((device->back->flags & SDL_SURFACE_LOCKED) != 0) {
        return kSurfaceBusy;
    }
    device->sceneActive = true;
    return 0;
}

std::int32_t EndLegacyTextureScene(LegacyTextureDevice* const device) noexcept
{
    if (device == nullptr || !device->sceneActive) {
        return kNativeFailure;
    }
    device->sceneActive = false;
    return 0;
}

const SDL_PixelFormatDetails* GetLegacyTextureDeviceFormat(
    LegacyTextureDevice* const device) noexcept
{
    return device == nullptr ? nullptr : SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888);
}

bool EnableLegacyTextureColorKey(LegacyTextureDevice* const device) noexcept
{
    return device != nullptr; // ARGB 纹理的透明度在上传时承接。
}

bool ConfigureLegacyTexturePipeline(LegacyTextureDevice* const device) noexcept
{
    if (device == nullptr) {
        return false;
    }
    const SDL_Rect viewport {0, 0, GetCpuSurfaceWidth(), GetCpuSurfaceHeight()};
    return SDL_SetSurfaceClipRect(device->back, &viewport);
}

std::int32_t BindLegacyTexture(
    LegacyTextureDevice* const device, const std::uint32_t stage,
    TextureSurface* const surface) noexcept
{
    if (device == nullptr || stage >= device->textures.size()) {
        return kNativeFailure;
    }
    device->textures[stage].Bind(surface);
    return 0;
}

std::int32_t QueryLegacyTexture(
    LegacyTextureDevice* const device, const std::uint32_t stage,
    TextureSurface** const surface) noexcept
{
    if (device == nullptr || stage >= device->textures.size() || surface == nullptr) {
        return kNativeFailure;
    }
    *surface = RetainTextureSurface(device->textures[stage].Get());
    return 0;
}

std::int32_t SubmitLegacyTextureFan(
    TextureSurface* const surface, const std::array<LegacyTextureVertex, 4>& vertices,
    const LegacyTextureBlend sourceBlend, const LegacyTextureBlend destinationBlend)
{
    LegacyTextureDevice* const device = GetRendererTextureDevice();
    if (device == nullptr) {
        return kNativeFailure;
    }
    device->sourceBlend = sourceBlend;
    device->destinationBlend = destinationBlend;
    static_cast<void>(BindLegacyTexture(device, gLegacyTextureStage, surface));
    return DrawTextureRectangle(*device, vertices);
}

// 原函数：0x0044C9A0。
std::uint32_t FillRect_44C9A0(
    std::int16_t left, std::int16_t right, std::int16_t top, std::int16_t bottom,
    const std::uint32_t color, const std::uint8_t mixFlags)
{
    if (HasLegacy3DDevice_44B610() == 0) {
        return 0;
    }
    const std::int32_t width = GetRendererLogicalWidth();
    if (left >= width || right <= 0
        || top >= GetRendererLogicalHeight() || bottom <= 0) {
        return 0;
    }
    if (left < 0) {
        left = 0;
    }
    if (right > width) {
        right = static_cast<std::int16_t>(GetRendererLogicalWidth());
    }
    if (top < 0) {
        top = 0;
    }
    const std::int32_t height = GetRendererLogicalHeight();
    if (bottom > height) {
        bottom = static_cast<std::int16_t>(height);
    }
    if (HasLegacy3DDevice_44B610() == 0 || GetLegacyTextureState().textureDrawingEnabled == 0) {
        return 0;
    }
    const TextureRectangle destination {left, top, right, bottom};
    return std::bit_cast<std::uint32_t>(SubmitTexturedRectangle(
        nullptr, destination, 0.0f, 0.0f, 0.0f, 0.0f, mixFlags, color));
}

// 原函数：0x0044D520。
std::int32_t RenderTextureToSurface_44D520(
    const std::int32_t x, const std::int32_t y, TextureRectangle rectangle,
    TextureSurface* surface, const std::int32_t /*imageNo*/, const std::int16_t /*paletteIndex*/,
    const std::uint8_t mixFlags, const std::uint32_t color)
{
    auto& left = rectangle[0];
    auto& top = rectangle[1];
    auto& right = rectangle[2];
    auto& bottom = rectangle[3];
    // 原初始交集和裁剪位移用低 S16；负值分支仍读取完整 S32 实参。
    const std::int32_t lowX = static_cast<std::int16_t>(x);
    const std::int32_t lowY = static_cast<std::int16_t>(y);
    if (lowX >= GetRendererLogicalWidth() || lowX - left + right <= 0
        || lowY >= GetRendererLogicalHeight() || lowY - top + bottom <= 0) {
        return 0;
    }

    std::int32_t destinationX = lowX;
    const bool flipX = (mixFlags & GraphicMixFlag_FlipX) != 0;
    if (flipX) {
        if (lowX < 0) {
            right += lowX;
            destinationX = 0;
        }
        const std::int32_t width = GetRendererLogicalWidth();
        if (destinationX - left + right > width) {
            left = destinationX - width + right;
        }
    } else {
        if (x < 0) {
            left -= lowX;
            destinationX = 0;
        }
        const std::int32_t width = GetRendererLogicalWidth();
        if (destinationX - left + right > width) {
            right = width - destinationX + left;
        }
    }

    std::int32_t destinationY = lowY;
    const bool flipY = (mixFlags & GraphicMixFlag_FlipY) != 0;
    if (flipY) {
        if (y < 0) {
            bottom += lowY;
            destinationY = 0;
        }
        const std::int32_t height = GetRendererLogicalHeight();
        if (destinationY - top + bottom > height) {
            top = destinationY - height + bottom;
        }
    } else {
        if (y < 0) {
            top -= lowY;
            destinationY = 0;
        }
        const std::int32_t height = GetRendererLogicalHeight();
        if (destinationY - top + bottom > height) {
            bottom = height - destinationY + top;
        }
    }

    const TextureRectangle destination {
        destinationX, destinationY,
        destinationX + (right - left), destinationY + (bottom - top)
    };
    return RenderImageTexture(surface, rectangle, destination, mixFlags, color);
}

// 原函数：0x0044DA90。
std::int32_t RenderTexture_44DA90(
    const std::int16_t x, const std::int16_t y, const TextureRectangle* rectangle,
    const float scaleX, const float scaleY, TextureSurface* surface,
    const std::int32_t /*imageNo*/, const std::int16_t /*paletteIndex*/,
    const std::uint8_t mixFlags, const std::uint32_t color)
{
    // GRT01：保持完整源矩形，让 GPU 裁剪目标；不再将屏幕裁剪反算成整数源 UV。
    // 整图不需要旧小块防接缝的 +1 扩张或 0.05 texel 偏移。
    const float left = static_cast<float>(static_cast<double>((*rectangle)[0]) * scaleX);
    const float top = static_cast<float>(static_cast<double>((*rectangle)[1]) * scaleY);
    const float right = static_cast<float>(static_cast<double>((*rectangle)[2]) * scaleX);
    const float bottom = static_cast<float>(static_cast<double>((*rectangle)[3]) * scaleY);
    const TextureRectangle destination {
        x, y, TruncateTextureCoordinate(static_cast<double>(x) + right - left),
        TruncateTextureCoordinate(static_cast<double>(y) + bottom - top)
    };
    return RenderImageTexture(surface, *rectangle, destination, mixFlags, color);
}

// GRT01：整图、字形图集与 PNG 共用源矩形提交；缩放／翻转／混合均由 GPU 执行。
std::int32_t RenderImageTexture(
    TextureSurface* const surface, const TextureRectangle& source,
    const TextureRectangle& destination, const std::uint8_t mixFlags, const std::uint32_t color)
{
    SDL_Surface* const input = GetTextureSdlSurface(surface);
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (input == nullptr || back == nullptr) { return kNativeFailure; }
    if (((input->flags | back->flags) & SDL_SURFACE_LOCKED) != 0) { return kSurfaceBusy; }
    const bool flipX = (mixFlags & GraphicMixFlag_FlipX) != 0;
    const bool flipY = (mixFlags & GraphicMixFlag_FlipY) != 0;
    const bool additive = (mixFlags & GraphicMixFlag_Additive) != 0;
    return DrawGpuTexture(surface,
        {static_cast<float>(flipX ? source[2] : source[0]),
         static_cast<float>(flipY ? source[3] : source[1]),
         static_cast<float>(flipX ? source[0] : source[2]),
         static_cast<float>(flipY ? source[1] : source[3])},
        {static_cast<float>(destination[0]), static_cast<float>(destination[1]),
         static_cast<float>(destination[2]), static_cast<float>(destination[3])}, color,
        additive ? LegacyTextureBlend::One : LegacyTextureBlend::SourceAlpha,
        additive ? LegacyTextureBlend::One : LegacyTextureBlend::InverseSourceAlpha) ? 0 : kNativeFailure;
}

// R08／GRT01：纯色三角形直接提交 GPU，保留调用方裁剪与顶点顺序。
std::int32_t RenderColorTriangles(
    const std::span<const ColorVertex> vertices, const TextureRectangle& clipRectangle)
{
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (back == nullptr) { return kNativeFailure; }
    if ((back->flags & SDL_SURFACE_LOCKED) != 0) { return kSurfaceBusy; }
    const TextureRectangle clip {std::max(0, clipRectangle[0]), std::max(0, clipRectangle[1]),
        std::min(back->w, clipRectangle[2]), std::min(back->h, clipRectangle[3])};
    if (clip[2] <= clip[0] || clip[3] <= clip[1] || vertices.size() < 3) { return 0; }
    return DrawGpuTriangles(vertices, clip) ? 0 : kNativeFailure;
}

std::int32_t RenderColorRectangle(
    const TextureRectangle& rectangle, const std::uint32_t color)
{
    if (rectangle[2] <= rectangle[0] || rectangle[3] <= rectangle[1]) {
        return 0;
    }
    const auto left = static_cast<float>(rectangle[0]);
    const auto top = static_cast<float>(rectangle[1]);
    const auto right = static_cast<float>(rectangle[2]);
    const auto bottom = static_cast<float>(rectangle[3]);
    const std::array<ColorVertex, 6> vertices {{
        {left, top, color}, {right, top, color}, {right, bottom, color},
        {left, top, color}, {right, bottom, color}, {left, bottom, color}
    }};
    return RenderColorTriangles(vertices, rectangle);
}

} // namespace cg::render
