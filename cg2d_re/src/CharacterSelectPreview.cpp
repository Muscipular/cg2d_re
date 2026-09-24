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

namespace detail {

std::int32_t Sub_51BA10(const std::int32_t imageNo)
{
    if (imageNo >= 30000 && imageNo <= 98000) {
        return 0;
    }
    if (imageNo >= 202800 && imageNo <= 204199) {
        return 3;
    }
    if (imageNo >= 200000 && imageNo <= 200799) {
        return 1;
    }
    if (imageNo >= 200800 && imageNo <= 202799) {
        return 2;
    }
    if (imageNo >= 240000 && imageNo < 241400) {
        return 4;
    }
    if (imageNo >= 241400 && imageNo < 242800) {
        return 5;
    }
    if ((imageNo >= 205000 && imageNo <= 205600)
        || (imageNo >= 270000 && imageNo <= 270200)) {
        return 5;
    }
    return 0;
}

std::int32_t Sub_51BB80(std::int32_t imageNo)
{
    if (imageNo < 30000 || imageNo > 97999) {
        return imageNo;
    }
    if (imageNo >= 70000) {
        imageNo -= 12000;
    }

    const std::int32_t delta = imageNo - 30000;
    std::int32_t category = delta / 4000;
    const std::int32_t remainder = delta - 4000 * category;
    const std::int32_t thousands = remainder / 1000;
    const std::int32_t tensRemainder = remainder - 1000 * thousands;
    const std::int32_t tens = tensRemainder / 10;
    const std::int32_t ones = tensRemainder - 10 * tens;
    if (category == 3) {
        category = 4;
    } else if (category == 4) {
        category = 3;
    }

    const std::int32_t base = thousands + 4 * category + 8112;
    return 25 * base + 5 * tens + ones;
}

std::int32_t Sub_51BAC0(const std::int32_t imageNo)
{
    if (imageNo < 202800 || imageNo > 204199) {
        return imageNo;
    }

    const std::int32_t delta = imageNo - 202800;
    std::int32_t category = delta / 100;
    const std::int32_t remainder = delta % 100;
    const std::int32_t quarter = remainder / 25;
    const std::int32_t quarterRemainder = remainder % 25;
    const std::int32_t tens = quarterRemainder / 5;
    const std::int32_t ones = quarterRemainder % 5;
    if (category == 3) {
        category = 4;
    } else if (category == 4) {
        category = 3;
    }

    std::int32_t result =
        1000 * (quarter + 4 * category + 30) + 10 * tens + ones;
    if (result >= 58000) {
        result += 12000;
    }
    return result;
}

std::int32_t Sub_51BD00(const std::int32_t imageNo)
{
    if (imageNo >= 200000 && imageNo <= 200399) {
        return 33100;
    }
    if (imageNo >= 200800 && imageNo <= 201799) {
        return 33100;
    }
    if (imageNo >= 200400 && imageNo <= 200799) {
        return 33101;
    }
    if (imageNo >= 201800 && imageNo <= 202799) {
        return 33101;
    }
    if (imageNo < 240000) {
        return -1;
    }
    if (imageNo < 240700) {
        return 33100;
    }
    if (imageNo < 241400) {
        return 33101;
    }
    if (imageNo < 242100) {
        return 33100;
    }
    if (imageNo < 242800) {
        return 33101;
    }
    return -1;
}

std::int32_t Sub_51BF00(const std::int32_t imageNo)
{
    if (imageNo >= 240000 && imageNo < 241400) {
        return imageNo - 37200;
    }
    if (imageNo >= 205000 && imageNo <= 205600) {
        return imageNo;
    }
    if (imageNo >= 270000 && imageNo <= 270200) {
        return imageNo;
    }

    const std::int32_t index = (imageNo - 241400) / 100;
    const std::int32_t remainder = imageNo % 100;
    const std::int32_t categories[14] {
        200000, 200200, 200800, 201000, 201200, 201400, 201600,
        200400, 200600, 201800, 202000, 202200, 202400, 202600
    };
    return remainder + categories[index];
}

std::int32_t Sub_51BFF0(const std::int32_t imageNo)
{
    if (imageNo >= 202800 && imageNo <= 204200) {
        return imageNo + 37200;
    }

    const std::int32_t category = imageNo / 100;
    const std::array<std::int32_t, 14> categories {
        2000, 2002, 2008, 2010, 2012, 2014, 2016,
        2004, 2006, 2018, 2020, 2022, 2024, 2026
    };
    for (std::size_t index = 0; index < categories.size(); ++index) {
        if (category == categories[index]) {
            return imageNo % 100
                + 20 * (5 * static_cast<std::int32_t>(index) + 12070);
        }
    }
    return imageNo;
}

std::int32_t* Sub_4C56E0(
    const std::int32_t imageNo,
    std::int32_t* const outBase,
    std::int32_t* const outClass,
    std::int32_t* const outQuarter,
    std::int32_t* const outFifth,
    std::int32_t* const outOnes)
{
    const std::int32_t imageClass = Sub_51BA10(imageNo);
    *outClass = 0;
    if (imageClass >= 1) {
        *outClass = 1;
    }
    if (imageClass >= 4) {
        *outClass = 2;
    }

    const std::int32_t converted = Sub_51BFF0(Sub_51BB80(imageNo));
    const std::int32_t ones = converted % 5;
    *outOnes = ones;
    const std::int32_t withoutOnes = converted - ones;
    const std::int32_t fifth = (withoutOnes / 5) % 5;
    *outFifth = fifth;
    const std::int32_t withoutFifth = withoutOnes - 5 * fifth;
    const std::int32_t quarter = (withoutFifth / 25) % 4;
    *outQuarter = quarter;
    *outBase = withoutFifth - 25 * quarter;
    return outBase;
}

} // namespace detail

