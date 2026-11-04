#include "../platform/MainLoopDependencies.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "GraphicDxCacheState.hpp"

#include "cg/resource/GraphicRead.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/BattleState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <Windows.h>

namespace cg::render {

// 原 [466B80,466D41)：失败投递后仍继续；分支与资源顺序见 cg2d_sdl_presenter.md。
void RebuildRenderer_466B80(session::ClientSession& session)
{
    const std::uint32_t previousQueueMode = gRenderQueueMode103CF10;
    RendererDestroy_44C3C0();
    const std::int8_t rendererResult = RendererInit_44B630();
    if (rendererResult != 0) {
        platform::ShowWideMessage(L"DirectDraw初期化失败。", L"确认", 0x10);
        ::PostMessageW(reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0);
    }

    static_cast<void>(BuildSurfaceCache_5231A0());
    if (InitPalette32() == 0) {
        ::PostMessageW(reinterpret_cast<HWND>(platform::gLegacyWindow), WM_CLOSE, 0, 0);
    }
    const std::int32_t sceneType = session.WorldScene().sceneType;
    if (sceneType == 10) {
        platform::gPendingGraphicCount = 0;
        session.TextQueue().count = 0;
        static_cast<void>(ResetGraphicCache_456D30(session));
        static_cast<void>(world::ResetMapDrawCacheCounts_46BD40());
        world::DrawMapScene_46CF60(session, 0);
        static_cast<void>(cg::render::DrawBattleBackgroundScaled(
            200, session.WorldScene().battle.backgroundImage));
        SortTextures_520790();
        static_cast<void>(ClearSurface_4512C0());
        gRenderQueueMode103CF10 = 0;
        DrawAllQueued_520DD0(session);
        static_cast<void>(CopyBackToTemp());
        gRenderQueueMode103CF10 = previousQueueMode;
        platform::gPendingGraphicCount = 0;
        session.TextQueue().count = 0;
        static_cast<void>(ResetGraphicCache_456D30(session));
        platform::gLastRenderDt = static_cast<double>(platform::ReadTickCount64());
    } else if (sceneType == 9) {
        static_cast<void>(world::CaptureMapTilesForTransition_51D640(session));
    }

    auto* palette = GetRendererPalette(gRenderer);
    if (palette != nullptr) {
        static_cast<void>(SetPaletteEntries(palette, 0, 0, 256, gPalette));
    }
    resource::ReleaseIndependentPalettes_522C50();
    static_cast<void>(resource::InitializeIndependentPalettes_522B30());
    session.UiFrame().mouse.state->heldButtons = 0;
    session.UiFrame().mouse.state->edgeButtons = 0;
    platform::gLastRenderDt = static_cast<double>(platform::ReadTickCount64());
}

} // namespace cg::render
