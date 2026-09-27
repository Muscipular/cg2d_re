#pragma once

#include "cg/Palette.hpp"

#include <cstdint>

namespace cg::render {

// 原 gCRenderer@0x627360 的 x64 不透明资源 owner；由 Init 发布、Destroy 最后释放。
struct RendererResources;
struct PaletteBinding;
extern RendererResources* gRenderer;

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
