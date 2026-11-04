#include "../scene/login/LoginSavedState.hpp"
#include "MainLoopDependencies.hpp"
#include "imeImpl.hpp"
#include "ClientShutdown.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "StartupNative.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "../render/GraphicDxCacheState.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include "cg/resource/AnimationRuntime.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/resource/GraphicAlloc.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
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
#include <random>

namespace cg::platform {
namespace initialization_detail {

// 原加载入口，完整本体位于 LoadingScreen.cpp。
// 原函数：0x00457B10。
std::int32_t InitLoading_457B10(network::CgConnection& connection);
// 原直接目标的SDL音乐提供者，实现在LegacyAudio.cpp；保留本父调用顺序。

} // namespace initialization_detail

static constexpr wchar_t kConfirmCaption[] = L"确认";
static constexpr std::uint32_t kErrorMessageFlags = 0x10;

// 原 73ECE8；两段淡变共用的图号，PE 零填充初值。
static std::int32_t gLoadingGraphicImageNo = 0;

// 原 458489–458598 / 458695–4587A0 的相同公共段；不代替普通 MainLoop。
static void AdvanceLoadingFadeFrame(session::ClientSession& session)
{
    render::SortTextures_520790(session);
    static_cast<void>(render::UpdatePalette_450BB0());
    ClearKeyFlags_459760(session);
    const double threshold = gFrameThreshold + gLastRenderDt;
    const std::uint64_t tick = ReadTickCount64();
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
    render::DrawAllQueued_520DD0(session);
    if (gPendingPaletteReload == 1) {
        auto* palette = render::GetRendererPalette(render::gRenderer);
        static_cast<void>(render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette));
        gPendingPaletteReload = 0;
    }
    render::Flip_44C560();
    std::uint64_t waitTick = ReadTickCount64();
    while (!(static_cast<double>(waitTick) > gLastRenderDt)) {
        Sleep(1);
        waitTick = ReadTickCount64();
    }
    const double deadline = gFrameIncrement + gLastRenderDt;
    const std::uint32_t frame = gFrameCount;
    gCatchUpCount = 1;
    gFrameCount = frame + 1u;
    gLastRenderDt = deadline;
}

namespace initialization_detail {

// 原函数：0x00458390。
static std::int32_t FadeInLoading_458390(session::ClientSession& session)
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
    gLastRenderDt = static_cast<double>(ReadTickCount64());
    bool firstFrame = true;
    bool keepRunning = true;
    for (;;) {
        if (PumpWindowMessages() == 0) {
            static_cast<void>(ShutdownClient_457A40(session));
            return 0;
        }
        if (firstFrame) {
            static_cast<void>(render::SelectPalette(palette, 30));
            // LUR01：loading整图随客户区缩放，保留原图号、层序和调色板淡变。
            static_cast<void>(render::StretchGraphicToScreen(session, cg::render::DrawGraphic(session,
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr)));
            firstFrame = false;
        } else {
            static_cast<void>(render::StretchGraphicToScreen(session, cg::render::DrawGraphic(session,
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr)));
            if (render::gPaletteChanging == 0) {
                keepRunning = false;
            }
        }
        AdvanceLoadingFadeFrame(session);
        if (!keepRunning) {
            static_cast<void>(render::StretchGraphicToScreen(session, cg::render::DrawGraphic(session,
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr)));
            static_cast<void>(render::ClearSurface_4512C0());
            render::DrawAllQueued_520DD0(session);
            render::Flip_44C560();
            return 1;
        }
    }
}

// 原函数：0x004585F0。
static std::int32_t FadeOutLoading_4585F0(session::ClientSession& session)
{
    gLastRenderDt = static_cast<double>(ReadTickCount64());
    bool firstFrame = true;
    bool keepRunning = true;
    for (;;) {
        if (PumpWindowMessages() == 0) {
            static_cast<void>(ShutdownClient_457A40(session));
            return 0;
        }
        if (firstFrame) {
            static_cast<void>(render::StretchGraphicToScreen(session, cg::render::DrawGraphic(session,
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr)));
            static_cast<void>(render::SelectPalette(19, 30));
            firstFrame = false;
        } else {
            static_cast<void>(render::StretchGraphicToScreen(session, cg::render::DrawGraphic(session,
                320, 240, 255, gLoadingGraphicImageNo, 0, nullptr)));
            if (render::gPaletteChanging == 0) {
                keepRunning = false;
            }
        }
        AdvanceLoadingFadeFrame(session);
        if (!keepRunning) {
            return 1;
        }
    }
}

} // namespace initialization_detail

