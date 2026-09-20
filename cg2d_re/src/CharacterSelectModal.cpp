#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr char kReturnText[] = "返回";
constexpr char kCannotDeleteFamilyLeader[] = "無法刪除家族負責人。";
constexpr char kDisbandFamilyFirst[] = "若要解散家族，";
constexpr char kLeaveFamilyFirst[] = "請在退出此家族之後，";
constexpr char kDeleteAfterLeaving[] = "再進行刪除。";

} // namespace

std::int32_t ProcessCharacterModal_462090(
    std::int32_t x,
    std::int32_t y)
{
    detail::CharacterModalUiRecord uiRecord {
        x - 27,
        y + 48,
        x - 29,
        y + 48,
        54,
        18,
        kReturnText,
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

    login::YobiAction* const action = state::gCharacterModalAction;
    if (!login::detail::IsYobiActionReady(action, 1)) {
        return 0;
    }

    if ((detail::Sub_503990(&uiRecord) & 1) != 0) {
        result = 1;
        login::detail::PlaySe(79, 320, 240);
        login::YobiAction* const actionToDestroy =
            state::gCharacterModalAction;
        if (actionToDestroy != nullptr) {
            login::detail::MarkDeath(actionToDestroy);
        }
        state::gCharacterModalAction = nullptr;
    }

    detail::Sub_503A20(
        &uiRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        login::detail::gValueD69768,
        std::uint8_t{214});

    char textBuffer[256];
    std::int32_t textWidth;

    std::strcpy(textBuffer, kCannotDeleteFamilyLeader);
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 76),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    std::strcpy(textBuffer, kDisbandFamilyFirst);
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 44),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    std::strcpy(textBuffer, kLeaveFamilyFirst);
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 20),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    std::strcpy(textBuffer, kDeleteAfterLeaving);
    textWidth = login::detail::TextMeasure(textBuffer, 0);
    login::detail::DrawText(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
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
