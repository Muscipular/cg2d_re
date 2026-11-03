#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginMessageDependencies.hpp"

#include "cg/audio/LegacyAudio.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring> // 原CRT接口：0x0053C2F0；原参数／返回合同由直接调用承接。

namespace cg::login {

namespace detail {

std::int32_t PlaySe(
    const std::int32_t seNo,
    const std::int32_t screenX,
    const std::int32_t unused)
{
    static_cast<void>(unused);
    return audio::PlaySe_52C450(seNo, screenX);
}

} // namespace detail

// 原 0x55C5D4：“確定”，4 字节正文，字符串字面量隐式追加唯一 NUL。
static constexpr wchar_t kConfirmText[] = L"确定";

static constexpr std::int32_t kScreenWidth = 640;
static constexpr std::int32_t kScreenHeight = 480;

static constexpr cg::render::GraphicDescriptor16 kGraphicDescriptor {
    0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
};

static std::int32_t RoundMessageWidth(std::int32_t textWidth)
{
    return ((textWidth + 95) / 64) * 64;
}

// 原函数：0x0045CE00。
void ResetLoginMessage()
{
    cg::network::GetGameConnection().LoginMessages().phase = 0;
}

// 原函数：0x0045CE10。
std::int32_t UpdateLoginMessage(const std::wstring_view text)
{
    auto& message = cg::network::GetGameConnection().LoginMessages();
    std::vector<std::wstring_view> lines;
    std::int32_t maximumTextWidth = 0;
    std::size_t start = 0;
    for (;;) {
        const std::size_t newline = text.find(L'\n', start);
        const std::wstring_view line = text.substr(start, newline == std::wstring_view::npos ? newline : newline - start);
        lines.push_back(line);
        const std::int32_t width = cg::render::TextMeasure(line, 0);
        if (width > maximumTextWidth) { maximumTextWidth = width; }
        if (newline == std::wstring_view::npos) { break; }
        start = newline + 1;
    }
    const std::int32_t lineCount = static_cast<std::int32_t>(lines.size());

    if (text == L"Multiline") {
        if (message.phase == 0) {
            message.phase = 1;
            message.result = -2;
            message.width =
                RoundMessageWidth(cg::render::TextMeasure(message.lines[8], 0));
            message.height = 192;
            message.x = (kScreenWidth - message.width) / 2;
            message.y = 144;
            message.action = cg::yobi::CreateModalAction_5044E0(
                message.x,
                message.y,
                message.width,
                message.height,
                1,
                1);
        }

        if (cg::render::FindClickedTextIndex_45D7A0(&message.result, 1) == 0) {
            detail::PlaySe(79, 320, 240);
            if (message.action != nullptr) {
                cg::yobi::MarkDeath(message.action);
                message.action = nullptr;
            }
            return 1;
        }

        if (message.action == nullptr
            || !cg::yobi::IsYobiActionReady(message.action, 1)) {
            return 0;
        }

        const std::int32_t contentX = message.x
            + (message.width - cg::render::TextMeasure(message.lines[6], 0)) / 2;
        for (std::size_t index = 6; index < 12; ++index) {
            const std::int32_t y = message.y
                + 30
                + static_cast<std::int32_t>(index - 6) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                0,
                message.lines[index],
                0,
                0);
        }

        const std::int32_t confirmWidth = cg::render::TextMeasure(kConfirmText, 0);
        const std::int32_t confirmX = message.x
            + (message.width - confirmWidth) / 2;
        const std::int8_t hoverImageSequence = static_cast<std::int8_t>(
            static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768));
        message.result = cg::render::DrawText_456780(
            static_cast<std::int16_t>(confirmX),
            static_cast<std::int16_t>(message.y + 156),
            2,
            0,
            4,
            kConfirmText,
            2,
            hoverImageSequence);
        return 0;
    }

    if (text == L"Multiline_New") {
        if (message.phase == 0) {
            message.phase = 1;
            message.result = -2;
            message.width =
                RoundMessageWidth(cg::render::TextMeasure(message.lines[0], 0));
            message.height = 336;
            message.x = (kScreenWidth - message.width) / 2;
            message.y = 72;
            message.action = cg::yobi::CreateModalAction_5044E0(
                message.x,
                message.y,
                message.width,
                message.height,
                1,
                1);
        }

        if (cg::render::FindClickedTextIndex_45D7A0(&message.result, 1) == 0) {
            detail::PlaySe(79, 320, 240);
            if (message.action != nullptr) {
                cg::yobi::MarkDeath(message.action);
                message.action = nullptr;
            }
            return 1;
        }

        if (message.action == nullptr
            || !cg::yobi::IsYobiActionReady(message.action, 1)) {
            return 0;
        }

        const std::int32_t contentX = message.x
            + (message.width - cg::render::TextMeasure(message.lines[0], 0)) / 2;
        for (std::size_t index = 0; index < 6; ++index) {
            const std::int32_t y = message.y
                + 30
                + static_cast<std::int32_t>(index) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                4,
                message.lines[index],
                0,
                0);
        }
        for (std::size_t index = 6; index < 12; ++index) {
            const std::int32_t y = message.y
                + 160
                + static_cast<std::int32_t>(index - 6) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                0,
                message.lines[index],
                0,
                0);
        }

        const std::int32_t confirmWidth = cg::render::TextMeasure(kConfirmText, 0);
        const std::int32_t confirmX = message.x
            + (message.width - confirmWidth) / 2;
        const std::int8_t hoverImageSequence = static_cast<std::int8_t>(
            static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768));
        message.result = cg::render::DrawText_456780(
            static_cast<std::int16_t>(confirmX),
            static_cast<std::int16_t>(message.y + 296),
            2,
            0,
            4,
            kConfirmText,
            2,
            hoverImageSequence);
        return 0;
    }

    if (message.phase == 0) {
        message.phase = 1;
        const std::int32_t messageHeight =
            (lineCount - 1) * (cg::render::GetFontMetrics(0).doubleHeight + 2) + 96;
        message.result = -2;
        message.width = RoundMessageWidth(maximumTextWidth);
        message.height = messageHeight;
        message.x = (kScreenWidth - message.width) / 2;
        message.y = (kScreenHeight - message.height) / 2;
        message.action = cg::yobi::CreateModalAction_5044E0(
            message.x,
            message.y,
            message.width,
            message.height,
            4,
            1);
    }

    const std::int32_t lineOffset =
        (lineCount - 1) * (cg::render::GetFontMetrics(0).doubleHeight + 2);
    const std::int32_t left = message.x + (message.width - 66) / 2;
    const std::int32_t top = message.y + lineOffset + 61;
    const bool hovered = cg::ui::HitTestMouseRect_5049A0(left, top, left + 66, top + 17, -1) != 0;
    if (hovered) {
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
        if ((mouseClickState & 1U) != 0) {
            detail::PlaySe(79, 320, 240);
            if (message.action != nullptr) {
                cg::yobi::MarkDeath(message.action);
                message.action = nullptr;
            }
            return 1;
        }
    }

    if (message.action != nullptr
        && cg::yobi::IsYobiActionReady(message.action, 1)) {
        const std::int32_t textX = message.x
            + (message.width - maximumTextWidth) / 2;
        for (std::int32_t index = 0; index < lineCount; ++index) {
            const std::int32_t y = message.y
                + index * (cg::render::GetFontMetrics(0).doubleHeight + 2)
                + 30;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(textX),
                static_cast<std::int16_t>(y),
                2,
                0,
                0,
                lines[static_cast<std::size_t>(index)],
                0,
                0);
        }

        const std::int32_t graphicY = message.y
            + (lineCount - 1) * (cg::render::GetFontMetrics(0).doubleHeight + 2)
            + 69;
        cg::render::DrawGraphic(
            message.x + message.width / 2,
            graphicY,
            0xDF,
            hovered ? 243050 : 243048,
            0,
            &kGraphicDescriptor);
    }
    return 0;
}

// 原0x0045CE10：消息调用已统一UTF-16正文。

} // namespace cg::login
