#pragma once

#include <array>
#include <cstdint>

namespace cg::render {

// L07：MapEx拥有的可更新像素图；绘制队列独立持有当帧纹理引用。
struct PixelImage;
PixelImage* CreatePixelImage(std::int32_t width, std::int32_t height);
bool UpdatePixelImage(PixelImage* image, const std::uint32_t* pixels, std::int32_t width, std::int32_t height);
void ReleasePixelImage(PixelImage* image);

// R04：源区域及目标裁剪统一为 left/top/right/bottom；不暴露 SDK 对象。
using PngRectangle = std::array<std::int32_t, 4>;
std::int32_t DrawPixelImage(PixelImage* image,
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::uint32_t color, std::uint8_t layer, const PngRectangle* clipRectangle = nullptr);

// 以宽字符文件路径加载 PNG，进入现有层序队列；空源区域表示完整图片。
// 失败 -1，队列失败透传原返回，成功返回队列索引；color 为 0xAARRGGBB。
std::int32_t DrawPngImage(
    const wchar_t* path, const PngRectangle* sourceRectangle,
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::uint32_t color, std::uint8_t layer, const PngRectangle* clipRectangle = nullptr);

} // namespace cg::render
