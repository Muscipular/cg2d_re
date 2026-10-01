#include "cg/LoginInput.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginMessageDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "platform/WindowProcedureState.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace {

constexpr LoginSceneControl32 kLowFirstControl {
    338, 390, 288, 364, 100, 52, 0, 22003,
};
constexpr LoginSceneControl32 kLowSecondControl {
    506, 390, 456, 364, 100, 52, 0, 22004,
};
constexpr LoginSceneControl32 kHighFirstControl {
    320, 240, 71, 376, 90, 50, 0, 22238,
};
constexpr LoginSceneControl32 kHighSecondControl {
    320, 240, 195, 376, 90, 50, 0, 22239,
};

constexpr std::array<std::int32_t, 4> kBackgroundImages {
    22002, 22212, 22311, 22311,
};
constexpr char kVersion[] = "7.1.00";

// 原 sub_464E30 栈上按钮记录，固定 7×W32；末项只在点击后覆盖常态图号。
struct Capability4InputButton32 final {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t normalImageNo;
    std::int32_t activeImageNo;
    std::int32_t clickedImageNo;
};

static_assert(offsetof(Capability4InputButton32, x) == 0x00);
static_assert(offsetof(Capability4InputButton32, y) == 0x04);
static_assert(offsetof(Capability4InputButton32, width) == 0x08);
static_assert(offsetof(Capability4InputButton32, height) == 0x0C);
static_assert(offsetof(Capability4InputButton32, normalImageNo) == 0x10);
static_assert(offsetof(Capability4InputButton32, activeImageNo) == 0x14);
static_assert(offsetof(Capability4InputButton32, clickedImageNo) == 0x18);
static_assert(sizeof(Capability4InputButton32) == 0x1C);

constexpr Capability4InputButton32 kCapability4LoginButton {
    240, 421, 64, 16, 243048, 243050, 243049,
};
constexpr Capability4InputButton32 kCapability4ExitButton {
    332, 421, 64, 16, 245153, 245155, 245154,
};

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t SubtractW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        - std::bit_cast<std::uint32_t>(right));
}

std::uint16_t LowW16(const std::int32_t value) noexcept
{
    return static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value));
}

// 原 dword_55A048@0x55A048 与 dword_9A6E28@0x9A6E28；仅本函数引用。
std::int32_t gFocus55A048 = 1;
std::int32_t gPending9A6E28 = 0;

} // namespace

