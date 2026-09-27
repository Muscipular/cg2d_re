#include "cg/render/RendererResources.hpp"

#include "LegacyTextureDrawing.hpp"

#include <algorithm>
#include <memory>
#include <new>
#include <utility>

namespace cg::render {

struct PaletteBinding {
    PaletteEntry entries[256] {};
};

struct RendererResources {
    std::unique_ptr<PaletteBinding> palette;
    // 原纹理/像素格式字段；GPTR 零初值，后续由 RendererInit 在原时点发布。
    LegacyTextureState textureState {};
};

RendererResources* gRenderer = nullptr;
std::uint32_t gRendererRedMask = 0;
std::uint32_t gRendererGreenMask = 0;
std::uint32_t gRendererBlueMask = 0;
std::int32_t gRendererRedShift = 0;
std::int32_t gRendererGreenShift = 0;
std::int32_t gRendererBlueShift = 0;
std::int32_t gRendererRedOffset = 0;
std::int32_t gRendererGreenOffset = 0;
std::int32_t gRendererBlueOffset = 0;

const LegacyTextureState& GetLegacyTextureState()
{
    return gRenderer->textureState;
}

void PublishRendererPixelFormat32_44BB1E() noexcept
{
    gRenderer->textureState.pixelBitCount = 32;
    gRendererRedMask = 0x00FF0000u;
    gRendererGreenMask = 0x0000FF00u;
    gRendererBlueMask = 0x000000FFu;
    gRendererRedShift = 8 - CountMaskBits_453DE0(gRendererRedMask);
    gRendererGreenShift = 8 - CountMaskBits_453DE0(gRendererGreenMask);
    gRendererBlueShift = 8 - CountMaskBits_453DE0(gRendererBlueMask);
    gRendererRedOffset = CountMaskBits_453DE0(gRendererBlueMask)
        + CountMaskBits_453DE0(gRendererGreenMask);
    gRendererGreenOffset = CountMaskBits_453DE0(gRendererBlueMask);
    gRendererBlueOffset = 0;
}

bool AllocateRendererResources() noexcept
{
    if (gRenderer != nullptr) {
        return false;
    }
    gRenderer = new (std::nothrow) RendererResources;
    return gRenderer != nullptr;
}

void ReleaseRendererResources() noexcept
{
    delete gRenderer;
    gRenderer = nullptr;
}

bool CreateRendererPaletteBinding(const PaletteEntry* entries) noexcept
{
    if (gRenderer == nullptr || entries == nullptr) {
        return false;
    }
    std::unique_ptr<PaletteBinding> pending(new (std::nothrow) PaletteBinding);
    if (!pending) {
        return false;
    }
    std::copy_n(entries, 256, pending->entries);
    gRenderer->palette = std::move(pending);
    return true;
}

void ReleaseRendererPaletteBinding() noexcept
{
    if (gRenderer != nullptr) {
        gRenderer->palette.reset();
    }
}

PaletteBinding* GetRendererPalette(RendererResources* renderer) noexcept
{
    return renderer == nullptr ? nullptr : renderer->palette.get();
}

std::int32_t SetPaletteEntries(
    PaletteBinding* palette, std::uint32_t flags, std::uint32_t first,
    std::uint32_t count, const PaletteEntry* entries) noexcept
{
    if (palette == nullptr || entries == nullptr || flags != 0
        || first > 256 || count > 256 - first) {
        return -1;
    }
    std::copy_n(entries, count, palette->entries + first);
    return 0;
}

} // namespace cg::render
