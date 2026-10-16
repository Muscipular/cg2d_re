#include "cg/platform/MainLoop.hpp"
#include "MainLoopDependencies.hpp"

namespace cg::platform {

std::uint8_t gSupportedRenderMode = 0; // 原全局：0x00628FEA。
std::int32_t gRendererLost = 0; // 原全局：0x0062B064。
std::uint16_t gPendingGraphicCount = 0; // 原全局：0x01020EB8。
std::uint32_t gPendingGraphicCount2 = 0; // 原全局：0x0073ECB8。
std::int32_t gValue58D3C0 = -1; // 原全局：0x0058D3C0。
std::int32_t gForceRender = 0; // 原全局：0x0073ECE4。
std::int32_t gValue73ECC0 = 0; // 原全局：0x0073ECC0。

double gLastRenderDt = 0; // 原全局：0x0073ECC8。
double gFrameThreshold = 1000.0 / 60.0; // 原全局：0x005590E8。
double gFrameIncrement = 1000.0 / 60.0; // 原全局：0x005590F0。
std::uint32_t gCatchUpCount = 1; // 原全局：0x005590F8。
std::uint32_t gFrameCount = 3; // 原全局：0x0058B7E4。
std::int32_t gPendingPaletteReload = 0; // 原全局：0x0062A65C。

std::uintptr_t gConnectionSocket = 0; // 原全局：0x00D9B2B8。
std::int32_t gWindowReportRequested = 0; // 原全局：0x0073ECEC。
std::uint32_t gWindowReportFlags = 0; // 原全局：0x0073ECF0。
std::int32_t gWindowReportAttempts = 0; // 原全局：0x0073ECF8。
std::int32_t gWindowReportValue = 0; // 原全局：0x0073ECF4。
double gWindowEnumerationDeadline = 0; // 原全局：0x0073ED00。
std::uint32_t gWindowReportDeadline = 0; // 原全局：0x0073ED08。
SystemTimeWords gLastSystemTime {};
std::uint32_t gPeriodicReportSeconds = 0; // 原全局：0x0073ED18。

} // namespace cg::platform
