#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"
#include "LegacyPanelRuntime.hpp"
#include "LoginSceneDependencies.hpp"
#include "platform/MainLoopDependencies.hpp"
#include "ui/WindowRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LoginInput.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/DisplaySettings.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/Input.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace {

using QueryStateFunction = std::int32_t (*)(const void* uiRecord);
using DrawBaseFunction = std::int32_t (*)(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t value5);

constexpr std::size_t kXOffset = 0x00;
constexpr std::size_t kYOffset = 0x04;
constexpr std::size_t kWidthOffset = 0x08;
constexpr std::size_t kHeightOffset = 0x0C;
constexpr std::size_t kNormalImageOffset = 0x10;
constexpr std::size_t kActiveImageOffset = 0x14;

std::int32_t ReadRecordW32(
    const void* const uiRecord,
    const std::size_t offset)
{
    std::int32_t value;
    const auto* const bytes = static_cast<const std::byte*>(uiRecord);
    std::memcpy(&value, bytes + offset, sizeof(value));
    return value;
}

std::int32_t QueryLegacyUiRecordState_45F100(
    const void* const uiRecord)
{
    if (uiRecord == nullptr) {
        return 0;
    }

    cg::login::LoginSceneControl32 control {};
    control.top = ReadRecordW32(uiRecord, kYOffset);
    control.left = ReadRecordW32(uiRecord, kXOffset);
    control.height = ReadRecordW32(uiRecord, kHeightOffset);
    control.width = ReadRecordW32(uiRecord, kWidthOffset);
    return cg::login::QueryLoginControlState_5036F0(&control);
}

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right)
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::uint16_t Low16(const std::int32_t value)
{
    return static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value));
}

std::int32_t QueryCharacterSelectState(const void* const uiRecord)
{
    return cg::character_select::detail::Sub_45F100(
        static_cast<
            const cg::character_select::detail::CharacterSelectUiRecord*>(
                uiRecord));
}

std::int32_t QueryPanelButtonState(const cg::login::detail::LegacyPanelButtonRecord* const button)
{
    // 503990仅消费矩形；避免将图号尾部当作CharacterModalUiRecord的文本指针。
    const cg::character_select::detail::CharacterModalUiRecord rectangle {
        0, 0, button->left, button->top, button->width, button->height, nullptr,
    };
    return cg::character_select::detail::Sub_503990(&rectangle);
}

void DrawLegacyUiRecord_45EFD0(
    const void* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style,
    const QueryStateFunction queryState,
    const DrawBaseFunction drawBase)
{
    if (uiRecord == nullptr) {
        return;
    }

    std::int32_t imageNo = ReadRecordW32(uiRecord, kNormalImageOffset);
    if (style == 0) {
        const std::uint8_t input =
            static_cast<std::uint8_t>(queryState(uiRecord));
        if ((input & 0x40U) != 0) {
            imageNo = ReadRecordW32(uiRecord, kActiveImageOffset);
        }
    } else if (style == 2) {
        imageNo = ReadRecordW32(uiRecord, kActiveImageOffset);
    }

    const std::int32_t baseY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t baseX = ReadRecordW32(uiRecord, kXOffset);
    static_cast<void>(drawBase(
        baseX,
        baseY,
        layer,
        imageNo,
        1,
        -1));

    if (overlayMode == 0) {
        return;
    }

    const std::int32_t hitY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t hitHeight = ReadRecordW32(uiRecord, kHeightOffset);
    const std::int32_t hitX = ReadRecordW32(uiRecord, kXOffset);
    const std::int32_t hitBottom = AddW32(hitHeight, hitY);
    const std::int32_t hitWidth = ReadRecordW32(uiRecord, kWidthOffset);
    const std::int32_t hitRight = AddW32(hitWidth, hitX);
    if (cg::login::detail::Sub_5049A0(
            hitX,
            hitY,
            hitRight,
            hitBottom,
            -1)
        == 0) {
        return;
    }

    if (style != 0) {
        return;
    }

    const std::int32_t hoverY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t hoverX = ReadRecordW32(uiRecord, kXOffset);
    const std::int32_t hoverHeight =
        ReadRecordW32(uiRecord, kHeightOffset);
    const std::int32_t hoverBottom = AddW32(
        AddW32(hoverHeight, hoverY),
        2);
    const std::int32_t hoverTop = AddW32(hoverY, -2);
    const std::int32_t hoverWidth = ReadRecordW32(uiRecord, kWidthOffset);
    const std::int32_t hoverRight = AddW32(
        AddW32(hoverWidth, hoverX),
        2);
    const std::int32_t hoverLeft = AddW32(hoverX, -2);
    cg::login::detail::DrawGraphicHover(
        Low16(hoverLeft),
        Low16(hoverTop),
        Low16(hoverRight),
        Low16(hoverBottom),
        layer,
        std::bit_cast<std::uint32_t>(overlayImageNo),
        overlayMode == 2 ? 1 : 0);
}

} // namespace

