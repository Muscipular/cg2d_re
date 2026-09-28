#include "MainLoopDependencies.hpp"
#include "ImeLifecycle.hpp"
#include "ClientShutdown.hpp"
#include "StartupNative.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../render/GraphicDxCacheState.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include "cg/AnimationRuntime.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/GraphicRead.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/ResourcePaths.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPaletteCache.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdio>

namespace cg::platform {
namespace initialization_detail {

// 原加载入口，完整本体位于 LoadingScreen.cpp。
std::int32_t InitLoading_457B10();
// 原直接目标尚待完整实现；不以固定返回或省略调用代替音频副作用。
std::int32_t InitializeMusic_454C30(std::uintptr_t window);

} // namespace initialization_detail

namespace {

constexpr char kConfirmCaption[] = "\xBD\x54\xBB\x7B";
constexpr char kRendererError[] =
    "DirectDraw\xAA\xEC\xB4\xC1\xA4\xC6\xA5\xA2\xB1\xD1\xA1\x43";
constexpr char kSurfaceMemoryError[] =
    "\xB0\x4F\xBE\xD0\xC5\xE9\xB0\x74\xB8\x6D\xBF\xF9\xBB\x7E";
constexpr char kAllocationError[] = "\xB0\x74\xB8\x6D\xBF\xF9\xBB\x7E";
constexpr char kGraphicError[] =
    "Graphic.bin\xB6\x7D\xB1\xD2\xA5\xA2\xB1\xD1\xA1\x43";
constexpr char kGraphicV3Error[] =
    "GraphicV3.bin\xB6\x7D\xB1\xD2\xA5\xA2\xB1\xD1";
constexpr char kGraphicInformation[] =
    "\xC0\xC9\xAE\xD7\xAC\x4F\xA7\x5F\xB7\x6C\xB7\xB4\xA1\x41"
    "\xA5\xB2\xB6\xB7\xA8\xCF\xA5\xCE\xB7\x73\xAA\xBA\xB1\xD2"
    "\xB0\xCA\xB5\x7B\xA6\xA1\xA1\x43\xBD\xD0\xAB\xF6\xA1\x75"
    "\xA2\xDD\xA2\xD9\xA1\x76\xBE\x5C\xC5\xAA\xB8\xD4\xB2\xD3"
    "\xBB\xA1\xA9\xFA\xA1\x43";
constexpr char kGraphicVersionError[] =
    "\xB5\x7B\xA6\xA1\xAA\xA9\xA5\xBB\xA4\xA3\xA5\xBF\xBD\x54"
    "\xA1\x41\xBD\xD0\xC1\xCA\xB6\x52\xB8\xEA\xAE\xC6\xA4\xF9"
    "\xA9\xCE\xA4\x55\xB8\xFC\xC2\xF7\xBD\x75\xA7\xF3\xB7\x73"
    "\xC0\xC9\xA1\x43";
constexpr char kSavedStateErrorFormat[] =
    "\xC5\xAA\xA4\x4A\xA6\x73\xC0\xC9\xA5\xA2\xB1\xD1\xA1\x43"
    "(code = %d)";
constexpr char kAnimeError[] = "Anime.bin \xC5\xAA\xA8\xFA\xBF\xF9\xBB\x7E";
constexpr std::uint32_t kErrorMessageFlags = 0x10;

// 原 73ECE8；两段淡变共用的图号，PE 零填充初值。
std::int32_t gLoadingGraphicImageNo = 0;

// 原 458489–458598 / 458695–4587A0 的相同公共段；不代替普通 MainLoop。
void AdvanceLoadingFadeFrame()
{
    render::SortTextures_520790();
    static_cast<void>(render::UpdatePalette_450BB0());
    ClearKeyFlags_459760();
    const double threshold = gFrameThreshold + gLastRenderDt;
    const std::uint32_t tick = ReadTickCount32();
    if (static_cast<double>(tick) > threshold) {
        const std::uint32_t catchUpCount = gCatchUpCount;
        if (std::bit_cast<std::int32_t>(catchUpCount) < 20) {
            const double deadline = gFrameIncrement + gLastRenderDt;
            gCatchUpCount = catchUpCount + 1u;
            gLastRenderDt = deadline;
            return;
        }
    }

    if (render::HasLegacy3DDevice_44B610() == 0) {
        render::gPaletteFrameVersion = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(render::gPaletteFrameVersion) + 1u);
    }
    static_cast<void>(render::ClearSurface_4512C0());
    render::DrawAllQueued_520DD0();
    if (gPendingPaletteReload == 1) {
        auto* palette = render::GetRendererPalette(render::gRenderer);
        static_cast<void>(render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette));
        gPendingPaletteReload = 0;
    }
    render::Flip_44C560();
    std::uint32_t waitTick = ReadTickCount32();
    while (!(static_cast<double>(waitTick) > gLastRenderDt)) {
        Sleep(1);
        waitTick = ReadTickCount32();
    }
    const double deadline = gFrameIncrement + gLastRenderDt;
    const std::uint32_t frame = gFrameCount;
    gCatchUpCount = 1;
    gFrameCount = frame + 1u;
    gLastRenderDt = deadline;
}

} // namespace

