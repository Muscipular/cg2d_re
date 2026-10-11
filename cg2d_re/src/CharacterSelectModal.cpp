#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr char kReturnText[] = "\xB7\xB5\xBB\xD8"; // 原地址：0x55CE04
constexpr char kCannotDeleteFamilyLeader[] = "\x9F\x6F\xB7\xA8\x84\x68\xB3\xFD\xBC\xD2\xD7\xE5\xD8\x93\xD8\x9F\xC8\xCB\xA1\xA3"; // 原地址：0x55CDEC
constexpr char kDisbandFamilyFirst[] = "\xC8\xF4\xD2\xAA\xBD\xE2\xC9\xA2\xBC\xD2\xD7\xE5\xA3\xAC"; // 原地址：0x55CDDC
constexpr char kLeaveFamilyFirst[] = "\xD5\x88\xD4\xDA\xCD\xCB\xB3\xF6\xB4\xCB\xBC\xD2\xD7\xE5\xD6\xAE\xE1\xE1\xA3\xAC"; // 原地址：0x55CDC4
constexpr char kDeleteAfterLeaving[] = "\xD4\xD9\xDF\x4D\xD0\xD0\x84\x68\xB3\xFD\xA1\xA3"; // 原地址：0x55CDB4
static_assert(sizeof(kReturnText) == 5);
static_assert(sizeof(kCannotDeleteFamilyLeader) == 21);
static_assert(sizeof(kDisbandFamilyFirst) == 15);
static_assert(sizeof(kLeaveFamilyFirst) == 21);
static_assert(sizeof(kDeleteAfterLeaving) == 13);

} // namespace

// 原函数：0x00462090。
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

    login::YobiAction* modalAction = state::gCharacterModalAction;
    if (modalAction == nullptr) {
        modalAction = login::detail::Sub_5044E0(
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

    if (login::detail::IsYobiActionStateLessOrEqual(modalAction, 0)) {
        return 0;
    }

    if ((detail::Sub_503990(&uiRecord) & 1) != 0) {
        result = 1;
        login::detail::PlaySe(79, 320, 240);
        login::YobiAction* const actionToDestroy =
            state::gCharacterModalAction;
        if (actionToDestroy != nullptr) {
            login::detail::MarkDeath(actionToDestroy);
            state::gCharacterModalAction = nullptr;
        }
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
