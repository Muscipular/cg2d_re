#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace {

// 原 0x9A6E24 只有 sub_45C2D0 内一写一读；保持唯一非拥有原生指针。
cg::yobi::YobiAction* gValue9A6E24 = nullptr;

// 原 0x9A4BA0 只有本函数的消息读取与固定文本写入，原容量未获证。
// 新端只为同一文本的原字节及末尾 NUL 建立最窄唯一缓冲，不据此推定原总容量。
std::wstring gValue9A4BA0 {};

} // namespace

// 原函数：0x0045C2D0。
void ProcessOuterScene_45C2D0()
{
    bool processLoginInput = false;
    const std::uint32_t initialState =
        std::bit_cast<std::uint32_t>(world::gCurrentSceneState);

    if (initialState <= 10u) {
        switch (initialState) {
        case 0:
            cg::yobi::detail::YobiClearUp_404D30();
            gValue9A6E24 = nullptr;
            world::gCurrentSceneState = 1;
            static_cast<void>(render::SelectPalette(30, 0));
            static_cast<void>(cg::render::DrawGraphic(
                320, 240, 3, 22312, 0, nullptr));
            static_cast<void>(cg::yobi::detail::MarkDeath(gValue9A6E24));
            InitializeInputContexts_45C7C0();
            world::gCurrentSceneState = 6;
            static_cast<void>(render::SelectPalette(32, 60));
            static_cast<void>(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr));
            break;
        case 6:
            static_cast<void>(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr));
            break;
        case 7:
            if (render::gPaletteChanging == 0) {
                if (render::gPaletteCapability == 0) {
                    static_cast<void>(render::SelectPalette(18, 30));
                } else {
                    static_cast<void>(render::SelectPalette(19, 60));
                }
                world::gCurrentSceneState = 10;
                input::SetContext_44B330(nullptr);
                return;
            }
            break;
        case 8:
            ResetLoginMessage();
            static_cast<void>(detail::PlaySe(56, 320, 240));
            world::gCurrentSceneState = 9;
            input::SetContext_44B330(nullptr);
            return;
        case 9:
            if (UpdateLoginMessage(gValue9A4BA0) != 0) {
                world::gCurrentSceneState = 6;
                processLoginInput = true;
            }
            break;
        case 10:
            static_cast<void>(cg::render::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr));
            if (render::gPaletteChanging == 0) {
                static_cast<void>(world::SetCurrentSceneType(2));
                cg::yobi::detail::YobiClearUp_404D30();
                return;
            }
            break;
        default:
            break;
        }
    }

    if (!processLoginInput) {
        const std::uint32_t sampledState =
            std::bit_cast<std::uint32_t>(world::gCurrentSceneState);
        if (sampledState < 6u) {
            const std::uint8_t mouseClickState =
                static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
            if ((mouseClickState & 1u) != 0) {
                world::gCurrentSceneState = 6;
                processLoginInput = true;
            }
        } else if (sampledState == 6u) {
            processLoginInput = true;
        }
    }

    if (!processLoginInput) {
        input::SetContext_44B330(nullptr);
        return;
    }

    const std::int32_t result = ProcessLoginInput_45CA20(0);
    if (result == 1) {
        cg::core::NullSub_509080();
        if (!SaveInputValues(input::ContextText(gInputContext9A5960), input::ContextText(gInputContext9A5678))) {
            return;
        }
        static_cast<void>(detail::Sub_520200());
        static_cast<void>(detail::PlaySe(53, 320, 240));
        if (render::gPaletteCapability < 1) {
            static_cast<void>(render::SelectPalette(17, 30));
        }
        world::gCurrentSceneState = 10;
    } else if (result == 2) {
        static_cast<void>(detail::PlaySe(54, 320, 240));
        static_cast<void>(::PostMessageW(
            reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0));
    } else if (result < 0) {
        world::gCurrentSceneState = 8;
        gValue9A4BA0 = L"不正确的帐号和密码。" /* 原55C584「不正確的帳號和密碼。」；20B正文，字面量隐式追加NUL。 */;
    }
}

} // namespace cg::login
