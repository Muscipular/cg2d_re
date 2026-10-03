#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"
#include "LegacyPanelRuntime.hpp"
#include "LoginSceneDependencies.hpp"
#include "platform/MainLoopDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "ui/WindowRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LoginState.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/DisplaySettings.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/Input.hpp"
#include "cg/Blacklist.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
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
std::int32_t gLegacyPetSkillCatalogPageD45F74 = 0; // 原虚拟零初值；旧58翻页写入，旧59/60/61共用。
std::int32_t gLegacyPetSkillCatalogSelectionC8AF70 = 0;
std::int32_t gLegacyPetSkillCatalogMessageD2E818 = 0;
std::int32_t gLegacyPetSkillPetSelectionC8A94C = 0;
std::int32_t gLegacyTradeTargetPageCC3208 = 0; // 原虚拟零初值；旧51翻页发布，不在开页时重置。

namespace {

// 原旧53/54共享状态；虚拟零仅承载映像初值，布局/文本/输入生产仍待。
LegacyPanelRecord gLegacyDialogPanelD2D9C8 {};
LegacyPanelRecord gLegacyLineDialogPanelCC4150 {};
std::int32_t gLegacyDialogLineCountCA043C = 0;
std::int32_t gLegacyDialogOffsetXCAFEAC = 0;
std::int32_t gLegacyDialogOffsetYCAFEA8 = 0;
std::int32_t gLegacyDialogColumnsCA0010 = 0;
std::int32_t gLegacyDialogFirstLineD673D4 = 0;
input::InputContext* gLegacyDialogInputD67C4C = nullptr;
constexpr char kLegacyDialogButton58599C[] = "\x20\xBD\x54\xA9\x77\x20";
constexpr char kLegacyDialogButton585994[] = "\x20\xA8\xFA\xAE\xF8\x20";
constexpr char kLegacyDialogButton58598C[] = "\x20\x20\xAC\x4F\x20\x20";
constexpr char kLegacyDialogButton585984[] = "\x20\x20\xA7\x5F\x20\x20";
constexpr char kLegacyDialogButton58597C[] = "\xA4\x57\xA4\x40\xAD\xB6";
constexpr char kLegacyDialogButton585974[] = "\xA4\x55\xA4\x40\xAD\xB6";
std::array<character_select::detail::CharacterModalUiRecord, 6> gLegacyDialogButtons581808 {{
    {0, 0, 0, 0, 54, 16, kLegacyDialogButton58599C},
    {0, 0, 0, 0, 54, 16, kLegacyDialogButton585994},
    {0, 0, 0, 0, 51, 16, kLegacyDialogButton58598C},
    {0, 0, 0, 0, 51, 16, kLegacyDialogButton585984},
    {0, 0, 0, 0, 52, 16, kLegacyDialogButton58597C},
    {0, 0, 0, 0, 52, 16, kLegacyDialogButton585974},
}};

LegacyPanelRecord gLegacySkillChoicePanel581D60 {
    0, 0, 292, 260, 55, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySkillChoiceActionD67C58 = nullptr; // 原旧55/56/57同源借用，关闭标记死亡后清槽。
constexpr char kLegacySkillChoiceLearn585AC4[] = "\xB7\x51\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0";
constexpr char kLegacySkillChoiceForget588BC8[] = "\xB7\x51\xA7\xD1\xB0\x4F\xA7\xDE\xAF\xE0";
constexpr char kLegacySkillChoiceCancel588BBC[] = "\xC1\xD9\xAC\x4F\xBA\xE2\xA4\x46";
constexpr char kLegacySkillChoiceLearnHint588BA8[] = "\xA6\x56\xA6\xB9\xA4\x48\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacySkillChoiceForgetHint588B90[] = "\xA7\xD1\xB0\x4F\xA6\xDB\xA4\x76\xA9\xD2\xAB\xF9\xA6\xB3\xAA\xBA\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacySkillChoiceCancelHint588B80[] = "\xB0\xB1\xA4\xEE\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";

LegacyPanelRecord gLegacySkillLearningPanel581E28 {
    0, 0, 576, 432, 56, {23301, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacySkillLearningAccept588C20[] = "\x20\xBE\xC7\x20\x20\xB2\xDF";
constexpr char kLegacySkillLearningCancel588C18[] = "\x20\xA4\xA3\x20\x20\xBE\xC7";
constexpr char kLegacySkillLearningSlots588C00[] = "\xA8\xCF\xA5\xCE\xA7\xDE\xAF\xE0\xC4\xE6\xA6\xEC\xBC\xC6\xA1\x47\x25\x64\x20\xC4\xE6";
constexpr char kLegacySkillLearningCost588BEC[] = "\xBE\xC7\xB2\xDF\xB6\x4F\xA5\xCE\xA1\x47\x25\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacySkillLearningAcceptHint588BE0[] = "\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacySkillLearningCancelHint588BD4[] = "\xB0\xB1\xA4\xEE\xBE\xC7\xB2\xDF\xA1\x43";

LegacyPanelRecord gLegacySkillForgetPanel581EF0 {
    0, 0, 576, 432, 57, {23300, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacySkillForgetCancel588CBC[] = "\xC1\xD9\xAC\x4F\xA4\xA3\xAD\x6E\xA7\xD1\xB0\x4F\xA6\x6E\xA4\x46";
constexpr char kLegacySkillForgetNotes588CA4[] = "\xA1\xB0\xA1\xB0\xA1\xB0\x20\x20\xAA\x60\xB7\x4E\x20\x20\xA1\xB0\xA1\xB0\xA1\xB0";
constexpr char kLegacySkillForgetWarning588C94[] = "\xA7\xDE\xAF\xE0\xA5\x75\xAD\x6E\xA4\x40\xA7\xD1\xB0\x4F";
constexpr char kLegacySkillForgetNoRestore588C88[] = "\xB4\x4E\xB5\x4C\xAA\x6B\xB4\x5F\xAD\xEC";
constexpr char kLegacySkillForgetCareful588C7C[] = "\xBD\xD0\xAA\x60\xB7\x4E\xA1\x43";
constexpr char kLegacySkillForgetSelect588C68[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xA7\xD1\xB0\x4F\xAA\xBA\xA7\xDE\xAF\xE0";
constexpr char kLegacySkillForgetTotal588C48[] = "\xB2\x7B\xA6\x62\xA7\xDE\xAF\xE0\xC4\xE6\xA8\xCF\xA5\xCE\xB6\x71\xAA\xBA\xA6\x58\xAD\x70\xA1\x47\x25\x32\x64\x2F\x31\x30";
constexpr char kLegacySkillForgetCancelHint588C38[] = "\xB0\xB1\xA4\xEE\xA7\xD1\xB0\x4F\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacySkillForgetRowHint588C28[] = "\xA7\xD1\xB0\x4F\xA6\xB9\xA7\xDE\xAF\xE0\xA1\x43";

LegacyPanelRecord gLegacySkillStatusPanel583C90 {
    64, 108, 512, 240, 36, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacySkillStatusPrompt589B58[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xA8\xCF\xA5\xCE\xB9\xEF\xB6\x48\xA1\x43";
constexpr char kLegacySkillStatusHeader589B2C[] = "\xB0\xB7\xB1\x64\x20\x20\xB5\xA5\xAF\xC5\x20\xA9\x6D\xA6\x57\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xA5\xCD\xA9\x52\x20\x20\x20\x20\x20\x20\xC5\x5D\xA4\x4F";
constexpr char kLegacySkillStatusCancelTransform589B20[] = "\xB0\xB1\xA4\xEE\xC5\xDC\xB8\xCB\xA1\x43";
constexpr char kLegacySkillStatusCancelDisguise589B14[] = "\xB0\xB1\xA4\xEE\xC5\xDC\xA8\xAD\xA1\x43";
constexpr char kLegacySkillStatusCancelRestore589B08[] = "\xB0\xB1\xA4\xEE\xAB\xE6\xB1\xCF\xA1\x43";
constexpr char kLegacySkillStatusCancelHeal589AFC[] = "\xB0\xB1\xA4\xEE\xAA\x76\xC0\xF8\xA1\x43";
constexpr char kLegacySkillStatusTransform589AEC[] = "\xC5\xDC\xB8\xCB\xA6\xA8\xA6\xB9\xA8\xA4\xA6\xE2\xA1\x43";
constexpr char kLegacySkillStatusDisguise589ADC[] = "\xC5\xDC\xA8\xAD\xA6\xA8\xA6\xB9\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacySkillStatusSelf589ACC[] = "\xB4\xC0\xA6\xDB\xA4\x76\xC0\xF8\xB6\xCB\xA1\x43";
constexpr char kLegacySkillStatusPet589AB8[] = "\xB4\xC0\xB3\x6F\xAD\xD3\xC3\x64\xAA\xAB\xC0\xF8\xB6\xCB\xA1\x43";
constexpr char kLegacySkillStatusPerson589AA8[] = "\xB4\xC0\xA6\xB9\xA4\x48\xC0\xF8\xB6\xCB\xA1\x43";

LegacyPanelRecord gLegacySkillTargetPanel583D58 {
    160, 132, 320, 192, 35, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacySkillTargetNext589360[] = "\xA4\x55\xAD\xB6";
constexpr char kLegacySkillTargetPrevious589358[] = "\xAB\x65\xAD\xB6";
constexpr char kLegacySkillTargetTransform589BD8[] = "\xC5\xDC\xA6\xA8\xB3\x6F\xAD\xD3\xA8\xA4\xA6\xE2\xA1\x43";
constexpr char kLegacySkillTargetDisguiseSelf589BC4[] = "\xC5\xDC\xA8\xAD\xA6\xA8\xA6\xDB\xA4\x76\xAA\xBA\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacySkillTargetDisguisePerson589BAC[] = "\xC5\xDC\xA8\xAD\xA6\xA8\xA6\xB9\xA8\xA4\xA6\xE2\xAA\xBA\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacySkillTargetRestoreSelf589B98[] = "\xB4\xC0\xA6\xDB\xA4\x76\xA9\xCE\xC3\x64\xAA\xAB\xC0\xF8\xB6\xCB\xA1\x43";
constexpr char kLegacySkillTargetRestorePerson589B80[] = "\xB4\xC0\xB3\x6F\xAD\xD3\xA8\xA4\xA6\xE2\xA9\xCE\xC3\x64\xAA\xAB\xC0\xF8\xB6\xCB\xA1\x43";

LegacyPanelRecord gLegacyItemTargetPanel582D40 {
    160, 132, 320, 192, 77, {}, {}, 0, 0, {}, {},
};

LegacyPanelRecord gLegacyTradeTargetPanel582E08 {
    160, 132, 320, 192, 51, {}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyTradeTargetSelectionCC4D48 = 0; // 4E4EA0发送前发布，发送后重读。
constexpr char kLegacyTradeTargetHint589368[] = "\xBB\x50\xB3\x6F\xAD\xD3\xA8\xA4\xA6\xE2\xB6\x69\xA6\xE6\xA5\xE6\xA9\xF6\xA1\x43";

LegacyPanelRecord gLegacyTextNoticePanel583EE8 {
    130, 180, 336, 96, 81, {}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyTextNoticePanel583FB0 {
    130, 180, 336, 96, 82, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacyTextNotice589C24[] = "\xA5\x5D\xA7\x74\x20\x5C\x20\xA9\x4D\x20\x7C\x20\xA1\x43\x20";
constexpr char kLegacyTextNotice589C08[] = "\xA8\xE8\xA6\x4C\xA4\xA3\xAF\xE0\xA8\xCF\xA5\xCE\x20\x5C\x20\xA9\x4D\x20\x7C\x20\xAA\xBA\xA6\x72\xA1\x43";
constexpr char kLegacyTextNotice589C4C[] = "\xA4\xA3\xA5\x5D\xAC\x41\xAA\xC5\xA5\xD5\xA5\x48\xA5\x7E\xAA\xBA\xA4\xE5\xA6\x72\xA1\x43";
constexpr char kLegacyTextNotice589C34[] = "\xA8\xE8\xA6\x4C\xA6\x57\xA4\xA3\xAF\xE0\xAC\x4F\xAA\xC5\xA5\xD5\xAA\xBA\xA1\x43";

LegacyPanelRecord gLegacyBuyPanel5823A0 {
    0, 0, 576, 432, 63, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBuyConfirmPanel582468 {
    0, 36, 640, 384, 65, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyBuyActionD67C6C = nullptr;
std::int32_t gLegacyBuyImageD2DBFC = 0; // 独立图号，原生产待证。
std::int32_t gLegacyBuyPageCountD2D63C = 0; // 原生产待证，不以初值替代实际页数。
std::int32_t gLegacyBuyDetailPageD67C70 = 0;
std::int32_t gLegacyBuyPreviousHover582464 = -1;
std::int32_t gLegacyBuyTotalC8AF6C = 0; // 旧63统计，旧65借读。
std::int32_t gLegacyBuyMessageD30414 = 0;
LegacyPanelRecord gLegacySellPanel582530 {
    0, 0, 576, 432, 64, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacySellConfirmPanel5825F8 {
    0, 36, 640, 384, 66, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySellActionD67C74 = nullptr; // 旧64预览借用，关闭时标记死亡。
std::int32_t gLegacySellImageCC3EFC = 0; // 独立图号，原生产待证。
std::int32_t gLegacySellPageCF49FC = 0; // 独立旧64页号，初始生产待证。
std::int32_t gLegacySellPageCountCA0ABC = 0; // 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacySellDetailPageD67C78 = 0;
std::int32_t gLegacySellPreviousHover5825F4 = -1;
std::int32_t gLegacySellTotalCF3FCC = 0; // 旧64入口统计，旧66借读。
std::int32_t gLegacySellLimitMessageCB0A88 = 0; // 增量金额失败写1，数量成功后清0。
LegacyPanelRecord gLegacyHealingPanel5826C0 {
    96, 86, 448, 288, 67, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacyHealingPrompt588FF8[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAA\x76\xC0\xF8\xB9\xEF\xB6\x48";
constexpr char kLegacyHealingHeader588FD0[] = "\xB0\xB7\xB1\x64\x20\xB5\xA5\xAF\xC5\x20\x20\x20\x20\x20\xA9\x6D\x20\x20\x20\xA6\x57\x20\x20\x20\x20\x20\x20\x20\x20\xA9\xD2\xBB\xDD\xAA\xE1\xB6\x4F";
constexpr char kLegacyHealingTotal588FA8[] = "\xA5\xFE\xB3\xA1\xA4\x40\xB0\x5F\xAA\x76\xC0\xF8\x20\x20\x20\x20\x20\x25\x38\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacyHealingGold588F94[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacyHealingCancel588F34[] = "\xA8\xFA\xAE\xF8\xAA\x76\xC0\xF8\xA1\x43";
constexpr char kLegacyHealingOne588F40[] = "\xAA\x76\xC0\xF8\xB3\x6F\xAD\xD3\xA8\xA4\xA6\xE2\xAA\xBA\xB6\xCB\xA1\x43";
constexpr char kLegacyHealingHealthy588F54[] = "\xB3\x6F\xAD\xD3\xA8\xA4\xA6\xE2\xA8\xC3\xA8\x53\xA6\xB3\xA8\xFC\xB6\xCB\xA1\x43";
constexpr char kLegacyHealingAllHealthy588F6C[] = "\xA8\x53\xA6\xB3\xA5\xF4\xA6\xF3\xA8\xA4\xA6\xE2\xA8\xFC\xB6\xCB\xA1\x43";
constexpr char kLegacyHealingAll588F80[] = "\xAA\x76\xC0\xF8\xA9\xD2\xA6\xB3\xA8\xFC\xB6\xCB\xA8\xA4\xA6\xE2\xA1\x43";

LegacyPanelRecord gLegacyRepairPanel582788 {
    32, 12, 576, 432, 70, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyRepairConfirmPanel582850 {
    0, 36, 640, 384, 71, {}, {}, 0, 0, {}, {},
};
// 原CE4548独立二十条C48H记录，字段合同见legacy_repair_list.md；上游生产待补。
std::array<char, 20 * 0xC48> gLegacyRepairItemsCE4548 {};
YobiAction* gLegacyRepairActionD67C7C = nullptr; // 旧70预览借用，关闭时标记死亡。
std::int32_t gLegacyRepairImageD673E8 = 0; // 独立图号，原生产待证。
std::int32_t gLegacyRepairPageCF4A08 = 0; // 独立页号，初始生产待证。
std::int32_t gLegacyRepairPageCountCC4B24 = 0; // 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacyRepairDetailPageD67C80 = 0;
std::int32_t gLegacyRepairPreviousHover58284C = -1;
std::int32_t gLegacyRepairTotalD460C4 = 0; // 旧70入口统计，旧71借读。
std::int32_t gLegacyRepairMessageD64E14 = 0; // 初始化/数量成功/翻页清0，金额不足写1。
constexpr char kLegacyRepairWelcome587D2C[] = "\xC5\x77\xAA\xEF\xA5\xFA\xC1\x7B\xA1\x43";
constexpr char kLegacyRepairDecrease589010[] = "\xA8\xFA\xAE\xF8\xAD\xD7\xB2\x7A\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyRepairIncrease589024[] = "\xAD\xD7\xB2\x7A\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyRepairCancel589034[] = "\xA8\xFA\xAE\xF8\xAD\xD7\xB2\x7A\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyRepairConfirm589048[] = "\xAD\xD7\xB2\x7A\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyRepairPrompt589058[] = "\xAD\x6E\xAD\xD7\xB2\x7A\xAD\xFE\xA4\x40\xAD\xD3\xA9\x4F\xA1\x48";
constexpr char kLegacyRepairNoMoney58906C[] = "\xA7\x41\xAA\xBA\xBF\xFA\xA4\xA3\xB0\xF7\xA1\x49";
constexpr char kLegacyRepairStopHint58907C[] = "\xA4\xA3\xAD\xD7\xA1\x43";
constexpr char kLegacyRepairAcceptHint589084[] = "\xAD\xD7\xB2\x7A\xA1\x43";
constexpr char kLegacyRepairQuestion58908C[] = "\xAD\xD7\xB2\x7A\xA5\x48\xA4\x57\xAA\xAB\xAB\x7E\xA1\x43\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacyRepairSummary5890A4[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4\x20\x20\x20\x20\xAD\xD7\xB2\x7A\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";

// 原旧62的独立面板与非拥有预览槽；父入口4D8DA0的模式边界仍待决。
LegacyPanelRecord gLegacyShopChoicePanel5822D8 {
    0, 0, 292, 260, 62, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyShopChoiceActionD67C68 = nullptr;

LegacyPanelRecord gLegacyPetSkillCatalogPanel581FB8 {
    0, 0, 576, 432, 58, {23090, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyPetSkillCatalogActionD67C5C = nullptr; // 4D7440发布，4D7470关闭时标死/清槽。
LegacyPanelRecord gLegacyPetSkillPetPanel582080 {
    0, 0, 576, 432, 59, {23091, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyPetSkillPetActionD67C60 = nullptr; // 旧59独立预览，不共享旧58对象槽。
LegacyPanelRecord gLegacyPetSkillSlotsPanel582148 {
    0, 0, 576, 432, 60, {23092, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyPetSkillSlotsActionD67C64 = nullptr;
LegacyPanelRecord gLegacyPetSkillConfirmPanel582210 {
    160, 108, 320, 240, 61, {}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyPetSkillSlotCF4B64 = 0; // 旧60写入，旧61借读；不同于现代替换选择。
constexpr char kLegacyPetSkillSlotHint588D00[] = "\xA8\xCF\xA5\xCE\xA6\xB9\xC4\xE6\xA8\xD3\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacyPetSkillStop588D18[] = "\xB0\xB1\xA4\xEE\xA8\xCF\xA5\xCE\xA6\xB9\xC4\xE6\xA8\xD3\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacyPetSkillAccept588D34[] = "\xC5\xFD\xC3\x64\xAA\xAB\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacyPetSkillTitle588344[] = "\xBE\xC7\xB2\xDF\xA1\x79\x25\x73\xA1\x7A";
constexpr char kLegacyPetSkillPetName588338[] = "\xC5\xFD\xA1\x79\x25\x73\xA1\x7A";
constexpr char kLegacyPetSkillPetMeasure588320[] = "\xC5\xFD\xA1\x79\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\xA1\x7A";
constexpr char kLegacyPetSkillUsedSlot58830C[] = "\xA8\xCF\xA5\xCE\xA1\x79\x25\x73\xA1\x7A\xAA\xBA\xA6\xEC\xB8\x6D";
constexpr char kLegacyPetSkillEmptySlot5882F4[] = "\xA8\xCF\xA5\xCE\xA1\x79\xAA\xC5\xC4\xE6\xA6\xEC\xA1\x7A\xAA\xBA\xA6\xEC\xB8\x6D";
constexpr char kLegacyPetSkillLearnLine5882E8[] = "\xA8\xD3\xBE\xC7\xB2\xDF\xA1\x43";
constexpr char kLegacyPetSkillQuestion55C9B0[] = "\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacyPetSkillGold588D5C[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42\xA1\x40\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacyPetSkillPrice588D48[] = "\xC1\xCA\xB6\x52\xAA\xF7\xC3\x42\x20\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacyPetSkillPetCancel588CE8[] = "\xA8\xFA\xAE\xF8\xC5\xFD\xC3\x64\xAA\xAB\xBE\xC7\xB2\xDF\xA6\xB9\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacyPetSkillCatalogCancel588CD0[] = "\xA8\xFA\xAE\xF8\xC5\xFD\xC3\x64\xAA\xAB\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43";
constexpr char kLegacyPetSkillCatalogLearn588268[] = "\xC5\xFD\xC3\x64\xAA\xAB\xBE\xC7\xB2\xDF\xA6\xB9\xA7\xDE\xAF\xE0\xA1\x43";

LegacyPanelRecord gLegacyIdentifyPanel582918 {
    32, 12, 576, 432, 68, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyIdentifyConfirmPanel5829E0 {
    0, 36, 640, 384, 69, {}, {}, 0, 0, {}, {},
};
// 原C8B390独立二十条C48H记录，末端C9A930；原字段合同见legacy_identify_list.md。
std::array<char, 20 * 0xC48> gLegacyIdentifyItemsC8B390 {};
YobiAction* gLegacyIdentifyActionD67C84 = nullptr; // 旧68预览借用，关闭时标记死亡。
std::int32_t gLegacyIdentifyImageCF4698 = 0; // 独立图号，原生产待证。
std::int32_t gLegacyIdentifyPageCC3F00 = 0; // 独立页号，初始生产待证。
std::int32_t gLegacyIdentifyPageCountC9A950 = 0; // 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacyIdentifyDetailPageD67C88 = 0;
std::int32_t gLegacyIdentifyPreviousHover5829DC = -1;
std::int32_t gLegacyIdentifyTotalCB1E3C = 0; // 入口统计，翻页/数量成功清0，旧69借读。
std::int32_t gLegacyIdentifyMessageC9A948 = 0; // 初始化清0，金币不足写1；翻页/数量成功不清。
constexpr char kLegacyIdentifyDecrease5890D0[] = "\xA8\xFA\xAE\xF8\xC5\xB2\xA9\x77\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyIdentifyIncrease5890E4[] = "\xC5\xB2\xA9\x77\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyIdentifyConfirm5890F4[] = "\xC5\xB2\xA9\x77\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyIdentifyCancel589104[] = "\xA8\xFA\xAE\xF8\xC5\xB2\xA9\x77\xA1\x43";
constexpr char kLegacyIdentifyPrompt589110[] = "\xAD\x6E\xC5\xB2\xA9\x77\xAD\xFE\xA4\x40\xAD\xD3\xAA\xAB\xAB\x7E\xA1\x48";
constexpr char kLegacyIdentifyStopHint589124[] = "\xA4\xA3\xC5\xB2\xA9\x77\xA1\x43";
constexpr char kLegacyIdentifyAcceptHint589130[] = "\xC5\xB2\xA9\x77\xA1\x43";
constexpr char kLegacyIdentifyQuestion589138[] = "\xC5\xB2\xA9\x77\xA5\x48\xA4\x57\xAA\xAB\xAB\x7E\xA1\x43\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacyIdentifySummary589150[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4\x20\x20\x20\x20\xC5\xB2\xA9\x77\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";

LegacyPanelRecord gLegacyMessagePanel582AA8 {
    20, 9, 600, 438, 73, {23224, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyMessageInputD67C8C = nullptr; // 非拥有CC46E0借用，关闭只清槽/归还焦点。
constexpr char kLegacyMessageDelete58917C[] = "\xA7\x52\xB0\xA3\xA6\xB9\xAF\x64\xA8\xA5\xA1\x43";
constexpr char kLegacyMessagePrevious58918C[] = "\xAB\x65\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyMessageCancel589198[] = "\xA8\xFA\xAE\xF8\xAF\x64\xA8\xA5\xA1\x43";
constexpr char kLegacyMessageEmpty5891A4[] = "\xBD\xD0\xBF\xE9\xA4\x4A\xA4\xE5\xA6\x72\xA1\x43";
constexpr char kLegacyMessagePost5891B4[] = "\xAF\x64\xA8\xA5\xA1\x43";
constexpr char kLegacyMessageNext553574[] = "\xA4\x55\xA4\x40\xAD\xB6\xA1\x43";

constexpr char kLegacySellDecrease588E74[] = "\xB4\xEE\xA4\xD6\xBD\xE6\xB1\xBC\xA6\xB9\xAA\xAB\xAB\x7E\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacySellIncrease588E8C[] = "\xBC\x57\xA5\x5B\xBD\xE6\xB1\xBC\xA6\xB9\xAA\xAB\xAB\x7E\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacySellConfirm588EA4[] = "\xBD\xE6\xB1\xBC\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacySellCancel588EB4[] = "\xA4\xA3\xBD\xE6\xA4\x46\xA1\x43";
constexpr char kLegacySellLimitLine588ECC[] = "\xB5\x4C\xAA\x6B\xA6\x41\xAE\xB3";
constexpr char kLegacySellLimitLine588EC0[] = "\xA7\xF3\xA6\x68\xAA\xBA\xBF\xFA\xA4\x46";
constexpr char kLegacySellStopHint588ED8[] = "\xA4\xA3\xBD\xE6\xA1\x43";
constexpr char kLegacySellAcceptHint588EE0[] = "\xBD\xE6\xA5\x58\xA1\x43";
constexpr char kLegacySellQuestion588EE8[] = "\xBD\xE6\xA5\x58\xA5\x48\xA4\x57\xAA\xAB\xAB\x7E\xA1\x43\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacySellSummary588F00[] = "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4\x20\x20\x20\xBD\xE6\xA5\x58\xAA\xF7\xC3\x42\x20\x20\x25\x37\x64\x20\xAA\xF7\xB9\xF4";
constexpr char kLegacySellQuantity588F28[] = "\x25\x73\x28\x25\x64\x81\x7E\x25\x64\x29";

constexpr char kLegacyBuyUnavailable588E20[] = "\x81\x7E";
constexpr char kLegacyBuyCancel588E14[] = "\xA4\xA3\xB6\x52\xA4\x46\xA1\x43";
constexpr char kLegacyBuyConfirm588E04[] = "\xC1\xCA\xB6\x52\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyBuyDecrease588DD4[] = "\xB4\xEE\xA4\xD6\xA6\xB9\xAA\xAB\xAB\x7E\xA4\xA7\xC1\xCA\xB6\x52\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyBuyIncrease588DEC[] = "\xBC\x57\xA5\x5B\xA6\xB9\xAA\xAB\xAB\x7E\xA4\xA7\xC1\xCA\xB6\x52\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyBuyUnavailableHint586BF4[] = "\xA1\xAF\xB2\x7B\xA6\x62\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6";
constexpr char kLegacyBuyQuestion588E34[] = "\xC1\xCA\xB6\x52\xA5\x48\xA4\x57\xAA\xAB\xAB\x7E\xA1\x43\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacyBuyAcceptHint588E2C[] = "\xC1\xCA\xB6\x52\xA1\x43";
constexpr char kLegacyBuyStopHint588E24[] = "\xA4\xA3\xB6\x52\xA1\x43";
constexpr char kLegacyBuySummary588E4C[] =
    "\xAB\xF9\xA6\xB3\xAA\xF7\xC3\x42  %7d \xAA\xF7\xB9\xF4  \xC1\xCA\xB6\x52\xAA\xF7\xC3\x42  %7d \xAA\xF7\xB9\xF4";

// 原CC1A0C是现有商店文本区尾部别名，不能另建页号副本。
std::int32_t ReadLegacyBuyPageCC1A0C()
{
    return ReadRecordW32(ui::detail::gShopMoneyTextCC1A00.data(), 12);
}

void WriteLegacyBuyPageCC1A0C(const std::int32_t page)
{
    std::memcpy(ui::detail::gShopMoneyTextCC1A00.data() + 12, &page, sizeof(page));
}

char* BorrowLegacyShopItemCF4FE0(const std::int32_t index)
{
    return ui::detail::gShopItemsCF4FE0.data() + index * 0xC58;
}

char* BorrowLegacyRepairItemCE4548(const std::int32_t index)
{
    return gLegacyRepairItemsCE4548.data() + index * 0xC48;
}

char* BorrowLegacyIdentifyItemC8B390(const std::int32_t index)
{
    return gLegacyIdentifyItemsC8B390.data() + index * 0xC48;
}

// 原CA0B30二十条C2CH记录，物理末端CAFEA0不含相邻状态。
struct LegacyBarterItemRecord {
    std::uint8_t used;
    std::array<char, 29> name;
    std::array<std::array<char, 96>, 32> descriptions;
    std::array<std::byte, 2> reservedC1E;
    std::int32_t image;
    std::int32_t cost;
    std::int32_t selected;
};
static_assert(sizeof(LegacyBarterItemRecord) == 0xC2C);
static_assert(offsetof(LegacyBarterItemRecord, name) == 1);
static_assert(offsetof(LegacyBarterItemRecord, descriptions) == 0x1E);
static_assert(offsetof(LegacyBarterItemRecord, image) == 0xC20);
static_assert(offsetof(LegacyBarterItemRecord, cost) == 0xC24);
static_assert(offsetof(LegacyBarterItemRecord, selected) == 0xC28);
std::array<LegacyBarterItemRecord, 20> gLegacyBarterItemsCA0B30 {};
LegacyPanelRecord gLegacyBarterPanel5818B0 {
    0, 0, 576, 432, 75, {23330, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBarterConfirmPanel581978 {
    0, 36, 640, 384, 76, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyBarterActionD67C50 = nullptr;
std::int32_t gLegacyBarterImageCF4A0C = 0; // 独立图号，原生产待证。
std::int32_t gLegacyBarterDetailPageD67C54 = 0;
std::int32_t gLegacyBarterPreviousHover581974 = -1;
std::int32_t gLegacyBarterPageCB10D0 = 0;
std::int32_t gLegacyBarterPageCountC8A954 = 0; // 原生产待证，不以初值替代实际页数。
std::int32_t gLegacyBarterTotalD2F334 = 0; // 旧75统计，旧76借读。
std::int32_t gLegacyBarterMessageCC2E54 = 0;
constexpr char kLegacyBarterCancel588994[] = "\xA8\xFA\xAE\xF8\xA5\xE6\xA9\xF6\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyBarterConfirm588984[] = "\xA5\xE6\xA9\xF6\xAB\xFC\xA9\x77\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyBarterIncrease58896C[] = "\xBC\x57\xA5\x5B\xA6\xB9\xAA\xAB\xAB\x7E\xA5\xE6\xA9\xF6\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyBarterDecrease588954[] = "\xB4\xEE\xA4\xD6\xA6\xB9\xAA\xAB\xAB\x7E\xA5\xE6\xA9\xF6\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyBarterYes588A08[] = "\x20\x20\xAC\x4F";
constexpr char kLegacyBarterNo588A00[] = "\x20\x20\xA7\x5F";
constexpr char kLegacyBarterQuestion5889C0[] = "\xA5\xE6\xA9\xF6\xA5\x48\xA4\x57\xAA\xAB\xAB\x7E\xA1\x43\xA5\x69\xA5\x48\xB6\xDC\xA1\x48";
constexpr char kLegacyBarterAcceptHint5889B8[] = "\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyBarterStopHint5889AC[] = "\xB0\xB1\xA4\xEE\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyBarterSummary5889D8[] =
    "%s  %7d \xAD\xD3   \xA5\xE6\xB4\xAB\xAD\xD3\xBC\xC6  %7d \xAD\xD3";

LegacyPanelRecord gLegacyExchangePanel581A40 {
    0, 0, 292, 260, 74, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyGiveReceivePanel581B08 {
    0, 0, 292, 260, 83, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyExchangeActionCF4B5C = nullptr;
YobiAction* gLegacyGiveReceiveActionCB2228 = nullptr;
std::int32_t gLegacyGiveReceiveModeC8AA98 = 0; // 独立原状态，非4A0850的receive局部；非零生产待证。
LegacyPanelRecord gLegacyGiveReceiveQuantityPanel581BD0 {
    0, 0, 576, 432, 84, {23330, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyGiveReceiveQuantityActionCB1DD4 = nullptr;
std::int32_t gLegacyGiveReceiveImageD2D814 = 0; // 独立图号，生产待证，不借现代D2DAD0。
std::int32_t gLegacyGiveReceiveDetailPageD2EAB0 = 0;
constexpr char kLegacyExchangeChoice588A40[] = "\x20\xB7\x51\x20\xA5\xE6\x20\xA9\xF6";
constexpr char kLegacyExchangeCancel588A34[] = "\x20\x20\x20\xA8\x53\x20\xA8\xC6";
constexpr char kLegacyExchangeHint588A20[] = "\xBB\x50\xA6\xB9\xA9\xB1\xA5\xE6\xA9\xF6\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyExchangeCancelHint588A10[] = "\xA4\xA3\xB7\x51\xA5\xE6\xA9\xF6\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyGiveReceiveCancel588A70[] = "\xA1\x40\xA1\x40\xA8\xFA\xAE\xF8\xA1\x40\xA1\x40";
constexpr char kLegacyGiveChoice588A64[] = "\xA1\x40\xA5\xE6\xA5\x58\xAA\xAB\xAB\x7E";
constexpr char kLegacyReceiveChoice585A40[] = "\xB1\xB5\xA8\xFC\xAA\xAB\xAB\x7E";
constexpr char kLegacyGiveHint588A58[] = "\xA5\xE6\xA5\x58\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyGiveReceiveCancelHint588A4C[] = "\xA8\xFA\xAE\xF8\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyQuantityUnit5889A4[] = "\xAD\xD3";
constexpr char kLegacyQuantityIncreaseLimit588ABC[] = "\xB5\x4C\xAA\x6B\xA6\x41\xBC\x57\xA5\x5B\xA1\x43";
constexpr char kLegacyReceiveIncrease588AA8[] = "\xBC\x57\xA5\x5B\xB1\xB5\xA8\xFC\xAA\xBA\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyGiveIncrease588ACC[] = "\xBC\x57\xA5\x5B\xA5\xE6\xA5\x58\xAA\xBA\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyQuantityDecreaseLimit588944[] = "\xB5\x4C\xAA\x6B\xA6\x41\xB4\xEE\xA4\xD6\xA1\x43";
constexpr char kLegacyReceiveDecrease588A80[] = "\xB4\xEE\xA4\xD6\xB1\xB5\xA8\xFC\xAA\xBA\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyGiveDecrease588A94[] = "\xB4\xEE\xA4\xD6\xA5\xE6\xA5\x58\xAA\xBA\xBC\xC6\xB6\x71\xA1\x43";
constexpr char kLegacyReceiveCancel588AEC[] = "\xA8\xFA\xAE\xF8\xB1\xB5\xA6\xAC\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyGiveCancel588AFC[] = "\xA8\xFA\xAE\xF8\xA5\xE6\xA5\x58\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyReceiveHint588AE0[] = "\xB1\xB5\xA8\xFC\xAA\xAB\xAB\x7E\xA1\x43";

// 旧74/83完全同构的标题及五行正文；每次绘制后仍重读面板坐标。
void DrawLegacySmallNpcBody(const LegacyPanelRecord* const panel)
{
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    const std::int32_t titleY = panel->y + 8;
    const std::int32_t titleX = panel->x - width / 2 + 142;
    static_cast<void>(DrawText(static_cast<std::int16_t>(titleX),
        static_cast<std::int16_t>(titleY), 2, 0, 0, lines[0].text.data(), 0, 0));
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t y = panel->y + 16 * (row + 3);
        const std::int32_t x = panel->x + 90;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y), 2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
}

LegacyPanelRecord gLegacyAlbumDetailPanel5846C0 {
    0, 0, 272, 438, 24, {22890, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyAlbumListPanel584788 {
    0, 0, 272, 438, 23, {22880, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyAlbumSelectionC8AF7C = 0;
std::int32_t gLegacyAlbumPageC8B1B0 = 0;
std::int32_t gLegacyAlbumReturningCC40D0 = 0;
YobiAction* gLegacyAlbumActionD67D14 = nullptr;
constexpr char kLegacyAlbumSkipTen58A0F0[] = "\xB8\xF5\xB9\x4C\x31\x30\xAD\xB6\xA1\x43";
constexpr char kLegacyAlbumBack58A0FC[] = "\xA6\x5E\xA8\xEC\xB9\xCF\xC5\xB2\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyAlbumOpen58A114[] = "\xA5\xB4\xB6\x7D\xA6\xB9\xC3\x64\xAA\xAB\xAA\xBA\xB9\xCF\xC5\xB2\xB5\xF8\xB5\xA1\xA1\x43";

// 原旧20列表面板及两帧展开标志；4F7140初始化，4F7150/4F7870共享。
LegacyPanelRecord gLegacyPetListPanel584850 {
    0, 0, 272, 408, 20, {22840, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int16_t gLegacyPetStateOpenD67D18 = 0;
std::int16_t gLegacyPetStatePreviousD2D48C = 0;
constexpr std::array<std::int32_t, 10> kLegacyPetTribeImages57CB2C {
    22865, 22866, 22867, 22868, 22869, 22870, 22871, 22872, 22873, 22874,
};
constexpr char kLegacyPetOpenDetails586D0C[] = "\xA5\xB4\xB6\x7D\xA6\xB9\xC3\x64\xAA\xAB\xAA\xBA\xAF\xE0\xA4\x4F\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyPetChooseState58A12C[] = "\xAB\xFC\xA9\x77\xC3\x64\xAA\xAB\xAA\xBA\xAA\xAC\xBA\x41\xA1\x43";

constexpr char kLegacyPetBasicTab585958[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA6\xB9\xC3\x64\xAA\xAB\xAA\xBA\xAF\xE0\xA4\x4F\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyPetAttributeTab58593C[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA6\xB9\xC3\x64\xAA\xAB\xAA\xBA\xB8\xD4\xB2\xD3\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyPetSkillTab585920[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA6\xB9\xC3\x64\xAA\xAB\xAA\xBA\xA7\xDE\xAF\xE0\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyPetBack58A204[] = "\xA6\x5E\xA8\xEC\xC3\x64\xAA\xAB\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacyPetRename586D38[] = "\xC5\xDC\xA7\xF3\xC3\x64\xAA\xAB\xA6\x57\xA6\x72\xA1\x43";
constexpr char kLegacyPetLevelLimit58A214[] = "   ---   ";
constexpr char kLegacyPetVitality58A288[] = "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xA5\xCD\xA9\x52\xA1\x43";
constexpr char kLegacyPetToughness58A260[] = "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xA8\xBE\xBF\x6D\xA1\x43";
constexpr char kLegacyPetMagic58A238[] = "\xB1\x4E\xC2\x49\xBC\xC6\xA4\xC0\xB0\x74\xA6\xDC\xC5\x5D\xAA\x6B\xA1\x43";
LegacyPanelRecord gLegacyPetDetailPanel584918 {
    0, 0, 272, 438, 21, {22800, 22810, 22824, 0, 0},
    {{{1, 6, 415, 78, 17, kLegacyPetBasicTab585958},
      {1, 97, 415, 78, 17, kLegacyPetAttributeTab58593C},
      {1, 187, 415, 78, 17, kLegacyPetSkillTab585920}, {}, {}}},
    0, 0, {}, {},
};
std::int32_t gLegacyPetSelectionD67ADC = 0; // 旧20生产，旧21/22共用位置，不是PetPair中的槽号。
YobiAction* gLegacyPetDetailActionD67AE0 = nullptr;
LegacyPanelRecord gLegacyPetRenamePanel5849E0 {
    0, 0, 272, 200, 22, {22809, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyPetRenameInputD67D1C = nullptr; // 借用独立D2DFD0，不复用现代宠物输入。

bool HandleLegacyPetNavigation(const LegacyPanelButtonRecord* const back,
    const LegacyPanelButtonRecord* const previous, const LegacyPanelButtonRecord* const next)
{
    if ((QueryPanelButtonState(back) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetDetailPanel584918.id));
        static_cast<void>(audio::Sub_504180(20));
    } else if ((QueryPanelButtonState(previous) & 0x10) != 0) {
        --gLegacyPetSelectionD67ADC;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyPetSelectionD67ADC < 0) {
            gLegacyPetSelectionD67ADC = world::detail::gPetCountDE0B00 - 1;
        }
    } else if ((QueryPanelButtonState(next) & 0x10) != 0) {
        ++gLegacyPetSelectionD67ADC;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        if (gLegacyPetSelectionD67ADC >= world::detail::gPetCountDE0B00) {
            gLegacyPetSelectionD67ADC = 0;
        }
    } else {
        return false;
    }
    return true;
}

// 旧52独立面板/拆分参数；交易展示、金额和公开阶段借WindowRuntime原owner。
LegacyPanelRecord gLegacyTradePanel582F00 {
    40, 0, 560, 364, 52, {23230, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr std::array<std::array<std::int32_t, 4>, 2> kLegacyTradeGrid582ECC {{
    {13, 94, 60, 141}, {291, 94, 338, 141},
}};
std::int32_t gLegacyTradeSplitSourceD45F90 = 0;
std::int32_t gLegacyTradeSplitTargetD2D668 = 0;
std::int32_t gLegacyTradeSplitAmountD2D490 = 0;
std::int32_t gLegacyTradeMouseXCF3FEC = 0;
std::int32_t gLegacyTradeMouseYCF3FE8 = 0;
std::int32_t gLegacyTradeDetailPageD67CC8 = 0;
std::int32_t gLegacyTradePrevious582FCC = -1;
constexpr std::array<std::array<std::int32_t, 2>, 2> kLegacyTradePetRows582EEC {{
    {70, 123}, {348, 123},
}};
std::int32_t gLegacyTradeRemoteItemPrevious582FC4 = -1;
std::int32_t gLegacyTradeRemotePetPrevious582FC8 = -1;
std::int32_t gLegacyTradeLocalPetPrevious582FD0 = -1;
std::int32_t gLegacyTradeRemoteItemPageD67CC0 = 0;
std::int32_t gLegacyTradeRemotePetPageD67CC4 = 0;
std::int32_t gLegacyTradeLocalPetPageD67CCC = 0;
YobiAction* gLegacyTradePetActionD67CB4 = nullptr; // 旧52外层创建/关闭，属性页借用。
constexpr char kLegacyTradeAvailable589524[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xA5\x69\xA5\x48\xAE\xB3\xA8\xD3\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeUnavailable58953C[] = "\xA6\xB9\xAA\xAB\xAB\x7E\xA4\xA3\xAF\xE0\xAE\xB3\xA8\xD3\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeSelected589554[] = "\xA7\xE2\xA6\xB9\xAA\xAB\xAB\x7E\xAE\xB3\xA8\xD3\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeRemoteItem5894DC[] = "\xB3\x6F\xAC\x4F\xB9\xEF\xA4\xE8\xB7\x51\xA5\xE6\xA9\xF6\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyTradeRemotePet5894F4[] = "\xB3\x6F\xAC\x4F\xB9\xEF\xA4\xE8\xB7\x51\xA5\xE6\xA9\xF6\xAA\xBA\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacyTradeRemoteMoney58950C[] = "\xB3\x6F\xAC\x4F\xB9\xEF\xA4\xE8\xB7\x51\xA5\xE6\xA9\xF6\xAA\xBA\xAA\xF7\xC3\x42\xA1\x43";
constexpr char kLegacyTradePetAvailable589568[] = "\xA6\xB9\xC3\x64\xAA\xAB\xA5\x69\xA5\x48\xAE\xB3\xA8\xD3\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradePetSelected589580[] = "\xA7\xE2\xA6\xB9\xC3\x64\xAA\xAB\xAE\xB3\xA8\xD3\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeChoosing5894CC[] = "\xBF\xEF\xA8\xFA\xA5\xE6\xA9\xF6\xAA\xAB\xAB\x7E\xA4\xA4";
constexpr char kLegacyTradePublished5894B8[] = "\xA5\xE6\xA9\xF6\xAA\xAB\xAB\x7E\xBF\xEF\xA8\xFA\xA7\xB9\xA6\xA8";
constexpr char kLegacyTradeConfirming5894AC[] = "\xBD\x54\xBB\x7B\xA5\xE6\xA9\xF6\xA4\xA4";
constexpr char kLegacyTradeConfirmSelected589484[] = "\xB2\x7B\xA6\x62\xA5\x69\xA5\x48\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeConfirm589478[] = "\xB6\x69\xA6\xE6\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeCancel5889AC[] = "\xB0\xB1\xA4\xEE\xA5\xE6\xA9\xF6\xA1\x43";
constexpr char kLegacyTradeRemoteItemTab589458[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA5\xE6\xA9\xF6\xB9\xEF\xA4\xE2\xAA\xBA\xAA\xAB\xAB\x7E\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradeRemotePetTab589438[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA5\xE6\xA9\xF6\xB9\xEF\xA4\xE2\xAA\xBA\xC3\x64\xAA\xAB\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradeRemoteMoneyTab589418[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xA5\xE6\xA9\xF6\xB9\xEF\xA4\xE2\xAA\xBA\xAA\xF7\xBF\xFA\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradeLocalItemTab589400[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xAA\xAB\xAB\x7E\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradeLocalPetTab5893E8[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xC3\x64\xAA\xAB\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradeLocalMoneyTab5893D0[] = "\xA4\xC1\xB4\xAB\xA6\xDC\xAA\xF7\xBF\xFA\xC4\xE6\xA8\xBA\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyTradePublishItems5893B4[] = "\xA7\xE2\xBF\xEF\xBE\xDC\xAA\xBA\xAA\xAB\xAB\x7E\xAE\xB3\xB5\xB9\xB9\xEF\xA4\xE8\xAC\xDD\xA1\x43";
constexpr char kLegacyTradePublishPets589398[] = "\xA7\xE2\xBF\xEF\xBE\xDC\xAA\xBA\xC3\x64\xAA\xAB\xAE\xB3\xB5\xB9\xB9\xEF\xA4\xE8\xAC\xDD\xA1\x43";
constexpr char kLegacyTradePublishMoney589380[] = "\xA7\xE2\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xAE\xB3\xB5\xB9\xB9\xEF\xA4\xE8\xAC\xDD\xA1\x43";
constexpr char kLegacyTradePetCount58949C[] = "%d\xB0\xA6";
constexpr char kLegacyTradeItemCount5894A4[] = "%2d\xAD\xD3";
constexpr char kLegacyTradeMoney589494[] = "%7d\xAA\xF7\xB9\xF4";

char* BorrowLegacyTradeItem(const std::int32_t side, const std::int32_t item)
{
    return ui::detail::gTradeItemsD460D0.data() + (side * 20 + item) * 0xC4C;
}

char* BorrowLegacyTradePet(const std::int32_t side, const std::int32_t pet)
{
    return ui::detail::gTradePetsC9C108.data() + (side * 5 + pet) * 0x5FC;
}

std::int16_t ReadLegacyTradeW16(const char* const record, const std::size_t offset)
{
    std::int16_t value;
    std::memcpy(&value, record + offset, sizeof(value));
    return value;
}

void CancelLegacyTradeOffer()
{
    static_cast<void>(network::Sub_512230(platform::gConnectionSocket));
    const std::int32_t remote = ui::detail::gTradeValueCC4224;
    ui::detail::gTradeValueD2E2B0 = 0;
    if (remote == 2) {
        ui::detail::gTradeValueCC4224 = 1;
    }
}

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
constexpr std::array<std::array<std::int32_t, 2>, 2> kLegacyStoragePetRows582C54 {{
    {70, 123}, {348, 123},
}};
std::int32_t gLegacyStoragePetPreviousLeft582C6C = -1;
std::int32_t gLegacyStoragePetPreviousRight582C70 = -1;
std::int32_t gLegacyStoragePetPageD67CA0 = 0;
std::int32_t gLegacyStoragePetMessageD67CA4 = 0;
std::uint32_t gLegacyStoragePetTickD67CA8 = 0;
std::int32_t gLegacyStoragePetPutCountD2D600 = 0;
std::int32_t gLegacyStoragePetTakeCountD2D824 = 0;
std::int32_t gLegacyStoragePetCapacityCB1DDC = 0;
YobiAction* gLegacyStoragePetActionD67C90 = nullptr;
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
constexpr char kLegacyStoragePetTake5892C4[] = "\xA8\xFA\xA5\x58\xB3\x6F\xB0\xA6\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacyStoragePetPut5892B4[] = "\xA6\x73\xA9\xF1\xB3\x6F\xB0\xA6\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacyStoragePetCanPut5892A0[] = "\xA6\xB9\xC3\x64\xAA\xAB\xA5\x69\xA5\x48\xA6\x73\xA9\xF1\xA1\x43";
constexpr char kLegacyStoragePetFull58928C[] = "\xB5\x4C\xAA\x6B\xA6\x41\xA6\x73\xA9\xF1\xC3\x64\xAA\xAB\xA1\x43";
constexpr char kLegacyStorageItemTab589214[] = "\xAA\xAB\xAB\x7E\xA6\x73\xA9\xF1\xBB\x50\xA8\xFA\xA5\x58\xA1\x43";
constexpr char kLegacyStoragePetTab589200[] = "\xC3\x64\xAA\xAB\xA6\x73\xA9\xF1\xBB\x50\xA8\xFA\xA5\x58\xA1\x43";
constexpr char kLegacyStorageDepositTab5891F8[] = "\xA6\x73\xBF\xFA\xA1\x43";
constexpr char kLegacyStorageWithdrawTab5891F0[] = "\xBB\xE2\xBF\xFA\xA1\x43";
constexpr char kLegacyStorageConfirm5891C4[] = "\xBD\x54\xA9\x77\xA1\x43";
constexpr char kLegacyStorageCancel5891BC[] = "\xA8\xFA\xAE\xF8\xA1\x43";
constexpr char kLegacyStorageItemCount5891D4[] = "%2d \xAD\xD3";
constexpr char kLegacyStoragePetCount5891CC[] = "%d \xB0\xA6";

char* BorrowLegacyStorageItem(const std::int32_t side, const std::int32_t item)
{
    return ui::detail::gItemDetailStorageCC4D58.data() + side * 0xFBF8 + item * 0xC44;
}

char* BorrowLegacyStoragePet(const std::int32_t side, const std::int32_t pet)
{
    return ui::detail::gItemDetailStorageCC4D58.data() + side * 0xFBF8 + 0xF550 + pet * 0x154;
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
// 旧80独立状态；583164为已加载-1，其余五槽按原虚拟零初始化。
LegacyPanelRecord gLegacyBurialPanel5830A0 {
    368, 0, 272, 252, 80, {23358, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyBurialHover583164 = -1;
std::int32_t gLegacyBurialPageCC4B64 = 0;
std::int32_t gLegacyBurialSelectedCC4214 = 0;
std::int32_t gLegacyBurialExcludedCC48AC = 0;
std::int32_t gLegacyBurialExcludedColumnD2E2B4 = 0;
std::int32_t gLegacyBurialExcludedRowD671D0 = 0;
constexpr char kLegacyBurialSelect5895FC[] = "\xAE\x49\xB0\x5F\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
constexpr char kLegacyBurialReject5895E4[] = "\xB3\x6F\xAD\xD3\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xAE\x49\xB0\x5F\xA8\xD3\xA1\x43";
constexpr char kLegacyBurialChoose5895D0[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAD\x6E\xAE\x49\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43";
// 旧50赠送页；CC1A2C与后续旧49共用唯一选择，不复制邮件草稿。
LegacyPanelRecord gLegacyMailGiftPanel583168 {
    368, 0, 272, 252, 50, {23205, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMailGiftHover58322C = -1;
std::int32_t gLegacyMailGiftPageCC2E58 = 0;
std::int32_t gLegacyMailGiftSelectedCC1A2C = 0;
std::int32_t gLegacyMailGiftColumnD2D498 = 0;
std::int32_t gLegacyMailGiftRowCC3E6C = 0;
constexpr char kLegacyMailGift58960C[] = "\xB1\x4E\xA6\xB9\xAA\xAB\xAB\x7E\xB0\x65\xB5\xB9\xA6\x57\xA4\xF9\xA4\x57\xAA\xBA\xA6\x6E\xA4\xCD\xA1\x43";
// 旧49联系人/协议值唯一承载；分别由旧42联系人发布与旧48邮件宠物选择生产。
LegacyPanelRecord gLegacyMailComposePanel583230 {
    0, 0, 272, 438, 49, {23200, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMailContactCC4228 = 0;
std::int32_t gLegacyMailValueCC2E60 = 0;
constexpr char kLegacyMailSend589640[] = "\xB0\x65\xB6\x6C\xA5\xF3\xB5\xB9\xBF\xEF\xBE\xDC\xAA\xBA\xA6\x57\xA4\xF9\xA4\x57\xAA\xBA\xA6\x6E\xA4\xCD\xA1\x43";
constexpr char kLegacyMailReturn589628[] = "\xA6\x5E\xA8\xEC\xA6\x57\xA4\xF9\xB5\x6E\xBF\xFD\xA5\xBB\xAA\xBA\xB5\xF8\xB5\xA1\xA1\x43";
LegacyPanelRecord gLegacyMailPetPanel5832F8 {
    0, 0, 272, 238, 48, {23189, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyMailPet589660[] = "\xC5\xFD\xB3\x6F\xB0\xA6\xC3\x64\xAA\xAB\xB9\x42\xB0\x65\xB6\x6C\xA5\xF3\xA1\x43";
// 旧42四个未读提示T*槽，仅借用真实动画；初始化补空，关闭逐槽标死并清引用。
LegacyPanelRecord gLegacyMailContactPanel5837A8 {
    0, 0, 272, 451, 42, {238521, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<YobiAction*, 4> gLegacyMailUnreadActionsCC3E48 {};
std::int32_t gLegacyMailSelectedD30410 = 0;
std::int32_t gLegacyMailPreviousPageD2DACC = 0;
constexpr std::array<std::int32_t, 15> kLegacyMailStatusImages58386C {
    30131, 30231, 30331, 30431, 30531, 30631, 30731, 30831,
    30931, 31131, 31231, 31331, 31431, 31531, 31631,
};
constexpr char kLegacyMailEdit5897AC[] = "\xB1\x4E\xA6\x57\xA4\xF9\xAA\xBA\xAA\xED\xA5\xDC\xB6\xB6\xA7\xC7\xB1\xC6\xA7\xC7\xA1\x43";
constexpr char kLegacyMailGroup589798[] = "\xC5\xE3\xA5\xDC\xAE\x61\xB1\xDA\xB3\x71\xB0\x54\xBF\xFD\xA1\x43";
constexpr char kLegacyMailContactSend589780[] = "\xB0\x65\xB6\x6C\xA5\xF3\xB5\xB9\xB3\x6F\xA6\x57\xA4\xF9\xA4\x57\xAA\xBA\xA6\x6E\xA4\xCD";
constexpr char kLegacyMailContactRead589760[] = "\xAC\xDD\xB3\x6F\xA6\x57\xA4\xF9\xA4\x57\xA6\x6E\xA4\xCD\xAA\xBA\xB6\x6C\xA5\xF3\xB3\x71\xAB\x48\xAC\xF6\xBF\xFD\xA1\x43";
constexpr char kLegacyMailContactDelete589750[] = "\xA7\x52\xB0\xA3\xB3\x6F\xB1\x69\xA6\x57\xA4\xF9\xA1\x43";
LegacyPanelRecord gLegacyMailDeliveryPanel583550 {
    0, 0, 192, 174, 45, {23184, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyMailNormalMode5896E0[] = "\xA5\x48\xB4\xB6\xB3\x71\xBC\xD2\xA6\xA1\xB0\x65\xB6\x6C\xA5\xF3\xA1\x43";
constexpr char kLegacyMailPetMode5896D0[] = "\xA5\x48\xC3\x64\xAA\xAB\xB0\x65\xB6\x6C\xA5\xF3\xA1\x43";
LegacyPanelRecord gLegacyMailDeletePanel583618 {
    0, 0, 320, 240, 44, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacyMailDeleteYes589728[] = "\x20\xAC\x4F";
constexpr char kLegacyMailDeleteLead589714[] = "\xA6\xDB\xA6\x57\xA4\xF9\xB5\x6E\xBF\xFD\xA5\xBB\xA7\x52\xB0\xA3";
constexpr char kLegacyMailDeleteEnd589708[] = "\xAA\xBA\xA6\x57\xA4\xF9\xB6\xDC\xA1\x48";
constexpr char kLegacyMailDeleteHint589700[] = "\xA7\x52\xB0\xA3\xA1\x43";
constexpr char kLegacyMailKeepHint5896F4[] = "\xA4\xA3\xA7\x52\xB0\xA3\xA1\x43";
// 旧46普通邮件；CB0AA8的60个U8由本页初始化生产，与旧42共享目标和页号。
LegacyPanelRecord gLegacyMailNormalPanel583488 {
    0, 0, 272, 438, 46, {23195, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 60> gLegacyMailSelectedFlagsCB0AA8 {};
constexpr char kLegacyMailChooseRecipients5896A4[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xB0\x65\xAB\x48\xAA\xBA\xB9\xEF\xB6\x48\xA1\x43";
constexpr char kLegacyMailSendSelected5896B8[] = "\xB0\x65\xB6\x6C\xA5\xF3\xB5\xB9\xBF\xEF\xBE\xDC\xAA\xBA\xA6\x6E\xA4\xCD\xA1\x43";
constexpr char kLegacyMailUnselect58968C[] = "\xB0\xB1\xA4\xEE\xB0\x65\xB6\x6C\xA5\xF3\xB5\xB9\xB3\x6F\xA6\xEC\xA6\x6E\xA4\xCD\xA1\x43";
constexpr char kLegacyMailSelect589678[] = "\xB0\x65\xB6\x6C\xA5\xF3\xB5\xB9\xB3\x6F\xA6\xEC\xA6\x6E\xA4\xCD\xA1\x43";
LegacyPanelRecord gLegacyMailSortPanel5836E0 {
    0, 0, 192, 254, 43, {23170, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyMailSortFirst589744[] = "\xB5\x6E\xBF\xFD\xB6\xB6\xA7\xC7";
constexpr char kLegacyMailSortSecond589738[] = "\xAB\xF7\xAD\xB5\xB6\xB6\xA7\xC7";
constexpr char kLegacyMailSortThird58972C[] = "\xA4\x57\xBD\x75\xB6\xB6\xA7\xC7";
// 旧47只借同源MailState消息、联系人及页号，面板独立于现代多类别阅读窗口。
LegacyPanelRecord gLegacyMailReadPanel5833C0 {
    0, 0, 272, 428, 47, {23190, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
// 旧94独立面板与导航状态；成员/排序/职级文本借既有唯一背板，详见旧公会通讯录SPEC。
LegacyPanelRecord gLegacyGuildContactPanel58EF90 {
    0, 0, 272, 451, 94, {238522, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildPage1050B78 = 0; // 52FCF0写入、旧94翻页与四行消费。
std::int32_t gLegacyGuildInitialPage1050DA4 = 0; // 原虚拟零；直接xref只有初始化读取，间接生产仍待。
std::int32_t gLegacyGuildReadTarget1050DB4 = 0; // 旧94发布，旧89读取目标。
std::array<YobiAction*, 4> gLegacyGuildUnreadActions1050DB8 {}; // 专用未读动画的四个非拥有槽。
std::int32_t gLegacyGuildComposeTarget1051340 = 0; // 旧94发布普通撰写初选。
std::int32_t gLegacyGuildComposePage1051330 = 0; // 旧94发布、旧90继续翻页。
std::int32_t gLegacyGuildMailTarget1050DCC = 0; // 旧94发布、旧87宠物发送消费。
std::int32_t gLegacyGuildDeleteTarget105132C = 0; // 旧94发布、旧92删除消费。
std::int32_t gLegacyGuildRankTarget1057C70 = 0; // 旧94发布、旧96职级页消费。
constexpr std::array<std::int32_t, 17> kLegacyGuildStatusImages58F054 {
    23110, 23120, 23121, 23122, 23123, 23124, 23125, 23126, 23127,
    23128, 23129, 23130, 23131, 23132, 23133, 23134, 23135,
};
constexpr char kLegacyGuildClose58F248[] = "\x82\xB1\x82\xCC\x83\x45\x83\x42\x3F\x83\x68\x83\x45\x82\xF0\x95\xC2\x82\xB6\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildPage58F2D4[] = "\x83\x79\x81\x5B\x83\x57\x82\xF0\x90\xD8\x82\xE8\x91\xD6\x82\xA6\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildSort58F4A8[] = "\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xCC\x95\x5C\x8E\xA6\x3F\x94\xD4\x82\xF0\x83\x5C\x81\x5B\x83\x67\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildDetails58F490[] = "\x83\x4D\x3F\x83\x68\x8F\xDA\x8D\xD7\x82\xF0\x95\x5C\x8E\xA6\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildContacts58F474[] = "\x83\x41\x83\x68\x3F\x83\x58\x83\x75\x83\x62\x83\x4E\x82\xF0\x95\x5C\x8E\xA6\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildSend58F2EC[] = "\x82\xB1\x82\xCC\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xC9\x3F\x81\x5B\x3F\x82\xF0\x3F\x82\xE8\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildRead58F44C[] = "\x82\xB1\x82\xCC\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xCC\x3F\x81\x5B\x3F\x82\xCC\x3F\x97\xF0\x82\xF0\x8C\xA9\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildRemove58F42C[] = "\x82\xB1\x82\xCC\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xF0\x8D\xED\x3F\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildRank58F410[] = "\x83\x4D\x3F\x83\x68\x83\x5E\x83\x43\x83\x67\x3F\x82\xF0\x95\xCF\x8D\x58\x82\xB5\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildSortPanel58EEC8 {
    0, 0, 192, 254, 93, {23170, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyGuildSortFirst58F408[] = "\x93\x6F\x98\x5E\x3F";
constexpr char kLegacyGuildSortSecond58F400[] = "\x82\x54\x82\x4F\x89\xB9\x3F";
constexpr char kLegacyGuildSortThird58F3F4[] = "\x83\x49\x3F\x3F\x83\x43\x3F\x3F";
LegacyPanelRecord gLegacyGuildDeletePanel58EE00 {
    0, 0, 320, 240, 92, {}, {}, 0, 0, {}, {},
};
constexpr char kLegacyGuildDeleteYes58F3EC[] = "\x82\xCD\x82\xA2";
constexpr char kLegacyGuildDeleteNo58F3E4[] = "\x82\xA2\x82\xA2\x82\xA6";
constexpr char kLegacyGuildDeleteDenied58F3C8[] = "\xA7\x41\xA8\x53\xA6\xB3\xAD\xE7\xB0\xA3\xAE\x61\xB1\xDA\xA6\xA8\xAD\xFB\xAA\xBA\xC5\x76\xAD\xAD";
constexpr char kLegacyGuildDeleteLead58F3B8[] = "\x83\x4D\x3F\x83\x68\x83\x75\x83\x62\x83\x4E\x82\xA9\x82\xE7";
constexpr char kLegacyGuildDeleteEnd58F3AC[] = "\x82\xF0\x8D\xED\x3F\x82\xB5\x82\xDC\x82\xB7";
constexpr char kLegacyGuildDeleteHint58F3A0[] = "\x8D\xED\x3F\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildKeepHint58F38C[] = "\x8D\xED\x3F\x82\xF0\x8E\xE6\x82\xE8\x82\xE2\x82\xDF\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildDeliveryPanel58ED38 {
    0, 0, 192, 174, 91, {23184, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyGuildNormalMode58F374[] = "\x92\xCA\x8F\xED\x3F\x81\x5B\x3F\x82\xC5\x3F\x90\x4D\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildPetMode58F358[] = "\x3F\x3F\x83\x58\x83\x5E\x81\x5B\x3F\x81\x5B\x3F\x82\xC5\x3F\x90\x4D\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildReturn58F264[] = "\x83\x4D\x3F\x83\x68\x83\x75\x83\x62\x83\x4E\x83\x45\x83\x43\x3F\x83\x68\x83\x45\x82\xC9\x96\xDF\x82\xE8\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildPetPanel58EAE0 {
    0, 0, 272, 238, 88, {23189, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildPetIndex1050DB0 = 0; // 旧88写入、旧87发送消费，独立于普通邮件CC2E60。
constexpr char kLegacyGuildPetHint58F2B4[] = "\x82\xB1\x82\xCC\x8E\x67\x82\xA2\x3F\x82\xC9\x3F\x81\x5B\x3F\x82\xF0\x89\x5E\x82\xCE\x82\xB9\x82\xDC\x82\xB7\x81\x42";
// 旧86独立赠品状态；58EA14已加载-1，其余四槽按原虚拟零初始化。
LegacyPanelRecord gLegacyGuildGiftPanel58E950 {
    368, 0, 272, 252, 86, {23205, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildGiftHover58EA14 = -1;
std::int32_t gLegacyGuildGiftPage1050DAC = 0;
std::int32_t gLegacyGuildGiftSelected1050DA8 = 0; // 旧86生产、旧87消费，不能借普通邮件赠品槽。
std::int32_t gLegacyGuildGiftColumn1050F10 = 0;
std::int32_t gLegacyGuildGiftRow1050DC8 = 0;
constexpr char kLegacyGuildGift58F224[] = "\x82\xB1\x82\xCC\x83\x41\x83\x43\x83\x65\x3F\x82\xF0\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xC9\x3F\x82\xE8\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildComposePanel58EA18 {
    0, 0, 272, 438, 87, {23200, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr char kLegacyGuildSendMail58F288[] = "\x91\x49\x91\xF0\x82\xB5\x82\xC4\x82\xA2\x82\xE9\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xC9\x3F\x81\x5B\x3F\x82\xF0\x3F\x82\xE8\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildReadPanel58EBA8 {
    0, 0, 272, 428, 89, {23190, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildReadPage1050EF8 = 0; // 旧89独立页号，读取同源公会十条邮件。
LegacyPanelRecord gLegacyGuildNormalPanel58EC70 {
    0, 0, 272, 438, 90, {23195, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 200> gLegacyGuildSelectedFlags1050CDC {}; // 旧90多人选择，不包含1050DA4。
constexpr char kLegacyGuildUnselect58F310[] = "\x82\xB1\x82\xCC\x83\x4D\x3F\x83\x68\x3F\x3F\x83\x6F\x81\x5B\x82\xC9\x3F\x81\x5B\x3F\x82\xF0\x3F\x82\xE9\x82\xCC\x82\xF0\x82\xE2\x82\xDF\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildChooseRecipients58F33C[] = "\x3F\x90\x4D\x3F\x8E\xE8\x82\xF0\x91\x49\x91\xF0\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42";
LegacyPanelRecord gLegacyGuildRankPanel58F160 {
    0, 0, 272, 411, 96, {238556, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildRankReady1057C8C = 0;
std::int32_t gLegacyGuildRankPage1051334 = 0;
std::int32_t gLegacyGuildRankPageCount105133C = 0;
std::int32_t gLegacyGuildRankSelected1051338 = 0;
std::array<std::int32_t, 8> gLegacyGuildRankHighlights1050ED8 {};
std::uint32_t gLegacyGuildRankTick1057C90 = 0;
std::int32_t gLegacyGuildEditAllowed1050F14 = 0; // 旧95/96共享资格，532430按本人职级是否0生产。
constexpr char kLegacyGuildRankRequired58F584[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xAE\x61\xB1\xDA\xBA\xD9\xB8\xB9";
constexpr char kLegacyGuildRankRemove58F568[] = "\x83\x4D\x3F\x83\x68\x83\x5E\x83\x43\x83\x67\x3F\x82\xF0\x8A\x4F\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildRankSelect58F4FC[] = "\x83\x4D\x3F\x83\x68\x83\x5E\x83\x43\x83\x67\x3F\x82\xF0\x91\x49\x91\xF0\x82\xB5\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyGuildDetailsPanel58F098 {
    0, 0, 300, 438, 95, {238535, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildDetailsSelected58F15C = -1;
std::array<input::InputContext*, 3> gLegacyGuildDetailsInputs1057C7C {}; // 原三T*缓存，只借1050F18实体。
std::uint32_t gLegacyGuildDetailsTick1057C88 = 0;
std::array<char, 128> gLegacyGuildNameCache1050DD0 {};
std::array<char, 128> gLegacyGuildSecondCache1050E50 {};
std::int32_t gLegacyGuildDetailsPageCount1050ED0 = 0;
std::int32_t gLegacyGuildDetailsPage1050ED4 = 0;
std::array<std::int32_t, 5> gLegacyGuildDetailsHighlights1050EFC {};
constexpr char kLegacyGuildNameEdit58F550[] = "\x83\x4D\x3F\x83\x68\x96\xBC\x82\xF0\x93\xFC\x97\xCD\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildSecondEdit58F534[] = "\x83\x4D\x3F\x83\x68\x83\x6E\x83\x45\x83\x58\x96\xBC\x82\xF0\x93\xFC\x97\xCD\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildRankEdit58F518[] = "\x83\x4D\x3F\x83\x68\x83\x5E\x83\x43\x83\x67\x3F\x82\xF0\x93\xFC\x97\xCD\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildPermissionFirst58F4E4[] = "\x89\xC1\x93\xFC\x3F\x8C\xC0\x82\xF0\x3F\x89\xC2\x82\xB5\x82\xDC\x82\xB7\x81\x42";
constexpr char kLegacyGuildPermissionSecond58F4D0[] = "\x3F\x96\xBC\x3F\x8C\xC0\x82\xF0\x3F\x89\xC2\x82\xB5\x82\xDC\x82\xB7\x81\x42";
LegacyPanelRecord gLegacyEmotePanel584AA8 {
    448, 0, 192, 360, 17, {22640, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr const char* kLegacyEmoteLabels58A370[18] {
    "\x20\xA7\xA4\xA1\x40\xA4\x55\x20", // 原58A370
    "\x20\xAF\xB8\xA1\x40\xA5\xDF\x20", // 原58A364
    "\x20\xB4\xA7\x20\x20\xA4\xE2\x20", // 原58A358
    "\x20\xA8\xAB\xA1\x40\xB8\xF4\x20", // 原58A34C
    "\x20\xC2\x49\x20\x20\xC0\x59\x20", // 原58A340
    "\x20\xAD\xCB\x20\x20\xA4\x55\x20", // 原58A334
    "\x20\xB0\xAA\x20\x20\xBF\xB3\x20", // 原58A328
    "\x20\xA7\xF0\x20\x20\xC0\xBB\x20", // 原58A31C
    "\x20\xBC\xAB\x20\x20\xAB\xE3\x20", // 原58A310
    "\x20\xA8\xBE\x20\x20\xBF\x6D\x20", // 原58A304
    "\x20\xB4\x64\x20\x20\xB6\xCB\x20", // 原58A2F8
    "\x20\xA8\xFC\x20\x20\xB6\xCB\x20", // 原58A2EC
    "\x20\xA7\xEB\x20\x20\xC2\x59\x20", // 原58A2E0
    "\x20\xC5\x5D\x20\x20\xAA\x6B\x20", // 原58A2D4
    "\x20\xB0\xC5\x20\x20\xA4\x4D\x20", // 原58A2C8
    "\x20\xB2\xBE\xB0\xCA\xA4\xA4\x20", // 原58A2BC
    "\x20\x20\x20\xA5\xAC\x20\x20\x20", // 原58A2B0
    "\x20\xA5\xDB\x20\x20\xC0\x59\x20", // 原58A2A4
};
constexpr const char* kLegacyEmoteHints5871B0[18] {
    "\xB0\xB5\xA1\x75\xA7\xA4\xA4\x55\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原5871B0
    "\xB0\xB5\xA1\x75\xAF\xB8\xA5\xDF\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原5870D8
    "\xB0\xB5\xA1\x75\xB4\xA7\xA4\xE2\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587198
    "\xB0\xB5\xA1\x75\xA8\xAB\xB8\xF4\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原5870C0
    "\xB0\xB5\xA1\x75\xC2\x49\xC0\x59\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587180
    "\xB0\xB5\xA1\x75\xAD\xCB\xA4\x55\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原5870A8
    "\xB0\xB5\xA1\x75\xB0\xAA\xBF\xB3\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587168
    "\xB0\xB5\xA1\x75\xA7\xF0\xC0\xBB\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587090
    "\xB0\xB5\xA1\x75\xBC\xAB\xAB\xE3\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587150
    "\xB0\xB5\xA1\x75\xA8\xBE\xBF\x6D\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587078
    "\xB0\xB5\xA1\x75\xB4\x64\xB6\xCB\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587138
    "\xB0\xB5\xA1\x75\xA8\xFC\xB6\xCB\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587060
    "\xB0\xB5\xA1\x75\xA7\xEB\xC2\x59\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587120
    "\xB0\xB5\xA1\x75\xC5\x5D\xAA\x6B\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587048
    "\xB0\xB5\xA1\x75\xA5\x58\xB0\xC5\xA4\x4D\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587108
    "\xB0\xB5\xA1\x75\xB2\xBE\xB0\xCA\xA4\xA4\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587030
    "\xB0\xB5\xA1\x75\xA5\x58\xA5\xAC\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原5870F0
    "\xB0\xB5\xA1\x75\xA5\x58\xA5\xDB\xC0\x59\xA1\x76\xB3\x6F\xAD\xD3\xB0\xCA\xA7\x40\xA1\x43", // 原587018
};
constexpr char kLegacyBlacklistText5533F4[] = "\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\xBC\xF6\xC1\xE4\xA8\xCF\xA5\xCE\xBB\xA1\xA9\xFA\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D";
constexpr char kLegacyBlacklistText5533B4[] = "\xB7\xC6\xB9\xAB\xAE\xD8\xAE\xD8\xB9\xEF\xB7\xC7\xAD\x6E\xA5\x5B\xA4\x4A\xAA\xBA\xA4\x48\x20\x2B\x20\x5B\x43\x54\x52\x4C\x5D\x20\x2B\x20\x5B\x49\x6E\x73\x65\x72\x74\x5D\x20\x3D\x20\xA7\xE2\xA8\xBA\xAD\xD3\xA4\x48\xA9\xF1\xA6\x62\xB6\xC2\xA6\x57\xB3\xE6";
constexpr char kLegacyBlacklistText553370[] = "\xB7\xC6\xB9\xAB\xAE\xD8\xAE\xD8\xB9\xEF\xB7\xC7\xAD\x6E\xB2\xBE\xB0\xA3\xAA\xBA\xA4\x48\x20\x2B\x20\x5B\x43\x54\x52\x4C\x5D\x20\x2B\x20\x5B\x44\x65\x6C\x65\x74\x65\x5D\x20\x3D\x20\xA7\xE2\xA8\xBA\xAD\xD3\xA4\x48\xB1\x71\xB6\xC2\xA6\x57\xB3\xE6\xB2\xBE\xB0\xA3";
constexpr char kLegacyBlacklistText55332C[] = "\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\xA5\xD8\xAB\x65\xA6\x40\xA6\xB3\x25\x32\x64\xA4\x48\xA6\x62\xB6\xC2\xA6\x57\xB3\xE6\xA4\xA4\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D\x2D";
constexpr char kLegacyBlacklistText553304[] = "\xB7\x73\xBC\x57\xA1\xFE\xB2\xBE\xB0\xA3\xB6\xC2\xA6\x57\xB3\xE6\xAA\xBA\xA4\xBA\xAE\x65";
constexpr char kLegacyBlacklistText5532EC[] = "\xA4\x77\xB1\x4E\x25\x73\xB1\x71\xB6\xC2\xA6\x57\xB3\xE6\xA4\xA4\xB2\xBE\xB0\xA3";
constexpr char kLegacyBlacklistText5532D8[] = "\xB1\x71\xB6\xC2\xA6\x57\xB3\xE6\xB2\xBE\xB0\xA3\x25\x73\xA5\xA2\xB1\xD1";
constexpr char kLegacyBlacklistText553294[] = "\xA7\x41\xAA\xBA\xB6\xC2\xA6\x57\xB3\xE6\xA4\x77\xBA\xA1\xA1\x41\xB5\x4C\xAA\x6B\xA6\x41\xA5\x5B\xA4\x4A";
constexpr char kLegacyBlacklistText5532C4[] = "\xA4\x77\xB1\x4E\x25\x73\xA5\x5B\xA4\x4A\xB6\xC2\xA6\x57\xB3\xE6";
constexpr char kLegacyBlacklistText5532B0[] = "\x25\x73\xA5\x5B\xA4\x4A\xB6\xC2\xA6\x57\xB3\xE6\xA5\xA2\xB1\xD1";
LegacyPanelRecord gLegacyBlacklistPanel5531D0 {
    20, 10, 600, 438, 99, {99100, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyBlacklistInput5939B8 = nullptr; // 旧99借用槽，独立于当前输入焦点。
constexpr char kLegacyPresetConfirm58A6C0[] = "\xB5\xB2\xA7\xF4\xB3\x5D\xA9\x77\xB1\x60\xA5\xCE\xA5\x79\xA1\x43";
constexpr char kLegacyPresetCtrl58A68C[] = "\xB1\x4E\x43\x74\x72\x6C\x2B\xA2\xD4\x25\x64\xC1\xE4\xB3\x5D\xA9\x77\xAC\xB0\xB1\x60\xA5\xCE\xA5\x79\xA1\x43";
constexpr char kLegacyPresetShift58A66C[] = "\xB1\x4E\x53\x68\x69\x66\x74\x2B\xA2\xD4\x25\x64\xC1\xE4\xB3\x5D\xA9\x77\xAC\xB0\xB1\x60\xA5\xCE\xA5\x79\xA1\x43";
constexpr char kLegacyPresetNormal58A6A8[] = "\xB1\x4E\xA2\xD4\x25\x64\xC1\xE4\xB3\x5D\xA9\x77\xAC\xB0\xB1\x60\xA5\xCE\xA5\x79\xA1\x43";
constexpr char kLegacyPresetTab585910[] = "\xA4\xC1\xB4\xAB\xA8\xEC\xB2\xC4\xA4\x40\xAD\xB6\xA1\x43";
constexpr char kLegacyPresetTab585900[] = "\xA4\xC1\xB4\xAB\xA8\xEC\xB2\xC4\xA4\x47\xAD\xB6\xA1\x43";
constexpr char kLegacyPresetTab5858F0[] = "\xA4\xC1\xB4\xAB\xA8\xEC\xB2\xC4\xA4\x54\xAD\xB6\xA1\x43";
LegacyPanelRecord gLegacyPresetPanel5850C8 {
    0, 0, 272, 438, 6, {22950, 22951, 22952, 0, 0},
    {{{22953, 24, 43, 52, 18, kLegacyPresetTab585910},
      {22954, 110, 43, 52, 18, kLegacyPresetTab585900},
      {22955, 196, 43, 52, 18, kLegacyPresetTab5858F0}, {}, {}}},
    0, 0, {}, {},
};
std::int32_t gLegacyPresetRowD30408 = 0; // 页号直接借面板selected，只有行号是独立S32。
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

void Sub_4D3180()
{
    std::int32_t mask = 1;
    for (std::int32_t index = 0; index < 6; ++index, mask *= 2) {
        if ((ui::detail::gDialogValueCF4E5C & mask) != 0) {
            static_cast<void>(character_select::detail::Sub_503A20(
                &gLegacyDialogButtons581808[index], 2, 4, 1, gValueD69768, 218));
        }
    }
}

std::int32_t Sub_4D31C0()
{
    std::int32_t mask = 1;
    for (std::int32_t index = 0; index < 6; ++index, mask *= 2) {
        if ((ui::detail::gDialogValueCF4E5C & mask) != 0
            && (character_select::detail::Sub_503990(&gLegacyDialogButtons581808[index]) & 1) != 0) {
            return mask;
        }
    }
    return 0;
}

std::uintptr_t Sub_4D3210(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyDialogPanelD2D9C8, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        std::uintptr_t result = static_cast<std::uint32_t>(
            input::IsCurrentContext_503E40(gLegacyDialogInputD67C4C));
        if (result != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            // 原44B330返回输入指针；焦点归还先于类型/借用槽清理。
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        world::detail::gValue581800 = -1;
        gLegacyDialogInputD67C4C = nullptr;
        return result;
    }
    if ((world::detail::gValue581800 == 1 || world::detail::gValue581800 == 11)
        && gLegacyDialogInputD67C4C == nullptr) {
        gLegacyDialogInputD67C4C = &input::gInputContextCF47E0;
        input::SetContext_44B330(&input::gInputContextCF47E0);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyDialogPanelD2D9C8.id));
    }
    const std::int32_t buttonMask = Sub_4D31C0();
    if (buttonMask != 0) {
        const char* const source = world::detail::gValue581800 == 1 || world::detail::gValue581800 == 11
            ? gLegacyDialogInputD67C4C->text : input::LegacyText593850();
        char buffer[256];
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(source, buffer, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, buttonMask, buffer));
        static_cast<void>(audio::Sub_5042D0(gLegacyDialogPanelD2D9C8.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    for (std::int32_t index = 0; index < gLegacyDialogLineCountCA043C; ++index) {
        if (std::strlen(lines[index].text.data()) != 0) {
            const std::int8_t hoverByte = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gValueD69768));
            const std::int32_t top = gLegacyDialogPanelD2D9C8.y + gLegacyDialogOffsetYCAFEA8 + 16 * index;
            const std::int32_t left = gLegacyDialogPanelD2D9C8.x + gLegacyDialogOffsetXCAFEAC;
            static_cast<void>(DrawText(static_cast<std::int16_t>(left),
                static_cast<std::int16_t>(top), 2, 0, 0, lines[index].text.data(), 0, hoverByte));
        }
    }
    Sub_4D3180();
    if (world::detail::gValue581800 == 1 || world::detail::gValue581800 == 11) {
        render::DrawInputText_4568A0(gLegacyDialogInputD67C4C);
    }
    return Sub_503440(&gLegacyDialogPanelD2D9C8, 0);
}

std::uint32_t Sub_4D3410(const std::int32_t opened)
{
    const std::int32_t state = Sub_503160(opened, &gLegacyLineDialogPanelCC4150, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        world::detail::gValue581800 = -1;
        return 2;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyLineDialogPanelCC4150.id));
    }
    std::int32_t chosen = -1;
    std::int32_t hovered = -1;
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    for (std::int32_t index = 0, rowOffset = 0; index < gLegacyDialogLineCountCA043C;
         ++index, rowOffset += 20) {
        if (index < gLegacyDialogFirstLineD673D4) {
            continue;
        }
        const std::int32_t y = rowOffset + gLegacyDialogOffsetYCAFEA8 + gLegacyLineDialogPanelCC4150.y;
        const std::int32_t columns = gLegacyDialogColumnsCA0010;
        const std::int32_t x = gLegacyDialogOffsetXCAFEAC + gLegacyLineDialogPanelCC4150.x;
        if (Sub_5049A0(x - 2, y - 2, x + 9 * columns - 2, y + 18, -1) == 0) {
            continue;
        }
        hovered = index;
        if ((static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0
            && std::strlen(lines[index].text.data()) != 0) {
            chosen = index;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
        break;
    }
    const std::int32_t buttonMask = Sub_4D31C0();
    if (buttonMask != 0 || chosen >= 0) {
        char buffer[512];
        platform::FormatLegacyString_53B304(buffer, "%d", chosen - gLegacyDialogFirstLineD673D4 + 1);
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, buttonMask, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacyLineDialogPanelCC4150.id));
        if (buttonMask != 0) {
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    for (std::int32_t index = 0, rowOffset = 0; index < gLegacyDialogLineCountCA043C;
         ++index, rowOffset += 20) {
        if (std::strlen(lines[index].text.data()) == 0) {
            continue;
        }
        const std::int8_t hoverByte = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gValueD69768));
        const std::int32_t top = gLegacyLineDialogPanelCC4150.y + gLegacyDialogOffsetYCAFEA8 + 20 * index;
        const std::int32_t left = gLegacyLineDialogPanelCC4150.x + gLegacyDialogOffsetXCAFEAC;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, lines[index].text.data(), 0, hoverByte));
        if (hovered == index) {
            // 文字入队后重读布局；rowOffset原为完整S32，不采用反编译的S16。
            const std::int32_t y = gLegacyDialogOffsetYCAFEA8 + rowOffset + gLegacyLineDialogPanelCC4150.y;
            const std::uint32_t image = gValueD69768;
            const std::int32_t offsetX = gLegacyDialogOffsetXCAFEAC;
            const std::int32_t columns = gLegacyDialogColumnsCA0010;
            const std::int32_t x = offsetX + gLegacyLineDialogPanelCC4150.x;
            DrawGraphicHover(Low16(x - 2), Low16(y - 2), Low16(x + 9 * columns - 2), Low16(y + 18),
                214, image, 0);
        }
    }
    Sub_4D3180();
    return Sub_503440(&gLegacyLineDialogPanelCC4150, 0);
}

YobiAction* Sub_4D66E0()
{
    const std::int32_t y = gLegacySkillChoicePanel581D60.y + 133;
    const std::int32_t x = gLegacySkillChoicePanel581D60.x + 45;
    const std::int32_t image = ui::detail::ReadLegacyChoiceImageCC4890();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}

std::uintptr_t Sub_4D6710(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillChoicePanel581D60.x;
    const std::int32_t y = gLegacySkillChoicePanel581D60.y;
    std::array<character_select::detail::CharacterModalUiRecord, 3> buttons {{
        {x + 137, y + 164, x + 112, y + 164, 136, 18, kLegacySkillChoiceLearn585AC4},
        {x + 137, y + 190, x + 112, y + 190, 136, 18, kLegacySkillChoiceForget588BC8},
        {x + 146, y + 216, x + 112, y + 216, 136, 18, kLegacySkillChoiceCancel588BBC},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacySkillChoicePanel581D60,
        +[] { static_cast<void>(Sub_4D66E0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* result = gLegacySkillChoiceActionD67C58;
        world::detail::gValue581800 = -1;
        if (result != nullptr) {
            result = MarkDeath(result);
            gLegacySkillChoiceActionD67C58 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(result);
    }
    if (gLegacySkillChoiceActionD67C58 != nullptr) {
        gLegacySkillChoiceActionD67C58->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySkillChoicePanel581D60.id));
    }
    for (std::int32_t index = 0; index < 3; ++index) {
        if ((character_select::detail::Sub_503990(&buttons[index]) & 1) == 0) {
            continue;
        }
        if (index <= 1) {
            char buffer[16];
            char escaped[16];
            platform::FormatLegacyString_53B304(buffer, "%d", index + 1);
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        }
        static_cast<void>(audio::Sub_5042D0(gLegacySkillChoicePanel581D60.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    const std::int32_t titleY = gLegacySkillChoicePanel581D60.y + 8;
    const std::int32_t titleX = gLegacySkillChoicePanel581D60.x - titleWidth / 2 + 142;
    static_cast<void>(DrawText(static_cast<std::int16_t>(titleX),
        static_cast<std::int16_t>(titleY), 2, 0, 0, lines[0].text.data(), 0, 0));
    for (std::int32_t index = 0; index < 5; ++index) {
        const std::int32_t top = gLegacySkillChoicePanel581D60.y + 16 * (index + 3);
        const std::int32_t left = gLegacySkillChoicePanel581D60.x + 90;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, lines[index + 1].text.data(), 0, 0));
    }
    for (const auto& button : buttons) {
        static_cast<void>(character_select::detail::Sub_503A20(&button, 2, 0, 1, gValueD69768, 218));
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacySkillChoicePanel581D60) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((character_select::detail::Sub_503990(&buttons[0]) & 0x40) != 0) {
        hint = kLegacySkillChoiceLearnHint588BA8;
    } else if ((character_select::detail::Sub_503990(&buttons[1]) & 0x40) != 0) {
        hint = kLegacySkillChoiceForgetHint588B90;
    } else if ((character_select::detail::Sub_503990(&buttons[2]) & 0x40) != 0) {
        hint = kLegacySkillChoiceCancelHint588B80;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacySkillChoicePanel581D60, 1);
}

YobiAction* Sub_4D6A30()
{
    const std::int32_t y = gLegacySkillLearningPanel581E28.y + 246;
    const std::int32_t x = gLegacySkillLearningPanel581E28.x + 469;
    const std::int32_t image = ui::detail::ReadLegacyChoiceImageCC4890();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}

std::uintptr_t Sub_4D6A60(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillLearningPanel581E28.x;
    const std::int32_t y = gLegacySkillLearningPanel581E28.y;
    std::array<character_select::detail::CharacterModalUiRecord, 2> buttons {{
        {x + 432, y + 344, x + 430, y + 344, 72, 18, kLegacySkillLearningAccept588C20},
        {x + 432, y + 380, x + 430, y + 380, 72, 18, kLegacySkillLearningCancel588C18},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacySkillLearningPanel581E28,
        +[] { static_cast<void>(Sub_4D6A30()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* result = gLegacySkillChoiceActionD67C58;
        world::detail::gValue581800 = -1;
        if (result != nullptr) {
            result = MarkDeath(result);
            gLegacySkillChoiceActionD67C58 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(result);
    }
    if (gLegacySkillChoiceActionD67C58 != nullptr) {
        gLegacySkillChoiceActionD67C58->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySkillLearningPanel581E28.id));
    }
    for (std::int32_t index = 0; index < 2; ++index) {
        if ((character_select::detail::Sub_503990(&buttons[index]) & 1) == 0) {
            continue;
        }
        char buffer[16];
        char escaped[16];
        platform::FormatLegacyString_53B304(buffer, "%d", index + 1);
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        static_cast<void>(audio::Sub_5042D0(gLegacySkillLearningPanel581E28.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    const std::int32_t titleY = gLegacySkillLearningPanel581E28.y + 8;
    const std::int32_t titleX = gLegacySkillLearningPanel581E28.x - titleWidth / 2 + 288;
    static_cast<void>(DrawText(static_cast<std::int16_t>(titleX),
        static_cast<std::int16_t>(titleY), 2, 0, 0, lines[0].text.data(), 0, 0));
    for (std::int32_t index = 0; index < 5; ++index) {
        const std::int32_t top = gLegacySkillLearningPanel581E28.y + 16 * index + 58;
        const std::int32_t left = gLegacySkillLearningPanel581E28.x + 379;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, lines[index + 1].text.data(), 0, 0));
    }
    char text[64];
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    platform::FormatLegacyString_53B304(text, "%7d", gold);
    const std::int32_t goldY = gLegacySkillLearningPanel581E28.y + 287;
    const std::int32_t goldX = gLegacySkillLearningPanel581E28.x + 463;
    static_cast<void>(DrawText(static_cast<std::int16_t>(goldX),
        static_cast<std::int16_t>(goldY), 2, 0, 0, text, 0, 0));
    for (const auto& button : buttons) {
        static_cast<void>(character_select::detail::Sub_503A20(&button, 2, 0, 1, gValueD69768, 218));
    }
    const std::int32_t nameWidth = TextMeasure(lines[9].text.data(), 0);
    const std::int32_t nameY = gLegacySkillLearningPanel581E28.y + 41;
    const std::int32_t nameX = gLegacySkillLearningPanel581E28.x - nameWidth / 2 + 182;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
        static_cast<std::int16_t>(nameY), 2, 0, 0, lines[9].text.data(), 0, 0));
    for (std::int32_t index = 0; index < 8; ++index) {
        const std::int32_t top = gLegacySkillLearningPanel581E28.y + 20 * index + 116;
        const std::int32_t left = gLegacySkillLearningPanel581E28.x + 45;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, lines[index + 10].text.data(), 0, 0));
    }
    const std::int32_t slots = ui::detail::gWindow54ValueCC2E68;
    platform::FormatLegacyString_53B304(text, kLegacySkillLearningSlots588C00, slots);
    const std::int32_t slotsY = gLegacySkillLearningPanel581E28.y + 324;
    const std::int32_t slotsX = gLegacySkillLearningPanel581E28.x + 45;
    static_cast<void>(DrawText(static_cast<std::int16_t>(slotsX),
        static_cast<std::int16_t>(slotsY), 2, 0, 0, text, 0, 0));
    const std::int32_t cost = ui::detail::gWindow54ValueCF4A3C;
    platform::FormatLegacyString_53B304(text, kLegacySkillLearningCost588BEC, cost);
    const std::int32_t costY = gLegacySkillLearningPanel581E28.y + 372;
    const std::int32_t costX = gLegacySkillLearningPanel581E28.x + 96;
    static_cast<void>(DrawText(static_cast<std::int16_t>(costX),
        static_cast<std::int16_t>(costY), 2, 0, 0, text, 0, 0));
    const char* hint = nullptr;
    if (Sub_503590(&gLegacySkillLearningPanel581E28) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((character_select::detail::Sub_503990(&buttons[0]) & 0x40) != 0) {
        hint = kLegacySkillLearningAcceptHint588BE0;
    } else if ((character_select::detail::Sub_503990(&buttons[1]) & 0x40) != 0) {
        hint = kLegacySkillLearningCancelHint588BD4;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacySkillLearningPanel581E28, 1);
}

YobiAction* Sub_4D6E70()
{
    const std::int32_t y = gLegacySkillForgetPanel581EF0.y + 246;
    const std::int32_t x = gLegacySkillForgetPanel581EF0.x + 469;
    const std::int32_t image = ui::detail::ReadLegacyChoiceImageCC4890();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}

std::uintptr_t Sub_4D6EA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillForgetPanel581EF0.x;
    const std::int32_t y = gLegacySkillForgetPanel581EF0.y;
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 400, y + 394, x + 398, y + 394, 140, 18, kLegacySkillForgetCancel588CBC,
    };
    std::int32_t selection = 0;
    const std::int32_t state = Sub_503160(opened, &gLegacySkillForgetPanel581EF0,
        +[] { static_cast<void>(Sub_4D6E70()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* result = gLegacySkillChoiceActionD67C58;
        world::detail::gValue581800 = -1;
        if (result != nullptr) {
            result = MarkDeath(result);
            gLegacySkillChoiceActionD67C58 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(result);
    }
    if (gLegacySkillChoiceActionD67C58 != nullptr) {
        gLegacySkillChoiceActionD67C58->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySkillForgetPanel581EF0.id));
    }
    if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        selection = 99;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    std::int32_t hovered = -1;
    std::int32_t scanIndex = 0;
    for (std::int32_t rowOffset = 0; scanIndex < 15; ++scanIndex, rowOffset += 24) {
        const std::int32_t top = gLegacySkillForgetPanel581EF0.y + rowOffset + 142;
        const std::int32_t left = gLegacySkillForgetPanel581EF0.x + 47;
        if (Sub_5049A0(left, top, left + 258, top + 24, -1) == 0) {
            continue;
        }
        hovered = scanIndex;
        if ((static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
            selection = scanIndex + 1;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
        break;
    }
    if (selection != 0) {
        // 原CB01A4+15*4恰为独立CB01E0；借真实页号，不扩表或默认准许取消。
        const std::int32_t value = scanIndex == 15 ? ui::detail::gBattleItemPageCB01E0
            : ui::detail::gWindow54ValuesCB01A4[scanIndex];
        if (value >= 0) {
            char buffer[16];
            char escaped[16];
            platform::FormatLegacyString_53B304(buffer, "%d", selection);
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
            static_cast<void>(audio::Sub_5042D0(gLegacySkillForgetPanel581EF0.id));
        }
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY) {
        const std::int32_t top = gLegacySkillForgetPanel581EF0.y + offsetY;
        const std::int32_t left = gLegacySkillForgetPanel581EF0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - titleWidth / 2, 8);
    for (std::int32_t index = 0; index < 5; ++index) {
        drawAt(lines[index + 1].text.data(), 379, 58 + 16 * index);
    }
    drawAt(kLegacySkillForgetNotes588CA4, 406, 280);
    drawAt(kLegacySkillForgetWarning588C94, 382, 310);
    drawAt(kLegacySkillForgetNoRestore588C88, 382, 330);
    drawAt(kLegacySkillForgetCareful588C7C, 382, 350);
    drawAt(kLegacySkillForgetSelect588C68, 70, 100);
    std::int32_t total = 0;
    char text[256];
    for (std::int32_t index = 0, rowOffset = 0; index < 15; ++index, rowOffset += 24) {
        if (ui::detail::gWindow54ValuesCB01A4[index] < 0) {
            continue;
        }
        total += ui::detail::gWindow54ValuesCB09D8[index];
        drawAt(lines[index + 10].text.data(), 78, 145 + rowOffset);
        platform::FormatLegacyString_53B304(text, "%2d", ui::detail::gWindow54ValuesCB01A4[index]);
        drawAt(text, 245, 145 + rowOffset);
        platform::FormatLegacyString_53B304(text, "%d", ui::detail::gWindow54ValuesCB09D8[index]);
        drawAt(text, 292, 145 + rowOffset);
        const std::int32_t imageY = gLegacySkillForgetPanel581EF0.y + rowOffset + 154;
        const std::int32_t imageX = gLegacySkillForgetPanel581EF0.x + 176;
        static_cast<void>(DrawGraphic(imageX, imageY, 212, 22703, 0, nullptr));
        if (hovered == index) {
            const std::int32_t hoverY = gLegacySkillForgetPanel581EF0.y + rowOffset + 142;
            const std::uint32_t image = gValueD69768;
            const std::int32_t hoverX = gLegacySkillForgetPanel581EF0.x + 47;
            DrawGraphicHover(Low16(hoverX), Low16(hoverY), Low16(hoverX + 258), Low16(hoverY + 24),
                214, image, 0);
        }
    }
    platform::FormatLegacyString_53B304(text, kLegacySkillForgetTotal588C48, total);
    drawAt(text, 14, 42);
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 0, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if (Sub_503590(&gLegacySkillForgetPanel581EF0) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacySkillForgetCancelHint588C38;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    if (hovered >= 0 && ui::detail::gWindow54ValuesCB01A4[hovered] >= 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillForgetRowHint588C28);
    }
    return Sub_503440(&gLegacySkillForgetPanel581EF0, 1);
}

std::uint32_t Sub_4EFB70(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillStatusPanel583C90.x;
    const std::int32_t y = gLegacySkillStatusPanel583C90.y;
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 169, y + 202, x + 167, y + 202, 57, 18, kLegacyItemUseCancel589008,
    };
    const character_select::detail::CharacterModalUiRecord previous {
        x + 289, y + 202, x + 287, y + 202, 57, 18, kLegacyDialogButton58597C,
    };
    const std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacySkillStatusPanel583C90, nullptr, 0));
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::uint8_t pending = gLegacySkillPendingD67C48;
        if (pending != 0) {
            gLegacySkillPendingD67C48 = 0;
            return pending; // 原mov AL高W24来自状态2，均为0；保留原字节值。
        }
        const std::uint8_t flag = ui::detail::gValue5F9754;
        if (flag == 0) {
            return 0;
        }
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        const std::uint32_t result = std::bit_cast<std::uint32_t>(
            network::Sub_511DD0(socket, playerX, playerY, 62));
        ui::detail::gValue5F9754 = 0;
        return result;
    }
    const auto rows = ui::detail::BorrowLegacySkillStatusView();
    std::int32_t hovered = -1;
    for (std::int32_t index = 0; index < *rows.count; ++index) {
        const std::int32_t top = gLegacySkillStatusPanel583C90.y + 20 * index + 74;
        const std::int32_t left = gLegacySkillStatusPanel583C90.x + 44;
        if (Sub_5049A0(left, top, left + 424, top + 20, -1) != 0) {
            hovered = index;
            break;
        }
    }
    const std::int32_t cancelState = character_select::detail::Sub_503990(&cancel);
    const std::int32_t previousState = character_select::detail::Sub_503990(&previous);
    char buffer[128];
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        buffer[0] = '\0';
        std::strcpy(ui::detail::gSkillTargetNameD2F748.data(), rows.names[hovered].data());
        cg::NullSub_509080(); // 原只ret，传入的缓冲指针不被消费。
        const std::int32_t target = rows.targets[hovered];
        const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
        const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
        const auto socket = platform::gConnectionSocket;
        const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
        static_cast<void>(network::Sub_511EF0(socket, skill, level, target, buffer));
        static_cast<void>(audio::Sub_5042D0(gLegacySkillStatusPanel583C90.id));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
        gLegacySkillPendingD67C48 = 1;
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySkillStatusPanel583C90.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((previousState & 1) != 0) {
        const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
        const auto socket = platform::gConnectionSocket;
        const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
        static_cast<void>(network::Sub_511C30(socket, skill));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        gLegacySkillPendingD67C48 = 1;
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacySkillStatusPanel583C90.y + offsetY;
        const std::int32_t left = gLegacySkillStatusPanel583C90.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    drawAt(kLegacySkillStatusPrompt589B58, 172, 24, 4);
    drawAt(kLegacySkillStatusHeader589B2C, 44, 50, 0);
    for (std::int32_t index = 0; index < *rows.count; ++index) {
        platform::FormatLegacyString_53B304(buffer, "%3d", rows.levels[index]);
        drawAt(buffer, 107, 76 + 20 * index, 0);
        platform::FormatLegacyString_53B304(buffer, "%-16s", rows.names[index].data());
        const std::int8_t color = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(colors[rows.kinds[index]]));
        drawAt(buffer, 143, 76 + 20 * index, color);
        const std::int32_t maxFp = rows.maxFp[index];
        const std::int32_t fp = rows.fp[index];
        const std::int32_t maxHp = rows.maxHp[index];
        const std::int32_t hp = rows.hp[index];
        platform::FormatLegacyString_53B304(buffer, "%4d/%4d %4d/%4d", hp, maxHp, fp, maxFp);
        drawAt(buffer, 296, 76 + 20 * index, 0);
        const std::int32_t image = ui::detail::ReadLegacyHealthImage(rows.injuries[index]);
        const std::int32_t imageY = gLegacySkillStatusPanel583C90.y + 20 * index + 84;
        const std::int32_t imageX = gLegacySkillStatusPanel583C90.x + 70;
        static_cast<void>(DrawGraphic(imageX, imageY, 213, image, 0, nullptr));
    }
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&previous, 2, 4, 1, gValueD69768, 218));
    if (hovered >= 0) {
        const std::int32_t top = gLegacySkillStatusPanel583C90.y + 20 * hovered + 74;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacySkillStatusPanel583C90.x + 44;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 424), Low16(top + 20), 218, image, 0);
    }
    const char* hint = nullptr;
    if ((cancelState & 0x40) != 0) {
        switch (ui::detail::gSkillTargetKindCF4A10) {
        case 0: hint = kLegacySkillStatusCancelTransform589B20; break;
        case 1: hint = kLegacySkillStatusCancelDisguise589B14; break;
        case 2: hint = kLegacySkillStatusCancelRestore589B08; break;
        case 3: hint = kLegacySkillStatusCancelHeal589AFC; break;
        }
    } else if (hovered >= 0) {
        const std::int32_t mode = ui::detail::gSkillTargetKindCF4A10;
        const std::int32_t kind = rows.kinds[hovered];
        switch (mode) {
        case 0: hint = kLegacySkillStatusTransform589AEC; break;
        case 1: hint = kLegacySkillStatusDisguise589ADC; break;
        case 2:
        case 3:
            if (kind == 0) {
                hint = kLegacySkillStatusSelf589ACC;
            } else if (kind == 1 || kind == 2) {
                hint = kLegacySkillStatusPerson589AA8;
            } else if (kind == 4 || kind == 5 || kind == 6) {
                hint = kLegacySkillStatusPet589AB8;
            }
            break;
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacySkillStatusPanel583C90, 0);
}

std::uint32_t Sub_4F0460(const std::int32_t opened)
{
    const std::int32_t x = gLegacySkillTargetPanel583D58.x;
    const std::int32_t y = gLegacySkillTargetPanel583D58.y;
    const std::array<character_select::detail::CharacterModalUiRecord, 2> cancels {{
        {x + 133, y + 162, x + 131, y + 162, 57, 18, kLegacyItemUseCancel589008},
        {x + 79, y + 162, x + 77, y + 162, 57, 18, kLegacyItemUseCancel589008},
    }};
    const std::array<character_select::detail::CharacterModalUiRecord, 2> pages {{
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetNext589360},
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetPrevious589358},
    }};
    const std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacySkillTargetPanel583D58, nullptr, 0));
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::uint8_t pending = gLegacySkillPendingD67C48;
        if (pending != 0) {
            gLegacySkillPendingD67C48 = 0;
            return pending; // 原高W24来自完整状态2，返回原低W8值。
        }
        const std::uint8_t flag = ui::detail::gValue5F9754;
        if (flag == 0) {
            return 0;
        }
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        const std::uint32_t result = std::bit_cast<std::uint32_t>(
            network::Sub_511DD0(socket, playerX, playerY, 62));
        ui::detail::gValue5F9754 = 0;
        return result;
    }
    const bool manyPages = ui::detail::gSkillTargetPageCountCA0178 > 1;
    std::int32_t page = ui::detail::gSkillTargetPageC8A974;
    std::int32_t hovered = -1;
    for (std::int32_t index = 0; index < 5; ++index) {
        if (index + 5 * page >= ui::detail::gSkillTargetCountCA0004) {
            continue;
        }
        const std::int32_t top = gLegacySkillTargetPanel583D58.y + 20 * index + 50;
        const std::int32_t left = gLegacySkillTargetPanel583D58.x + 86;
        if (Sub_5049A0(left, top, left + 148, top + 20, -1) != 0) {
            hovered = index;
            break;
        }
        page = ui::detail::gSkillTargetPageC8A974;
    }
    const auto* const cancel = &cancels[manyPages ? 1 : 0];
    const std::int32_t cancelState = character_select::detail::Sub_503990(cancel);
    const std::int32_t pageState = manyPages
        ? character_select::detail::Sub_503990(&pages[ui::detail::gSkillTargetPageC8A974]) : 0;
    char buffer[128];
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        const std::int32_t selected = hovered + 5 * ui::detail::gSkillTargetPageC8A974;
        const std::int32_t mode = ui::detail::gSkillTargetKindCF4A10;
        ui::detail::gSkillSelectedTargetCC4B1C = selected;
        if (mode == 0 || ui::detail::gSkillTargetKindsD2F75C[selected] == 3) {
            buffer[0] = '\0';
            std::strcpy(ui::detail::gSkillTargetNameD2F748.data(),
                ui::detail::gSkillTargetNamesCB0550[selected].data());
            cg::NullSub_509080(); // 原C3空叶不消费缓冲指针。
            const std::int32_t target = ui::detail::gSkillSelectedTargetCC4B1C;
            const std::int32_t level = ui::detail::ReadSkillRequestD64E13();
            const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
            const auto socket = platform::gConnectionSocket;
            const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
            static_cast<void>(network::Sub_511EF0(socket, skill, level, target, buffer));
        } else {
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_511C80(socket, selected));
        }
        gLegacySkillPendingD67C48 = 1;
        static_cast<void>(audio::Sub_5042D0(gLegacySkillTargetPanel583D58.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySkillTargetPanel583D58.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((pageState & 1) != 0) {
        ui::detail::gSkillTargetPageC8A974 = (ui::detail::gSkillTargetPageC8A974 - 1) & 1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacySkillTargetPanel583D58.y + offsetY;
        const std::int32_t left = gLegacySkillTargetPanel583D58.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    drawAt(kLegacyItemUseTitle585F54, 58, 24, 4);
    if (manyPages) {
        const std::int32_t currentPage = ui::detail::gSkillTargetPageC8A974;
        const std::int32_t pageCount = ui::detail::gSkillTargetPageCountCA0178;
        platform::FormatLegacyString_53B304(buffer, "%d/%d", currentPage + 1, pageCount);
        drawAt(buffer, 270, 24, 0);
    }
    page = ui::detail::gSkillTargetPageC8A974;
    for (std::int32_t index = 0; index < 5; ++index) {
        const std::int32_t absolute = index + 5 * page;
        if (absolute < ui::detail::gSkillTargetCountCA0004) {
            const char* const name = ui::detail::gSkillTargetNamesCB0550[absolute].data();
            const std::int8_t color = std::bit_cast<std::int8_t>(
                static_cast<std::uint8_t>(colors[ui::detail::gSkillTargetKindsD2F75C[absolute]]));
            drawAt(name, 88, 52 + 20 * index, color);
            page = ui::detail::gSkillTargetPageC8A974;
        }
    }
    static_cast<void>(character_select::detail::Sub_503A20(cancel, 2, 4, 1, gValueD69768, 218));
    if (manyPages) {
        const auto* const button = &pages[ui::detail::gSkillTargetPageC8A974];
        static_cast<void>(character_select::detail::Sub_503A20(button, 2, 4, 1, gValueD69768, 218));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacySkillTargetPanel583D58.y + 20 * hovered + 50;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacySkillTargetPanel583D58.x + 86;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 148), Low16(top + 20), 218, image, 0);
    }
    const char* hint = nullptr;
    if ((cancelState & 0x40) != 0) {
        switch (ui::detail::gSkillTargetKindCF4A10) {
        case 0: hint = kLegacySkillStatusCancelTransform589B20; break;
        case 1: hint = kLegacySkillStatusCancelDisguise589B14; break;
        case 2: hint = kLegacySkillStatusCancelRestore589B08; break;
        case 3: hint = kLegacySkillStatusCancelHeal589AFC; break;
        }
    } else if (hovered >= 0) {
        const std::int32_t absolute = hovered + 5 * ui::detail::gSkillTargetPageC8A974;
        const std::int32_t mode = ui::detail::gSkillTargetKindCF4A10;
        const std::int32_t kind = ui::detail::gSkillTargetKindsD2F75C[absolute];
        switch (mode) {
        case 0: hint = kLegacySkillTargetTransform589BD8; break;
        case 1:
            if (kind == 0) {
                hint = kLegacySkillTargetDisguiseSelf589BC4;
            } else if (kind == 1 || kind == 2) {
                hint = kLegacySkillTargetDisguisePerson589BAC;
            } else if (kind == 3) {
                hint = kLegacySkillStatusDisguise589ADC;
            }
            break;
        case 2:
        case 3:
            if (kind == 0) {
                hint = kLegacySkillTargetRestoreSelf589B98;
            } else if (kind == 1 || kind == 2) {
                hint = kLegacySkillTargetRestorePerson589B80;
            } else if (kind == 3) {
                hint = kLegacySkillStatusPet589AB8;
            }
            break;
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacySkillTargetPanel583D58, 0);
}

std::uint32_t Sub_4E4A50(const std::int32_t opened)
{
    const std::int32_t x = gLegacyItemTargetPanel582D40.x;
    const std::int32_t y = gLegacyItemTargetPanel582D40.y;
    const std::array<character_select::detail::CharacterModalUiRecord, 2> cancels {{
        {x + 133, y + 162, x + 131, y + 162, 57, 18, kLegacyItemUseCancel589008},
        {x + 79, y + 162, x + 77, y + 162, 57, 18, kLegacyItemUseCancel589008},
    }};
    const std::array<character_select::detail::CharacterModalUiRecord, 2> pages {{
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetNext589360},
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetPrevious589358},
    }};
    const std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacyItemTargetPanel582D40, nullptr, 0));
    if (state == 0 || state == 2) {
        return state;
    }
    const auto rows = ui::detail::BorrowLegacyItemUseTargetView();
    const bool manyPages = *rows.pageCount > 1;
    std::int32_t page = *rows.page;
    std::int32_t hovered = -1;
    for (std::int32_t index = 0; index < 5; ++index) {
        if (index + 5 * page >= *rows.count) {
            continue;
        }
        const std::int32_t top = gLegacyItemTargetPanel582D40.y + 20 * index + 50;
        const std::int32_t left = gLegacyItemTargetPanel582D40.x + 86;
        if (Sub_5049A0(left, top, left + 148, top + 20, -1) != 0) {
            hovered = index;
            break;
        }
        page = *rows.page;
    }
    const auto* const cancel = &cancels[manyPages ? 1 : 0];
    const std::int32_t cancelState = character_select::detail::Sub_503990(cancel);
    const std::int32_t pageState = manyPages ? character_select::detail::Sub_503990(&pages[*rows.page]) : 0;
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        const std::int32_t currentPage = *rows.page;
        const auto socket = platform::gConnectionSocket;
        const std::int32_t selected = hovered + 5 * currentPage;
        *rows.selected = selected;
        static_cast<void>(network::Sub_511D10(socket, selected));
        static_cast<void>(audio::Sub_5042D0(gLegacyItemTargetPanel582D40.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyItemTargetPanel582D40.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((pageState & 1) != 0) {
        *rows.page = (*rows.page - 1) & 1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyItemTargetPanel582D40.y + offsetY;
        const std::int32_t left = gLegacyItemTargetPanel582D40.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    drawAt(kLegacyItemUseTitle585F54, 58, 24, 4);
    if (manyPages) {
        const std::int32_t currentPage = *rows.page;
        const std::int32_t pageCount = *rows.pageCount;
        char buffer[128];
        platform::FormatLegacyString_53B304(buffer, "%d/%d", currentPage + 1, pageCount);
        drawAt(buffer, 270, 24, 0);
    }
    page = *rows.page;
    for (std::int32_t index = 0; index < 5; ++index) {
        const std::int32_t absolute = index + 5 * page;
        if (absolute < *rows.count) {
            const std::byte* const record = rows.records + 20 * absolute;
            std::int16_t kind;
            std::memcpy(&kind, record, sizeof(kind));
            const char* const name = reinterpret_cast<const char*>(record + 2);
            const std::int8_t color = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(colors[kind]));
            drawAt(name, 88, 52 + 20 * index, color);
            page = *rows.page;
        }
    }
    static_cast<void>(character_select::detail::Sub_503A20(cancel, 2, 4, 1, gValueD69768, 218));
    if (manyPages) {
        const auto* const button = &pages[*rows.page];
        static_cast<void>(character_select::detail::Sub_503A20(button, 2, 4, 1, gValueD69768, 218));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyItemTargetPanel582D40.y + 20 * hovered + 50;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacyItemTargetPanel582D40.x + 86;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 148), Low16(top + 20), 218, image, 0);
    }
    return Sub_503440(&gLegacyItemTargetPanel582D40, 0);
}

std::uint32_t Sub_4E4EA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyTradeTargetPanel582E08.x;
    const std::int32_t y = gLegacyTradeTargetPanel582E08.y;
    const std::array<character_select::detail::CharacterModalUiRecord, 2> cancels {{
        {x + 133, y + 162, x + 131, y + 162, 57, 18, kLegacyItemUseCancel589008},
        {x + 79, y + 162, x + 77, y + 162, 57, 18, kLegacyItemUseCancel589008},
    }};
    const std::array<character_select::detail::CharacterModalUiRecord, 2> pages {{
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetNext589360},
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetPrevious589358},
    }};
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacyTradeTargetPanel582E08, nullptr, 0));
    if (state == 0 || state == 2) {
        return state;
    }
    const bool manyPages = gLegacyTradeTargetPageCountCF46D0 > 1;
    std::int32_t page = gLegacyTradeTargetPageCC3208;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (row + 5 * page >= gLegacyTradeTargetCountD30134) {
            continue;
        }
        const std::int32_t top = gLegacyTradeTargetPanel582E08.y + 20 * row + 50;
        const std::int32_t left = gLegacyTradeTargetPanel582E08.x + 86;
        if (Sub_5049A0(left, top, left + 148, top + 20, -1) != 0) {
            hovered = row;
            break;
        }
        page = gLegacyTradeTargetPageCC3208;
    }
    const auto* const cancel = &cancels[manyPages ? 1 : 0];
    const std::int32_t cancelState = character_select::detail::Sub_503990(cancel);
    const std::int32_t pageState = manyPages
        ? character_select::detail::Sub_503990(&pages[gLegacyTradeTargetPageCC3208]) : 0;
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        const std::int32_t currentPage = gLegacyTradeTargetPageCC3208;
        const auto socket = platform::gConnectionSocket;
        const std::int32_t selected = hovered + 5 * currentPage;
        gLegacyTradeTargetSelectionCC4D48 = selected;
        static_cast<void>(network::Sub_511FF0(socket, selected));
        const char* const name = gLegacyTradeTargetNamesCB1D1C[gLegacyTradeTargetSelectionCC4D48].data();
        static_cast<void>(ui::detail::Sub_4E5620(name));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyTradeTargetPanel582E08.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((pageState & 1) != 0) {
        gLegacyTradeTargetPageCC3208 = (gLegacyTradeTargetPageCC3208 - 1) & 1;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 58),
        static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 24),
        2, 0, 4, kLegacyItemUseTitle585F54, 0, 0));
    if (manyPages) {
        const std::int32_t currentPage = gLegacyTradeTargetPageCC3208;
        const std::int32_t pageCount = gLegacyTradeTargetPageCountCF46D0;
        char buffer[128];
        platform::FormatLegacyString_53B304(buffer, "%d/%d", currentPage + 1, pageCount);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 270),
            static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 24), 2, 0, 0, buffer, 0, 0));
    }
    page = gLegacyTradeTargetPageCC3208;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t index = row + 5 * page;
        if (index < gLegacyTradeTargetCountD30134) {
            const char* const name = gLegacyTradeTargetNamesCB1D1C[index].data();
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 88),
                static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 52 + 20 * row), 2, 0, 0, name, 0, 0));
            page = gLegacyTradeTargetPageCC3208;
        }
    }
    static_cast<void>(character_select::detail::Sub_503A20(cancel, 2, 4, 1, gValueD69768, 218));
    if (manyPages) {
        const auto* const button = &pages[gLegacyTradeTargetPageCC3208];
        static_cast<void>(character_select::detail::Sub_503A20(button, 2, 4, 1, gValueD69768, 218));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyTradeTargetPanel582E08.y + 50 + 20 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyTradeTargetPanel582E08.x + 86;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 148), Low16(top + 20), 218, color, 0);
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeTargetHint589368);
    } else if ((cancelState & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeCancel5889AC);
    }
    return Sub_503440(&gLegacyTradeTargetPanel582E08, 0);
}

std::uint32_t Sub_4F0CD0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyTextNoticePanel583EE8.x;
    const std::int32_t y = gLegacyTextNoticePanel583EE8.y;
    const character_select::detail::CharacterModalUiRecord confirm {
        x + 162, y + 64, x + 160, y + 64, 36, 18, kLegacyFpConfirm55C5D4,
    };
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacyTextNoticePanel583EE8, nullptr, 0));
    if (state == 0 || state == 2) {
        return state;
    }
    if ((character_select::detail::Sub_503990(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyTextNoticePanel583EE8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[256];
    platform::FormatLegacyString_53B304(buffer, kLegacyTextNotice589C24);
    const std::int32_t firstY = gLegacyTextNoticePanel583EE8.y + 24;
    const std::int32_t firstX = gLegacyTextNoticePanel583EE8.x + 16;
    static_cast<void>(DrawText(static_cast<std::int16_t>(firstX),
        static_cast<std::int16_t>(firstY), 2, 0, 0, buffer, 0, 0));
    // 原D2F798额外变参不被无25H的格式消费；不制造未知值或默认owner。
    platform::FormatLegacyString_53B304(buffer, kLegacyTextNotice589C08);
    const std::int32_t secondY = gLegacyTextNoticePanel583EE8.y + 44;
    const std::int32_t secondX = gLegacyTextNoticePanel583EE8.x + 16;
    static_cast<void>(DrawText(static_cast<std::int16_t>(secondX),
        static_cast<std::int16_t>(secondY), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&confirm, 2, 4, 1, gValueD69768, 218));
    return Sub_503440(&gLegacyTextNoticePanel583EE8, 0);
}

std::uint32_t Sub_4F0E20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyTextNoticePanel583FB0.x;
    const std::int32_t y = gLegacyTextNoticePanel583FB0.y;
    const character_select::detail::CharacterModalUiRecord confirm {
        x + 162, y + 64, x + 160, y + 64, 36, 18, kLegacyFpConfirm55C5D4,
    };
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacyTextNoticePanel583FB0, nullptr, 0));
    if (state == 0 || state == 2) {
        return state;
    }
    if ((character_select::detail::Sub_503990(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyTextNoticePanel583FB0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[256];
    platform::FormatLegacyString_53B304(buffer, kLegacyTextNotice589C4C);
    const std::int32_t firstY = gLegacyTextNoticePanel583FB0.y + 24;
    const std::int32_t firstX = gLegacyTextNoticePanel583FB0.x + 16;
    static_cast<void>(DrawText(static_cast<std::int16_t>(firstX),
        static_cast<std::int16_t>(firstY), 2, 0, 0, buffer, 0, 0));
    // 原CF3B08额外变参不被固定格式消费；仅消去无可观察结果的普通读。
    platform::FormatLegacyString_53B304(buffer, kLegacyTextNotice589C34);
    const std::int32_t secondY = gLegacyTextNoticePanel583FB0.y + 44;
    const std::int32_t secondX = gLegacyTextNoticePanel583FB0.x + 16;
    static_cast<void>(DrawText(static_cast<std::int16_t>(secondX),
        static_cast<std::int16_t>(secondY), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&confirm, 2, 4, 1, gValueD69768, 218));
    return Sub_503440(&gLegacyTextNoticePanel583FB0, 0);
}

YobiAction* Sub_4D7440()
{
    const std::int32_t y = gLegacyPetSkillCatalogPanel581FB8.y;
    const std::int32_t x = gLegacyPetSkillCatalogPanel581FB8.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillCatalogActionD67C5C = action;
    return action;
}

// 原旧58完整消费路径；目录、图号、页数三项定义/生产尚缺，不提前注册页面。
std::uint32_t Sub_4D7470(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillCatalogPanel581FB8.x;
    const std::int32_t y = gLegacyPetSkillCatalogPanel581FB8.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 52, x + 126, y + 40, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 52, x + 216, y + 40, 28, 24, 23023, 23027};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillCatalogPanel581FB8,
        +[] { static_cast<void>(Sub_4D7440()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* const action = gLegacyPetSkillCatalogActionD67C5C;
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            static_cast<void>(MarkDeath(action));
            gLegacyPetSkillCatalogActionD67C5C = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    if (gLegacyPetSkillCatalogActionD67C5C != nullptr) {
        gLegacyPetSkillCatalogActionD67C5C->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 8; ++row) {
        if (gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].used != 0
            && Sub_5049A0(gLegacyPetSkillCatalogPanel581FB8.x + 18,
                gLegacyPetSkillCatalogPanel581FB8.y + 84 + 32 * row,
                gLegacyPetSkillCatalogPanel581FB8.x + 345,
                gLegacyPetSkillCatalogPanel581FB8.y + 105 + 32 * row, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        const std::int32_t index = hovered + 8 * gLegacyPetSkillCatalogPageD45F74;
        if (world::detail::ReadPlayerValueFC() < gLegacyPetSkillCatalogCA0478[index].price08) {
            gLegacyPetSkillCatalogMessageD2E818 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else if (world::detail::gPetCountDE0B00 == 0) {
            gLegacyPetSkillCatalogMessageD2E818 = 2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            char buffer[16];
            char escaped[16];
            gLegacyPetSkillCatalogSelectionC8AF70 = hovered;
            platform::FormatLegacyString_53B304(buffer, "%d", index);
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
            static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        --gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogPageD45F74 < 0) {
            gLegacyPetSkillCatalogPageD45F74 = gLegacyPetSkillCatalogPageCountC8AF84 - 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        ++gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogPageD45F74 >= gLegacyPetSkillCatalogPageCountC8AF84) {
            gLegacyPetSkillCatalogPageD45F74 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 8),
        2, 0, 0, lines[0].text.data(), 0, 0));
    for (std::int32_t row = 0; row < 4; ++row) {
        const char* const text = lines[1 + 4 * gLegacyPetSkillCatalogMessageD2E818 + row].text.data();
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 59 + 20 * row),
            2, 0, 0, text, 0, 0));
    }
    char buffer[256];
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 286), 2, 0, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%d/%d", gLegacyPetSkillCatalogPageD45F74 + 1, gLegacyPetSkillCatalogPageCountC8AF84);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 172),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 44), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t index = row + 8 * gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogCA0478[index].used == 0) {
            continue;
        }
        const std::int32_t nameWidth = TextMeasure(gLegacyPetSkillCatalogCA0478[index].text0C.data(), 0);
        const char* const name = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 97 - nameWidth / 2),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 86 + 32 * row), 2, 0, 0, name, 0, 0));
        const std::int32_t value = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
        platform::FormatLegacyString_53B304(buffer, "%4d", value);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 203),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 88 + 32 * row), 2, 1, 0, buffer, 0, 0));
        const std::int32_t page = gLegacyPetSkillCatalogPageD45F74;
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t price = gLegacyPetSkillCatalogCA0478[row + 8 * page].price08;
        const std::uint8_t color = gold >= price ? 0 : 6;
        platform::FormatLegacyString_53B304(buffer, "%7d", price);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 261),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 88 + 32 * row), 2, 1, color, buffer, 0, 0));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillCatalogPanel581FB8.y + 32 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillCatalogPanel581FB8.x;
        DrawGraphicHover(Low16(left + 18), Low16(top + 84), Low16(left + 345),
            Low16(top + 105), 214, color, 0);
        for (std::int32_t row = 0; row < 2; ++row) {
            const char* const text = gLegacyPetSkillCatalogCA0478[
                hovered + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data() + 25 + 29 * row;
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 70),
                static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 359 + 32 * row),
                2, 0, 0, text, 0, 0));
        }
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillCatalogPanel581FB8) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillCatalogCancel588CD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillCatalogLearn588268;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillCatalogPanel581FB8, 1);
}

YobiAction* Sub_4D7BF0()
{
    const std::int32_t y = gLegacyPetSkillPetPanel582080.y;
    const std::int32_t x = gLegacyPetSkillPetPanel582080.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillPetActionD67C60 = action;
    return action;
}

std::uint32_t Sub_4D7C20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillPetPanel582080.x;
    const std::int32_t y = gLegacyPetSkillPetPanel582080.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillPetPanel582080,
        +[] { static_cast<void>(Sub_4D7BF0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* const action = gLegacyPetSkillPetActionD67C60;
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            static_cast<void>(MarkDeath(action));
            gLegacyPetSkillPetActionD67C60 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    if (gLegacyPetSkillPetActionD67C60 != nullptr) {
        gLegacyPetSkillPetActionD67C60->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillPetPanel582080.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto& option = ui::detail::gWindow54PetOptionsD67448[row];
        if (option.allowed != 0 && option.used != 0
            && Sub_5049A0(gLegacyPetSkillPetPanel582080.x + 70,
                gLegacyPetSkillPetPanel582080.y + 131 + 40 * row,
                gLegacyPetSkillPetPanel582080.x + 301,
                gLegacyPetSkillPetPanel582080.y + 152 + 40 * row, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        char buffer[16];
        char escaped[16];
        platform::FormatLegacyString_53B304(buffer, "%d", hovered);
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        const std::uint32_t panelId = gLegacyPetSkillPetPanel582080.id;
        gLegacyPetSkillPetSelectionC8A94C = hovered;
        static_cast<void>(audio::Sub_5042D0(panelId));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillPetPanel582080.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    const std::int32_t skillWidth = TextMeasure(measuredName, 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 44), 2, 0, 0, skillName, 0, 0));
    char buffer[64];
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    platform::FormatLegacyString_53B304(buffer, "%4d", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, "%7d", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto& option = ui::detail::gWindow54PetOptionsD67448[row];
        if (option.used == 0) {
            continue;
        }
        const std::uint8_t color = option.allowed != 0 ? 0 : 7;
        const std::int32_t width = TextMeasure(option.name.data(), 0);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 147 - width / 2),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 133 + 40 * row),
            2, 0, color, option.name.data(), 0, 0));
        platform::FormatLegacyString_53B304(buffer, "%3d", option.value04);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 272),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 135 + 40 * row), 2, 1, color, buffer, 0, 0));
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 59 + 20 * row),
            2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 286), 2, 0, 0, buffer, 0, 0));
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillPetPanel582080.y + 40 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillPetPanel582080.x;
        DrawGraphicHover(Low16(left + 70), Low16(top + 131), Low16(left + 301), Low16(top + 152), 214, color, 0);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillPetPanel582080) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillCatalogLearn588268;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillPetPanel582080, 1);
}

YobiAction* Sub_4D8200()
{
    const std::int32_t y = gLegacyPetSkillSlotsPanel582148.y;
    const std::int32_t x = gLegacyPetSkillSlotsPanel582148.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillSlotsActionD67C64 = action;
    return action;
}

std::uint32_t Sub_4D8230(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillSlotsPanel582148.x;
    const std::int32_t y = gLegacyPetSkillSlotsPanel582148.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillSlotsPanel582148,
        +[] { static_cast<void>(Sub_4D8200()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(61) == 0) {
            world::detail::gValue581800 = -1;
        }
        if (gLegacyPetSkillSlotsActionD67C64 != nullptr) {
            static_cast<void>(MarkDeath(gLegacyPetSkillSlotsActionD67C64));
            gLegacyPetSkillSlotsActionD67C64 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    if (gLegacyPetSkillSlotsActionD67C64 != nullptr) {
        gLegacyPetSkillSlotsActionD67C64->flags &= ~2U;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillSlotsPanel582148.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t selected = gLegacyPetSkillPetSelectionC8A94C;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (ui::detail::gWindow54PetOptionsD67448[selected].slots <= row) {
            continue;
        }
        if (Sub_5049A0(gLegacyPetSkillSlotsPanel582148.x + 60,
                gLegacyPetSkillSlotsPanel582148.y + 116 + 24 * row,
                gLegacyPetSkillSlotsPanel582148.x + 302,
                gLegacyPetSkillSlotsPanel582148.y + 136 + 24 * row, -1) != 0) {
            hovered = row;
            break;
        }
        selected = gLegacyPetSkillPetSelectionC8A94C;
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        gLegacyPetSkillSlotCF4B64 = hovered;
        static_cast<void>(audio::Sub_504180(61));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillSlotsPanel582148.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    const std::int32_t skillWidth = TextMeasure(measuredName, 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 44), 2, 0, 0, skillName, 0, 0));
    char buffer[64];
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    platform::FormatLegacyString_53B304(buffer, "%4d", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, "%7d", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 0, 0, buffer, 0, 0));
    const std::int32_t petWidth = TextMeasure(
        ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data(), 0);
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 142 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 81), 2, 0, 0, petName, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%3d", ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].value04);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 267),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 83), 2, 1, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 10; ++row) {
        const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[row];
        if (slot.used != 0) {
            const std::int32_t width = TextMeasure(slot.name.data(), 0);
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 156 - width / 2),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 118 + 24 * row),
                2, 0, 0, slot.name.data(), 0, 0));
            platform::FormatLegacyString_53B304(buffer, "%4d", slot.value04);
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 260),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 120 + 24 * row), 2, 1, 0, buffer, 0, 0));
        }
        if (ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].slots > row) {
            static_cast<void>(DrawGraphic(gLegacyPetSkillSlotsPanel582148.x + 181,
                gLegacyPetSkillSlotsPanel582148.y + 126 + 24 * row, 213, 23093, 0, nullptr));
        }
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 59 + 20 * row),
            2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillSlotsPanel582148.y + 24 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillSlotsPanel582148.x;
        DrawGraphicHover(Low16(left + 60), Low16(top + 116), Low16(left + 302), Low16(top + 136), 214, color, 0);
        const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[hovered];
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 69),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 369 + 20 * row),
                2, 0, 0, slot.description[row].data(), 0, 0));
        }
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 286), 2, 0, 0, buffer, 0, 0));
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillSlotsPanel582148) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillSlotHint588D00;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillSlotsPanel582148, 1);
}

std::uint32_t Sub_4D88C0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillConfirmPanel582210.x;
    const std::int32_t y = gLegacyPetSkillConfirmPanel582210.y;
    character_select::detail::CharacterModalUiRecord accept {
        x + 82, y + 200, x + 80, y + 200, 54, 18, kLegacyBarterYes588A08,
    };
    character_select::detail::CharacterModalUiRecord decline {
        x + 188, y + 200, x + 186, y + 200, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillConfirmPanel582210, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(60);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        char requestText[16];
        platform::FormatLegacyString_53B304(requestText, "%d", gLegacyPetSkillSlotCF4B64);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 0, requestText));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&decline) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(60));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    char buffer[256];
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillTitle588344, skillName);
    const std::int32_t skillWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 20), 2, 0, 0, buffer, 0, 0));
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillPetName588338, petName);
    // 原4D8AEE测量固定示例文本，绘制仍使用实际名称，不能改成测量buffer。
    const std::int32_t petWidth = TextMeasure(kLegacyPetSkillPetMeasure588320, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 44), 2, 0, 0, buffer, 0, 0));
    const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[gLegacyPetSkillSlotCF4B64];
    if (slot.used != 0) {
        platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillUsedSlot58830C, slot.name.data());
    } else {
        std::strcpy(buffer, kLegacyPetSkillEmptySlot5882F4);
    }
    const std::int32_t slotWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - slotWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 68), 2, 0, 0, buffer, 0, 0));
    const std::int32_t learnWidth = TextMeasure(kLegacyPetSkillLearnLine5882E8, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - learnWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 88),
        2, 0, 0, kLegacyPetSkillLearnLine5882E8, 0, 0));
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillGold588D5C, world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 120), 2, 0, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillPrice588D48, price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 140), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 92),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 172),
        2, 0, 0, kLegacyPetSkillQuestion55C9B0, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&decline, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyPetSkillAccept588D34;
    } else if ((character_select::detail::Sub_503990(&decline) & 0x40) != 0) {
        hint = kLegacyPetSkillStop588D18;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillConfirmPanel582210, 0);
}

YobiAction* Sub_4D8D70()
{
    const std::int32_t y = gLegacyShopChoicePanel5822D8.y;
    const std::int32_t x = gLegacyShopChoicePanel5822D8.x;
    const std::int32_t image = ui::detail::ReadLegacyShopImageC9A9A0();
    YobiAction* const action = Sub_4F9FF0(image, x + 45, y + 133, 213);
    gLegacyShopChoiceActionD67C68 = action;
    return action;
}

YobiAction* Sub_4D91E0()
{
    const std::int32_t y = gLegacyBuyPanel5823A0.y + 245;
    const std::int32_t x = gLegacyBuyPanel5823A0.x + 469;
    const std::int32_t image = gLegacyBuyImageD2DBFC;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyBuyActionD67C6C = action;
    return action;
}

std::uintptr_t Sub_4D9220(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBuyPanel5823A0.x;
    const std::int32_t y = gLegacyBuyPanel5823A0.y;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23073, 23075};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23074, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 51, x + 126, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 51, x + 216, y + 39, 28, 24, 23023, 23027};
    std::array<LegacyPanelButtonRecord, 10> increases;
    std::array<LegacyPanelButtonRecord, 10> decreases;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t dx = 176 * (index / 5);
        const std::int32_t dy = 68 * (index % 5);
        increases[index] = {x + 128 + dx, y + 100 + dy, x + 114 + dx, y + 88 + dy, 28, 24, 23020, 23024};
        decreases[index] = {x + 162 + dx, y + 100 + dy, x + 148 + dx, y + 88 + dy, 28, 24, 23021, 23025};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t increaseHovered = -1;
    std::int32_t decreaseHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacyBuyPanel5823A0,
        +[] { static_cast<void>(Sub_4D91E0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(65) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacyBuyActionD67C6C;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyBuyActionD67C6C = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacyBuyActionD67C6C != nullptr) {
        SetYobiActionFlag2(gLegacyBuyActionD67C6C, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBuyPanel5823A0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gLegacyBuyTotalC8AF6C = 0;
    std::int32_t total = 0;
    std::int32_t requiredSlots = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyShopItemCF4FE0(index);
        if (item[0] != 0) {
            std::int32_t perSlot = ReadRecordW32(item, 0xC34);
            const std::int32_t selected = ReadRecordW32(item, 0xC28);
            if (perSlot <= 0) {
                perSlot = 2;
            }
            requiredSlots += (perSlot + selected - 1) / perSlot;
            const std::int32_t price = ReadRecordW32(item, 0xC24);
            total += price * selected;
        }
    }
    gLegacyBuyTotalC8AF6C = total;
    const std::int32_t freeSlots = world::detail::Sub_518C60();
    std::int32_t hitPage = ReadLegacyBuyPageCC1A0C();
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (BorrowLegacyShopItemCF4FE0(10 * hitPage + row)[0] == 0) {
            continue;
        }
        const std::int32_t top = gLegacyBuyPanel5823A0.y + 68 * (row % 5);
        const std::int32_t left = gLegacyBuyPanel5823A0.x + 176 * (row / 5);
        if (Sub_5049A0(left + 20, top + 92, left + 67, top + 139, -1) != 0) {
            hovered = row;
            break;
        }
        hitPage = ReadLegacyBuyPageCC1A0C();
    }
    if (hovered != gLegacyBuyPreviousHover582464) {
        gLegacyBuyDetailPageD67C70 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyBuyPanel5823A0.id));
    } else if (requiredSlots > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t currentTotal = gLegacyBuyTotalC8AF6C;
        if (gold < currentTotal) {
            gLegacyBuyMessageD30414 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else if (requiredSlots > freeSlots) {
            gLegacyBuyMessageD30414 = 2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::Sub_504180(65));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        const std::int32_t page = ReadLegacyBuyPageCC1A0C() - 1;
        WriteLegacyBuyPageCC1A0C(page);
        if (page < 0) {
            WriteLegacyBuyPageCC1A0C(gLegacyBuyPageCountD2D63C - 1);
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t page = ReadLegacyBuyPageCC1A0C();
        const std::int32_t count = gLegacyBuyPageCountD2D63C;
        WriteLegacyBuyPageCC1A0C(page + 1);
        if (page + 1 >= count) {
            WriteLegacyBuyPageCC1A0C(0);
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacyBuyDetailPageD67C70;
        if (gLegacyBuyDetailPageD67C70 >= 4) {
            gLegacyBuyDetailPageD67C70 = 0;
        }
    } else {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t index = row + 10 * ReadLegacyBuyPageCC1A0C();
            char* const item = BorrowLegacyShopItemCF4FE0(index);
            if (item[0] == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x40) != 0) {
                increaseHovered = index;
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x40) != 0) {
                decreaseHovered = index;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected < 20) {
                    const std::int32_t updated = selected + 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected > 0) {
                    gLegacyBuyMessageD30414 = 0;
                    const std::int32_t updated = selected - 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyBuyPanel5823A0.y + 68 * (hovered % 5);
        const std::int32_t left = gLegacyBuyPanel5823A0.x + 176 * (hovered / 5);
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139),
            213, gValueD69768, 0);
        const std::int32_t page = gLegacyBuyDetailPageD67C70;
        const std::int32_t listPage = ReadLegacyBuyPageCC1A0C();
        const std::int32_t detailY = gLegacyBuyPanel5823A0.y + 32;
        const std::int32_t detailX = gLegacyBuyPanel5823A0.x + 298;
        Sub_4DA400(detailX, detailY, hovered + 10 * listPage, page, 0);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyBuyPanel5823A0.y + offsetY;
        const std::int32_t left = gLegacyBuyPanel5823A0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 10; ++row) {
        const char* const item = BorrowLegacyShopItemCF4FE0(row + 10 * ReadLegacyBuyPageCC1A0C());
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t dx = 176 * (row / 5);
        const std::int32_t dy = 68 * (row % 5);
        const std::int32_t image = ReadRecordW32(item, 0xC20);
        const std::int32_t top = gLegacyBuyPanel5823A0.y + dy + 116;
        const std::int32_t left = gLegacyBuyPanel5823A0.x + dx + 44;
        static_cast<void>(DrawGraphic(left, top, 214, image, 0, &descriptor));
        const std::int8_t priceColor = item[0xC3C] != 0 ? 0 : 4;
        if (priceColor != 0) {
            drawAt(kLegacyBuyUnavailable588E20, dx + 21, dy + 123, 6);
        }
        platform::FormatLegacyString_53B304(buffer, "%7d", ReadRecordW32(item, 0xC24));
        drawAt(buffer, dx + 87, dy + 120, priceColor);
        platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(item, 0xC28));
        drawAt(buffer, dx + 82, dy + 92, 0);
        Sub_5035E0(&increases[row], 213, 1, gValueD69768, 0);
        Sub_5035E0(&decreases[row], 213, 1, gValueD69768, 0);
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - width / 2, 8, 0);
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t group = gLegacyBuyMessageD30414;
        drawAt(lines[1 + 4 * group + row].text.data(), 375, 59 + 20 * row, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 463, 286, 0);
    const std::int32_t currentTotal = gLegacyBuyTotalC8AF6C;
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    const std::int8_t totalColor = currentTotal > gold ? 6 : 0;
    platform::FormatLegacyString_53B304(buffer, "%7d", currentTotal);
    drawAt(buffer, 463, 338, totalColor);
    const std::int32_t page = ReadLegacyBuyPageCC1A0C();
    const std::int32_t pageCount = gLegacyBuyPageCountD2D63C;
    platform::FormatLegacyString_53B304(buffer, "%d/%d", page + 1, pageCount);
    drawAt(buffer, 172, 43, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (requiredSlots > 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyBuyPanel5823A0) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyBuyCancel588E14;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyBuyConfirm588E04;
    } else if (increaseHovered >= 0) {
        hint = ReadRecordW32(BorrowLegacyShopItemCF4FE0(increaseHovered), 0xC28) < 20
            ? kLegacyBuyIncrease588DEC : kLegacyQuantityIncreaseLimit588ABC;
    } else if (decreaseHovered >= 0) {
        hint = ReadRecordW32(BorrowLegacyShopItemCF4FE0(decreaseHovered), 0xC28) > 0
            ? kLegacyBuyDecrease588DD4 : kLegacyQuantityDecreaseLimit588944;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    static_cast<void>(Sub_503440(&gLegacyBuyPanel5823A0, 1));
    gLegacyBuyPreviousHover582464 = hovered;
    return static_cast<std::uint32_t>(hovered);
}

void Sub_4DA400(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t page, const std::int32_t mode)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const char* const item = BorrowLegacyShopItemCF4FE0(index);
    if (item[0] == 0) {
        return;
    }
    char title[32];
    const std::int32_t count = ReadRecordW32(item, 0xC30);
    if (count <= 1) {
        std::strcpy(title, item + 1);
    } else {
        platform::FormatLegacyBoundedString_53D21D(title, 28, "%s(%d)", item + 1, count);
    }
    const std::int32_t flags = ReadRecordW32(item, 0xC40);
    Sub_504BA0(x, y, 272, 4, 0, title, item + 0xC44, 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 7; ++row) {
        const char* const line = item + 0x1E + page * 768 + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, line, 0, 0));
        }
    }
    if (item[0xC3C] == 0 && mode == 0) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + 10),
            static_cast<std::int16_t>(y + 160), 4, 1, 4, kLegacyBuyUnavailableHint586BF4, 0, 0));
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

std::uint32_t Sub_4DA580(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBuyConfirmPanel582468.x;
    const std::int32_t y = gLegacyBuyConfirmPanel582468.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 190, y + 334, x + 188, y + 334, 54, 18, kLegacyBarterYes588A08,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 403, y + 334, x + 401, y + 334, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyBuyConfirmPanel582468, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(63);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBuyConfirmPanel582468.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[512];
    char entry[32];
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        buffer[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const char* const item = BorrowLegacyShopItemCF4FE0(index);
            if (item[0] != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected != 0) {
                    if (count > 0) {
                        std::strcat(buffer, "|");
                    }
                    platform::FormatLegacyString_53B304(entry, "%d|%d", index, selected);
                    std::strcat(buffer, entry);
                    ++count;
                }
            }
        }
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacyBuyConfirmPanel582468.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(63));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyBuyConfirmPanel582468.y + offsetY;
        const std::int32_t left = gLegacyBuyConfirmPanel582468.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::int32_t shown = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyShopItemCF4FE0(index);
        if (item[0] != 0 && ReadRecordW32(item, 0xC28) != 0) {
            const std::int32_t dx = 302 * (shown % 2);
            const std::int32_t dy = 20 * (shown / 2);
            const std::int32_t nameX = gLegacyBuyConfirmPanel582468.x + dx + 24;
            const std::int32_t nameY = gLegacyBuyConfirmPanel582468.y + dy + 34;
            static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
                static_cast<std::int16_t>(nameY), 2, 0, 0, item + 1, 0, 0));
            platform::FormatLegacyString_53B304(buffer, "x%2d", ReadRecordW32(item, 0xC28));
            drawAt(buffer, dx + 280, dy + 34);
            ++shown;
        }
    }
    const std::int32_t total = gLegacyBuyTotalC8AF6C;
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    platform::FormatLegacyString_53B304(buffer, kLegacyBuySummary588E4C, gold, total);
    drawAt(buffer, 144, 254);
    drawAt(kLegacyBuyQuestion588E34, 178, 294);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyBuyAcceptHint588E2C;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacyBuyStopHint588E24;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyBuyConfirmPanel582468, 0);
}

YobiAction* Sub_4DAA00()
{
    const std::int32_t y = gLegacySellPanel582530.y + 245;
    const std::int32_t x = gLegacySellPanel582530.x + 469;
    const std::int32_t image = gLegacySellImageCC3EFC;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacySellActionD67C74 = action;
    return action;
}

std::uintptr_t Sub_4DAA40(const std::int32_t opened)
{
    const std::int32_t x = gLegacySellPanel582530.x;
    const std::int32_t y = gLegacySellPanel582530.y;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23070, 23071};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23072, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 51, x + 126, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 51, x + 216, y + 39, 28, 24, 23023, 23027};
    std::array<LegacyPanelButtonRecord, 10> increases;
    std::array<LegacyPanelButtonRecord, 10> decreases;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t dx = 176 * (index / 5);
        const std::int32_t dy = 68 * (index % 5);
        increases[index] = {x + 128 + dx, y + 100 + dy, x + 114 + dx, y + 88 + dy, 28, 24, 23020, 23024};
        decreases[index] = {x + 162 + dx, y + 100 + dy, x + 148 + dx, y + 88 + dy, 28, 24, 23021, 23025};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t increaseHovered = -1;
    std::int32_t decreaseHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacySellPanel582530,
        +[] { static_cast<void>(Sub_4DAA00()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(66) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacySellActionD67C74;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacySellActionD67C74 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacySellActionD67C74 != nullptr) {
        SetYobiActionFlag2(gLegacySellActionD67C74, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySellPanel582530.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gLegacySellTotalCF3FCC = 0;
    std::int32_t total = 0;
    std::int32_t selectedSum = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyShopItemCF4FE0(index);
        if (item[0] != 0) {
            const std::int32_t selected = ReadRecordW32(item, 0xC28);
            selectedSum += selected;
            const std::int32_t price = ReadRecordW32(item, 0xC24);
            total += price * selected;
        }
    }
    gLegacySellTotalCF3FCC = total;
    static_cast<void>(world::detail::Sub_518C60());
    std::int32_t hitPage = gLegacySellPageCF49FC;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (BorrowLegacyShopItemCF4FE0(10 * hitPage + row)[0] == 0) {
            continue;
        }
        const std::int32_t top = gLegacySellPanel582530.y + 68 * (row % 5);
        const std::int32_t left = gLegacySellPanel582530.x + 176 * (row / 5);
        if (Sub_5049A0(left + 20, top + 92, left + 67, top + 139, -1) != 0) {
            hovered = row;
            break;
        }
        hitPage = gLegacySellPageCF49FC;
    }
    if (hovered != gLegacySellPreviousHover5825F4) {
        gLegacySellDetailPageD67C78 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacySellPanel582530.id));
    } else if (selectedSum > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(66));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        const std::int32_t page = gLegacySellPageCF49FC - 1;
        gLegacySellPageCF49FC = page;
        if (page < 0) {
            gLegacySellPageCF49FC = gLegacySellPageCountCA0ABC - 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t page = gLegacySellPageCF49FC;
        const std::int32_t count = gLegacySellPageCountCA0ABC;
        gLegacySellPageCF49FC = page + 1;
        if (page + 1 >= count) {
            gLegacySellPageCF49FC = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacySellDetailPageD67C78;
        if (gLegacySellDetailPageD67C78 >= 4) {
            gLegacySellDetailPageD67C78 = 0;
        }
    } else {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t index = row + 10 * gLegacySellPageCF49FC;
            char* const item = BorrowLegacyShopItemCF4FE0(index);
            if (item[0] == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x40) != 0) {
                increaseHovered = index;
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x40) != 0) {
                decreaseHovered = index;
            }
            if (item[0xC3C] == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                const std::int32_t maximum = ReadRecordW32(item, 0xC34);
                if (selected < maximum) {
                    const std::int32_t price = ReadRecordW32(item, 0xC24);
                    const std::int32_t updated = selected + 1;
                    const std::int32_t amount = price * updated;
                    const std::int32_t gold = world::detail::ReadPlayerValueFC();
                    const std::int32_t currentTotal = gLegacySellTotalCF3FCC;
                    const std::int32_t limit = world::detail::ReadPlayerValue15BD0(0);
                    if (amount + gold + currentTotal <= limit) {
                        std::memcpy(item + 0xC28, &updated, sizeof(updated));
                        static_cast<void>(audio::PlaySe_52C450(51, 320));
                        gLegacySellLimitMessageCB0A88 = 0;
                    } else {
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                        gLegacySellLimitMessageCB0A88 = 1;
                    }
                }
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected > 0) {
                    const std::int32_t updated = selected - 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                    gLegacySellLimitMessageCB0A88 = 0;
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacySellPanel582530.y + 68 * (hovered % 5);
        const std::int32_t left = gLegacySellPanel582530.x + 176 * (hovered / 5);
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139),
            213, gValueD69768, 0);
        const std::int32_t page = gLegacySellDetailPageD67C78;
        const std::int32_t listPage = gLegacySellPageCF49FC;
        const std::int32_t detailY = gLegacySellPanel582530.y + 32;
        const std::int32_t detailX = gLegacySellPanel582530.x + 298;
        Sub_4DA400(detailX, detailY, hovered + 10 * listPage, page, 1);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacySellPanel582530.y + offsetY;
        const std::int32_t left = gLegacySellPanel582530.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 10; ++row) {
        const char* const item = BorrowLegacyShopItemCF4FE0(row + 10 * gLegacySellPageCF49FC);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t dx = 176 * (row / 5);
        const std::int32_t dy = 68 * (row % 5);
        const std::int32_t image = ReadRecordW32(item, 0xC20);
        const std::int32_t top = gLegacySellPanel582530.y + dy + 116;
        const std::int32_t left = gLegacySellPanel582530.x + dx + 44;
        static_cast<void>(DrawGraphic(left, top, 214, image, 0, &descriptor));
        const std::int8_t priceColor = item[0xC3C] == 0 ? 6
            : ReadRecordW32(item, 0xC28) != 0 ? 0 : 7;
        platform::FormatLegacyString_53B304(buffer, "%7d", ReadRecordW32(item, 0xC24));
        drawAt(buffer, dx + 87, dy + 120, priceColor);
        platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(item, 0xC28));
        drawAt(buffer, dx + 82, dy + 92, 0);
        Sub_5035E0(&increases[row], 213, 1, gValueD69768, 0);
        Sub_5035E0(&decreases[row], 213, 1, gValueD69768, 0);
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - width / 2, 8, 0);
    if (gLegacySellLimitMessageCB0A88 != 0) {
        drawAt(kLegacySellLimitLine588ECC, 375, 79, 0);
        drawAt(kLegacySellLimitLine588EC0, 375, 99, 0);
    } else {
        for (std::int32_t row = 0; row < 4; ++row) {
            drawAt(lines[1 + row].text.data(), 375, 59 + 20 * row, 0);
        }
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 463, 286, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacySellTotalCF3FCC);
    drawAt(buffer, 463, 338, 0);
    const std::int32_t page = gLegacySellPageCF49FC;
    const std::int32_t pageCount = gLegacySellPageCountCA0ABC;
    platform::FormatLegacyString_53B304(buffer, "%d/%d", page + 1, pageCount);
    drawAt(buffer, 172, 43, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (selectedSum > 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacySellPanel582530) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacySellCancel588EB4;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacySellConfirm588EA4;
    } else if (increaseHovered >= 0) {
        const char* const item = BorrowLegacyShopItemCF4FE0(increaseHovered);
        hint = ReadRecordW32(item, 0xC28) < ReadRecordW32(item, 0xC34)
            ? kLegacySellIncrease588E8C : kLegacyQuantityIncreaseLimit588ABC;
    } else if (decreaseHovered >= 0) {
        hint = ReadRecordW32(BorrowLegacyShopItemCF4FE0(decreaseHovered), 0xC28) > 0
            ? kLegacySellDecrease588E74 : kLegacyQuantityDecreaseLimit588944;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    } else if (increaseHovered < 0 && decreaseHovered < 0) {
        // 原4DBC11先读取备注，再判悬停；越界路径沿RPLAN.24，不另加门禁。
        const char* const item = BorrowLegacyShopItemCF4FE0(hovered + 10 * gLegacySellPageCF49FC);
        if (std::strlen(item + 0xC44) != 0 && hovered >= 0) {
            const std::int32_t flags = ReadRecordW32(item, 0xC40);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 1);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            } else {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 0xC44);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            }
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacySellPanel582530, 1);
    gLegacySellPreviousHover5825F4 = hovered;
    return result;
}

std::uint32_t Sub_4DBD30(const std::int32_t opened)
{
    const std::int32_t x = gLegacySellConfirmPanel5825F8.x;
    const std::int32_t y = gLegacySellConfirmPanel5825F8.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 190, y + 334, x + 188, y + 334, 54, 18, kLegacyBarterYes588A08,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 403, y + 334, x + 401, y + 334, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacySellConfirmPanel5825F8, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(64);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacySellConfirmPanel5825F8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[512];
    char entry[32];
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        buffer[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const char* const item = BorrowLegacyShopItemCF4FE0(index);
            if (item[0] != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected != 0) {
                    if (count > 0) {
                        std::strcat(buffer, "|");
                    }
                    const std::int32_t inventoryIndex = ReadRecordW32(item, 0xC2C);
                    platform::FormatLegacyString_53B304(entry, "%d|%d", inventoryIndex, selected);
                    std::strcat(buffer, entry);
                    ++count;
                }
            }
        }
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacySellConfirmPanel5825F8.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(64));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacySellConfirmPanel5825F8.y + offsetY;
        const std::int32_t left = gLegacySellConfirmPanel5825F8.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::int32_t shown = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyShopItemCF4FE0(index);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t selected = ReadRecordW32(item, 0xC28);
        if (selected != 0) {
            const std::int32_t dx = 302 * (shown % 2);
            const std::int32_t dy = 20 * (shown / 2);
            const std::int32_t flags = ReadRecordW32(item, 0xC40);
            const char* const name = (flags & 0x100) != 0 ? item + 0xC44 : item + 1;
            const std::int32_t count = ReadRecordW32(item, 0xC30);
            const std::int8_t color = (flags & 0x100) != 0 ? 5 : (flags & 0x200) != 0 ? 1 : 0;
            if (count <= 1) {
                std::strcpy(buffer, name);
            } else {
                const std::int32_t value = ReadRecordW32(item, 0xC38);
                platform::FormatLegacyString_53B304(buffer, kLegacySellQuantity588F28, name, value, selected);
            }
            // 原100H/200H分支借旧65坐标，普通分支才取旧66，不能统一改成本面板。
            const LegacyPanelRecord& textPanel = (flags & 0x300) != 0
                ? gLegacyBuyConfirmPanel582468 : gLegacySellConfirmPanel5825F8;
            const std::int32_t nameY = textPanel.y + dy + 34;
            const std::int32_t nameX = textPanel.x + dx + 24;
            static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
                static_cast<std::int16_t>(nameY), 2, 0, color, buffer, 0, 0));
            ++shown;
        }
    }
    const std::int32_t total = gLegacySellTotalCF3FCC;
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    platform::FormatLegacyString_53B304(buffer, kLegacySellSummary588F00, gold, total);
    drawAt(buffer, 144, 254);
    drawAt(kLegacySellQuestion588EE8, 178, 294);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacySellAcceptHint588EE0;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacySellStopHint588ED8;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacySellConfirmPanel5825F8, 0);
}

std::uint32_t Sub_4DC290(const std::int32_t opened)
{
    const std::int32_t x = gLegacyHealingPanel5826C0.x;
    const std::int32_t y = gLegacyHealingPanel5826C0.y;
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 202, y + 250, x + 200, y + 250, 57, 18, kLegacyItemUseCancel589008,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyHealingPanel5826C0, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        world::detail::gValue581800 = -1;
        return 2;
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyHealingPanel5826C0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const std::byte* const records = ui::detail::BorrowLegacyHealingRecordsD672F0();
    std::int32_t hovered = -1;
    for (std::int32_t index = 0; index < 7; ++index) {
        const std::int32_t top = gLegacyHealingPanel5826C0.y + 20 * index + 74;
        const std::int32_t left = gLegacyHealingPanel5826C0.x + 52;
        if (Sub_5049A0(left, top, left + 335, top + 20, -1) != 0
            && ReadRecordW32(records + 32 * index, 0) > 0) {
            hovered = index;
            break;
        }
    }
    const std::int32_t cancelState = character_select::detail::Sub_503990(&cancel);
    char buffer[128];
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        char escaped[16];
        platform::FormatLegacyString_53B304(buffer, "%d", hovered + 1);
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        static_cast<void>(audio::Sub_5042D0(gLegacyHealingPanel5826C0.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyHealingPanel5826C0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyHealingPanel5826C0.y + offsetY;
        const std::int32_t left = gLegacyHealingPanel5826C0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    drawAt(kLegacyHealingPrompt588FF8, 96, 24, 4);
    drawAt(kLegacyHealingHeader588FD0, 52, 50, 0);
    std::int32_t totalCost = 0;
    std::int32_t totalHealth = 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        const std::byte* const record = records + 32 * index;
        const std::int32_t level = ReadRecordW32(record, 0);
        if (level <= 0) {
            continue;
        }
        platform::FormatLegacyString_53B304(buffer, "%3d", level);
        drawAt(buffer, 100, 76 + 20 * index, 0);
        platform::FormatLegacyString_53B304(buffer, "%-16s", reinterpret_cast<const char*>(record + 12));
        drawAt(buffer, 141, 76 + 20 * index, 0);
        platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, ReadRecordW32(record, 8));
        drawAt(buffer, 290, 76 + 20 * index, 0);
        const std::int32_t image = ui::detail::ReadLegacyHealthImage(ReadRecordW32(record, 4));
        const std::int32_t imageY = gLegacyHealingPanel5826C0.y + 20 * index + 84;
        const std::int32_t imageX = gLegacyHealingPanel5826C0.x + 73;
        static_cast<void>(DrawGraphic(imageX, imageY, 213, image, 0, nullptr));
        const std::int32_t cost = ReadRecordW32(record, 8);
        const std::int32_t health = ReadRecordW32(record, 4);
        totalCost += cost;
        totalHealth += health;
    }
    platform::FormatLegacyString_53B304(buffer, kLegacyHealingTotal588FA8, totalCost);
    drawAt(buffer, 114, 196, 0);
    platform::FormatLegacyString_53B304(buffer, kLegacyHealingGold588F94, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 151, 224, 0);
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if (hovered >= 0) {
        const std::int32_t top = gLegacyHealingPanel5826C0.y + 20 * hovered + 74;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacyHealingPanel5826C0.x + 52;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 350), Low16(top + 20), 218, image, 0);
        if (hovered == 6) {
            hint = totalHealth != 0 ? kLegacyHealingAll588F80 : kLegacyHealingAllHealthy588F6C;
        } else {
            hint = ReadRecordW32(records + 32 * hovered, 4) != 0
                ? kLegacyHealingOne588F40 : kLegacyHealingHealthy588F54;
        }
    } else if ((cancelState & 0x40) != 0) {
        hint = kLegacyHealingCancel588F34;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyHealingPanel5826C0, 0);
}

YobiAction* Sub_4DC7A0()
{
    const std::int32_t y = gLegacyRepairPanel582788.y + 245;
    const std::int32_t x = gLegacyRepairPanel582788.x + 469;
    const std::int32_t image = gLegacyRepairImageD673E8;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyRepairActionD67C7C = action;
    gLegacyRepairMessageD64E14 = 0;
    return action;
}

std::uintptr_t Sub_4DC7E0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyRepairPanel582788.x;
    const std::int32_t y = gLegacyRepairPanel582788.y;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23080, 23082};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23081, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 51, x + 126, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 51, x + 216, y + 39, 28, 24, 23023, 23027};
    std::array<LegacyPanelButtonRecord, 10> increases;
    std::array<LegacyPanelButtonRecord, 10> decreases;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t dx = 176 * (index / 5);
        const std::int32_t dy = 68 * (index % 5);
        increases[index] = {x + 128 + dx, y + 100 + dy, x + 114 + dx, y + 88 + dy, 28, 24, 23020, 23024};
        decreases[index] = {x + 162 + dx, y + 100 + dy, x + 148 + dx, y + 88 + dy, 28, 24, 23021, 23025};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t increaseHovered = -1;
    std::int32_t decreaseHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacyRepairPanel582788,
        +[] { static_cast<void>(Sub_4DC7A0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(71) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacyRepairActionD67C7C;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyRepairActionD67C7C = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacyRepairActionD67C7C != nullptr) {
        SetYobiActionFlag2(gLegacyRepairActionD67C7C, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyRepairPanel582788.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gLegacyRepairTotalD460C4 = 0;
    std::int32_t total = 0;
    std::int32_t selectedSum = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyRepairItemCE4548(index);
        if (item[0] != 0) {
            const std::int32_t selected = ReadRecordW32(item, 0xC28);
            selectedSum += selected;
            const std::int32_t price = ReadRecordW32(item, 0xC24);
            total += price * selected;
        }
    }
    gLegacyRepairTotalD460C4 = total;
    std::int32_t hitPage = gLegacyRepairPageCF4A08;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (BorrowLegacyRepairItemCE4548(10 * hitPage + row)[0] == 0) {
            continue;
        }
        const std::int32_t top = gLegacyRepairPanel582788.y + 68 * (row % 5);
        const std::int32_t left = gLegacyRepairPanel582788.x + 176 * (row / 5);
        if (Sub_5049A0(left + 20, top + 92, left + 67, top + 139, -1) != 0) {
            hovered = row;
            break;
        }
        hitPage = gLegacyRepairPageCF4A08;
    }
    if (hovered != gLegacyRepairPreviousHover58284C) {
        gLegacyRepairDetailPageD67C80 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyRepairPanel582788.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if (selectedSum > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t currentTotal = gLegacyRepairTotalD460C4;
        if (gold < currentTotal) {
            gLegacyRepairMessageD64E14 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::Sub_504180(71));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        const std::int32_t page = gLegacyRepairPageCF4A08 - 1;
        gLegacyRepairPageCF4A08 = page;
        if (page < 0) {
            gLegacyRepairPageCF4A08 = gLegacyRepairPageCountCC4B24 - 1;
        }
        gLegacyRepairMessageD64E14 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t page = gLegacyRepairPageCF4A08;
        const std::int32_t count = gLegacyRepairPageCountCC4B24;
        gLegacyRepairPageCF4A08 = page + 1;
        if (page + 1 >= count) {
            gLegacyRepairPageCF4A08 = 0;
        }
        gLegacyRepairMessageD64E14 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacyRepairDetailPageD67C80;
        if (gLegacyRepairDetailPageD67C80 >= 4) {
            gLegacyRepairDetailPageD67C80 = 0;
        }
    } else {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t index = row + 10 * gLegacyRepairPageCF4A08;
            char* const item = BorrowLegacyRepairItemCE4548(index);
            if (item[0] == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x40) != 0) {
                increaseHovered = index;
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x40) != 0) {
                decreaseHovered = index;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected < 1) {
                    const std::int32_t updated = selected + 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    gLegacyRepairMessageD64E14 = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected > 0) {
                    const std::int32_t updated = selected - 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    gLegacyRepairMessageD64E14 = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyRepairPanel582788.y + 68 * (hovered % 5);
        const std::int32_t left = gLegacyRepairPanel582788.x + 176 * (hovered / 5);
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139),
            213, gValueD69768, 0);
        const std::int32_t page = gLegacyRepairDetailPageD67C80;
        const std::int32_t listPage = gLegacyRepairPageCF4A08;
        const std::int32_t detailY = gLegacyRepairPanel582788.y + 32;
        const std::int32_t detailX = gLegacyRepairPanel582788.x + 298;
        Sub_4DDA30(detailX, detailY, hovered + 10 * listPage, page);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyRepairPanel582788.y + offsetY;
        const std::int32_t left = gLegacyRepairPanel582788.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 10; ++row) {
        const char* const item = BorrowLegacyRepairItemCE4548(row + 10 * gLegacyRepairPageCF4A08);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t dx = 176 * (row / 5);
        const std::int32_t dy = 68 * (row % 5);
        const std::int32_t image = ReadRecordW32(item, 0xC20);
        const std::int32_t top = gLegacyRepairPanel582788.y + dy + 116;
        const std::int32_t left = gLegacyRepairPanel582788.x + dx + 44;
        static_cast<void>(DrawGraphic(left, top, 214, image, 0, &descriptor));
        const std::int8_t priceColor = ReadRecordW32(item, 0xC28) != 0 ? 0 : 7;
        platform::FormatLegacyString_53B304(buffer, "%7d", ReadRecordW32(item, 0xC24));
        drawAt(buffer, dx + 87, dy + 120, priceColor);
        platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(item, 0xC28));
        drawAt(buffer, dx + 82, dy + 92, 0);
        Sub_5035E0(&increases[row], 213, 1, gValueD69768, 0);
        Sub_5035E0(&decreases[row], 213, 1, gValueD69768, 0);
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - width / 2, 8, 0);
    if (gLegacyRepairMessageD64E14 == 0) {
        drawAt(kLegacyRepairWelcome587D2C, 375, 79, 0);
        drawAt(kLegacyRepairPrompt589058, 375, 99, 0);
    } else if (gLegacyRepairMessageD64E14 == 1) {
        drawAt(kLegacyRepairNoMoney58906C, 375, 79, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 463, 286, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyRepairTotalD460C4);
    drawAt(buffer, 463, 338, 0);
    const std::int32_t page = gLegacyRepairPageCF4A08;
    const std::int32_t pageCount = gLegacyRepairPageCountCC4B24;
    platform::FormatLegacyString_53B304(buffer, "%d/%d", page + 1, pageCount);
    drawAt(buffer, 172, 43, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (selectedSum > 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyRepairPanel582788) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyRepairConfirm589048;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyRepairCancel589034;
    } else if (increaseHovered >= 0) {
        const char* const item = BorrowLegacyRepairItemCE4548(increaseHovered);
        hint = ReadRecordW32(item, 0xC28) < 1
            ? kLegacyRepairIncrease589024 : kLegacyQuantityIncreaseLimit588ABC;
    } else if (decreaseHovered >= 0) {
        hint = ReadRecordW32(BorrowLegacyRepairItemCE4548(decreaseHovered), 0xC28) > 0
            ? kLegacyRepairDecrease589010 : kLegacyQuantityDecreaseLimit588944;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    } else if (increaseHovered < 0 && decreaseHovered < 0) {
        // 原4DD92C直接读取备注，无悬停门禁；越界沿RPLAN.24，不另加保护。
        const char* const item = BorrowLegacyRepairItemCE4548(hovered + 10 * gLegacyRepairPageCF4A08);
        if (std::strlen(item + 0xC34) != 0) {
            const std::int32_t flags = ReadRecordW32(item, 0xC30);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 1);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            } else {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 0xC34);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            }
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyRepairPanel582788, 1);
    gLegacyRepairPreviousHover58284C = hovered;
    return result;
}

void Sub_4DDA30(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t page)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const char* const item = BorrowLegacyRepairItemCE4548(index);
    if (item[0] == 0) {
        return;
    }
    const std::int32_t flags = ReadRecordW32(item, 0xC30);
    Sub_504BA0(x, y, 272, 4, 0, item + 1, item + 0xC34, 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item + 0x1E + page * 768 + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, line, 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

std::uint32_t Sub_4DDB20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyRepairConfirmPanel582850.x;
    const std::int32_t y = gLegacyRepairConfirmPanel582850.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 190, y + 334, x + 188, y + 334, 54, 18, kLegacyBarterYes588A08,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 403, y + 334, x + 401, y + 334, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyRepairConfirmPanel582850, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(70);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyRepairConfirmPanel582850.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[512];
    char entry[32];
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        buffer[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const char* const item = BorrowLegacyRepairItemCE4548(index);
            if (item[0] != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected != 0) {
                    if (count > 0) {
                        std::strcat(buffer, "|");
                    }
                    const std::int32_t inventoryIndex = ReadRecordW32(item, 0xC2C);
                    platform::FormatLegacyString_53B304(entry, "%d", inventoryIndex);
                    std::strcat(buffer, entry);
                    ++count;
                }
            }
        }
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacyRepairConfirmPanel582850.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(70));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyRepairConfirmPanel582850.y + offsetY;
        const std::int32_t left = gLegacyRepairConfirmPanel582850.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::int32_t shown = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyRepairItemCE4548(index);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t selected = ReadRecordW32(item, 0xC28);
        if (selected != 0) {
            const std::int32_t dx = 302 * (shown % 2);
            const std::int32_t dy = 20 * (shown / 2);
            const std::int32_t flags = ReadRecordW32(item, 0xC30);
            const char* const name = (flags & 0x100) != 0 ? item + 0xC34 : item + 1;
            const std::int8_t color = (flags & 0x100) != 0 ? 5 : (flags & 0x200) != 0 ? 1 : 0;
            const std::int32_t nameY = gLegacyRepairConfirmPanel582850.y + dy + 34;
            const std::int32_t nameX = gLegacyRepairConfirmPanel582850.x + dx + 24;
            static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
                static_cast<std::int16_t>(nameY), 2, 0, color, name, 0, 0));
            ++shown;
        }
    }
    const std::int32_t total = gLegacyRepairTotalD460C4;
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    platform::FormatLegacyString_53B304(buffer, kLegacyRepairSummary5890A4, gold, total);
    drawAt(buffer, 134, 254);
    drawAt(kLegacyRepairQuestion58908C, 214, 294);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyRepairAcceptHint589084;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacyRepairStopHint58907C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyRepairConfirmPanel582850, 0);
}

YobiAction* Sub_4DDF80()
{
    const std::int32_t y = gLegacyIdentifyPanel582918.y + 245;
    const std::int32_t x = gLegacyIdentifyPanel582918.x + 469;
    const std::int32_t image = gLegacyIdentifyImageCF4698;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyIdentifyActionD67C84 = action;
    gLegacyIdentifyMessageC9A948 = 0;
    return action;
}

std::uintptr_t Sub_4DDFC0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyIdentifyPanel582918.x;
    const std::int32_t y = gLegacyIdentifyPanel582918.y;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23083, 23085};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23084, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 51, x + 126, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 51, x + 216, y + 39, 28, 24, 23023, 23027};
    std::array<LegacyPanelButtonRecord, 10> increases;
    std::array<LegacyPanelButtonRecord, 10> decreases;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t dx = 176 * (index / 5);
        const std::int32_t dy = 68 * (index % 5);
        increases[index] = {x + 128 + dx, y + 100 + dy, x + 114 + dx, y + 88 + dy, 28, 24, 23020, 23024};
        decreases[index] = {x + 162 + dx, y + 100 + dy, x + 148 + dx, y + 88 + dy, 28, 24, 23021, 23025};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t increaseHovered = -1;
    std::int32_t decreaseHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacyIdentifyPanel582918,
        +[] { static_cast<void>(Sub_4DDF80()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(69) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacyIdentifyActionD67C84;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyIdentifyActionD67C84 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacyIdentifyActionD67C84 != nullptr) {
        SetYobiActionFlag2(gLegacyIdentifyActionD67C84, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyIdentifyPanel582918.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gLegacyIdentifyTotalCB1E3C = 0;
    std::int32_t total = 0;
    std::int32_t selectedSum = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyIdentifyItemC8B390(index);
        if (item[0] != 0) {
            const std::int32_t selected = ReadRecordW32(item, 0xC28);
            selectedSum += selected;
            const std::int32_t price = ReadRecordW32(item, 0xC24);
            total += price * selected;
        }
    }
    gLegacyIdentifyTotalCB1E3C = total;
    std::int32_t hitPage = gLegacyIdentifyPageCC3F00;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (BorrowLegacyIdentifyItemC8B390(10 * hitPage + row)[0] == 0) {
            continue;
        }
        const std::int32_t top = gLegacyIdentifyPanel582918.y + 68 * (row % 5);
        const std::int32_t left = gLegacyIdentifyPanel582918.x + 176 * (row / 5);
        if (Sub_5049A0(left + 20, top + 92, left + 67, top + 139, -1) != 0) {
            hovered = row;
            break;
        }
        hitPage = gLegacyIdentifyPageCC3F00;
    }
    if (hovered != gLegacyIdentifyPreviousHover5829DC) {
        gLegacyIdentifyDetailPageD67C88 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyIdentifyPanel582918.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if (selectedSum > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t currentTotal = gLegacyIdentifyTotalCB1E3C;
        if (gold < currentTotal) {
            gLegacyIdentifyMessageC9A948 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::Sub_504180(69));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        const std::int32_t page = gLegacyIdentifyPageCC3F00 - 1;
        gLegacyIdentifyPageCC3F00 = page;
        if (page < 0) {
            gLegacyIdentifyPageCC3F00 = gLegacyIdentifyPageCountC9A950 - 1;
        }
        // 原4DE9A5翻页清总价，不清C9A948提示；数量成功同样清总价。
        gLegacyIdentifyTotalCB1E3C = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t page = gLegacyIdentifyPageCC3F00;
        const std::int32_t count = gLegacyIdentifyPageCountC9A950;
        gLegacyIdentifyPageCC3F00 = page + 1;
        if (page + 1 >= count) {
            gLegacyIdentifyPageCC3F00 = 0;
        }
        gLegacyIdentifyTotalCB1E3C = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacyIdentifyDetailPageD67C88;
        if (gLegacyIdentifyDetailPageD67C88 >= 4) {
            gLegacyIdentifyDetailPageD67C88 = 0;
        }
    } else {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t index = row + 10 * gLegacyIdentifyPageCC3F00;
            char* const item = BorrowLegacyIdentifyItemC8B390(index);
            if (item[0] == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x40) != 0) {
                increaseHovered = index;
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x40) != 0) {
                decreaseHovered = index;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected < 1) {
                    const std::int32_t updated = selected + 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    gLegacyIdentifyTotalCB1E3C = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x10) != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected > 0) {
                    const std::int32_t updated = selected - 1;
                    std::memcpy(item + 0xC28, &updated, sizeof(updated));
                    gLegacyIdentifyTotalCB1E3C = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyIdentifyPanel582918.y + 68 * (hovered % 5);
        const std::int32_t left = gLegacyIdentifyPanel582918.x + 176 * (hovered / 5);
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139),
            213, gValueD69768, 0);
        const std::int32_t page = gLegacyIdentifyDetailPageD67C88;
        const std::int32_t listPage = gLegacyIdentifyPageCC3F00;
        const std::int32_t detailY = gLegacyIdentifyPanel582918.y + 32;
        const std::int32_t detailX = gLegacyIdentifyPanel582918.x + 298;
        Sub_4DF210(detailX, detailY, hovered + 10 * listPage, page);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyIdentifyPanel582918.y + offsetY;
        const std::int32_t left = gLegacyIdentifyPanel582918.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 10; ++row) {
        const char* const item = BorrowLegacyIdentifyItemC8B390(row + 10 * gLegacyIdentifyPageCC3F00);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t dx = 176 * (row / 5);
        const std::int32_t dy = 68 * (row % 5);
        const std::int32_t image = ReadRecordW32(item, 0xC20);
        const std::int32_t top = gLegacyIdentifyPanel582918.y + dy + 116;
        const std::int32_t left = gLegacyIdentifyPanel582918.x + dx + 44;
        static_cast<void>(DrawGraphic(left, top, 214, image, 0, &descriptor));
        const std::int8_t priceColor = ReadRecordW32(item, 0xC28) != 0 ? 0 : 7;
        platform::FormatLegacyString_53B304(buffer, "%7d", ReadRecordW32(item, 0xC24));
        drawAt(buffer, dx + 87, dy + 120, priceColor);
        platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(item, 0xC28));
        drawAt(buffer, dx + 82, dy + 92, 0);
        Sub_5035E0(&increases[row], 213, 1, gValueD69768, 0);
        Sub_5035E0(&decreases[row], 213, 1, gValueD69768, 0);
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - width / 2, 8, 0);
    if (gLegacyIdentifyMessageC9A948 == 0) {
        drawAt(kLegacyRepairWelcome587D2C, 375, 79, 0);
        drawAt(kLegacyIdentifyPrompt589110, 375, 99, 0);
    } else if (gLegacyIdentifyMessageC9A948 == 1) {
        drawAt(kLegacyRepairNoMoney58906C, 375, 79, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 463, 286, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyIdentifyTotalCB1E3C);
    drawAt(buffer, 463, 338, 0);
    const std::int32_t page = gLegacyIdentifyPageCC3F00;
    const std::int32_t pageCount = gLegacyIdentifyPageCountC9A950;
    platform::FormatLegacyString_53B304(buffer, "%d/%d", page + 1, pageCount);
    drawAt(buffer, 172, 43, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (selectedSum > 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyIdentifyPanel582918) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyIdentifyCancel589104;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyIdentifyConfirm5890F4;
    } else if (increaseHovered >= 0) {
        const char* const item = BorrowLegacyIdentifyItemC8B390(increaseHovered);
        hint = ReadRecordW32(item, 0xC28) < 1
            ? kLegacyIdentifyIncrease5890E4 : kLegacyQuantityIncreaseLimit588ABC;
    } else if (decreaseHovered >= 0) {
        hint = ReadRecordW32(BorrowLegacyIdentifyItemC8B390(decreaseHovered), 0xC28) > 0
            ? kLegacyIdentifyDecrease5890D0 : kLegacyQuantityDecreaseLimit588944;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    } else if (increaseHovered < 0 && decreaseHovered < 0) {
        // 原4DF10C直接读取备注，无悬停门禁；越界沿RPLAN.24，不另加保护。
        const char* const item = BorrowLegacyIdentifyItemC8B390(hovered + 10 * gLegacyIdentifyPageCC3F00);
        if (std::strlen(item + 0xC34) != 0) {
            const std::int32_t flags = ReadRecordW32(item, 0xC30);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 1);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            } else {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), item + 0xC34);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            }
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyIdentifyPanel582918, 1);
    gLegacyIdentifyPreviousHover5829DC = hovered;
    return result;
}

void Sub_4DF210(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t page)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const char* const item = BorrowLegacyIdentifyItemC8B390(index);
    if (item[0] == 0) {
        return;
    }
    const std::int32_t flags = ReadRecordW32(item, 0xC30);
    Sub_504BA0(x, y, 272, 4, 0, item + 1, item + 0xC34, 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item + 0x1E + page * 768 + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, line, 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

std::uint32_t Sub_4DF300(const std::int32_t opened)
{
    const std::int32_t x = gLegacyIdentifyConfirmPanel5829E0.x;
    const std::int32_t y = gLegacyIdentifyConfirmPanel5829E0.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 190, y + 334, x + 188, y + 334, 54, 18, kLegacyBarterYes588A08,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 403, y + 334, x + 401, y + 334, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyIdentifyConfirmPanel5829E0, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(68);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyIdentifyConfirmPanel5829E0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[512];
    char entry[32];
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        buffer[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const char* const item = BorrowLegacyIdentifyItemC8B390(index);
            if (item[0] != 0) {
                const std::int32_t selected = ReadRecordW32(item, 0xC28);
                if (selected != 0) {
                    if (count > 0) {
                        std::strcat(buffer, "|");
                    }
                    const std::int32_t inventoryIndex = ReadRecordW32(item, 0xC2C);
                    platform::FormatLegacyString_53B304(entry, "%d", inventoryIndex);
                    std::strcat(buffer, entry);
                    ++count;
                }
            }
        }
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacyIdentifyConfirmPanel5829E0.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(68));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyIdentifyConfirmPanel5829E0.y + offsetY;
        const std::int32_t left = gLegacyIdentifyConfirmPanel5829E0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::int32_t shown = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const item = BorrowLegacyIdentifyItemC8B390(index);
        if (item[0] == 0) {
            continue;
        }
        const std::int32_t selected = ReadRecordW32(item, 0xC28);
        if (selected != 0) {
            const std::int32_t dx = 302 * (shown % 2);
            const std::int32_t dy = 20 * (shown / 2);
            const std::int32_t flags = ReadRecordW32(item, 0xC30);
            const char* const name = item + 1;
            const std::int8_t color = (flags & 0x200) != 0 ? 1 : 0;
            const std::int32_t nameY = gLegacyIdentifyConfirmPanel5829E0.y + dy + 34;
            const std::int32_t nameX = gLegacyIdentifyConfirmPanel5829E0.x + dx + 24;
            static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
                static_cast<std::int16_t>(nameY), 2, 0, color, name, 0, 0));
            ++shown;
        }
    }
    const std::int32_t total = gLegacyIdentifyTotalCB1E3C;
    const std::int32_t gold = world::detail::ReadPlayerValueFC();
    platform::FormatLegacyString_53B304(buffer, kLegacyIdentifySummary589150, gold, total);
    drawAt(buffer, 134, 254);
    drawAt(kLegacyIdentifyQuestion589138, 214, 294);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyIdentifyAcceptHint589130;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacyIdentifyStopHint589124;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyIdentifyConfirmPanel5829E0, 0);
}

std::uintptr_t Sub_4DF730(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMessagePanel582AA8.x;
    const std::int32_t y = gLegacyMessagePanel582AA8.y;
    LegacyPanelButtonRecord post {x + 557, y + 394, x + 529, y + 384, 56, 20, 23216, 23217};
    LegacyPanelButtonRecord cancel {x + 557, y + 418, x + 529, y + 408, 56, 20, 23214, 23215};
    LegacyPanelButtonRecord previous {x + 192, y + 355, x + 164, y + 345, 56, 20, 23225, 23226};
    LegacyPanelButtonRecord next {x + 408, y + 355, x + 380, y + 345, 56, 20, 23227, 23228};
    std::array<LegacyPanelButtonRecord, 4> deletes;
    for (std::int32_t index = 0; index < 4; ++index) {
        deletes[index] = {x + 560, y + 52 + 76 * index, x + 532, y + 42 + 76 * index,
            56, 20, 23218, 23219};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t deleteHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacyMessagePanel582AA8, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        world::detail::gValue581800 = -1;
        std::uintptr_t result = static_cast<std::uint32_t>(
            input::IsCurrentContext_503E40(gLegacyMessageInputD67C8C));
        if (result != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            // 原44B330返回输入指针，关闭分支保留原生T*，不截回W32。
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyMessageInputD67C8C = nullptr;
        return result;
    }
    if (gLegacyMessageInputD67C8C == nullptr) {
        const std::int32_t inputY = gLegacyMessagePanel582AA8.y + 389;
        const std::int32_t inputX = gLegacyMessagePanel582AA8.x + 21;
        gLegacyMessageInputD67C8C = &input::gInputContextCC46E0;
        input::gInputContextCC46E0.text[0] = '\0';
        input::gInputContextCC46E0.caretByteOffset = 0;
        input::gInputContextCC46E0.totalByteLength = 0;
        input::InitializeContext_449CF0(&input::gInputContextCC46E0,
            inputX, inputY, 2, 0, 0, nullptr, 2, 70, 18, 0);
        input::SetContext_44B330(gLegacyMessageInputD67C8C);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto sendReply = [](const std::int32_t mode, const char* const text) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, mode, text));
    };
    char buffer[384];
    if ((QueryPanelButtonState(&post) & 1) != 0) {
        if (gLegacyMessageInputD67C8C->text[0] != '\0') {
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(
                gLegacyMessageInputD67C8C->text, buffer + 128, 255));
            sendReply(1, buffer + 128);
            static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((ui::detail::gDialogValueCF4E5C & 0x10) != 0
               && (QueryPanelButtonState(&previous) & 1) != 0) {
        buffer[0] = '\0';
        sendReply(16, buffer);
        static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((ui::detail::gDialogValueCF4E5C & 0x20) != 0
               && (QueryPanelButtonState(&next) & 1) != 0) {
        buffer[0] = '\0';
        sendReply(32, buffer);
        static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else {
        for (std::int32_t index = 0; index < 4; ++index) {
            const std::byte* const record = ui::detail::BorrowLegacyMessageRecordsD30138() + index * 0xB0;
            if (record[8] == std::byte{0}) {
                continue;
            }
            if ((QueryPanelButtonState(&deletes[index]) & 0x40) != 0) {
                deleteHovered = index;
            }
            if ((QueryPanelButtonState(&deletes[index]) & 1) != 0) {
                platform::FormatLegacyString_53B304(buffer, "%d", index);
                static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 128, 127));
                sendReply(64, buffer + 128);
                static_cast<void>(audio::Sub_5042D0(gLegacyMessagePanel582AA8.id));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int32_t font) {
        const std::int32_t top = gLegacyMessagePanel582AA8.y + offsetY;
        const std::int32_t left = gLegacyMessagePanel582AA8.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 305 - titleWidth / 2, 9, 0);
    for (std::int32_t index = 0; index < 4; ++index) {
        const std::byte* const record = ui::detail::BorrowLegacyMessageRecordsD30138() + index * 0xB0;
        const std::int32_t image = ReadRecordW32(record, 0);
        if (image > 999) {
            // 原4DFD4B图标纵坐标借旧68面板，其他文字使用旧73。
            const std::int32_t imageY = gLegacyIdentifyPanel582918.y + 70 + 76 * index;
            const std::int32_t imageX = gLegacyMessagePanel582AA8.x + 44;
            static_cast<void>(DrawGraphic(imageX, imageY, 214, image, 0, &descriptor));
        }
        const char* const name = reinterpret_cast<const char*>(record + 9);
        if (name[0] != '\0') {
            const std::int32_t nameWidth = TextMeasure(name, 0);
            drawAt(name, 160 - nameWidth / 2, 43 + 76 * index, 0);
        }
        std::int32_t timeValue = ReadRecordW32(record, 4);
        if (timeValue != 0) {
            const platform::LegacyTm53B8C6* const time = platform::LocalTime_53B8C6(&timeValue);
            if (time != nullptr) {
                const std::int32_t minutes = time->minutes;
                const std::int32_t hours = time->hours;
                const std::int32_t day = time->day;
                const std::int32_t month = time->month0based + 1;
                const std::int32_t year = time->yearMinus1900 + 1900;
                platform::FormatLegacyString_53B304(buffer, "%4d/%02d/%02d/%02d:%02d", year, month, day, hours, minutes);
                drawAt(buffer, 248, 45 + 76 * index, 1);
            }
        }
        for (std::int32_t line = 0; line < 2; ++line) {
            if (input::ExtractLegacyLine_503AB0(buffer, 128,
                    reinterpret_cast<const char*>(record + 26), line, 70) == 0) {
                buffer[0] = '\0';
            }
            drawAt(buffer, 93, 72 + 76 * index + 18 * line, 0);
        }
        if (record[8] != std::byte{0}) {
            Sub_5035E0(&deletes[index], 213, 1, gValueD69768, 0);
        }
    }
    Sub_5035E0(&post, 213, 1, gValueD69768, 0);
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    if ((ui::detail::gDialogValueCF4E5C & 0x10) != 0) {
        Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    }
    if ((ui::detail::gDialogValueCF4E5C & 0x20) != 0) {
        Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    }
    render::DrawInputText_4568A0(gLegacyMessageInputD67C8C);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMessagePanel582AA8) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&post) & 0x40) != 0) {
        hint = gLegacyMessageInputD67C8C->text[0] != '\0'
            ? kLegacyMessagePost5891B4 : kLegacyMessageEmpty5891A4;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyMessageCancel589198;
    } else if ((ui::detail::gDialogValueCF4E5C & 0x10) != 0
               && (QueryPanelButtonState(&previous) & 0x40) != 0) {
        hint = kLegacyMessagePrevious58918C;
    } else if ((ui::detail::gDialogValueCF4E5C & 0x20) != 0
               && (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyMessageNext553574;
    } else if (deleteHovered >= 0) {
        hint = kLegacyMessageDelete58917C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMessagePanel582AA8, 1);
}

YobiAction* Sub_4D36B0()
{
    const std::int32_t y = gLegacyBarterPanel5818B0.y + 245;
    const std::int32_t x = gLegacyBarterPanel5818B0.x + 469;
    const std::int32_t image = gLegacyBarterImageCF4A0C;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyBarterActionD67C50 = action;
    return action;
}

std::uintptr_t Sub_4D36F0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBarterPanel5818B0.x;
    const std::int32_t y = gLegacyBarterPanel5818B0.y;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23331, 23332};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23333, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 136, y + 51, x + 122, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 226, y + 51, x + 212, y + 39, 28, 24, 23023, 23027};
    std::array<LegacyPanelButtonRecord, 10> increases;
    std::array<LegacyPanelButtonRecord, 10> decreases;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t dx = 176 * (index / 5);
        const std::int32_t dy = 68 * (index % 5);
        increases[index] = {x + 128 + dx, y + 100 + dy, x + 114 + dx, y + 88 + dy, 28, 24, 23020, 23024};
        decreases[index] = {x + 162 + dx, y + 100 + dy, x + 148 + dx, y + 88 + dy, 28, 24, 23021, 23025};
    }
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t increaseHovered = -1;
    std::int32_t decreaseHovered = -1;
    const std::int32_t state = Sub_503160(opened, &gLegacyBarterPanel5818B0,
        +[] { static_cast<void>(Sub_4D36B0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(76) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacyBarterActionD67C50;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyBarterActionD67C50 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacyBarterActionD67C50 != nullptr) {
        SetYobiActionFlag2(gLegacyBarterActionD67C50, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBarterPanel5818B0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const std::int32_t itemId = ui::detail::gValueD2D608;
    std::int32_t available = 0;
    ui::detail::gShopAvailableCF4A24 = 0;
    for (const auto& item : world::detail::gPlayerItemsFDA8A4) {
        if (item.used != 0 && item.itemId == itemId) {
            available += item.count;
        }
    }
    ui::detail::gShopAvailableCF4A24 = available;
    gLegacyBarterTotalD2F334 = 0;
    std::int32_t total = 0;
    std::int32_t selectedTotal = 0;
    for (const auto& item : gLegacyBarterItemsCA0B30) {
        if (item.used != 0) {
            const std::int32_t selected = item.selected;
            selectedTotal += selected;
            total += selected * item.cost;
        }
    }
    std::int32_t hitPage = gLegacyBarterPageCB10D0;
    gLegacyBarterTotalD2F334 = total;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (gLegacyBarterItemsCA0B30[10 * hitPage + row].used == 0) {
            continue;
        }
        const std::int32_t top = gLegacyBarterPanel5818B0.y + 68 * (row % 5);
        const std::int32_t left = gLegacyBarterPanel5818B0.x + 176 * (row / 5);
        if (Sub_5049A0(left + 20, top + 92, left + 67, top + 139, -1) != 0) {
            hovered = row;
            break;
        }
        hitPage = gLegacyBarterPageCB10D0;
    }
    if (hovered != gLegacyBarterPreviousHover581974) {
        gLegacyBarterDetailPageD67C54 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyBarterPanel5818B0.id));
    } else if (selectedTotal > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(76));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        --gLegacyBarterPageCB10D0;
        if (gLegacyBarterPageCB10D0 < 0) {
            gLegacyBarterPageCB10D0 = gLegacyBarterPageCountC8A954 - 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t page = gLegacyBarterPageCB10D0;
        const std::int32_t count = gLegacyBarterPageCountC8A954;
        gLegacyBarterPageCB10D0 = page + 1;
        if (page + 1 >= count) {
            gLegacyBarterPageCB10D0 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacyBarterDetailPageD67C54;
        if (gLegacyBarterDetailPageD67C54 >= 4) {
            gLegacyBarterDetailPageD67C54 = 0;
        }
    } else {
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t index = row + 10 * gLegacyBarterPageCB10D0;
            auto& item = gLegacyBarterItemsCA0B30[index];
            if (item.used == 0) {
                continue;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x40) != 0) {
                increaseHovered = index;
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x40) != 0) {
                decreaseHovered = index;
            }
            if ((QueryPanelButtonState(&increases[row]) & 0x10) != 0) {
                std::int32_t remaining = item.cost;
                remaining += gLegacyBarterTotalD2F334;
                const std::int32_t material = ui::detail::gValueD2D608;
                std::int32_t released = 0;
                for (std::int32_t slot = 8; slot < 28; ++slot) {
                    const auto& inventory = world::detail::gPlayerItemsFDA8A4[slot];
                    if (inventory.used != 0 && inventory.itemId == material) {
                        remaining -= inventory.count;
                        if (remaining < 0) {
                            break;
                        }
                        ++released;
                    }
                }
                const std::int32_t empty = world::detail::Sub_518C60();
                const std::int32_t cost = item.cost;
                const std::int32_t currentTotal = gLegacyBarterTotalD2F334;
                const std::int32_t currentAvailable = ui::detail::gShopAvailableCF4A24;
                if (currentAvailable < cost + currentTotal) {
                    gLegacyBarterMessageCC2E54 = 1;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (empty + released <= selectedTotal) {
                    gLegacyBarterMessageCC2E54 = 2;
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    const std::int32_t selected = item.selected;
                    if (selected < 20) {
                        gLegacyBarterMessageCC2E54 = 0;
                        item.selected = selected + 1;
                        static_cast<void>(audio::PlaySe_52C450(51, 320));
                    }
                }
            } else if ((QueryPanelButtonState(&decreases[row]) & 0x10) != 0) {
                const std::int32_t selected = item.selected;
                if (selected > 0) {
                    item.selected = selected - 1;
                    gLegacyBarterMessageCC2E54 = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyBarterPanel5818B0.y + 68 * (hovered % 5);
        const std::int32_t left = gLegacyBarterPanel5818B0.x + 176 * (hovered / 5);
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139),
            213, gValueD69768, 0);
        const std::int32_t page = gLegacyBarterDetailPageD67C54;
        const std::int32_t listPage = gLegacyBarterPageCB10D0;
        const std::int32_t detailY = gLegacyBarterPanel5818B0.y + 32;
        const std::int32_t detailX = gLegacyBarterPanel5818B0.x + 298;
        Sub_4D4960(detailX, detailY, hovered + 10 * listPage, page);
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyBarterPanel5818B0.y + offsetY;
        const std::int32_t left = gLegacyBarterPanel5818B0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 10; ++row) {
        const auto& item = gLegacyBarterItemsCA0B30[row + 10 * gLegacyBarterPageCB10D0];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t dx = 176 * (row / 5);
        const std::int32_t dy = 68 * (row % 5);
        const std::int32_t image = item.image;
        const std::int32_t top = gLegacyBarterPanel5818B0.y + dy + 116;
        const std::int32_t left = gLegacyBarterPanel5818B0.x + dx + 44;
        static_cast<void>(DrawGraphic(left, top, 214, image, 0, &descriptor));
        const std::int32_t cost = item.cost;
        const std::int32_t currentTotal = gLegacyBarterTotalD2F334;
        const std::int32_t currentAvailable = ui::detail::gShopAvailableCF4A24;
        const std::int8_t color = currentAvailable < cost + currentTotal ? 7 : 0;
        platform::FormatLegacyString_53B304(buffer, "%7d", cost);
        drawAt(buffer, dx + 87, dy + 120, color);
        platform::FormatLegacyString_53B304(buffer, "%2d", item.selected);
        drawAt(buffer, dx + 82, dy + 92, 0);
        drawAt(kLegacyQuantityUnit5889A4, dx + 155, dy + 120, 4);
        Sub_5035E0(&increases[row], 213, 1, gValueD69768, 0);
        Sub_5035E0(&decreases[row], 213, 1, gValueD69768, 0);
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t width = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - width / 2, 8, 0);
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t group = gLegacyBarterMessageCC2E54;
        drawAt(lines[1 + 4 * group + row].text.data(), 375, 59 + 20 * row, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", ui::detail::gShopAvailableCF4A24);
    drawAt(buffer, 463, 286, 0);
    drawAt(kLegacyQuantityUnit5889A4, 531, 286, 4);
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyBarterTotalD2F334);
    drawAt(buffer, 463, 338, 0);
    drawAt(kLegacyQuantityUnit5889A4, 531, 338, 4);
    const std::int32_t page = gLegacyBarterPageCB10D0;
    const std::int32_t pageCount = gLegacyBarterPageCountC8A954;
    platform::FormatLegacyString_53B304(buffer, "%d/%d", page + 1, pageCount);
    drawAt(buffer, 168, 43, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (selectedTotal > 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyBarterPanel5818B0) != 0) {
        hint = kLegacyEngravingClose5868EC;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0
               && (QueryPanelButtonState(&previous) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyBarterCancel588994;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyBarterConfirm588984;
    } else if (increaseHovered >= 0) {
        hint = kLegacyBarterIncrease58896C;
    } else if (decreaseHovered >= 0) {
        hint = gLegacyBarterItemsCA0B30[decreaseHovered].selected > 0
            ? kLegacyBarterDecrease588954 : kLegacyQuantityDecreaseLimit588944;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    const std::uint32_t result = Sub_503440(&gLegacyBarterPanel5818B0, 1);
    gLegacyBarterPreviousHover581974 = hovered;
    return result;
}

void Sub_4D4960(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t page)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const auto& item = gLegacyBarterItemsCA0B30[index];
    if (item.used == 0) {
        return;
    }
    const std::int32_t width = TextMeasure(item.name.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, item.name.data(), 0, 0));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item.descriptions[8 * page + row].data();
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, line, 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

std::uint32_t Sub_4D4A50(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBarterConfirmPanel581978.x;
    const std::int32_t y = gLegacyBarterConfirmPanel581978.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 190, y + 334, x + 188, y + 334, 54, 18, kLegacyBarterYes588A08,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 403, y + 334, x + 401, y + 334, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyBarterConfirmPanel581978, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(75);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBarterConfirmPanel581978.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[512];
    char entry[32];
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        buffer[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t index = 0; index < 20; ++index) {
            const auto& item = gLegacyBarterItemsCA0B30[index];
            if (item.used != 0) {
                const std::int32_t selected = item.selected;
                if (selected != 0) {
                    if (count > 0) {
                        std::strcat(buffer, "|");
                    }
                    platform::FormatLegacyString_53B304(entry, "%d|%d", index, selected);
                    std::strcat(buffer, entry);
                    ++count;
                }
            }
        }
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 256, 255));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, buffer + 256));
        static_cast<void>(audio::Sub_5042D0(gLegacyBarterConfirmPanel581978.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyBarterConfirmPanel581978.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyBarterConfirmPanel581978.y + offsetY;
        const std::int32_t left = gLegacyBarterConfirmPanel581978.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::int32_t shown = 0;
    for (const auto& item : gLegacyBarterItemsCA0B30) {
        if (item.used != 0 && item.selected != 0) {
            const std::int32_t dx = 302 * (shown % 2);
            const std::int32_t dy = 20 * (shown / 2);
            drawAt(item.name.data(), dx + 24, dy + 34);
            platform::FormatLegacyString_53B304(buffer, "x%2d", item.selected);
            drawAt(buffer, dx + 280, dy + 34);
            ++shown;
        }
    }
    const std::int32_t total = gLegacyBarterTotalD2F334;
    const std::int32_t available = ui::detail::gShopAvailableCF4A24;
    platform::FormatLegacyString_53B304(buffer, kLegacyBarterSummary5889D8, ui::detail::gShopTextCB0184.data(), available, total);
    const std::int32_t width = MeasureDefaultText_44B260(buffer);
    const std::int32_t summaryY = gLegacyBarterConfirmPanel581978.y + 254;
    static_cast<void>(DrawText(static_cast<std::int16_t>(320 - width / 2),
        static_cast<std::int16_t>(summaryY), 2, 0, 0, buffer, 0, 0));
    drawAt(kLegacyBarterQuestion5889C0, 178, 294);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyBarterAcceptHint5889B8;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacyBarterStopHint5889AC;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyBarterConfirmPanel581978, 0);
}

YobiAction* Sub_4D4ED0()
{
    const std::int32_t y = gLegacyExchangePanel581A40.y + 133;
    const std::int32_t x = gLegacyExchangePanel581A40.x + 45;
    const std::int32_t image = ui::detail::ReadLegacyChoiceImageC9A99C();
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyExchangeActionCF4B5C = action;
    return action;
}

std::uintptr_t Sub_4D4F00(const std::int32_t opened)
{
    const std::int32_t x = gLegacyExchangePanel581A40.x;
    const std::int32_t y = gLegacyExchangePanel581A40.y;
    const character_select::detail::CharacterModalUiRecord accept {
        x + 137, y + 172, x + 120, y + 172, 120, 18, kLegacyExchangeChoice588A40,
    };
    const character_select::detail::CharacterModalUiRecord cancel {
        x + 129, y + 206, x + 120, y + 206, 120, 18, kLegacyExchangeCancel588A34,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyExchangePanel581A40,
        +[] { static_cast<void>(Sub_4D4ED0()); }, 1);
    if (state == 0) {
        return 0;
    }
    YobiAction* action = gLegacyExchangeActionCF4B5C;
    if (state == 2) {
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyExchangeActionCF4B5C = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyExchangePanel581A40.id));
    }
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        char response[16];
        char escaped[16];
        std::strcpy(response, "1");
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(response, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, escaped));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
        static_cast<void>(audio::Sub_5042D0(gLegacyExchangePanel581A40.id));
    } else if ((character_select::detail::Sub_503990(&cancel) & 1) != 0) {
        static_cast<void>(audio::PlaySe_52C450(79, 320));
        static_cast<void>(audio::Sub_5042D0(gLegacyExchangePanel581A40.id));
    }
    DrawLegacySmallNpcBody(&gLegacyExchangePanel581A40);
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 0, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&cancel, 2, 0, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyExchangePanel581A40) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyExchangeHint588A20;
    } else if ((character_select::detail::Sub_503990(&cancel) & 0x40) != 0) {
        hint = kLegacyExchangeCancelHint588A10;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyExchangePanel581A40, 1);
}

YobiAction* Sub_4D5220()
{
    const std::int32_t y = gLegacyGiveReceivePanel581B08.y + 133;
    const std::int32_t x = gLegacyGiveReceivePanel581B08.x + 45;
    const std::int32_t image = ui::detail::ReadLegacyChoiceImageD2EA74();
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyGiveReceiveActionCB2228 = action;
    return action;
}

std::uintptr_t Sub_4D5250(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGiveReceivePanel581B08.x;
    const std::int32_t y = gLegacyGiveReceivePanel581B08.y;
    std::array<character_select::detail::CharacterModalUiRecord, 2> buttons {{
        {x + 122, y + 164, x + 112, y + 164, 124, 18, input::LegacyText593850()},
        {x + 122, y + 190, x + 112, y + 190, 124, 18, kLegacyGiveReceiveCancel588A70},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyGiveReceivePanel581B08,
        +[] { static_cast<void>(Sub_4D5220()); }, 1);
    if (state == 0) {
        return 0;
    }
    YobiAction* action = gLegacyGiveReceiveActionCB2228;
    if (state == 2) {
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyGiveReceiveActionCB2228 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyGiveReceivePanel581B08.id));
    }
    buttons[0].text = gLegacyGiveReceiveModeC8AA98 != 0
        ? kLegacyReceiveChoice585A40 : kLegacyGiveChoice588A64;
    char response[16];
    char escaped[16];
    for (std::int32_t choice = 0; choice < 2; ++choice) {
        if ((character_select::detail::Sub_503990(&buttons[choice]) & 1) != 0) {
            platform::FormatLegacyString_53B304(response, "%d", choice + 1);
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(response, escaped, 15));
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
                request, target, 0, escaped));
            static_cast<void>(audio::PlaySe_52C450(79, 320));
            static_cast<void>(audio::Sub_5042D0(gLegacyGiveReceivePanel581B08.id));
        }
    }
    DrawLegacySmallNpcBody(&gLegacyGiveReceivePanel581B08);
    for (const auto& button : buttons) {
        static_cast<void>(character_select::detail::Sub_503A20(&button, 2, 0, 1, gValueD69768, 218));
    }
    if (Sub_503590(&gLegacyGiveReceivePanel581B08) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyEngravingClose5868EC);
    }
    if (character_select::detail::Sub_503990(&buttons[0]) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(),
            gLegacyGiveReceiveModeC8AA98 != 0 ? kLegacyReceiveChoice585A40 : kLegacyGiveHint588A58);
    }
    if (character_select::detail::Sub_503990(&buttons[1]) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyGiveReceiveCancelHint588A4C);
    }
    return Sub_503440(&gLegacyGiveReceivePanel581B08, 1);
}

YobiAction* Sub_4D5580()
{
    const std::int32_t y = gLegacyGiveReceiveQuantityPanel581BD0.y + 245;
    const std::int32_t x = gLegacyGiveReceiveQuantityPanel581BD0.x + 469;
    const std::int32_t image = gLegacyGiveReceiveImageD2D814;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyGiveReceiveQuantityActionCB1DD4 = action;
    return action;
}

std::uintptr_t Sub_4D55C0(const std::int32_t opened)
{
    const std::int32_t y = gLegacyGiveReceiveQuantityPanel581BD0.y;
    const std::int32_t x = gLegacyGiveReceiveQuantityPanel581BD0.x;
    LegacyPanelButtonRecord confirm {x + 418, y + 396, x + 378, y + 380, 80, 32, 23331, 23332};
    LegacyPanelButtonRecord disabled {x + 418, y + 396, x + 378, y + 380, 80, 32, 23333, 0};
    LegacyPanelButtonRecord cancel {x + 514, y + 396, x + 474, y + 380, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 136, y + 51, x + 122, y + 39, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 226, y + 51, x + 212, y + 39, 28, 24, 23023, 23027};
    LegacyPanelButtonRecord increase {x + 128, y + 100, x + 114, y + 88, 28, 24, 23020, 23024};
    LegacyPanelButtonRecord decrease {x + 162, y + 100, x + 148, y + 88, 28, 24, 23021, 23025};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGiveReceiveQuantityPanel581BD0,
        +[] { static_cast<void>(Sub_4D5580()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(85) == 0) {
            world::detail::gValue581800 = -1;
        }
        YobiAction* action = gLegacyGiveReceiveQuantityActionCB1DD4;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyGiveReceiveQuantityActionCB1DD4 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (gLegacyGiveReceiveQuantityActionCB1DD4 != nullptr) {
        SetYobiActionFlag2(gLegacyGiveReceiveQuantityActionCB1DD4, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyGiveReceiveQuantityPanel581BD0.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const char* const detail = ui::detail::gShopDetailCC3210.data();
    const auto value = [detail](const std::size_t offset) { return ReadRecordW32(detail, offset); };
    std::int32_t available = 0;
    if (ui::detail::gShopValueCAFEA4 == 0) {
        available = value(0xC24);
    } else {
        const std::int32_t itemId = value(0xC28);
        for (const auto& item : world::detail::gPlayerItemsFDA8A4) {
            if (item.used != 0 && item.itemId == itemId) {
                available += item.count;
            }
        }
    }
    const std::int32_t hitY = gLegacyGiveReceiveQuantityPanel581BD0.y;
    const std::int32_t price = value(0xC2C);
    const std::int32_t total = price * ui::detail::gShopValueD046C4;
    const std::int32_t hitX = gLegacyGiveReceiveQuantityPanel581BD0.x;
    const bool hovered = Sub_5049A0(hitX + 20, hitY + 92, hitX + 67, hitY + 139, -1) != 0;
    if (!hovered) {
        gLegacyGiveReceiveDetailPageD2EAB0 = 0;
    }
    if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyGiveReceiveQuantityPanel581BD0.id));
    } else if (ui::detail::gShopValueD046C4 > 0 && (QueryPanelButtonState(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(85));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if (hovered && (static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
        ++gLegacyGiveReceiveDetailPageD2EAB0;
        if (gLegacyGiveReceiveDetailPageD2EAB0 >= 4) {
            gLegacyGiveReceiveDetailPageD2EAB0 = 0;
        }
    } else if ((QueryPanelButtonState(&increase) & 0x10) != 0) {
        if (ui::detail::gShopValueCAFEA4 == 0) {
            const std::int32_t selected = ui::detail::gShopValueD046C4;
            const std::int32_t capacity = value(0xC30);
            const std::int32_t current = value(0xC34);
            const std::int32_t remaining = available - selected;
            if (remaining > 0 && current + selected < capacity && selected < ui::detail::gQuantityLimit581C94) {
                ui::detail::gShopValueD672D4 = 0;
                ui::detail::gShopValueD046C4 = selected + 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                if (remaining <= 0) {
                    ui::detail::gShopValueD672D4 = 1;
                } else if (current + selected >= capacity) {
                    ui::detail::gShopValueD672D4 = 2;
                }
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                ui::detail::gShopValueD046C4 = 0;
            }
        } else {
            const std::int32_t selected = ui::detail::gShopValueD046C4;
            const std::int32_t quantity = value(0xC24);
            if (selected >= quantity) {
                ui::detail::gShopValueD672D4 = 1;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                ui::detail::gShopValueD046C4 = 0;
            } else {
                const std::int32_t current = value(0xC34);
                const std::int32_t money = world::detail::ReadPlayerValueFC();
                const std::int32_t remaining = current - selected;
                if (remaining > 0 && total < money && selected < ui::detail::gQuantityLimit581C94) {
                    ui::detail::gShopValueD672D4 = 0;
                    ui::detail::gShopValueD046C4 = selected + 1;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    if (remaining <= 0) {
                        ui::detail::gShopValueD672D4 = 2;
                    } else if (total >= money) {
                        ui::detail::gShopValueD672D4 = 3;
                    }
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                    ui::detail::gShopValueD046C4 = 0;
                }
            }
        }
    } else if ((QueryPanelButtonState(&decrease) & 0x10) != 0) {
        const std::int32_t selected = ui::detail::gShopValueD046C4;
        if (selected > 0) {
            ui::detail::gShopValueD672D4 = 0;
            ui::detail::gShopValueD046C4 = selected - 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            const std::int32_t capacity = value(0xC30);
            const std::int32_t current = value(0xC34);
            std::int32_t amount = capacity - current;
            if (amount >= available) {
                amount = available;
            }
            const std::int32_t limit = ui::detail::gQuantityLimit581C94;
            ui::detail::gShopValueD046C4 = amount;
            if (amount >= limit) {
                ui::detail::gShopValueD046C4 = limit;
            }
        }
    }
    if (hovered) {
        const std::int32_t top = gLegacyGiveReceiveQuantityPanel581BD0.y;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyGiveReceiveQuantityPanel581BD0.x;
        DrawGraphicHover(Low16(left + 20), Low16(top + 92), Low16(left + 67), Low16(top + 139), 213, color, 0);
        const std::int32_t detailY = gLegacyGiveReceiveQuantityPanel581BD0.y + 32;
        const std::int32_t page = gLegacyGiveReceiveDetailPageD2EAB0;
        const std::int32_t detailX = gLegacyGiveReceiveQuantityPanel581BD0.x + 298;
        static_cast<void>(Sub_4D61E0(detailX, detailY, page));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
                           const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyGiveReceiveQuantityPanel581BD0.y + offsetY;
        const std::int32_t left = gLegacyGiveReceiveQuantityPanel581BD0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, text, 0, 0));
    };
    const std::int32_t image = value(0xC20);
    const std::int32_t imageY = gLegacyGiveReceiveQuantityPanel581BD0.y + 116;
    const std::int32_t imageX = gLegacyGiveReceiveQuantityPanel581BD0.x + 44;
    static_cast<void>(DrawGraphic(imageX, imageY, 214, image, 0, &descriptor));
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%7d", value(0xC34));
    drawAt(buffer, 87, 120, 0);
    platform::FormatLegacyString_53B304(buffer, "%2d", ui::detail::gShopValueD046C4);
    drawAt(buffer, 82, 92, 0);
    drawAt(kLegacyQuantityUnit5889A4, 155, 120, 4);
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    drawAt(lines[0].text.data(), 288 - titleWidth / 2, 8, 0);
    if (ui::detail::gShopValueCAFEA4 == 0 && available <= 0) {
        ui::detail::gShopValueD672D4 = 3;
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t group = ui::detail::gShopValueD672D4;
        drawAt(lines[1 + 4 * group + row].text.data(), 375, 59 + 20 * row, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", ui::detail::gShopValueCAFEA4 != 0 ? available : value(0xC24));
    drawAt(buffer, 463, 286, 0);
    drawAt(kLegacyQuantityUnit5889A4, 531, 286, 4);
    platform::FormatLegacyString_53B304(buffer, "%7d", ui::detail::gShopValueD046C4);
    drawAt(buffer, 463, 338, 0);
    drawAt(kLegacyQuantityUnit5889A4, 531, 338, 4);
    platform::FormatLegacyString_53B304(buffer, "1/1");
    drawAt(buffer, 168, 43, 0);
    Sub_5035E0(&increase, 213, 1, gValueD69768, 0);
    Sub_5035E0(&decrease, 213, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    if (ui::detail::gShopValueD046C4 != 0) {
        Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    } else {
        Sub_5035E0(&disabled, 213, 1, gValueD69768, 1);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGiveReceiveQuantityPanel581BD0) != 0) {
        hint = kLegacyEngravingClose5868EC;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = ui::detail::gShopValueCAFEA4 != 0 ? kLegacyReceiveCancel588AEC : kLegacyGiveCancel588AFC;
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = ui::detail::gShopValueCAFEA4 != 0 ? kLegacyReceiveHint588AE0 : kLegacyGiveHint588A58;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    if ((QueryPanelButtonState(&increase) & 0x40) != 0) {
        const std::int32_t mode = ui::detail::gShopValueCAFEA4;
        const std::int32_t selected = ui::detail::gShopValueD046C4;
        bool canIncrease = false;
        if (mode == 0) {
            if (available - selected > 0) {
                const std::int32_t current = value(0xC34);
                const std::int32_t capacity = value(0xC30);
                canIncrease = current + selected < capacity && selected < ui::detail::gQuantityLimit581C94;
            }
            hint = canIncrease ? kLegacyGiveIncrease588ACC : kLegacyQuantityIncreaseLimit588ABC;
        } else {
            canIncrease = selected < value(0xC24) && value(0xC34) - selected > 0
                && total < world::detail::ReadPlayerValueFC() && selected < ui::detail::gQuantityLimit581C94;
            hint = canIncrease ? kLegacyReceiveIncrease588AA8 : kLegacyQuantityIncreaseLimit588ABC;
        }
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    if ((QueryPanelButtonState(&decrease) & 0x40) != 0) {
        hint = ui::detail::gShopValueD046C4 <= 0 ? kLegacyQuantityDecreaseLimit588944
            : (ui::detail::gShopValueCAFEA4 != 0 ? kLegacyReceiveDecrease588A80 : kLegacyGiveDecrease588A94);
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGiveReceiveQuantityPanel581BD0, 1);
}

std::int32_t Sub_4D61E0(const std::int32_t x, const std::int32_t y, const std::int32_t page)
{
    const char* const name = ui::detail::gShopDetailCC3210.data();
    const std::int32_t width = TextMeasure(name, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, name, 0, 0));
    const char* const description = ui::detail::gShopDetailCC3210.data() + 0x1D + 768 * page;
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = description + 96 * row;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, line, 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

std::uintptr_t Sub_4E0380(const std::int32_t opened)
{
    const std::int32_t x = gLegacyStoragePanel582B70.x;
    const std::int32_t y = gLegacyStoragePanel582B70.y;
    LegacyPanelButtonRecord confirm {x + 167, y + 345, x + 105, y + 334, 124, 22, 23345, 23346};
    LegacyPanelButtonRecord cancel {x + 394, y + 345, x + 332, y + 334, 124, 22, 23272, 23273};
    const std::array<std::uint32_t (*)(), 4> pages {
        &Sub_4E2030, &Sub_4E28F0, &Sub_4E2D90, &Sub_4E3750,
    };
    constexpr std::array<std::array<std::int32_t, 4>, 4> tabs {{
        {14, 64, 134, 82}, {151, 64, 271, 82}, {289, 64, 409, 82}, {426, 64, 546, 82},
    }};
    const std::array<const char*, 4> hints {kLegacyStorageItemTab589214, kLegacyStoragePetTab589200,
        kLegacyStorageDepositTab5891F8, kLegacyStorageWithdrawTab5891F0};
    const std::int32_t state = Sub_503160(opened, &gLegacyStoragePanel582B70, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        world::detail::gValue581800 = -1;
        if (input::IsCurrentContext_503E40(nullptr) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
        }
        YobiAction* action = gLegacyStoragePetActionD67C90;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyStoragePetActionD67C90 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    YobiAction* action = gLegacyStoragePetActionD67C90;
    if (action == nullptr) {
        const std::int32_t top = gLegacyStoragePanel582B70.y;
        const std::int32_t left = gLegacyStoragePanel582B70.x;
        action = Sub_4F9FF0(0, left + 74, top + 247, 228);
        gLegacyStoragePetActionD67C90 = action;
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, true);
    }
    std::int32_t hovered = -1;
    for (std::int32_t tab = 0; tab < 4; ++tab) {
        const std::int32_t top = gLegacyStoragePanel582B70.y;
        const std::int32_t left = gLegacyStoragePanel582B70.x;
        if (Sub_5049A0(left + tabs[tab][0], top + tabs[tab][1],
                left + tabs[tab][2], top + tabs[tab][3], -1) != 0
            && ReadRecordW32(ui::detail::gShopValueTextCF4A28.data(), 12) != tab) {
            hovered = tab;
            break;
        }
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyStoragePanel582B70.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    char buffer[64];
    if (gLegacyStorageSplitModeCA0AA8 != 0) {
        hovered = -1;
    } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        std::int32_t takeItems = 0;
        std::int32_t putItems = 0;
        for (std::int32_t item = 0; item < 20; ++item) {
            if (ReadLegacyStorageW16(BorrowLegacyStorageItem(0, item), 2) > 0) {
                takeItems |= 1 << item;
            }
            if (ReadLegacyStorageW16(BorrowLegacyStorageItem(1, item), 2) > 0) {
                putItems |= 1 << item;
            }
        }
        std::int32_t takePets = 0;
        std::int32_t putPets = 0;
        for (std::int32_t pet = 0; pet < 5; ++pet) {
            if (ReadLegacyStorageW16(BorrowLegacyStoragePet(0, pet), 2) > 0) {
                takePets |= 1 << pet;
            }
            if (ReadLegacyStorageW16(BorrowLegacyStoragePet(1, pet), 2) > 0) {
                putPets |= 1 << pet;
            }
        }
        const std::int32_t deposit = gLegacyStorageMoneyD2F79C;
        const std::int32_t withdraw = gLegacyStorageWithdrawAmountCF49B8;
        platform::FormatLegacyString_53B304(buffer, "%d|%d|%d|%d|%d|%d", takeItems, putItems, takePets, putPets, withdraw, deposit);
        char escaped[128];
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 127));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(platform::gConnectionSocket, playerX, playerY,
            request, target, 0, escaped));
        static_cast<void>(audio::Sub_5042D0(gLegacyStoragePanel582B70.id));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyStoragePanel582B70.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if (hovered >= 0 && (gValueD68D9C & 1) != 0) {
        std::memcpy(ui::detail::gShopValueTextCF4A28.data() + 12, &hovered, sizeof(hovered));
        input::SetContext_44B330(hovered == 2 || hovered == 3 ? nullptr : &input::gInputContext626AA0);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    const auto update = pages[ReadRecordW32(ui::detail::gShopValueTextCF4A28.data(), 12)];
    if (update != nullptr) {
        static_cast<void>(update());
    }
    // 确认掩码不查used；逐帧汇总则只统计used且正选择的记录。
    std::int32_t fee = 0;
    std::int32_t takeItems = 0;
    std::int32_t putItems = 0;
    gLegacyStorageFeeD40B04 = 0;
    gLegacyStorageTakeCountCF4A04 = 0;
    gLegacyStoragePutCountD2DAD4 = 0;
    for (std::int32_t item = 0; item < 20; ++item) {
        const char* const left = BorrowLegacyStorageItem(0, item);
        const char* const right = BorrowLegacyStorageItem(1, item);
        if (ReadLegacyStorageW16(left, 0) != 0 && ReadLegacyStorageW16(left, 2) > 0) {
            ++takeItems;
        }
        if (ReadLegacyStorageW16(right, 0) != 0 && ReadLegacyStorageW16(right, 2) > 0) {
            ++putItems;
            ++fee;
        }
    }
    gLegacyStoragePutCountD2DAD4 = putItems;
    gLegacyStorageTakeCountCF4A04 = takeItems;
    const std::int32_t level = world::detail::ReadPlayerLevel();
    gLegacyStorageFeeD40B04 = fee;
    gLegacyStoragePetTakeCountD2D824 = 0;
    gLegacyStoragePetPutCountD2D600 = 0;
    std::int32_t takePets = 0;
    std::int32_t putPets = 0;
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const char* const left = BorrowLegacyStoragePet(0, pet);
        const char* const right = BorrowLegacyStoragePet(1, pet);
        if (ReadLegacyStorageW16(left, 0) != 0 && ReadLegacyStorageW16(left, 2) > 0) {
            ++takePets;
        }
        if (ReadLegacyStorageW16(right, 0) != 0 && ReadLegacyStorageW16(right, 2) > 0) {
            ++putPets;
            fee += level;
        }
    }
    gLegacyStorageFeeD40B04 = fee;
    gLegacyStoragePetPutCountD2D600 = putPets;
    gLegacyStoragePetTakeCountD2D824 = takePets;
    if (hovered >= 0) {
        const std::uint32_t color = gValueD69768;
        const std::int32_t top = gLegacyStoragePanel582B70.y;
        const std::int32_t left = gLegacyStoragePanel582B70.x;
        DrawGraphicHover(Low16(left + tabs[hovered][0]), Low16(top + tabs[hovered][1]),
            Low16(left + tabs[hovered][2]), Low16(top + tabs[hovered][3]), 214, color, 0);
    }
    const std::int32_t nameWidth = TextMeasure(world::detail::BorrowPlayerDisplayText(1), 0);
    const std::int32_t nameY = gLegacyStoragePanel582B70.y + 9;
    const std::int32_t nameX = gLegacyStoragePanel582B70.x - nameWidth / 2 + 431;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameX), static_cast<std::int16_t>(nameY),
        2, 0, 0, world::detail::BorrowPlayerDisplayText(1), 0, 0));
    const auto drawCount = [&buffer](const std::int32_t value, const char* const format,
        const std::int32_t offsetX, const std::uint8_t color) {
        platform::FormatLegacyString_53B304(buffer, format, value);
        const std::int32_t top = gLegacyStoragePanel582B70.y + 38;
        const std::int32_t left = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 1, color, buffer, 0, 0));
    };
    drawCount(gLegacyStorageTakeCountCF4A04, kLegacyStorageItemCount5891D4, 37, 1);
    drawCount(gLegacyStoragePetTakeCountD2D824, kLegacyStoragePetCount5891CC, 131, 1);
    drawCount(gLegacyStorageWithdrawAmountCF49B8, kLegacyStorageAmount588FC4, 180, 1);
    drawCount(gLegacyStoragePutCountD2DAD4, kLegacyStorageItemCount5891D4, 311, 5);
    drawCount(gLegacyStoragePetPutCountD2D600, kLegacyStoragePetCount5891CC, 405, 5);
    drawCount(gLegacyStorageMoneyD2F79C, kLegacyStorageAmount588FC4, 455, 5);
    const std::int32_t style = gLegacyStorageSplitModeCA0AA8 != 0 ? 1 : 0;
    Sub_5035E0(&confirm, 212, 1, gValueD69768, style);
    Sub_5035E0(&cancel, 212, 1, gValueD69768, style);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyStoragePanel582B70) != 0) {
        hint = kLegacyLogout553FD0;
    } else if (hovered >= 0) {
        hint = hints[hovered];
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyStorageConfirm5891C4;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyStorageCancel5891BC;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyStoragePanel582B70, 1);
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
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
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
    platform::FormatLegacyString_53B304(buffer, "%7d",
        world::detail::gPlayerItemsFDA8A4[gLegacyStorageSplitSourceD3FACC + 8].count);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 46), 4, 1, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyStorageSplitAmountCB2220);
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

std::int32_t Sub_4E18F0(const std::int32_t side)
{
    if (side < 0 || side > 1) {
        return -1;
    }
    const auto& origin = kLegacyStoragePetRows582C54[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
        const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0];
        if (Sub_5049A0(x - 3, y - 2, x + 146, y + 16, -1) != 0) {
            return pet;
        }
    }
    return -1;
}

void Sub_4E1980(const std::int32_t side, const std::int32_t pet, const std::uint32_t color)
{
    if (side < 0 || side > 1) {
        return;
    }
    const auto& origin = kLegacyStoragePetRows582C54[side];
    const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
    const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0];
    DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146), Low16(y + 16), 213, color, 0);
}

void Sub_4E19E0(const std::int32_t side)
{
    const auto& origin = kLegacyStoragePetRows582C54[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const char* const record = BorrowLegacyStoragePet(side, pet);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            if (ReadLegacyStorageW16(record, 2) > 0) {
                const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
                const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0];
                DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146),
                    Low16(y + 16), 213, 251, 0);
            }
            const char* const name = record + 0x142;
            const std::int32_t width = TextMeasure(name, 0);
            const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
            const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0] - width / 2 + 72;
            static_cast<void>(DrawText(static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y), 2, 1, 0, name, 0, 0));
        }
        if (ReadLegacyStorageW16(record, 2) < 0) {
            const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet + 8;
            const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0] + 72;
            static_cast<void>(DrawGraphic(x, y, 218, 23348, 0, nullptr));
        }
    }
}

std::int32_t Sub_4E1B10(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const char* const record = BorrowLegacyStoragePet(side, pet);
    if (ReadLegacyStorageW16(record, 0) == 0) {
        return 0x154 * pet;
    }
    YobiAction* action = gLegacyStoragePetActionD67C90;
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
        SetYobiActionField30(gLegacyStoragePetActionD67C90, x + 74);
        SetYobiActionField34(gLegacyStoragePetActionD67C90, y + 220);
        action = gLegacyStoragePetActionD67C90;
        const std::int32_t image = ReadRecordW32(record, 8);
        if (GetYobiActionField2A8(action) != image) {
            const std::int32_t mapped = character_select::detail::Sub_51B740(image);
            SetYobiActionField2A8(gLegacyStoragePetActionD67C90, mapped);
        }
    }
    for (std::int32_t element = 0; element < 4; ++element) {
        const std::uint32_t color = kLegacyAttributeBars57CBB8[element][0];
        const std::int32_t value = ReadRecordW32(record, 0x34 + 4 * element);
        DrawGraphicHover(Low16(x + 191), Low16(y + 70 + 4 * element),
            Low16(x + 191 + 7 * (value / 10)), Low16(y + 73 + 4 * element), 229, color, 1);
    }
    const auto drawAt = [x, y](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 4, font, 0, text, 0, 0));
    };
    const char* const name = record + 0x142;
    const std::int32_t width = TextMeasure(name, 0);
    drawAt(name, 136 - width / 2, 5, 0);
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%3d", ReadRecordW32(record, 0x0C));
    drawAt(buffer, 191, 30, 1);
    const std::int32_t tribeWidth = TextMeasure(ui::detail::gTribeNames57CB54[ReadLegacyStorageW16(record, 4)], 1);
    drawAt(ui::detail::gTribeNames57CB54[ReadLegacyStorageW16(record, 4)], 227 - tribeWidth / 2, 50, 1);
    const std::int32_t maxHp = ReadRecordW32(record, 0x14);
    const std::int32_t hp = ReadRecordW32(record, 0x10);
    platform::FormatLegacyString_53B304(buffer, "%4d/%4d", hp, maxHp);
    drawAt(buffer, 191, 90, 1);
    const std::int32_t maxFp = ReadRecordW32(record, 0x1C);
    const std::int32_t fp = ReadRecordW32(record, 0x18);
    platform::FormatLegacyString_53B304(buffer, "%4d/%4d", fp, maxFp);
    drawAt(buffer, 191, 110, 1);
    for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
        platform::FormatLegacyString_53B304(buffer, "%3d", ReadRecordW32(record, 0x20 + 4 * attribute));
        drawAt(buffer, 191, 130 + 20 * attribute, 1);
    }
    platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(record, 0x44));
    drawAt(buffer, 191, 230, 1);
    drawAt("1/2", 240, 9, 1);
    return DrawGraphic(x + 136, y + 126, 222, 23277, 0, nullptr);
}

std::int32_t Sub_4E1EE0(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const char* const record = BorrowLegacyStoragePet(side, pet);
    if (ReadLegacyStorageW16(record, 0) == 0) {
        return 0x154 * pet;
    }
    std::int32_t row = 0;
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        const char* const text = record + 0x48 + 25 * skill;
        if (std::strlen(text) != 0) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 75),
                static_cast<std::int16_t>(y + 22 * row + 29), 4, 0, 0, text, 0, 0));
            static_cast<void>(DrawGraphic(x + 137, y + 22 * row + 38, 223, 23281, 0, nullptr));
            ++row;
        }
    }
    const char* const name = record + 0x142;
    const std::int32_t width = TextMeasure(name, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, name, 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 240),
        static_cast<std::int16_t>(y + 9), 4, 1, 0, "2/2", 0, 0));
    return DrawGraphic(x + 136, y + 126, 222, 23280, 0, nullptr);
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
        platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
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

std::uint32_t Sub_4E28F0()
{
    if (gLegacyStoragePetMessageD67CA4 > 0 && gLegacyStoragePetTickD67CA8 != 0) {
        const std::uint32_t now = platform::ReadTickCount32();
        if (gLegacyStoragePetTickD67CA8 + 2000u < now) {
            gLegacyStoragePetMessageD67CA4 = 0;
            gLegacyStoragePetTickD67CA8 = 0;
        }
    }
    const std::int32_t left = Sub_4E18F0(0);
    const std::int32_t right = Sub_4E18F0(1);
    if (left != gLegacyStoragePetPreviousLeft582C6C || right != gLegacyStoragePetPreviousRight582C70) {
        gLegacyStoragePetPageD67CA0 = 0;
    }
    const std::uint8_t mouse = static_cast<std::uint8_t>(gValueD68D9C);
    if (left >= 0 && (mouse & 0x10) != 0) {
        char* const record = BorrowLegacyStoragePet(0, left);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            const std::int16_t selection = ReadLegacyStorageW16(record, 2);
            if (selection >= 0) {
                WriteLegacyStorageSelection(record, selection == 1 ? 0 : static_cast<std::int16_t>(selection ^ 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    } else if (right >= 0 && (mouse & 0x10) != 0) {
        char* const record = BorrowLegacyStoragePet(1, right);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            const std::int16_t selection = ReadLegacyStorageW16(record, 2);
            if (selection >= 0) {
                if (selection == 1) {
                    WriteLegacyStorageSelection(record, 0);
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    const std::int32_t available = world::detail::ReadPlayerValueFC()
                        - gLegacyStorageFeeD40B04 - gLegacyStorageMoneyD2F79C;
                    if (available < world::detail::ReadPlayerLevel()) {
                        gLegacyStoragePetMessageD67CA4 = 1;
                        gLegacyStoragePetTickD67CA8 = platform::ReadTickCount32();
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else {
                        WriteLegacyStorageSelection(record, static_cast<std::int16_t>(selection ^ 1));
                        static_cast<void>(audio::PlaySe_52C450(51, 320));
                    }
                }
            }
        }
    } else if ((left >= 0 || right >= 0) && (mouse & 2) != 0) {
        gLegacyStoragePetPageD67CA0 ^= 1;
    }
    if (left >= 0) {
        Sub_4E1980(0, left, gValueD69768);
        const std::int32_t page = gLegacyStoragePetPageD67CA0;
        const std::int32_t y = gLegacyStoragePanel582B70.y + 50;
        const std::int32_t x = gLegacyStoragePanel582B70.x + 283;
        if (page == 0) {
            static_cast<void>(Sub_4E1B10(x, y, 0, left));
        } else {
            static_cast<void>(Sub_4E1EE0(x, y, 0, left));
        }
    } else if (right >= 0) {
        Sub_4E1980(1, right, gValueD69768);
        const std::int32_t page = gLegacyStoragePetPageD67CA0;
        const std::int32_t y = gLegacyStoragePanel582B70.y + 50;
        const std::int32_t x = gLegacyStoragePanel582B70.x + 5;
        if (page == 0) {
            static_cast<void>(Sub_4E1B10(x, y, 1, right));
        } else {
            static_cast<void>(Sub_4E1EE0(x, y, 1, right));
        }
    }
    Sub_4E19E0(0);
    Sub_4E19E0(1);
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t x = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y), 2, font, 0, text, 0, 0));
    };
    char buffer[64];
    const std::int32_t message = gLegacyStoragePetMessageD67CA4;
    if (message == 1) {
        drawAt(kLegacyStorageNoMoney58906C, 63, 305, 0);
    } else if (message == 0) {
        drawAt(kLegacyStorageFee589284, 80, 305, 0);
        platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    std::uint32_t result = static_cast<std::uint32_t>(
        DrawGraphic(backgroundX, backgroundY, 212, 23342, 0, nullptr));
    const char* hint = nullptr;
    if (left >= 0) {
        result = static_cast<std::uint32_t>(0x154 * left);
        const char* const record = BorrowLegacyStoragePet(0, left);
        if (ReadLegacyStorageW16(record, 0) != 0 && ReadLegacyStorageW16(record, 2) >= 0) {
            hint = kLegacyStoragePetTake5892C4;
            result = 0;
        }
    }
    if (hint == nullptr && right >= 0) {
        result = static_cast<std::uint32_t>(0x154 * right);
        const char* const record = BorrowLegacyStoragePet(1, right);
        if (ReadLegacyStorageW16(record, 0) != 0) {
            const std::int16_t selection = ReadLegacyStorageW16(record, 2);
            result = (result & 0xFFFF0000u) | static_cast<std::uint16_t>(selection);
            if (selection > 0) {
                hint = kLegacyStoragePetPut5892B4;
                result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
            } else if (selection == 0) {
                if (gLegacyStoragePetPutCountD2D600 < gLegacyStoragePetCapacityCB1DDC) {
                    hint = kLegacyStoragePetCanPut5892A0;
                    result = 0;
                } else {
                    hint = kLegacyStoragePetFull58928C;
                    result = static_cast<std::uint32_t>(std::strlen(hint) + 1);
                }
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    gLegacyStoragePetPreviousRight582C70 = right;
    gLegacyStoragePetPreviousLeft582C6C = left;
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
    platform::FormatLegacyString_53B304(buffer, "%7d", balance);
    drawAt(buffer, 146, 150, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", maximum);
    drawAt(buffer, 421, 118, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", readDraft());
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
        platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
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
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 423, 151, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", maximum);
    drawAt(buffer, 143, 118, 0);
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyStorageWithdrawDraftD2EA98);
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
    platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, currentBalance);
    drawAt(buffer, 139, 306, 1);
    drawAt(kLegacyStorageFee589284, 358, 305, 0);
    platform::FormatLegacyString_53B304(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
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
        platform::FormatLegacyString_53B304(buffer, "%3d", record.level);
        const std::int32_t levelY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 76;
        const std::int32_t levelX = gLegacyItemUseStatusPanel582C78.x + 107;
        static_cast<void>(DrawText(static_cast<std::int16_t>(levelX),
            static_cast<std::int16_t>(levelY), 2, 0, 0, buffer, 0, 0));
        platform::FormatLegacyString_53B304(buffer, "%-16s", record.name.data());
        const std::uint8_t color = static_cast<std::uint8_t>(colors[record.kind]);
        const std::int32_t nameY = gLegacyItemUseStatusPanel582C78.y + 20 * index + 76;
        const std::int32_t nameX = gLegacyItemUseStatusPanel582C78.x + 143;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, color, buffer, 0, 0));
        const std::int32_t maxFp = record.maxFp;
        const std::int32_t fp = record.fp;
        const std::int32_t maxHp = record.maxHp;
        const std::int32_t hp = record.hp;
        platform::FormatLegacyString_53B304(buffer, "%4d/%4d %4d/%4d", hp, maxHp, fp, maxFp);
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

std::uint32_t Sub_4E5E40(const std::int32_t opened)
{
    const std::int32_t x = gLegacyTradePanel582F00.x;
    const std::int32_t y = gLegacyTradePanel582F00.y;
    LegacyPanelButtonRecord publish {x + 419, y + 314, x + 377, y + 303, 84, 22, 23264, 23265};
    LegacyPanelButtonRecord cancel {x + 394, y + 346, x + 332, y + 335, 124, 22, 23272, 23273};
    LegacyPanelButtonRecord confirm {x + 166, y + 346, x + 104, y + 335, 124, 22, 23270, 23271};
    const std::array<std::int32_t (*)(), 3> remotePages {&Sub_4E7FC0, &Sub_4E81B0, &Sub_4E82F0};
    const std::array<std::int32_t (*)(), 3> localPages {&Sub_4E83D0, &Sub_4E8920, &Sub_4E8AD0};
    constexpr std::array<std::array<std::int32_t, 4>, 3> remoteTabs {{
        {13, 65, 93, 81}, {102, 65, 182, 81}, {190, 65, 270, 81},
    }};
    constexpr std::array<std::array<std::int32_t, 4>, 3> localTabs {{
        {292, 65, 372, 81}, {380, 65, 460, 81}, {468, 65, 548, 81},
    }};
    const std::array<const char*, 3> phaseTexts {
        kLegacyTradeChoosing5894CC, kLegacyTradePublished5894B8, kLegacyTradeConfirming5894AC,
    };
    constexpr std::array<std::uint8_t, 3> phaseColors {6, 4, 5};
    const std::int32_t state = Sub_503160(opened, &gLegacyTradePanel582F00, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (input::IsCurrentContext_503E40(nullptr) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
        }
        if (gLegacyTradePetActionD67CB4 != nullptr) {
            static_cast<void>(MarkDeath(gLegacyTradePetActionD67CB4));
            gLegacyTradePetActionD67CB4 = nullptr;
        }
        return static_cast<std::uint32_t>(network::Sub_512270(platform::gConnectionSocket, 0));
    }
    YobiAction* action = gLegacyTradePetActionD67CB4;
    if (action == nullptr) {
        const std::int32_t top = gLegacyTradePanel582F00.y;
        const std::int32_t left = gLegacyTradePanel582F00.x;
        action = Sub_4F9FF0(0, left + 74, top + 247, 228);
        gLegacyTradePetActionD67CB4 = action;
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, true);
    }
    std::int32_t remoteHovered = -1;
    for (std::int32_t tab = 0; tab < 3; ++tab) {
        const std::int32_t top = gLegacyTradePanel582F00.y;
        const std::int32_t left = gLegacyTradePanel582F00.x;
        if (Sub_5049A0(left + remoteTabs[tab][0], top + remoteTabs[tab][1],
                left + remoteTabs[tab][2], top + remoteTabs[tab][3], -1) != 0
            && ui::detail::gTradeValueCF4A1C != tab) {
            remoteHovered = tab;
            break;
        }
    }
    std::int32_t localHovered = -1;
    for (std::int32_t tab = 0; tab < 3; ++tab) {
        const std::int32_t top = gLegacyTradePanel582F00.y;
        const std::int32_t left = gLegacyTradePanel582F00.x;
        if (Sub_5049A0(left + localTabs[tab][0], top + localTabs[tab][1],
                left + localTabs[tab][2], top + localTabs[tab][3], -1) != 0
            && ui::detail::gTradeValueD673E0 != tab) {
            localHovered = tab;
            break;
        }
    }
    if (ui::detail::gTradeValueD2E2B0 <= 1) {
        ui::detail::gTradeValueD67CB8 = 0;
    }
    if (ui::detail::gTradeValueCC2344 != 0) {
        remoteHovered = -1;
        localHovered = -1;
    } else {
        const std::uint8_t mouse = static_cast<std::uint8_t>(gValueD68D9C);
        if (remoteHovered >= 0 && (mouse & 1) != 0) {
            ui::detail::gTradeValueCF4A1C = remoteHovered;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else if (localHovered >= 0 && (mouse & 1) != 0) {
            ui::detail::gTradeValueD673E0 = localHovered;
            input::SetContext_44B330(localHovered == 2 ? nullptr : &input::gInputContext626AA0);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else if ((QueryPanelButtonState(&publish) & 1) != 0) {
            const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
            const std::uint32_t now = platform::ReadTickCount32();
            if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
                if (phase == 0) {
                    char itemSource[1024];
                    char petSource[1024];
                    char itemPayload[1024];
                    char petPayload[1024];
                    char buffer[16];
                    itemSource[0] = '\0';
                    petSource[0] = '\0';
                    std::int32_t itemCount = 0;
                    for (std::int32_t item = 0; item < 20; ++item) {
                        const char* const record = BorrowLegacyTradeItem(1, item);
                        if (ReadLegacyTradeW16(record, 0) != 0 && ReadLegacyTradeW16(record, 2) > 0) {
                            if (itemCount != 0) {
                                std::strcat(itemSource, "|");
                            }
                            platform::FormatLegacyString_53B304(buffer, "%d", item + 8);
                            std::strcat(itemSource, buffer);
                            ++itemCount;
                        }
                    }
                    platform::StrNCpy_53BEC0(itemPayload, itemSource, static_cast<std::uint32_t>(sizeof(itemPayload)));
                    std::int32_t petCount = 0;
                    for (std::int32_t pet = 0; pet < 5; ++pet) {
                        const char* const record = BorrowLegacyTradePet(1, pet);
                        if (ReadLegacyTradeW16(record, 0) != 0 && ReadLegacyTradeW16(record, 2) > 0) {
                            if (petCount != 0) {
                                std::strcat(petSource, "|");
                            }
                            platform::FormatLegacyString_53B304(buffer, "%d", pet);
                            std::strcat(petSource, buffer);
                            ++petCount;
                        }
                    }
                    platform::StrNCpy_53BEC0(petPayload, petSource, static_cast<std::uint32_t>(sizeof(petPayload)));
                    const std::int32_t amount = ui::detail::gTradeValueD046D0;
                    const auto socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_512040(socket, itemPayload, petPayload, amount));
                    ui::detail::gTradeValueD2E2B0 = 1;
                } else {
                    CancelLegacyTradeOffer();
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount32();
            }
        } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
            const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
            if ((phase == 1 && ui::detail::gTradeValueCC4224 >= 1) || phase == 2) {
                const std::uint32_t now = platform::ReadTickCount32();
                if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
                    if (phase == 1) {
                        static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 1));
                        const std::int32_t remotePhase = ui::detail::gTradeValueCC4224;
                        ui::detail::gTradeValueD2E2B0 = 2;
                        ui::detail::gTradeValueD67CB8 = 1;
                        ui::detail::gTradeValueD67CBC = 1;
                        if (remotePhase == 2) {
                            static_cast<void>(audio::Sub_5042D0(52));
                        }
                    } else {
                        CancelLegacyTradeOffer();
                    }
                    ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount32();
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
            static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 0));
            static_cast<void>(audio::Sub_5042D0(52));
            ui::detail::gTradeValueD67CBC = 1;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    // 请求关闭也继续本帧子页和绘制；第二侧页号在第一侧调用后重读。
    const auto remoteUpdate = remotePages[ui::detail::gTradeValueCF4A1C];
    if (remoteUpdate != nullptr) {
        static_cast<void>(remoteUpdate());
    }
    const auto localUpdate = localPages[ui::detail::gTradeValueD673E0];
    if (localUpdate != nullptr) {
        static_cast<void>(localUpdate());
    }
    std::int32_t remoteItems = 0;
    std::int32_t localItems = 0;
    ui::detail::gTradeValueCC4C94 = 0;
    ui::detail::gTradeValueCB03CC = 0;
    for (std::int32_t item = 0; item < 20; ++item) {
        if (ReadLegacyTradeW16(BorrowLegacyTradeItem(0, item), 0) != 0) {
            ++remoteItems;
        }
        const char* const record = BorrowLegacyTradeItem(1, item);
        if (ReadLegacyTradeW16(record, 0) != 0 && ReadLegacyTradeW16(record, 2) > 0) {
            ++localItems;
        }
    }
    ui::detail::gTradeValueCC4C94 = remoteItems;
    ui::detail::gTradeValueCB03CC = localItems;
    std::int32_t remotePets = 0;
    std::int32_t localPets = 0;
    ui::detail::gTradeValueC8AF68 = 0;
    ui::detail::gTradeValueCF4AB8 = 0;
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        if (ReadLegacyTradeW16(BorrowLegacyTradePet(0, pet), 0) != 0) {
            ++remotePets;
        }
        const char* const record = BorrowLegacyTradePet(1, pet);
        if (ReadLegacyTradeW16(record, 0) != 0 && ReadLegacyTradeW16(record, 2) > 0) {
            ++localPets;
        }
    }
    ui::detail::gTradeValueCF4AB8 = localPets;
    ui::detail::gTradeValueC8AF68 = remotePets;
    const auto drawTab = [](const std::array<std::int32_t, 4>& tab) {
        const std::uint32_t color = gValueD69768;
        const std::int32_t top = gLegacyTradePanel582F00.y;
        const std::int32_t left = gLegacyTradePanel582F00.x;
        DrawGraphicHover(Low16(left + tab[0]), Low16(top + tab[1]),
            Low16(left + tab[2]), Low16(top + tab[3]), 214, color, 0);
    };
    if (remoteHovered >= 0) {
        drawTab(remoteTabs[remoteHovered]);
    }
    if (localHovered >= 0) {
        drawTab(localTabs[localHovered]);
    }
    const std::int32_t remoteWidth = TextMeasure(ui::detail::gTradeNameD2D654.data(), 0);
    const std::int32_t remoteY = gLegacyTradePanel582F00.y + 9;
    const std::int32_t remoteX = gLegacyTradePanel582F00.x - remoteWidth / 2 + 131;
    static_cast<void>(DrawText(static_cast<std::int16_t>(remoteX), static_cast<std::int16_t>(remoteY),
        2, 0, 0, ui::detail::gTradeNameD2D654.data(), 0, 0));
    const std::int32_t localWidth = TextMeasure(world::detail::BorrowPlayerDisplayText(1), 0);
    const std::int32_t localY = gLegacyTradePanel582F00.y + 9;
    const std::int32_t localX = gLegacyTradePanel582F00.x - localWidth / 2 + 431;
    static_cast<void>(DrawText(static_cast<std::int16_t>(localX), static_cast<std::int16_t>(localY),
        2, 0, 0, world::detail::BorrowPlayerDisplayText(1), 0, 0));
    char buffer[128];
    const auto drawCount = [&buffer](const std::int32_t value, const char* const format,
        const std::int32_t offsetX) {
        platform::FormatLegacyString_53B304(buffer, format, value);
        const std::int32_t top = gLegacyTradePanel582F00.y + 38;
        const std::int32_t left = gLegacyTradePanel582F00.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 1, 0, buffer, 0, 0));
    };
    drawCount(ui::detail::gTradeValueCC4C94, kLegacyTradeItemCount5894A4, 37);
    drawCount(ui::detail::gTradeValueC8AF68, kLegacyTradePetCount58949C, 131);
    drawCount(ui::detail::gTradeValueCF4688, kLegacyTradeMoney589494, 180);
    drawCount(ui::detail::gTradeValueCB03CC, kLegacyTradeItemCount5894A4, 331);
    drawCount(ui::detail::gTradeValueCF4AB8, kLegacyTradePetCount58949C, 405);
    drawCount(ui::detail::gTradeValueD046D0, kLegacyTradeMoney589494, 455);
    const std::int32_t phaseWidth = TextMeasure(phaseTexts[ui::detail::gTradeValueCC4224], 0);
    const std::int32_t remotePhase = ui::detail::gTradeValueCC4224;
    const char* const phaseText = phaseTexts[remotePhase];
    const std::uint8_t phaseColor = phaseColors[remotePhase];
    const std::int32_t phaseY = gLegacyTradePanel582F00.y + 305;
    const std::int32_t phaseX = gLegacyTradePanel582F00.x - phaseWidth / 2 + 142;
    static_cast<void>(DrawText(static_cast<std::int16_t>(phaseX), static_cast<std::int16_t>(phaseY),
        2, 1, phaseColor, phaseText, 0, 0));
    if (ui::detail::gTradeValueCC2344 == 0) {
        Sub_5038B0(&confirm, 212, 1, gValueD69768, ui::detail::gTradeValueD67CB8);
        const std::int32_t selected = ui::detail::gTradeValueD2E2B0 > 0 ? 1 : 0;
        Sub_5038B0(&publish, 212, 1, gValueD69768, selected);
        Sub_5035E0(&cancel, 212, 1, gValueD69768, 0);
    } else {
        Sub_5038B0(&confirm, 212, 0, gValueD69768, 0);
        Sub_5038B0(&publish, 212, 1, gValueD69768, 0);
        Sub_5035E0(&cancel, 212, 1, gValueD69768, 1);
    }
    const char* hint = nullptr;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = ui::detail::gTradeValueD67CB8 != 0 ? kLegacyTradeConfirmSelected589484 : kLegacyTradeConfirm589478;
    } else if (Sub_503590(&gLegacyTradePanel582F00) != 0 || (QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyTradeCancel5889AC;
    } else if (remoteHovered == 0) {
        hint = kLegacyTradeRemoteItemTab589458;
    } else if (remoteHovered == 1) {
        hint = kLegacyTradeRemotePetTab589438;
    } else if (remoteHovered == 2) {
        hint = kLegacyTradeRemoteMoneyTab589418;
    } else if (localHovered == 0) {
        hint = kLegacyTradeLocalItemTab589400;
    } else if (localHovered == 1) {
        hint = kLegacyTradeLocalPetTab5893E8;
    } else if (localHovered == 2) {
        hint = kLegacyTradeLocalMoneyTab5893D0;
    } else if ((QueryPanelButtonState(&publish) & 0x40) != 0) {
        switch (ui::detail::gTradeValueD673E0) {
        case 0: hint = kLegacyTradePublishItems5893B4; break;
        case 1: hint = kLegacyTradePublishPets589398; break;
        case 2: hint = kLegacyTradePublishMoney589380; break;
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyTradePanel582F00, 1);
}

std::int32_t Sub_4E6CC0(const std::int32_t side)
{
    std::int32_t hovered = -1;
    if (side < 0 || side > 1) {
        return hovered;
    }
    const auto& grid = kLegacyTradeGrid582ECC[side];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t y = gLegacyTradePanel582F00.y;
            const std::int32_t x = gLegacyTradePanel582F00.x;
            if (Sub_5049A0(x + 52 * column + grid[0], y + 52 * row + grid[1],
                    x + 52 * column + grid[2], y + 52 * row + grid[3], -1) != 0) {
                hovered = column + 5 * row;
                break;
            }
        }
    }
    return hovered;
}

void Sub_4E6D70(const std::int32_t side, const std::int32_t item, const std::uint32_t color)
{
    if (side < 0 || side > 1) {
        return;
    }
    const auto& grid = kLegacyTradeGrid582ECC[side];
    const std::int32_t row = 52 * (item / 5);
    const std::int32_t column = 52 * (item % 5);
    const std::int32_t y = gLegacyTradePanel582F00.y;
    const std::int32_t x = gLegacyTradePanel582F00.x;
    DrawGraphicHover(Low16(x + column + grid[0]), Low16(y + row + grid[1]),
        Low16(x + column + grid[2]), Low16(y + row + grid[3]), 213, color, 0);
}

std::int32_t Sub_4E6E10(const std::int32_t side)
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
    const auto& grid = kLegacyTradeGrid582ECC[side];
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            char* const record = BorrowLegacyTradeItem(side, column + 5 * row);
            if (ReadLegacyTradeW16(record, 0) != 0) {
                if (ui::detail::gTradeValueCC3EC4 == column + 5 * row && side == 1) {
                    const std::int32_t y = gLegacyTradePanel582F00.y;
                    const std::int32_t x = gLegacyTradePanel582F00.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 212, 254, 0);
                }
                const std::int16_t selection = ReadLegacyTradeW16(record, 2);
                if (selection > 0) {
                    const std::int32_t y = gLegacyTradePanel582F00.y;
                    const std::int32_t x = gLegacyTradePanel582F00.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 213, 251, 0);
                } else if (selection < 0) {
                    const std::int32_t y = gLegacyTradePanel582F00.y;
                    const std::int32_t x = gLegacyTradePanel582F00.x;
                    static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                        y + 52 * row + grid[1] + 24, 218, 23040, 0, nullptr));
                }
                const std::int32_t image = ReadRecordW32(record, 4);
                const std::int32_t y = gLegacyTradePanel582F00.y;
                const std::int32_t x = gLegacyTradePanel582F00.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 214, image, 0, &descriptor));
                const std::int32_t count = ReadRecordW32(record, 8);
                if (count > 0) {
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyTradePanel582F00.y + 52 * row + grid[1] + 31;
                    const std::int32_t countX = gLegacyTradePanel582F00.x + 52 * column + grid[0] + 21;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    return 4;
}

std::int32_t Sub_4E70B0(const std::int32_t side)
{
    if (side < 0 || side > 1) {
        return -1;
    }
    const auto& origin = kLegacyTradePetRows582EEC[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
        const std::int32_t x = gLegacyTradePanel582F00.x + origin[0];
        if (Sub_5049A0(x - 3, y - 2, x + 146, y + 16, -1) != 0) {
            return pet;
        }
    }
    return -1;
}

void Sub_4E7140(const std::int32_t side, const std::int32_t pet, const std::uint32_t color)
{
    if (side < 0 || side > 1) {
        return;
    }
    const auto& origin = kLegacyTradePetRows582EEC[side];
    const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
    const std::int32_t x = gLegacyTradePanel582F00.x + origin[0];
    DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146), Low16(y + 16), 213, color, 0);
}

void Sub_4E71A0(const std::int32_t side)
{
    const auto& origin = kLegacyTradePetRows582EEC[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const char* const record = BorrowLegacyTradePet(side, pet);
        if (ReadLegacyTradeW16(record, 0) != 0) {
            if (ReadLegacyTradeW16(record, 2) > 0) {
                const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
                const std::int32_t x = gLegacyTradePanel582F00.x + origin[0];
                DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146),
                    Low16(y + 16), 213, 251, 0);
            }
            const char* const name = record + 0x5D8;
            const std::int32_t width = TextMeasure(name, 0);
            const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
            const std::int32_t x = gLegacyTradePanel582F00.x + origin[0] - width / 2 + 72;
            static_cast<void>(DrawText(static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y), 2, 1, 0, name, 0, 0));
        }
    }
}

std::int32_t Sub_4E7290(const std::int32_t x, const std::int32_t y,
    const std::int32_t item, const std::int32_t page)
{
    if (item < 0 || item >= 20) {
        return item;
    }
    const char* const record = BorrowLegacyTradeItem(0, item);
    if (ReadLegacyTradeW16(record, 0) == 0) {
        return item;
    }
    const std::int32_t flags = ReadRecordW32(record, 0xC34);
    Sub_504BA0(x, y, 272, 4, 0, record + 0x10, record + 0xC38, 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    const char* text = record + 0x2D + page * 768;
    for (std::int32_t row = 0; row < 8; ++row, text += 96) {
        if (std::strlen(text) != 0) {
            static_cast<void>(ui::detail::Sub_4FBA30(x + 10, y + 34 + 18 * row, 4, 1, 0, text, 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

std::int32_t Sub_4E7380(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const char* const record = BorrowLegacyTradePet(side, pet);
    if (ReadLegacyTradeW16(record, 0) == 0) {
        return pet + 5 * side;
    }
    YobiAction* action = gLegacyTradePetActionD67CB4;
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
        SetYobiActionField30(gLegacyTradePetActionD67CB4, x + 74);
        SetYobiActionField34(gLegacyTradePetActionD67CB4, y + 220);
        action = gLegacyTradePetActionD67CB4;
        const std::int32_t image = ReadRecordW32(record, 8);
        if (GetYobiActionField2A8(action) != image) {
            const std::int32_t mapped = character_select::detail::Sub_51B740(image);
            SetYobiActionField2A8(gLegacyTradePetActionD67CB4, mapped);
        }
    }
    for (std::int32_t element = 0; element < 4; ++element) {
        const std::uint32_t color = kLegacyAttributeBars57CBB8[element][0];
        const std::int32_t value = ReadRecordW32(record, 0x4C + 4 * element);
        DrawGraphicHover(Low16(x + 191), Low16(y + 70 + 4 * element),
            Low16(x + 191 + 7 * (value / 10)), Low16(y + 73 + 4 * element), 229, color, 1);
    }
    const auto drawAt = [x, y](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 4, font, 0, text, 0, 0));
    };
    const char* const name = record + 0x5D8;
    const std::int32_t width = TextMeasure(name, 0);
    drawAt(name, 136 - width / 2, 5, 0);
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%3d", ReadRecordW32(record, 0x0C));
    drawAt(buffer, 191, 30, 1);
    const std::int32_t tribeWidth = TextMeasure(ui::detail::gTribeNames57CB54[ReadLegacyTradeW16(record, 4)], 1);
    drawAt(ui::detail::gTribeNames57CB54[ReadLegacyTradeW16(record, 4)], 227 - tribeWidth / 2, 50, 1);
    const std::int32_t maxHp = ReadRecordW32(record, 0x14);
    const std::int32_t hp = ReadRecordW32(record, 0x10);
    platform::FormatLegacyString_53B304(buffer, "%4d/%4d", hp, maxHp);
    drawAt(buffer, 191, 90, 1);
    const std::int32_t maxFp = ReadRecordW32(record, 0x1C);
    const std::int32_t fp = ReadRecordW32(record, 0x18);
    platform::FormatLegacyString_53B304(buffer, "%4d/%4d", fp, maxFp);
    drawAt(buffer, 191, 110, 1);
    for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
        platform::FormatLegacyString_53B304(buffer, "%3d", ReadRecordW32(record, 0x38 + 4 * attribute));
        drawAt(buffer, 191, 130 + 20 * attribute, 1);
    }
    platform::FormatLegacyString_53B304(buffer, "%2d", ReadRecordW32(record, 0x5C));
    drawAt(buffer, 191, 230, 1);
    drawAt("1/2", 240, 9, 1);
    return DrawGraphic(x + 136, y + 126, 222, 23277, 0, nullptr);
}

std::int32_t Sub_4E7740(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const char* const record = BorrowLegacyTradePet(side, pet);
    if (ReadLegacyTradeW16(record, 0) == 0) {
        return pet + 5 * side;
    }
    // 每次绘制后重读技能数；未使用槽也占一行并绘底图。
    for (std::int32_t skill = 0; skill < ReadRecordW32(record, 0x5C); ++skill) {
        const char* const entry = record + 0x60 + skill * 0x8C;
        if (ReadLegacyTradeW16(entry, 0) != 0) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 75),
                static_cast<std::int16_t>(y + 22 * skill + 29), 4, 0, 0, entry + 6, 0, 0));
        }
        static_cast<void>(DrawGraphic(x + 137, y + 22 * skill + 38, 223, 23281, 0, nullptr));
    }
    const char* const name = record + 0x5D8;
    const std::int32_t width = TextMeasure(name, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, name, 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 240),
        static_cast<std::int16_t>(y + 9), 4, 1, 0, "2/2", 0, 0));
    return DrawGraphic(x + 136, y + 126, 222, 23280, 0, nullptr);
}

std::int32_t Sub_4E7870(const std::int32_t x, const std::int32_t y)
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
        world::detail::gPlayerItemsFDA8A4[gLegacyTradeSplitSourceD45F90 + 8].count;
    bool close = false;
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t amount = gLegacyTradeSplitAmountD2D490;
        if (amount != 0) {
            const std::int32_t source = gLegacyTradeSplitSourceD45F90;
            const auto socket = platform::gConnectionSocket;
            const std::int32_t target = gLegacyTradeSplitTargetD2D668;
            static_cast<void>(network::Sub_510EC0(socket, source + 8, target + 8, amount));
            gLegacyTradeSplitAmountD2D490 = 0;
            close = true;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        close = true;
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&all) & 1) != 0) {
            gLegacyTradeSplitAmountD2D490 = count;
        } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
            gLegacyTradeSplitAmountD2D490 = 0;
        } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
            gLegacyTradeSplitAmountD2D490 /= 10;
        } else {
            changed = false;
            for (std::int32_t digit = 0; digit < 10; ++digit) {
                if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                    gLegacyTradeSplitAmountD2D490 = 10 * gLegacyTradeSplitAmountD2D490 + digit;
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
        ui::detail::gTradeValueCC2344 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    // 原层关闭后仍处理键盘；上限用入口快照，显示则重新读取来源物品。
    static_cast<void>(Sub_502B80(&gLegacyTradeSplitAmountD2D490, 7));
    if (gLegacyTradeSplitAmountD2D490 > count) {
        gLegacyTradeSplitAmountD2D490 = count;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    char buffer[256];
    platform::FormatLegacyString_53B304(buffer, "%7d",
        world::detail::gPlayerItemsFDA8A4[gLegacyTradeSplitSourceD45F90 + 8].count);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 46), 4, 1, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacyTradeSplitAmountD2D490);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 86), 4, 1, 0, buffer, 0, 0));
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 223, 1, gValueD69768, 0);
    }
    if (gLegacyTradeSplitAmountD2D490 != 0) {
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

std::int32_t Sub_4E7FC0()
{
    std::int32_t hovered = Sub_4E6CC0(0);
    if (ui::detail::gTradeValueCC2344 != 0) {
        hovered = -1;
    }
    if (hovered != gLegacyTradeRemoteItemPrevious582FC4) {
        gLegacyTradeRemoteItemPageD67CC0 = 0;
    }
    if (hovered >= 0) {
        if ((static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
            ++gLegacyTradeRemoteItemPageD67CC0;
            if (gLegacyTradeRemoteItemPageD67CC0 >= world::detail::gPlayerItemsFDA8A4[hovered + 8].pages) {
                gLegacyTradeRemoteItemPageD67CC0 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        Sub_4E6D70(0, hovered, gValueD69768);
        const std::int32_t page = gLegacyTradeRemoteItemPageD67CC0;
        const std::int32_t y = gLegacyTradePanel582F00.y + 104;
        const std::int32_t x = gLegacyTradePanel582F00.x + 283;
        static_cast<void>(Sub_4E7290(x, y, hovered, page));
        const char* const record = BorrowLegacyTradeItem(0, hovered);
        if (ReadLegacyTradeW16(record, 0) != 0) {
            const std::int32_t flags = ReadRecordW32(record, 0xC34);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0x10);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            } else if ((flags & 0x200) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0xC38);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            } else {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeRemoteItem5894DC);
            }
        }
    }
    static_cast<void>(Sub_4E6E10(0));
    const std::int32_t y = gLegacyTradePanel582F00.y + 180;
    const std::int32_t x = gLegacyTradePanel582F00.x + 141;
    const std::int32_t result = DrawGraphic(x, y, 212, 23231, 0, nullptr);
    gLegacyTradeRemoteItemPrevious582FC4 = hovered;
    return result;
}

std::int32_t Sub_4E81B0()
{
    std::int32_t hovered = Sub_4E70B0(0);
    if (ui::detail::gTradeValueCC2344 != 0) {
        hovered = -1;
    }
    if (hovered != gLegacyTradeRemotePetPrevious582FC8) {
        gLegacyTradeRemotePetPageD67CC4 = 0;
    }
    if (hovered >= 0) {
        if ((static_cast<std::uint8_t>(gValueD68D9C) & 2) != 0) {
            gLegacyTradeRemotePetPageD67CC4 ^= 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        Sub_4E7140(0, hovered, gValueD69768);
        const std::int32_t page = gLegacyTradeRemotePetPageD67CC4;
        const std::int32_t y = gLegacyTradePanel582F00.y + 50;
        const std::int32_t x = gLegacyTradePanel582F00.x + 283;
        if (page == 0) {
            static_cast<void>(Sub_4E7380(x, y, 0, hovered));
        } else {
            static_cast<void>(Sub_4E7740(x, y, 0, hovered));
        }
        if (ReadLegacyTradeW16(BorrowLegacyTradePet(0, hovered), 0) != 0) {
            std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeRemotePet5894F4);
        }
    }
    Sub_4E71A0(0);
    const std::int32_t y = gLegacyTradePanel582F00.y + 180;
    const std::int32_t x = gLegacyTradePanel582F00.x + 141;
    const std::int32_t result = DrawGraphic(x, y, 212, 23233, 0, nullptr);
    gLegacyTradeRemotePetPrevious582FC8 = hovered;
    return result;
}

std::int32_t Sub_4E82F0()
{
    const std::int32_t y = gLegacyTradePanel582F00.y;
    const std::int32_t x = gLegacyTradePanel582F00.x;
    if (Sub_5049A0(x + 115, y + 146, x + 235, y + 170, -1) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeRemoteMoney58950C);
    }
    char buffer[32];
    platform::FormatLegacyString_53B304(buffer, "%7d", ui::detail::gTradeValueCF4688);
    const std::int32_t textY = gLegacyTradePanel582F00.y + 150;
    const std::int32_t textX = gLegacyTradePanel582F00.x + 146;
    static_cast<void>(DrawText(static_cast<std::int16_t>(textX),
        static_cast<std::int16_t>(textY), 2, 0, 0, buffer, 0, 0));
    const std::int32_t backgroundY = gLegacyTradePanel582F00.y + 180;
    const std::int32_t backgroundX = gLegacyTradePanel582F00.x + 141;
    return DrawGraphic(backgroundX, backgroundY, 212, 23234, 0, nullptr);
}

std::int32_t Sub_4E83D0()
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    std::int32_t hovered = Sub_4E6CC0(1);
    const std::int32_t splitMode = ui::detail::gTradeValueCC2344;
    if (splitMode != 0) {
        hovered = -1;
    }
    if (hovered != gLegacyTradePrevious582FCC) {
        gLegacyTradeDetailPageD67CC8 = 0;
    }
    if (splitMode != 0) {
        const std::int32_t y = gLegacyTradePanel582F00.y + 96;
        const std::int32_t x = gLegacyTradePanel582F00.x + 44;
        static_cast<void>(Sub_4E7870(x, y));
    }
    const std::uint8_t mouse = static_cast<std::uint8_t>(gValueD68D9C);
    bool drawDrag = true;
    if (hovered >= 0 && (mouse & 0x10) != 0) {
        char* const record = BorrowLegacyTradeItem(1, hovered);
        if (ReadLegacyTradeW16(record, 0) != 0) {
            const std::int16_t selection = ReadLegacyTradeW16(record, 2);
            if (selection >= 0) {
                static_cast<void>(audio::PlaySe_52C450(selection != 0 ? 54 : 79, 320));
                const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
                const std::int16_t toggled = static_cast<std::int16_t>(ReadLegacyTradeW16(record, 2) ^ 1);
                std::memcpy(record + 2, &toggled, sizeof(toggled));
                if (phase >= 1) {
                    CancelLegacyTradeOffer();
                }
                ui::detail::gTradeValueCC3EC4 = -1;
                ui::detail::gTradeValueD66E08 = 0;
                drawDrag = false;
            }
        }
    } else if (hovered >= 0 && (mouse & 1) != 0) {
        const std::int32_t source = ui::detail::gTradeValueCC3EC4;
        char* const target = BorrowLegacyTradeItem(1, hovered);
        if (source < 0) {
            if (ReadLegacyTradeW16(target, 0) != 0 && ReadLegacyTradeW16(target, 2) >= 0) {
                const std::int32_t image = ReadRecordW32(target, 4);
                const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
                ui::detail::gTradeValueCC3EC4 = hovered;
                ui::detail::gTradeValueD66E08 = image;
                if (phase >= 1) {
                    CancelLegacyTradeOffer();
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        } else if (source == hovered) {
            ui::detail::gTradeValueCC3EC4 = -1;
            ui::detail::gTradeValueD66E08 = 0;
            drawDrag = false;
        } else {
            const std::uint32_t moveMode = platform::gValueD67AE8;
            const std::uint8_t control = platform::gKeyStates[17];
            const bool split = (moveMode != 0 ? control & 2 : (~control) & 2) != 0;
            if (ReadRecordW32(BorrowLegacyTradeItem(1, source), 8) > 0
                && ReadLegacyTradeW16(target, 0) == 0 && split) {
                ui::detail::gTradeValueCC2344 = 1;
                gLegacyTradeSplitSourceD45F90 = source;
                gLegacyTradeSplitTargetD2D668 = hovered;
                gLegacyTradeSplitAmountD2D490 = 0;
                ui::detail::gTradeValueCC3EC4 = -1;
                ui::detail::gTradeValueD66E08 = 0;
                static_cast<void>(audio::PlaySe_52C450(56, 320));
            } else if (ReadLegacyTradeW16(target, 0) != 0 && ReadLegacyTradeW16(target, 2) < 0) {
                ui::detail::gTradeValueCC3EC4 = -1;
                ui::detail::gTradeValueD66E08 = 0;
                drawDrag = false;
            } else {
                static_cast<void>(network::Sub_510EC0(platform::gConnectionSocket, source + 8, hovered + 8, 0));
                ui::detail::gTradeValueCC3EC4 = -1;
                ui::detail::gTradeValueD66E08 = 0;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    } else if (hovered < 0 && (mouse & 1) != 0) {
        if (ui::detail::gTradeValueCC3EC4 != -1) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        ui::detail::gTradeValueCC3EC4 = -1;
        ui::detail::gTradeValueD66E08 = 0;
        drawDrag = false;
    } else if (ui::detail::gTradeValueCC3EC4 >= 0 && (mouse & 2) != 0) {
        ui::detail::gTradeValueCC3EC4 = -1;
        ui::detail::gTradeValueD66E08 = 0;
        drawDrag = false;
    } else if (hovered >= 0 && (mouse & 2) != 0) {
        ++gLegacyTradeDetailPageD67CC8;
        if (gLegacyTradeDetailPageD67CC8 >= world::detail::gPlayerItemsFDA8A4[hovered + 8].pages) {
            gLegacyTradeDetailPageD67CC8 = 0;
        }
    }
    // 原分支分别跳往拖图尾/公共尾，不能统一在清槽后重新判定。
    if (drawDrag && ui::detail::gTradeValueD66E08 != 0) {
        gLegacyTradeMouseXCF3FEC = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t image = ui::detail::gTradeValueD66E08;
        gLegacyTradeMouseYCF3FE8 = mouseY;
        const std::int32_t mouseX = gLegacyTradeMouseXCF3FEC;
        static_cast<void>(DrawGraphic(mouseX, mouseY, 230, image, 0, &descriptor));
    }
    if (hovered >= 0) {
        Sub_4E6D70(1, hovered, gValueD69768);
        const std::int32_t page = gLegacyTradeDetailPageD67CC8;
        const std::int32_t y = gLegacyTradePanel582F00.y + 104;
        const std::int32_t x = gLegacyTradePanel582F00.x + 5;
        Sub_4FB940(x, y, hovered + 8, page);
        const char* const record = BorrowLegacyTradeItem(1, hovered);
        if (ReadLegacyTradeW16(record, 0) != 0) {
            const std::int32_t flags = ReadRecordW32(record, 0xC34);
            if ((flags & 0x100) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0x10);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[kok]");
            } else if ((flags & 0x200) != 0) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), record + 0xC38);
                std::strcat(ui::detail::gHoverTextD68AE8.data(), "[han]");
            } else {
                const std::int16_t selection = ReadLegacyTradeW16(record, 2);
                const char* const hint = selection > 0 ? kLegacyTradeSelected589554
                    : (selection < 0 ? kLegacyTradeUnavailable58953C : kLegacyTradeAvailable589524);
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
            }
        }
    }
    static_cast<void>(Sub_4E6E10(1));
    const std::int32_t backgroundY = gLegacyTradePanel582F00.y + 180;
    const std::int32_t backgroundX = gLegacyTradePanel582F00.x + 419;
    const std::int32_t result = DrawGraphic(backgroundX, backgroundY, 212, 23241, 0, nullptr);
    gLegacyTradePrevious582FCC = hovered;
    return result;
}

std::int32_t Sub_4E8920()
{
    std::int32_t hovered = Sub_4E70B0(1);
    if (ui::detail::gTradeValueCC2344 != 0) {
        hovered = -1;
    }
    if (hovered != gLegacyTradeLocalPetPrevious582FD0) {
        gLegacyTradeLocalPetPageD67CCC = 0;
    }
    if (hovered >= 0) {
        const std::uint8_t mouse = static_cast<std::uint8_t>(gValueD68D9C);
        if ((mouse & 0x10) != 0) {
            char* const record = BorrowLegacyTradePet(1, hovered);
            if (ReadLegacyTradeW16(record, 0) != 0) {
                const std::int16_t selection = ReadLegacyTradeW16(record, 2);
                if (selection >= 0) {
                    const std::int16_t toggled = static_cast<std::int16_t>(selection ^ 1);
                    std::memcpy(record + 2, &toggled, sizeof(toggled));
                    if (ui::detail::gTradeValueD2E2B0 >= 1) {
                        CancelLegacyTradeOffer();
                    }
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
        } else if ((mouse & 2) != 0) {
            gLegacyTradeLocalPetPageD67CCC ^= 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        Sub_4E7140(1, hovered, gValueD69768);
        const std::int32_t page = gLegacyTradeLocalPetPageD67CCC;
        const std::int32_t y = gLegacyTradePanel582F00.y + 50;
        const std::int32_t x = gLegacyTradePanel582F00.x + 5;
        if (page == 0) {
            static_cast<void>(Sub_4E7380(x, y, 1, hovered));
        } else {
            static_cast<void>(Sub_4E7740(x, y, 1, hovered));
        }
        const char* const record = BorrowLegacyTradePet(1, hovered);
        if (ReadLegacyTradeW16(record, 0) != 0) {
            const char* const hint = ReadLegacyTradeW16(record, 2) != 0
                ? kLegacyTradePetSelected589580 : kLegacyTradePetAvailable589568;
            std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
        }
    }
    Sub_4E71A0(1);
    const std::int32_t y = gLegacyTradePanel582F00.y + 180;
    const std::int32_t x = gLegacyTradePanel582F00.x + 419;
    const std::int32_t result = DrawGraphic(x, y, 212, 23243, 0, nullptr);
    gLegacyTradeLocalPetPrevious582FD0 = hovered;
    return result;
}

std::int32_t Sub_4E8AD0()
{
    const std::int32_t x = gLegacyTradePanel582F00.x;
    const std::int32_t y = gLegacyTradePanel582F00.y;
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
    bool changed = true;
    bool cancelOffer = false;
    if ((QueryPanelButtonState(&all) & 1) != 0) {
        ui::detail::gTradeValueD046D0 = world::detail::ReadPlayerValueFC();
        cancelOffer = ui::detail::gTradeValueD2E2B0 >= 1;
    } else if ((QueryPanelButtonState(&clear) & 1) != 0) {
        const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
        ui::detail::gTradeValueD046D0 = 0;
        cancelOffer = phase >= 1;
    } else if ((QueryPanelButtonState(&backspace) & 1) != 0) {
        ui::detail::gTradeValueD046D0 /= 10;
        cancelOffer = ui::detail::gTradeValueD2E2B0 >= 1;
    } else {
        changed = false;
        for (std::int32_t digit = 0; digit < 10; ++digit) {
            if ((QueryPanelButtonState(&digits[digit]) & 1) != 0) {
                ui::detail::gTradeValueD046D0 = 10 * ui::detail::gTradeValueD046D0 + digit;
                cancelOffer = ui::detail::gTradeValueD2E2B0 >= 1;
                changed = true;
                break;
            }
        }
    }
    if (changed) {
        if (cancelOffer) {
            CancelLegacyTradeOffer();
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    const std::int32_t previousAmount = ui::detail::gTradeValueD046D0;
    static_cast<void>(Sub_502B80(&ui::detail::gTradeValueD046D0, 7));
    if (previousAmount != ui::detail::gTradeValueD046D0 && ui::detail::gTradeValueD2E2B0 >= 1) {
        CancelLegacyTradeOffer();
    }
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    if (ui::detail::gTradeValueD046D0 > money) {
        ui::detail::gTradeValueD046D0 = money;
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyTradePanel582F00.y + offsetY;
        const std::int32_t left = gLegacyTradePanel582F00.x + 421;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%7d", money);
    drawAt(buffer, 118);
    platform::FormatLegacyString_53B304(buffer, "%7d", ui::detail::gTradeValueD046D0);
    drawAt(buffer, 158);
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 213, 1, gValueD69768, 0);
    }
    Sub_5035E0(&backspace, 213, 1, gValueD69768, 0);
    Sub_5035E0(&all, 213, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if ((QueryPanelButtonState(&all) & 0x40) != 0) {
        hint = kLegacyMoneyDigit585B6C;
    } else if ((QueryPanelButtonState(&clear) & 0x40) != 0) {
        hint = kLegacyMoneyClear585B14;
    } else if ((QueryPanelButtonState(&backspace) & 0x40) != 0) {
        hint = kLegacySplitBackspace585B34;
    } else {
        for (auto& digit : digits) {
            if ((QueryPanelButtonState(&digit) & 0x40) != 0) {
                const std::int32_t amount = ui::detail::gTradeValueD046D0;
                const std::int32_t currentMoney = world::detail::ReadPlayerValueFC();
                hint = amount < currentMoney ? kLegacyMoneyDigit585B6C : kLegacyStorageAmountLimit5892D4;
                break;
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    const std::int32_t backgroundY = gLegacyTradePanel582F00.y + 180;
    const std::int32_t backgroundX = gLegacyTradePanel582F00.x + 419;
    return DrawGraphic(backgroundX, backgroundY, 212, 23244, 0, nullptr);
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
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
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

void Sub_4E97C0()
{
    gLegacyBurialPageCC4B64 = 0;
    gLegacyBurialSelectedCC4214 = -1;
}

std::uint32_t Sub_4E97E0(const std::int32_t opened)
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyBurialPanel5830A0, Sub_4E97C0, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_5042D0(80);
        world::detail::gValue581800 = -1;
        return static_cast<std::uint32_t>(result);
    }
    std::int32_t hovered = -1;
    std::int32_t usedHovered = -1;
    std::int32_t hoverRow;
    std::int32_t hoverColumn;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyBurialPanel5830A0.y + 36 + 53 * row;
            const std::int32_t left = gLegacyBurialPanel5830A0.x + 8 + 52 * column;
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
    if (hovered != gLegacyBurialHover583164) {
        gLegacyBurialPageCC4B64 = 0;
    }
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    if (usedHovered >= 0 && (clicks & 1u) != 0) {
        const std::int32_t selected = gLegacyBurialSelectedCC4214;
        bool rejected = (selected >= 0 && selected == usedHovered) || usedHovered == gLegacyBurialExcludedCC48AC;
        if (!rejected) {
            const std::uint8_t flags = static_cast<std::uint8_t>(world::detail::gPlayerItemsFDA8A4[hovered].flags);
            rejected = (flags & 0x60u) != 0;
        }
        if (rejected) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            gLegacyBurialSelectedCC4214 = usedHovered;
        }
    } else if (hovered >= 0 && (clicks & 2u) != 0) {
        gLegacyBurialPageCC4B64 ^= 1;
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[8 + row * 5 + column];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyBurialPanel5830A0.y + 60 + 53 * row;
                const std::int32_t left = gLegacyBurialPanel5830A0.x + 32 + 52 * column;
                static_cast<void>(DrawGraphic(left, top, 213, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyBurialPanel5830A0.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyBurialPanel5830A0.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t x = gLegacyBurialPanel5830A0.x;
        const std::int32_t top = gLegacyBurialPanel5830A0.y + 53 * hoverRow;
        const std::int32_t left = x + 52 * hoverColumn;
        const std::uint32_t image = gValueD69768;
        DrawGraphicHover(Low16(left + 8), Low16(top + 36), Low16(left + 55), Low16(top + 83),
            212, image, 0);
        const std::int32_t page = gLegacyBurialPageCC4B64;
        const std::int32_t y = gLegacyBurialPanel5830A0.y;
        const std::int32_t height = gLegacyBurialPanel5830A0.height;
        const std::int32_t detailX = gLegacyBurialPanel5830A0.x;
        Sub_4FB940(detailX, y + height, hovered, page);
    }
    const std::int32_t selected = gLegacyBurialSelectedCC4214;
    if (selected >= 0) {
        platform::FormatLegacyString_53B304(buffer, "%d", selected);
        char escaped[16];
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        static_cast<void>(audio::Sub_5042D0(80));
        world::detail::gValue581800 = -1;
    }
    if (gLegacyBurialExcludedCC48AC >= 0) {
        const std::int32_t row = gLegacyBurialExcludedRowD671D0;
        const std::int32_t column = gLegacyBurialExcludedColumnD2E2B4;
        const std::int32_t top = gLegacyBurialPanel5830A0.y + 53 * row;
        const std::int32_t left = gLegacyBurialPanel5830A0.x + 52 * column;
        DrawGraphicHover(Low16(left + 10), Low16(top + 38), Low16(left + 53), Low16(top + 81),
            212, 251, 0);
    }
    // 原4E9B93无悬停非负门禁；异常索引按RPLAN.24，不补记录或保护。
    const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
    const char* hint;
    if (item->used != 0) {
        const std::uint8_t flags = static_cast<std::uint8_t>(item->flags);
        hint = (flags & 0x60u) == 0 && usedHovered != gLegacyBurialExcludedCC48AC
            ? kLegacyBurialSelect5895FC : kLegacyBurialReject5895E4;
    } else {
        hint = Sub_503590(&gLegacyBurialPanel5830A0) != 0
            ? kLegacyEngravingClose5868EC : kLegacyBurialChoose5895D0;
    }
    std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    const std::uint32_t result = Sub_503440(&gLegacyBurialPanel5830A0, 1);
    gLegacyBurialHover583164 = hovered;
    return result;
}

void Sub_4E9C60()
{
    gLegacyMailGiftPageCC2E58 = 0;
    gLegacyMailGiftSelectedCC1A2C = -1;
}

std::uint32_t Sub_4E9C80(const std::int32_t opened)
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyMailGiftPanel583168, Sub_4E9C60, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        // 原4E9CD7关闭49，与本页id50不同，不擅自修正。
        return static_cast<std::uint32_t>(audio::Sub_5042D0(49));
    }
    std::int32_t hovered = -1;
    std::int32_t giftHovered = -1;
    std::int32_t hoverRow;
    std::int32_t hoverColumn;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyMailGiftPanel583168.y + 36 + 53 * row;
            const std::int32_t left = gLegacyMailGiftPanel583168.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 47, top + 47, -1) != 0) {
                hoverRow = row;
                hoverColumn = column;
                hovered = 8 + row * 5 + column;
                const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
                if (item->used != 0 && (static_cast<std::uint8_t>(item->flags) & 1u) != 0) {
                    giftHovered = hovered;
                }
                break;
            }
        }
    }
    if (hovered != gLegacyMailGiftHover58322C) {
        gLegacyMailGiftPageCC2E58 = 0;
    }
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    if (giftHovered >= 0 && (clicks & 0x10u) != 0) {
        const std::int32_t selected = gLegacyMailGiftSelectedCC1A2C;
        if (selected >= 0 && selected == giftHovered) {
            gLegacyMailGiftSelectedCC1A2C = -1;
            static_cast<void>(audio::PlaySe_52C450(54, 320));
        } else {
            gLegacyMailGiftSelectedCC1A2C = giftHovered;
            gLegacyMailGiftColumnD2D498 = hoverColumn;
            gLegacyMailGiftRowCC3E6C = hoverRow;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    } else if (hovered >= 0 && (clicks & 2u) != 0) {
        gLegacyMailGiftPageCC2E58 ^= 1;
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[8 + row * 5 + column];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyMailGiftPanel583168.y + 60 + 53 * row;
                const std::int32_t left = gLegacyMailGiftPanel583168.x + 32 + 52 * column;
                static_cast<void>(DrawGraphic(left, top, 213, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyMailGiftPanel583168.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyMailGiftPanel583168.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t x = gLegacyMailGiftPanel583168.x;
        const std::int32_t top = gLegacyMailGiftPanel583168.y + 53 * hoverRow;
        const std::int32_t left = x + 52 * hoverColumn;
        const std::uint32_t image = gValueD69768;
        DrawGraphicHover(Low16(left + 8), Low16(top + 36), Low16(left + 55), Low16(top + 83),
            212, image, 0);
        const std::int32_t page = gLegacyMailGiftPageCC2E58;
        const std::int32_t y = gLegacyMailGiftPanel583168.y;
        const std::int32_t height = gLegacyMailGiftPanel583168.height;
        const std::int32_t detailX = gLegacyMailGiftPanel583168.x;
        Sub_4FB940(detailX, y + height, hovered, page);
    }
    if (gLegacyMailGiftSelectedCC1A2C >= 0) {
        const std::int32_t row = gLegacyMailGiftRowCC3E6C;
        const std::int32_t top = gLegacyMailGiftPanel583168.y + 53 * row;
        const std::int32_t column = gLegacyMailGiftColumnD2D498;
        const std::int32_t left = gLegacyMailGiftPanel583168.x + 52 * column;
        DrawGraphicHover(Low16(left + 10), Low16(top + 38), Low16(left + 53), Low16(top + 81),
            212, 251, 0);
    }
    // 原4E9FD8直接访问同源Items[hovered]，按RPLAN.24不补记录或保护。
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
        }
    } else if (Sub_503590(&gLegacyMailGiftPanel583168) != 0) {
        std::strcpy(hint, kLegacyLogout553FD0);
    } else {
        const std::int32_t selected = gLegacyMailGiftSelectedCC1A2C;
        if (selected >= 0 && selected == hovered) {
            std::strcpy(hint, kLegacyMailGift58960C);
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyMailGiftPanel583168, 1);
    gLegacyMailGiftHover58322C = hovered;
    return result;
}

void Sub_4EA130()
{
    char preserved[400];
    std::strcpy(preserved, input::gInputContextCB03F0.text);
    const std::int32_t y = gLegacyMailComposePanel583230.y;
    const std::int32_t x = gLegacyMailComposePanel583230.x;
    input::InitializeContext_449CF0(&input::gInputContextCB03F0,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContextCB03F0);
}

std::uint32_t Sub_4EA1B0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailComposePanel583230.x;
    const std::int32_t y = gLegacyMailComposePanel583230.y;
    std::array<LegacyPanelButtonRecord, 2> buttons {{
        {x + 80, y + 197, x + 40, y + 187, 80, 20, 23196, 23197},
        {x + 192, y + 197, x + 152, y + 187, 80, 20, 22710, 22711},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyMailComposePanel583230, Sub_4EA130, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        input::SetContext_44B330(&input::gInputContext626AA0);
        // 原4EA295关闭50，与本页id49不同；沿原关闭顺序保留。
        return static_cast<std::uint32_t>(audio::Sub_5042D0(50));
    }
    if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        if (input::gInputContextCB03F0.text[0] != '\0') {
            std::int32_t timeValue;
            static_cast<void>(platform::ReadLegacyTime32_53BA26(&timeValue));
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&timeValue);
            const std::int32_t minutes = calendar->minutes;
            const std::int32_t hours = calendar->hours;
            const std::int32_t day = calendar->day;
            const std::int32_t month = calendar->month0based + 1;
            const std::int32_t year = calendar->yearMinus1900 + 1900;
            char timestamp[64];
            platform::FormatLegacyString_53B304(timestamp, "%4d/%2d/%2d %02d:%02d", year, month, day, hours, minutes);
            char source[512];
            std::strcpy(source, input::gInputContextCB03F0.text);
            char escaped[600];
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(source, escaped, 600));
            const std::int32_t gift = gLegacyMailGiftSelectedCC1A2C;
            const std::int32_t value = gLegacyMailValueCC2E60;
            const std::int32_t contact = gLegacyMailContactCC4228;
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_5110B0(socket, contact, value, gift, escaped, 0));
            const std::int32_t savedContact = gLegacyMailContactCC4228;
            world::detail::Sub_5191C0(savedContact, 1, timestamp, input::gInputContextCB03F0.text);
            static_cast<void>(world::detail::Sub_519300());
            static_cast<void>(world::detail::Sub_519550());
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            static_cast<void>(audio::Sub_504180(42));
            input::gInputContextCB03F0.text[0] = '\0';
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    }
    const std::int32_t imageContact = gLegacyMailContactCC4228;
    const std::int32_t contactY = gLegacyMailComposePanel583230.y + 84;
    const std::int32_t contactImage = world::detail::gContactsF19FB0[imageContact].value0C;
    const std::int32_t contactX = gLegacyMailComposePanel583230.x + 52;
    static_cast<void>(DrawGraphic(contactX, contactY, 212, contactImage, 0, &descriptor));
    const std::int32_t measureContact = gLegacyMailContactCC4228;
    const std::int32_t contactWidth = TextMeasure(world::detail::gContactsF19FB0[measureContact].name10.data(), 0);
    const std::int32_t textContact = gLegacyMailContactCC4228;
    const char* const contactName = world::detail::gContactsF19FB0[textContact].name10.data();
    const std::int32_t contactNameY = gLegacyMailComposePanel583230.y + 62;
    const std::int32_t contactNameX = gLegacyMailComposePanel583230.x - contactWidth / 2 + 176;
    static_cast<void>(DrawText(static_cast<std::int16_t>(contactNameX),
        static_cast<std::int16_t>(contactNameY), 2, 0, 0, contactName, 0, 0));
    const std::int32_t gift = gLegacyMailGiftSelectedCC1A2C;
    if (gift > 0) {
        const std::int32_t giftY = gLegacyMailComposePanel583230.y + 120;
        const std::int32_t image = world::detail::gPlayerItemsFDA8A4[gift].imageNo;
        const std::int32_t giftX = gLegacyMailComposePanel583230.x + 216;
        static_cast<void>(DrawGraphic(giftX, giftY, 212, image, 0, &descriptor));
        std::int32_t nameGift = gLegacyMailGiftSelectedCC1A2C;
        const std::int32_t count = world::detail::gPlayerItemsFDA8A4[nameGift].count;
        if (count > 0) {
            char buffer[128];
            platform::FormatLegacyString_53B304(buffer, "%3d", count);
            const std::int32_t countY = gLegacyMailComposePanel583230.y + 127;
            const std::int32_t countX = gLegacyMailComposePanel583230.x + 213;
            static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
            nameGift = gLegacyMailGiftSelectedCC1A2C;
        }
        const std::int32_t width = TextMeasure(world::detail::gPlayerItemsFDA8A4[nameGift].name.data(), 0);
        const std::int32_t textGift = gLegacyMailGiftSelectedCC1A2C;
        const char* const name = world::detail::gPlayerItemsFDA8A4[textGift].name.data();
        const std::int32_t nameY = gLegacyMailComposePanel583230.y + 152;
        const std::int32_t nameX = gLegacyMailComposePanel583230.x - width / 2 + 136;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, 0, name, 0, 0));
    }
    Sub_5035E0(&buttons[0], 212, 1, gValueD69768, 0);
    Sub_5035E0(&buttons[1], 212, 1, gValueD69768, 0);
    render::DrawInputText_4568A0(&input::gInputContextCB03F0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMailComposePanel583230) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = kLegacyMailSend589640;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyMailReturn589628;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailComposePanel583230, 1);
}

std::uint32_t Sub_4EA690(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailPetPanel5832F8.x;
    const std::int32_t y = gLegacyMailPetPanel5832F8.y;
    LegacyPanelButtonRecord back {x + 136, y + 206, x + 96, y + 196, 80, 20, 22710, 22711};
    const std::int32_t state = Sub_503160(opened, &gLegacyMailPetPanel5832F8, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::array<std::int32_t, 5> availability;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        const std::int32_t available = world::detail::Sub_52E1D0(index);
        availability[row] = available;
        const std::int32_t top = gLegacyMailPetPanel5832F8.y + 41 + 28 * row;
        const std::int32_t left = gLegacyMailPetPanel5832F8.x + 12;
        if (Sub_5049A0(left, top, left + 249, top + 20, -1) != 0
            && world::detail::gPetRuntimeF633F8[index].used != 0 && available != 0) {
            hovered = row;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[hovered].index);
        gLegacyMailValueCC2E60 = index;
        static_cast<void>(audio::Sub_504180(49));
        static_cast<void>(audio::Sub_504180(50));
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyMailPetPanel5832F8.y + 28 * hovered;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacyMailPetPanel5832F8.x;
        DrawGraphicHover(Low16(left + 12), Low16(top + 41), Low16(left + 261), Low16(top + 61),
            212, image, 0);
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        const auto* const pet = &world::detail::gPetRuntimeF633F8[index];
        if (pet->used == 0) {
            continue;
        }
        const char* const name = pet->name6BD[0] != '\0' ? pet->name6BD.data() : pet->name6AC.data();
        const std::int8_t color = availability[row] != 0 ? 0 : 7;
        std::strcpy(buffer, name);
        const std::int32_t width = TextMeasure(buffer, 0);
        const std::int32_t nameY = gLegacyMailPetPanel5832F8.y + 43 + 28 * row;
        const std::int32_t nameX = gLegacyMailPetPanel5832F8.x - width / 2 + 90;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, color, buffer, 0, 0));
        // 原4EA912实际F63400，即宠物+08；不借反编译lv名改成+6C。
        const std::int32_t value = pet->value08;
        platform::FormatLegacyString_53B304(buffer, "%3d", value);
        const std::int32_t valueY = gLegacyMailPetPanel5832F8.y + 45 + 28 * row;
        const std::int32_t valueX = gLegacyMailPetPanel5832F8.x + 226;
        static_cast<void>(DrawText(static_cast<std::int16_t>(valueX),
            static_cast<std::int16_t>(valueY), 2, 1, color, buffer, 0, 0));
    }
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMailPetPanel5832F8) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyMailReturn589628;
    } else if (hovered >= 0) {
        hint = kLegacyMailPet589660;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailPetPanel5832F8, 1);
}

std::uintptr_t Sub_4EC1D0()
{
    world::detail::Sub_4EC1A0();
    static_cast<void>(world::detail::Sub_518DA0());
    const std::uint32_t page = world::detail::gValueCB1384;
    world::detail::gValueCB0354 = page;
    std::uintptr_t result = page;
    for (std::int32_t row = 0; row < 4; ++row) {
        if (gLegacyMailUnreadActionsCC3E48[row] == nullptr) {
            const std::int32_t y = gLegacyMailContactPanel5837A8.y;
            const std::int32_t x = gLegacyMailContactPanel5837A8.x;
            YobiAction* const action = Sub_4EBF70(103101, x + 72, y + 38 + 90 * row, 228);
            gLegacyMailUnreadActionsCC3E48[row] = action;
            result = reinterpret_cast<std::uintptr_t>(action);
            if (action != nullptr) {
                SetYobiActionFlag2(action, true);
            }
        }
    }
    return result;
}

std::uintptr_t Sub_4EC240(const std::int32_t opened)
{
    const std::int32_t y = gLegacyMailContactPanel5837A8.y;
    const std::int32_t x = gLegacyMailContactPanel5837A8.x;
    std::array<LegacyPanelButtonRecord, 4> sends;
    std::array<LegacyPanelButtonRecord, 4> reads;
    std::array<LegacyPanelButtonRecord, 4> deletes;
    for (std::int32_t row = 0; row < 4; ++row) {
        sends[row] = {x + 238, y + 48 + 90 * row, x + 210, y + 38 + 90 * row, 56, 20, 23101, 23102};
        reads[row] = {x + 238, y + 75 + 90 * row, x + 210, y + 65 + 90 * row, 56, 20, 23103, 23104};
        deletes[row] = {x + 238, y + 102 + 90 * row, x + 210, y + 92 + 90 * row, 56, 20, 23105, 23106};
    }
    LegacyPanelButtonRecord edit {x + 60, y + 406, x + 20, y + 396, 80, 20, 23107, 23108};
    LegacyPanelButtonRecord previous {x + 146, y + 406, x + 132, y + 396, 28, 20, 22832, 22833};
    LegacyPanelButtonRecord next {x + 246, y + 406, x + 232, y + 396, 28, 20, 22834, 22835};
    LegacyPanelButtonRecord group {x + 45, y + 436, x, y + 422, 90, 28, 0, 0};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyMailContactPanel5837A8,
        [] { static_cast<void>(Sub_4EC1D0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* result = nullptr;
        for (auto& slot : gLegacyMailUnreadActionsCC3E48) {
            result = slot;
            if (result != nullptr) {
                result = MarkDeath(result);
                slot = nullptr;
            }
        }
        return reinterpret_cast<std::uintptr_t>(result);
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        if (gLegacyMailUnreadActionsCC3E48[row] != nullptr) {
            const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
            const std::int32_t contact = world::detail::gContactIndicesFDA608[row + 4 * page];
            const std::int32_t unread = world::detail::Sub_519090(contact);
            YobiAction* const action = gLegacyMailUnreadActionsCC3E48[row];
            SetYobiActionFlag2(action, unread == 0);
        }
    }
    std::int32_t hoverKind = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
        const std::int32_t position = row + 4 * page;
        const std::int32_t contact = world::detail::gContactIndicesFDA608[position];
        if (world::detail::gContactsF19FB0[contact].used == 0) {
            continue;
        }
        if ((QueryPanelButtonState(&sends[row]) & 0x40) != 0) {
            hoverKind = 0;
        } else if ((QueryPanelButtonState(&reads[row]) & 0x40) != 0) {
            hoverKind = 1;
        } else if ((QueryPanelButtonState(&deletes[row]) & 0x40) != 0) {
            hoverKind = 2;
        }
        if ((QueryPanelButtonState(&sends[row]) & 1) != 0) {
            const std::int32_t selected = world::detail::gContactIndicesFDA608[position];
            const std::int32_t currentPage = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
            gLegacyMailSelectedD30410 = selected;
            gLegacyMailContactCC4228 = selected;
            const std::int32_t scene = world::gCurrentSceneType;
            gLegacyMailPreviousPageD2DACC = currentPage;
            bool available = false;
            if (scene != 10) {
                for (std::int32_t pet = 0; pet < 5; ++pet) {
                    if (world::detail::Sub_52E1D0(pet) != 0) {
                        available = true;
                        break;
                    }
                }
            }
            static_cast<void>(audio::Sub_504180(available ? 45 : 46));
            break;
        }
        if ((QueryPanelButtonState(&reads[row]) & 1) != 0) {
            const std::int32_t selected = world::detail::gContactIndicesFDA608[position];
            const std::int32_t currentPage = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
            world::detail::gValueCF4FC0 = selected;
            gLegacyMailSelectedD30410 = selected;
            gLegacyMailPreviousPageD2DACC = currentPage;
            gLegacyMailContactCC4228 = selected;
            static_cast<void>(audio::Sub_504180(47));
            break;
        }
        if ((QueryPanelButtonState(&deletes[row]) & 1) != 0) {
            const std::int32_t selected = world::detail::gContactIndicesFDA608[position];
            ui::detail::BorrowLegacyMailDeleteIndexD2DA8C() = selected;
            static_cast<void>(audio::Sub_504180(44));
            break;
        }
    }
    const std::uint8_t editState = static_cast<std::uint8_t>(QueryPanelButtonState(&edit));
    const std::uint8_t previousState = static_cast<std::uint8_t>(QueryPanelButtonState(&previous));
    const std::uint8_t nextState = static_cast<std::uint8_t>(QueryPanelButtonState(&next));
    const std::uint8_t groupState = static_cast<std::uint8_t>(QueryPanelButtonState(&group));
    if ((previousState & 0x10u) != 0) {
        const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354) - 1;
        world::detail::gValueCB0354 = static_cast<std::uint32_t>(page);
        if (page < 0) {
            world::detail::gValueCB0354 = 14;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((nextState & 0x10u) != 0) {
        const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354) + 1;
        world::detail::gValueCB0354 = static_cast<std::uint32_t>(page);
        if (page >= 15) {
            world::detail::gValueCB0354 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((editState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(43));
    } else if ((groupState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyMailContactPanel5837A8.y + offsetY;
        const std::int32_t left = gLegacyMailContactPanel5837A8.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 1, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
        const std::int32_t position = row + 4 * page;
        const std::int32_t contact = world::detail::gContactIndicesFDA608[position];
        const auto* const record = &world::detail::gContactsF19FB0[contact];
        if (record->used == 0) {
            continue;
        }
        const std::int8_t color = record->value02 > 0 ? 0 : 7;
        const std::int32_t image = record->value0C;
        const std::int32_t imageY = gLegacyMailContactPanel5837A8.y + 68 + 90 * row;
        const std::int32_t imageX = gLegacyMailContactPanel5837A8.x + 36;
        static_cast<void>(DrawGraphic(imageX, imageY, 212, image, 0, &descriptor));
        const std::int32_t statusContact = world::detail::gContactIndicesFDA608[position];
        const std::int16_t status = world::detail::gContactsF19FB0[statusContact].value02;
        std::int32_t statusImage = 23110;
        if (status != 0) {
            std::int32_t server = login::gValue55A054;
            if (server >= 15) {
                server -= 15;
            }
            statusImage = kLegacyMailStatusImages58386C[server] + status - 1;
        }
        const std::int32_t statusY = gLegacyMailContactPanel5837A8.y + 52 + 90 * row;
        const std::int32_t statusX = gLegacyMailContactPanel5837A8.x + 105;
        static_cast<void>(DrawGraphic(statusX, statusY, 212, statusImage, 0, nullptr));
        const std::int32_t valueContact = world::detail::gContactIndicesFDA608[position];
        const std::int32_t value = world::detail::gContactsF19FB0[valueContact].value08;
        platform::FormatLegacyString_53B304(buffer, "%3d", value);
        drawAt(buffer, 180, 45 + 90 * row, color);
        const std::int32_t nameContact = world::detail::gContactIndicesFDA608[position];
        drawAt(world::detail::gContactsF19FB0[nameContact].name10.data(), 77, 68 + 90 * row, color);
        const std::int32_t memoContact = world::detail::gContactIndicesFDA608[position];
        const char* const memo = world::detail::gContactsF19FB0[memoContact].text21.data();
        if (memo[0] != '\0') {
            drawAt(memo, 77, 91 + 90 * row, color);
        }
        Sub_5035E0(&sends[row], 212, 1, gValueD69768, 0);
        Sub_5035E0(&reads[row], 212, 1, gValueD69768, 0);
        Sub_5035E0(&deletes[row], 212, 1, gValueD69768, 0);
    }
    const std::int32_t page = std::bit_cast<std::int32_t>(world::detail::gValueCB0354);
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", page + 1, 15);
    drawAt(buffer, 176, 399, 0);
    Sub_5035E0(&edit, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&group, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    // 原4ECCEB查询5857D0人物资料面板，不擅自改为本页。
    if (Sub_503590(&gLegacyProfilePanel5857D0) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((previousState & 0x40u) != 0 || (nextState & 0x40u) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((editState & 0x40u) != 0) {
        hint = kLegacyMailEdit5897AC;
    } else if ((groupState & 0x40u) != 0) {
        hint = kLegacyMailGroup589798;
    } else if (hoverKind == 0) {
        hint = kLegacyMailContactSend589780;
    } else if (hoverKind == 1) {
        hint = kLegacyMailContactRead589760;
    } else if (hoverKind == 2) {
        hint = kLegacyMailContactDelete589750;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailContactPanel5837A8, 1);
}

std::uint32_t Sub_4EB890(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailDeliveryPanel583550.x;
    const std::int32_t y = gLegacyMailDeliveryPanel583550.y;
    std::array<LegacyPanelButtonRecord, 3> buttons {{
        {x + 95, y + 55, x + 25, y + 43, 140, 24, 23185, 23186},
        {x + 95, y + 95, x + 25, y + 83, 140, 24, 23187, 23188},
        {x + 96, y + 137, x + 56, y + 127, 80, 20, 22710, 22711},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyMailDeliveryPanel583550, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(46));
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(48));
    } else if ((QueryPanelButtonState(&buttons[2]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    }
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMailDeliveryPanel583550) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = kLegacyMailNormalMode5896E0;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyMailPetMode5896D0;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0) {
        hint = kLegacyMailReturn589628;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailDeliveryPanel583550, 1);
}

std::uint32_t Sub_4EBAA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailDeletePanel583618.x;
    const std::int32_t y = gLegacyMailDeletePanel583618.y;
    std::array<character_select::detail::CharacterModalUiRecord, 2> buttons {{
        {x + 89, y + 182, x + 79, y + 180, 55, 20, kLegacyMailDeleteYes589728},
        {x + 187, y + 182, x + 185, y + 180, 55, 20, kLegacyBarterNo588A00},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyMailDeletePanel583618, nullptr, 0);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    const std::uint8_t confirmState = static_cast<std::uint8_t>(character_select::detail::Sub_503990(&buttons[0]));
    const std::uint8_t cancelState = static_cast<std::uint8_t>(character_select::detail::Sub_503990(&buttons[1]));
    if ((confirmState & 1u) != 0) {
        const std::int32_t index = ui::detail::BorrowLegacyMailDeleteIndexD2DA8C();
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_511190(socket, index));
        const std::int32_t currentIndex = ui::detail::BorrowLegacyMailDeleteIndexD2DA8C();
        static_cast<void>(world::detail::Sub_5192C0(currentIndex));
        static_cast<void>(world::detail::Sub_519300());
        static_cast<void>(audio::Sub_5042D0(gLegacyMailDeletePanel583618.id));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((cancelState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyMailDeletePanel583618.y + offsetY;
        const std::int32_t left = gLegacyMailDeletePanel583618.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    drawAt(kLegacyMailDeleteLead589714, 84, 38);
    const std::int32_t measureIndex = ui::detail::BorrowLegacyMailDeleteIndexD2DA8C();
    const std::int32_t width = TextMeasure(world::detail::gContactsF19FB0[measureIndex].name10.data(), 0);
    const std::int32_t textIndex = ui::detail::BorrowLegacyMailDeleteIndexD2DA8C();
    drawAt(world::detail::gContactsF19FB0[textIndex].name10.data(), 160 - width / 2, 86);
    drawAt(kLegacyMailDeleteEnd589708, 109, 134);
    Sub_503A20(&buttons[0], 2, 4, 1, gValueD69768, 218);
    Sub_503A20(&buttons[1], 2, 4, 1, gValueD69768, 218);
    const char* hint = nullptr;
    if ((confirmState & 0x40u) != 0) {
        hint = kLegacyMailDeleteHint589700;
    } else if ((cancelState & 0x40u) != 0) {
        hint = kLegacyMailKeepHint5896F4;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailDeletePanel583618, 0);
}

std::byte* Sub_4EAA50()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t contact = world::detail::gValueCF4FC0;
    world::detail::gValueD046D4 = 0;
    std::int32_t page = 9;
    std::byte* result = world::detail::BorrowMailMessageF7CA08(selected, contact, page).unread00;
    while (*result == std::byte{0}) {
        --page;
        result -= 0x140;
        if (page < 0) {
            return result;
        }
    }
    world::detail::gValueD046D4 = page;
    return result;
}

std::uint32_t Sub_4EAAA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailReadPanel5833C0.x;
    const std::int32_t y = gLegacyMailReadPanel5833C0.y;
    std::array<LegacyPanelButtonRecord, 4> buttons {{
        {x + 26, y + 410, x + 12, y + 400, 28, 20, 22832, 22833},
        {x + 126, y + 410, x + 112, y + 400, 28, 20, 22834, 22835},
        {x + 204, y + 410, x + 164, y + 400, 80, 20, 22714, 22715},
        {x + 65, y + 136, x + 37, y + 126, 56, 20, 23101, 23102},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyMailReadPanel5833C0,
        [] { static_cast<void>(Sub_4EAA50()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(world::detail::Sub_519300());
        return static_cast<std::uint32_t>(world::detail::Sub_518DA0());
    }
    if ((QueryPanelButtonState(&buttons[0]) & 0x10) != 0) {
        --world::detail::gValueD046D4;
        if (world::detail::gValueD046D4 < 0) {
            world::detail::gValueD046D4 = 9;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x10) != 0) {
        ++world::detail::gValueD046D4;
        if (world::detail::gValueD046D4 >= 10) {
            world::detail::gValueD046D4 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[2]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    } else if ((QueryPanelButtonState(&buttons[3]) & 1) != 0) {
        bool available = false;
        if (world::gCurrentSceneType != 10) {
            for (std::int32_t pet = 0; pet < 5; ++pet) {
                if (world::detail::Sub_52E1D0(pet) != 0) {
                    available = true;
                    break;
                }
            }
        }
        static_cast<void>(audio::Sub_504180(available ? 45 : 46));
    }
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t contact = world::detail::gValueCF4FC0;
    const std::int32_t page = world::detail::gValueD046D4;
    *world::detail::BorrowMailMessageF7CA08(selected, contact, page).unread00 = std::byte{0};
    const std::int32_t image = character_select::detail::Sub_51BC50(
        world::detail::gContactsF19FB0[contact].value0C);
    const std::int32_t imageY = gLegacyMailReadPanel5833C0.y + 73;
    const std::int32_t imageX = gLegacyMailReadPanel5833C0.x + 43;
    static_cast<void>(DrawGraphic(imageX, imageY, 212, image, 0, &descriptor));
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int32_t font, const std::int8_t color) {
        const std::int32_t top = gLegacyMailReadPanel5833C0.y + offsetY;
        const std::int32_t left = gLegacyMailReadPanel5833C0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, color, text, 0, 0));
    };
    char buffer[128];
    const std::int32_t valueContact = world::detail::gValueCF4FC0;
    platform::FormatLegacyString_53B304(buffer, "%3d", world::detail::gContactsF19FB0[valueContact].value08);
    drawAt(buffer, 134, 49, 1, 0);
    const std::int32_t measuredNameContact = world::detail::gValueCF4FC0;
    const std::int32_t nameWidth = TextMeasure(world::detail::gContactsF19FB0[measuredNameContact].name10.data(), 0);
    const std::int32_t nameContact = world::detail::gValueCF4FC0;
    drawAt(world::detail::gContactsF19FB0[nameContact].name10.data(), 162 - nameWidth / 2, 70, 0, 0);
    const std::int32_t measuredMemoContact = world::detail::gValueCF4FC0;
    const std::int32_t memoWidth = TextMeasure(world::detail::gContactsF19FB0[measuredMemoContact].text21.data(), 0);
    const std::int32_t memoContact = world::detail::gValueCF4FC0;
    drawAt(world::detail::gContactsF19FB0[memoContact].text21.data(), 162 - memoWidth / 2, 93, 0, 0);
    const std::int16_t titleSelected = login::gSelectedCharacterIndex;
    const std::int32_t titleContact = world::detail::gValueCF4FC0;
    const std::int32_t titlePage = world::detail::gValueD046D4;
    drawAt(world::detail::BorrowMailMessageF7CA08(titleSelected, titleContact, titlePage).text02, 128, 128, 1, 0);
    const std::int16_t colorSelected = login::gSelectedCharacterIndex;
    const std::int32_t colorContact = world::detail::gValueCF4FC0;
    const std::int32_t colorPage = world::detail::gValueD046D4;
    const std::int8_t color = *world::detail::BorrowMailMessageF7CA08(colorSelected, colorContact, colorPage).kind01
        != std::byte{0} ? 0 : 4;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int16_t rowSelected = login::gSelectedCharacterIndex;
        const std::int32_t rowContact = world::detail::gValueCF4FC0;
        const std::int32_t rowPage = world::detail::gValueD046D4;
        const char* const body = world::detail::BorrowMailMessageF7CA08(rowSelected, rowContact, rowPage).text14;
        // 原4EAF4D不检查分行结果，十次均绘制；颜色只在正文循环前取样。
        static_cast<void>(input::ExtractLegacyLine_503AB0(buffer, 128, body, row, 28));
        drawAt(buffer, 10, 169 + 20 * row, 0, color);
    }
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", world::detail::gValueD046D4 + 1, 10);
    drawAt(buffer, 56, 403, 1, 0);
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMailReadPanel5833C0) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0
        || (QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&buttons[3]) & 0x40) != 0) {
        hint = kLegacyMailSelect589678;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0) {
        hint = kLegacyMailReturn589628;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyMailReadPanel5833C0, 1);
}

std::int32_t Sub_4EB120()
{
    char preserved[400];
    std::strcpy(preserved, input::gInputContextD66CA8.text);
    const std::int32_t y = gLegacyMailNormalPanel583488.y;
    const std::int32_t x = gLegacyMailNormalPanel583488.x;
    input::InitializeContext_449CF0(&input::gInputContextD66CA8,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContextD66CA8);
    gLegacyMailSelectedFlagsCB0AA8.fill(0);
    const std::int32_t selected = gLegacyMailSelectedD30410;
    gLegacyMailSelectedFlagsCB0AA8[selected] = 1;
    return 0;
}

std::uintptr_t Sub_4EB1C0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailNormalPanel583488.x;
    const std::int32_t y = gLegacyMailNormalPanel583488.y;
    std::array<LegacyPanelButtonRecord, 4> buttons {{
        {x + 80, y + 197, x + 40, y + 187, 80, 20, 23196, 23197},
        {x + 192, y + 197, x + 152, y + 187, 80, 20, 22710, 22711},
        {x + 89, y + 159, x + 75, y + 149, 28, 20, 22832, 22833},
        {x + 189, y + 159, x + 175, y + 149, 28, 20, 22834, 22835},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyMailNormalPanel583488,
        [] { static_cast<void>(Sub_4EB120()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        input::SetContext_44B330(&input::gInputContext626AA0);
        return reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
    }
    // 原4EB320在行点击前统计，本帧绘制后的发送提示仍使用该计数。
    std::int32_t selectedCount = 0;
    for (const std::uint8_t selected : gLegacyMailSelectedFlagsCB0AA8) {
        if (selected != 0) {
            ++selectedCount;
        }
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t top = gLegacyMailNormalPanel583488.y + 27 * row;
        const std::int32_t left = gLegacyMailNormalPanel583488.x;
        if (Sub_5049A0(left + 64, top + 38, left + 213, top + 58, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        const std::int32_t page = gLegacyMailPreviousPageD2DACC;
        const std::int32_t contact = world::detail::gContactIndicesFDA608[hovered + 4 * page];
        // 原4EB4E7命中空联系人仍跳到公共尾，不继续查询四个按钮。
        if (world::detail::gContactsF19FB0[contact].used != 0) {
            gLegacyMailSelectedFlagsCB0AA8[contact] ^= 1u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        if (input::gInputContextD66CA8.text[0] != '\0' && selectedCount > 0) {
            std::int32_t timeValue;
            static_cast<void>(platform::ReadLegacyTime32_53BA26(&timeValue));
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&timeValue);
            const std::int32_t minutes = calendar->minutes;
            const std::int32_t hours = calendar->hours;
            const std::int32_t day = calendar->day;
            const std::int32_t month = calendar->month0based + 1;
            const std::int32_t year = calendar->yearMinus1900 + 1900;
            char timestamp[64];
            platform::FormatLegacyString_53B304(timestamp, "%4d/%2d/%2d %02d:%02d", year, month, day, hours, minutes);
            char source[512];
            std::strcpy(source, input::gInputContextD66CA8.text);
            char escaped[600];
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(source, escaped, 600));
            for (std::int32_t position = 0; position < 60; ++position) {
                const std::int32_t contact = world::detail::gContactIndicesFDA608[position];
                if (gLegacyMailSelectedFlagsCB0AA8[contact] != 0) {
                    const auto socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_510FA0(socket, contact, escaped, 0));
                    const std::int32_t savedContact = world::detail::gContactIndicesFDA608[position];
                    world::detail::Sub_5191C0(savedContact, 1, timestamp, input::gInputContextD66CA8.text);
                }
            }
            static_cast<void>(world::detail::Sub_519300());
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            static_cast<void>(audio::Sub_504180(42));
            input::gInputContextD66CA8.text[0] = '\0';
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x10) != 0) {
        --gLegacyMailPreviousPageD2DACC;
        if (gLegacyMailPreviousPageD2DACC < 0) {
            gLegacyMailPreviousPageD2DACC = 14;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[3]) & 0x10) != 0) {
        ++gLegacyMailPreviousPageD2DACC;
        if (gLegacyMailPreviousPageD2DACC >= 15) {
            gLegacyMailPreviousPageD2DACC = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyMailNormalPanel583488.y + 27 * hovered;
        const std::int32_t image = gValueD69768;
        const std::int32_t left = gLegacyMailNormalPanel583488.x;
        static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(left + 64),
            static_cast<std::int16_t>(top + 38), static_cast<std::int16_t>(left + 209),
            static_cast<std::int16_t>(top + 58), 212, image, 0));
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = gLegacyMailPreviousPageD2DACC;
        const std::int32_t position = row + 4 * page;
        const std::int32_t contact = world::detail::gContactIndicesFDA608[position];
        const auto* const record = &world::detail::gContactsF19FB0[contact];
        if (record->used == 0) {
            continue;
        }
        const std::int8_t color = gLegacyMailSelectedFlagsCB0AA8[contact] != 0
            ? 1 : (record->value02 != 0 ? 0 : 7);
        const std::int32_t width = TextMeasure(record->name10.data(), 0);
        const std::int32_t nameContact = world::detail::gContactIndicesFDA608[position];
        const char* const name = world::detail::gContactsF19FB0[nameContact].name10.data();
        const std::int32_t top = gLegacyMailNormalPanel583488.y + 40 + 27 * row;
        const std::int32_t left = gLegacyMailNormalPanel583488.x - width / 2 + 136;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, name, 0, 0));
    }
    char pageText[128];
    platform::FormatLegacyString_53B304(pageText, "%2d/%2d", gLegacyMailPreviousPageD2DACC + 1, 15);
    const std::int32_t pageY = gLegacyMailNormalPanel583488.y + 152;
    const std::int32_t pageX = gLegacyMailNormalPanel583488.x + 119;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, pageText, 0, 0));
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    render::DrawInputText_4568A0(&input::gInputContextD66CA8);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyMailNormalPanel583488) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0
        || (QueryPanelButtonState(&buttons[3]) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = selectedCount != 0 ? kLegacyMailSendSelected5896B8 : kLegacyMailChooseRecipients5896A4;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyMailReturn589628;
    } else if (hovered >= 0) {
        const std::int32_t page = gLegacyMailPreviousPageD2DACC;
        const std::int32_t contact = world::detail::gContactIndicesFDA608[hovered + 4 * page];
        hint = world::detail::gContactsF19FB0[contact].used != 0
            && gLegacyMailSelectedFlagsCB0AA8[contact] != 0
            ? kLegacyMailUnselect58968C : kLegacyMailSelect589678;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyMailNormalPanel583488, 1));
}

std::uint32_t Sub_4EBD20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyMailSortPanel5836E0.x;
    const std::int32_t y = gLegacyMailSortPanel5836E0.y;
    LegacyPanelButtonRecord confirm {x + 96, y + 220, x + 56, y + 210, 80, 20, 22712, 22713};
    const std::int32_t state = Sub_503160(opened, &gLegacyMailSortPanel5836E0, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t top = gLegacyMailSortPanel5836E0.y + 30 * row;
        const std::int32_t left = gLegacyMailSortPanel5836E0.x;
        if (Sub_5049A0(left + 19, top + 38, left + 173, top + 64, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        platform::gValueD67CD0 = static_cast<std::uint32_t>(hovered);
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        static_cast<void>(Sub_5206F0());
        static_cast<void>(Sub_520200());
        static_cast<void>(audio::Sub_504180(42));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    }
    if (hovered >= 0) {
        const std::int32_t image = gValueD69768;
        const std::int32_t left = gLegacyMailSortPanel5836E0.x;
        const std::int32_t top = gLegacyMailSortPanel5836E0.y + 30 * hovered;
        static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(left + 19),
            static_cast<std::int16_t>(top + 38), static_cast<std::int16_t>(left + 173),
            static_cast<std::int16_t>(top + 64), 212, image, 0));
    }
    // D67CD0借既有W32 owner；原框坐标按完整S32解释，不新增范围钳制。
    const std::int32_t selected = std::bit_cast<std::int32_t>(platform::gValueD67CD0);
    const std::int32_t selectedTop = gLegacyMailSortPanel5836E0.y + 30 * selected;
    const std::int32_t selectedLeft = gLegacyMailSortPanel5836E0.x;
    static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(selectedLeft + 19),
        static_cast<std::int16_t>(selectedTop + 38), static_cast<std::int16_t>(selectedLeft + 173),
        static_cast<std::int16_t>(selectedTop + 64), 212, 254, 0));
    const auto drawAt = [](const char* const text, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyMailSortPanel5836E0.y + offsetY;
        const std::int32_t left = gLegacyMailSortPanel5836E0.x + 62;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    drawAt(kLegacyMailSortFirst589744, 43);
    drawAt(kLegacyMailSortSecond589738, 73);
    drawAt(kLegacyMailSortThird58972C, 103);
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacyMailSortPanel5836E0, 1);
}

void Sub_52D790()
{
    gLegacyGuildGiftPage1050DAC = 0;
    gLegacyGuildGiftSelected1050DA8 = -1;
}

std::uint32_t Sub_52D7B0(const std::int32_t opened)
{
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildGiftPanel58E950, Sub_52D790, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        // 原52D809关闭49，与本页id86不同，不擅自改成87或86。
        return static_cast<std::uint32_t>(audio::Sub_5042D0(49));
    }
    std::int32_t hovered = -1;
    std::int32_t giftHovered = -1;
    std::int32_t hoverRow;
    std::int32_t hoverColumn;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t top = gLegacyGuildGiftPanel58E950.y + 36 + 53 * row;
            const std::int32_t left = gLegacyGuildGiftPanel58E950.x + 8 + 52 * column;
            if (Sub_5049A0(left, top, left + 47, top + 47, -1) != 0) {
                hoverRow = row;
                hoverColumn = column;
                hovered = 8 + row * 5 + column;
                const auto* const item = &world::detail::gPlayerItemsFDA8A4[hovered];
                if (item->used != 0 && (static_cast<std::uint8_t>(item->flags) & 1u) != 0) {
                    giftHovered = hovered;
                }
                break;
            }
        }
    }
    if (hovered != gLegacyGuildGiftHover58EA14) {
        gLegacyGuildGiftPage1050DAC = 0;
    }
    const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
    if (giftHovered >= 0 && (clicks & 0x10u) != 0) {
        const std::int32_t selected = gLegacyGuildGiftSelected1050DA8;
        if (selected >= 0 && selected == giftHovered) {
            gLegacyGuildGiftSelected1050DA8 = -1;
            static_cast<void>(audio::PlaySe_52C450(54, 320));
        } else {
            gLegacyGuildGiftSelected1050DA8 = giftHovered;
            gLegacyGuildGiftColumn1050F10 = hoverColumn;
            gLegacyGuildGiftRow1050DC8 = hoverRow;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    } else if (hovered >= 0 && (clicks & 2u) != 0) {
        gLegacyGuildGiftPage1050DAC ^= 1;
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            const auto* const item = &world::detail::gPlayerItemsFDA8A4[8 + row * 5 + column];
            if (item->used != 0) {
                const std::int32_t image = item->imageNo;
                const std::int32_t top = gLegacyGuildGiftPanel58E950.y + 60 + 53 * row;
                const std::int32_t left = gLegacyGuildGiftPanel58E950.x + 32 + 52 * column;
                static_cast<void>(DrawGraphic(left, top, 213, image, 0, &descriptor));
                const std::int32_t count = item->count;
                if (count > 0) {
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
                    const std::int32_t countY = gLegacyGuildGiftPanel58E950.y + 67 + 53 * row;
                    const std::int32_t countX = gLegacyGuildGiftPanel58E950.x + 29 + 52 * column;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
        }
    }
    if (hovered >= 0) {
        const std::int32_t x = gLegacyGuildGiftPanel58E950.x;
        const std::int32_t top = gLegacyGuildGiftPanel58E950.y + 53 * hoverRow;
        const std::int32_t left = x + 52 * hoverColumn;
        const std::uint32_t image = gValueD69768;
        DrawGraphicHover(Low16(left + 8), Low16(top + 36), Low16(left + 55), Low16(top + 83),
            212, image, 0);
        const std::int32_t page = gLegacyGuildGiftPage1050DAC;
        const std::int32_t y = gLegacyGuildGiftPanel58E950.y;
        const std::int32_t height = gLegacyGuildGiftPanel58E950.height;
        const std::int32_t detailX = gLegacyGuildGiftPanel58E950.x;
        Sub_4FB940(detailX, y + height, hovered, page);
    }
    if (gLegacyGuildGiftSelected1050DA8 >= 0) {
        const std::int32_t row = gLegacyGuildGiftRow1050DC8;
        const std::int32_t top = gLegacyGuildGiftPanel58E950.y + 53 * row;
        const std::int32_t column = gLegacyGuildGiftColumn1050F10;
        const std::int32_t left = gLegacyGuildGiftPanel58E950.x + 52 * column;
        DrawGraphicHover(Low16(left + 10), Low16(top + 38), Low16(left + 53), Low16(top + 81),
            212, 251, 0);
    }
    // 原52DB08直接访问Items[hovered]，按RPLAN.24不补记录或新增非负门禁。
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
        }
    } else if (Sub_503590(&gLegacyGuildGiftPanel58E950) != 0) {
        std::strcpy(hint, kLegacyGuildClose58F248);
    } else {
        const std::int32_t selected = gLegacyGuildGiftSelected1050DA8;
        if (selected >= 0 && selected == hovered) {
            std::strcpy(hint, kLegacyGuildGift58F224);
        }
    }
    const std::uint32_t result = Sub_503440(&gLegacyGuildGiftPanel58E950, 1);
    gLegacyGuildGiftHover58EA14 = hovered;
    return result;
}

void Sub_52DC60()
{
    char preserved[400];
    std::strcpy(preserved, input::gInputContext1050B80.text);
    const std::int32_t y = gLegacyGuildComposePanel58EA18.y;
    const std::int32_t x = gLegacyGuildComposePanel58EA18.x;
    input::InitializeContext_449CF0(&input::gInputContext1050B80,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContext1050B80);
}

std::uint32_t Sub_52DCE0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildComposePanel58EA18.x;
    const std::int32_t y = gLegacyGuildComposePanel58EA18.y;
    std::array<LegacyPanelButtonRecord, 2> buttons {{
        {x + 80, y + 197, x + 40, y + 187, 80, 20, 23196, 23197},
        {x + 192, y + 197, x + 152, y + 187, 80, 20, 22710, 22711},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildComposePanel58EA18, Sub_52DC60, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        input::SetContext_44B330(&input::gInputContext626AA0);
        // 原52DDC7关闭86，先恢复聊天焦点，沿原顺序保留。
        return static_cast<std::uint32_t>(audio::Sub_5042D0(86));
    }
    if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        if (input::gInputContext1050B80.text[0] != '\0') {
            std::int32_t timeValue;
            static_cast<void>(platform::ReadLegacyTime32_53BA26(&timeValue));
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&timeValue);
            const std::int32_t minutes = calendar->minutes;
            const std::int32_t hours = calendar->hours;
            const std::int32_t day = calendar->day;
            const std::int32_t month = calendar->month0based + 1;
            const std::int32_t year = calendar->yearMinus1900 + 1900;
            char timestamp[64];
            platform::FormatLegacyString_53B304(timestamp, "%4d/%2d/%2d %02d:%02d", year, month, day, hours, minutes);
            char source[512];
            std::strcpy(source, input::gInputContext1050B80.text);
            char escaped[600];
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(source, escaped, 600));
            const std::int32_t gift = gLegacyGuildGiftSelected1050DA8;
            const std::int32_t value = gLegacyGuildPetIndex1050DB0;
            const std::int32_t contact = gLegacyGuildMailTarget1050DCC;
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_5110B0(socket, contact, value, gift, escaped, 0));
            const std::int32_t savedContact = gLegacyGuildMailTarget1050DCC;
            static_cast<void>(world::detail::Sub_519370(savedContact, 1, timestamp, input::gInputContext1050B80.text));
            static_cast<void>(world::detail::Sub_519300());
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            static_cast<void>(audio::Sub_504180(94));
            input::gInputContext1050B80.text[0] = '\0';
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    }
    const std::int32_t imageContact = gLegacyGuildMailTarget1050DCC;
    const std::int32_t contactY = gLegacyGuildComposePanel58EA18.y + 84;
    const std::int32_t contactImage = gOrderedRecords1051B14[imageContact].value14;
    const std::int32_t contactX = gLegacyGuildComposePanel58EA18.x + 52;
    static_cast<void>(DrawGraphic(contactX, contactY, 212, contactImage, 0, &descriptor));
    const std::int32_t measureContact = gLegacyGuildMailTarget1050DCC;
    const std::int32_t contactWidth = TextMeasure(gOrderedRecords1051B14[measureContact].text18, 0);
    const std::int32_t textContact = gLegacyGuildMailTarget1050DCC;
    const char* const contactName = gOrderedRecords1051B14[textContact].text18;
    const std::int32_t contactNameY = gLegacyGuildComposePanel58EA18.y + 62;
    const std::int32_t contactNameX = gLegacyGuildComposePanel58EA18.x - contactWidth / 2 + 176;
    static_cast<void>(DrawText(static_cast<std::int16_t>(contactNameX),
        static_cast<std::int16_t>(contactNameY), 2, 0, 0, contactName, 0, 0));
    const std::int32_t gift = gLegacyGuildGiftSelected1050DA8;
    if (gift > 0) {
        const std::int32_t giftY = gLegacyGuildComposePanel58EA18.y + 120;
        const std::int32_t image = world::detail::gPlayerItemsFDA8A4[gift].imageNo;
        const std::int32_t giftX = gLegacyGuildComposePanel58EA18.x + 216;
        static_cast<void>(DrawGraphic(giftX, giftY, 212, image, 0, &descriptor));
        std::int32_t nameGift = gLegacyGuildGiftSelected1050DA8;
        const std::int32_t count = world::detail::gPlayerItemsFDA8A4[nameGift].count;
        if (count > 0) {
            char buffer[128];
            platform::FormatLegacyString_53B304(buffer, "%3d", count);
            const std::int32_t countY = gLegacyGuildComposePanel58EA18.y + 127;
            const std::int32_t countX = gLegacyGuildComposePanel58EA18.x + 213;
            static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
            nameGift = gLegacyGuildGiftSelected1050DA8;
        }
        const std::int32_t width = TextMeasure(world::detail::gPlayerItemsFDA8A4[nameGift].name.data(), 0);
        const std::int32_t textGift = gLegacyGuildGiftSelected1050DA8;
        const char* const name = world::detail::gPlayerItemsFDA8A4[textGift].name.data();
        const std::int32_t nameY = gLegacyGuildComposePanel58EA18.y + 152;
        const std::int32_t nameX = gLegacyGuildComposePanel58EA18.x - width / 2 + 136;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, 0, name, 0, 0));
    }
    Sub_5035E0(&buttons[0], 212, 1, gValueD69768, 0);
    Sub_5035E0(&buttons[1], 212, 1, gValueD69768, 0);
    render::DrawInputText_4568A0(&input::gInputContext1050B80);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGuildComposePanel58EA18) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = kLegacyGuildSendMail58F288;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyGuildReturn58F264;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGuildComposePanel58EA18, 1);
}

std::uint32_t Sub_52E240(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildPetPanel58EAE0.x;
    const std::int32_t y = gLegacyGuildPetPanel58EAE0.y;
    LegacyPanelButtonRecord back {x + 136, y + 206, x + 96, y + 196, 80, 20, 22710, 22711};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildPetPanel58EAE0, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::array<std::int32_t, 5> availability;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        const std::int32_t available = world::detail::Sub_52E1D0(index);
        availability[row] = available;
        const std::int32_t top = gLegacyGuildPetPanel58EAE0.y + 41 + 28 * row;
        const std::int32_t left = gLegacyGuildPetPanel58EAE0.x + 12;
        if (Sub_5049A0(left, top, left + 249, top + 20, -1) != 0
            && world::detail::gPetRuntimeF633F8[index].used != 0 && available != 0) {
            hovered = row;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[hovered].index);
        gLegacyGuildPetIndex1050DB0 = index;
        static_cast<void>(audio::Sub_504180(87));
        static_cast<void>(audio::Sub_504180(86));
    } else if ((QueryPanelButtonState(&back) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyGuildPetPanel58EAE0.y + 28 * hovered;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacyGuildPetPanel58EAE0.x;
        DrawGraphicHover(Low16(left + 12), Low16(top + 41), Low16(left + 261), Low16(top + 61),
            212, image, 0);
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        const auto* const pet = &world::detail::gPetRuntimeF633F8[index];
        if (pet->used == 0) {
            continue;
        }
        const char* const name = pet->name6BD[0] != '\0' ? pet->name6BD.data() : pet->name6AC.data();
        const std::int8_t color = availability[row] != 0 ? 0 : 7;
        std::strcpy(buffer, name);
        const std::int32_t width = TextMeasure(buffer, 0);
        const std::int32_t nameY = gLegacyGuildPetPanel58EAE0.y + 43 + 28 * row;
        const std::int32_t nameX = gLegacyGuildPetPanel58EAE0.x - width / 2 + 90;
        static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY), 2, 0, color, buffer, 0, 0));
        // 原52E4C2操作数F63400为宠物+08，不按反编译lv名称改到+6C。
        const std::int32_t value = pet->value08;
        platform::FormatLegacyString_53B304(buffer, "%3d", value);
        const std::int32_t valueY = gLegacyGuildPetPanel58EAE0.y + 45 + 28 * row;
        const std::int32_t valueX = gLegacyGuildPetPanel58EAE0.x + 226;
        static_cast<void>(DrawText(static_cast<std::int16_t>(valueX),
            static_cast<std::int16_t>(valueY), 2, 1, color, buffer, 0, 0));
    }
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGuildPetPanel58EAE0) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyGuildReturn58F264;
    } else if (hovered >= 0) {
        hint = kLegacyGuildPetHint58F2B4;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGuildPetPanel58EAE0, 1);
}

std::byte* Sub_52E600()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t contact = gLegacyGuildReadTarget1050DB4;
    gLegacyGuildReadPage1050EF8 = 0;
    std::int32_t page = 9;
    std::byte* result = world::detail::BorrowGuildMailMessageDE0B30(selected, contact, page).unread00;
    while (*result == std::byte{0}) {
        --page;
        result -= 0x140;
        if (page < 0) {
            return result;
        }
    }
    gLegacyGuildReadPage1050EF8 = page;
    return result;
}

std::uint32_t Sub_52E650(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildReadPanel58EBA8.x;
    const std::int32_t y = gLegacyGuildReadPanel58EBA8.y;
    std::array<LegacyPanelButtonRecord, 4> buttons {{
        {x + 26, y + 410, x + 12, y + 400, 28, 20, 22832, 22833},
        {x + 126, y + 410, x + 112, y + 400, 28, 20, 22834, 22835},
        {x + 204, y + 410, x + 164, y + 400, 80, 20, 22714, 22715},
        {x + 65, y + 136, x + 37, y + 126, 56, 20, 23101, 23102},
    }};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildReadPanel58EBA8,
        [] { static_cast<void>(Sub_52E600()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        static_cast<void>(world::detail::Sub_519550());
        return static_cast<std::uint32_t>(world::detail::Sub_518DA0());
    }
    if ((QueryPanelButtonState(&buttons[0]) & 0x10) != 0
        || platform::ReadControlKeyPressFlag_4597C0(90) != 0) {
        --gLegacyGuildReadPage1050EF8;
        if (gLegacyGuildReadPage1050EF8 < 0) {
            gLegacyGuildReadPage1050EF8 = 9;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x10) != 0
        || platform::ReadControlKeyPressFlag_4597C0(88) != 0) {
        ++gLegacyGuildReadPage1050EF8;
        if (gLegacyGuildReadPage1050EF8 >= 10) {
            gLegacyGuildReadPage1050EF8 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[2]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    } else if ((QueryPanelButtonState(&buttons[3]) & 1) != 0) {
        bool available = false;
        if (world::gCurrentSceneType != 10) {
            for (std::int32_t pet = 0; pet < 5; ++pet) {
                if (world::detail::Sub_52E1D0(pet) != 0) {
                    available = true;
                    break;
                }
            }
        }
        static_cast<void>(audio::Sub_504180(available ? 91 : 90));
    }
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t contact = gLegacyGuildReadTarget1050DB4;
    const std::int32_t page = gLegacyGuildReadPage1050EF8;
    *world::detail::BorrowGuildMailMessageDE0B30(selected, contact, page).unread00 = std::byte{0};
    // 原52E907缓存成员T*，后续数字/名称/职级沿同一记录取值。
    const auto* const member = &gOrderedRecords1051B14[contact];
    const std::int32_t image = character_select::detail::Sub_51BC50(member->value14);
    const std::int32_t imageY = gLegacyGuildReadPanel58EBA8.y + 73;
    const std::int32_t imageX = gLegacyGuildReadPanel58EBA8.x + 43;
    static_cast<void>(DrawGraphic(imageX, imageY, 212, image, 0, &descriptor));
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int32_t font, const std::int8_t color) {
        const std::int32_t top = gLegacyGuildReadPanel58EBA8.y + offsetY;
        const std::int32_t left = gLegacyGuildReadPanel58EBA8.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, color, text, 0, 0));
    };
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%3d", member->value10);
    drawAt(buffer, 134, 49, 1, 0);
    const char* const memberName = member->text18;
    const std::int32_t nameWidth = TextMeasure(memberName, 0);
    drawAt(memberName, 162 - nameWidth / 2, 70, 0, 0);
    const std::int32_t rankWidth = TextMeasure(BorrowGuildRankText1051518(member->value00), 0);
    const char* const rankName = BorrowGuildRankText1051518(member->value00);
    drawAt(rankName, 162 - rankWidth / 2, 93, 0, 0);
    const std::int16_t titleSelected = login::gSelectedCharacterIndex;
    const std::int32_t titleContact = gLegacyGuildReadTarget1050DB4;
    const std::int32_t titlePage = gLegacyGuildReadPage1050EF8;
    drawAt(world::detail::BorrowGuildMailMessageDE0B30(titleSelected, titleContact, titlePage).text02, 128, 128, 1, 0);
    const std::int16_t colorSelected = login::gSelectedCharacterIndex;
    const std::int32_t colorContact = gLegacyGuildReadTarget1050DB4;
    const std::int32_t colorPage = gLegacyGuildReadPage1050EF8;
    const std::int8_t color = *world::detail::BorrowGuildMailMessageDE0B30(colorSelected, colorContact, colorPage).kind01
        != std::byte{0} ? 0 : 4;
    std::int32_t rowPage = colorPage;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (row != 0) {
            rowPage = gLegacyGuildReadPage1050EF8;
        }
        const std::int16_t rowSelected = login::gSelectedCharacterIndex;
        const std::int32_t rowContact = gLegacyGuildReadTarget1050DB4;
        const char* const body = world::detail::BorrowGuildMailMessageDE0B30(rowSelected, rowContact, rowPage).text14;
        // 原52EAE1不检查分行结果，十次均绘制；颜色只在正文循环前取样。
        static_cast<void>(input::ExtractLegacyLine_503AB0(buffer, 128, body, row, 28));
        drawAt(buffer, 10, 169 + 20 * row, 0, color);
    }
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", gLegacyGuildReadPage1050EF8 + 1, 10);
    drawAt(buffer, 56, 403, 1, 0);
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGuildReadPanel58EBA8) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0
        || (QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyGuildPage58F2D4;
    } else if ((QueryPanelButtonState(&buttons[3]) & 0x40) != 0) {
        hint = kLegacyGuildSend58F2EC;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0) {
        hint = kLegacyGuildReturn58F264;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGuildReadPanel58EBA8, 1);
}

std::int32_t Sub_52ECB0()
{
    char preserved[400];
    std::strcpy(preserved, input::gInputContext1051348.text);
    const std::int32_t y = gLegacyGuildNormalPanel58EC70.y;
    const std::int32_t x = gLegacyGuildNormalPanel58EC70.x;
    input::InitializeContext_449CF0(&input::gInputContext1051348,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContext1051348);
    gLegacyGuildSelectedFlags1050CDC.fill(0);
    const std::int32_t selected = gLegacyGuildComposeTarget1051340;
    gLegacyGuildSelectedFlags1050CDC[selected] = 1;
    return 0;
}

std::uintptr_t Sub_52ED50(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildNormalPanel58EC70.x;
    const std::int32_t y = gLegacyGuildNormalPanel58EC70.y;
    std::array<LegacyPanelButtonRecord, 4> buttons {{
        {x + 80, y + 197, x + 40, y + 187, 80, 20, 23196, 23197},
        {x + 192, y + 197, x + 152, y + 187, 80, 20, 22710, 22711},
        {x + 89, y + 159, x + 75, y + 149, 28, 20, 22832, 22833},
        {x + 189, y + 159, x + 175, y + 149, 28, 20, 22834, 22835},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildNormalPanel58EC70,
        [] { static_cast<void>(Sub_52ECB0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        input::SetContext_44B330(&input::gInputContext626AA0);
        return reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
    }
    // 原52EEB8在行点击前统计，本帧绘制后的发送提示仍使用该计数。
    std::int32_t selectedCount = 0;
    for (const std::uint8_t selected : gLegacyGuildSelectedFlags1050CDC) {
        if (selected != 0) {
            ++selectedCount;
        }
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t top = gLegacyGuildNormalPanel58EC70.y + 27 * row;
        const std::int32_t left = gLegacyGuildNormalPanel58EC70.x;
        if (Sub_5049A0(left + 64, top + 38, left + 213, top + 58, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        const std::int32_t page = gLegacyGuildComposePage1051330;
        const std::int32_t contact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[hovered + 4 * page]);
        // 原52F06B命中空联系人仍跳到公共尾，不继续查询四个按钮。
        if (gOrderedRecords1051B14[contact].value08 != 0) {
            gLegacyGuildSelectedFlags1050CDC[contact] ^= 1u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        if (input::gInputContext1051348.text[0] != '\0' && selectedCount > 0) {
            std::int32_t timeValue;
            static_cast<void>(platform::ReadLegacyTime32_53BA26(&timeValue));
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&timeValue);
            const std::int32_t minutes = calendar->minutes;
            const std::int32_t hours = calendar->hours;
            const std::int32_t day = calendar->day;
            const std::int32_t month = calendar->month0based + 1;
            const std::int32_t year = calendar->yearMinus1900 + 1900;
            char timestamp[64];
            platform::FormatLegacyString_53B304(timestamp, "%4d/%2d/%2d %02d:%02d", year, month, day, hours, minutes);
            char source[512];
            std::strcpy(source, input::gInputContext1051348.text);
            char escaped[600];
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(source, escaped, 600));
            for (std::int32_t position = 0; position < 200; ++position) {
                const std::int32_t contact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
                if (gLegacyGuildSelectedFlags1050CDC[contact] != 0) {
                    const auto socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_511480(socket, contact, escaped, 0));
                    const std::int32_t savedContact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
                    static_cast<void>(world::detail::Sub_519370(savedContact, 1, timestamp, input::gInputContext1051348.text));
                }
            }
            static_cast<void>(world::detail::Sub_519550());
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            static_cast<void>(audio::Sub_504180(94));
            input::gInputContext1051348.text[0] = '\0';
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x10) != 0
        || platform::ReadControlKeyPressFlag_4597C0(90) != 0) {
        --gLegacyGuildComposePage1051330;
        if (gLegacyGuildComposePage1051330 < 0) {
            gLegacyGuildComposePage1051330 = 49;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&buttons[3]) & 0x10) != 0
        || platform::ReadControlKeyPressFlag_4597C0(88) != 0) {
        ++gLegacyGuildComposePage1051330;
        if (gLegacyGuildComposePage1051330 >= 50) {
            gLegacyGuildComposePage1051330 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyGuildNormalPanel58EC70.y + 27 * hovered;
        const std::int32_t image = gValueD69768;
        const std::int32_t left = gLegacyGuildNormalPanel58EC70.x;
        static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(left + 64),
            static_cast<std::int16_t>(top + 38), static_cast<std::int16_t>(left + 209),
            static_cast<std::int16_t>(top + 58), 212, image, 0));
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = gLegacyGuildComposePage1051330;
        const std::int32_t position = row + 4 * page;
        const std::int32_t contact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        const auto* const record = &gOrderedRecords1051B14[contact];
        if (record->value08 == 0) {
            continue;
        }
        const std::int8_t color = gLegacyGuildSelectedFlags1050CDC[contact] != 0
            ? 1 : (record->value0A != 0 ? 0 : 7);
        const std::int32_t width = TextMeasure(record->text18, 0);
        const std::int32_t nameContact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        const char* const name = gOrderedRecords1051B14[nameContact].text18;
        const std::int32_t top = gLegacyGuildNormalPanel58EC70.y + 40 + 27 * row;
        const std::int32_t left = gLegacyGuildNormalPanel58EC70.x - width / 2 + 136;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, color, name, 0, 0));
    }
    char pageText[128];
    platform::FormatLegacyString_53B304(pageText, "%2d/%2d", gLegacyGuildComposePage1051330 + 1, 50);
    const std::int32_t pageY = gLegacyGuildNormalPanel58EC70.y + 152;
    const std::int32_t pageX = gLegacyGuildNormalPanel58EC70.x + 119;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, pageText, 0, 0));
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    render::DrawInputText_4568A0(&input::gInputContext1051348);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGuildNormalPanel58EC70) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0
        || (QueryPanelButtonState(&buttons[3]) & 0x40) != 0) {
        hint = kLegacyGuildPage58F2D4;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = selectedCount != 0 ? kLegacyGuildSendMail58F288 : kLegacyGuildChooseRecipients58F33C;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyGuildReturn58F264;
    } else if (hovered >= 0) {
        const std::int32_t page = gLegacyGuildComposePage1051330;
        const std::int32_t contact = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[hovered + 4 * page]);
        hint = gOrderedRecords1051B14[contact].value08 != 0
            && gLegacyGuildSelectedFlags1050CDC[contact] != 0
            ? kLegacyGuildUnselect58F310 : kLegacyGuildSend58F2EC;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyGuildNormalPanel58EC70, 1));
}

std::uint32_t Sub_52F460(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildDeliveryPanel58ED38.x;
    const std::int32_t y = gLegacyGuildDeliveryPanel58ED38.y;
    std::array<LegacyPanelButtonRecord, 3> buttons {{
        {x + 95, y + 55, x + 25, y + 43, 140, 24, 23185, 23186},
        {x + 95, y + 95, x + 25, y + 83, 140, 24, 23187, 23188},
        {x + 96, y + 137, x + 56, y + 127, 80, 20, 22710, 22711},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildDeliveryPanel58ED38, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    if ((QueryPanelButtonState(&buttons[0]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(90));
    } else if ((QueryPanelButtonState(&buttons[1]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(88));
    } else if ((QueryPanelButtonState(&buttons[2]) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    }
    for (auto& button : buttons) {
        Sub_5035E0(&button, 212, 1, gValueD69768, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyGuildDeliveryPanel58ED38) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((QueryPanelButtonState(&buttons[0]) & 0x40) != 0) {
        hint = kLegacyGuildNormalMode58F374;
    } else if ((QueryPanelButtonState(&buttons[1]) & 0x40) != 0) {
        hint = kLegacyGuildPetMode58F358;
    } else if ((QueryPanelButtonState(&buttons[2]) & 0x40) != 0) {
        hint = kLegacyGuildReturn58F264;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGuildDeliveryPanel58ED38, 1);
}

std::uint32_t Sub_52F670(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildDeletePanel58EE00.x;
    const std::int32_t y = gLegacyGuildDeletePanel58EE00.y;
    std::array<character_select::detail::CharacterModalUiRecord, 2> buttons {{
        {x + 89, y + 182, x + 79, y + 180, 55, 20, kLegacyGuildDeleteYes58F3EC},
        {x + 187, y + 182, x + 185, y + 180, 55, 20, kLegacyGuildDeleteNo58F3E4},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildDeletePanel58EE00, nullptr, 0);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    const std::uint8_t confirmState = static_cast<std::uint8_t>(character_select::detail::Sub_503990(&buttons[0]));
    const std::uint8_t cancelState = static_cast<std::uint8_t>(character_select::detail::Sub_503990(&buttons[1]));
    if ((confirmState & 1u) != 0) {
        const std::int32_t rank = ReadGuildTextIndex10514B0();
        const std::uint8_t permissions = ReadGuildRankFlagsByte1051514(rank);
        if ((permissions & 2u) != 0) {
            const std::int32_t target = gLegacyGuildDeleteTarget105132C;
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_511430(socket, target));
            const std::int32_t currentTarget = gLegacyGuildDeleteTarget105132C;
            // 原52F75E直接传选择索引；其后也只有普通邮件组保存519300。
            static_cast<void>(world::detail::Sub_5194F0(currentTarget));
            static_cast<void>(world::detail::Sub_519300());
            static_cast<void>(audio::Sub_5042D0(gLegacyGuildDeletePanel58EE00.id));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        } else {
            world::detail::Sub_44B050(kLegacyGuildDeleteDenied58F3C8, 4, 0);
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((cancelState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(94));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyGuildDeletePanel58EE00.y + offsetY;
        const std::int32_t left = gLegacyGuildDeletePanel58EE00.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    drawAt(kLegacyGuildDeleteLead58F3B8, 84, 38);
    const std::int32_t measureTarget = gLegacyGuildDeleteTarget105132C;
    const std::int32_t width = TextMeasure(gOrderedRecords1051B14[measureTarget].text18, 0);
    const std::int32_t textTarget = gLegacyGuildDeleteTarget105132C;
    drawAt(gOrderedRecords1051B14[textTarget].text18, 160 - width / 2, 86);
    drawAt(kLegacyGuildDeleteEnd58F3AC, 109, 134);
    Sub_503A20(&buttons[0], 2, 4, 1, gValueD69768, 218);
    Sub_503A20(&buttons[1], 2, 4, 1, gValueD69768, 218);
    const char* hint = nullptr;
    if ((confirmState & 0x40u) != 0) {
        hint = kLegacyGuildDeleteHint58F3A0;
    } else if ((cancelState & 0x40u) != 0) {
        hint = kLegacyGuildKeepHint58F38C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyGuildDeletePanel58EE00, 0);
}

std::uint32_t Sub_52F910(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildSortPanel58EEC8.x;
    const std::int32_t y = gLegacyGuildSortPanel58EEC8.y;
    LegacyPanelButtonRecord confirm {x + 96, y + 220, x + 56, y + 210, 80, 20, 22712, 22713};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildSortPanel58EEC8, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t top = gLegacyGuildSortPanel58EEC8.y + 30 * row;
        const std::int32_t left = gLegacyGuildSortPanel58EEC8.x;
        if (Sub_5049A0(left + 19, top + 38, left + 173, top + 64, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1u) != 0) {
        gOrderMode1057C78 = static_cast<std::uint32_t>(hovered);
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        // 原52F9DD保存的是普通D67CD0，不能改成此页公会排序值。
        static_cast<void>(Sub_5206F0());
        static_cast<void>(Sub_520200());
        static_cast<void>(audio::Sub_504180(94));
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    }
    if (hovered >= 0) {
        const std::int32_t image = gValueD69768;
        const std::int32_t left = gLegacyGuildSortPanel58EEC8.x;
        const std::int32_t top = gLegacyGuildSortPanel58EEC8.y + 30 * hovered;
        static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(left + 19),
            static_cast<std::int16_t>(top + 38), static_cast<std::int16_t>(left + 173),
            static_cast<std::int16_t>(top + 64), 212, image, 0));
    }
    const std::int32_t selected = std::bit_cast<std::int32_t>(gOrderMode1057C78);
    const std::int32_t selectedTop = gLegacyGuildSortPanel58EEC8.y + 30 * selected;
    const std::int32_t selectedLeft = gLegacyGuildSortPanel58EEC8.x;
    static_cast<void>(DrawGraphicHover(static_cast<std::int16_t>(selectedLeft + 19),
        static_cast<std::int16_t>(selectedTop + 38), static_cast<std::int16_t>(selectedLeft + 173),
        static_cast<std::int16_t>(selectedTop + 64), 212, 254, 0));
    const auto drawAt = [](const char* const text, const std::int32_t offsetX, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyGuildSortPanel58EEC8.y + offsetY;
        const std::int32_t left = gLegacyGuildSortPanel58EEC8.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    drawAt(kLegacyGuildSortFirst58F408, 71, 43);
    drawAt(kLegacyGuildSortSecond58F400, 62, 73);
    drawAt(kLegacyGuildSortThird58F3F4, 45, 103);
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacyGuildSortPanel58EEC8, 1);
}

std::uintptr_t Sub_52FCF0()
{
    BuildOrderedRecordIndices_52FCC0();
    static_cast<void>(world::detail::Sub_518DA0());
    const std::int32_t page = gLegacyGuildInitialPage1050DA4;
    gLegacyGuildPage1050B78 = page;
    std::uintptr_t result = static_cast<std::uint32_t>(page);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (gLegacyGuildUnreadActions1050DB8[row] == nullptr) {
            const std::int32_t y = gLegacyGuildContactPanel58EF90.y;
            const std::int32_t x = gLegacyGuildContactPanel58EF90.x;
            YobiAction* const action = Sub_4EBF70(103101, x + 72, y + 38 + 90 * row, 228);
            gLegacyGuildUnreadActions1050DB8[row] = action;
            result = reinterpret_cast<std::uintptr_t>(action);
            if (action != nullptr) {
                SetYobiActionFlag2(action, true);
            }
        }
    }
    return result;
}

std::uintptr_t Sub_52FD60(const std::int32_t opened)
{
    const std::int32_t y = gLegacyGuildContactPanel58EF90.y;
    const std::int32_t x = gLegacyGuildContactPanel58EF90.x;
    std::array<LegacyPanelButtonRecord, 4> sends {};
    std::array<LegacyPanelButtonRecord, 4> reads {};
    std::array<LegacyPanelButtonRecord, 4> removes {};
    std::array<LegacyPanelButtonRecord, 4> ranks {};
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t top = y + 90 * row;
        sends[row] = {x + 236, top + 45, x + 210, top + 38, 53, 15, 238523, 238524};
        reads[row] = {x + 236, top + 65, x + 210, top + 58, 53, 15, 238525, 238526};
        removes[row] = {x + 236, top + 106, x + 210, top + 99, 53, 15, 238527, 238528};
        ranks[row] = {x + 236, top + 86, x + 210, top + 79, 53, 15, 238529, 238530};
    }
    LegacyPanelButtonRecord sort {x + 40, y + 406, x + 10, y + 396, 60, 20, 238531, 238532};
    LegacyPanelButtonRecord previous {x + 157, y + 406, x + 144, y + 396, 26, 20, 238515, 238516};
    LegacyPanelButtonRecord next {x + 248, y + 406, x + 235, y + 396, 26, 20, 238513, 238514};
    LegacyPanelButtonRecord details {x + 107, y + 406, x + 77, y + 396, 60, 20, 238533, 238534};
    LegacyPanelButtonRecord contacts {x + 135, y + 436, x + 90, y + 422, 90, 28, 0, 0};
    GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildContactPanel58EF90,
        [] { static_cast<void>(Sub_52FCF0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* result = nullptr;
        for (auto& slot : gLegacyGuildUnreadActions1050DB8) {
            result = slot;
            if (result != nullptr) {
                result = MarkDeath(result);
                slot = nullptr;
            }
        }
        return reinterpret_cast<std::uintptr_t>(result);
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        if (gLegacyGuildUnreadActions1050DB8[row] != nullptr) {
            const std::int32_t page = gLegacyGuildPage1050B78;
            const std::int32_t index = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[row + 4 * page]);
            // 原53043C为普通邮件查询，不改成公会5190F0；查询后重取动画槽。
            const std::int32_t unread = world::detail::Sub_519090(index);
            YobiAction* const action = gLegacyGuildUnreadActions1050DB8[row];
            SetYobiActionFlag2(action, unread == 0);
        }
    }
    std::int32_t hoverKind = -1;
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = gLegacyGuildPage1050B78;
        const std::int32_t position = row + 4 * page;
        const std::int32_t index = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        if (gOrderedRecords1051B14[index].value08 == 0) {
            continue;
        }
        if ((QueryPanelButtonState(&sends[row]) & 0x40) != 0) {
            hoverKind = 0;
        } else if ((QueryPanelButtonState(&reads[row]) & 0x40) != 0) {
            hoverKind = 1;
        } else if ((QueryPanelButtonState(&removes[row]) & 0x40) != 0) {
            hoverKind = 2;
        } else if ((QueryPanelButtonState(&ranks[row]) & 0x40) != 0) {
            hoverKind = 3;
        }
        if ((QueryPanelButtonState(&sends[row]) & 1) != 0) {
            const std::int32_t selected = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
            const std::int32_t currentPage = gLegacyGuildPage1050B78;
            gLegacyGuildComposeTarget1051340 = selected;
            gLegacyGuildMailTarget1050DCC = selected;
            const std::int32_t scene = world::gCurrentSceneType;
            gLegacyGuildComposePage1051330 = currentPage;
            bool available = false;
            if (scene != 10) {
                for (std::int32_t pet = 0; pet < 5; ++pet) {
                    if (world::detail::Sub_52E1D0(pet) != 0) {
                        available = true;
                        break;
                    }
                }
            }
            static_cast<void>(audio::Sub_504180(available ? 91 : 90));
            break;
        }
        if ((QueryPanelButtonState(&reads[row]) & 1) != 0) {
            const std::int32_t selected = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
            const std::int32_t currentPage = gLegacyGuildPage1050B78;
            gLegacyGuildReadTarget1050DB4 = selected;
            gLegacyGuildComposeTarget1051340 = selected;
            gLegacyGuildComposePage1051330 = currentPage;
            gLegacyGuildMailTarget1050DCC = selected;
            static_cast<void>(audio::Sub_504180(89));
            break;
        }
        if ((QueryPanelButtonState(&removes[row]) & 1) != 0) {
            const std::int32_t selected = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
            gLegacyGuildDeleteTarget105132C = selected;
            static_cast<void>(audio::Sub_504180(92));
            break;
        }
        if ((QueryPanelButtonState(&ranks[row]) & 1) != 0) {
            const std::int32_t selected = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
            gLegacyGuildRankTarget1057C70 = selected;
            static_cast<void>(audio::Sub_504180(96));
            break;
        }
    }
    const std::uint8_t sortState = static_cast<std::uint8_t>(QueryPanelButtonState(&sort));
    const std::uint8_t previousState = static_cast<std::uint8_t>(QueryPanelButtonState(&previous));
    const std::uint8_t nextState = static_cast<std::uint8_t>(QueryPanelButtonState(&next));
    const std::uint8_t detailsState = static_cast<std::uint8_t>(QueryPanelButtonState(&details));
    const std::uint8_t contactsState = static_cast<std::uint8_t>(QueryPanelButtonState(&contacts));
    if ((previousState & 0x10u) != 0 || platform::ReadControlKeyPressFlag_4597C0(90) != 0) {
        --gLegacyGuildPage1050B78;
        if (gLegacyGuildPage1050B78 < 0) {
            gLegacyGuildPage1050B78 = 49;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((nextState & 0x10u) != 0 || platform::ReadControlKeyPressFlag_4597C0(88) != 0) {
        ++gLegacyGuildPage1050B78;
        if (gLegacyGuildPage1050B78 >= 50) {
            gLegacyGuildPage1050B78 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((sortState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(93));
    } else if ((detailsState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(95));
    } else if ((contactsState & 1u) != 0) {
        static_cast<void>(audio::Sub_504180(42));
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyGuildContactPanel58EF90.y + offsetY;
        const std::int32_t left = gLegacyGuildContactPanel58EF90.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 1, color, text, 0, 0));
    };
    char buffer[128];
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t page = gLegacyGuildPage1050B78;
        const std::int32_t position = row + 4 * page;
        const std::int32_t index = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        const auto* const record = &gOrderedRecords1051B14[index];
        if (record->value08 == 0) {
            continue;
        }
        const std::int8_t color = record->value0A > 0 ? 0 : 7;
        const std::int32_t image = character_select::detail::Sub_51BC50(record->value14);
        const std::int32_t imageY = gLegacyGuildContactPanel58EF90.y + 68 + 90 * row;
        const std::int32_t imageX = gLegacyGuildContactPanel58EF90.x + 36;
        static_cast<void>(DrawGraphic(imageX, imageY, 212, image, 0, &descriptor));
        const std::int32_t statusIndex = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        const std::int16_t status = gOrderedRecords1051B14[statusIndex].value0A;
        const std::int32_t statusY = gLegacyGuildContactPanel58EF90.y + 52 + 90 * row;
        const std::int32_t statusImage = kLegacyGuildStatusImages58F054[status];
        const std::int32_t statusX = gLegacyGuildContactPanel58EF90.x + 105;
        static_cast<void>(DrawGraphic(statusX, statusY, 212, statusImage, 0, &descriptor));
        const std::int32_t valueIndex = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        platform::FormatLegacyString_53B304(buffer, "%3d", gOrderedRecords1051B14[valueIndex].value10);
        drawAt(buffer, 180, 45 + 90 * row, color);
        const std::int32_t nameIndex = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        drawAt(gOrderedRecords1051B14[nameIndex].text18, 77, 68 + 90 * row, color);
        const std::int32_t rankIndex = std::bit_cast<std::int32_t>(platform::BgmRuntimeOrderWords()[position]);
        const char* const rank = BorrowGuildRankText1051518(gOrderedRecords1051B14[rankIndex].value00);
        if (rank[0] != '\0') {
            drawAt(rank, 77, 91 + 90 * row, color);
        }
        Sub_5035E0(&sends[row], 212, 1, gValueD69768, 0);
        Sub_5035E0(&reads[row], 212, 1, gValueD69768, 0);
        Sub_5035E0(&removes[row], 212, 1, gValueD69768, 0);
        Sub_5035E0(&ranks[row], 212, 1, gValueD69768, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", gLegacyGuildPage1050B78 + 1, 50);
    drawAt(buffer, 176, 399, 0);
    Sub_5035E0(&sort, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&details, 212, 1, gValueD69768, 0);
    Sub_5035E0(&contacts, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    // 原530A33查询5857D0人物面板；五公共按钮提示沿绘制前缓存状态。
    if (Sub_503590(&gLegacyProfilePanel5857D0) != 0) {
        hint = kLegacyGuildClose58F248;
    } else if ((previousState & 0x40u) != 0 || (nextState & 0x40u) != 0) {
        hint = kLegacyGuildPage58F2D4;
    } else if ((sortState & 0x40u) != 0) {
        hint = kLegacyGuildSort58F4A8;
    } else if ((detailsState & 0x40u) != 0) {
        hint = kLegacyGuildDetails58F490;
    } else if ((contactsState & 0x40u) != 0) {
        hint = kLegacyGuildContacts58F474;
    } else if (hoverKind == 0) {
        hint = kLegacyGuildSend58F2EC;
    } else if (hoverKind == 1) {
        hint = kLegacyGuildRead58F44C;
    } else if (hoverKind == 2) {
        hint = kLegacyGuildRemove58F42C;
    } else if (hoverKind == 3) {
        hint = kLegacyGuildRank58F410;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyGuildContactPanel58EF90, 1));
}


void Sub_530B70()
{
    if (ReadGuildKey10514A8() == -1) {
        gLegacyGuildEditAllowed1050F14 = 0;
        return;
    }
    gLegacyGuildEditAllowed1050F14 = ReadGuildTextIndex10514B0() == 0 ? 1 : 0;
}

std::uintptr_t Sub_530BA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildDetailsPanel58F098.x;
    const std::int32_t y = gLegacyGuildDetailsPanel58F098.y;
    std::array<LegacyPanelButtonRecord, 3> changes {{
        {x + 139, y + 212, x + 205, y + 58, 64, 24, 22981, 22982},
        {x + 139, y + 272, x + 205, y + 118, 64, 24, 22981, 22982},
        {x + 139, y + 334, x + 205, y + 180, 64, 24, 22981, 22982},
    }};
    LegacyPanelButtonRecord previous {x + 120, y + 233, x + 112, y + 225, 17, 17, 238538, 238539};
    LegacyPanelButtonRecord next {x + 197, y + 233, x + 189, y + 225, 17, 17, 238536, 238537};
    std::array<std::array<LegacyPanelButtonRecord, 5>, 2> permissions;
    std::array<std::array<LegacyPanelButtonRecord, 5>, 2> marks;
    std::array<LegacyPanelButtonRecord, 5> rows;
    for (std::int32_t row = 0; row < 5; ++row) {
        permissions[0][row] = {x + 216, y + 268 + 30 * row, x + 208, y + 261 + 30 * row, 14, 14, 0, 0};
        permissions[1][row] = {x + 232, y + 268 + 30 * row, x + 224, y + 261 + 30 * row, 14, 14, 0, 0};
        marks[0][row] = {x + 216, y + 268 + 30 * row, x + 208, y + 261 + 30 * row, 14, 14, 238540, 0};
        marks[1][row] = {x + 233, y + 268 + 30 * row, x + 224, y + 261 + 30 * row, 14, 14, 238540, 0};
        rows[row] = {x + 199, y + 280 + 30 * row, x + 11, y + 258 + 30 * row, 376, 44, 0, 0};
    }
    std::array<std::int32_t, 3> changeStates {};
    std::int32_t nextState = 0;
    std::array<std::int32_t, 5> rowStates {};
    std::array<std::array<std::int32_t, 5>, 2> permissionStates {};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildDetailsPanel58F098, Sub_530B70, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        std::int32_t currentCount = 0;
        for (input::InputContext* const context : gLegacyGuildDetailsInputs1057C7C) {
            if (input::IsCurrentContext_503E40(context) != 0) {
                ++currentCount;
            }
        }
        std::uintptr_t result = 0;
        if (currentCount != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyGuildDetailsInputs1057C7C.fill(nullptr);
        return result;
    }
    // 原末尾384B为三个128B串区，不能按反编译InputContext解释。
    char temporary[384];
    char* const first = temporary;
    char* const second = temporary + 128;
    char* const buffer = temporary + 256;
    if (gLegacyGuildDetailsInputs1057C7C[0] == nullptr) {
        for (std::int32_t index = 0; index < 3; ++index) {
            gLegacyGuildDetailsInputs1057C7C[index] = &input::gInputContexts1050F18[index];
            gLegacyGuildDetailsInputs1057C7C[index]->text[0] = '\0';
            gLegacyGuildDetailsInputs1057C7C[index]->totalByteLength = 0;
            gLegacyGuildDetailsInputs1057C7C[index]->caretByteOffset = 0;
            const char* const initial = index == 0 ? BorrowGuildName10514BC()
                : (index == 1 ? BorrowGuildSecondText10514E5() : input::LegacyText593850());
            std::strcpy(buffer, initial);
            const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y + 60 * (index + 1);
            input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[index];
            const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x + 13;
            input::InitializeContext_449CF0(context, inputX, inputY, 2, 0, 0, buffer, 1, 16, 0, 0);
        }
        std::strcpy(gLegacyGuildNameCache1050DD0.data(), BorrowGuildName10514BC());
        input::InputContext* const focus = gLegacyGuildDetailsInputs1057C7C[0];
        std::strcpy(gLegacyGuildSecondCache1050E50.data(), BorrowGuildSecondText10514E5());
        input::SetContext_44B330(focus);
        gLegacyGuildDetailsHighlights1050EFC[0] = 0;
        gLegacyGuildDetailsPage1050ED4 = 0;
        gLegacyGuildDetailsHighlights1050EFC[1] = 0;
        gLegacyGuildDetailsPageCount1050ED0 = 7;
        gLegacyGuildDetailsHighlights1050EFC[2] = 0;
        gLegacyGuildDetailsSelected58F15C = -1;
        gLegacyGuildDetailsHighlights1050EFC[3] = 0;
        gLegacyGuildDetailsHighlights1050EFC[4] = 0;
    }
    if (std::strcmp(gLegacyGuildNameCache1050DD0.data(), BorrowGuildName10514BC()) != 0) {
        const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y;
        std::strcpy(gLegacyGuildNameCache1050DD0.data(), BorrowGuildName10514BC());
        input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[0];
        const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x;
        input::InitializeContext_449CF0(context, inputX + 13, inputY + 60, 2, 0, 0,
            gLegacyGuildNameCache1050DD0.data(), 1, 16, 0, 0);
    }
    if (std::strcmp(gLegacyGuildSecondCache1050E50.data(), BorrowGuildSecondText10514E5()) != 0) {
        const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x;
        std::strcpy(gLegacyGuildSecondCache1050E50.data(), BorrowGuildSecondText10514E5());
        input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[1];
        const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y;
        input::InitializeContext_449CF0(context, inputX + 13, inputY + 120, 2, 0, 0,
            gLegacyGuildSecondCache1050E50.data(), 1, 16, 0, 0);
    }
    const std::int32_t previousState = QueryPanelButtonState(&previous);
    bool pageChanged = false;
    if ((previousState & 1) != 0 || platform::ReadControlKeyPressFlag_4597C0(90) != 0) {
        --gLegacyGuildDetailsPage1050ED4;
        if (gLegacyGuildDetailsPage1050ED4 < 0) {
            gLegacyGuildDetailsPage1050ED4 = gLegacyGuildDetailsPageCount1050ED0 - 1;
        }
        pageChanged = true;
    } else {
        nextState = QueryPanelButtonState(&next);
        if ((nextState & 1) != 0 || platform::ReadControlKeyPressFlag_4597C0(88) != 0) {
            gLegacyGuildDetailsPage1050ED4 = (gLegacyGuildDetailsPage1050ED4 + 1) % gLegacyGuildDetailsPageCount1050ED0;
            pageChanged = true;
        }
    }
    if (pageChanged) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gLegacyGuildDetailsHighlights1050EFC.fill(0);
        gLegacyGuildDetailsSelected58F15C = -1;
    }
    std::int32_t drawPage;
    if (gLegacyGuildEditAllowed1050F14 != 0) {
        for (std::int32_t index = 0; index < 3; ++index) {
            const std::int32_t top = gLegacyGuildDetailsPanel58F098.y + 60 * index;
            const std::int32_t left = gLegacyGuildDetailsPanel58F098.x;
            if (Sub_5049A0(left + 8, top + 58, left + 196, top + 80, -1) != 0) {
                const std::int32_t hoverY = gLegacyGuildDetailsPanel58F098.y + 60 * index;
                const std::uint32_t image = gValueD69768;
                const std::int32_t hoverX = gLegacyGuildDetailsPanel58F098.x;
                DrawGraphicHover(Low16(hoverX + 8), Low16(hoverY + 58),
                    Low16(hoverX + 196), Low16(hoverY + 80), 214, image, 0);
                input::SetContext_44B330(gLegacyGuildDetailsInputs1057C7C[index]);
            }
        }
        const std::uint32_t now = platform::ReadTickCount32();
        const std::uint32_t deadline = gLegacyGuildDetailsTick1057C88 + 500u;
        if (deadline < now) {
            changeStates[0] = QueryPanelButtonState(&changes[0]);
            bool changeClicked = (changeStates[0] & 1) != 0;
            if (changeClicked) {
                input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[0];
                if (std::strcmp(context->text, BorrowGuildName10514BC()) != 0) {
                    std::strcpy(second, context->text);
                    static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(second, first, 128));
                    const auto socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_511250(socket, 2, first));
                    static_cast<void>(audio::PlaySe_52C450(79, 320));
                }
            } else {
                changeStates[1] = QueryPanelButtonState(&changes[1]);
                changeClicked = (changeStates[1] & 1) != 0;
                if (changeClicked) {
                    input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[1];
                    if (std::strcmp(context->text, BorrowGuildSecondText10514E5()) != 0) {
                        std::strcpy(first, context->text);
                        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(first, second, 128));
                        const auto socket = platform::gConnectionSocket;
                        static_cast<void>(network::Sub_511250(socket, 3, second));
                        static_cast<void>(audio::PlaySe_52C450(79, 320));
                    }
                } else {
                    changeStates[2] = QueryPanelButtonState(&changes[2]);
                    changeClicked = (changeStates[2] & 1) != 0;
                    if (changeClicked) {
                        input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[2];
                        if (context->text[0] != '\0') {
                            std::int32_t selectedRow = 0;
                            while (selectedRow < 5 && gLegacyGuildDetailsHighlights1050EFC[selectedRow] == 0) {
                                ++selectedRow;
                            }
                            if (selectedRow < 5) {
                                std::strcpy(first, context->text);
                                static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(first, second, 128));
                                const std::int32_t page = gLegacyGuildDetailsPage1050ED4;
                                const std::int32_t index = selectedRow + 5 * page;
                                const std::int32_t flags = ReadGuildRankFlags1051514(index);
                                const auto socket = platform::gConnectionSocket;
                                static_cast<void>(network::Sub_5112C0(socket, index, flags, second));
                                static_cast<void>(audio::PlaySe_52C450(79, 320));
                                const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y;
                                const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x;
                                gLegacyGuildDetailsHighlights1050EFC.fill(0);
                                input::InputContext* const resetContext = gLegacyGuildDetailsInputs1057C7C[2];
                                gLegacyGuildDetailsSelected58F15C = -1;
                                input::InitializeContext_449CF0(resetContext, inputX + 13, inputY + 180,
                                    2, 0, 0, input::LegacyText593850(), 1, 16, 0, 0);
                            } else {
                                std::int32_t emptyRank = 0;
                                while (emptyRank < 32 && BorrowGuildRankText1051518(emptyRank)[0] != '\0') {
                                    ++emptyRank;
                                }
                                if (emptyRank < 32) {
                                    std::strcpy(first, context->text);
                                    static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(first, second, 128));
                                    const auto socket = platform::gConnectionSocket;
                                    const std::int32_t flags = ReadGuildRankFlags1051514(emptyRank);
                                    static_cast<void>(network::Sub_5112C0(socket, -1, flags, second));
                                    static_cast<void>(audio::PlaySe_52C450(79, 320));
                                    const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y;
                                    const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x;
                                    input::InputContext* const resetContext = gLegacyGuildDetailsInputs1057C7C[2];
                                    input::InitializeContext_449CF0(resetContext, inputX + 13, inputY + 180,
                                        2, 0, 0, input::LegacyText593850(), 1, 16, 0, 0);
                                } else {
                                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                                }
                            }
                        }
                    }
                }
            }
            if (changeClicked) {
                gLegacyGuildDetailsTick1057C88 = platform::ReadTickCount32();
            }
            // 原531D03/531E06仍继续各组，不因前面已发送或更新时间而重判门槛。
            for (std::int32_t group = 0; group < 2; ++group) {
                for (std::int32_t row = 0; row < 5; ++row) {
                    permissionStates[group][row] = QueryPanelButtonState(&permissions[group][row]);
                    if ((permissionStates[group][row] & 1) == 0) {
                        continue;
                    }
                    const std::int32_t page = gLegacyGuildDetailsPage1050ED4;
                    const std::int32_t index = row + 5 * page;
                    if (index < 32 && BorrowGuildRankText1051518(index)[0] != '\0') {
                        std::strcpy(first, BorrowGuildRankText1051518(index));
                        BorrowGuildRankFlags1051514(index) ^= (1 << group);
                        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(first, second, 128));
                        const std::int32_t sendPage = gLegacyGuildDetailsPage1050ED4;
                        const std::int32_t sendIndex = row + 5 * sendPage;
                        const std::int32_t flags = ReadGuildRankFlags1051514(sendIndex);
                        const auto socket = platform::gConnectionSocket;
                        static_cast<void>(network::Sub_5112C0(socket, sendIndex, flags, second));
                        static_cast<void>(audio::PlaySe_52C450(79, 320));
                        gLegacyGuildDetailsTick1057C88 = platform::ReadTickCount32();
                    }
                    break;
                }
            }
        }
        for (auto& button : changes) {
            Sub_5035E0(&button, 212, 1, gValueD69768, 0);
        }
        drawPage = gLegacyGuildDetailsPage1050ED4;
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t index = row + 5 * drawPage;
            if (index < 32 && BorrowGuildRankText1051518(index)[0] != '\0') {
                Sub_5035E0(&permissions[0][row], 212, 1, gValueD69768, 0);
                Sub_5035E0(&permissions[1][row], 212, 1, gValueD69768, 0);
                drawPage = gLegacyGuildDetailsPage1050ED4;
                if ((ReadGuildRankFlagsByte1051514(row + 5 * drawPage) & 1u) != 0) {
                    Sub_5035E0(&marks[0][row], 212, 0, gValueD69768, 1);
                    drawPage = gLegacyGuildDetailsPage1050ED4;
                }
                if ((ReadGuildRankFlagsByte1051514(row + 5 * drawPage) & 2u) != 0) {
                    Sub_5035E0(&marks[1][row], 212, 0, gValueD69768, 1);
                    drawPage = gLegacyGuildDetailsPage1050ED4;
                }
            }
        }
        for (std::int32_t row = 0; row < 5; ++row) {
            if (row + 5 * drawPage < 32) {
                const std::int32_t top = gLegacyGuildDetailsPanel58F098.y + 30 * row;
                const std::int32_t left = gLegacyGuildDetailsPanel58F098.x;
                const bool hit = Sub_5049A0(left + 11, top + 258, left + 199, top + 280, -1) != 0;
                drawPage = gLegacyGuildDetailsPage1050ED4;
                if (hit && BorrowGuildRankText1051518(row + 5 * drawPage)[0] != '\0') {
                    const std::int32_t hoverY = gLegacyGuildDetailsPanel58F098.y + 30 * row;
                    const std::uint32_t image = gValueD69768;
                    const std::int32_t hoverX = gLegacyGuildDetailsPanel58F098.x;
                    DrawGraphicHover(Low16(hoverX + 11), Low16(hoverY + 257),
                        Low16(hoverX + 199), Low16(hoverY + 279), 214, image, 0);
                    rowStates[row] = QueryPanelButtonState(&rows[row]);
                    if ((rowStates[row] & 1) != 0) {
                        const std::int32_t selectedPage = gLegacyGuildDetailsPage1050ED4;
                        gLegacyGuildDetailsHighlights1050EFC[0] = 0;
                        gLegacyGuildDetailsHighlights1050EFC[1] = 0;
                        const std::int32_t inputX = gLegacyGuildDetailsPanel58F098.x;
                        gLegacyGuildDetailsSelected58F15C = row + 5 * selectedPage;
                        gLegacyGuildDetailsHighlights1050EFC[2] = 0;
                        gLegacyGuildDetailsHighlights1050EFC[3] = 0;
                        gLegacyGuildDetailsHighlights1050EFC[4] = 0;
                        const char* const initial = BorrowGuildRankText1051518(gLegacyGuildDetailsSelected58F15C);
                        const std::int32_t inputY = gLegacyGuildDetailsPanel58F098.y;
                        input::InputContext* const context = gLegacyGuildDetailsInputs1057C7C[2];
                        gLegacyGuildDetailsHighlights1050EFC[row] = 1;
                        input::InitializeContext_449CF0(context, inputX + 15, inputY + 182,
                            2, 0, 0, initial, 1, 16, 0, 0);
                    }
                    drawPage = gLegacyGuildDetailsPage1050ED4;
                }
            }
        }
    } else {
        drawPage = gLegacyGuildDetailsPage1050ED4;
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int8_t color = gLegacyGuildDetailsHighlights1050EFC[row] != 0 ? 1 : 0;
        const std::int32_t index = row + 5 * drawPage;
        if (index < 32) {
            const char* const text = BorrowGuildRankText1051518(index);
            const std::int32_t top = gLegacyGuildDetailsPanel58F098.y + 260 + 30 * row;
            const std::int32_t left = gLegacyGuildDetailsPanel58F098.x + 13;
            static_cast<void>(DrawText(Low16(left), Low16(top), 2, 0, color, text, 0, 0));
            drawPage = gLegacyGuildDetailsPage1050ED4;
        }
    }
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const std::int32_t page = gLegacyGuildDetailsPage1050ED4;
    const std::int32_t pageCount = gLegacyGuildDetailsPageCount1050ED0;
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", page + 1, pageCount);
    const std::int32_t pageY = gLegacyGuildDetailsPanel58F098.y + 225;
    const std::int32_t pageX = gLegacyGuildDetailsPanel58F098.x + 132;
    static_cast<void>(DrawText(Low16(pageX), Low16(pageY), 2, 0, 0, buffer, 0, 0));
    for (input::InputContext* const context : gLegacyGuildDetailsInputs1057C7C) {
        render::DrawInputText_4568A0(context);
    }
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if (gLegacyGuildEditAllowed1050F14 != 0) {
        if ((changeStates[0] & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildNameEdit58F550);
        } else if ((changeStates[1] & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildSecondEdit58F534);
        } else if ((changeStates[2] & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildRankEdit58F518);
        }
    }
    if ((previousState & 0x40) != 0 || (nextState & 0x40) != 0) {
        std::strcpy(hint, kLegacyGuildPage58F2D4);
    }
    for (const std::int32_t rowState : rowStates) {
        if ((rowState & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildRankSelect58F4FC);
            break;
        }
    }
    const std::int32_t hintPage = gLegacyGuildDetailsPage1050ED4;
    for (std::int32_t group = 0; group < 2; ++group) {
        for (std::int32_t row = 0; row < 5; ++row) {
            if ((permissionStates[group][row] & 0x40) != 0) {
                const std::int32_t index = row + 5 * hintPage;
                if (index < 32 && BorrowGuildRankText1051518(index)[0] != '\0') {
                    std::strcpy(hint, group == 0 ? kLegacyGuildPermissionFirst58F4E4 : kLegacyGuildPermissionSecond58F4D0);
                    break;
                }
            }
        }
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyGuildDetailsPanel58F098, 1));
}

std::int32_t Sub_532430()
{
    const std::int32_t rank = ReadGuildTextIndex10514B0();
    const std::int32_t allowed = rank == 0 ? 1 : 0;
    gLegacyGuildEditAllowed1050F14 = allowed;
    return allowed;
}

std::uint32_t Sub_532450(const std::int32_t opened)
{
    const std::int32_t x = gLegacyGuildRankPanel58F160.x;
    const std::int32_t y = gLegacyGuildRankPanel58F160.y;
    LegacyPanelButtonRecord previous {x + 139, y + 109, x + 125, y + 97, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 239, y + 109, x + 225, y + 97, 28, 24, 23023, 23027};
    LegacyPanelButtonRecord remove {x + 81, y + 384, x + 41, y + 374, 80, 20, 22983, 22984};
    // 原修改按钮中心99/526与命中矩形165/372不一致，保留真实八字段。
    LegacyPanelButtonRecord change {x + 99, y + 526, x + 165, y + 372, 64, 24, 22981, 22982};
    std::array<LegacyPanelButtonRecord, 8> rows;
    for (std::int32_t row = 0; row < 8; ++row) {
        rows[row] = {x + 229, y + 155 + 30 * row, x + 41, y + 133 + 30 * row, 376, 44, 0, 0};
    }
    std::int32_t nextState = 0;
    std::int32_t removeState = 0;
    std::int32_t changeState = 0;
    std::array<std::int32_t, 8> rowStates {};
    const std::int32_t state = Sub_503160(opened, &gLegacyGuildRankPanel58F160,
        [] { static_cast<void>(Sub_532430()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        gLegacyGuildRankReady1057C8C = 0;
        return 2;
    }
    if (gLegacyGuildRankReady1057C8C == 0) {
        gLegacyGuildRankPage1051334 = 0;
        gLegacyGuildRankPageCount105133C = 4;
        gLegacyGuildRankReady1057C8C = 1;
        gLegacyGuildRankHighlights1050ED8.fill(0);
        gLegacyGuildRankSelected1051338 = -1;
    }
    bool pageChanged = false;
    const std::int32_t previousState = QueryPanelButtonState(&previous);
    if ((previousState & 1) != 0 || platform::ReadControlKeyPressFlag_4597C0(90) != 0) {
        --gLegacyGuildRankPage1051334;
        if (gLegacyGuildRankPage1051334 < 0) {
            gLegacyGuildRankPage1051334 = gLegacyGuildRankPageCount105133C - 1;
        }
        pageChanged = true;
    } else {
        nextState = QueryPanelButtonState(&next);
        if ((nextState & 1) != 0 || platform::ReadControlKeyPressFlag_4597C0(88) != 0) {
            gLegacyGuildRankPage1051334 = (gLegacyGuildRankPage1051334 + 1) % gLegacyGuildRankPageCount105133C;
            pageChanged = true;
        } else if (gLegacyGuildEditAllowed1050F14 != 0) {
            for (std::int32_t row = 0; row < 8; ++row) {
                const std::int32_t top = gLegacyGuildRankPanel58F160.y + 30 * row;
                const std::int32_t left = gLegacyGuildRankPanel58F160.x;
                if (Sub_5049A0(left + 41, top + 133, left + 229, top + 155, -1) == 0) {
                    continue;
                }
                const std::int32_t page = gLegacyGuildRankPage1051334;
                if (BorrowGuildRankText1051518(row + 8 * page)[0] == '\0') {
                    continue;
                }
                const std::int32_t hoverY = gLegacyGuildRankPanel58F160.y + 30 * row;
                const std::uint32_t image = gValueD69768;
                const std::int32_t hoverX = gLegacyGuildRankPanel58F160.x;
                DrawGraphicHover(Low16(hoverX + 41), Low16(hoverY + 132),
                    Low16(hoverX + 229), Low16(hoverY + 154), 214, image, 0);
                rowStates[row] = QueryPanelButtonState(&rows[row]);
                if ((rowStates[row] & 1) != 0) {
                    const std::int32_t selectedPage = gLegacyGuildRankPage1051334;
                    gLegacyGuildRankHighlights1050ED8.fill(0);
                    gLegacyGuildRankHighlights1050ED8[row] = 1;
                    gLegacyGuildRankSelected1051338 = row + 8 * selectedPage;
                }
            }
            const std::uint32_t now = platform::ReadTickCount32();
            const std::uint32_t deadline = gLegacyGuildRankTick1057C90 + 500u;
            if (deadline < now) {
                removeState = QueryPanelButtonState(&remove);
                if ((removeState & 1) != 0) {
                    const std::int32_t target = gLegacyGuildRankTarget1057C70;
                    const auto socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_511350(socket, target, -1));
                    gLegacyGuildRankTick1057C90 = platform::ReadTickCount32();
                } else {
                    changeState = QueryPanelButtonState(&change);
                    if ((changeState & 1) != 0) {
                        const std::int32_t rank = gLegacyGuildRankSelected1051338;
                        if (rank == -1) {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                            world::detail::Sub_44B050(kLegacyGuildRankRequired58F584, 4, 0);
                        } else {
                            const auto socket = platform::gConnectionSocket;
                            const std::int32_t target = gLegacyGuildRankTarget1057C70;
                            static_cast<void>(network::Sub_511350(socket, target, rank));
                            static_cast<void>(audio::PlaySe_52C450(79, 320));
                        }
                        gLegacyGuildRankTick1057C90 = platform::ReadTickCount32();
                    }
                }
            }
        }
    }
    if (pageChanged) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gLegacyGuildRankHighlights1050ED8.fill(0);
        gLegacyGuildRankSelected1051338 = -1;
    }
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t color) {
        const std::int32_t top = gLegacyGuildRankPanel58F160.y + offsetY;
        const std::int32_t left = gLegacyGuildRankPanel58F160.x + offsetX;
        static_cast<void>(DrawText(Low16(left), Low16(top), 2, 0, color, text, 0, 0));
    };
    const std::int32_t nameTarget = gLegacyGuildRankTarget1057C70;
    drawAt(gOrderedRecords1051B14[nameTarget].text18, 30, 8, 0);
    const std::int32_t rankTarget = gLegacyGuildRankTarget1057C70;
    const std::int32_t currentRank = gOrderedRecords1051B14[rankTarget].value00;
    if (currentRank != -1) {
        drawAt(BorrowGuildRankText1051518(currentRank), 8, 58, 0);
    }
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t page = gLegacyGuildRankPage1051334;
        const std::int8_t color = gLegacyGuildRankHighlights1050ED8[row] != 0 ? 1 : 0;
        drawAt(BorrowGuildRankText1051518(row + 8 * page), 41, 133 + 30 * row, color);
    }
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const std::int32_t page = gLegacyGuildRankPage1051334;
    const std::int32_t pageCount = gLegacyGuildRankPageCount105133C;
    char buffer[128];
    platform::FormatLegacyString_53B304(buffer, "%2d/%2d", page + 1, pageCount);
    drawAt(buffer, 161, 100, 0);
    if (gLegacyGuildEditAllowed1050F14 != 0) {
        Sub_5035E0(&remove, 212, 1, gValueD69768, 0);
        Sub_5035E0(&change, 212, 1, gValueD69768, 0);
    }
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if ((previousState & 0x40) != 0 || (nextState & 0x40) != 0) {
        std::strcpy(hint, kLegacyGuildPage58F2D4);
    }
    if (gLegacyGuildEditAllowed1050F14 != 0) {
        if ((removeState & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildRankRemove58F568);
        }
        if ((changeState & 0x40) != 0) {
            std::strcpy(hint, kLegacyGuildRank58F410);
        }
        for (const std::int32_t rowState : rowStates) {
            if ((rowState & 0x40) != 0) {
                std::strcpy(hint, kLegacyGuildRankSelect58F4FC);
                break;
            }
        }
    }
    return Sub_503440(&gLegacyGuildRankPanel58F160, 1);
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
    platform::FormatLegacyString_53B304(buffer, "%+9d", world::detail::gValueFF0D04);
    const std::int32_t deltaY = gLegacyPointResultPanel5838A8.y;
    const std::int32_t deltaX = gLegacyPointResultPanel5838A8.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(deltaX + 69),
        static_cast<std::int16_t>(deltaY + 44), 2, 1, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%9d", world::detail::gValueFF0D08);
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
            platform::FormatLegacyString_53B304(buffer, "%+6d", result->value04);
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
                platform::FormatLegacyString_53B304(buffer, "%+6d", skillResults[skillIndex].value04);
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
    platform::FormatLegacyString_53B304(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
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
    platform::FormatLegacyString_53B304(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
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
    platform::FormatLegacyString_53B304(buffer, kLegacyFpNotice589BFC);
    const std::int32_t messageY = gLegacyFpNoticePanel583E20.y;
    const std::int32_t messageX = gLegacyFpNoticePanel583E20.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(messageX + 16),
        static_cast<std::int16_t>(messageY + 24), 2, 0, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, kLegacyFpRequired589BE8, gLegacyRequiredFpCC31D8);
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
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::gPlayerItemsFDA8A4[gLegacySplitSourceCC3E60].count);
    const std::int32_t countY = gLegacyItemSplitPanel584078.y + 46;
    const std::int32_t countX = gLegacyItemSplitPanel584078.x + 88;
    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%7d", gLegacySplitAmountC8A948);
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
    platform::FormatLegacyString_53B304(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    const bool belowMaximum =
        world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel;
    // 原满级分支仍格式化一次，只替换最终绘制文本。
    platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
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
    platform::FormatLegacyString_53B304(buffer, "%2d", world::detail::gSkillsF1BAFC[skill].level);
    drawAt(buffer, 234, 39, 1);
    platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].experience);
    drawAt(buffer, 52, 62, 1);
    if (world::detail::gSkillsF1BAFC[skill].level < world::detail::gSkillsF1BAFC[skill].maxLevel) {
        platform::FormatLegacyString_53B304(buffer, "%8d", world::detail::gSkillsF1BAFC[skill].nextExperience);
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
        platform::StrNCpy_53BEC0(gLegacyItemWorkNameCC40D8.data(), source, 64);
    } else {
        platform::StrNCpy_53BEC0(gLegacyItemWorkNameCC40D8.data(),
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
            platform::StrNCpy_53BEC0(gLegacyItemWorkNameCC40D8.data(),
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
                    platform::FormatLegacyString_53B304(buffer, "%3d", count);
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
        platform::FormatLegacyString_53B304(buffer, kItemWorkText589F2C, cost);
        gLegacyItemWorkMessageCA0ACC = buffer;
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
        platform::FormatLegacyString_53B304(buffer, message);
        gLegacyItemWorkMessageCA0ACC = buffer;
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

YobiAction* Sub_4F61C0()
{
    YobiAction* action = gLegacyAlbumActionD67D14;
    gLegacyAlbumReturningCC40D0 = 0;
    if (action == nullptr) {
        const std::int32_t y = gLegacyAlbumDetailPanel5846C0.y + 145;
        const std::int32_t x = gLegacyAlbumDetailPanel5846C0.x + 136;
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t index = gLegacyAlbumSelectionC8AF7C;
        const std::int32_t image = ReadRecordW32(records + index * 0x80, 8);
        action = Sub_4F9FF0(image, x, y, 221);
        gLegacyAlbumActionD67D14 = action;
    }
    return action;
}

std::uintptr_t Sub_4F6220(const std::int32_t opened)
{
    const std::int32_t x = gLegacyAlbumDetailPanel5846C0.x;
    const std::int32_t y = gLegacyAlbumDetailPanel5846C0.y;
    LegacyPanelButtonRecord back {x + 48, y + 418, x + 8, y + 408, 80, 20, 22891, 22892};
    LegacyPanelButtonRecord previousTen {x + 102, y + 418, x + 92, y + 408, 20, 20, 22893, 22894};
    LegacyPanelButtonRecord previous {x + 130, y + 418, x + 120, y + 408, 20, 20, 22895, 22896};
    LegacyPanelButtonRecord next {x + 226, y + 418, x + 216, y + 408, 20, 20, 22897, 22898};
    LegacyPanelButtonRecord nextTen {x + 254, y + 418, x + 244, y + 408, 20, 20, 22899, 22900};
    constexpr std::array<std::int32_t, 2> firstMarks {22904, 22903};
    constexpr std::array<std::int32_t, 3> secondMarks {0, 22905, 22906};
    const std::int32_t state = Sub_503160(opened, &gLegacyAlbumDetailPanel5846C0,
        +[] { static_cast<void>(Sub_4F61C0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        world::detail::Sub_5186A0();
        YobiAction* action = gLegacyAlbumActionD67D14;
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyAlbumActionD67D14 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if ((QueryPanelButtonState(&back) & 1) != 0) {
        gLegacyAlbumReturningCC40D0 = 1;
        static_cast<void>(audio::Sub_504180(23));
    } else {
        bool changed = true;
        if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
            --gLegacyAlbumSelectionC8AF7C;
            if (gLegacyAlbumSelectionC8AF7C < 0) {
                gLegacyAlbumSelectionC8AF7C += std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
            }
        } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
            const std::int32_t selection = gLegacyAlbumSelectionC8AF7C;
            const std::int32_t count = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
            gLegacyAlbumSelectionC8AF7C = selection + 1;
            if (gLegacyAlbumSelectionC8AF7C >= count) {
                gLegacyAlbumSelectionC8AF7C -= count;
            }
        } else if ((QueryPanelButtonState(&previousTen) & 0x10) != 0) {
            gLegacyAlbumSelectionC8AF7C -= 10;
            if (gLegacyAlbumSelectionC8AF7C < 0) {
                gLegacyAlbumSelectionC8AF7C += std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
            }
        } else if ((QueryPanelButtonState(&nextTen) & 0x10) != 0) {
            const std::int32_t selection = gLegacyAlbumSelectionC8AF7C;
            const std::int32_t count = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
            gLegacyAlbumSelectionC8AF7C = selection + 10;
            if (gLegacyAlbumSelectionC8AF7C >= count) {
                gLegacyAlbumSelectionC8AF7C -= count;
            }
        } else {
            changed = false;
        }
        if (changed) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    const auto selectedRecord = [] {
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t index = gLegacyAlbumSelectionC8AF7C;
        return records + index * 0x80;
    };
    const auto drawAt = [](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyAlbumDetailPanel5846C0.y + offsetY;
        const std::int32_t left = gLegacyAlbumDetailPanel5846C0.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    char buffer[128];
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
    const std::int32_t index = gLegacyAlbumSelectionC8AF7C;
    const std::byte* const record = records + index * 0x80;
    if (record[0] != std::byte{0}) {
        YobiAction* const action = gLegacyAlbumActionD67D14;
        static_cast<void>(world::detail::ClearAlbumNewFlagAndBorrowName(selected, index));
        if (action != nullptr) {
            const std::int32_t mapped = character_select::detail::Sub_51B740(ReadRecordW32(record, 8));
            SetYobiActionField2A8(gLegacyAlbumActionD67D14, mapped);
            SetYobiActionFlag2(gLegacyAlbumActionD67D14, false);
            ClearYobiActionFlags0C(gLegacyAlbumActionD67D14);
        }
        std::sprintf(buffer, "%03d", ReadRecordW32(selectedRecord(), 4));
        drawAt(buffer, 46, 170, 1);
        const std::int32_t nameWidth = TextMeasure(reinterpret_cast<const char*>(selectedRecord() + 0x34), 0);
        const char* const name = reinterpret_cast<const char*>(selectedRecord() + 0x34);
        drawAt(name, 154 - nameWidth / 2, 168, 0);
        const auto firstMark = std::bit_cast<std::int8_t>(selectedRecord()[1]);
        const std::int32_t firstY = gLegacyAlbumDetailPanel5846C0.y + 207;
        const std::int32_t firstImage = firstMarks[firstMark];
        const std::int32_t firstX = gLegacyAlbumDetailPanel5846C0.x + 44;
        static_cast<void>(DrawGraphic(firstX, firstY, 213, firstImage, 0, nullptr));
        const auto secondMark = std::bit_cast<std::int8_t>(selectedRecord()[2]);
        const std::int32_t secondY = gLegacyAlbumDetailPanel5846C0.y + 207;
        const std::int32_t secondImage = secondMarks[secondMark];
        const std::int32_t secondX = gLegacyAlbumDetailPanel5846C0.x + 98;
        static_cast<void>(DrawGraphic(secondX, secondY, 213, secondImage, 0, nullptr));
        const auto tribe = std::bit_cast<std::int8_t>(selectedRecord()[3]);
        std::strcpy(buffer, ui::detail::gTribeNames57CB54[tribe]);
        const std::int32_t tribeWidth = TextMeasure(buffer, 1);
        drawAt(buffer, 215 - tribeWidth / 2, 193, 1);
        std::sprintf(buffer, "%2d", ReadRecordW32(selectedRecord(), 0x30));
        drawAt(buffer, 195, 325, 1);
        for (std::int32_t rating = 0; rating < 5; ++rating) {
            const std::byte* const ratingRecord = selectedRecord();
            const std::int32_t top = gLegacyAlbumDetailPanel5846C0.y + 228 + 24 * rating;
            const std::int32_t value = ReadRecordW32(ratingRecord, 0x0C + 4 * rating);
            const std::int32_t left = gLegacyAlbumDetailPanel5846C0.x + 45;
            Sub_4F6BF0(left, top, value, 213);
        }
        for (std::int32_t element = 0; element < 4; ++element) {
            const std::int32_t top = gLegacyAlbumDetailPanel5846C0.y + 240 + 20 * element;
            const std::int32_t left = gLegacyAlbumDetailPanel5846C0.x + 187;
            const std::int32_t elementIndex = gLegacyAlbumSelectionC8AF7C;
            const std::byte* const elementRecords = world::detail::BorrowSelectedAlbumRecords();
            const std::uint32_t color = kLegacyAttributeBars57CBB8[element][0];
            const std::int32_t value = ReadRecordW32(elementRecords + elementIndex * 0x80, 0x20 + 4 * element);
            const std::int32_t right = left + 7 * value;
            DrawGraphicHover(Low16(left), Low16(top), Low16(right), Low16(top + 9), 215, color, 1);
            DrawGraphicHover(Low16(left + 1), Low16(top + 1), Low16(right + 1), Low16(top + 10),
                214, kLegacyAttributeBars57CBB8[element][1], 0);
            DrawGraphicHover(Low16(left + 2), Low16(top + 2), Low16(right + 2), Low16(top + 11),
                214, kLegacyAttributeBars57CBB8[element][1], 0);
        }
        for (std::int32_t line = 0; line < 2; ++line) {
            const char* const text = reinterpret_cast<const char*>(selectedRecord() + 0x45 + 29 * line);
            drawAt(text, 10, 354 + 20 * line, 0);
        }
    } else if (gLegacyAlbumActionD67D14 != nullptr) {
        SetYobiActionFlag2(gLegacyAlbumActionD67D14, true);
    }
    const std::int32_t current = gLegacyAlbumSelectionC8AF7C;
    const std::int32_t count = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
    std::sprintf(buffer, "%3d/%3d", current + 1, count);
    drawAt(buffer, 150, 411, 1);
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previousTen, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&nextTen, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyAlbumDetailPanel5846C0) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if ((QueryPanelButtonState(&previousTen) & 0x40) != 0 || (QueryPanelButtonState(&nextTen) & 0x40) != 0) {
        hint = kLegacyAlbumSkipTen58A0F0;
    } else if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyAlbumBack58A0FC;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyAlbumDetailPanel5846C0, 1);
}

void Sub_4F6BF0(const std::int32_t x, const std::int32_t y,
    const std::int32_t rating, const std::uint8_t layer)
{
    const std::int32_t count = rating + 1;
    constexpr std::array<std::int32_t, 2> images {22901, 22902};
    if (count < 1 || count > 10) {
        return;
    }
    for (std::int32_t part = 0; part < count; ++part) {
        static_cast<void>(DrawGraphic(x + 4 + 8 * part, y + 8, layer, images[part % 2], 0, nullptr));
    }
}

std::uint32_t Sub_4F6C60()
{
    if (gLegacyAlbumReturningCC40D0 != 0) {
        const std::int32_t index = gLegacyAlbumSelectionC8AF7C;
        gLegacyAlbumPageC8B1B0 = index / 15;
        return index < 0 ? 1u : 0u;
    }
    gLegacyAlbumPageC8B1B0 = 0;
    return 0;
}

std::uint32_t Sub_4F6CA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyAlbumListPanel584788.x;
    const std::int32_t y = gLegacyAlbumListPanel584788.y;
    LegacyPanelButtonRecord previous {x + 78, y + 413, x + 64, y + 403, 28, 20, 22881, 22882};
    LegacyPanelButtonRecord next {x + 189, y + 413, x + 175, y + 403, 28, 20, 22883, 22884};
    const std::int32_t state = Sub_503160(opened, &gLegacyAlbumListPanel584788,
        +[] { static_cast<void>(Sub_4F6C60()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        gLegacyAlbumReturningCC40D0 = 0;
        return 2;
    }
    std::int32_t page = gLegacyAlbumPageC8B1B0;
    std::int32_t hovered = -1;
    std::int32_t hoveredIndex = -1;
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::int32_t index = row + 15 * page;
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        if (records[index * 0x80] != std::byte{0}) {
            const std::int32_t top = gLegacyAlbumListPanel584788.y + 24 * row;
            const std::int32_t left = gLegacyAlbumListPanel584788.x;
            if (Sub_5049A0(left + 42, top + 39, left + 230, top + 59, -1) != 0) {
                hovered = row;
                hoveredIndex = index;
                break;
            }
            page = gLegacyAlbumPageC8B1B0;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        gLegacyAlbumSelectionC8AF7C = hoveredIndex;
        static_cast<void>(audio::Sub_504180(24));
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        --gLegacyAlbumPageC8B1B0;
        if (gLegacyAlbumPageC8B1B0 < 0) {
            gLegacyAlbumPageC8B1B0 = std::bit_cast<std::int32_t>(resource::gValueCC3E68) - 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        const std::int32_t oldPage = gLegacyAlbumPageC8B1B0;
        const std::int32_t count = std::bit_cast<std::int32_t>(resource::gValueCC3E68);
        gLegacyAlbumPageC8B1B0 = oldPage + 1;
        if (gLegacyAlbumPageC8B1B0 >= count) {
            gLegacyAlbumPageC8B1B0 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    char buffer[128];
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t index = row + 15 * gLegacyAlbumPageC8B1B0;
        const std::byte* const record = records + index * 0x80;
        if (record[0] != std::byte{0}) {
            std::sprintf(buffer, "%03d", ReadRecordW32(record, 4));
            const std::int32_t numberY = gLegacyAlbumListPanel584788.y + 24 * row + 42;
            const std::int32_t numberX = gLegacyAlbumListPanel584788.x + 46;
            static_cast<void>(DrawText(static_cast<std::int16_t>(numberX),
                static_cast<std::int16_t>(numberY), 2, 1, 0, buffer, 0, 0));
            const std::byte* const measureRecords = world::detail::BorrowSelectedAlbumRecords();
            const std::int32_t width = TextMeasure(
                reinterpret_cast<const char*>(measureRecords + index * 0x80 + 0x34), 0);
            const std::byte* const nameRecords = world::detail::BorrowSelectedAlbumRecords();
            const std::int32_t nameY = gLegacyAlbumListPanel584788.y + 24 * row + 40;
            const char* const name = reinterpret_cast<const char*>(nameRecords + index * 0x80 + 0x34);
            const std::int32_t nameX = gLegacyAlbumListPanel584788.x - width / 2 + 154;
            static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
                static_cast<std::int16_t>(nameY), 2, 0, 0, name, 0, 0));
            const std::byte* const flagRecords = world::detail::BorrowSelectedAlbumRecords();
            if (flagRecords[index * 0x80 + 0x7F] != std::byte{0}) {
                const std::int32_t markY = gLegacyAlbumListPanel584788.y + 48 + 24 * row;
                const std::int32_t markX = gLegacyAlbumListPanel584788.x + 23;
                static_cast<void>(DrawGraphic(markX, markY, 213, 22885, 0, nullptr));
            }
        }
    }
    const std::int32_t current = gLegacyAlbumPageC8B1B0;
    const std::int32_t count = std::bit_cast<std::int32_t>(resource::gValueCC3E68);
    std::sprintf(buffer, "%2d/%2d", current + 1, count);
    const std::int32_t pageY = gLegacyAlbumListPanel584788.y + 406;
    const std::int32_t pageX = gLegacyAlbumListPanel584788.x + 115;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    if (hovered >= 0) {
        const std::int32_t top = gLegacyAlbumListPanel584788.y + 24 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyAlbumListPanel584788.x;
        DrawGraphicHover(Low16(left + 40), Low16(top + 37),
            Low16(left + 232), Low16(top + 61), 214, color, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyAlbumListPanel584788) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if (hovered >= 0) {
        hint = kLegacyAlbumOpen58A114;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyAlbumListPanel584788, 1);
}

std::int32_t Sub_4F7140()
{
    gLegacyPetStateOpenD67D18 = 0;
    gLegacyPetStatePreviousD2D48C = 0;
    return 0;
}

// 原旧20完整列表入口；内嵌4F7870仍为真实缺失依赖，不注册不完整页面。
std::uint32_t Sub_4F7150(const std::int32_t opened)
{
    const std::int32_t panelY = gLegacyPetListPanel584850.y;
    const std::int32_t panelX = gLegacyPetListPanel584850.x;
    std::array<LegacyPanelButtonRecord, 5> buttons;
    for (std::int32_t row = 0; row < 5; ++row) {
        buttons[row] = {panelX + 231, panelY + 83 + 75 * row,
            panelX + 199, panelY + 67 + 75 * row, 64, 32, 22841, 22842};
    }
    constexpr std::array<std::array<std::int32_t, 2>, 3> stateImages {{
        {{22845, 22846}}, {{22843, 22844}}, {{22841, 22842}},
    }};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetListPanel584850,
        +[] { static_cast<void>(Sub_4F7140()); }, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::int32_t stateHovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (world::detail::gPetPairsDE0B08[row].used == 0) {
            continue;
        }
        if ((QueryPanelButtonState(&buttons[row]) & 0x40) != 0) {
            stateHovered = row;
        }
        if ((QueryPanelButtonState(&buttons[row]) & 1) != 0) {
            if (gLegacyPetStateOpenD67D18 != 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            gLegacyPetStateOpenD67D18 = 1;
            break;
        }
    }
    std::int32_t nameHovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (Sub_5049A0(gLegacyPetListPanel584850.x + 8,
                gLegacyPetListPanel584850.y + 32 + 75 * row,
                gLegacyPetListPanel584850.x + 154,
                gLegacyPetListPanel584850.y + 53 + 75 * row, -1) != 0) {
            nameHovered = row;
            break;
        }
    }
    if (nameHovered >= 0 && world::detail::gPetPairsDE0B08[nameHovered].used != 0) {
        const auto clicks = static_cast<std::uint8_t>(gValueD68D9C);
        if ((clicks & 1) != 0) {
            static_cast<void>(audio::Sub_504180(21));
            gLegacyPetDetailPanel584918.selected = 0;
            gLegacyPetSelectionD67ADC = nameHovered;
        } else if ((clicks & 2) != 0) {
            const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[nameHovered].index);
            const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            const std::uintptr_t socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_510E30(socket, x, y, index));
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    if (gLegacyPetStateOpenD67D18 != 0) {
        Sub_4F7870(gLegacyPetListPanel584850.x + gLegacyPetListPanel584850.width,
            gLegacyPetListPanel584850.y);
    }
    gLegacyPetStatePreviousD2D48C = gLegacyPetStateOpenD67D18;
    char buffer[128];
    for (std::int32_t row = 0; row < 5; ++row) {
        if (world::detail::gPetPairsDE0B08[row].used == 0) {
            continue;
        }
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        auto& pet = world::detail::gPetRuntimeF633F8[index];
        const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
        const std::int32_t width = TextMeasure(name, 0);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetListPanel584850.x + 83 - width / 2),
            static_cast<std::int16_t>(gLegacyPetListPanel584850.y + 36 + 75 * row),
            2, 0, 0, name, 0, 0));
        std::sprintf(buffer, "%3d", pet.value08);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetListPanel584850.x + 186),
            static_cast<std::int16_t>(gLegacyPetListPanel584850.y + 38 + 75 * row),
            2, 1, 0, buffer, 0, 0));
        const std::int32_t maxHp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[1]);
        const std::int32_t hp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[0]);
        std::sprintf(buffer, "%4d/%4d", hp, maxHp);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetListPanel584850.x + 49),
            static_cast<std::int16_t>(gLegacyPetListPanel584850.y + 61 + 75 * row),
            2, 1, 0, buffer, 0, 0));
        const std::int32_t maxFp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[3]);
        const std::int32_t fp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[2]);
        std::sprintf(buffer, "%4d/%4d", fp, maxFp);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetListPanel584850.x + 49),
            static_cast<std::int16_t>(gLegacyPetListPanel584850.y + 83 + 75 * row),
            2, 1, 0, buffer, 0, 0));
        const auto tribe = std::bit_cast<std::int16_t>(pet.value04);
        static_cast<void>(DrawGraphic(gLegacyPetListPanel584850.x + 163,
            gLegacyPetListPanel584850.y + 91 + 75 * row, 212,
            kLegacyPetTribeImages57CB2C[tribe], 0, nullptr));
        const std::int32_t injuryImage = ui::detail::ReadLegacyHealthImage(pet.values70[25]);
        static_cast<void>(DrawGraphic(gLegacyPetListPanel584850.x + 258,
            gLegacyPetListPanel584850.y + 45 + 75 * row, 212, injuryImage, 0, nullptr));
        for (std::int32_t element = 0; element < 4; ++element) {
            const std::int32_t left = gLegacyPetListPanel584850.x + 148;
            const std::int32_t top = gLegacyPetListPanel584850.y + 61 + 75 * row + 4 * element;
            const std::int32_t right = left + 4 * (pet.values70[21 + element] / 10);
            DrawGraphicHover(Low16(left), Low16(top), Low16(right), Low16(top + 3),
                215, kLegacyAttributeBars57CBB8[element][0], 1);
        }
        const auto battleState = std::bit_cast<std::int16_t>(pet.flags6AA);
        buttons[row].normalImage = stateImages[battleState][0];
        buttons[row].pressedImage = stateImages[battleState][1];
        Sub_5035E0(&buttons[row], 212, 1, gValueD69768, 0);
    }
    if (nameHovered >= 0) {
        const std::int32_t top = gLegacyPetListPanel584850.y + 32 + 75 * nameHovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetListPanel584850.x;
        DrawGraphicHover(Low16(left + 8), Low16(top), Low16(left + 154), Low16(top + 21),
            214, color, 0);
    }
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetListPanel584850) != 0) {
        hint = kLegacyLogout553FD0;
    } else if (nameHovered >= 0 && world::detail::gPetPairsDE0B08[nameHovered].used != 0) {
        hint = kLegacyPetOpenDetails586D0C;
    } else if (stateHovered >= 0 && world::detail::gPetPairsDE0B08[stateHovered].used != 0) {
        hint = kLegacyPetChooseState58A12C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetListPanel584850, 1);
}

YobiAction* Sub_4F8340()
{
    YobiAction* action = gLegacyPetDetailActionD67AE0;
    if (action == nullptr) {
        const std::int32_t y = gLegacyPetDetailPanel584918.y + 160;
        const std::int32_t position = gLegacyPetSelectionD67ADC;
        const std::int32_t x = gLegacyPetDetailPanel584918.x + 202;
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
        const std::int32_t image = world::detail::gPetRuntimeF633F8[index].imageNo6A0;
        action = Sub_4F9FF0(image, x, y, 221);
        gLegacyPetDetailActionD67AE0 = action;
        if (action != nullptr) {
            SetYobiActionField2B8(action, 6);
        }
    }
    return action;
}

std::uintptr_t Sub_4F83B0(const std::int32_t opened)
{
    using PageCallback = std::int32_t (*)(std::int32_t, std::int32_t);
    constexpr std::array<PageCallback, 3> pages {Sub_4F8550, Sub_4F8E40, Sub_4F9B10};
    if (gLegacyPetDetailPanel584918.selected < 0 || gLegacyPetDetailPanel584918.selected > 2) {
        gLegacyPetDetailPanel584918.selected = 0;
    }
    const std::int32_t state = Sub_503160(opened, &gLegacyPetDetailPanel584918,
        +[] { static_cast<void>(Sub_4F8340()); }, 1);
    if (state == 0) {
        return 0;
    }
    YobiAction* action = gLegacyPetDetailActionD67AE0;
    if (state == 2) {
        if (action != nullptr) {
            action = MarkDeath(action);
            gLegacyPetDetailActionD67AE0 = nullptr;
        }
        return reinterpret_cast<std::uintptr_t>(action);
    }
    if (action != nullptr) {
        SetYobiActionFlag2(action, true);
    }
    if (platform::ReadControlKeyPressFlag_4597C0(9) != 0) {
        const auto next = static_cast<std::int8_t>(gLegacyPetDetailPanel584918.selected + 1);
        gLegacyPetDetailPanel584918.selected = next;
        if (next >= 3) {
            gLegacyPetDetailPanel584918.selected = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(52, 320));
    }
    const auto page = pages[gLegacyPetDetailPanel584918.selected];
    if (page != nullptr) {
        const std::int32_t y = gLegacyPetDetailPanel584918.y;
        const std::int32_t x = gLegacyPetDetailPanel584918.x;
        static_cast<void>(page(x, y));
    }
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    const auto& pet = world::detail::gPetRuntimeF633F8[index];
    const char* name = pet.name6BD.data();
    if (std::strlen(name) == 0) {
        name = pet.name6AC.data();
    }
    const std::int32_t width = TextMeasure(name, 0);
    const std::int32_t y = gLegacyPetDetailPanel584918.y + 9;
    const std::int32_t x = gLegacyPetDetailPanel584918.x - width / 2 + 116;
    static_cast<void>(DrawText(static_cast<std::int16_t>(x),
        static_cast<std::int16_t>(y), 2, 0, 0, name, 0, 0));
    if (Sub_503590(&gLegacyPetDetailPanel584918) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyLogout553FD0);
    }
    return Sub_503440(&gLegacyPetDetailPanel584918, 1);
}

std::int32_t Sub_4F8550(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t panelY = gLegacyPetDetailPanel584918.y;
    const std::int32_t panelX = gLegacyPetDetailPanel584918.x;
    LegacyPanelButtonRecord back {panelX + 70, panelY + 389, panelX + 30, panelY + 379, 80, 20, 22830, 22831};
    LegacyPanelButtonRecord previous {panelX + 155, panelY + 389, panelX + 141, panelY + 379, 28, 20, 22832, 22833};
    LegacyPanelButtonRecord next {panelX + 229, panelY + 389, panelX + 215, panelY + 379, 28, 20, 22834, 22835};
    LegacyPanelButtonRecord rename {panelX + 70, panelY + 356, panelX + 30, panelY + 346, 80, 20, 22807, 22808};
    if (Sub_5049A0(panelX + 136, panelY + 32, panelX + 267, panelY + 203, -1) != 0
        && (static_cast<std::uint8_t>(gValueD68DA0) & 1) != 0) {
        YobiAction* action = gLegacyPetDetailActionD67AE0;
        if (action != nullptr) {
            SetYobiActionField2B8(action, GetYobiActionField2B8(action) + 1);
            action = gLegacyPetDetailActionD67AE0;
            if (GetYobiActionField2B8(action) >= 8) {
                SetYobiActionField2B8(action, 0);
            }
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (!HandleLegacyPetNavigation(&back, &previous, &next) && (QueryPanelButtonState(&rename) & 1) != 0) {
        if (input::IsChatContext_503E60() == 0) {
            static_cast<void>(audio::Sub_504380(22));
        }
        static_cast<void>(audio::Sub_504180(22));
    }
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    auto& pet = world::detail::gPetRuntimeF633F8[index];
    YobiAction* action = gLegacyPetDetailActionD67AE0;
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
        action = gLegacyPetDetailActionD67AE0;
        const std::int32_t currentImage = GetYobiActionField2A8(action);
        const std::int32_t image = pet.imageNo6A0;
        if (currentImage != image) {
            SetYobiActionField2A8(action, image);
            SetYobiActionField2B8(gLegacyPetDetailActionD67AE0, 6);
        }
    }
    const auto drawAt = [x, y](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    char buffer[256];
    std::sprintf(buffer, "%3d", pet.value08);
    drawAt(buffer, 62, 44);
    std::sprintf(buffer, "%9d", platform::legacy_crypt_int::Read_401EC0(&pet.values0C[4]));
    drawAt(buffer, 55, 66);
    if (pet.value08 < 130) {
        std::sprintf(buffer, "%9d", platform::legacy_crypt_int::Read_401EC0(&pet.values0C[5]));
        drawAt(buffer, 55, 89);
    } else {
        drawAt(kLegacyPetLevelLimit58A214, 55, 89);
    }
    const std::int32_t maxHp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[1]);
    const std::int32_t hp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[0]);
    std::sprintf(buffer, "%4d/%4d", hp, maxHp);
    drawAt(buffer, 55, 113);
    const std::int32_t maxFp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[3]);
    const std::int32_t fp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[2]);
    std::sprintf(buffer, "%4d/%4d", fp, maxFp);
    drawAt(buffer, 55, 136);
    for (std::int32_t field = 0; field < 6; ++field) {
        std::sprintf(buffer, "%3d", pet.values70[5 + field]);
        drawAt(buffer, 62, 159 + 23 * field);
    }
    std::sprintf(buffer, "%2d", pet.value6A4);
    drawAt(buffer, 70, 297);
    std::strcpy(buffer, ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(pet.value04)]);
    const std::int32_t width = TextMeasure(buffer, 1);
    drawAt(buffer, 218 - width / 2, 239);
    for (std::int32_t element = 0; element < 4; ++element) {
        const std::int32_t right = x + 183 + 7 * (pet.values70[21 + element] / 10);
        const std::int32_t top = y + 286 + 20 * element;
        DrawGraphicHover(Low16(x + 183), Low16(top), Low16(right), Low16(top + 9),
            215, kLegacyAttributeBars57CBB8[element][0], 1);
        DrawGraphicHover(Low16(x + 184), Low16(top + 1), Low16(right + 1), Low16(top + 10),
            214, kLegacyAttributeBars57CBB8[element][1], 0);
        DrawGraphicHover(Low16(x + 185), Low16(top + 2), Low16(right + 2), Low16(top + 11),
            214, kLegacyAttributeBars57CBB8[element][1], 0);
    }
    const std::int32_t position = gLegacyPetSelectionD67ADC;
    const std::int32_t count = world::detail::gPetCountDE0B00;
    std::sprintf(buffer, "%d/%d", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 382;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    const std::int32_t healthImage = ui::detail::ReadLegacyHealthImage(pet.values70[25]);
    const std::int32_t healthY = gLegacyPetDetailPanel584918.y + 222;
    const std::int32_t healthX = gLegacyPetDetailPanel584918.x + 184;
    static_cast<void>(DrawGraphic(healthX, healthY, 212, healthImage, 0, nullptr));
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&rename, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyPetBack58A204;
    } else if ((QueryPanelButtonState(&rename) & 0x40) != 0) {
        hint = kLegacyPetRename586D38;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return 0;
}

std::int32_t Sub_4F8E40(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t panelY = gLegacyPetDetailPanel584918.y;
    const std::int32_t panelX = gLegacyPetDetailPanel584918.x;
    std::array<LegacyPanelButtonRecord, 5> attributes;
    for (std::int32_t field = 0; field < 5; ++field) {
        attributes[field] = {panelX + 242, panelY + 90 + 23 * field, panelX + 226,
            panelY + 80 + 23 * field, 32, 20, 22812, 22813};
    }
    LegacyPanelButtonRecord back {panelX + 70, panelY + 395, panelX + 30, panelY + 385, 80, 20, 22830, 22831};
    LegacyPanelButtonRecord previous {panelX + 155, panelY + 395, panelX + 141, panelY + 385, 28, 20, 22832, 22833};
    LegacyPanelButtonRecord next {panelX + 229, panelY + 395, panelX + 215, panelY + 385, 28, 20, 22834, 22835};
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    auto& pet = world::detail::gPetRuntimeF633F8[index];
    std::array<std::int32_t, 6> points;
    for (std::int32_t field = 0; field < 5; ++field) {
        points[field] = pet.values70[field];
    }
    points[5] = pet.lvFlag6C;
    std::int32_t total = 0;
    for (const std::int32_t value : points) {
        total += value;
    }
    const std::int32_t maximum = total / 2;
    static_cast<void>(HandleLegacyPetNavigation(&back, &previous, &next));
    // 原页导航后仍对入口宠物操作；资格与提示使用导航前六值快照。
    std::int32_t hovered = -1;
    if (pet.lvFlag6C != 0) {
        for (std::int32_t field = 0; field < 5; ++field) {
            if ((QueryPanelButtonState(&attributes[field]) & 0x40) != 0) {
                hovered = field;
            }
            if ((QueryPanelButtonState(&attributes[field]) & 1) != 0 && points[field] < maximum) {
                static_cast<void>(network::Sub_512400(platform::gConnectionSocket, index, field));
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                break;
            }
        }
    }
    const auto drawAt = [x, y](const char* const text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    char buffer[256];
    std::sprintf(buffer, "%3d", pet.value08);
    drawAt(buffer, 62, 36);
    const std::int32_t maxHp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[1]);
    const std::int32_t hp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[0]);
    std::sprintf(buffer, "%4d/%4d", hp, maxHp);
    drawAt(buffer, 55, 59);
    const std::int32_t maxFp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[3]);
    const std::int32_t fp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[2]);
    std::sprintf(buffer, "%4d/%4d", fp, maxFp);
    drawAt(buffer, 55, 82);
    for (std::int32_t field = 0; field < 5; ++field) {
        std::sprintf(buffer, "%3d", pet.values70[5 + field]);
        drawAt(buffer, 62, 105 + 23 * field);
    }
    for (std::int32_t field = 0; field < 6; ++field) {
        std::sprintf(buffer, "%+4d", pet.values70[11 + field]);
        drawAt(buffer, 69, 243 + 23 * field);
    }
    for (std::int32_t field = 0; field < 5; ++field) {
        std::sprintf(buffer, "%3d", pet.values70[field]);
        drawAt(buffer, 192, 82 + 23 * field);
    }
    std::sprintf(buffer, "%3d", pet.values70[10]);
    drawAt(buffer, 186, 202);
    std::sprintf(buffer, "%2d", pet.value6A4);
    drawAt(buffer, 194, 227);
    for (std::int32_t field = 0; field < 4; ++field) {
        std::sprintf(buffer, "%+4d", pet.values70[17 + field]);
        drawAt(buffer, 193, 273 + 23 * field);
    }
    const std::int32_t position = gLegacyPetSelectionD67ADC;
    const std::int32_t count = world::detail::gPetCountDE0B00;
    std::sprintf(buffer, "%d/%d", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 388;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    std::sprintf(buffer, "%3d", pet.lvFlag6C);
    drawAt(buffer, 220, 59);
    if (pet.lvFlag6C != 0) {
        static_cast<void>(DrawGraphic(x + 178, y + 68, 212, 22811, 1, nullptr));
        for (auto& attribute : attributes) {
            Sub_5035E0(&attribute, 212, 1, gValueD69768, 0);
        }
    }
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyPetBack58A204;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if (hovered >= 0) {
        if (points[hovered] >= maximum) {
            hint = kLegacyAttributeLimit58A220;
        } else {
            switch (hovered) {
            case 0: hint = kLegacyPetVitality58A288; break;
            case 1: hint = kLegacyAttributeHint58A274; break;
            case 2: hint = kLegacyPetToughness58A260; break;
            case 3: hint = kLegacyAttributeHint58A24C; break;
            case 4: hint = kLegacyPetMagic58A238; break;
            }
        }
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return 0;
}

std::int32_t Sub_4F9B10(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t panelX = gLegacyPetDetailPanel584918.x;
    const std::int32_t panelY = gLegacyPetDetailPanel584918.y;
    LegacyPanelButtonRecord back {panelX + 70, panelY + 393, panelX + 30, panelY + 383, 80, 20, 22830, 22831};
    LegacyPanelButtonRecord previous {panelX + 155, panelY + 393, panelX + 141, panelY + 383, 28, 20, 22832, 22833};
    LegacyPanelButtonRecord next {panelX + 229, panelY + 393, panelX + 215, panelY + 383, 28, 20, 22834, 22835};
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    const auto& pet = world::detail::gPetRuntimeF633F8[index];
    std::int32_t slots = pet.value6A4;
    if (slots < 1 || slots > 10) {
        slots = 0;
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < slots; ++row) {
        if (Sub_5049A0(x + 37, y + 36 + 24 * row, x + 257, y + 54 + 24 * row, -1) != 0) {
            hovered = row;
            break;
        }
    }
    static_cast<void>(HandleLegacyPetNavigation(&back, &previous, &next));
    char buffer[256];
    for (std::int32_t row = 0; row < slots; ++row) {
        const auto& pair = pet.techniquePairs650[row];
        if (pair.used != 0) {
            const auto skill = std::bit_cast<std::int16_t>(pair.index);
            const auto& technique = pet.techniquesD8[skill];
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 39),
                static_cast<std::int16_t>(y + 37 + 24 * row), 2, 0, 0, technique.name.data(), 0, 0));
            std::sprintf(buffer, "%4d", technique.fp);
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 211),
                static_cast<std::int16_t>(y + 37 + 24 * row), 2, 1, 0, buffer, 0, 0));
        }
        static_cast<void>(DrawGraphic(x + 136, y + 46 + 24 * row, 212, 22825, 0, nullptr));
    }
    if (hovered >= 0 && pet.techniquePairs650[hovered].used != 0) {
        const auto skill = std::bit_cast<std::int16_t>(pet.techniquePairs650[hovered].index);
        const char* const description = pet.techniquesD8[skill].description.data();
        for (std::int32_t line = 0; line < 4; ++line) {
            if (input::ExtractLegacyLine_503AB0(buffer, 256, description, line, 26) != 0) {
                static_cast<void>(DrawText(static_cast<std::int16_t>(x + 19),
                    static_cast<std::int16_t>(y + 294 + 19 * line), 2, 0, 0, buffer, 0, 0));
            }
        }
    }
    const std::int32_t position = gLegacyPetSelectionD67ADC;
    const std::int32_t count = world::detail::gPetCountDE0B00;
    std::sprintf(buffer, "%d/%d", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 386;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyPetBack58A204;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return 0;
}

std::int32_t Sub_503BB0()
{
    return audio::Sub_504460(2);
}

std::int32_t Sub_503D00()
{
    world::detail::Sub_4FA8E0(0);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D10()
{
    world::detail::Sub_4FA8E0(2);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D20()
{
    world::detail::Sub_4FA8E0(4);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D30()
{
    world::detail::Sub_4FA8E0(6);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D40()
{
    world::detail::Sub_4FA8E0(8);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D50()
{
    world::detail::Sub_4FA8E0(10);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D60()
{
    world::detail::Sub_4FA8E0(1);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D70()
{
    world::detail::Sub_4FA8E0(3);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D80()
{
    world::detail::Sub_4FA8E0(7);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503D90()
{
    world::detail::Sub_4FA8E0(5);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503DA0()
{
    world::detail::Sub_4FA8E0(9);
    return platform::NextRandom_53B4C9();
}

std::int32_t Sub_503DB0()
{
    world::detail::Sub_4FA8E0(12);
    return platform::NextRandom_53B4C9();
}

std::uintptr_t Sub_503DC0()
{
    const std::int32_t selected = platform::NextRandom_53B4C9() % 3;
    if (selected == 0) {
        return world::detail::RequestEmoteWithResult_4FA8E0(14);
    }
    if (selected == 1) {
        return world::detail::RequestEmoteWithResult_4FA8E0(16);
    }
    return world::detail::RequestEmoteWithResult_4FA8E0(17);
}

std::int32_t Sub_503E00()
{
    return Sub_4FD4C0();
}

std::int32_t Sub_503E10()
{
    return Sub_4FD510();
}

std::int32_t Sub_503E20()
{
    return Sub_4FD5F0();
}

std::int32_t Sub_503E30()
{
    return Sub_4FD640();
}

std::int32_t Sub_402B30()
{
    if (audio::Sub_5043D0(2) != 0) {
        static_cast<void>(audio::Sub_5042D0(100));
        return audio::PlaySe_52C450(57, 320);
    }
    return audio::Sub_504180(98);
}

std::int32_t Sub_503B70()
{
    if (audio::Sub_5043D0(2) != 0) {
        static_cast<void>(audio::Sub_5042D0(100));
        return audio::PlaySe_52C450(57, 320);
    }
    return audio::Sub_504180(3);
}

std::int32_t Sub_503BC0()
{
    static_cast<void>(audio::Sub_504460(18));
    return audio::Sub_5042D0(31);
}

std::int32_t Sub_503BE0()
{
    if (audio::Sub_504410(16) != 0) {
        static_cast<void>(audio::Sub_5042D0(16));
        return audio::PlaySe_52C450(57, 320);
    }
    if (audio::Sub_504410(15) != 0) {
        static_cast<void>(audio::Sub_5042D0(15));
        return audio::PlaySe_52C450(57, 320);
    }
    return audio::Sub_504460(14);
}

std::int32_t Sub_503C30()
{
    return audio::Sub_504460(42);
}

std::int32_t Sub_503C40()
{
    return audio::Sub_504460(23);
}

std::int32_t Sub_503C50()
{
    return audio::Sub_504460(1);
}

std::int32_t Sub_503C60()
{
    return audio::Sub_504460(20);
}

std::int32_t Sub_503C70()
{
    return audio::Sub_504460(17);
}

std::uintptr_t Sub_4014B0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyBlacklistPanel5531D0.x;
    const std::int32_t y = gLegacyBlacklistPanel5531D0.y;
    LegacyPanelButtonRecord confirm {x + 522, y + 416, x + 451, y + 404, 140, 24, 99101, 99102};
    const std::int32_t state = Sub_503160(opened, &gLegacyBlacklistPanel5531D0, nullptr, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        std::uintptr_t result = 0;
        if (input::IsCurrentContext_503E40(gLegacyBlacklistInput5939B8) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyBlacklistInput5939B8 = nullptr;
        return result;
    }
    if (gLegacyBlacklistInput5939B8 == nullptr) {
        const std::int32_t inputY = gLegacyBlacklistPanel5531D0.y;
        const std::int32_t inputX = gLegacyBlacklistPanel5531D0.x;
        input::InputContext* const context = input::BorrowLegacyBlacklistContext593858();
        gLegacyBlacklistInput5939B8 = context;
        context->text[0] = 0;
        context->totalByteLength = 0;
        context->caretByteOffset = 0;
        input::InitializeContext_449CF0(context, inputX + 21, inputY + 408, 2, 0, 0, nullptr, 1, 16, 18, 0);
        input::SetContext_44B330(gLegacyBlacklistInput5939B8);
    }
    constexpr const char* instructions[3] {
        kLegacyBlacklistText5533F4, kLegacyBlacklistText5533B4, kLegacyBlacklistText553370,
    };
    for (std::int32_t index = 0; index < 3; ++index) {
        const std::int32_t textY = gLegacyBlacklistPanel5531D0.y;
        const std::int32_t textX = gLegacyBlacklistPanel5531D0.x;
        DrawText(Low16(textX + 10), Low16(textY + 35 + 20 * index), 2, 0,
            static_cast<std::int8_t>(index == 0 ? 8 : 5), instructions[index], 0, 0);
    }
    const std::int32_t count = blacklist::GetBlacklistCount_401190();
    char countText[512];
    std::sprintf(countText, kLegacyBlacklistText55332C, count);
    const std::int32_t countY = gLegacyBlacklistPanel5531D0.y;
    const std::int32_t countX = gLegacyBlacklistPanel5531D0.x;
    DrawText(Low16(countX + 10), Low16(countY + 95), 2, 0, 8, countText, 0, 0);
    std::int32_t column = 0;
    std::int32_t row = 0;
    for (std::int32_t index = 0; index < count; ++index) {
        const char* const name = blacklist::BlacklistRecordAt_401170(index);
        if (std::strlen(name) != 0) {
            // 原端先取记录并扫字符串，只有非空记录才检查72项显示边界。
            if (index >= 72) {
                break;
            }
            if (index / 18 > column) {
                row = 0;
                column = index / 18;
            }
            const std::int32_t nameY = gLegacyBlacklistPanel5531D0.y;
            const std::int32_t nameX = gLegacyBlacklistPanel5531D0.x;
            DrawText(Low16(nameX + 148 * column + 10), Low16(nameY + 15 * row + 115),
                2, 1, 0, name, 0, 0);
            ++row;
        }
    }
    render::DrawInputText_4568A0(gLegacyBlacklistInput5939B8);
    Sub_5035E0(&confirm, 213, 1, gValueD69768, 0);
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if (Sub_503590(&gLegacyBlacklistPanel5531D0) != 0) {
        std::strcpy(hint, kLegacySkillClose55331C);
    } else if ((QueryPanelButtonState(&confirm) & 1) == 0 && (platform::gKeyStates[13] & 4u) == 0) {
        if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
            std::strcpy(hint, kLegacyBlacklistText553304);
        }
    } else if (gLegacyBlacklistInput5939B8->text[0] != 0) {
        char message[128];
        if (blacklist::ContainsBlacklistRecord_4013D0(gLegacyBlacklistInput5939B8->text) != 0) {
            const bool removed = blacklist::RemoveBlacklistRecord_4012F0(gLegacyBlacklistInput5939B8->text) != 0;
            std::sprintf(message, removed ? kLegacyBlacklistText5532EC : kLegacyBlacklistText5532D8,
                gLegacyBlacklistInput5939B8->text);
        } else {
            if (blacklist::GetBlacklistCount_401190() >= 72) {
                world::detail::Sub_44B050(kLegacyBlacklistText553294, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                return static_cast<std::uintptr_t>(Sub_503440(&gLegacyBlacklistPanel5531D0, 1));
            }
            const bool added = blacklist::AddBlacklistRecord_401270(gLegacyBlacklistInput5939B8->text) != 0;
            std::sprintf(message, added ? kLegacyBlacklistText5532C4 : kLegacyBlacklistText5532B0,
                gLegacyBlacklistInput5939B8->text);
        }
        gLegacyBlacklistInput5939B8->text[0] = 0;
        gLegacyBlacklistInput5939B8->totalByteLength = 0;
        gLegacyBlacklistInput5939B8->caretByteOffset = 0;
        const std::int32_t inputY = gLegacyBlacklistPanel5531D0.y;
        const std::int32_t inputX = gLegacyBlacklistPanel5531D0.x;
        input::InitializeContext_449CF0(gLegacyBlacklistInput5939B8, inputX + 21, inputY + 408,
            2, 0, 0, nullptr, 1, 16, 18, 0);
        world::detail::Sub_44B050(message, 4, 3);
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyBlacklistPanel5531D0, 1));
}

std::int32_t Sub_4FD690()
{
    gLegacyPresetPanel5850C8.selected = 0;
    gLegacyPresetRowD30408 = 0;
    return 0;
}

std::uintptr_t Sub_4FD6A0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPresetPanel5850C8.x;
    const std::int32_t y = gLegacyPresetPanel5850C8.y;
    LegacyPanelButtonRecord confirm {x + 136, y + 420, x + 96, y + 410, 80, 20, 22712, 22713};
    const std::int32_t state = Sub_503160(opened, &gLegacyPresetPanel5850C8,
        [] { static_cast<void>(Sub_4FD690()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t page = gLegacyPresetPanel5850C8.selected;
        const std::int32_t row = gLegacyPresetRowD30408;
        input::InputContext* const context = input::BorrowLegacyPresetContext5F9758(row + 26 * page);
        if (input::IsCurrentContext_503E40(context) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            return reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        return 0;
    }
    if (platform::ReadControlKeyPressFlag_4597C0(9) != 0) {
        gLegacyPresetPanel5850C8.selected = static_cast<std::int8_t>(gLegacyPresetPanel5850C8.selected + 1);
        if (gLegacyPresetPanel5850C8.selected >= 3) {
            gLegacyPresetPanel5850C8.selected = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(52, 320));
    } else if (platform::ReadKeyPressFlag_4597B0(9) != 0 || platform::ReadKeyPressFlag_4597B0(13) != 0) {
        ++gLegacyPresetRowD30408;
        if (gLegacyPresetRowD30408 >= 26) {
            gLegacyPresetRowD30408 = 0;
        }
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 26; ++row) {
        const std::int32_t top = gLegacyPresetPanel5850C8.y + 42 * row;
        const std::int32_t left = gLegacyPresetPanel5850C8.x;
        if (Sub_5049A0(left + 19, top + 91, left + 255, top + 111, -1) != 0) {
            if ((static_cast<std::uint8_t>(gValueD68DA0) & 1u) != 0) {
                gLegacyPresetRowD30408 = row;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            hovered = row;
            break;
        }
    }
    const std::int32_t page = gLegacyPresetPanel5850C8.selected;
    const std::int32_t row = gLegacyPresetRowD30408;
    input::SetContext_44B330(input::BorrowLegacyPresetContext5F9758(row + 26 * page));
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(5));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPresetPanel5850C8.y + 42 * hovered;
        const std::uint32_t image = gValueD69768;
        const std::int32_t left = gLegacyPresetPanel5850C8.x;
        DrawGraphicHover(Low16(left + 18), Low16(top + 90), Low16(left + 256), Low16(top + 112), 214, image, 0);
    }
    for (std::int32_t index = 0; index < 26; ++index) {
        const std::int32_t currentPage = gLegacyPresetPanel5850C8.selected;
        render::DrawInputText_4568A0(input::BorrowLegacyPresetContext5F9758(index + 26 * currentPage));
    }
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    char* const hint = ui::detail::gHoverTextD68AE8.data();
    if (Sub_503590(&gLegacyPresetPanel5850C8) != 0) {
        std::strcpy(hint, kLegacySkillClose55331C);
    } else if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        std::strcpy(hint, kLegacyPresetConfirm58A6C0);
    } else if (hovered >= 0) {
        const std::int8_t currentPage = gLegacyPresetPanel5850C8.selected;
        if (currentPage == 0) {
            std::sprintf(hint, kLegacyPresetNormal58A6A8, hovered + 1);
        } else if (currentPage == 1) {
            std::sprintf(hint, kLegacyPresetCtrl58A68C, hovered + 1);
        } else if (currentPage == 2) {
            std::sprintf(hint, kLegacyPresetShift58A66C, hovered + 1);
        }
    }
    return static_cast<std::uintptr_t>(Sub_503440(&gLegacyPresetPanel5850C8, 1));
}

std::uint32_t Sub_4FA350(const std::int32_t opened)
{
    const std::int32_t y = gLegacyEmotePanel584AA8.y;
    const std::int32_t x = gLegacyEmotePanel584AA8.x;
    std::array<character_select::detail::CharacterModalUiRecord, 18> buttons;
    for (std::int32_t index = 0; index < 18; ++index) {
        const std::int32_t columnX = (index & 1) != 0 ? 84 : 0;
        const std::int32_t top = y + 44 + 30 * (index / 2);
        buttons[index] = {x + 20 + columnX, top, x + 18 + columnX, top,
            72, 16, kLegacyEmoteLabels58A370[index]};
    }
    const std::int32_t state = Sub_503160(opened, &gLegacyEmotePanel584AA8, nullptr, 1);
    if (state == 0 || state == 2) {
        return static_cast<std::uint32_t>(state);
    }
    std::int32_t hovered = -1;
    std::int32_t chosen = -1;
    for (std::int32_t index = 0; index < 18; ++index) {
        const std::uint8_t buttonState = static_cast<std::uint8_t>(
            character_select::detail::Sub_503990(&buttons[index]));
        if ((buttonState & 0x40u) != 0) {
            hovered = index;
        }
        if ((buttonState & 1u) != 0) {
            chosen = index;
            break;
        }
    }
    // 原4FA868无点击也传-1；场景、角色和发送间隔由真实动作入口处理。
    world::detail::Sub_4FA8E0(chosen);
    for (auto& button : buttons) {
        static_cast<void>(character_select::detail::Sub_503A20(&button, 2, 0, 1, gValueD69768, 218));
    }
    if (hovered >= 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyEmoteHints5871B0[hovered]);
    }
    return Sub_503440(&gLegacyEmotePanel584AA8, 1);
}

std::uintptr_t Sub_4FA050(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetRenamePanel5849E0.x;
    const std::int32_t y = gLegacyPetRenamePanel5849E0.y;
    LegacyPanelButtonRecord confirm {x + 90, y + 172, x + 50, y + 162, 80, 20, 22712, 22713};
    LegacyPanelButtonRecord cancel {x + 182, y + 172, x + 142, y + 162, 80, 20, 22710, 22711};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetRenamePanel5849E0, NullSub_509080, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        std::uintptr_t result = 0;
        if (input::IsCurrentContext_503E40(gLegacyPetRenameInputD67D1C) != 0) {
            input::SetContext_44B330(&input::gInputContext626AA0);
            result = reinterpret_cast<std::uintptr_t>(&input::gInputContext626AA0);
        }
        gLegacyPetRenameInputD67D1C = nullptr;
        return result;
    }
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    auto& pet = world::detail::gPetRuntimeF633F8[index];
    char buffer[256];
    if (gLegacyPetRenameInputD67D1C == nullptr) {
        const std::int32_t top = gLegacyPetRenamePanel5849E0.y;
        std::strcpy(buffer, pet.name6BD.data());
        const std::int32_t left = gLegacyPetRenamePanel5849E0.x;
        gLegacyPetRenameInputD67D1C = &input::gInputContextD2DFD0;
        input::gInputContextD2DFD0.text[0] = '\0';
        input::gInputContextD2DFD0.totalByteLength = 0;
        input::gInputContextD2DFD0.caretByteOffset = 0;
        input::InitializeContext_449CF0(&input::gInputContextD2DFD0,
            left + 94, top + 93, 2, 0, 0, buffer, 1, 16, 0, 0);
        input::SetContext_44B330(gLegacyPetRenameInputD67D1C);
    }
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        if (std::strcmp(pet.name6BD.data(), gLegacyPetRenameInputD67D1C->text) != 0) {
            std::strcpy(buffer, gLegacyPetRenameInputD67D1C->text);
            NullSub_509080();
            const std::uintptr_t socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_5125B0(socket, index, buffer));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
        }
        static_cast<void>(audio::Sub_5042D0(gLegacyPetRenamePanel5849E0.id));
        static_cast<void>(audio::Sub_504180(21));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetRenamePanel5849E0.id));
        static_cast<void>(audio::Sub_504180(21));
    }
    const std::int32_t nameY = gLegacyPetRenamePanel5849E0.y + 51;
    const std::int32_t nameX = gLegacyPetRenamePanel5849E0.x + 94;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameX),
        static_cast<std::int16_t>(nameY), 2, 0, 0, pet.name6BD.data(), 0, 0));
    render::DrawInputText_4568A0(gLegacyPetRenameInputD67D1C);
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    Sub_5035E0(&cancel, 212, 1, gValueD69768, 0);
    return Sub_503440(&gLegacyPetRenamePanel5849E0, 1);
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
