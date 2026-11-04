#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/session/ClientHost.hpp"
#include "../scene/login/LoginStateDependencies.hpp"
#include <format>
#include "StartupNative.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "ClientShutdown.hpp"
#include "MainLoopDependencies.hpp"
#include "SessionWindow.hpp"

#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/world/GameTime.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/WorldMapState.hpp"

#include <array>
#include <bit>
#include <cstdint>

#include "cg/core/vm.h"

namespace cg::platform {

std::uint8_t gSupportedRenderMode = 0; // 原全局：0x00628FEA。
std::int32_t gRendererLost = 0; // 原全局：0x0062B064。

double gLastRenderDt = 0; // 原全局：0x0073ECC8。
double gFrameThreshold = 1000.0 / 60.0; // 原全局：0x005590E8。
double gFrameIncrement = 1000.0 / 60.0; // 原全局：0x005590F0。
std::uint32_t gCatchUpCount = 1; // 原全局：0x005590F8。
std::uint32_t gFrameCount = 3; // 原全局：0x0058B7E4。
std::int32_t gPendingPaletteReload = 0; // 原全局：0x0062A65C。

__declspec(noinline)
static void UpdateFpsBuckets(float& fps, std::size_t& frameIndex, std::array<std::uint32_t, 3>& frameCounts)
{
    VM_LIGHT_START
    fps = static_cast<float>(frameCounts[0] + frameCounts[1] + frameCounts[2])
        / static_cast<float>(frameCounts.size());
    frameIndex = (frameIndex + 1) % frameCounts.size();
    frameCounts[frameIndex] = 0;
    VM_LIGHT_END
}

// CTM01/A11：参考UIExt::DrawFps，按实际提交帧统计三秒桶，沿现有文本队列绘制。
static void UpdateFrameDiagnostics(session::ClientSession& session)
{
    static std::uint64_t previousTick = ReadTickCount64();
    static float fps = 0;
    static std::size_t frameIndex = 0;
    static std::array<std::uint32_t, 3> frameCounts {};
    VM_LIGHT_START
    const std::uint64_t now = ReadTickCount64();
    const std::uint64_t interval = now - previousTick;
    ++frameCounts[frameIndex];
    if (previousTick / 1000u != now / 1000u) {
        UpdateFpsBuckets(fps, frameIndex, frameCounts);
    }
    previousTick = now;
#ifdef MY_DEBUG
    if (IsClientDiagnosticsEnabled()) {
        std::wstring message;
        text::FormatTo(message, L"FPS: {:.2f} {} ms", fps, interval);
        static_cast<void>(render::DrawText2_456670(200, 3, 3, 1, 5, message, 0));
        text::FormatTo(message, L"M: {} S: {}", session.WorldScene().sceneType, session.WorldScene().sceneState);
        static_cast<void>(render::DrawText2_456670(200, 20, 3, 1, 5, message, 0));
    }
#endif
    VM_LIGHT_END
}

// 原函数：0x00458A40。
void ShowModeChange_458A40(session::ClientSession& session)
{
    cg::render::gGraphicQueue.count = 0;
    cg::render::gTextQueue.count = 0;
    render::ResetGraphicCache_456D30();
    static_cast<void>(render::DrawText2_456670(
        250, 223, 3, 0, 0, L"正在改变描绘模式。", 0));
    cg::yobi::DrawNineGrid_49BE00(
        245310, 196, 180, 256, 96, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}),
        74, 54);
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0(session.WorldScene().backgroundColor));
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0(session);
    render::gRenderQueueMode103CF10 = 1;
    cg::render::gGraphicQueue.count = 0;
    cg::render::gTextQueue.count = 0;
    render::ResetGraphicCache_456D30();
    render::Flip_44C560();
}

// NSM01：每项完整消费共享工作区后才交给下一会话，业务来源始终显式传入。
static void AdvanceSessionFrame(session::ClientSession& session)
{
    const bool isCurrent = cg::session::GetClientHost().CurrentSession() == &session;
    auto& connection = session.Connection();
    KeepAlive_457AB0(connection);
    world::UpdateGameTime(session, &session.WorldScene().gameTime.value);
    cg::render::gGraphicQueue.count = 0;
    cg::render::gTextQueue.count = 0;
    render::ResetGraphicCache_456D30();
    static_cast<void>(world::ResetMapDrawCacheCounts_46BD40());
    if (connection.Active()) {
        const auto disconnect = connection.DisconnectState();
        if (disconnect == 1) {
            world::SetCurrentSceneType(session, 11);
            connection.DisconnectState() = 2;
        } else if (disconnect == 0) {
            const auto result = connection.Pump();
            if (result.terminated) {
                session.MarkDisconnected();
                connection.DisconnectState() = 1;
                const auto diagnostic = std::format(L"NETWORK CLOSED PHASE={} ERROR={} ENDPOINT={}",
                    static_cast<int>(result.terminal.phase), result.terminal.error, result.terminal.endpoint);
                static_cast<void>(login::detail::AppendSystemLog_44B5C0(diagnostic));
            }
        }
    }
    UpdateAudioAndSkipMciClose_52CD30(session, -1);
    world::Update(session);
    script::UpdateLuaScene(session);
    if (gWindowActiveState != 0 || !isCurrent) {
        UpdateCursor_506E00(session);
        render::SortTextures_520790();
        UpdateQueuedTextAndGraphicHover_507140(session);
    }
    if (isCurrent) {
        render::UpdatePalette_450BB0();
    }
    if (gWindowActiveState != 0 || !isCurrent) {
        cg::yobi::UpdateMapEnvironmentEffects_449160(session);
    }
    ClearKeyFlags_459760(session);
}