namespace cg::login::detail {

std::uint32_t gLegacyPanelHitIdD046C0 = 0;
LegacyPanelRecord gLegacySkillList585708 {
    0, 0, 272, 438, 18, {0x58DE, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::uint8_t gLegacySkillPendingD67C48 = 0;
std::int32_t gLegacyRequiredFpCC31D8 = 0;
std::int32_t gLegacyItemWorkSelection584394 = -1;
std::array<char, 64> gLegacyItemWorkNameCC40D8 {};
std::int32_t gLegacyItemWorkImageCC320C = 0;
std::int32_t gLegacyCraftPhaseD67D00 = 0;
std::int32_t gLegacyGemPhaseD68A8C = 0;
std::int32_t gLegacyRecipePageD67D0C = 0;
std::int32_t gLegacyInventoryValueC8B388 = 0;
std::int32_t gLegacySpecialUseItemC8A96C = 0;
std::int32_t gLegacySpecialUseColumnCF4AA8 = 0;
std::int32_t gLegacySpecialUseRowD2D614 = 0;
std::int32_t gLegacySplitSourceCC3E60 = 0;
std::int32_t gLegacySplitTargetCC21DC = 0;
std::int32_t gLegacySplitAmountC8A948 = 0;
std::int32_t gLegacyLastUsedItemCB01EC = 0;

namespace {

// 旧72物品子页与后续外层切页共用；CC4D58展示记录仍归WindowRuntime。
LegacyPanelRecord gLegacyStoragePanel582B70 {
    40, 16, 560, 364, 72, {23340, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr std::array<std::array<std::int32_t, 4>, 2> kLegacyStorageGrid582C34 {{
    {13, 94, 60, 141}, {291, 94, 338, 141},
}};
std::int32_t gLegacyStoragePreviousLeft582C64 = -1;
std::int32_t gLegacyStoragePreviousRight582C68 = -1;
std::int32_t gLegacyStorageSplitModeCA0AA8 = 0;
std::int32_t gLegacyStorageSplitSourceD3FACC = 0;
std::int32_t gLegacyStorageSplitTargetCC4D38 = 0;
std::int32_t gLegacyStorageSplitAmountCB2220 = 0;
std::int32_t gLegacyStorageDragSlotCC4C98 = 0;
std::int32_t gLegacyStorageDragImageCB09D4 = 0;
std::int32_t gLegacyStorageDetailPageD67C94 = 0;
std::int32_t gLegacyStorageMessageD67C98 = 0;
std::uint32_t gLegacyStorageMessageTickD67C9C = 0;
std::int32_t gLegacyStorageEmptySlotsCF49BC = 0;
std::int32_t gLegacyStorageTakeCountCF4A04 = 0;
std::int32_t gLegacyStoragePutCountD2DAD4 = 0;
std::int32_t gLegacyStorageCapacityD30404 = 0;
std::int32_t gLegacyStorageFeeD40B04 = 0;
std::int32_t gLegacyStorageMoneyD2F79C = 0;
std::int32_t gLegacyStorageWithdrawDraftD2EA98 = 0;
std::int32_t gLegacyStorageWithdrawAmountCF49B8 = 0;
std::int32_t gLegacyStorageDepositMessageD67CAC = 0;
std::uint32_t gLegacyStorageDepositTickD67CB0 = 0;
constexpr char kLegacyStorageNoMoney58906C[] = "\xA7\x41\xAA\xBA\xBF\xFA\xA4\xA3\xB0\xF7\xA1\x49";
constexpr char kLegacyStorageFee589284[] = "\xA4\xE2\xC4\xF2\xB6\x4F";
constexpr char kLegacyStorageGold589278[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42";
constexpr char kLegacyStorageAmount588FC4[] = "%7d \xAA\xF7\xB9\xF4";
constexpr char kLegacyStorageTake589268[] = "\xA8\xFA\xA5\x58\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyStorageFull589230[] = "\xB5\x4C\xAA\x6B\xA6\x41\xA6\x73\xA9\xF1\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyStorageCanPut589244[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xA5\x69\xA5\x48\xA6\x73\xA9\xF1\xA1\x43";
constexpr char kLegacyStoragePut589258[] = "\xA6\x73\xA9\xF1\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyStorageCannotPut5878BC[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xA6\x73\xA9\xF1\xA1\x43";
constexpr char kLegacyStorageClear5859FC[] = "\xA7\xE2\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43";
constexpr char kLegacyStorageAmountLimit5892D4[] = "\xB5\x4C\xAA\x6B\xAB\xFC\xA9\x77\xA7\xF3\xA6\x68\xAA\xF7\xC3\x42\xA1\x43";
constexpr char kLegacyStorageDeposit5892F4[] = "\xA6\x73\xA4\x4A\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43";
constexpr char kLegacyStorageCannotDeposit5892E8[] = "\xB5\x4C\xAA\x6B\xA6\x73\xA4\x4A\xA1\x43";
constexpr char kLegacyStorageBalance589314[] = "\xA6\x73\xB4\xDA";
constexpr char kLegacyStorageWithdraw589304[] = "\xA8\xFA\xA5\x58\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43";

char* BorrowLegacyStorageItem(const std::int32_t side, const std::int32_t item)
{
    return ui::detail::gItemDetailStorageCC4D58.data() + side * 0xFBF8 + item * 0xC44;
}

std::int16_t ReadLegacyStorageW16(const char* const record, const std::size_t offset)
{
    std::int16_t value;
    std::memcpy(&value, record + offset, sizeof(value));
    return value;
}

void WriteLegacyStorageSelection(char* const record, const std::int16_t value)
{
    std::memcpy(record + 2, &value, sizeof(value));
}

constexpr char kLegacySkillClose55331C[] =
    "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacySkillOpen5864C8[] =
    "\xA5\xB4\xB6\x7D\xB3\x6F\xAD\xD3\xA7\xDE\xAF\xE0\xAA\xBA\xA7\xDE"
    "\xAF\xE0\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyAttributeLimit58A220[] =
    "\xA4\x77\xB8\x67\xA4\xA3\xAF\xE0\xA6\x41\xA4\xC0\xB0\x74\xC2\x49"
    "\xBC\xC6\xA4\x46\xA1\x43";
constexpr char kLegacyAttributeHint58AA08[] =
    "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xC5\xE9\xA4\x4F\xA1\x43";
constexpr char kLegacyAttributeHint58A274[] =
    "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xA4\x4F\xB6\x71\xA1\x43";
constexpr char kLegacyAttributeHint58A9F4[] =
    "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xB1\x6A\xAB\xD7\xA1\x43";
constexpr char kLegacyAttributeHint58A24C[] =
    "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xB3\x74\xAB\xD7\xA1\x43";
constexpr char kLegacyAttributeHint58A9E0[] =
    "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xC5\x5D\xA4\x4F\xA1\x43";
constexpr char kLegacyProfileTab5858D8[] =
    "\xA4\xC1\xB4\xAB\xA6\xA8\xAA\xB1\xAE\x61\xAA\xAC\xBA\x41\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyProfileTab5858C0[] =
    "\xA4\xC1\xB4\xAB\xA6\xA8\xAA\xB1\xAE\x61\xA9\xFA\xB2\xD3\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyProfileTab5858A8[] =
    "\xA4\xC1\xB4\xAB\xA6\xA8\xAA\xB1\xAE\x61\xBA\xD9\xB8\xB9\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyProfileRear586344[] =
    "\xA7\xE2\xBE\xD4\xB0\xAB\xAE\xC9\xAA\xBA\xA6\xEC\xB8\x6D\xC5\xDC"
    "\xA7\xF3\xAC\xB0\xAB\xE1\xB1\xC6\xA1\x43";
constexpr char kLegacyProfileFront586360[] =
    "\xA7\xE2\xBE\xD4\xB0\xAB\xAE\xC9\xAA\xBA\xA6\xEC\xB8\x6D\xC5\xDC"
    "\xA7\xF3\xAC\xB0\xAB\x65\xB1\xC6\xA1\x43";
constexpr std::array<std::array<std::uint32_t, 2>, 4> kLegacyAttributeBars57CBB8 {{
    {250, 2}, {254, 6}, {249, 1}, {251, 3},
}};
std::uint32_t gLegacyProfilePositionTickD68860 = 0;
input::InputContext* gLegacyProfileInputD6885C = nullptr;
std::int32_t gLegacyProfilePreviousPage585894 = -1;
std::uint32_t gLegacyProfileEditTickD68864 = 0;
std::int8_t gLegacyTitlePageCF49B4 = 0;
std::int8_t gLegacyTitlePageCountC8AF80 = 0;
std::int8_t gLegacyTitleDeleteCF4678 = 0;
LegacyPanelRecord gLegacyMapPanelD68868 {};
std::int32_t gLegacyMapDefaultX57CB24 = 392;
std::int32_t gLegacyMapDefaultYD67AD4 = 0;
constexpr char kLegacyMapEast58AA54[] = "\xAA\x46 %3d";
constexpr char kLegacyMapSouth58AA4C[] = "\xAB\x6E %3d";
LegacyPanelRecord gLegacyPointResultPanel5838A8 {
    236, 152, 168, 152, 41, {23385, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBattleResultPanel583970 {
    12, 74, 616, 308, 40, {23380, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::uint32_t gLegacyPointResultTickCC3E74 = 0;
std::uint32_t gLegacyBattleResultTickD3FAB8 = 0;
// 旧选择39；结果数据借窗口12，独立开关/时间槽不能套用其现代窗口状态机。
LegacyPanelRecord gLegacyGatherResultPanel583A38 {
    0, 0, 272, 440, 39, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGatherRepeat583AFC = 1;
std::uint32_t gLegacyGatherResultTickCA0AB8 = 0;
YobiAction* gLegacyGatherResultActionD67CD4 = nullptr;
constexpr char kLegacyGatherNoFp5864F0[] = "\xC5\x5D\xA4\x4F\xA4\xA3\xA8\xAC\xA1\x43";
constexpr char kLegacyGatherNoSpace589868[] = "\xAA\xAB\xAB\x7E\xC4\xE6\xA4\x77\xBA\xA1\xA1\x43";
constexpr char kLegacyGatherLimit589858[] = "   --   ";
constexpr char kLegacyGatherStop589848[] = "\xB1\x4E\xA6\x41\xB0\xF5\xA6\xE6\xA4\x40\xA6\xB8\xA1\x43";
constexpr char kLegacyGatherStart58983C[] = "\xA6\x41\xB0\xF5\xA6\xE6\xA4\x40\xA6\xB8";
LegacyPanelRecord gLegacySkillResultPanel583BC8 {
    0, 0, 272, 440, 37, {23356, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySkillResultActionD67CDC = nullptr;
LegacyPanelRecord gLegacyFpNoticePanel583E20 {
    160, 180, 320, 96, 34, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacySkillRetry589A94[] =
    "\xA6\x41\xA8\xCF\xA5\xCE\xA6\xB9\xA7\xDE\xAF\xE0\xA4\x40\xA6\xB8\xA1\x43";
constexpr char kLegacyFpConfirm55C5D4[] = "\xBD\x54\xA9\x77";
constexpr char kLegacyFpNotice589BFC[] = "\xC5\x5D\xA4\x4F\xA4\xA3\xB0\xF7\xA1\x43";
constexpr char kLegacyFpRequired589BE8[] =
    "\xBB\xDD\xAD\x6E%d\xC2\x49\xAA\xBA\xC5\x5D\xA4\x4F\xAD\xC8\xA1\x43";
LegacyPanelRecord gLegacyItemWorkResultPanel584140 {
    0, 0, 272, 440, 33, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyItemWorkResultActionD67CE0 = nullptr;
LegacyPanelRecord gLegacyItemWorkNamePanel584208 {
    368, 0, 272, 104, 32, {23355, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyItemWorkPanel5842D0 {
    368, 0, 272, 440, 31, {23354, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyItemWorkInputD67CE4 = nullptr;
std::string gLegacyItemWorkDraftCC31E0; // 仅已证NUL复制合同，不猜原固定容量。
std::int32_t gLegacyItemWorkInitGateD67CE8 = 0; // 只证W32测试/返回，尚无地址用途。
std::int32_t gLegacyItemWorkButtonKindD67CF4 = 0;
std::int32_t gLegacyItemWorkPhaseCC4220 = 0;
YobiAction* gLegacyItemWorkActionD67CEC = nullptr;
std::int32_t gLegacyItemWorkHover584398 = -1;
std::int32_t gLegacyItemWorkPageD67CF8 = 0;
std::uint32_t gLegacyItemWorkTickD2E12C = 0;
std::uint32_t gLegacyItemWorkDurationCC3EDC = 0;
std::string gLegacyItemWorkMessageCA0ACC; // 原NUL格式化/分行合同，未知阶段保留旧文本。
constexpr char kItemWorkText589F84[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xA8\xE8\xA6\x4C\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589F6C[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xC5\xB2\xA9\x77\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589F54[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xAD\xD7\xB2\x7A\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589F40[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xB9\xEF\xB6\x48\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589F2C[] = "\xBB\xDD\xAD\x6E%d\xC2\x49\xAA\xBA\xC5\x5D\xA4\x4F\xA1\x43";
constexpr char kItemWorkText589F1C[] = "\xA4\x77\xB8\x67\xC5\xB2\xA9\x77\xB9\x4C\xA4\x46\xA1\x43";
constexpr char kItemWorkText589F04[] = "\xA5\xBC\xB8\x67\xC5\xB2\xA9\x77\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xAD\xD7\xB2\x7A\xA1\x43";
constexpr char kItemWorkText589EF4[] = "\xA4\x77\xB8\x67\xA8\xE8\xA6\x4C\xB9\x4C\xA4\x46\xA1\x43";
constexpr char kItemWorkText589EDC[] = "\x0A\x0A      \xAD\xD7\xB2\x7A\xA4\xA4......";
constexpr char kItemWorkText589EC4[] = "\x0A\x0A      \xC5\xB2\xA9\x77\xA4\xA4......";
constexpr char kItemWorkText589EAC[] = "\x0A\x0A      \xA8\xE8\xA6\x4C\xA4\xA4......";
constexpr char kItemWorkText589E94[] = "\xB5\x4C\xAA\x6B\xA5\x48\xB8\xD3\xA7\xDE\xAF\xE0\xB6\x69\xA6\xE6\xAD\xD7\xB2\x7A\xA1\x43";
constexpr char kItemWorkText589E80[] = "\xB5\x4C\xAA\x6B\xAD\xD7\xB2\x7A\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589E58[] = "\xB8\xD3\xAA\xAB\xAB\x7E\xA5\x48\xB2\x7B\xA6\x62\xAA\xBA\xA7\xDE\xAF\xE0\xB5\xA5\xAF\xC5\xAC\x4F\xB5\x4C\xAA\x6B\xC5\xB2\xA9\x77\xAA\xBA\xA1\x43";
constexpr char kItemWorkText589E30[] = "\xB8\xD3\xAA\xAB\xAB\x7E\xA5\x48\xB2\x7B\xA6\x62\xAA\xBA\xA7\xDE\xAF\xE0\xB5\xA5\xAF\xC5\xAC\x4F\xB5\x4C\xAA\x6B\xAD\xD7\xB2\x7A\xAA\xBA\xA1\x43";
constexpr char kItemWorkText589E1C[] = "\xB5\x4C\xAA\x6B\xA8\xE8\xA6\x4C\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589E04[] = "\xB5\x4C\xAA\x6B\xA8\xE8\xA6\x4C\xA5\xBC\xC5\xB2\xA9\x77\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589DF4[] = "\xA8\xE8\xA6\x4C\xA9\xD2\xBF\xEF\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589DE0[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xA8\xE8\xA6\x4C\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589DD0[] = "\xC5\xB2\xA9\x77\xA9\xD2\xBF\xEF\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589DBC[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xC5\xB2\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589DAC[] = "\xAD\xD7\xB2\x7A\xA9\xD2\xBF\xEF\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589D98[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\xD7\xB2\x7A\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589D88[] = "\xA8\xE8\xA6\x4C\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589D74[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xA8\xE8\xA6\x4C\xA1\x43";
constexpr char kItemWorkText589D54[] = "\xA5\x48\xB2\x7B\xA6\x62\xAA\xBA\xA7\xDE\xAF\xE0\xB5\xA5\xAF\xC5\xAC\x4F\xB5\x4C\xAA\x6B\xC5\xB2\xA9\x77\xAA\xBA\xA1\x43";
constexpr char kItemWorkText589D44[] = "\xC5\xB2\xA9\x77\xB8\xD3\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kItemWorkText589D30[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xA4\x77\xC5\xB2\xA9\x77\xB9\x4C\xA4\x46\xA1\x43";
constexpr char kItemWorkText589D10[] = "\xA5\xBC\xB8\x67\xC5\xB2\xA9\x77\xAA\xBA\xAA\xAB\xAB\x7E\xAC\x4F\xB5\x4C\xAA\x6B\xAD\xD7\xB2\x7A\xAA\xBA\xA1\x43";
constexpr char kItemWorkText589024[] = "\xAD\xD7\xB2\x7A\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
LegacyPanelRecord gLegacyCraftResultPanel5843A0 {
    0, 0, 272, 440, 28, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyCraftResultActionD67CFC = nullptr;
LegacyPanelRecord gLegacyCraftExecutionPanel584530 {
    0, 0, 344, 380, 26, {23351, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyCraftExecutionActionD67D08 = nullptr;
std::uint32_t gLegacyCraftExecutionTickD2F78C = 0;
std::uint32_t gLegacyCraftExecutionDurationD2DC08 = 0;
constexpr char kLegacyCraftMissing58A098[] = "\xA7\xF7\xAE\xC6\xA5\xBC\xB3\x51\xB5\x6E\xBF\xFD\xA1\x43";
constexpr char kLegacyCraftExecute58A0A8[] = "\xB1\x4E\xA6\xB9\xA7\xF7\xAE\xC6\xA6\x58\xA6\xA8\xA1\x43";
constexpr char kLegacyCraftBack58A084[] = "\xA6\x5E\xA8\xEC\xA4\x57\xA4\x40\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43";
LegacyPanelRecord gLegacyCraftMaterialsPanel584468 {
    368, 0, 272, 380, 27, {23352, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 20> gLegacyCraftMarksC8AA7C {};
std::int32_t gLegacyCraftMaterialPageD67D04 = 0;
std::int32_t gLegacyCraftMaterialHoverD45F84 = 0; // 位于12B金额文本之后，非文本别名。
std::int32_t gLegacyCraftDragSlotD2F330 = 0;
std::int32_t gLegacyCraftDragImageCC3E7C = 0;
constexpr char kLegacyMaterialText58A078[] = "\xA7\xF7\xAE\xC6\xA4\xA3\xA8\xAC\xA1\x43";
constexpr char kLegacyMaterialText58A064[] = "\xA7\xF7\xAE\xC6\xAA\xBA\xBF\xEF\xBE\xDC\xA4\xA3\xA5\xBF\xBD\x54\xA1\x43";
constexpr char kLegacyMaterialText58A054[] = "\xA9\x7C\xA5\xBC\xB5\x6E\xBF\xFD\xA7\xF7\xAE\xC6\xA1\x43";
constexpr char kLegacyMaterialText58A044[] = "\xA7\xF7\xAE\xC6\xB5\x6E\xBF\xFD\xA4\x46\xA1\x43";
constexpr char kLegacyMaterialText58A034[] = "\xA5\x5B\xA4\x75\xA4\xA4......";
constexpr char kLegacyMaterialText58A010[] = "\xA8\x53\xA6\xB3\xBF\xEF\xBE\xDC\xA7\xF7\xAE\xC6\xA1\x43";
constexpr char kLegacyMaterialText58A020[] = "\xB5\x6E\xBF\xFD\xA9\xD2\xBF\xEF\xAA\xBA\xA7\xF7\xAE\xC6\xA1\x43";
constexpr char kLegacyMaterialText589FFC[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xA4\x77\xB3\x51\xBF\xEF\xBE\xDC\xA1\x43";
constexpr char kLegacyMaterialText589FEC[] = "\xBF\xEF\xBE\xDC\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
LegacyPanelRecord gLegacyCraftRecipePanel5845F8 {
    0, 0, 344, 380, 25, {23350, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyRecipePageCountD67D10 = 0;
constexpr char kLegacyRecipePage586D2C[] = "\xA4\xC1\xB4\xAB\xAD\xB6\xBC\xC6\xA1\x43";
constexpr char kLegacyRecipeBack58A0E0[] = "\xA6\x5E\xA8\xEC\xAF\xE0\xA4\x4F\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyRecipeUnavailable58A0B8[] = "\xB2\x7B\xA6\x62\xB5\x4C\xAA\x6B\xBB\x73\xA7\x40\xA6\xB9\xB6\xB5\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyRecipeSelect58A0D0[] = "\xBB\x73\xA7\x40\xA6\xB9\xB6\xB5\xAA\xAB\xAB\x7E\xA1\x43";
LegacyPanelRecord gLegacyInventoryPanel584C00 {
    368, 0, 272, 436, 14, {22540, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyInventoryActionD67D2C = nullptr;
std::int32_t gLegacyInventoryPageD67D28 = 0;
std::int32_t gLegacyInventoryHoverC8AF78 = 0;
constexpr std::array<std::array<std::int32_t, 2>, 28> kLegacyInventoryPositions584CC4 {{
    {69, 39}, {11, 149}, {11, 94}, {127, 94}, {127, 149}, {11, 39}, {127, 39}, {202, 144},
    {9, 215}, {61, 215}, {113, 215}, {165, 215}, {217, 215},
    {9, 268}, {61, 268}, {113, 268}, {165, 268}, {217, 268},
    {9, 321}, {61, 321}, {113, 321}, {165, 321}, {217, 321},
    {9, 374}, {61, 374}, {113, 374}, {165, 374}, {217, 374},
}};
constexpr char kLegacyInventoryTarget58A394[] =
    "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xB1\x4E\xAA\xAB\xAB\x7E\xA8\xCF\xA5\xCE\xA6\x62\xBD\xD6\xA8\xAD\xA4\x57\xA1\x43";
constexpr char kLegacyInventoryMoney58A37C[] = "\xA5\xB4\xB6\x7D\xAA\xF7\xB9\xF4\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyInventoryHead585C38[] = "\xB8\xCB\xB3\xC6\xC0\x59\xB3\xA1\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryBody585C20[] = "\xB8\xCB\xB3\xC6\xA8\xAD\xC5\xE9\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryHand585C08[] = "\xB8\xCB\xB3\xC6\xAA\x5A\xBE\xB9\xB8\xF2\xAC\xDE\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryFeet585BF0[] = "\xB8\xCB\xB3\xC6\xB8\x7D\xB3\xA1\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryAccessory585BDC[] = "\xB8\xCB\xB3\xC6\xB8\xCB\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryCrystal585BC8[] = "\xB8\xCB\xB3\xC6\xA4\xF4\xB4\xB9\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43";
constexpr char kLegacyInventoryMemo58A38C[] = "\xA1\x79%s\xA1\x7A";
LegacyPanelRecord gLegacyItemSplitPanel584078 {
    440, 0, 200, 200, 16, {23310, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacySplitConfirm589CC4[] = "\xAB\xFC\xA9\x77\xA4\xC0\xB0\x74\xAD\xD3\xBC\xC6";
constexpr char kLegacySplitEmpty589CB4[] = "\xBD\xD0\xAB\xFC\xA9\x77\xAD\xD3\xBC\xC6\xA1\x43";
constexpr char kLegacySplitBack589CA4[] = "\xA6\x5E\xA8\xEC\xAA\xAB\xAB\x7E\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacySplitAll589C98[] = "\xA5\xFE\xB3\xA1\xAB\xFC\xA9\x77\xA1\x43";
constexpr char kLegacySplitClear589C84[] = "\xB1\x4E\xAB\xFC\xA9\x77\xAD\xD3\xBC\xC6\xC2\x6B\xB9\x73\xA1\x43";
constexpr char kLegacySplitBackspace585B34[] = "\xB4\xEE\xA4\xD6\xA4\x40\xA6\xEC\xBC\xC6\xA1\x43";
constexpr char kLegacySplitLimit589C64[] = "\xB5\x4C\xAA\x6B\xAB\xFC\xA9\x77\xA7\xF3\xA6\x68\xAD\xD3\xBC\xC6\xA1\x43";
constexpr char kLegacySplitDigit589C78[] = "\xAB\xFC\xA9\x77\xAD\xD3\xBC\xC6\xA1\x43";
LegacyPanelRecord gLegacyMoneyPanel584DA8 {
    440, 0, 200, 200, 15, {22930, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMoneyAmountD2D618 = 0;
constexpr char kLegacyMoneyDrop585B54[] = "\xB1\x4E\xAB\xFC\xA9\x77\xAA\xBA\xAA\xF7\xC3\x42\xA9\xF1\xA6\x61\xA4\x57\xA1\x43";
constexpr char kLegacyMoneyEmpty585B44[] = "\xBD\xD0\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43";
constexpr char kLegacyMoneyAll585B28[] = "\xAB\xFC\xA9\x77\xA5\xFE\xC3\x42\xA1\x43";
constexpr char kLegacyMoneyClear585B14[] = "\xB1\x4E\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43";
constexpr char kLegacyMoneyDigit585B6C[] = "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43";
constexpr char kLegacyMoneyLimit58A3B4[] = "\xA4\xA3\xAF\xE0\xB6\x57\xA5\x58\xB3\x6F\xAD\xD3\xAA\xF7\xC3\x42\xA1\x43";
LegacyPanelRecord gLegacyEngravingPanel582FD8 {
    368, 0, 272, 252, 79, {23358, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyEngravingHover58309C = -1;
std::int32_t gLegacyEngravingPageC8AA9C = 0;
std::int32_t gLegacyEngravingTargetCC1A14 = 0;
constexpr char kLegacyEngravingSelect5895C0[] = "\xA8\xE8\xA6\x4C\xA6\xB9\xB9\x44\xA8\xE3\xA1\x43";
constexpr char kLegacyEngravingReject5895AC[] = "\xB3\x6F\xAD\xD3\xB9\x44\xA8\xE3\xB5\x4C\xAA\x6B\xA8\xE8\xA6\x4C\xA1\x43";
constexpr char kLegacyEngravingClose5868EC[] = "\xC3\xF6\xB3\xAC\xA6\xB9\xB5\xF8\xB5\xA1";
constexpr char kLegacyEngravingChoose589594[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xA8\xE8\xA6\x4C\xAA\xBA\xB9\x44\xA8\xE3\xA1\x43";
LegacyPanelRecord gLegacyItemUseStatusPanel582C78 {
    64, 108, 512, 240, 78, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacyItemUseCancel589008[] = "\xA8\xFA\x20\x20\xAE\xF8";
constexpr char kLegacyItemUsePrevious58597C[] = "\xA4\x57\xA4\x40\xAD\xB6";
constexpr char kLegacyItemUseTitle585F54[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xB9\xEF\xB6\x48\xA1\x43";
constexpr char kLegacyItemUseHeader58932C[] =
    "\xB0\xB7\xB1\x64\x20\x20\xB5\xA5\xAF\xC5\x20\xA9\x6D\xA6\x57\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
    "\xA5\xCD\xA9\x52\xA4\x4F\x20\x20\x20\x20\xC5\x5D\xA4\x4F";
LegacyPanelRecord gLegacyGemExecutionPanel58AB28 {
    0, 0, 344, 380, 29, {23351, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyGemActionD68A88 = nullptr;
std::uint32_t gLegacyGemTickD68AB4 = 0;
std::int32_t gLegacyGemDurationD68AB8 = 0;
std::int32_t gLegacyGemElapsedD68ABC = 0;
constexpr char kLegacyGemMissing58AC7C[] = "\xAF\xC0\xA7\xF7\xA8\x53\xA6\xB3\xB3\x51\xB5\x6E\xBF\xFD\xA1\x43";
constexpr char kLegacyGemExecute58ACB0[] = "\xC6\x5E\xA4\x4A\xC4\x5F\xA5\xDB\xA1\x43";
constexpr char kLegacyGemBack5871F0[] = "\xAA\xF0\xA6\x5E\xA7\xDE\xAF\xE0\xB5\xF8\xB5\xA1\xA1\x43";
LegacyPanelRecord gLegacyGemMaterialsPanel58AA60 {
    368, 0, 272, 380, 30, {23352, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGemMaterialHoverD68A84 = 0;
std::int32_t gLegacyGemDragImageD68AA4 = 0;
std::int32_t gLegacyGemDragSlotD68AA8 = 0;
std::int32_t gLegacyGemMaterialPageD68AAC = 0;
constexpr char kLegacyGemTooFew58ACA4[] = "\xAF\xC0\xA7\xF7\xA4\xA3\xA8\xAC\xA1\x43";
constexpr char kLegacyGemInvalid58AC90[] = "\xBF\xEF\xBE\xDC\xAA\xBA\xAF\xC0\xA7\xF7\xA4\xA3\xA5\xBF\xBD\x54\xA1\x43";
constexpr char kLegacyGemRegistered58AC6C[] = "\xAF\xC0\xA7\xF7\xA4\x77\xB5\x6E\xBF\xFD\xA1\x43";
constexpr char kLegacyGemWorking58AC58[] = "\xA5\x5B\xA4\x75\xA4\xA4\xA1\x45\xA1\x45\xA1\x45\xA1\x45\xA1\x45\xA1\x45";
constexpr char kLegacyGemNone58AC28[] = "\xA8\x53\xA6\xB3\xBF\xEF\xBE\xDC\xA8\xEC\xAF\xC0\xA7\xF7\xA1\x43";
constexpr char kLegacyGemRegister58AC3C[] = "\xB1\x4E\xBF\xEF\xA8\xEC\xAA\xBA\xB9\x44\xA8\xE3\xB5\x6E\xBF\xFD\xAC\xB0\xAF\xC0\xA7\xF7\xA1\x43";
constexpr char kLegacyGemSelected58AC10[] = "\xB3\x6F\xAD\xD3\xB9\x44\xA8\xE3\xA4\x77\xB8\x67\xB3\x51\xBF\xEF\xBE\xDC\xA1\x43";
constexpr char kLegacyGemSelect58AC00[] = "\xBF\xEF\xBE\xDC\xB3\x6F\xAD\xD3\xB9\x44\xA8\xE3\xA1\x43";
constexpr char kLegacyGemReject58ABEC[] = "\xB5\x4C\xAA\x6B\xBF\xEF\xBE\xDC\xB3\x6F\xAD\xD3\xB9\x44\xA8\xE3\xA1\x43";
constexpr char kLegacyCraftAdd589FC8[] = "\xA6\x5E\xA8\xEC\xB0\x6C\xA5\x5B\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyCraftChoose589FD8[] =
    "\xA6\x5E\xA8\xEC\xA5\xCD\xA6\xA8\xBF\xEF\xBE\xDC\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyResultLimit5897D4[] = "  --  ";
constexpr char kLegacyTitleSubmit5863C4[] =
    "\xB3\x5D\xA9\x77\xA6\xDB\xA4\x76\xAA\xBA\xBA\xD9\xB8\xB9\xA1\x43";
constexpr char kLegacyTitlePage58A98C[] = "\xB4\xAB\xAD\xB6\xA1\x43";
constexpr char kLegacyTitleDelete5863E8[] =
    "\xA7\x52\xB0\xA3\xA9\xD2\xBF\xEF\xBE\xDC\xAA\xBA\xBA\xD9\xB8\xB9\xA1\x43";
constexpr char kLegacyTitleCancelDelete58AA3C[] =
    "\xA4\xA3\xAF\xE0\xA7\x52\xB0\xA3\xBA\xD9\xB8\xB9\xA1\x43";
constexpr char kLegacyTitleClear5863FC[] =
    "\xA8\xFA\xAE\xF8\xA9\xD2\xBF\xEF\xBE\xDC\xAA\xBA\xBA\xD9\xB8\xB9\xA1\x43";
constexpr char kLegacyTitleRemove58AA1C[] =
    "\xA8\xFA\xAE\xF8\xB3\x6F\xAD\xD3\xBA\xD9\xB8\xB9\xA1\x43";
constexpr char kLegacyTitleSelect58AA2C[] =
    "\xB3\x5D\xA9\x77\xB3\x6F\xAD\xD3\xBA\xD9\xB8\xB9\xA1\x43";
std::uint32_t gLegacyAttributeBlinkCC3EF8 = 0;
std::uint32_t gLegacyAttributeTickD67440 = 0;

LegacyPanelRecord gLegacySettingsMenu584E70 {
    0, 0, 192, 304, 3, {22500, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 8> gLegacySettingsButtonsD67D38 {};
constexpr char kLegacySettingsText58A4A4[] = "\xB5\x6E\x20\x20\x20\x20\x20\x20\x20\xA5\x58";
constexpr char kLegacySettingsHint586DE8[] = "\xB5\x6E\xA5\x58\xA6\xF8\xAA\x41\xBE\xB9\xA1\x43";
constexpr char kLegacySettingsText58A498[] = "\xB2\xE1\x20\xA4\xD1\x20\xB3\x5D\x20\xA9\x77";
constexpr char kLegacySettingsHint58A484[] = "\xB3\x5D\xA9\x77\xB2\xE1\xA4\xD1\xAA\xBA\xBF\xEF\xB6\xB5\xA1\x43";
constexpr char kLegacySettingsText58A478[] = "\xAD\xB5\x20\xBC\xD6\x20\xB3\x5D\x20\xA9\x77";
constexpr char kLegacySettingsHint58A460[] = "\xB3\x5D\xA9\x77\xAD\x49\xB4\xBA\xAD\xB5\xBC\xD6\xAA\xBA\xBF\xEF\xB6\xB5\xA1\x43";
constexpr char kLegacySettingsText58A454[] = "\xAD\xB5\x20\xAE\xC4\x20\xB3\x5D\x20\xA9\x77";
constexpr char kLegacySettingsHint58A440[] = "\xB3\x5D\xA9\x77\xAD\xB5\xAE\xC4\xAA\xBA\xBF\xEF\xB6\xB5\xA1\x43";
constexpr char kLegacySettingsText58A434[] = "\xB7\xC6\x20\xB9\xAB\x20\xB3\x5D\x20\xA9\x77";
constexpr char kLegacySettingsHint58A420[] = "\xB3\x5D\xA9\x77\xB7\xC6\xB9\xAB\xAA\xBA\xBF\xEF\xB6\xB5\xA1\x43";
constexpr char kLegacySettingsText58A414[] = "\xC1\xE4\x20\xBD\x4C\x20\xBC\xF6\x20\xC1\xE4";
constexpr char kLegacySettingsHint58A400[] = "\xC1\xE4\xBD\x4C\xBC\xF6\xC1\xE4\xAA\xBA\xBB\xA1\xA9\xFA\xA1\x43";
constexpr char kLegacySettingsText58A3F4[] = "\xC3\xB8\x20\xB9\xCF\x20\xB3\x5D\x20\xA9\x77";
constexpr char kLegacySettingsHint58A3E4[] = "\xB3\x5D\xA9\x77\xC3\xB8\xB9\xCF\xBC\xD2\xA6\xA1\xA1\x43";
constexpr char kLegacySettingsText58A3D8[] = "\xB3\x5D\xA9\x77\xAB\xF6\xB6\x73";
constexpr char kLegacySettingsHint58A3C8[] = "\xB6\x69\xA6\xE6\xAB\xF6\xB6\x73\xB3\x5D\xA9\x77";

LegacyPanelRecord gLegacyChatSettings585000 {
    0, 0, 192, 304, 5, {22960, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 18> gLegacyChatRecordsD67E80 {};
constexpr char kLegacyChat58A584[] = "\xB3\x5D\xA9\x77\xC5\xE3\xA5\xDC\xA6\xE6\xBC\xC6";
constexpr char kLegacyChat58A57C[] = "\xA5\xD8\xAB\x65";
constexpr char kLegacyChat58A578[] = "\xA6\xE6";
constexpr char kLegacyChat58A574[] = "\xA1\xF4";
constexpr char kLegacyChat58A570[] = "\xA1\xF5";
constexpr char kLegacyChat58A560[] = "\xA4\xE5\xA6\x72\xC3\x43\xA6\xE2\xB3\x5D\xA9\x77";
constexpr char kLegacyChat58A554[] = "\xA5\xD8\xAB\x65\xC3\x43\xA6\xE2";
constexpr char kLegacyChat58A54C[] = "\x3C\xC5\xDC\xA7\xF3\x3E";
constexpr char kLegacyChat5877B4[] = "\xC5\xDC\xA7\xF3\xA4\xE5\xA6\x72\xAA\xBA\xC3\x43\xA6\xE2\xA1\x43";
constexpr char kLegacyChat58A540[] = "\xB1\x60\xA5\xCE\xA5\x79\xB3\x5D\xA9\x77";
constexpr char kLegacyChat58A538[] = "\x3C\xB3\x5D\xA9\x77\x3E";
constexpr char kLegacyChat58A524[] = "\xA5\xB4\xB6\x7D\xB1\x60\xA5\xCE\xA5\x79\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyChat58A514[] = "\xB3\x5D\xA9\x77\xB2\xE1\xA4\xD1\xAD\xB5\xAE\xC4";
constexpr char kLegacyChat58A508[] = "\xBD\x64\xB3\xF2\xB3\x5D\xA9\x77";
constexpr char kLegacyChat58A500[] = "\xB2\x7B\xA6\x62";
constexpr char kLegacyChat58A4F8[] = "\x20\xA4\x57\xA4\x40\xAD\xB6";
constexpr char kLegacyChat58A4E8[] = "\xA6\x5E\xA8\xEC\xA8\x74\xB2\xCE\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyChat58A668[] = "\xB6\x7D";
constexpr char kLegacyChat58A664[] = "\xC3\xF6";
constexpr char kLegacyChat58A654[] = "\xBC\x57\xA5\x5B\xC5\xE3\xA5\xDC\xA6\xE6\xBC\xC6\xA1\x43";
constexpr char kLegacyChat58A63C[] = "\xA4\xA3\xAF\xE0\xA6\x41\xBC\x57\xA5\x5B\xC5\xE3\xA5\xDC\xA6\xE6\xBC\xC6\xA4\x46\xA1\x43";
constexpr char kLegacyChat58A62C[] = "\xB4\xEE\xA4\xD6\xC5\xE3\xA5\xDC\xA6\xE6\xBC\xC6\xA1\x43";
constexpr char kLegacyChat58A614[] = "\xA4\xA3\xAF\xE0\xA6\x41\xB4\xEE\xA4\xD6\xC5\xE3\xA5\xDC\xA6\xE6\xBC\xC6\xA4\x46\xA1\x43";
constexpr char kLegacyChat58A604[] = "\xC3\xF6\xB3\xAC\xB2\xE1\xA4\xD1\xAD\xB5\xAE\xC4\xA1\x43";
constexpr char kLegacyChat58A5F4[] = "\xB6\x7D\xB1\xD2\xB2\xE1\xA4\xD1\xAD\xB5\xAE\xC4\xA1\x43";
constexpr char kLegacyChat58A5E0[] = "\xBC\x57\xA4\x6A\xB2\xE1\xA4\xD1\xAA\xBA\xBD\x64\xB3\xF2\xA1\x43";
constexpr char kLegacyChat58A5C4[] = "\xA4\xA3\xAF\xE0\xA6\x41\xBC\x57\xA4\x6A\xB2\xE1\xA4\xD1\xAA\xBA\xBD\x64\xB3\xF2\xA4\x46\xA1\x43";
constexpr char kLegacyChat58A5B0[] = "\xC1\x59\xA4\x70\xB2\xE1\xA4\xD1\xAA\xBA\xBD\x64\xB3\xF2\xA1\x43";
constexpr char kLegacyChat58A594[] = "\xA4\xA3\xAF\xE0\xA6\x41\xB4\xEE\xA4\xD6\xB2\xE1\xA4\xD1\xAA\xBA\xBD\x64\xB3\xF2\xA4\x46\xA1\x43";
constexpr char kLegacyChat553FB4[] = "\x25\x32\x64";
constexpr char kLegacyChat5531A4[] = "\x25\x64";

LegacyPanelRecord gLegacyMouseSettings585320 {
    0, 0, 192, 304, 9, {22963, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 11> gLegacyMouseRecordsD68358 {};
constexpr char kLegacyMouse58A7B0[] = "\xB7\xC6\xB9\xAB\xB4\xE5\xBC\xD0\xB3\x5D\xA9\x77";
constexpr char kLegacyMouse58A7A0[] = "\xB3\x5D\xA9\x77\xAA\xAB\xAB\x7E\xAA\xBA\xB2\xBE\xB0\xCA";
constexpr char kLegacyMouse58A790[] = "\xBD\xD5\xBE\xE3\xB7\xC6\xB9\xAB\xB3\x74\xAB\xD7";
constexpr char kLegacyMouse58A784[] = "\xA5\xD8\xAB\x65\xB3\x74\xAB\xD7";
constexpr char kLegacyMouse58A774[] = "\xA8\xEC\xA8\x74\xB2\xCE\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyMouse58A838[] = "\xA5\xBF\xB1\x60";
constexpr char kLegacyMouse58A830[] = "\xA5\xAD\xB7\xC6";
constexpr char kLegacyMouse58A820[] = "\x43\x74\x72\x6C\x2B\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4";
constexpr char kLegacyMouse58A814[] = "\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4";
constexpr char kLegacyMouse58A7FC[] = "\xC5\xDC\xA7\xF3\xAC\xB0\xA5\xBF\xB1\x60\xA1\x43";
constexpr char kLegacyMouse58A7EC[] = "\xC5\xDC\xA7\xF3\xAC\xB0\xA5\xAD\xB7\xC6\xA1\x43";
constexpr char kLegacyMouse58A7D4[] = "\xC5\xDC\xA7\xF3\xAC\xB0\x43\x74\x72\x6C\x2B\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4\xA1\x43";
constexpr char kLegacyMouse58A7C0[] = "\xC5\xDC\xA7\xF3\xAC\xB0\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4\xA1\x43";
constexpr char kLegacyMouse5535F8[] = "\x25\x73";
constexpr char kLegacyMouse58A80C[] = "\x25\x32\x2E\x31\x66";

LegacyPanelRecord gLegacyHotkeyHelp5853E8 {
    0, 0, 192, 304, 10, {22964, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 13> gLegacyHotkeyRecordsD68510 {};
constexpr char kLegacyHotkey58A8AC[] = "\xA4\x48\xAA\xAB\xA6\x57\xBA\xD9\xC5\xE3\xA5\xDC";
constexpr char kLegacyHotkey58A8A8[] = "\x46\x31\x31";
constexpr char kLegacyHotkey58A89C[] = "\xBF\xC3\xB9\xF5\xC2\x5E\xA8\xFA";
constexpr char kLegacyHotkey58A898[] = "\x46\x31\x32";
constexpr char kLegacyHotkey58A884[] = "\xB2\x4D\xB0\xA3\xBF\xC3\xB9\xF5\xB2\xE1\xA4\xD1\xB0\x54\xAE\xA7";
constexpr char kLegacyHotkey58A87C[] = "\x48\x4F\x4D\x45";
constexpr char kLegacyHotkey58A86C[] = "\xC3\xF6\xB3\xAC\xB2\xE1\xA4\xD1\xC0\x57\xB9\x44";
constexpr char kLegacyHotkey58A85C[] = "\x53\x68\x69\x66\x74\x20\x2B\x20\x48\x4F\x4D\x45";
constexpr char kLegacyHotkey58A850[] = "\xAB\x5F\xC0\x49\xAA\xCC\xA4\xE9\xBB\x78";
constexpr char kLegacyHotkey58A84C[] = "\x46\x39";
constexpr char kLegacyHotkey58A840[] = "\xB6\xC2\xA6\x57\xB3\xE6\xA6\x43\xAA\xED";
constexpr char kLegacyHotkey553F00[] = "\x45\x4E\x44";
LegacyPanelRecord gLegacyScanlineSettings5854B0 {
    0, 0, 192, 304, 11, {22965, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 4> gLegacyScanlineRecordsD68718 {};
constexpr char kLegacyScanline586E24[] = "\xB1\xBD\xB4\x79\xBD\x75\xA6\x50\xB4\xC1\xB3\x5D\xA9\x77";
constexpr char kLegacyScanline58A8F8[] = "\xB1\xBD\xB4\x79\xBD\x75\xA6\x50\xA8\x42";
constexpr char kLegacyScanline58A8E8[] = "\xB1\xBD\xB4\x79\xBD\x75\xAB\x44\xA6\x50\xA8\x42";
constexpr char kLegacyScanline58A8D4[] = "\xB1\xBD\xB4\x79\xBD\x75\xC5\xDC\xA7\xF3\xAC\xB0\xA6\x50\xA8\x42\xA1\x43";
constexpr char kLegacyScanline58A8BC[] = "\xB1\xBD\xB4\x79\xBD\x75\xC5\xDC\xA7\xF3\xAC\xB0\xAB\x44\xA6\x50\xA8\x42\xA1\x43";

LegacyPanelRecord gLegacyButtonSettings585578 {
    0, 0, 192, 304, 12, {22549, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 4> gLegacyButtonRecordsD687B8 {};
std::int32_t gLegacyUiHeight556C28 = 64;
std::int32_t gLegacyUiY556C2C = -1;
std::int32_t gLegacyUiWidth556C30 = 192;
std::int32_t gLegacyUiX62B39C = 0;
// 455490组只存已证的八槽初始化值；不据此公开未知消费者的记录布局。
std::array<std::array<std::int32_t, 8>, 5> gLegacyUiInitWords62B3B8 {};
std::array<std::int32_t, 8> gLegacyUiInitWords62B460 {};
std::array<std::int32_t, 8> gLegacyUiInitWords62B488 {};
std::array<std::array<std::int32_t, 8>, 3> gLegacyUiInitWords62B4A8 {};
std::array<std::array<std::int32_t, 8>, 5> gLegacyUiInitWords62B508 {};
std::int32_t gLegacyBottomWidth5858A0 = 640;
std::int32_t gLegacyBottomHeight5858A4 = 24;
std::int32_t gLegacyBottomXD68930 = 0;
std::int32_t gLegacyBottomYCB1DD8 = 0;
std::int32_t gLegacyBottomHeightCA0014 = 0;
std::int32_t gLegacyBottomHeightD2DAB4 = 0;
// 仅承接九个已证W32初始化值；不是原40B结构，未知+24H尾部及指针语义不定型。
std::array<std::array<std::int32_t, 9>, 8> gLegacyBottomInitWordsD68938 {};
constexpr char kLegacyButton58A914[] = "\xAB\xF6\xB6\x73\xB3\x5D\xA9\x77";
constexpr char kLegacyButton58A4BC[] = "\xA6\x5E\xA4\x57\xA4\x40\xAD\xB6";
constexpr char kLegacyButton58A904[] = "\xA6\x5E\xA8\xEC\xA8\x74\xB2\xCE\xBF\xEF\xB3\xE6\xA1\x43";
constexpr char kLegacyButton58A97C[] = "\xB2\xE1\xA4\xD1\xA4\xE5\xA6\x72\xA4\x6A\xA4\x70";
constexpr char kLegacyButton58A970[] = "\xB2\xE1\xA4\xD1\xBD\x64\xB3\xF2";
constexpr char kLegacyButton58A948[] =
    "\xB1\x4E\xB2\xE1\xA4\xD1\xA4\xE5\xA6\x72\xA4\x6A\xA4\x70\xAB\xF6\xB6\x73"
    "\xC5\xDC\xA7\xF3\xA6\xA8\xB2\xE1\xA4\xD1\xBD\x64\xB3\xF2\xAB\xF6\xB6\x73\xA1\x43";
constexpr char kLegacyButton58A920[] =
    "\xB1\x4E\xB2\xE1\xA4\xD1\xBD\x64\xB3\xF2\xAB\xF6\xB6\x73\xC5\xDC\xA7\xF3"
    "\xA6\xA8\xB2\xE1\xA4\xD1\xA4\xE5\xA6\x72\xA4\x6A\xA4\x70\xAB\xF6\xB6\x73\xA1\x43";

LegacyPanelRecord gLegacyLogoutPanel584F38 {
    0, 0, 192, 304, 4, {22504, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyLogout58A4D8[] = "\xA6\x5E\xA8\xEC\xAB\xB0\xA4\xBA\xB5\x6E\xA4\x4A\xC2\x49";
constexpr char kLegacyLogout58A4C8[] = "\xA6\xF8\xAA\x41\xBE\xB9\xBF\xEF\xBE\xDC\xB5\x65\xAD\xB1";
constexpr char kLegacyLogout553FD0[] = "\xC3\xF6\xB3\xAC\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyLogout586E60[] =
    "\xB5\x6E\xA5\x58\xAB\xE1\xA6\x5E\xA8\xEC\xA6\xF8\xAA\x41\xBE\xB9\xBF\xEF\xBE\xDC\xB5\x65\xAD\xB1\xA1\x43";
constexpr char kLegacyLogout586E48[] =
    "\xB5\x6E\xA5\x58\xA8\xC3\xA6\x5E\xA8\xEC\xAB\xB0\xA4\xBA\xB5\x6E\xA4\x4A\xC2\x49\xA1\x43";
constexpr char kLegacyLogout58A4B0[] = "\xA9\xF1\xB1\xF3\xB5\x6E\xA5\x58\xA1\x43";

std::int32_t Sub_4FC7F0()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(4));
    return 0;
}

std::int32_t Sub_4FC810()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(5));
    return 0;
}

std::int32_t Sub_4FC830()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(7));
    return 0;
}

std::int32_t Sub_4FC850()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(8));
    return 0;
}

std::int32_t Sub_4FC870()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(9));
    return 0;
}

std::int32_t Sub_4FC890()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(10));
    return 0;
}

std::int32_t Sub_4FC8B0()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(11));
    return 0;
}

std::int32_t Sub_4FC8D0()
{
    static_cast<void>(audio::Sub_5042D0(3));
    static_cast<void>(audio::Sub_504180(12));
    return 0;
}

std::int32_t InitializeLegacyUiSquareRecord(
    std::array<std::int32_t, 8>& record,
    const std::int32_t centerOffsetX, const std::int32_t centerOffsetY,
    const std::int32_t hitOffsetX, const std::int32_t normalImage)
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = gLegacyUiX62B39C;
    record[6] = normalImage;
    record[7] = normalImage + 1;
    record[0] = width / 2 + x + centerOffsetX;
    const std::int32_t height = gLegacyUiHeight556C28;
    record[2] = x + hitOffsetX;
    const std::int32_t y = gLegacyUiY556C2C;
    record[3] = y + 5;
    record[1] = height / 2 + y + centerOffsetY;
    record[4] = 27;
    record[5] = 27;
    return 27;
}

std::int32_t InitializeLegacyBottomBarPrefix(
    std::array<std::int32_t, 9>& record, const std::int32_t xOffset,
    const std::int32_t value1C)
{
    const std::int32_t width = gLegacyBottomWidth5858A0;
    const std::int32_t x = gLegacyBottomXD68930;
    record[4] = 80;
    record[0] = width / 2 + x;
    const std::int32_t height = gLegacyBottomHeight5858A4;
    record[2] = x + xOffset;
    const std::int32_t y = gLegacyBottomYCB1DD8;
    record[3] = y;
    record[1] = height / 2 + y;
    record[5] = 24;
    record[6] = 0;
    record[7] = value1C;
    record[8] = 0;
    return 0;
}

std::uint32_t Sub_4ECDF0()
{
    const std::uint32_t now = platform::ReadTickCount32();
    gLegacyPointResultTickCC3E74 = now;
    return now;
}

std::uint32_t Sub_4ED010()
{
    const std::uint32_t now = platform::ReadTickCount32();
    gLegacyBattleResultTickD3FAB8 = now;
    return now;
}

std::uint32_t Sub_501F60()
{
    if (world::detail::gValueD2F788 == 0) {
        gLegacyAttributeBlinkCC3EF8 = 0;
        gLegacyAttributeTickD67440 = 0;
        return 0;
    }
    if (gLegacyAttributeTickD67440 == 0) {
        gLegacyAttributeBlinkCC3EF8 = 1;
        const std::uint32_t now = platform::ReadTickCount32();
        gLegacyAttributeTickD67440 = now;
        return now;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gLegacyAttributeTickD67440 + 1000u < now) {
        gLegacyAttributeBlinkCC3EF8 = (gLegacyAttributeBlinkCC3EF8 - 1u) & 1u;
        const std::uint32_t next = platform::ReadTickCount32();
        gLegacyAttributeTickD67440 = next;
        return next;
    }
    return now;
}

} // namespace

LegacyPanelRecord gLegacyProfilePanel5857D0 {
    0, 0, 272, 438, 2, {22501, 22515, 22980, 0, 0},
    {{{1, 7, 416, 77, 16, kLegacyProfileTab5858D8},
      {1, 97, 416, 77, 16, kLegacyProfileTab5858C0},
      {1, 187, 416, 77, 16, kLegacyProfileTab5858A8}, {}, {}}},
    0, 0, {}, {},
};

std::int32_t Sub_455490()
{
    return Sub_4554A0();
}

std::int32_t Sub_4554A0()
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = 640 - width;
    gLegacyUiX62B39C = x;
    return x;
}

std::int32_t Sub_4554C0()
{
    return Sub_4554D0();
}

std::int32_t Sub_4554D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[0], -16, 0, 8, 22581);
}

std::int32_t Sub_455540()
{
    return Sub_455550();
}

std::int32_t Sub_455550()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[1], -16, 0, 38, 22583);
}

std::int32_t Sub_4555C0()
{
    return Sub_4555D0();
}

std::int32_t Sub_4555D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[2], -16, 0, 68, 22585);
}

std::int32_t Sub_455640()
{
    return Sub_455650();
}

std::int32_t Sub_455650()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[3], -16, 0, 98, 22587);
}

std::int32_t Sub_4556C0()
{
    return Sub_4556D0();
}

std::int32_t Sub_4556D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[4], -16, 0, 128, 22589);
}

std::int32_t Sub_455740()
{
    return Sub_455750();
}

std::int32_t Sub_455750()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B460, 76, -13, 158, 238544);
}

std::int32_t Sub_4557C0()
{
    return Sub_4557D0();
}

std::int32_t Sub_4557D0()
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = gLegacyUiX62B39C;
    auto& record = gLegacyUiInitWords62B488;
    record[4] = 65;
    record[5] = 17;
    record[6] = 22591;
    record[0] = width / 2 + x - 16;
    const std::int32_t height = gLegacyUiHeight556C28;
    record[2] = x + 90;
    const std::int32_t y = gLegacyUiY556C2C;
    const std::int32_t hitY = y + 38;
    record[1] = height / 2 + y;
    record[3] = hitY;
    record[7] = 22592;
    return hitY;
}

std::int32_t Sub_455840()
{
    return Sub_455850();
}

std::int32_t Sub_455850()
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = gLegacyUiX62B39C;
    const std::int32_t y = gLegacyUiY556C2C;
    auto& records = gLegacyUiInitWords62B4A8;
    records[0][6] = 22563;
    records[0][7] = 22564;
    const std::int32_t centerX = width / 2 + x - 16;
    const std::int32_t height = gLegacyUiHeight556C28;
    const std::int32_t hitX = x + 71;
    const std::int32_t centerY = height / 2 + y;
    const std::int32_t hitY = y + 38;
    records[0][3] = hitY;
    records[0][4] = 16;
    records[0][5] = 16;
    records[1][4] = 16;
    records[1][5] = 16;
    records[2][4] = 16;
    records[2][5] = 16;
    records[1][3] = hitY;
    records[2][3] = hitY;
    records[0][0] = centerX;
    records[0][1] = centerY;
    records[0][2] = hitX;
    records[1][0] = centerX;
    records[1][1] = centerY;
    records[1][2] = hitX;
    records[1][6] = 22561;
    records[1][7] = 22562;
    records[2][0] = centerX;
    records[2][1] = centerY;
    records[2][2] = hitX;
    records[2][6] = 22565;
    records[2][7] = 22566;
    return centerY;
}

std::int32_t Sub_455930()
{
    return Sub_455940();
}

std::int32_t Sub_455940()
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = gLegacyUiX62B39C;
    const std::int32_t y = gLegacyUiY556C2C;
    auto& records = gLegacyUiInitWords62B508;
    records[0][6] = 22990;
    records[0][7] = 22991;
    const std::int32_t centerX = width / 2 + x - 16;
    const std::int32_t height = gLegacyUiHeight556C28;
    const std::int32_t hitX = x + 71;
    const std::int32_t centerY = height / 2 + y;
    const std::int32_t hitY = y + 38;
    records[0][3] = hitY;
    records[0][4] = 16;
    records[0][5] = 16;
    records[1][4] = 16;
    records[1][5] = 16;
    records[2][4] = 16;
    records[2][5] = 16;
    records[3][4] = 16;
    records[3][5] = 16;
    records[4][4] = 16;
    records[4][5] = 16;
    records[1][3] = hitY;
    records[2][3] = hitY;
    records[3][3] = hitY;
    records[4][3] = hitY;
    records[0][0] = centerX;
    records[0][1] = centerY;
    records[0][2] = hitX;
    records[1][0] = centerX;
    records[1][1] = centerY;
    records[1][2] = hitX;
    records[1][6] = 22992;
    records[1][7] = 22993;
    records[2][0] = centerX;
    records[2][1] = centerY;
    records[2][2] = hitX;
    records[2][6] = 22994;
    records[2][7] = 22995;
    records[3][0] = centerX;
    records[3][1] = centerY;
    records[3][2] = hitX;
    records[3][6] = 22996;
    records[3][7] = 22997;
    records[4][0] = centerX;
    records[4][1] = centerY;
    records[4][2] = hitX;
    records[4][6] = 22998;
    records[4][7] = 22999;
    return centerY;
}

char* Sub_4E0270(const std::int32_t side)
{
    gLegacyStorageEmptySlotsCF49BC = 0;
    std::int32_t count = 0;
    char* record = BorrowLegacyStorageItem(side, 0);
    for (std::int32_t item = 0; item < 20; ++item, record += 0xC44) {
        if (ReadLegacyStorageW16(record, 0) == 0) {
            ++count;
        }
    }
    gLegacyStorageEmptySlotsCF49BC = count;
    return record;
}

std::int32_t Sub_4E0BC0(const std::int32_t side)
{
    std::int32_t hovered = -1;
    if (side < 0 || side > 1) {
        return hovered;
    }
    const auto& grid = kLegacyStorageGrid582C34[side];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t y = gLegacyStoragePanel582B70.y;
            const std::int32_t x = gLegacyStoragePanel582B70.x;
            if (Sub_5049A0(x + 52 * column + grid[0], y + 52 * row + grid[1],
                    x + 52 * column + grid[2], y + 52 * row + grid[3], -1) != 0) {
                hovered = column + 5 * row;
                break;
            }
        }
    }
    return hovered;
}

void Sub_4E0C70(const std::int32_t side, const std::int32_t item, const std::uint32_t color)
{
    if (side < 0 || side > 1) {
        return;
    }
    const auto& grid = kLegacyStorageGrid582C34[side];
    const std::int32_t row = 52 * (item / 5);
    const std::int32_t column = 52 * (item % 5);
    const std::int32_t y = gLegacyStoragePanel582B70.y;
    const std::int32_t x = gLegacyStoragePanel582B70.x;
    DrawGraphicHover(Low16(x + column + grid[0]), Low16(y + row + grid[1]),
        Low16(x + column + grid[2]), Low16(y + row + grid[3]), 213, color, 0);
}

std::int32_t Sub_4E0D10(const std::int32_t side)
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    if (side < 0 || side > 1) {
        return 0;
    }
    const auto& grid = kLegacyStorageGrid582C34[side];
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            char* const record = BorrowLegacyStorageItem(side, column + 5 * row);
            if (ReadLegacyStorageW16(record, 0) != 0) {
                if (gLegacyStorageDragSlotCC4C98 == column + 5 * row && side == 1) {
                    const std::int32_t y = gLegacyStoragePanel582B70.y;
                    const std::int32_t x = gLegacyStoragePanel582B70.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 212, 254, 0);
                }
                const std::int16_t selection = ReadLegacyStorageW16(record, 2);
                if (selection > 0) {
                    const std::int32_t y = gLegacyStoragePanel582B70.y;
                    const std::int32_t x = gLegacyStoragePanel582B70.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 213, 251, 0);
                } else if (selection == -1) {
                    const std::int32_t y = gLegacyStoragePanel582B70.y;
                    const std::int32_t x = gLegacyStoragePanel582B70.x;
                    static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                        y + 52 * row + grid[1] + 24, 218, 23040, 0, nullptr));
                }
                const std::int32_t image = ReadRecordW32(record, 4);
                const std::int32_t y = gLegacyStoragePanel582B70.y;
                const std::int32_t x = gLegacyStoragePanel582B70.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 214, image, 0, &descriptor));
                const std::int32_t count = ReadRecordW32(record, 8);
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyStoragePanel582B70.y + 52 * row + grid[1] + 31;
                    const std::int32_t countX = gLegacyStoragePanel582B70.x + 52 * column + grid[0] + 21;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
            if (ReadLegacyStorageW16(record, 2) == -2) {
                const std::int32_t y = gLegacyStoragePanel582B70.y;
                const std::int32_t x = gLegacyStoragePanel582B70.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 218, 23041, 0, nullptr));
            }
        }
    }
    return 20;
}

std::int32_t Sub_4E0FF0(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t item, const std::int32_t page)
{
    if (item < 0 || item >= 20) {
        return item;
    }
    const char* const record = BorrowLegacyStorageItem(side, item);
    if (ReadLegacyStorageW16(record, 0) == 0) {
        return item;
    }
    const std::int32_t flags = ReadRecordW32(record, 0xC40);
    Sub_504BA0(x, y, 272, 4, 0, record + 0x0C, record + 0xC29, 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    const char* text = record + 0x29 + page * 768;
    for (std::int32_t row = 0; row < 8; ++row, text += 96) {
        if (std::strlen(text) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, text, 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

std::int32_t Sub_4E10F0(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t centerX = x + 100;
    const std::int32_t centerY = y + 100;
    LegacyPanelButtonRecord confirm {centerX, centerY, x + 24, y + 167, 72, 18, 23314, 23314};
    LegacyPanelButtonRecord back {centerX, centerY, x + 107, y + 167, 72, 18, 0, 23316};
    LegacyPanelButtonRecord backspace {centerX, centerY, x + 138, y + 111, 52, 12, 0, 23311};
    LegacyPanelButtonRecord all {centerX, centerY, x + 138, y + 127, 52, 12, 0, 23312};
    LegacyPanelButtonRecord clear {centerX, centerY, x + 138, y + 143, 52, 12, 0, 23313};
    std::array<LegacyPanelButtonRecord, 10> digits;
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        digits[digit] = {centerX, centerY, x + 16 + 24 * (digit % 5),
            y + (digit < 5 ? 135 : 111), 20, 20, 0, 23320 + digit};
    }
    input::SetContext_44B330(nullptr);
    const std::int32_t count =
        world::detail::gPlayerItemsFDA8A4[gLegacyStorageSplitSourceD3FACC + 8].count;
    bool close = false;
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t amount = gLegacyStorageSplitAmountCB2220;
        if (amount != 0) {
            const std::int32_t source = gLegacyStorageSplitSourceD3FACC;
            const auto socket = platform::gConnectionSocket;
            const std::int32_t target = gLegacyStorageSplitTargetCC4D38;
            static_cast<void>(network::Sub_510EC0(socket, source + 8, target + 8, amount));
            gLegacyStorageSplitAmountCB2220 = 0;
            close = true;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        close = true;
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            gLegacyStorageSplitAmountCB2220 = count;
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            gLegacyStorageSplitAmountCB2220 = 0;
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            gLegacyStorageSplitAmountCB2220 /= 10;
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    gLegacyStorageSplitAmountCB2220 = 10 * gLegacyStorageSplitAmountCB2220 + digit;
                    changed = true;
                    break;
                }
            }
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    if (close) {
        if (input::IsCurrentContext_503E40(nullptr) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
        }
        gLegacyStorageSplitModeCA0AA8 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    // 原层关闭后仍处理键盘；上限用入口快照，显示则重新读取来源物品。
    static_cast<void>(Sub_502B80(&gLegacyStorageSplitAmountCB2220, 7));
    if (gLegacyStorageSplitAmountCB2220 > count) {
        gLegacyStorageSplitAmountCB2220 = count;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    char buffer[256];
    std::sprintf(buffer, "%7d",
        world::detail::gPlayerItemsFDA8A4[gLegacyStorageSplitSourceD3FACC + 8].count);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 46), 4, 1, 0, buffer, 0, 0));
    std::sprintf(buffer, "%7d", gLegacyStorageSplitAmountCB2220);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 86), 4, 1, 0, buffer, 0, 0));
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 223, 1, gValueD69768, 0);
    }
    if (gLegacyStorageSplitAmountCB2220 != 0) {
        confirm.normalImage = 0;
        confirm.pressedImage = 23315;
    } else {
        confirm.normalImage = 23314;
        confirm.pressedImage = 23314;
    }
    Sub_5035E0(&confirm, 223, 1, gValueD69768, 0);
    Sub_5035E0(&back, 223, 1, gValueD69768, 0);
    Sub_5035E0(&backspace, 223, 1, gValueD69768, 0);
    Sub_5035E0(&all, 223, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 223, 1, gValueD69768, 0);
    return DrawGraphic(centerX, centerY, 222, 23310, 0, nullptr);
}

std::uint32_t Sub_4E2030()
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    if (gLegacyStorageMessageD67C98 > 0 && gLegacyStorageMessageTickD67C9C != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyStorageMessageTickD67C9C + 2000u < now) {
            gLegacyStorageMessageD67C98 = 0;
            gLegacyStorageMessageTickD67C9C = 0;
        }
    }
    std::int32_t left = Sub_4E0BC0(0);
    std::int32_t right = Sub_4E0BC0(1);
    const std::int32_t splitMode = gLegacyStorageSplitModeCA0AA8;
    if (splitMode != 0) {
        left = -1;
        right = -1;
    }
    if (left != gLegacyStoragePreviousLeft582C64 || right != gLegacyStoragePreviousRight582C68) {
        gLegacyStorageDetailPageD67C94 = 0;
    }
    if (splitMode != 0) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + 96;
        const std::int32_t x = gLegacyStoragePanel582B70.x + 44;
        static_cast<void>(Sub_4E10F0(x, y));
    }
    static_cast<void>(Sub_4E0270(1));
    std::uint8_t mouse = static_cast<std::uint8_t>(gValueD68D9C);
    if (right >= 0 && (mouse & 0x10) != 0) {
        char* const record = BorrowLegacyStorageItem(1, right);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            const std::int16_t selection = ReadLegacyStorageW16(record, 2);
            if (selection >= 0) {
                std::int32_t sound = 55;
                if (selection == 1) {
                    WriteLegacyStorageSelection(record, 0);
                    sound = 54;
                } else if (gLegacyStoragePutCountD2DAD4 < gLegacyStorageCapacityD30404) {
                    const std::int32_t money = world::detail::ReadPlayerValueFC();
                    if (money - gLegacyStorageFeeD40B04 - gLegacyStorageMoneyD2F79C >= 1) {
                        WriteLegacyStorageSelection(record, static_cast<std::int16_t>(selection ^ 1));
                        sound = 79;
                    } else {
                        gLegacyStorageMessageD67C98 = 1;
                        gLegacyStorageMessageTickD67C9C = platform::ReadTickCount32();
                    }
                }
                static_cast<void>(audio::PlaySe_52C450(sound, 320));
                mouse = static_cast<std::uint8_t>(gValueD68D9C);
                gLegacyStorageDragSlotCC4C98 = -1;
                gLegacyStorageDragImageCB09D4 = 0;
            }
        }
    } else if (right >= 0 && (mouse & 1) != 0) {
        const std::int32_t source = gLegacyStorageDragSlotCC4C98;
        char* const target = BorrowLegacyStorageItem(1, right);
        if (source < 0) {
            if (ReadLegacyStorageW16(target, 0) != 0 && ReadLegacyStorageW16(target, 2) >= 0) {
                const std::int32_t image = ReadRecordW32(target, 4);
                gLegacyStorageDragSlotCC4C98 = right;
                gLegacyStorageDragImageCB09D4 = image;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                mouse = static_cast<std::uint8_t>(gValueD68D9C);
            }
        } else {
            if (source != right) {
                const std::uint32_t moveMode = platform::gValueD67AE8;
                const std::uint8_t control = platform::gKeyStates[17];
                const bool split = (moveMode != 0 ? control & 2 : (~control) & 2) != 0;
                if (ReadRecordW32(BorrowLegacyStorageItem(1, source), 8) > 0
                    && ReadLegacyStorageW16(target, 0) == 0 && split) {
                    gLegacyStorageSplitModeCA0AA8 = 1;
                    gLegacyStorageSplitSourceD3FACC = source;
                    gLegacyStorageSplitTargetCC4D38 = right;
                    gLegacyStorageSplitAmountCB2220 = 0;
                    gLegacyStorageDragSlotCC4C98 = -1;
                    gLegacyStorageDragImageCB09D4 = 0;
                    static_cast<void>(audio::PlaySe_52C450(56, 320));
                    mouse = static_cast<std::uint8_t>(gValueD68D9C);
                } else if (ReadLegacyStorageW16(target, 0) == 0 || ReadLegacyStorageW16(target, 2) >= 0) {
                    static_cast<void>(network::Sub_510EC0(platform::gConnectionSocket, source + 8, right + 8, 0));
                    gLegacyStorageDragSlotCC4C98 = -1;
                    gLegacyStorageDragImageCB09D4 = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                    mouse = static_cast<std::uint8_t>(gValueD68D9C);
                } else {
                    gLegacyStorageDragSlotCC4C98 = -1;
                    gLegacyStorageDragImageCB09D4 = 0;
                }
            } else {
                gLegacyStorageDragSlotCC4C98 = -1;
                gLegacyStorageDragImageCB09D4 = 0;
            }
        }
    } else {
        const std::int32_t source = gLegacyStorageDragSlotCC4C98;
        if ((mouse & 1) != 0 || (source >= 0 && (mouse & 2) != 0)) {
            if (source != -1) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                mouse = static_cast<std::uint8_t>(gValueD68D9C);
            }
            gLegacyStorageDragSlotCC4C98 = -1;
            gLegacyStorageDragImageCB09D4 = 0;
        } else if (right >= 0 && (mouse & 2) != 0) {
            ++gLegacyStorageDetailPageD67C94;
            if (gLegacyStorageDetailPageD67C94 >= ReadRecordW32(BorrowLegacyStorageItem(1, right), 0xC3C)) {
                gLegacyStorageDetailPageD67C94 = 0;
            }
        }
    }
    if (left >= 0) {
        char* const record = BorrowLegacyStorageItem(0, left);
        if ((mouse & 0x10) != 0) {
            if (ReadLegacyStorageW16(record, 0) != 0) {
                const std::int16_t selection = ReadLegacyStorageW16(record, 2);
                if (selection >= 0) {
                    if (selection == 1) {
                        WriteLegacyStorageSelection(record, 0);
                        static_cast<void>(audio::PlaySe_52C450(54, 320));
                    } else if (gLegacyStorageTakeCountCF4A04 < gLegacyStorageEmptySlotsCF49BC) {
                        WriteLegacyStorageSelection(record, static_cast<std::int16_t>(selection ^ 1));
                        static_cast<void>(audio::PlaySe_52C450(79, 320));
                    } else {
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    }
                }
            }
        } else if ((mouse & 2) != 0) {
            ++gLegacyStorageDetailPageD67C94;
            if (gLegacyStorageDetailPageD67C94 >= ReadRecordW32(record, 0xC3C)) {
                gLegacyStorageDetailPageD67C94 = 0;
            }
        }
    }
    const std::int32_t image = gLegacyStorageDragImageCB09D4;
    if (image != 0) {
        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        static_cast<void>(DrawGraphic(mouseX, mouseY, 230, image, 0, nullptr));
    }
    if (left >= 0) {
        Sub_4E0C70(0, left, gValueD69768);
        const std::int32_t page = gLegacyStorageDetailPageD67C94;
        const std::int32_t y = gLegacyStoragePanel582B70.y + 104;
        const std::int32_t x = gLegacyStoragePanel582B70.x + 283;
        static_cast<void>(Sub_4E0FF0(x, y, 0, left, page));
    } else if (right >= 0) {
        Sub_4E0C70(1, right, gValueD69768);
        const std::int32_t page = gLegacyStorageDetailPageD67C94;
        const std::int32_t y = gLegacyStoragePanel582B70.y + 104;
        const std::int32_t x = gLegacyStoragePanel582B70.x + 5;
        static_cast<void>(Sub_4E0FF0(x, y, 1, right, page));
    }
    static_cast<void>(Sub_4E0D10(0));
    static_cast<void>(Sub_4E0D10(1));
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t x = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y), 2, font, 0, text, 0, 0));
    };
    char buffer[64];
    const std::int32_t message = gLegacyStorageMessageD67C98;
    if (message == 1) {
        drawAt(kLegacyStorageNoMoney58906C, 63, 305, 0);
    } else if (message == 0) {
        drawAt(kLegacyStorageFee589284, 80, 305, 0);
        std::sprintf(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    std::sprintf(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    std::uint32_t result = static_cast<std::uint32_t>(
        DrawGraphic(backgroundX, backgroundY, 212, 23341, 0, &descriptor));
    const char* hint = nullptr;
    bool leftHint = false;
    if (left >= 0) {
        result = static_cast<std::uint32_t>(0xC44 * left);
        const char* const record = BorrowLegacyStorageItem(0, left);
        if (ReadLegacyStorageW16(record, 0) != 0 && ReadLegacyStorageW16(record, 2) >= 0) {
            leftHint = true;
            const std::int32_t flags = ReadRecordW32(record, 0xC40);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0x0C);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
                result = 0;
            } else if ((flags & 0x200) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0xC29);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
                result = 0;
            } else {
                hint = kLegacyStorageTake589268;
                result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
            }
        }
    }
    if (!leftHint && right >= 0) {
        result = static_cast<std::uint32_t>(0xC44 * right);
        const char* const record = BorrowLegacyStorageItem(1, right);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            const std::int32_t flags = ReadRecordW32(record, 0xC40);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0x0C);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
                result = 0;
            } else if ((flags & 0x200) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0xC29);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
                result = 0;
            } else {
                const std::int16_t selection = ReadLegacyStorageW16(record, 2);
                if (selection > 0) {
                    hint = kLegacyStoragePut589258;
                    result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
                } else if (selection < 0) {
                    hint = kLegacyStorageCannotPut5878BC;
                    result = 0;
                } else if (gLegacyStoragePutCountD2DAD4 >= gLegacyStorageCapacityD30404) {
                    hint = kLegacyStorageFull589230;
                    result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
                } else {
                    hint = kLegacyStorageCanPut589244;
                    result = 0;
                }
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    gLegacyStoragePreviousRight582C68 = right;
    gLegacyStoragePreviousLeft582C64 = left;
    return result;
}

std::uint32_t Sub_4E2D90()
{
    const std::int32_t x = gLegacyStoragePanel582B70.x;
    const std::int32_t y = gLegacyStoragePanel582B70.y;
    LegacyPanelButtonRecord confirm {x + 419, y + 276, x + 379, y + 266, 80, 20, 22712, 22713};
    LegacyPanelButtonRecord backspace {x + 480, y + 204, x + 454, y + 198, 52, 12, 0, 23260};
    LegacyPanelButtonRecord all {x + 480, y + 220, x + 454, y + 214, 52, 12, 0, 23261};
    LegacyPanelButtonRecord clear {x + 480, y + 236, x + 454, y + 230, 52, 12, 0, 23262};
    std::array<LegacyPanelButtonRecord, 10> digits;
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        const std::int32_t column = 24 * (digit % 5);
        const std::int32_t row = digit < 5 ? 232 : 208;
        digits[digit] = {x + 342 + column, y + row, x + 332 + column, y + row - 10,
            20, 20, 0, 23250 + digit};
    }
    const auto readDraft = [] {
        return ReadRecordW32(ui::detail::gShopTotalTextCC4B2C.data(), 44);
    };
    const auto writeDraft = [](const std::int32_t value) {
        std::memcpy(ui::detail::gShopTotalTextCC4B2C.data() + 44, &value, sizeof(value));
    };
    const std::int32_t side = ui::detail::ReadLegacyStorageSideD67618();
    // 非0/1模式取原var_21C已写几何值，不以默认上限或补零替换。
    std::int32_t maximum = x + 404;
    if (side == 0 || side == 1) {
        const std::int32_t limit = world::detail::ReadPlayerValue15BD0(side + 1);
        const std::int32_t amount = ReadRecordW32(
            ui::detail::gItemDetailStorageCC4D58.data() + side * 0xFBF8, 0xFBF4);
        maximum = limit - amount;
    }
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    if (maximum > money) {
        maximum = money;
    }
    if (gLegacyStorageDepositMessageD67CAC > 0 && gLegacyStorageDepositTickD67CB0 != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyStorageDepositTickD67CB0 + 2000u < now) {
            gLegacyStorageDepositMessageD67CAC = 0;
            gLegacyStorageDepositTickD67CB0 = 0;
        }
    }
    std::int32_t hoveredDigit = -1;
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t currentMoney = world::detail::ReadPlayerValueFC();
        const std::int32_t draft = readDraft();
        const std::int32_t fee = gLegacyStorageFeeD40B04;
        if (currentMoney - draft < fee) {
            gLegacyStorageDepositMessageD67CAC = 1;
            gLegacyStorageDepositTickD67CB0 = platform::ReadTickCount32();
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            gLegacyStorageMoneyD2F79C = draft;
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            writeDraft(maximum);
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            writeDraft(0);
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            writeDraft(readDraft() / 10);
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 0x40) != 0) {
                    hoveredDigit = digit;
                }
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    writeDraft(10 * readDraft() + digit);
                    changed = true;
                    break;
                }
            }
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    // 原4B槽别名char数组；键盘链仅同步读键态，借用结束即写回原区。
    std::int32_t keyboardDraft = readDraft();
    static_cast<void>(Sub_502B80(&keyboardDraft, 7));
    writeDraft(keyboardDraft);
    if (readDraft() > maximum) {
        writeDraft(maximum);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t left = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    char buffer[64];
    const std::int32_t displaySide = ui::detail::ReadLegacyStorageSideD67618();
    const std::int32_t balance = ReadRecordW32(
        ui::detail::gItemDetailStorageCC4D58.data() + displaySide * 0xFBF8, 0xFBF4);
    std::sprintf(buffer, "%7d", balance);
    drawAt(buffer, 146, 150, 0);
    std::sprintf(buffer, "%7d", maximum);
    drawAt(buffer, 421, 118, 0);
    std::sprintf(buffer, "%7d", readDraft());
    drawAt(buffer, 421, 158, 0);
    Sub_5035E0(&confirm, 223, 1, gValueD69768, 0);
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 223, 1, gValueD69768, 0);
    }
    Sub_5035E0(&backspace, 223, 1, gValueD69768, 0);
    Sub_5035E0(&all, 223, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 223, 1, gValueD69768, 0);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    static_cast<void>(DrawGraphic(backgroundX, backgroundY, 212, 23343, 0, nullptr));
    const std::int32_t message = gLegacyStorageDepositMessageD67CAC;
    if (message == 1) {
        drawAt(kLegacyStorageNoMoney58906C, 63, 305, 0);
    } else if (message == 0) {
        drawAt(kLegacyStorageFee589284, 80, 305, 0);
        std::sprintf(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    std::sprintf(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const char* hint;
    std::uint32_t result = 0;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        const std::int32_t currentMoney = world::detail::ReadPlayerValueFC();
        const std::int32_t draft = readDraft();
        const std::int32_t fee = gLegacyStorageFeeD40B04;
        if (currentMoney - draft < fee) {
            hint = kLegacyStorageCannotDeposit5892E8;
        } else {
            hint = kLegacyStorageDeposit5892F4;
            result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
        }
    } else if ((QueryPanelButtonState(&all) & 0x40) != 0) {
        hint = kLegacyMoneyDigit585B6C;
    } else if ((QueryPanelButtonState(&clear) & 0x40) != 0) {
        hint = kLegacyStorageClear5859FC;
    } else {
        const std::int32_t backspaceState = QueryPanelButtonState(&backspace);
        if ((backspaceState & 0x40) != 0) {
            hint = kLegacySplitBackspace585B34;
        } else if (hoveredDigit < 0) {
            return static_cast<std::uint32_t>(backspaceState);
        } else if (readDraft() < maximum) {
            hint = kLegacyMoneyDigit585B6C;
            result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
        } else {
            hint = kLegacyStorageAmountLimit5892D4;
        }
    }
    std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    return result;
}

std::uint32_t Sub_4E3750()
{
    const std::int32_t x = gLegacyStoragePanel582B70.x;
    const std::int32_t y = gLegacyStoragePanel582B70.y;
    LegacyPanelButtonRecord confirm {x + 141, y + 276, x + 101, y + 266, 80, 20, 22712, 22713};
    LegacyPanelButtonRecord backspace {x + 202, y + 204, x + 176, y + 198, 52, 12, 0, 23260};
    LegacyPanelButtonRecord all {x + 202, y + 220, x + 176, y + 214, 52, 12, 0, 23261};
    LegacyPanelButtonRecord clear {x + 202, y + 236, x + 176, y + 230, 52, 12, 0, 23262};
    std::array<LegacyPanelButtonRecord, 10> digits;
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        const std::int32_t column = 24 * (digit % 5);
        const std::int32_t row = digit < 5 ? 232 : 208;
        digits[digit] = {x + 64 + column, y + row, x + 54 + column, y + row - 10,
            20, 20, 0, 23250 + digit};
    }
    const std::int32_t side = ui::detail::ReadLegacyStorageSideD67618();
    const std::int32_t limit = world::detail::ReadPlayerValue15BD0(0);
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    std::int32_t maximum = limit - money;
    std::int32_t hoveredDigit = -1;
    const std::int32_t balance = ReadRecordW32(
        ui::detail::gItemDetailStorageCC4D58.data() + side * 0xFBF8, 0xFBF4);
    if (maximum > balance) {
        maximum = balance;
    }
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        gLegacyStorageWithdrawAmountCF49B8 = gLegacyStorageWithdrawDraftD2EA98;
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            gLegacyStorageWithdrawDraftD2EA98 = maximum;
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            gLegacyStorageWithdrawDraftD2EA98 = 0;
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            gLegacyStorageWithdrawDraftD2EA98 /= 10;
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 0x40) != 0) {
                    hoveredDigit = digit;
                }
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    gLegacyStorageWithdrawDraftD2EA98 = 10 * gLegacyStorageWithdrawDraftD2EA98 + digit;
                    changed = true;
                    break;
                }
            }
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    static_cast<void>(Sub_502B80(&gLegacyStorageWithdrawDraftD2EA98, 7));
    if (gLegacyStorageWithdrawDraftD2EA98 > maximum) {
        gLegacyStorageWithdrawDraftD2EA98 = maximum;
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t left = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    char buffer[64];
    std::sprintf(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 423, 151, 0);
    std::sprintf(buffer, "%7d", maximum);
    drawAt(buffer, 143, 118, 0);
    std::sprintf(buffer, "%7d", gLegacyStorageWithdrawDraftD2EA98);
    drawAt(buffer, 143, 158, 0);
    Sub_5035E0(&confirm, 223, 1, gValueD69768, 0);
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 223, 1, gValueD69768, 0);
    }
    Sub_5035E0(&backspace, 223, 1, gValueD69768, 0);
    Sub_5035E0(&all, 223, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 223, 1, gValueD69768, 0);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    static_cast<void>(DrawGraphic(backgroundX, backgroundY, 212, 23344, 0, nullptr));
    drawAt(kLegacyStorageBalance589314, 97, 305, 0);
    const std::int32_t displaySide = ui::detail::ReadLegacyStorageSideD67618();
    const std::int32_t currentBalance = ReadRecordW32(
        ui::detail::gItemDetailStorageCC4D58.data() + displaySide * 0xFBF8, 0xFBF4);
    std::sprintf(buffer, kLegacyStorageAmount588FC4, currentBalance);
    drawAt(buffer, 139, 306, 1);
    drawAt(kLegacyStorageFee589284, 358, 305, 0);
    std::sprintf(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
    drawAt(buffer, 417, 306, 1);
    const char* hint;
    std::uint32_t result = 0;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyStorageWithdraw589304;
        result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
    } else if ((QueryPanelButtonState(&all) & 0x40) != 0) {
        hint = kLegacyMoneyDigit585B6C;
    } else if ((QueryPanelButtonState(&clear) & 0x40) != 0) {
        hint = kLegacyStorageClear5859FC;
    } else {
        const std::int32_t backspaceState = QueryPanelButtonState(&backspace);
        if ((backspaceState & 0x40) != 0) {
            hint = kLegacySplitBackspace585B34;
        } else if (hoveredDigit < 0) {
            return static_cast<std::uint32_t>(backspaceState);
        } else if (gLegacyStorageWithdrawDraftD2EA98 < maximum) {
            hint = kLegacyMoneyDigit585B6C;
            result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
        } else {
            hint = kLegacyStorageAmountLimit5892D4;
        }
    }
    std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    return result;
}

