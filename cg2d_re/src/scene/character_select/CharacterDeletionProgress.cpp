#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/network/CgConnection.hpp"
#include "CharacterSelectSceneState.hpp"

#include "cg/scene/login/CharacterDeletionRequest.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {

// 原 0x55ABE8：正文 10 字节，含隐式 NUL 共 11 字节。
static constexpr wchar_t kCharacterDeletionProgressText[] =
    L"角色删除中";
static constexpr std::int32_t kColumnWidth = 64;
static constexpr std::int32_t kRowHeight = 48;

// 原函数：0x0045DBF0。
void ResetCharacterDeletionProgress_45DBF0()
{
    cg::network::GetGameConnection().CharacterSelectUi().deletionProgress.stage = 0;
}

// 原函数：0x0045DC00。
std::int32_t UpdateCharacterDeletionProgress_45DC00()
{
    auto& progress = cg::network::GetGameConnection().CharacterSelectUi().deletionProgress;
    std::uint16_t stage = progress.stage;
    std::int32_t result = 0;

    if (stage == 0) {
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(kCharacterDeletionProgressText, 0));
        progress.stage = 1;
        const std::int32_t columns =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        progress.rows = 2;
        const std::int32_t unscaledX = 10 - columns;
        progress.columns = columns;
        const std::int32_t centeredWidth = unscaledX * kColumnWidth;
        progress.y = 192;
        const std::int32_t width = columns * kColumnWidth;
        const std::int32_t x = centeredWidth / 2;
        progress.x = x;
        progress.action = cg::yobi::CreateModalAction_5044E0(
            x,
            192,
            width,
            96,
            4,
            1);
        stage = progress.stage;
    }

    if (stage == 1) {
        login::StartCharacterDeletionRequest();
        ++progress.stage;
    } else if (stage == 2) {
        result = login::PollCharacterDeletionRequest();
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
        const std::int32_t y = progress.y;
        const std::int32_t verticalHeight = rows * kRowHeight - 16;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t textWidth = cg::render::TextMeasure(kCharacterDeletionProgressText, 0);
        const std::int32_t columns = progress.columns;
        const std::int32_t horizontalWidth = columns * kColumnWidth - textWidth;
        const std::int32_t x = progress.x;
        const std::int32_t textX = horizontalWidth / 2 + x;
        cg::render::DrawText3_456560(
            static_cast<std::int16_t>(textX),
            static_cast<std::int16_t>(textY),
            2,
            0,
            kCharacterDeletionProgressText,
            0);
    }
    return result;
}

} // namespace cg::character_select
