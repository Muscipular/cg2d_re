#include "LegacyTextureDrawing.hpp"

#include "GraphicDxCacheState.hpp"
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
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> row {
        nullptr, SDL_DestroySurface
    };
    std::array<TextureSurfaceBinding, 8> textures;
    LegacyTextureBlend sourceBlend {LegacyTextureBlend::One};
    LegacyTextureBlend destinationBlend {LegacyTextureBlend::Zero};
};

namespace {

constexpr std::uint8_t kFlipX = 1;
constexpr std::uint8_t kFlipY = 2;
constexpr std::uint8_t kAdditive = 8;
constexpr float kTextureBias = std::bit_cast<float>(0x3D4CCCCDu);
constexpr auto kNativeFailure = std::bit_cast<std::int32_t>(0x80004005u);
constexpr auto kSurfaceBusy = std::bit_cast<std::int32_t>(0x887601AEu);

// POINT 后做 WRAP；先取 texel，避免微小负 UV 的小数部分舍入成 1。
std::int32_t SampleWrappedCoordinate(const double uv, const std::int32_t extent)
{
    const auto texel = static_cast<std::int64_t>(std::floor(uv * extent));
    const auto wrapped = texel % extent;
    return static_cast<std::int32_t>(wrapped < 0 ? wrapped + extent : wrapped);
}

std::int32_t DrawTextureRectangle(
    LegacyTextureDevice& device, const std::array<LegacyTextureVertex, 4>& vertices)
{
    SDL_Surface* const input = GetTextureSdlSurface(device.textures[0].Get());
    SDL_Surface* const back = device.back;
    SDL_Surface* const row = device.row.get();
    if (((back->flags | (input == nullptr ? 0u : input->flags)) & SDL_SURFACE_LOCKED) != 0) {
        return kSurfaceBusy;
    }

    SDL_BlendMode blendMode;
    if (device.sourceBlend == LegacyTextureBlend::One
        && device.destinationBlend == LegacyTextureBlend::Zero) {
        blendMode = SDL_BLENDMODE_NONE;
    } else if (device.sourceBlend == LegacyTextureBlend::SourceAlpha
        && device.destinationBlend == LegacyTextureBlend::InverseSourceAlpha) {
        blendMode = SDL_BLENDMODE_BLEND;
    } else if (device.sourceBlend == LegacyTextureBlend::One
        && device.destinationBlend == LegacyTextureBlend::One) {
        blendMode = SDL_BLENDMODE_ADD_PREMULTIPLIED;
    } else {
        return kNativeFailure;
    }

    const bool colorKeyEnabled = input != nullptr && GetLegacyTextureState().colorKeyEnabled != 0;
    std::uint32_t colorKey = 0;
    if (colorKeyEnabled && !SDL_GetSurfaceColorKey(input, &colorKey)) {
        return kNativeFailure;
    }
    const std::uint32_t color = vertices[0].color;
    if (!SDL_SetSurfaceColorKey(row, colorKeyEnabled, colorKey)
        || !SDL_SetSurfaceColorMod(row,
            static_cast<std::uint8_t>(color >> 16),
            static_cast<std::uint8_t>(color >> 8),
            static_cast<std::uint8_t>(color))
        || !SDL_SetSurfaceAlphaMod(row, static_cast<std::uint8_t>(color >> 24))
        || !SDL_SetSurfaceBlendMode(row, blendMode)) {
        return kNativeFailure;
    }

    const auto left = std::max(0, static_cast<std::int32_t>(
        std::ceil(std::min(vertices[0].x, vertices[1].x))));
    const auto right = std::min(kCpuSurfaceWidth, static_cast<std::int32_t>(
        std::ceil(std::max(vertices[0].x, vertices[1].x))));
    const auto top = std::max(0, static_cast<std::int32_t>(
        std::ceil(std::min(vertices[0].y, vertices[3].y))));
    const auto bottom = std::min(kCpuSurfaceHeight, static_cast<std::int32_t>(
        std::ceil(std::max(vertices[0].y, vertices[3].y))));
    if (left >= right || top >= bottom) {
        return 0;
    }

    const double width = static_cast<double>(vertices[1].x) - vertices[0].x;
    const double height = static_cast<double>(vertices[3].y) - vertices[0].y;
    const double deltaU = static_cast<double>(vertices[1].u) - vertices[0].u;
    const double deltaV = static_cast<double>(vertices[3].v) - vertices[0].v;
    const SDL_Rect sourceRect {0, 0, right - left, 1};
    auto* const pixels = static_cast<std::uint32_t*>(row->pixels);
    for (std::int32_t y = top; y < bottom; ++y) {
        if (input != nullptr) {
            const double v = vertices[0].v
                + deltaV * ((static_cast<double>(y) - vertices[0].y) / height);
            const std::int32_t sourceY = SampleWrappedCoordinate(v, input->h);
            const auto* const sourceRow = reinterpret_cast<const std::uint32_t*>(
                static_cast<const std::uint8_t*>(input->pixels) + sourceY * input->pitch);
            for (std::int32_t x = left; x < right; ++x) {
                const double u = vertices[0].u
                    + deltaU * ((static_cast<double>(x) - vertices[0].x) / width);
                pixels[x - left] = sourceRow[SampleWrappedCoordinate(u, input->w)];
            }
        } else {
            // 无纹理时禁用纹理阶段，SDL 调制白色后得到完整 diffuse 色。
            std::fill_n(pixels, sourceRect.w, 0x00FFFFFFu);
        }
        const SDL_Rect destinationRect {left, y, sourceRect.w, 1};
        if (!SDL_BlitSurfaceUnchecked(row, &sourceRect, back, &destinationRect)) {
            return kNativeFailure;
        }
    }
    return 0;
}

// 原 __ftol 在 PC53 下先转 S64，caller 取低 W32；沿用既有局部承接。
std::int32_t TruncateTextureCoordinate(const double value)
{
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value));
}