std::uint32_t Sub_4E4460(const std::int32_t opened)
{
    const std::int32_t x = gLegacyItemUseStatusPanel582C78.x;
    const std::int32_t y = gLegacyItemUseStatusPanel582C78.y;
    character_select::detail::CharacterModalUiRecord cancel {
        x + 169, y + 202, x + 167, y + 202, 57, 18, kLegacyItemUseCancel589008,
    };
    character_select::detail::CharacterModalUiRecord previous {
        x + 289, y + 202, x + 287, y + 202, 57, 18, kLegacyItemUsePrevious58597C,
    };
    const std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    const std::int32_t state = Sub_503160(opened, &gLegacyItemUseStatusPanel582C78, nullptr, 0);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::int32_t hovered = -1;
    for (std::int32_t index = 0; index < ui::detail::ReadLegacyItemUseStatusCount(); ++index) {
        const std::int32_t top = gLegacyItemUseStatusPanel582C78.y + 20 * index;
        const std::int32_t left = gLegacyItemUseStatusPanel582C78.x;
        if (Sub_5049A0(left + 44, top + 74, left + 468, top + 94, -1) != 0) {
            hovered = index;
            break;
        }
    }
    const std::int32_t cancelState = character_select::detail::Sub_503990(&cancel);
    const std::int32_t previousState = character_select::detail::Sub_503990(&previous);
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        const std::int32_t item = gLegacyLastUsedItemCB01EC;
        const std::int32_t target = ui::detail::ReadLegacyItemUseStatus(hovered).target;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_510AC0(platform::gConnectionSocket, playerX, playerY, item, target));
        static_cast<void>(audio::Sub_504180(14));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyItemUseStatusPanel582C78.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((previousState & 1) != 0) {
        static_cast<void>(network::Sub_511CD0(platform::gConnectionSocket));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const std::int32_t titleY = gLegacyItemUseStatusPanel582C78.y + 24;
    const std::int32_t titleX = gLegacyItemUseStatusPanel582C78.x + 172;
    static_cast<void>(DrawText(static_cast<std::int16_t>(titleX),
        static_cast<std::int16_t>(titleY), 2, 0, 4, kLegacyItemUseTitle585F54, 0, 0));
    const std::int32_t headerY = gLegacyItemUseStatusPanel582C78.y + 50;
    const std::int32_t headerX = gLegacyItemUseStatusPanel582C78.x + 44;
    static_cast<void>(DrawText(static_cast<std::int16_t>(headerX),
        static_cast<std::int16_t>(headerY), 2, 0, 0, kLegacyItemUseHeader58932C, 0, 0));
    char buffer[128];
    for (std::int32_t index = 0; index < ui::detail::ReadLegacyItemUseStatusCount(); ++index) {
        const auto& record = ui::detail::ReadLegacyItemUseStatus(index);
        std::sprintf(buffer, "%3d", record.level);
        const std::int32_t levelY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 76;
        const std::int32_t levelX = gLegacyItemUseStatusPanel582C78.x + 107;
        static_cast<void>(DrawText(static_cast<std::int16_t>(levelX),
            static_cast<std::int16_t>(levelY), 2, 0, 0, buffer, 0, 0));
        std::sprintf(buffer, "%-16s", record.name.data());
        const std::uint8_t color = static_cast<std::uint8_t>(colors[record.kind]);
        const std::int32_t nameY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 76;
        const std::int32_t nameX = gLegacyItemUseStatusPanel582C78.x + 143;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, color, buffer, 0, 0));
        const std::int32_t maxFp = record.maxFp;
        const std::int32_t fp = record.fp;
        const std::int32_t maxHp = record.maxHp;
        const std::int32_t hp = record.hp;
        std::sprintf(buffer, "%4d/%4d %4d/%4d", hp, maxHp, fp, maxFp);
        const std::int32_t valuesY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 76;
        const std::int32_t valuesX = gLegacyItemUseStatusPanel582C78.x + 296;
        static_cast<void>(DrawText(static_cast<std::int16_t>(valuesX),
            static_cast<std::int16_t>(valuesY), 2, 0, 0, buffer, 0, 0));
        const std::int32_t image = ui::detail::ReadLegacyHealthImage(record.injury);
        const std::int32_t imageY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 84;
        const std::int32_t imageX = gLegacyItemUseStatusPanel582C78.x + 70;
        static_cast<void>(DrawGraphic(imageX, imageY, 213, image, 0, nullptr));
    }
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&previous, 2, 4, 1, gValueD69768, 218));
    if (hovered >= 0) {
        const std::int32_t top = gLegacyItemUseStatusPanel582C78.y + 20 * hovered;
        const std::uint32_t hoverImage = gValueD69768;
        const std::int32_t left = gLegacyItemUseStatusPanel582C78.x;
        DrawGraphicHover(Low16(left + 44), Low16(top + 74), Low16(left + 468), Low16(top + 94),
            218, hoverImage, 0);
    }
    return Sub_503440(&gLegacyItemUseStatusPanel582C78, 0);
}

void Sub_4E9280()
{
    gLegacyEngravingPageC8AA9C = 0;
    gLegacyEngravingTargetCC1A14 = -1;
}

std::uint32_t Sub_4E92A0(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyEngravingPanel582FD8, Sub_4E9280, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        return static_cast<std::uint32_t>(audio::Sub_5042D0(79));
    }
    std::int32_t hovered = -1;
    std::int32_t usedHovered = -1;
    std::int32_t hoverColumn;
    std::int32_t hoverRow;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyEngravingPanel582FD8.y + 36 + 53 * row;
            const std::int32_t left = gLegacyEngravingPanel582FD8.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 47, top + 47, -1) != 0) {
                hoverRow = row;
                hoverColumn = column;
                hovered = 8 + row * 5 + column;
                if (world::detail::gPlayerItemsFDA8A4[hovered].used != 0) {
                    usedHovered = hovered;
                }
                break;
            }
        }
    }
    if (hovered != gLegacyEngravingHover58309C) {
        gLegacyEngravingPageC8AA9C = 0;
    }
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    if (usedHovered >= 0 && (clicks & 1u) != 0) {
        const std::int32_t target = gLegacyEngravingTargetCC1A14;
        bool rejected = (target >= 0 && target == usedHovered) || usedHovered == gLegacySpecialUseItemC8A96C;
        if (!rejected) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
            if (item->itemType == 47) {
                rejected = true;
            } else {
                const std::uint16_t flags = item->flags;
                rejected = (flags & 0x100u) != 0 || item->count > 0
                    || ((flags & 0x200u) != 0 && (flags & 0x400u) == 0);
            }
        }
        if (rejected) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            gLegacyEngravingTargetCC1A14 = usedHovered;
        }
    } else if (hovered >= 0 && (clicks & 2u) != 0) {
        gLegacyEngravingPageC8AA9C ^= 1;
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[8 + row * 5 + column];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyEngravingPanel582FD8.y + 60 + 53 * row;
                const std::int32_t left = gLegacyEngravingPanel582FD8.x + 32 + 52 * column;
                static_cast<void>(DrawGraphic(left, top, 213, image, 0, nullptr));
                const std::int32_t count = item->count;
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyEngravingPanel582FD8.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyEngravingPanel582FD8.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyEngravingPanel582FD8.y + 53 * hoverRow;
        const std::int32_t left = gLegacyEngravingPanel582FD8.x + 52 * hoverColumn;
        DrawGraphicHover(Low16(left + 8), Low16(top + 36), Low16(left + 55), Low16(top + 83),
            212, gValueD69768, 0);
        const std::int32_t page = gLegacyEngravingPageC8AA9C;
        const std::int32_t y = gLegacyEngravingPanel582FD8.y;
        const std::int32_t height = gLegacyEngravingPanel582FD8.height;
        const std::int32_t x = gLegacyEngravingPanel582FD8.x;
        Sub_4FB940(x, y + height, hovered, page);
    }
    const std::int32_t target = gLegacyEngravingTargetCC1A14;
    if (target >= 0) {
        const std::int32_t source = gLegacySpecialUseItemC8A96C;
        static_cast<void>(network::Sub_511D60(platform::gConnectionSocket, target, source));
        static_cast<void>(audio::PlaySe_52C450(77, 320));
        static_cast<void>(audio::Sub_5042D0(79));
    }
    if (gLegacySpecialUseItemC8A96C >= 0) {
        const std::int32_t row = gLegacySpecialUseRowD2D614;
        const std::int32_t top = gLegacyEngravingPanel582FD8.y + 53 * row;
        const std::int32_t column = gLegacySpecialUseColumnCF4AA8;
        const std::int32_t left = gLegacyEngravingPanel582FD8.x + 52 * column;
        DrawGraphicHover(Low16(left + 10), Low16(top + 38), Low16(left + 53), Low16(top + 81),
            212, 251, 0);
    }
    // 原4E9621无悬停非负门禁；异常索引按RPLAN.24，不引入替代物品。
    const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if (item->used != 0) {
        const std::uint16_t flags = item->flags;
        if ((flags & 0x100u) != 0) {
            std::strcpy(hint, item->name.data());
            std::strcat(hint, "[kok]");
        } else if ((flags & 0x200u) != 0) {
            std::strcpy(hint, item->memo.data());
            std::strcat(hint, "[han]");
        } else {
            const bool selectable = item->itemType != 47 && item->count <= 0
                && usedHovered != gLegacySpecialUseItemC8A96C;
            std::strcpy(hint, selectable ? kLegacyEngravingSelect5895C0 : kLegacyEngravingReject5895AC);
        }
    } else {
        std::strcpy(hint, Sub_503590(&gLegacyEngravingPanel582FD8) != 0
            ? kLegacyEngravingClose5868EC : kLegacyEngravingChoose589594);
    }
    const std::uint32_t result = Sub_503440(&gLegacyEngravingPanel582FD8, 1);
    gLegacyEngravingHover58309C = hovered;
    return result;
}

std::uint32_t Sub_4ECE00(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPointResultPanel5838A8.x;
    const std::int32_t y = gLegacyPointResultPanel5838A8.y;
    LegacyPanelButtonRecord button {x + 84, y + 118, x + 34, y + 106, 100, 24, 23000, 23002};
    const std::int32_t state = Sub_503160(opened, &gLegacyPointResultPanel5838A8,
        [] { static_cast<void>(Sub_4ECDF0()); }, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::uint32_t restore = world::detail::gValueCAFEA0;
        if (restore != 0 && world::gCurrentSceneType != 10) {
            const std::int32_t result = audio::Sub_504180(1);
            world::detail::gValueCAFEA0 = 0;
            return static_cast<std::uint32_t>(result);
        }
        return restore;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gLegacyPointResultTickCC3E74 + 5000u < now) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPointResultPanel5838A8.id));
        if (world::detail::gValueCAFEA0 == 0) {
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    const std::int32_t buttonState = QueryPanelButtonState(&button);
    if ((buttonState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPointResultPanel5838A8.id));
        if (world::detail::gValueCAFEA0 == 0) {
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    char buffer[128];
    std::sprintf(buffer, "%+9d", world::detail::gValueFF0D04);
    const std::int32_t deltaY = gLegacyPointResultPanel5838A8.y;
    const std::int32_t deltaX = gLegacyPointResultPanel5838A8.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(deltaX + 69),
        static_cast<std::int16_t>(deltaY + 44), 2, 1, 0, buffer, 0, 0));
    std::sprintf(buffer, "%9d", world::detail::gValueFF0D08);
    const std::int32_t totalY = gLegacyPointResultPanel5838A8.y;
    const std::int32_t totalX = gLegacyPointResultPanel5838A8.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(totalX + 69),
        static_cast<std::int16_t>(totalY + 79), 2, 1, 0, buffer, 0, 0));
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    static_cast<void>(Sub_503440(&gLegacyPointResultPanel5838A8, 0));
    const std::uint32_t hitId = gLegacyPanelHitIdD046C0;
    if (hitId == 41 && (buttonState & 0x40) == 0) {
        world::detail::gValueC8A780 = 1;
        world::detail::gValueC8A77C = 1;
    }
    return hitId;
}

std::uint32_t Sub_4ED020(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBattleResultPanel583970.x;
    const std::int32_t y = gLegacyBattleResultPanel583970.y;
    LegacyPanelButtonRecord button {x + 164, y + 284, x + 114, y + 272, 100, 24, 23000, 23002};
    const std::int32_t state = Sub_503160(opened, &gLegacyBattleResultPanel583970,
        [] { static_cast<void>(Sub_4ED010()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (world::detail::gValueCAFEA0 != 0 && world::gCurrentSceneType != 10) {
            const std::int32_t result = audio::Sub_504180(1);
            world::detail::gValueCAFEA0 = 0;
            return static_cast<std::uint32_t>(result);
        }
        return static_cast<std::uint32_t>(state);
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gLegacyBattleResultTickD3FAB8 + 5000u < now) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBattleResultPanel583970.id));
        if (world::detail::gValueCAFEA0 == 0) {
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    const std::int32_t buttonState = QueryPanelButtonState(&button);
    if ((buttonState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBattleResultPanel583970.id));
        if (world::detail::gValueCAFEA0 == 0) {
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    char buffer[128];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font, const std::int8_t color = 0) {
        const std::int32_t top = gLegacyBattleResultPanel583970.y;
        const std::int32_t left = gLegacyBattleResultPanel583970.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, color, text, 0, 0));
    };
    const auto drawImage = [](const std::int32_t image, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        const std::int32_t top = gLegacyBattleResultPanel583970.y;
        const std::int32_t left = gLegacyBattleResultPanel583970.x;
        static_cast<void>(DrawGraphic(left + offsetX, top + offsetY, 212, image, 0, nullptr));
    };
    const auto& results = world::detail::BorrowBattleResults();
    for (std::int32_t row = 0; row < 4; ++row) {
        const world::detail::BattleResultRecord* const result = &results[row];
        const std::int16_t subject = std::bit_cast<std::int16_t>(result->value00);
        if (subject == -1) {
            continue;
        }
        std::int32_t injury;
        const char* name;
        if (subject == -2) {
            name = world::detail::BorrowPlayerDisplayText(1);
            injury = world::detail::ReadPlayerInjury();
        } else {
            const world::detail::PetRuntimeRecord* const pet = &world::detail::gPetRuntimeF633F8[subject];
            const bool hasUserName = pet->name6BD[0] != '\0';
            injury = pet->values70[25];
            name = hasUserName ? pet->name6BD.data() : pet->name6AC.data();
        }
        std::strcpy(buffer, name);
        const std::int32_t injuryImage = ui::detail::ReadLegacyHealthImage(injury);
        drawImage(injuryImage, 55, row * 24 + 68);
        drawAt(buffer, 71, row * 24 + 57, 0);
        if (result->value02 != 0) {
            drawImage(23382, 244, row * 24 + 67);
        }
        const std::int16_t currentSubject = std::bit_cast<std::int16_t>(result->value00);
        const std::int32_t level = currentSubject == -2
            ? world::detail::ReadPlayerLevel() : world::detail::gPetRuntimeF633F8[currentSubject].value08;
        if (level >= 130) {
            drawAt(kLegacyResultLimit5897D4, 267, row * 24 + 59, 1);
        } else {
            std::sprintf(buffer, "%+6d", result->value04);
            drawAt(buffer, 267, row * 24 + 59, 1);
        }
    }
    const auto& names = world::detail::BorrowBattleResultNames();
    for (std::int32_t row = 0; row < 3; ++row) {
        const char* const text = names[row].data();
        if (std::strlen(text) != 0) {
            const std::int32_t width = TextMeasure(text, 0);
            drawAt(text, 164 - width / 2, row * 24 + 186, 0);
        }
    }
    const auto& skillResults = world::detail::BorrowBattleSkillResults();
    std::int32_t visibleRow = 0;
    for (std::int32_t pairIndex = 0; pairIndex < 15; ++pairIndex) {
        const world::detail::SkillPairRecord* const pair = &world::detail::gSkillPairsF63370[pairIndex];
        const std::uint16_t used = pair->used;
        const std::int32_t skillIndex = std::bit_cast<std::int16_t>(pair->index);
        if (used == 0) {
            continue;
        }
        const world::detail::SkillRuntimeRecord* const skill = &world::detail::gSkillsF1BAFC[skillIndex];
        if (skill->canRebirth == 0) {
            drawAt(skill->name.data(), 357, visibleRow * 24 + 55, 0);
            const std::int32_t level = skill->level;
            const std::int32_t maxLevel = skill->maxLevel;
            if (level >= maxLevel) {
                drawAt(kLegacyResultLimit5897D4, 554, visibleRow * 24 + 57, 1);
            } else {
                std::sprintf(buffer, "%+6d", skillResults[skillIndex].value04);
                drawAt(buffer, 554, visibleRow * 24 + 57, 1);
            }
            if (skillResults[skillIndex].value00 != 0) {
                drawImage(23382, 531, visibleRow * 24 + 65);
            }
        } else {
            drawAt(skill->name.data(), 357, visibleRow * 24 + 55, 0, 7);
            drawAt(kLegacyResultLimit5897D4, 554, visibleRow * 24 + 57, 1, 7);
        }
        drawImage(23381, 469, visibleRow * 24 + 64);
        ++visibleRow;
    }
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    static_cast<void>(Sub_503440(&gLegacyBattleResultPanel583970, 1));
    const std::uint32_t hitId = gLegacyPanelHitIdD046C0;
    if (hitId == 40 && (buttonState & 0x40) == 0) {
        world::detail::gValueC8A780 = 1;
        world::detail::gValueC8A77C = 1;
    }
    return hitId;
}

std::uintptr_t Sub_4EDB20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGatherResultPanel583A38.x;
    const std::int32_t y = gLegacyGatherResultPanel583A38.y;
    LegacyPanelButtonRecord button {x + 140, y + 418, x + 100, y + 408, 80, 20, 23395, 23396};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGatherResultPanel583A38, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const bool notify = audio::ReadLegacyPanelSelectionCC236C() == 38
            ? world::detail::ReadPlayerTimerRemaining0C() <= 0
            : ui::detail::gValue5F9754 != 0;
        if (notify) {
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
            ui::detail::gValue5F9754 = 0;
        }
        YobiAction* action = gLegacyGatherResultActionD67CD4;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyGatherResultActionD67CD4 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }

    YobiAction* action = gLegacyGatherResultActionD67CD4;
    if (action != nullptr) {
        const std::int32_t requestedImage = world::detail::ReadPlayerImageNo();
        const std::int32_t mapped = Sub_504C50(GetYobiActionField2A8(action), requestedImage);
        action = gLegacyGatherResultActionD67CD4;
        if (mapped != GetYobiActionField2A8(action)) {
            SetYobiActionField2A8(action, mapped);
            if (mapped >= 110350 && mapped <= 110373) {
                SetYobiActionField309(gLegacyGatherResultActionD67CD4, 4);
                SetYobiActionField310(gLegacyGatherResultActionD67CD4, 4);
            } else {
                SetYobiActionField309(gLegacyGatherResultActionD67CD4, 0);
            }
            action = gLegacyGatherResultActionD67CD4;
        }
    }
    if (action == nullptr) {
        const std::int32_t top = gLegacyGatherResultPanel583A38.y;
        const std::int32_t left = gLegacyGatherResultPanel583A38.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        action = Sub_4FAAA0(image, left + 86, top + 212);
        gLegacyGatherResultActionD67CD4 = action;
    }
    Sub_4EF5F0(action, ui::detail::ReadGatherResultD2D820());
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    if ((QueryPanelButtonState(&button) & 1) != 0) {
        gLegacyGatherRepeat583AFC = gLegacyGatherRepeat583AFC != 1;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (gLegacyGatherRepeat583AFC == 1) {
        button.normalImage = 23395;
        button.pressedImage = 23396;
    } else {
        button.normalImage = 23390;
        button.pressedImage = 23391;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gLegacyGatherResultTickCA0AB8 + 2000u < now && gLegacyGatherRepeat583AFC == 1) {
        const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
        const std::int32_t cost = world::detail::gSkillsF1BAFC[skill].techniques3C[level].value7C
            * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
        if (world::detail::IsPlayerFpLessThan(cost)) {
            world::detail::Sub_44B050(kLegacyGatherNoFp5864F0, 4, 3);
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            gLegacyGatherRepeat583AFC = 0;
        } else if (world::detail::Sub_518C60() <= 0) {
            world::detail::Sub_44B050(kLegacyGatherNoSpace589868, 4, 3);
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            gLegacyGatherRepeat583AFC = 0;
        } else {
            static_cast<void>(audio::Sub_504180(38));
            ui::detail::WriteSkillExtraDelayCF46C4(0);
        }
    }

    char buffer[256];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyGatherResultPanel583A38.y;
        const std::int32_t left = gLegacyGatherResultPanel583A38.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, 0, text, 0, 0));
    };
    const char* const skillName = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t skillWidth = TextMeasure(skillName, 0);
    drawAt(skillName, 82 - skillWidth / 2, 37, 0);
    std::sprintf(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
        drawAt(buffer, 198, 62, 1);
    } else {
        drawAt(kLegacyGatherLimit589858, 198, 62, 1);
    }
    if (ui::detail::ReadGatherResultD2D820() != 0) {
        const std::int32_t width = TextMeasure(ui::detail::BorrowGatherResultNameD2DA94(), 0);
        drawAt(ui::detail::BorrowGatherResultNameD2DA94(), 136 - width / 2, 91, 0);
        const std::int32_t image = ui::detail::ReadGatherResultImageCF3FF0();
        const std::int32_t top = gLegacyGatherResultPanel583A38.y;
        const std::int32_t left = gLegacyGatherResultPanel583A38.x;
        static_cast<void>(DrawGraphic(left + 192, top + 184, 213, image, 0, &descriptor));
        drawAt(ui::detail::BorrowGatherResultSuffixCB1E1C(), 10, 114, 0);
    }
    for (std::int32_t row = 0; row < 8; ++row) {
        if (input::ExtractLegacyLine_503AB0(
                buffer, 256, ui::detail::BorrowGatherResultDetailsCC190C(), row, 28) != 0) {
            drawAt(buffer, 10, 237 + 20 * row, 0);
        }
    }
    if (Sub_503590(&gLegacyGatherResultPanel583A38) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&button) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(),
            gLegacyGatherRepeat583AFC == 1 ? kLegacyGatherStop589848 : kLegacyGatherStart58983C);
    }
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacyGatherResultPanel583A38, 1);
}

