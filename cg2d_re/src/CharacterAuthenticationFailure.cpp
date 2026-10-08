#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>

namespace cg::character_select {
namespace {

// 原 0x55C5E8：正文 14 字节，含唯一 NUL 共 15 字节；来源编码名称不猜。
constexpr char kAuthenticationFailedText[] =
    "\xB5\xC7\xC8\xEB\xD5\x4A\xD7\x43\xCA\xA7\x94\xA1\xA1\xA3";
static_assert(sizeof(kAuthenticationFailedText) == 15);

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
            64,
            168,
            512,
            144,
            4,
            1);
    }

    const std::int32_t buttonWidth = gAuthenticationFailureWidth;
    const std::int32_t buttonX = gAuthenticationFailureX;
    const std::int32_t centeredButtonWidth = buttonWidth - 66;
    const std::int32_t buttonY = gAuthenticationFailureY;
    const std::int32_t buttonLeft = (centeredButtonWidth >> 1) + buttonX;
    const std::int32_t buttonBottom = buttonY + 132;
    const std::int32_t buttonTop = buttonY + 115;
    const std::int32_t buttonRight = buttonLeft + 66;
    if (login::detail::Sub_5049A0(
            buttonLeft,
            buttonTop,
            buttonRight,
            buttonBottom,
            -1)
        != 0) {
        hovered = true;
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(login::detail::gValueD68D9C);
        if ((mouseClickState & 1U) != 0) {
            result = 1;
            login::detail::PlaySe(51, 320, 240);
            login::YobiAction* const action = gAuthenticationFailureAction;
            if (action != nullptr) {
                login::detail::MarkDeath(action);
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

    const std::int32_t textYOrigin = gAuthenticationFailureY;
    const std::int32_t textXOrigin = gAuthenticationFailureX;
    const std::int32_t textY = textYOrigin + 20;
    const std::int32_t textX = textXOrigin + 140;
    login::detail::DrawText3(
        static_cast<std::int16_t>(textX),
        static_cast<std::int16_t>(textY),
        2,
        6,
        kAuthenticationFailedText,
        0);
    const std::int32_t graphicWidth = gAuthenticationFailureWidth;
    const std::int32_t graphicYOrigin = gAuthenticationFailureY;
    const std::int32_t imageNo = hovered ? 243050 : 243048;
    const std::int32_t graphicXOrigin = gAuthenticationFailureX;
    const std::int32_t graphicHalfWidth = graphicWidth >> 1;
    const std::int32_t graphicY = graphicYOrigin + 123;
    const std::int32_t graphicX = graphicHalfWidth + graphicXOrigin;
    login::detail::DrawGraphic(
        graphicX,
        graphicY,
        std::uint8_t{0xDF},
        imageNo,
        0,
        &graphicDescriptor);
    return result;
}

} // namespace cg::character_select
