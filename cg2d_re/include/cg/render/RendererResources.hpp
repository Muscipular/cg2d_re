#pragma once

#include "cg/Palette.hpp"

#include <cstdint>

namespace cg::render {

// 原 gCRenderer@0x627360 的 x64 不透明资源 owner；由 Init 发布、Destroy 最后释放。
struct RendererResources;
struct PaletteBinding;
extern RendererResources* gRenderer;

// 原 [44B630,44C3B7)：初始化父入口；调用者按低 S8 区分 1/2，其余首次值继续。
// 尚待完整实现 SDL/CPU 资源与原模式分流，不提供固定返回。
std::int8_t RendererInit_44B630();

// 原 44BB1E–44BBB7：Back 像素格式发布的共享 W32 槽。
extern std::uint32_t gRendererRedMask;   // 0x628FE4。
extern std::uint32_t gRendererGreenMask; // 0x62A660。
extern std::uint32_t gRendererBlueMask;  // 0x628FC0。
extern std::int32_t gRendererRedShift;   // 0x6287A8：8 - 红掩码位数。
extern std::int32_t gRendererGreenShift; // 0x62A658：8 - 绿掩码位数。
extern std::int32_t gRendererBlueShift;  // 0x628BB8：8 - 蓝掩码位数。
extern std::int32_t gRendererRedOffset;  // 0x629314：蓝位数 + 绿位数。
extern std::int32_t gRendererGreenOffset; // 0x629718：蓝位数。
extern std::int32_t gRendererBlueOffset;  // 0x62971C：原 ESI 零值。

// SDL XRGB8888 Back 已建且 32 位格式判定成功后，由 Init 按原位调用。
void PublishRendererPixelFormat32_44BB1E() noexcept;

// 原 Init 的 0xC4 字节零初始化分配在新端仅承载已还原字段。
[[nodiscard]] bool AllocateRendererResources() noexcept;
void ReleaseRendererResources() noexcept;

// 原 InitPalette 的 +0x90 CreatePalette；32 位 CPU 路径仍保留独立创建失败。
[[nodiscard]] bool CreateRendererPaletteBinding(const PaletteEntry* entries) noexcept;
// 原 Destroy@44C4A4–44C4BF：在 Back 和 renderer owner 之前释放并清绑定。
void ReleaseRendererPaletteBinding() noexcept;
PaletteBinding* GetRendererPalette(RendererResources* renderer) noexcept;
std::int32_t SetPaletteEntries(
    PaletteBinding* palette, std::uint32_t flags, std::uint32_t first,
    std::uint32_t count, const PaletteEntry* entries) noexcept;

} // namespace cg::render
