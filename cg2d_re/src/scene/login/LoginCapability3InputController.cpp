#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/network/CgConnection.hpp"
#include "LoginSceneState.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginMessageDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "../../platform/WindowProcedureState.hpp"

#include "cg/render/Palette.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::login {

// 原 sub_45FA70 栈上按钮记录，固定 7×W32。
using Capability3InputButton32 = cg::ui::UiControlRecord;

static constexpr Capability3InputButton32 kPrimaryButton {
    {240, 438, 64, 16}, 243048, 243050, 243049,
};
static constexpr Capability3InputButton32 kSecondaryButton {
    {332, 438, 64, 16}, 245153, 245155, 245154,
};
// 原函数：0x0045FA70。
std::int32_t ProcessCapability3Input_45FA70(network::CgConnection& connection, const std::int32_t mode)
{
    auto& sceneUi = connection.LoginSceneUi();
    Capability3InputButton32 primaryButton = kPrimaryButton;
    Capability3InputButton32 secondaryButton = kSecondaryButton;
    std::int32_t result = 0;
    const std::int32_t nonzeroMode = mode != 0 ? 1 : 0;

    if ((cg::ui::QueryUiControlInputState_45F100(connection.Owner(), &primaryButton) & 1) != 0
        && mode == 0) {
        primaryButton.normalImageNo = primaryButton.clickedImageNo;
        if (sceneUi.context9A5678.length() == 0
            || sceneUi.context9A5960.length() == 0) {
            result = -1;
        } else {
            const std::int32_t accountByteLength =
                sceneUi.context9A5960.totalByteLength;
            const std::int32_t passwordByteLength =
                sceneUi.context9A5678.totalByteLength;
            result = 1;
            sceneUi.context9A5960.text.resize(cg::text::Prefix(sceneUi.context9A5960.text, static_cast<std::size_t>(
                accountByteLength)));
            sceneUi.context9A5678.text.resize(cg::text::Prefix(sceneUi.context9A5678.text, static_cast<std::size_t>(
                passwordByteLength)));
        }
    } else if ((cg::ui::QueryUiControlInputState_45F100(connection.Owner(), &secondaryButton) & 1) != 0
        && mode == 0) {
        secondaryButton.normalImageNo = secondaryButton.clickedImageNo;
        result = 2;
    }

    if (mode < 2) {
        const std::int32_t measureFontType =
            connection.Owner().UiFrame().inputState.currentContext->fontType;
        const std::int32_t compositionWidth = cg::render::TextMeasure(
            connection.Owner().UiFrame().ime.composition,
            measureFontType);
        input::InputContext* const drawContext = connection.Owner().UiFrame().inputState.currentContext;
        const std::int32_t drawY = drawContext->y;
        const std::int32_t drawX = drawContext->x;
        const std::int32_t drawFontType = drawContext->fontType;
        const std::int32_t compositionRight = drawX + compositionWidth;
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
        render::DrawInputText_4568A0(connection.Owner(), &sceneUi.context9A4D20);
        render::DrawInputText_4568A0(connection.Owner(), &sceneUi.context9A5960);
        render::DrawInputText_4568A0(connection.Owner(), &sceneUi.context9A5678);
    }

    std::int32_t requestedFocus = -1;
    bool keyTriggered = platform::ReadKeyPressFlag_4597B0(connection.Owner(), 9) != 0;
    if (!keyTriggered) {
        keyTriggered = platform::ReadKeyPressFlag_4597B0(connection.Owner(), 13) != 0;
    }
    if (keyTriggered) {
        const std::int32_t focus = sceneUi.capability3InputFocus;
        if (focus == 1) {
            if (platform::ReadKeyPressFlag_4597B0(connection.Owner(), 13) != 0
                && sceneUi.context9A5960.length() != 0
                && sceneUi.context9A5678.length() != 0
                && !input::IsRejectedInputText_404060(input::ContextText(sceneUi.context9A5960))
                && !input::IsRejectedInputText_404060(input::ContextText(sceneUi.context9A5678))) {
                const std::int32_t accountByteLength =
                    sceneUi.context9A5960.totalByteLength;
                const std::int32_t passwordByteLength =
                    sceneUi.context9A5678.totalByteLength;
                result = 1;
                sceneUi.context9A5960.text.resize(cg::text::Prefix(sceneUi.context9A5960.text, static_cast<std::size_t>(
                    accountByteLength)));
                sceneUi.context9A5678.text.resize(cg::text::Prefix(sceneUi.context9A5678.text, static_cast<std::size_t>(
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
        const std::int32_t passwordY = sceneUi.context9A5678Y;
        const std::int32_t passwordX = sceneUi.context9A5678X;
        if (cg::ui::HitTestMouseRect_5049A0(connection.Owner(),
                passwordX - 5,
                passwordY - 4,
                passwordX + 227,
                passwordY + 22,
                206)
            != 0) {
            requestedFocus = 1;
        }

        const std::int32_t accountY = sceneUi.context9A5960Y;
        const std::int32_t accountX = sceneUi.context9A5960X;
        if (cg::ui::HitTestMouseRect_5049A0(connection.Owner(),
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
        input::SetContext_44B330(connection.Owner(), nullptr);
        sceneUi.capability3InputPending = 1;
    } else {
        std::int32_t selectedFocus = requestedFocus;
        bool applyFocus = requestedFocus >= 0 && requestedFocus <= 2;
        if (sceneUi.capability3InputPending != 0) {
            selectedFocus = sceneUi.capability3InputFocus;
            applyFocus = true;
        }

        if (applyFocus) {
            input::SetContext_44B330(
                connection.Owner(),
                sceneUi.inputFocusTable[selectedFocus]);
            sceneUi.capability3InputPending = 0;
            sceneUi.capability3InputFocus = selectedFocus;
        }
    }

    cg::ui::DrawUiControl_45EFD0(connection.Owner(),
        &primaryButton, 207, 0, 0, nonzeroMode);
    cg::ui::DrawUiControl_45EFD0(connection.Owner(),
        &secondaryButton, 207, 0, 0, nonzeroMode);
    static_cast<void>(DrawClientVersion(connection.Owner(), 540, 424, render::gPaletteCapability >= 1 ? 3 : 0));
    return result;
}

} // namespace cg::login