void ProcessCharacterPreview_460D00(std::uint32_t slot)
{
    const std::size_t slotIndex = static_cast<std::size_t>(slot);
    login::YobiAction* const actionToDestroy =
        state::gPreviewActions[slotIndex];
    if (actionToDestroy != nullptr) {
        login::detail::MarkDeath(actionToDestroy);
        state::gPreviewActions[slotIndex] = nullptr;
    }

    if (detail::Sub_51B110(slot) == 0) {
        return;
    }

    login::YobiAction* const existingAction =
        state::gPreviewActions[slotIndex];
    if (existingAction != nullptr) {
        return;
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

    login::YobiAction* const action = login::detail::NewYobiAction_404AE0(
        std::uint8_t{2},
        std::uint32_t{0});
    state::gPreviewActions[slotIndex] = action;
    if (action == nullptr) {
        return;
    }

    const std::int32_t graphicIndex = SelectGraphicIndex(converted);
    const std::int32_t normalizedGraphic =
        ReadRecordInt32(record, kGraphicNoOffset);
    if (normalizedGraphic == 33100) {
        login::detail::SetYobiActionField2A8(action, 100900);
    } else if (normalizedGraphic == 33101) {
        login::detail::SetYobiActionField2A8(action, 100901);
    } else {
        login::detail::SetYobiActionField2A8(
            action,
            graphics[static_cast<std::size_t>(graphicIndex)]);
        const std::int32_t baseGraphic =
            login::detail::GetYobiActionField2A8(action);
        login::detail::SetYobiActionField2A8(
            action,
            baseGraphic + 6 * ((converted % 100) / 25));
    }

    login::detail::SetYobiActionField2B0(action, 0);
    login::detail::SetYobiActionField15(action, 10);
    login::detail::SetYobiActionField30(
        action,
        315 * static_cast<std::int32_t>(slot) + 163);
    login::detail::SetYobiActionField34(action, 292);
    login::detail::SetYobiActionField2B8(action, 5);

    login::YobiAction* const actionForFrameMove =
        state::gPreviewActions[slotIndex];
    login::detail::Sub_517880(actionForFrameMove, 100, 0, 0);
}

} // namespace cg::character_select
