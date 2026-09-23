#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr char kReturnText[] = "\xAA\xF0\xA6\x5E"; // 原地址：0x55CE04
constexpr char kCannotDeleteFamilyLeader[] = "\xB5\x4C\xAA\x6B\xA7\x52\xB0\xA3\xAE\x61\xB1\xDA\xAD\x74\xB3\x64\xA4\x48\xA1\x43"; // 原地址：0x55CDEC
constexpr char kDisbandFamilyFirst[] = "\xAD\x59\xAD\x6E\xB8\xD1\xB4\xB2\xAE\x61\xB1\xDA\xA1\x41"; // 原地址：0x55CDDC
constexpr char kLeaveFamilyFirst[] = "\xBD\xD0\xA6\x62\xB0\x68\xA5\x58\xA6\xB9\xAE\x61\xB1\xDA\xA4\xA7\xAB\xE1\xA1\x41"; // 原地址：0x55CDC4
constexpr char kDeleteAfterLeaving[] = "\xA6\x41\xB6\x69\xA6\xE6\xA7\x52\xB0\xA3\xA1\x43"; // 原地址：0x55CDB4
static_assert(sizeof(kReturnText) == 5);
static_assert(sizeof(kCannotDeleteFamilyLeader) == 21);
static_assert(sizeof(kDisbandFamilyFirst) == 15);
static_assert(sizeof(kLeaveFamilyFirst) == 21);
static_assert(sizeof(kDeleteAfterLeaving) == 13);

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
