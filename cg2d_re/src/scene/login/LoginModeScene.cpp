#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/network/CgConnection.hpp"
#include "LoginSceneState.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#include <Windows.h>

#include <cstdint>
#include <cstring>

namespace cg::login {

// 原函数：0x0045C5A0。
void ProcessModeScene_45C5A0()
{
    auto& sceneUi = cg::network::GetGameConnection().LoginSceneUi();
    std::int32_t inputMode = 2;
    const std::int32_t initialState = world::gCurrentSceneState;
    if (initialState == 0) {
        world::gCurrentSceneState = 1;
        render::gPaletteCapability =
            resource::ProbePaletteCapability_45C970();
        InitializeInputContexts_45C7C0();

        const std::int32_t capability = render::gPaletteCapability;
        if (capability == 3) {
            static_cast<void>(render::SelectPalette(29, 30));
        } else if (capability == 2 || capability == 1) {
            static_cast<void>(render::SelectPalette(32, 30));
        } else {
            static_cast<void>(render::SelectPalette(24, 30));
        }
    } else if (initialState == 1) {
        if (render::gPaletteChanging == 0) {
            world::gCurrentSceneState = 2;
        }
    } else if (initialState == 2) {
        inputMode = 0;
    } else if (initialState == 3) {
        if (render::gPaletteChanging == 0) {
            if (render::gPaletteCapability == 0) {
                static_cast<void>(render::SelectPalette(18, 30));
            } else {
                static_cast<void>(render::SelectPalette(19, 60));
            }
            ++world::gCurrentSceneState;
        }
    } else if (initialState == 4) {
        if (render::gPaletteChanging == 0) {
            static_cast<void>(render::SelectPalette(19, 30));
            ++world::gCurrentSceneState;
        }
    } else if (initialState == 5 && render::gPaletteChanging == 0) {
        static_cast<void>(world::SetCurrentSceneType(2));
    }

    const std::int32_t result = ProcessLoginInput_45CA20(inputMode);
    if (result == 1) {
        cg::core::NullSub_509080();
        if (!SaveInputValues(input::ContextText(sceneUi.context9A5960), input::ContextText(sceneUi.context9A5678))) {
            return;
        }
        static_cast<void>(detail::SaveCurrentLoginRecord_520200());
        static_cast<void>(detail::PlaySe(53, 320, 240));
        if (render::gPaletteCapability >= 1) {
            static_cast<void>(render::SelectPalette(19, 30));
        }
        world::gCurrentSceneState = 3;
        return;
    }

    if (result == 2) {
        static_cast<void>(detail::PlaySe(54, 320, 240));
        static_cast<void>(::PostMessageW(
            reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0));
    } else if (result < 0) {
        world::gCurrentSceneState = 100;
        sceneUi.loginModeMessage = L"请输入ＩＤ和ＰＡＳＳＷＯＲＤ" /* 原55C59C「請輸入ＩＤ和ＰＡＳＳＷＯＲＤ」；28B正文，字面量隐式追加NUL。 */;
    }

    if (result < 0 || world::gCurrentSceneState == 100) {
        ResetLoginMessage();
        static_cast<void>(detail::PlaySe(56, 320, 240));
        ++world::gCurrentSceneState;
    }
    if (world::gCurrentSceneState == 101
        && UpdateLoginMessage(sceneUi.loginModeMessage) != 0) {
        world::gCurrentSceneState = 1;
    }
}

} // namespace cg::login
