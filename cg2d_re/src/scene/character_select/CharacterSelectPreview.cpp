#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/scene/login/LoginRequest.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::character_select {

static constexpr std::size_t kPreviewGraphicCount = 14;

static constexpr std::array<std::int32_t, kPreviewGraphicCount> kState0Graphics {{
    100002, 100027, 100052, 100077, 100102, 100127, 100152,
    100252, 100277, 100302, 100327, 100352, 100377, 100402,
}};

static constexpr std::array<std::int32_t, kPreviewGraphicCount> kState1Graphics {{
    105002, 105027, 105052, 105077, 105102, 105127, 105152,
    105252, 105277, 105302, 105327, 105352, 105377, 105402,
}};

static constexpr std::array<std::int32_t, kPreviewGraphicCount> kState2Graphics {{
    106002, 106027, 106052, 106077, 106102, 106127, 106152,
    106252, 106277, 106302, 106327, 106352, 106377, 106402,
}};

static constexpr std::array<std::int32_t, kPreviewGraphicCount> kState3Graphics {{
    105502, 105527, 104402, 104432, 104462, 104492, 120102,
    100900, 100900, 100900, 120126, 100900, 100900, 100900,
}};

static const std::array<std::int32_t, kPreviewGraphicCount>& SelectGraphicTable(
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

static std::int32_t SelectGraphicIndex(std::int32_t converted)
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

namespace detail {

// 原函数：0x0051BA10。
std::int32_t ClassifyCharacterGraphicFamily_51BA10(const std::int32_t imageNo)
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

// 原函数：0x0051BB80。
std::int32_t ConvertClassicCharacterGraphicToCompact_51BB80(std::int32_t imageNo)
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

// 原函数：0x0051BAC0。
std::int32_t ConvertCompactCharacterGraphicToClassic_51BAC0(const std::int32_t imageNo)
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

// 原函数：0x0051BD00。
std::int32_t ResolveFallbackCharacterGraphic_51BD00(const std::int32_t imageNo)
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

// 原函数：0x0051BF00。
std::int32_t ConvertModernCharacterGraphicToEarlier_51BF00(const std::int32_t imageNo)
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

// 原函数：0x0051BFF0。
std::int32_t ConvertEarlierCharacterGraphicToModern_51BFF0(const std::int32_t imageNo)
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

// 原函数：0x0051BC50。
std::int32_t ResolveCompatibleCharacterGraphic_51BC50(const std::int32_t imageNo)
{
    std::int32_t version = state::gGraphicVersionOverride58B6A0;
    if (version == 0xFF) {
        version = cg::network::GetGameConnection().PukVersion();
    }
    const std::int32_t category = ClassifyCharacterGraphicFamily_51BA10(imageNo);
    const std::int32_t allowedCategory = version >= 7 ? 6 : (version >= 3 ? 3 : 0);
    if (allowedCategory >= category || category == 0) {
        return imageNo;
    }
    switch (category) {
    case 1:
    case 2:
    case 6:
        return ResolveFallbackCharacterGraphic_51BD00(imageNo);
    case 3:
        return ConvertCompactCharacterGraphicToClassic_51BAC0(imageNo);
    case 4:
        if (allowedCategory == 0) {
            return ConvertCompactCharacterGraphicToClassic_51BAC0(ConvertModernCharacterGraphicToEarlier_51BF00(imageNo));
        }
        return ConvertModernCharacterGraphicToEarlier_51BF00(imageNo);
    case 5:
        return allowedCategory == 0 ? ResolveFallbackCharacterGraphic_51BD00(imageNo) : ConvertModernCharacterGraphicToEarlier_51BF00(imageNo);
    default:
        return imageNo;
    }
}

} // namespace detail

// LDT01：版本低8值先取；FF回用Puk版本，再分类/按0、3、6、7门槛选择原图或回退。
// 原函数：0x0051B740。
std::int32_t ResolveCharacterImageForVersion(const std::int32_t imageNo)
{
    std::int32_t version = state::gGraphicVersionOverride58B6A0;
    if (version == 0xFF) {
        version = cg::network::GetGameConnection().PukVersion();
    }
    const std::int32_t category = detail::ClassifyCharacterAnimationFamily_51B7D0(imageNo);
    std::int32_t allowedCategory;
    if (version >= 8) {
        allowedCategory = 7;
    } else if (version >= 7) {
        allowedCategory = 6;
    } else if (version >= 3) {
        allowedCategory = 3;
    } else {
        allowedCategory = 0;
    }
    if (allowedCategory >= category || category == 0) {
        return imageNo;
    }
    switch (category) {
    case 3:
        return imageNo - 5000;
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
        return detail::ResolveFallbackCharacterAnimation_51B910(imageNo);
    default:
        return imageNo;
    }
}

namespace detail {

// 原函数：0x0051B7D0。
std::int32_t ClassifyCharacterAnimationFamily_51B7D0(const std::int32_t imageNo)
{
    if (imageNo >= 100000 && imageNo <= 103102) {
        return 0;
    }
    if (imageNo >= 105000 && imageNo <= 105423) {
        return 3;
    }
    if (imageNo >= 106000 && imageNo <= 106048) {
        return 1;
    }
    if (imageNo >= 106050 && imageNo <= 106173) {
        return 2;
    }
    if (imageNo >= 106250 && imageNo <= 106298) {
        return 1;
    }
    if (imageNo >= 106300 && imageNo <= 106423) {
        return 2;
    }
    if (imageNo >= 100900 && imageNo <= 100902) {
        return 2;
    }
    if (imageNo >= 107000 && imageNo < 107999) {
        return 2;
    }
    if (imageNo >= 106425 && imageNo < 106570) {
        return 2;
    }
    if (imageNo >= 106600 && imageNo < 106755) {
        return 2;
    }
    if (imageNo >= 110300 && imageNo < 110350) {
        return 6;
    }
    if (imageNo >= 110376 && imageNo < 110400) {
        return 6;
    }
    if (imageNo >= 110505 && imageNo < 110599) {
        return 7;
    }
    if (imageNo >= 110770 && imageNo <= 110799) {
        return 7;
    }
    if (imageNo >= 114170 && imageNo <= 114200) {
        return 0;
    }
    if (imageNo < 110504 || imageNo >= 114999) {
        return 0;
    }
    return 6;
}

// 原函数：0x0051B910。
std::int32_t ResolveFallbackCharacterAnimation_51B910(const std::int32_t imageNo)
{
    if (imageNo >= 106000 && imageNo <= 106048) {
        return 100900;
    }
    if (imageNo >= 106050 && imageNo <= 106173) {
        return 100900;
    }
    if (imageNo >= 106250 && imageNo <= 106298) {
        return 100901;
    }
    if (imageNo >= 106300 && imageNo <= 106423) {
        return 100901;
    }
    if (imageNo >= 107000 && imageNo < 107999) {
        return 100902;
    }
    if (imageNo >= 106425 && imageNo < 106570) {
        return 100900;
    }
    if (imageNo >= 106600 && imageNo < 106755) {
        return 100901;
    }
    if (imageNo >= 110300 && imageNo < 110350) {
        return 100902;
    }
    if (imageNo >= 110376 && imageNo < 110400) {
        return 100902;
    }
    if (imageNo >= 110505 && imageNo <= 110599) {
        return 110598;
    }
    if (imageNo >= 110770 && imageNo < 110799) {
        return 110598;
    }
    if (imageNo < 110504 || imageNo >= 114999) {
        return -1;
    }
    return 100902;
}

// 原函数：0x004C56E0。
std::int32_t* DecomposeCharacterAppearanceGraphic_4C56E0(
    const std::int32_t imageNo,
    std::int32_t* const outBase,
    std::int32_t* const outClass,
    std::int32_t* const outQuarter,
    std::int32_t* const outFifth,
    std::int32_t* const outOnes)
{
    const std::int32_t imageClass = ClassifyCharacterGraphicFamily_51BA10(imageNo);
    *outClass = 0;
    if (imageClass >= 1) {
        *outClass = 1;
    }
    if (imageClass >= 4) {
        *outClass = 2;
    }

    const std::int32_t converted = ConvertEarlierCharacterGraphicToModern_51BFF0(ConvertClassicCharacterGraphicToCompact_51BB80(imageNo));
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

// 原函数：0x004C5650。
std::int32_t ComposeCharacterAppearanceGraphic_4C5650(
    const std::int32_t base,
    const std::int32_t imageClass,
    const std::int32_t quarter,
    const std::int32_t fifth,
    const std::int32_t ones)
{
    if (imageClass == 2) {
        return base + 25 * quarter + 5 * fifth + ones;
    }
    if (imageClass == 1) {
        return ConvertModernCharacterGraphicToEarlier_51BF00(base) + 25 * quarter + 5 * fifth + ones;
    }
    if (imageClass == 0) {
        return ConvertCompactCharacterGraphicToClassic_51BAC0(ConvertModernCharacterGraphicToEarlier_51BF00(base))
            + 1000 * quarter + 10 * fifth + ones;
    }
    return base;
}

} // namespace detail

// 原函数：0x00460D00。
void ProcessCharacterPreview_460D00(network::CgConnection& connection, std::uint32_t slot)
{
    const std::size_t slotIndex = static_cast<std::size_t>(slot);
    cg::yobi::YobiAction* const actionToDestroy =
        state::gPreviewActions[slotIndex];
    if (actionToDestroy != nullptr) {
        cg::yobi::MarkDeath(actionToDestroy);
        state::gPreviewActions[slotIndex] = nullptr;
    }

    if (cg::login::HasCharacterInSlot_51B110(connection, slot) == 0) {
        return;
    }

    cg::yobi::YobiAction* const existingAction =
        state::gPreviewActions[slotIndex];
    if (existingAction != nullptr) {
        return;
    }

    login::CharacterRecord& record =
        connection.LoginRequests().characters[slotIndex];
    if (record.graphicNo < 30000) {
        record.graphicNo = 33100;
    }

    const std::int32_t converted = detail::ConvertEarlierCharacterGraphicToModern_51BFF0(
        detail::ConvertClassicCharacterGraphicToCompact_51BB80(record.graphicNo));

    const std::int32_t oldPreviewState =
        record.previewState;
    if (oldPreviewState < 0) {
        record.previewState = converted >= 240000 && converted < 241400 ? 1 : 2;
    } else {
        if (converted >= 241400) {
            record.previewState = 2;
        }
        if ((converted >= 205000 && converted <= 205600)
            || (converted >= 270000 && converted <= 270200)) {
            record.previewState = 3;
        }
    }

    const std::int32_t previewState =
        record.previewState;
    const std::array<std::int32_t, kPreviewGraphicCount>& graphics =
        SelectGraphicTable(previewState);

    cg::yobi::YobiAction* const action = cg::yobi::NewYobiAction_404AE0(
        std::uint8_t{2},
        std::uint32_t{0});
    state::gPreviewActions[slotIndex] = action;
    if (action == nullptr) {
        return;
    }

    const std::int32_t graphicIndex = SelectGraphicIndex(converted);
    const std::int32_t normalizedGraphic =
        record.graphicNo;
    if (normalizedGraphic == 33100) {
        cg::yobi::SetYobiActionField2A8(action, 100900);
    } else if (normalizedGraphic == 33101) {
        cg::yobi::SetYobiActionField2A8(action, 100901);
    } else {
        cg::yobi::SetYobiActionField2A8(
            action,
            graphics[static_cast<std::size_t>(graphicIndex)]);
        const std::int32_t remainder = converted % 100;
        cg::yobi::YobiAction* const actionForVariant =
            state::gPreviewActions[slotIndex];
        const std::int32_t variant = remainder / 25;
        const std::int32_t baseGraphic =
            cg::yobi::GetYobiActionField2A8(actionForVariant);
        cg::yobi::SetYobiActionField2A8(
            actionForVariant,
            baseGraphic + 6 * variant);
    }

    cg::yobi::SetYobiActionField2B0(state::gPreviewActions[slotIndex], 0);
    cg::yobi::SetYobiActionField15(state::gPreviewActions[slotIndex], 10);
    const std::int32_t x = 315 * static_cast<std::int32_t>(slot) + 163;
    cg::yobi::SetYobiActionField30(state::gPreviewActions[slotIndex], x);
    cg::yobi::SetYobiActionField34(state::gPreviewActions[slotIndex], 292);
    cg::yobi::SetYobiActionField2B8(state::gPreviewActions[slotIndex], cg::yobi::YobiDirection_Down);

    cg::yobi::YobiAction* const actionForFrameMove =
        state::gPreviewActions[slotIndex];
    cg::yobi::AdvanceYobiAnimationFrame_517880(actionForFrameMove, 100, 0, 0);
}

} // namespace cg::character_select
