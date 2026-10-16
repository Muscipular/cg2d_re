#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"

#include "cg/scene/login/CharacterListRequest.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

constexpr std::int32_t kLegacyGlyphWidth = 8;
constexpr std::int32_t kColumnWidth = 64;
constexpr std::int32_t kRowHeight = 48;
constexpr std::int32_t kTextHeight = 16;

// 原阶段 0x9A6E5C、对象 0x9A6E60 及文本/布局状态的全部引用都只在
// sub_45DD50/sub_45DD60 内。
std::uint16_t gCharacterListProgressStage = 0;
cg::yobi::YobiAction* gCharacterListProgressAction = nullptr;
std::wstring gCharacterListProgressText {};
std::int32_t gCharacterListProgressColumns = 0;
std::int32_t gCharacterListProgressRows = 0;
std::int32_t gCharacterListProgressX = 0;
std::int32_t gCharacterListProgressY = 0;


} // namespace

// 原函数：0x0045DD50。
void ResetCharacterListProgress_45DD50()
{
    gCharacterListProgressStage = 0;
}

// 原函数：0x0045DD60。
std::int32_t UpdateCharacterListProgress_45DD60()
{
    std::uint16_t stage = gCharacterListProgressStage;
    std::int32_t result = 0;

    if (stage == 0) {
        gCharacterListProgressText = L"登入成功，开始计费！" /* 原 0x55C864：正文 20 字节，含隐式 NUL 共 21 字节。 */;
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(gCharacterListProgressText, 0));
        const std::int32_t columns =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        gCharacterListProgressStage = 1;
        const std::int32_t unscaledX = 10 - columns;
        gCharacterListProgressColumns = columns;
        const std::int32_t centeredWidth = unscaledX * kColumnWidth;
        gCharacterListProgressRows = 2;
        const std::int32_t width = columns * kColumnWidth;
        const std::int32_t x = centeredWidth / 2;
        gCharacterListProgressX = x;
        gCharacterListProgressY = 192;
        gCharacterListProgressAction = cg::yobi::detail::CreateModalAction_5044E0(
            x,
            192,
            width,
            96,
            4,
            1);
        stage = gCharacterListProgressStage;
    }

    if (stage == 1) {
        login::StartCharacterListRequest();
        ++gCharacterListProgressStage;
    } else if (stage == 2) {
        result = login::PollCharacterListRequest();
        if (result != 0) {
            cg::yobi::YobiAction* const action = gCharacterListProgressAction;
            if (action != nullptr) {
                cg::yobi::detail::MarkDeath(action);
                gCharacterListProgressAction = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = gCharacterListProgressAction;
    if (action != nullptr && cg::yobi::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gCharacterListProgressRows;
        const std::int32_t verticalHeight = rows * kRowHeight - kTextHeight;
        const std::int32_t y = gCharacterListProgressY;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t columns = gCharacterListProgressColumns;
        const std::int32_t textWidth = cg::render::TextMeasure(gCharacterListProgressText, 0);
        const std::int32_t horizontalWidth =
            columns * kColumnWidth - textWidth;
        const std::int32_t x = gCharacterListProgressX;
        const std::int32_t textX = horizontalWidth / 2 + x;
        cg::render::DrawText3_456560(
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
