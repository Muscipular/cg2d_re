#include "MainLoopDependencies.hpp"
#include "ImeLifecycle.hpp"
#include "ClientShutdown.hpp"
#include "StartupNative.hpp"
#include "LegacyFormat.hpp"

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
#include <format>
#include <string>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::platform {
namespace initialization_detail {

// 原加载入口，完整本体位于 LoadingScreen.cpp。
// 原函数：0x00457B10。
std::int32_t InitLoading_457B10();
// 原直接目标的SDL音乐提供者，实现在LegacyAudio.cpp；保留本父调用顺序。
// 原函数：0x00454C30。
std::int32_t InitializeMusic_454C30(std::uintptr_t window);

} // namespace initialization_detail

namespace {

constexpr wchar_t kConfirmCaption[] = L"確認";
constexpr wchar_t kRendererError[] = L"DirectDraw初期化失敗。";
constexpr wchar_t kSurfaceMemoryError[] = L"記憶體配置錯誤";
constexpr wchar_t kAllocationError[] = L"配置錯誤";
constexpr wchar_t kGraphicError[] = L"Graphic.bin開啟失敗。";
constexpr wchar_t kGraphicV3Error[] = L"GraphicV3.bin開啟失敗";
constexpr wchar_t kGraphicInformation[] = L"檔案是否損毀，必須使用新的啟動程式。請按「ＯＫ」閱讀詳細說明。";
constexpr wchar_t kGraphicVersionError[] = L"程式版本不正確，請購買資料片或下載離線更新檔。";
constexpr wchar_t kSavedStateErrorFormat[] = L"讀入存檔失敗。(code = {})";
constexpr wchar_t kAnimeError[] = L"Anime.bin 讀取錯誤";
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

// 原函数：0x00458390。
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

// 原函数：0x004585F0。
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

// 原函数：0x004576B0。
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
        ShowWideMessage(kRendererError, kConfirmCaption, kErrorMessageFlags);
        PostLegacyWindowMessage(gLegacyWindow, 0x10, 0, 0);
    }

    // 原失败提示/PostMessage 之后仍继续，不在此插入返回或清理。
    render::gRenderQueueMode103CF10 = 1;
    if (render::BuildSurfaceCache_5231A0() == 0) {
        ShowWideMessage(kSurfaceMemoryError, L"Error Code = 22", kErrorMessageFlags);
        return 0;
    }
    render::InitializeGraphicPaletteCache_44E0C0();
    if (render::InitPalette32() == 0) {
        return 0;
    }
    if (resource::GraphicAlloc_4574E0() == 0) {
        ShowWideMessage(kAllocationError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }
    static_cast<void>(render::InvalidateGraphicRefs_523350());
    const std::int32_t graphicResult = resource::GraphicInit_459960(
        resource::gGraphicPaths, resource::gGraphicInfoPaths);
    switch (graphicResult) {
    case 1:
        ShowWideMessage(kGraphicError, kConfirmCaption, kErrorMessageFlags);
        return 0;
    case 2:
        ShowWideMessage(kGraphicV3Error, kConfirmCaption, kErrorMessageFlags);
        ShowWideMessage(kGraphicInformation, L"INFORMATION", kErrorMessageFlags);
        return 0;
    case 7:
        ShowWideMessage(kGraphicVersionError, kConfirmCaption, kErrorMessageFlags);
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
        const std::wstring message = std::format(kSavedStateErrorFormat, static_cast<int>(gValueFF4E28));
        ShowWideMessage(message, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }

    const std::uint32_t drawCursor = gValueD67AE4;
    ::ShowCursor(0);
    if (drawCursor != 0) {
        const std::int32_t y = (BorrowLegacyMouseStateStorage()->mouseY + 16);
        const std::int32_t x = (BorrowLegacyMouseStateStorage()->mouseX + 16);
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
        ShowWideMessage(kAnimeError, kConfirmCaption, kErrorMessageFlags);
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
