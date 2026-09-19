#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

extern std::uint8_t& gRendererMode;         // 原 0x62A640，参数模块四字节存储的首字节别名。
extern std::uint8_t gSupportedRenderMode;   // 原 0x628FEA，不能与请求模式合并。
extern std::int32_t gRendererLost;          // 原 0x62B064，恢复门槛。
extern std::uint16_t gPendingGraphicCount;  // 原 0x1020EB8，仅清零 16 位。
extern std::uint32_t gPendingGraphicCount2; // 原 0x73ECB8。
extern std::int32_t gValue58D3C0;           // 原 0x58D3C0，实传给 0x52CD30。
extern std::int32_t gForceRender;           // 原 0x73ECE4。
extern std::int32_t gValue73ECC0;           // 原 0x73ECC0，清屏前分支选择，完整枚举未定。

extern double gLastRenderDt;       // 原 0x73ECC8，共享帧截止，不是上一帧耗时。
extern double gFrameThreshold;     // 原 0x5590E8，可写，不能合并为下面的增量。
extern double gFrameIncrement;     // 原 0x5590F0，可写。
extern std::uint32_t gCatchUpCount; // 原 0x5590F8，比较时按 signed32，递增按 32 位回绕。
extern std::uint32_t gFrameCount;   // 原 0x58B7E4，映像初值 3。
extern std::int32_t gPendingPaletteReload; // 原 0x62A65C，仅等于 1 时消费。

extern std::uintptr_t gConnectionSocket; // 原 0xD9B2B8，SOCKET 仅在本机边界拓宽。
extern std::int32_t gWindowReportRequested; // 原 0x73ECEC，F10 写入。
extern std::int32_t gWindowReportAttempts;  // 原 0x73ECF8，signed32 比较。
extern std::int32_t gWindowReportValue;     // 原 0x73ECF4，报告值具体枚举未定。
extern std::uint32_t gWindowReportDeadline; // 原 0x73ED08，32 位 tick 与 0xFFFFFFFF 哨兵。

// 原 0x73ED10 的 64 位整秒基线；保留原两次 DWORD 存储，而不是原生原子计时值。
struct SystemTimeWords {
    std::uint32_t low;
    std::uint32_t high;
};
static_assert(sizeof(SystemTimeWords) == 8);
static_assert(offsetof(SystemTimeWords, high) == 4);
extern SystemTimeWords gLastSystemTime;
extern std::uint32_t gPeriodicReportSeconds; // 原 0x73ED18，阈值比较按 signed32。

// Win32 GetTickCount 的最小平台边界；保留原 32 位回绕值，不向 world 公开 Win32 类型。
std::uint32_t ReadTickCount32() noexcept;

} // namespace cg::platform