namespace initialization_detail {

std::int32_t FadeInLoading_458390()
{
    const std::int32_t capability = resource::ProbePaletteCapability_45C970();
    render::gPaletteCapability = capability;
    gLoadingGraphicImageNo = 22025;
    std::int32_t palette = 0;
    if (capability == 4 || capability == 3) {
        gLoadingGraphicImageNo = 98979;
        palette = 26;
    } else if (capability == 2 || capability == 1) {
        gLoadingGraphicImageNo = 22211;
        palette = 29;
    }
    gLastRenderDt = static_cast<double>(ReadTickCount32());
    bool firstFrame = true;
    bool keepRunning = true;
    for (;;) {
        if (PumpWindowMessages() == 0) {
            static_cast<void>(ShutdownClient_457A40());
            return 0;
        }
        if (firstFrame) {
            static_cast<void>(render::SelectPalette(palette, 30));
            static_cast<void>(login::detail::DrawGraphic(
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr));
            firstFrame = false;
        } else {
            static_cast<void>(login::detail::DrawGraphic(
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr));
            if (render::gPaletteChanging == 0) {
                keepRunning = false;
            }
        }
        AdvanceLoadingFadeFrame();
        if (!keepRunning) {
            static_cast<void>(login::detail::DrawGraphic(
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr));
            static_cast<void>(render::ClearSurface_4512C0());
            render::DrawAllQueued_520DD0();
            render::Flip_44C560();
            return 1;
        }
    }
}

std::int32_t FadeOutLoading_4585F0()
{
    gLastRenderDt = static_cast<double>(ReadTickCount32());
    bool firstFrame = true;
    bool keepRunning = true;
    for (;;) {
        if (PumpWindowMessages() == 0) {
            static_cast<void>(ShutdownClient_457A40());
            return 0;
        }
        if (firstFrame) {
            static_cast<void>(login::detail::DrawGraphic(
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr));
            static_cast<void>(render::SelectPalette(19, 30));
            firstFrame = false;
        } else {
            static_cast<void>(login::detail::DrawGraphic(
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr));
            if (render::gPaletteChanging == 0) {
                keepRunning = false;
            }
        }
        AdvanceLoadingFadeFrame();
        if (!keepRunning) {
            return 1;
        }
    }
}

} // namespace initialization_detail

