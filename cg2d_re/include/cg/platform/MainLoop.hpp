#pragma once

#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::platform {

extern std::uint8_t& gRendererMode;         // 原 0x62A640，参数模块四字节存储的首字节别名。
extern std::uint8_t gSupportedRenderMode;   // 原 0x628FEA，不能与请求模式合并。
extern std::int32_t gRendererLost;          // 原 0x62B064，恢复门槛。
extern std::uint16_t gPendingGraphicCount;  // 原 0x1020EB8，仅清零 16 位。
extern std::uint32_t gPendingGraphicCount2; // 原 0x73ECB8。

extern double gLastRenderDt;       // 原 0x73ECC8，共享帧截止，不是上一帧耗时。
extern double gFrameThreshold;     // 原 0x5590E8，可写，不能合并为下面的增量。
extern double gFrameIncrement;     // 原 0x5590F0，可写。
extern std::uint32_t gCatchUpCount; // 原 0x5590F8，比较时按 signed32，递增按 32 位回绕。
extern std::uint32_t gFrameCount;   // 原 0x58B7E4，映像初值 3。
extern std::int32_t gPendingPaletteReload; // 原 0x62A65C，仅等于 1 时消费。

// CTM01：Win32 GetTickCount64减启动基线，完整64位毫秒；所有业务共用该时间域。
std::uint64_t ReadTickCount64() noexcept;
// CTM01/A11：WinMain按Debug／--debug设置，主循环据此提交诊断文本。
bool IsClientDiagnosticsEnabled() noexcept;
// 原函数：0x00457010。
void RequestRender_457010(session::ClientSession& session) noexcept; // 原457010：只将该会话强制呈现标志写1。

// 原 0x458DC0：发布 DX/输入能力探测结果，再返回其无符号阈值门禁。
std::int32_t CheckLegacyDxCapability_458DC0();

} // namespace cg::platform
