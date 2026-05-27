#pragma once

#include <cstdint>

namespace cg::render {

// PixelFormat 表示渲染层当前支持的纹理像素格式。
enum class PixelFormat {
    Unknown,
    Rgba8,
    Bgra8,
};

// TextureScaleMode 控制纹理放大/缩小时的采样方式。
enum class TextureScaleMode {
    Nearest,
    Linear,
};

// TextureDesc 描述需要创建的纹理资源。
struct TextureDesc {
    int width = 0;
    int height = 0;
    PixelFormat format = PixelFormat::Rgba8;
    TextureScaleMode scaleMode = TextureScaleMode::Nearest;
};

// TextureHandle 是渲染层纹理资源的稳定句柄，不暴露后端对象。
struct TextureHandle {
    std::uint32_t id = 0;
    std::uint32_t generation = 0;

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return id != 0;
    }

    [[nodiscard]] constexpr bool operator==(const TextureHandle&) const noexcept = default;
};

} // namespace cg::render