std::uintptr_t Sub_4EF0F0(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillResultPanel583BC8.x;
    const std::int32_t y = gLegacySkillResultPanel583BC8.y;
    LegacyPanelButtonRecord button {x + 140, y + 418, x + 100, y + 408, 80, 20, 23390, 23391};
    const std::int32_t state = Sub_503160(opened, &gLegacySkillResultPanel583BC8, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const bool pending = gLegacySkillPendingD67C48 != 0;
        const bool notify = pending ? world::detail::ReadPlayerTimerRemaining0C() <= 0
                                    : ui::detail::gValue5F9754 != 0;
        if (notify) {
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
            ui::detail::gValue5F9754 = 0;
        }
        if (pending) {
            gLegacySkillPendingD67C48 = 0;
        }
        YobiAction* action = gLegacySkillResultActionD67CDC;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacySkillResultActionD67CDC = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }

    YobiAction* action = gLegacySkillResultActionD67CDC;
    if (action != nullptr) {
        const std::int32_t requestedImage = world::detail::ReadPlayerImageNo();
        const std::int32_t mapped = Sub_504C50(GetYobiActionField2A8(action), requestedImage);
        action = gLegacySkillResultActionD67CDC;
        if (mapped != GetYobiActionField2A8(action)) {
            SetYobiActionField2A8(action, mapped);
            if (mapped >= 110350 && mapped <= 110373) {
                SetYobiActionField309(gLegacySkillResultActionD67CDC, 4);
                action = gLegacySkillResultActionD67CDC;
                SetYobiActionField310(action, static_cast<std::uint8_t>(GetYobiActionField310(action) | 4u));
            } else {
                SetYobiActionField309(gLegacySkillResultActionD67CDC, 0);
            }
            action = gLegacySkillResultActionD67CDC;
        }
    }
    if (action == nullptr) {
        const std::int32_t top = gLegacySkillResultPanel583BC8.y;
        const std::int32_t left = gLegacySkillResultPanel583BC8.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        action = Sub_4FAAA0(image, left + 86, top + 212);
        gLegacySkillResultActionD67CDC = action;
    }
    Sub_4EF5F0(action, ui::detail::ReadLegacySkillResultD66E2C());
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    if ((QueryPanelButtonState(&button) & 1) != 0) {
        const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
        const std::int32_t cost = world::detail::gSkillsF1BAFC[skill].techniques3C[level].value7C
            * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
        if (world::detail::IsPlayerFpLessThan(cost)) {
            // 原FP比较后重新取等级及两个乘数，不复用第一次计算值。
            const std::int32_t currentLevel = ui::detail::ReadSkillRequestD64E13();
            gLegacyRequiredFpCC31D8 = world::detail::gSkillsF1BAFC[skill].techniques3C[currentLevel].value7C
                * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
            static_cast<void>(audio::Sub_504180(34));
        } else {
            const std::int32_t currentPair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
            const std::uintptr_t socket = platform::gConnectionSocket;
            const std::int32_t currentSkill =
                std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[currentPair].index);
            static_cast<void>(network::Sub_511C30(socket, currentSkill));
            gLegacySkillPendingD67C48 = 1;
        }
    }
    char buffer[128];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font, const std::int8_t color = 0) {
        const std::int32_t top = gLegacySkillResultPanel583BC8.y;
        const std::int32_t left = gLegacySkillResultPanel583BC8.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, color, text, 0, 0));
    };
    const char* const skillName = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t skillWidth = TextMeasure(skillName, 0);
    drawAt(skillName, 82 - skillWidth / 2, 37, 0);
    std::sprintf(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
        drawAt(buffer, 198, 62, 1);
    } else {
        drawAt(kLegacyGatherLimit589858, 198, 62, 1);
    }
    const std::int32_t targetWidth = TextMeasure(ui::detail::gSkillTargetNameD2F748.data(), 0);
    drawAt(ui::detail::gSkillTargetNameD2F748.data(), 90 - targetWidth / 2, 91, 0);
    drawAt(ui::detail::BorrowLegacySkillSummaryCC18CC(), 10, 114, 0, 4);
    for (std::int32_t row = 0; row < 8; ++row) {
        if (input::ExtractLegacyLine_503AB0(
                buffer, 128, ui::detail::BorrowLegacySkillDetailsCB0FDC(), row, 28) != 0) {
            drawAt(buffer, 10, 237 + 20 * row, 0);
        }
    }
    if (Sub_503590(&gLegacySkillResultPanel583BC8) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&button) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillRetry589A94);
    }
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacySkillResultPanel583BC8, 1);
}

std::uint32_t Sub_4F0B40(const std::int32_t opened)
{
    const std::int32_t x = gLegacyFpNoticePanel583E20.x;
    const std::int32_t y = gLegacyFpNoticePanel583E20.y;
    character_select::detail::CharacterModalUiRecord button {
        x + 146, y + 64, x + 144, y + 64, 36, 18, kLegacyFpConfirm55C5D4,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyFpNoticePanel583E20, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (ui::detail::gValue5F9754 == 0) {
            return 0;
        }
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const std::int32_t result = network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62);
        ui::detail::gValue5F9754 = 0;
        return static_cast<std::uint32_t>(result);
    }
    if ((character_select::detail::Sub_503990(&button) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyFpNoticePanel583E20.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[256];
    std::sprintf(buffer, kLegacyFpNotice589BFC);
    const std::int32_t messageY = gLegacyFpNoticePanel583E20.y;
    const std::int32_t messageX = gLegacyFpNoticePanel583E20.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(messageX + 16),
        static_cast<std::int16_t>(messageY + 24), 2, 0, 0, buffer, 0, 0));
    std::sprintf(buffer, kLegacyFpRequired589BE8, gLegacyRequiredFpCC31D8);
    const std::int32_t valueY = gLegacyFpNoticePanel583E20.y;
    const std::int32_t valueX = gLegacyFpNoticePanel583E20.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(valueX + 16),
        static_cast<std::int16_t>(valueY + 44), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&button, 2, 4, 1, gValueD69768, 218));
    return Sub_503440(&gLegacyFpNoticePanel583E20, 0);
}

std::uintptr_t Sub_4F0F70(const std::int32_t opened)
{
    const std::int32_t width = gLegacyItemSplitPanel584078.width;
    const std::int32_t x = gLegacyItemSplitPanel584078.x;
    const std::int32_t height = gLegacyItemSplitPanel584078.height;
    const std::int32_t y = gLegacyItemSplitPanel584078.y;
    const std::int32_t centerX = x + width / 2;
    const std::int32_t centerY = y + height / 2;
    LegacyPanelButtonRecord confirm {centerX, centerY, x + 24, y + 167, 72, 18, 23314, 23314};
    LegacyPanelButtonRecord back {centerX, centerY, x + 107, y + 167, 72, 18, 0, 23316};
    LegacyPanelButtonRecord backspace {centerX, centerY, x + 138, y + 111, 52, 12, 0, 23311};
    LegacyPanelButtonRecord all {centerX, centerY, x + 138, y + 127, 52, 12, 0, 23312};
    LegacyPanelButtonRecord clear {centerX, centerY, x + 138, y + 143, 52, 12, 0, 23313};
    std::array<LegacyPanelButtonRecord, 10> digits;
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        digits[digit] = {centerX, centerY, x + 16 + 24 * (digit % 5),
            y + (digit < 5 ? 135 : 111), 20, 20, 0, 23320 + digit};
    }
    const std::int32_t state = Sub_503160(opened, &gLegacyItemSplitPanel584078, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (input::IsCurrentContext_503E40(nullptr) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            return reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        return 0;
    }
    input::SetContext_44B330(nullptr);
    const std::int32_t count = world::detail::gPlayerItemsFDA8A4[gLegacySplitSourceCC3E60].count;
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t amount = gLegacySplitAmountC8A948;
        if (amount != 0) {
            const std::int32_t target = gLegacySplitTargetCC21DC;
            const std::int32_t source = gLegacySplitSourceCC3E60;
            static_cast<void>(network::Sub_510EC0(platform::gConnectionSocket, source, target, amount));
            gLegacySplitAmountC8A948 = 0;
            static_cast<void>(audio::Sub_504180(14));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(14));
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            gLegacySplitAmountC8A948 = count;
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            gLegacySplitAmountC8A948 = 0;
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            gLegacySplitAmountC8A948 /= 10;
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    gLegacySplitAmountC8A948 = 10 * gLegacySplitAmountC8A948 + digit;
                    changed = true;
                    break;
                }
            }
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    // 确认/返回请求不结束原帧，键盘仍可改数量，随后才按入口快照限制。
    static_cast<void>(Sub_502B80(&gLegacySplitAmountC8A948, 7));
    if (gLegacySplitAmountC8A948 > count) {
        gLegacySplitAmountC8A948 = count;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    char buffer[256];
    std::sprintf(buffer, "%7d", world::detail::gPlayerItemsFDA8A4[gLegacySplitSourceCC3E60].count);
    const std::int32_t countY = gLegacyItemSplitPanel584078.y + 46;
    const std::int32_t countX = gLegacyItemSplitPanel584078.x + 88;
    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
    std::sprintf(buffer, "%7d", gLegacySplitAmountC8A948);
    const std::int32_t amountY = gLegacyItemSplitPanel584078.y + 86;
    const std::int32_t amountX = gLegacyItemSplitPanel584078.x + 88;
    static_cast<void>(DrawText(static_cast<std::int16_t>(amountX),
        static_cast<std::int16_t>(amountY), 2, 1, 0, buffer, 0, 0));
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 212, 1, gValueD69768, 0);
    }
    if (gLegacySplitAmountC8A948 != 0) {
        confirm.normalImage = 0;
        confirm.pressedImage = 23315;
    } else {
        confirm.normalImage = 23314;
        confirm.pressedImage = 23314;
    }
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&backspace, 212, 1, gValueD69768, 0);
    Sub_5035E0(&all, 212, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyItemSplitPanel584078) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = gLegacySplitAmountC8A948 != 0 ? kLegacySplitConfirm589CC4 : kLegacySplitEmpty589CB4;
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacySplitBack589CA4;
    } else if ((QueryPanelButtonState(&all) & 0x40) != 0) {
        hint = kLegacySplitAll589C98;
    } else if ((QueryPanelButtonState(&clear) & 0x40) != 0) {
        hint = kLegacySplitClear589C84;
    } else if ((QueryPanelButtonState(&backspace) & 0x40) != 0) {
        hint = kLegacySplitBackspace585B34;
    } else {
        for (auto& digit : digits) {
            if ((QueryPanelButtonState(&digit) & 0x40) != 0) {
                hint = gLegacySplitAmountC8A948 < count ? kLegacySplitDigit589C78 : kLegacySplitLimit589C64;
                break;
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyItemSplitPanel584078, 1);
}

std::uintptr_t Sub_4F1C40(const std::int32_t opened)
{
    const std::int32_t x = gLegacyItemWorkResultPanel584140.x;
    const std::int32_t y = gLegacyItemWorkResultPanel584140.y;
    LegacyPanelButtonRecord button {x + 140, y + 418, x + 100, y + 408, 80, 20, 23390, 23391};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyItemWorkResultPanel584140, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const bool notify = audio::ReadLegacyPanelSelectionCC236C() == 31
            ? world::detail::ReadPlayerTimerRemaining0C() <= 0
            : ui::detail::gValue5F9754 != 0;
        if (notify) {
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
            ui::detail::gValue5F9754 = 0;
        }
        YobiAction* action = gLegacyItemWorkResultActionD67CE0;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyItemWorkResultActionD67CE0 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    YobiAction* action = gLegacyItemWorkResultActionD67CE0;
    if (action != nullptr) {
        const std::int32_t requestedImage = world::detail::ReadPlayerImageNo();
        const std::int32_t mapped = Sub_504C50(GetYobiActionField2A8(action), requestedImage);
        action = gLegacyItemWorkResultActionD67CE0;
        if (mapped != GetYobiActionField2A8(action)) {
            SetYobiActionField2A8(action, mapped);
            if (mapped >= 110350 && mapped <= 110373) {
                SetYobiActionField309(gLegacyItemWorkResultActionD67CE0, 4);
                SetYobiActionField310(gLegacyItemWorkResultActionD67CE0, 4);
            } else {
                SetYobiActionField309(gLegacyItemWorkResultActionD67CE0, 0);
            }
            action = gLegacyItemWorkResultActionD67CE0;
        }
    }
    if (action == nullptr) {
        const std::int32_t top = gLegacyItemWorkResultPanel584140.y;
        const std::int32_t left = gLegacyItemWorkResultPanel584140.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        action = Sub_4FAAA0(image, left + 86, top + 212);
        gLegacyItemWorkResultActionD67CE0 = action;
    }
    Sub_4EF5F0(action, ui::detail::ReadLegacyItemWorkResultCB05FC());
    if ((QueryPanelButtonState(&button) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyItemWorkResultPanel584140.id));
        static_cast<void>(audio::Sub_504180(31));
    }
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    char buffer[256];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyItemWorkResultPanel584140.y;
        const std::int32_t left = gLegacyItemWorkResultPanel584140.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, 0, text, 0, 0));
    };
    const char* const name = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t width = TextMeasure(name, 0);
    drawAt(name, 82 - width / 2, 37, 0);
    std::sprintf(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    const bool belowMaximum =
        world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel;
    // 原满级分支仍格式化一次，只替换最终绘制文本。
    std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
    drawAt(belowMaximum ? buffer : kLegacyGatherLimit589858, 198, 62, 1);
    if (gLegacyItemWorkSelection584394 >= 0) {
        const std::int32_t itemWidth = TextMeasure(gLegacyItemWorkNameCC40D8.data(), 0);
        drawAt(gLegacyItemWorkNameCC40D8.data(), 136 - itemWidth / 2, 91, 0);
        const std::int32_t image = gLegacyItemWorkImageCC320C;
        const std::int32_t top = gLegacyItemWorkResultPanel584140.y;
        const std::int32_t left = gLegacyItemWorkResultPanel584140.x;
        static_cast<void>(DrawGraphic(left + 192, top + 184, 212, image, 0, &descriptor));
    }
    drawAt(ui::detail::BorrowLegacyItemWorkSuffixD2DADC(), 10, 114, 0);
    for (std::int32_t row = 0; row < 8; ++row) {
        if (input::ExtractLegacyLine_503AB0(
                buffer, 256, ui::detail::BorrowLegacyItemWorkDetailsD2DB00(), row, 28) != 0) {
            drawAt(buffer, 10, 237 + 20 * row, 0);
        }
    }
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    if (Sub_503590(&gLegacyItemWorkResultPanel584140) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&button) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillRetry589A94);
    }
    return Sub_503440(&gLegacyItemWorkResultPanel584140, 1);
}

std::uintptr_t Sub_4F3ED0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyCraftResultPanel5843A0.x;
    const std::int32_t y = gLegacyCraftResultPanel5843A0.y;
    LegacyPanelButtonRecord button {x + 140, y + 418, x + 100, y + 408, 80, 20, 23390, 23391};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyCraftResultPanel5843A0, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::uint32_t selection = audio::ReadLegacyPanelSelectionCC236C();
        const bool notify = selection == 25 || selection == 29
            ? world::detail::ReadPlayerTimerRemaining0C() <= 0
            : ui::detail::gValue5F9754 != 0;
        if (notify) {
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
            ui::detail::gValue5F9754 = 0;
        }
        YobiAction* action = gLegacyCraftResultActionD67CFC;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyCraftResultActionD67CFC = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    YobiAction* action = gLegacyCraftResultActionD67CFC;
    if (action != nullptr) {
        const std::int32_t requestedImage = world::detail::ReadPlayerImageNo();
        const std::int32_t mapped = Sub_504C50(GetYobiActionField2A8(action), requestedImage);
        action = gLegacyCraftResultActionD67CFC;
        if (mapped != GetYobiActionField2A8(action)) {
            SetYobiActionField2A8(action, mapped);
            if (mapped >= 110350 && mapped <= 110373) {
                SetYobiActionField309(gLegacyCraftResultActionD67CFC, 4);
                SetYobiActionField310(gLegacyCraftResultActionD67CFC, 4);
            } else {
                SetYobiActionField309(gLegacyCraftResultActionD67CFC, 0);
            }
            action = gLegacyCraftResultActionD67CFC;
        }
    }
    if (action == nullptr) {
        const std::int32_t top = gLegacyCraftResultPanel5843A0.y;
        const std::int32_t left = gLegacyCraftResultPanel5843A0.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        action = Sub_4FAAA0(image, left + 86, top + 212);
        gLegacyCraftResultActionD67CFC = action;
    }
    Sub_4EF5F0(action, ui::detail::ReadLegacyCraftResultCC46D8());
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    if ((QueryPanelButtonState(&button) & 1) != 0) {
        const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
        if (skillId == 255 || skillId == 606 || skillId == 2026) {
            static_cast<void>(audio::Sub_504180(29));
            static_cast<void>(audio::Sub_504180(30));
        } else {
            static_cast<void>(audio::Sub_504180(25));
        }
    }
    char buffer[256];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyCraftResultPanel5843A0.y;
        const std::int32_t left = gLegacyCraftResultPanel5843A0.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, 0, text, 0, 0));
    };
    const char* const name = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t width = TextMeasure(name, 0);
    drawAt(name, 82 - width / 2, 37, 0);
    std::sprintf(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        std::sprintf(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
        drawAt(buffer, 198, 62, 1);
    } else {
        drawAt(kLegacyGatherLimit589858, 198, 62, 1);
    }
    const std::int32_t itemWidth = TextMeasure(ui::detail::BorrowLegacyCraftNameCF46D4(), 0);
    drawAt(ui::detail::BorrowLegacyCraftNameCF46D4(), 136 - itemWidth / 2, 91, 0);
    const std::int32_t image = ui::detail::ReadLegacyCraftImageCF4B60();
    const std::int32_t top = gLegacyCraftResultPanel5843A0.y;
    const std::int32_t left = gLegacyCraftResultPanel5843A0.x;
    static_cast<void>(DrawGraphic(left + 192, top + 184, 212, image, 0, &descriptor));
    drawAt(ui::detail::BorrowLegacyCraftSuffixD672B4(), 10, 114, 0);
    for (std::int32_t row = 0; row < 8; ++row) {
        if (input::ExtractLegacyLine_503AB0(
                buffer, 256, ui::detail::BorrowLegacyCraftDetailsD2E92C(), row, 28) != 0) {
            drawAt(buffer, 10, 237 + 20 * row, 0);
        }
    }
    Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    if (Sub_503590(&gLegacyCraftResultPanel5843A0) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&button) & 0x40) != 0) {
        const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
        std::strcpy(ui::detail::gHoverTextD68AE8.data(),
            skillId == 255 || skillId == 606 || skillId == 2026
                ? kLegacyCraftAdd589FC8 : kLegacyCraftChoose589FD8);
    }
    return Sub_503440(&gLegacyCraftResultPanel5843A0, 1);
}

std::uintptr_t Sub_4F2130(const std::int32_t opened)
{
    const std::int32_t y = gLegacyItemWorkNamePanel584208.y;
    const std::int32_t x = gLegacyItemWorkNamePanel584208.x;
    LegacyPanelButtonRecord confirm {x + 218, y + 50, x + 178, y + 40, 80, 20, 22712, 22713};
    LegacyPanelButtonRecord back {x + 218, y + 78, x + 178, y + 68, 80, 20, 22710, 22711};
    const std::int32_t state = Sub_503160(opened, &gLegacyItemWorkNamePanel584208, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    input::InputContext* context = gLegacyItemWorkInputD67CE4;
    if (state == 2) {
        std::uintptr_t result = 0;
        if (input::IsCurrentContext_503E40(context) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyItemWorkInputD67CE4 = nullptr;
        return result;
    }
    if (context == nullptr) {
        const std::int32_t top = gLegacyItemWorkNamePanel584208.y;
        const std::int32_t left = gLegacyItemWorkNamePanel584208.x;
        gLegacyItemWorkInputD67CE4 = &input::gInputContextCB01F8;
        input::gInputContextCB01F8.text[0] = '\0';
        input::gInputContextCB01F8.totalByteLength = 0;
        input::gInputContextCB01F8.caretByteOffset = 0;
        input::InitializeContext_449CF0(&input::gInputContextCB01F8,
            left + 19, top + 41, 2, 0, 0, input::LegacyText593850(), 1, 16, 0, 0);
        input::SetContext_44B330(gLegacyItemWorkInputD67CE4);
    }
    char trimmed[32];
    bool submit = false;
    if ((QueryPanelButtonState(&confirm) & 1) != 0
        && (world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8].flags & 0x400u) != 0) {
        if (input::Sub_45E770(gLegacyItemWorkInputD67CE4->text) == 1) {
            static_cast<void>(audio::Sub_504180(81));
        } else {
            std::strcpy(trimmed, gLegacyItemWorkInputD67CE4->text);
            static_cast<void>(input::RemoveLegacyBytes_52D570(trimmed, " "));
            static_cast<void>(input::RemoveLegacyBytes_52D570(trimmed, "\xA1\x40"));
            if (std::strlen(trimmed) == 0 && std::strlen(gLegacyItemWorkInputD67CE4->text) != 0) {
                static_cast<void>(audio::Sub_504180(82));
            } else {
                submit = true;
            }
        }
    } else if ((QueryPanelButtonState(&confirm) & 1) != 0
               && std::strlen(gLegacyItemWorkInputD67CE4->text) != 0) {
        std::strcpy(trimmed, gLegacyItemWorkInputD67CE4->text);
        static_cast<void>(input::RemoveLegacyBytes_52D570(trimmed, " "));
        static_cast<void>(input::RemoveLegacyBytes_52D570(trimmed, "\xA1\x40"));
        if (std::strlen(trimmed) == 0) {
            static_cast<void>(audio::Sub_504180(82));
        } else if (input::Sub_45E770(gLegacyItemWorkInputD67CE4->text) == 1) {
            static_cast<void>(audio::Sub_504180(81));
        } else {
            submit = true;
        }
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(31));
    }
    if (submit) {
        gLegacyItemWorkDraftCC31E0 = gLegacyItemWorkInputD67CE4->text;
        static_cast<void>(Sub_4F24A0());
        static_cast<void>(audio::Sub_5042D0(100));
    }
    render::DrawInputText_4568A0(gLegacyItemWorkInputD67CE4);
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacyItemWorkNamePanel584208, 1);
}

std::int32_t Sub_4F24A0()
{
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    char request[256];
    char source[256];
    std::strcpy(request, gLegacyItemWorkDraftCC31E0.c_str());
    std::strcpy(source, gLegacyItemWorkDraftCC31E0.c_str());
    NullSub_509080();
    const std::int32_t item = gLegacyItemWorkSelection584394 + 8;
    const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::Sub_511EF0(socket, skill, level, item, request));
    if (std::strlen(source) != 0) {
        std::strncpy(gLegacyItemWorkNameCC40D8.data(), source, 64);
    } else {
        std::strncpy(gLegacyItemWorkNameCC40D8.data(),
            world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8].name.data(), 64);
    }
    const std::int32_t selected = gLegacyItemWorkSelection584394;
    const std::uint32_t panelId = gLegacyItemWorkPanel5842D0.id;
    gLegacyItemWorkImageCC320C = world::detail::gPlayerItemsFDA8A4[selected + 8].imageNo;
    return audio::Sub_5042D0(panelId);
}

void Sub_4F25C0()
{
    YobiAction* action = gLegacyItemWorkActionD67CEC;
    if (action == nullptr) {
        return;
    }
    const std::int32_t image = GetYobiActionField2A8(action);
    const bool extended = (image >= 100000 && image <= 100999)
        || (image >= 105000 && image <= 106800)
        || (image >= 104400 && image <= 104520)
        || (image >= 120100 && image <= 120147)
        || (image >= 120156 && image <= 120163);
    SetYobiActionField2B0(action, extended ? 16 : 6);
    action = gLegacyItemWorkActionD67CEC;
    if (GetYobiActionField2B0(action) != 1) {
        SetYobiActionField2B4(action, -1);
    }
}

std::int32_t Sub_4F2650()
{
    const std::int32_t gate = gLegacyItemWorkInitGateD67CE8;
    if (gate != 0) {
        return gate;
    }
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    static_cast<void>(ui::detail::Sub_4D3020(x, y));
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    gLegacyItemWorkSelection584394 = -1;
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
    ui::detail::WriteLegacyItemWorkSkillD67CF0(skillId);
    if (skillId == 219 || skillId == 2033) {
        gLegacyItemWorkButtonKindD67CF4 = 1;
    } else if (skillId == 220) {
        gLegacyItemWorkButtonKindD67CF4 = 2;
    } else if (skillId != 218 && skillId != 2032) {
        gLegacyItemWorkButtonKindD67CF4 = 0;
    }
    gLegacyItemWorkPhaseCC4220 = 0;
    return skillId;
}

std::int32_t Sub_4F2720(const std::int32_t cost, const std::int32_t skill)
{
    if (world::detail::IsCostGreaterThanPlayerFp(cost)) {
        gLegacyItemWorkPhaseCC4220 = 3;
        return 1;
    }
    const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
    if (skillId == 219 || skillId == 2033) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8];
        if (item->hasDescription != 0) {
            gLegacyItemWorkPhaseCC4220 = 4;
            return 1;
        }
        if (item->level > world::detail::gSkillsF1BAFC[skill].level) {
            gLegacyItemWorkPhaseCC4220 = 12;
            return 1;
        }
    } else if (skillId == 217 || skillId == 2031 || skillId == 218 || skillId == 2032) {
        const auto* item = &world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8];
        if (item->hasDescription == 0) {
            gLegacyItemWorkPhaseCC4220 = 5;
            return 1;
        }
        const std::int32_t rejectedKind = skillId == 217 || skillId == 2031 ? 2 : 1;
        if (ui::detail::Sub_482F70(item->itemType) == rejectedKind) {
            gLegacyItemWorkPhaseCC4220 = 10;
            return 1;
        }
        item = &world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8];
        if (ui::detail::Sub_482F70(item->itemType) == 0) {
            gLegacyItemWorkPhaseCC4220 = 11;
            return 1;
        }
        item = &world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8];
        if (item->level > world::detail::gSkillsF1BAFC[skill].level) {
            gLegacyItemWorkPhaseCC4220 = 13;
            return 1;
        }
    } else if (skillId == 220) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8];
        const std::uint16_t flags = item->flags;
        if ((flags & 0x200u) != 0 && (flags & 0x400u) == 0) {
            gLegacyItemWorkPhaseCC4220 = 14;
            return 1;
        }
        if ((flags & 0x100u) != 0 && (flags & 0x400u) == 0) {
            gLegacyItemWorkPhaseCC4220 = 6;
            return 1;
        }
        if (item->hasDescription == 0) {
            gLegacyItemWorkPhaseCC4220 = 15;
            return 1;
        }
        if (item->count > 0 || item->itemType == 47) {
            gLegacyItemWorkPhaseCC4220 = 14;
            return 1;
        }
    }
    return 0;
}

std::uintptr_t Sub_4F2A30(const std::int32_t opened)
{
    const std::int32_t x = gLegacyItemWorkPanel5842D0.x;
    const std::int32_t y = gLegacyItemWorkPanel5842D0.y;
    std::array<LegacyPanelButtonRecord, 3> buttons;
    std::array<LegacyPanelButtonRecord, 3> disabled;
    for (std::int32_t i = 0; i < 3; ++i) {
        buttons[i] = {x + 192, y + 408, x + 152, y + 398, 80, 20, 23366 + 3 * i, 23368 + 3 * i};
        disabled[i] = {x + 192, y + 408, x + 152, y + 398, 80, 20, 23367 + 3 * i, 0};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyItemWorkPanel5842D0,
        [] { static_cast<void>(Sub_4F2650()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::ReadLegacyPanelSelectionCC236C() != 19
            && audio::ReadLegacyPanelSelectionCC2370() != 32) {
            if (gLegacySkillPendingD67C48 != 0) {
                gLegacySkillPendingD67C48 = 0;
            } else if (ui::detail::gValue5F9754 != 0) {
                const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
                ui::detail::gValue5F9754 = 0;
            }
        }
        YobiAction* action = gLegacyItemWorkActionD67CEC;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyItemWorkActionD67CEC = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (ui::detail::Sub_4D3040(1, 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyItemWorkPanel5842D0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gLegacyItemWorkActionD67CEC == nullptr) {
        const std::int32_t top = gLegacyItemWorkPanel5842D0.y;
        const std::int32_t left = gLegacyItemWorkPanel5842D0.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        gLegacyItemWorkActionD67CEC = Sub_4FAAA0(image, left + 78, top + 408);
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyItemWorkPanel5842D0.y + 36 + 53 * row;
            const std::int32_t left = gLegacyItemWorkPanel5842D0.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = row * 5 + column;
                break;
            }
        }
    }
    if (hovered != gLegacyItemWorkHover584398) {
        gLegacyItemWorkPageD67CF8 = 0;
    }
    const std::int32_t selected = gLegacyItemWorkSelection584394;
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    std::int32_t cost = world::detail::gPlayerItemsFDA8A4[selected + 8].level * 10;
    if (ui::detail::ReadLegacyItemWorkSkillD67CF0() == 220) {
        cost = 10;
    }
    char buffer[256];
    const std::int32_t phase = gLegacyItemWorkPhaseCC4220;
    if (phase == 7 || phase == 8 || phase == 9) {
        const std::uint32_t now = platform::ReadTickCount32();
        const std::uint32_t start = gLegacyItemWorkTickD2E12C;
        const std::uint32_t duration = gLegacyItemWorkDurationCC3EDC;
        const std::uint32_t elapsed = now - start;
        if (elapsed > duration && elapsed > 4000u) {
            const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
            if (skillId == 217 || skillId == 218 || skillId == 2031
                || skillId == 2032 || skillId == 219 || skillId == 2033) {
                buffer[0] = '\0';
            } else {
                std::strcpy(buffer, gLegacyItemWorkDraftCC31E0.c_str());
            }
            NullSub_509080();
            const std::int32_t item = gLegacyItemWorkSelection584394 + 8;
            const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
            static_cast<void>(network::Sub_511EF0(platform::gConnectionSocket, skill, level, item, buffer));
            const std::int32_t resultItem = gLegacyItemWorkSelection584394;
            gLegacySkillPendingD67C48 = 1;
            std::strncpy(gLegacyItemWorkNameCC40D8.data(),
                world::detail::gPlayerItemsFDA8A4[resultItem + 8].name.data(), 64);
            const std::int32_t imageItem = gLegacyItemWorkSelection584394;
            const std::uint32_t panelId = gLegacyItemWorkPanel5842D0.id;
            gLegacyItemWorkImageCC320C = world::detail::gPlayerItemsFDA8A4[imageItem + 8].imageNo;
            static_cast<void>(audio::Sub_5042D0(panelId));
        }
    } else if ((QueryPanelButtonState(&buttons[gLegacyItemWorkButtonKindD67CF4]) & 1) != 0) {
        if (gLegacyItemWorkSelection584394 < 0) {
            gLegacyItemWorkPhaseCC4220 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else if (Sub_4F2720(cost, skill) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
            if (skillId == 217 || skillId == 218 || skillId == 2031 || skillId == 2032) {
                gLegacyItemWorkPhaseCC4220 = 7;
            } else if (skillId == 219 || skillId == 2033) {
                gLegacyItemWorkPhaseCC4220 = 8;
            }
            if (skillId == 220) {
                if (input::IsChatContext_503E60() == 0) {
                    static_cast<void>(audio::Sub_504380(32));
                }
                static_cast<void>(audio::Sub_504180(32));
            } else {
                gLegacyItemWorkTickD2E12C = platform::ReadTickCount32();
                const std::int32_t random = platform::NextRandom_53B4C9() % 6;
                const std::int32_t level =
                    world::detail::gPlayerItemsFDA8A4[gLegacyItemWorkSelection584394 + 8].level;
                const auto duration = static_cast<std::uint32_t>(1000 * (random + 3 * level + 5));
                const std::uintptr_t socket = platform::gConnectionSocket;
                gLegacyItemWorkDurationCC3EDC = duration;
                static_cast<void>(network::Sub_512DA0(socket, static_cast<std::int32_t>(duration / 1000u)));
                Sub_4F25C0();
            }
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
    } else if (hovered >= 0) {
        const std::uint16_t used = world::detail::gPlayerItemsFDA8A4[hovered + 8].used;
        const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
        if (used != 0 && (clicks & 0x10u) != 0) {
            if (gLegacyItemWorkSelection584394 >= 0 && gLegacyItemWorkSelection584394 == hovered) {
                gLegacyItemWorkSelection584394 = -1;
                gLegacyItemWorkPhaseCC4220 = 0;
                static_cast<void>(audio::PlaySe_52C450(54, 320));
            } else {
                gLegacyItemWorkSelection584394 = hovered;
                gLegacyItemWorkPhaseCC4220 = 2;
                static_cast<void>(audio::PlaySe_52C450(79, 320));
                static_cast<void>(Sub_4F2720(cost, skill));
            }
        } else if (used != 0 && (clicks & 2u) != 0) {
            gLegacyItemWorkPageD67CF8 ^= 1;
        }
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t index = row * 5 + column;
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[index + 8];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyItemWorkPanel5842D0.y;
                const std::int32_t left = gLegacyItemWorkPanel5842D0.x;
                static_cast<void>(DrawGraphic(left + 32 + 52 * column, top + 60 + 53 * row,
                    212, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyItemWorkPanel5842D0.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyItemWorkPanel5842D0.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
            if (hovered == index) {
                const std::uint32_t hoverImage = gValueD69768;
                const std::int32_t left = gLegacyItemWorkPanel5842D0.x + 52 * column;
                const std::int32_t top = gLegacyItemWorkPanel5842D0.y + 53 * row;
                DrawGraphicHover(Low16(left + 8), Low16(top + 36),
                    Low16(left + 56), Low16(top + 84), 212, hoverImage, 0);
            }
            if (gLegacyItemWorkSelection584394 == index) {
                const std::int32_t top = gLegacyItemWorkPanel5842D0.y + 53 * row;
                const std::int32_t left = gLegacyItemWorkPanel5842D0.x + 52 * column;
                DrawGraphicHover(Low16(left + 10), Low16(top + 38),
                    Low16(left + 54), Low16(top + 82), 212, 251, 0);
            }
        }
    }
    const char* message = nullptr;
    switch (gLegacyItemWorkPhaseCC4220) {
    case 0: {
        const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
        if (skillId == 217 || skillId == 218 || skillId == 2031 || skillId == 2032) {
            message = kItemWorkText589F54;
        } else if (skillId == 219 || skillId == 2033) {
            message = kItemWorkText589F6C;
        } else if (skillId == 220) {
            message = kItemWorkText589F84;
        }
        break;
    }
    case 1: message = kItemWorkText589F40; break;
    case 2:
        std::sprintf(buffer, kItemWorkText589F2C, cost);
        message = buffer;
        break;
    case 3: message = kLegacyGatherNoFp5864F0; break;
    case 4: message = kItemWorkText589F1C; break;
    case 5: message = kItemWorkText589F04; break;
    case 6: message = kItemWorkText589EF4; break;
    case 7: message = kItemWorkText589EDC; break;
    case 8: message = kItemWorkText589EC4; break;
    case 9: message = kItemWorkText589EAC; break;
    case 10: message = kItemWorkText589E94; break;
    case 11: message = kItemWorkText589E80; break;
    case 12: message = kItemWorkText589E58; break;
    case 13: message = kItemWorkText589E30; break;
    case 14: message = kItemWorkText589E1C; break;
    case 15: message = kItemWorkText589E04; break;
    default: break;
    }
    if (message != nullptr) {
        gLegacyItemWorkMessageCA0ACC = message;
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        if (input::ExtractLegacyLine_503AB0(
                buffer, 256, gLegacyItemWorkMessageCA0ACC.c_str(), row, 28) != 0) {
            const std::int32_t top = gLegacyItemWorkPanel5842D0.y + 255 + 20 * row;
            const std::int32_t left = gLegacyItemWorkPanel5842D0.x + 9;
            static_cast<void>(DrawText(static_cast<std::int16_t>(left),
                static_cast<std::int16_t>(top), 2, 0, 0, buffer, 0, 0));
        }
    }
    if (gLegacyItemWorkPhaseCC4220 >= 7 && gLegacyItemWorkPhaseCC4220 <= 9) {
        Sub_5035E0(&disabled[gLegacyItemWorkButtonKindD67CF4], 212, 1, gValueD69768, 1);
    } else {
        Sub_5035E0(&buttons[gLegacyItemWorkButtonKindD67CF4], 212, 1, gValueD69768, 0);
    }
    const auto writeTaggedHint = [](const world::detail::PlayerItemRecord* const item, const bool name) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), name ? item->name.data() : item->memo.data());
        std::strcat(ui::detail::gHoverTextD68AE8.data(), name ? "[kok]" : "[han]");
    };
    if (hovered >= 0) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered + 8];
        if (item->used != 0) {
            const std::int32_t page = gLegacyItemWorkPageD67CF8;
            const std::int32_t top = gLegacyItemWorkPanel5842D0.y + 244;
            const std::int32_t left = gLegacyItemWorkPanel5842D0.x;
            Sub_4FB940(left, top, hovered + 8, page);
            // 原4F3492的phase>=7 || phase<=9恒真，所有阶段都保留该标签生产。
            const std::uint16_t flags = item->flags;
            if ((flags & 0x100u) != 0) {
                writeTaggedHint(item, true);
            } else if ((flags & 0x200u) != 0) {
                writeTaggedHint(item, false);
            }
        }
    }
    if (Sub_503590(&gLegacyItemWorkPanel5842D0) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else {
        const std::int32_t buttonState = QueryPanelButtonState(&buttons[gLegacyItemWorkButtonKindD67CF4]);
        const std::int32_t hintPhase = gLegacyItemWorkPhaseCC4220;
        const char* hint = nullptr;
        if ((buttonState & 0x40) != 0 && (hintPhase < 7 || hintPhase > 9)) {
            const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
            if (skillId == 217 || skillId == 218) {
                hint = gLegacyItemWorkSelection584394 >= 0 ? kItemWorkText589DAC : kItemWorkText589D98;
            } else if (skillId == 219 || skillId == 2033) {
                hint = gLegacyItemWorkSelection584394 >= 0 ? kItemWorkText589DD0 : kItemWorkText589DBC;
            } else if (skillId == 220) {
                hint = gLegacyItemWorkSelection584394 >= 0 ? kItemWorkText589DF4 : kItemWorkText589DE0;
            }
        } else if (hovered >= 0) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered + 8];
            if (item->used != 0 && (hintPhase < 7 || hintPhase > 9)) {
                const std::int32_t skillId = ui::detail::ReadLegacyItemWorkSkillD67CF0();
                if (skillId == 220) {
                    const std::uint16_t flags = item->flags;
                    if ((flags & 0x100u) != 0 && item->itemType != 47) {
                        writeTaggedHint(item, true);
                    } else if ((flags & 0x200u) != 0 && item->itemType != 47) {
                        writeTaggedHint(item, false);
                    } else if (item->hasDescription == 0 || item->count > 0) {
                        hint = kItemWorkText589D74;
                    } else {
                        hint = item->memo[0] != '\0' || item->itemType == 47
                            ? kItemWorkText589D74 : kItemWorkText589D88;
                    }
                } else if (skillId == 219 || skillId == 2033) {
                    const std::uint16_t flags = item->flags;
                    const std::uint16_t described = item->hasDescription;
                    if ((flags & 0x100u) != 0) {
                        writeTaggedHint(item, true);
                    } else if ((flags & 0x200u) != 0) {
                        writeTaggedHint(item, false);
                    } else if (described == 0) {
                        hint = item->level > world::detail::gSkillsF1BAFC[skill].level
                            ? kItemWorkText589D54 : kItemWorkText589D44;
                    } else {
                        hint = kItemWorkText589D30;
                    }
                } else if (skillId == 217 || skillId == 2031 || skillId == 218 || skillId == 2032) {
                    const std::int32_t kind = ui::detail::Sub_482F70(item->itemType);
                    const std::uint16_t flags = item->flags;
                    if ((flags & 0x100u) != 0) {
                        writeTaggedHint(item, true);
                    } else if ((flags & 0x200u) != 0) {
                        writeTaggedHint(item, false);
                    } else if (item->hasDescription == 0) {
                        hint = kItemWorkText589D10;
                    } else if (kind == (skillId == 217 || skillId == 2031 ? 2 : 1)) {
                        hint = kItemWorkText589E94;
                    } else {
                        hint = kind == 0 ? kItemWorkText589E80 : kItemWorkText589024;
                    }
                }
            }
        }
        if (hint != nullptr) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyItemWorkPanel5842D0, 1);
    gLegacyItemWorkHover584398 = hovered;
    return result;
}

