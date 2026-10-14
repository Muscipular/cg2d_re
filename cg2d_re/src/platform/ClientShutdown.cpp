#include "ClientShutdown.hpp"
#include "ImeLifecycle.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/AnimationRuntime.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/SdlPresenter.hpp"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {
namespace shutdown_detail {

// 原0x454EE0的SDL音乐清理，实现在LegacyAudio.cpp；保留本父无实参调用。
int Sub_454EE0();

// 原 [0x4705F0,0x4705FF)：两个调用后尾透传窗口关闭结果。
std::int32_t Sub_4705F0()
{
    static_cast<void>(ui::detail::WindowRuntime_SaveSettings());
    world::detail::Sub_5186A0();
    return ui::CloseAllWindows_472190();
}
} // namespace shutdown_detail

static_assert(sizeof(gDesktopColor) == sizeof(COLORREF));

// 原函数：0x00457A40。
int ShutdownClient_457A40()
{
    shutdown_detail::Sub_4705F0();
    resource::ReleaseIndependentPalettes_522C50();
    render::ReleaseGraphicPaletteCache_44E110();
    render::RendererDestroy_44C3C0();
    shutdown_detail::Sub_454EE0();
    audio::Sub_52B630();
    audio::Sub_52CCE0();
    cg::resource::animation::AnimeUnload_45BEF0();
    cg::resource::animation::Sub_45BF20();
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
