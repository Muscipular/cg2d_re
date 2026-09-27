#include "cg/render/SurfaceState.hpp"

#include <algorithm>
#include <memory>
#include <new>

namespace cg::render {
namespace {

using CpuBackStorage = std::array<std::uint32_t, kCpuSurfacePixelCount + 1>;

struct CpuCacheStorage {
    CpuSurfacePixels pixels {};
    CpuCacheStorage* nextOwned {};
};

class CpuCacheOwner final {
public:
    ~CpuCacheOwner()
    {
        while (head_ != nullptr) {
            CpuCacheStorage* next = head_->nextOwned;
            delete head_;
            head_ = next;
        }
    }

    CpuCacheStorage* Create() noexcept
    {
        auto* surface = new (std::nothrow) CpuCacheStorage {};
        if (surface != nullptr) {
            surface->nextOwned = head_;
            head_ = surface;
        }
        return surface;
    }

    void Release(CpuCacheStorage*& surface) noexcept
    {
        CpuCacheStorage* current = surface;
        surface = nullptr;
        if (current == nullptr) {
            return;
        }
        CpuCacheStorage** link = &head_;
        while (*link != current) {
            link = &(*link)->nextOwned;
        }
        *link = current->nextOwned;
        delete current;
    }

private:
    CpuCacheStorage* head_ {};
};

std::unique_ptr<CpuBackStorage> gCpuBackSurface;
CpuCacheOwner gCpuCacheOwner;
CpuCacheStorage* gCpuAutoMapSurface {};
CpuCacheStorage* gCpuTempSurface {};

} // namespace

std::uint32_t gBackgroundColor = 0;
std::uint32_t gValueFF48E4 = 0;

bool CreateCpuBackSurface() noexcept
{
    gCpuBackSurface.reset(new (std::nothrow) CpuBackStorage {});
    return gCpuBackSurface != nullptr;
}

bool CreateCpuAutoMapSurface() noexcept
{
    gCpuAutoMapSurface = gCpuCacheOwner.Create();
    return gCpuAutoMapSurface != nullptr;
}

bool CreateCpuTempSurface() noexcept
{
    gCpuTempSurface = gCpuCacheOwner.Create();
    return gCpuTempSurface != nullptr;
}

bool BuildCpuSurfaceBuffers() noexcept
{
    if (!CreateCpuAutoMapSurface()) {
        return false;
    }
    return CreateCpuTempSurface();
}

void DestroyCpuCacheSurfaces() noexcept
{
    gCpuCacheOwner.Release(gCpuTempSurface);
    gCpuCacheOwner.Release(gCpuAutoMapSurface);
}

void DestroyCpuBackSurface() noexcept
{
    gCpuBackSurface.reset();
}

std::span<const std::uint32_t> ReadCpuBackPixels() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return {};
    }
    return {gCpuBackSurface->data(), kCpuSurfacePixelCount};
}

std::int32_t ClearSurface_4512C0() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return -1;
    }
    const std::uint32_t color = gBackgroundColor;
    std::fill_n(gCpuBackSurface->data(), kCpuSurfacePixelCount, color);
    return 0;
}

bool CopyBackToTemp() noexcept
{
    if (gCpuBackSurface == nullptr || gCpuTempSurface == nullptr) {
        return false;
    }
    std::copy_n(
        gCpuBackSurface->data(),
        kCpuSurfacePixelCount,
        gCpuTempSurface->pixels.data());
    return true;
}

bool BlitTempToBack(
    const std::int32_t destinationX,
    const std::int32_t destinationY,
    const std::int32_t sourceLeft,
    const std::int32_t sourceTop,
    const std::int32_t sourceRight,
    const std::int32_t sourceBottom) noexcept
{
    if (gCpuTempSurface == nullptr
        || gCpuBackSurface == nullptr
        || destinationX < 0
        || destinationY < 0
        || sourceLeft < 0
        || sourceTop < 0
        || sourceRight > kCpuSurfaceWidth
        || sourceBottom > kCpuSurfaceHeight
        || sourceLeft >= sourceRight
        || sourceTop >= sourceBottom) {
        return false;
    }

    const std::int32_t copyWidth = sourceRight - sourceLeft;
    const std::int32_t copyHeight = sourceBottom - sourceTop;
    if (destinationX > kCpuSurfaceWidth - copyWidth
        || destinationY > kCpuSurfaceHeight - copyHeight) {
        return false;
    }

    const std::size_t width = static_cast<std::size_t>(copyWidth);
    for (std::int32_t row = 0; row < copyHeight; ++row) {
        const std::size_t sourceIndex =
            static_cast<std::size_t>(sourceTop + row)
                * kCpuSurfacePitchPixels
            + static_cast<std::size_t>(sourceLeft);
        const std::size_t destinationIndex =
            static_cast<std::size_t>(destinationY + row)
                * kCpuSurfacePitchPixels
            + static_cast<std::size_t>(destinationX);
        std::copy_n(
            gCpuTempSurface->pixels.data() + sourceIndex,
            width,
            gCpuBackSurface->data() + destinationIndex);
    }
    return true;
}

} // namespace cg::render
