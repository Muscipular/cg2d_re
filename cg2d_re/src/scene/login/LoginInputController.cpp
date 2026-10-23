#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginMessageDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "../../platform/WindowProcedureState.hpp"
#include "../../ui/UiControlRecord.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::login {

static constexpr LoginSceneControl32 kLowFirstControl {
    338, 390, 288, 364, 100, 52, 0, 22003,
};
static constexpr LoginSceneControl32 kLowSecondControl {
    506, 390, 456, 364, 100, 52, 0, 22004,
};
static constexpr LoginSceneControl32 kHighFirstControl {
    320, 240, 71, 376, 90, 50, 0, 22238,
};
static constexpr LoginSceneControl32 kHighSecondControl {
    320, 240, 195, 376, 90, 50, 0, 22239,
};

static constexpr std::array<std::int32_t, 4> kBackgroundImages {
    22002, 22212, 22311, 22311,
};

// 原 sub_464E30 栈上按钮记录，固定 7×W32；末项只在点击后覆盖常态图号。
using Capability4InputButton32 = cg::ui::UiControlRecord;

static constexpr Capability4InputButton32 kCapability4LoginButton {
    {240, 421, 64, 16}, 243048, 243050, 243049,
};
static constexpr Capability4InputButton32 kCapability4ExitButton {
    {332, 421, 64, 16}, 245153, 245155, 245154,
};

// 原 dword_55A048@0x55A048 与 dword_9A6E28@0x9A6E28；仅本函数引用。
static std::int32_t gFocus55A048 = 1; // 原全局：0x0055A048。
static std::int32_t gPending9A6E28 = 0; // 原全局：0x009A6E28。

