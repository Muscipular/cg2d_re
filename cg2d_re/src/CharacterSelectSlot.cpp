#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/LoginRequest.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr std::size_t kNameOffset = 0x00;
constexpr std::size_t kValue12Offset = 0x12;
constexpr std::size_t kValue14Offset = 0x14;
constexpr std::size_t kGraphicNoOffset = 0x18;
constexpr std::size_t kToggleOffset = 0x50;
constexpr std::size_t kText54Offset = 0x54;

constexpr std::size_t kUiXIndex = 0;
constexpr std::size_t kUiYIndex = 1;
constexpr std::size_t kUiCount = 7;
constexpr std::int32_t kSlotWidth = 315;
constexpr std::int32_t kSlotLeft = 10;
constexpr std::int32_t kSlotTop = 51;

static_assert(kValue12Offset + sizeof(std::int16_t) <= login::CharacterRecordSize);
static_assert(kValue14Offset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kGraphicNoOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kToggleOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kNameOffset < login::CharacterRecordSize);
static_assert(kText54Offset < login::CharacterRecordSize);

struct SlotCoordinate {
    std::int32_t x;
    std::int32_t y;
};

constexpr std::array<detail::CharacterSelectUiRecord, kUiCount> kUiTemplates {{
    {{119, 270, 64, 16, 245069, 245071, 245070}},
    {{82, 270, 64, 16, 245075, 245077, 245076}},
    {{156, 270, 64, 16, 245072, 245074, 245073}},
    {{161, 90, 12, 16, 245066, 245068, 245067}},
    {{238, 90, 12, 16, 245063, 245065, 245064}},
    {{55, 154, 12, 16, 245066, 245068, 245067}},
    {{240, 154, 12, 16, 245063, 245065, 245064}},
}};

constexpr std::array<SlotCoordinate, kUiCount> kCoordinateTemplates {{
    {62, 48},
    {104, 88},
    {180, 109},
    {98, 127},
    {172, 30},
    {153, 241},
    {53, 27},
}};

std::int16_t ReadRecordInt16(
    const login::CharacterRecordBytes& record,
    std::size_t offset)
{
    std::int16_t value;
    std::memcpy(&value, record.data() + offset, sizeof(value));
    return value;
}

std::int32_t ReadRecordInt32(
    const login::CharacterRecordBytes& record,
    std::size_t offset)
{
    std::int32_t value;
    std::memcpy(&value, record.data() + offset, sizeof(value));
    return value;
}

std::int32_t IncrementWrapping(std::int32_t value)
{
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(value) + 1u;
    return std::bit_cast<std::int32_t>(bits);
}

} // namespace

std::int32_t ProcessCharacterSlot_461B30(
    std::uint32_t slot,
    std::int32_t mode)
{
    std::array<detail::CharacterSelectUiRecord, kUiCount> uiRecords =
        kUiTemplates;
    std::array<SlotCoordinate, kUiCount> coordinates = kCoordinateTemplates;

    const std::int32_t baseX =
        kSlotWidth * static_cast<std::int32_t>(slot) + kSlotLeft;
    for (detail::CharacterSelectUiRecord& uiRecord : uiRecords) {
        uiRecord.values[kUiXIndex] += baseX;
        uiRecord.values[kUiYIndex] += kSlotTop;
    }
    for (SlotCoordinate& coordinate : coordinates) {
        coordinate.x += baseX;
        coordinate.y += kSlotTop;
    }

    std::int32_t result = -1;
    std::int32_t style;
    const login::CharacterRecordBytes& record =
        login::gCharacterRecords[static_cast<std::size_t>(slot)];

    if (detail::Sub_51B110(slot) != 0) {
        style = mode;
        for (std::int32_t item = 1; item < 3; ++item) {
            detail::CharacterSelectUiRecord& uiRecord =
                uiRecords[static_cast<std::size_t>(item)];
            detail::Sub_45EFD0(&uiRecord, 5, 0, 0, mode);
            if (mode == 0 && (detail::Sub_45F100(&uiRecord) & 1) != 0) {
                result = item;
            }
        }

        std::int8_t color;
        const std::uint8_t pukVersion = state::gPukVersion;
        if (pukVersion == 1u || pukVersion == 2u) {
            color = 3;
        } else if (pukVersion < 5u) {
            color = 0;
        } else {
            color = 8;
        }

        detail::Sub_45F090(
            coordinates[4].x,
            coordinates[4].y,
            10,
            ReadRecordInt32(record, kGraphicNoOffset),
            0,
            -1);

        const char* const name =
            reinterpret_cast<const char*>(record.data() + kNameOffset);
        login::detail::DrawText(
            static_cast<std::int16_t>(coordinates[0].x),
            static_cast<std::int16_t>(coordinates[0].y),
            0,
            9,
            color,
            name,
            0,
            0);

        char buffer[256];
        std::sprintf(buffer, "%4d", ReadRecordInt16(record, kValue12Offset));
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[1].x),
            static_cast<std::int16_t>(coordinates[1].y),
            0,
            9,
            color,
            buffer,
            0);

        std::int32_t value14 = ReadRecordInt32(record, kValue14Offset);
        if (value14 >= 10000) {
            value14 = 9999;
        }
        std::sprintf(buffer, "%4d", value14);
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[2].x),
            static_cast<std::int16_t>(coordinates[2].y),
            0,
            9,
            color,
            buffer,
            0);

        const char* const text54 =
            reinterpret_cast<const char*>(record.data() + kText54Offset);
        std::sprintf(buffer, "%s", text54);
        render::DrawText2_456670(
            static_cast<std::int16_t>(coordinates[3].x),
            static_cast<std::int16_t>(coordinates[3].y),
            0,
            9,
            color,
            buffer,
            0);

        login::YobiAction* const actionForFieldWrite =
            state::gPreviewActions[static_cast<std::size_t>(slot)];
        if (actionForFieldWrite != nullptr) {
            login::detail::SetYobiActionField2B0(actionForFieldWrite, 1);
            login::YobiAction* const actionForFrameMove =
                state::gPreviewActions[static_cast<std::size_t>(slot)];
            login::detail::Sub_517880(actionForFrameMove, 100, 0, 0);
        }
    } else {
        detail::Sub_45EFD0(&uiRecords[0], 5, 0, 0, mode);
        if ((detail::Sub_45F100(&uiRecords[0]) & 1) != 0) {
            result = 0;
        }
        style = IncrementWrapping(mode);
    }

    login::detail::DrawText3(30, 450, 0, 4, detail::gText9A4EAC, 0);

    if (state::gPukVersion >= 5u && detail::Sub_51B110(slot) != 0) {
        for (std::int32_t item = 3; item < 7; ++item) {
            const std::uint32_t toggle = static_cast<std::uint32_t>(
                ReadRecordInt32(record, kToggleOffset));
            const bool enabled = item < 5
                ? toggle < 2u
                    || (state::gPukVersion >= 7u && toggle != 3u)
                : toggle <= 1u;
            if (!enabled) {
                continue;
            }

            detail::CharacterSelectUiRecord& uiRecord =
                uiRecords[static_cast<std::size_t>(item)];
            detail::Sub_45EFD0(&uiRecord, 5, 0, 0, style);
            if (style == 0 && (detail::Sub_45F100(&uiRecord) & 1) != 0) {
                result = item;
            }
        }
    }

    detail::Sub_45F090(
        coordinates[6].x,
        coordinates[6].y,
        4,
        245062,
        0,
        -1);
    detail::Sub_45F090(baseX, 51, 4, 245061, 0, -1);
    return result;
}

} // namespace cg::character_select
