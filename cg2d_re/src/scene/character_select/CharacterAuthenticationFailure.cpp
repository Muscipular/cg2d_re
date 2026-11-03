#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"
#include "CharacterSelectSceneState.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>

namespace cg::character_select {

// 原函数：0x0045D7D0。
void ResetAuthenticationFailure_45D7D0()
{
    cg::network::GetGameConnection().CharacterSelectUi().authenticationFailure.stage = 0;
}

// 原函数：0x0045D7E0。
std::int32_t UpdateAuthenticationFailure_45D7E0()
{
    auto& failure = cg::network::GetGameConnection().CharacterSelectUi().authenticationFailure;
    std::int32_t result = 0;
    bool hovered = false;
    const cg::render::GraphicDescriptor16 graphicDescriptor {
        0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
    };

    if (failure.stage == 0) {
        failure.stage = 1;
        failure.sentinel = -2;
        failure.width = 512;
        failure.height = 144;
        failure.x = 64;
        failure.y = 168;
        failure.action = cg::yobi::CreateModalAction_5044E0(
            64,
            168,
            512,
            144,
            4,
            1);
    }

    const std::int32_t buttonWidth = failure.width;
    const std::int32_t buttonX = failure.x;
    const std::int32_t centeredButtonWidth = buttonWidth - 66;
    const std::int32_t buttonY = failure.y;
    const std::int32_t buttonLeft = (centeredButtonWidth >> 1) + buttonX;
    const std::int32_t buttonBottom = buttonY + 132;
    const std::int32_t buttonTop = buttonY + 115;
    const std::int32_t buttonRight = buttonLeft + 66;
    if (cg::ui::HitTestMouseRect_5049A0(
            buttonLeft,
            buttonTop,
            buttonRight,
            buttonBottom,
            -1)
        != 0) {
        hovered = true;
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
        if ((mouseClickState & 1U) != 0) {
            result = 1;
            login::detail::PlaySe(51, 320, 240);
            cg::yobi::YobiAction* const action = failure.action;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                failure.action = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = failure.action;
    if (action == nullptr
        || !cg::yobi::IsYobiActionReady(action, 1)) {
        return result;
    }

    const std::int32_t textYOrigin = failure.y;
    const std::int32_t textXOrigin = failure.x;
    const std::int32_t textY = textYOrigin + 20;
    const std::int32_t textX = textXOrigin + 140;
    cg::render::DrawText3_456560(
        static_cast<std::int16_t>(textX),
        static_cast<std::int16_t>(textY),
        2,
        6,
        L"登入认证失败。" /* 原 0x55C5E8：正文 14 字节，含唯一 NUL 共 15 字节；来源编码名称不猜。 */,
        0);
    const std::int32_t graphicWidth = failure.width;
    const std::int32_t graphicYOrigin = failure.y;
    const std::int32_t imageNo = hovered ? 243050 : 243048;
    const std::int32_t graphicXOrigin = failure.x;
    const std::int32_t graphicHalfWidth = graphicWidth >> 1;
    const std::int32_t graphicY = graphicYOrigin + 123;
    const std::int32_t graphicX = graphicHalfWidth + graphicXOrigin;
    cg::render::DrawGraphic(
        graphicX,
        graphicY,
        std::uint8_t{0xDF},
        imageNo,
        0,
        &graphicDescriptor);
    return result;
}

} // namespace cg::character_select
