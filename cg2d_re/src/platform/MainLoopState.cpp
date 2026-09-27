#include "cg/platform/MainLoop.hpp"
#include "MainLoopDependencies.hpp"

#include <bit>

namespace cg::platform {

std::uint8_t gSupportedRenderMode = 0;
std::int32_t gRendererLost = 0;
std::uint16_t gPendingGraphicCount = 0;
std::uint32_t gPendingGraphicCount2 = 0;
std::int32_t gValue58D3C0 = -1;
std::int32_t gForceRender = 0;
std::int32_t gValue73ECC0 = 0;

double gLastRenderDt = 0;
double gFrameThreshold = std::bit_cast<double>(std::uint64_t {0x4030AAAAAAAAAAAB});
double gFrameIncrement = std::bit_cast<double>(std::uint64_t {0x4030AAAAAAAAAAAB});
std::uint32_t gCatchUpCount = 1;
std::uint32_t gFrameCount = 3;
std::int32_t gPendingPaletteReload = 0;

std::uintptr_t gConnectionSocket = 0;
std::int32_t gWindowReportRequested = 0;
std::uint32_t gWindowReportFlags = 0;
std::int32_t gWindowReportAttempts = 0;
std::int32_t gWindowReportValue = 0;
double gWindowEnumerationDeadline = 0;
std::uint32_t gWindowReportDeadline = 0;
SystemTimeWords gLastSystemTime {};
std::uint32_t gPeriodicReportSeconds = 0;

} // namespace cg::platform
