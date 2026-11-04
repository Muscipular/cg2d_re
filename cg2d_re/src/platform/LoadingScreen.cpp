#include "cg/session/ClientSession.hpp"
#include "ClientShutdown.hpp"
#include "MainLoopDependencies.hpp"
#include "StartupNative.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>

namespace cg::platform::initialization_detail {

// 原 [73ED28,73ED38)，跨次调用保留；只有 phase 0 执行原初始化写入。
static render::GraphicDescriptor16 gLoadingDescriptor{};

// 原 InitLoading@[457B10,4582FE)，完整阶段及公共帧尾见 cg2d_loading_screen.md。
std::int32_t InitLoading_457B10(network::CgConnection& connection)
{
    std::int32_t phase = 21; // 自动跳过片头，复用原按键跳过后的清理阶段。
    std::int32_t image = 0;
    bool keepRunning = true;
    render::gPaletteCapability = resource::ProbePaletteCapability_45C970();
    std::uint64_t phaseTick = ReadTickCount64();
    gLastRenderDt = static_cast<double>(phaseTick);
    auto& descriptor = gLoadingDescriptor;
    auto* const colorBytes = reinterpret_cast<unsigned char*>(&descriptor.color);
    for (;;) {
        if (PumpWindowMessages() == 0) {
            static_cast<void>(ShutdownClient_457A40());
            return 0;
        }
        cg::render::gGraphicQueue.count = 0;
        cg::render::gTextQueue.count = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(connection.Owner())) & 3u) != 0
            || (ReadSessionKeyState(connection.Owner(), 13) & 2u) != 0
            || (ReadSessionKeyState(connection.Owner(), 32) & 2u) != 0
            || (ReadSessionKeyState(connection.Owner(), 27) & 2u) != 0) {
            if (phase < 21 || (phase >= 30 && phase <= 33)) {
                phase = 21;
            }
        }

        switch (phase) {
        case 0: {
            static_cast<void>(render::ClearSurface_4512C0(connection.Owner().WorldScene().backgroundColor));
            descriptor = {};
            colorBytes[3] = 0xFF;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 1));
            phase = 1;
            cg::yobi::YobiClearUp_404D30(connection.Owner());
            break;
        }
        case 1:
            image = 22029;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 23, 30));
            phase = 5;
            break;
        case 2:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 3;
            }
            break;
        case 3: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 4000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                phase = 4;
            }
            break;
        }
        case 4:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                std::int32_t mapped;
                static_cast<void>(resource::GetGraphicMapNoIndex(238564, &mapped));
                if (mapped < 1) {
                    static_cast<void>(resource::GetGraphicMapNoIndex(22049, &mapped));
                }
                phase = 5;
            }
            break;
        case 5:
            image = 99103;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 23, 30));
            phase = 6;
            break;
        case 6:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 7;
            }
            break;
        case 7: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 4000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                phase = 8;
            }
            break;
        }
        case 8:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phase = 9;
            }
            break;
        case 9:
            image = 22027;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 34, 30));
            phase = 10;
            break;
        case 10:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 11;
            }
            break;
        case 11: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 4000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                phase = 12;
            }
            break;
        }
        case 12:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                std::int32_t mapped;
                static_cast<void>(resource::GetGraphicMapNoIndex(22049, &mapped));
                phase = mapped < 1 ? 13 : 17;
            }
            break;
        case 13:
            image = 22028;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 22, 30));
            phase = 14;
            break;
        case 14:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 15;
            }
            break;
        case 15: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 2000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                phase = 16;
            }
            break;
        }
        case 16:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phase = 26;
            }
            break;
        case 17:
            image = 22049;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 26, 60));
            phase = 18;
            break;
        case 18:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 19;
            }
            break;
        case 19: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 2000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 60));
                phase = 20;
            }
            break;
        }
        case 20:
            if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phase = 13;
            }
            break;
        case 21:
            image = 0;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 0, 1));
            static_cast<void>(render::ClearSurface_4512C0(connection.Owner().WorldScene().backgroundColor));
            phase = 22;
            if (resource::ProbePaletteCapability_45C970() < 0) {
                phase = 24;
            }
            break;
        case 22:
            image = 0;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 1));
            static_cast<void>(render::ClearSurface_4512C0(connection.Owner().WorldScene().backgroundColor));
            phase = 23;
            break;
        case 23:
            image = 0;
            keepRunning = false;
            break;
        case 24:
            phase = 25;
            login::ResetLoginMessage(connection);
            break;
        case 25:
            if (login::UpdateLoginMessage(connection, L"没有按照正常程序安装") == 1) {
                return 0;
            }
            break;
        case 26:
            image = 238564;
            if (render::HasLegacy3DDevice_44B610() != 0) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 0, 0));
                colorBytes[0] = 0;
                colorBytes[1] = 0;
                colorBytes[2] = 0;
            } else {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 0, 30));
            }
            phase = 27;
            break;
        case 27:
            if (render::HasLegacy3DDevice_44B610() != 0) {
                if (static_cast<int>(colorBytes[2]) + 4 <= 255) {
                    const std::uint8_t red = colorBytes[2];
                    const std::uint8_t green = colorBytes[1];
                    const std::uint8_t blue = colorBytes[0];
                    colorBytes[2] = static_cast<std::uint8_t>(red + 4);
                    colorBytes[1] = static_cast<std::uint8_t>(green + 4);
                    colorBytes[0] = static_cast<std::uint8_t>(blue + 4);
                } else {
                    colorBytes[2] = 0xFF;
                    colorBytes[1] = 0xFF;
                    colorBytes[0] = 0xFF;
                    phaseTick = ReadTickCount64();
                    phase = 28;
                }
            } else if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phaseTick = ReadTickCount64();
                phase = 28;
            }
            break;
        case 28: {
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 2000u < now) {
                if (render::HasLegacy3DDevice_44B610() == 0) {
                    static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                }
                phase = 29;
            }
            break;
        }
        case 29:
            if (render::HasLegacy3DDevice_44B610() != 0) {
                const std::uint8_t red = colorBytes[2];
                if (red != 0) {
                    if (red >= 4) {
                        const std::uint8_t green = colorBytes[1];
                        colorBytes[2] = static_cast<std::uint8_t>(red - 4);
                        const std::uint8_t blue = colorBytes[0];
                        colorBytes[1] = static_cast<std::uint8_t>(green - 4);
                        colorBytes[0] = static_cast<std::uint8_t>(blue - 4);
                    } else {
                        colorBytes[0] = 0;
                        colorBytes[1] = 0;
                        colorBytes[2] = 0;
                    }
                } else {
                    if (render::HasLegacy3DDevice_44B610() != 0) {
                        static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 0));
                    }
                    phase = 30;
                }
            } else if (render::ReadSessionPaletteChanging(connection.Owner()) == 0) {
                phase = 30;
            }
            break;
        case 30:
            connection.Owner().UiFrame().mouse.state->drawingSuppression = 1;
            image = 99304;
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 38, 60));
            phase = 31;
            break;
        case 31: {
            const std::int32_t changing = render::ReadSessionPaletteChanging(connection.Owner());
            connection.Owner().UiFrame().mouse.state->drawingSuppression = 1;
            if (changing == 0) {
                phaseTick = ReadTickCount64();
                phase = 32;
            }
            break;
        }
        case 32: {
            connection.Owner().UiFrame().mouse.state->drawingSuppression = 1;
            const std::uint64_t now = ReadTickCount64();
            if (phaseTick + 2000u < now) {
                static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 30));
                phase = 33;
            }
            break;
        }
        case 33: {
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 19, 0));
            std::int32_t mapped;
            static_cast<void>(resource::GetGraphicMapNoIndex(22049, &mapped));
            connection.Owner().UiFrame().mouse.state->drawingSuppression = 0;
            phase = mapped < 1 ? 13 : 17;
            break;
        }
        }

        static_cast<void>(cg::yobi::YobiUpdateAll_404C50(connection.Owner()));
        if (image != 0) {
            static_cast<void>(render::StretchGraphicToScreen(cg::render::DrawGraphic(
                320, 240, 255, image, 0,
                phase >= 26 && phase <= 29 ? &gLoadingDescriptor : nullptr)));
        }
        UpdateCursor_506E00(connection.Owner());
        render::SortTextures_520790();
        UpdateQueuedTextAndGraphicHover_507140(connection.Owner());
        static_cast<void>(render::UpdatePalette_450BB0());
        ClearKeyFlags_459760(connection.Owner());
        const double threshold = gFrameThreshold + gLastRenderDt;
        const std::uint64_t tick = ReadTickCount64();
        bool skipDrawing = false;
        if (static_cast<double>(tick) > threshold) {
            const std::uint32_t catchUpCount = gCatchUpCount;
            if (std::bit_cast<std::int32_t>(catchUpCount) < 20) {
                const double deadline = gFrameIncrement + gLastRenderDt;
                gCatchUpCount = catchUpCount + 1u;
                gLastRenderDt = deadline;
                skipDrawing = true;
            }
        }
        if (!skipDrawing) {
            if (render::HasLegacy3DDevice_44B610() == 0) {
                render::gPaletteFrameVersion = std::bit_cast<std::int32_t>(
                    std::bit_cast<std::uint32_t>(render::gPaletteFrameVersion) + 1u);
            }
            if (gWindowActiveState == 1) {
                gWindowActiveState = 2;
            }
            render::DrawAllQueued_520DD0(connection.Owner());
            if (gPendingPaletteReload == 1) {
                auto* palette = render::GetRendererPalette(render::gRenderer);
                static_cast<void>(render::SetPaletteEntries(
                    palette, 0, 0, 256, render::gPalette));
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
        if (!keepRunning) {
            return 1;
        }
    }
}

} // namespace cg::platform::initialization_detail
