#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"
#include "CharacterSelectSceneState.hpp"
#include <string>

#include "CharacterSelectSceneDependencies.hpp"

#include "cg/scene/login/CharacterListRequest.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {

static constexpr std::int32_t kLegacyGlyphWidth = 8;
static constexpr std::int32_t kColumnWidth = 64;
static constexpr std::int32_t kRowHeight = 48;
static constexpr std::int32_t kTextHeight = 16;

// 原函数：0x0045DD50。
void ResetCharacterListProgress_45DD50(network::CgConnection& connection)
{
    connection.CharacterSelectUi().listProgress.stage = 0;
}

// 原函数：0x0045DD60。
std::int32_t UpdateCharacterListProgress_45DD60(network::CgConnection& connection)
{
    auto& progress = connection.CharacterSelectUi().listProgress;
    std::uint16_t stage = progress.stage;
    std::int32_t result = 0;

    if (stage == 0) {
        progress.text = L"登入成功，开始计费！" /* 原 0x55C864：正文 20 字节，含隐式 NUL 共 21 字节。 */;
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(progress.text, 0));
        const std::int32_t columns =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        progress.stage = 1;
        const std::int32_t unscaledX = 10 - columns;
        progress.columns = columns;
        const std::int32_t centeredWidth = unscaledX * kColumnWidth;
        progress.rows = 2;
        const std::int32_t width = columns * kColumnWidth;
        const std::int32_t x = centeredWidth / 2;
        progress.x = x;
        progress.y = 192;
        progress.action = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), x,
            192,
            width,
            96,
            4,
            1);
        stage = progress.stage;
    }

    if (stage == 1) {
        login::StartCharacterListRequest(connection);
        ++progress.stage;
    } else if (stage == 2) {
        result = login::PollCharacterListRequest(connection);
        if (result != 0) {
            cg::yobi::YobiAction* const action = progress.action;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                progress.action = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = progress.action;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = progress.rows;
        const std::int32_t verticalHeight = rows * kRowHeight - kTextHeight;
        const std::int32_t y = progress.y;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t columns = progress.columns;
        const std::int32_t textWidth = cg::render::TextMeasure(progress.text, 0);
        const std::int32_t horizontalWidth =
            columns * kColumnWidth - textWidth;
        const std::int32_t x = progress.x;
        const std::int32_t textX = horizontalWidth / 2 + x;
        cg::render::DrawText3_456560(
            static_cast<std::int16_t>(textX),
            static_cast<std::int16_t>(textY),
            2,
            0,
            progress.text,
            0);
    }
    return result;
}

} // namespace cg::character_select
