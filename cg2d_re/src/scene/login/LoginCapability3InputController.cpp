#include "cg/ui/HitTest.hpp"
#include "cg/LoginInput.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginMessageDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "../../platform/WindowProcedureState.hpp"

#include "cg/Palette.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace {

// 原 sub_45FA70 栈上按钮记录，固定 7×W32。
using Capability3InputButton32 = cg::ui::UiControlRecord;

constexpr Capability3InputButton32 kPrimaryButton {
    {240, 438, 64, 16}, 243048, 243050, 243049,
};
constexpr Capability3InputButton32 kSecondaryButton {
    {332, 438, 64, 16}, 245153, 245155, 245154,
};
// 原 dword_9A6EA0 与 dword_9A6E9C；仅 sub_45FA70 引用。
std::int32_t gFocus9A6EA0 = 0; // 原全局：0x009A6EA0。
std::int32_t gPending9A6E9C = 0; // 原全局：0x009A6E9C。

} // namespace

// 原函数：0x0045FA70。
std::int32_t ProcessCapability3Input_45FA70(const std::int32_t mode)
{
    Capability3InputButton32 primaryButton = kPrimaryButton;
    Capability3InputButton32 secondaryButton = kSecondaryButton;
    std::int32_t result = 0;
    const std::int32_t nonzeroMode = mode != 0 ? 1 : 0;

    if ((detail::QueryUiControlInputState_45F100(&primaryButton) & 1) != 0
        && mode == 0) {
        primaryButton.normalImageNo = primaryButton.clickedImageNo;
        if (gInputContext9A5678.length() == 0
            || gInputContext9A5960.length() == 0) {
            result = -1;
        } else {
            const std::int32_t accountByteLength =
                gInputContext9A5960.totalByteLength;
            const std::int32_t passwordByteLength =
                gInputContext9A5678.totalByteLength;
            result = 1;
            gInputContext9A5960.text.resize(cg::text::Prefix(gInputContext9A5960.text, static_cast<std::size_t>(
                accountByteLength)));
            gInputContext9A5678.text.resize(cg::text::Prefix(gInputContext9A5678.text, static_cast<std::size_t>(
                passwordByteLength)));
        }
    } else if ((detail::QueryUiControlInputState_45F100(&secondaryButton) & 1) != 0
        && mode == 0) {
        secondaryButton.normalImageNo = secondaryButton.clickedImageNo;
        result = 2;
    }

    if (mode < 2) {
        const std::int32_t measureFontType =
            input::gCurrentInputData->field13C;
        const std::int32_t compositionWidth = detail::TextMeasure(
            platform::gImeComposition,
            measureFontType);
        input::InputContext* const drawContext = input::gCurrentInputData;
        const std::int32_t drawY = drawContext->y;
        const std::int32_t drawX = drawContext->x;
        const std::int32_t drawFontType = drawContext->field13C;
        const std::int32_t compositionRight = drawX + compositionWidth;
        const std::int32_t compositionBottom = drawY
            + render::gFontMetrics.data()[drawFontType].doubleHeight;
        detail::DrawGraphicHover(
            static_cast<std::uint16_t>(drawX - 1),
            static_cast<std::uint16_t>(drawY - 1),
            static_cast<std::uint16_t>(compositionRight),
            static_cast<std::uint16_t>(compositionBottom),
            211,
            0,
            1);
        render::DrawInputText_4568A0(&gInputContext9A4D20);
        render::DrawInputText_4568A0(&gInputContext9A5960);
        render::DrawInputText_4568A0(&gInputContext9A5678);
    }

    std::int32_t requestedFocus = -1;
    bool keyTriggered = platform::ReadKeyPressFlag_4597B0(9) != 0;
    if (!keyTriggered) {
        keyTriggered = platform::ReadKeyPressFlag_4597B0(13) != 0;
    }
    if (keyTriggered) {
        const std::int32_t focus = gFocus9A6EA0;
        if (focus == 1) {
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
                requestedFocus = 0;
            }
        } else if (focus == 0) {
            // 已排除外部屏幕键盘，沿原关闭分支切换普通输入焦点。
            requestedFocus = 1;
        }
    }

    if (platform::gMonitorKeyboard == 0 && mode == 0) {
        const std::int32_t passwordY = gInputContext9A5678Y;
        const std::int32_t passwordX = gInputContext9A5678X;
        if (cg::ui::HitTestMouseRect_5049A0(
                passwordX - 5,
                passwordY - 4,
                passwordX + 227,
                passwordY + 22,
                206)
            != 0) {
            requestedFocus = 1;
        }

        const std::int32_t accountY = gInputContext9A5960Y;
        const std::int32_t accountX = gInputContext9A5960X;
        if (cg::ui::HitTestMouseRect_5049A0(
                accountX - 5,
                accountY - 4,
                accountX + 227,
                accountY + 22,
                206)
            != 0) {
            requestedFocus = 0;
        }
    }

    if (mode > 0) {
        input::SetContext_44B330(nullptr);
        gPending9A6E9C = 1;
    } else {
        std::int32_t selectedFocus = requestedFocus;
        bool applyFocus = requestedFocus >= 0 && requestedFocus <= 2;
        if (gPending9A6E9C != 0) {
            selectedFocus = gFocus9A6EA0;
            applyFocus = true;
        }

        if (applyFocus) {
            input::SetContext_44B330(
                gInputContextFocusTable[selectedFocus]);
            gPending9A6E9C = 0;
            gFocus9A6EA0 = selectedFocus;
        }
    }

    detail::DrawUiControl_45EFD0(
        &primaryButton, 207, 0, 0, nonzeroMode);
    detail::DrawUiControl_45EFD0(
        &secondaryButton, 207, 0, 0, nonzeroMode);
    static_cast<void>(render::DrawText3_456560(
        540,
        424,
        0,
        render::gPaletteCapability >= 1 ? 3 : 0,
        L"7.1.00",
        0));
    return result;
}

} // namespace cg::login
