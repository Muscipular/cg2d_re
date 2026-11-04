#include "cg/session/ClientSession.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/network/CgConnection.hpp"
#include "LoginSceneState.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#include <Windows.h>

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::login {

// 原函数：0x0045C2D0。
void ProcessOuterScene_45C2D0(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    bool processLoginInput = false;
    const std::uint32_t initialState =
        std::bit_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState);

    if (initialState <= 10u) {
        switch (initialState) {
        case 0:
            cg::yobi::YobiClearUp_404D30(connection.Owner());
            connection.Owner().WorldScene().sceneState = 1;
            static_cast<void>(render::SelectPalette(30, 0));
            static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::DrawGraphic(
                320, 240, 3, 22312, 0, nullptr)));
            static_cast<void>(cg::yobi::MarkDeath(nullptr));
            InitializeInputContexts_45C7C0(connection);
            connection.Owner().WorldScene().sceneState = 6;
            static_cast<void>(render::SelectPalette(32, 60));
            static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr)));
            break;
        case 6:
            static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr)));
            break;
        case 7:
            if (render::gPaletteChanging == 0) {
                if (render::gPaletteCapability == 0) {
                    static_cast<void>(render::SelectPalette(18, 30));
                } else {
                    static_cast<void>(render::SelectPalette(19, 60));
                }
                connection.Owner().WorldScene().sceneState = 10;
                input::SetContext_44B330(connection.Owner(), nullptr);
                return;
            }
            break;
        case 8:
            ResetLoginMessage(connection);
            static_cast<void>(detail::PlaySe(56, 320, 240));
            connection.Owner().WorldScene().sceneState = 9;
            input::SetContext_44B330(connection.Owner(), nullptr);
            return;
        case 9:
            if (UpdateLoginMessage(connection, sceneUi.outerLoginErrorMessage) != 0) {
                connection.Owner().WorldScene().sceneState = 6;
                processLoginInput = true;
            }
            break;
        case 10:
            static_cast<void>(cg::render::StretchGraphicToScreen(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr)));
            if (render::gPaletteChanging == 0) {
                static_cast<void>(world::SetCurrentSceneType(connection.Owner(), 2));
                cg::yobi::YobiClearUp_404D30(connection.Owner());
                return;
            }
            break;
        default:
            break;
        }
    }

    if (!processLoginInput) {
        const std::uint32_t sampledState =
            std::bit_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState);
        if (sampledState < 6u) {
            const std::uint8_t mouseClickState =
                static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(connection.Owner()));
            if ((mouseClickState & 1u) != 0) {
                connection.Owner().WorldScene().sceneState = 6;
                processLoginInput = true;
            }
        } else if (sampledState == 6u) {
            processLoginInput = true;
        }
    }

    if (!processLoginInput) {
        input::SetContext_44B330(connection.Owner(), nullptr);
        return;
    }

    const std::int32_t result = ProcessLoginInput_45CA20(connection, 0);
    if (result == 1) {
        cg::core::NullSub_509080();
        if (!SaveInputValues(input::ContextText(sceneUi.context9A5960), input::ContextText(sceneUi.context9A5678))) {
            return;
        }
        static_cast<void>(detail::SaveCurrentLoginRecord_520200());
        static_cast<void>(detail::PlaySe(53, 320, 240));
        if (render::gPaletteCapability < 1) {
            static_cast<void>(render::SelectPalette(17, 30));
        }
        connection.Owner().WorldScene().sceneState = 10;
    } else if (result == 2) {
        static_cast<void>(detail::PlaySe(54, 320, 240));
        static_cast<void>(::PostMessageW(
            reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0));
    } else if (result < 0) {
        connection.Owner().WorldScene().sceneState = 8;
        sceneUi.outerLoginErrorMessage = L"不正确的帐号和密码。" /* 原55C584「不正確的帳號和密碼。」；20B正文，字面量隐式追加NUL。 */;
    }
}

} // namespace cg::login
