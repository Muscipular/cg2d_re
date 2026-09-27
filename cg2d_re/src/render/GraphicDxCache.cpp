#include "GraphicDxCacheState.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/render/SurfaceState.hpp"

#include <memory>
#include <new>

namespace cg::render {
struct TextureSurface {
    std::int16_t width {};
    std::int16_t height {};
    std::uint32_t colorKey {};
    std::unique_ptr<std::uint32_t[]> pixels;
    TextureSurface* nextOwned {};
};

namespace {

class TextureSurfaceOwner final {
public:
    ~TextureSurfaceOwner()
    {
        while (head_ != nullptr) {
            TextureSurface* next = head_->nextOwned;
            delete head_;
            head_ = next;
        }
    }

    TextureSurface* Create(std::int16_t width, std::int16_t height) noexcept
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        auto* surface = new (std::nothrow) TextureSurface {};
        if (surface == nullptr) {
            return nullptr;
        }
        const std::size_t pixelCount =
            static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        surface->pixels.reset(new (std::nothrow) std::uint32_t[pixelCount] {});
        if (surface->pixels == nullptr) {
            delete surface;
            return nullptr;
        }
        surface->width = width;
        surface->height = height;
        surface->nextOwned = head_;
        head_ = surface;
        return surface;
    }

private:
    TextureSurface* head_ {};
};

TextureSurfaceOwner gTextureSurfaceOwner;

} // namespace

DxGraphicEntry* gGraphicDxCache103CFA4 = nullptr;
std::array<TextureNode, kTextureNodeCapacity> gTextureNodes {};
std::int32_t gTextureNodeCount = 0;
std::int32_t gTextureScanIndex = 0;
std::int32_t gTexturePreferredCount = 0;
std::int32_t gTextureFallbackCount = 0;
std::int32_t gTextureSizeW = 0;
std::int32_t gTextureSizeH = 0;

void ResetTextureNodes_523310() noexcept
{
    gTextureScanIndex = 0;
    for (std::int32_t index = 0; index < gTextureNodeCount; ++index) {
        auto& node = gTextureNodes[static_cast<std::size_t>(index)];
        node.imageNo = -1;
        node.reuseState = 0;
        node.next = nullptr;
    }
}

std::int32_t InvalidateGraphicRefs_523350() noexcept
{
    const std::int32_t count = resource::gMaxGraphicCount;
    if (count <= 0) {
        return count;
    }

    std::int32_t processed = 0;
    std::uint32_t legacyOffset = 0;
    do {
        const auto index = static_cast<std::size_t>(processed);
        DxGraphicEntry* headBase = gGraphicDxCache103CFA4;
        ++processed;
        headBase[index].head = nullptr;

        DxGraphicEntry* paletteBase = gGraphicDxCache103CFA4;
        paletteBase[index].palette = nullptr;
        const std::int32_t currentCount = resource::gMaxGraphicCount;
        legacyOffset += 0x10;
        if (processed >= currentCount) {
            break;
        }
    } while (true);

    return static_cast<std::int32_t>(legacyOffset);
}

std::int32_t BuildSurfaceCache_5231A0()
{
    gTextureNodeCount = 0;
    gTexturePreferredCount = 0;
    gTextureFallbackCount = 0;

    if (!CreateCpuAutoMapSurface()) {
        platform::ShowLegacyMessage(
            "AutoMap作業用Surface產生失敗。", "確認", 0x10);
        return 0;
    }

    if (!CreateCpuTempSurface() && !CreateCpuTempSurface()) {
        platform::ShowLegacyMessage(
            "建立戰鬥畫面錯誤", "確認", 0x10);
        return 0;
    }

    bool fallback = false;
    for (std::size_t index = 0; index < kTextureNodeCapacity; ++index) {
        if (!fallback) {
            const auto width = static_cast<std::int16_t>(gTextureSizeW);
            const auto height = static_cast<std::int16_t>(gTextureSizeH);
            gTextureNodes[index].surface = gTextureSurfaceOwner.Create(width, height);
            if (gTextureNodes[index].surface != nullptr) {
                ++gTexturePreferredCount;
                continue;
            }
            fallback = true;
        }

        const auto width = static_cast<std::int16_t>(gTextureSizeW);
        const auto height = static_cast<std::int16_t>(gTextureSizeH);
        gTextureNodes[index].surface = gTextureSurfaceOwner.Create(width, height);
        if (gTextureNodes[index].surface == nullptr) {
            platform::ShowLegacyMessage(
                "建立繪圖頁失敗", "確認", 0x10);
            return 0;
        }
        ++gTextureFallbackCount;
    }

    gTextureNodeCount = static_cast<std::int32_t>(kTextureNodeCapacity);
    ResetTextureNodes_523310();
    static_cast<void>(InvalidateGraphicRefs_523350());
    return 1;
}

} // namespace cg::render
