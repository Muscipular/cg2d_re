#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

#include <Windows.h>

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::world {

static constexpr std::int32_t kWhiteColor = 0x00FFFFFF;
static constexpr double kFrameGateIncrement = 1000.0 / 60.0;
// sub_45F190 的七个唯一私有 owner；原映像均为 virtual-only 零填充。
static std::int32_t gPhaseValue9A5550 = 0; // 原全局：0x009A5550。
static std::int32_t gAlpha9A4D08 = 0; // 原全局：0x009A4D08。
static std::int32_t gAlpha9A4CA0 = 0; // 原全局：0x009A4CA0。
static std::int32_t gAlpha9A5E70 = 0; // 原全局：0x009A5E70。
static cg::yobi::YobiAction* gLeftAction9A50C0 = nullptr; // 原全局：0x009A50C0。
static cg::yobi::YobiAction* gRightAction9A50C4 = nullptr; // 原全局：0x009A50C4。
static std::wstring gMessage9A50E8 {}; // 原全局：0x009A50E8。

static std::int32_t ComposeWhiteAlpha(const std::int32_t alpha) noexcept
{
    const std::uint32_t bits =
        (std::bit_cast<std::uint32_t>(alpha) << 24U) | 0x00FFFFFFU;
    return std::bit_cast<std::int32_t>(bits);
}

// 原函数：0x0045EF80。
static bool HasLoginIntroFrameDeadlineElapsed_45EF80()
{
    const double deadline = cg::platform::gLastRenderDt + kFrameGateIncrement;
    const std::uint64_t tick = cg::platform::ReadTickCount64();
    return tick > deadline;
}

static void DrawFiveCapability3Images()
{
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(638, 478, 3, 245152, 0, -1));
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(638, 478, 3, 245150, 0, -1));
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(638, 478, 3, 245156, 0, -1));
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(638, 478, 3, 245157, 0, -1));
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(638, 478, 3, 245151, 0, -1));
}

static std::int32_t DrawCapability3CommonTail()
{
    const std::uint32_t firstAlpha = cg::login::gCapability3LoginOverlayAlpha_9A6200;
    if (firstAlpha != 0U) {
        static_cast<void>(cg::render::QueueOriginalPaletteGraphic_45F090(
            139,
            350,
            206,
            245152,
            0,
            ComposeWhiteAlpha(std::bit_cast<std::int32_t>(firstAlpha))));
    }

    const std::int32_t secondAlpha = gAlpha9A5E70;
    if (secondAlpha != 0) {
        static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(
            0,
            0,
            206,
            245157,
            0,
            ComposeWhiteAlpha(secondAlpha))));
    }

    const std::int32_t thirdAlpha = gAlpha9A4CA0;
    if (thirdAlpha != 0) {
        static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(
            0,
            0,
            206,
            245156,
            0,
            ComposeWhiteAlpha(thirdAlpha))));
    }

    const std::int32_t fourthAlpha = gAlpha9A4D08;
    if (fourthAlpha != 0) {
        return cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(
            0,
            0,
            3,
            245151,
            0,
            ComposeWhiteAlpha(fourthAlpha)));
    }
    return fourthAlpha;
}

static std::int32_t ProcessCapability3InputAndDraw()
{
    const std::int32_t result = cg::login::ProcessCapability3Input_45FA70(0);
    if (result == 1) {
        cg::core::NullSub_509080();
        if (!cg::login::SaveInputValues(
                cg::input::ContextText(cg::login::gInputContext9A5678),
                cg::input::ContextText(cg::login::gInputContext9A5960))) {
            return DrawCapability3CommonTail();
        }
        static_cast<void>(cg::login::detail::SaveCurrentLoginRecord_520200());
        static_cast<void>(cg::login::detail::PlaySe(53, 320, 240));
        static_cast<void>(cg::render::SelectPalette(30, 30));
        cg::world::gCurrentSceneState = 10;
    } else if (result == 2) {
        static_cast<void>(cg::login::detail::PlaySe(54, 320, 240));
        static_cast<void>(::PostMessageW(
            reinterpret_cast<HWND>(cg::platform::gLegacyWindow),
            0x10,
            0,
            0));
    } else if (result < 0) {
        cg::world::gCurrentSceneState = 8;
        gMessage9A50E8 = L"请设定ID、PASSWORD。" /* 原55CA8C「請設定ID、PASSWORD。」；20B正文，字面量隐式追加NUL。 */;
        cg::login::gInputContext9A5960.clearText();
        static_cast<void>(cg::login::WriteInputValueFF494F_5203B0(cg::input::ContextText(cg::login::gInputContext9A5960).c_str()));
    }
    return DrawCapability3CommonTail();
}