// 两入口相同的四顶点、混合选择及 native 提交段。
std::int32_t SubmitTexturedRectangle(
    TextureSurface* surface, const TextureRectangle& destination,
    const float u0, const float v0, const float u1, const float v1,
    const std::uint8_t mixMode, const std::uint32_t color)
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
    if ((mixMode & kAdditive) == 0) {
        if ((color & 0xFF000000u) == 0xFF000000u) {
            destinationBlend = LegacyTextureBlend::Zero;
        } else {
            sourceBlend = LegacyTextureBlend::SourceAlpha;
            destinationBlend = LegacyTextureBlend::InverseSourceAlpha;
        }
    }
    return SubmitLegacyTextureFan(surface, vertices, sourceBlend, destinationBlend);
}

} // namespace

LegacyTextureDevice* CreateLegacyTextureDevice(SDL_Surface* const back) noexcept
{
    if (back == nullptr) {
        return nullptr;
    }
    auto* const device = new (std::nothrow) LegacyTextureDevice {};
    if (device == nullptr) {
        return nullptr;
    }
    device->back = back;
    device->row.reset(SDL_CreateSurface(kCpuSurfaceWidth, 1, SDL_PIXELFORMAT_XRGB8888));
    if (device->row == nullptr) {
        delete device;
        return nullptr;
    }
    return device;
}