std::int32_t ProcessCapability4Input_464E30()
{
    Capability4InputButton32 loginButton = kCapability4LoginButton;
    Capability4InputButton32 exitButton = kCapability4ExitButton;
    std::int32_t result = 0;

    if ((detail::Sub_45F100(&loginButton) & 1) != 0) {
        loginButton.normalImageNo = loginButton.clickedImageNo;
        if (std::strlen(gInputContext9A5678.text) != 0
            && std::strlen(gInputContext9A5960.text) != 0) {
            result = 1;
            gInputContext9A5678.text[static_cast<std::size_t>(
                gInputContext9A5678.totalByteLength)] = '\0';
            gInputContext9A5960.text[static_cast<std::size_t>(
                gInputContext9A5960.totalByteLength)] = '\0';
        } else if (std::strlen(gInputContext9A5678.text) != 0
            || std::strlen(gInputContext9A5960.text) != 0) {
            result = -1;
        } else {
            result = -2;
        }
    } else if ((detail::Sub_45F100(&exitButton) & 1) != 0) {
        exitButton.normalImageNo = exitButton.clickedImageNo;
        result = 2;
    }

    const std::int32_t measureFontType =
        input::gCurrentInputData->field13C;
    const std::int32_t compositionWidth = detail::TextMeasure(
        platform::gImeComposition,
        measureFontType);
    input::InputContext* const drawContext = input::gCurrentInputData;
    const std::int32_t drawY = drawContext->y;
    const std::int32_t drawX = drawContext->x;
    const std::int32_t drawFontType = drawContext->field13C;
    const std::int32_t compositionRight =
        AddW32(drawX, compositionWidth);
    const std::int32_t compositionBottom = AddW32(
        drawY,
        render::gFontMetrics.data()[drawFontType].doubleHeight);
    detail::DrawGraphicHover(
        LowW16(SubtractW32(drawX, 1)),
        LowW16(SubtractW32(drawY, 1)),
        LowW16(compositionRight),
        LowW16(compositionBottom),
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
        const std::int32_t focus = gValue9A6E20;
        if (focus == 0) {
            if (platform::ReadKeyPressFlag_4597B0(13) != 0
                && std::strlen(gInputContext9A5960.text) != 0
                && std::strlen(gInputContext9A5678.text) != 0
                && !input::IsRejectedInputText_404060(
                    gInputContext9A5960.text)
                && !input::IsRejectedInputText_404060(
                    gInputContext9A5678.text)) {
                result = 1;
                gInputContext9A5960.text[static_cast<std::size_t>(
                    gInputContext9A5960.totalByteLength)] = '\0';
                gInputContext9A5678.text[static_cast<std::size_t>(
                    gInputContext9A5678.totalByteLength)] = '\0';
            } else {
                gValue9A6E20 = 1;
            }
        } else if (focus == 1) {
            // 已排除外部屏幕键盘，沿原关闭分支切换普通输入焦点。
            gValue9A6E20 = 0;
        }
    }

    if (platform::gMonitorKeyboard == 0) {
        if (static_cast<std::int32_t>(gInputContext9A5678X)
                < detail::ReadLegacyMouseX()
            && detail::ReadLegacyMouseX()
                < AddW32(
                    static_cast<std::int32_t>(gInputContext9A5678X),
                    372)
            && static_cast<std::int32_t>(gInputContext9A5678Y)
                < detail::ReadLegacyMouseY()
            && detail::ReadLegacyMouseY()
                < AddW32(
                    static_cast<std::int32_t>(gInputContext9A5678Y),
                    26)
            && (static_cast<std::uint8_t>(detail::gValueD68D9C)
                    & 1U)
                != 0) {
            static_cast<void>(detail::PlaySe(51, 320, 240));
            gValue9A6E20 = 1;
        }
        if (static_cast<std::int32_t>(gInputContext9A5960X)
                < detail::ReadLegacyMouseX()
            && detail::ReadLegacyMouseX()
                < AddW32(
                    static_cast<std::int32_t>(gInputContext9A5960X),
                    372)
            && static_cast<std::int32_t>(gInputContext9A5960Y)
                < detail::ReadLegacyMouseY()
            && detail::ReadLegacyMouseY()
                < AddW32(
                    static_cast<std::int32_t>(gInputContext9A5960Y),
                    26)
            && (static_cast<std::uint8_t>(detail::gValueD68D9C)
                    & 1U)
                != 0) {
            static_cast<void>(detail::PlaySe(51, 320, 240));
            gValue9A6E20 = 0;
        }

        input::SetContext_44B330(
            gInputContextFocusTable[gValue9A6E20]);

        const std::int32_t accountTop = SubtractW32(
            static_cast<std::int32_t>(gInputContext9A5678Y),
            4);
        const std::int32_t accountLeft = SubtractW32(
            static_cast<std::int32_t>(gInputContext9A5678X),
            5);
        static_cast<void>(detail::Sub_5049A0(
            accountLeft,
            accountTop,
            AddW32(accountLeft, 372),
            AddW32(accountTop, 26),
            206));

        const std::int32_t passwordTop = SubtractW32(
            static_cast<std::int32_t>(gInputContext9A5960Y),
            4);
        const std::int32_t passwordLeft = SubtractW32(
            static_cast<std::int32_t>(gInputContext9A5960X),
            5);
        static_cast<void>(detail::Sub_5049A0(
            passwordLeft,
            passwordTop,
            AddW32(passwordLeft, 372),
            AddW32(passwordTop, 26),
            206));
    }

    detail::Sub_45EFD0(&loginButton, 207, 0, 0, 0);
    detail::Sub_45EFD0(&exitButton, 207, 0, 0, 0);
    static_cast<void>(render::DrawText3_456560(
        460,
        462,
        0,
        render::gPaletteCapability >= 1 ? 3 : 0,
        kVersion,
        0));
    return result;
}

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
        if (std::strlen(gInputContext9A5960.text) == 0
            || std::strlen(gInputContext9A5678.text) == 0
            || input::IsRejectedInputText_404060(
                gInputContext9A5678.text)
            || input::IsRejectedInputText_404060(
                gInputContext9A5960.text)) {
            result = -1;
        } else {
            const std::int32_t firstLength =
                gInputContext9A5960.totalByteLength;
            const std::int32_t secondLength =
                gInputContext9A5678.totalByteLength;
            result = 1;
            gInputContext9A5960.text[
                static_cast<std::size_t>(firstLength)] = '\0';
            gInputContext9A5678.text[
                static_cast<std::size_t>(secondLength)] = '\0';
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
                && std::strlen(gInputContext9A5960.text) != 0
                && std::strlen(gInputContext9A5678.text) != 0
                && !input::IsRejectedInputText_404060(
                    gInputContext9A5960.text)
                && !input::IsRejectedInputText_404060(
                    gInputContext9A5678.text)) {
                const std::int32_t firstLength =
                    gInputContext9A5960.totalByteLength;
                const std::int32_t secondLength =
                    gInputContext9A5678.totalByteLength;
                result = 1;
                gInputContext9A5960.text[
                    static_cast<std::size_t>(firstLength)] = '\0';
                gInputContext9A5678.text[
                    static_cast<std::size_t>(secondLength)] = '\0';
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
            if (detail::Sub_5049A0(
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
            if (detail::Sub_5049A0(
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
            if (detail::Sub_5049A0(
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
            if (detail::Sub_5049A0(
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
    static_cast<void>(detail::DrawGraphic(
        320,
        240,
        3,
        kBackgroundImages[backgroundIndex],
        0,
        nullptr));
    static_cast<void>(detail::DrawText3(
        540, 424, 0, 0, kVersion, 0));
    return result;
}

} // namespace cg::login
