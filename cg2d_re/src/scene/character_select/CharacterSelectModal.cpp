#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectSceneState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
// 原函数：0x00462090。
std::int32_t ProcessCharacterModal_462090(
    network::CgConnection& connection,
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

    auto& actionSlot = connection.CharacterSelectUi().characterModalAction;
    cg::yobi::YobiAction* modalAction = actionSlot;
    if (modalAction == nullptr) {
        modalAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), x - 192,
            y - 96,
            384,
            192,
            4,
            1);
        actionSlot = modalAction;
        if (modalAction == nullptr) {
            return 0;
        }
    }

    if (cg::yobi::IsYobiActionStateLessOrEqual(modalAction, 0)) {
        return 0;
    }

    if ((detail::ReadCharacterModalButtonMouseState_503990(connection.Owner(), &uiRecord) & 1) != 0) {
        result = 1;
        login::detail::PlaySe(connection.Owner(), 79, 320, 240);
        cg::yobi::YobiAction* const actionToDestroy =
            actionSlot;
        if (actionToDestroy != nullptr) {
            cg::yobi::MarkDeath(actionToDestroy);
            actionSlot = nullptr;
        }
    }

    detail::DrawCharacterModalButton_503A20(connection.Owner(),
        &uiRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        connection.Owner().UiFrame().hoverFramePaletteIndex,
        std::uint8_t{214});

    std::wstring textBuffer;
    std::int32_t textWidth;

    textBuffer = L"无法删除家族负责人。" /* 原地址：0x55CDEC */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 76),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"若要解散家族，" /* 原地址：0x55CDDC */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 44),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"请在退出此家族之后，" /* 原地址：0x55CDC4 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(x - 192 + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 20),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"再进行删除。" /* 原地址：0x55CDB4 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
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