// 原函数：0x004576B0。
int InitializeClient_4576B0(session::ClientSession& session)
{
    static_cast<void>(ResetSystemLog_44B5A0());
    const std::int8_t rendererResult = render::RendererInit_44B630();
    if (rendererResult != 0) {
        ShowWideMessage(L"DirectDraw初期化失败。", kConfirmCaption, kErrorMessageFlags);
        ::PostMessageW(reinterpret_cast<HWND>(gLegacyWindow), 0x10, 0, 0);
    }

    // 原失败提示/PostMessage 之后仍继续，不在此插入返回或清理。
    render::gRenderQueueMode103CF10 = 1;
    if (render::BuildSurfaceCache_5231A0() == 0) {
        ShowWideMessage(L"记忆体配置错误", L"Error Code = 22", kErrorMessageFlags);
        return 0;
    }
    render::InitializeGraphicPaletteCache_44E0C0();
    if (render::InitPalette32() == 0) {
        return 0;
    }
    const std::int32_t graphicResult = resource::GraphicInit_459960(
        resource::gGraphicPaths, resource::gGraphicInfoPaths);
    switch (graphicResult) {
    case 1:
        ShowWideMessage(L"Graphic.bin开启失败。", kConfirmCaption, kErrorMessageFlags);
        return 0;
    case 2:
        ShowWideMessage(L"GraphicV3.bin开启失败", kConfirmCaption, kErrorMessageFlags);
        ShowWideMessage(L"档案是否损毁，必须使用新的启动程式。请按「ＯＫ」阅读详细说明。", L"INFORMATION", kErrorMessageFlags);
        return 0;
    case 7:
        ShowWideMessage(L"程式版本不正确，请购买资料片或下载离线更新档。", kConfirmCaption, kErrorMessageFlags);
        return 0;
    default:
        if (graphicResult != 0) {
            ShowWideMessage(L"图形资源初始化失败。", kConfirmCaption, kErrorMessageFlags);
            return 0;
        }
        break;
    }

    // RAC01：先完成索引覆盖和引用整理，首次画面使用前发布稳定图形表。
    if (resource::animation::AnimeInit_45BB20(
            resource::gAnimePaths, resource::gAnimeInfoPaths) != 0) {
        resource::CloseGraphicFiles();
        resource::FreePrimaryGraphicResourceTables_457670();
        ShowWideMessage(L"Anime.bin 读取错误", kConfirmCaption, kErrorMessageFlags);
        return 0;
    }
    resource::animation::CoordLoadInit_45C040(
        resource::gCoordinatePaths, resource::gCoordinateInfoPaths);
    cg::yobi::RetainSceneEffectGraphicReferences();
    if (resource::GraphicAlloc_4574E0() == 0) {
        resource::animation::AnimeUnload_45BEF0();
        resource::CloseGraphicFiles();
        resource::FreePrimaryGraphicResourceTables_457670();
        ShowWideMessage(L"资源引用整理或配置失败", kConfirmCaption, kErrorMessageFlags);
        return 0;
    }
    static_cast<void>(render::InvalidateGraphicRefs_523350());
    static_cast<void>(resource::InitializeIndependentPalettes_522B30());
    SeedRandom_53B4BF(std::random_device()() ^ static_cast<std::uint32_t>(ReadTickCount64()));
    static_cast<void>(world::InitializeRotationTable_46FB20(session));
    static_cast<void>(ResetLegacyMouseState_506870(session));
    static_cast<void>(cg::yobi::InitializeYobiActionList_404A40(session));
    static_cast<void>(render::InitializeFont_456190(0));
    static_cast<void>(InitializeIme_458FD0(session));
    if (cg::login::detail::RestoreCurrentSavedState_5201B0(session) == 0) {
        const std::wstring message = std::format(L"读入存档失败。(code = {})", static_cast<int>(session.SavedLogin().loadErrorCode));
        ShowWideMessage(message, kConfirmCaption, kErrorMessageFlags);
        return 0;
    }

    const std::uint32_t drawCursor = gNativeCursorEnabled_D67AE4;
    ::ShowCursor(0);
    if (drawCursor != 0) {
        const std::int32_t y = (session.UiFrame().mouse.state->mouseY + 16);
        const std::int32_t x = (session.UiFrame().mouse.state->mouseX + 16);
        static_cast<void>(cg::render::DrawGraphic(session, x, y, 231, 22000, 0, nullptr));
    }
    render::gRenderQueueMode103CF10 = 0;
    if (initialization_detail::InitLoading_457B10(session.Connection()) == 0) {
        return 0;
    }
    login::detail::AppendSystemLog_44B5C0(L"fadeInNowLoading Start");
    if (initialization_detail::FadeInLoading_458390(session) == 0) {
        return 0;
    }

    login::detail::AppendSystemLog_44B5C0(L"initAutoMapColor Start");
    static_cast<void>(resource::LoadAutoMapColor_45A130(resource::gGraphicInfoPaths));
    login::detail::AppendSystemLog_44B5C0(L"t_music_init Start");
    static_cast<void>(audio::InitializeAudio_5241A0(session));
    static_cast<void>(cg::audio::InitializeMusic_454C30(session, gLegacyWindow));
    login::detail::AppendSystemLog_44B5C0(L"initAlbumStartUp Start");
    static_cast<void>(world::InitializeAlbum_518C20(session));

    const double initialFrameTime = static_cast<double>(ReadTickCount64());
    session.WorldScene().pendingSceneType = -1;
    gLastRenderDt = initialFrameTime;
    login::detail::AppendSystemLog_44B5C0(L"fadeOutNowLoading Start");
    if (initialization_detail::FadeOutLoading_4585F0(session) == 0) {
        return 0;
    }
    render::gRenderQueueMode103CF10 = 1;
    login::detail::AppendSystemLog_44B5C0(L"initGame OK");
    static_cast<void>(ui::detail::WindowRuntime_Initialize(session));
    return script::InitializeLua(session) ? 1 : 0;
}

} // namespace cg::platform