static std::int32_t ProcessCapability3CommonDispatch(
    const std::int32_t sampledState)
{
    if (std::bit_cast<std::uint32_t>(sampledState) < 4U) {
        const std::uint8_t clickState =
            static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
        if ((clickState & 1U) != 0U) {
            cg::world::gCurrentSceneState = 5;
            cg::input::SetContext_44B330(nullptr);
            return DrawCapability3CommonTail();
        }
    }

    if (sampledState == 6) {
        return ProcessCapability3InputAndDraw();
    }

    cg::input::SetContext_44B330(nullptr);
    return DrawCapability3CommonTail();
}

static std::int32_t DispatchCapability3FromCurrentState()
{
    const std::int32_t sampledState = cg::world::gCurrentSceneState;
    return ProcessCapability3CommonDispatch(sampledState);
}

// 原函数：0x0045F190。
std::int32_t UpdateCapability3LoginScene_45F190()
{
    const std::uint32_t state =
        std::bit_cast<std::uint32_t>(world::gCurrentSceneState);
    if (state > 10U) {
        return DispatchCapability3FromCurrentState();
    }

    switch (state) {
    case 0:
        cg::yobi::YobiClearUp_404D30();
        gPhaseValue9A5550 = 0;
        gAlpha9A4D08 = 0;
        gAlpha9A4CA0 = 0;
        gAlpha9A5E70 = 0;
        login::gCapability3LoginOverlayAlpha_9A6200 = 0;
        DrawFiveCapability3Images();
        world::gCurrentSceneState = 1;
        return ProcessCapability3CommonDispatch(1);

    case 1: {
        DrawFiveCapability3Images();
        if (HasLoginIntroFrameDeadlineElapsed_45EF80()) {
            return DispatchCapability3FromCurrentState();
        }

        const std::int32_t fade = gPhaseValue9A5550 + 4;
        gPhaseValue9A5550 = fade;
        const std::uint32_t fadeBits = std::bit_cast<std::uint32_t>(fade);
        render::gBackgroundColor =
            fadeBits | ((fadeBits | (fadeBits << 8U)) << 8U);
        if (fade < 255) {
            return DispatchCapability3FromCurrentState();
        }

        gPhaseValue9A5550 = 0;
        render::gBackgroundColor = 0x00FFFFFFU;
        world::gCurrentSceneState = 2;
        return ProcessCapability3CommonDispatch(2);
    }

    case 2: {
        const std::int32_t phase = gPhaseValue9A5550 + 1;
        gPhaseValue9A5550 = phase;
        const std::int32_t numerator =
            (phase << 8U) - phase;
        gAlpha9A4D08 = numerator / 40;
        if (phase < 40) {
            return DispatchCapability3FromCurrentState();
        }

        gPhaseValue9A5550 = 0;
        gAlpha9A4D08 = 255;

        gLeftAction9A50C0 = cg::yobi::NewYobiAction_404AE0(2, 0);
        if (gLeftAction9A50C0 != nullptr) {
            cg::yobi::SetYobiActionField2A8(
                gLeftAction9A50C0,
                245150);
            cg::yobi::SetYobiActionField2B0(gLeftAction9A50C0, 0);
            cg::yobi::SetYobiActionField15(gLeftAction9A50C0, 10);
            cg::yobi::SetYobiActionField30(gLeftAction9A50C0, 451);
            cg::yobi::SetYobiActionField34(gLeftAction9A50C0, 193);
            cg::yobi::SetYobiActionField2B8(gLeftAction9A50C0, cg::yobi::YobiDirection_UpLeft);
            cg::yobi::SetYobiActionField308(gLeftAction9A50C0, static_cast<std::int32_t>(cg::yobi::YobiColorChannelFlag_Alpha));
            cg::yobi::SetYobiActionField30C(
                gLeftAction9A50C0,
                kWhiteColor);
            static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(
                gLeftAction9A50C0,
                100,
                0,
                0));
        }

        gRightAction9A50C4 = cg::yobi::NewYobiAction_404AE0(2, 0);
        if (gRightAction9A50C4 == nullptr) {
            world::gCurrentSceneState = 6;
            return ProcessCapability3InputAndDraw();
        }

        cg::yobi::SetYobiActionField2A8(
            gRightAction9A50C4,
            245150);
        cg::yobi::SetYobiActionField2B0(gRightAction9A50C4, 0);
        cg::yobi::SetYobiActionField15(gRightAction9A50C4, 10);
        cg::yobi::SetYobiActionField30(gRightAction9A50C4, 189);
        cg::yobi::SetYobiActionField34(gRightAction9A50C4, 287);
        cg::yobi::SetYobiActionField2B8(gRightAction9A50C4, cg::yobi::YobiDirection_UpLeft);
        cg::yobi::SetYobiActionField308(gRightAction9A50C4, static_cast<std::int32_t>(cg::yobi::YobiColorChannelFlag_Alpha));
        cg::yobi::SetYobiActionField30C(
            gRightAction9A50C4,
            kWhiteColor);
        static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(
            gRightAction9A50C4,
            100,
            0,
            0));
        world::gCurrentSceneState = 3;
        return ProcessCapability3CommonDispatch(3);
    }

    case 3: {
        cg::yobi::YobiAction* const leftForX = gLeftAction9A50C0;
        cg::yobi::SetYobiActionField30(
            leftForX,
            cg::yobi::GetYobiActionField30(leftForX) - 3);

        cg::yobi::YobiAction* const leftForY = gLeftAction9A50C0;
        cg::yobi::SetYobiActionField34(
            leftForY,
            cg::yobi::GetYobiActionField34(leftForY) + 1);

        cg::yobi::YobiAction* const rightForX = gRightAction9A50C4;
        cg::yobi::SetYobiActionField30(
            rightForX,
            cg::yobi::GetYobiActionField30(rightForX) + 3);

        cg::yobi::YobiAction* const rightForY = gRightAction9A50C4;
        cg::yobi::SetYobiActionField34(
            rightForY,
            cg::yobi::GetYobiActionField34(rightForY) - 1);

        cg::yobi::YobiAction* const rightForDelta = gRightAction9A50C4;
        const std::int32_t delta = 287 - cg::yobi::GetYobiActionField34(rightForDelta);
        gPhaseValue9A5550 = delta;
        if (delta >= 0) {
            const std::int32_t alpha = (delta << 7U) / 47;
            cg::yobi::SetYobiActionField30F(
                gLeftAction9A50C0,
                static_cast<std::uint8_t>(alpha));
            cg::yobi::YobiAction* const leftForAlphaCopy = gLeftAction9A50C0;
            cg::yobi::YobiAction* const rightForAlphaCopy = gRightAction9A50C4;
            const std::uint8_t copiedAlpha =
                cg::yobi::GetYobiActionField30F(leftForAlphaCopy);
            cg::yobi::SetYobiActionField30F(
                rightForAlphaCopy,
                copiedAlpha);
        }

        cg::yobi::YobiAction* const leftForCompare = gLeftAction9A50C0;
        if (cg::yobi::GetYobiActionField30(leftForCompare) > 320) {
            return DispatchCapability3FromCurrentState();
        }

        cg::yobi::SetYobiActionField30(leftForCompare, 320);
        cg::yobi::SetYobiActionField34(gLeftAction9A50C0, 240);
        cg::yobi::SetYobiActionField30F(gLeftAction9A50C0, 0);
        cg::yobi::MarkDeath(gRightAction9A50C4);
        gRightAction9A50C4 = nullptr;
        gAlpha9A4D08 = 0;
        gAlpha9A4CA0 = 0;
        gAlpha9A5E70 = 0;
        world::gCurrentSceneState = 4;
        gPhaseValue9A5550 = 16;
        static_cast<void>(cg::audio::LoadAndPlayBGM(33));
        return DispatchCapability3FromCurrentState();
    }

    case 4:
        if (gPhaseValue9A5550 > 0) {
            gPhaseValue9A5550 = gPhaseValue9A5550 - 1;
            if (gPhaseValue9A5550 == 0) {
                cg::yobi::YobiAction* const action = gLeftAction9A50C0;
                if (action != nullptr) {
                    cg::yobi::SetYobiActionField30F(action, 0xFF);
                    gAlpha9A4D08 = 255;
                }
            }
            return DispatchCapability3FromCurrentState();
        }

        if (gAlpha9A4CA0 < 255) {
            gAlpha9A4CA0 = gAlpha9A4CA0 + 4;
            if (gAlpha9A4CA0 >= 255) {
                cg::yobi::YobiAction* const action = gLeftAction9A50C0;
                gAlpha9A4CA0 = 255;
                gPhaseValue9A5550 = 60;
                cg::yobi::MarkDeath(action);
                gLeftAction9A50C0 = nullptr;
                render::gBackgroundColor = 0;
                gAlpha9A4D08 = 0;
            }
            return DispatchCapability3FromCurrentState();
        }

        gAlpha9A5E70 = gAlpha9A5E70 + 4;
        if (gAlpha9A5E70 <= 255) {
            return DispatchCapability3FromCurrentState();
        }
        gAlpha9A4CA0 = 0;
        gAlpha9A5E70 = 255;
        login::InitializeCapabilityInput_45F970();
        world::gCurrentSceneState = 6;
        return ProcessCapability3InputAndDraw();

    case 5: {
        cg::yobi::YobiAction* const leftAction = gLeftAction9A50C0;
        gAlpha9A4D08 = 0;
        gAlpha9A4CA0 = 0;
        gAlpha9A5E70 = 255;
        render::gBackgroundColor = 0;
        cg::yobi::MarkDeath(leftAction);
        cg::yobi::YobiAction* const rightAction = gRightAction9A50C4;
        cg::yobi::MarkDeath(rightAction);
        gRightAction9A50C4 = nullptr;
        gLeftAction9A50C0 = nullptr;
        login::InitializeCapabilityInput_45F970();
        world::gCurrentSceneState = 6;
        static_cast<void>(cg::audio::LoadAndPlayBGM(33));
        return DispatchCapability3FromCurrentState();
    }

    case 6:
        return DispatchCapability3FromCurrentState();

    case 7:
        if (render::gPaletteChanging != 0) {
            return DispatchCapability3FromCurrentState();
        }
        static_cast<void>(render::SelectPalette(19, 60));
        world::gCurrentSceneState = 10;
        input::SetContext_44B330(nullptr);
        return DrawCapability3CommonTail();

    case 8:
        login::ResetLoginMessage();
        static_cast<void>(login::detail::PlaySe(56, 320, 240));
        world::gCurrentSceneState = 9;
        input::SetContext_44B330(nullptr);
        return DrawCapability3CommonTail();

    case 9:
        if (login::UpdateLoginMessage(gMessage9A50E8) != 0) {
            world::gCurrentSceneState = 5;
            input::SetContext_44B330(nullptr);
            return DrawCapability3CommonTail();
        }
        return DispatchCapability3FromCurrentState();

    case 10:
        static_cast<void>(world::SetCurrentSceneType(2));
        cg::yobi::YobiClearUp_404D30();
        render::gBackgroundColor = 0;
        return DispatchCapability3FromCurrentState();
    }

    return DispatchCapability3FromCurrentState();
}

} // namespace cg::world
