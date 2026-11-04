#include "cg/session/ClientSession.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectSceneState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
// 原函数：0x0045DEE0。
std::int32_t ProcessConfirmationDialog_45DEE0(
    network::CgConnection& connection,
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

    auto& actionSlot = connection.CharacterSelectUi().characterModalAction;
    cg::yobi::YobiAction* modalAction = actionSlot;
    if (modalAction == nullptr) {
        modalAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), left,
            top,
            width,
            height,
            4,
            1);
        actionSlot = modalAction;
        if (modalAction == nullptr) {
            return 0;
        }
    }

    if (cg::yobi::IsYobiActionStateLessOrEqual(
            modalAction,
            0)) {
        return 0;
    }

    if (cg::ui::HitTestMouseRect_5049A0(connection.Owner(),
            x - 73,
            top + 59,
            left + 158,
            top + 76,
            -1)
        != 0) {
        hover = 1;
    }
    if (cg::ui::HitTestMouseRect_5049A0(connection.Owner(),
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
            static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(connection.Owner()));
        if ((mouseClickState & 1U) != 0) {
            result = hover;
            login::detail::PlaySe(79, 320, 240);
            cg::yobi::YobiAction* const clickedAction = actionSlot;
            if (clickedAction != nullptr) {
                cg::yobi::MarkDeath(clickedAction);
                actionSlot = nullptr;
            }
        }
    }

    cg::render::DrawGraphic(
        x - 40,
        top + 67,
        std::uint8_t{223},
        hover == 1 ? 245325 : 245323,
        0,
        nullptr);
    cg::render::DrawGraphic(
        x + 40,
        top + 67,
        std::uint8_t{223},
        hover == 2 ? 245328 : 245326,
        0,
        nullptr);

    const std::int32_t textWidth = cg::render::TextMeasure(text, 0);
    cg::render::DrawText_456780(
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
    network::CgConnection& connection,
    std::int32_t x,
    std::int32_t y,
    std::wstring_view text,
    std::int8_t color)
{
    return ProcessConfirmationDialog_45DEE0(connection, x, y, text, color, 0, 0);
}

// 原函数：0x0045E0C0。
std::int32_t ProcessCharacterDeletionWarning_45E0C0(
    network::CgConnection& connection,
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
        L"  确  定 " /* 原55C8D4「  確  定 」；9B正文，字面量隐式追加NUL。 */,
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

    if (cg::yobi::IsYobiActionStateLessOrEqual(
            modalAction,
            0)) {
        return 0;
    }

    if ((detail::ReadCharacterModalButtonMouseState_503990(connection.Owner(), &confirmRecord) & 1) != 0) {
        result = 1;
    } else if ((detail::ReadCharacterModalButtonMouseState_503990(connection.Owner(), &cancelRecord) & 1) != 0) {
        result = 2;
    }

    if (result != 0) {
        login::detail::PlaySe(79, 320, 240);
        cg::yobi::YobiAction* const clickedAction = actionSlot;
        if (clickedAction != nullptr) {
            cg::yobi::MarkDeath(clickedAction);
            actionSlot = nullptr;
        }
    }

    detail::DrawCharacterModalButton_503A20(connection.Owner(),
        &confirmRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        cg::render::gHoverFramePaletteIndex_D69768,
        std::uint8_t{214});
    detail::DrawCharacterModalButton_503A20(connection.Owner(),
        &cancelRecord,
        std::uint8_t{2},
        std::int8_t{4},
        std::int32_t{1},
        cg::render::gHoverFramePaletteIndex_D69768,
        std::uint8_t{214});

    const std::int32_t left = x - 192;
    std::wstring textBuffer;
    std::int32_t textWidth;

    textBuffer = L"＊＊＊ 警  告 ＊＊＊" /* 原55C8B0「＊＊＊ 警  告 ＊＊＊」；20B正文，字面量隐式追加NUL。 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 76),
        2,
        0,
        4,
        textBuffer,
        0,
        0);

    textBuffer = L"所选择的人物一经删除" /* 原55C898「所選擇的人物一經刪除」；20B正文，字面量隐式追加NUL。 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 44),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"将无法恢复" /* 原55C88C「將無法恢復」；10B正文，字面量隐式追加NUL。 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
        static_cast<std::int16_t>(left + (384 - textWidth) / 2),
        static_cast<std::int16_t>(y - 20),
        2,
        0,
        0,
        textBuffer,
        0,
        0);

    textBuffer = L"确定要删除吗？" /* 原55C87C「確定要刪除嗎？」；14B正文，字面量隐式追加NUL。 */;
    textWidth = cg::render::TextMeasure(textBuffer, 0);
    cg::render::DrawText_456780(
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
