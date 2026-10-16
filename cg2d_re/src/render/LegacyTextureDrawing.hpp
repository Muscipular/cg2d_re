#pragma once

#include <array>
#include <cstdint>

struct SDL_Surface;
struct SDL_PixelFormatDetails;

namespace cg::render {

struct TextureSurface;
struct LegacyTextureDevice;

// 原 RECT 的四项 S32，顺序为 left/top/right/bottom；缓存使用同一数值顺序。
using TextureRectangle = std::array<std::int32_t, 4>;

// R03/R04：完整图像使用自身实际纹理尺寸计算UV，不借用节点池的64×48尺寸。
std::int32_t RenderImageTexture(
    TextureSurface* surface, const TextureRectangle& source,
    const TextureRectangle& destination, std::uint8_t mixFlags, std::uint32_t color);

// 原 renderer 的共享值；查询须借用 RendererResources 的唯一存储。
struct LegacyTextureState {
    std::int16_t width;            // +B8：纹理采样分母，不是请求块宽。
    std::int16_t height;           // +BA。
    std::int32_t colorKeyEnabled;  // +BC。
    std::uint32_t textureCaps;     // +B4：bit1 幂次方、bit5 正方形限制。
    std::uint32_t pixelBitCount;   // +68：native 格式查询结果。
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

// 原三个矩形入口实际使用的 blend 数值。
enum class LegacyTextureBlend : std::uint32_t {
    Zero = 1,
    One = 2,
    SourceAlpha = 5,
    InverseSourceAlpha = 6
};

// 只读借用 RendererResources 的唯一成员，不补空 owner 或默认能力。
const LegacyTextureState& GetLegacyTextureState();
// 原 dwStage@62A648；由 Init 探测后发布，销毁不重置。
extern std::uint32_t gLegacyTextureStage;

// 私有 native 设备拥有单行 scratch 和绑定引用，借用 Back；不能替代 +9C 接口。
LegacyTextureDevice* CreateLegacyTextureDevice(
    const SDL_PixelFormatDetails* format, SDL_Surface* back) noexcept;
void DestroyLegacyTextureDevice(LegacyTextureDevice* device) noexcept;
// 查询真实 scratch 格式；配置对应 Init 的色键、固定管线和 viewport 阶段。
const SDL_PixelFormatDetails* GetLegacyTextureDeviceFormat(LegacyTextureDevice* device) noexcept;
[[nodiscard]] bool EnableLegacyTextureColorKey(LegacyTextureDevice* device) noexcept;
[[nodiscard]] bool ConfigureLegacyTexturePipeline(LegacyTextureDevice* device) noexcept;
// RendererResources 持有同一设备；创建成功才发布，销毁必须先于 Back。
[[nodiscard]] bool CreateRendererTextureDevice() noexcept;
void ReleaseRendererTextureDevice() noexcept;
// 独立 +9C 接口槽，设备释放之后、SDL presenter 之前释放。
void ReleaseRendererTextureInterface() noexcept;
LegacyTextureDevice* GetRendererTextureDevice() noexcept;
// DrawAllQueued 的 native 场景边界；失败不改变入场状态，CPU 提交同步完成。
std::int32_t BeginLegacyTextureScene(LegacyTextureDevice* device) noexcept;
std::int32_t EndLegacyTextureScene(LegacyTextureDevice* device) noexcept;
// 原 native Set/GetTexture 边界；查询成功返回的一份引用须 ReleaseTextureSurface。
std::int32_t BindLegacyTexture(
    LegacyTextureDevice* device, std::uint32_t stage, TextureSurface* surface) noexcept;
std::int32_t QueryLegacyTexture(
    LegacyTextureDevice* device, std::uint32_t stage, TextureSurface** surface) noexcept;

// 三个入口共用矩形四顶点提交；F32 UV 保留到 POINT/WRAP 采样，再由 SDL 混合写 Back。
// 顺序为 source blend、destination blend、按共享 stage 绑面、绘制。
// 前三段失败不成为额外门禁；同步借用顶点并透传最后的完整 W32 状态。
std::int32_t SubmitLegacyTextureFan(
    TextureSurface* surface, const std::array<LegacyTextureVertex, 4>& vertices,
    LegacyTextureBlend sourceBlend, LegacyTextureBlend destinationBlend);
// 仅在 render 后端同步借用；Back 销毁/重建后失效，公开接口不暴露 SDL 类型。
SDL_Surface* GetCpuBackSdlSurface() noexcept;
// 原 case 6 的无色键 BltFast；保留 caller 原矩形，不附加波纹 helper 的范围门禁。
bool BlitTempRectangleToBack(
    std::int32_t destinationX, std::int32_t destinationY,
    const TextureRectangle& source) noexcept;
// 源色键写入唯一 Back，不做颜色调制；目标尺寸不同则执行原 Blt 缩放。
std::int32_t BlitTextureToBack(
    TextureSurface* surface, const TextureRectangle& source,
    const TextureRectangle& destination);

// 原 44C9A0：两次接口检查、S16 裁剪及空纹理四顶点；透传完整返回位型。
std::uint32_t FillRect_44C9A0(
    std::int16_t left, std::int16_t right, std::int16_t top, std::int16_t bottom,
    std::uint32_t color, std::uint8_t mixFlags);

// 原 44D520：完整 S32 坐标实参，交集/位移取低 S16，负值门禁保原宽度；矩形为值副本。
std::int32_t RenderTextureToSurface_44D520(
    std::int32_t x, std::int32_t y, TextureRectangle rectangle,
    TextureSurface* surface, std::int32_t imageNo, std::int16_t paletteIndex,
    std::uint8_t mixFlags, std::uint32_t color);

// 原 44DA90：F32 局部/F64 中间计算及 S64 截断；不回写输入矩形。
std::int32_t RenderTexture_44DA90(
    std::int16_t x, std::int16_t y, const TextureRectangle* rectangle,
    float scaleX, float scaleY, TextureSurface* surface,
    std::int32_t imageNo, std::int16_t paletteIndex,
    std::uint8_t mixFlags, std::uint32_t color);

} // namespace cg::render
