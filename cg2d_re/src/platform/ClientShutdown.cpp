#include "ClientShutdown.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/Startup.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

struct IDirectSound;

namespace cg::platform {
namespace shutdown_detail {

// 以下均为原 0x457A40 的无实参直接依赖；尚未实现，只保留 IDA 签名。
char Sub_4705F0();                         // 原 0x4705F0：char()。
HGLOBAL Sub_522C50();                     // 原 0x522C50：HGLOBAL()。
HGLOBAL Sub_44E110();                     // 原 0x44E110：HGLOBAL()。
void RendererDestroy_44C3C0();            // 原 0x44C3C0：void __cdecl()。
int Sub_454EE0();                          // 原 0x454EE0：int()。
IDirectSound* Sub_52B630();                // 原 0x52B630：LPDIRECTSOUND()。
bool Sub_52CCE0();                         // 原 0x52CCE0：bool()。
int AnimeUnload_45BEF0();                  // 原 AnimeUnload@0x45BEF0：int()。
int Sub_45BF20();                          // 原 0x45BF20：int()。
void Sub_457670();                         // 原 0x457670：void()。
HFONT Sub_456530();                        // 原 0x456530：HFONT()。
void Sub_459040();                         // 原 0x459040：void()。
void Sub_5081A0();                         // 原 0x5081A0：void()。
int Sub_5066D0();                          // 原 0x5066D0：int()。

} // namespace shutdown_detail

static_assert(sizeof(gDesktopColor) == sizeof(COLORREF));

int ShutdownClient_457A40()
{
    shutdown_detail::Sub_4705F0();
    shutdown_detail::Sub_522C50();
    shutdown_detail::Sub_44E110();
    shutdown_detail::RendererDestroy_44C3C0();
    shutdown_detail::Sub_454EE0();
    shutdown_detail::Sub_52B630();
    shutdown_detail::Sub_52CCE0();
    shutdown_detail::AnimeUnload_45BEF0();
    shutdown_detail::Sub_45BF20();
    cg::resource::CloseGraphicFiles();
    shutdown_detail::Sub_457670();
    shutdown_detail::Sub_456530();
    shutdown_detail::Sub_459040();
    login::detail::Sub_404D60();
    shutdown_detail::Sub_5081A0();
    shutdown_detail::Sub_5066D0();

    ReleaseMutex(reinterpret_cast<HANDLE>(gStartupMutex));

    const INT element = COLOR_BACKGROUND;
    const COLORREF color = static_cast<COLORREF>(gDesktopColor);
    return SetSysColors(1, &element, &color);
}

} // namespace cg::platform
