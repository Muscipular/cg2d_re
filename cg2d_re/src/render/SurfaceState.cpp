#include "cg/render/SurfaceState.hpp"

#include <algorithm>
#include <memory>
#include <new>

namespace cg::render {
namespace {

std::unique_ptr<CpuSurfacePixels> gCpuBackSurface;
std::unique_ptr<CpuSurfacePixels> gCpuAutoMapSurface;
std::unique_ptr<CpuSurfacePixels> gCpuTempSurface;

} // namespace

std::uint32_t gBackgroundColor = 0;
std::uint32_t gValueFF48E4 = 0;

bool CreateCpuBackSurface() noexcept
{
    gCpuBackSurface.reset(new (std::nothrow) CpuSurfacePixels {});
    return gCpuBackSurface != nullptr;
}

bool BuildCpuSurfaceBuffers() noexcept
{
    gCpuAutoMapSurface.reset(new (std::nothrow) CpuSurfacePixels {});
    if (gCpuAutoMapSurface == nullptr) {
        return false;
    }

    gCpuTempSurface.reset(new (std::nothrow) CpuSurfacePixels {});
    return gCpuTempSurface != nullptr;
}

void DestroyCpuCacheSurfaces() noexcept
{
    gCpuTempSurface.reset();
    gCpuAutoMapSurface.reset();
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
    return *gCpuBackSurface;
}

std::int32_t ClearSurface_4512C0() noexcept
{
    if (gCpuBackSurface == nullptr) {
        return -1;
    }
    const std::uint32_t color = gBackgroundColor;
    gCpuBackSurface->fill(color);
    return 0;
}

bool CopyBackToTemp() noexcept
{
    if (gCpuBackSurface == nullptr || gCpuTempSurface == nullptr) {
        return false;
    }
    *gCpuTempSurface = *gCpuBackSurface;
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
            gCpuTempSurface->data() + sourceIndex,
            width,
            gCpuBackSurface->data() + destinationIndex);
    }
    return true;
}

} // namespace cg::render