int InitializeClient_4576B0()
{
    static_cast<void>(ResetSystemLog_44B5A0());
    static_cast<void>(ResetLegacyMouseTracker_506580());
    const std::int8_t rendererResult = render::RendererInit_44B630();
    bool reportRendererFailure = rendererResult == 1;
    if (rendererResult == 2) {
        gWindowMode = gWindowMode != 1;
        reportRendererFailure = render::RendererInit_44B630() != 0;
    }
    if (reportRendererFailure) {
        ShowLegacyMessage(kRendererError, kConfirmCaption, kErrorMessageFlags);
        PostLegacyWindowMessage(gLegacyWindow, 0x10, 0, 0);
    }

    // 原失败提示/PostMessage 之后仍继续，不在此插入返回或清理。
    render::gRenderQueueMode103CF10 = 1;
    if (render::BuildSurfaceCache_5231A0() == 0) {
        ShowLegacyMessage(kSurfaceMemoryError, "Error Code = 22", kErrorMessageFlags);
        return 0;
    }
    render::InitializeGraphicPaletteCache_44E0C0();
    if (render::InitPalette32() == 0) {
        return 0;
    }
    if (resource::GraphicAlloc_4574E0() == 0) {
        ShowLegacyMessage(kAllocationError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }
    static_cast<void>(render::InvalidateGraphicRefs_523350());
    const std::int32_t graphicResult = resource::GraphicInit_459960(
        resource::gGraphicPaths, resource::gGraphicInfoPaths);
    switch (graphicResult) {
    case 1:
        ShowLegacyMessage(kGraphicError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    case 2:
        ShowLegacyMessage(kGraphicV3Error, kConfirmCaption, kErrorMessageFlags);
        ShowLegacyMessage(kGraphicInformation, "INFORMATION", kErrorMessageFlags);
        return 0;
    case 7:
        ShowLegacyMessage(kGraphicVersionError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }

    static_cast<void>(resource::InitializeIndependentPalettes_522B30());
    SeedRandom_53B4BF(ReadTickCount32());
    static_cast<void>(world::InitializeRotationTable_46FB20());
    static_cast<void>(ResetLegacyMouseState_506870());
    static_cast<void>(login::detail::Sub_404A40());
    static_cast<void>(render::InitializeFont_456190(0));
    static_cast<void>(InitializeIme_458FD0());
    if (RestoreCurrentSavedState_5201B0() == 0) {
        char message[1024];
        std::sprintf(message, kSavedStateErrorFormat, static_cast<int>(gValueFF4E28));
        ShowLegacyMessage(message, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }

    const std::uint32_t drawCursor = gValueD67AE4;
    ShowLegacyCursor(0);
    if (drawCursor != 0) {
        const std::int32_t y = legacy_crypt_int::Add_401E10(&gLegacyMouseState.mouseY, 16);
        const std::int32_t x = legacy_crypt_int::Add_401E10(&gLegacyMouseState.mouseX, 16);
        static_cast<void>(login::detail::DrawGraphic(x, y, 231, 22000, 0, nullptr));
    }
    render::gRenderQueueMode103CF10 = 0;
    if (initialization_detail::InitLoading_457B10() == 0) {
        return 0;
    }
    login::detail::Sub_44B5C0("fadeInNowLoading Start");
    if (initialization_detail::FadeInLoading_458390() == 0) {
        return 0;
    }

    login::detail::Sub_44B5C0("initAutoMapColor Start");
    static_cast<void>(resource::LoadAutoMapColor_45A130(resource::gGraphicInfoPaths));
    login::detail::Sub_44B5C0("InitSprBinFileOpen Start");
    if (resource::animation::AnimeInit_45BB20(
            resource::gAnimePaths, resource::gAnimeInfoPaths) != 0) {
        ShowLegacyMessage(kAnimeError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }
    resource::animation::CoordLoadInit_45C040(
        resource::gCoordinatePaths, resource::gCoordinateInfoPaths);
    login::detail::Sub_44B5C0("t_music_init Start");
    static_cast<void>(audio::InitializeAudio_5241A0());
    static_cast<void>(initialization_detail::InitializeMusic_454C30(gLegacyWindow));
    login::detail::Sub_44B5C0("initAlbumStartUp Start");
    static_cast<void>(world::InitializeAlbum_518C20());

    const double initialFrameTime = static_cast<double>(ReadTickCount32());
    world::gPendingSceneType = -1;
    gLastRenderDt = initialFrameTime;
    gWindowEnumerationDeadline = initialFrameTime + 3000.0;
    login::detail::Sub_44B5C0("fadeOutNowLoading Start");
    if (initialization_detail::FadeOutLoading_4585F0() == 0) {
        return 0;
    }
    render::gRenderQueueMode103CF10 = 1;
    login::detail::Sub_44B5C0("initGame OK");
    static_cast<void>(ui::detail::InitializeWindowSystem_4705D0());
    return 1;
}

} // namespace cg::platform
