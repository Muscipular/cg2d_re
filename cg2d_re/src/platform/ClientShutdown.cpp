#include "ClientShutdown.hpp"
#include "ImeLifecycle.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {
namespace shutdown_detail {

// 原 0x4705F0 的三个真实依赖；共享 owner 与实现仍待落实。
int Sub_4C9130();                          // 原 0x4C9130：int()。
void Sub_5186A0();                         // 原 0x5186A0：void()。
char WindowCloseAll_472190();              // 原 Window::CloseAll@0x472190：char()。

// 以下均为原 0x457A40 的无实参直接依赖；未实现项只保留 IDA 签名。
HGLOBAL Sub_522C50();                     // 原 0x522C50：HGLOBAL()。
HGLOBAL Sub_44E110();                     // 原 0x44E110：HGLOBAL()。
void RendererDestroy_44C3C0();            // 原 0x44C3C0：void __cdecl()。
int Sub_454EE0();                          // 原 0x454EE0：int()。
int AnimeUnload_45BEF0();                  // 原 AnimeUnload@0x45BEF0：int()。
int Sub_45BF20();                          // 原 0x45BF20：int()。

// 原 [0x4705F0,0x4705FF)：两个调用后尾透传窗口关闭结果。
char Sub_4705F0()
{
    Sub_4C9130();
    Sub_5186A0();
    return WindowCloseAll_472190();
}
} // namespace shutdown_detail

static_assert(sizeof(gDesktopColor) == sizeof(COLORREF));

int ShutdownClient_457A40()
{
    shutdown_detail::Sub_4705F0();
    shutdown_detail::Sub_522C50();
    shutdown_detail::Sub_44E110();
    shutdown_detail::RendererDestroy_44C3C0();
    shutdown_detail::Sub_454EE0();
    audio::Sub_52B630();
    audio::Sub_52CCE0();
    shutdown_detail::AnimeUnload_45BEF0();
    shutdown_detail::Sub_45BF20();
    cg::resource::CloseGraphicFiles();
    cg::resource::Sub_457670();
    static_cast<void>(render::ReleaseFont_456530());
    ShutdownIme_459040();
    login::detail::Sub_404D60();
    cg::network::ShutdownNetworkRuntime_5081A0();
    ResetLegacyMouseTrackerDefaults_5066D0();

    ReleaseMutex(reinterpret_cast<HANDLE>(gStartupMutex));

    const INT element = COLOR_BACKGROUND;
    const COLORREF color = static_cast<COLORREF>(gDesktopColor);
    return SetSysColors(1, &element, &color);
}

} // namespace cg::platform
