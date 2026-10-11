#include "cg/LoginScene.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::login {

namespace {

// 原55C59C「請輸入ＩＤ和ＰＡＳＳＷＯＲＤ」；28B正文，字面量隐式追加NUL。
constexpr char kMissingCredentials[] = "\xD5\x88\xDD\x94\xC8\xEB\xA3\xC9\xA3\xC4\xBA\xCD\xA3\xD0\xA3\xC1\xA3\xD3\xA3\xD3\xA3\xD7\xA3\xCF\xA3\xD2\xA3\xC4";
static_assert(sizeof(kMissingCredentials) == 29);

// 原 0x9A5FE0：本函数私有的 256 字节消息缓冲，映像初值为零。
char gLoginModeMessage[256] {};

static_assert(sizeof(kMissingCredentials) <= sizeof(gLoginModeMessage));

} // namespace

// 原函数：0x0045C5A0。
void ProcessModeScene_45C5A0()
{
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
        cg::NullSub_509080();
        static_cast<void>(
            WriteInputValueFF494F_5203B0(gInputContext9A5960.text));
        static_cast<void>(
            WriteInputValueFF4982_520410(gInputContext9A5678.text));
        static_cast<void>(detail::Sub_520200());
        static_cast<void>(detail::PlaySe(53, 320, 240));
        if (render::gPaletteCapability >= 1) {
            static_cast<void>(render::SelectPalette(19, 30));
        }
        world::gCurrentSceneState = 3;
        return;
    }

    if (result == 2) {
        static_cast<void>(detail::PlaySe(54, 320, 240));
        static_cast<void>(platform::PostLegacyWindowMessage(
            platform::gLegacyWindow, 0x10, 0, 0));
    } else if (result < 0) {
        world::gCurrentSceneState = 100;
        std::memcpy(
            gLoginModeMessage,
            kMissingCredentials,
            sizeof(kMissingCredentials));
    }

    if (result < 0 || world::gCurrentSceneState == 100) {
        ResetLoginMessage();
        static_cast<void>(detail::PlaySe(56, 320, 240));
        ++world::gCurrentSceneState;
    }
    if (world::gCurrentSceneState == 101
        && UpdateLoginMessage(gLoginModeMessage) != 0) {
        world::gCurrentSceneState = 1;
    }
}

} // namespace cg::login