void Sub_4FB940(
    const std::int32_t x, const std::int32_t y, const std::int32_t index, const std::int32_t page)
{
    if (index < 0 || index >= 28) {
        return;
    }
    const auto* const item = &world::detail::gPlayerItemsFDA8A4[index];
    if (item->used == 0) {
        return;
    }
    Sub_504BA0(x, y, 272, 4, 0, item->name.data(), item->memo.data(),
        0, 0, std::bit_cast<std::int16_t>(item->flags));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const text = item->description[page * 8 + row].data();
        if (std::strlen(text) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, text, 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

void Sub_504BA0(
    const std::int32_t x, const std::int32_t y, const std::int32_t width, const std::uint8_t layer,
    const std::int32_t font, const char* const name, const char* const memo,
    const std::int32_t, const std::int32_t, const std::int16_t flags)
{
    std::int32_t textWidth = TextMeasure(name, font);
    const char* text = name;
    std::int8_t color = 0;
    if ((flags & 0x100) != 0) {
        textWidth = TextMeasure(memo, font);
        text = memo;
        color = 5;
    } else if ((flags & 0x200) != 0) {
        color = 1;
    }
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + width / 2 - textWidth / 2),
        static_cast<std::int16_t>(y + 5), layer, static_cast<std::int8_t>(font), color, text, 0, 0));
}

std::int32_t Sub_4F4470()
{
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
    for (std::int32_t first = 0; first < 20; ++first) {
        if (gLegacyCraftMarksC8AA7C[first] != 0) {
            for (std::int32_t second = 0; second < 20; ++second) {
                if (first != second && gLegacyCraftMarksC8AA7C[second] != 0
                    && world::detail::gPlayerItemsFDA8A4[first + 8].itemId
                        == world::detail::gPlayerItemsFDA8A4[second + 8].itemId) {
                    return 2;
                }
            }
        }
    }
    if (skillId != 215 && skillId != 216) {
        bool foundSpecial = false;
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            if (gLegacyCraftMarksC8AA7C[slot] != 0
                && ui::detail::Sub_4F4430(world::detail::gPlayerItemsFDA8A4[slot + 8].itemType, skillId) != 0) {
                if (foundSpecial) {
                    return 2;
                }
                foundSpecial = true;
            }
        }
    }
    std::int32_t recipe = ui::detail::gRecipeIndex5846BC;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (gLegacyCraftMarksC8AA7C[slot] == 0) {
            continue;
        }
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
        if (skillId != 215 && skillId != 216) {
            const std::int32_t special = ui::detail::Sub_4F4430(item->itemType, skillId);
            recipe = ui::detail::gRecipeIndex5846BC;
            if (special != 0) {
                continue;
            }
        }
        bool found = false;
        for (std::int32_t material = 0; material < 5; ++material) {
            const std::int32_t itemId = world::detail::gSkillsF1BAFC[skill].recipes8E8[recipe].items6C[material].itemId;
            if (itemId < 0) {
                return 2;
            }
            if (item->itemId == itemId) {
                found = true;
                break;
            }
        }
        if (!found) {
            return 2;
        }
    }
    const auto* const materials = world::detail::gSkillsF1BAFC[skill].recipes8E8[recipe].items6C.data();
    for (std::int32_t material = 0; material < 5; ++material) {
        const std::int32_t itemId = materials[material].itemId;
        if (itemId >= 0) {
            bool found = false;
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
                const std::int32_t storedCount = item->count;
                const std::int32_t count = storedCount == 0 ? 1 : storedCount;
                if (gLegacyCraftMarksC8AA7C[slot] != 0 && item->itemId == itemId
                    && count >= materials[material].count) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return 1;
            }
        }
    }
    return 4;
}

void Sub_4F4690()
{
    std::int32_t normalCount = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (gLegacyCraftMarksC8AA7C[slot] != 0) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
            const bool special = ui::detail::Sub_4F4430(item->itemType, 0) != 0;
            const std::int32_t itemId = item->itemId;
            const std::int32_t image = item->imageNo;
            const std::int32_t destination = special ? 5 : normalCount;
            ui::detail::gSelectionImagesCF3FF4[destination] = image;
            const std::int32_t count = ui::detail::Sub_4F4630(itemId);
            ui::detail::gSelectionCountsD66E0C[destination] = count;
            ui::detail::gSelectedSlotsCA0A90[destination] = slot;
            if (!special) {
                ++normalCount;
            }
            if (normalCount >= 6) {
                break;
            }
        }
    }
}

std::int32_t Sub_4F4720()
{
    std::int32_t count = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (gLegacyCraftMarksC8AA7C[slot] != 0) {
            ++count;
        }
    }
    return count;
}

std::int32_t Sub_4F4740()
{
    gLegacyCraftMarksC8AA7C.fill(0);
    return 0;
}

std::int32_t Sub_4F4760()
{
    static_cast<void>(Sub_4F4740());
    gLegacyCraftPhaseD67D00 = 0;
    gLegacyCraftMaterialPageD67D04 = 0;
    gLegacyCraftMaterialHoverD45F84 = -1;
    gLegacyCraftDragSlotD2F330 = -1;
    gLegacyCraftDragImageCC3E7C = 0;
    return 0;
}

std::uint32_t Sub_4F4790(const std::int32_t opened)
{
    const std::int32_t x = gLegacyCraftMaterialsPanel584468.x;
    const std::int32_t y = gLegacyCraftMaterialsPanel584468.y;
    LegacyPanelButtonRecord confirm {x + 140, y + 357, x + 100, y + 347, 80, 20, 23363, 23365};
    LegacyPanelButtonRecord disabled {x + 140, y + 357, x + 100, y + 347, 80, 20, 23364, 0};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyCraftMaterialsPanel584468,
        [] { static_cast<void>(Sub_4F4760()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        return static_cast<std::uint32_t>(audio::Sub_5042D0(26));
    }
    const std::int32_t markedCount = Sub_4F4720();
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyCraftMaterialsPanel584468.y + 36 + 53 * row;
            const std::int32_t left = gLegacyCraftMaterialsPanel584468.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = row * 5 + column;
                break;
            }
        }
    }
    if (hovered != gLegacyCraftMaterialHoverD45F84) {
        gLegacyCraftMaterialPageD67D04 = 0;
    }
    const std::int32_t initialDrag = gLegacyCraftDragSlotD2F330;
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    bool drawDragged = true;
    if (initialDrag >= 0 && (clicks & 2u) != 0) {
        gLegacyCraftDragSlotD2F330 = -1;
        gLegacyCraftDragImageCC3E7C = 0;
        drawDragged = false;
    } else if (hovered >= 0 && (clicks & 2u) != 0
               && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
        const std::int32_t page = gLegacyCraftMaterialPageD67D04;
        const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered + 8].pages;
        gLegacyCraftMaterialPageD67D04 = page + 1;
        if (gLegacyCraftMaterialPageD67D04 >= pages) {
            gLegacyCraftMaterialPageD67D04 = 0;
        }
    } else if (gLegacyCraftPhaseD67D00 < 4) {
        if ((clicks & 0x10u) != 0 && hovered >= 0
            && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
            if (gLegacyCraftMarksC8AA7C[hovered] != 0) {
                gLegacyCraftMarksC8AA7C[hovered] = 0;
                static_cast<void>(audio::PlaySe_52C450(54, 320));
            } else if (markedCount < 6) {
                gLegacyCraftMarksC8AA7C[hovered] = 1;
                static_cast<void>(audio::PlaySe_52C450(79, 320));
            }
            gLegacyCraftDragSlotD2F330 = -1;
            gLegacyCraftDragImageCC3E7C = 0;
            drawDragged = false;
        } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
            const std::int32_t result = Sub_4F4470();
            gLegacyCraftPhaseD67D00 = result;
            if (result == 4) {
                static_cast<void>(Sub_4F5170());
                Sub_4F4690();
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            gLegacyCraftDragSlotD2F330 = -1;
            gLegacyCraftDragImageCC3E7C = 0;
            drawDragged = false;
        } else if (hovered >= 0) {
            const std::int32_t source = gLegacyCraftDragSlotD2F330;
            const std::uint8_t dragClicks = static_cast<std::uint8_t>(gValueD68D9C);
            if (source >= 0 && (dragClicks & 1u) != 0) {
                if (source != hovered) {
                    const std::uintptr_t socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_510EC0(socket, source + 8, hovered + 8, 0));
                    gLegacyCraftMarksC8AA7C[gLegacyCraftDragSlotD2F330] = 0;
                    gLegacyCraftMarksC8AA7C[hovered] = 0;
                }
                gLegacyCraftDragSlotD2F330 = -1;
                gLegacyCraftDragImageCC3E7C = 0;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0
                       && source < 0 && (dragClicks & 1u) != 0) {
                const std::int32_t image = world::detail::gPlayerItemsFDA8A4[hovered + 8].imageNo;
                gLegacyCraftDragSlotD2F330 = hovered;
                gLegacyCraftDragImageCC3E7C = image;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    }
    if (drawDragged && gLegacyCraftDragSlotD2F330 >= 0) {
        const std::int32_t image = gLegacyCraftDragImageCC3E7C;
        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        static_cast<void>(DrawGraphic(mouseX, mouseY, 221, image, 0, &descriptor));
    }
    char buffer[256];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t slot = row * 5 + column;
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyCraftMaterialsPanel584468.y;
                const std::int32_t left = gLegacyCraftMaterialsPanel584468.x;
                static_cast<void>(DrawGraphic(left + 32 + 52 * column, top + 60 + 53 * row,
                    212, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyCraftMaterialsPanel584468.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyCraftMaterialsPanel584468.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
            if (hovered == slot) {
                const std::uint32_t image = gValueD69768;
                const std::int32_t left = gLegacyCraftMaterialsPanel584468.x + 52 * column;
                const std::int32_t top = gLegacyCraftMaterialsPanel584468.y + 53 * row;
                DrawGraphicHover(Low16(left + 8), Low16(top + 36),
                    Low16(left + 56), Low16(top + 84), 212, image, 0);
            }
            std::uint32_t selectedColor = 0;
            if (gLegacyCraftDragSlotD2F330 == slot) {
                selectedColor = 254;
            } else if (gLegacyCraftMarksC8AA7C[slot] != 0) {
                selectedColor = 251;
            }
            if (selectedColor != 0) {
                const std::int32_t left = gLegacyCraftMaterialsPanel584468.x + 52 * column;
                const std::int32_t top = gLegacyCraftMaterialsPanel584468.y + 53 * row;
                DrawGraphicHover(Low16(left + 10), Low16(top + 38),
                    Low16(left + 54), Low16(top + 82), 212, selectedColor, 0);
            }
        }
    }
    if (hovered >= 0 && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
        const std::int32_t page = gLegacyCraftMaterialPageD67D04;
        const std::int32_t height = gLegacyCraftMaterialsPanel584468.height;
        const std::int32_t top = gLegacyCraftMaterialsPanel584468.y;
        const std::int32_t left = gLegacyCraftMaterialsPanel584468.x;
        Sub_4FB940(left, height / 2 + top + 54, hovered + 8, page);
    }
    if (gLegacyCraftPhaseD67D00 >= 4) {
        Sub_5035E0(&disabled, 212, 1, gValueD69768, 1);
    } else {
        Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    }
    std::int32_t phase = gLegacyCraftPhaseD67D00;
    if (phase != 0) {
        const char* message = nullptr;
        switch (phase) {
        case 1: message = kLegacyMaterialText58A078; break;
        case 2: message = kLegacyMaterialText58A064; break;
        case 3: message = kLegacyMaterialText58A054; break;
        case 4: message = kLegacyMaterialText58A044; break;
        case 5: case 6: message = kLegacyMaterialText58A034; break;
        case 7: message = kLegacyGatherNoFp5864F0; break;
        default: break;
        }
        // 原default保留局部缓冲；已证正常阶段生产为0..7，不新增补零或文案。
        if (message != nullptr) {
            std::strcpy(buffer, message);
        }
        const std::int32_t width = TextMeasure(buffer, 0);
        const std::int32_t top = gLegacyCraftMaterialsPanel584468.y + 278;
        const std::int32_t panelWidth = gLegacyCraftMaterialsPanel584468.width;
        const std::int32_t left = gLegacyCraftMaterialsPanel584468.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + panelWidth / 2 - width / 2),
            static_cast<std::int16_t>(top), 2, 0, 0, buffer, 0, 0));
        phase = gLegacyCraftPhaseD67D00;
    }
    const auto writeTaggedHint = [](const world::detail::PlayerItemRecord* const item) {
        const std::uint16_t flags = item->flags;
        if ((flags & 0x100u) != 0) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(), item->name.data());
            std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            return true;
        }
        if ((flags & 0x200u) != 0) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(), item->memo.data());
            std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            return true;
        }
        return false;
    };
    if (hovered >= 0 && phase == 4) {
        static_cast<void>(writeTaggedHint(&world::detail::gPlayerItemsFDA8A4[hovered + 8]));
    }
    if (Sub_503590(&gLegacyCraftMaterialsPanel584468) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else {
        const std::int32_t buttonState = QueryPanelButtonState(&confirm);
        const std::int32_t hintPhase = gLegacyCraftPhaseD67D00;
        if ((buttonState & 0x40) != 0 && hintPhase < 4) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(),
                markedCount > 0 ? kLegacyMaterialText58A020 : kLegacyMaterialText58A010);
        } else if (hovered >= 0) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered + 8];
            if (item->used != 0 && hintPhase < 4 && !writeTaggedHint(item)) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(),
                    gLegacyCraftMarksC8AA7C[hovered] != 0 ? kLegacyMaterialText589FFC : kLegacyMaterialText589FEC);
            }
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyCraftMaterialsPanel584468, 1);
    gLegacyCraftMaterialHoverD45F84 = hovered;
    return result;
}

std::int32_t Sub_4F50D0()
{
    YobiAction* action = gLegacyCraftExecutionActionD67D08;
    if (action != nullptr) {
        const std::int32_t image = GetYobiActionField2A8(action);
        const bool extended = (image >= 100000 && image <= 100999)
            || (image >= 105000 && image <= 106800)
            || (image >= 104400 && image <= 104520)
            || (image >= 120100 && image <= 120147)
            || (image >= 120156 && image <= 120163);
        SetYobiActionField2B0(action, extended ? 16 : 6);
        action = gLegacyCraftExecutionActionD67D08;
        if (GetYobiActionField2B0(action) != 1) {
            SetYobiActionField2B4(action, -1);
            action = gLegacyCraftExecutionActionD67D08;
        }
    }
    return Sub_517880(action, 100, 0, 0);
}

std::int32_t Sub_4F5170()
{
    ui::detail::gSelectedSlotsCA0A90.fill(-1);
    ui::detail::gSelectionCountsD66E0C.fill(-1);
    ui::detail::gSelectionImagesCF3FF4.fill(-1);
    return -1;
}

std::int32_t Sub_4F51A0()
{
    static_cast<void>(Sub_4F5170());
    gLegacyCraftPhaseD67D00 = 0;
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    return ui::detail::Sub_4D3020(x, y);
}

std::uint32_t Sub_4F51D0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyCraftExecutionPanel584530.x;
    const std::int32_t y = gLegacyCraftExecutionPanel584530.y;
    LegacyPanelButtonRecord execute {x + 97, y + 357, x + 57, y + 347, 80, 20, 23360, 23362};
    LegacyPanelButtonRecord disabled {x + 97, y + 357, x + 57, y + 347, 80, 20, 23361, 0};
    LegacyPanelButtonRecord back {x + 257, y + 357, x + 217, y + 347, 80, 20, 22710, 22711};
    constexpr std::array<std::array<std::int32_t, 2>, 6> positions {{
        {177, 214}, {229, 214}, {281, 214}, {177, 267}, {229, 267}, {281, 267},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyCraftExecutionPanel584530,
        [] { static_cast<void>(Sub_4F51A0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::ReadLegacyPanelSelectionCC236C() != 25) {
            if (gLegacySkillPendingD67C48 != 0) {
                gLegacySkillPendingD67C48 = 0;
            } else if (ui::detail::gValue5F9754 != 0) {
                const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
                ui::detail::gValue5F9754 = 0;
            }
        }
        if (gLegacyCraftExecutionActionD67D08 != nullptr) {
            static_cast<void>(MarkDeath(gLegacyCraftExecutionActionD67D08));
            gLegacyCraftExecutionActionD67D08 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::Sub_5042D0(27));
    }
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    if (ui::detail::Sub_4D3040(1, 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyCraftExecutionPanel584530.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gLegacyCraftExecutionActionD67D08 == nullptr) {
        const std::int32_t top = gLegacyCraftExecutionPanel584530.y;
        const std::int32_t left = gLegacyCraftExecutionPanel584530.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        gLegacyCraftExecutionActionD67D08 = Sub_4FAAA0(image, left + 94, top + 290);
    }
    char buffer[256];
    char part[128];
    if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyCraftExecutionPanel584530.id));
        static_cast<void>(audio::Sub_504180(25));
    } else {
        const std::int32_t phase = gLegacyCraftPhaseD67D00;
        if (phase != 7 && phase != 6) {
            if (phase == 5) {
                const std::uint32_t now = platform::ReadTickCount32();
                const std::uint32_t elapsed = now - gLegacyCraftExecutionTickD2F78C;
                NullSub_509080();
                if (gLegacyCraftExecutionDurationD2DC08 < elapsed) {
                    const std::int32_t recipe = ui::detail::gRecipeIndex5846BC;
                    std::sprintf(buffer, "%d", world::detail::gSkillsF1BAFC[skill].recipes8E8[recipe].itemId);
                    for (std::int32_t slot = 0; slot < 6; ++slot) {
                        const std::int32_t selected = ui::detail::gSelectedSlotsCA0A90[slot];
                        if (selected >= 0) {
                            std::sprintf(part, "|%d", selected + 8);
                            std::strcat(buffer, part);
                        }
                    }
                    NullSub_509080();
                    const std::int32_t nameRecipe = ui::detail::gRecipeIndex5846BC;
                    ui::detail::WriteLegacyCraftNameCF46D4(
                        world::detail::gSkillsF1BAFC[skill].recipes8E8[nameRecipe].name.data());
                    const std::uintptr_t socket = platform::gConnectionSocket;
                    const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
                    static_cast<void>(network::Sub_511EF0(socket, skill, level, -1, buffer));
                    gLegacySkillPendingD67C48 = 1;
                    gLegacyCraftPhaseD67D00 = 6;
                }
            } else if ((QueryPanelButtonState(&execute) & 1) != 0) {
                if (ui::detail::gSelectionImagesCF3FF4[0] <= 0) {
                    gLegacyCraftPhaseD67D00 = 3;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    const std::int32_t recipe = ui::detail::gRecipeIndex5846BC;
                    const std::int32_t fp = world::detail::gSkillsF1BAFC[skill].recipes8E8[recipe].value04;
                    const std::int32_t cost = fp * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
                    if (world::detail::IsPlayerFpGreaterEqual(cost)) {
                        gLegacyCraftPhaseD67D00 = 5;
                        gLegacyCraftExecutionTickD2F78C = platform::ReadTickCount32();
                        const std::int32_t random = platform::NextRandom_53B4C9() % 6;
                        const std::int32_t levelRecipe = ui::detail::gRecipeIndex5846BC;
                        const std::int32_t level =
                            world::detail::gSkillsF1BAFC[skill].recipes8E8[levelRecipe].level;
                        const auto duration = static_cast<std::uint32_t>(1000 * (random + 3 * level + 5));
                        gLegacyCraftExecutionDurationD2DC08 = duration;
                        const std::uintptr_t socket = platform::gConnectionSocket;
                        static_cast<void>(network::Sub_512DA0(socket, static_cast<std::int32_t>(duration / 1000u)));
                        static_cast<void>(Sub_4F50D0());
                        static_cast<void>(audio::PlaySe_52C450(78, 320));
                    } else {
                        gLegacyCraftPhaseD67D00 = 7;
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    }
                }
            }
        }
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyCraftExecutionPanel584530.y;
        const std::int32_t left = gLegacyCraftExecutionPanel584530.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, 0, text, 0, 0));
    };
    const std::int32_t measuredRecipe = ui::detail::gRecipeIndex5846BC;
    const std::int32_t width =
        TextMeasure(world::detail::gSkillsF1BAFC[skill].recipes8E8[measuredRecipe].name.data(), 0);
    const std::int32_t nameRecipe = ui::detail::gRecipeIndex5846BC;
    drawAt(world::detail::gSkillsF1BAFC[skill].recipes8E8[nameRecipe].name.data(), 137 - width / 2, 37, 0);
    const std::int32_t costRecipe = ui::detail::gRecipeIndex5846BC;
    const std::int32_t fp = world::detail::gSkillsF1BAFC[skill].recipes8E8[costRecipe].value04;
    const std::int32_t cost = fp * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
    std::sprintf(buffer, "%4d", cost);
    drawAt(buffer, 295, 39, 1);
    std::int32_t materialRecipe = ui::detail::gRecipeIndex5846BC;
    for (std::int32_t material = 4; material >= 0; --material) {
        const auto* const item = &world::detail::gSkillsF1BAFC[skill].recipes8E8[materialRecipe].items6C[material];
        if (item->itemId >= 0) {
            drawAt(item->name.data(), 11, 65 + 24 * material, 0);
            materialRecipe = ui::detail::gRecipeIndex5846BC;
            const std::int32_t count =
                world::detail::gSkillsF1BAFC[skill].recipes8E8[materialRecipe].items6C[material].count;
            if (count > 0) {
                std::sprintf(buffer, "%3d", count);
                drawAt(buffer, 281, 67 + 24 * material, 1);
                materialRecipe = ui::detail::gRecipeIndex5846BC;
            }
        }
    }
    for (std::int32_t slot = 0; slot < 6; ++slot) {
        const std::int32_t image = ui::detail::gSelectionImagesCF3FF4[slot];
        if (image > 0) {
            const std::int32_t top = gLegacyCraftExecutionPanel584530.y;
            const std::int32_t left = gLegacyCraftExecutionPanel584530.x;
            static_cast<void>(DrawGraphic(left + positions[slot][0] + 24,
                top + positions[slot][1] + 24, 213, image, 0, &descriptor));
            const std::int32_t count = ui::detail::gSelectionCountsD66E0C[slot];
            if (count > 0) {
                std::sprintf(buffer, "%3d", count);
                drawAt(buffer, positions[slot][0] + 21, positions[slot][1] + 31, 1);
            }
        }
    }
    if (gLegacyCraftPhaseD67D00 >= 5) {
        Sub_5035E0(&disabled, 212, 1, gValueD69768, 1);
    } else {
        Sub_5035E0(&execute, 212, 1, gValueD69768, 0);
    }
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    if (Sub_503590(&gLegacyCraftExecutionPanel584530) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&execute) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(),
            ui::detail::gSelectionImagesCF3FF4[0] > 0 ? kLegacyCraftExecute58A0A8 : kLegacyCraftMissing58A098);
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyCraftBack58A084);
    }
    return Sub_503440(&gLegacyCraftExecutionPanel584530, 1);
}

std::int32_t Sub_504EE0()
{
    gLegacyGemDragSlotD68AA8 = -1;
    ui::detail::gItemMarksD68A90.fill(0);
    gLegacyGemPhaseD68A8C = 0;
    gLegacyGemDragImageD68AA4 = 0;
    return 0;
}

std::int32_t Sub_504FA0()
{
    bool foundNormal = false;
    bool foundSpecial = false;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (ui::detail::gItemMarksD68A90[slot] == 0) {
            continue;
        }
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
        const std::int32_t type = item->itemType;
        if ((type == 38 || type == 25 || type == 39) && item->hasDescription != 0) {
            if (foundSpecial) {
                return 2;
            }
            foundSpecial = true;
        } else {
            if ((world::detail::Sub_504F10(item->itemId, 0) == 0
                    && world::detail::Sub_504F10(item->itemId, 1) == 0)
                || foundNormal || (item->flags & 0x800u) == 0) {
                return 2;
            }
            foundNormal = true;
        }
    }
    return foundNormal && foundSpecial ? 4 : 2;
}

std::uint32_t Sub_505170(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGemMaterialsPanel58AA60.x;
    const std::int32_t y = gLegacyGemMaterialsPanel58AA60.y;
    LegacyPanelButtonRecord confirm {x + 140, y + 357, x + 100, y + 347, 80, 20, 23363, 23365};
    LegacyPanelButtonRecord disabled {x + 140, y + 357, x + 100, y + 347, 80, 20, 23364, 0};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGemMaterialsPanel58AA60,
        [] { static_cast<void>(Sub_504EE0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        return static_cast<std::uint32_t>(audio::Sub_5042D0(29));
    }
    std::int32_t markedCount = 0;
    for (const std::uint8_t mark : ui::detail::gItemMarksD68A90) {
        markedCount += std::bit_cast<std::int8_t>(mark);
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y + 36 + 53 * row;
            const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = row * 5 + column;
                break;
            }
        }
    }
    if (hovered != gLegacyGemMaterialHoverD68A84) {
        gLegacyGemMaterialPageD68AAC = 0;
    }
    const std::int32_t initialDrag = gLegacyGemDragSlotD68AA8;
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    bool skipDrag = false;
    if (initialDrag >= 0 && (clicks & 2u) != 0) {
        gLegacyGemDragSlotD68AA8 = -1;
        gLegacyGemDragImageD68AA4 = 0;
        skipDrag = true;
    } else if (hovered >= 0 && (clicks & 2u) != 0
               && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
        const std::int32_t page = gLegacyGemMaterialPageD68AAC;
        const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered + 8].pages;
        gLegacyGemMaterialPageD68AAC = page + 1;
        if (gLegacyGemMaterialPageD68AAC >= pages) {
            gLegacyGemMaterialPageD68AAC = 0;
        }
    } else if (gLegacyGemPhaseD68A8C < 4) {
        if ((clicks & 0x10u) != 0 && hovered >= 0
            && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
            if (world::detail::Sub_505040(hovered + 8) != 0
                && world::detail::Sub_5050B0(hovered + 8) != 0) {
                if (ui::detail::gItemMarksD68A90[hovered] != 0) {
                    ui::detail::gItemMarksD68A90[hovered] = 0;
                    static_cast<void>(audio::PlaySe_52C450(54, 320));
                } else if (markedCount < 2) {
                    ui::detail::gItemMarksD68A90[hovered] = 1;
                    static_cast<void>(audio::PlaySe_52C450(79, 320));
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            gLegacyGemDragSlotD68AA8 = -1;
            gLegacyGemDragImageD68AA4 = 0;
            skipDrag = true;
        } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
            if (markedCount > 2) {
                gLegacyGemPhaseD68A8C = 2;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (markedCount < 2) {
                gLegacyGemPhaseD68A8C = 1;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                gLegacyGemPhaseD68A8C = Sub_504FA0();
            }
            if (gLegacyGemPhaseD68A8C == 4) {
                std::int32_t normalCount = 0;
                for (std::int32_t slot = 0; slot < 20; ++slot) {
                    if (ui::detail::gItemMarksD68A90[slot] != 0) {
                        const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
                        const std::int32_t type = item->itemType;
                        const bool special = type == 38 || type == 25 || type == 39;
                        const std::int32_t image = item->imageNo;
                        const std::int32_t target = special ? 5 : normalCount;
                        ui::detail::gSelectionCountsD66E0C[target] = 1;
                        ui::detail::gSelectionImagesCF3FF4[target] = image;
                        ui::detail::gSelectedSlotsCA0A90[target] = slot;
                        if (!special) {
                            ++normalCount;
                        }
                        if (normalCount >= 6) {
                            break;
                        }
                    }
                }
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        } else if (hovered >= 0) {
            const std::int32_t source = gLegacyGemDragSlotD68AA8;
            const std::uint8_t dragClicks = static_cast<std::uint8_t>(gValueD68D9C);
            if (source >= 0 && (dragClicks & 1u) != 0) {
                if (source != hovered) {
                    static_cast<void>(network::Sub_510EC0(platform::gConnectionSocket, source + 8, hovered + 8, 0));
                    const std::int32_t currentSource = gLegacyGemDragSlotD68AA8;
                    ui::detail::gItemMarksD68A90[currentSource] = 0;
                    ui::detail::gItemMarksD68A90[hovered] = 0;
                }
                gLegacyGemDragSlotD68AA8 = -1;
                gLegacyGemDragImageD68AA4 = 0;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0
                       && source < 0 && (dragClicks & 1u) != 0) {
                const std::int32_t image = world::detail::gPlayerItemsFDA8A4[hovered + 8].imageNo;
                gLegacyGemDragSlotD68AA8 = hovered;
                gLegacyGemDragImageD68AA4 = image;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    }
    if (!skipDrag && gLegacyGemDragSlotD68AA8 >= 0) {
        const std::int32_t image = gLegacyGemDragImageD68AA4;
        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        static_cast<void>(DrawGraphic(mouseX, mouseY, 221, image, 0, nullptr));
    }
    char buffer[256];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t slot = row * 5 + column;
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot + 8];
            if (item->used != 0) {
                const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y + 60 + 53 * row;
                const std::int32_t image = item->imageNo;
                const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x + 32 + 52 * column;
                static_cast<void>(DrawGraphic(left, top, 212, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyGemMaterialsPanel58AA60.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyGemMaterialsPanel58AA60.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
                if (world::detail::Sub_505040(slot + 8) == 0 || world::detail::Sub_5050B0(slot + 8) == 0) {
                    const std::int32_t coverY = gLegacyGemMaterialsPanel58AA60.y + 153 + 53 * row;
                    const std::int32_t coverX = gLegacyGemMaterialsPanel58AA60.x + 137 + 52 * column;
                    static_cast<void>(DrawGraphic(coverX, coverY, 213, 22739, 0, nullptr));
                }
            }
            if (hovered == slot) {
                const std::uint32_t hoverImage = gValueD69768;
                const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y + 53 * row;
                const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x + 52 * column;
                DrawGraphicHover(Low16(left + 8), Low16(top + 36), Low16(left + 56), Low16(top + 84),
                    212, hoverImage, 0);
            }
            const bool dragged = gLegacyGemDragSlotD68AA8 == slot;
            if (dragged || ui::detail::gItemMarksD68A90[slot] != 0) {
                const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x + 52 * column;
                const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y + 53 * row;
                DrawGraphicHover(Low16(left + 10), Low16(top + 38), Low16(left + 54), Low16(top + 82),
                    212, dragged ? 254 : 251, 0);
            }
        }
    }
    if (hovered >= 0 && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
        const std::int32_t page = gLegacyGemMaterialPageD68AAC;
        const std::int32_t height = gLegacyGemMaterialsPanel58AA60.height;
        const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y;
        const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x;
        Sub_4FB940(left, top + height / 2 + 54, hovered + 8, page);
    }
    if (gLegacyGemPhaseD68A8C >= 4) {
        Sub_5035E0(&disabled, 212, 1, gValueD69768, 1);
    } else {
        Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    }
    const std::int32_t phase = gLegacyGemPhaseD68A8C;
    if (phase != 0) {
        switch (phase) {
        case 1: std::strcpy(buffer, kLegacyGemTooFew58ACA4); break;
        case 2: std::strcpy(buffer, kLegacyGemInvalid58AC90); break;
        case 3: std::strcpy(buffer, kLegacyGemMissing58AC7C); break;
        case 4: std::strcpy(buffer, kLegacyGemRegistered58AC6C); break;
        case 5: std::strcpy(buffer, kLegacyGatherNoFp5864F0); break;
        case 6: case 7: std::strcpy(buffer, kLegacyGemWorking58AC58); break;
        default: break;
        }
        const std::int32_t textWidth = TextMeasure(buffer, 0);
        const std::int32_t top = gLegacyGemMaterialsPanel58AA60.y + 278;
        const std::int32_t width = gLegacyGemMaterialsPanel58AA60.width;
        const std::int32_t left = gLegacyGemMaterialsPanel58AA60.x + width / 2 - textWidth / 2;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, buffer, 0, 0));
    }
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if (Sub_503590(&gLegacyGemMaterialsPanel58AA60) != 0) {
        std::strcpy(hint, kLegacySkillClose55331C);
    }
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0 && gLegacyGemPhaseD68A8C <= 4) {
        std::strcpy(hint, markedCount > 0 ? kLegacyGemRegister58AC3C : kLegacyGemNone58AC28);
    } else if (hovered >= 0) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered + 8];
        if (item->used != 0) {
            const std::uint16_t flags = item->flags;
            if ((flags & 0x100u) != 0) {
                std::strcpy(hint, item->name.data());
                std::strcat(hint, "[kok]");
            } else if ((flags & 0x200u) != 0) {
                std::strcpy(hint, item->memo.data());
                std::strcat(hint, "[han]");
            } else if (ui::detail::gItemMarksD68A90[hovered] != 0) {
                std::strcpy(hint, kLegacyGemSelected58AC10);
            } else {
                const bool selectable = world::detail::Sub_505040(hovered + 8) != 0
                    || world::detail::Sub_5050B0(hovered + 8) != 0;
                std::strcpy(hint, selectable ? kLegacyGemSelect58AC00 : kLegacyGemReject58ABEC);
            }
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyGemMaterialsPanel58AA60, 1);
    gLegacyGemMaterialHoverD68A84 = hovered;
    return result;
}

std::int32_t Sub_505BC0()
{
    const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    static_cast<void>(ui::detail::Sub_4D3020(playerX, playerY));
    YobiAction* action = gLegacyGemActionD68A88;
    if (action == nullptr) {
        const std::int32_t top = gLegacyGemExecutionPanel58AB28.y;
        const std::int32_t left = gLegacyGemExecutionPanel58AB28.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        action = Sub_4FAAA0(image, left + 94, top + 290);
        gLegacyGemActionD68A88 = action;
    }
    SetYobiActionFlag2(action, true);
    return Sub_4F5170();
}

std::int32_t Sub_5062A0()
{
    YobiAction* action = gLegacyGemActionD68A88;
    if (action != nullptr) {
        const std::int32_t image = GetYobiActionField2A8(action);
        const bool extended = (image >= 100000 && image <= 100999)
            || (image >= 105000 && image <= 106800)
            || (image >= 104400 && image <= 104520)
            || (image >= 120100 && image <= 120147)
            || (image >= 120156 && image <= 120163);
        SetYobiActionField2B0(action, extended ? 16 : 6);
        action = gLegacyGemActionD68A88;
        if (GetYobiActionField2B0(action) != 1) {
            SetYobiActionField2B4(action, -1);
            action = gLegacyGemActionD68A88;
        }
    }
    return Sub_517880(action, 100, 0, 0);
}

