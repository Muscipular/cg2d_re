#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace cg::render {

// 原 gBackgroundColor@0x62B0A8；固定 32bpp 路径使用 0x00RRGGBB 位模式。
extern std::uint32_t gBackgroundColor;
extern std::uint32_t gValueFF48E4; // 原 0xFF48E4；51D750 写入的完整 W32 随机状态。

// Renderer::Init@0x44B75A/0x44B76A 固定写入的逻辑尺寸。
inline constexpr std::int32_t kCpuSurfaceWidth = 640;
inline constexpr std::int32_t kCpuSurfaceHeight = 480;
inline constexpr std::size_t kCpuSurfacePitchPixels = 640;
inline constexpr std::size_t kCpuSurfacePitchBytes = 2560;
inline constexpr std::size_t kCpuSurfacePixelCount =
    kCpuSurfacePitchPixels * static_cast<std::size_t>(kCpuSurfaceHeight);
static_assert(
    kCpuSurfacePitchPixels == static_cast<std::size_t>(kCpuSurfaceWidth));
static_assert(
    kCpuSurfacePitchBytes
    == kCpuSurfacePitchPixels * sizeof(std::uint32_t));

// 32 位 0x00RRGGBB、自上而下、行主序；固定 pitch 为 640 像素/2560 字节。
using CpuSurfacePixels = std::array<std::uint32_t, kCpuSurfacePixelCount>;

// 原 renderer 初始化与缓存构建的底层资源阶段；不代替父函数的其他副作用。
bool CreateCpuBackSurface() noexcept;
bool CreateCpuAutoMapSurface() noexcept;
bool CreateCpuTempSurface() noexcept;
// 原 523228 的独立回退：使用 MakeSurface，实际尺寸可能大于 640x480。
bool CreateCpuTempSurfaceFallback() noexcept;
bool BuildCpuSurfaceBuffers() noexcept;

// 原 44C436–44C45B 与 44C4E5–44C4F8 的两个销毁阶段；
// 父函数在二者之间还须释放设备、调色板等资源。
void DestroyCpuCacheSurfaces() noexcept;
void DestroyCpuBackSurface() noexcept;

// 非拥有视图只在 Back 生命周期内有效；未创建时返回空视图。
std::span<const std::uint32_t> ReadCpuBackPixels() noexcept;

// 原 4512C0：已创建的 Back 按当次完整 W32 背景色填充；无 Back 返回非零。
std::int32_t ClearSurface_4512C0() noexcept;

// 原 Back→Temp 的 BltFast：只复制左上 640x480，不拉伸或使用色键。
// 源或目标尚未创建时不复制。
bool CopyBackToTemp() noexcept;

// 将 Temp 的半开源矩形同步复制到 Back，不混合、不缩放、不使用色键。
// 调用方必须先裁剪到逻辑 640x480 边界；内部采用 Temp 的实际行距。
// 非法或空请求不复制并返回 false。
bool BlitTempToBack(
    std::int32_t destinationX,
    std::int32_t destinationY,
    std::int32_t sourceLeft,
    std::int32_t sourceTop,
    std::int32_t sourceRight,
    std::int32_t sourceBottom) noexcept;

} // namespace cg::render