// 原函数：0x00464E30。
std::int32_t ProcessCapability4Input_464E30()
{
    Capability4InputButton32 loginButton = kCapability4LoginButton;
    Capability4InputButton32 exitButton = kCapability4ExitButton;
    std::int32_t result = 0;

    if ((cg::ui::QueryUiControlInputState_45F100(&loginButton) & 1) != 0) {
        loginButton.normalImageNo = loginButton.clickedImageNo;
        if (gInputContext9A5678.length() != 0
            && gInputContext9A5960.length() != 0) {
            const std::int32_t passwordByteLength =
                gInputContext9A5678.totalByteLength;
            const std::int32_t accountByteLength =
                gInputContext9A5960.totalByteLength;
            result = 1;
            gInputContext9A5678.text.resize(cg::text::Prefix(gInputContext9A5678.text, static_cast<std::size_t>(
                passwordByteLength)));
            gInputContext9A5960.text.resize(cg::text::Prefix(gInputContext9A5960.text, static_cast<std::size_t>(
                accountByteLength)));
        } else if (gInputContext9A5678.length() != 0
            || gInputContext9A5960.length() != 0) {
            result = -1;
        } else {
            result = -2;
        }
    } else if ((cg::ui::QueryUiControlInputState_45F100(&exitButton) & 1) != 0) {
        exitButton.normalImageNo = exitButton.clickedImageNo;
        result = 2;
    }

    const std::int32_t measureFontType =
        input::gCurrentInputData->fontType;
    const std::int32_t compositionWidth = cg::render::TextMeasure(
        platform::gImeComposition,
        measureFontType);
    input::InputContext* const drawContext = input::gCurrentInputData;
    const std::int32_t drawY = drawContext->y;
    const std::int32_t drawX = drawContext->x;
    const std::int32_t drawFontType = drawContext->fontType;
    const std::int32_t compositionRight =
        drawX + compositionWidth;
    const std::int32_t compositionBottom = drawY
        + render::GetFontMetrics(drawFontType).doubleHeight;
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(drawX - 1),
        static_cast<std::uint16_t>(drawY - 1),
        static_cast<std::uint16_t>(compositionRight),
        static_cast<std::uint16_t>(compositionBottom),
        211,
        0,
        1);
    render::DrawInputText_4568A0(&gInputContext9A5960);
    render::DrawInputText_4568A0(&gInputContext9A5678);

    bool keyTriggered = platform::ReadKeyPressFlag_4597B0(9) != 0;
    if (!keyTriggered) {
        keyTriggered = platform::ReadKeyPressFlag_4597B0(13) != 0;
    }
    if (keyTriggered) {
        const std::int32_t focus = gLoginInputFocusIndex_9A6E20;
        if (focus == 0) {
            if (platform::ReadKeyPressFlag_4597B0(13) != 0
                && gInputContext9A5960.length() != 0
                && gInputContext9A5678.length() != 0
                && !input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5960))
                && !input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5678))) {
                const std::int32_t accountByteLength =
                    gInputContext9A5960.totalByteLength;
                const std::int32_t passwordByteLength =
                    gInputContext9A5678.totalByteLength;
                result = 1;
                gInputContext9A5960.text.resize(cg::text::Prefix(gInputContext9A5960.text, static_cast<std::size_t>(
                    accountByteLength)));
                gInputContext9A5678.text.resize(cg::text::Prefix(gInputContext9A5678.text, static_cast<std::size_t>(
                    passwordByteLength)));
            } else {
                gLoginInputFocusIndex_9A6E20 = 1;
            }
        } else if (focus == 1) {
            // 已排除外部屏幕键盘，沿原关闭分支切换普通输入焦点。
            gLoginInputFocusIndex_9A6E20 = 0;
        }
    }

    if (platform::gMonitorKeyboard == 0) {
        const std::int32_t offsetX = ui::LegacyUiLayoutScope::GetOffsetX();
        const std::int32_t offsetY = ui::LegacyUiLayoutScope::GetOffsetY();
        if ((static_cast<std::int32_t>(gInputContext9A5678X) + offsetX
                < platform::gLegacyMouseState.mouseX)
            && (platform::gLegacyMouseState.mouseX < static_cast<std::int32_t>(gInputContext9A5678X) + offsetX + 372)
            && (static_cast<std::int32_t>(gInputContext9A5678Y) + offsetY
                < platform::gLegacyMouseState.mouseY)
            && (platform::gLegacyMouseState.mouseY < static_cast<std::int32_t>(gInputContext9A5678Y) + offsetY + 26)
            && (static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons)
                    & 1U)
                != 0) {
            static_cast<void>(detail::PlaySe(51, 320, 240));
            gLoginInputFocusIndex_9A6E20 = 1;
        }
        if ((static_cast<std::int32_t>(gInputContext9A5960X) + offsetX
                < platform::gLegacyMouseState.mouseX)
            && (platform::gLegacyMouseState.mouseX < static_cast<std::int32_t>(gInputContext9A5960X) + offsetX + 372)
            && (static_cast<std::int32_t>(gInputContext9A5960Y) + offsetY
                < platform::gLegacyMouseState.mouseY)
            && (platform::gLegacyMouseState.mouseY < static_cast<std::int32_t>(gInputContext9A5960Y) + offsetY + 26)
            && (static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons)
                    & 1U)
                != 0) {
            static_cast<void>(detail::PlaySe(51, 320, 240));
            gLoginInputFocusIndex_9A6E20 = 0;
        }

        input::SetContext_44B330(
            gInputContextFocusTable[gLoginInputFocusIndex_9A6E20]);

        const std::int32_t accountTop = static_cast<std::int32_t>(gInputContext9A5678Y) - 4;
        const std::int32_t accountLeft = static_cast<std::int32_t>(gInputContext9A5678X) - 5;
        static_cast<void>(cg::ui::HitTestMouseRect_5049A0(
            accountLeft,
            accountTop,
            accountLeft + 372,
            accountTop + 26,
            206));

        const std::int32_t passwordTop = static_cast<std::int32_t>(gInputContext9A5960Y) - 4;
        const std::int32_t passwordLeft = static_cast<std::int32_t>(gInputContext9A5960X) - 5;
        static_cast<void>(cg::ui::HitTestMouseRect_5049A0(
            passwordLeft,
            passwordTop,
            passwordLeft + 372,
            passwordTop + 26,
            206));
    }

    cg::ui::DrawUiControl_45EFD0(&loginButton, 207, 0, 0, 0);
    cg::ui::DrawUiControl_45EFD0(&exitButton, 207, 0, 0, 0);
    static_cast<void>(DrawClientVersion(460, 462, render::gPaletteCapability >= 1 ? 3 : 0));
    return result;
}

