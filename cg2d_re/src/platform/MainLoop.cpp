#include "StartupNative.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "ClientShutdown.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/scene/login/LoginRequest.hpp"
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
#include "../world/PlayerState.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring> // 原CRT接口：0x0053BFC0；原参数／返回合同由直接调用承接。
#include <iterator>
#include <string_view>

namespace cg::platform {

std::uint8_t gSupportedRenderMode = 0; // 原全局：0x00628FEA。
std::int32_t gRendererLost = 0; // 原全局：0x0062B064。
std::uint16_t gPendingGraphicCount = 0; // 原全局：0x01020EB8。
std::uint32_t gPendingGraphicCount2 = 0; // 原全局：0x0073ECB8。
std::int32_t gValue58D3C0 = -1; // 原全局：0x0058D3C0。
std::int32_t gForceRender = 0; // 原全局：0x0073ECE4。
std::int32_t gValue73ECC0 = 0; // 原全局：0x0073ECC0。

double gLastRenderDt = 0; // 原全局：0x0073ECC8。
double gFrameThreshold = 1000.0 / 60.0; // 原全局：0x005590E8。
double gFrameIncrement = 1000.0 / 60.0; // 原全局：0x005590F0。
std::uint32_t gCatchUpCount = 1; // 原全局：0x005590F8。
std::uint32_t gFrameCount = 3; // 原全局：0x0058B7E4。
std::int32_t gPendingPaletteReload = 0; // 原全局：0x0062A65C。

std::uintptr_t gConnectionSocket = 0; // 原全局：0x00D9B2B8。
std::int32_t gWindowReportRequested = 0; // 原全局：0x0073ECEC。
std::uint32_t gWindowReportFlags = 0; // 原全局：0x0073ECF0。
std::int32_t gWindowReportAttempts = 0; // 原全局：0x0073ECF8。
std::int32_t gWindowReportValue = 0; // 原全局：0x0073ECF4。
double gWindowEnumerationDeadline = 0; // 原全局：0x0073ED00。
std::uint64_t gWindowReportDeadline = 0; // 原全局：0x0073ED08。
SystemTimeWords gLastSystemTime {};
std::uint32_t gPeriodicReportSeconds = 0; // 原全局：0x0073ED18。

namespace {

// CTM01/A11：参考UIExt::DrawFps，按实际提交帧统计三秒桶，沿现有文本队列绘制。
void UpdateFrameDiagnostics()
{
    static std::uint64_t previousTick = ReadTickCount64();
    const std::uint64_t now = ReadTickCount64();
    const std::uint64_t interval = now - previousTick;
    static float fps = 0;
    static std::size_t frameIndex = 0;
    static std::array<std::uint32_t, 3> frameCounts {};
    ++frameCounts[frameIndex];
    if (previousTick / 1000u != now / 1000u) {
        fps = static_cast<float>(frameCounts[0] + frameCounts[1] + frameCounts[2])
            / static_cast<float>(frameCounts.size());
        frameIndex = (frameIndex + 1) % frameCounts.size();
        frameCounts[frameIndex] = 0;
        std::wstring title(gWindowClassName);
        const char* const playerName = world::GetPlayerDisplayText(1);
        if (playerName[0] != '\0') {
            title += L" - [";
            title += text::FromGbkWide(playerName);
            title += L"]";
        }
        static_cast<void>(::SetWindowTextW(reinterpret_cast<HWND>(gLegacyWindow), title.c_str()));
    }
    previousTick = now;
    if (IsClientDiagnosticsEnabled()) {
        std::wstring message;
        text::FormatTo(message, L"FPS: {:.2f} {} ms", fps, interval);
        static_cast<void>(render::DrawText2_456670(200, 3, 3, 1, 5, message, 0));
        text::FormatTo(message, L"M: {} S: {}", world::gCurrentSceneType, world::gCurrentSceneState);
        static_cast<void>(render::DrawText2_456670(200, 20, 3, 1, 5, message, 0));
    }
}

// 原 CALLBACK@0x458840；保持 EnumWindows 的 HWND/LPARAM/BOOL 原生调用约定。
BOOL CALLBACK EnumerateWindowsCallback_458840(HWND window, LPARAM)
{
    wchar_t wideTitle[128] {};
    static_cast<void>(network::ReadProtocolByte593850());
    RECT rect; // 原 GetWindowRect 失败后仍读取，未提供默认矩形；风险见证据文档。
    ::GetWindowTextW(window, wideTitle, static_cast<int>(std::size(wideTitle)));
    const std::wstring_view title(wideTitle);
    ::GetWindowRect(window, &rect);

    const std::uint32_t height = static_cast<std::uint32_t>(rect.bottom)
        - static_cast<std::uint32_t>(rect.top);
    const std::uint32_t width = static_cast<std::uint32_t>(rect.right)
        - static_cast<std::uint32_t>(rect.left);

    auto* lowFlagByte = reinterpret_cast<unsigned char*>(&gWindowReportFlags);
    if (title.starts_with(L"魔力宝贝" /* 原0x559384、0x55937C、0x559370：直接比较UTF-16标题前缀。 */)) {
        *lowFlagByte |= 1u;
    }
    if (title.starts_with(L"PC精灵" /* 原常量参考：0x0055937C。 */)) {
        *lowFlagByte |= 2u;
    }

    std::uint16_t flags;
    std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
    if (title.starts_with(L" 信息资料 " /* 原常量参考：0x00559370。 */)) {
        flags = static_cast<std::uint16_t>(flags | 4u);
        std::memcpy(&gWindowReportFlags, &flags, sizeof(flags));
    }
    if (width == 249u && height == 136u) {
        flags = static_cast<std::uint16_t>(flags | 8u);
        std::memcpy(&gWindowReportFlags, &flags, sizeof(flags));
    }

    switch (flags) {
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15: {
        // 原 0x458975..0x458988：先读低 16 位，再只清低 16 位，报告值零扩展。
        std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
        constexpr std::uint16_t zero = 0;
        std::memcpy(&gWindowReportFlags, &zero, sizeof(zero));
        gWindowReportValue = static_cast<std::int32_t>(flags);
        return FALSE;
    }
    default:
        return TRUE;
    }
}

} // namespace

// 原 [0x4587D0,0x458839)：未触发枚举时的 -1 和独立 binary64 截止均保留。
int EnumerateWindows_4587D0()
{
    if (!(gLastRenderDt > gWindowEnumerationDeadline)) {
        return -1;
    }

    int result = -1;
    if (gWindowReportRequested == 1 && gWindowReportAttempts < 3) {
        result = static_cast<int>(::EnumWindows(&EnumerateWindowsCallback_458840, static_cast<LPARAM>(0)));
        const std::uint32_t attempts = std::bit_cast<std::uint32_t>(gWindowReportAttempts);
        gWindowReportAttempts = std::bit_cast<std::int32_t>(attempts + 1u);
    }

    gWindowEnumerationDeadline = gLastRenderDt + 3000.0;
    if (result != 0) {
        return result;
    }
    return gWindowReportValue > 0 ? 1 : 0;
}

// 原函数：0x00458A40。
void ShowModeChange_458A40()
{
    gPendingGraphicCount = 0;
    gPendingGraphicCount2 = 0;
    render::ResetGraphicCache_456D30();
    static_cast<void>(render::DrawText2_456670(
        250, 223, 3, 0, 0, L"正在改变描绘模式。", 0));
    cg::yobi::DrawNineGrid_49BE00(
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
    const auto getTickCount = &ReadTickCount64;

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
        script::UpdateLuaScene();
        if (gWindowActiveState != 0) {
            UpdateCursor_506E00();
            render::SortTextures_520790();
            Sub_507140();
        }
        render::UpdatePalette_450BB0();
        if (gWindowActiveState != 0) {
            cg::yobi::Sub_449160();
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

        if (CheckProtection_4043B0("GetTickCount64") != 0 && login::ReadRequestOutcomeFlag() == 1) {
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
        UpdateFrameDiagnostics();
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
            const std::uint64_t nextTick = getTickCount();
            gWindowReportDeadline += 60000u * (nextTick % 10u + 10u);
        }
        if (gWindowReportValue > 0 && login::ReadRequestOutcomeFlag() == 1) {
            const std::uint64_t reportTick = getTickCount();
            const std::uint64_t reportDeadline = gWindowReportDeadline;
            if (reportTick > reportDeadline
                && std::bit_cast<std::int64_t>(reportDeadline) > 0) {
                Report_512CA0(gConnectionSocket, gWindowReportValue);
                gWindowReportDeadline = UINT64_MAX;
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
            SeedRandom_53B4BF(static_cast<std::uint32_t>(getTickCount()));
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