std::uint32_t Sub_505C20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGemExecutionPanel58AB28.x;
    const std::int32_t y = gLegacyGemExecutionPanel58AB28.y;
    LegacyPanelButtonRecord execute {x + 97, y + 357, x + 57, y + 347, 80, 20, 23360, 23362};
    LegacyPanelButtonRecord disabled {x + 97, y + 357, x + 57, y + 347, 80, 20, 23361, 0};
    LegacyPanelButtonRecord back {x + 257, y + 357, x + 217, y + 347, 80, 20, 22710, 22711};
    constexpr std::array<std::array<std::int32_t, 2>, 6> positions {{
        {177, 214}, {229, 214}, {281, 214}, {177, 267}, {229, 267}, {281, 267},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::int32_t state = Sub_503160(opened, &gLegacyGemExecutionPanel58AB28,
        [] { static_cast<void>(Sub_505BC0()); }, 1);
    if (state == 0) {
        return 0;
    }
    YobiAction* const action = gLegacyGemActionD68A88;
    if (state == 2) {
        if (action != nullptr) {
            static_cast<void>(MarkDeath(action));
            gLegacyGemActionD68A88 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::Sub_5042D0(30));
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
    }
    if (ui::detail::Sub_4D3040(1, 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyGemExecutionPanel58AB28.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char part[128];
    char buffer[256];
    if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyGemExecutionPanel58AB28.id));
        static_cast<void>(audio::Sub_504180(19));
    } else {
        const std::int32_t phase = gLegacyGemPhaseD68A8C;
        if (phase != 5 && phase != 7) {
            if (phase == 6) {
                const std::uint32_t now = platform::ReadTickCount32();
                const std::uint32_t tick = gLegacyGemTickD68AB4;
                const std::int32_t duration = gLegacyGemDurationD68AB8;
                const std::int32_t elapsed = std::bit_cast<std::int32_t>(now - tick);
                gLegacyGemElapsedD68ABC = elapsed;
                if (duration < elapsed) {
                    const std::int32_t first = ui::detail::gSelectedSlotsCA0A90[0];
                    if (first >= 0) {
                        std::sprintf(part, "%d|", first + 8);
                        std::strcpy(buffer, part);
                    }
                    const std::int32_t last = ui::detail::gSelectedSlotsCA0A90[5];
                    if (last >= 0) {
                        std::sprintf(part, "%d", last + 8);
                        std::strcat(buffer, part);
                    }
                    NullSub_509080();
                    const std::int32_t source = ui::detail::gSelectedSlotsCA0A90[0];
                    ui::detail::WriteLegacyCraftNameCF46D4(world::detail::gPlayerItemsFDA8A4[source + 8].name.data());
                    const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
                    const std::uintptr_t socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_511EF0(socket, skill, level, -1, buffer));
                    gLegacyGemPhaseD68A8C = 7;
                }
            } else if ((QueryPanelButtonState(&execute) & 1) != 0) {
                if (ui::detail::gSelectionImagesCF3FF4[0] <= 0) {
                    gLegacyGemPhaseD68A8C = 3;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    const std::int32_t reduce = world::detail::gSkillsF1BAFC[skill].fpReduce;
                    const std::int32_t fp = world::detail::gSkillsF1BAFC[skill].techniques3C[0].value7C;
                    if (world::detail::IsPlayerFpGreaterEqual(fp * reduce / 100)) {
                        gLegacyGemPhaseD68A8C = 6;
                        gLegacyGemTickD68AB4 = platform::ReadTickCount32();
                        gLegacyGemDurationD68AB8 = 8000;
                        static_cast<void>(Sub_5062A0());
                        static_cast<void>(audio::PlaySe_52C450(78, 320));
                    } else {
                        gLegacyGemPhaseD68A8C = 5;
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    }
                }
            }
        }
    }
    const char* const name = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t width = TextMeasure(name, 0);
    const std::int32_t nameY = gLegacyGemExecutionPanel58AB28.y + 37;
    const std::int32_t nameX = gLegacyGemExecutionPanel58AB28.x + 137 - width / 2;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
        static_cast<std::int16_t>(nameY), 2, 0, 0, name, 0, 0));
    const std::int32_t reduce = world::detail::gSkillsF1BAFC[skill].fpReduce;
    const std::int32_t fp = world::detail::gSkillsF1BAFC[skill].techniques3C[0].value7C;
    std::sprintf(buffer, "%4d", fp * reduce / 100);
    const std::int32_t costY = gLegacyGemExecutionPanel58AB28.y + 39;
    const std::int32_t costX = gLegacyGemExecutionPanel58AB28.x + 295;
    static_cast<void>(DrawText(static_cast<std::int16_t>(costX),
        static_cast<std::int16_t>(costY), 2, 1, 0, buffer, 0, 0));
    if (ui::detail::gSelectionImagesCF3FF4[0] > 0) {
        for (std::int32_t slot = 0; slot < 6; ++slot) {
            const std::int32_t image = ui::detail::gSelectionImagesCF3FF4[slot];
            if (image > 0) {
                const std::int32_t top = gLegacyGemExecutionPanel58AB28.y + positions[slot][1] + 24;
                const std::int32_t left = gLegacyGemExecutionPanel58AB28.x + positions[slot][0] + 24;
                static_cast<void>(DrawGraphic(left, top, 213, image, 0, &descriptor));
                const std::int32_t count = ui::detail::gSelectionCountsD66E0C[slot];
                if (count > 0) {
                    std::sprintf(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyGemExecutionPanel58AB28.y + positions[slot][1] + 31;
                    const std::int32_t countX = gLegacyGemExecutionPanel58AB28.x + positions[slot][0] + 21;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    if (gLegacyGemPhaseD68A8C > 4) {
        Sub_5035E0(&disabled, 212, 1, gValueD69768, 1);
    } else {
        Sub_5035E0(&execute, 212, 1, gValueD69768, 0);
    }
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    if (Sub_503590(&gLegacyGemExecutionPanel58AB28) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    } else if ((QueryPanelButtonState(&execute) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(),
            ui::detail::gSelectionImagesCF3FF4[0] > 0 ? kLegacyGemExecute58ACB0 : kLegacyGemMissing58AC7C);
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyGemBack5871F0);
    }
    return Sub_503440(&gLegacyGemExecutionPanel58AB28, 1);
}

std::int32_t Sub_519640(const std::int32_t skill)
{
    std::int32_t count = 0;
    while (count < 56 && world::detail::gSkillsF1BAFC[skill].recipes8E8[count].itemId >= 0) {
        ++count;
    }
    return count == 0 ? 1 : (count - 1) / 8 + 1;
}

std::uint32_t Sub_4F5A60(const std::int32_t opened)
{
    const std::int32_t x = gLegacyCraftRecipePanel5845F8.x;
    const std::int32_t y = gLegacyCraftRecipePanel5845F8.y;
    LegacyPanelButtonRecord previous {x + 61, y + 357, x + 47, y + 345, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 153, y + 357, x + 139, y + 345, 28, 24, 23023, 23027};
    LegacyPanelButtonRecord back {x + 257, y + 357, x + 217, y + 347, 80, 20, 22710, 22711};
    const std::int32_t state = Sub_503160(opened, &gLegacyCraftRecipePanel5845F8, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::uint32_t selected = audio::ReadLegacyPanelSelectionCC236C();
        if (selected == 19 || selected == 26) {
            return selected;
        }
        const std::uint8_t notify = ui::detail::gValue5F9754;
        if (notify == 0) {
            return (selected & 0xFFFFFF00u) | notify;
        }
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const std::int32_t result = network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62);
        ui::detail::gValue5F9754 = 0;
        return static_cast<std::uint32_t>(result);
    }
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    gLegacyRecipePageCountD67D10 = Sub_519640(skill);
    std::int32_t hoveredRow = -1;
    std::int32_t hoveredRecipe = -1;
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t top = gLegacyCraftRecipePanel5845F8.y;
        const std::int32_t left = gLegacyCraftRecipePanel5845F8.x;
        if (Sub_5049A0(left + 11, top + 89 + 24 * row, left + 333, top + 109 + 24 * row, -1) != 0) {
            hoveredRow = row;
            hoveredRecipe = row + 8 * gLegacyRecipePageD67D0C;
            const auto* const recipe = &world::detail::gSkillsF1BAFC[skill].recipes8E8[hoveredRecipe];
            if (recipe->itemId >= 0 && (gValueD68D9C & 1u) != 0) {
                if (recipe->value0C == 1) {
                    ui::detail::gRecipeIndex5846BC = hoveredRecipe;
                    static_cast<void>(audio::Sub_504180(26));
                    static_cast<void>(audio::Sub_504180(27));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
            break;
        }
    }
    if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        --gLegacyRecipePageD67D0C;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyRecipePageD67D0C < 0) {
            gLegacyRecipePageD67D0C = gLegacyRecipePageCountD67D10 - 1;
        }
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        ++gLegacyRecipePageD67D0C;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyRecipePageD67D0C >= gLegacyRecipePageCountD67D10) {
            gLegacyRecipePageD67D0C = 0;
        }
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyCraftRecipePanel5845F8.id));
        static_cast<void>(audio::Sub_504180(19));
    }
    char buffer[256];
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font, const std::int8_t color) {
        const std::int32_t top = gLegacyCraftRecipePanel5845F8.y;
        const std::int32_t left = gLegacyCraftRecipePanel5845F8.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left + offsetX),
            static_cast<std::int16_t>(top + offsetY), 2, font, color, text, 0, 0));
    };
    const char* const name = world::detail::gSkillsF1BAFC[skill].name.data();
    const std::int32_t width = TextMeasure(name, 0);
    drawAt(name, 85 - width / 2, 35, 0, 0);
    std::sprintf(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 66, 60, 1, 0);
    // 原4F5DD9复制16B CryptInt作为可变实参，%9d实际仅消费首W32，不解密FP。
    const platform::LegacyCryptInt rawFp = world::detail::ReadPlayerFpRawSnapshot();
    std::sprintf(buffer, "%9d", std::bit_cast<std::int32_t>(rawFp.legacyVftToken));
    drawAt(buffer, 258, 60, 1, 0);
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t recipeIndex = row + 8 * gLegacyRecipePageD67D0C;
        const auto* const recipe = &world::detail::gSkillsF1BAFC[skill].recipes8E8[recipeIndex];
        if (recipe->itemId >= 0) {
            const std::int8_t color = recipe->value0C == 0 ? 7 : 0;
            drawAt(recipe->name.data(), 11, 90 + 24 * row, 0, color);
            const std::int32_t costRecipe = row + 8 * gLegacyRecipePageD67D0C;
            const std::int32_t multiplierSkill = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
            const std::int32_t fp = world::detail::gSkillsF1BAFC[skill].recipes8E8[costRecipe].value04;
            // 原4F5EB2直接以配对槽号取倍率，不再次通过pair.index映射。
            const std::int32_t cost = fp * world::detail::gSkillsF1BAFC[multiplierSkill].fpReduce / 100;
            std::sprintf(buffer, "%4d", cost);
            drawAt(buffer, 295, 92 + 24 * row, 1, color);
        }
    }
    const std::int32_t page = gLegacyRecipePageD67D0C;
    const std::int32_t pageCount = gLegacyRecipePageCountD67D10;
    std::sprintf(buffer, "%2d/%2d", page + 1, pageCount);
    drawAt(buffer, 87, 350, 1, 0);
    if (hoveredRow >= 0) {
        const std::int32_t recipeIndex = hoveredRow + 8 * gLegacyRecipePageD67D0C;
        if (world::detail::gSkillsF1BAFC[skill].recipes8E8[recipeIndex].itemId >= 0) {
            for (std::int32_t line = 0; line < 2; ++line) {
                const std::int32_t descriptionRecipe = hoveredRow + 8 * gLegacyRecipePageD67D0C;
                const char* const description =
                    world::detail::gSkillsF1BAFC[skill].recipes8E8[descriptionRecipe].description.data();
                if (input::ExtractLegacyLine_503AB0(buffer, 256, description, line, 28) != 0) {
                    drawAt(buffer, 46, 293 + 20 * line, 0, 0);
                }
            }
            const std::int32_t top = gLegacyCraftRecipePanel5845F8.y;
            const std::uint32_t image = gValueD69768;
            const std::int32_t left = gLegacyCraftRecipePanel5845F8.x;
            DrawGraphicHover(Low16(left + 11), Low16(top + 89 + 24 * hoveredRow),
                Low16(left + 333), Low16(top + 109 + 24 * hoveredRow), 214, image, 0);
        }
    }
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    if (Sub_503590(&gLegacyCraftRecipePanel5845F8) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               || (QueryPanelButtonState(&next) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyRecipePage586D2C);
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyRecipeBack58A0E0);
    } else if (hoveredRecipe >= 0) {
        const auto* const recipe = &world::detail::gSkillsF1BAFC[skill].recipes8E8[hoveredRecipe];
        if (recipe->itemId >= 0) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(),
                recipe->value0C == 1 ? kLegacyRecipeSelect58A0D0 : kLegacyRecipeUnavailable58A0B8);
        }
    }
    return Sub_503440(&gLegacyCraftRecipePanel5845F8, 1);
}

std::int32_t Sub_4FAED0()
{
    const std::int32_t result = ui::detail::Sub_4FB8F0();
    gLegacyInventoryPageD67D28 = 0;
    gLegacyInventoryHoverC8AF78 = -1;
    return result;
}

std::uintptr_t Sub_4FAEF0(const std::int32_t opened)
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t x = gLegacyInventoryPanel584C00.x;
    const std::int32_t width = gLegacyInventoryPanel584C00.width;
    const std::int32_t height = gLegacyInventoryPanel584C00.height;
    const std::int32_t y = gLegacyInventoryPanel584C00.y;
    LegacyPanelButtonRecord money {
        x + width / 2, y + height / 2, x + 194, y + 93, 64, 20, 22545, 22546,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyInventoryPanel584C00,
        [] { static_cast<void>(Sub_4FAED0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_4FAED0());
        YobiAction* action = gLegacyInventoryActionD67D2C;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyInventoryActionD67D2C = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    const std::uint8_t rebirthMode = ui::detail::gValue5F9754;
    YobiAction* const initialAction = gLegacyInventoryActionD67D2C;
    bool createPreview = initialAction == nullptr;
    if (rebirthMode != 0) {
        if (world::detail::ReadPlayerValue08() == 0 || world::detail::ReadPlayerTimerRemaining0C() <= 0) {
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, playerX, playerY, 62));
            YobiAction* const current = gLegacyInventoryActionD67D2C;
            ui::detail::gValue5F9754 = 0;
            if (current != nullptr) {
                static_cast<void>(MarkDeath(current));
                gLegacyInventoryActionD67D2C = nullptr;
            }
            createPreview = true;
        } else if (initialAction != nullptr) {
            const std::int32_t previewGroup = (GetYobiActionField2A8(initialAction) - 110350) / 6;
            const std::int32_t playerGroup = (world::detail::ReadPlayerImageNo() - 110350) / 6;
            if (playerGroup != previewGroup) {
                static_cast<void>(MarkDeath(initialAction));
                gLegacyInventoryActionD67D2C = nullptr;
                createPreview = true;
            }
        }
    }
    if (createPreview) {
        const std::int32_t top = gLegacyInventoryPanel584C00.y;
        const std::int32_t left = gLegacyInventoryPanel584C00.x;
        const std::int32_t image = world::detail::ReadPlayerImageNo();
        gLegacyInventoryActionD67D2C = Sub_4FAAA0(image, left + 94, top + 182);
    }
    std::int32_t hovered = -1;
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        const std::int32_t top = kLegacyInventoryPositions584CC4[slot][1] + gLegacyInventoryPanel584C00.y;
        const std::int32_t left = kLegacyInventoryPositions584CC4[slot][0] + gLegacyInventoryPanel584C00.x;
        if (Sub_5049A0(left, top, left + 47, top + 47, -1) != 0) {
            hovered = slot;
            if (slot != gLegacyInventoryHoverC8AF78) {
                gLegacyInventoryPageD67D28 = 0;
            }
            break;
        }
    }
    if ((QueryPanelButtonState(&money) & 1) != 0) {
        if (input::IsChatContext_503E60() == 0) {
            static_cast<void>(audio::Sub_504380(15));
        }
        static_cast<void>(audio::Sub_504180(15));
    }
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    if (hovered >= 0 && (clicks & 0x10u) != 0) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
        if (item->used != 0 && (item->canUseInField & 1u) != 0) {
            if (item->imageNo == 26569) {
                gLegacyInventoryValueC8B388 = 0;
                gLegacySpecialUseItemC8A96C = hovered;
                gLegacySpecialUseColumnCF4AA8 = (hovered - 8) % 5;
                gLegacySpecialUseRowD2D614 = (hovered - 8) / 5;
                static_cast<void>(audio::Sub_504180(79));
            } else {
                const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                static_cast<void>(network::Sub_510A00(platform::gConnectionSocket, playerX, playerY, hovered, 0));
                static_cast<void>(audio::PlaySe_52C450(77, 320));
                static_cast<void>(ui::detail::Sub_4FB8F0());
                gLegacyLastUsedItemCB01EC = hovered;
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            static_cast<void>(ui::detail::Sub_4FB8F0());
        }
    } else {
        const std::int32_t source = ui::detail::gDragItemIndex584DA4;
        if (source < 0) {
            if (hovered >= 0) {
                const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
                if (item->used != 0) {
                    if ((clicks & 1u) != 0) {
                        ui::detail::gDragImageNoD67D24 = item->imageNo;
                        ui::detail::gDragItemIndex584DA4 = hovered;
                        ui::detail::gDraggingCA0B28 = 1;
                        static_cast<void>(audio::PlaySe_52C450(51, 320));
                    } else if ((clicks & 2u) != 0) {
                        const std::int32_t page = gLegacyInventoryPageD67D28;
                        const std::int32_t pages = item->pages;
                        gLegacyInventoryPageD67D28 = page + 1;
                        if (gLegacyInventoryPageD67D28 >= pages) {
                            gLegacyInventoryPageD67D28 = 0;
                        }
                    }
                }
            }
        } else {
            bool resetDrag = false;
            if (hovered >= 0 && (clicks & 1u) != 0) {
                const std::int32_t mode = ui::detail::gDraggingCA0B28;
                if (mode == 1) {
                    if (source != hovered) {
                        const std::uint32_t moveMode = platform::gValueD67AE8;
                        const std::uint8_t control = platform::gKeyStates[17];
                        const std::uint8_t splitKey = moveMode != 0 ? control : static_cast<std::uint8_t>(~control);
                        if (source >= 8 && hovered >= 8
                            && world::detail::gPlayerItemsFDA8A4[source].count > 0
                            && world::detail::gPlayerItemsFDA8A4[hovered].used == 0
                            && (splitKey & 2u) != 0) {
                            gLegacySplitSourceCC3E60 = source;
                            gLegacySplitTargetCC21DC = hovered;
                            gLegacySplitAmountC8A948 = 0;
                            if (input::IsChatContext_503E60() == 0) {
                                static_cast<void>(audio::Sub_504380(16));
                            }
                            static_cast<void>(audio::Sub_504180(16));
                        } else if (ui::detail::gValue5F9754 == 0 || (source != 7 && hovered != 7)) {
                            static_cast<void>(network::Sub_510EC0(platform::gConnectionSocket, source, hovered, 0));
                            static_cast<void>(audio::PlaySe_52C450(51, 320));
                        }
                    }
                } else if (mode == 2 && world::detail::gPlayerItemsFDA8A4[hovered].used != 0) {
                    const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                    const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                    static_cast<void>(network::Sub_510A00(platform::gConnectionSocket, playerX, playerY, source, hovered));
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                }
                resetDrag = true;
            } else if (static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 212u
                       && (clicks & 1u) != 0) {
                if (ui::detail::gDraggingCA0B28 == 1) {
                    const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                    const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                    static_cast<void>(network::Sub_510CE0(platform::gConnectionSocket, playerX, playerY, 0, source));
                }
                resetDrag = true;
            } else if ((clicks & 2u) != 0) {
                resetDrag = true;
            }
            if (resetDrag) {
                static_cast<void>(ui::detail::Sub_4FB8F0());
            }
        }
    }
    if (ui::detail::gDragImageNoD67D24 != 0) {
        ui::detail::gItemDragXCC4880 = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t image = ui::detail::gDragImageNoD67D24;
        ui::detail::gItemDragYCC4884 = mouseY;
        if (image != 0) {
            const std::int32_t mouseX = ui::detail::gItemDragXCC4880;
            static_cast<void>(DrawGraphic(mouseX, mouseY, 221, image, 0, &descriptor));
        }
    }
    char buffer[256];
    for (std::int32_t slot = 27; slot >= 0; --slot) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[slot];
        const auto& position = kLegacyInventoryPositions584CC4[slot];
        if (item->used != 0) {
            if (slot < 7 && (item->flags & 0x80u) == 0) {
                const std::int32_t top = position[1] + gLegacyInventoryPanel584C00.y + 117;
                const std::int32_t left = position[0] + gLegacyInventoryPanel584C00.x + 129;
                static_cast<void>(DrawGraphic(left, top, 213, 22739, 0, &descriptor));
            }
            const std::int32_t image = item->imageNo;
            const std::int32_t top = position[1] + gLegacyInventoryPanel584C00.y + 24;
            const std::int32_t left = position[0] + gLegacyInventoryPanel584C00.x + 24;
            static_cast<void>(DrawGraphic(left, top, 213, image, 0, &descriptor));
            const std::int32_t count = item->count;
            if (count > 0) {
                std::sprintf(buffer, "%3d", count);
                const std::int32_t countY = position[1] + gLegacyInventoryPanel584C00.y + 31;
                const std::int32_t countX = position[0] + gLegacyInventoryPanel584C00.x + 21;
                static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                    static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = kLegacyInventoryPositions584CC4[hovered][1] + gLegacyInventoryPanel584C00.y;
        const std::int32_t left = kLegacyInventoryPositions584CC4[hovered][0] + gLegacyInventoryPanel584C00.x;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 47), Low16(top + 47), 212, gValueD69768, 0);
        const std::int32_t page = gLegacyInventoryPageD67D28;
        std::int32_t detailY = gLegacyInventoryPanel584C00.y;
        if (hovered < 8) {
            detailY += gLegacyInventoryPanel584C00.height / 2;
        }
        const std::int32_t detailX = gLegacyInventoryPanel584C00.x;
        Sub_4FB940(detailX, detailY, hovered, page);
    }
    const std::int32_t dragged = ui::detail::gDragItemIndex584DA4;
    if (dragged >= 0) {
        const std::int32_t top = kLegacyInventoryPositions584CC4[dragged][1] + gLegacyInventoryPanel584C00.y;
        const std::int32_t left = kLegacyInventoryPositions584CC4[dragged][0] + gLegacyInventoryPanel584C00.x;
        DrawGraphicHover(Low16(left + 2), Low16(top + 2), Low16(left + 45), Low16(top + 45), 212, 254, 0);
    }
    std::sprintf(buffer, "%7d", world::detail::ReadPlayerValueFC());
    const std::int32_t moneyY = gLegacyInventoryPanel584C00.y + 71;
    const std::int32_t moneyX = gLegacyInventoryPanel584C00.x + 200;
    static_cast<void>(DrawText(static_cast<std::int16_t>(moneyX),
        static_cast<std::int16_t>(moneyY), 2, 1, 0, buffer, 0, 0));
    Sub_5035E0(&money, 218, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyInventoryPanel584C00) != 0) {
        hint = kLegacySkillClose55331C;
    } else if (ui::detail::gDraggingCA0B28 == 2) {
        hint = kLegacyInventoryTarget58A394;
    } else if (hovered >= 0) {
        const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
        if (item->used != 0 && std::strlen(item->memo.data()) != 0 && (item->flags & 0x100u) != 0) {
            std::sprintf(buffer, "%s[kok]", item->name.data());
            hint = buffer;
        } else if (item->used != 0 && (item->flags & 0x200u) != 0) {
            std::sprintf(buffer, kLegacyInventoryMemo58A38C, item->memo.data());
            hint = buffer;
        } else {
            switch (hovered) {
            case 0: hint = kLegacyInventoryHead585C38; break;
            case 1: hint = kLegacyInventoryBody585C20; break;
            case 2: case 3: hint = kLegacyInventoryHand585C08; break;
            case 4: hint = kLegacyInventoryFeet585BF0; break;
            case 5: case 6: hint = kLegacyInventoryAccessory585BDC; break;
            case 7: hint = kLegacyInventoryCrystal585BC8; break;
            default: break;
            }
        }
    } else if ((QueryPanelButtonState(&money) & 0x40) != 0) {
        hint = kLegacyInventoryMoney58A37C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    const std::uint32_t result = Sub_503440(&gLegacyInventoryPanel584C00, 1);
    gLegacyInventoryHoverC8AF78 = hovered;
    return result;
}

void Sub_4FBB30()
{
    gLegacyMoneyAmountD2D618 = 0;
}

std::uintptr_t Sub_4FBB40(const std::int32_t opened)
{
    const std::int32_t width = gLegacyMoneyPanel584DA8.width;
    const std::int32_t x = gLegacyMoneyPanel584DA8.x;
    const std::int32_t height = gLegacyMoneyPanel584DA8.height;
    const std::int32_t y = gLegacyMoneyPanel584DA8.y;
    const std::int32_t centerX = x + width / 2;
    const std::int32_t centerY = y + height / 2;
    LegacyPanelButtonRecord confirm {centerX, centerY, x + 24, y + 167, 72, 18, 22934, 22934};
    LegacyPanelButtonRecord back {centerX, centerY, x + 107, y + 167, 72, 18, 0, 22936};
    LegacyPanelButtonRecord backspace {centerX, centerY, x + 138, y + 111, 52, 12, 0, 22931};
    LegacyPanelButtonRecord all {centerX, centerY, x + 138, y + 127, 52, 12, 0, 22932};
    LegacyPanelButtonRecord clear {centerX, centerY, x + 138, y + 143, 52, 12, 0, 22933};
    std::array<LegacyPanelButtonRecord, 10> digits;
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        digits[digit] = {centerX, centerY, x + 16 + 24 * (digit % 5),
            y + (digit < 5 ? 135 : 111), 20, 20, 0, 22940 + digit};
    }
    const std::int32_t state = Sub_503160(opened, &gLegacyMoneyPanel584DA8, Sub_4FBB30, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        std::uintptr_t result = 0;
        if (input::IsCurrentContext_503E40(nullptr) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        Sub_4FBB30();
        return result;
    }
    input::SetContext_44B330(nullptr);
    if (gLegacyMoneyAmountD2D618 != 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t amount = gLegacyMoneyAmountD2D618;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_510DA0(platform::gConnectionSocket, playerX, playerY, amount));
        gLegacyMoneyAmountD2D618 = 0;
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(14));
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            gLegacyMoneyAmountD2D618 = world::detail::ReadPlayerValueFC();
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            gLegacyMoneyAmountD2D618 = 0;
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            gLegacyMoneyAmountD2D618 /= 10;
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    gLegacyMoneyAmountD2D618 = 10 * gLegacyMoneyAmountD2D618 + digit;
                    changed = true;
                    break;
                }
            }
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    static_cast<void>(Sub_502B80(&gLegacyMoneyAmountD2D618, 7));
    // 原4FC0C7只读一次金额，供上限修正和第一行显示共同使用。
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    if (money < gLegacyMoneyAmountD2D618) {
        gLegacyMoneyAmountD2D618 = money;
    }
    char buffer[256];
    std::sprintf(buffer, "%7d", money);
    const std::int32_t moneyY = gLegacyMoneyPanel584DA8.y + 46;
    const std::int32_t moneyX = gLegacyMoneyPanel584DA8.x + 88;
    static_cast<void>(DrawText(static_cast<std::int16_t>(moneyX),
        static_cast<std::int16_t>(moneyY), 2, 1, 0, buffer, 0, 0));
    std::sprintf(buffer, "%7d", gLegacyMoneyAmountD2D618);
    const std::int32_t amountY = gLegacyMoneyPanel584DA8.y + 86;
    const std::int32_t amountX = gLegacyMoneyPanel584DA8.x + 88;
    static_cast<void>(DrawText(static_cast<std::int16_t>(amountX),
        static_cast<std::int16_t>(amountY), 2, 1, 0, buffer, 0, 0));
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 218, 1, gValueD69768, 0);
    }
    if (gLegacyMoneyAmountD2D618 != 0) {
        confirm.normalImage = 0;
        confirm.pressedImage = 22935;
    } else {
        confirm.normalImage = 22934;
        confirm.pressedImage = 22934;
    }
    Sub_5035E0(&confirm, 218, 1, gValueD69768, 0);
    Sub_5035E0(&back, 218, 1, gValueD69768, 0);
    Sub_5035E0(&backspace, 218, 1, gValueD69768, 0);
    Sub_5035E0(&all, 218, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 218, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMoneyPanel584DA8) != 0) {
        hint = kLegacySkillClose55331C;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = gLegacyMoneyAmountD2D618 != 0 ? kLegacyMoneyDrop585B54 : kLegacyMoneyEmpty585B44;
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacySplitBack589CA4;
    } else if ((QueryPanelButtonState(&all) & 0x40) != 0) {
        hint = kLegacyMoneyAll585B28;
    } else if ((QueryPanelButtonState(&clear) & 0x40) != 0) {
        hint = kLegacyMoneyClear585B14;
    } else if ((QueryPanelButtonState(&backspace) & 0x40) != 0) {
        hint = kLegacySplitBackspace585B34;
    } else {
        for (auto& digit : digits) {
            if ((QueryPanelButtonState(&digit) & 0x40) != 0) {
                const std::int32_t amount = gLegacyMoneyAmountD2D618;
                const std::int32_t currentMoney = world::detail::ReadPlayerValueFC();
                hint = amount < currentMoney ? kLegacyMoneyDigit585B6C : kLegacyMoneyLimit58A3B4;
                break;
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMoneyPanel584DA8, 1);
}

std::int32_t Sub_500370(const std::int32_t opened)
{
    std::uint8_t usedSlots = 0;
    const std::int32_t result = Sub_503160(opened, &gLegacySkillList585708, nullptr, 1);
    if (result == 0 || result == 2) {
        return result;
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::int32_t top = gLegacySkillList585708.y;
        const std::int32_t bottom = row * 29 + top + 139;
        const std::int32_t y = row * 29 + top + 119;
        const std::int32_t left = gLegacySkillList585708.x;
        const std::int32_t right = left + 263;
        if (Sub_5049A0(left + 5, y, right, bottom, -1) != 0) {
            hovered = row;
            const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
            if ((clicks & 1u) != 0 && world::detail::gSkillPairsF63370[row].used != 0) {
                static_cast<void>(audio::Sub_504180(19));
                ui::detail::gSkillPairIndexD67AD8 = static_cast<std::uint8_t>(row);
                gLegacySkillPendingD67C48 = 0;
            } else if ((clicks & 2u) != 0 && world::detail::gSkillPairsF63370[row].used != 0
                && world::detail::gValueDA3B20 == 0) {
                const std::int32_t skill = std::bit_cast<std::int16_t>(
                    world::detail::gSkillPairsF63370[row].index);
                const std::uintptr_t socket = platform::gConnectionSocket;
                static_cast<void>(network::Sub_512470(socket, skill, -1));
                world::detail::gValueDA3B20 = 1;
            }
            break;
        }
    }
    char buffer[128];
    const std::int32_t nameWidth = TextMeasure(world::detail::BorrowPlayerJobName(), 0);
    const std::int32_t nameTop = gLegacySkillList585708.y;
    const std::int32_t nameLeft = gLegacySkillList585708.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameLeft - nameWidth / 2 + 147),
        static_cast<std::int16_t>(nameTop + 45), 2, 0, 0,
        world::detail::BorrowPlayerJobName(), 0, 0));
    for (std::int32_t row = 0; row < 15; ++row) {
        const world::detail::SkillPairRecord* const pair = &world::detail::gSkillPairsF63370[row];
        if (pair->used == 0) {
            continue;
        }
        const std::int32_t skillIndex = std::bit_cast<std::int16_t>(pair->index);
        const world::detail::SkillRuntimeRecord* const skill = &world::detail::gSkillsF1BAFC[skillIndex];
        usedSlots = static_cast<std::uint8_t>(usedSlots + skill->requiredSlots19[0]);
        const std::int32_t skillTop = gLegacySkillList585708.y;
        const std::int32_t skillLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(skillLeft + 30),
            static_cast<std::int16_t>(skillTop + row * 29 + 120), 2, 0, 0,
            skill->name.data(), 0, 0));
        std::sprintf(buffer, "%2d", skill->level);
        const std::int32_t levelTop = gLegacySkillList585708.y;
        const std::int32_t levelLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(levelLeft + 203),
            static_cast<std::int16_t>(levelTop + row * 29 + 120), 2, 0, 0, buffer, 0, 0));
        const std::int32_t required = std::bit_cast<std::int8_t>(skill->requiredSlots19[0]);
        std::sprintf(buffer, "%d", required);
        const std::int32_t slotsTop = gLegacySkillList585708.y;
        const std::int32_t slotsLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(slotsLeft + 250),
            static_cast<std::int16_t>(slotsTop + row * 29 + 120), 2, 0, 0, buffer, 0, 0));
        const std::int32_t imageTop = gLegacySkillList585708.y;
        const std::int32_t imageLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawGraphic(imageLeft + 134, row * 29 + imageTop + 129,
            212, 0x58E4, 0, nullptr));
        if (row == hovered) {
            const std::int32_t hoverTop = gLegacySkillList585708.y;
            const std::uint32_t image = gValueD69768;
            const std::int32_t hoverLeft = gLegacySkillList585708.x;
            DrawGraphicHover(
                static_cast<std::uint16_t>(hoverLeft + 5),
                static_cast<std::uint16_t>(row * 29 + hoverTop + 119),
                static_cast<std::uint16_t>(hoverLeft + 263),
                static_cast<std::uint16_t>(row * 29 + hoverTop + 139),
                214, image, 0);
        }
    }
    const std::int32_t signedUsed = std::bit_cast<std::int8_t>(usedSlots);
    std::sprintf(buffer, "%2d", 10 - signedUsed);
    const std::int32_t footerTop = gLegacySkillList585708.y;
    const std::int32_t footerLeft = gLegacySkillList585708.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(footerLeft + 156),
        static_cast<std::int16_t>(footerTop + 73), 2, 0, 0, buffer, 0, 0));
    if (Sub_503590(&gLegacySkillList585708) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    } else if (hovered >= 0 && world::detail::gSkillPairsF63370[hovered].used != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillOpen5864C8);
    }
    return std::bit_cast<std::int32_t>(Sub_503440(&gLegacySkillList585708, 1));
}

std::uintptr_t Sub_5006F0(const std::int32_t opened)
{
    gLegacyProfilePanel5857D0.backgrounds[0] =
        character_select::state::gPukVersion < 7 ? 0x57E5 : 0x3A3D2;
    using PageCallback = std::int32_t (*)(std::int32_t, std::int32_t);
    constexpr std::array<PageCallback, 3> pages {Sub_5008C0, Sub_5014A0, Sub_501FD0};
    const std::int32_t state = Sub_503160(opened, &gLegacyProfilePanel5857D0, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        input::InputContext* const context = gLegacyProfileInputD6885C;
        gLegacyProfilePanel5857D0.selected = 0;
        std::uintptr_t result = 0;
        if (input::IsCurrentContext_503E40(context) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            // 原44B330在EAX透传上下文；只在混合返回边界转换地址。
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyProfileInputD6885C = nullptr;
        gLegacyProfilePreviousPage585894 = -1;
        return result;
    }
    if (platform::ReadControlKeyPressFlag_4597C0(9) != 0) {
        const std::int8_t next = static_cast<std::int8_t>(gLegacyProfilePanel5857D0.selected + 1);
        gLegacyProfilePanel5857D0.selected = next;
        if (next >= 3) {
            gLegacyProfilePanel5857D0.selected = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(52, 320));
    }
    std::int8_t selected = gLegacyProfilePanel5857D0.selected;
    const PageCallback page = pages[selected];
    if (page != nullptr) {
        const std::int32_t y = gLegacyProfilePanel5857D0.y;
        const std::int32_t x = gLegacyProfilePanel5857D0.x;
        static_cast<void>(page(x, y));
        selected = gLegacyProfilePanel5857D0.selected;
    }
    if (selected < 2) {
        const std::int32_t width = TextMeasure(world::detail::BorrowPlayerDisplayText(1), 0);
        const std::int32_t y = gLegacyProfilePanel5857D0.y;
        const std::int32_t x = gLegacyProfilePanel5857D0.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 116),
            static_cast<std::int16_t>(y + 9), 2, 0, 0, world::detail::BorrowPlayerDisplayText(1), 0, 0));
        selected = gLegacyProfilePanel5857D0.selected;
    }
    if (gLegacyProfilePreviousPage585894 != selected) {
        input::InputContext* context;
        if (selected < 2) {
            context = &input::gInputContext626AA0;
        } else {
            if (input::IsChatContext_503E60() == 0) {
                static_cast<void>(audio::Sub_504380(2));
            }
            context = gLegacyProfileInputD6885C;
        }
        input::SetContext_44B330(context);
    }
    if (Sub_503590(&gLegacyProfilePanel5857D0) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    }
    const std::uint32_t result = Sub_503440(&gLegacyProfilePanel5857D0, 1);
    gLegacyProfilePreviousPage585894 = gLegacyProfilePanel5857D0.selected;
    return static_cast<std::uintptr_t>(result);
}