// 原函数：0x0045CA20。
std::int32_t ProcessLoginInput_45CA20(const std::int32_t mode)
{
    std::int32_t result = 0;

    const std::int32_t probedCapability =
        resource::ProbePaletteCapability_45C970();
    const bool useHighControls = probedCapability >= 1;
    render::gPaletteCapability = probedCapability;

    const LoginSceneControl32* const firstControl =
        useHighControls ? &kHighFirstControl : &kLowFirstControl;
    const LoginSceneControl32* const secondControl =
        useHighControls ? &kHighSecondControl : &kLowSecondControl;
    const std::int32_t controlRenderMode = mode != 0 ? 1 : 0;

    const std::int32_t firstControlState =
        QueryLoginControlState_5036F0(firstControl);
    if ((firstControlState & 1) != 0 && mode == 0) {
        if (gInputContext9A5960.length() == 0
            || gInputContext9A5678.length() == 0
            || input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5678))
            || input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5960))) {
            result = -1;
        } else {
            const std::int32_t firstLength =
                gInputContext9A5960.totalByteLength;
            const std::int32_t secondLength =
                gInputContext9A5678.totalByteLength;
            result = 1;
            gInputContext9A5960.text.resize(cg::text::Prefix(gInputContext9A5960.text, static_cast<std::size_t>(firstLength)));
            gInputContext9A5678.text.resize(cg::text::Prefix(gInputContext9A5678.text, static_cast<std::size_t>(secondLength)));
        }
    } else {
        const std::int32_t secondControlState =
            QueryLoginControlState_5036F0(secondControl);
        if ((secondControlState & 1) != 0 && mode == 0) {
            result = 2;
        }
    }

    if (mode < 2) {
        render::DrawInputText_4568A0(&gInputContext9A5960);
        render::DrawInputText_4568A0(&gInputContext9A5678);
    }

    std::int32_t candidate = -1;
    bool keyTriggered = platform::ReadKeyPressFlag_4597B0(9) != 0;
    if (!keyTriggered) {
        keyTriggered = platform::ReadKeyPressFlag_4597B0(13) != 0;
    }
    if (keyTriggered) {
        const std::int32_t rawFocus = gFocus55A048;
        if (rawFocus == 0) {
            if (platform::ReadKeyPressFlag_4597B0(13) != 0
                && gInputContext9A5960.length() != 0
                && gInputContext9A5678.length() != 0
                && !input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5960))
                && !input::IsRejectedInputText_404060(input::ContextText(gInputContext9A5678))) {
                const std::int32_t firstLength =
                    gInputContext9A5960.totalByteLength;
                const std::int32_t secondLength =
                    gInputContext9A5678.totalByteLength;
                result = 1;
                gInputContext9A5960.text.resize(cg::text::Prefix(gInputContext9A5960.text, static_cast<std::size_t>(firstLength)));
                gInputContext9A5678.text.resize(cg::text::Prefix(gInputContext9A5678.text, static_cast<std::size_t>(secondLength)));
            } else {
                candidate = 1;
            }
        } else if (rawFocus == 1) {
            candidate = 0;
        }
    }

    if (mode == 0) {
        const std::int32_t rectangleCapability =
            render::gPaletteCapability;
        const std::int32_t firstX = gInputContext9A5960X;
        if (rectangleCapability >= 1) {
            const std::int32_t firstY = gInputContext9A5960Y;
            if (cg::ui::HitTestMouseRect_5049A0(
                    firstX - 5,
                    firstY - 5,
                    firstX + 222,
                    firstY + 21,
                    206)
                != 0) {
                candidate = 0;
            }

            const std::int32_t secondY = gInputContext9A5678Y;
            const std::int32_t secondX = gInputContext9A5678X;
            if (cg::ui::HitTestMouseRect_5049A0(
                    secondX - 5,
                    secondY - 5,
                    secondX + 222,
                    secondY + 21,
                    206)
                != 0) {
                candidate = 1;
            }
        } else {
            const std::int32_t firstY = gInputContext9A5960Y;
            if (cg::ui::HitTestMouseRect_5049A0(
                    firstX - 5,
                    firstY - 5,
                    firstX + 268,
                    firstY + 25,
                    206)
                != 0) {
                candidate = 0;
            }

            const std::int32_t secondY = gInputContext9A5678Y;
            const std::int32_t secondX = gInputContext9A5678X;
            if (cg::ui::HitTestMouseRect_5049A0(
                    secondX - 5,
                    secondY - 5,
                    secondX + 268,
                    secondY + 25,
                    206)
                != 0) {
                candidate = 1;
            }
        }
    }

    if (mode > 0) {
        input::SetContext_44B330(nullptr);
        gPending9A6E28 = 1;
    } else {
        std::int32_t selectedFocus = candidate;
        bool applyFocus = candidate >= 0 && candidate <= 1;
        if (gPending9A6E28 != 0) {
            selectedFocus = gFocus55A048;
            applyFocus = true;
        }

        if (applyFocus) {
            const std::size_t focusIndex =
                selectedFocus >= 0 && selectedFocus < 3
                ? static_cast<std::size_t>(selectedFocus)
                : std::size_t{0};
            input::SetContext_44B330(
                gInputContextFocusTable[focusIndex]);
            gPending9A6E28 = 0;
            gFocus55A048 = selectedFocus;
        }
    }

    DrawLoginControl_503780(
        firstControl, 4, 0, 0, controlRenderMode);
    DrawLoginControl_503780(
        secondControl, 4, 0, 0, controlRenderMode);

    const std::int32_t rawBackgroundIndex =
        render::gPaletteCapability;
    const std::size_t backgroundIndex =
        rawBackgroundIndex >= 0
            && rawBackgroundIndex
                < static_cast<std::int32_t>(kBackgroundImages.size())
        ? static_cast<std::size_t>(rawBackgroundIndex)
        : std::size_t{0};
    static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::DrawGraphic(
        320,
        240,
        3,
        kBackgroundImages[backgroundIndex],
        0,
        nullptr)));
    static_cast<void>(DrawClientVersion(540, 424, 0));
    return result;
}

} // namespace cg::login