void DestroyLegacyTextureDevice(LegacyTextureDevice* const device) noexcept
{
    delete device;
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

std::int32_t RenderTextureToSurface_44D520(
    const std::int16_t x, const std::int16_t y, TextureRectangle rectangle,
    TextureSurface* surface, const std::int32_t imageNo, const std::int16_t paletteIndex,
    const std::uint8_t mixMode, const std::uint32_t color)
{
    auto& left = rectangle[0];
    auto& top = rectangle[1];
    auto& right = rectangle[2];
    auto& bottom = rectangle[3];
    if (x >= kCpuSurfaceWidth || x - left + right <= 0
        || y >= kCpuSurfaceHeight || y - top + bottom <= 0) {
        return 0;
    }

    std::int32_t destinationX = x;
    const bool flipX = (mixMode & kFlipX) != 0;
    if (flipX) {
        if (x < 0) {
            right += x;
            destinationX = 0;
        }
        if (destinationX - left + right > kCpuSurfaceWidth) {
            left = destinationX - kCpuSurfaceWidth + right;
        }
    } else {
        if (x < 0) {
            left -= x;
            destinationX = 0;
        }
        if (destinationX - left + right > kCpuSurfaceWidth) {
            right = kCpuSurfaceWidth - destinationX + left;
        }
    }

    std::int32_t destinationY = y;
    const bool flipY = (mixMode & kFlipY) != 0;
    if (flipY) {
        if (y < 0) {
            bottom += y;
            destinationY = 0;
        }
        if (destinationY - top + bottom > kCpuSurfaceHeight) {
            top = destinationY - kCpuSurfaceHeight + bottom;
        }
    } else {
        if (y < 0) {
            top -= y;
            destinationY = 0;
        }
        if (destinationY - top + bottom > kCpuSurfaceHeight) {
            bottom = kCpuSurfaceHeight - destinationY + top;
        }
    }

    if (HasLegacy3DDevice_44B610() != 0 && GetLegacyTextureState().colorKeyEnabled != 0) {
        const auto textureWidth = static_cast<float>(GetLegacyTextureState().width);
        const double u0 =
            (static_cast<double>(flipX ? right - 1 : left) + kTextureBias) / textureWidth;
        const double u1 = static_cast<double>(flipX ? left - 1 : right) / textureWidth;
        const double textureHeight = GetLegacyTextureState().height;
        const auto v0 = static_cast<float>(
            (static_cast<double>(flipY ? bottom - 1 : top) + kTextureBias) / textureHeight);
        const double v1 = static_cast<double>(flipY ? top - 1 : bottom) / textureHeight;
        const TextureRectangle destination {
            destinationX, destinationY,
            destinationX + (right - left), destinationY + (bottom - top)
        };
        return SubmitTexturedRectangle(surface, destination,
            static_cast<float>(u0), v0, static_cast<float>(u1), static_cast<float>(v1),
            mixMode, color);
    }
    if ((mixMode & kAdditive) != 0) {
        return 0;
    }
    const TextureRectangle destination {
        destinationX, destinationY,
        destinationX - left + right, destinationY - top + bottom
    };
    if (flipX || flipY) {
        const auto& key = world::gCurrentSceneType == 10 ? destination : rectangle;
        surface = TransformTextureSurface_44CD80(
            surface, imageNo, paletteIndex,
            static_cast<std::uint8_t>(flipX), static_cast<std::uint8_t>(flipY),
            rectangle.data(), key.data());
    }
    return BlitTextureToBack(surface, rectangle, destination);
}

std::int32_t RenderTexture_44DA90(
    const std::int16_t x, const std::int16_t y, const TextureRectangle* rectangle,
    const float scaleX, const float scaleY, TextureSurface* surface,
    const std::int32_t imageNo, const std::int16_t paletteIndex,
    const std::uint8_t mixMode, const std::uint32_t color)
{
    float top = static_cast<float>(static_cast<double>((*rectangle)[1]) * scaleY);
    float left = static_cast<float>(static_cast<double>((*rectangle)[0]) * scaleX);
    float right = static_cast<float>(static_cast<double>((*rectangle)[2]) * scaleX);
    float bottom = static_cast<float>(static_cast<double>((*rectangle)[3]) * scaleY);
    const auto originalX = static_cast<float>(x);
    float destinationX = originalX;
    const auto originalY = static_cast<float>(y);
    float destinationY = originalY;
    if (x >= kCpuSurfaceWidth) {
        return 0;
    }
    if (!(static_cast<double>(right) - left + originalX > 0.0)) {
        return 0;
    }
    if (y >= kCpuSurfaceHeight
        || !(static_cast<double>(bottom) - top + originalY > 0.0)) {
        return 0;
    }

    const bool flipX = (mixMode & kFlipX) != 0;
    if (flipX) {
        if (destinationX < 0.0f) {
            right = static_cast<float>(static_cast<double>(destinationX) + right);
            destinationX = 0.0f;
        }
        const double extent = static_cast<double>(right) - left + destinationX;
        if (extent > kCpuSurfaceWidth) {
            left = static_cast<float>((extent - kCpuSurfaceWidth) + left);
        }
    } else {
        if (destinationX < 0.0f) {
            left = static_cast<float>(static_cast<double>(left) - originalX);
            destinationX = 0.0f;
        }
        const double extent = static_cast<double>(right) - left + destinationX;
        if (extent > kCpuSurfaceWidth) {
            right = static_cast<float>(right - (extent - kCpuSurfaceWidth));
        }
    }
    const bool flipY = (mixMode & kFlipY) != 0;
    if (flipY) {
        if (destinationY < 0.0f) {
            bottom = static_cast<float>(static_cast<double>(destinationY) + bottom);
            destinationY = 0.0f;
        }
        const double extent = static_cast<double>(bottom) - top + destinationY;
        if (extent > kCpuSurfaceHeight) {
            // 原 44DC61 的 fsubr；这里确为减去超出量，不改成横向的加法。
            top = static_cast<float>(top - (extent - kCpuSurfaceHeight));
        }
    } else {
        if (destinationY < 0.0f) {
            top = static_cast<float>(static_cast<double>(top) - destinationY);
            destinationY = 0.0f;
        }
        const double extent = static_cast<double>(bottom) - top + destinationY;
        if (extent > kCpuSurfaceHeight) {
            bottom = static_cast<float>(bottom - (extent - kCpuSurfaceHeight));
        }
    }

    const std::int32_t sourceBottom = TruncateTextureCoordinate(static_cast<double>(bottom) / scaleY);
    const std::int32_t sourceRight = TruncateTextureCoordinate(static_cast<double>(right) / scaleX);
    const std::int32_t sourceTop = TruncateTextureCoordinate(static_cast<double>(top) / scaleY);
    const std::int32_t sourceLeft = TruncateTextureCoordinate(static_cast<double>(left) / scaleX);
    const TextureRectangle source {sourceLeft, sourceTop, sourceRight, sourceBottom};
    double expandedRight = right;
    if (kCpuSurfaceWidth > static_cast<double>(destinationX) + right - left) {
        expandedRight += 1.0;
    }
    double expandedBottom = bottom;
    if (kCpuSurfaceHeight > static_cast<double>(destinationY) + bottom - top) {
        expandedBottom += 1.0;
    }
    const std::int32_t destinationBottom =
        TruncateTextureCoordinate(static_cast<double>(destinationY) + expandedBottom - top);
    const std::int32_t destinationRight =
        TruncateTextureCoordinate(static_cast<double>(destinationX) + expandedRight - left);
    const std::int32_t destinationTop = TruncateTextureCoordinate(destinationY);
    const std::int32_t destinationLeft = TruncateTextureCoordinate(destinationX);
    const TextureRectangle destination {
        destinationLeft, destinationTop, destinationRight, destinationBottom
    };

    if (HasLegacy3DDevice_44B610() != 0 && GetLegacyTextureState().colorKeyEnabled != 0) {
        const auto textureWidth = static_cast<float>(GetLegacyTextureState().width);
        const double u0 =
            (static_cast<double>(flipX ? sourceRight : sourceLeft) + kTextureBias) / textureWidth;
        const double u1 = static_cast<double>(flipX ? sourceLeft : sourceRight) / textureWidth;
        const double textureHeight = GetLegacyTextureState().height;
        const auto v0 = static_cast<float>(
            (static_cast<double>(flipY ? sourceBottom : sourceTop) + kTextureBias) / textureHeight);
        const double v1 = static_cast<double>(flipY ? sourceTop : sourceBottom) / textureHeight;
        return SubmitTexturedRectangle(surface, destination,
            static_cast<float>(u0), v0, static_cast<float>(u1), static_cast<float>(v1),
            mixMode, color);
    }
    if ((mixMode & kAdditive) != 0) {
        return 0;
    }
    if (flipX || flipY) {
        const auto& key = world::gCurrentSceneType == 10 ? destination : source;
        surface = TransformTextureSurface_44CD80(
            surface, imageNo, paletteIndex,
            static_cast<std::uint8_t>(flipX), static_cast<std::uint8_t>(flipY),
            source.data(), key.data());
    }
    return BlitTextureToBack(surface, source, destination);
}

} // namespace cg::render
