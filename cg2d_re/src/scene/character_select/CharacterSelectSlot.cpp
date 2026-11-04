#include "cg/render/TextPendingQueue.hpp"
#include "cg/network/CgConnection.hpp"
#include "CharacterSelectSceneState.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/text/Encoding.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/scene/login/LoginRequest.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::character_select {

static constexpr std::size_t kUiCount = 7;
static constexpr std::int32_t kSlotWidth = 315;
static constexpr std::int32_t kSlotLeft = 10;
static constexpr std::int32_t kSlotTop = 51;

struct SlotCoordinate {
    std::int32_t x;
    std::int32_t y;
};

static constexpr std::array<detail::CharacterSelectUiRecord, kUiCount> kUiTemplates {{
    {{119, 270, 64, 16}, 245069, 245071, 245070},
    {{82, 270, 64, 16}, 245075, 245077, 245076},
    {{156, 270, 64, 16}, 245072, 245074, 245073},
    {{161, 90, 12, 16}, 245066, 245068, 245067},
    {{238, 90, 12, 16}, 245063, 245065, 245064},
    {{55, 154, 12, 16}, 245066, 245068, 245067},
    {{240, 154, 12, 16}, 245063, 245065, 245064},
}};

static constexpr std::array<SlotCoordinate, kUiCount> kCoordinateTemplates {{
    {62, 48},
    {104, 88},
    {180, 109},
    {98, 127},
    {172, 30},
    {153, 241},
    {53, 27},
}};

// 原函数：0x00461B30。
std::int32_t ProcessCharacterSlot_461B30(
    network::CgConnection& connection,
    std::uint32_t slot,
    std::int32_t mode)
{
    std::array<detail::CharacterSelectUiRecord, kUiCount> uiRecords =
        kUiTemplates;
    std::array<SlotCoordinate, kUiCount> coordinates = kCoordinateTemplates;

    const std::int32_t baseX =
        kSlotWidth * static_cast<std::int32_t>(slot) + kSlotLeft;
    for (detail::CharacterSelectUiRecord& uiRecord : uiRecords) {
        uiRecord.bounds.left += baseX;
        uiRecord.bounds.top += kSlotTop;
    }
    for (SlotCoordinate& coordinate : coordinates) {
        coordinate.x += baseX;
        coordinate.y += kSlotTop;
    }

    std::int32_t result = -1;
    std::int32_t style;
    const login::CharacterRecord& record =
        connection.LoginRequests().characters[static_cast<std::size_t>(slot)];

    if (cg::login::HasCharacterInSlot_51B110(connection, slot) != 0) {
        style = mode;
        for (std::int32_t item = 1; item < 3; ++item) {
            detail::CharacterSelectUiRecord& uiRecord =
                uiRecords[static_cast<std::size_t>(item)];
            cg::ui::DrawUiControl_45EFD0(&uiRecord, 5, 0, 0, mode);
            if (mode == 0 && (cg::ui::QueryUiControlInputState_45F100(&uiRecord) & 1) != 0) {
                result = item;
            }
        }

        std::int8_t color;
        const std::uint8_t pukVersion = connection.PukVersion();
        if (pukVersion == 1u || pukVersion == 2u) {
            color = 3;
        } else if (pukVersion < 5u) {
            color = 0;
        } else {
            color = 8;
        }

        cg::render::QueueOriginalPaletteGraphic_45F090(
            coordinates[4].x,
            coordinates[4].y,
            10,
            record.graphicNo,
            0,
            -1);

        const wchar_t* const name = record.name.data();
        cg::render::DrawText_456780(
            static_cast<std::int16_t>(coordinates[0].x),
            static_cast<std::int16_t>(coordinates[0].y),
            0,
            9,
            color,
            name,
            0,
            0);

        std::wstring buffer;
        cg::text::FormatTo(buffer, L"{:4}", record.value12);
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[1].x),
            static_cast<std::int16_t>(coordinates[1].y),
            0,
            9,
            color,
            buffer,
            0);

        std::int32_t value14 = record.value14;
        if (value14 >= 10000) {
            value14 = 9999;
        }
        cg::text::FormatTo(buffer, L"{:4}", value14);
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[2].x),
            static_cast<std::int16_t>(coordinates[2].y),
            0,
            9,
            color,
            buffer,
            0);

        const wchar_t* const text54 = record.jobText.data();
        cg::text::FormatTo(buffer, L"{}", text54);
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[3].x),
            static_cast<std::int16_t>(coordinates[3].y),
            0,
            9,
            color,
            buffer,
            0);

        cg::yobi::YobiAction* const actionForFieldWrite =
            connection.CharacterSelectUi().previewActions[static_cast<std::size_t>(slot)];
        if (actionForFieldWrite != nullptr) {
            cg::yobi::SetYobiActionField2B0(actionForFieldWrite, 1);
            cg::yobi::YobiAction* const actionForFrameMove =
                connection.CharacterSelectUi().previewActions[static_cast<std::size_t>(slot)];
            cg::yobi::AdvanceYobiAnimationFrame_517880(actionForFrameMove, 100, 0, 0);
        }
    } else {
        cg::ui::DrawUiControl_45EFD0(&uiRecords[0], 5, 0, 0, mode);
        if ((cg::ui::QueryUiControlInputState_45F100(&uiRecords[0]) & 1) != 0) {
            result = 0;
        }
        style = mode + 1;
    }

    cg::render::DrawText3_456560(30, 450, 0, 4,
        connection.CharacterSelectUi().loginNotice, 0);

    if (connection.PukVersion() >= 5u && cg::login::HasCharacterInSlot_51B110(connection, slot) != 0) {
        for (std::int32_t item = 3; item < 7; ++item) {
            const std::uint32_t toggle = static_cast<std::uint32_t>(
                record.previewState);
            const bool enabled = item < 5
                ? toggle < 2u
                    || (connection.PukVersion() >= 7u && toggle != 3u)
                : toggle <= 1u;
            if (!enabled) {
                continue;
            }

            detail::CharacterSelectUiRecord& uiRecord =
                uiRecords[static_cast<std::size_t>(item)];
            cg::ui::DrawUiControl_45EFD0(&uiRecord, 5, 0, 0, style);
            if (style == 0 && (cg::ui::QueryUiControlInputState_45F100(&uiRecord) & 1) != 0) {
                result = item;
            }
        }
    }

    cg::render::QueueOriginalPaletteGraphic_45F090(
        coordinates[6].x,
        coordinates[6].y,
        4,
        245062,
        0,
        -1);
    cg::render::QueueOriginalPaletteGraphic_45F090(baseX, 51, 4, 245061, 0, -1);
    // A08：绘制完成后返回原选择动作，由场景既有分支继续入场。
    if (connection.Owner().WorldScene().sceneState == 11 && record.name[0] != '\0'
        && platform::ConsumeAutoLoginCharacterSlot(slot)) {
        connection.LoginRequests().selectedCharacterIndex = static_cast<std::int16_t>(slot);
        result = 1;
    }
    return result;
}

} // namespace cg::character_select
