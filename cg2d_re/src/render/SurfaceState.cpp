#include "cg/render/SurfaceState.hpp"

#include <algorithm>

namespace cg::render {

std::uint32_t gBackgroundColor = 0;
CpuSurfacePixels gCpuBackSurface {};
CpuSurfacePixels gCpuTempSurface {};

bool BlitTempToBack(
    const std::int32_t destinationX,
    const std::int32_t destinationY,
    const std::int32_t sourceLeft,
    const std::int32_t sourceTop,
    const std::int32_t sourceRight,
    const std::int32_t sourceBottom) noexcept
{
    if (destinationX < 0
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
            gCpuTempSurface.data() + sourceIndex,
            width,
            gCpuBackSurface.data() + destinationIndex);
    }
    return true;
}

} // namespace cg::render
