#include "cg/CharacterSelectScene.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
// 原函数：0x0045DEE0。
std::int32_t ProcessConfirmationDialog_45DEE0(
    std::int32_t x,
    std::int32_t y,
    std::wstring_view text,
    std::int8_t color,
    std::int32_t width,
    std::int32_t height)
{
    if (width <= 0) {
        width = 192;
    }
    if (height <= 0) {
        height = 96;
    }

    const std::int32_t left = x - width / 2;
    const std::int32_t top = y - height / 2;
    std::int32_t hover = 0;
    std::int32_t result = 0;

    login::YobiAction* modalAction = state::gCharacterModalAction;
    if (modalAction == nullptr) {
        modalAction = login::detail::CreateModalAction_5044E0(
            left,
            top,
            width,
            height,
            4,
            1);
        state::gCharacterModalAction = modalAction;
        if (modalAction == nullptr) {
            return 0;
        }
    }

    if (login::detail::IsYobiActionStateLessOrEqual(
            modalAction,
            0)) {
        return 0;
    }

    if (login::detail::Sub_5049A0(
            x - 73,
            top + 59,
            left + 158,
            top + 76,
            -1)
        != 0) {
        hover = 1;
    }
    if (login::detail::Sub_5049A0(
            x + 7,
            top + 59,
            left + 158,
            top + 76,
            -1)
        != 0) {
        hover = 2;
    }

    if (hover != 0) {
        const std::uint8_t mouseClickState =
            static_cast<std::uint8_t>(login::detail::gValueD68D9C);
        if ((mouseClickState & 1U) != 0) {
            result = hover;
            login::detail::PlaySe(79, 320, 240);
            login::YobiAction* const clickedAction = state::gCharacterModalAction;
            if (clickedAction != nullptr) {
                login::detail::MarkDeath(clickedAction);
                state::gCharacterModalAction = nullptr;
            }
        }
    }

    login::detail::DrawGraphic(
        x - 40,
        top + 67,
        std::uint8_t{223},
        hover == 1 ? 245325 : 245323,
        0,
        nullptr);
    login::detail::DrawGraphic(
        x + 40,
        top + 67,
        std::uint8_t{223},
        hover == 2 ? 245328 : 245326,
        0,
        nullptr);

    const std::int32_t textWidth = login::detail::TextMeasure(text, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(left + (width - textWidth) / 2),
        static_cast<std::int16_t>(top + 24),
        2,
        0,
        color,
        text,
        0,
        0);
    return result;
}

// 原函数：0x0045E090。
std::int32_t ProcessDefaultConfirmationDialog_45E090(
    std::int32_t x,
    std::int32_t y,
    std::wstring_view text,
    std::int8_t color)
{
    return ProcessConfirmationDialog_45DEE0(x, y, text, color, 0, 0);
}

// 原函数：0x0045E0C0。
std::int32_t ProcessCharacterDeletionWarning_45E0C0(
    std::int32_t x,
    std::int32_t y)
{
    detail::CharacterModalUiRecord confirmRecord {
        x - 118,
        y + 48,
        x - 120,
        y + 48,
        90,
        18,
        L"  確  定 " /* 原55C8D4「  確  定 」；9B正文，字面量隐式追加NUL。 */,
    };
    detail::CharacterModalUiRecord cancelRecord {
        x + 28,
        y + 48,
        x + 26,
        y + 48,
        90,
        18,
        L"  取  消 " /* 原55C8C8「  取  消 」；9B正文，字面量隐式追加NUL。 */,
    };
    std::int32_t result = 0;

    login::YobiAction* modalAction = state::gCharacterModalAction;
    if (modalAction == nullptr) {
        modalAction = login::detail::CreateModalAction_5044E0(
            x - 192,
            y - 96,
            384,
            192,
            4,
            1);
        state::gCharacterModalAction = modalAction;
        if (modalAction == nullptr) {
            return 0;
        }
    }

    if (login::detail::IsYobiActionStateLessOrEqual(
            modalAction,
            0)) {
        return 0;
    }

    if ((detail::Sub_503990(&confirmRecord) & 1) != 0) {
        result = 1;
    } else if ((detail::Sub_503990(&cancelRecord) & 1) != 0) {
        result = 2;
    }

    if (result != 0) {
        login::detail::PlaySe(79, 320, 240);
        login::YobiAction* const clickedAction = state::gCharacterModalAction;
        if (clickedAction != nullptr) {
            login::detail::MarkDeath(clickedAction);
            state::gCharacterModalAction = nullptr;
        }
    }

    detail::Sub_503A20(
        &confirmRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        login::detail::gValueD69768,
        std::uint8_t{214});
    detail::Sub_503A20(
        &cancelRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        login::detail::gValueD69768,
        std::uint8_t{214});

    const std::int32_t left = x - 192;
    std::wstring textBuffer;
    std::int32_t textWidth;

    textBuffer = L"＊＊＊ 警  告 ＊＊＊" /* 原55C8B0「＊＊＊ 警  告 ＊＊＊」；20B正文，字面量隐式追加NUL。 */;
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 76),
        2,
        0,
        4,
        textBuffer,
        0,
        0);

    textBuffer = L"所選擇的人物一經刪除" /* 原55C898「所選擇的人物一經刪除」；20B正文，字面量隐式追加NUL。 */;
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 44),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"將無法恢復" /* 原55C88C「將無法恢復」；10B正文，字面量隐式追加NUL。 */;
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 20),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"確定要刪除嗎？" /* 原55C87C「確定要刪除嗎？」；14B正文，字面量隐式追加NUL。 */;
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y + 4),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    return result;
}

} // namespace cg::character_select
