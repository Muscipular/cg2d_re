#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/CharacterSelectScene.hpp"

#include "cg/CharacterDeletionRequest.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>
#include <cstring>

namespace cg::character_select {
namespace {

// 原 0x55ABE8：正文 10 字节，含隐式 NUL 共 11 字节。
constexpr wchar_t kCharacterDeletionProgressText[] =
    L"角色刪除中";
constexpr std::int32_t kColumnWidth = 64;
constexpr std::int32_t kRowHeight = 48;

// 下列原存储的全部引用都只在 sub_45DBF0/sub_45DC00 内。
std::uint16_t gCharacterDeletionProgressStage = 0; // 原 0x9A6E54，R/W16。
login::YobiAction* gCharacterDeletionProgressAction = nullptr; // 原 0x9A6E58。
std::int32_t gCharacterDeletionProgressX = 0;       // 原 0x9A4EA4，R/W32。
std::int32_t gCharacterDeletionProgressY = 0;       // 原 0x9A5FDC，R/W32。
std::int32_t gCharacterDeletionProgressColumns = 0; // 原 0x9A50DC，R/W32。
std::int32_t gCharacterDeletionProgressRows = 0;    // 原 0x9A5950，R/W32。

} // namespace

// 原函数：0x0045DBF0。
void ResetCharacterDeletionProgress_45DBF0()
{
    gCharacterDeletionProgressStage = 0;
}

// 原函数：0x0045DC00。
std::int32_t UpdateCharacterDeletionProgress_45DC00()
{
    std::uint16_t stage = gCharacterDeletionProgressStage;
    std::int32_t result = 0;

    if (stage == 0) {
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(kCharacterDeletionProgressText, 0));
        gCharacterDeletionProgressStage = 1;
        const std::int32_t columns =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        gCharacterDeletionProgressRows = 2;
        const std::int32_t unscaledX = 10 - columns;
        gCharacterDeletionProgressColumns = columns;
        const std::int32_t centeredWidth = unscaledX * kColumnWidth;
        gCharacterDeletionProgressY = 192;
        const std::int32_t width = columns * kColumnWidth;
        const std::int32_t x = centeredWidth / 2;
        gCharacterDeletionProgressX = x;
        gCharacterDeletionProgressAction = login::detail::CreateModalAction_5044E0(
            x,
            192,
            width,
            96,
            4,
            1);
        stage = gCharacterDeletionProgressStage;
    }

    if (stage == 1) {
        login::StartCharacterDeletionRequest();
        ++gCharacterDeletionProgressStage;
    } else if (stage == 2) {
        result = login::PollCharacterDeletionRequest();
        if (result != 0) {
            login::YobiAction* const action = gCharacterDeletionProgressAction;
            if (action != nullptr) {
                login::detail::MarkDeath(action);
                gCharacterDeletionProgressAction = nullptr;
            }
            return result;
        }
    }

    login::YobiAction* const action = gCharacterDeletionProgressAction;
    if (action != nullptr && login::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gCharacterDeletionProgressRows;
        const std::int32_t y = gCharacterDeletionProgressY;
        const std::int32_t verticalHeight = rows * kRowHeight - 16;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t textWidth = cg::render::TextMeasure(kCharacterDeletionProgressText, 0);
        const std::int32_t columns = gCharacterDeletionProgressColumns;
        const std::int32_t horizontalWidth = columns * kColumnWidth - textWidth;
        const std::int32_t x = gCharacterDeletionProgressX;
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
