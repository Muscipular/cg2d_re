#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::render {

// 原 gBackgroundColor@0x62B0A8；固定 32bpp 路径使用 0x00RRGGBB 位模式。
extern std::uint32_t gBackgroundColor;

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

extern CpuSurfacePixels gCpuBackSurface;
extern CpuSurfacePixels gCpuTempSurface;

// 将 Temp 的半开源矩形同步复制到 Back，不混合、不缩放、不使用色键。
// 调用方必须先裁剪到固定 Back/Temp 边界；非法或空请求不复制并返回 false。
bool BlitTempToBack(
    std::int32_t destinationX,
    std::int32_t destinationY,
    std::int32_t sourceLeft,
    std::int32_t sourceTop,
    std::int32_t sourceRight,
    std::int32_t sourceBottom) noexcept;

} // namespace cg::render
