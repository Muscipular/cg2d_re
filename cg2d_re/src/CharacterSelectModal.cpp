#include "cg/CharacterSelectScene.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
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
        L"返回" /* 原地址：0x55CE04 */,
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

    std::wstring textBuffer;
    std::int32_t textWidth;

    textBuffer = L"無法刪除家族負責人。" /* 原地址：0x55CDEC */;
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

    textBuffer = L"若要解散家族，" /* 原地址：0x55CDDC */;
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

    textBuffer = L"請在退出此家族之後，" /* 原地址：0x55CDC4 */;
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

    textBuffer = L"再進行刪除。" /* 原地址：0x55CDB4 */;
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
