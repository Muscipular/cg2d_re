#include "StartupNative.hpp"
#include "ClientShutdown.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>

namespace cg::platform {

// 原函数：0x00458A40。
void ShowModeChange_458A40()
{
    static constexpr wchar_t kModeChangeText[] = L"正在改變描繪模式。";

    gPendingGraphicCount = 0;
    gPendingGraphicCount2 = 0;
    render::ResetGraphicCache_456D30();
    static_cast<void>(render::DrawText2_456670(
        250, 223, 3, 0, 0, kModeChangeText, 0));
    login::detail::DrawNineGrid_49BE00(
        245310, 196, 180, 256, 96, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}),
        74, 54);
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0());
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0();
    render::gRenderQueueMode103CF10 = 1;
    gPendingGraphicCount = 0;
    gPendingGraphicCount2 = 0;
    render::ResetGraphicCache_456D30();
    render::Flip_44C560();
}

// 原 [0x457020,0x4574DA)，分支、调用及数值边界见 doc/cg2d_main_loop.md。
int MainLoop()
{
    if (InitializeClient_4576B0() == 0) {
        return 0;
    }
    InitializeProtection_4041E0("kernel32.dll");
    ResetProtection_4048D0();
    const auto getTickCount = &GetTickCount;

    for (;;) {
        if (gRendererMode != gSupportedRenderMode) {
            ShowModeChange_458A40();
            render::RebuildRenderer_466B80();
            auto* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr && gWindowMode != 0) {
                render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
            }
        }
        if (gRendererLost != 0 && render::TestCooperativeLevel_44C6B0() != 0) {
            render::RebuildRenderer_466B80();
            auto* palette = render::GetRendererPalette(render::gRenderer);
            if (palette != nullptr && gWindowMode != 0) {
                render::SetPaletteEntries(palette, 0, 0, 256, render::gPalette);
            }
        }

        if (PumpWindowMessages() == 0) {
            ShutdownClient_457A40();
            ReleaseProtection_4042B0();
            return 0;
        }
        KeepAlive_457AB0();
        world::UpdateGameTime(&world::gCgTime);
        gPendingGraphicCount = 0;
        gPendingGraphicCount2 = 0;
        render::ResetGraphicCache_456D30();
        NetworkLoop_507DF0();
        Sub_52CD30(gValue58D3C0);
        world::Update();
        if (gWindowActiveState != 0) {
            UpdateCursor_506E00();
            render::SortTextures_520790();
            Sub_507140();
        }
        render::UpdatePalette_450BB0();
        if (gWindowActiveState != 0) {
            login::detail::Sub_449160();
        }
        ClearKeyFlags_459760();

        if (gForceRender == 0) {
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
                continue;
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
                    continue;
                }
            }
        }

        if (CheckProtection_4043B0("GetTickCount") != 0 && login::ReadRequestOutcomeFlag() == 1) {
            Report_512CA0(gConnectionSocket, 4);
        }
        const std::int32_t report = CheckWindows_4048E0();
        if (report > 0 && login::ReadRequestOutcomeFlag() == 1) {
            Report_512CA0(gConnectionSocket, report);
        }
        const std::int32_t clearMode = gValue73ECC0;
        gForceRender = 0;
        if (clearMode == 0) {
            render::ClearSurface_4512C0();
        } else if (clearMode == 3) {
            Sub_423640();
        }
        render::DrawAllQueued_520DD0();
        world::CopyWorldProjectionState_468FF0();
        render::Flip_44C560();

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
        render::BackupValues_5208F0();

        if (GetAsyncKeyState(VK_F10) < 0) {
            gWindowReportRequested = 1;
        }
        if (EnumerateWindows_4587D0() == 1 && gWindowReportAttempts >= 3) {
            gWindowReportRequested = 0;
            gWindowReportAttempts = 0;
            gWindowReportDeadline = getTickCount();
            const std::uint32_t nextTick = getTickCount();
            gWindowReportDeadline += 60000u * (nextTick % 10u + 10u);
        }
        if (gWindowReportValue > 0 && login::ReadRequestOutcomeFlag() == 1) {
            const std::uint32_t reportTick = getTickCount();
            const std::uint32_t reportDeadline = gWindowReportDeadline;
            if (reportTick > reportDeadline
                && std::bit_cast<std::int32_t>(reportDeadline) > 0) {
                Report_512CA0(gConnectionSocket, gWindowReportValue);
                gWindowReportDeadline = 0xFFFFFFFFu;
            }
        }

        bool initializeSystemTime = false;
        if (login::ReadRequestOutcomeFlag() == 1) {
            const std::uint32_t low = gLastSystemTime.low;
            const std::uint32_t high = gLastSystemTime.high;
            initializeSystemTime = (low | high) == 0;
        }
        std::int32_t seconds;
        if (initializeSystemTime) {
            SeedRandom_53B4BF(getTickCount());
            const std::uint64_t systemTime = ReadSystemSeconds_4589D0();
            gLastSystemTime.low = static_cast<std::uint32_t>(systemTime);
            gLastSystemTime.high = static_cast<std::uint32_t>(systemTime >> 32);
            seconds = Rand16() % 200 + 1100;
            gPeriodicReportSeconds = static_cast<std::uint32_t>(seconds);
        } else {
            bool resetSystemTime = false;
            if (login::ReadRequestOutcomeFlag() == 0) {
                const std::uint32_t high = gLastSystemTime.high;
                if (high != 0) {
                    resetSystemTime = true;
                } else {
                    resetSystemTime = gLastSystemTime.low != 0;
                }
            }
            if (resetSystemTime) {
                gLastSystemTime.low = 0;
                gLastSystemTime.high = 0;
                gPeriodicReportSeconds = 0;
                seconds = 0;
            } else {
                seconds = std::bit_cast<std::int32_t>(gPeriodicReportSeconds);
            }
        }
        if (CheckSystemSeconds_458A10(seconds) != 1) {
            continue;
        }
        if (std::bit_cast<std::int32_t>(gPeriodicReportSeconds) >= 1650) {
            continue;
        }
        Report_512CA0(gConnectionSocket, 101);
        const std::int32_t remainder = Rand16() % 200;
        gPeriodicReportSeconds += static_cast<std::uint32_t>(remainder);
    }
}

} // namespace cg::platform
