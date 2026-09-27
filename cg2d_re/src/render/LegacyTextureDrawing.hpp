#pragma once

#include <array>
#include <cstdint>

namespace cg::render {

struct TextureSurface;

// 原 RECT 的四项 S32，顺序为 left/top/right/bottom；缓存使用同一数值顺序。
using TextureRectangle = std::array<std::int32_t, 4>;

// 原 renderer +B8/+BA/+BC 的共享值；查询须借用 RendererResources 的唯一存储。
struct LegacyTextureState {
    std::int16_t width;
    std::int16_t height;
    std::int32_t colorKeyEnabled;
};

// 原 FVF=0x144 顶点；四点按左上、右上、右下、左下提交。
struct LegacyTextureVertex {
    float x;
    float y;
    float z;
    float rhw;
    std::uint32_t color;
    float u;
    float v;
};

// 原纹理入口实际使用的 blend 数值。
enum class LegacyTextureBlend : std::uint32_t {
    Zero = 1,
    One = 2,
    SourceAlpha = 5,
    InverseSourceAlpha = 6
};

// 底层真实依赖尚待实现；不以固定状态或空提交补链接。
const LegacyTextureState& GetLegacyTextureState();
// 顺序为 source blend、destination blend、按共享 stage 绑面、绘制。
// 前三段失败不成为额外门禁；同步借用顶点并透传最后的完整 W32 状态。
std::int32_t SubmitLegacyTextureFan(
    TextureSurface* surface, const std::array<LegacyTextureVertex, 4>& vertices,
    LegacyTextureBlend sourceBlend, LegacyTextureBlend destinationBlend);
// 源色键写入唯一 Back，不做颜色调制；目标尺寸不同则执行原 Blt 缩放。
std::int32_t BlitTextureToBack(
    TextureSurface* surface, const TextureRectangle& source,
    const TextureRectangle& destination);

// 原 44D520：整数裁剪、可选四顶点或变换缓存路径；rectangle 为值副本。
std::int32_t RenderTextureToSurface_44D520(
    std::int16_t x, std::int16_t y, TextureRectangle rectangle,
    TextureSurface* surface, std::int32_t imageNo, std::int16_t paletteIndex,
    std::uint8_t mixMode, std::uint32_t color);

// 原 44DA90：F32 局部/F64 中间计算及 S64 截断；不回写输入矩形。
std::int32_t RenderTexture_44DA90(
    std::int16_t x, std::int16_t y, const TextureRectangle* rectangle,
    float scaleX, float scaleY, TextureSurface* surface,
    std::int32_t imageNo, std::int16_t paletteIndex,
    std::uint8_t mixMode, std::uint32_t color);

} // namespace cg::render
