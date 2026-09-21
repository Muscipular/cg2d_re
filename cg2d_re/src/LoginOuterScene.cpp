#include "cg/LoginScene.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/Input.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace {

constexpr char kIncorrectAccountOrPassword[] = "不正確的帳號和密碼。";

// 原 0x9A6E24 只有 sub_45C2D0 内一写一读；保持唯一非拥有原生指针。
YobiAction* gValue9A6E24 = nullptr;

// 原 0x9A4BA0 只有本函数的消息读取与固定文本写入，原容量未获证。
// 新端只为同一文本的 UTF-8 字节及末尾 NUL 建立最窄唯一缓冲。
char gValue9A4BA0[sizeof(kIncorrectAccountOrPassword)] {};

} // namespace

void ProcessOuterScene_45C2D0()
{
    bool processLoginInput = false;
    const std::uint32_t initialState =
        std::bit_cast<std::uint32_t>(world::gCurrentSceneState);

    if (initialState <= 10u) {
        switch (initialState) {
        case 0:
            detail::YobiClearUp_404D30();
            gValue9A6E24 = nullptr;
            world::gCurrentSceneState = 1;
            static_cast<void>(render::SelectPalette(30, 0));
            static_cast<void>(detail::DrawGraphic(
                320, 240, 3, 22312, 0, nullptr));
            static_cast<void>(detail::MarkDeath(gValue9A6E24));
            InitializeInputContexts_45C7C0();
            world::gCurrentSceneState = 6;
            static_cast<void>(render::SelectPalette(32, 60));
            static_cast<void>(detail::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr));
            break;
        case 6:
            static_cast<void>(detail::DrawGraphic(
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
            static_cast<void>(detail::DrawGraphic(
                320, 240, 3, 22311, 0, nullptr));
            if (render::gPaletteChanging == 0) {
                static_cast<void>(world::SetCurrentSceneType(2));
                detail::YobiClearUp_404D30();
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
                static_cast<std::uint8_t>(detail::gValueD68D9C);
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
        cg::NullSub_509080();
        static_cast<void>(
            WriteInputValueFF494F_5203B0(gInputContext9A5960.text));
        static_cast<void>(
            WriteInputValueFF4982_520410(gInputContext9A5678.text));
        static_cast<void>(detail::Sub_520200());
        static_cast<void>(detail::PlaySe(53, 320, 240));
        if (render::gPaletteCapability < 1) {
            static_cast<void>(render::SelectPalette(17, 30));
        }
        world::gCurrentSceneState = 10;
    } else if (result == 2) {
        static_cast<void>(detail::PlaySe(54, 320, 240));
        static_cast<void>(platform::detail::PostMessageA_54D2F0(
            platform::gLegacyWindow, 0x10, 0, 0));
    } else if (result < 0) {
        world::gCurrentSceneState = 8;
        std::memcpy(
            gValue9A4BA0,
            kIncorrectAccountOrPassword,
            sizeof(kIncorrectAccountOrPassword));
    }
}

} // namespace cg::login