std::int32_t Sub_5008C0(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t panelX = gLegacyProfilePanel5857D0.x;
    const std::int32_t panelY = gLegacyProfilePanel5857D0.y;
    std::array<LegacyPanelButtonRecord, 2> buttons {{
        {panelX + 203, panelY + 388, panelX + 159, panelY + 378, 88, 20, 22513, 22514},
        {panelX + 203, panelY + 388, panelX + 159, panelY + 378, 88, 20, 22511, 22512},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    if (character_select::state::gPukVersion >= 7) {
        for (LegacyPanelButtonRecord& button : buttons) {
            button.imageY += 6;
            button.top += 6;
        }
    }
    const std::int32_t clickedPosition = std::bit_cast<std::int16_t>(world::detail::ReadPlayerValue1A0());
    if ((QueryPanelButtonState(&buttons[clickedPosition]) & 1) != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyProfilePositionTickD68860 + 500u < now) {
            static_cast<void>(network::Sub_512570(platform::gConnectionSocket));
            gLegacyProfilePositionTickD68860 = platform::ReadTickCount32();
        }
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    }

    char buffer[256];
    const auto versionY = [](const std::int32_t oldY, const std::int32_t newY) {
        return character_select::state::gPukVersion >= 7 ? newY : oldY;
    };
    const auto drawAt = [&](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    const auto drawNumber = [&](const std::int32_t value, const std::int32_t offsetX,
        const std::int32_t oldY, const std::int32_t newY, const char* const format) {
        std::sprintf(buffer, format, value);
        drawAt(buffer, offsetX, versionY(oldY, newY));
    };
    const std::int32_t tribeWidth = TextMeasure(
        ui::detail::gTribeNames57CB54[world::detail::ReadPlayerTribe()], 1);
    const std::int32_t tribe = world::detail::ReadPlayerTribe();
    const std::int32_t tribeY = versionY(168, 185);
    drawAt(ui::detail::gTribeNames57CB54[tribe], 220 - tribeWidth / 2, tribeY);
    drawNumber(world::detail::ReadPlayerDuelPoint(), 185, 145, 162, "%9d");
    const std::int32_t title = world::detail::ReadPlayerTitleIndex();
    if (title >= 0) {
        const std::int32_t width = TextMeasure(world::detail::gTitlesFF0508[title].text.data(), 1);
        const std::int32_t titleY = versionY(75, 68);
        const std::int32_t currentTitle = world::detail::ReadPlayerTitleIndex();
        drawAt(world::detail::gTitlesFF0508[currentTitle].text.data(), 198 - width / 2, titleY);
    }
    const char* const ownTitle = world::detail::BorrowPlayerDisplayText(2);
    if (std::strlen(ownTitle) != 0) {
        const std::int32_t width = TextMeasure(ownTitle, 1);
        const std::int32_t titleY = versionY(98, 91);
        drawAt(world::detail::BorrowPlayerDisplayText(2), 198 - width / 2, titleY);
    }
    if (character_select::state::gPukVersion >= 7) {
        const std::int32_t rankIndex = ReadGuildTextIndex10514B0();
        const char* const rank = BorrowGuildRankText1051518(rankIndex);
        if (std::strlen(rank) != 0) {
            const std::int32_t width = TextMeasure(rank, 1);
            const std::int32_t currentRank = ReadGuildTextIndex10514B0();
            drawAt(BorrowGuildRankText1051518(currentRank), 198 - width / 2, 114);
        }
    }
    const char* const job = world::detail::BorrowPlayerJobName();
    if (std::strlen(job) != 0) {
        const std::int32_t width = TextMeasure(job, 1);
        const std::int32_t jobY = versionY(52, 45);
        drawAt(world::detail::BorrowPlayerJobName(), 198 - width / 2, jobY);
    }
    drawNumber(world::detail::ReadPlayerLevel(), 62, 122, 140, "%3d");
    const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
    const std::int32_t hp = world::detail::ReadPlayerHpOnce();
    std::sprintf(buffer, "%4d/%4d", hp, maxHp);
    drawAt(buffer, 55, versionY(191, 206));
    const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
    const std::int32_t fp = world::detail::ReadPlayerFpOnce();
    std::sprintf(buffer, "%4d/%4d", fp, maxFp);
    drawAt(buffer, 55, versionY(214, 228));
    drawNumber(world::detail::ReadPlayerValue78(8), 62, 237, 250, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(9), 62, 260, 272, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(10), 62, 283, 294, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(11), 62, 306, 316, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(12), 62, 329, 338, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(13), 62, 352, 360, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(5), 193, 306, 316, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(6), 193, 329, 338, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(7), 193, 352, 360, "%3d");
    drawNumber(world::detail::ReadPlayerExperienceOnce(), 55, 145, 162, "%9d");
    const char* nextExperience = "   ---   ";
    if (world::detail::ReadPlayerLevel() < 130) {
        const std::int32_t next = world::detail::ReadPlayerNextExperienceOnce();
        std::sprintf(buffer, "%9d", next);
        nextExperience = buffer;
    }
    drawAt(nextExperience, 55, versionY(168, 184));

    const std::int32_t barX = x + 185;
    std::int32_t barY = y + 241;
    for (std::int32_t index = 0; index < 4; ++index) {
        const std::int32_t right = barX + 7 * (world::detail::ReadPlayerAttribute(index) / 10);
        const bool modern = character_select::state::gPukVersion >= 7;
        DrawGraphicHover(static_cast<std::uint16_t>(barX),
            static_cast<std::uint16_t>(barY - (modern ? 11 : 26)),
            static_cast<std::uint16_t>(right),
            static_cast<std::uint16_t>(barY - (modern ? 2 : 17)),
            215, kLegacyAttributeBars57CBB8[index][0], 1);
        DrawGraphicHover(static_cast<std::uint16_t>(barX + 2),
            static_cast<std::uint16_t>(barY - (modern ? 9 : 24)),
            static_cast<std::uint16_t>(right + 2),
            static_cast<std::uint16_t>(barY - (modern ? 0 : 15)),
            214, kLegacyAttributeBars57CBB8[index][1], 0);
        barY += 20;
    }
    const std::int32_t portraitOffsetY = versionY(77, 86);
    const std::int32_t portrait = world::detail::ReadPlayerValue17C();
    const std::int32_t portraitY = gLegacyProfilePanel5857D0.y;
    const std::int32_t portraitX = gLegacyProfilePanel5857D0.x;
    static_cast<void>(DrawGraphic(portraitX + 40, portraitY + portraitOffsetY,
        212, portrait, 0, &descriptor));
    const std::int32_t injuryOffsetY = versionY(129, 146);
    const std::int32_t injuryImage = ui::detail::ReadLegacyHealthImage(world::detail::ReadPlayerInjury());
    const std::int32_t injuryY = gLegacyProfilePanel5857D0.y;
    const std::int32_t injuryX = gLegacyProfilePanel5857D0.x;
    static_cast<void>(DrawGraphic(injuryX + 183, injuryY + injuryOffsetY, 212, injuryImage, 0, nullptr));
    const std::int32_t drawnPosition = std::bit_cast<std::int16_t>(world::detail::ReadPlayerValue1A0());
    const std::uint32_t hoverImage = gValueD69768;
    Sub_5035E0(&buttons[drawnPosition], 212, 1, hoverImage, 0);
    if (world::detail::ReadPlayerPenalty() != 0) {
        const std::int32_t penaltyOffsetY = versionY(129, 146);
        const std::int32_t penaltyY = gLegacyProfilePanel5857D0.y;
        const std::int32_t penaltyX = gLegacyProfilePanel5857D0.x;
        static_cast<void>(DrawGraphic(penaltyX + 227, penaltyY + penaltyOffsetY, 212, 22527, 0, nullptr));
        drawNumber(world::detail::ReadPlayerPenalty(), 241, 122, 139, "x%d");
    }
    const std::int32_t hoveredPosition = std::bit_cast<std::int16_t>(world::detail::ReadPlayerValue1A0());
    if ((QueryPanelButtonState(&buttons[hoveredPosition]) & 0x40) != 0) {
        const char* const hint = world::detail::ReadPlayerValue1A0() != 0
            ? kLegacyProfileRear586344 : kLegacyProfileFront586360;
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return 0;
}

std::int32_t Sub_5014A0(const std::int32_t x, const std::int32_t y)
{
    std::array<LegacyPanelButtonRecord, 5> buttons;
    for (std::int32_t index = 0; index < 5; ++index) {
        buttons[index] = {x + 241, y + 92 + index * 23, x + 226, y + 82 + index * 23,
            31, 19, 22517, 22518};
    }
    std::array<std::int32_t, 6> values;
    values[1] = world::detail::ReadPlayerValue78(1);
    values[0] = world::detail::ReadPlayerValue78(0);
    values[2] = world::detail::ReadPlayerValue78(2);
    values[3] = world::detail::ReadPlayerValue78(3);
    values[4] = world::detail::ReadPlayerValue78(4);
    values[5] = world::detail::gValueD2F788;
    std::int32_t hovered = -1;
    std::int32_t total = 0;
    for (const std::int32_t value : values) {
        total += value;
    }
    const std::int32_t limit = total / 2;
    if (values[5] != 0) {
        for (std::int32_t index = 0; index < 5; ++index) {
            if ((QueryPanelButtonState(&buttons[index]) & 0x40) != 0) {
                hovered = index;
                if (values[index] >= limit) {
                    hovered = -2;
                }
            }
            if ((QueryPanelButtonState(&buttons[index]) & 1) != 0 && values[index] < limit) {
                static_cast<void>(network::Sub_5123B0(platform::gConnectionSocket, index));
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                break;
            }
        }
    }
    char buffer[256];
    const auto drawNumber = [&](const std::int32_t value, const std::int32_t offsetX,
        const std::int32_t offsetY, const char* const format, const std::int8_t color = 0) {
        std::sprintf(buffer, format, value);
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, color, buffer, 0, 0));
    };
    drawNumber(world::detail::ReadPlayerLevel(), 62, 38, "%3d");
    const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
    const std::int32_t hp = world::detail::ReadPlayerHpOnce();
    std::sprintf(buffer, "%4d/%4d", hp, maxHp);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 55),
        static_cast<std::int16_t>(y + 61), 2, 1, 0, buffer, 0, 0));
    const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
    const std::int32_t fp = world::detail::ReadPlayerFpOnce();
    std::sprintf(buffer, "%4d/%4d", fp, maxFp);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 55),
        static_cast<std::int16_t>(y + 84), 2, 1, 0, buffer, 0, 0));
    drawNumber(world::detail::ReadPlayerValue78(8), 62, 107, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(9), 62, 130, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(10), 62, 153, "%3d");
    drawNumber(world::detail::ReadPlayerValueB0(0), 62, 176, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(11), 62, 199, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(12), 62, 222, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(13), 62, 245, "%3d");
    drawNumber(world::detail::ReadPlayerValueB0(1), 62, 269, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(5), 186, 206, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(6), 186, 228, "%3d");
    drawNumber(world::detail::ReadPlayerValue78(7), 186, 252, "%3d");
    drawNumber(world::detail::ReadPlayerValueD4(0), 194, 298, "%+4d");
    drawNumber(world::detail::ReadPlayerValueD4(1), 194, 321, "%+4d");
    drawNumber(world::detail::ReadPlayerValueD4(2), 194, 344, "%+4d");
    drawNumber(world::detail::ReadPlayerValueD4(3), 194, 367, "%+4d");
    drawNumber(world::detail::gValueD2F788, 220, 61, "%4d");
    const std::int8_t color = world::detail::ReadPlayerPenalty() != 0 ? 6 : 0;
    drawNumber(world::detail::ReadPlayerValue78(0), 192, 84, "%3d", color);
    drawNumber(world::detail::ReadPlayerValue78(1), 192, 107, "%3d", color);
    drawNumber(world::detail::ReadPlayerValue78(2), 192, 130, "%3d", color);
    drawNumber(world::detail::ReadPlayerValue78(3), 192, 153, "%3d", color);
    drawNumber(world::detail::ReadPlayerValue78(4), 192, 176, "%3d", color);
    drawNumber(world::detail::ReadPlayerValueBC(0), 69, 268, "%+4d");
    drawNumber(world::detail::ReadPlayerValueBC(1), 69, 291, "%+4d");
    drawNumber(world::detail::ReadPlayerValueBC(2), 69, 314, "%+4d");
    drawNumber(world::detail::ReadPlayerValueBC(3), 69, 337, "%+4d");
    drawNumber(world::detail::ReadPlayerValueBC(4), 69, 360, "%+4d");
    drawNumber(world::detail::ReadPlayerValueBC(5), 69, 383, "%+4d");
    static_cast<void>(Sub_501F60());
    if (gLegacyAttributeBlinkCC3EF8 != 0) {
        static_cast<void>(DrawGraphic(x + 177, y + 68, 212, 22516, 1, nullptr));
    }
    if (world::detail::gValueD2F788 != 0) {
        for (LegacyPanelButtonRecord& button : buttons) {
            const std::uint32_t hoverImage = gValueD69768;
            Sub_5035E0(&button, 212, 1, hoverImage, 0);
        }
    }
    const char* hint;
    switch (hovered) {
    case -2: hint = kLegacyAttributeLimit58A220; break;
    case 0: hint = kLegacyAttributeHint58AA08; break;
    case 1: hint = kLegacyAttributeHint58A274; break;
    case 2: hint = kLegacyAttributeHint58A9F4; break;
    case 3: hint = kLegacyAttributeHint58A24C; break;
    case 4: hint = kLegacyAttributeHint58A9E0; break;
    default: return 0;
    }
    std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    return 0;
}

std::int32_t Sub_501FD0(const std::int32_t x, const std::int32_t y)
{
    LegacyPanelButtonRecord submit {x + 135, y + 211, x + 201, y + 57, 64, 24, 22981, 22982};
    LegacyPanelButtonRecord previous {x + 143, y + 109, x + 129, y + 97, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 235, y + 109, x + 221, y + 97, 28, 24, 23023, 23027};
    LegacyPanelButtonRecord remove {x + 77, y + 387, x + 37, y + 377, 80, 20, 22985, 22986};
    LegacyPanelButtonRecord clear {x + 195, y + 387, x + 155, y + 377, 80, 20, 22983, 22984};
    std::int32_t submitState = 0;
    std::int32_t nextState = 0;
    std::int32_t removeState = 0;
    std::int32_t clearState = 0;
    std::int32_t hovered = -1;
    std::int32_t hoveredTitle = -1;
    char buffer[32];
    char text[128];
    char escaped[128];
    if (gLegacyProfileInputD6885C == nullptr) {
        std::strcpy(text, world::detail::BorrowPlayerDisplayText(2));
        input::gInputContextD2E2E0.text[0] = '\0';
        input::gInputContextD2E2E0.totalByteLength = 0;
        input::gInputContextD2E2E0.caretByteOffset = 0;
        gLegacyProfileInputD6885C = &input::gInputContextD2E2E0;
        input::InitializeContext_449CF0(&input::gInputContextD2E2E0,
            x + 12, y + 62, 2, 0, 0, text, 1, 16, 0, 0);
        gLegacyTitlePageCF49B4 = 0;
        gLegacyTitleDeleteCF4678 = 0;
        gLegacyTitlePageCountC8AF80 = static_cast<std::int8_t>(world::detail::Sub_519870());
    }
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t top = y + 133 + row * 30;
        if (Sub_5049A0(x + 41, top, x + 229, top + 22, -1) != 0) {
            hovered = row;
            hoveredTitle = row + 8 * gLegacyTitlePageCF49B4;
            break;
        }
    }
    const std::int32_t previousState = QueryPanelButtonState(&previous);
    if ((previousState & 1) != 0) {
        gLegacyTitlePageCF49B4 = static_cast<std::int8_t>(gLegacyTitlePageCF49B4 - 1);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyTitlePageCF49B4 < 0) {
            gLegacyTitlePageCF49B4 = static_cast<std::int8_t>(gLegacyTitlePageCountC8AF80 - 1);
        }
    } else if (((nextState = QueryPanelButtonState(&next)) & 1) != 0) {
        gLegacyTitlePageCF49B4 = static_cast<std::int8_t>(gLegacyTitlePageCF49B4 + 1);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyTitlePageCF49B4 >= gLegacyTitlePageCountC8AF80) {
            gLegacyTitlePageCF49B4 = 0;
        }
    } else if (((submitState = QueryPanelButtonState(&submit)) & 1) != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyProfileEditTickD68864 + 500u < now) {
            const input::InputContext* const context = gLegacyProfileInputD6885C;
            if (std::strcmp(world::detail::BorrowPlayerDisplayText(2), context->text) != 0) {
                std::strcpy(text, context->text);
                static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(text, escaped, 128));
                static_cast<void>(network::Sub_512360(platform::gConnectionSocket, escaped));
            }
            gLegacyProfileEditTickD68864 = platform::ReadTickCount32();
        }
        gLegacyTitleDeleteCF4678 = 0;
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if (((removeState = QueryPanelButtonState(&remove)) & 1) != 0) {
        if (gLegacyTitleDeleteCF4678 != 0) {
            gLegacyTitleDeleteCF4678 = 0;
            static_cast<void>(audio::PlaySe_52C450(52, 320));
        } else {
            gLegacyTitleDeleteCF4678 = 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    } else if (((clearState = QueryPanelButtonState(&clear)) & 1) != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyProfileEditTickD68864 + 500u < now) {
            static_cast<void>(network::Sub_5122C0(platform::gConnectionSocket, -1));
            gLegacyTitleDeleteCF4678 = 0;
            gLegacyProfileEditTickD68864 = platform::ReadTickCount32();
        }
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else {
        const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
        if (hovered >= 0 && (clicks & 1u) != 0) {
            const world::detail::TitleRecord* const title = &world::detail::gSortedTitlesDE03F8[hoveredTitle];
            if (title->id >= 0) {
                const std::uint32_t now = platform::ReadTickCount32();
                if (gLegacyProfileEditTickD68864 + 500u < now) {
                    if (gLegacyTitleDeleteCF4678 == 0) {
                        const std::int32_t current = world::detail::ReadPlayerTitleIndex();
                        bool different = current < 0;
                        if (!different) {
                            const std::int32_t id = title->id;
                            different = world::detail::gTitlesFF0508[current].id != id;
                        }
                        if (different) {
                            const std::int32_t index = world::detail::Sub_519840(title->id);
                            if (index >= 0) {
                                static_cast<void>(network::Sub_5122C0(platform::gConnectionSocket, index));
                            }
                        }
                    } else {
                        const std::int32_t index = world::detail::Sub_519840(title->id);
                        if (index >= 0) {
                            static_cast<void>(network::Sub_512310(platform::gConnectionSocket, index));
                            gLegacyTitleDeleteCF4678 = 0;
                        }
                    }
                    gLegacyProfileEditTickD68864 = platform::ReadTickCount32();
                }
                static_cast<void>(audio::PlaySe_52C450(79, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        } else if (gLegacyTitleDeleteCF4678 != 0 && (clicks & 3u) != 0) {
            gLegacyTitleDeleteCF4678 = 0;
        }
    }
    if (hovered >= 0) {
        const std::uint32_t hoverImage = gLegacyTitleDeleteCF4678 != 0 ? 249 : gValueD69768;
        DrawGraphicHover(static_cast<std::uint16_t>(x + 41),
            static_cast<std::uint16_t>(y + hovered * 30 + 133),
            static_cast<std::uint16_t>(x + 229),
            static_cast<std::uint16_t>(y + hovered * 30 + 155), 214, hoverImage, 0);
    }
    std::int32_t currentTitle = world::detail::ReadPlayerTitleIndex();
    std::int32_t rowTop = y + 133;
    for (std::int32_t row = 0; row < 8; ++row, rowTop += 30) {
        const std::int32_t index = row + 8 * gLegacyTitlePageCF49B4;
        const world::detail::TitleRecord* const title = &world::detail::gSortedTitlesDE03F8[index];
        if (title->id >= 0) {
            std::int8_t color = 0;
            if (currentTitle >= 0) {
                const std::int32_t id = title->id;
                color = world::detail::gTitlesFF0508[currentTitle].id == id ? 1 : 0;
            }
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 44),
                static_cast<std::int16_t>(y + row * 30 + 136), 2, 0, color, title->text.data(), 0, 0));
            currentTitle = world::detail::ReadPlayerTitleIndex();
            if (currentTitle >= 0) {
                const std::int32_t currentIndex = row + 8 * gLegacyTitlePageCF49B4;
                const std::int32_t id = world::detail::gSortedTitlesDE03F8[currentIndex].id;
                if (world::detail::gTitlesFF0508[currentTitle].id == id) {
                    DrawGraphicHover(static_cast<std::uint16_t>(x + 41),
                        static_cast<std::uint16_t>(rowTop), static_cast<std::uint16_t>(x + 229),
                        static_cast<std::uint16_t>(rowTop + 22), 214, 254, 0);
                    currentTitle = world::detail::ReadPlayerTitleIndex();
                }
            }
        }
    }
    render::DrawInputText_4568A0(gLegacyProfileInputD6885C);
    const std::int32_t page = gLegacyTitlePageCF49B4;
    const std::int32_t pages = gLegacyTitlePageCountC8AF80;
    std::sprintf(buffer, "%2d/%2d", page + 1, pages);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 166),
        static_cast<std::int16_t>(y + 101), 2, 0, 0, buffer, 0, 0));
    Sub_5035E0(&submit, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const std::int32_t deleteMode = gLegacyTitleDeleteCF4678;
    Sub_5038B0(&remove, 212, 1, gValueD69768, deleteMode);
    Sub_5035E0(&clear, 212, 1, gValueD69768, 0);
    const char* hint;
    if ((submitState & 0x40) != 0) {
        hint = kLegacyTitleSubmit5863C4;
    } else if ((previousState & 0x40) != 0 || (nextState & 0x40) != 0) {
        hint = kLegacyTitlePage58A98C;
    } else if ((removeState & 0x40) != 0) {
        hint = gLegacyTitleDeleteCF4678 == 0 ? kLegacyTitleDelete5863E8 : kLegacyTitleCancelDelete58AA3C;
    } else if ((clearState & 0x40) != 0) {
        hint = kLegacyTitleClear5863FC;
    } else {
        if (hovered < 0 || world::detail::gSortedTitlesDE03F8[hoveredTitle].id < 0) {
            return 0;
        }
        hint = gLegacyTitleDeleteCF4678 != 0 ? kLegacyTitleRemove58AA1C : kLegacyTitleSelect58AA2C;
    }
    std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    return 0;
}

std::int32_t Sub_4FC420()
{
    return Sub_4FC430();
}

std::int32_t Sub_4FC430()
{
    const std::int32_t y = gLegacySettingsMenu584E70.y;
    const std::int32_t x = gLegacySettingsMenu584E70.x;
    auto& buttons = gLegacySettingsButtonsD67D38;
    // 保留原构造交错写序；两坐标均来自入口单次快照，原+23字节不写。
    buttons[1].label.textY = y + 80;
    buttons[1].label.top = y + 80;
    buttons[2].label.textY = y + 110;
    buttons[2].label.top = y + 110;
    buttons[3].label.textY = y + 140;
    buttons[3].label.top = y + 140;
    buttons[0].label.textY = y + 50;
    buttons[0].label.top = y + 50;
    buttons[4].label.textY = y + 170;
    buttons[4].label.top = y + 170;
    buttons[0].label.textX = x + 45;
    buttons[0].label.left = x + 27;
    buttons[0].label.width = 136;
    buttons[0].label.height = 18;
    buttons[0].label.text = kLegacySettingsText58A4A4;
    buttons[0].callback = &Sub_4FC7F0;
    buttons[0].color = 0;
    buttons[0].reserved21[0] = std::byte {1};
    buttons[0].reserved21[1] = std::byte {0xF9};
    buttons[0].hint = kLegacySettingsHint586DE8;
    buttons[1].label.textX = x + 45;
    buttons[1].label.left = x + 27;
    buttons[1].label.width = 136;
    buttons[1].label.height = 18;
    buttons[1].label.text = kLegacySettingsText58A498;
    buttons[1].callback = &Sub_4FC810;
    buttons[1].color = 0;
    buttons[1].reserved21[0] = std::byte {1};
    buttons[1].reserved21[1] = std::byte {0xF9};
    buttons[1].hint = kLegacySettingsHint58A484;
    buttons[2].label.textX = x + 45;
    buttons[2].label.left = x + 27;
    buttons[2].label.width = 136;
    buttons[2].label.height = 18;
    buttons[2].label.text = kLegacySettingsText58A478;
    buttons[2].callback = &Sub_4FC830;
    buttons[2].color = 0;
    buttons[2].reserved21[0] = std::byte {1};
    buttons[2].reserved21[1] = std::byte {0xF9};
    buttons[2].hint = kLegacySettingsHint58A460;
    buttons[3].label.textX = x + 45;
    buttons[3].label.left = x + 27;
    buttons[3].label.width = 136;
    buttons[3].label.height = 18;
    buttons[3].label.text = kLegacySettingsText58A454;
    buttons[3].callback = &Sub_4FC850;
    buttons[3].color = 0;
    buttons[3].reserved21[0] = std::byte {1};
    buttons[3].reserved21[1] = std::byte {0xF9};
    buttons[3].hint = kLegacySettingsHint58A440;
    buttons[4].label.textX = x + 45;
    buttons[4].label.left = x + 27;
    buttons[4].label.width = 136;
    buttons[4].label.height = 18;
    buttons[4].label.text = kLegacySettingsText58A434;
    buttons[4].callback = &Sub_4FC870;
    buttons[4].color = 0;
    buttons[4].reserved21[0] = std::byte {1};
    buttons[4].reserved21[1] = std::byte {0xF9};
    buttons[4].hint = kLegacySettingsHint58A420;
    buttons[5].label.textX = x + 45;
    buttons[5].label.textY = y + 200;
    buttons[5].label.left = x + 27;
    buttons[5].label.top = y + 200;
    buttons[5].label.width = 136;
    buttons[6].label.width = 136;
    buttons[7].label.width = 136;
    buttons[5].label.height = 18;
    buttons[6].label.textX = x + 45;
    buttons[6].label.height = 18;
    buttons[7].label.textX = x + 45;
    buttons[7].label.height = 18;
    buttons[6].label.textY = y + 230;
    buttons[6].label.top = y + 230;
    buttons[5].reserved21[1] = std::byte {0xF9};
    buttons[6].reserved21[1] = std::byte {0xF9};
    buttons[7].reserved21[1] = std::byte {0xF9};
    buttons[5].label.text = kLegacySettingsText58A414;
    buttons[5].callback = &Sub_4FC890;
    buttons[5].color = 0;
    buttons[5].reserved21[0] = std::byte {1};
    buttons[5].hint = kLegacySettingsHint58A400;
    buttons[6].label.left = x + 27;
    buttons[6].label.text = kLegacySettingsText58A3F4;
    buttons[6].callback = &Sub_4FC8B0;
    buttons[6].color = 0;
    buttons[6].reserved21[0] = std::byte {1};
    buttons[6].hint = kLegacySettingsHint58A3E4;
    buttons[7].label.textY = y + 260;
    buttons[7].label.left = x + 27;
    buttons[7].label.top = y + 260;
    buttons[7].label.text = kLegacySettingsText58A3D8;
    buttons[7].callback = &Sub_4FC8D0;
    buttons[7].color = 0;
    buttons[7].reserved21[0] = std::byte {1};
    buttons[7].hint = kLegacySettingsHint58A3C8;
    return y + 260;
}

std::uint32_t Sub_4FC730(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacySettingsMenu584E70, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacySettingsButtonsD67D38.data(), 8);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    Sub_503120(gLegacySettingsButtonsD67D38.data(), 8);
    if (Sub_503590(&gLegacySettingsMenu584E70) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    }
    return Sub_503440(&gLegacySettingsMenu584E70, 1);
}

std::uint32_t Sub_4FC8F0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyLogoutPanel584F38.x;
    const std::int32_t y = gLegacyLogoutPanel584F38.y;
    const character_select::detail::CharacterModalUiRecord town {
        x + 35, y + 70, x + 26, y + 70, 138, 18, kLegacyLogout58A4D8,
    };
    const character_select::detail::CharacterModalUiRecord server {
        x + 35, y + 120, x + 26, y + 120, 138, 18, kLegacyLogout58A4C8,
    };
    const character_select::detail::CharacterModalUiRecord back {
        x + 62, y + 260, x + 60, y + 260, 69, 18, kLegacyButton58A4BC,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyLogoutPanel584F38, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    if ((character_select::detail::Sub_503990(&server) & 1) != 0) {
        platform::LegacyCryptInt temporary;
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 0);
        const std::uint32_t encoded = temporary.encoded;
        const std::uint32_t mask = temporary.mask;
        const std::uint8_t readCount = temporary.readCount;
        world::detail::gValue73ECD0.encoded = encoded;
        world::detail::gValue73ECD0.mask = mask;
        world::detail::gValue73ECD0.readCount = readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);
        static_cast<void>(world::SetPendingSceneType(7));
        static_cast<void>(PlaySe(60, 320, 240));
    } else if ((character_select::detail::Sub_503990(&town) & 1) != 0) {
        static_cast<void>(network::Sub_512CF0(platform::gConnectionSocket));
        if (world::gCurrentSceneType == 10) {
            world::gCurrentSceneState = 7;
        }
        static_cast<void>(PlaySe(60, 320, 240));
        world::gValueD67E78 = 1;
    } else if ((character_select::detail::Sub_503990(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(3));
    }
    static_cast<void>(character_select::detail::Sub_503A20(&town, 2, 0, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&server, 2, 0, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&back, 2, 0, 1, gValueD69768, 218));
    if (Sub_503590(&gLegacyLogoutPanel584F38) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    }
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&server) & 0x40) != 0) {
        hint = kLegacyLogout586E60;
    } else if ((character_select::detail::Sub_503990(&town) & 0x40) != 0) {
        hint = kLegacyLogout586E48;
    } else if ((character_select::detail::Sub_503990(&back) & 0x40) != 0) {
        hint = kLegacyLogout58A4B0;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyLogoutPanel584F38, 1);
}

std::int32_t Sub_4FCB90()
{
    return Sub_4FCBA0();
}

std::int32_t Sub_4FCBA0()
{
    const std::int32_t x = gLegacyChatSettings585000.x;
    const std::int32_t y = gLegacyChatSettings585000.y;
    auto& records = gLegacyChatRecordsD67E80;
    // 按原216次字段写入推进；保留每条+23H字节，不整表赋值。
    records[2].label.textX = x + 84;
    records[3].label.textX = x + 124;
    records[3].label.left = x + 123;
    records[0].label.textY = y + 40;
    records[1].label.textY = y + 60;
    records[2].label.textY = y + 60;
    records[3].label.textY = y + 60;
    records[3].label.top = y + 60;
    records[4].label.textX = x + 156;
    records[4].label.textY = y + 60;
    records[4].label.top = y + 60;
    records[0].label.textX = x + 20;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyChat58A584;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[0] = std::byte {0xFF};
    records[0].reserved21[1] = std::byte {0};
    records[0].hint = nullptr;
    records[1].label.textX = x + 20;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A57C;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[1] = std::byte {0};
    records[1].hint = nullptr;
    records[2].label.left = 0;
    records[2].label.top = 0;
    records[2].label.width = 0;
    records[2].label.height = 0;
    records[2].label.text = kLegacyChat58A578;
    records[2].callback = nullptr;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {0xFF};
    records[2].reserved21[1] = std::byte {0};
    records[2].hint = nullptr;
    records[3].label.width = 19;
    records[3].label.height = 18;
    records[3].label.text = kLegacyChat58A574;
    records[3].callback = &Sub_4FD4C0;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {1};
    records[3].reserved21[1] = std::byte {0xF9};
    records[3].hint = nullptr;
    records[4].label.left = x + 155;
    records[4].label.width = 19;
    records[4].label.height = 18;
    records[4].label.text = kLegacyChat58A570;
    records[4].callback = &Sub_4FD510;
    records[4].color = 0;
    records[4].reserved21[0] = std::byte {1};
    records[4].reserved21[1] = std::byte {0xF9};
    records[4].hint = nullptr;
    records[5].label.textX = x + 20;
    records[5].label.textY = y + 80;
    records[5].label.left = 0;
    records[5].label.top = 0;
    records[6].label.textY = y + 100;
    records[7].label.textY = y + 100;
    records[7].label.top = y + 100;
    records[8].label.textY = y + 120;
    records[9].label.textX = x + 120;
    records[9].label.textY = y + 140;
    records[9].label.top = y + 140;
    records[7].label.textX = x + 120;
    records[10].label.textY = y + 160;
    records[5].label.width = 0;
    records[5].label.height = 0;
    records[5].label.text = kLegacyChat58A560;
    records[5].callback = nullptr;
    records[5].color = 4;
    records[5].reserved21[0] = std::byte {0xFF};
    records[5].reserved21[1] = std::byte {0};
    records[5].hint = nullptr;
    records[6].label.textX = x + 20;
    records[6].label.left = 0;
    records[6].label.top = 0;
    records[6].label.width = 0;
    records[6].label.height = 0;
    records[6].label.text = kLegacyChat58A554;
    records[6].callback = nullptr;
    records[6].color = 0;
    records[6].reserved21[0] = std::byte {0xFF};
    records[6].reserved21[1] = std::byte {0};
    records[6].hint = nullptr;
    records[7].label.left = x + 119;
    records[7].label.width = 54;
    records[7].label.height = 18;
    records[7].label.text = kLegacyChat58A54C;
    records[7].callback = &Sub_4FD550;
    records[7].color = 0;
    records[7].reserved21[0] = std::byte {1};
    records[7].reserved21[1] = std::byte {0xF9};
    records[7].hint = kLegacyChat5877B4;
    records[8].label.textX = x + 20;
    records[8].label.left = 0;
    records[8].label.top = 0;
    records[8].label.width = 0;
    records[8].label.height = 0;
    records[8].label.text = kLegacyChat58A540;
    records[8].callback = nullptr;
    records[8].color = 4;
    records[8].reserved21[0] = std::byte {0xFF};
    records[8].reserved21[1] = std::byte {0};
    records[8].hint = nullptr;
    records[9].label.left = x + 119;
    records[9].label.width = 54;
    records[9].label.height = 18;
    records[9].label.text = kLegacyChat58A538;
    records[9].callback = &Sub_4FD5A0;
    records[9].color = 0;
    records[9].reserved21[0] = std::byte {1};
    records[9].reserved21[1] = std::byte {0xF9};
    records[9].hint = kLegacyChat58A524;
    records[10].label.textX = x + 20;
    records[10].label.left = 0;
    records[10].label.top = 0;
    records[10].label.width = 0;
    records[10].label.height = 0;
    records[10].label.text = kLegacyChat58A514;
    records[10].callback = nullptr;
    records[10].color = 4;
    records[10].reserved21[0] = std::byte {0xFF};
    records[10].reserved21[1] = std::byte {0};
    records[10].hint = nullptr;
    records[11].label.textX = x + 20;
    records[11].label.textY = y + 180;
    records[11].label.left = 0;
    records[11].label.top = 0;
    records[11].label.width = 0;
    records[13].label.textX = x + 20;
    records[12].label.textX = x + 120;
    records[12].label.textY = y + 180;
    records[12].label.top = y + 180;
    records[14].label.textX = x + 20;
    records[13].label.textY = y + 200;
    records[15].label.textX = x + 124;
    records[15].label.left = x + 123;
    records[11].reserved21[0] = std::byte {0xFF};
    records[12].label.left = x + 119;
    records[13].reserved21[0] = std::byte {0xFF};
    records[14].label.textY = y + 220;
    records[14].reserved21[0] = std::byte {0xFF};
    records[15].label.textY = y + 220;
    records[15].label.top = y + 220;
    records[16].label.textX = x + 156;
    records[16].label.textY = y + 220;
    records[16].label.top = y + 220;
    records[11].label.height = 0;
    records[11].label.text = kLegacyChat58A57C;
    records[11].callback = nullptr;
    records[11].color = 0;
    records[11].reserved21[1] = std::byte {0};
    records[11].hint = nullptr;
    records[12].label.width = 54;
    records[12].label.height = 18;
    records[12].label.text = kLegacyChat58A54C;
    records[12].callback = &Sub_4FD5C0;
    records[12].color = 0;
    records[12].reserved21[0] = std::byte {1};
    records[12].reserved21[1] = std::byte {0xF9};
    records[12].hint = nullptr;
    records[13].label.left = 0;
    records[13].label.top = 0;
    records[13].label.width = 0;
    records[13].label.height = 0;
    records[13].label.text = kLegacyChat58A508;
    records[13].callback = nullptr;
    records[13].color = 4;
    records[13].reserved21[1] = std::byte {0};
    records[13].hint = nullptr;
    records[14].label.left = 0;
    records[14].label.top = 0;
    records[14].label.width = 0;
    records[14].label.height = 0;
    records[14].label.text = kLegacyChat58A500;
    records[14].callback = nullptr;
    records[14].color = 0;
    records[14].reserved21[1] = std::byte {0};
    records[14].hint = nullptr;
    records[15].label.width = 19;
    records[15].label.height = 18;
    records[15].label.text = kLegacyChat58A574;
    records[15].callback = &Sub_4FD5F0;
    records[15].color = 0;
    records[15].reserved21[0] = std::byte {1};
    records[15].reserved21[1] = std::byte {0xF9};
    records[15].hint = nullptr;
    records[16].label.left = x + 155;
    records[16].label.width = 19;
    records[16].label.height = 18;
    records[16].label.text = kLegacyChat58A570;
    records[16].callback = &Sub_4FD640;
    records[16].color = 0;
    records[16].reserved21[0] = std::byte {1};
    records[16].reserved21[1] = std::byte {0xF9};
    records[16].hint = nullptr;
    records[17].label.textX = x + 62;
    records[17].label.textY = y + 260;
    records[17].label.height = 18;
    records[17].label.left = x + 60;
    records[17].label.top = y + 260;
    records[17].label.width = 69;
    records[17].label.text = kLegacyChat58A4F8;
    records[17].callback = &Sub_4FE3E0;
    records[17].color = 0;
    records[17].reserved21[0] = std::byte {1};
    records[17].reserved21[1] = std::byte {0xF9};
    records[17].hint = kLegacyChat58A4E8;
    return 0;
}

std::uint32_t Sub_4FD1A0(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyChatSettings585000, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacyChatRecordsD67E80.data(), 18);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    char buffer[128];
    std::sprintf(buffer, kLegacyChat553FB4, platform::gValue55463C);
    const std::int32_t linesY = gLegacyChatSettings585000.y;
    const std::int32_t linesX = gLegacyChatSettings585000.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(linesX + 62),
        static_cast<std::int16_t>(linesY + 60), 2, 0, 0, buffer, 0, 0));
    const char* const soundText = platform::gValue554644 != 0 ? kLegacyChat58A668 : kLegacyChat58A664;
    const std::int32_t soundY = gLegacyChatSettings585000.y;
    std::strcpy(buffer, soundText);
    const std::int32_t soundX = gLegacyChatSettings585000.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(soundX + 62),
        static_cast<std::int16_t>(soundY + 180), 2, 0, 0, buffer, 0, 0));
    std::sprintf(buffer, kLegacyChat5531A4, platform::gValue554640);
    const std::int32_t rangeY = gLegacyChatSettings585000.y;
    const std::int32_t rangeX = gLegacyChatSettings585000.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(rangeX + 62),
        static_cast<std::int16_t>(rangeY + 220), 2, 0, 0, buffer, 0, 0));
    gLegacyChatRecordsD67E80[6].color = static_cast<std::uint8_t>(input::gInputContext626AA0.field140);
    Sub_503120(gLegacyChatRecordsD67E80.data(), 18);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyChatSettings585000) != 0) {
        hint = kLegacySkillClose55331C;
    } else if ((character_select::detail::Sub_503990(&gLegacyChatRecordsD67E80[3].label) & 0x40) != 0) {
        hint = platform::gValue55463C < 512 ? kLegacyChat58A654 : kLegacyChat58A63C;
    } else if ((character_select::detail::Sub_503990(&gLegacyChatRecordsD67E80[4].label) & 0x40) != 0) {
        hint = platform::gValue55463C > 0 ? kLegacyChat58A62C : kLegacyChat58A614;
    } else if ((character_select::detail::Sub_503990(&gLegacyChatRecordsD67E80[12].label) & 0x40) != 0) {
        hint = platform::gValue554644 != 0 ? kLegacyChat58A604 : kLegacyChat58A5F4;
    } else if ((character_select::detail::Sub_503990(&gLegacyChatRecordsD67E80[15].label) & 0x40) != 0) {
        hint = platform::gValue554640 < 5 ? kLegacyChat58A5E0 : kLegacyChat58A5C4;
    } else if ((character_select::detail::Sub_503990(&gLegacyChatRecordsD67E80[16].label) & 0x40) != 0) {
        hint = platform::gValue554640 > 1 ? kLegacyChat58A5B0 : kLegacyChat58A594;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyChatSettings585000, 1);
}

std::int32_t Sub_4FE490()
{
    return Sub_4FE4A0();
}

