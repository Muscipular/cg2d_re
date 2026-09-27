#include "LegacyTextureDrawing.hpp"

#include "GraphicDxCacheState.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>

namespace cg::render {
namespace {

constexpr std::uint8_t kFlipX = 1;
constexpr std::uint8_t kFlipY = 2;
constexpr std::uint8_t kAdditive = 8;
constexpr float kTextureBias = std::bit_cast<float>(0x3D4CCCCDu);

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
