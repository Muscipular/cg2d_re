#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>

namespace cg::character_select {
namespace {

constexpr char kAuthenticationFailedText[] = "登入認證失敗。";

// 下列原存储的全部引用都只在 sub_45D7D0/sub_45D7E0 内。
std::uint16_t gAuthenticationFailureStage = 0; // 原 0x9A6E34，R/W16。
login::YobiAction* gAuthenticationFailureAction = nullptr; // 原 0x9A6E38。
std::int32_t gAuthenticationFailureUnknown = -2; // 原 0x55A050，R/W32。
std::int32_t gAuthenticationFailureWidth = 0;  // 原 0x9A50CC，R/W32。
std::int32_t gAuthenticationFailureHeight = 0; // 原 0x9A5E6C，R/W32。
std::int32_t gAuthenticationFailureX = 0;      // 原 0x9A5654，R/W32。
std::int32_t gAuthenticationFailureY = 0;      // 原 0x9A57DC，R/W32。

} // namespace

void ResetAuthenticationFailure_45D7D0()
{
    gAuthenticationFailureStage = 0;
}

std::int32_t UpdateAuthenticationFailure_45D7E0()
{
    std::int32_t result = 0;
    bool hovered = false;
    const login::detail::GraphicDescriptor16 graphicDescriptor {{
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x04, 0x00, 0x00,
    }};

    if (gAuthenticationFailureStage == 0) {
        gAuthenticationFailureStage = 1;
        gAuthenticationFailureUnknown = -2;
        gAuthenticationFailureWidth = 512;
        gAuthenticationFailureHeight = 144;
        gAuthenticationFailureX = 64;
        gAuthenticationFailureY = 168;
        gAuthenticationFailureAction = login::detail::Sub_5044E0(
            gAuthenticationFailureX,
            gAuthenticationFailureY,
            gAuthenticationFailureWidth,
            gAuthenticationFailureHeight,
            4,
            1);
    }

    const std::int32_t buttonLeft = gAuthenticationFailureX
        + ((gAuthenticationFailureWidth - 66) >> 1);
    if (login::detail::Sub_5049A0(
            buttonLeft,
            gAuthenticationFailureY + 115,
            buttonLeft + 66,
            gAuthenticationFailureY + 132,
            -1)
        != 0) {
        hovered = true;
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(login::detail::gValueD68D9C);
        if ((mouseClickState & 1U) != 0) {
            result = 1;
            login::detail::PlaySe(51, 320, 240);
            if (gAuthenticationFailureAction != nullptr) {
                login::detail::MarkDeath(gAuthenticationFailureAction);
                gAuthenticationFailureAction = nullptr;
            }
            return result;
        }
    }

    login::YobiAction* const action = gAuthenticationFailureAction;
    if (action == nullptr
        || !login::detail::IsYobiActionReady(action, 1)) {
        return result;
    }

    login::detail::DrawText3(
        static_cast<std::int16_t>(gAuthenticationFailureX + 140),
        static_cast<std::int16_t>(gAuthenticationFailureY + 20),
        2,
        6,
        kAuthenticationFailedText,
        0);
    login::detail::DrawGraphic(
        gAuthenticationFailureX + gAuthenticationFailureWidth / 2,
        gAuthenticationFailureY + 123,
        std::uint8_t{0xDF},
        hovered ? 243050 : 243048,
        0,
        &graphicDescriptor);
    return result;
}

} // namespace cg::character_select
