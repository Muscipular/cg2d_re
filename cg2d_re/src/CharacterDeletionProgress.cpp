#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"

#include <cstdint>

namespace cg::character_select {
namespace {

constexpr char kCharacterDeletionProgressText[] = "角色刪除中";
constexpr std::int32_t kLegacyTextBytes = 10;
constexpr std::int32_t kColumnWidth = 64;
constexpr std::int32_t kRowHeight = 48;
constexpr std::int16_t kTextX = 280;
constexpr std::int16_t kTextY = 232;

// 下列原存储的全部引用都只在 sub_45DBF0/sub_45DC00 内。
std::uint16_t gCharacterDeletionProgressStage = 0; // 原 0x9A6E54，R/W16。
login::YobiAction* gCharacterDeletionProgressAction = nullptr; // 原 0x9A6E58。
std::int32_t gCharacterDeletionProgressX = 0;       // 原 0x9A4EA4，R/W32。
std::int32_t gCharacterDeletionProgressY = 0;       // 原 0x9A5FDC，R/W32。
std::int32_t gCharacterDeletionProgressColumns = 0; // 原 0x9A50DC，R/W32。
std::int32_t gCharacterDeletionProgressRows = 0;    // 原 0x9A5950，R/W32。

} // namespace

void ResetCharacterDeletionProgress_45DBF0()
{
    gCharacterDeletionProgressStage = 0;
}

std::int32_t UpdateCharacterDeletionProgress_45DC00()
{
    std::uint16_t stage = gCharacterDeletionProgressStage;
    std::int32_t result = 0;

    if (stage == 0) {
        gCharacterDeletionProgressStage = 1;
        gCharacterDeletionProgressRows = 2;
        gCharacterDeletionProgressColumns =
            ((9 * kLegacyTextBytes) >> 6) + 2;
        gCharacterDeletionProgressY = 192;
        gCharacterDeletionProgressX =
            ((10 - gCharacterDeletionProgressColumns) * kColumnWidth) / 2;
        gCharacterDeletionProgressAction = login::detail::Sub_5044E0(
            gCharacterDeletionProgressX,
            gCharacterDeletionProgressY,
            gCharacterDeletionProgressColumns * kColumnWidth,
            gCharacterDeletionProgressRows * kRowHeight,
            4,
            1);
        stage = gCharacterDeletionProgressStage;
    }

    if (stage == 1) {
        detail::Sub_508F70();
        ++gCharacterDeletionProgressStage;
    } else if (stage == 2) {
        result = detail::Sub_508FC0();
        if (result != 0) {
            login::YobiAction* const action = gCharacterDeletionProgressAction;
            if (action != nullptr) {
                login::detail::MarkDeath(action);
            }
            gCharacterDeletionProgressAction = nullptr;
            return result;
        }
    }

    login::YobiAction* const action = gCharacterDeletionProgressAction;
    if (action != nullptr && login::detail::IsYobiActionReady(action, 1)) {
        login::detail::DrawText3(
            kTextX,
            kTextY,
            2,
            0,
            kCharacterDeletionProgressText,
            0);
    }
    return result;
}

} // namespace cg::character_select
