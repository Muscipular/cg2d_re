#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/LoginRequest.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr std::size_t kGraphicNoOffset = 0x18;
constexpr std::size_t kPreviewStateOffset = 0x50;
constexpr std::size_t kPreviewGraphicCount = 14;

static_assert(kGraphicNoOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kPreviewStateOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);

constexpr std::array<std::int32_t, kPreviewGraphicCount> kState0Graphics {{
    100002, 100027, 100052, 100077, 100102, 100127, 100152,
    100252, 100277, 100302, 100327, 100352, 100377, 100402,
}};

constexpr std::array<std::int32_t, kPreviewGraphicCount> kState1Graphics {{
    105002, 105027, 105052, 105077, 105102, 105127, 105152,
    105252, 105277, 105302, 105327, 105352, 105377, 105402,
}};

constexpr std::array<std::int32_t, kPreviewGraphicCount> kState2Graphics {{
    106002, 106027, 106052, 106077, 106102, 106127, 106152,
    106252, 106277, 106302, 106327, 106352, 106377, 106402,
}};

constexpr std::array<std::int32_t, kPreviewGraphicCount> kState3Graphics {{
    105502, 105527, 104402, 104432, 104462, 104492, 120102,
    100900, 100900, 100900, 120126, 100900, 100900, 100900,
}};

std::int32_t ReadRecordInt32(
    const login::CharacterRecordBytes& record,
    std::size_t offset)
{
    std::int32_t value;
    std::memcpy(&value, record.data() + offset, sizeof(value));
    return value;
}

void WriteRecordInt32(
    login::CharacterRecordBytes& record,
    std::size_t offset,
    std::int32_t value)
{
    std::memcpy(record.data() + offset, &value, sizeof(value));
}

const std::array<std::int32_t, kPreviewGraphicCount>& SelectGraphicTable(
    std::int32_t previewState)
{
    switch (previewState) {
    case 0:
        return kState0Graphics;
    case 1:
        return kState1Graphics;
    case 3:
        return kState3Graphics;
    case 2:
    default:
        return kState2Graphics;
    }
}

std::int32_t SelectGraphicIndex(std::int32_t converted)
{
    if (converted >= 270000 && converted <= 270200) {
        const std::int32_t quotient = (converted - 270000) / 100;
        if (quotient == 0) {
            return 6;
        }
        if (quotient == 1) {
            return 10;
        }
        return quotient;
    }
    if (converted >= 205000 && converted <= 205600) {
        return (converted - 205000) / 100;
    }
    if (converted >= 241400) {
        return (converted - 241400) / 100;
    }
    return (converted - 240000) / 100;
}

} // namespace

std::int32_t ProcessCharacterPreview_460D00(std::uint32_t slot)
{
    const std::size_t slotIndex = static_cast<std::size_t>(slot);
    login::YobiAction* const actionToDestroy =
        state::gPreviewActions[slotIndex];
    if (actionToDestroy != nullptr) {
        login::detail::MarkDeath(actionToDestroy);
        state::gPreviewActions[slotIndex] = nullptr;
    }

    if (detail::Sub_51B110(slot) == 0) {
        return 0;
    }

    login::YobiAction* const existingAction =
        state::gPreviewActions[slotIndex];
    if (existingAction != nullptr) {
        return detail::GetYobiActionLegacyPointerBits32(existingAction);
    }

    login::CharacterRecordBytes& record = login::gCharacterRecords[slotIndex];
    if (ReadRecordInt32(record, kGraphicNoOffset) < 30000) {
        WriteRecordInt32(record, kGraphicNoOffset, 33100);
    }

    const std::int32_t converted = detail::Sub_51BFF0(
        detail::Sub_51BB80(ReadRecordInt32(record, kGraphicNoOffset)));

    const std::int32_t oldPreviewState =
        ReadRecordInt32(record, kPreviewStateOffset);
    if (oldPreviewState < 0) {
        WriteRecordInt32(
            record,
            kPreviewStateOffset,
            converted >= 240000 && converted < 241400 ? 1 : 2);
    } else {
        if (converted >= 241400) {
            WriteRecordInt32(record, kPreviewStateOffset, 2);
        }
        if ((converted >= 205000 && converted <= 205600)
            || (converted >= 270000 && converted <= 270200)) {
            WriteRecordInt32(record, kPreviewStateOffset, 3);
        }
    }

    const std::int32_t previewState =
        ReadRecordInt32(record, kPreviewStateOffset);
    const std::array<std::int32_t, kPreviewGraphicCount>& graphics =
        SelectGraphicTable(previewState);

    login::YobiAction* const action = detail::NewYobiAction_404AE0(
        std::uint8_t{2},
        std::uint32_t{0});
    state::gPreviewActions[slotIndex] = action;
    if (action == nullptr) {
        return 0;
    }

    const std::int32_t graphicIndex = SelectGraphicIndex(converted);
    const std::int32_t normalizedGraphic =
        ReadRecordInt32(record, kGraphicNoOffset);
    if (normalizedGraphic == 33100) {
        detail::SetYobiActionField2A8(action, 100900);
    } else if (normalizedGraphic == 33101) {
        detail::SetYobiActionField2A8(action, 100901);
    } else {
        detail::SetYobiActionField2A8(
            action,
            graphics[static_cast<std::size_t>(graphicIndex)]);
        const std::int32_t baseGraphic =
            detail::GetYobiActionField2A8(action);
        detail::SetYobiActionField2A8(
            action,
            baseGraphic + 6 * ((converted % 100) / 25));
    }

    detail::SetYobiActionField2B0(action, 0);
    detail::SetYobiActionField15(action, 10);
    detail::SetYobiActionField30(
        action,
        315 * static_cast<std::int32_t>(slot) + 163);
    detail::SetYobiActionField34(action, 292);
    detail::SetYobiActionField2B8(action, 5);

    login::YobiAction* const actionForFrameMove =
        state::gPreviewActions[slotIndex];
    return detail::Sub_517880(actionForFrameMove, 100, 0, 0);
}

} // namespace cg::character_select