std::int32_t Sub_4FE4A0()
{
    const std::int32_t y = gLegacyMouseSettings585320.y;
    const std::int32_t x = gLegacyMouseSettings585320.x;
    auto& records = gLegacyMouseRecordsD68358;
    // 原132条交错存储；不清空每条记录未写的+23H字节。
    records[1].label.textY = y + 60;
    records[2].label.textY = y + 80;
    records[2].label.top = y + 80;
    records[0].label.textY = y + 40;
    records[0].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[0] = std::byte {0xFF};
    records[2].label.textX = x + 120;
    records[3].label.textY = y + 100;
    records[5].label.textX = x + 120;
    records[0].label.textX = x + 20;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyMouse58A7B0;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[1] = std::byte {0};
    records[0].hint = nullptr;
    records[1].label.textX = x + 20;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A500;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[1] = std::byte {0};
    records[1].hint = nullptr;
    records[2].label.left = x + 119;
    records[2].label.width = 54;
    records[2].label.height = 18;
    records[2].label.text = kLegacyChat58A54C;
    records[2].callback = &Sub_4FEAC0;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {1};
    records[2].reserved21[1] = std::byte {0xF9};
    records[2].hint = nullptr;
    records[3].label.textX = x + 20;
    records[3].label.left = 0;
    records[3].label.top = 0;
    records[3].label.width = 0;
    records[3].label.height = 0;
    records[3].label.text = kLegacyMouse58A7A0;
    records[3].callback = nullptr;
    records[3].color = 4;
    records[3].reserved21[0] = std::byte {0xFF};
    records[3].reserved21[1] = std::byte {0};
    records[3].hint = nullptr;
    records[4].label.textX = x + 20;
    records[4].label.textY = y + 120;
    records[4].label.left = 0;
    records[4].label.top = 0;
    records[4].label.width = 0;
    records[4].label.height = 0;
    records[4].label.text = kLegacyChat58A500;
    records[4].callback = nullptr;
    records[4].color = 0;
    records[4].reserved21[0] = std::byte {0xFF};
    records[4].reserved21[1] = std::byte {0};
    records[4].hint = nullptr;
    records[5].label.textY = y + 160;
    records[5].label.left = x + 119;
    records[5].label.top = y + 160;
    records[6].label.textX = x + 20;
    records[7].label.textX = x + 20;
    records[7].label.textY = y + 200;
    records[8].label.textX = x + 124;
    records[6].label.textY = y + 180;
    records[8].label.textY = y + 220;
    records[8].label.top = y + 220;
    records[9].label.textX = x + 156;
    records[9].label.textY = y + 220;
    records[9].label.top = y + 220;
    records[8].label.left = x + 123;
    records[5].label.height = 18;
    records[8].label.height = 18;
    records[9].label.height = 18;
    records[10].label.height = 18;
    records[5].label.width = 54;
    records[5].label.text = kLegacyChat58A54C;
    records[5].callback = &Sub_4FEFF0;
    records[5].color = 0;
    records[5].reserved21[0] = std::byte {1};
    records[5].reserved21[1] = std::byte {0xF9};
    records[5].hint = nullptr;
    records[6].label.left = 0;
    records[6].label.top = 0;
    records[6].label.width = 0;
    records[6].label.height = 0;
    records[6].label.text = kLegacyMouse58A790;
    records[6].callback = nullptr;
    records[6].color = 4;
    records[6].reserved21[0] = std::byte {0xFF};
    records[6].reserved21[1] = std::byte {0};
    records[6].hint = nullptr;
    records[7].label.left = 0;
    records[7].label.top = 0;
    records[7].label.width = 0;
    records[7].label.height = 0;
    records[7].label.text = kLegacyMouse58A784;
    records[7].callback = nullptr;
    records[7].color = 0;
    records[7].reserved21[0] = std::byte {0xFF};
    records[7].reserved21[1] = std::byte {0};
    records[7].hint = nullptr;
    records[8].label.width = 19;
    records[8].label.text = kLegacyChat58A574;
    records[8].callback = &Sub_4FE3F0;
    records[8].color = 0;
    records[8].reserved21[0] = std::byte {1};
    records[8].reserved21[1] = std::byte {0xF9};
    records[8].hint = nullptr;
    records[9].label.left = x + 155;
    records[9].label.width = 19;
    records[9].label.text = kLegacyChat58A570;
    records[9].callback = &Sub_4FE440;
    records[9].color = 0;
    records[9].reserved21[0] = std::byte {1};
    records[9].reserved21[1] = std::byte {0xF9};
    records[9].hint = nullptr;
    records[10].label.textX = x + 62;
    records[10].label.textY = y + 260;
    records[10].label.left = x + 60;
    records[10].label.top = y + 260;
    records[10].label.width = 69;
    records[10].label.text = kLegacyChat58A4F8;
    records[10].callback = &Sub_4FE3E0;
    records[10].color = 0;
    records[10].reserved21[0] = std::byte {1};
    records[10].reserved21[1] = std::byte {0xF9};
    records[10].hint = kLegacyMouse58A774;
    return 0;
}

std::uint32_t Sub_4FE860(const std::int32_t opened)
{
    const char* const cursorNames[2] {kLegacyMouse58A838, kLegacyMouse58A830};
    const char* const moveNames[2] {kLegacyMouse58A820, kLegacyMouse58A814};
    const std::int32_t state = Sub_503160(opened, &gLegacyMouseSettings585320, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        static_cast<void>(Sub_520720());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacyMouseRecordsD68358.data(), 11);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    char buffer[128];
    const std::uint32_t cursorMode = platform::gValueD67AE4;
    std::sprintf(buffer, kLegacyMouse5535F8, cursorNames[cursorMode != 0]);
    const std::int32_t cursorY = gLegacyMouseSettings585320.y;
    const std::int32_t cursorX = gLegacyMouseSettings585320.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(cursorX + 62),
        static_cast<std::int16_t>(cursorY + 60), 2, 0, 0, buffer, 0, 0));
    const std::uint32_t moveMode = platform::gValueD67AE8;
    std::sprintf(buffer, kLegacyMouse5535F8, moveNames[moveMode]);
    const std::int32_t moveY = gLegacyMouseSettings585320.y;
    const std::int32_t moveX = gLegacyMouseSettings585320.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(moveX + 31),
        static_cast<std::int16_t>(moveY + 140), 2, 0, 0, buffer, 0, 0));
    // 原54D4BC为F32 0x3DCCCCCD，不能改成double字面量0.1。
    const double speed = static_cast<double>(platform::gValue553438)
        * static_cast<double>(std::bit_cast<float>(std::uint32_t {0x3DCCCCCD}));
    std::sprintf(buffer, kLegacyMouse58A80C, speed);
    const std::int32_t speedY = gLegacyMouseSettings585320.y;
    const std::int32_t speedX = gLegacyMouseSettings585320.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(speedX + 100),
        static_cast<std::int16_t>(speedY + 200), 2, 0, 0, buffer, 0, 0));
    Sub_503120(gLegacyMouseRecordsD68358.data(), 11);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMouseSettings585320) != 0) {
        hint = kLegacySkillClose55331C;
    } else if ((character_select::detail::Sub_503990(&gLegacyMouseRecordsD68358[2].label) & 0x40) != 0) {
        hint = platform::gValueD67AE4 != 0 ? kLegacyMouse58A7FC : kLegacyMouse58A7EC;
    } else if ((character_select::detail::Sub_503990(&gLegacyMouseRecordsD68358[5].label) & 0x40) != 0) {
        hint = platform::gValueD67AE8 != 0 ? kLegacyMouse58A7D4 : kLegacyMouse58A7C0;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMouseSettings585320, 1);
}

std::int32_t Sub_4FEB00()
{
    return Sub_4FEB10();
}

std::int32_t Sub_4FEB10()
{
    const std::int32_t x = gLegacyHotkeyHelp5853E8.x;
    const std::int32_t y = gLegacyHotkeyHelp5853E8.y;
    auto& records = gLegacyHotkeyRecordsD68510;
    // 原156条字段写序，说明行不添加命中区域或回调。
    records[1].label.textX = x + 140;
    records[3].label.textX = x + 140;
    records[3].label.textY = y + 90;
    records[4].label.textY = y + 108;
    records[0].label.textY = y + 40;
    records[1].label.textY = y + 57;
    records[5].label.textX = x + 130;
    records[0].label.textX = x + 20;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyHotkey58A8AC;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[0] = std::byte {0xFF};
    records[0].reserved21[1] = std::byte {0};
    records[0].hint = nullptr;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyHotkey58A8A8;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[1] = std::byte {0};
    records[1].hint = nullptr;
    records[2].label.textX = x + 20;
    records[2].label.textY = y + 74;
    records[2].label.left = 0;
    records[2].label.top = 0;
    records[2].label.width = 0;
    records[2].label.height = 0;
    records[2].label.text = kLegacyHotkey58A89C;
    records[2].callback = nullptr;
    records[2].color = 4;
    records[2].reserved21[0] = std::byte {0xFF};
    records[2].reserved21[1] = std::byte {0};
    records[2].hint = nullptr;
    records[3].label.left = 0;
    records[3].label.top = 0;
    records[3].label.width = 0;
    records[3].label.height = 0;
    records[3].label.text = kLegacyHotkey58A898;
    records[3].callback = nullptr;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {0xFF};
    records[3].reserved21[1] = std::byte {0};
    records[3].hint = nullptr;
    records[4].label.textX = x + 20;
    records[4].label.left = 0;
    records[4].label.top = 0;
    records[4].label.width = 0;
    records[4].label.height = 0;
    records[4].label.text = kLegacyHotkey58A884;
    records[4].callback = nullptr;
    records[4].color = 4;
    records[4].reserved21[0] = std::byte {0xFF};
    records[4].reserved21[1] = std::byte {0};
    records[4].hint = nullptr;
    records[5].label.textY = y + 125;
    records[5].label.left = 0;
    records[5].label.top = 0;
    records[6].label.textX = x + 20;
    records[6].label.textY = y + 142;
    records[7].label.textX = x + 58;
    records[7].label.textY = y + 159;
    records[8].label.textX = x + 20;
    records[10].label.textX = x + 20;
    records[8].label.textY = y + 176;
    records[10].label.textY = y + 210;
    records[9].label.textX = x + 147;
    records[11].label.textX = x + 138;
    records[5].label.width = 0;
    records[5].label.height = 0;
    records[5].label.text = kLegacyHotkey58A87C;
    records[5].callback = nullptr;
    records[5].color = 0;
    records[5].reserved21[0] = std::byte {0xFF};
    records[5].reserved21[1] = std::byte {0};
    records[5].hint = nullptr;
    records[6].label.left = 0;
    records[6].label.top = 0;
    records[6].label.width = 0;
    records[6].label.height = 0;
    records[6].label.text = kLegacyHotkey58A86C;
    records[6].callback = nullptr;
    records[6].color = 4;
    records[6].reserved21[0] = std::byte {0xFF};
    records[6].reserved21[1] = std::byte {0};
    records[6].hint = nullptr;
    records[7].label.left = 0;
    records[7].label.top = 0;
    records[7].label.width = 0;
    records[7].label.height = 0;
    records[7].label.text = kLegacyHotkey58A85C;
    records[7].callback = nullptr;
    records[7].color = 0;
    records[7].reserved21[0] = std::byte {0xFF};
    records[7].reserved21[1] = std::byte {0};
    records[7].hint = nullptr;
    records[8].label.left = 0;
    records[8].label.top = 0;
    records[8].label.width = 0;
    records[8].label.height = 0;
    records[8].label.text = kLegacyHotkey58A850;
    records[8].callback = nullptr;
    records[8].color = 4;
    records[8].reserved21[0] = std::byte {0xFF};
    records[8].reserved21[1] = std::byte {0};
    records[8].hint = nullptr;
    records[9].label.textY = y + 193;
    records[9].label.left = 0;
    records[9].label.top = 0;
    records[9].label.width = 0;
    records[9].label.height = 0;
    records[9].label.text = kLegacyHotkey58A84C;
    records[9].callback = nullptr;
    records[9].color = 0;
    records[9].reserved21[0] = std::byte {0xFF};
    records[9].reserved21[1] = std::byte {0};
    records[9].hint = nullptr;
    records[10].label.left = 0;
    records[10].label.top = 0;
    records[10].label.width = 0;
    records[10].label.height = 0;
    records[10].label.text = kLegacyHotkey58A840;
    records[10].callback = nullptr;
    records[10].color = 4;
    records[10].reserved21[0] = std::byte {0xFF};
    records[10].reserved21[1] = std::byte {0};
    records[10].hint = nullptr;
    records[11].label.textY = y + 227;
    records[11].label.left = 0;
    records[11].label.top = 0;
    records[12].label.textX = x + 62;
    records[11].reserved21[0] = std::byte {0xFF};
    records[11].label.width = 0;
    records[11].label.height = 0;
    records[11].label.text = kLegacyHotkey553F00;
    records[11].callback = nullptr;
    records[11].color = 0;
    records[11].reserved21[1] = std::byte {0};
    records[11].hint = nullptr;
    records[12].label.textY = y + 260;
    records[12].label.left = x + 60;
    records[12].label.top = y + 260;
    records[12].label.width = 69;
    records[12].label.height = 18;
    records[12].label.text = kLegacyChat58A4F8;
    records[12].callback = &Sub_4FE3E0;
    records[12].color = 0;
    records[12].reserved21[0] = std::byte {1};
    records[12].reserved21[1] = std::byte {0xF9};
    records[12].hint = kLegacyChat58A4E8;
    return 0;
}

std::uint32_t Sub_4FEF30(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyHotkeyHelp5853E8, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacyHotkeyRecordsD68510.data(), 13);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    Sub_503120(gLegacyHotkeyRecordsD68510.data(), 13);
    if (Sub_503590(&gLegacyHotkeyHelp5853E8) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    }
    return Sub_503440(&gLegacyHotkeyHelp5853E8, 1);
}

std::int32_t Sub_4FF020()
{
    return Sub_4FF030();
}

std::int32_t Sub_4FF030()
{
    const std::int32_t x = gLegacyScanlineSettings5854B0.x;
    const std::int32_t y = gLegacyScanlineSettings5854B0.y;
    auto& records = gLegacyScanlineRecordsD68718;
    records[0].label.textX = x + 20;
    records[1].label.textX = x + 20;
    records[1].label.textY = y + 60;
    records[2].label.textX = x + 120;
    records[2].label.left = x + 119;
    records[2].label.textY = y + 100;
    records[2].label.top = y + 100;
    records[0].label.textY = y + 40;
    records[3].label.textX = x + 62;
    records[2].label.height = 18;
    records[3].label.height = 18;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyScanline586E24;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[0] = std::byte {0xFF};
    records[0].reserved21[1] = std::byte {0};
    records[0].hint = nullptr;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A500;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[1] = std::byte {0};
    records[1].hint = nullptr;
    records[2].label.width = 54;
    records[2].label.text = kLegacyChat58A54C;
    records[2].callback = &Sub_4FF310;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {1};
    records[2].reserved21[1] = std::byte {0xF9};
    records[2].hint = nullptr;
    records[3].label.textY = y + 260;
    records[3].label.left = x + 60;
    records[3].label.top = y + 260;
    records[3].label.width = 69;
    records[3].label.text = kLegacyChat58A4F8;
    records[3].callback = &Sub_4FE3E0;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {1};
    records[3].reserved21[1] = std::byte {0xF9};
    records[3].hint = kLegacyChat58A4E8;
    return 0;
}

std::uint32_t Sub_4FF1A0(const std::int32_t opened)
{
    const char* const modeNames[2] {kLegacyScanline58A8F8, kLegacyScanline58A8E8};
    const std::int32_t state = Sub_503160(opened, &gLegacyScanlineSettings5854B0, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacyScanlineRecordsD68718.data(), 4);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    char buffer[128];
    const std::int32_t mode = std::bit_cast<std::int16_t>(platform::gScanlineSyncMode);
    std::sprintf(buffer, kLegacyMouse5535F8, modeNames[mode]);
    const std::int32_t y = gLegacyScanlineSettings5854B0.y;
    const std::int32_t x = gLegacyScanlineSettings5854B0.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 31),
        static_cast<std::int16_t>(y + 80), 2, 0, 0, buffer, 0, 0));
    Sub_503120(gLegacyScanlineRecordsD68718.data(), 4);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyScanlineSettings5854B0) != 0) {
        hint = kLegacySkillClose55331C;
    } else if ((character_select::detail::Sub_503990(&gLegacyScanlineRecordsD68718[2].label) & 0x40) != 0) {
        hint = platform::gScanlineSyncMode != 0 ? kLegacyScanline58A8D4 : kLegacyScanline58A8BC;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyScanlineSettings5854B0, 1);
}

std::int32_t Sub_4FF340()
{
    return Sub_4FF350();
}

std::int32_t Sub_4FF350()
{
    const std::int32_t x = gLegacyButtonSettings585578.x;
    const std::int32_t y = gLegacyButtonSettings585578.y;
    auto& records = gLegacyButtonRecordsD687B8;
    records[0].label.textX = x + 20;
    records[1].label.textX = x + 20;
    records[1].label.textY = y + 60;
    records[2].label.textX = x + 120;
    records[2].label.left = x + 119;
    records[2].label.textY = y + 100;
    records[2].label.top = y + 100;
    records[0].label.textY = y + 40;
    records[3].label.textX = x + 62;
    records[2].label.height = 18;
    records[3].label.height = 18;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyButton58A914;
    records[0].callback = nullptr;
    records[0].color = 0;
    records[0].reserved21[0] = std::byte {0xFF};
    records[0].reserved21[1] = std::byte {0};
    records[0].hint = nullptr;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A500;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[1] = std::byte {0};
    records[1].hint = nullptr;
    records[2].label.width = 54;
    records[2].label.text = kLegacyChat58A54C;
    records[2].callback = &Sub_4FF620;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {1};
    records[2].reserved21[1] = std::byte {0xF9};
    records[2].hint = nullptr;
    records[3].label.textY = y + 260;
    records[3].label.left = x + 60;
    records[3].label.top = y + 260;
    records[3].label.width = 69;
    records[3].label.text = kLegacyButton58A4BC;
    records[3].callback = &Sub_4FE3E0;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {1};
    records[3].reserved21[1] = std::byte {0xF9};
    records[3].hint = kLegacyButton58A904;
    return 0;
}

std::uint32_t Sub_4FF4C0(const std::int32_t opened)
{
    const char* const modeNames[2] {kLegacyButton58A97C, kLegacyButton58A970};
    const std::int32_t state = Sub_503160(opened, &gLegacyButtonSettings585578, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(Sub_520470());
        static_cast<void>(Sub_520590());
        const std::int32_t selected = gSelectedCharacterIndex;
        static_cast<void>(Sub_520510(selected));
        static_cast<void>(Sub_520640());
        static_cast<void>(Sub_5206F0());
        return static_cast<std::uint32_t>(Sub_520200());
    }
    const std::int32_t interaction = Sub_5030A0(gLegacyButtonRecordsD687B8.data(), 4);
    if (interaction != 0) {
        return static_cast<std::uint32_t>(interaction);
    }
    const std::uint32_t mode = platform::gValueD68858;
    const std::int32_t y = gLegacyButtonSettings585578.y;
    const char* const text = modeNames[mode];
    const std::int32_t width = TextMeasure(text, 0);
    const std::int32_t x = gLegacyButtonSettings585578.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width + 172),
        static_cast<std::int16_t>(y + 80), 2, 0, 4, text, 0, 0));
    Sub_503120(gLegacyButtonRecordsD687B8.data(), 4);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyButtonSettings585578) != 0) {
        hint = kLegacySkillClose55331C;
    } else if ((character_select::detail::Sub_503990(&gLegacyButtonRecordsD687B8[2].label) & 0x40) != 0) {
        hint = platform::gValueD68858 == 0 ? kLegacyButton58A948 : kLegacyButton58A920;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyButtonSettings585578, 1);
}

std::int32_t Sub_502930()
{
    return Sub_502940();
}

std::int32_t Sub_502940()
{
    const std::int32_t x = gLegacyMapDefaultX57CB24;
    const std::int32_t y = gLegacyMapDefaultYD67AD4;
    gLegacyMapPanelD68868.backgrounds[1] = 0;
    gLegacyMapPanelD68868.x = x;
    gLegacyMapPanelD68868.backgrounds[2] = 0;
    gLegacyMapPanelD68868.y = y;
    gLegacyMapPanelD68868.backgrounds[3] = 0;
    gLegacyMapPanelD68868.width = 248;
    gLegacyMapPanelD68868.height = 220;
    gLegacyMapPanelD68868.id = 1;
    gLegacyMapPanelD68868.backgrounds[0] = 22535;
    gLegacyMapPanelD68868.backgrounds[4] = 0;
    // 原尾部156B含页签指针；按原生字段清空，不能沿用x86字节数。
    gLegacyMapPanelD68868.tabs = {};
    gLegacyMapPanelD68868.selected = 0;
    gLegacyMapPanelD68868.hovered = 0;
    gLegacyMapPanelD68868.reservedA2 = {};
    gLegacyMapPanelD68868.closeButton = {};
    return 0;
}

std::uint32_t Sub_5029B0(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyMapPanelD68868, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    const std::int32_t nameWidth = TextMeasure(world::ReadText9DAD30(), 0);
    const std::int32_t nameY = gLegacyMapPanelD68868.y;
    const std::int32_t nameX = gLegacyMapPanelD68868.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameX - nameWidth / 2 + 124),
        static_cast<std::int16_t>(nameY + 36), 2, 0, 0, world::ReadText9DAD30(), 0, 0));
    char buffer[256];
    const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    std::sprintf(buffer, kLegacyMapEast58AA54, playerX);
    const std::int32_t eastY = gLegacyMapPanelD68868.y;
    const std::int32_t eastX = gLegacyMapPanelD68868.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(eastX + 62),
        static_cast<std::int16_t>(eastY + 186), 2, 0, 0, buffer, 0, 0));
    const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    std::sprintf(buffer, kLegacyMapSouth58AA4C, playerY);
    const std::int32_t southY = gLegacyMapPanelD68868.y;
    const std::int32_t southX = gLegacyMapPanelD68868.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(southX + 133),
        static_cast<std::int16_t>(southY + 186), 2, 0, 0, buffer, 0, 0));
    if (Sub_503590(&gLegacyMapPanelD68868) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    }
    const std::int32_t width = gLegacyMapPanelD68868.width;
    const std::int32_t x = gLegacyMapPanelD68868.x;
    gLegacyMapPanelD68868.closeButton.normalImage = 22509;
    gLegacyMapPanelD68868.closeButton.imageX = width + x - 16;
    const std::int32_t y = gLegacyMapPanelD68868.y;
    gLegacyMapPanelD68868.closeButton.left = width + x - 26;
    gLegacyMapPanelD68868.closeButton.top = y + 6;
    gLegacyMapPanelD68868.closeButton.imageY = y + 16;
    gLegacyMapPanelD68868.closeButton.width = 20;
    gLegacyMapPanelD68868.closeButton.height = 20;
    gLegacyMapPanelD68868.closeButton.pressedImage = 22510;
    static_cast<void>(render::QueueMap_521AA0(x, y, 212));
    static_cast<void>(Sub_503440(&gLegacyMapPanelD68868, 1));
    const std::uint32_t hitId = gLegacyPanelHitIdD046C0;
    if (hitId == 1) {
        world::detail::gValueC8A780 = 1;
        world::detail::gValueC8A77C = 1;
    }
    return hitId;
}

std::int32_t Sub_502B80(std::int32_t* const value, const std::int32_t digits)
{
    if (digits <= 0) {
        return digits;
    }
    std::int32_t divisor = 1;
    for (std::int32_t remaining = digits - 1; remaining > 0; --remaining) {
        divisor *= 10;
    }
    const std::int32_t quotient = *value / divisor;
    if (quotient >= 10) {
        return quotient;
    }
    const std::int32_t backspace = platform::ReadKeyFlag4_4597A0(8);
    if (backspace != 0) {
        const std::int32_t current = *value;
        if (current < 10) {
            *value = 0;
            return backspace;
        }
        *value = current / 10;
        return 0; // 原502BCE的EAX取非负商符号位，不返回新数量。
    }
    if ((platform::gKeyStates[17] & 2u) != 0) {
        return 0;
    }
    for (std::int32_t digit = 0; digit < 10; ++digit) {
        if (platform::ReadKeyFlag4_4597A0(digit + 48) != 0
            || platform::ReadKeyFlag4_4597A0(digit + 96) != 0) {
            const std::int32_t result = *value * 10 + digit;
            *value = result;
            return result;
        }
    }
    return 0;
}

std::int32_t Sub_502C30()
{
    return Sub_502C40();
}

std::int32_t Sub_502C40()
{
    const std::int32_t height = gLegacyBottomHeight5858A4;
    const std::int32_t y = 480 - height;
    gLegacyBottomYCB1DD8 = y;
    return y;
}

std::int32_t Sub_502C60()
{
    return Sub_502C70();
}

std::int32_t Sub_502C70()
{
    const std::int32_t height = gLegacyBottomHeight5858A4;
    gLegacyBottomHeightCA0014 = height;
    return height;
}

std::int32_t Sub_502C80()
{
    return Sub_502C90();
}

std::int32_t Sub_502C90()
{
    const std::int32_t height = gLegacyBottomHeightCA0014;
    gLegacyBottomHeightD2DAB4 = height;
    return height;
}

std::int32_t Sub_502CA0()
{
    return Sub_502CB0();
}

std::int32_t Sub_502CB0()
{
    const std::int32_t width = gLegacyBottomWidth5858A0;
    const std::int32_t x = gLegacyBottomXD68930;
    auto& record = gLegacyBottomInitWordsD68938[0];
    record[2] = x;
    record[4] = 80;
    record[0] = width / 2 + x;
    const std::int32_t height = gLegacyBottomHeight5858A4;
    record[5] = 24;
    const std::int32_t y = gLegacyBottomYCB1DD8;
    record[3] = y;
    record[1] = height / 2 + y;
    record[6] = 0;
    record[7] = 22551;
    record[8] = 0;
    return 0;
}

std::int32_t Sub_502D20()
{
    return Sub_502D30();
}

std::int32_t Sub_502D30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[1], 80, 22552);
}

std::int32_t Sub_502DA0()
{
    return Sub_502DB0();
}

std::int32_t Sub_502DB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[2], 160, 22553);
}

std::int32_t Sub_502E20()
{
    return Sub_502E30();
}

std::int32_t Sub_502E30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[3], 240, 22554);
}

std::int32_t Sub_502EA0()
{
    return Sub_502EB0();
}

std::int32_t Sub_502EB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[4], 320, 22555);
}

std::int32_t Sub_502F20()
{
    return Sub_502F30();
}

std::int32_t Sub_502F30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[5], 400, 22556);
}

std::int32_t Sub_502FA0()
{
    return Sub_502FB0();
}

std::int32_t Sub_502FB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[6], 480, 22557);
}

std::int32_t Sub_503020()
{
    return Sub_503030();
}

std::int32_t Sub_503030()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[7], 560, 22558);
}

std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        LegacyPanelTextButtonRecord* const record = &records[index];
        const std::int32_t state = character_select::detail::Sub_503990(&record->label);
        if ((state & 0x40) != 0) {
            const char* const hint = record->hint;
            if (hint != nullptr) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
            }
        }
        if ((state & 0x10) != 0) {
            auto const callback = record->callback;
            if (callback != nullptr && callback() != 0) {
                return 1;
            }
        }
    }
    return 0;
}

void Sub_503120(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        const std::uint32_t hoverImage = gValueD69768;
        const std::int8_t color = std::bit_cast<std::int8_t>(records[index].color);
        static_cast<void>(character_select::detail::Sub_503A20(
            &records[index].label, 2, color, 1, hoverImage, 214));
    }
}

std::int32_t Sub_503160(
    const std::int32_t opened, LegacyPanelRecord* const panel,
    void (*const initialize)(), const std::int32_t flags)
{
    const std::int32_t right = panel->x + panel->width;
    const std::int32_t top = panel->y;
    panel->closeButton.normalImage = 22509;
    panel->closeButton.pressedImage = 22510;
    panel->closeButton.left = right - 26;
    panel->closeButton.imageX = right - 16;
    panel->closeButton.width = 20;
    panel->closeButton.height = 20;
    const std::uint32_t id = panel->id;
    panel->closeButton.imageY = top + 16;
    panel->closeButton.top = top + 6;
    const std::int32_t index = audio::Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    YobiAction*& actionSlot = audio::BorrowLegacyPanelAction57C374(index);
    if (opened == 0) {
        if (actionSlot != nullptr) {
            static_cast<void>(MarkDeath(actionSlot));
            actionSlot = nullptr;
        }
        std::uint8_t& dirty = audio::BorrowLegacyPanelDirty57C378(index);
        if (dirty != 0) {
            dirty = 0;
            return 2;
        }
        return 0;
    }
    if (actionSlot == nullptr) {
        const std::int32_t background = panel->backgrounds[0];
        std::int32_t x;
        std::int32_t y;
        std::int32_t width;
        const std::int32_t height = panel->height;
        const std::int32_t style = static_cast<std::uint32_t>(background) <= 999 ? background : -2;
        if (static_cast<std::uint32_t>(background) > 999) {
            width = panel->width;
            y = panel->y;
            x = panel->x;
        } else {
            y = panel->y;
            width = panel->width;
            x = panel->x;
        }
        actionSlot = Sub_5044E0(x, y, width, height, style, 1);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        if (initialize != nullptr) {
            initialize();
        }
    }
    YobiAction* const action = actionSlot;
    if (action == nullptr || IsYobiActionStateLessOrEqual(action, 0)) {
        return 0;
    }
    const std::int32_t firstTabImage = panel->tabs[0].image;
    panel->hovered = -1;
    if (firstTabImage != 0) {
        for (std::int32_t tabIndex = 0; tabIndex < 5; ++tabIndex) {
            if (panel->selected == tabIndex) {
                continue;
            }
            LegacyPanelTabRecord* const tab = &panel->tabs[tabIndex];
            const std::int32_t tabY = tab->y;
            const std::int32_t tabHeight = tab->height;
            const std::int32_t panelY = panel->y;
            const std::int32_t tabX = tab->x;
            const std::int32_t panelX = panel->x;
            const std::int32_t bottom = panelY + tabY + tabHeight;
            const std::int32_t tabRight = panelX + tabX + tab->width;
            if (Sub_5049A0(panelX + tabX, panelY + tabY, tabRight, bottom, -1) != 0
                && tab->image != 0) {
                panel->hovered = static_cast<std::int8_t>(tabIndex);
                const char* const hint = tab->hint;
                if (hint != nullptr) {
                    char buffer[128];
                    std::sprintf(buffer, "%s", hint);
                    std::strcpy(ui::detail::gHoverTextD68AE8.data(), buffer);
                }
                if ((gValueD68D9C & 1u) != 0) {
                    panel->selected = static_cast<std::int8_t>(tabIndex);
                    static_cast<void>(audio::PlaySe_52C450(52, 320));
                    break;
                }
            }
        }
    }
    const std::int32_t panelY = panel->y;
    const std::int32_t panelHeight = panel->height;
    const std::int32_t panelX = panel->x;
    const std::int32_t bottom = panelHeight + panelY + 1;
    const std::int32_t panelRight = panelX + panel->width + 1;
    if (Sub_5049A0(panelX, panelY, panelRight, bottom, -1) != 0
        && (QueryPanelButtonState(&panel->closeButton) & 0x40) == 0) {
        gLegacyPanelHitIdD046C0 = panel->id;
    }
    if ((flags & 1) != 0 && (QueryPanelButtonState(&panel->closeButton) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(panel->id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

std::uint32_t Sub_503440(LegacyPanelRecord* const panel, const std::int32_t flags)
{
    const std::int32_t hovered = panel->hovered;
    GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (hovered >= 0) {
        const std::uint32_t hoverImage = gValueD69768;
        const std::int32_t panelX = panel->x;
        const LegacyPanelTabRecord* const tab = &panel->tabs[hovered];
        const std::int32_t tabY = tab->y;
        const std::int32_t panelY = panel->y;
        const std::int32_t tabX = tab->x;
        const std::int32_t tabHeight = tab->height;
        const std::int32_t tabWidth = tab->width;
        DrawGraphicHover(
            static_cast<std::uint16_t>(panelX + tabX - 3),
            static_cast<std::uint16_t>(panelY + tabY - 3),
            static_cast<std::uint16_t>(panelX + tabWidth + tabX + 4),
            static_cast<std::uint16_t>(panelY + tabY + tabHeight + 3),
            214, hoverImage, 0);
    }
    for (std::int32_t index = 0; index < 5; ++index) {
        if (panel->selected != index) {
            const std::int32_t image = panel->tabs[index].image;
            const std::int32_t height = panel->height;
            const std::int32_t top = panel->y;
            const std::int32_t y = height / 2 + top;
            const std::int32_t width = panel->width;
            const std::int32_t left = panel->x;
            const std::int32_t x = width / 2 + left;
            static_cast<void>(DrawGraphic(x, y, 218, image, 0, &descriptor));
        }
    }
    if ((flags & 1) != 0) {
        const std::uint32_t hoverImage = gValueD69768;
        Sub_5035E0(&panel->closeButton, 218, 1, hoverImage, 0);
    }
    const std::int32_t selected = panel->selected;
    const std::int32_t image = panel->backgrounds[selected];
    if (static_cast<std::uint32_t>(image) <= 999) {
        return static_cast<std::uint32_t>(image);
    }
    const std::int32_t height = panel->height;
    const std::int32_t top = panel->y;
    const std::int32_t y = height / 2 + top;
    const std::int32_t width = panel->width;
    const std::int32_t left = panel->x;
    const std::int32_t x = width / 2 + left;
    return static_cast<std::uint32_t>(DrawGraphic(x, y, 212, image, 1, &descriptor));
}

std::int32_t Sub_503590(LegacyPanelRecord* const panel)
{
    const std::int32_t y = panel->y;
    const std::int32_t height = panel->height;
    const std::int32_t x = panel->x;
    const std::int32_t bottom = height + y + 1;
    const std::int32_t right = panel->width + x + 1;
    if (Sub_5049A0(x, y, right, bottom, -1) != 0
        && (QueryPanelButtonState(&panel->closeButton) & 0x40) != 0) {
        return 1;
    }
    return 0;
}

void Sub_5035E0(
    LegacyPanelButtonRecord* const button, const std::uint8_t layer,
    const std::int32_t hoverMode, const std::uint32_t hoverImage, const std::int32_t style)
{
    GraphicDescriptor16 descriptor {};
    const std::int32_t top = button->top;
    const std::int32_t height = button->height;
    const std::int32_t left = button->left;
    const std::int32_t bottom = top + height;
    const std::int32_t right = left + button->width;
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const bool pressed = Sub_5049A0(left, top, right, bottom, -1) != 0
        && (gValueD68D98 & 1u) != 0 && style == 0;
    const std::int32_t image = pressed ? button->pressedImage : button->normalImage;
    const std::int32_t y = button->imageY;
    const std::int32_t x = button->imageX;
    static_cast<void>(DrawGraphic(x, y, layer, image, 1, &descriptor));
    if (hoverMode != 0) {
        const std::int32_t hoverTop = button->top;
        const std::int32_t hoverLeft = button->left;
        const std::int32_t hoverBottom = hoverTop + button->height;
        const std::int32_t hoverRight = button->width + hoverLeft;
        if (Sub_5049A0(hoverLeft, hoverTop, hoverRight, hoverBottom, -1) != 0 && style == 0) {
            const std::int32_t frameTop = button->top;
            const std::int32_t frameLeft = button->left;
            const std::int32_t frameBottom = frameTop + button->height + 2;
            const std::int32_t frameRight = button->width + frameLeft + 2;
            DrawGraphicHover(
                static_cast<std::uint16_t>(frameLeft - 2),
                static_cast<std::uint16_t>(frameTop - 2),
                static_cast<std::uint16_t>(frameRight),
                static_cast<std::uint16_t>(frameBottom),
                layer, hoverImage, hoverMode == 2 ? 1 : 0);
        }
    }
}

void Sub_5038B0(
    LegacyPanelButtonRecord* const button, const std::uint8_t layer,
    const std::int32_t hoverMode, const std::uint32_t hoverImage, const std::int32_t selected)
{
    GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const std::int32_t image = selected != 0 ? button->pressedImage : button->normalImage;
    const std::int32_t y = button->imageY;
    const std::int32_t x = button->imageX;
    static_cast<void>(DrawGraphic(x, y, layer, image, 1, &descriptor));
    if (hoverMode != 0) {
        const std::int32_t top = button->top;
        const std::int32_t height = button->height;
        const std::int32_t left = button->left;
        const std::int32_t width = button->width;
        if (Sub_5049A0(left, top, left + width, top + height, -1) != 0) {
            const std::int32_t frameTop = button->top;
            const std::int32_t frameLeft = button->left;
            const std::int32_t frameHeight = button->height;
            const std::int32_t frameWidth = button->width;
            DrawGraphicHover(
                static_cast<std::uint16_t>(frameLeft - 2),
                static_cast<std::uint16_t>(frameTop - 2),
                static_cast<std::uint16_t>(frameLeft + frameWidth + 2),
                static_cast<std::uint16_t>(frameTop + frameHeight + 2),
                layer, hoverImage, hoverMode == 2 ? 1 : 0);
        }
    }
}

std::int32_t Sub_45F100(const void* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

void Sub_45EFD0(
    const void* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style)
{
    DrawLegacyUiRecord_45EFD0(
        uiRecord,
        layer,
        overlayMode,
        overlayImageNo,
        style,
        Sub_45F100,
        Sub_45F090);
}

} // namespace cg::login::detail

namespace cg::character_select::detail {

std::int32_t Sub_45F100(
    const CharacterSelectUiRecord* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

void Sub_45EFD0(
    const CharacterSelectUiRecord* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style)
{
    DrawLegacyUiRecord_45EFD0(
        uiRecord,
        layer,
        overlayMode,
        overlayImageNo,
        style,
        QueryCharacterSelectState,
        Sub_45F090);
}

} // namespace cg::character_select::detail