// 当前项独占宿主等待、追帧与呈现帧号，原两处continue只结束这一项。
static void FinishCurrentSessionFrame(session::ClientSession& session, const bool presentationReady)
{
    const auto getTickCount = &ReadTickCount64;
    if (session.WorldScene().forceRender == 0) {
        const std::int32_t windowActiveState = gWindowActiveState;
        if (windowActiveState == 0) {
            auto tick = getTickCount();
            // 原 x87 非“大于”包含 unordered，不能改成 <=。
            while (!(static_cast<double>(tick) > gLastRenderDt)) {
                Sleep(1);
                if (gPendingPaletteReload == 1) {
                    auto* palette = render::GetRendererPalette(render::gRenderer);
                    render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
                    gPendingPaletteReload = 0;
                }
                tick = getTickCount();
            }
            const double deadline = gFrameIncrement + gLastRenderDt;
            gCatchUpCount = 1;
            gLastRenderDt = deadline;
            return;
        }
        if (windowActiveState == 1) {
            gWindowActiveState = 2;
        }
        const double threshold = gFrameThreshold + gLastRenderDt;
        const auto tick = getTickCount();
        if (static_cast<double>(tick) > threshold) {
            const std::uint32_t catchUpCount = gCatchUpCount;
            if (std::bit_cast<std::int32_t>(catchUpCount) < 20) {
                const double deadline = gFrameIncrement + gLastRenderDt;
                gCatchUpCount = catchUpCount + 1u;
                gLastRenderDt = deadline;
                return;
            }
        }
    }

    const std::int32_t clearMode = session.WorldScene().frameBackgroundMode;
    session.WorldScene().forceRender = 0;
    if (clearMode == 0) {
        render::ClearSurface_4512C0(session.WorldScene().backgroundColor);
    } else if (clearMode == 3) {
        ApplyScreenShake_423640(session);
    }
    if (presentationReady) {
        UpdateFrameDiagnostics(session);
    }
    render::DrawAllQueued_520DD0(session);
    world::CopyWorldProjectionState_468FF0(session);
    if (presentationReady) {
        render::Flip_44C560();
    }

    auto tick = getTickCount();
    while (!(static_cast<double>(tick) > gLastRenderDt)) {
        Sleep(1);
        tick = getTickCount();
    }
    if (gPendingPaletteReload == 1) {
        auto* palette = render::GetRendererPalette(render::gRenderer);
        render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
        gPendingPaletteReload = 0;
    }
    const double deadline = gFrameIncrement + gLastRenderDt;
    const std::uint32_t frame = gFrameCount;
    gCatchUpCount = 1;
    gFrameCount = frame + 1;
    gLastRenderDt = deadline;
    render::BackupValues_5208F0(session);
}

static void FinishBackgroundSessionFrame(session::ClientSession& session)
{
    const std::int32_t clearMode = session.WorldScene().frameBackgroundMode;
    session.WorldScene().forceRender = 0;
    if (clearMode == 3) {
        ApplyScreenShake_423640(session, false);
    }
    render::DrawAllQueued_520DD0(session);
    world::CopyWorldProjectionState_468FF0(session);
    render::BackupValues_5208F0(session);
}

// 原 [0x457020,0x4574DA)，当前呈现分支见 doc/cg2d_main_loop.md；宿主扩展见NSM01。
int MainLoop(session::ClientSession& initialSession)
{
    auto& host = cg::session::GetClientHost();
    if (InitializeClient_4576B0(initialSession) == 0) {
        if (host.SessionCount() != 0) {
            static_cast<void>(ShutdownClient_457A40());
        }
        return 0;
    }
    host.Sessions().front().initialized = true;
    RefreshSessionWindow();

    for (;;) {
        auto* current = host.CurrentSession();
        if (current == nullptr) {
            static_cast<void>(ShutdownClient_457A40());
            return 0;
        }
        if (gRendererMode != gSupportedRenderMode) {
            ShowModeChange_458A40(*current);
            render::RebuildRenderer_466B80(*current);
            auto* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr) {
                render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
            }
        }
        if (gRendererLost != 0 && render::TestCooperativeLevel_44C6B0() != 0) {
            render::RebuildRenderer_466B80(*current);
            auto* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr) {
                render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
            }
        }

        if (PumpWindowMessages() == 0) {
            ShutdownClient_457A40();
            return 0;
        }
        if (!ProcessSessionWindowCommands()) {
            ShutdownClient_457A40();
            DestroyWindow(reinterpret_cast<HWND>(gLegacyWindow));
            return 0;
        }
        RefreshSessionWindow();
        current = host.CurrentSession();
        if (current == nullptr) {
            static_cast<void>(ShutdownClient_457A40());
            return 0;
        }
        const bool paletteReady = render::PrepareSessionPalette(*current);
        const bool presentationReady = paletteReady && world::PrepareSessionBackground(*current);
        for (auto& entry : host.Sessions()) {
            if (!entry.initialized) continue;
            AdvanceSessionFrame(entry.session);
            if (&entry.session == current) {
                FinishCurrentSessionFrame(entry.session, presentationReady);
            } else {
                FinishBackgroundSessionFrame(entry.session);
            }
        }
    }
}

} // namespace cg::platform
