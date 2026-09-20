#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr char kConfirmText[] = "  確  定 ";
constexpr char kCancelText[] = "  取  消 ";
constexpr char kWarningTitle[] = "＊＊＊ 警  告 ＊＊＊";
constexpr char kDeletionCannotRecoverLine1[] = "所選擇的人物一經刪除";
constexpr char kDeletionCannotRecoverLine2[] = "將無法恢復";
constexpr char kDeletionQuestion[] = "確定要刪除嗎？";

} // namespace

std::int32_t ProcessConfirmationDialog_45DEE0(
    std::int32_t x,
    std::int32_t y,
    const char* text,
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

    if (state::gCharacterModalAction == nullptr) {
        state::gCharacterModalAction = login::detail::Sub_5044E0(
            left,
            top,
            width,
            height,
            4,
            1);
        if (state::gCharacterModalAction == nullptr) {
            return 0;
        }
    }

    if (login::detail::IsYobiActionStateLessOrEqual(
            state::gCharacterModalAction,
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
            if (state::gCharacterModalAction != nullptr) {
                login::detail::MarkDeath(state::gCharacterModalAction);
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

std::int32_t ProcessDefaultConfirmationDialog_45E090(
    std::int32_t x,
    std::int32_t y,
    const char* text,
    std::int8_t color)
{
    return ProcessConfirmationDialog_45DEE0(x, y, text, color, 0, 0);
}

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
        kConfirmText,
    };
    detail::CharacterModalUiRecord cancelRecord {
        x + 28,
        y + 48,
        x + 26,
        y + 48,
        90,
        18,
        kCancelText,
    };
    std::int32_t result = 0;

    if (state::gCharacterModalAction == nullptr) {
        state::gCharacterModalAction = login::detail::Sub_5044E0(
            x - 192,
            y - 96,
            384,
            192,
            4,
            1);
        if (state::gCharacterModalAction == nullptr) {
            return 0;
        }
    }

    if (login::detail::IsYobiActionStateLessOrEqual(
            state::gCharacterModalAction,
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
        if (state::gCharacterModalAction != nullptr) {
            login::detail::MarkDeath(state::gCharacterModalAction);
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
    char textBuffer[256];
    std::int32_t textWidth;

    std::strcpy(textBuffer, kWarningTitle);
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

    std::strcpy(textBuffer, kDeletionCannotRecoverLine1);
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

    std::strcpy(textBuffer, kDeletionCannotRecoverLine2);
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

    std::strcpy(textBuffer, kDeletionQuestion);
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
