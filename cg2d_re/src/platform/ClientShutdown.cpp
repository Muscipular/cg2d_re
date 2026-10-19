#include "ClientShutdown.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "imeImpl.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include "cg/resource/GraphicRead.hpp"
#include "cg/resource/GraphicAlloc.hpp"
#include "cg/resource/AnimationRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "../world/AlbumState.hpp"

#include <cstdint>

#include <Windows.h>

namespace cg::platform {
namespace shutdown_detail {


// 原 [0x4705F0,0x4705FF)：两个调用后尾透传窗口关闭结果。
static std::int32_t Sub_4705F0()
{
    static_cast<void>(ui::detail::WindowRuntime_SaveSettings());
    world::Sub_5186A0();
    return ui::WindowRuntime_CloseAll();
}
} // namespace shutdown_detail

static_assert(sizeof(gDesktopColor) == sizeof(COLORREF));

// 原函数：0x00457A40。
int ShutdownClient_457A40()
{
    script::ShutdownLua();
    shutdown_detail::Sub_4705F0();
    resource::ReleaseIndependentPalettes_522C50();
    render::ReleaseGraphicPaletteCache_44E110();
    render::RendererDestroy_44C3C0();
    cg::audio::Sub_454EE0();
    audio::Sub_52B630();
    audio::Sub_52CCE0();
    cg::resource::animation::AnimeUnload_45BEF0();
    cg::resource::animation::CloseFlaggedAnimationFilesAndClearCache_45BF20();
    cg::resource::CloseGraphicFiles();
    cg::resource::FreePrimaryGraphicResourceTables_457670();
    static_cast<void>(render::ReleaseFont_456530());
    ShutdownIme_459040();
    cg::yobi::Sub_404D60();
    cg::network::ShutdownNetworkRuntime_5081A0();

    const INT element = COLOR_BACKGROUND;
    const COLORREF color = static_cast<COLORREF>(gDesktopColor);
    return SetSysColors(1, &element, &color);
}

} // namespace cg::platform
