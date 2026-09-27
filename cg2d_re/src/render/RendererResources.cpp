#include "cg/render/RendererResources.hpp"

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
};

RendererResources* gRenderer = nullptr;

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
