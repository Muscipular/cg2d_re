#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/login/LoginMessage.hpp"
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

struct MessageLayout {
    std::int32_t width;
    std::int32_t height;
    std::int32_t x;
    std::int32_t y;
};

// 原 0x9A6E2C、0x55A04C、0x9A50A8/0x9A5658/0x9A5660/0x9A5ABC、0x9A6E30。
static std::uint16_t gMessageState = 0;
static std::int32_t gMessageResult = -2;
static MessageLayout gMessageLayout {};
static cg::yobi::YobiAction* gMessageAction = nullptr;

static constexpr cg::render::GraphicDescriptor16 kGraphicDescriptor {
    0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
};

static std::int32_t RoundMessageWidth(std::int32_t textWidth)
{
    return ((textWidth + 95) / 64) * 64;
}

std::array<std::wstring, 12> gLoginMsgLines_9A620C {}; // 原全局：0x009A620C。

// 原函数：0x0045CE00。
void ResetLoginMessage()
{
    gMessageState = 0;
}

// 原函数：0x0045CE10。
std::int32_t UpdateLoginMessage(const std::wstring_view text)
{
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
        if (gMessageState == 0) {
            gMessageState = 1;
            gMessageResult = -2;
            gMessageLayout.width =
                RoundMessageWidth(cg::render::TextMeasure(gLoginMsgLines_9A620C[8], 0));
            gMessageLayout.height = 192;
            gMessageLayout.x = (kScreenWidth - gMessageLayout.width) / 2;
            gMessageLayout.y = 144;
            gMessageAction = cg::yobi::CreateModalAction_5044E0(
                gMessageLayout.x,
                gMessageLayout.y,
                gMessageLayout.width,
                gMessageLayout.height,
                1,
                1);
        }

        if (cg::render::FindClickedTextIndex_45D7A0(&gMessageResult, 1) == 0) {
            detail::PlaySe(79, 320, 240);
            if (gMessageAction != nullptr) {
                cg::yobi::MarkDeath(gMessageAction);
                gMessageAction = nullptr;
            }
            return 1;
        }

        if (gMessageAction == nullptr
            || !cg::yobi::IsYobiActionReady(gMessageAction, 1)) {
            return 0;
        }

        const std::int32_t contentX = gMessageLayout.x
            + (gMessageLayout.width - cg::render::TextMeasure(gLoginMsgLines_9A620C[6], 0)) / 2;
        for (std::size_t index = 6; index < 12; ++index) {
            const std::int32_t y = gMessageLayout.y
                + 30
                + static_cast<std::int32_t>(index - 6) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                0,
                gLoginMsgLines_9A620C[index],
                0,
                0);
        }

        const std::int32_t confirmWidth = cg::render::TextMeasure(kConfirmText, 0);
        const std::int32_t confirmX = gMessageLayout.x
            + (gMessageLayout.width - confirmWidth) / 2;
        const std::int8_t hoverImageSequence = static_cast<std::int8_t>(
            static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768));
        gMessageResult = cg::render::DrawText_456780(
            static_cast<std::int16_t>(confirmX),
            static_cast<std::int16_t>(gMessageLayout.y + 156),
            2,
            0,
            4,
            kConfirmText,
            2,
            hoverImageSequence);
        return 0;
    }

    if (text == L"Multiline_New") {
        if (gMessageState == 0) {
            gMessageState = 1;
            gMessageResult = -2;
            gMessageLayout.width =
                RoundMessageWidth(cg::render::TextMeasure(gLoginMsgLines_9A620C[0], 0));
            gMessageLayout.height = 336;
            gMessageLayout.x = (kScreenWidth - gMessageLayout.width) / 2;
            gMessageLayout.y = 72;
            gMessageAction = cg::yobi::CreateModalAction_5044E0(
                gMessageLayout.x,
                gMessageLayout.y,
                gMessageLayout.width,
                gMessageLayout.height,
                1,
                1);
        }

        if (cg::render::FindClickedTextIndex_45D7A0(&gMessageResult, 1) == 0) {
            detail::PlaySe(79, 320, 240);
            if (gMessageAction != nullptr) {
                cg::yobi::MarkDeath(gMessageAction);
                gMessageAction = nullptr;
            }
            return 1;
        }

        if (gMessageAction == nullptr
            || !cg::yobi::IsYobiActionReady(gMessageAction, 1)) {
            return 0;
        }

        const std::int32_t contentX = gMessageLayout.x
            + (gMessageLayout.width - cg::render::TextMeasure(gLoginMsgLines_9A620C[0], 0)) / 2;
        for (std::size_t index = 0; index < 6; ++index) {
            const std::int32_t y = gMessageLayout.y
                + 30
                + static_cast<std::int32_t>(index) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                4,
                gLoginMsgLines_9A620C[index],
                0,
                0);
        }
        for (std::size_t index = 6; index < 12; ++index) {
            const std::int32_t y = gMessageLayout.y
                + 160
                + static_cast<std::int32_t>(index - 6) * 20;
            cg::render::DrawText_456780(
                static_cast<std::int16_t>(contentX),
                static_cast<std::int16_t>(y),
                2,
                0,
                0,
                gLoginMsgLines_9A620C[index],
                0,
                0);
        }

        const std::int32_t confirmWidth = cg::render::TextMeasure(kConfirmText, 0);
        const std::int32_t confirmX = gMessageLayout.x
            + (gMessageLayout.width - confirmWidth) / 2;
        const std::int8_t hoverImageSequence = static_cast<std::int8_t>(
            static_cast<std::uint8_t>(cg::render::gHoverFramePaletteIndex_D69768));
        gMessageResult = cg::render::DrawText_456780(
            static_cast<std::int16_t>(confirmX),
            static_cast<std::int16_t>(gMessageLayout.y + 296),
            2,
            0,
            4,
            kConfirmText,
            2,
            hoverImageSequence);
        return 0;
    }

    if (gMessageState == 0) {
        gMessageState = 1;
        const std::int32_t messageHeight =
            (lineCount - 1) * (cg::render::gFontMetrics[0].doubleHeight + 2) + 96;
        gMessageResult = -2;
        gMessageLayout.width = RoundMessageWidth(maximumTextWidth);
        gMessageLayout.height = messageHeight;
        gMessageLayout.x = (kScreenWidth - gMessageLayout.width) / 2;
        gMessageLayout.y = (kScreenHeight - gMessageLayout.height) / 2;
        gMessageAction = cg::yobi::CreateModalAction_5044E0(
            gMessageLayout.x,
            gMessageLayout.y,
            gMessageLayout.width,
            gMessageLayout.height,
            4,
            1);
    }

    const std::int32_t lineOffset =
        (lineCount - 1) * (cg::render::gFontMetrics[0].doubleHeight + 2);
    const std::int32_t left = gMessageLayout.x + (gMessageLayout.width - 66) / 2;
    const std::int32_t top = gMessageLayout.y + lineOffset + 61;
    const bool hovered = cg::ui::HitTestMouseRect_5049A0(left, top, left + 66, top + 17, -1) != 0;
    if (hovered) {
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
        if ((mouseClickState & 1U) != 0) {
            detail::PlaySe(79, 320, 240);
            if (gMessageAction != nullptr) {
                cg::yobi::MarkDeath(gMessageAction);
                gMessageAction = nullptr;
            }
            return 1;
        }
    }

    if (gMessageAction != nullptr
        && cg::yobi::IsYobiActionReady(gMessageAction, 1)) {
        const std::int32_t textX = gMessageLayout.x
            + (gMessageLayout.width - maximumTextWidth) / 2;
        for (std::int32_t index = 0; index < lineCount; ++index) {
            const std::int32_t y = gMessageLayout.y
                + index * (cg::render::gFontMetrics[0].doubleHeight + 2)
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

        const std::int32_t graphicY = gMessageLayout.y
            + (lineCount - 1) * (cg::render::gFontMetrics[0].doubleHeight + 2)
            + 69;
        cg::render::DrawGraphic(
            gMessageLayout.x + gMessageLayout.width / 2,
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
