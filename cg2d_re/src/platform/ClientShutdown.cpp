#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/session/ClientHost.hpp"
#include "ClientShutdown.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "imeImpl.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "../scene/login/LoginSavedState.hpp"

#include "cg/resource/GraphicRead.hpp"
#include "cg/resource/GraphicAlloc.hpp"
#include "cg/resource/AnimationRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/world/SceneState.hpp"
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
static std::int32_t SaveUiAndAlbumThenCloseWindows_4705F0(session::ClientSession& session)
{
    static_cast<void>(ui::detail::WindowRuntime_SaveSettings(session));
    world::SaveAlbumData_5186A0(session);
    return ui::WindowRuntime_CloseAll(session);
}
} // namespace shutdown_detail

static_assert(sizeof(gDesktopColor) == sizeof(COLORREF));

void ShutdownSessionRuntime(session::ClientSession& session, const bool save)
{
    session.Connection().StopIo();
    script::ShutdownLua(session);
    if (save) {
        static_cast<void>(login::detail::SaveCurrentClientPreferences_520160(session.Connection()));
        static_cast<void>(shutdown_detail::SaveUiAndAlbumThenCloseWindows_4705F0(session));
    } else {
        static_cast<void>(ui::WindowRuntime_CloseAll(session));
    }
    audio::ShutdownSessionPlayback(session);
    ShutdownIme_459040(session);
    if (session.Yobi().start != nullptr && session.Yobi().end != nullptr) {
        cg::yobi::ShutdownYobiActionList_404D60(session);
    }
    session.Connection().Shutdown();
    session.Connection().DisconnectState() = 0;
}

// 原函数：0x00457A40；NSM01先完整释放各会话，再销毁唯一共享设备和资源。
int ShutdownClient_457A40()
{
    auto& host = session::GetClientHost();
    while (!host.Sessions().empty()) {
        auto& entry = host.Sessions().front();
        const std::uint64_t id = entry.id;
        ShutdownSessionRuntime(entry.session, entry.initialized);
        static_cast<void>(host.RemoveSession(id));
    }
    resource::ReleaseIndependentPalettes_522C50();
    render::ReleaseGraphicPaletteCache_44E110();
    render::RendererDestroy_44C3C0();
    cg::audio::ShutdownFixedMusic_454EE0();
    audio::ShutdownAudio_52B630();
    audio::StopAndCloseMciDevice_52CCE0();
    cg::resource::animation::ShutdownAnimationResources();
    cg::resource::CloseGraphicFiles();
    cg::resource::FreePrimaryGraphicResourceTables_457670();
    static_cast<void>(render::ReleaseFont_456530());
    const INT element = COLOR_BACKGROUND;
    const COLORREF color = static_cast<COLORREF>(gDesktopColor);
    return SetSysColors(1, &element, &color);
}

} // namespace cg::platform
