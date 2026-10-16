#pragma once

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"

#include <span>

namespace cg::render {

// WIC 第一帧矩形解码；输出 BGRA 字节，即小端 0xAARRGGBB，成功返回字节数。
std::size_t ReadPngRectangle(
    std::span<const std::uint8_t> data, std::int32_t x, std::int32_t y,
    std::int32_t width, std::int32_t height, std::span<std::uint32_t> pixels);

// 队列只借用这一帧的批次；纹理引用独立于可淘汰的文件缓存条目。
struct PngImageBatch {
    TextureSurfaceBinding surface;
    TextureRectangle source;
    TextureRectangle destination;
    std::uint32_t color;
};

void DrawPngImageBatch(const PngImageBatch& batch);
void CleanupPngImageCache();
void ReleasePngImageResources() noexcept;

} // namespace cg::render
