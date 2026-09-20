#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr char kCharacterListProgressText[] = "登入成功，開始計費！";
constexpr std::int32_t kLegacyTextLength = 20;
constexpr std::int32_t kLegacyGlyphWidth = 8;
constexpr std::int32_t kColumnWidth = 64;
constexpr std::int32_t kRowHeight = 48;
constexpr std::int32_t kTextHeight = 16;

// 原阶段 0x9A6E5C、对象 0x9A6E60 及文本/布局状态的全部引用都只在
// sub_45DD50/sub_45DD60 内。
std::uint16_t gCharacterListProgressStage = 0;
login::YobiAction* gCharacterListProgressAction = nullptr;
char gCharacterListProgressText[256] {};
std::int32_t gCharacterListProgressColumns = 0;
std::int32_t gCharacterListProgressRows = 0;
std::int32_t gCharacterListProgressX = 0;
std::int32_t gCharacterListProgressY = 0;

static_assert(sizeof(kCharacterListProgressText) == 31);
static_assert(sizeof(gCharacterListProgressText) == 256);

} // namespace

void ResetCharacterListProgress_45DD50()
{
    gCharacterListProgressStage = 0;
}

std::int32_t UpdateCharacterListProgress_45DD60()
{
    std::uint16_t stage = gCharacterListProgressStage;
    std::int32_t result = 0;

    if (stage == 0) {
        std::memcpy(
            gCharacterListProgressText,
            kCharacterListProgressText,
            sizeof(kCharacterListProgressText));
        gCharacterListProgressStage = 1;
        gCharacterListProgressColumns =
            kLegacyTextLength / kLegacyGlyphWidth + 2;
        gCharacterListProgressRows = 2;
        gCharacterListProgressX =
            ((10 - gCharacterListProgressColumns) * kColumnWidth) / 2;
        gCharacterListProgressY = 192;
        gCharacterListProgressAction = login::detail::Sub_5044E0(
            gCharacterListProgressX,
            gCharacterListProgressY,
            gCharacterListProgressColumns * kColumnWidth,
            96,
            4,
            1);
        stage = gCharacterListProgressStage;
    }

    if (stage == 1) {
        detail::Sub_508910();
        ++gCharacterListProgressStage;
    } else if (stage == 2) {
        result = detail::Sub_508950();
        if (result != 0) {
            login::YobiAction* const action = gCharacterListProgressAction;
            if (action != nullptr) {
                login::detail::MarkDeath(action);
            }
            gCharacterListProgressAction = nullptr;
            return result;
        }
    }

    login::YobiAction* const action = gCharacterListProgressAction;
    if (action != nullptr && login::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t textX = gCharacterListProgressX
            + (gCharacterListProgressColumns * kColumnWidth
                - kLegacyTextLength * kLegacyGlyphWidth)
                / 2;
        const std::int32_t textY = gCharacterListProgressY
            + (gCharacterListProgressRows * kRowHeight - kTextHeight) / 2;
        login::detail::DrawText3(
            static_cast<std::int16_t>(textX),
            static_cast<std::int16_t>(textY),
            2,
            0,
            gCharacterListProgressText,
            0);
    }
    return result;
}

} // namespace cg::character_select
