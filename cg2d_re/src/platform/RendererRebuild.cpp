#include "MainLoopDependencies.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../render/GraphicDxCacheState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"

namespace cg::render {

// 原 [466B80,466D41)：失败投递后仍继续；分支与资源顺序见 cg2d_sdl_presenter.md。
void RebuildRenderer_466B80()
{
    constexpr char kRendererError[] =
        "DirectDraw\xB3\xF5\xC6\xDA\xBB\xAF\xCA\xA7\x94\xA1\xA1\xA3";
    constexpr char kConfirmCaption[] = "\xB4\x5F\xD5\x4A";

    const std::uint32_t previousQueueMode = gRenderQueueMode103CF10;
    RendererDestroy_44C3C0();
    const std::int8_t rendererResult = RendererInit_44B630();
    bool reportRendererFailure = rendererResult == 1;
    if (rendererResult == 2) {
        platform::gWindowMode = platform::gWindowMode != 1;
        reportRendererFailure = RendererInit_44B630() != 0;
    }
    if (reportRendererFailure) {
        platform::ShowLegacyMessage(kRendererError, kConfirmCaption, 0x10);
        platform::PostLegacyWindowMessage(platform::gLegacyWindow, 0x10, 0, 0);
    }

    static_cast<void>(BuildSurfaceCache_5231A0());
    if (InitPalette32() == 0) {
        platform::PostLegacyWindowMessage(platform::gLegacyWindow, 0x104, 0x0D, 0);
    }
    const std::int32_t sceneType = world::gCurrentSceneType;
    if (sceneType == 10) {
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(ResetGraphicCache_456D30());
        static_cast<void>(world::detail::Sub_46BD40());
        world::detail::Sub_46CF60(0);
        static_cast<void>(login::detail::DrawGraphic(
            320, 240, 200, world::detail::gBattleBackgroundImage59F9F8, 0, nullptr));
        SortTextures_520790();
        static_cast<void>(ClearSurface_4512C0());
        gRenderQueueMode103CF10 = 0;
        DrawAllQueued_520DD0();
        static_cast<void>(CopyBackToTemp());
        gRenderQueueMode103CF10 = previousQueueMode;
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(ResetGraphicCache_456D30());
        platform::gLastRenderDt = static_cast<double>(platform::ReadTickCount32());
    } else if (sceneType == 9) {
        static_cast<void>(world::detail::Sub_51D640());
    }

    auto* palette = GetRendererPalette(gRenderer);
    if (palette != nullptr && platform::gWindowMode != 0) {
        static_cast<void>(SetPaletteEntries(palette, 0, 0, 256, gPalette));
    }
    resource::ReleaseIndependentPalettes_522C50();
    static_cast<void>(resource::InitializeIndependentPalettes_522B30());
    platform::BorrowLegacyMouseStateStorage()->heldButtons = 0;
    platform::BorrowLegacyMouseStateStorage()->edgeButtons = 0;
    platform::gLastRenderDt = static_cast<double>(platform::ReadTickCount32());
}

} // namespace cg::render
